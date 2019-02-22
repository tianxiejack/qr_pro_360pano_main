/*
 * main.cpp
 *
 *  Created on: 2018年8月23日
 *      Author: fsmdn121
 */
#include <glew.h>
#include <glut.h>
#include <freeglut_ext.h>
#include "ChosenCaptureGroup.h"
#include "Displayer.hpp"
#include "main.h"
#include "StlGlDefines.h"
//#include "cuda_convert.cuh"
#include "osa_image_queue.h"


static CRender render;
static int curChannel = TV_DEV_ID;
static OSA_BufHndl *imgQ[QUE_CHID_COUNT];
/*
void processFrame_cap(int cap_chid,unsigned char *src, struct v4l2_buffer capInfo, int format)
{
	char WindowName[64]={0};
	Mat img;

	if(curChannel == cap_chid)
	{
		//OSA_printf("%d %s. [%p]", OSA_getCurTimeInMsec(), __func__, src);
		if(cap_chid==TV_DEV_ID)
		{
			Mat temp;
			OSA_BufInfo* info = image_queue_getEmpty(imgQ[cap_chid]);
			if(info == NULL){
				//info = image_queue_getFull(imgQ[cap_chid]);
				//OSA_assert(info != NULL);
				return;
			}
			img = Mat(TV_HEIGHT,TV_WIDTH,CV_8UC3, info->physAddr);
		    temp = Mat(TV_HEIGHT,TV_WIDTH,CV_8UC2,src);
		    {
				cv::line(temp, Point(0, TV_HEIGHT/2+1), Point(TV_WIDTH/2, TV_HEIGHT/2+1), cvScalar(255) ,2);
				cv::line(temp, Point(0, TV_HEIGHT/4), Point(TV_WIDTH/2, TV_HEIGHT/4), cvScalar(255) ,2);
				cv::line(temp, Point(0, 0), Point(TV_WIDTH/2, TV_HEIGHT/2+200), cvScalar(255) ,2);
		    }
		   // cutColor_yuv2bgr_yuyv(cap_chid, temp, img);
		    //gpuConvertYUYVtoBGR_dev(temp.data,img.data,TV_WIDTH,TV_HEIGHT);
			//cv::cvtColor(temp,img, CV_YUV2BGR_YUYV);
			info->channels = img.channels();
			info->width = img.cols;
			info->height = img.rows;
			info->timestamp = (uint64)capInfo.timestamp.tv_sec*1000*1000*1000
					+ (uint64)capInfo.timestamp.tv_usec*1000;
		    image_queue_putFull(imgQ[cap_chid], info);
		}
		else if(cap_chid==HOT_DEV_ID)
		{
			OSA_BufInfo* info = image_queue_getEmpty(imgQ[cap_chid]);
			if(info == NULL){
				//info = image_queue_getFull(imgQ[cap_chid]);
				//OSA_assert(info != NULL);
				return;
			}
			img = Mat(HOT_HEIGHT,HOT_WIDTH,CV_8UC1,info->physAddr);
			{
			    Mat temp = Mat(HOT_HEIGHT,HOT_WIDTH,CV_8UC1,src);
				cv::line(temp, Point(0, HOT_HEIGHT/2+1), Point(HOT_WIDTH/2, HOT_HEIGHT/2+1), cvScalar(255) ,2);
				cv::line(temp, Point(0, HOT_HEIGHT/4), Point(HOT_WIDTH/2, HOT_HEIGHT/4), cvScalar(255) ,2);
				cv::line(temp, Point(0, 0), Point(HOT_WIDTH/2, HOT_HEIGHT/2+200), cvScalar(255) ,2);
			}
			cudaMemcpy(info->physAddr, src, HOT_HEIGHT*HOT_WIDTH, cudaMemcpyHostToDevice);
			info->channels = img.channels();
			info->width = img.cols;
			info->height = img.rows;
			info->timestamp = (uint64)capInfo.timestamp.tv_sec*1000*1000*1000
					+ (uint64)capInfo.timestamp.tv_usec*1000;
		    image_queue_putFull(imgQ[cap_chid], info);
		}
		//int a=0;
		//sprintf(WindowName, "video%d", a);
		//cv::imshow(WindowName, img);
		//OSA_printf("%d %s. 1", OSA_getCurTimeInMsec(), __func__);
	}
}

static void keyboard_event(unsigned char key, int x, int y)
{
	switch(key)
	{
	case '0':
		curChannel = TV_DEV_ID;
		render.dynamic_config(CRender::DS_CFG_ChId, 0, &curChannel);
		break;
	case '1':
		curChannel = HOT_DEV_ID;
		render.dynamic_config(CRender::DS_CFG_ChId, 0, &curChannel);
		break;
	case 'q':
	case 27:
		glutLeaveMainLoop();
		break;
	default:
		break;
	}
}

int main_cap(int argc, char **argv)
{
	ChosenCaptureGroup *grop[2];
	DS_InitPrm dsInit;
	memset(&dsInit, 0, sizeof(DS_InitPrm));
	//cuConvertInit(2);
	render.create();
	OSA_printf("render create success!");
	dsInit.bFullScreen = false;
	dsInit.keyboardfunc = keyboard_event;
	dsInit.nChannels = 2;
	dsInit.nQueueSize = 3;
	dsInit.channelsSize[0].w = TV_WIDTH;
	dsInit.channelsSize[0].h = TV_HEIGHT;
	dsInit.channelsSize[0].c = 3;
	dsInit.channelsSize[1].w = HOT_WIDTH;
	dsInit.channelsSize[1].h = HOT_HEIGHT;
	dsInit.channelsSize[1].c = 1;
	render.init(&dsInit);
	OSA_printf("render init success!");
	render.run();
	OSA_printf("render run success!");

	imgQ[0] = &render.m_bufQue[0];
	imgQ[1] = &render.m_bufQue[1];

	grop[0] = ChosenCaptureGroup :: GetTVInstance();
	grop[1] = ChosenCaptureGroup::GetHOTInstance();
	OSA_printf("capture success!");

	glutSetCursor(GLUT_CURSOR_NONE);
	glutMainLoop();

	render.stop();
	render.destroy();

//	cuConvertUinit();

	return 0;
}
*/

