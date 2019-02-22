/*
 * Camera.h
 *
 *  Created on: Dec 31, 2016
 *      Author: wang
 *  Refactored on Jan 6, 2017 by Hoover
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include<opencv2/opencv.hpp>
#include <GL/gl.h>
#include <pthread.h>
#include "StlGlDefines.h"
#include <memory>
#include <iostream>

using namespace std;
using namespace cv;

#if __cplusplus >= 201103L
template <typename T>

using auto_ptr = std::unique_ptr<T>;
#else
using std::auto_ptr;
//using std::auto_ptr;
#endif

#define DEFAULT_THREAD_SLEEP_MS 16
#define interface class __declspec(novtable)

//interface Interface{
//public:
//	virtual void get()=0;
//};

typedef class Interface_VCap{
	public:
		virtual bool Open() = 0;
		virtual void Close() = 0;
		virtual void Capture(char* ptr) = 0;
		virtual void CaptureFish(char* ptr) = 0;
		virtual void SetDefaultImg(char*) = 0;
		virtual void SavePic(const char* name) = 0;
		virtual void saveOverLap()=0;
} Interface_VCap, *pInterface_VCap;
//--------------abstract base class------------------
class BaseVCap:public Interface_VCap{
	public:
		BaseVCap(char * pImg = NULL):width(DEFAULT_IMAGE_WIDTH),height(DEFAULT_IMAGE_HEIGHT),depth(DEFAULT_IMAGE_DEPTH), pDefaultImg(pImg){};
		inline unsigned int GetTotalBytes(){return width*height*depth;};
		char* GetDefaultImg(){return pDefaultImg;};
		virtual void SetDefaultImg(char *pImg){pDefaultImg = pImg;};
		virtual void SavePic(const char* name);
		virtual ~BaseVCap()=0;
	protected:
		unsigned int width, height,depth;
		char * pDefaultImg;
};
//-------------decorator implementations-------------------------
class AsyncVCap:public Interface_VCap{
	public:
		AsyncVCap(auto_ptr<BaseVCap> coreCap);
		virtual ~AsyncVCap();
		virtual bool Open();
		virtual void Close();
		virtual void Capture(char* ptr);
		void CaptureFish(char* ptr){};
		virtual void SetDefaultImg( char *);
		virtual void SavePic(const char* name);
	private:
		enum{
			THREAD_READY,
			THREAD_RUNNING,
			THREAD_STOPPING,
			THREAD_IDLE
		} thread_state;
		AsyncVCap(){};
		void destroyLock();
		void initLock();
		void lock_read(char *ptr);
		void lock_write(char *ptr);
		static void* capThread(void*);
		void Run();
		void Start();
		auto_ptr<BaseVCap> m_core;
		pthread_t tid;
		pthread_rwlock_t rwlock;
        pthread_rwlockattr_t rwlockattr;
		unsigned int sleepMs;
};
//---------------------------implementations-------------------
// a pseudo capture to avoid null pointer issue.
class PseudoVCap:public BaseVCap{
public:
	PseudoVCap(int i=0){};
	~PseudoVCap(){};
	bool Open(){return true;};
	void Close(){};
	void Capture(char* ptr){};
	void CaptureFish(char* ptr){};
	void saveOverLap(){};
};

class SmallVCap:public BaseVCap{
public:
	SmallVCap(int chId):m_chId(chId){m_buffer = (unsigned char*)malloc(720*576*4);
	yuvImg = Mat(DEFAULT_IMAGE_HEIGHT,DEFAULT_IMAGE_WIDTH,CV_8UC3);};
	~SmallVCap(){free(m_buffer);};
	bool Open(){return true;};
	void Close(){};
	void Capture(char* ptr);
	void CaptureFish(char* ptr);
	void SavePic(const char* name);
	void saveOverLap();
private:
	int m_chId;
	unsigned char *m_buffer;
	Mat yuvImg;
};


class HDVCap:public BaseVCap{
public:
	HDVCap(int qid,int wide,int height):m_qid(qid){};
	bool Open(){return true;};
	void Close(){};
	void Capture(char* ptr);
	void CaptureFish(char* ptr){};
	void SavePic(const char* name);
	void saveOverLap(){};
//	void YUYVEnhance(unsigned char *ptr,unsigned char *temp_data,int w,int h);
	void YUYV2RGB(unsigned char*dst,unsigned char *src,int w,int h);
private:
	int m_qid;
};


class V4lVcap : public BaseVCap{
	public:
		V4lVcap(int deviceId):m_deviceId(deviceId),capUSB(NULL){};
		virtual ~V4lVcap();
		virtual bool Open();
		virtual void Close();
		virtual void Capture(char* ptr);
	private:
		V4lVcap(){};
		int m_deviceId;
		CvCapture *capUSB;
};

class BMPVcap : public BaseVCap{
	public:
		BMPVcap(const char * fileName);
		virtual ~BMPVcap();
		virtual bool Open();
		virtual void Close();
		virtual void Capture(char* ptr);
		void CaptureFish(char* ptr){};
		void SavePic(const char* name){};
		void saveOverLap(){};
	private:
		BMPVcap(){};
		char pFileName[64];
		IplImage * pic;
		Mat yuv_alpha;
};



class NVcamVcap : public BaseVCap{
	public:
		NVcamVcap(int sensorId):m_sensorId(sensorId),capCSI(NULL){};
		virtual ~NVcamVcap();
		virtual bool Open();
		virtual void Close();
		virtual void Capture(char* ptr);
	private:
		NVcamVcap(){};
		int m_sensorId;
		VideoCapture *capCSI;
};

#endif /* CAMERA_H_ */
