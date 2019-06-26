int max_period = 1;
#if 1
//#include <demo_global_def.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/gstclock.h>
#include "unistd.h"
#include "MP4cabinCapture.h"
#include <time.h>
#include"Xlib.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "sys/stat.h"
#include "ReSplic.h"
#include "videorecord.hpp"
#include"Gststreamercontrl.hpp"



#define VIDEODIR  "/home/nvidia/calib/video"
	int CurSteps = 0;
	char MP4_pipeline[16]={};

int data_cnt = 0;
int video_cnt = 0;
date_t record_date_bak;
char filename_bak[128] = {0};
char filename_gyro_bak[128] = {0};




typedef struct _CustomData
{
  	GstElement *pipeline, *source, *videoconvert0, *tee0, *queue0, *fakesink0;
  	GstElement *queue1, *nvvidconv0, *omxh264enc, *h264parse, *tee1, *queue2, *splitmuxsink1, *mp4mux1, *filesink1;
  	GstElement *queue3, *filesink2;
	GstElement *clockoverlay;

	GstBus *bus;
	GMainLoop *loop;
	gboolean playing;

	GstStateChangeReturn ret;

	GstCaps *caps_src_to_convert;
  	GstCaps *caps_enc_to_tee1;
  	GstCaps *caps_nvconv_to_enc;

	GstCaps *caps_src_to_savemp4;

	GstPad *nvvidconv0_srcpad;
	GstPad *videoconvert0_srcpad;

	GstPadTemplate *tee0_src_pad_template;
	GstPad *tee0queue0_srcpad;
	GstPad *tee0queue1_srcpad;

	GstPadTemplate *tee1_src_pad_template;
	GstPad *tee1queue2_srcpad;
	GstPad *tee1queue3_srcpad;

	int height;
	int width;
	int framerate;
	char format[30];

	gchar* filename_format;
	int tempfileNum;
	int filp_method;
	gboolean stopInput;
	int testCount;
	RecordHandle *record;
	unsigned int bitrate;
	CAPTURE_SRC capture_src;
} CustomData;

static char * name_by_timestamp(GstElement* splitmuxsink, guint id, gpointer userdata)
{
	/* Names the encoded files by timestamp. */
	printf("video_cnt=%d\n",get_video_cnt());

	char video_dir[64];
	char date_dir[16];
	struct tm curtm_set;
	struct timeval tv_set;
	struct timezone tz_set;
	time_t tt_set=0;
	date_t record_date;
	char filename_total[128] = {0};
	char *filename_next = (gchar *)malloc(100);
	char filename_gyro_next[128] = {0};
	char filename_gyro_total[128] = {0};
	
	gettimeofday(&tv_set, &tz_set);
	tt_set = tv_set.tv_sec;
	memcpy(&curtm_set, localtime(&tt_set), sizeof(struct tm));
	
	record_date.year=curtm_set.tm_year+1900;
	record_date.mon=curtm_set.tm_mon+1;
	record_date.day=curtm_set.tm_mday;
	record_date.hour=curtm_set.tm_hour;
	record_date.min=curtm_set.tm_min;
	record_date.sec=curtm_set.tm_sec;

	// Create top-level video directory
	strcpy(video_dir, VIDEODIR);
	if (access(video_dir, F_OK))
		mkdir(video_dir, 0775);

	// Create data directory
	sprintf(date_dir, "/%d%02d%02d", record_date.year,record_date.mon, record_date.day);
	strcat(video_dir, date_dir);
	if (access(video_dir, F_OK))
	mkdir(video_dir, 0775);

	// Create next filename
	sprintf(filename_next, "%s/local_%04d%02d%02d-%02d%02d%02d.mp4", 
			video_dir, 
			record_date.year, record_date.mon, record_date.day,
			record_date.hour, record_date.min,record_date.sec);
	sprintf(filename_gyro_next, "%s/local_%04d%02d%02d-%02d%02d%02d.xml", 
			video_dir, 
			record_date.year, record_date.mon, record_date.day,
			record_date.hour, record_date.min,record_date.sec);
	
	if(get_video_cnt() > 0)
	{
		sprintf(filename_total, "%s/record_%04d%02d%02d-%02d%02d%02d_%04d%02d%02d-%02d%02d%02d.mp4", 
					video_dir, 
					record_date_bak.year, record_date_bak.mon, record_date_bak.day,
					record_date_bak.hour, record_date_bak.min,record_date_bak.sec,
					record_date.year, record_date.mon, record_date.day,
					record_date.hour, record_date.min,record_date.sec);

		sprintf(filename_gyro_total, "%s/record_%04d%02d%02d-%02d%02d%02d_%04d%02d%02d-%02d%02d%02d.xml", 
					video_dir, 
					record_date_bak.year, record_date_bak.mon, record_date_bak.day,
					record_date_bak.hour, record_date_bak.min,record_date_bak.sec,
					record_date.year, record_date.mon, record_date.day,
					record_date.hour, record_date.min,record_date.sec);
		
		VideoRecord::getinstance()->mydata.close();
	}

	VideoRecord::getinstance()->mydata.open(filename_gyro_next);
	
	if(get_video_cnt() == 1)
	{
		remove(filename_bak);
		remove(filename_gyro_bak);
		printf("SetMTime 5minutes\n");
		GstreaemerContrl::getinstance()->gstreamer_mp4->SetMTime(120000000000);
	}
	if(get_video_cnt() > 1)
	{
		rename(filename_bak, filename_total);
		rename(filename_gyro_bak, filename_gyro_total);
	}


	
	record_date_bak = record_date;
	memcpy(filename_bak, filename_next, strlen(filename_next));
	memcpy(filename_gyro_bak, filename_gyro_next, strlen(filename_gyro_next));
	printf("filename_bak=%s\n", filename_bak);
		

	printf("%s, %d, data_count=%d, name_by_timestamp start, %d-%d-%d %d:%d:%d\n", __FILE__,__LINE__,get_data_cnt(), record_date.year, record_date.mon, record_date.day,
					record_date.hour, record_date.min,record_date.sec);

	
	set_video_cnt(get_video_cnt() + 1);
	

	static int a=0;
	if(a==max_period)
	{
		CurSteps=100;
	}
	a++;

	printf("%s,%d, filename_next=%s\n",__FILE__,__LINE__, filename_next);
	return filename_next;


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
			//fflush();
			//fclose();
			g_main_loop_quit (loop);
			CurSteps=100;
			break;

		default:
			break;
	}
	return TRUE;
}


static void CustomData_init(CustomData *pData)
{
	pData->queue1 = NULL;
	pData->omxh264enc = NULL;
	pData->nvvidconv0 = NULL;
	pData->splitmuxsink1 = NULL;
	pData->mp4mux1 = NULL;
	pData->filesink1 = NULL;
}


static void * record_main_loop(void* arg)
{
	RecordHandle * recordHandle = (RecordHandle *)arg;
	CustomData* pCustomData = (CustomData *)recordHandle->context;
	g_main_loop_run(pCustomData->loop); //链路停止，释放资源。
	gst_element_set_state(pCustomData->pipeline,GST_STATE_NULL);
	gst_object_unref(pCustomData->pipeline);
	//CabinUninit(recordHandle);
	printf("record_main_loop done.\n");
	return NULL;
}



int CabinPushData(RecordHandle *recordHandle, char *pbuffer , int datasize)
{
	if(recordHandle == NULL)
	{
		//printf("recordHandle == NULL************\n");
		return -1;
	}
	CustomData* pData = (CustomData *)recordHandle->context;

	if(pData==NULL || pData->source == NULL || pData->stopInput)
	{
		//printf("appsrc == NULL************\n");
		return -1;
	}

//	if(pData->testCount >1000)
	{
	//	printf("input size = %d\n", datasize);
	//	pData->testCount=0;
	}
//	pData->testCount++;

	/* if Init success; go on. */

	GstMapInfo info;
	GstBuffer *buffer;
	guint size;
	GstFlowReturn ret;

	buffer = gst_buffer_new_allocate(NULL, datasize, NULL);

	if(gst_buffer_map(buffer, &info, GST_MAP_WRITE))
	{
		memcpy(info.data, pbuffer, datasize);
		gst_buffer_unmap(buffer, &info);
	}

	gst_buffer_ref(buffer);
	//printf("push data %d************\n", datasize);
	ret = gst_app_src_push_buffer(GST_APP_SRC(pData->source), buffer);
	if(ret != GST_FLOW_OK)
	{
		//pData->stopInput = 1;
		//g_printerr("could not push buffer\n");
		//g_main_loop_quit (pData->loop);
		//CabinUninit(recordHandle);
	}
	gst_buffer_unref(buffer);

	return 0;
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

void 	EOS(RecordHandle *recordHandle)
{
	CustomData* pData = (CustomData* )recordHandle->context;
	if(pData && pData->pipeline)
	{
		GstFlowReturn ret;
		g_signal_emit_by_name(pData->source,"end-of-stream",&ret);
		gst_element_send_event(pData->pipeline,gst_event_new_eos());
	}

//	gst_element_send_event(pData->pipeline,gst_event_new_eos());
//gst_bus_post(pData->bus,gst_message_new_eos());

}

void SetTime(unsigned long Tnm,RecordHandle *recordHandle)
{
//	Tnm=38000000000;
	//Tnm = 38000000000;
	CustomData* pData = (CustomData* )recordHandle->context;

	g_object_set (pData->splitmuxsink1, "max-size-time", Tnm, NULL);
}
void Seteos(RecordHandle *recordHandle)
{
	GstFlowReturn ret; 
	CustomData* pData = (CustomData* )recordHandle->context;
	gst_app_src_end_of_stream((GstAppSrc *)pData->source);
	//g_signal_emit_by_name (pData->source, "end-of-stream", &ret); 
}

int gstlinkInit_appsrc_mp4(RecordHandle *recordHandle)
{
	int ret =0;

	recordHandle->context = (CustomData *)malloc(sizeof(CustomData));
	printf("malloc CustomData...\n");
	memset(recordHandle->context, 0, sizeof(CustomData));

	CustomData* pData = (CustomData* )recordHandle->context;
	pData->record = recordHandle;
	if(pData == NULL)
	{
		printf("CustomData malloc failed.\n");
		return -1;
	}

	pData->height = recordHandle->height;
	pData->width = recordHandle->width;
	pData->framerate = recordHandle->framerate;
	pData->bitrate = recordHandle->bitrate;
	pData->filp_method = recordHandle->filp_method;
	pData->capture_src = recordHandle->capture_src;
	strcpy(pData->format, recordHandle->format);
	/* Initialize GStreamer */
	static int bGstInit = 0;
	if(!bGstInit)
		gst_init (NULL, NULL);
	bGstInit = 1;
	CustomData_init(pData);

	//创建空的管道

	sprintf(MP4_pipeline,"MP4_pipeline_%d",recordHandle->index);
	pData->pipeline = gst_pipeline_new (MP4_pipeline);

  	//创建元件
	if(XIMAGESRC ==pData->capture_src )
	{
		pData->source = gst_element_factory_make ("ximagesrc", NULL);
		getDisplayHeight( &(pData->width), &(pData->height));
	}
	else if(APPSRC == pData->capture_src )
	{
		pData->source = gst_element_factory_make ("appsrc", NULL);
		if(!pData->source){
			g_printerr ("Not all pData->source could be created.\n");
					return -1;
		}
	}

	if(XIMAGESRC ==pData->capture_src )
	{
		g_object_set(pData->source, "use-damage", 0, NULL);
	}
	else if(APPSRC == pData->capture_src )
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

	pData->caps_src_to_savemp4 = gst_caps_new_simple("video/x-h264",
							"stream-format", G_TYPE_STRING, "byte-stream",
							"width", G_TYPE_INT, pData->width,
							"height", G_TYPE_INT, pData->height,
							"framerate", GST_TYPE_FRACTION, pData->framerate, 1,
							 NULL);
	pData->videoconvert0 = gst_element_factory_make ("nvvidconv", NULL);
	pData->clockoverlay = gst_element_factory_make ("clockoverlay", "clockoverlay");
	pData->omxh264enc = gst_element_factory_make ("omxh264enc", NULL);
	pData->splitmuxsink1 = gst_element_factory_make("splitmuxsink", NULL);
	pData->h264parse = gst_element_factory_make("h264parse", NULL);
	if (!pData->pipeline || !pData->source || !pData->videoconvert0  )
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
    }
	if (!pData->omxh264enc )
	{
		g_printerr ("omxh264enc could be created.\n");
		return -1;
    }
	if ( !pData->splitmuxsink1)
	{
		g_printerr ("splitmuxsink1 could be created.\n");
		return -1;
    }
	if ( !pData->h264parse)
	{
		g_printerr ("h264parse could be created.\n");
		return -1;
    }


	gst_bin_add_many (GST_BIN(pData->pipeline), pData->source,pData->clockoverlay,pData->videoconvert0, pData->omxh264enc,pData->h264parse, pData->splitmuxsink1,  NULL);
	if(!gst_element_link_many(pData->source,pData->clockoverlay,pData->videoconvert0,pData->omxh264enc,pData->h264parse, pData->splitmuxsink1,NULL))
	{
		g_printerr ("Elements could not be linked:data.source->data0.videoconvert0.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

	g_object_set (pData->clockoverlay, "time-format", "%Y/%m/%d %a %H:%M:%S", NULL); // 2019/06/18  Tue 18:47:35
    g_object_set (pData->omxh264enc, "iframeinterval", pData->framerate, NULL);
    g_object_set (pData->omxh264enc, "insert-sps-pps", 1, NULL);
    g_object_set (pData->splitmuxsink1, "max-size-time", MAX_SIZE_TIME, NULL); //10s
//    g_object_set (pData->splitmuxsink1, "max-size-bytes", 1024000, NULL); 
    g_object_set (pData->splitmuxsink1, "max-file", 100, NULL); //最大100个文件，多则覆盖最初始的
    g_object_set (pData->splitmuxsink1, "location","%2d.mp4", NULL);
    g_signal_connect(pData->splitmuxsink1, "format-location", G_CALLBACK(name_by_timestamp), pData);
	gst_element_sync_state_with_parent (pData->source);
	gst_element_sync_state_with_parent (pData->videoconvert0);
    gst_element_sync_state_with_parent (pData->omxh264enc);
	gst_element_sync_state_with_parent (pData->splitmuxsink1);
	gst_element_sync_state_with_parent (pData->h264parse);

	g_print("\n\ngst starting ...\n\n");

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

	return ret;
}


RecordHandle * CabinInit(int width, int height, int framerate, const char* format, int chId, FLIP_METHOD filp_method)
{
	int res;
	pthread_t thread_0;
	printf("\r\n cabinCapture  Build date: %s %s \r\n", __DATE__, __TIME__);

	RecordHandle * recordHandle = (RecordHandle *)malloc(sizeof(RecordHandle));
	printf("malloc RecordHandle...\n");
	recordHandle->width = width;
	recordHandle->height = height;
	recordHandle->CHID = chId;
	recordHandle->filp_method = filp_method;
	recordHandle->framerate = framerate;
	strcpy(recordHandle->format, format);
	recordHandle->bEnable = FALSE;

	recordHandle->capture_src =APPSRC;

	//res = record_main_init(recordHandle); //初始化gstreamer.
	res=gstlinkInit_appsrc_mp4(recordHandle);
	if(res == -1)
	{
		g_printerr("gst record init failed\n")	;
		return NULL;
	}else
		g_printerr("gst record init scuccess\n")	;

	res = pthread_create(&thread_0, NULL, record_main_loop, (void*)recordHandle);
	if(res == -1)
		return NULL;
	printf("CabinInit return = %p\n",recordHandle);
	return recordHandle;
}



int CabinUninit(RecordHandle *recordHandle) //释放资源，关闭链路，还没添加。
{
	if(recordHandle != NULL)
	{
		if(recordHandle->context != NULL)
		{
			CustomData* pCustomData = (CustomData *)recordHandle->context;
			free(pCustomData);
			printf("free CustomData...\n");
			recordHandle->context = NULL;
		}
		free(recordHandle);
		printf("free recordHandle...\n");
		recordHandle = NULL;
	}
	return 0;
}


void set_video_cnt(int cnt)
{
	video_cnt = cnt;
}

int get_video_cnt()
{
	return video_cnt;
}
void set_data_cnt(int cnt)
{
	data_cnt = cnt;
}

int get_data_cnt()
{
	return data_cnt;
}

#endif

