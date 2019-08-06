/*
 * cucolor.hpp
 *
 *  Created on: Aug 2, 2019
 *      Author: wj
 */

#ifndef CUCOLOR_HPP_
#define CUCOLOR_HPP_


void yuyv2BGRI420(cv::Mat& yuyv,cv::Mat& BGR,cv::Mat& yuvplan);

void yuyv2BGRI420_MD(cv::Mat& yuyv,cv::Mat& BGR,cv::Mat& yuvplan);

void yuyv2BGR(cv::Mat& yuyv,cv::Mat& BGR);

void cuinit();
#endif /* CUCOLOR_HPP_ */
