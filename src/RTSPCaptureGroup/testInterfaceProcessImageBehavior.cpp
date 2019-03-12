/*
 * testInterfaceProcessImageBehavior.cpp
 *
 *  Created on: 2018年5月21日
 *      Author: ubuntu
 */
#include "RTSPCaptureGroup.h"
#include "testInterfaceProcessImageBehavior.h"
#include "StlGlDefines.h"
#include "osa_image_queue.h"
#include "Gyroprocess.hpp"
#include "Gststream.hpp"
#include "config.h"
#include "ImageProcess.hpp"
using namespace std;
using namespace cv;

extern ImageProcess *Imageprocesspt;
static unsigned char * test_rtsp_image = NULL;//用来存放分配缓冲区的地址，将获取到的数据，memcpy到该地址。
extern  OSA_BufHndl *imgQ[QUE_CHID_COUNT];
void bgr2yuv(unsigned char* bgrBuffer, int W, int H, unsigned char* yuvBuffer) {
	cv::Mat bgrImg(H, W, CV_8UC3, bgrBuffer);
	cv::Mat yuvImg(H * 3 / 2, W, CV_8UC1, yuvBuffer);
	cv::cvtColor(bgrImg, yuvImg, CV_BGR2YUV_I420);
}

void yuv2bgr(unsigned char* yuvBuffer, int W, int H, unsigned char* bgrBuffer) {
	cv::Mat yuvImg(H * 3 / 2, W, CV_8UC1, yuvBuffer);
	cv::Mat bgrImg(H, W, CV_8UC3, bgrBuffer);
	cv::cvtColor(yuvImg, bgrImg, CV_YUV2BGR_I420);
}

ProcessYUVBehavior::ProcessYUVBehavior() {

}

ProcessYUVBehavior::~ProcessYUVBehavior() {

}

//实现yuv数据处理的接口
void ProcessYUVBehavior::processImage_setInfo(int h, int w) {
	m_width = w;
	m_height =h;
}

void ProcessYUVBehavior::processImage(unsigned char * data,
		unsigned int length) {
	int rows = m_height *3 /2;
	int cols = m_width;
	Mat localyuvImg(rows, cols,CV_8UC1,data);
	Mat localbgrImg;
	cv::cvtColor(localyuvImg, localbgrImg, CV_YUV2RGB_I420);
/*	static int a=0;
	if (a < 300 &&(a++%10 == 1)) {
		char fname[50];
		sprintf(fname, "rgb_%d.bmp", a);
		imwrite(fname, localbgrImg);
	}*/
	doProcessImage(localbgrImg);
}

//----------------------ProcessYUVShowBehavior----------------------//
void ProcessYUVShowBehavior::doProcessImage(cv::Mat & rgb) {
	cv::imshow("video", rgb);
	//cv::waitKey(25);  使用waitkey将使画面停止不动,所以这里需要注释，有时即使注释还是会有画面不出现的情况。
}

//----------------ProcessYUVWriteBehavior----------------//
void ProcessYUVWriteBehavior::doProcessImage(cv::Mat & rgb) {
	static int count = 0;
	static int save_gap_count = 0;

	if (save_gap_count == 90) {
		char str_bgr[100] = { 0 };
		sprintf(str_bgr, "./img/bgrImg%d.jpg", count);
		imwrite(str_bgr, rgb);
		save_gap_count = 0;
	}

	save_gap_count++;
	count++;
}

//----------------ProcessQueueBehavior----------------//
ProcessQueueBehavior::ProcessQueueBehavior(int id) :
		dev_id(id) {
	static bool Once = true;
	if (Once) {
	//	init_buffer();		//分配【LINK_CHANL_MAX】组相机的缓冲
	//	start_queue();	//填充第【MAIN_ONE_OF_TEN】组相机的空缓冲
	//	Once = false;
	//	printf("[ProcessQueueBehavior:]Init ProcessQueueBehavior Done.\n");
	}
}

void ProcessQueueBehavior::doProcessImage(cv::Mat & rgb) {
	unsigned char ** transformed_src = &test_rtsp_image;
	int queueid=RTSP_QUE_ID;
	OSA_BufInfo* info=NULL;
	//memcpy(test_rtsp_image, rgb.data, rgb.total()*rgb.channels());
	int chId = 0;
	/*
	switch (dev_id) {
	case RTSP_QUE_ID:
		chId = RTSP_QUE_ID;
		break;
	default:
		break;
	}
*/


		info = image_queue_getEmpty(imgQ[queueid]);
		if(info == NULL){
			return;
			}
		getEuler(&info->framegyroroll,&info->framegyropitch,&info->framegyroyaw);



			info->channels = rgb.channels();
			info->width = rgb.cols;
			info->height = rgb.rows;
			info->virtAddr=rgb.data;
		//	info->timestamp = (uint64)capInfo.timestamp.tv_sec*1000*1000*1000
		//			+ (uint64)capInfo.timestamp.tv_usec*1000;
		//	info->calibration=calibration;

			Privatedata privatedata;
			privatedata.gyrox= info->framegyroroll*1.0/ANGLESCALE ;
			privatedata.gyroy=info->framegyropitch*1.0/ANGLESCALE;
			privatedata.gyroz=info->framegyroyaw*1.0/ANGLESCALE;
//			if(info->calibration==1)
//			GstreaemerContrl::getinstance()->gstputmux(rgb,&privatedata);
							//___Imageprocesspt->CaptureThreadProcess(rgb,info,queueid);
			//cv::imshow(WindowName, rgb);
			//waitKey(1);
			image_queue_putFull(imgQ[queueid], info);


/*	if (Data2Queue(*transformed_src, rgb.cols, rgb.rows, queueid)) {
		//printf("rgba.cols=%d, rgba.rows=%d, chId=%d, dev_id=%d\n",rgba.cols, rgba.rows, chId, dev_id);
		if (getEmpty(transformed_src, chId)) {
		}
	}*/
}

bool ProcessQueueBehavior::getEmpty(unsigned char** pYuvBuf, int chId) {
/*	int status = 0;
	bool ret = true;
	while (1) {
		status = OSA_bufGetEmpty(&queue_main_sub->bufHndl[chId], &m_bufId[chId],
				0);
		if (status == 0) {
			*pYuvBuf =
					(unsigned char*) queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].virtAddr;
			break;
		} else {
			if (!OSA_bufGetFull(&queue_main_sub->bufHndl[chId], &m_bufId[chId],
					OSA_TIMEOUT_FOREVER)) {
				if (!OSA_bufPutEmpty(&queue_main_sub->bufHndl[chId],
						m_bufId[chId])) {
					;
				}
			}
		}
	}
	return ret;*/
}

bool ProcessQueueBehavior::Data2Queue(unsigned char *pYuvBuf, int width,
		int height, int chId) {
/*	if (chId >= RTSP_QUEUE_ID_MAX) {
		printf("[Data2Queue:]chId=%d>=%d ;Failed\n", chId, RTSP_QUEUE_ID_MAX);
		return false;
	}

	queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].width = width;
	queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].height = height;
	queue_main_sub->bufHndl[chId].bufInfo[m_bufId[chId]].strid = width;
	OSA_bufPutFull(&queue_main_sub->bufHndl[chId], m_bufId[chId]);
	return true;*/
}

void ProcessQueueBehavior::start_queue() {
	//getEmpty(&test_rtsp_image, MAIN_ONE_OF_TEN);
}

void ProcessQueueBehavior::init_buffer() {
//	queue_main_sub = (Alg_Obj *) alg_buf_init();
//	alg_obj_init(queue_main_sub);
}

