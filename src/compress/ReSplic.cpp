/*
 * ReSplic.cpp
 *
 *  Created on: 2018年12月26日
 *      Author: fsmdn121
 */
#include "ReSplic.h"
#include "stdio.h"


 void SaveVideoByGST::init()
 {

	// char rgba[8]="RGBA";
	char rgba[8]="I420";
#if 1
	 FLIP_METHOD flip=FLIP_METHOD_ROTATE_180;
	 record_handle = CabinInit(1920,1080,30,rgba,1,flip);
#endif
 }
 void SaveVideoByGST::SetMTime(unsigned long Tnm)
 {
	 SetTime(Tnm,record_handle);
 };
 void SaveVideoByGST::SetEos()
 {
 	Seteos(record_handle);
 };
 void SaveVideoByGST::EndOfStream()
 {
#if 0
	 EOS(record_handle);
#endif
	 CabinUninit(record_handle);
 }
 void SaveVideoByGST::SaveAsMp4(Mat *pmatSrc)
 {
#if 0
	 static bool Once=true;
	 unsigned char *resized_sdi_buffer_data=NULL;
	 unsigned char *tmp=NULL;

	 if(Once)
	 {
		 Once=false;
		resized_sdi_buffer_data=(unsigned char *)malloc(1920*1080*4);
		tmp=(unsigned char *)malloc(1920*1080*3);
	 }
	 Mat Tmp(1080,1920,CV_8UC4,tmp);
	 Mat Dst(1080,1920,CV_8UC4,resized_sdi_buffer_data);
	 Mat dstsize=Mat::zeros(1080,1920,CV_8UC3);
	 resize(*pmatSrc,Tmp,dstsize.size());
	 cvtColor(Tmp,Dst,CV_RGB2RGBA);

	 static int a=0;
	 static int count =0;
	 if(a++%20==1 && count<=5)
	 {
		 count ++;
		 char buf[64]={0};
		 sprintf(buf,"%d.bmp",a);
//		 imwrite(buf,Dst);
	 }
#endif
 	Mat Tmp(1080,1920,CV_8UC4);
	//double exec_time = (double)getTickCount();
	
 	//cvtColor(*pmatSrc, Tmp,CV_BGR2RGBA);
	cvtColor(*pmatSrc, Tmp,CV_BGR2YUV_I420);

	 //Mat Dst(1080,1920,CV_8UC4);
	 //flip(Tmp,Dst,0);
#if 1
	 CabinPushData(record_handle, (char *)Tmp.data , 1920*1080*4);
	 //CabinPushData(record_handle, (char *)pmatSrc->data , 1920*1080*3/2);
#endif
	//exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
     	//printf("%s:%f\n",__func__, exec_time);
 }




