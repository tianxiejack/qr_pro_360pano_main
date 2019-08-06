#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <npp.h>
//#include <helper_cuda.h>
#include <assert.h>
#include <cuda_runtime.h>

#define DESCALE(x, n)    (((x) + (1 << ((n)-1)))>>(n))
#define COEFFS_0 		(22987)
#define COEFFS_1 		(-11698)
#define COEFFS_2 		(-5636)
#define COEFFS_3 		(29049)
#define clip(minv, maxv, value)  ((value)<minv) ? minv : (((value)>maxv) ? maxv : (value))

__global__ void kernel_yuyv2bgr_(
	unsigned char *dst, const unsigned char *src,
	int width, int height)
{
	int x = blockDim.x * blockIdx.x + threadIdx.x;
	int y = blockDim.y * blockIdx.y + threadIdx.y;

	if(x >= (width>>1) || y >= height)
		return;

	{
		int Y1, Y2, U, V;
		int r, g, b;

		int si = y*width*2 + x*4;
		int di = y*width*3 + x*6;

		Y1 =  src[si+0];
		U = src[si+1];
		Y2 =  src[si+2];
		V = src[si+3];


		b = DESCALE((U - 128)*COEFFS_3, 14);
		g = DESCALE((U - 128)*COEFFS_2 + (V - 128)*COEFFS_1, 14);
		r = DESCALE((V - 128)*COEFFS_0, 14);

		dst[di+0] = clip(0, 255, Y1 + b);//B
		dst[di+1] = clip(0, 255, Y1 + g);//G
		dst[di+2] = clip(0, 255, Y1 + r);//R
		dst[di+3] = clip(0, 255, Y2 + b);//B
		dst[di+4] = clip(0, 255, Y2 + g);//G
		dst[di+5] = clip(0, 255, Y2 + r);//R
	}
}

extern "C" int yuyv2bgr(
	unsigned char *dst, const unsigned char *src,
	int width, int height,cudaStream_t stream)
{
	//dim3 block((width/2+15)/16, (height+63)/64);
	//dim3 thread(16, 64);
	dim3 block((width/2+31)/32,(height+31)/32);
	dim3 thread(32, 32);
	//dim3 block((width/2+127)/128,(height+127)/128);
	//dim3 thread(128, 128);

	//kernel_uyvy2bgr_and_sphere_tp_erect<<<block, thread>>>(dst, src, width, height);
	kernel_yuyv2bgr_<<<block, thread, 0,stream>>>(dst, src, width, height);

	return 0;
}

__global__ void kernel_yuyv2I420(
	unsigned char *dsty, unsigned char *dstu, unsigned char *dstv, const unsigned char *src,
	int width, int height)
{
	int x = blockDim.x * blockIdx.x + threadIdx.x;
	int y = blockDim.y * blockIdx.y + threadIdx.y;
	int uvwidth = (width>>1);

	if(x >= (width>>1) || y >= height)
		return;

	dsty[y*width + 2*x] 		= 		src[y*width*2 + x*4];
	dstu[y*uvwidth + x] 	= 		src[y*width*2 + x*4+1];
	dsty[y*width + 2*x+1] = 		src[y*width*2 + x*4+2];
	dstv[y*uvwidth + x] 		= 		src[y*width*2 + x*4+3];
}

extern "C" int yuyv2yuvI420_(
	unsigned char *dsty, unsigned char *dstu,unsigned char *dstv,const unsigned char *src,
	int width, int height,cudaStream_t stream)
{
	dim3 block((width/2+31)/32,(height+31)/32);
	dim3 thread(32, 32);

	kernel_yuyv2I420<<<block, thread, 0,stream>>>(dsty,dstu,dstv, src, width, height);

	return 0;
}
__global__ void kernel_yuyv2bgr_I420(
	unsigned char *dstbgr, unsigned char *dsty, unsigned char *dstu,unsigned char *dstv,const unsigned char *src,
	int width, int height)
{
	int x = blockDim.x * blockIdx.x + threadIdx.x;
	int y = blockDim.y * blockIdx.y + threadIdx.y;

	if(x >= (width>>2) || y >= height)
		return;

	{
		int Y1, Y2, Y3, Y4, U1, U2, V1, V2 ;
		int r1, g1, b1, r2, g2, b2;

		int si = y*width*2 + x*8;
		int di = y*width*3 + x*12;

		Y1 =  src[si+0];
		U1 = src[si+1];
		Y2 =  src[si+2];
		V1= src[si+3];
		Y3 =  src[si+4];
		U2 = src[si+5];
		Y4 =  src[si+6];
		V2 = src[si+7];

		b1 = DESCALE((U1 - 128)*COEFFS_3, 14);
		g1 = DESCALE((U1 - 128)*COEFFS_2 + (V1 - 128)*COEFFS_1, 14);
		r1 = DESCALE((V1 - 128)*COEFFS_0, 14);

		b2 = DESCALE((U2 - 128)*COEFFS_3, 14);
		g2 = DESCALE((U2 - 128)*COEFFS_2 + (V2 - 128)*COEFFS_1, 14);
		r2 = DESCALE((V2 - 128)*COEFFS_0, 14);

		dstbgr[di+0] = clip(0, 255, Y1 + b1);//B
		dstbgr[di+1] = clip(0, 255, Y1 + g1);//G
		dstbgr[di+2] = clip(0, 255, Y1 + r1);//R
		dstbgr[di+3] = clip(0, 255, Y2 + b1);//B
		dstbgr[di+4] = clip(0, 255, Y2 + g1);//G
		dstbgr[di+5] = clip(0, 255, Y2 + r1);//R

		dstbgr[di+6] = clip(0, 255, Y3 + b2);//B
		dstbgr[di+7] = clip(0, 255, Y3 + g2);//G
		dstbgr[di+8] = clip(0, 255, Y3 + r2);//R
		dstbgr[di+9] = clip(0, 255, Y4 + b2);//B
		dstbgr[di+10] = clip(0, 255, Y4 + g2);//G
		dstbgr[di+11] = clip(0, 255, Y4 + r2);//R

		int dstystride = y*width;
		int dstuvstride = dstystride>>1;

		dsty[dstystride + 4*x] 		= 		Y1;
		dsty[dstystride + 4*x+1] 	= 		Y2;
		dsty[dstystride + 4*x+2] 	= 		Y3;
		dsty[dstystride + 4*x+3] 	= 		Y4;
		dstu[dstuvstride + x] 			= 		(U1+U2)*0.5;
		dstv[dstuvstride + x] 			= 		(V1+V2)*0.5;
	}
}

extern "C" int yuyv2yuvBGRI420_(
		unsigned char *dstrgb, unsigned char *dsty, unsigned char *dstu,unsigned char *dstv,const unsigned char *src,
	int width, int height,cudaStream_t stream)
{
	dim3 block((width/4+31)/32,(height+31)/32);
	dim3 thread(32, 32);

	kernel_yuyv2bgr_I420<<<block, thread, 0,stream>>>(dstrgb, dsty,dstu,dstv, src, width, height);

	return 0;
}
cudaStream_t streamBGR[4];
unsigned char *tempRGB;
unsigned char *tempYUYV;
unsigned char *tempY;
unsigned char *tempU;
unsigned char *tempV;
unsigned char *pRGALockMem = NULL;
void cuinit()
{
	int w=1920;
	int h=1080;
	for(int i=0; i<4; i++)
		cudaStreamCreate(&streamBGR[i]);
	cudaMalloc(&tempRGB,w*h*3);
	cudaMalloc(&tempYUYV,w*h*2);
	cudaMalloc(&tempY,w*h);
	cudaMalloc(&tempU,w*h/4);
	cudaMalloc(&tempV,w*h/4);

	if(pRGALockMem == NULL){
		int ret = cudaHostAlloc((void**)&pRGALockMem, w*h*3, cudaHostAllocDefault);
		assert(ret == cudaSuccess);
	}
}

void yuyv2BGRI420(cv::Mat& yuyv,cv::Mat& BGR,cv::Mat& yuvplan)
{
	//BGR.create(yuyv.rows,yuyv.cols,CV_8UC3);
	yuvplan.create(yuyv.rows*3/2,yuyv.cols,CV_8UC1);

	cudaMemcpyAsync(tempYUYV, yuyv.data, yuyv.cols*yuyv.rows*2, cudaMemcpyHostToDevice,0);
	yuyv2bgr(tempRGB,tempYUYV,yuyv.cols,yuyv.rows,0);
	yuyv2yuvI420_(tempY,tempU,tempV,tempYUYV,yuyv.cols,yuyv.rows,0);
//	cudaStreamSynchronize(streamBGR);
//	cudaDeviceSynchronize();
	cudaMemcpy(BGR.data,tempRGB,yuyv.cols*yuyv.rows*3,cudaMemcpyDeviceToHost);
//	cudaDeviceSynchronize();
	cudaMemcpy(yuvplan.data,tempY,yuyv.cols*yuyv.rows,cudaMemcpyDeviceToHost);
	cudaMemcpy(yuvplan.data+yuyv.cols*yuyv.rows,tempU,yuyv.cols*yuyv.rows/4,cudaMemcpyDeviceToHost);
	cudaMemcpy(yuvplan.data+yuyv.cols*yuyv.rows*5/4,tempV,yuyv.cols*yuyv.rows/4,cudaMemcpyDeviceToHost);
//	cudaStreamSynchronize(NULL);
	//cudaStreamDestroy(streamBGR);
	//cudaStreamDestroy(streamYUV);
}

void yuyv2BGRI420_MD(cv::Mat& yuyv,cv::Mat& BGR,cv::Mat& yuvplan)
{
	//BGR.create(yuyv.rows,yuyv.cols,CV_8UC3);
	yuvplan.create(yuyv.rows*3/2,yuyv.cols,CV_8UC1);

	int byteCount =  yuyv.cols*yuyv.rows*2;
	cudaMemcpyAsync(tempYUYV + (byteCount>>2)*0, yuyv.data + (byteCount>>2)*0, (byteCount>>2), cudaMemcpyHostToDevice, streamBGR[0]);
	cudaMemcpyAsync(tempYUYV + (byteCount>>2)*1, yuyv.data + (byteCount>>2)*1, (byteCount>>2), cudaMemcpyHostToDevice, streamBGR[1]);
	cudaMemcpyAsync(tempYUYV + (byteCount>>2)*2, yuyv.data + (byteCount>>2)*2, (byteCount>>2), cudaMemcpyHostToDevice, streamBGR[2]);
	cudaMemcpyAsync(tempYUYV + (byteCount>>2)*3, yuyv.data + (byteCount>>2)*3, (byteCount>>2), cudaMemcpyHostToDevice, streamBGR[3]);

	int byteCount_rgb = yuyv.cols*yuyv.rows*3;
	int byteCount_y = yuyv.cols*yuyv.rows;
	int byteCount_uv = (byteCount_y>>1);
	yuyv2yuvBGRI420_(tempRGB + (byteCount_rgb>>2)*0,
			tempY+(byteCount_y>>2)*0,
			tempU+(byteCount_uv>>2)*0,
			tempV+(byteCount_uv>>2)*0,
			tempYUYV + (byteCount>>2)*0,
			yuyv.cols, (yuyv.rows>>2), streamBGR[0]);
	yuyv2yuvBGRI420_(tempRGB + (byteCount_rgb>>2)*1,
				tempY+(byteCount_y>>2)*1,
				tempU+(byteCount_uv>>2)*1,
				tempV+(byteCount_uv>>2)*1,
				tempYUYV + (byteCount>>2)*1,
				yuyv.cols, (yuyv.rows>>2), streamBGR[1]);
	yuyv2yuvBGRI420_(tempRGB + (byteCount_rgb>>2)*2,
				tempY+(byteCount_y>>2)*2,
				tempU+(byteCount_uv>>2)*2,
				tempV+(byteCount_uv>>2)*2,
				tempYUYV + (byteCount>>2)*2,
				yuyv.cols, (yuyv.rows>>2), streamBGR[2]);
	yuyv2yuvBGRI420_(tempRGB + (byteCount_rgb>>2)*3,
				tempY+(byteCount_y>>2)*3,
				tempU+(byteCount_uv>>2)*3,
				tempV+(byteCount_uv>>2)*3,
				tempYUYV + (byteCount>>2)*3,
				yuyv.cols, (yuyv.rows>>2), streamBGR[3]);

	cudaMemcpyAsync(BGR.data + (byteCount_rgb>>2)*0, tempRGB + (byteCount_rgb>>2)*0, (byteCount_rgb>>2), cudaMemcpyDeviceToHost, streamBGR[0]);
	cudaMemcpyAsync(BGR.data + (byteCount_rgb>>2)*1, tempRGB + (byteCount_rgb>>2)*1, (byteCount_rgb>>2), cudaMemcpyDeviceToHost, streamBGR[1]);
	cudaMemcpyAsync(BGR.data + (byteCount_rgb>>2)*2, tempRGB + (byteCount_rgb>>2)*2, (byteCount_rgb>>2), cudaMemcpyDeviceToHost, streamBGR[2]);
	cudaMemcpyAsync(BGR.data + (byteCount_rgb>>2)*3, tempRGB + (byteCount_rgb>>2)*3, (byteCount_rgb>>2), cudaMemcpyDeviceToHost, streamBGR[3]);

	cudaMemcpyAsync(yuvplan.data + (byteCount_y>>2)*0, tempY + (byteCount_y>>2)*0, (byteCount_y>>2), cudaMemcpyDeviceToHost, streamBGR[0]);
	cudaMemcpyAsync(yuvplan.data + (byteCount_y>>2)*1, tempY + (byteCount_y>>2)*1, (byteCount_y>>2), cudaMemcpyDeviceToHost, streamBGR[1]);
	cudaMemcpyAsync(yuvplan.data + (byteCount_y>>2)*2, tempY + (byteCount_y>>2)*2, (byteCount_y>>2), cudaMemcpyDeviceToHost, streamBGR[2]);
	cudaMemcpyAsync(yuvplan.data + (byteCount_y>>2)*3, tempY + (byteCount_y>>2)*3, (byteCount_y>>2), cudaMemcpyDeviceToHost, streamBGR[3]);

	unsigned char *pu = yuvplan.data+yuyv.cols*yuyv.rows;
	cudaMemcpyAsync(pu + (byteCount_uv>>2)*0, tempU + (byteCount_uv>>2)*0, (byteCount_uv>>2), cudaMemcpyDeviceToHost, streamBGR[0]);
	cudaMemcpyAsync(pu + (byteCount_uv>>2)*1, tempU + (byteCount_uv>>2)*1, (byteCount_uv>>2), cudaMemcpyDeviceToHost, streamBGR[1]);
	cudaMemcpyAsync(pu + (byteCount_uv>>2)*2, tempU + (byteCount_uv>>2)*2, (byteCount_uv>>2), cudaMemcpyDeviceToHost, streamBGR[2]);
	cudaMemcpyAsync(pu + (byteCount_uv>>2)*3, tempU + (byteCount_uv>>2)*3, (byteCount_uv>>2), cudaMemcpyDeviceToHost, streamBGR[3]);

	unsigned char *pv = yuvplan.data+yuyv.cols*yuyv.rows*5/4;
	cudaMemcpyAsync(pv + (byteCount_uv>>2)*0, tempV + (byteCount_uv>>2)*0, (byteCount_uv>>2), cudaMemcpyDeviceToHost, streamBGR[0]);
	cudaMemcpyAsync(pv + (byteCount_uv>>2)*1, tempV + (byteCount_uv>>2)*1, (byteCount_uv>>2), cudaMemcpyDeviceToHost, streamBGR[1]);
	cudaMemcpyAsync(pv + (byteCount_uv>>2)*2, tempV + (byteCount_uv>>2)*2, (byteCount_uv>>2), cudaMemcpyDeviceToHost, streamBGR[2]);
	cudaMemcpyAsync(pv + (byteCount_uv>>2)*3, tempV + (byteCount_uv>>2)*3, (byteCount_uv>>2), cudaMemcpyDeviceToHost, streamBGR[3]);
}

void yuyv2BGR(cv::Mat& yuyv,cv::Mat& BGR)
{
	int byteCount =  yuyv.cols*yuyv.rows*2;
	cudaMemcpyAsync(tempYUYV + (byteCount>>2)*0, yuyv.data + (byteCount>>2)*0, (byteCount>>2), cudaMemcpyHostToDevice, streamBGR[0]);
	cudaMemcpyAsync(tempYUYV + (byteCount>>2)*1, yuyv.data + (byteCount>>2)*1, (byteCount>>2), cudaMemcpyHostToDevice, streamBGR[1]);
	cudaMemcpyAsync(tempYUYV + (byteCount>>2)*2, yuyv.data + (byteCount>>2)*2, (byteCount>>2), cudaMemcpyHostToDevice, streamBGR[2]);
	cudaMemcpyAsync(tempYUYV + (byteCount>>2)*3, yuyv.data + (byteCount>>2)*3, (byteCount>>2), cudaMemcpyHostToDevice, streamBGR[3]);

	int byteCount_rgb = yuyv.cols*yuyv.rows*3;

	yuyv2bgr(tempRGB + (byteCount_rgb>>2)*0,
			tempYUYV + (byteCount>>2)*0,
			yuyv.cols, (yuyv.rows>>2), streamBGR[0]);
	yuyv2bgr(tempRGB + (byteCount_rgb>>2)*1,
				tempYUYV + (byteCount>>2)*1,
				yuyv.cols, (yuyv.rows>>2), streamBGR[1]);
	yuyv2bgr(tempRGB + (byteCount_rgb>>2)*2,
				tempYUYV + (byteCount>>2)*2,
				yuyv.cols, (yuyv.rows>>2), streamBGR[2]);
	yuyv2bgr(tempRGB + (byteCount_rgb>>2)*3,
				tempYUYV + (byteCount>>2)*3,
				yuyv.cols, (yuyv.rows>>2), streamBGR[3]);

	cudaMemcpyAsync(pRGALockMem + (byteCount_rgb>>2)*0, tempRGB + (byteCount_rgb>>2)*0, (byteCount_rgb>>2), cudaMemcpyDeviceToHost, streamBGR[0]);
	cudaMemcpyAsync(pRGALockMem + (byteCount_rgb>>2)*1, tempRGB + (byteCount_rgb>>2)*1, (byteCount_rgb>>2), cudaMemcpyDeviceToHost, streamBGR[1]);
	cudaMemcpyAsync(pRGALockMem + (byteCount_rgb>>2)*2, tempRGB + (byteCount_rgb>>2)*2, (byteCount_rgb>>2), cudaMemcpyDeviceToHost, streamBGR[2]);
	cudaMemcpyAsync(pRGALockMem + (byteCount_rgb>>2)*3, tempRGB + (byteCount_rgb>>2)*3, (byteCount_rgb>>2), cudaMemcpyDeviceToHost, streamBGR[3]);

	memcpy(BGR.data, pRGALockMem, BGR.rows*BGR.cols*3);
}

