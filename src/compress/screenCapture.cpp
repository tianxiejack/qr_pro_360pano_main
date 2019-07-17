#include "Render.hpp"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/gstclock.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include <iostream>
#include <demo_global_def.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <time.h>

#include "Queuebuffer.hpp"
#include <gst_ring_buffer.h>

using namespace std;
using namespace cv;

typedef struct _CustomData
{
  	GstElement *pipeline, *source, *clockoverlay, *videoconvert, *nvvidconv;
	GstElement *tee0, *queue0, *fakesink0, *queue1, *fakesink1;
  	GstElement *omxh264enc, *h264parse, *rtph264pay, *udpsink;
	GstElement *splitmuxsink, *multifilesink, *mp4mux, *filesink;
	GstElement *jpegenc;

	GstBus *bus;
	GMainLoop *loop;
	gboolean playing;
	GstStateChangeReturn ret;

	GstCaps *caps_src_to_convert;
	GstCaps *caps_nvconv_to_enc;
  	GstCaps *caps_enc_to_rtp;
	GstCaps *caps_enc_to_jpeg;

	//GstPad *nvvidconv_srcpad;
	//GstPad *videoconvert_srcpad;

	GstPadTemplate *tee0_src_pad_template;
	GstPad *tee0queue0_srcpad;
	GstPad *tee0queue1_srcpad;

	GstClockTime buffer_timestamp;
	bool bPush;
	volatile bool bRec;

	int height;
	int width;
	int framerate;
	char format[30];

	pthread_t threadPushBuffer;
	Mat Screenmapx;
	Mat Screenmapy;
	GstBuffer *buffer;
	void *bufferdata;
	void *pushshare;

} CustomData;

#define GST_ENCBITRATE	(5600000)
#define REALTIMEDIR  "/home/nvidia/calib/realtimevideo"
#define VIDEODIR  "/home/nvidia/calib/video"
#define VIDRECHEAD "rtsp"
#define VIDFMTHEAD "screen"
#define VIDFMTEND ".h264"
static char localVidFname[128], localPicFname[128];
static FILE *localVidwrFd = NULL, *localPicwrFd = NULL;
static int localwrsize = 0;
static int localsizemax = (GST_ENCBITRATE/8)*60*2;	// 2min
static int localwrframe = 0;
static int localframemax = (30)*60*2;	// 2min
extern Render render;

typedef enum
{
	link_h264 = 0,
	link_shot,
}GLINK_mode;

typedef enum
{
	format_vid_close = 0,
	format_vid_open,
	format_shot_close,
	format_shot_open,
}FMT_mode;

static void historylocation(void)
{
	DIR *dir, *dir2;
	struct dirent *ptr, *ptr2;
	char date_dir[128];
	char filename_cur[128] = {0};
	char cmdbuf[128] = {0};

	dir = opendir(VIDEODIR);
	while((ptr = readdir(dir)) != NULL)
	{
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)
			continue;
		if(ptr->d_type & DT_DIR)
		{
			sprintf(date_dir, "%s/%s", VIDEODIR, ptr->d_name);
			dir2 = opendir(date_dir);
			while((ptr2 = readdir(dir2)) != NULL)
			{
				if(strcmp(ptr2->d_name,".")==0 || strcmp(ptr2->d_name,"..")==0)
					continue;
				if(ptr2->d_type & DT_DIR)
					;
				else
				{
					if(strncmp(ptr2->d_name, VIDRECHEAD, strlen(VIDRECHEAD))==0)
					{
						sprintf(filename_cur, "%s/%s/%s", VIDEODIR, date_dir, ptr2->d_name);
						printf("%s, %d, rm %s\n",  __FILE__,__LINE__, filename_cur);
						//remove(filename_cur);
						sprintf(cmdbuf, "rm -rf %s", filename_cur);
						system(cmdbuf);
					}
				}
			}
			closedir(dir2);
		}
		else
		{
			if(strncmp(ptr->d_name, VIDRECHEAD, strlen(VIDRECHEAD))==0)
			{
				sprintf(filename_cur, "%s/%s", VIDEODIR, ptr->d_name);
				printf("%s, %d, rm %s\n",  __FILE__,__LINE__, filename_cur);
				//remove(filename_cur);
				sprintf(cmdbuf, "rm -rf %s", filename_cur);
				system(cmdbuf);
			}
		}
	}
	closedir(dir);
	return ;
}

static void formatlocation(FMT_mode mode)
{
	static char baktmstr[64];
	char curtmstr[64];
	struct tm tm_set;
	struct timeval tv_set;
	struct timezone tz_set;
	time_t tt_set=0;
	char video_dir[64];
	char date_dir[16];
	char filename_prev[128] = {0};
	char filename_cur[128] = {0};
	char cmdbuf[128];

	gettimeofday(&tv_set, &tz_set);
	tt_set = tv_set.tv_sec;
	memcpy(&tm_set, localtime(&tt_set), sizeof(struct tm));

	// Create top-level realtimevideo directory
	strcpy(video_dir, REALTIMEDIR);
	if (access(video_dir, F_OK))
		mkdir(video_dir, 0775);

	// Create top-level video directory
	strcpy(video_dir, VIDEODIR);
	if (access(video_dir, F_OK))
		mkdir(video_dir, 0775);

	// Create data directory
	sprintf(date_dir, "/%d%02d%02d", tm_set.tm_year+1900, tm_set.tm_mon+1, tm_set.tm_mday);
	strcat(video_dir, date_dir);
	if (access(video_dir, F_OK))
		mkdir(video_dir, 0775);

	sprintf(curtmstr, "%04d%02d%02d-%02d%02d%02d", 
		tm_set.tm_year+1900, tm_set.tm_mon+1, tm_set.tm_mday,
		tm_set.tm_hour, tm_set.tm_min,tm_set.tm_sec);

	if(mode == format_vid_close)
	{
		fclose(localVidwrFd);
		sprintf(filename_prev, "%s/%s_%s%s", 
					video_dir, VIDRECHEAD, baktmstr, VIDFMTEND);
		if(localwrsize > 0)
		{
			sprintf(filename_cur, "%s/%s_%s_%s%s", 
						video_dir, VIDFMTHEAD, baktmstr, curtmstr, VIDFMTEND);
			rename(filename_prev, filename_cur);
			printf("screen vid rename to %s\n", filename_cur);
		}
		else
		{
			//remove(filename_prev);
			sprintf(cmdbuf, "rm -rf %s", filename_prev);
			system(cmdbuf);
			printf("screen vid delete empty %s\n", filename_prev);
		}
		localVidwrFd = NULL;
		return ;
	}

	if(mode == format_vid_open)
	{
		sprintf(filename_cur, "%s/%s_%s%s", 
					video_dir, VIDRECHEAD, curtmstr, VIDFMTEND);
		strcpy(baktmstr, curtmstr);
		localVidwrFd = fopen(filename_cur, "wb");
		localwrsize = 0;
		localwrframe = 0;
		if(localVidwrFd == NULL)
			printf("screen vid start as %s failed\n", filename_cur);
		else
			printf("screen vid start as %s ok\n", filename_cur);
		return ;
	}

	if(mode == format_shot_close)
	{
		fclose(localPicwrFd);
		localPicwrFd = NULL;
		return ;
	}

	if(mode == format_shot_open)
	{
		sprintf(curtmstr, "%04d%02d%02d%02d%02d%02d",
			tm_set.tm_year+1900, tm_set.tm_mon+1, tm_set.tm_mday,
			tm_set.tm_hour, tm_set.tm_min,tm_set.tm_sec);
		sprintf(filename_cur, "%s/Shotfull-%s.jpg", REALTIMEDIR, curtmstr);
		localPicwrFd = fopen(filename_cur, "wb");
		if(localPicwrFd == NULL)
			printf("screen shot start as %s failed\n", filename_cur);
		else
			printf("screen shot start as %s ok\n", filename_cur);
		strcpy(localPicFname, filename_cur);
		return ;
	}

	return ;
}

static int outbufCb_vid(void* arg, unsigned char *buf, int size)
{
	CustomData* pData = (CustomData *)arg;
	int ret=0;
#if 0
	static Uint32 pushcnt=0, mstimeprev=0, mstimecur;
	mstimecur = OSA_getCurTimeInMsec()/1000;
	pushcnt++;
	if(mstimecur != mstimeprev)
	{
		if(mstimeprev != 0 && mstimecur != 0)
			g_print("======== %d %s %d fps \n", OSA_getCurTimeInMsec(), __func__, pushcnt/(mstimecur-mstimeprev));
		mstimeprev = mstimecur;
		pushcnt = 0;
	}
#endif
	if(!pData->bRec)
	{
		if(localVidwrFd != NULL)
			formatlocation(format_vid_close);
		return 0;
	}

	if(buf == NULL || size <= 0)
		return -1;
	if(localVidwrFd == NULL)
		formatlocation(format_vid_open);
	if(localVidwrFd != NULL)
	{
		ret = fwrite(buf, 1, size, localVidwrFd);
		if(ret != size)
		{
			printf(" %s write err size=%d ret=%d \n", __func__, size, ret);
			formatlocation(format_vid_close);
		}
		else
		{
			localwrframe++;
			localwrsize += ret;
			if((localwrsize >= localsizemax) || (localwrframe >= localframemax))
				formatlocation(format_vid_close);
		}
	}
	
	return 0;
}

static int outbufCb_shot(void* arg, unsigned char *buf, int size)
{
	CustomData* pData = (CustomData *)arg;
	int ret=0;

	if(buf == NULL || size <= 0)
		return -1;

	if(localPicwrFd == NULL)
		formatlocation(format_shot_open);
	if(localPicwrFd != NULL)
	{
		ret = fwrite(buf, 1, size, localPicwrFd);
		if(ret != size)
		{
			printf(" %s write err size=%d ret=%d \n", __func__, size, ret);
		}
		formatlocation(format_shot_close);
		render.sendfile(localPicFname);
	}

	if(pData->bRec)
		pData->bRec = false;	// only push once for one shot
	return 0;
}

/* Bus messages processing, similar to all gstreamer examples */
static gboolean bus_call(GstBus *bus, GstMessage *message, gpointer user_data)
{
	GMainLoop *loop = (GMainLoop *)user_data;

	switch (GST_MESSAGE_TYPE (message))
	{
		case GST_MESSAGE_ERROR:
			{
				GError *err = NULL;
				gchar *name, *debug = NULL;
				name = gst_object_get_path_string (message->src);
				gst_message_parse_error (message, &err, &debug);
				g_printerr ("ERROR: from element %s: %s\n", name, err->message);
				if (debug != NULL)
					g_printerr ("Additional debug info:\n%s\n", debug);
				g_error_free (err);
				g_free (debug);
				g_free (name);
				g_main_loop_quit (loop);
				break;
			}
		case GST_MESSAGE_WARNING:
			{
				GError *err = NULL;
				gchar *name, *debug = NULL;
				name = gst_object_get_path_string (message->src);
				gst_message_parse_warning (message, &err, &debug);
				g_printerr ("ERROR: from element %s: %s\n", name, err->message);
				if (debug != NULL)
					g_printerr ("Additional debug info:\n%s\n", debug);
				g_error_free (err);
				g_free (debug);
				g_free (name);
				break;
			}
		case GST_MESSAGE_EOS:
			g_print ("Got EOS\n");
			g_main_loop_quit (loop);
			break;

		default:
			break;
	}
	return TRUE;
}

static void getDisplayHeight(int *width, int *height)
{
	char *display_name = getenv("DISPLAY");
	Display* display = XOpenDisplay(display_name);
	int screen_num = DefaultScreen(display);
	*width = DisplayWidth(display, screen_num);
	*height = DisplayHeight(display, screen_num);
	printf("screen w=%d , h=%d\n", *width, *height);
}

static void * thrdhndl_push_buffer(void* arg)
{
	CustomData* pData = (CustomData *)arg;
	CustomData* pDataShot = (CustomData *)pData->pushshare;
	pData->bPush = true;
	Queue *queue;
	queue=Queue::getinstance();
	//VideoWriter videowriter;
	//videowriter=VideoWriter("screen.avi", CV_FOURCC('M', 'J', 'P', 'G'), 2, Size(1920,1080));
	Mat dst, dst2;
	Mat src;
	Mat src_conv(1080,1920,CV_8UC4);
	int inputfmt=0;
	Uint32 pushcnt=0, mstimeprev=0, mstimecur;

	if(strcmp(pData->format, "BGR") == 0)
	{
		inputfmt = 1;
		dst=Mat(pData->height,pData->width,CV_8UC3,pData->bufferdata);
		if(pDataShot != NULL)
			dst2=Mat(pDataShot->height,pDataShot->width,CV_8UC3,pDataShot->bufferdata);
	}
	else if(strcmp(pData->format, "RGBA") == 0)
	{
		inputfmt = 2;
		dst=Mat(pData->height,pData->width,CV_8UC4,pData->bufferdata);
		if(pDataShot != NULL)
			dst2=Mat(pDataShot->height,pDataShot->width,CV_8UC4,pDataShot->bufferdata);
	}
	else
	{
		return NULL;
	}
	while(pData->bPush){
		//OSA_semWait(&pData->pushSem, OSA_TIMEOUT_FOREVER);
		if(!pData->bPush)
			break;
		OSA_BufInfo* bufInfo =(OSA_BufInfo*)queue->getfull(Queue::DISPALYTORTP, 0, OSA_TIMEOUT_FOREVER);
		
		if(bufInfo != NULL){
#if 0
			mstimecur = OSA_getCurTimeInMsec()/1000;
			pushcnt++;
			if(mstimecur != mstimeprev)
			{
				if(mstimeprev != 0 && mstimecur != 0)
					g_print("======== %d %s %d fps \n", OSA_getCurTimeInMsec(), __func__, pushcnt/(mstimecur-mstimeprev));
				mstimeprev = mstimecur;
				pushcnt = 0;
			}
#endif
			src=Mat(pData->height,pData->width,CV_8UC3,bufInfo->virtAddr);
			if(inputfmt == 1)
			{
				remap(src, dst, pData->Screenmapx, pData->Screenmapy, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0) );
				if(pDataShot != NULL && pDataShot->bRec)
					remap(src, dst2, pData->Screenmapx, pData->Screenmapy, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0) );
			}
			else if(inputfmt == 2)
			{
				remap(src, src_conv, pData->Screenmapx, pData->Screenmapy, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0) );
				cvtColor(src_conv, dst, CV_BGR2RGBA);
				if(pDataShot != NULL && pDataShot->bRec)
					cvtColor(src_conv, dst2, CV_BGR2RGBA);
			}
			else
			{
			}
			#if 1
			//GstBuffer *buffer = (GstBuffer *)bufInfo->physAddr;
			if(pDataShot != NULL && pDataShot->bRec)
			{
				GstBuffer *buffer =pDataShot->buffer;
				OSA_assert(buffer != NULL);
				GST_BUFFER_PTS(buffer) = pDataShot->buffer_timestamp;
				GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale_int(1, GST_SECOND, pDataShot->framerate);
				pDataShot->buffer_timestamp+=GST_BUFFER_DURATION(buffer);
				gst_buffer_ref(buffer);
				GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(pDataShot->source), buffer);
				if( ret != GST_FLOW_OK )
				{
					g_print("\n %s %d: gst_app_src_push_buffer error!\n", __func__, __LINE__);
				}
				pDataShot->bRec = false;	// only push once for one shot
			}
			if(1)
			{
				GstBuffer *buffer =pData->buffer;
				OSA_assert(buffer != NULL);
				GST_BUFFER_PTS(buffer) = pData->buffer_timestamp;
				GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale_int(1, GST_SECOND, pData->framerate);
				pData->buffer_timestamp+=GST_BUFFER_DURATION(buffer);
				gst_buffer_ref(buffer);
				//printf("*********begin*****************\n");
				GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(pData->source), buffer);
				//printf("*********end*****************\n");
				if( ret != GST_FLOW_OK )
				{
					g_print("\n %s %d: gst_app_src_push_buffer error!\n", __func__, __LINE__);
				}
			}
			#else
			//Mat src=Mat(1080,1920,CV_8UC3,bufInfo->virtAddr);
			//videowriter<<dst;
			//imshow("zhongguo",src);
			//waitKey(2);
			#endif
			queue->putempty(Queue::DISPALYTORTP, 0,bufInfo);
		}
	 
	}
	return NULL;
}

static GstPadProbeReturn encsrc_out_buffer (GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	CustomData *pData = (CustomData *)user_data;
	//gint iBufSize = 0;
	gchar* pbuffer = NULL;
	GstMapInfo map;
	GstBuffer *buffer;

	buffer = GST_PAD_PROBE_INFO_BUFFER(info);
	if(buffer == NULL)
		return GST_PAD_PROBE_OK;

	//iBufSize = gst_buffer_get_size(buffer);
	if(gst_buffer_map(buffer, &map, GST_MAP_READ))
	{
		//printf("***********%s**************\n",__func__);
		put_ring_buffer((char *)map.data, map.size);
		gst_buffer_unmap(buffer, &map);
	}

	GST_PAD_PROBE_INFO_DATA(info) = buffer;

	return GST_PAD_PROBE_OK;  //just into print one time
}

static GstPadProbeReturn fakesink_out_vidbuffer (GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	CustomData *pData = (CustomData *)user_data;
	//gint iBufSize = 0;
	gchar* pbuffer = NULL;
	GstMapInfo map;
	GstBuffer *buffer;

	buffer = GST_PAD_PROBE_INFO_BUFFER(info);
	if(buffer == NULL)
		return GST_PAD_PROBE_OK;

	//iBufSize = gst_buffer_get_size(buffer);
	if(gst_buffer_map(buffer, &map, GST_MAP_READ))
	{
		outbufCb_vid(pData, map.data, map.size);
		gst_buffer_unmap(buffer, &map);
	}

	GST_PAD_PROBE_INFO_DATA(info) = buffer;

	return GST_PAD_PROBE_OK;  //just into print one time
}

static GstPadProbeReturn fakesink_out_shotbuffer (GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	CustomData *pData = (CustomData *)user_data;
	//gint iBufSize = 0;
	gchar* pbuffer = NULL;
	GstMapInfo map;
	GstBuffer *buffer;

	buffer = GST_PAD_PROBE_INFO_BUFFER(info);
	if(buffer == NULL)
		return GST_PAD_PROBE_OK;

	//iBufSize = gst_buffer_get_size(buffer);
	if(gst_buffer_map(buffer, &map, GST_MAP_READ))
	{
		outbufCb_shot(pData, map.data, map.size);
		gst_buffer_unmap(buffer, &map);
	}

	GST_PAD_PROBE_INFO_DATA(info) = buffer;

	return GST_PAD_PROBE_OK;  //just into print one time
}
#if 1
static int gstlinkInit_appsrc_h264(CustomData* pData)	// with tee-queue
{
	if(pData == NULL)
	{
		printf("CustomData malloc failed.\n");
		return -1;
	}

	char formatgst[30]="BGR";
	//char formatgst[30]="RGBA";
	//char formatgst[30]="I420";
	getDisplayHeight(&(pData->width),&(pData->height));
	pData->framerate = 60;
	memcpy(pData->format,formatgst,sizeof(formatgst));

	pData->Screenmapx.create(Size(pData->width,pData->height), CV_32FC1 );
	pData->Screenmapy.create(Size(pData->width,pData->height), CV_32FC1 );
	for (int i = 0; i < pData->height;i++)
	{
		for (int j = 0; j < pData->width;j++)
		{
			pData->Screenmapx.at<float>(i,j)= static_cast<float>(j);
			pData->Screenmapy.at<float>(i,j)= static_cast<float>(pData->height-i);
		}
	}

	GstMapInfo *info = new GstMapInfo;
	pData->buffer = gst_buffer_new_allocate(NULL,pData->width*pData->height*4, NULL);	// for CV_BGR2RGBA need CV_8UC4 here is *4
	int iret = gst_buffer_map(pData->buffer, info, GST_MAP_WRITE);
	pData->bufferdata=info->data;

	pData->pipeline = gst_pipeline_new ("h264_pipeline");
	pData->source = gst_element_factory_make ("appsrc", NULL);
	//pData->clockoverlay = gst_element_factory_make ("clockoverlay", "clockoverlay");
	pData->videoconvert = gst_element_factory_make ("videoconvert", NULL);
	pData->tee0 = gst_element_factory_make("tee", NULL);
	pData->queue0 = gst_element_factory_make("queue", NULL);
	pData->fakesink0  = gst_element_factory_make("fakesink", NULL);
	pData->queue1 = gst_element_factory_make("queue", NULL);
	pData->fakesink1 = gst_element_factory_make ("fakesink", NULL);
	pData->nvvidconv = gst_element_factory_make ("nvvidconv", NULL);
	pData->omxh264enc = gst_element_factory_make ("omxh264enc", NULL);

	if (!pData->pipeline || !pData->source || !pData->omxh264enc)
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
	}
	if (!pData->videoconvert || !pData->nvvidconv)
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
	}
  	if (!pData->tee0 || !pData->queue0 || !pData->fakesink0 || !pData->queue1 || !pData->fakesink1)
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
	}

	pData->caps_src_to_convert = gst_caps_new_simple("video/x-raw",
								"format", G_TYPE_STRING, pData->format,
								"width", G_TYPE_INT, pData->width,
								"height", G_TYPE_INT, pData->height,
								"framerate", GST_TYPE_FRACTION, pData->framerate,
								1, NULL);
	printf("caps_src_to_convert = %s\n", gst_caps_to_string(pData->caps_src_to_convert));
	//char * capsStr = g_strdup_printf("video/x-raw(memory:NVMM),width=(int)%d,height=(int)%d,alignment=(string)au,format=(string)I420,framerate=(fraction)%d/1,pixel-aspect-ratio=(fraction)1/1", pData->width, pData->height, pData->framerate);
	//pData->caps_nvconv_to_enc = gst_caps_from_string(capsStr);
	//g_free(capsStr);
	g_object_set(G_OBJECT(pData->source), "caps", pData->caps_src_to_convert, NULL);
	g_object_set(G_OBJECT(pData->source),
				"stream-type", 0,
				"is-live", TRUE,
				//"block", TRUE,
				"do-timestamp", TRUE,
				"format", GST_FORMAT_TIME, NULL);

	gst_bin_add_many (GST_BIN(pData->pipeline), pData->source, pData->videoconvert, pData->tee0, NULL);
	gst_bin_add_many (GST_BIN(pData->pipeline), pData->queue0, pData->fakesink0, NULL);
	gst_bin_add_many (GST_BIN(pData->pipeline), pData->queue1, pData->nvvidconv, pData->omxh264enc, pData->fakesink1, NULL);
	if(!gst_element_link_many(pData->source, pData->videoconvert, pData->tee0, NULL))
	{
		g_printerr ("Elements could not be linked:pData->source--->pData->tee0.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

	//Manually link the tee0, which has "Request" pads
	pData->tee0_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (pData->tee0), "src_%u");
	pData->tee0queue0_srcpad = gst_element_request_pad (pData->tee0, pData->tee0_src_pad_template, NULL, NULL);
	pData->tee0queue1_srcpad = gst_element_request_pad (pData->tee0, pData->tee0_src_pad_template, NULL, NULL);

	GstPad *queue0_sinkpad = gst_element_get_static_pad (pData->queue0, "sink");
	if (gst_pad_link (pData->tee0queue0_srcpad, queue0_sinkpad) != GST_PAD_LINK_OK) {
		g_printerr ("GstPad could not be linked:pData->tee0--->pData->queue0.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}
	gst_object_unref (queue0_sinkpad);
	if(!gst_element_link_many(pData->queue0, pData->fakesink0, NULL))
	{
		g_printerr ("Elements could not be linked:pData->queue0--->pData->fakesink0.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

	GstPad *queue1_sinkpad = gst_element_get_static_pad (pData->queue1, "sink");
	if (gst_pad_link (pData->tee0queue1_srcpad, queue1_sinkpad) != GST_PAD_LINK_OK) {
		g_printerr ("GstPad could not be linked:pData->tee0--->pData->queue1.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}
	gst_object_unref (queue1_sinkpad);
	if(!gst_element_link_many(pData->queue1, pData->nvvidconv, pData->omxh264enc, NULL))
	{
		g_printerr ("Elements could not be linked:pData->queue1--->pData->omxh264enc.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}
	pData->caps_enc_to_rtp = gst_caps_new_simple("video/x-h264",
										"stream-format", G_TYPE_STRING, "byte-stream",
										"width", G_TYPE_INT, pData->width,
										"height", G_TYPE_INT, pData->height,
										"framerate", GST_TYPE_FRACTION,  pData->framerate,
										1, NULL);
	if(!gst_element_link_filtered(pData->omxh264enc, pData->fakesink1, pData->caps_enc_to_rtp))
	{
		g_printerr ("Elements could not be filtered:pData->caps_enc_to_rtp.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

	g_object_set (pData->omxh264enc, "iframeinterval", pData->framerate, NULL);
	g_object_set (pData->omxh264enc, "bitrate", GST_ENCBITRATE, NULL);
	g_object_set (pData->omxh264enc, "control-rate", 2, NULL);
	//g_object_set (pData->omxh264enc, "insert-sps-pps", 1, NULL);		// mp4 need h264 noneed

	GstPad *encsrc_pad = gst_element_get_static_pad(pData->omxh264enc, "src");
	gst_pad_add_probe (encsrc_pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback)encsrc_out_buffer, pData, NULL);	// for rtsp
	gst_object_unref(encsrc_pad);
	GstPad *fakesink_pad = gst_element_get_static_pad(pData->fakesink1, "sink");
	gst_pad_add_probe (fakesink_pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback)fakesink_out_vidbuffer, pData, NULL);	// for record
	gst_object_unref(fakesink_pad);

	//gst_element_sync_state_with_parent (pData->source);
	//gst_element_sync_state_with_parent (pData->videoconvert);
	//gst_element_sync_state_with_parent (pData->queue0);
	//gst_element_sync_state_with_parent (pData->fakesink0);
	gst_element_sync_state_with_parent (pData->queue1);
	gst_element_sync_state_with_parent (pData->fakesink1);
	gst_element_sync_state_with_parent (pData->nvvidconv);
	gst_element_sync_state_with_parent (pData->omxh264enc);

	pthread_create(&pData->threadPushBuffer, NULL, thrdhndl_push_buffer, (void*)pData);
	OSA_assert(pData->threadPushBuffer != 0);

	g_print("\n\ngst screen ench264 starting ...\n\n");

	/* Create gstreamer loop */
	pData->loop = g_main_loop_new(NULL, FALSE);
	pData->ret = gst_element_set_state (pData->pipeline, GST_STATE_PLAYING);
	if (pData->ret == GST_STATE_CHANGE_FAILURE)
	{
		g_printerr ("Unable to set the data.pipeline to the playing state.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

  	/* Wait until error or EOS */
	pData->bus = gst_element_get_bus(pData->pipeline);
  	gst_bus_add_watch(pData->bus, bus_call, pData->loop);

  	return 0;
}
#else
static int gstlinkInit_appsrc_h264(CustomData* pData)	// without tee-queue
{
	if(pData == NULL)
	{
		printf("CustomData malloc failed.\n");
		return -1;
	}

	//char formatgst[30]="BGR";
	char formatgst[30]="RGBA";
	//char formatgst[30]="I420";
	getDisplayHeight(&(pData->width),&(pData->height));
	pData->framerate = 60;
	memcpy(pData->format,formatgst,sizeof(formatgst));

	pData->Screenmapx.create(Size(pData->width,pData->height), CV_32FC1 );
	pData->Screenmapy.create(Size(pData->width,pData->height), CV_32FC1 );

	for (int i = 0; i < pData->height;i++)
	{
		for (int j = 0; j < pData->width;j++)
		{
			pData->Screenmapx.at<float>(i,j)= static_cast<float>(j);
			pData->Screenmapy.at<float>(i,j)= static_cast<float>(pData->height-i);
		}
	}

	GstMapInfo *info = new GstMapInfo;
	pData->buffer = gst_buffer_new_allocate(NULL,pData->width*pData->height*4, NULL);	// for CV_BGR2RGBA need CV_8UC4 here is *4
	int iret = gst_buffer_map(pData->buffer, info, GST_MAP_WRITE);
	pData->bufferdata=info->data;

	pData->pipeline = gst_pipeline_new ("h264_pipeline");
	pData->source = gst_element_factory_make ("appsrc", NULL);
	//pData->clockoverlay = gst_element_factory_make ("clockoverlay", "clockoverlay");
	//pData->videoconvert = gst_element_factory_make ("videoconvert", NULL);
	pData->nvvidconv = gst_element_factory_make ("nvvidconv", NULL);
	pData->omxh264enc = gst_element_factory_make ("omxh264enc", NULL);
	pData->fakesink0  = gst_element_factory_make("fakesink", NULL);

	pData->caps_src_to_convert = gst_caps_new_simple("video/x-raw",
								"format", G_TYPE_STRING, pData->format,
								"width", G_TYPE_INT, pData->width,
								"height", G_TYPE_INT, pData->height,
								"framerate", GST_TYPE_FRACTION, pData->framerate,
								1, NULL);
	printf("caps_src_to_convert = %s\n", gst_caps_to_string(pData->caps_src_to_convert));
	//char * capsStr = g_strdup_printf("video/x-raw(memory:NVMM),width=(int)%d,height=(int)%d,alignment=(string)au,format=(string)I420,framerate=(fraction)%d/1,pixel-aspect-ratio=(fraction)1/1", pData->width, pData->height, pData->framerate);
	//pData->caps_nvconv_to_enc = gst_caps_from_string(capsStr);
	//g_free(capsStr);
	g_object_set(G_OBJECT(pData->source), "caps", pData->caps_src_to_convert, NULL);
	g_object_set(G_OBJECT(pData->source),
				"stream-type", 0,
				"is-live", TRUE,
				//"block", TRUE,
				"do-timestamp", TRUE,
				"format", GST_FORMAT_TIME, NULL);

	if (!pData->pipeline || !pData->source || !pData->omxh264enc || !pData->fakesink0)
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
	}
	if (/*!pData->videoconvert || */!pData->nvvidconv)
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
	}

	gst_bin_add_many (GST_BIN(pData->pipeline), pData->source, /*pData->clockoverlay, *//*pData->videoconvert, */pData->nvvidconv, pData->omxh264enc, pData->fakesink0, NULL);
	if(!gst_element_link_many(pData->source, /*pData->clockoverlay, *//*pData->videoconvert, */pData->nvvidconv, pData->omxh264enc, NULL))
	{
		g_printerr ("Elements could not be linked:pData->source--->pData->omxh264enc.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

	pData->caps_enc_to_rtp = gst_caps_new_simple("video/x-h264",
										"stream-format", G_TYPE_STRING, "byte-stream",
										"width", G_TYPE_INT, pData->width,
										"height", G_TYPE_INT, pData->height,
										"framerate", GST_TYPE_FRACTION,  pData->framerate,
										1, NULL);
	if(!gst_element_link_filtered(pData->omxh264enc, pData->fakesink0, pData->caps_enc_to_rtp))
	{
		g_printerr ("Elements could not be filtered:pData->caps_enc_to_rtp.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

	g_object_set (pData->omxh264enc, "iframeinterval", pData->framerate, NULL);
	g_object_set (pData->omxh264enc, "bitrate", GST_ENCBITRATE, NULL);
	//g_object_set (pData->omxh264enc, "insert-sps-pps", 1, NULL);		// mp4 need h264 noneed
	//g_object_set (pData->omxh264enc, "control-rate", 2, NULL);

	GstPad *encsrc_pad = gst_element_get_static_pad(pData->omxh264enc, "src");
	gst_pad_add_probe (encsrc_pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback)encsrc_out_buffer, pData, NULL);	// for rtsp
	gst_object_unref(encsrc_pad);
	GstPad *fakesink_pad = gst_element_get_static_pad(pData->fakesink0, "sink");
	gst_pad_add_probe (fakesink_pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback)fakesink_out_buffer, pData, NULL);	// for record
	gst_object_unref(fakesink_pad);

	gst_element_sync_state_with_parent (pData->source);
	//gst_element_sync_state_with_parent (pData->videoconvert);
	gst_element_sync_state_with_parent (pData->nvvidconv);
	gst_element_sync_state_with_parent (pData->omxh264enc);
	gst_element_sync_state_with_parent (pData->fakesink0);

	pthread_create(&pData->threadPushBuffer, NULL, thrdhndl_push_buffer, (void*)pData);
	OSA_assert(pData->threadPushBuffer != 0);

	g_print("\n\ngst screen ench264 starting ...\n\n");

	/* Create gstreamer loop */
	pData->loop = g_main_loop_new(NULL, FALSE);
	pData->ret = gst_element_set_state (pData->pipeline, GST_STATE_PLAYING);
	if (pData->ret == GST_STATE_CHANGE_FAILURE)
	{
		g_printerr ("Unable to set the data.pipeline to the playing state.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

  	/* Wait until error or EOS */
	pData->bus = gst_element_get_bus(pData->pipeline);
  	gst_bus_add_watch(pData->bus, bus_call, pData->loop);

  	return 0;
}
#endif

/*static void * record_main_loop(void* arg)
{
	CustomData* pCustomData = (CustomData *)arg;
	g_main_loop_run(pCustomData->loop); //链路停止，释放资源。

	printf("record_main_loop done.\n");

	return NULL;
}*/

static int gstlinkInit_appsrc_shot(CustomData* pData)
{
	if(pData == NULL)
	{
		printf("CustomData malloc failed.\n");
		return -1;
	}

	char formatgst[30]="BGR";
	//char formatgst[30]="RGBA";
	getDisplayHeight(&(pData->width),&(pData->height));
	pData->framerate = 60;
	memcpy(pData->format,formatgst,sizeof(formatgst));
#if 0
	pData->Screenmapx.create(Size(pData->width,pData->height), CV_32FC1 );
	pData->Screenmapy.create(Size(pData->width,pData->height), CV_32FC1 );
	for (int i = 0; i < pData->height;i++)
	{
		for (int j = 0; j < pData->width;j++)
		{
			pData->Screenmapx.at<float>(i,j)= static_cast<float>(j);
			pData->Screenmapy.at<float>(i,j)= static_cast<float>(pData->height-i);
		}
	}
#endif

	GstMapInfo *info = new GstMapInfo;
	pData->buffer = gst_buffer_new_allocate(NULL,pData->width*pData->height*4, NULL);	// for CV_BGR2RGBA need CV_8UC4 here is *4
	int iret = gst_buffer_map(pData->buffer, info, GST_MAP_WRITE);
	pData->bufferdata=info->data;

	pData->pipeline = gst_pipeline_new ("shot_pipeline");
	//pData->source = gst_element_factory_make ("ximagesrc", NULL);
	pData->source = gst_element_factory_make ("appsrc", NULL);
	//pData->clockoverlay = gst_element_factory_make ("clockoverlay", "clockoverlay");
	pData->videoconvert = gst_element_factory_make ("videoconvert", NULL);
	pData->jpegenc = gst_element_factory_make ("jpegenc", NULL);
	//pData->nvvidconv = gst_element_factory_make ("nvvidconv", NULL);
	//pData->jpegenc = gst_element_factory_make ("nvjpegenc", NULL);
	pData->fakesink0  = gst_element_factory_make("fakesink", NULL);

#if 1
	pData->caps_src_to_convert = gst_caps_new_simple("video/x-raw",
								"format", G_TYPE_STRING, pData->format,
								"width", G_TYPE_INT, pData->width,
								"height", G_TYPE_INT, pData->height,
								"framerate", GST_TYPE_FRACTION, pData->framerate,
								1, NULL);
	printf("caps_src_to_convert = %s\n", gst_caps_to_string(pData->caps_src_to_convert));
	//char * capsStr = g_strdup_printf("video/x-raw(memory:NVMM),width=(int)%d,height=(int)%d,alignment=(string)au,format=(string)I420,framerate=(fraction)%d/1,pixel-aspect-ratio=(fraction)1/1", pData->width, pData->height, pData->framerate);
	//pData->caps_nvconv_to_enc = gst_caps_from_string(capsStr);
	//g_free(capsStr);
	g_object_set(G_OBJECT(pData->source), "caps", pData->caps_src_to_convert, NULL);
	g_object_set(G_OBJECT(pData->source),
				"stream-type", 0,
				"is-live", TRUE,
				//"block", TRUE,
				"do-timestamp", TRUE,
				"format", GST_FORMAT_TIME, NULL);
#else
	g_object_set(pData->source, "use-damage", 0, NULL);
#endif

	if (!pData->pipeline || !pData->source || !pData->jpegenc || !pData->fakesink0)
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
	}
	if (!pData->videoconvert /*|| !pData->nvvidconv*/)
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
	}

	gst_bin_add_many (GST_BIN(pData->pipeline), pData->source, /*pData->clockoverlay, */pData->videoconvert, /*pData->nvvidconv, */pData->jpegenc, pData->fakesink0, NULL);
	if(!gst_element_link_many(pData->source, /*pData->clockoverlay, */pData->videoconvert, /*pData->nvvidconv, */pData->jpegenc, NULL))
	{
		g_printerr ("Elements could not be linked:pData->source--->pData->jpegenc.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

	pData->caps_enc_to_jpeg = gst_caps_new_simple("image/jpeg",
								"width", G_TYPE_INT, pData->width,
								"height", G_TYPE_INT, pData->height,
								"framerate", GST_TYPE_FRACTION, pData->framerate,
								1, NULL);
	if(!gst_element_link_filtered(pData->jpegenc, pData->fakesink0, pData->caps_enc_to_jpeg))
	{
		g_printerr ("Elements could not be filtered:pData->caps_enc_to_jpeg.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

	GstPad *fakesink_pad = gst_element_get_static_pad(pData->fakesink0, "sink");
	gst_pad_add_probe (fakesink_pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback)fakesink_out_shotbuffer, pData, NULL);	// for record
	gst_object_unref(fakesink_pad);

	//gst_element_sync_state_with_parent (pData->source);
	//gst_element_sync_state_with_parent (pData->videoconvert);
	//gst_element_sync_state_with_parent (pData->nvvidconv);
	//gst_element_sync_state_with_parent (pData->jpegenc);
	//gst_element_sync_state_with_parent (pData->fakesink0);

#if 0
	pthread_create(&pData->threadPushBuffer, NULL, thrdhndl_push_buffer, (void*)pData);
	OSA_assert(pData->threadPushBuffer != 0);
#endif

	g_print("\n\ngst screen shot starting ...\n\n");

	/* Create gstreamer loop */
	pData->loop = g_main_loop_new(NULL, FALSE);
	pData->ret = gst_element_set_state (pData->pipeline, GST_STATE_PLAYING);
	if (pData->ret == GST_STATE_CHANGE_FAILURE)
	{
		g_printerr ("Unable to set the data.pipeline to the playing state.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

  	/* Wait until error or EOS */
	pData->bus = gst_element_get_bus(pData->pipeline);
  	gst_bus_add_watch(pData->bus, bus_call, pData->loop);

  	return 0;
}

static int screenInit(CustomData* pDataVid, CustomData* pDataShot)
{
	int res;
	printf("\r\n screenCapture  Build date: %s %s \r\n", __DATE__, __TIME__);

	res = gstlinkInit_appsrc_shot(pDataShot);	// for shot use share inpush so must run before thrdhndl_push_buffer()
	if(res == -1)
		return -1;
	pDataVid->pushshare = (void *)pDataShot;

	//res = gstlinkInit_appsrc_mp4(pData);
	res = gstlinkInit_appsrc_h264(pDataVid);
	if(res == -1)
		return -1;
/*
	pthread_t thread_0;
	res = pthread_create(&thread_0, NULL, record_main_loop, (void*)&customData);
	if(res == -1)
		return -1;
*/
	return 0;
}

static int screenUninit()
{
	return 0;
}

/******************************/
CustomData customData[2];
int gst_videnc_create(void)
{
	historylocation();
	memset(&customData, 0, sizeof(customData));
	screenInit(&customData[link_h264], &customData[link_shot]);
	return 0;
}

int gst_videnc_enable(int bEnable)
{
	customData[link_h264].bRec = (bEnable)?true:false;
	printf("gst set screen vid enable %d\n", customData[link_h264].bRec);
	return 0;
}

int gst_shotenc_record(void)
{
	customData[link_shot].bRec = true;
	printf("gst set screen shot\n");
	return 0;
}

