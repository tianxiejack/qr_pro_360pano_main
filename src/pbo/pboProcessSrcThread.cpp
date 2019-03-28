
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <pthread.h>
#include "pboProcessSrcThread.h"
#include<string.h>
#include <glew.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include"Render.hpp"
#include "PBOManager.h"
#include <osa_sem.h>
using namespace cv;
static int iniCC=3;
extern Render render;
void *pbo_process_threadPIC(void *arg)
{
		while(1)
		{
			OSA_semWait(render.GetPBORcr(PANO_PIC)->getSemPBO(),100000);
			if(render.IstoSavePic(PANO_PIC))
			{
				Mat testData(360, 1920, CV_8UC3);
				int processId=render.GetPBORcr(PANO_PIC)->getCurrentPBOIdx();
				if((char *)*render.GetPBORcr(ROI_C)->getPixelBuffer(processId)!=NULL)
				{
					int offset=1920*(1080-360)*3;
					memcpy(testData.data,offset+ (char *)*render.GetPBORcr(PANO_PIC)->getPixelBuffer(processId),PANO360FBOW*360*iniCC);

					flip(testData,testData,0);
					imwrite("./PANO_PIC.bmp",testData);
					render.ResetSaveState(PANO_PIC);
				}
			}
		}
}

void *pbo_process_threadROI_A(void *arg)
{
	while(1)
	{
		OSA_semWait(render.GetPBORcr(ROI_A)->getSemPBO(),100000);
		if(render.IstoSavePic(ROI_A))
		{
			Mat testData(ROIFBOH, ROIFBOW, CV_8UC3);
			int processId=render.GetPBORcr(ROI_A)->getCurrentPBOIdx();
			if((char *)*render.GetPBORcr(ROI_A)->getPixelBuffer(processId)!=NULL)
			{
					memcpy(testData.data, (char *)*render.GetPBORcr(ROI_A)->getPixelBuffer(processId),ROIFBOH*ROIFBOW*iniCC);
					imwrite("./ROI_A.bmp",testData);
					render.ResetSaveState(ROI_A);
			}
		}
	}
}

void *pbo_process_threadROI_B(void *arg)
{
	while(1)
	{
		OSA_semWait(render.GetPBORcr(ROI_B)->getSemPBO(),100000);
		if(render.IstoSavePic(ROI_B))
		{
			Mat testData(ROIFBOH, ROIFBOW, CV_8UC3);
			int processId=render.GetPBORcr(ROI_B)->getCurrentPBOIdx();
			if((char *)*render.GetPBORcr(ROI_B)->getPixelBuffer(processId)!=NULL)
			{
					memcpy(testData.data, (char *)*render.GetPBORcr(ROI_B)->getPixelBuffer(processId),ROIFBOH*ROIFBOW*iniCC);
					imwrite("./ROI_B.bmp",testData);
					render.ResetSaveState(ROI_B);
			}
		}
	}
}
void *pbo_process_threadROI_C(void *arg)
{
	while(1)
	{
		OSA_semWait(render.GetPBORcr(ROI_C)->getSemPBO(),100000);
		if(render.IstoSavePic(ROI_C))
		{
			Mat testData(ROIFBOH, ROIFBOW, CV_8UC3);
			int processId=render.GetPBORcr(ROI_C)->getCurrentPBOIdx();
			if((char *)*render.GetPBORcr(ROI_C)->getPixelBuffer(processId)!=NULL)
			{
					memcpy(testData.data, (char *)*render.GetPBORcr(ROI_C)->getPixelBuffer(processId),ROIFBOH*ROIFBOW*iniCC);
					imwrite("./ROI_C.bmp",testData);
					render.ResetSaveState(ROI_C);
			}
		}
	}
}
void start_pbo_process_thread(int idx)
{
	pthread_t tid;
	int ret;
	switch (idx)
	{
		case	PANO_PIC:
				ret = pthread_create( &tid, NULL,pbo_process_threadPIC, NULL );
			break;
		case	ROI_A:
			ret = pthread_create( &tid, NULL,pbo_process_threadROI_A, NULL );
			break;
		case	ROI_B:
			ret = pthread_create( &tid, NULL,pbo_process_threadROI_B, NULL );
			break;
		case	ROI_C:
			ret = pthread_create( &tid, NULL,pbo_process_threadROI_C, NULL );
			break;
		default:
			break;
	}

}
