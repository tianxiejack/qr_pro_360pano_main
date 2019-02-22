#include"Gststream.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include "screenCapture.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/gstclock.h>
#include <X11/Xlib.h>
#include <sys/time.h>
#include "osa.h"
#include "osa_image_queue.h"
#include "osa_sem.h"
#include "config.h"
typedef struct _CustomData
{
  	GstElement *pipeline, *source, *videoconvert0, *tee0, *queue0, *fakesink0;
  	GstElement *queue1, *nvvidconv0, *omxh265enc, *fakesink1, *rtph265pay,*clockoverlay, *udpsink,*filesink1;
  	GstElement *queue3, *filesink2,*qtmux ,*h264parse,*avimux;

	GstBus *bus;
	GMainLoop *loop;
	gboolean playing;
	gboolean source_screen;

	GstStateChangeReturn ret;
	GstCaps *	caps_enc_to_rtp_265;
	GstCaps *caps_src_to_convert;
  	GstCaps *caps_enc_to_rtp;
  	GstCaps *caps_nvconv_to_enc;

	GstPad *nvvidconv0_srcpad;
	GstPad *videoconvert0_srcpad;
	GstPadTemplate *tee0_src_pad_template;
	GstPad *tee0queue0_srcpad;
	GstPad *tee0queue1_srcpad;

	int height;
	int width;
	int framerate;
	int bitrate;
	char format[30];
	char ip_addr[30];

	gchar* filename_format;
	int tempfileNum;
	int filp_method;
	int capture_src;

	int testCount;
	int frame_cnt;
	int frame_Bps;

	unsigned int  port;
	void *hdlSink;
	RecordHandle *record;
	GstClockTime buffer_timestamp;

	OSA_BufHndl pushBuffQueue;
	pthread_t threadPushBuffer;
	OSA_SemHndl pushSem;
	bool bPush;

	OSA_BufHndl outBuffQueue;
	pthread_t threadOutBuffer;
	pthread_t threadTimer;
	OSA_SemHndl *outSem;
	void *notify;
	bool bOut;
	int outDgFlag;
	unsigned long counts[8];
} GstCustomData;
Gstreamer::Gstreamer()
{
	
}
Gstreamer::~Gstreamer()
{
	

}


void * Gstreamer::thrdhndl_push_buffer(void* arg)
{
	GstCustomData* pData = (GstCustomData *)arg;
	pData->bPush = true;
	while(pData->bPush){
		
		OSA_semWait(&pData->pushSem, OSA_TIMEOUT_FOREVER);
		
		if(!pData->bPush)
			break;
		OSA_BufInfo* bufInfo = image_queue_getFull(&pData->pushBuffQueue);
		if(bufInfo != NULL){
			Privatedata privatedata;
			privatedata.gyrox=bufInfo->framegyroroll*1.0/ANGLESCALE;
			privatedata.gyroy=bufInfo->framegyropitch*1.0/ANGLESCALE;
			privatedata.gyroz=bufInfo->framegyroyaw*1.0/ANGLESCALE;
			if( pData->record->sy_cb!=NULL)
		    		pData->record->sy_cb(&privatedata);
			GstBuffer *buffer = (GstBuffer *)bufInfo->physAddr;
			OSA_assert(buffer != NULL);
			GST_BUFFER_PTS(buffer) = pData->buffer_timestamp;
			GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale_int(1, GST_SECOND, pData->framerate);
			//printf("the frame rate=%d\n",pData->framerate);
			pData->buffer_timestamp+=GST_BUFFER_DURATION(buffer);
			gst_buffer_ref(buffer);
			GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(pData->source), buffer);
			if( ret != GST_FLOW_OK )
			{
				g_print("\n %s %d: gst_app_src_push_buffer error!\n", __func__, __LINE__);
			}
			image_queue_putEmpty(&pData->pushBuffQueue, bufInfo);
				

			//printf("^^^^^^^push_buffer^^^^pData->source=%p^^^^^^\n",pData->source);
			//printf("%s  ok \n",__func__);
		}
	    if(image_queue_fullCount(&pData->pushBuffQueue)>0){
	    	bufInfo = image_queue_getFull(&pData->pushBuffQueue);
	    	image_queue_putEmpty(&pData->pushBuffQueue, bufInfo);
	    	g_print("%s %d: full !! \n", __func__, __LINE__);
	    }
	}
	return NULL;
}
void * Gstreamer::thrdhndl_timer(void* arg)
{
	GstCustomData* pData = (GstCustomData *)arg;
	struct timeval timerStart, timerLast;
	unsigned long ulCount = 0;
	gettimeofday(&timerStart, NULL);
	timerLast = timerStart;
	while(pData->bOut){
		OSA_semSignal(pData->outSem);
		pData->counts[0] ++;
		ulCount ++;
		bool bSync = false;
		struct timeval timerStop, timerElapsed;
		gettimeofday(&timerStop, NULL);
		timersub(&timerStop, &timerStart, &timerElapsed);
		double ms = (timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0);
		double nms = 1000.0/pData->framerate*ulCount;
		double wms = nms-ms-0.5;
		/*if(pData->record->index == 0){
			timersub(&timerStop, &timerLast, &timerElapsed);
			g_print("%s %d: %ld.%ld\n", __func__, __LINE__, timerElapsed.tv_sec, timerElapsed.tv_usec);
			timerLast = timerStop;
		}*/
		if(wms<0.000001 || wms>1000.0/pData->framerate+5.0){
			bSync = true;
			g_print("%s %d: tm sync!! \n", __func__, __LINE__);
		}
		if(bSync){
			gettimeofday(&timerStart, NULL);
			ulCount = 0;
		}else{
			struct timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = wms*1000.0;
			select( 0, NULL, NULL, NULL, &timeout );
		}
	}
	return NULL;
}

void * Gstreamer::thrdhndl_out_buffer(void* arg)
{
	GstCustomData* pData = (GstCustomData *)arg;
	pData->bOut = true;
	while(pData->bOut){
		OSA_semWait(pData->outSem, OSA_TIMEOUT_FOREVER);
		if(!pData->bOut)
			break;
		pData->counts[1] ++;
		pData->outDgFlag = 1;
		OSA_BufInfo* bufInfo = image_queue_getFull(&pData->outBuffQueue);
		GstBuffer *buffer;
	    GstMapInfo map;
		if(bufInfo != NULL){
			buffer = (GstBuffer *)bufInfo->physAddr;
			OSA_assert(buffer != NULL);
		    if(gst_buffer_map(buffer, &map, GST_MAP_READ))
		    {
		    	pData->outDgFlag = 2;
		    	if( pData->record->sd_cb!=NULL)
		    		pData->record->sd_cb(pData->record->index, map.data, map.size);
		    	pData->outDgFlag = 3;
		        gst_buffer_unmap(buffer, &map);
		        pData->outDgFlag = 4;
		    }
		    gst_buffer_unref(buffer);
			bufInfo->physAddr = NULL;
			image_queue_putEmpty(&pData->outBuffQueue, bufInfo);
			pData->outDgFlag = 5;
		}
		int cnt;
		if((cnt = image_queue_fullCount(&pData->outBuffQueue))>1){
			pData->outDgFlag = 6;
			g_print("%s %d: queue full count = %d\n", __func__, __LINE__, cnt);
			OSA_semSignal(pData->outSem);
			pData->outDgFlag = 7;
		}
		pData->outDgFlag = 0;
	}
	return NULL;
}

void Gstreamer::getDisplayHeight(int *width, int *height)
{
	char *display_name = getenv("DISPLAY");
	Display* display = XOpenDisplay(display_name);
	int screen_num = DefaultScreen(display);
	*width = DisplayWidth(display, screen_num);
	*height = DisplayHeight(display, screen_num);
	printf("\nscreen w=%d , h=%d\n", *width, *height);
}
GstPadProbeReturn Gstreamer::enc_buffer(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
    GstCustomData *pData = (GstCustomData *)user_data;
    GstBuffer *buffer;

    buffer = GST_PAD_PROBE_INFO_BUFFER(info);
    if(buffer == NULL)
        return GST_PAD_PROBE_OK;

    pData->counts[2] ++;

    OSA_BufInfo* bufInfo = image_queue_getEmpty(&pData->outBuffQueue);
    if(bufInfo == NULL){
    	g_print("%s %d: WARN encoder%d overflow !(flag = %d) %ld %ld %ld\n",
    			__func__, __LINE__, pData->record->index, pData->outDgFlag,
    			pData->counts[0],pData->counts[1],pData->counts[2]);
    	return GST_PAD_PROBE_OK;
    }
    bufInfo->physAddr = buffer;
	struct timeval tmCur;
	gettimeofday(&tmCur, NULL);
    bufInfo->timestamp = (uint64_t)tmCur.tv_sec*1000000000ul + (uint64_t)tmCur.tv_usec*1000ul;
    bufInfo->timestampCap = GST_BUFFER_PTS(buffer);
    gst_buffer_ref(buffer);
    image_queue_putFull(&pData->outBuffQueue, bufInfo);
	if(pData->threadOutBuffer == 0){
		pthread_create(&pData->threadOutBuffer, NULL, thrdhndl_out_buffer, (void*)pData);
		OSA_assert(pData->threadOutBuffer != 0);
	}
	if(pData->threadTimer == 0 && pData->notify == NULL){
		pthread_create(&pData->threadTimer, NULL, thrdhndl_timer, (void*)pData);
		OSA_assert(pData->threadTimer != 0);
	}

    return GST_PAD_PROBE_OK;  //just into print one time
}


int Gstreamer::gstlinkInit_convert_enc_rtp(RecordHandle *recordHandle)
{
	int ret =0;

	GstCustomData* pData = (GstCustomData* )recordHandle->context;
	if(pData == NULL)
	{
		printf("CustomData malloc failed.\n");
		return -1;
	}
	//创建空的管道
	char test_pipeline[16]={};
	sprintf(test_pipeline,"test_pipeline_%d",recordHandle->index);
	pData->pipeline = gst_pipeline_new (test_pipeline);

  	//创建元件
	if(XIMAGESRC ==pData->capture_src )
	{
		pData->source = gst_element_factory_make ("ximagesrc", NULL);
		getDisplayHeight( &(pData->width), &(pData->height));
	}
	else if(APPSRC == pData->capture_src )
	{
		pData->source = gst_element_factory_make ("appsrc", NULL);
	}

	if(XIMAGESRC ==pData->capture_src )
	{
		g_object_set(pData->source, "use-damage", 0, NULL);
	}
	else if(APPSRC == pData->capture_src )
	{
		//printf("^^^^^^^^^^^pData->source=%p^^^^^^\n",pData->source);
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

	pData->caps_enc_to_rtp = gst_caps_new_simple("video/x-h265",
							"stream-format", G_TYPE_STRING, "byte-stream",
							"width", G_TYPE_INT, pData->width,
							"height", G_TYPE_INT, pData->height,
							"framerate", GST_TYPE_FRACTION, pData->framerate, 1,
							 NULL);

	pData->queue1 = gst_element_factory_make("queue", NULL);
	pData->videoconvert0 = gst_element_factory_make ("nvvidconv", NULL);
	pData->omxh265enc = gst_element_factory_make ("omxh265enc", NULL);
	pData->rtph265pay = gst_element_factory_make("rtph265pay", NULL);
	pData->udpsink = gst_element_factory_make("udpsink", NULL);

	if (!pData->pipeline || !pData->source || !pData->videoconvert0||!pData->queue1)
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
    }

	gst_bin_add_many (GST_BIN(pData->pipeline), pData->source,pData->queue1,pData->videoconvert0, pData->omxh265enc,pData->rtph265pay, pData->udpsink,  NULL);
	if(!gst_element_link_many(pData->source,pData->queue1,
			pData->videoconvert0, pData->omxh265enc, NULL))
	{
		g_printerr ("Elements could not be linked:data.source->data0.videoconvert0.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

    if(!gst_element_link_filtered(pData->omxh265enc, pData->rtph265pay, pData->caps_enc_to_rtp))
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

    g_object_set (pData->omxh265enc, "iframeinterval", pData->framerate, NULL);
    g_object_set (pData->omxh265enc, "bitrate", pData->bitrate, NULL);
    g_object_set (pData->rtph265pay, "config-interval", 1, NULL);

	g_object_set (pData->udpsink, "host","192.168.1.26", NULL);
	g_object_set (pData->udpsink, "port", 16000, NULL);
//	g_object_set (pData->udpsink, "host", pData->ip_addr, NULL);
//	g_object_set (pData->udpsink, "port", pData->port, NULL);

  //  GstPad *h265enc_pad = gst_element_get_static_pad(pData->omxh265enc,"src");
    //gst_pad_add_probe (h265enc_pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback) enc_buffer, pData, NULL);
   // gst_object_unref(h265enc_pad);

	gst_element_sync_state_with_parent (pData->source);
	gst_element_sync_state_with_parent (pData->queue1);
	gst_element_sync_state_with_parent (pData->videoconvert0);
      gst_element_sync_state_with_parent (pData->omxh265enc);
      gst_element_sync_state_with_parent (pData->rtph265pay);
	gst_element_sync_state_with_parent (pData->udpsink);

	g_print("\n\n%s gst starting ... %s: %d\n\n", __func__, pData->ip_addr, pData->port);

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
int Gstreamer::gstlinkInit_appsrc_enc_rtp(RecordHandle *recordHandle)
{
	int ret =0;

	GstCustomData* pData = (GstCustomData* )recordHandle->context;
	if(pData == NULL)
	{
		printf("CustomData malloc failed.\n");
		return -1;
	}
	//创建空的管道
	char test_pipeline[16]={};
	sprintf(test_pipeline,"test_pipeline_%d",recordHandle->index);
	pData->pipeline = gst_pipeline_new (test_pipeline);

  	//创建元件
	g_assert(APPSRC == pData->capture_src );
	{
		pData->source = gst_element_factory_make ("appsrc", NULL);
	}

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

	pData->caps_enc_to_rtp = gst_caps_new_simple("video/x-h265",
							"stream-format", G_TYPE_STRING, "byte-stream",
							"width", G_TYPE_INT, pData->width,
							"height", G_TYPE_INT, pData->height,
							"framerate", GST_TYPE_FRACTION, pData->framerate, 1,
							 NULL);

	pData->omxh265enc = gst_element_factory_make ("omxh265enc", NULL);
	pData->rtph265pay = gst_element_factory_make("rtph265pay", NULL);
	pData->udpsink = gst_element_factory_make("udpsink", NULL);

	if (!pData->pipeline || !pData->source || !pData->omxh265enc || !pData->rtph265pay || !pData->udpsink)
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
    }

	gst_bin_add_many (GST_BIN(pData->pipeline), pData->source, pData->omxh265enc,pData->rtph265pay, pData->udpsink,  NULL);
	if(!gst_element_link_many(pData->source, pData->omxh265enc, NULL))
	{
		g_printerr ("Elements could not be linked:data.source->data0.omxh265enc.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

    if(!gst_element_link_filtered(pData->omxh265enc, pData->rtph265pay, pData->caps_enc_to_rtp))
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

    g_object_set (pData->omxh265enc, "iframeinterval", pData->framerate, NULL);
    g_object_set (pData->omxh265enc, "bitrate", pData->bitrate, NULL);
    g_object_set (pData->rtph265pay, "config-interval", 1, NULL);
	g_object_set (pData->udpsink, "host", pData->ip_addr, NULL);
	g_object_set (pData->udpsink, "port", pData->port, NULL);

    //GstPad *h265enc_pad = gst_element_get_static_pad(pData->omxh265enc,"src");
    //gst_pad_add_probe (h265enc_pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback) enc_buffer, pData, NULL);
    //gst_object_unref(h265enc_pad);

	gst_element_sync_state_with_parent (pData->source);
    gst_element_sync_state_with_parent (pData->omxh265enc);
    gst_element_sync_state_with_parent (pData->rtph265pay);
	gst_element_sync_state_with_parent (pData->udpsink);

	g_print("\n\n%s gst starting ... %s: %d    iframeinterval=%d\n\n", __func__, pData->ip_addr, pData->port,pData->framerate);

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

GstPadProbeReturn Gstreamer::filesink1_buffer (GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	GstCustomData *pData = (GstCustomData *)user_data;
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

			//malloc 2 p1 p2
			if( pData->record->sd_cb!=NULL)
		    		pData->record->sd_cb(pData->record->index, map.data, map.size);
		

		gst_buffer_unmap(buffer, &map);
	}

	GST_PAD_PROBE_INFO_DATA(info) = buffer;

	return GST_PAD_PROBE_OK;  //just into print one time
}

#define UDPSINK  (0)
#define GST_ENCBITRATE	(5600000)

#define RECORDAVI (1)
GstPadProbeReturn Gstreamer::enc_tick_cb(GstPad * pad, GstPadProbeInfo * info, gpointer user_data)
{
	GstCustomData *pData = (GstCustomData *)user_data;
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
if(RECORDAVI)
{
	//pData->h264parse = gst_element_factory_make ("h264parse", NULL);
	//pData->qtmux = gst_element_factory_make ("qtmux", NULL);
	//pData->avimux = gst_element_factory_make("matroskamux", NULL);
	
	pData->avimux = gst_element_factory_make("avimux", NULL);
}else
{
	pData->h264parse = gst_element_factory_make ("h264parse", NULL);
	pData->qtmux = gst_element_factory_make ("qtmux", NULL);
}
	
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

if(RECORDAVI)
{
	gst_object_ref(pData->avimux);
}
else
{
	gst_object_ref(pData->h264parse);

	gst_object_ref(pData->qtmux);
}
	
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
		{
		if(RECORDAVI)
			gst_bin_add_many (GST_BIN(pData->pipeline), pData->queue1, pData->nvvidconv0, pData->omxh265enc,pData->avimux, pData->filesink1, NULL);
		else
			gst_bin_add_many (GST_BIN(pData->pipeline), pData->queue1, pData->nvvidconv0, pData->omxh265enc,pData->h264parse,pData->qtmux, pData->filesink1, NULL);
		}
	//gst_bin_add_many (GST_BIN(pData->pipeline), pData->queue1, pData->nvvidconv0, pData->omxh265enc,pData->h264parse,pData->qtmux, pData->filesink1, NULL);
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
		if(RECORDAVI)
			{
			if(!gst_element_link_filtered(pData->omxh265enc,pData->avimux, pData->caps_enc_to_rtp_265))
				{
					g_printerr ("Elements could not be linked.\n");
					gst_object_unref (pData->pipeline);
					return (GstPadProbeReturn)-1;
				}
			}
		else
			{

			if(!gst_element_link_filtered(pData->omxh265enc,pData->h264parse, pData->caps_enc_to_rtp_265))
				{
					g_printerr ("Elements could not be linked.\n");
					gst_object_unref (pData->pipeline);
					return (GstPadProbeReturn)-1;
				}

			}
		if(RECORDAVI)
			{
			if(!gst_element_link_many(pData->avimux, pData->filesink1, NULL))
				{
					g_printerr ("Elements could not be linked:pData->qtmux--->pData->filesink1.\n");
					gst_object_unref (pData->pipeline);
					return (GstPadProbeReturn)-1;
				}
			}
			else
				{
				if(!gst_element_link_many(pData->h264parse, pData->qtmux, NULL))
					{
						g_printerr ("Elements could not be linked:pData->qtmux--->pData->filesink1.\n");
						gst_object_unref (pData->pipeline);
						return (GstPadProbeReturn)-1;
					}
				if(!gst_element_link_many(pData->qtmux, pData->filesink1, NULL))
					{
						g_printerr ("Elements could not be linked:pData->qtmux--->pData->filesink1.\n");
						gst_object_unref (pData->pipeline);
						return (GstPadProbeReturn)-1;
					}
				}
	
		}

	

	g_object_set (pData->omxh265enc, "iframeinterval", 30, NULL);
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
		{
			if(RECORDAVI)
			gst_element_sync_state_with_parent (pData->avimux);
			else
				{
					gst_element_sync_state_with_parent (pData->h264parse);
					gst_element_sync_state_with_parent (pData->qtmux);
				}
		//	gst_element_sync_state_with_parent (pData->qtmux);
			gst_element_sync_state_with_parent (pData->filesink1);
		}

	g_print ("\nEnable record done\n");

	return GST_PAD_PROBE_REMOVE;
}
int Gstreamer::record_main_init(RecordHandle *recordHandle)
{
	GstCustomData* pData = (GstCustomData* )recordHandle->context;
	if(pData == NULL)
	{
		printf("CustomData malloc failed.\n");
		return -1;
	}

	getDisplayHeight( &(pData->width),&(pData->height));

	//pData->framerate = 60;

//	memcpy(pData->format,formatgst,sizeof(formatgst));

//	 pData->Screenmapx.create(Size(pData->width,pData->height), CV_32FC1 );
//     pData->Screenmapy.create(Size(pData->width,pData->height), CV_32FC1 );
/*

	for (int i = 0; i < pData->height;i++)
	{
		for (int j = 0; j < pData->width;j++)
		{

			pData->Screenmapx.at<float>(i,j)= static_cast<float>(j);
			pData->Screenmapy.at<float>(i,j)= static_cast<float>(pData->height-i);
		
		}
	}
*/

/*
	GstMapInfo *info = new GstMapInfo;
	
	pData->buffer = gst_buffer_new_allocate(NULL,pData->width*pData->height*3, NULL);
	int iret = gst_buffer_map(pData->buffer, info, GST_MAP_WRITE);
	pData->bufferdata=info->data;
	CustomData_init(pData);
*/
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
	pData->pipeline = gst_pipeline_new ("test-pipeline");

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

	//screenEnable(1);
	gst_pad_add_probe (pData->tee0queue1_srcpad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, enc_tick_cb, pData, (GDestroyNotify)NULL);

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


int Gstreamer::gstlinkInit_convert_enc_fakesink(RecordHandle *recordHandle)
{
	int ret =0;

	GstCustomData* pData = (GstCustomData* )recordHandle->context;
	if(pData == NULL)
	{
		printf("CustomData malloc failed.\n");
		return -1;
	}

	//创建空的管道
	char test_pipeline[16]={};
	sprintf(test_pipeline,"J_pipeline_%d",recordHandle->index);
	pData->pipeline = gst_pipeline_new (test_pipeline);

  	//创建元件
	if(XIMAGESRC ==pData->capture_src )
	{
		pData->source = gst_element_factory_make ("ximagesrc", NULL);
		getDisplayHeight( &(pData->width), &(pData->height));
	}
	else if(APPSRC == pData->capture_src )
	{
		pData->source = gst_element_factory_make ("appsrc", NULL);
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
		/*printf("caps_src_to_convert = %s\n", gst_caps_to_string(pData->caps_src_to_convert));
		char * capsStr = g_strdup_printf("video/x-raw(memory:NVMM),width=(int)%d,height=(int)%d,alignment=(string)au,format=(string)I420,framerate=(fraction)%d/1,pixel-aspect-ratio=(fraction)1/1", pData->width, pData->height, pData->framerate*2);
		pData->caps_nvconv_to_enc = gst_caps_from_string(capsStr);
		g_free(capsStr);*/

		g_object_set(G_OBJECT(pData->source), "caps", pData->caps_src_to_convert, NULL);
		g_object_set(G_OBJECT(pData->source),
					"stream-type", 0,
					"is-live", TRUE,
					//"block", TRUE,
					"do-timestamp", TRUE,
					"format", GST_FORMAT_TIME, NULL);
	}

	pData->caps_enc_to_rtp = gst_caps_new_simple("video/x-h264",
							"stream-format", G_TYPE_STRING, "byte-stream",
							"width", G_TYPE_INT, pData->width,
							"height", G_TYPE_INT, pData->height,
							"framerate", GST_TYPE_FRACTION, pData->framerate, 1,
							 NULL);

	pData->videoconvert0 = gst_element_factory_make ("nvvidconv", NULL);
	pData->omxh265enc = gst_element_factory_make ("omxh264enc", NULL);
	pData->fakesink0  = gst_element_factory_make("fakesink", NULL);

	if (!pData->pipeline || !pData->source || !pData->videoconvert0  )
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
    }

	gst_bin_add_many (GST_BIN(pData->pipeline), pData->source,	pData->videoconvert0, pData->omxh265enc, pData->fakesink0,  NULL);
	if(!gst_element_link_many(pData->source,
			pData->videoconvert0, pData->omxh265enc, NULL))
	{
		g_printerr ("Elements could not be linked:data.source->data0.videoconvert0.\n");
		gst_object_unref (pData->pipeline);
		return -1;
	}

    if(!gst_element_link_filtered(pData->omxh265enc, pData->fakesink0, pData->caps_enc_to_rtp))
	{
		g_printerr ("Elements could not be linked.\n");
		gst_object_unref (pData->pipeline);
		return (GstPadProbeReturn)-1;
	}

    g_object_set (pData->omxh265enc, "iframeinterval", pData->framerate, NULL);
    g_object_set (pData->omxh265enc, "bitrate", pData->bitrate, NULL);

    GstPad *h265enc_pad = gst_element_get_static_pad(pData->omxh265enc,"src");
    gst_pad_add_probe (h265enc_pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback) enc_buffer, pData, NULL);
    gst_object_unref(h265enc_pad);

    gst_element_sync_state_with_parent (pData->source);
    gst_element_sync_state_with_parent (pData->videoconvert0);
    gst_element_sync_state_with_parent (pData->omxh265enc);
    gst_element_sync_state_with_parent (pData->fakesink0);

	g_print("\n\n%s gst starting ... \n\n", __func__);

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

gboolean Gstreamer::bus_call(GstBus *bus, GstMessage *message, gpointer user_data)
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

void * Gstreamer::rtp_main_loop(void* arg)
{
	RecordHandle * recordHandle = (RecordHandle *)arg;
	GstCustomData* pCustomData = (GstCustomData *)recordHandle->context;
	g_main_loop_run(pCustomData->loop);  //链路停止，释放资源�?
	printf("rtp_main_loop done.\n");
	return NULL;
}
RecordHandle * Gstreamer::gstpipeadd(GstCapture_data gstCapture_data)
{
	int res;
	static int createNum = 0;
	pthread_t thread_0;
	printf("\r\n gst_Capture--------------Build date: %s %s \r\n", __DATE__, __TIME__);
	RecordHandle * recordHandle = (RecordHandle *)malloc(sizeof(RecordHandle));
	memset(recordHandle, 0, sizeof(RecordHandle));
	recordHandle->index = createNum;
	recordHandle->width = gstCapture_data.width;
	recordHandle->height = gstCapture_data.height;
	recordHandle->ip_port = gstCapture_data.ip_port;
	recordHandle->filp_method = gstCapture_data.filp_method;
	recordHandle->capture_src = gstCapture_data.capture_src;
	recordHandle->framerate = gstCapture_data.framerate;
	recordHandle->bitrate = gstCapture_data.bitrate;
	recordHandle->sd_cb=gstCapture_data.sd_cb;
	recordHandle->sy_cb=gstCapture_data.sy_cb;
	for(int i=0;i<ENC_QP_PARAMS_COUNT;i++)
		recordHandle->Q_PIB[i]=gstCapture_data.Q_PIB[i];
	OSA_assert(gstCapture_data.format!=NULL);
	strcpy(recordHandle->format, gstCapture_data.format);
	if(gstCapture_data.ip_addr!=NULL)
		strcpy(recordHandle->ip_addr, gstCapture_data.ip_addr);
	recordHandle->bEnable = FALSE;

	recordHandle->context = (GstCustomData *)malloc(sizeof(GstCustomData));
	GstCustomData* pData = (GstCustomData* )recordHandle->context;
	memset(pData, 0, sizeof(GstCustomData));
	pData->record = recordHandle;
	pData->height = recordHandle->height;
	pData->width = recordHandle->width;
	pData->framerate = recordHandle->framerate;
	pData->bitrate = recordHandle->bitrate;
	pData->filp_method = recordHandle->filp_method;
	pData->capture_src = recordHandle->capture_src;
	strcpy(pData->format, recordHandle->format);
	strcpy(pData->ip_addr, recordHandle->ip_addr);
	pData->notify = gstCapture_data.notify;
	pData->port = recordHandle->ip_port;
	pData->queue1 = NULL;
	pData->omxh265enc = NULL;
	pData->nvvidconv0 = NULL;
	pData->fakesink1 = NULL;
	/* Initialize GStreamer */
	/*
	static bool bGstInit = false;
	if(!bGstInit)
		gst_init (NULL, NULL);
	bGstInit = true;
	*/
	if(APPSRC == gstCapture_data.capture_src){
		res = image_queue_create(&pData->pushBuffQueue, 3, pData->width*pData->height*3, memtype_null);
		for(int i=0; i<3; i++){
			GstMapInfo *info = new GstMapInfo;
			GstBuffer *buffer;
			buffer = gst_buffer_new_allocate(NULL, pData->pushBuffQueue.bufInfo[i].size, NULL);
			int iret = gst_buffer_map(buffer, info, GST_MAP_WRITE);
			OSA_assert(iret != 0);
			pData->pushBuffQueue.bufInfo[i].virtAddr = info->data;
			pData->pushBuffQueue.bufInfo[i].physAddr = buffer;
			pData->pushBuffQueue.bufInfo[i].resource = (cudaGraphicsResource*)info;
		}
		res = OSA_semCreate(&pData->pushSem, 1, 0);
		pthread_create(&pData->threadPushBuffer, NULL, thrdhndl_push_buffer, (void*)pData);
		OSA_assert(pData->threadPushBuffer != 0);
		recordHandle->pushQueue = &pData->pushBuffQueue;
		recordHandle->pushSem = &pData->pushSem;

		printf("#############the APPSRC=%d##pData->capture_src=%d############\n",gstCapture_data.capture_src,pData->capture_src);
	}

	res = image_queue_create(&pData->outBuffQueue, 3, pData->width*pData->height*3, memtype_null);
	if(pData->notify == NULL){
		pData->outSem = new OSA_SemHndl;
		res = OSA_semCreate(pData->outSem, 1, 0);
	}else{
		pData->outSem = (OSA_SemHndl *)pData->notify;
	}


	//res = rtp_main_init(recordHandle);  //初始化gstreamer.
	/*
	if(XIMAGESRC == gstCapture_data.capture_src)
		res = gstlinkInit_convert_enc_fakesink(recordHandle);
		
	if(APPSRC == gstCapture_data.capture_src)
	*/
	//res = gstlinkInit_convert_enc_rtp(recordHandle);
	//res = gstlinkInit_convert_enc_fakesink(recordHandle);
	//if(createNum==0)
	res=record_main_init(recordHandle);
	//else
	//res = gstlinkInit_convert_enc_fakesink(recordHandle);
	
	printf("recordHandle=%p\n",recordHandle);
	if(res == -1)
	{
		g_printerr("gst record init failed\n");
		return NULL;
	}
/*
	res = pthread_create(&thread_0, NULL, rtp_main_loop, (void*)recordHandle);
	if(res == -1)
		return NULL;
*/
	createNum ++;

	return recordHandle;


}
void Gstreamer::create()
{
	gst_videnc_create();
	gst_videnc_bitrate(2);

}
int Gstreamer::gstCapturePushData(RecordHandle *recordHandle, char *pbuffer , int datasize)
{
	if(recordHandle == NULL)
	{
		printf("recordHandle=%p\n",recordHandle);
		return -1;
	}

	//printf("%s recordHandle=%p\n",__func__,recordHandle);
	GstCustomData* pData = (GstCustomData *)recordHandle->context;

	if(pData==NULL || pData->source == NULL || APPSRC != pData->capture_src)
	{
		printf("pData=%p pData->source=%p pData->capture_src=%d\n",pData,pData->source,pData->capture_src);
		return -1;
	}

	OSA_BufInfo* bufInfo = image_queue_getEmpty(&pData->pushBuffQueue);
	if(bufInfo != NULL)
	{
		//printf("%s  ok \n",__func__);
		memcpy(bufInfo->virtAddr, pbuffer, datasize);
		image_queue_putFull(&pData->pushBuffQueue, bufInfo);
		OSA_semSignal(&pData->pushSem);
	}

	return 0;
}

int Gstreamer::gstCapturePushDataMux(RecordHandle *recordHandle, char *pbuffer , int datasize,Privatedata *privatedata)
{
	if(recordHandle == NULL)
	{
		printf("recordHandle=%p\n",recordHandle);
		return -1;
	}

	//printf("%s recordHandle=%p\n",__func__,recordHandle);
	GstCustomData* pData = (GstCustomData *)recordHandle->context;

	if(pData==NULL || pData->source == NULL || APPSRC != pData->capture_src)
	{
		printf("pData=%p pData->source=%p pData->capture_src=%d\n",pData,pData->source,pData->capture_src);
		return -1;
	}

	OSA_BufInfo* bufInfo = image_queue_getEmpty(&pData->pushBuffQueue);
	if(bufInfo != NULL)
	{
		//printf("%s  ok \n",__func__);
		memcpy(bufInfo->virtAddr, pbuffer, datasize);
		bufInfo->framegyroroll=privatedata->gyrox*ANGLESCALE;
		bufInfo->framegyropitch=privatedata->gyroy*ANGLESCALE;
		bufInfo->framegyroyaw=privatedata->gyroz*ANGLESCALE;
		image_queue_putFull(&pData->pushBuffQueue, bufInfo);
		OSA_semSignal(&pData->pushSem);
	}

	return 0;
}
void Gstreamer::encode(Mat src)
{
	

}
