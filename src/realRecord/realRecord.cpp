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
#include <unistd.h>
#include <sys/stat.h>

#define VIDEODIR  "/home/nvidia/calib/video"

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

void RealRecordByCV::StartMtdRecord()
{
	char video_dir[64];
	char date_dir[16];
	
	memset(mpfilename,0,128);
	time_t now = time(NULL);
	struct tm curr_tm = *localtime(&now);

	strcpy(video_dir, VIDEODIR);
	if (access(video_dir, F_OK))
		mkdir(video_dir, 0775);

	sprintf(date_dir, "/%d%02d%02d", curr_tm.tm_year+1900,curr_tm.tm_mon+1, curr_tm.tm_mday);
	strcat(video_dir, date_dir);
	
	if (access(video_dir, F_OK))
	mkdir(video_dir, 0775);
		
	char Idxstring[PIC_COUNT][32]={"Panoramic","Partial1","Partial2","Partial3", "detect"};
	sprintf(mpfilename,"%s/%s_%04d%02d%02d-%02d%02d%02d.avi",
			video_dir,
			Idxstring[videoIdx],
			curr_tm.tm_year+1900, curr_tm.tm_mon+1, curr_tm.tm_mday,
			curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec
			);

	outputVideo.open(mpfilename,CV_FOURCC('M','P','4','2'),vdieoFrameRate, cv::Size(videoW,videoH));

	tm_bak = curr_tm;
	printf("%s,%d, mtdname:%04d-%02d-%02d %02d:%02d:%02d\n",__FILE__,__LINE__,curr_tm.tm_year+1900, curr_tm.tm_mon+1, curr_tm.tm_mday,
			curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec);
}

void RealRecordByCV::StopMtdRecord()
{
	char filename[128] = {0};
	char video_dir[64];
	char date_dir[16];
	
	while(pushFlag)
	{
		OSA_waitMsecs(20);
	}
	outputVideo.release();

	time_t now = time(NULL);
	struct tm curr_tm = *localtime(&now);
	
	strcpy(video_dir, VIDEODIR);
	sprintf(date_dir, "/%d%02d%02d", tm_bak.tm_year+1900,tm_bak.tm_mon+1, tm_bak.tm_mday);
	strcat(video_dir, date_dir);

	sprintf(filename, "%s/detect_%04d%02d%02d-%02d%02d%02d_%04d%02d%02d-%02d%02d%02d.avi",
		video_dir,
		tm_bak.tm_year+1900, tm_bak.tm_mon+1, tm_bak.tm_mday,
		tm_bak.tm_hour, tm_bak.tm_min, tm_bak.tm_sec,
		curr_tm.tm_year+1900, curr_tm.tm_mon+1, curr_tm.tm_mday,
		curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec
		);
	printf("%s,%d, rename %s to %s\n", __FILE__, __LINE__, mpfilename, filename);
	rename(mpfilename, filename);
}
