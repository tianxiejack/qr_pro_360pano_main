#ifndef HDV4LCAP_H_
#define HDV4LCAP_H_

#include <sys/types.h>
#include <opencv2/opencv.hpp>
#include "Camera.h"
#include"driver_types.h"

#include <cuda.h>
#include <cuda_runtime.h>

using namespace cv;
//#define SHOW_TIMES
#ifndef V4L2CAMERA_HPP_
#define V4L2CAMERA_HPP_

#include <linux/videodev2.h>
#include <sys/types.h>
#include <opencv2/opencv.hpp>
//#include "Camera.h"
//#include "timing.h"
//#include "struct.hpp"
#include "osa_buf.h"
//#include"queue_display.h"
//struct v4l2_buffer;
using namespace cv;
//#define SHOW_TIMES
#define CLEAR(x) memset(&(x), 0, sizeof(x))



#define FRAME_WIDTH  1448
#define FRAME_HEIGHT 1156

#define MYVGA_WIDTH 1024
#define MYVGA_HEIGHT 768

#define MYSDI_WIDTH 1920
#define MYSDI_HEIGHT 1080



#define INPUT_IMAGE_WIDTH 1920
#define INPUT_IMAGE_HEIGHT 1080

#define CAP_CH_NUM 		(1)

//#define IMAGE_WIDTH  	1920		//1440
//#define IMAGE_HEIGHT 	1080	//576

#ifndef V4L2_PIX_FMT_H264
#define V4L2_PIX_FMT_H264     v4l2_fourcc('H', '2', '6', '4') /* H264 with start codes */
#endif

//#define EXIT_SUCCESS         0;
#define FAILURE_ALLOCBUFFER  -1;
#define FAILURE_DEVICEOPEN   -2;
#define FAILURE_DEVICEINIT   -3;

#define BUFFER_HEAD_LEN			(sizeof(int)*32)
#define BUFFER_ID(p)			((int*)(p) + 0)
#define BUFFER_CHID(p)			((int*)(p) + 1)
#define BUFFER_DATA(p)			((unsigned char*)(p) + BUFFER_HEAD_LEN)
#define BUFFER_ID_INV(data)		BUFFER_ID( ((unsigned char*)(data) - BUFFER_HEAD_LEN) )
#define BUFFER_CHID_INV(data)	BUFFER_CHID( ((unsigned char*)(data) - BUFFER_HEAD_LEN) )



enum	memory_type{
	MEMORY_NORMAL,
	MEMORY_LOCKED,
};

enum io_method {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
};

struct buffer {
	void   *start;
	size_t  length;
};



//-------------decorator implementations-------------------------
class HDAsyncVCap4:public Interface_VCap{
	public:
	    HDAsyncVCap4(auto_ptr<BaseVCap> coreCap,int now_format);
		virtual ~HDAsyncVCap4();
		virtual bool Open();
		virtual void Close();
		virtual void Capture(char* ptr);
		void CaptureFish(char* ptr){};
		virtual void SetDefaultImg( char *);
		virtual void SavePic(const char* name);
		void saveOverLap(){};
	private:
		int m_cap_chid;
		enum{
			THREAD_READY,
			THREAD_RUNNING,
			THREAD_STOPPING,
			THREAD_IDLE
		} thread_state;
		HDAsyncVCap4(){};
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




class HDv4l_cam:public BaseVCap {
public:
	HDv4l_cam(int devId,int width,int height,	int format,int type,enum memory_type mt=MEMORY_NORMAL);
	virtual ~HDv4l_cam();
	bool Open();// open device   init deivce  start capturing
	void Close(){};
	void Capture(char*p){};
	void CaptureFish(char*p){};
	void saveOverLap(){};

	void mainloop(int cap_chid);
	void run(void);
	void stop();
	static int  xioctl(int fh, int request, void *arg);


	int m_devFd;
	int imgwidth;
	int imgheight;
	int imgtype;
	struct buffer   *buffers;
	bool bRun;
//	char *get_split_buffer_ch(){return split_buffer_ch;};
private:
	void UYVY2UYVx(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight);
	void UYVquar(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight);

	void UYVY2UYV(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight);
	void UYVnoXquar(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight);


	void YUYV2UYVx(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight);
	void YUVquar(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight);
	void YUYV2RGB(unsigned char * src,unsigned char * dst,int w,int h);
	void YUYV2GRAY(unsigned char * src,unsigned char * dst,int w,int h);
	void RectFromPixels(unsigned char *src);
	int GetNowPicIdx(unsigned char *src);
	int ChangeIdx2chid(int idx);
	void start_queue();
	bool Data2Queue(v4l2_buffer *pYuvBuf,int width,int height,int chId);
	bool getEmpty(v4l2_buffer ** pYuvBuf, int chId);
	int  open_device(void);
	void close_device(void);
	int  init_device(void);
	void uninit_device(void);
	void init_userp(unsigned int buffer_size);
	void init_mmap(void);
	void init_read(unsigned int buffer_size);
	void start_capturing(void);
	void stop_capturing(void);
	int  read_frame(int cap_chid);
	void errno_exit(const char *s);
	int  setCurChannum();
	void init_cuda();
	int cur_CHANnum;
	char            dev_name[16];
	enum io_method  io;
	enum	memory_type		memType;
	unsigned int     n_buffers;
	unsigned int     bufSize;
	int							 bufferCount;
	int imgstride;
	int imgformat;
	int force_format;
	int Id;
	 char   *split_buffer;
	 char   *split_buffer_ch;
	Mat rgbImg[4];
	char   *dstbuffer[4];
	unsigned char *pTmpBuf;
};



#endif
#endif /* V4L2CAMERA_H_ */
