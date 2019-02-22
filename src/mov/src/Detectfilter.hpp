/*
 * Detectfilter.hpp
 *
 *  Created on: 2018年11月21日
 *      Author: wj
 */

#ifndef DETECTFILTER_HPP_
#define DETECTFILTER_HPP_
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include "infoHead.h"
class DetectFilter{
public:
	DetectFilter();
	~DetectFilter();
	void create();
	void backgroundupdate(cv::Mat src);
	void process(cv::Mat src,std::vector<TRK_RECT_INFO> & target);
	void destroy();
	double  Templatematch(const cv::Mat & src, const cv::Mat & dst);
public:
	void enablebackgroundflag(){backgroundflag=0;};
	
public:
	cv::Mat background;
	int backgroundflag;
};



#endif /* DETECTFILTER_HPP_ */
