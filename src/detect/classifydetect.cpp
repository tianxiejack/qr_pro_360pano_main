#include"classifydetect.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include<opencv2/ml/ml.hpp>
#include <iostream>
#include<string.h>
#include "stdio.h"
#include "osa.h"
#include "config.h"
#include <cctype>
#include <iterator>

using namespace std;
using namespace cv;
const string cascade_names = "cars.xml";

#define HOGSVM (0)

#define FROMESVM (1)

#define GPUHOG (1)

#define XML (1)

ClassifyDetect::ClassifyDetect()
{


}
ClassifyDetect::~ClassifyDetect()
{


}


void ClassifyDetect::get_svm_detector(const SVM & svm, vector< float > & hog_detector)
{
	/*
	// get the support vectors
	Mat sv = svm.getSupportVectors();
	const int sv_total = sv.rows;
	// get the decision function
	Mat alpha, svidx;
	double rho = svm.getDecisionFunction(0, alpha, svidx);

	CV_Assert(alpha.total() == 1 && svidx.total() == 1 && sv_total == 1);
	CV_Assert((alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
		(alpha.type() == CV_32F && alpha.at<float>(0) == 1.f));
	CV_Assert(sv.type() == CV_32F);
	hog_detector.clear();

	hog_detector.resize(sv.cols + 1);
	memcpy(&hog_detector[0], sv.ptr(), sv.cols*sizeof(hog_detector[0]));
	hog_detector[sv.cols] = (float)-rho;
	*/
}
void ClassifyDetect::movdetectcreate(LPNOTIFYFUNCCD fun, void *context)
{
		callback=fun;
		cdcontext=context;
}
int  ClassifyDetect::create()
{


if(HOGSVM)
{
	vector<float> myDetector;
	if(FROMESVM)
		{
			if(XML)
				svm.load("svm.xml");
			else
				svm.load("svm.yml");
			DescriptorDim = svm.get_var_count();
			int supportVectorNum = svm.get_support_vector_count();
			//cout<<"\u652f\u6301\u5411\u91cf\u4e2a\u6570\uff1a"<<supportVectorNum<<endl;

			Mat alphaMat = Mat::zeros(1, supportVectorNum, CV_32FC1);
			Mat supportVectorMat = Mat::zeros(supportVectorNum, DescriptorDim, CV_32FC1);
			Mat resultMat = Mat::zeros(1, DescriptorDim, CV_32FC1);
			for(int i=0; i<supportVectorNum; i++)
			{
				const float * pSVData = svm.get_support_vector(i);
				for(int j=0; j<DescriptorDim; j++)
				{
					
					supportVectorMat.at<float>(i,j) = pSVData[j];
				}
			}

			
			double * pAlphaData = svm.get_alpha_vector();
			for(int i=0; i<supportVectorNum; i++)
			{
				alphaMat.at<float>(0,i) = pAlphaData[i];
			}

			resultMat = -1 * alphaMat * supportVectorMat;

			
			for(int i=0; i<DescriptorDim; i++)
			{
				myDetector.push_back(resultMat.at<float>(0,i));
			}

			myDetector.push_back(svm.get_rho());
			//cout<<"\u68c0\u6d4b\u5b50\u7ef4\u6570\uff1a"<<myDetector.size()<<endl;
		}
	/*
	if(GPUHOG)
		{
		gpu_hog=gpu::HOGDescriptor(Size(64, 128), Size(16, 16), Size(8, 8), Size(8, 8),9);
		gpu_hog.setSVMDetector(myDetector);
		}
	else
	*/
	myHOG.setSVMDetector(myDetector);

}
else
	 if( !cascade.load( cascade_names ) ){ printf("--(!)Error loading\n"); return -1; };
	

};

void ClassifyDetect::getMoveTarget(vector<Rect> &objects,int chId)
{
	objects.clear();
	for(int i=0;i<objectsrect.size();i++)
		{
			;
			objects.push_back(objectsrect[i]);
		}
	
}
void ClassifyDetect::detect(Mat frame,int chid)
{
	//std::vector<Rect> faces;
	objectsrect.clear();
	//printf("*************%s*****************\n",__func__);
	//return ;
	double exec_time = (double)getTickCount();
	/*	
		
	if(HOGSVM)
		{
			if(GPUHOG)
				{
					gpu_img.upload(frame);
					printf("load frame ok\n");
					gpu_hog.detectMultiScale(gpu_img, objectsrect, 0, Size(8,8), Size(0,0), 1.05, 2);
					printf("detect frame ok\n");
				}
			else
			myHOG.detectMultiScale(frame, objectsrect, 0, Size(8,8), Size(32,32), 1.05, 2);
		}
	else
		cascade.detectMultiScale( frame, objectsrect, 1.1, 1, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
		
	if(callback!=NULL)
		callback(cdcontext,chid);

	exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
	printf("ClassifyDetect time =%f\n",exec_time);
	*/
}


