/*
 * Gyroprocess.hpp
 *
 *  Created on: 2018年9月25日
 *      Author: wj
 */

#ifndef GYROPROCESS_HPP_
#define GYROPROCESS_HPP_
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>

#include "gyro.h"
using namespace cv;
int getGyroprocess(Mat& frame,GYRO_DATA_T *gyro);
	enum PROTOCOL_HEAD {
		HEAD1_FF_NEED = 0xFF,
		HEAD1_00_NEED = 0x00,
		HEAD1_55_NEED = 0x55
	};
void getEuler(double *roll,double *pitch,double *yaw);
void setyawbase(double base);
void setgyrozero(double zero);
void kalmanfilterinit();
int getoddenv();
void Gyrorest();
void setgyrostart(int gyro);
#endif /* GYROPROCESS_HPP_ */
