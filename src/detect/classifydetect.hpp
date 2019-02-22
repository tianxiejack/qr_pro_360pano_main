/*
 * classifydetect.hpp
 *
 *  Created on: 2018年11月28日
 *      Author: wj
 */

#ifndef CLASSIFYDETECT_HPP_
#define CLASSIFYDETECT_HPP_
#include <opencv2/opencv.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include<opencv2/ml/ml.hpp>
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/gpu/gpu.hpp>
typedef void ( *LPNOTIFYFUNCCD)(void *context, int chId);
using namespace cv;
class MySVM: public CvSVM
{
public:

    double * get_alpha_vector()
    {
        return this->decision_func->alpha;
    }


    float get_rho()
    {
        return this->decision_func->rho;
    }
    
};
class ClassifyDetect
{

public:
	ClassifyDetect();
	~ClassifyDetect();
	void movdetectcreate(LPNOTIFYFUNCCD fun, void *context);
	LPNOTIFYFUNCCD callback;
	void * cdcontext;
public:
	int create();
	void destory();
	void detect(cv::Mat frame,int chid);
	void getMoveTarget(std::vector<cv::Rect> &objects,int chId);
	void get_svm_detector(const cv::SVM& svm, std::vector< float > & hog_detector);
public:
	cv::CascadeClassifier cascade;
	std::vector<cv::Rect> objectsrect;

	//gpu::GpuMat gpu_img;
	//gpu::HOGDescriptor gpu_hog;
	MySVM svm;
	HOGDescriptor myHOG;
	int DescriptorDim;
};


#endif /* CLASSIFYDETECT_HPP_ */
