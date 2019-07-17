/*
 * realRecord.cpp
 *
 *  Created on: 2019年3月29日
 *      Author: fsmdn121
 */
#include "realRecord.h"
#include "StlGlDefines.h"
#include "stdio.h"
#include <osa.h>
void RealRecordByCV::SaveVideoByTime()
{
	memset(mpfilename,0,128);
	time_t now = time(NULL);
	struct tm curr_tm = *localtime(&now);
	char Idxstring[PIC_COUNT][32]={"Panoramic","Partial1","Partial2","Partial3"};
	sprintf(mpfilename,"/home/nvidia/calib/realtimevideo/%s-%d%02d%02d%02d%02d%02d.avi",
			Idxstring[videoIdx],
			curr_tm.tm_year+1900, curr_tm.tm_mon+1, curr_tm.tm_mday,
			curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec
			);
}

RealRecordByCV::RealRecordByCV(int idx,int w,int h,int framerate):videoIdx(idx),videoW(w),
videoH(h),vdieoFrameRate(framerate)
{
	 mpfilename=( char *)malloc(128);
	 frame.create(videoH,videoW,CV_8UC3);
	 pushFlag = 0;
}

void RealRecordByCV::StartRecord()
{
	SaveVideoByTime();
	// cv::Size sWH=(videoW,videoH);
	outputVideo.open(mpfilename,CV_FOURCC('M','P','4','2'),vdieoFrameRate, cv::Size(videoW,videoH));
}
void RealRecordByCV::PushData(Mat *pmatSrc)
{
	pushFlag |= 0x01;
	if(!pmatSrc->empty())
	{
		outputVideo<<*pmatSrc;
	}
	pushFlag &= ~0x01;
}

void RealRecordByCV::StopRecord()
{
	while(pushFlag)
	{
		//printf("=====%d %s optFlag=%x\n", OSA_getCurTimeInMsec(), __func__, pushFlag);
		OSA_waitMsecs(20);
	}
	outputVideo.release();
}

char *RealRecordByCV::GetFileName()
{
	return mpfilename;
}
