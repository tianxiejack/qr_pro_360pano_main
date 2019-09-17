/*
 * main_pano.cpp
 *
 *  Created on: 2018年9月14日
 *      Author: fsmdn113
 */

#include"Gldisplay.hpp"
#include <glew.h>
#include <glut.h>
#include <freeglut_ext.h>
#include "ChosenCaptureGroup.h"
#include "main.h"
#include "osa_image_queue.h"
#include "Gyroprocess.hpp"
#include "ImageProcess.hpp"
#include "Stich.hpp"
#include "config.hpp"
#include "StichAlg.hpp"
#include "Queuebuffer.hpp"
#include "DetectAlg.hpp"
#include "plantformcontrl.hpp"
#include <gst/gst.h>
#include"FileCapture.hpp"
#include"Gststreamercontrl.hpp"
#include"videorecord.hpp"
#include"videoload.hpp"
#include"rtspserver.hpp"
#include "CPortInterface.hpp"
#include "PortFactory.hpp"
#include"RecordManager.hpp"
#include "DxTimer.hpp"
#include"store.hpp"
#include"configfile.hpp"
#include"RTSPCaptureGroup/RTSPCaptureGroup.h"
#include "opencvCapIP.h"
#include "ptzProxyMsg.h"
#include "ipc_custom_head.hpp"
#include "ipcProc.h"
#include "eventParsing.hpp"
#include "CMessage.hpp"
#include "globalDate.h"
#include"cucolor.hpp"
#include "Render.hpp"

extern ptzProxyMsg ptzmsg;
static GLMain GLMain_render;
extern Render render;

ImageProcess *Imageprocesspt;
using namespace cv;
 OSA_BufHndl *imgQ[QUE_CHID_COUNT];
VideoCapture videocapture;
Mat fileframe;

#define AVINAME "/home/nvidia/calib1/mov11.avi"

static int fullframe=0;
int oddevenflag=-1;
static Config *config;
//static OSA_BufHndl *procossQ[QUE_CHID_COUNT];
#define FILEREAD 1
void processFrame_file(void *data,void *angle)
{
	bool status=0;
	int cap_chid=TV_DEV_ID;
	Mat img;
	int queueid=0;
	unsigned char *framdata=NULL;
	OSA_BufInfo* info=NULL;
	int calibration=1;
	Mat cap;// = Mat(TV_HEIGHT,TV_WIDTH,CV_8UC2,src);
	framdata=(unsigned char *)data;

	//printf("the angle=%d\n",*(int *)angle);
	if(Config::getinstance()->getcam_readfromfile()==0)
		return ;
	info = image_queue_getEmptytime(imgQ[queueid],OSA_TIMEOUT_FOREVER);
	if(info==NULL)
			return ;
	
	if(cap_chid==TV_DEV_ID)
		{
			img = Mat(config->getcamheight(),config->getcamwidth(),CV_8UC3, info->virtAddr);
		}
	memcpy(img.data,framdata,img.cols*img.rows*img.channels());
	info->channels = img.channels();
	info->width = img.cols;
	info->height = img.rows;
	info->timestamp = 0;
	info->calibration=calibration;
	info->framegyroyaw=*(double *)angle;
    	 Imageprocesspt->CaptureThreadProcess(img,info,queueid);
	//cv::imshow(WindowName, img);
	//waitKey(1);
	  image_queue_putFull(imgQ[queueid], info);

	if(DETECTTEST)
		OSA_waitMsecs(500);
	

}


void  processFrameRecord_pano(void *data,void *infodata)
{
	bool status=0;
	int cap_chid=TV_DEV_ID;
	Mat img;
	int queueid=0;
	Uint32 processtime=OSA_getCurTimeInMsec();
	unsigned char *framdata=NULL;
	OSA_BufInfo* info=NULL;
	int calibration=1;
	Mat cap;// = Mat(TV_HEIGHT,TV_WIDTH,CV_8UC2,src);
	framdata=(unsigned char *)data;
	//OSA_printf("**%s****%d*****\n", __func__,__LINE__);
	if(Config::getinstance()->getcam_readfromfile())
		return ;

	if(Config::getinstance()->getcamsource()==0)
		return ;
	//OSA_printf("**%s****%d*****\n", __func__,__LINE__);
	//printf("the angle=%d\n",*(int *)angle);
	//if(Config::getinstance()->getcam_readfromfile()==0)
	//	return ;
	if(data==NULL||infodata==NULL)
		return ;
	info = image_queue_getEmptytime(imgQ[queueid],OSA_TIMEOUT_FOREVER);
	if(info==NULL)
		{
			
			return ;
		}
	//OSA_printf("**%s****%d*****\n", __func__,__LINE__);
	VideoLoadData gryodata=*(VideoLoadData*)infodata;
	if(cap_chid==TV_DEV_ID)
		{
			img = Mat(config->getcamheight(),config->getcamwidth(),CV_8UC3, info->virtAddr);
		}

	memcpy(img.data,framdata,img.cols*img.rows*img.channels());

	info->channels = img.channels();
	info->width = img.cols;
	info->height = img.rows;
	info->timestamp = 0;
	info->calibration=calibration;
	info->framegyroyaw=gryodata.gyroz*ANGLESCALE;
	//OSA_printf("**%s****%d*****\n", __func__,__LINE__);
    	 Imageprocesspt->CaptureThreadProcess(img,info,queueid);
	///////////////////////////////////////////
	static Uint32 pretime=0;
	Uint32 currenttime=OSA_getCurTimeInMsec();
	if(currenttime-pretime>50||currenttime-pretime<30)
		{
			;
			//OSA_printf("********lost %d ms %s timeoffset=%d ms**********\n", OSA_getCurTimeInMsec(), __func__,currenttime-pretime);
		}
	
	pretime=currenttime;
	//OSA_printf("**%s****%d*****\n", __func__,__LINE__);
	//return;

	//////////////////////////////////////////
	//cv::imshow(WindowName, img);
	//waitKey(1);
	  image_queue_putFull(imgQ[queueid], info);
	//  OSA_printf("**%s****%d*****\n", __func__,__LINE__);
	//OSA_printf("********capture process%d ms**********\n", OSA_getCurTimeInMsec()-processtime);
	
	

}
void processFrame_pano(int cap_chid,unsigned char *src, struct v4l2_buffer capInfo, int format)
{
	bool status=0;
	if(Config::getinstance()->getcam_readfromfile())
		return ;
	if(Config::getinstance()->getcamsource())
		return ;
	char WindowName[64]={0};
	Mat img;
	int queueid=0;
	OSA_BufInfo* info=NULL;
	int calibration=0;
	setgyrostart(Plantformpzt::getinstance()->getplantinitflag());
	Mat cap;// = Mat(TV_HEIGHT,TV_WIDTH,CV_8UC2,src);
	Mat yuv;

	if(cap_chid==TV_DEV_ID)
		{
			fullframe=1;
			queueid=0;
			cap= Mat(config->getcamheight(),config->getcamwidth(),CV_8UC2,src);
			
		}
	else if(cap_chid==HOT_DEV_ID)
		{
			queueid=1;
			cap= Mat(config->getcamheight(),config->getcamwidth(),CV_8UC2,src);
			
		}
	
	GYRO_DATA_T gyro;

	///////////////////////////////////////////
	static Uint32 pretime=0;
	Uint32 currenttime=OSA_getCurTimeInMsec();
	if(currenttime-pretime>50||currenttime-pretime<30)
		{
			;
			//OSA_printf("********lost %d ms %s timeoffset=%d ms**********\n", OSA_getCurTimeInMsec(), __func__,currenttime-pretime);
		}
	pretime=currenttime;

	//return;

	//////////////////////////////////////////
	
	//OSA_printf("%d %s. 1 chid=%d", OSA_getCurTimeInMsec(), __func__,cap_chid);
	//if(cap_chid==TV_DEV_ID)
	{
		calibration=getGyroprocess(cap,&gyro);
		if(cap_chid==HOT_DEV_ID)
			{
				oddevenflag=getoddenv();
				fullframe=combition(cap,oddevenflag);
				cap=getcombition();
			}

		
		if(fullframe)
		info = image_queue_getEmpty(imgQ[queueid]);
		else
			return ;
		if(info == NULL){
			//info = image_queue_getFull(imgQ[queueid]);
			//OSA_assert(info != NULL);
			return;
			}
	}

	if(cap_chid==TV_DEV_ID)
		{
			img = Mat(config->getcamheight(),config->getcamwidth(),CV_8UC3, info->virtAddr);
		}
	else if(cap_chid==HOT_DEV_ID)
		{

			img = Mat(config->getcamheight()*2,config->getcamwidth(),CV_8UC3, info->virtAddr);
		}
	
	
	if(FILEVIDEO)
		{
			status=videocapture.read(fileframe);
			if(!fileframe.empty())
				{
					//printf("********1*file cap ok******\n");
					resize(fileframe,fileframe,Size(config->getpanoprocesswidth(),config->getpanoprocessheight()),0,0,INTER_LINEAR);
					//printf("********2*file cap ok******\n");
					fileframe.copyTo(img);

					
					//cvtColor(fileframe,img,CV_GRAY2BGR);
					//printf("********3*file cap ok******\n");
				}
			else
				{
					//videocapture.set(CV_CAP_PROP_POS_FRAMES,0);
					videocapture.release();
					videocapture.open(AVINAME);
					videocapture.read(fileframe);

				}
		


		}
	else{
		//double exec_time = (double)getTickCount();
		
		cvtColor(cap,img,CV_YUV2BGR_YUYV);

//		yuyv2BGR(cap,img);

/*
		yuv.create(cap.rows*3/2,cap.cols,CV_8UC1);
		unsigned char *pY, *pU, *pV, *pYUYV;
		pYUYV = cap.data;
		pY = yuv.data;
		pU = yuv.data+cap.cols*cap.rows;
		pV = yuv.data+(cap.cols*cap.rows*5>>2);

		cvtColor(img, yuv,CV_BGR2YUV_I420);
*/
		//exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
     		//printf("%s:line=%d, %f\n",__func__, __LINE__, exec_time);
		}
		

	
	FrameINFO fameinfo;
	
	getEuler(&info->framegyroroll,&info->framegyropitch,&info->framegyroyaw);
	

	//OSA_printf("the roll =%d pitch=%d yaw=%d\n",info->framegyroroll,info->framegyropitch,info->framegyroyaw);
	//info->rotaionstamp=fameinfo.intervalsita;
	//info->rotationvalid=fameinfo.validintervalsita;
	//OSA_printf("%d %s. 1   valid=%d rot=%d\n", OSA_getCurTimeInMsec(), __func__,info->rotationvalid,info->rotaionstamp);


	info->channels = img.channels();
	info->width = img.cols;
	info->height = img.rows;
	
	info->timestamp = (uint64)capInfo.timestamp.tv_sec*1000*1000*1000
			+ (uint64)capInfo.timestamp.tv_usec*1000;
	info->calibration=calibration;

	Privatedata privatedata;
	privatedata.gyrox= info->framegyroroll*1.0/ANGLESCALE ;
	privatedata.gyroy=info->framegyropitch*1.0/ANGLESCALE;
	privatedata.gyroz=info->framegyroyaw*1.0/ANGLESCALE;
	//info->calibration = 1;
	if(info->calibration==1)
	GstreaemerContrl::getinstance()->gstputmux(img,&privatedata);
    	 Imageprocesspt->CaptureThreadProcess(img,info,queueid);
	//cv::imshow(WindowName, img);
	//waitKey(1);
	image_queue_putFull(imgQ[queueid], info);

/*
	if(DETECTTEST)
		OSA_waitMsecs(500);
	*/
	//OSA_printf("%d %s. 1w=%d h=%d\n", OSA_getCurTimeInMsec(), __func__,info->width,info->height);

}
extern opencvCapIP cvCapIp;
int main_pano(int argc, char **argv)
{
	//cuinit();
	/* Initialize GStreamer */
	gst_init (NULL, NULL);
	#if CONFIGINIT
	Config::getinstance()->saveconfig();
	return 0;
	#endif
	/*config */
	CGlobalDate::Instance();
	config=Config::getinstance();
	config->loadconfig();
	/*dynamic status*/
	Status::getinstance()->loadconfig();
	/*video queue*/
	Queue::getinstance()->create();
	/*message queue*/
	CMessage::getInstance()->MSGDRIV_create();
	/*config */
	ConfigFile::getinstance()->create();
	/*timer */
	DxTimer::getinstance()->create();
	/*detect alg */
	#if USE_DETECTV2
	DetectAlg::getinstance()->createV2();
	#endif
	DetectAlg::getinstance()->create();
	/*stich alg*/
	StichAlg::getinstance()->create();
	/*plantform contrl*/
	Plantformpzt::getinstance()->create();
	/*record playback manager*/
	RecordManager::getinstance()->create();
	/*gst enc*/
	GstreaemerContrl::getinstance()->create();
	/*record video*/
	VideoRecord::getinstance()->create();
	/*playback video*/
	VideoLoad::getinstance()->create();
	VideoLoad::getinstance()->registerfun(processFrameRecord_pano);
	/*rtsp server*/
	RtspServer::getinstance()->create();
	/*video record information*/
	Store::getinstance()->create();

	GLMain_InitPrm dsInit;
	kalmanfilterinit();  
	/*video capture*/
	videocapture=VideoCapture(AVINAME);
	videocapture.read(fileframe);
	/*image process queue*/
	Imageprocesspt=new ImageProcess();
	Imageprocesspt->Init();
	Imageprocesspt->Create();
	//procossQ[0]=&Imageprocesspt->mcap_bufQue;
	GLMain_render.IPocess_bufQue[0]=&Imageprocesspt->m_bufQue[0];
	//memcpy(render.IPocess_bufQue,Imageprocesspt->m_bufQue,sizeof(Imageprocesspt->m_bufQue));
	dsInit.bFullScreen = false;
	//dsInit.keyboardfunc = keyboard_event;
	dsInit.nChannels = QUE_CHID_COUNT;
	dsInit.nQueueSize = QUE_CHID_COUNT;
	dsInit.memType = memtype_malloc;
	
	for(int i=0;i<QUE_CHID_COUNT;i++)
	{
		dsInit.channelsSize[i].w = config->getcamwidth();
		dsInit.channelsSize[i].h = config->getcamheight();
		dsInit.channelsSize[i].c = config->getcamchannel();
	}
	
	/*render create*/
	GLMain_render.start(argc,  argv,(void *)&dsInit);
	imgQ[TV_QUE_ID] = &GLMain_render.m_bufQue[TV_QUE_ID];
	imgQ[HOT_QUE_ID] = &GLMain_render.m_bufQue[HOT_QUE_ID];
	imgQ[RTSP_QUE_ID] = &GLMain_render.m_bufQue[RTSP_QUE_ID];
	ChosenCaptureGroup *grop[2];
	grop[0] = ChosenCaptureGroup :: GetTVInstance();
	grop[1] = ChosenCaptureGroup :: GetHOTInstance();
//	RTSPCaptureGroup :: GetRTSPInstance();
	/*test read file*/
	FileCapture filecapture;
	if(Config::getinstance()->getcam_readfromfile())
		{
			filecapture.create();
			filecapture.registcallback(processFrame_file);
		}
	//setgyrostart(1);
	OSA_printf("run app success!\n");

	Status::getinstance()->setworkmod(Status::PANOAUTO);
	CMessage::getInstance()->MSGDRIV_send(MSGID_EXT_INPUT_WorkModeCTRL, (void *)(Status::PANOAUTO));

	/*cs communication*/
	CEventParsing parsing;
	OSA_thrCreate(&parsing.comrecvEvent_thid, parsing.thread_comrecvEvent, 0, 0, NULL);
	OSA_thrCreate(&parsing.comsendEvent_thid, parsing.thread_comsendEvent, 0, 0, NULL);
	OSA_thrCreate(&parsing.Getaccept_thid, parsing.thread_Getaccept, 0, 0, NULL);
	OSA_thrCreate(&parsing.ReclaimConnect_thid, parsing.thread_ReclaimConnect, 0, 0, NULL);


	/*
	OSA_ThrHndl ipctest;

	CIPCProc ipchandle;
	OSA_thrCreate(&ipctest, ipchandle.thread_ipcEvent, 0, 0, NULL);
	 */
	  
	/*main loop*/
	GLMain_render.mainloop();

	OSA_thrDelete(&parsing.comrecvEvent_thid);
	OSA_thrDelete(&parsing.comsendEvent_thid);
	OSA_thrDelete(&parsing.Getaccept_thid);
	OSA_thrDelete(&parsing.ReclaimConnect_thid);

	return 0;
}



