
#include "HDV4lcap.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include<opencv2/opencv.hpp>
#include "StlGlDefines.h"
#include "thread.h"
#include <osa_buf.h>
#include"StlGlDefines.h"
#include <osa_sem.h>
#if ARM_NEON_ENABLE
#include"yuv2rgb.h"
#endif
#if TRACK_MODE
#include "VideoProcessTrack.hpp"
#endif
#include <malloc.h>
#include <omp.h>

#if MVDECT
#include"MvDetect.h"
#include "mvdetectInterface.h"
#endif

#include <cuda.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime_api.h>
#include "main.h"
#define MEMCPY memcpy

#define INPUT_IMAGE_WIDTH 1920
#define INPUT_IMAGE_HEIGHT 1080

using namespace std;
#if USE_QUE
static bool Once_buffer=true;
v4l2_buffer *p_tv_buf=NULL;
v4l2_buffer *p_hot_buf=NULL;
extern Alg_Obj *queue_TV_HOT;
int m_bufId[QUE_CHID_COUNT]={0};
#endif



extern void DeinterlaceYUV_Neon(unsigned char *lpYUVFrame, int ImgWidth, int ImgHeight, int ImgStride);

HDv4l_cam::HDv4l_cam(int devId,int width,int height,int format,int type,enum	memory_type mt):io(IO_METHOD_USERPTR),imgwidth(width),
imgheight(height),buffers(NULL),memType(mt),cur_CHANnum(0),
force_format(1),m_devFd(-1),n_buffers(0),bRun(false),Id(devId),BaseVCap(),
imgformat(format),imgtype(type)
{
		sprintf(dev_name, "/dev/video%d",devId);
			imgstride 	= imgwidth*2;
			bufSize 	= imgwidth * imgheight * 2;
			memType = MEMORY_NORMAL;
			bufferCount = 6;
#if USE_QUE
			if(Once_buffer)
			{
				init_buffer();
				Once_buffer=false;
			}
#endif
}

HDv4l_cam::~HDv4l_cam()
{
	stop_capturing();
	uninit_device();
	close_device();
}

void YUYV2UYVx(unsigned char *ptr,unsigned char *Yuyv, int ImgWidth, int ImgHeight)
{
	for(int j =0;j<ImgHeight;j++)
	{
		for(int i=0;i<ImgWidth*2/4;i++)
		{
			*(ptr+j*ImgWidth*4+i*8+1)=*(Yuyv+j*ImgWidth*2+i*4);
			*(ptr+j*ImgWidth*4+i*8+0)=*(Yuyv+j*ImgWidth*2+i*4+1);
			*(ptr+j*ImgWidth*4+i*8+2)=*(Yuyv+j*ImgWidth*2+i*4+3);
			*(ptr+j*ImgWidth*4+i*8+3)=0;

			*(ptr+j*ImgWidth*4+i*8+5)=*(Yuyv+j*ImgWidth*2+i*4+2);
			*(ptr+j*ImgWidth*4+i*8+4)=*(Yuyv+j*ImgWidth*2+i*4+1);
			*(ptr+j*ImgWidth*4+i*8+6)=*(Yuyv+j*ImgWidth*2+i*4+3);
			*(ptr+j*ImgWidth*4+i*8+7)=0;
		}
	}
}
void HDv4l_cam::RectFromPixels(unsigned char *src)
{
}
void save_SDIyuyv_pic(void *pic,int w,int h)
{
	FILE * fp;
	fp=fopen("./Van_save_YUV.yuv","w");
	fwrite(pic,w*h*2,1,fp);
	fclose(fp);
}

void HDv4l_cam::UYVnoXquar(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
	for(int j =0;j<ImgHeight;j++)
	{
		for(int i=0;i<ImgWidth*2/4;i++)
		{
			*(dst+j*ImgWidth*3+i*6+0)=*(src+j*ImgWidth*2+i*4+0);
			*(dst+j*ImgWidth*3+i*6+1)=*(src+j*ImgWidth*2+i*4+1);
			*(dst+j*ImgWidth*3+i*6+2)=*(src+j*ImgWidth*2+i*4+2);

			*(dst+j*ImgWidth*3+i*6+3)=*(src+j*ImgWidth*2+i*4+0);
			*(dst+j*ImgWidth*3+i*6+4)=*(src+j*ImgWidth*2+i*4+3);
			*(dst+j*ImgWidth*3+i*6+5)=*(src+j*ImgWidth*2+i*4+2);

		}
	}
}

void HDv4l_cam::UYVY2UYV(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
}



void HDv4l_cam::UYVquar(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
	for(int j =0;j<ImgHeight;j++)
	{
		for(int i=0;i<ImgWidth*2/4;i++)
		{
			*(dst+j*ImgWidth*4+i*8+1)=*(src+j*ImgWidth*2+i*4+1);
			*(dst+j*ImgWidth*4+i*8+0)=*(src+j*ImgWidth*2+i*4+0);
			*(dst+j*ImgWidth*4+i*8+2)=*(src+j*ImgWidth*2+i*4+2);
			*(dst+j*ImgWidth*4+i*8+3)=0;

			*(dst+j*ImgWidth*4+i*8+5)=*(src+j*ImgWidth*2+i*4+3);
			*(dst+j*ImgWidth*4+i*8+4)=*(src+j*ImgWidth*2+i*4+0);
			*(dst+j*ImgWidth*4+i*8+6)=*(src+j*ImgWidth*2+i*4+2);
			*(dst+j*ImgWidth*4+i*8+7)=0;
		}
	}
}
void HDv4l_cam::UYVY2UYVx(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
}

void HDv4l_cam::YUVquar(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
	for(int j =0;j<ImgHeight;j++)
		{
			for(int i=0;i<ImgWidth*2/4;i++)
			{
				*(dst+j*ImgWidth*4+i*8+1)=*(src+j*ImgWidth*2+i*4);
				*(dst+j*ImgWidth*4+i*8+0)=*(src+j*ImgWidth*2+i*4+1);
				*(dst+j*ImgWidth*4+i*8+2)=*(src+j*ImgWidth*2+i*4+3);
				*(dst+j*ImgWidth*4+i*8+3)=0;

				*(dst+j*ImgWidth*4+i*8+5)=*(src+j*ImgWidth*2+i*4+2);
				*(dst+j*ImgWidth*4+i*8+4)=*(src+j*ImgWidth*2+i*4+1);
				*(dst+j*ImgWidth*4+i*8+6)=*(src+j*ImgWidth*2+i*4+3);
				*(dst+j*ImgWidth*4+i*8+7)=0;
			}
		}
}
void HDv4l_cam::YUYV2UYVx(unsigned char *dst,unsigned char *src, int ImgWidth, int ImgHeight)
{
}

	//YUVquar(dst,src,ImgWidth,ImgHeight);

void HDv4l_cam::YUYV2RGB(unsigned char * src,unsigned char * dst,int w,int h)
{
}

void HDv4l_cam::YUYV2GRAY(unsigned char * src,unsigned char * dst,int w,int h)
{
	for(int i=0;i<w*h;i++)
	{
    *(dst++) =*(src) ;
    src+=2;
	}
}


bool HDv4l_cam::Open()
{
	int ret;
/*	setCurChannum();
	if(ret < 0)
			return false;
*/
	static bool Once=true;
	if(Once)
	{
		Once=false;
	}
	init_cuda();
	ret = open_device();
	if(ret < 0)
		return false;

	ret = init_device();
	if(ret < 0)
		return false;


	run();
	return true;
}

void HDv4l_cam::run()
{
	start_capturing();


	bRun = true;
}

void HDv4l_cam::stop()
{
	stop_capturing();
	bRun = false;
}


int HDv4l_cam::open_device(void)
{
	struct stat st;

	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno));
			return FAILURE_DEVICEOPEN;
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		return FAILURE_DEVICEOPEN;
	}

	m_devFd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
	printf("opened_dev_name=%s\n",dev_name);
	if (-1 == m_devFd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
		return FAILURE_DEVICEOPEN;
	}
	return EXIT_SUCCESS;
}

 void   HDv4l_cam::init_cuda()
{
    /* Check unified memory support. */
    if (	 memType==MEMORY_LOCKED) {
        cudaDeviceProp devProp;
        cudaGetDeviceProperties (&devProp, 0);
        if (!devProp.managedMemory) {
            printf ("CUDA device does not support managed memory.\n");
            memType  = MEMORY_NORMAL;
        }
        cudaDeviceSynchronize ();
    }
}

int HDv4l_cam::init_device(void)
{
	struct v4l2_capability cap;
		struct v4l2_cropcap cropcap;
		struct v4l2_crop crop;
		struct v4l2_format fmt;
		unsigned int min;

		if (-1 == xioctl(m_devFd, VIDIOC_QUERYCAP, &cap)) {
			if (EINVAL == errno) {
				fprintf(stderr, "%s is no V4L2 device\n", dev_name);
				return FAILURE_DEVICEINIT;
			} else {
				errno_exit("HD_VIDIOC_QUERYCAP");
			}
		}

		if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
			fprintf(stderr, "%s is no video capture device\n", dev_name);
			return FAILURE_DEVICEINIT;
		}

		switch (io) {
		case IO_METHOD_READ:
			if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
				fprintf(stderr, "%s does not support read i/o\n", dev_name);
				return FAILURE_DEVICEINIT;
			}
			break;
		case IO_METHOD_MMAP:
		case IO_METHOD_USERPTR:
			if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
				fprintf(stderr, "%s does not support streaming i/o\n", dev_name);
				return FAILURE_DEVICEINIT;
			}
			break;
		}

		/* Select video input, video standard and tune here. */

		CLEAR(cropcap);

		cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (0 == xioctl(m_devFd, VIDIOC_CROPCAP, &cropcap)) {
			crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			crop.c = cropcap.defrect; /* reset to default */

			if (-1 == xioctl(m_devFd, VIDIOC_S_CROP, &crop)) {
			}
		} else {
			/* Errors ignored. */
		}

		CLEAR(fmt);

		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (force_format) {
			fprintf(stderr, "Set HDyuyv\r\n");
			fmt.fmt.pix.width       = imgwidth;
			fmt.fmt.pix.height      = imgheight;
			fmt.fmt.pix.pixelformat = imgformat;
			fmt.fmt.pix.field       = V4L2_FIELD_ANY;
			//fmt.fmt.pix.code=0;
			//printf("******width =%d height=%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);

			if (-1 == xioctl(m_devFd, VIDIOC_S_FMT, &fmt))
			{
				 errno_exit("HD_VIDIOC_S_FMT");
			}

			/* Note VIDIOC_S_FMT may change width and height. */
		} else {
			/* Preserve original settings as set by v4l2-ctl for example */
			if (-1 == xioctl(m_devFd, VIDIOC_G_FMT, &fmt))
				errno_exit("HD_VIDIOC_G_FMT");
		}


		/* Buggy driver paranoia. */
		min = fmt.fmt.pix.width * 2;
		if (fmt.fmt.pix.bytesperline < min)
			fmt.fmt.pix.bytesperline = min;
		min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
		if (fmt.fmt.pix.sizeimage < min)
			fmt.fmt.pix.sizeimage = min;

		switch (io) {
		case IO_METHOD_READ:
			init_read(fmt.fmt.pix.sizeimage);
			break;
		case IO_METHOD_MMAP:
			init_mmap();
			break;
		case IO_METHOD_USERPTR:
			init_userp(fmt.fmt.pix.sizeimage);
			break;
		}
		return EXIT_SUCCESS;
}




void HDv4l_cam::errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

int HDv4l_cam::xioctl(int fh, int request, void *arg)
{
	int ret;
	do {
		ret = ioctl(fh, request, arg);
	} while (-1 == ret && EINTR == errno);

	return ret;
}

void yuyv2gray(unsigned char* src,unsigned char* dst,int width,int height)
{
	for(int j = 0;j<height*width;j++)
	{
			dst[j] = src[2*j +1];
	}
	return ;
}

int HDv4l_cam::GetNowPicIdx(unsigned char *src)
{
	int picIdx=-1;
	picIdx=(int)*src;
	return picIdx;
}
int HDv4l_cam::ChangeIdx2chid(int idx)
{//0~9
	//int picidx=(GetNowPicIdx()+2);
	//return picidx;
	return 0;
}

void save_yuyv(char *filename,void *pic,int w,int h)
{
	FILE * fp;
	char buf[30];
	fp=fopen(filename,"w");
	fwrite(pic,w*h*2,1,fp);
	fclose(fp);
}
void save_rgb(char *filename,void *pic,int w,int h)
{
	Mat Pic(h,w,CV_8UC3,pic);
	imwrite(filename,Pic);
}

void save_single_pic(char *filename,void *pic,int w,int h)
{
	unsigned char dst[DEFAULT_IMAGE_WIDTH*DEFAULT_IMAGE_HEIGHT*3];
	Mat Src(h,w,CV_8UC2,pic);
	Mat Dst(h,w,CV_8UC3,dst);
	cvtColor(Src,Dst,CV_YUV2BGR_UYVY);
	imwrite(filename,Dst);
}
void save_yuyv2rgb(char *filename,void *pic,int w,int h)
{
	char bmp[1920*1080*4];
	Mat Pic(h,w,CV_8UC2,pic);
	Mat BMP(h,w,CV_8UC4,bmp);

}
void save_gray(char *filename,void *pic,int w,int h)
{
	Mat Pic(h,w,CV_8UC1,pic);
	imwrite(filename,Pic);
}

int HDv4l_cam::read_frame(int cap_chid)
{
	struct v4l2_buffer buf;
	int i=0;
	static int  count=0;
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;

		if (-1 == xioctl(m_devFd, VIDIOC_DQBUF, &buf)) {
					switch (errno) {
					case EAGAIN:
						return -1;
					case EIO:
					//	printf("EIO~~~~~~~~~~~");
						/* Could ignore EIO, see spec. */
						/* fall through */
					default:
						errno_exit("VIDIOC_DQBUF");
					}
				}
				assert(buf.index < n_buffers);
			 if (buffers[buf.index].start!=NULL)
			{
				 PROCESS_FRAME(cap_chid,(unsigned char *)buffers[buf.index].start, buf, imgformat);
#if USE_QUE
				 v4l2_buffer* ptemp=NULL;
				 int 	 chid =-1;
				 switch (cap_chid)
				 {
				 case TV_DEV_ID:
					 p_tv_buf=&buf;
					 ptemp=p_tv_buf;
					 chid=TV_DEV_ID;
					 break;
				 case HOT_DEV_ID:
					 p_hot_buf=&buf;
					 ptemp=p_hot_buf;
					 chid=HOT_DEV_ID;
					 break;
				 default :
					 break;
				 }

					if(Data2Queue(ptemp,0,0,chid))
					{
						if(getEmpty(&ptemp, chid))
						{
						}
					}
#endif
			}
	if (-1 ==xioctl(m_devFd, VIDIOC_QBUF, &buf)){
		fprintf(stderr, "VIDIOC_QBUF error %d, %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	return 0;
}

void HDv4l_cam::stop_capturing(void)
{
	enum v4l2_buf_type type;
		switch (io) {
	//	case IO_METHOD_READ:
	//		/* Nothing to do. */
	//		break;
		case IO_METHOD_MMAP:
		case IO_METHOD_USERPTR:
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == xioctl(m_devFd, VIDIOC_STREAMOFF, &type))
				errno_exit("VIDIOC_STREAMOFF");
			break;
		}
}

void HDv4l_cam::start_capturing(void)
{
	unsigned int i;
	enum v4l2_buf_type type;

	switch (io) {
//	case IO_METHOD_READ:
//		/* Nothing to do. */
//		break;
	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i) {
			struct v4l2_buffer buf;

			CLEAR(buf);
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;

			if (-1 == xioctl(m_devFd, VIDIOC_QBUF, &buf))
				errno_exit("HD_VIDIOC_QBUF");
		}
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == xioctl(m_devFd, VIDIOC_STREAMON, &type))
			errno_exit("HD_VIDIOC_STREAMON");
		break;
	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i) {
			struct v4l2_buffer buf;

			CLEAR(buf);
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_USERPTR;
			buf.index = i;
			buf.m.userptr = (unsigned long)buffers[i].start;
			buf.length = buffers[i].length;

			if (-1 == xioctl(m_devFd, VIDIOC_QBUF, &buf))
				errno_exit("HD_VIDIOC_QBUF");
		}
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == xioctl(m_devFd, VIDIOC_STREAMON, &type))
			errno_exit("HD_VIDIOC_STREAMON");
		break;
	default:
		break;
	}
}

void HDv4l_cam::uninit_device(void)
{
	unsigned int i;

	switch (io) {
	case IO_METHOD_READ:
		free(buffers[0].start);
		break;
	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i)
			if (-1 == munmap(buffers[i].start, buffers[i].length))
				errno_exit("munmap");
		break;
	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i){
			if(memType == MEMORY_NORMAL){
				free(buffers[i].start);
			}else{
				cudaFreeHost(buffers[i].start);
						}

		}
		break;
	default:
		break;
	}

	free(buffers);
}

void HDv4l_cam::init_read(unsigned int buffer_size)
{
	buffers = (struct buffer *)calloc(1, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	buffers[0].length = buffer_size;
	buffers[0].start = (struct buffer *) malloc(buffer_size);

	if (!buffers[0].start) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}
}

void HDv4l_cam::init_mmap(void)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = bufferCount;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(m_devFd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support memory mapping\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
		exit(EXIT_FAILURE);
	}

//	printf("%s qbuf cnt = %d\n", dev_name, req.count);

	buffers = (struct buffer *)calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = n_buffers;

		if (-1 == xioctl(m_devFd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start  =	mmap(NULL /* start anywhere */,
										buf.length,
										PROT_READ | PROT_WRITE /* required */,
										MAP_SHARED /* recommended */,
										m_devFd, buf.m.offset);
		memset(buffers[n_buffers].start,0x80,buf.length);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

void HDv4l_cam::init_userp(unsigned int buffer_size)
{
	struct v4l2_requestbuffers req;
	cudaError_t ret = cudaSuccess;
	unsigned int page_size;
	page_size=getpagesize();
	buffer_size=(buffer_size+page_size-1)&~(page_size-1);
	CLEAR(req);

	req.count  = bufferCount;//  different
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;

	if (-1 == xioctl(m_devFd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
					"user pointer i/o\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}
	if (req.count < 2) {
			fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
			exit(EXIT_FAILURE);
		}
//	printf("%s qbuf cnt = %d\n", dev_name, req.count);
	buffers = (struct buffer  *)calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		buffers[n_buffers].length = buffer_size;
		if(memType == MEMORY_NORMAL)
			buffers[n_buffers].start = memalign(page_size,buffer_size*2);
		else // MEMORY_LOCKED
			ret = cudaHostAlloc((void**)&buffers[n_buffers].start, buffer_size, cudaHostAllocDefault);
			//ret=cudaMallocManaged (&buffers[n_buffers].start, buffer_size, cudaMemAttachGlobal);
		assert(ret == cudaSuccess);
		//cudaFreeHost();

		if (!buffers[n_buffers].start) {
			fprintf(stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
		}
	}
}


void HDv4l_cam::close_device(void)
{
	if (-1 == close(m_devFd))
		errno_exit("close");

	m_devFd = -1;
}

void HDv4l_cam::mainloop(int cap_chid)
{
	fd_set fds;
	struct timeval tv;
	int ret;

		FD_ZERO(&fds);
		FD_SET(m_devFd, &fds);

		/* Timeout. */
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		ret = select(m_devFd + 1, &fds, NULL, NULL, &tv);

		if (-1 == ret)
		{
			if (EINTR == errno)
				return;

			errno_exit("select");
		}else if (0 == ret)
		{
			fprintf(stderr, "select timeout\n");
			return;
		}
			if (-1 == read_frame(cap_chid))  /* EAGAIN - continue select loop. */
				return;
}


//--------------the decorator cap class------------
HDAsyncVCap4::HDAsyncVCap4(auto_ptr<BaseVCap> coreCap,int cap_chid):
	m_core(coreCap),thread_state(THREAD_READY),sleepMs(DEFAULT_THREAD_SLEEP_MS)
{
	m_cap_chid=cap_chid;
	initLock();
	start_thread(capThread,this);
	cout<<" HDstart a capture thread"<<endl;
}
HDAsyncVCap4:: ~HDAsyncVCap4()
{
	Close();
}
bool HDAsyncVCap4::Open()
{
	if(thread_state != THREAD_READY)
		return false;//do not support reopen.
	bool isOpened = m_core->Open();
	if(isOpened){
		Start();
	}
	return isOpened;
}

void HDAsyncVCap4::Close()
{
	thread_state = THREAD_STOPPING;
	while( THREAD_IDLE != thread_state){
		usleep(100*1000);
	}
	m_core->Close();
}


void HDAsyncVCap4::Capture(char* ptr)
{
	lock_read(ptr);
}

void HDAsyncVCap4::SetDefaultImg(char *p)
{
	m_core->SetDefaultImg(p);
}

 void HDAsyncVCap4::SavePic(const char* name)
 {
	m_core->SavePic(name);
 }

void HDAsyncVCap4::Run()
{

	do{
		usleep(100*1000);
	}while(THREAD_READY == thread_state);
	//cap in background thread
	while(thread_state == THREAD_RUNNING)
	{
			HDv4l_cam * pcore = dynamic_cast<HDv4l_cam*>(m_core.get());
			if(pcore){
				pcore->mainloop(m_cap_chid);
			}
//		usleep(sleepMs*1000);
	}
	thread_state = THREAD_IDLE;
	destroyLock();

}

void* HDAsyncVCap4::capThread(void*p)
{
	HDAsyncVCap4 *thread = (HDAsyncVCap4*)p;
	thread->Run();
	return NULL;
}

void HDAsyncVCap4::lock_read(char *ptr)
{
	int pic_size=0;
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_rdlock(&rwlock);
	pic_size=DEFAULT_IMAGE_WIDTH*DEFAULT_IMAGE_HEIGHT*2;
	MEMCPY(ptr, pImg,pic_size);
	pthread_rwlock_unlock(&rwlock);
}

void HDAsyncVCap4::lock_write(char *ptr)
{
	int pic_size=0;
	char *pImg = m_core->GetDefaultImg();
	pthread_rwlock_wrlock(&rwlock);
	pic_size=DEFAULT_IMAGE_WIDTH*DEFAULT_IMAGE_HEIGHT*2;
	MEMCPY(pImg,ptr,pic_size);
	pthread_rwlock_unlock(&rwlock);
}
void  HDAsyncVCap4::Start()
{
	thread_state = THREAD_RUNNING;
}
void HDAsyncVCap4::destroyLock()
{
	pthread_rwlock_destroy(&rwlock);
	pthread_rwlockattr_destroy(&rwlockattr);
}

void HDAsyncVCap4::initLock()
{
	pthread_rwlockattr_init(&rwlockattr);
	pthread_rwlockattr_setpshared(&rwlockattr,2);
	pthread_rwlock_init(&rwlock,&rwlockattr);
}



bool HDv4l_cam::getEmpty(v4l2_buffer ** pYuvBuf, int chId)
{
#if 0
	int status=0;
	bool ret = true;
	while(1)
	{
		status = OSA_bufGetEmpty(&queue_TV_HOT->bufHndl[chId],&m_bufId[chId],0);
		if(status == 0)
		{
			*pYuvBuf = (v4l2_buffer*)queue_TV_HOT->bufHndl[chId].bufInfo[m_bufId[chId]].virtAddr;
			break;
		}else{
			if(!OSA_bufGetFull(&queue_TV_HOT->bufHndl[chId],&m_bufId[chId],OSA_TIMEOUT_FOREVER))
			{
				if(!OSA_bufPutEmpty(&queue_TV_HOT->bufHndl[chId],m_bufId[chId]))
				{
					;
				}
			}
		}
	}
	 return ret;
#endif
}

bool HDv4l_cam::Data2Queue(v4l2_buffer *pYuvBuf,int width,int height,int chId)
{
#if USE_QUE
	int status;
	if(chId>=QUE_CHID_COUNT)//if(chId >= CAM_COUNT+1)
		return false;
	queue_TV_HOT->bufHndl[chId].bufInfo[m_bufId[chId]].width=width;
	queue_TV_HOT->bufHndl[chId].bufInfo[m_bufId[chId]].height=height;
	queue_TV_HOT->bufHndl[chId].bufInfo[m_bufId[chId]].strid=width;
	OSA_bufPutFull(&queue_TV_HOT->bufHndl[chId],m_bufId[chId]);
	return true;
#endif
}

void  HDv4l_cam::start_queue()
{
#if USE_QUE
	getEmpty(&p_tv_buf,TV_DEV_ID);
	getEmpty(&p_hot_buf,HOT_DEV_ID );
#endif
}

