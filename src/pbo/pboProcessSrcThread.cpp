
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
void SavePicByTime(char *filename,int idx)
{
	time_t now = time(NULL);
		struct tm curr_tm = *localtime(&now);

		char Idxstring[PIC_COUNT][32]={"Panoramic","Partial1","Partial2","Partial3"};
		sprintf(filename,"/home/nvidia/calib/realtimevideo/%s-%d%02d%02d%02d%02d%02d.jpg",
						Idxstring[idx],
						curr_tm.tm_year+1900, curr_tm.tm_mon+1, curr_tm.tm_mday,
						curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec
						);
}

void process(SaveIDX id,bool isCapVideo)
{
	int offsetPIC=1920*(1080-360)*3;
	int offsetROI=0;
	int totalsizePIC=PANO360FBOW*360*iniCC;
	int totalsizeROI=ROIFBOH*ROIFBOW*iniCC;

	int offset[PIC_COUNT]={offsetPIC,offsetROI,offsetROI,offsetROI};
	int totalSize[PIC_COUNT]={totalsizePIC,totalsizeROI,totalsizeROI,totalsizeROI};

	int matW[PIC_COUNT]={1920,ROIFBOW,ROIFBOW,ROIFBOW};
	int matH[PIC_COUNT]={360,ROIFBOH,ROIFBOH,ROIFBOH};

//	OSA_semWait(render.GetPBORcr(id)->getSemPBO(),100000);
	Mat testData(matH[id], matW[id], CV_8UC3);
	bool isCapedFrame=false;
	if(!isCapVideo)
	{
			if(render.IstoSavePic(id))
			{
				int processId=render.GetPBORcr(id)->getCurrentPBOIdx();
				if((char *)*render.GetPBORcr(id)->getPixelBuffer(processId)!=NULL)
				{
					memcpy(testData.data,offset[id]+ (char *)*render.GetPBORcr(id)->getPixelBuffer(processId),totalSize[id]);
					if(id==PANO_PIC)
							flip(testData,testData,0);
					char filename[128]={0};
					SavePicByTime(filename,id);
					imwrite(filename,testData);
					render.sendfile(filename);
					render.ResetSaveState(id);
				}
				isCapedFrame=true;
			}
	}
	else
	{
		if(!isCapedFrame)
		{
			int processId=render.GetPBORcr(id)->getCurrentPBOIdx();
			if((char *)*render.GetPBORcr(id)->getPixelBuffer(processId)!=NULL)
			{
				memcpy(testData.data,offset[id]+ (char *)*render.GetPBORcr(id)->getPixelBuffer(processId),totalSize[id]);
				if(id==PANO_PIC)
					flip(testData,testData,0);
			}
		}
		render.GetifRealRecord(id)->PushData(&testData);
	}
}


void *pbo_process_threadPIC(void *arg)
{
		while(1)
		{
			OSA_semWait(render.GetPBORcr(PANO_PIC)->getSemPBO(),100000);
			if(render.IstoSavePic(PANO_PIC))
			{
				process(PANO_PIC,false);
			}

			if(render.IstoRecordVideo(PANO_PIC))
			{
				process(PANO_PIC,true);
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
			process(ROI_A,false);
		}
		if(render.IstoRecordVideo(ROI_A))
		{
			process(ROI_A,true);
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
			process(ROI_B,false);
		}
		if(render.IstoRecordVideo(ROI_B))
		{
			process(ROI_B,true);
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
			process(ROI_C,false);
		}
		if(render.IstoRecordVideo(ROI_C))
		{
			process(ROI_C,true);
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
