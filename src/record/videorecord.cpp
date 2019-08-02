#include"videorecord.hpp"
#include<stdio.h>
#include <queue>
#include "cv.h"
#include "cxcore.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Render.hpp"


VideoRecord* VideoRecord::instance=NULL;
VideoRecord::VideoRecord():timeenable(1),mtdenable(0),eventenable(0),tm_year(0),tm_mon(0),tm_mday(0),tm_hour(0),tm_min(0),tm_sec(0),videorecordfb(NULL),aviheadenable(1),
timerdelayenable(0),callback(NULL),forceclose_(0),forcecloseonece_(0)
{


}
VideoRecord::~VideoRecord()
{


}

void VideoRecord::create()
{
	_syncdata=new queue<SyncDate>;
	
}

void VideoRecord::heldrecord()
{
	struct tm tm_set;
	struct timeval tv_set;
	struct timezone tz_set;
	time_t tt_set=0;
	gettimeofday(&tv_set, &tz_set);
	tt_set = tv_set.tv_sec;
	memcpy(&tm_set, localtime(&tt_set), sizeof(struct tm));

	int week=tm_set.tm_wday;
	week=week%7;
	int hour=tm_set.tm_hour;
	hour=hour%24;
	int timeenableweek=0;
	int movenableweek=0;

	timeenableweek=recordpositionheld[0][week-1][hour-1];
	movenableweek=recordpositionheld[1][week-1][hour-1];

	settimerecordenable(timeenableweek);

	if((0 == getmtdecordenable()) && movenableweek)
	{
		Render::pthis->set_mtd_record(1);
		Render::pthis->StartRecordMtdVideo();
	}
	if((1 == getmtdecordenable()) && (0 == movenableweek))
	{
		Render::pthis->set_mtd_record(0);
		Render::pthis->StopRecordMtdVideo();
	}
			
	setmtdecordenable(movenableweek);

	if(movenableweek)
		{	
			//if(getmovtimedelay())
			seteventrecordenable(getmovtimedelay());

		}

	
		
	
//	 for(int i=0;i<EVENTRECORD;i++)
	 	//for(int j=0;j<WEEKRECORD;j++)
		//	for(int k=0;k<HOURSRECORD;k++)
			
//				recordpositionheld[EVENTRECORD][WEEKRECORD][HOURSRECORD];
	 
	 

}

/************************************************************************
Flag==0: gray
Flag==1: rgb
************************************************************************/
void Unchar2IplImg(unsigned char *pImg, IplImage *pDis, int nW, int nH,bool Flag)
{
	if (Flag)
	{
		for (int j = 0; j<nH; j++)
		{
			memcpy(&pDis->imageData[(nH-j-1)*nW*3],&pImg[j*nW*3],nW*3);
		}
	}
	else
	{
		for (int j = 0; j<nH; j++)
		{
			memcpy(&pDis->imageData[(nH-j-1)*nW],&pImg[j*nW],nW);
		}
	}

}

#define TMP_TEST 0
#if TMP_TEST
void VideoRecord::recordvideo(void *data,void* size)
{
	//printf("data=%p size=%d instance=%p\n",data,size,instance);
	
	if(data==NULL||size==NULL||instance==NULL)
		return ;
	//return;
	static char filename[128];
	static char filedataname[128];

	static char filenamestart[128];
	static char filedatanamestart[128];
	struct tm tm_set;
	struct timeval tv_set;
	struct timezone tz_set;
	time_t tt_set=0;
	gettimeofday(&tv_set, &tz_set);
	tt_set = tv_set.tv_sec;
	memcpy(&tm_set, localtime(&tt_set), sizeof(struct tm));

	SyncDate syncdata;
	memset(&syncdata,0,sizeof(SyncDate));
	int filewriteenable=0;

	char *videodata=(char *)data;
	int videolen=*(int*)size;

/*
	char zzqname[20] = {0};
	static int flagg = 0;
	static Mat zzqimg = Mat(1080,1920,CV_8UC3);
	static int curlen = 0;

	if(flagg<1)
	{
		if(videolen == 8)
		{
			memcpy(zzqimg.data, videodata, 8);
			curlen = 8;
		}
		
		else if(videolen > 10)
		{
			memcpy(zzqimg.data+8, videodata, videolen);
			curlen = 8 + videolen;
		}
		else if(videolen == 1)
		{
			memcpy(zzqimg.data+curlen, videodata, 1);
			curlen = 0;

			sprintf(zzqname, "output%d.bmp", flagg);
			printf("zzqimg.channels=%d\n",zzqimg.channels());
			imwrite(zzqname,zzqimg);
			memset(zzqimg.data, 0 ,curlen+1);
			flagg++;

			//imshow("zzq  ", zzqimg);
			//waitKey(1);
			
		}
	}
*/

	/*
	static char zzq_filename[128];
	static int zzq_count = 0;
	static FILE *zzq_videorecordfb;
	sprintf(zzq_filename,"%s",  "zzqrecord2.avi");
	
	if(zzq_count == 0)
		zzq_videorecordfb =  fopen(zzq_filename,"wb");

	if(zzq_count<300)
	{
		fwrite(videodata, videolen, 1, zzq_videorecordfb);
		zzq_count++;
	}


	if(zzq_count == 300)
	{
		fclose(zzq_videorecordfb);
		zzq_count++;
	}
	*/
	


	if(videolen>10)
		filewriteenable=1;
	if(filewriteenable)
	instance->getsync(&syncdata);
	if(instance->aviheadenable)
		{
			instance->aviheadlen=videolen;
			memcpy(instance->avihead,videodata,instance->aviheadlen);
			instance->aviheadenable=0;
			return;
		}
	
	if(instance->callback!=NULL)
		instance->callback();
	//printf("the  size=%d \n",videolen);

	//return ;

	int year=tm_set.tm_year+1900;
	int mon=tm_set.tm_mon+1;
	int day=tm_set.tm_mday;
	int hour=tm_set.tm_hour;
	int mint=tm_set.tm_min;
	int sect=tm_set.tm_sec;
	//printf("instance->getrecordflag()=%d\n",instance->getrecordflag());

	//CvSize zzqsize = cvSize(1920, 1080);
	//static CvVideoWriter *writer = cvCreateVideoWriter("/home/nvidia/calib/video/zzq.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25, zzqsize);
	//IplImage *frame = cvCreateImage(cvSize(1920,1080), 8, 1);

	instance->heldrecord();
	if(instance->getrecordflag()==0||instance->getforceclose()||instance->getforcecloseonece())
		{
			//setforceclose(0);
			instance->setforcecloseonece(0);
			if((instance->videorecordfb!=NULL))
				{
					
					sprintf(filenamestart, "%s/local_%04d%02d%02d-%02d%02d%02d.avi", 
						SAVEDIR, 
						instance->tm_year, instance->tm_mon, instance->tm_mday,
					      instance->tm_hour, instance->tm_min,instance->tm_sec);
					
					sprintf(filedatanamestart, "%s/local_%04d%02d%02d-%02d%02d%02d.xml", 
						SAVEDIR, 
						instance->tm_year, instance->tm_mon, instance->tm_mday,
						instance->tm_hour, instance->tm_min,instance->tm_sec);
				
					sprintf(filename, "%s/record_%04d%02d%02d-%02d%02d%02d_%04d%02d%02d-%02d%02d%02d.avi", 
					SAVEDIR, 
					instance->tm_year, instance->tm_mon, instance->tm_mday,
					instance->tm_hour, instance->tm_min,instance->tm_sec,
					year, mon, day,
					hour, mint,sect);
					
					sprintf(filedataname, "%s/record_%04d%02d%02d-%02d%02d%02d_%04d%02d%02d-%02d%02d%02d.xml", 
					SAVEDIR,
					instance->tm_year, instance->tm_mon, instance->tm_mday,
					instance->tm_hour, instance->tm_min,instance->tm_sec,
					year, mon, day,
					hour, mint,sect);
					
					fclose(instance->videorecordfb);
					instance->mydata.close();

					rename(filenamestart, filename);
					rename(filedatanamestart, filedataname);
					instance->videorecordfb=NULL;

				}
			return ;
		}
	else {
			if(instance->getforcecloseonece())
				instance->setforcecloseonece(0);
		}
		if((instance->videorecordfb == NULL)&&(videolen==8))
		{
			
			sprintf(filename, "%s/local_%04d%02d%02d-%02d%02d%02d.avi", 
				SAVEDIR, 
				year, mon, day,
				hour, mint,sect);
			sprintf(filedataname, "%s/local_%04d%02d%02d-%02d%02d%02d.xml", 
				SAVEDIR, 
				year, mon, day,
				hour, mint,sect);
			instance->tm_year=year;
			instance->tm_mon=mon;
			instance->tm_mday=day;
			instance->tm_hour=hour;
			instance->tm_min=mint;
			instance->tm_sec=sect;
			
			instance->videorecordfb = fopen(filename,"wb");
			instance->mydata.open(filedataname);
			instance->g_gst_wrPkt = 0;
			if(instance->videorecordfb!=NULL&&instance->aviheadenable==0)
			{
				fwrite(instance->avihead, instance->aviheadlen, 1, instance->videorecordfb);
				//Unchar2IplImg((unsigned char *)instance->avihead, frame, 1920, 1080, 1);
				//cvWriteFrame(writer, frame);
				printf("%s,%d, write,len=%d$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n", __FILE__,__LINE__, instance->aviheadlen);
				
			}
				
			printf(" open local file %s\n", filename);
		}
		if((year!=instance->tm_year||instance->tm_mon!=mon||instance->tm_mday!=day||(instance->tm_hour*60+instance->tm_min+30<hour*60+mint))&&(videolen==8))
			{
					sprintf(filenamestart, "%s/local_%04d%02d%02d-%02d%02d%02d.avi", 
						SAVEDIR, 
						instance->tm_year, instance->tm_mon, instance->tm_mday,
					      instance->tm_hour, instance->tm_min,instance->tm_sec);
					sprintf(filedatanamestart, "%s/local_%04d%02d%02d-%02d%02d%02d.xml", 
						SAVEDIR, 
						instance->tm_year, instance->tm_mon, instance->tm_mday,
						instance->tm_hour, instance->tm_min,instance->tm_sec);

					
					sprintf(filename, "%s/record_%04d%02d%02d-%02d%02d%02d_%04d%02d%02d-%02d%02d%02d.avi", 
					SAVEDIR, 
					instance->tm_year, instance->tm_mon, instance->tm_mday,
					instance->tm_hour, instance->tm_min,instance->tm_sec,
					year, mon, day,
					hour, mint,sect);
					
					sprintf(filedataname, "%s/record_%04d%02d%02d-%02d%02d%02d_%04d%02d%02d-%02d%02d%02d.xml", 
					SAVEDIR,
					instance->tm_year, instance->tm_mon, instance->tm_mday,
					instance->tm_hour, instance->tm_min,instance->tm_sec,
					year, mon, day,
					hour, mint,sect);
					
				fclose(instance->videorecordfb);
				instance->mydata.close();
				instance->videorecordfb=NULL;
				rename(filenamestart, filename);
				rename(filedatanamestart, filedataname);
				
				if(instance->videorecordfb == NULL)
					{
						
						sprintf(filename, "%s/local_%04d%02d%02d-%02d%02d%02d.avi", 
							SAVEDIR, 
							year, mon, day,
							hour, mint,sect);
						sprintf(filedataname, "%s/local_%04d%02d%02d-%02d%02d%02d.xml", 
							SAVEDIR, 
							year, mon, day,
							hour, mint,sect);
						instance->tm_year=year;
						instance->tm_mon=mon;
						instance->tm_mday=day;
						instance->tm_hour=hour;
						instance->tm_min=mint;
						instance->tm_sec=sect;
						
						instance->videorecordfb = fopen(filename,"wb");
						instance->mydata.open(filedataname);
						instance->g_gst_wrPkt = 0;
						if(instance->videorecordfb!=NULL&&instance->aviheadenable==0)
						{
							fwrite(instance->avihead, instance->aviheadlen, 1, instance->videorecordfb);
							printf("%s,%d, write,len=%d$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n", __FILE__,__LINE__, instance->aviheadlen);
						}
						printf(" open local file %s\n", filename);
					}

			}

		if(instance->videorecordfb != NULL)
		{
			fwrite(videodata, videolen, 1, instance->videorecordfb);
			//Unchar2IplImg((unsigned char *)instance->avihead, frame, 1920, 1080, 1);
			//cvWriteFrame(writer, frame);
			printf("%s,%d, write,len=%d$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n", __FILE__,__LINE__, instance->aviheadlen);
			if(filewriteenable)
			instance->mydata.write(syncdata.event, syncdata.gyroX, syncdata.gyroY, syncdata.gyroZ);
			instance->g_gst_wrPkt++;
			//if((instance->g_gst_wrPkt % 50) == 0)
			{
				fflush(instance->videorecordfb);
			}
		}

}
#endif
void VideoRecord::putsync(void *data)
{
	if(data==NULL)
		return;
	SyncDate syncdata=*(SyncDate*)data;
	_syncdata->push(syncdata);
}


void VideoRecord::getsync(void *data)
{
	SyncDate*  syncdata=(SyncDate*)data;
	if(data==NULL)
		return;
	if(_syncdata->empty())
		return;
	*syncdata=_syncdata->front();
	_syncdata->pop();

	//printf("%s queue size=%d\n",__func__,_syncdata->size());

}


VideoRecord* VideoRecord::getinstance()
{
	if(instance==NULL)
		instance=new VideoRecord();
	return instance;

}
