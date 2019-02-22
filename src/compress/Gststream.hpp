/*
 * Compress.hpp
 *
 *  Created on: 2018年11月15日
 *      Author: wj
 */

#ifndef GSTREAMER_HPP_
#define GSTREAMER_HPP_

#include <opencv2/opencv.hpp>
#include "osa_buf.h"
#include "osa_sem.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/gstclock.h>
#include <gst_interfaces.h>
using namespace cv;
typedef int (*SendDataCallback)   (int dtype, unsigned char *buf, int size);

typedef int (*SendSyncDataCallback)   (void *data);
typedef enum
{
	FLIP_METHOD_NONE = 0,
	FLIP_METHOD_COUNTERCLOCKWISE,
	FLIP_METHOD_ROTATE_180,
	FLIP_METHOD_CLOCKWISE,
	FLIP_METHOD_HORIZONTAL_FLIP,
	FLIP_METHOD_UPPER_RIGHT_DIAGONAL,
	FLIP_METHOD_VERTICAL_FLIP,
	FLIP_METHOD_UPPER_LEFT_DIAGONAL,
	FLIP_METHOD_END
}FLIP_METHOD;

typedef enum
{
	XIMAGESRC = 0,
	APPSRC
}CAPTURE_SRC;

enum
{
	ENC_MIN_QP = 0,
	ENC_MAX_QP,
	ENC_MIN_QI,
	ENC_MAX_QI,
	ENC_MIN_QB,
	ENC_MAX_QB,
	ENC_QP_PARAMS_COUNT
};
typedef struct{
	double gyrox;
	double gyroy;
	double gyroz;
	int event;

}Privatedata;
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
	FLIP_METHOD filp_method;
	CAPTURE_SRC capture_src;
	SendDataCallback sd_cb;
	SendSyncDataCallback sy_cb;
	int Q_PIB[ENC_QP_PARAMS_COUNT];
	OSA_BufHndl *pushQueue;
	OSA_SemHndl *pushSem;

	Privatedata privatedata;
}RecordHandle;

typedef struct _gstCapture_data
{
	int width;
	int height;
	int framerate;
	int bitrate;
	unsigned int ip_port;
	FLIP_METHOD filp_method;
	CAPTURE_SRC capture_src;
	char* format;
	char* ip_addr;
	SendDataCallback sd_cb;
	SendSyncDataCallback sy_cb;
	int Q_PIB[ENC_QP_PARAMS_COUNT];
	void *notify;
}GstCapture_data;
//int (*screenGetData) (unsigned char * buf, int len);
class Gstreamer
{
	public :
		Gstreamer();
		~Gstreamer();
		void create();
		RecordHandle * gstpipeadd(GstCapture_data gstCapture_data);
		int gstCapturePushData(RecordHandle *recordHandle, char *pbuffer , int datasize);
		int gstCapturePushDataMux(RecordHandle *recordHandle, char *pbuffer , int datasize,Privatedata *privatedata);
		void encode(Mat src);
	private:
		int gstlinkInit_convert_enc_fakesink(RecordHandle *recordHandle);
		int gstlinkInit_appsrc_enc_rtp(RecordHandle *recordHandle);
		int gstlinkInit_convert_enc_rtp(RecordHandle *recordHandle);
		static GstPadProbeReturn enc_buffer(GstPad *pad, GstPadProbeInfo *info, gpointer user_data);
		static void * thrdhndl_out_buffer(void* arg);
		static void * thrdhndl_timer(void* arg);
		void getDisplayHeight(int *width, int *height);
		static gboolean bus_call(GstBus *bus, GstMessage *message, gpointer user_data);
		static void * thrdhndl_push_buffer(void* arg);
		static void * rtp_main_loop(void* arg);
		static GstPadProbeReturn enc_tick_cb(GstPad * pad, GstPadProbeInfo * info, gpointer user_data);
		int record_main_init(RecordHandle *recordHandle);
		static GstPadProbeReturn filesink1_buffer (GstPad *pad, GstPadProbeInfo *info, gpointer user_data);
};


#endif /* COMPRESS_HPP_ */
