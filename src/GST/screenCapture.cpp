//#include <demo_global_def.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/gstclock.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "screenCapture.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include <iostream>
#include <demo_global_def.h>
#include <X11/Xlib.h>
#include <gst_interfaces.h>
#include <unistd.h>
#include <time.h>

#include"Queuebuffer.hpp"
#include <gst_ring_buffer.h>


using namespace std;
using namespace cv;

int (*screenGetData) (unsigned char * buf, int len);

typedef struct _bufDataPP
{
	unsigned char *buf;
	int dataLenth;
	int used;
}bufDataPP;

static bufDataPP g_useObj[2];

typedef struct _CustomData
{
  	GstElement *pipeline, *source, *videoconvert0, *tee0, *queue0, *fakesink0;
  	GstElement *queue1, *nvvidconv0, *omxh265enc, *h265parse, *tee1, *queue2, *splitmuxsink1, *mp4mux1, *filesink1, *udpsink, *rtph265pay;
  	GstElement *queue3, *filesink2;

	GstBus *bus;
	GMainLoop *loop;
	gboolean playing;

	GstStateChangeReturn ret;

  	GstCaps *	caps_enc_to_rtp_265;
	GstCaps *caps_src_to_convert;
	GstCaps *caps_nvconv_to_enc;

	GstPad *nvvidconv0_srcpad;
	GstPad *videoconvert0_srcpad;

	GstPadTemplate *tee0_src_pad_template;
	GstPad *tee0queue0_srcpad;
	GstPad *tee0queue1_srcpad;

	GstPadTemplate *tee1_src_pad_template;
	GstPad *tee1queue2_srcpad;
	GstPad *tee1queue3_srcpad;

	GstClockTime buffer_timestamp;
	bool bPush;

	int height;
	int width;
	int framerate;
	char format[30];

	gchar* filename_format;
	int tempfileNum;

	gboolean stopInput;
	int testCount;

	char ipAddr[256];

	pthread_t threadPushBuffer;

	int frame_cnt;
	int frame_Bps;
	Mat Screenmapx;
	Mat Screenmapy;

	GstBuffer *buffer;
	void *bufferdata;

} CustomData;


typedef struct _recordHandle
{
	int index;
	void* context;
	unsigned int width;
	unsigned int height;
	unsigned int framerate;
	unsigned int bitrate;
	char format[30];
	char ip_addr[30];
	unsigned int ip_port;
	unsigned short bEnable;
	
	OSA_BufHndl *pushQueue;
	OSA_SemHndl *pushSem;
}RecordHandle;

CustomData customData;


#define APPSRC (1)

static GstPadProbeReturn filesink1_buffer (GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	CustomData *pData = (CustomData *)user_data;
	gint iBufSize = 0;
	gchar* pbuffer = NULL;
	gint x,y;
	GstMapInfo map;
	guint16 *ptr, t;
	GstBuffer *buffer;
	DATAHEADER dataPrm;

	buffer = GST_PAD_PROBE_INFO_BUFFER(info);

	if(buffer == NULL)
		return GST_PAD_PROBE_OK;

	//iBufSize = gst_buffer_get_size(buffer);

	if(gst_buffer_map(buffer, &map, GST_MAP_READ))
	{


	     //printf("***********%s**************\n",__func__);
	     put_ring_buffer((char *)map.data, map.size);
		/*
		if(screenGetData!= NULL)
		{
			//malloc 2 p1 p2
			screenGetData(map.data, map.size);
		}else{
			bufDataPP *pObj = NULL;
			for(int i=0; i<2; i++){
				pObj = &g_useObj[i];
				if(pObj->used == 0){
					memcpy(pObj->buf, map.data, map.size);
					pObj->dataLenth = map.size;
					pObj->used = 1;
				}else{
					pObj->dataLenth = 0;
					pObj->used = 0;
				}
				//printf("[%d] %p:%d\n", i, pObj->buf, pObj->dataLenth);
			}
		}
		*/
		

		gst_buffer_unmap(buffer, &map);
	}

	GST_PAD_PROBE_INFO_DATA(info) = buffer;

	return GST_PAD_PROBE_OK;  //just into print one time
}

gchararray split_buffer(GstElement* object, guint arg0, gpointer user_data)
{
	GstElement *splitmuxsink = (GstElement*)object;
	CustomData *pData = (CustomData *)user_data;
	char outPath[128];
	memset(outPath, 0, sizeof(outPath));
	//RDERDEV_getFileformat(pData->pRderDev, outPath, arg0);

	g_object_set(splitmuxsink, "location", outPath, NULL);

	return 0;
}


//enc_buffer, 添加探针，从H265中获取数据。
static GstPadProbeReturn enc_buffer (GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	gint iBufSize = 0;
	gchar* pbuffer = NULL;

	gint x,y;
	GstMapInfo map;
	guint16 *ptr, t;
	GstBuffer *buffer;

	buffer = GST_PAD_PROBE_INFO_BUFFER(info);

	if(buffer == NULL)
	{
		g_print("no data.\n");
		return GST_PAD_PROBE_OK;
	}

	if(gst_buffer_map(buffer, &map, GST_MAP_READ))
	{
		ptr = (guint16 *)map.data;
		gint iBufSize = 0;
		iBufSize = gst_buffer_get_size(buffer);

		//g_print (" buffer size =%d,appsink0_buffer = %02x %02x %02x %02x %02x\n", iBufSize, map.data[0], map.data[1], map.data[2], map.data[3], map.data[4]);

		gst_buffer_unmap(buffer, &map);
	}

	GST_PAD_PROBE_INFO_DATA(info) = buffer;

	return GST_PAD_PROBE_OK; 		//just into print one time
}


#define UDPSINK  (0)
#define GST_ENCBITRATE	(5600000)
static GstPadProbeReturn enc_tick_cb(GstPad * pad, GstPadProbeInfo * info, gpointer user_data)
{
	CustomData *pData = (CustomData *)user_data;
	GstPad *sinkpad, *srcpad;

	if(!GST_IS_ELEMENT (pData->nvvidconv0))
	{
		g_print ("Enable record\n");
	}
	else
	{
		g_print ("Already enable record\n");
		return GST_PAD_PROBE_REMOVE;
	}

	pData->nvvidconv0 = gst_element_factory_make ("nvvidconv", NULL);
	pData->omxh265enc = gst_element_factory_make ("omxh264enc", NULL);
	pData->queue1 = gst_element_factory_make("queue", NULL);
	
	if(UDPSINK)
		{
			pData->udpsink = gst_element_factory_make("udpsink", "udpsink");
			pData->rtph265pay = gst_element_factory_make("rtph265pay", "rtph265pay");
		}
	else
		{
			pData->filesink1 = gst_element_factory_make("fakesink", NULL);
		}

	gst_object_ref(pData->queue1);
	gst_object_ref(pData->nvvidconv0);
	gst_object_ref(pData->omxh265enc);
	
	if(UDPSINK)
		{
			gst_object_ref(pData->rtph265pay);
			gst_object_ref(pData->udpsink);
		}
	else
		{
			gst_object_ref(pData->filesink1);
		}
	if(UDPSINK)
		{
			gst_bin_add_many (GST_BIN(pData->pipeline), pData->queue1, pData->nvvidconv0, pData->omxh265enc, pData->rtph265pay, pData->udpsink,  NULL);
		}
	else
	gst_bin_add_many (GST_BIN(pData->pipeline), pData->queue1, pData->nvvidconv0, pData->omxh265enc, pData->filesink1, NULL);
	//

	GstPad *queue1_sinkpad = gst_element_get_static_pad (pData->queue1, "sink");
	if (gst_pad_link (pData->tee0queue1_srcpad, queue1_sinkpad) != GST_PAD_LINK_OK)
	{
		g_printerr ("Tee0 could not be linked : data->queue1_sinkpad.\n");
		gst_object_unref (queue1_sinkpad);
		return GST_PAD_PROBE_REMOVE;
	}
	gst_object_unref (queue1_sinkpad);

	if(!gst_element_link_many(pData->queue1, pData->nvvidconv0, pData->omxh265enc, NULL))
	{
		g_printerr ("Elements could not be linked:pData->nvvidconv0--->pData->omxh265enc.\n");
		gst_object_unref (pData->pipeline);
		return (GstPadProbeReturn)-1;
	}

	if(UDPSINK)
		{
			    if(!gst_element_link_filtered(pData->omxh265enc, pData->rtph265pay, pData->caps_enc_to_rtp_265))
				{
					g_printerr ("Elements could not be linked.\n");
					gst_object_unref (pData->pipeline);
					return (GstPadProbeReturn)-1;
				}

			    if(!gst_element_link_many(pData->rtph265pay, pData->udpsink, NULL))
				{
			        g_printerr ("Elements could not be linked:data->rtph265pay ---> data->udpsink.\n");
					gst_object_unref (pData->pipeline);
					return (GstPadProbeReturn)-1;
				}
			

		}
	else
		{
			if(!gst_element_link_filtered(pData->omxh265enc, pData->filesink1, pData->caps_enc_to_rtp_265))
				{
					g_printerr ("Elements could not be linked.\n");
					gst_object_unref (pData->pipeline);
					return (GstPadProbeReturn)-1;
				}
		}

	g_object_set (pData->omxh265enc, "iframeinterval", 60, NULL);
	g_object_set (pData->omxh265enc, "bitrate", GST_ENCBITRATE, NULL);
	g_object_set (pData->omxh265enc, "control-rate", 2, NULL);
	g_object_set (pData->omxh265enc, "quality-level", 2, NULL);
	

	if(UDPSINK)
		{
			g_object_set (pData->udpsink, "host","192.168.1.26", NULL);
			g_object_set (pData->udpsink, "port", 16000, NULL);

		}
	if(UDPSINK)
		{
		;
		}
		else
		{
			GstPad *filesink1_pad = gst_element_get_static_pad(pData->filesink1, "sink");
			gst_pad_add_probe (filesink1_pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback)filesink1_buffer, pData, NULL);
			gst_object_unref(filesink1_pad);
		}

	gst_element_sync_state_with_parent (pData->queue1);
	gst_element_sync_state_with_parent (pData->nvvidconv0);
	gst_element_sync_state_with_parent (pData->omxh265enc);
	if(UDPSINK)
		{
			gst_element_sync_state_with_parent (pData->rtph265pay);
			gst_element_sync_state_with_parent (pData->udpsink);
		}
	else
		gst_element_sync_state_with_parent (pData->filesink1);

	g_print ("\nEnable record done\n");

	return GST_PAD_PROBE_REMOVE;
}

static GstPadProbeReturn enc_unlink_cb(GstPad * pad, GstPadProbeInfo * info, gpointer user_data)
{
	CustomData *pData = (CustomData *)user_data;
	GstPad *srcpad, *sinkpad;

	if(!GST_IS_ELEMENT (pData->queue1))
	{
		return GST_PAD_PROBE_REMOVE;
	}

	g_print (">>> send eos ...\n");

	//Send an Eos on the queue
	GstPad * queueSinkPad = gst_element_get_static_pad(pData->queue1, "sink");
	if(!queueSinkPad)
	{
		g_critical("stop pipeline: could not obtain the queueSinkPad sinkpad!!\n");
		return GST_PAD_PROBE_REMOVE;
	}
	gst_pad_send_event(queueSinkPad, gst_event_new_eos());
	gst_object_unref(queueSinkPad);

	GST_DEBUG_OBJECT(pad,"pad is blocked now");
	/* remove the probe first. */
	gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));

	sinkpad = gst_element_get_static_pad(pData->queue1, "sink");
	gst_pad_unlink (pData->tee0queue1_srcpad, sinkpad);
	gst_object_unref (sinkpad);

	//gst_element_unlink_many(pData->queue1,pData->nvvidconv0, pData->omxh265enc, pData->rtph265pay,  pData->udpsink ,NULL);
	gst_element_unlink_many(pData->queue1,pData->nvvidconv0, pData->omxh265enc, pData->filesink1, NULL);

	gst_element_set_state (pData->queue1, GST_STATE_NULL);
	gst_element_set_state (pData->nvvidconv0, GST_STATE_NULL);
	gst_element_set_state (pData->omxh265enc, GST_STATE_NULL);
	gst_element_set_state (pData->filesink1, GST_STATE_NULL);
	//gst_element_set_state (pData->rtph265pay, GST_STATE_NULL);
	//gst_element_set_state (pData->udpsink, GST_STATE_NULL);

	gst_bin_remove (GST_BIN (pData->pipeline), pData->queue1);
	gst_bin_remove (GST_BIN (pData->pipeline), pData->nvvidconv0);
	gst_bin_remove (GST_BIN (pData->pipeline), pData->omxh265enc);
	gst_bin_remove (GST_BIN (pData->pipeline), pData->filesink1);
	//gst_bin_remove (GST_BIN (pData->pipeline), pData->rtph265pay);
	//gst_bin_remove (GST_BIN (pData->pipeline), pData->udpsink);

	gst_object_unref (pData->queue1);
	gst_object_unref (pData->nvvidconv0);
	gst_object_unref (pData->omxh265enc);
	gst_object_unref (pData->filesink1);
	//gst_object_unref (pData->rtph265pay);
	//gst_object_unref (pData->udpsink);

	pData->filesink1 = NULL;
	//pData->udpsink = NULL;
	//pData->rtph265pay = NULL;
	pData->omxh265enc = NULL;
	pData->nvvidconv0 = NULL;
	pData->queue1 = NULL;

	g_print (">>> disable record done ...\n");

	return GST_PAD_PROBE_OK;
}

/* Bus messages processing, similar to all gstreamer examples */

gboolean bus_call(GstBus *bus, GstMessage *message, gpointer user_data)
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


void CustomData_init(CustomData *pData)
{
	pData->queue1 = NULL;
	pData->omxh265enc = NULL;
	pData->nvvidconv0 = NULL;
	pData->splitmuxsink1 = NULL;
	pData->mp4mux1 = NULL;
	pData->filesink1 = NULL;
}

static void getDisplayHeight(int *width, int *height)
{
	char *display_name = getenv("DISPLAY");
	Display* display = XOpenDisplay(display_name);
	int screen_num = DefaultScreen(display);
	*width = DisplayWidth(display, screen_num);
	*height = DisplayHeight(display, screen_num);
	printf("\nscreen w=%d , h=%d\n", *width, *height);
}

int screenEnable(unsigned short bEnable)
{
	CustomData* pData = &customData;
	char cmdBuf[128];
	if(bEnable)
	{
		gst_pad_add_probe (pData->tee0queue1_srcpad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, enc_tick_cb, pData, (GDestroyNotify)NULL);
	}
	else
	{
		gst_pad_add_probe (pData->tee0queue1_srcpad, GST_PAD_PROBE_TYPE_IDLE, enc_unlink_cb, pData, (GDestroyNotify)NULL);
	}
	return 0;
}


static void * thrdhndl_push_buffer(void* arg)
{
	CustomData* pData = (CustomData *)arg;
	pData->bPush = true;
	Queue *queue;
	queue=Queue::getinstance();
	VideoWriter videowriter;
	videowriter=VideoWriter("screen.avi", CV_FOURCC('M', 'J', 'P', 'G'), 2, Size(1920,1080));
	Mat dst=Mat(pData->height,pData->width,CV_8UC3,pData->bufferdata);
	Mat src;
	while(pData->bPush){
		//OSA_semWait(&pData->pushSem, OSA_TIMEOUT_FOREVER);
		if(!pData->bPush)
			break;
		OSA_BufInfo* bufInfo =(OSA_BufInfo*)queue->getfull(Queue::DISPALYTORTP, 0, OSA_TIMEOUT_FOREVER);
		
		if(bufInfo != NULL){
			src=Mat(pData->height,pData->width,CV_8UC3,bufInfo->virtAddr);
			remap(src, dst, pData->Screenmapx, pData->Screenmapy, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0) );
			#if 1
			//GstBuffer *buffer = (GstBuffer *)bufInfo->physAddr;
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
			#else
			//Mat src=Mat(1080,1920,CV_8UC3,bufInfo->virtAddr);
			videowriter<<dst;
			//imshow("zhongguo",src);
			//waitKey(2);

			#endif
			queue->putempty(Queue::DISPALYTORTP, 0,bufInfo);
			
		}
	 
	}
	return NULL;
}
char formatgst[30]="BGR";


int record_img_init(int width,int height,char *formatgst)
{
	/*
	RecordHandle * recordHandle = (RecordHandle *)malloc(sizeof(RecordHandle));
	memset(recordHandle, 0, sizeof(RecordHandle));
	recordHandle->width = width;
	recordHandle->height = height;
	recordHandle->framerate = 20;
	OSA_assert(gstCapture_data.format!=NULL);
	strcpy(recordHandle->format, formatgst);
	recordHandle->context = (CustomData *)malloc(sizeof(CustomData));
	CustomData* pData = (CustomData* )recordHandle->context;
	memset(pData, 0, sizeof(CustomData));
	pData->height = recordHandle->height;
	pData->width = recordHandle->width;
	pData->framerate = recordHandle->framerate;
	strcpy(pData->format, recordHandle->format);

	pData->queue1 = NULL;
	pData->omxh265enc = NULL;
	pData->nvvidconv0 = NULL;
	pData->fakesink1 = NULL;
	*/

}
int record_main_init()
{
	CustomData* pData = &customData;
	if(pData == NULL)
	{
		printf("CustomData malloc failed.\n");
		return -1;
	}

	getDisplayHeight( &(pData->width),&(pData->height));

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
	
	pData->buffer = gst_buffer_new_allocate(NULL,pData->width*pData->height*3, NULL);
	int iret = gst_buffer_map(pData->buffer, info, GST_MAP_WRITE);
	pData->bufferdata=info->data;
	CustomData_init(pData);

  	//创建元件
	//
	if(APPSRC)
	pData->source = gst_element_factory_make ("appsrc", NULL);
	else
	pData->source = gst_element_factory_make ("ximagesrc", NULL);
	
	pData->videoconvert0 = gst_element_factory_make ("videoconvert", NULL);
	pData->tee0 = gst_element_factory_make("tee", NULL);
	pData->queue0 = gst_element_factory_make("queue", NULL);
	pData->fakesink0 = gst_element_factory_make ("fakesink", NULL);
	//创建空的管道
	pData->pipeline = gst_pipeline_new ("test-pipelinertsp");

	pData->caps_enc_to_rtp_265 = gst_caps_new_simple("video/x-h264",
										"stream-format", G_TYPE_STRING, "byte-stream",
										"width", G_TYPE_INT, pData->width,
										"height", G_TYPE_INT, pData->height,
										"framerate", GST_TYPE_FRACTION,  pData->framerate, 1,
										 NULL);

	if(APPSRC)
		{
			pData->caps_src_to_convert = gst_caps_new_simple("video/x-raw",
										"format", G_TYPE_STRING, pData->format,
										"width", G_TYPE_INT, pData->width,
										"height", G_TYPE_INT, pData->height,
										"framerate", GST_TYPE_FRACTION, pData->framerate ,
										1,
										 NULL);
		printf("caps_src_to_convert = %s\n", gst_caps_to_string(pData->caps_src_to_convert));
		char * capsStr = g_strdup_printf("video/x-raw(memory:NVMM),width=(int)%d,height=(int)%d,alignment=(string)au,format=(string)I420,framerate=(fraction)%d/1,pixel-aspect-ratio=(fraction)1/1", pData->width, pData->height, pData->framerate);
		pData->caps_nvconv_to_enc = gst_caps_from_string(capsStr);
		g_free(capsStr);

		g_object_set(G_OBJECT(pData->source), "caps", pData->caps_src_to_convert, NULL);
		g_object_set(G_OBJECT(pData->source),
					"stream-type", 0,
					"is-live", TRUE,
					//"block", TRUE,
					"do-timestamp", TRUE,
					"format", GST_FORMAT_TIME, NULL);

		}
	else
	g_object_set(pData->source, "use-damage", 0, NULL);

  	if (!pData->pipeline || !pData->source || !pData->videoconvert0 || !pData->tee0 || !pData->queue0 || !pData->fakesink0 )
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
    }

	gst_bin_add_many (GST_BIN(pData->pipeline), pData->source, pData->videoconvert0, pData->tee0, pData->queue0, pData->fakesink0, NULL);

	if(!gst_element_link_many(pData->source, pData->videoconvert0, pData->tee0, NULL))
	{
		g_printerr ("Elements could not be linked:data.source->data0.videoconvert0.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

	if(!gst_element_link_many(pData->queue0, pData->fakesink0, NULL))
	{
		g_printerr ("Elements could not be linked:pData->source   pData->videoconvert0.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

	//Manually link the tee0, which has "Request" pads
	pData->tee0_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (pData->tee0), "src_%u");

	pData->tee0queue0_srcpad = gst_element_request_pad (pData->tee0, pData->tee0_src_pad_template, NULL, NULL);
	pData->tee0queue1_srcpad = gst_element_request_pad (pData->tee0, pData->tee0_src_pad_template, NULL, NULL);

	GstPad *queue0filesink0_sinkpad = gst_element_get_static_pad (pData->queue0, "sink");
	
	if (gst_pad_link (pData->tee0queue0_srcpad, queue0filesink0_sinkpad) != GST_PAD_LINK_OK) {
		g_printerr ("Tee0 could not be linked to data.queue0filesink0.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

	
	gst_object_unref (queue0filesink0_sinkpad);



	if(APPSRC)
		{
			pthread_create(&pData->threadPushBuffer, NULL, thrdhndl_push_buffer, (void*)pData);
			OSA_assert(pData->threadPushBuffer != 0);
		}

	screenEnable(1);

	g_print("\ngst starting !!! \n");

	/* Create gstreamer loop */
	pData->frame_cnt = 0;
	pData->frame_Bps = 0;
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

void * record_main_loop(void* arg)
{
	CustomData* pCustomData = (CustomData *)arg;
	g_main_loop_run(pCustomData->loop); //链路停止，释放资源。

	printf("record_main_loop done.\n");

	return NULL;
}

int screenInit()
{
	int res;
	pthread_t thread_0;
	printf("\r\n screenCapture  Build date: %s %s \r\n", __DATE__, __TIME__);

	bufDataPP *pObj = NULL;

	for(int i=0; i <2; i++)
	{
		pObj = &g_useObj[i];
		memset(pObj, 0, sizeof(bufDataPP));

		pObj->buf = (unsigned char *)malloc(1024*1024);
		if(!pObj->buf)
		{
			g_printerr("gst alloc mem failed\n")	;
			return -1;
		}
	}
	g_useObj[0].used = 0;
	g_useObj[1].used = 1;

	res = record_main_init(); //初始化gstreamer.
	if(res == -1)
	{
		g_printerr("gst record init failed\n");
		return -1;
	}
/*
	res = pthread_create(&thread_0, NULL, record_main_loop, (void*)&customData);
	if(res == -1)
		return -1;
*/
	return 0;
}

int screenUninit() //释放资源，关闭链路，还没添加。
{
	return 0;
}

int GetDataBind (int (*getdataFun) (unsigned char * , int ))
{
	if(getdataFun == NULL)
		return (-1);

	screenGetData = getdataFun;
	return 0;
}

int ChangeBitRate(int bitrate)
{
	CustomData* pData = &customData;
	if(pData == NULL)
	{
		printf("CustomData malloc failed.\n");
		return -1;
	}
	if(pData->pipeline == NULL || pData->omxh265enc == NULL)
	{
		printf("CustomData need wait init element end.\n");
		return -1;
	}

	gst_element_set_state(pData->pipeline, GST_STATE_PAUSED);

	printf("%s set: %d \n", __func__, bitrate);
	g_object_set (pData->omxh265enc, "bitrate",  bitrate,  NULL);
	g_object_set (pData->omxh265enc, "iframeinterval", 25, NULL);
	g_object_set (pData->omxh265enc, "control-rate", 2, NULL);
//	g_object_set (pData->omxh265enc, "vbv-size",  1,  NULL);

	pData->frame_cnt = 0;
	pData->frame_Bps = 0;
	gst_element_set_state(pData->pipeline, GST_STATE_PLAYING);

	return 0;
}

unsigned char *getdata(int *len)
{
	bufDataPP *pObj = NULL;
	unsigned char *buf = NULL;

	for(int i=0; i<2; i++)
	{
		pObj = &g_useObj[i];
		if(pObj->used == 1)
		{
			buf = pObj->buf;
			*len = pObj->dataLenth;
			break;
		}
	}
	return buf;
}

int ChangeQP_range(int minQP, int maxQP, int minQI, int maxQI, int minQB, int maxQB)
{
	CustomData* pData = &customData;
	if(pData == NULL)
	{
		printf("CustomData malloc failed.\n");
		return -1;
	}
	if(pData->pipeline == NULL || pData->omxh265enc == NULL)
	{
		printf("CustomData need wait init element end.\n");
		return -1;
	}

	gst_element_set_state(pData->pipeline, GST_STATE_PAUSED);
	gst_element_set_state(pData->pipeline, GST_STATE_NULL);

	char str[100];
	sprintf(str, "%d,%d:%d,%d:%d,%d", minQP, maxQP, minQI, maxQI, -1, -1);
	printf("%s set: %s \n", __func__, str);
	g_object_set (pData->omxh265enc, "qp-range",  str,  NULL);

	pData->frame_cnt = 0;
	pData->frame_Bps = 0;
	gst_element_set_state(pData->pipeline, GST_STATE_PLAYING);

	return 0;
}

/******************************/
#define MAX_QP		(51)
#define MIN_QP_2M	(41)
#define MIN_QP_4M	(38)
#define MIN_QP_8M	(34)

#define MAX_I		(51)
#define MIN_I_2M	(41)
#define MIN_I_4M	(38)
#define MIN_I_8M	(34)

#define GST_ENCBITRATE_2M	(1400000)
#define GST_ENCBITRATE_4M	(2800000)
#define GST_ENCBITRATE_8M	(5600000)

#define SAVEDIR "/home/ubuntu/calib"

static int g_gst_recrod=true;
static FILE *g_gst_wrfp=NULL;
static int g_gst_wrPkt=0;
//static int g_gst_maxPkt=0, g_gst_minPkt=0;
extern int sync422_ontime_video(unsigned char *buf, int len);
int gst_getBufferMap(unsigned char *buf, int len)
{
	if(g_gst_recrod)
	{
		if(g_gst_wrfp == NULL)
		{
			char filename[64];
			struct tm tm_set;
			struct timeval tv_set;
			struct timezone tz_set;
			time_t tt_set=0;

			gettimeofday(&tv_set, &tz_set);
			tt_set = tv_set.tv_sec;
			memcpy(&tm_set, localtime(&tt_set), sizeof(struct tm));
			sprintf(filename, "%s/local_%04d%02d%02d-%02d%02d%02d.h265", 
				SAVEDIR, 
				tm_set.tm_year+1900, tm_set.tm_mon+1, tm_set.tm_mday,
				tm_set.tm_hour, tm_set.tm_min,tm_set.tm_sec);
			g_gst_wrfp = fopen(filename,"wb");
			g_gst_wrPkt = 0;
			printf(" open local file %s\n", filename);
		}

		if(g_gst_wrfp != NULL)
		{
			fwrite(buf, len, 1, g_gst_wrfp);
			g_gst_wrPkt++;
			if((g_gst_wrPkt % 50) == 0)
			{
				fflush(g_gst_wrfp);
			}
		}
	}
	else
	{
		if(g_gst_wrfp != NULL)
		{
			fclose(g_gst_wrfp);
			g_gst_wrfp = NULL;
			printf(" close local file\n");
		}
	}

	CustomData* pData = &customData;
	pData->frame_cnt++;
	pData->frame_Bps += len;
	if(pData->frame_cnt == pData->framerate)
	{
		//printf(" %d: current fps %d Bps %d.\n", OSA_getCurTimeInMsec(), pData->framerate, pData->frame_Bps);
		pData->frame_cnt = 0;
		pData->frame_Bps = 0;
	}

	/*if(g_gst_maxPkt < len)
	{
		g_gst_maxPkt = len;
		printf(" gst frame packet len max = %d\n", g_gst_maxPkt);
	}
	if(g_gst_minPkt > len || g_gst_minPkt == 0)
	{
		g_gst_minPkt = len;
		printf(" gst frame pacekt len min = %d\n", g_gst_minPkt);
	}*/
	
	return 0;//sync422_ontime_video(buf, len);
}

int gst_videnc_create(void)
{
	screenInit();
	GetDataBind(gst_getBufferMap);
	sleep(1);
	OSA_printf(" %d: %s done.\n", OSA_getCurTimeInMsec(), __func__);
	return 0;
}

int gst_videnc_bitrate(int mode)
{
	int irtn=-1;
	if(mode == 0)
	{
		irtn = ChangeBitRate(GST_ENCBITRATE_2M);
		irtn += ChangeQP_range(MIN_QP_2M, MAX_QP, MIN_I_2M, MAX_I, -1, -1);
	}
	else if(mode == 1)
	{
		irtn = ChangeBitRate(GST_ENCBITRATE_4M);
		//irtn += ChangeQP_range(MIN_QP_4M, MAX_QP, MIN_I_4M, MAX_I, -1, -1);
	}
	else if(mode == 2)
	{
		//irtn = ChangeBitRate(GST_ENCBITRATE_8M);
		//irtn += ChangeQP_range(MIN_QP_8M, MAX_QP, MIN_I_8M, MAX_I, -1, -1);
	}
	return irtn;
}

int gst_videnc_record(int enable)
{
	// record into local file
	g_gst_recrod = enable;
	OSA_printf(" %d:%s set enable %d\n", OSA_getCurTimeInMsec(), __func__, enable);
	return 0;
}

