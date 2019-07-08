/*
 * Compress.hpp
 *
 *  Created on: 2018年11月15日
 *      Author: wj
 */

#ifndef GSTREAMER_HPP_
#define GSTREAMER_HPP_

#include <opencv2/opencv.hpp>
//#include "osa_buf.h"
//#include "osa_sem.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/gstclock.h>
//#include "MP4cabinCapture.h"
using namespace cv;
typedef struct{
	double gyrox;
	double gyroy;
	double gyroz;
	int event;

}Privatedata;

#endif /* COMPRESS_HPP_ */
