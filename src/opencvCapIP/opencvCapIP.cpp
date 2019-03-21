/*
 * opencvCapIP.cpp
 *
 *  Created on: 2019年3月14日
 *      Author: fsmdn121
 */
#include "opencvCapIP.h"
#include "CaptureGroup.h"
#include "osa_image_queue.h"
#include "Gyroprocess.hpp"
#include"Gststream.hpp"
#include "config.h"
#include <vector>
#include "unistd.h"
#include <pthread.h>
using namespace std;
using namespace cv;

opencvCapIP cvCapIp(0);
#if 0
opencvIPCamCaptureGroup opencvIPCamCaptureGroup::m_opencvIPCamCaptureGroup(DEFAULT_IMAGE_WIDTH, DEFAULT_IMAGE_HEIGHT, 3, 1);
static opencvCapIP* ocCapIP[1]={0};		//IPCamera相机组
opencvIPCamCaptureGroup::opencvIPCamCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount):
CaptureGroup(w,h,NCHAN,capCount)
{

}

 void opencvIPCamCaptureGroup::CreateProducers()
 {
 	 int dev_id=CVIP_DEVICE_ID_0;
 	 if(ocCapIP[dev_id]==NULL)
 	 {
 		ocCapIP[dev_id] = new opencvCapIP(dev_id, "rtsp://admin:abc12345@192.168.1.26");
 	 }
 }
 void opencvIPCamCaptureGroup::OpenProducers()
 {
	 int dev_id=CVIP_DEVICE_ID_0;
	 ocCapIP[dev_id] ->Open();
 }
vector<Consumer>   opencvIPCamCaptureGroup::GetConsumers(int *queueid,int count)
{
	 Consumer cons;
	 for(int i=0;i<count;i++)
	 {
		 int qid=queueid[i];
		   cons.pcap = new opencvCapIP(qid);
		   cons.idx = i;
		   v_cons.push_back(cons);
	 }
	 return v_cons;
}

opencvIPCamCaptureGroup * opencvIPCamCaptureGroup::GetCVIPCAMInstance()
{
	int queueid[1]={RTSP_QUE_ID};
	int count=1;
	static bool once =true;
	if(once){
		m_opencvIPCamCaptureGroup.init(queueid,count);
		once =false;
	}
	return &m_opencvIPCamCaptureGroup;
}
#endif


opencvCapIP::opencvCapIP(int devid):mdevid(devid)
{
	mstrURL="rtsp://admin:abc12345@192.168.1.26:554";
	caplastframe.create(1080,1920,CV_8UC3);
	capframe.create(1080,1920,CV_8UC3);
}

bool opencvCapIP::Open()
{
	  bool ret=m_cap.open(mstrURL);
	  if(!ret)
	  {
		  printf("open IPCAM  failed !\n");
	  }
	  else
	  {
		  printf("!!!!!!!!!!open IPCAM  OK !\n");
	  }
	  start_capturing();
	  return ret;
}

void opencvCapIP::Close()
{
}

extern OSA_BufHndl *imgQ[QUE_CHID_COUNT];
void opencvCapIP::processImg()
{
	int queueid=RTSP_QUE_ID;
	OSA_BufInfo* info=NULL;
	int chId = 0;
	info = image_queue_getEmpty(imgQ[queueid]);
	if(info == NULL){
		return;
		}
		getEuler(&info->framegyroroll,&info->framegyropitch,&info->framegyroyaw);
			info->channels = capframe.channels();
			info->width = capframe.cols;
			info->height = capframe.rows;
			info->virtAddr=capframe.data;
			Privatedata privatedata;
			privatedata.gyrox= info->framegyroroll*1.0/ANGLESCALE ;
			privatedata.gyroy=info->framegyropitch*1.0/ANGLESCALE;
			privatedata.gyroz=info->framegyroyaw*1.0/ANGLESCALE;

		//	if(info->calibration==1)
			//	GstreaemerContrl::getinstance()->gstputmux(img,&privatedata);
			// Imageprocesspt->CaptureThreadProcess(capframe,info,queueid);

			image_queue_putFull(imgQ[queueid], info);
}

void opencvCapIP::Capture(char *ptr)
{
	int delay=40;
	if(m_cap.isOpened())
	{
		int fps=m_cap.get(CV_CAP_PROP_FPS);

		delay=1000/fps;
		printf("************~~~~~~~~~~~delay=%d\n",delay);
	}
	while(1)
	{
		if(m_cap.isOpened())
		{
			m_cap >> capframe;
		//	if(capframe.rows>0)
			{
		//		caplastframe=capframe;
			}
	//		else
			{
	//			capframe=caplastframe;
			}
			imshow("a",capframe);
	//		processImg();
			waitKey(delay);
		//	usleep(20000);
		}
		else
		{
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!not opened!\n");
			m_cap.open(mstrURL);
			sleep(3);
		}
	}
}


void *startcap(void *arg)
{
		cvCapIp.Capture(NULL);
		return 0;
}

void opencvCapIP::start_capturing(void)
{
	pthread_t tid;
	int ret;
	ret = pthread_create( &tid, NULL,startcap, NULL );
}
