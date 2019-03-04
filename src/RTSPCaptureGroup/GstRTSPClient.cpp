/*
 * GstRTSPClient.cpp
 *
 *  Created on: 2018年5月21日
 *      Author: lw
 */


#include "InterfaceProcessImageBehavior.h"
#include "GstRTSPClient.h"

#define ENABLE_OMX264ENC (0)
using namespace std;

class InterfaceProcessNoWayBehavior:public InterfaceProcessImageBehavior
{
	public:
		void processImage_setInfo(int height, int width ){ /*do nothing*/  };
		void processImage(unsigned char * data, unsigned int length){ /*do nothing*/ };
	private:

};

GstRTSPClient::GstRTSPClient() {
	// TODO Auto-generated constructor stub
	 pProcessImageBehavior = new  InterfaceProcessNoWayBehavior();
}

GstRTSPClient::~GstRTSPClient() {
	// TODO Auto-generated destructor stub
	delete pProcessImageBehavior;
}

/* bus messages processing, similar to all gstreamer examples  */
static gboolean bus_call(GstBus *bus, GstMessage *message, gpointer user_data)
{
	GstRTSPClient * obj= (GstRTSPClient *)user_data;
	GMainLoop * loop  = obj->getCustomData()->loop;

	switch(GST_MESSAGE_TYPE(message))
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

static void cb_new_rtspsrc_pad(GstElement *element, GstPad*pad, gpointer  data)
{
	gchar *name;
	GstCaps * p_caps;
	gchar * description;
	GstElement *p_rtph264depay;

	name = gst_pad_get_name(pad);
	//g_print("A new pad %s was created\n", name);

	// here, you would setup a new pad link for the newly created pad
	// sooo, now find that rtph264depay is needed and link them?
	p_caps = gst_pad_get_pad_template_caps (pad);
	description = gst_caps_to_string(p_caps);
	printf("[receive:]template_caps description:%s\n",description);
	g_free(description);

	p_rtph264depay = GST_ELEMENT(data);

	// try to link the pads then ...
	if(!gst_element_link_pads(element, name, p_rtph264depay, "sink"))
	{
	    printf("Failed to link elements 3\n");
	}

	g_free(name);
}

static GstPadProbeReturn decode_buffer(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	GstRTSPClient * obj= (GstRTSPClient *)user_data;

	gint iBufSize = 0;
	gchar * pbuffer = NULL;
	GstMapInfo map;
	GstBuffer *buffer;

	static bool once=true;

	if(once)
	{
		gchar *name;
		gint width, height;
		const GstStructure *str;
		const char * format_str=NULL;

		/* check media type */
		name = gst_pad_get_name(pad);
		GstCaps *caps =  gst_pad_get_current_caps (pad);
		str = gst_caps_get_structure (caps, 0);
		format_str = gst_structure_get_string(str, "format");
		const char * struct_name_str = gst_structure_get_name(str);
		 if (!gst_structure_get_int (str, "width", &width) ||  !gst_structure_get_int (str, "height", &height) )
		 {
			 	 static int count_print=0;
			 	 if(count_print++%100==0)
			 	 {
			 		 g_print("[receive:]str=%s,format=%s\n",struct_name_str,format_str);
			 		 g_print ("[receive:] get  width/height failed !\n");		//如果没有检测出宽高的话，将没法分配数据或进行空间转换
			 	 }
			 	 once=true;
		 }else	 {
			 	 	g_print("[receive:]str=%s\n",struct_name_str);
			 		g_print("[receive:]The video size of this set of capabilities is %d(W)x%d(H),format:%s\n", width, height, format_str);
			 		obj->getInterfaceProcessImageBehavior()->processImage_setInfo(height, width);
			 		once=false;
		 }
		 printf("format_str=%s,w=%d h=%d\n",format_str,width,height);
	}

	buffer = GST_PAD_PROBE_INFO_BUFFER(info);

	if(buffer == NULL)
	{
		return GST_PAD_PROBE_OK;
	}

	if (gst_buffer_map(buffer, &map, GST_MAP_READ))
	{
		/* do something here. */
		if(!once)
		{
			iBufSize = gst_buffer_get_size(buffer);
			obj->getInterfaceProcessImageBehavior()->processImage((unsigned char *)map.data, (unsigned int)map.size);
		}

		gst_buffer_unmap(buffer, &map);
	}
	GST_PAD_PROBE_INFO_DATA(info) = buffer;

	return GST_PAD_PROBE_OK;
}

static void* gstStart(void* pram)
{
		GstRTSPClient * obj= (GstRTSPClient *)pram;

		CustomData* p_customData = obj->getCustomData();
		p_customData->loop = g_main_loop_new(NULL, FALSE);

		/* Start playing   */
		p_customData->ret = gst_element_set_state (p_customData->pipeline, GST_STATE_PAUSED);
		if (p_customData->ret == GST_STATE_CHANGE_FAILURE) {
			g_printerr ("Unable to set the pipeline to the GST_STATE_NULL state.\n");
			gst_object_unref (p_customData->pipeline);
			return NULL;
		}

		/* Wait until error or EOS */
		p_customData->bus = gst_element_get_bus(p_customData->pipeline);
		gst_bus_add_watch(p_customData->bus, bus_call, pram);

		g_main_loop_run(p_customData->loop);

		/* Free resources */
		gst_object_unref (p_customData->bus);
		gst_element_set_state (p_customData->pipeline, GST_STATE_NULL);
		gst_object_unref (p_customData->pipeline);
		return NULL;
}

int GstRTSPClient::InitMain()
{
	CustomData* p_customData = &customData;

	/* Initialize GStreamer */
	gst_init (NULL, NULL);

	/*
	 server:Live555 mediaServer
 	 client:gst-launch-1.0 rtspsrc location=rtsp://192.168.3.120:8554/123.264 ! rtph264depay ! avdec_h264 ! videoconvert ! xvimagesink
	 */

	/* Create the elements */
	p_customData->source = gst_element_factory_make ("rtspsrc", "Source");
	p_customData->rtph264depay = gst_element_factory_make ("rtph264depay", "rtph264depay");
	p_customData->h264parse = gst_element_factory_make ("h264parse", "h264parse");
#if ENABLE_OMX264ENC
	p_customData->avdec_h264 = gst_element_factory_make ("omxh264dec", "omxh264dec");				/* 在ubuntu系统的PC机上解码的插件为avdec_h264, 在tegra板卡上解码的插件为omxh264dec。 */
#else
	p_customData->avdec_h264 = gst_element_factory_make ("avdec_h264", "avdec_h264");				/* 在ubuntu系统的PC机上解码的插件为avdec_h264, 在tegra板卡上解码的插件为omxh264dec。 */
#endif

	p_customData->videoconvert = gst_element_factory_make("videoconvert","videoconvert");
	p_customData->xvimagesink = gst_element_factory_make ("fakesink", "fakesink");

	/* Create the empty pipeline */
	p_customData->pipeline = gst_pipeline_new (" video player");

	if (!p_customData->pipeline			||
		!p_customData->source				||
		!p_customData->rtph264depay	||
		!p_customData->h264parse ||
		!p_customData->avdec_h264	||
		!p_customData->videoconvert	||
		!p_customData->xvimagesink	)
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
	}

	/* set video source. */
	//这里location的值需要与live555提供的rtsp对应上。这里只是默认值，具体在RTSPCaptureGroup::CreateProducers()中设置
	const char * source_location="rtsp://192.168.1.84:8554/stream1";
	g_object_set (G_OBJECT(p_customData->source), "location", source_location , NULL);
	//启动链路的时候，服务端必须得先启动，客户端不会等服务端数据进来，将会直接结束线程；设置一下参数，可以延时请求rtsp连接，但时间不是很长
	//g_object_set (G_OBJECT(p_customData->source), "tcp-timeout",1000000000000 0 , NULL);

	/* Add Elements to Bin */
	gst_bin_add_many (
					GST_BIN(p_customData->pipeline),
					p_customData->source,
					p_customData->rtph264depay ,
					p_customData->h264parse,
					p_customData->avdec_h264,
					p_customData->videoconvert,
					p_customData->xvimagesink ,
					NULL);

	/* dynamic pad creation */

	if(!g_signal_connect(p_customData->source, "pad-added", G_CALLBACK(cb_new_rtspsrc_pad), p_customData->rtph264depay))
	{
		g_printerr ("Linker part(A)  Fail....\n");
		gst_object_unref (p_customData->pipeline);
		return -1;
	}

	if (gst_element_link_many (p_customData->rtph264depay, p_customData->h264parse, p_customData->avdec_h264, p_customData->videoconvert, NULL) != TRUE)
	{
		g_printerr ("Linker part(B) Fail....\n");
		gst_object_unref (p_customData->pipeline);
		return -1;
	}

	GstCaps * caps_decode = gst_caps_new_simple("video/x-raw",
			"format", G_TYPE_STRING, "I420",
			NULL);
	if(!gst_element_link_filtered(p_customData->videoconvert, p_customData->xvimagesink, caps_decode))
	{
		g_printerr ("Linker part(B) with part(C) Fail....\n");
		gst_object_unref (p_customData->pipeline);
		return -1;
	}
	gst_caps_unref(caps_decode);


	GstPad *avdec_h264_pad = NULL;
	avdec_h264_pad = gst_element_get_static_pad(p_customData->videoconvert,"src");
	gst_pad_add_probe(avdec_h264_pad, GST_PAD_PROBE_TYPE_BUFFER, (GstPadProbeCallback)decode_buffer, this, (GDestroyNotify)NULL);
	gst_object_unref(avdec_h264_pad);

	createThread();
	return 0;
}

int GstRTSPClient::Play()
{
	int ret= gst_element_set_state (customData.pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr ("Unable to set the pipeline to the play state.\n");
		gst_object_unref (customData.pipeline);
		return -1;
	}
	return 0;
}

int GstRTSPClient::Pause()
{
	int ret= gst_element_set_state (customData.pipeline, GST_STATE_PAUSED);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr ("Unable to set the pipeline to the pause state.\n");
		gst_object_unref (customData.pipeline);
		return -1;
	}
	return 0;
}

int GstRTSPClient::Ready()
{
	int ret= gst_element_set_state (customData.pipeline, GST_STATE_READY);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr ("Unable to set the pipeline to the ready state.\n");
		gst_object_unref (customData.pipeline);
		return -1;
	}
	return 0;
}

void GstRTSPClient::SetSourceLocation(const char * source_location)
{
	g_object_set (G_OBJECT(customData.source), "location", source_location , NULL);
}

void GstRTSPClient::createThread()
{
		int res = pthread_create(&a_thread, NULL, gstStart, this);
		if(res != 0)
		{
			printf("thread create failed!\n");
		}
}



