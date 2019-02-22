/*
 * lkdetect.hpp
 *
 *  Created on: 2018年10月30日
 *      Author: wj
 */

#ifndef LKDETECT_HPP_
#define LKDETECT_HPP_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include"osa.h"
#include"osa_sem.h"
#include"osa_thr.h"
#include"osa_mutex.h"
#include"osa_buf.h"
#include"config.h"
#include <optical_flow_calculator.h>
#include <optical_flow_visualizer.h>
using namespace cv;
typedef void ( *LPNOTIFYFUNClk)(void *context, int chId);
class LKmove{

public:
	LKmove();
	~LKmove();
	void lkmovdetectreset();
	void lkmovdetectcreate(LPNOTIFYFUNClk fun, void *context);
	void lkmovdetect(Mat src,int chid);
	void lkmovdetectddestory();
	void lkmovdetectgetrect(Mat &src,vector<Rect> &objects);
	void runOpticalFlow(const cv::Mat &image1, const cv::Mat &image2, cv::Mat &optical_flow_vectors);
	void getMoveTarget(vector<Rect> &objects,int chId);

	void lkmovdetectpreprocess(Mat &src,Mat &dst,int chid);
	int OpticalFlowprocess(const cv::Mat &image1, const cv::Mat &image2, cv::Mat &dst);
	void runOpticalFlowtest( cv::Mat &image1,  cv::Mat &image2);
	cv::Rect findRectangle(cv::Mat Sub,cv::Point2f &r1, cv::Point2f &r2);
	LPNOTIFYFUNClk callback;
	void * lkcontext;

public:
	 OpticalFlowVisualizer ofv_;
	 OpticalFlowCalculator ofc_;
	 vector<Rect> objectsrect;
	 int pixel_step_;
	  double min_vector_size_;
	  int backgroundmov[MOVELKBLOCKNUM];
	  Mat backgroundmovmat[MOVELKBLOCKNUM];
	  Mat background;
	 
};


#endif /* LKDETECT_HPP_ */
