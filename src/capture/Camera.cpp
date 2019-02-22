/*
 * Camera.cpp
 *
 *  Created on: Dec 31, 2016
 *      Author: wang
 * Refactored on Jan 6, 2017 by Hoover
 */
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <string.h>
#include <stdbool.h>
#include "Camera.h"
#include <unistd.h>
#include "StlGlDefines.h"
#include<sys/time.h>
#include <cuda.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime_api.h>
#include <omp.h>
#include "thread.h"
#define MEMCPY memcpy


using namespace std;
using namespace cv;


extern bool enable_hance;
#if ENABLE_ENHANCE_FUNCTION
extern void cuHistEnh(cv::Mat src, cv::Mat dst);

extern void cuClahe(cv::Mat src, cv::Mat dst, unsigned int uiNrX = 8, unsigned int uiNrY = 8, float fCliplimit = 2.5, int procType = 0);

extern void cuUnhazed(cv::Mat src, cv::Mat dst);

extern "C" int rgbResize_(
	unsigned char *src, unsigned char *dst,
	int srcw, int srch, int dstw, int dsth );

extern "C" int uyvy2bgr_(
	unsigned char *dst, const unsigned char *src,
	int width, int height, cudaStream_t stream);

extern "C" int yuyv2bgr_ext_(
	unsigned char *dst, const unsigned char *src, unsigned char *gray,
	int width, int height, cudaStream_t stream);

extern "C" int yuyv2bgr_(
	unsigned char *dst, const unsigned char *src,
	int width, int height, cudaStream_t stream);


#endif


BaseVCap::~BaseVCap()
{
}
void BaseVCap::SavePic(const char* name)
{
	char * src = GetDefaultImg();
	Capture(src);
	cvSaveImage(name,src);
}

void SmallVCap::Capture(char* ptr)
{
};

void SmallVCap::CaptureFish(char* ptr)
{
}

void SmallVCap::SavePic(const char* name)
{

}
void SmallVCap::saveOverLap()
{

}

void HDVCap::SavePic(const char* name)
{
}

void save_yuyv_pic2(void *pic,int idx)
{
	FILE * fp;
	char buf[30];
	sprintf(buf,"./save_transYUV_%d.yuv",idx);
	fp=fopen(buf,"w");
	char data[20];
	fwrite(pic,1920*1080*2,1,fp);
	fclose(fp);
}


void my_save_rgb(char *filename,void *pic,int w,int h)
{
	Mat Pic(h,w,CV_8UC3,pic);
	imwrite(filename,Pic);
}
/*
void HDVCap::YUYVEnhance(unsigned char *ptr,unsigned char *temp_data,int w,int h)
{
#if 1
	static unsigned char * gpu_yuyv;
			static unsigned char * gpu_rgb;
			static unsigned char * gpu_enh;

			static bool once =true;
			static cudaStream_t m_cuStream[2];
			if(once)
			{
				cudaMalloc((void **)&gpu_yuyv,w*h*2);
				cudaMalloc((void **)&gpu_rgb,w*h*3);
				cudaMalloc((void **)&gpu_enh,(w)*(h)*3);

				for(int i=0; i<2; i++){
					cudaStreamCreate(&m_cuStream[i]);
				}
				once=false;
			}
			cudaMemcpy(gpu_yuyv,temp_data,w*h*2,cudaMemcpyHostToDevice);
#if ENABLE_ENHANCE_FUNCTION
			yuyv2bgr_(gpu_rgb,gpu_yuyv,w,h,m_cuStream[0]);

			Mat dst1(h,w,CV_8UC3,gpu_enh);
			Mat src1(h,w,CV_8UC3,gpu_rgb);
			cuClahe( src1,dst1, 4,4,4.5,0);

			cudaMemcpy(ptr,gpu_enh,w*h*3,cudaMemcpyDeviceToHost);
#endif
#endif
}
*/
void HDVCap::YUYV2RGB(unsigned char*dst,unsigned char *src,int w,int h)
{
#if 1
	int t[10]={0};
 timeval startT[20]={0};
	gettimeofday(&startT[4],0);
	//memcpy(dst,src,w*h*2);
	Mat rgbmat(h,w,CV_8UC3,dst);
	Mat yuyvmat(h,w,CV_8UC2,src);
	cvtColor(yuyvmat,rgbmat,CV_YUV2BGR_YUYV);
	gettimeofday(&startT[5],0);
			t[2]=((startT[5].tv_sec-startT[4].tv_sec)*1000000+(startT[5].tv_usec-startT[4].tv_usec))/1000.0;
			printf("deltatimet[5]-t[4] =%d ms   \n",t[2]);
#endif
}

#if ENABLE_ENHANCE_FUNCTION
void YUYVEnhanceFour(unsigned char * dst,unsigned char * src,int w,int h)
{
#if 1//ENABLE_ENHANCE_FUNCTION
		static unsigned char * gpu_uyvyFour;
		static unsigned char * gpu_rgbFour;
		static unsigned char * gpu_enhFour;
		static unsigned char * cpu_uyvyFour;
		static bool once =true;
		static cudaStream_t m_cuStreamFour[2];
		if(once)
		{
			cudaMalloc((void **)&gpu_uyvyFour,FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*4*2);
			cudaMalloc((void **)&gpu_rgbFour,FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*4*3);
			cudaMalloc((void **)&gpu_enhFour,(FPGA_SINGLE_PIC_W)*(FPGA_SINGLE_PIC_H*4)*3);

			for(int i=0; i<2; i++){
				cudaStreamCreate(&m_cuStreamFour[0]);
			}
			once=false;
			cpu_uyvyFour=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*4*2);
		}
		int noww=FPGA_SINGLE_PIC_W,nowh=0;
	//	if(w=1280)
	//	{
	//		nowh=FPGA_SINGLE_PIC_H*4;
	//	}
	//	else if(w==1920)
		{
			nowh=FPGA_SINGLE_PIC_H*4;
		}
		cudaMemcpy(gpu_uyvyFour,src,noww*nowh*2,cudaMemcpyHostToDevice);
		//yuyv2bgr_(gpu_rgb,gpu_yuyv,SDI_WIDTH,SDI_HEIGHT,m_cuStream[0]);
		uyvy2bgr_(gpu_rgbFour,gpu_uyvyFour,noww,nowh,m_cuStreamFour[0]);
		Mat dst1(nowh,noww,CV_8UC3,gpu_enhFour);
		Mat src1(nowh,noww,CV_8UC3,gpu_rgbFour);
		cuClahe( src1,dst1);		//, 4,4,4.5,0);
		cudaMemcpy(dst,gpu_enhFour,noww*nowh*3,cudaMemcpyDeviceToHost);
#endif
}
void YUYVEnhance(unsigned char * dst,unsigned char * src,int w,int h)
{
#if 1//ENABLE_ENHANCE_FUNCTION
		static unsigned char * gpu_uyvy;
		static unsigned char * gpu_rgb;
		static unsigned char * gpu_enh;
		static unsigned char * cpu_uyvy;
		static bool once =true;
		static cudaStream_t m_cuStream[2];
		if(once)
		{
			cudaMalloc((void **)&gpu_uyvy,FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*6*2);
			cudaMalloc((void **)&gpu_rgb,FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*6*3);
			cudaMalloc((void **)&gpu_enh,(FPGA_SINGLE_PIC_W)*(FPGA_SINGLE_PIC_H*6)*3);

			for(int i=0; i<2; i++){
				cudaStreamCreate(&m_cuStream[i]);
			}
			once=false;
			cpu_uyvy=(unsigned char *)malloc(FPGA_SINGLE_PIC_W*FPGA_SINGLE_PIC_H*6*2);
		}
		int noww=FPGA_SINGLE_PIC_W,nowh=0;
	//	if(w=1280)
	//	{
	//		nowh=FPGA_SINGLE_PIC_H*4;
	//	}
	//	else if(w==1920)
		{
			nowh=FPGA_SINGLE_PIC_H*6;
		}
		cudaMemcpy(gpu_uyvy,src,noww*nowh*2,cudaMemcpyHostToDevice);
		//yuyv2bgr_(gpu_rgb,gpu_yuyv,SDI_WIDTH,SDI_HEIGHT,m_cuStream[0]);
		uyvy2bgr_(gpu_rgb,gpu_uyvy,noww,nowh,m_cuStream[0]);
		Mat dst1(nowh,noww,CV_8UC3,gpu_enh);
		Mat src1(nowh,noww,CV_8UC3,gpu_rgb);
		cuClahe( src1,dst1);		//, 4,4,4.5,0);
		cudaMemcpy(dst,gpu_enh,noww*nowh*3,cudaMemcpyDeviceToHost);
#endif
}
#endif
void HDVCap::Capture(char* ptr){

}



//--------------the decorator cap class------------
AsyncVCap::AsyncVCap(auto_ptr<BaseVCap> coreCap):
	m_core(coreCap),thread_state(THREAD_READY),sleepMs(DEFAULT_THREAD_SLEEP_MS)
{
	initLock();
	start_thread(capThread,this);
	cout<<" start a capture thread"<<endl;
}
AsyncVCap:: ~AsyncVCap()
{
	Close();
}
bool AsyncVCap::Open()
{
	if(thread_state != THREAD_READY)
		return false;//do not support reopen.
	bool isOpened = m_core->Open();
	if(isOpened){
		Start();
	}
	return isOpened;
}

void AsyncVCap::Close()
{
	thread_state = THREAD_STOPPING;
	while( THREAD_IDLE != thread_state){
		usleep(100*1000);
	}
	m_core->Close();
}
void AsyncVCap::Capture(char* ptr)
{
	lock_read(ptr);
}

void AsyncVCap::SetDefaultImg(char *p)
{
	m_core->SetDefaultImg(p);
}

 void AsyncVCap::SavePic(const char* name)
 {
	m_core->SavePic(name);
 }
 
void AsyncVCap::Run()
{
	char *p = new char[m_core->GetTotalBytes()];
	do{
		usleep(100*1000);
	}while(THREAD_READY == thread_state);
	//cap in background thread
	while(thread_state == THREAD_RUNNING)
	{
		m_core->Capture(p);
		lock_write(p);
		usleep(sleepMs*1000);
	}
	thread_state = THREAD_IDLE;
	destroyLock();
	delete []p;
}

void* AsyncVCap::capThread(void*p)
{
	AsyncVCap *thread = (AsyncVCap*)p;
	thread->Run();
	return NULL;
}

void AsyncVCap::lock_read(char *ptr)
{
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_rdlock(&rwlock);
	MEMCPY(ptr, pImg,m_core->GetTotalBytes());
	pthread_rwlock_unlock(&rwlock);
}

void AsyncVCap::lock_write(char *ptr)
{
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_wrlock(&rwlock);
	MEMCPY(pImg,ptr,m_core->GetTotalBytes());
	pthread_rwlock_unlock(&rwlock);
}
void  AsyncVCap::Start()
{
	thread_state = THREAD_RUNNING;
}
void AsyncVCap::destroyLock()
{
	pthread_rwlock_destroy(&rwlock);
	pthread_rwlockattr_destroy(&rwlockattr);
}

void AsyncVCap::initLock()
{
	pthread_rwlockattr_init(&rwlockattr);
	pthread_rwlockattr_setpshared(&rwlockattr,2);
	pthread_rwlock_init(&rwlock,&rwlockattr);
}
//--------------V4lVcap instance------------------
V4lVcap:: ~V4lVcap()
{
	Close();
}
bool V4lVcap::Open()
{
	if(capUSB)
		cvReleaseCapture(&capUSB);
	capUSB = cvCreateCameraCapture(m_deviceId);

	if(capUSB){
		cvSetCaptureProperty(capUSB, CV_CAP_PROP_FRAME_WIDTH, width);
		cvSetCaptureProperty(capUSB, CV_CAP_PROP_FRAME_HEIGHT, height);
		cvSetCaptureProperty(capUSB, CV_CAP_PROP_FPS, 30);
		cout<<" starting a USB capture at "<<width<<"x"<<height<<endl;
	}
	return capUSB != NULL;
}
void V4lVcap::Close()
{
	if(capUSB)
		cvReleaseCapture(&capUSB);
	capUSB= NULL;
}
void V4lVcap::Capture(char* ptr)
{
	assert(ptr);
	IplImage *tmp;
	char * src = NULL;
	if(capUSB){
		tmp = cvQueryFrame(capUSB);
		if(tmp){
	//		cout<<"img depth="<<tmp->depth<<",width="<<tmp->width<<",height="<<tmp->height<<",widthstep="<<tmp->widthStep<<endl;
			src = tmp->imageData;
		}
	}
	
	if(!src){
		src= GetDefaultImg();
	}
	if(src != ptr)
		MEMCPY(ptr,src , GetTotalBytes());
}
//-------------------BMPVCap implementation-------
BMPVcap::BMPVcap(const char * fileName):pic(NULL)
{
	assert(fileName);
	strncpy(pFileName, fileName, 64);
}
BMPVcap:: ~BMPVcap()
{
	Close();
}
bool BMPVcap::Open()
{
	bool ret = false;
//	cout<<"   BMPVCcap opening "<<pFileName<<endl;
	if(pic)
		cvReleaseImage(&pic);
	pic = cvLoadImage(pFileName);
	if(strcmp(pFileName,"45.bmp")==0)
	{
		//printf("0\n");
	}
	if(pic == NULL)
	{
		cerr<<"failed 2 load "<<pFileName<<".bmp filled with color bar"<<endl;
		pic = cvCreateImage(Size(width, height), IPL_DEPTH_8U, depth);
		if(pic != NULL){
			int pic_bytes =0;
			pic_bytes=pic->height*pic->width*pic->depth;
			if(pic_bytes >GetTotalBytes() )
				pic_bytes=GetTotalBytes();
			memcpy(pic->imageData,GetDefaultImg(),pic_bytes);
			ret = true;
		}
	}
	else{
		Mat ycrcb;
		Mat im=cvarrToMat(pic);
#if 1

	/*	cvtColor( im,yuv_alpha,CV_RGB2BGR); //RGB->RGB
		memcpy(yuv_alpha.data,im.data,im.rows*im.cols*3);*/
		cvtColor( im, ycrcb, CV_RGB2YCrCb );
		cvtColor( im,yuv_alpha,CV_RGB2BGR); //RGB->RGB
		memcpy(yuv_alpha.data,im.data,im.rows*im.cols*3);
		Vec3b pix;
		Vec3b pix_alpha;
		for (int r = 0; r < ycrcb.rows; r++)
		{
			for (int c = 0; c < ycrcb.cols; c++)
			{
				pix = ycrcb.at<Vec3b>(r,c);
				pix_alpha.val[0]=pix.val[1];
				pix_alpha.val[1]=pix.val[0];
				pix_alpha.val[2]=pix.val[2];
				yuv_alpha.at<Vec3b>(r,c) = pix_alpha;
			}
		}
#else
				cvtColor( im, ycrcb, CV_RGB2YCrCb );
				cvtColor( im,yuv_alpha,CV_RGB2RGBA);
				memcpy(yuv_alpha.data,im.data,im.rows*im.cols*4);
				Vec3b pix;
				Vec4b pix_alpha;
				for (int r = 0; r < ycrcb.rows; r++)
				{
					for (int c = 0; c < ycrcb.cols; c++)
					{
						pix = ycrcb.at<Vec3b>(r,c);
						pix_alpha.val[0]=pix.val[1];
						pix_alpha.val[1]=pix.val[0];
						pix_alpha.val[2]=pix.val[2];
						pix_alpha.val[3]=pix.val[0];
						yuv_alpha.at<Vec4b>(r,c) = pix_alpha;
					}
				}
#endif
		cout<<"BMPVCap Open "<<pFileName<<" OK."<<endl;
		ret = true;
	}
	return ret;
}
void BMPVcap::Close()
{
	if(pic)
		cvReleaseImage(&pic);
	pic = NULL;
}
void BMPVcap::Capture(char* ptr)
{
}

//-----------------NVcamVcap implementation--------
NVcamVcap::~NVcamVcap()
{
	Close();
}

bool NVcamVcap::Open()
{
	if((capCSI !=NULL) && (capCSI->isOpened()))
		capCSI->release();
	
	char pGstPipelineString[1024];
	sprintf(pGstPipelineString,
			"nvcamerasrc sensor-id=%d ! video/x-raw(memory:NVMM), width=(int)%d, height=(int)%d,format=(string)I420, framerate=(fraction)60/1 ! nvvidconv flip-method=0 ! video/x-raw, format=(string)BGRx ! videoconvert ! appsink",
			m_sensorId, width, height);
	cout<<"NVcamVcap opening "<<pGstPipelineString<<endl;
	capCSI = new VideoCapture(pGstPipelineString); //open the default camera
	if((capCSI==NULL) || (!capCSI->isOpened())) { // check if we succeeded
		cerr << "Failed 2 open camera "<<m_sensorId<< endl;
		return false;
	}
	cout<<"NVcamVcap opening OK"<<endl;
	return true;
}
void NVcamVcap::Close()
{
	if((capCSI !=NULL) && (capCSI->isOpened()))
		capCSI->release();
	delete capCSI;
}
void NVcamVcap::Capture(char* ptr)
{
	assert(ptr);
	if(capCSI && capCSI->isOpened()){
		Mat frameDst(height,width,CV_8UC3,ptr);
		*capCSI>>(frameDst);
	}
	else{
		char * src = GetDefaultImg();
		if(ptr != src)
		    MEMCPY(ptr, src, GetTotalBytes());
	}
}

