#include"FileCapture.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "config.hpp"
using namespace cv;
using namespace std;
//char filename[50]="/home/ubuntu/file/"

#define BLOCKIDMAX (13)
#define FEILDIDMAX (40)
FileCapture::FileCapture():capturecallback(NULL),captureangle(0.0),blockid(0),fieldid(0)
{

}

FileCapture::~FileCapture()
{

}

void FileCapture::create()
{
	capture=Mat(Config::getinstance()->getcamheight(),Config::getinstance()->getcamwidth(),CV_8UC3,cv::Scalar(0));
	MAIN_threadRecvCreate();

}
void FileCapture::destory()
{
	MAIN_threadRecvDestroy();

}

void FileCapture::registcallback(CallBackfun fun)
{
	capturecallback=fun;

}
void *FileCapture::readfilepicture(void *info)
{
	char bufname[50];
	int *angle=(int *)info;
	sprintf(bufname,"/home/ubuntu/calib/xb/%da%d.jpg",blockid,fieldid);
	//puts(bufname);
	Mat filepic;
	filepic=imread(bufname,CV_LOAD_IMAGE_UNCHANGED);
	//printf("filepicw=%d h=%d\n",filepic.cols,filepic.rows);
	if(filepic.empty())
		return NULL;
	if(filepic.channels()==1)
		{
			cvtColor(filepic,filepic,CV_GRAY2BGR);
			
		}

	if(1)
		resize(filepic,capture,Size(Config::getinstance()->getcamwidth(),Config::getinstance()->getcamheight()),0,0,INTER_LINEAR);
	if(fieldid==FEILDIDMAX-1)
		blockid++;
	int fov=Config::getinstance()->getcam_fixcamereafov();
	*angle=fov*1000*fieldid;
	*angle=*angle%360000;
	
	fieldid=(fieldid+1)%FEILDIDMAX;
	blockid=blockid%BLOCKIDMAX;
	
	
	//*angle=*angle*1000;
	//printf("angle=%d blockid=%d fieldid=%d\n",*angle,blockid,fieldid);
	return capture.data;

}


void FileCapture::main_Recv_func()
{
	OSA_printf("%s: Main Proc Tsk Is Entering...\n",__func__);
	
	unsigned char *data=NULL;
	int angle=0;
	while(mainRecvThrObj.exitProcThread ==  false)
	{	
		int capangle=0;
		OSA_waitMsecs(30);
		data=(unsigned char *)readfilepicture(&angle);
		//capangle=angle*1000;
		if(capturecallback!=NULL)
		capturecallback(capture.data,&angle);
	}

}


int FileCapture::MAIN_threadRecvCreate(void)
{
	int iRet = OSA_SOK;
	iRet = OSA_semCreate(&mainRecvThrObj.procNotifySem ,1,0) ;
	OSA_assert(iRet == OSA_SOK);


	mainRecvThrObj.exitProcThread = false;

	mainRecvThrObj.initFlag = true;

	mainRecvThrObj.pParent = (void*)this;

	iRet = OSA_thrCreate(&mainRecvThrObj.thrHandleProc, mainRecvTsk, 0, 0, &mainRecvThrObj);
	

	return iRet;
}


int FileCapture::MAIN_threadRecvDestroy(void)
{
	int iRet = OSA_SOK;

	mainRecvThrObj.exitProcThread = true;
	OSA_semSignal(&mainRecvThrObj.procNotifySem);

	iRet = OSA_thrDelete(&mainRecvThrObj.thrHandleProc);

	mainRecvThrObj.initFlag = false;
	OSA_semDelete(&mainRecvThrObj.procNotifySem);

	return iRet;
}
