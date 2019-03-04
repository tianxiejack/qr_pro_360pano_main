/*
 * testInterfaceProcessImageBehavior.h
 *
 *  Created on: 2018年5月21日
 *      Author: ubuntu
 */

#ifndef	_TEST_INTERFACE_PROCESS_IMAGE_BEHAVIOR_H
#define	_TEST_INTERFACE_PROCESS_IMAGE_BEHAVIOR_H

#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "InterfaceProcessImageBehavior.h"

class ProcessYUVBehavior:public InterfaceProcessImageBehavior
{
	public:
		ProcessYUVBehavior();
		virtual ~ProcessYUVBehavior();
		virtual void processImage_setInfo(int height, int width );
		virtual void processImage(unsigned char * data, unsigned int length);
		virtual void doProcessImage(cv::Mat &rgba) = 0;
	private:
		int m_width, m_height;
};

class ProcessYUVShowBehavior:public ProcessYUVBehavior
{
	public:
		ProcessYUVShowBehavior(){ };
		virtual ~ProcessYUVShowBehavior(){ };
		virtual void doProcessImage(cv::Mat & rgba);

};

class ProcessYUVWriteBehavior:public ProcessYUVBehavior
{
	public:
	    ProcessYUVWriteBehavior(){ };
		virtual ~ProcessYUVWriteBehavior(){ };
		virtual void doProcessImage(cv::Mat & rgba);

};

class ProcessQueueBehavior:public ProcessYUVBehavior
{
	public:
		ProcessQueueBehavior(int dev_id);
		virtual ~ProcessQueueBehavior(){ };
		virtual void doProcessImage(cv::Mat & rgba);
	private:
		void init_buffer();
		void start_queue();
		bool Data2Queue(unsigned char *pYuvBuf,int width,int height,int chId);
		bool getEmpty(unsigned char** pYuvBuf, int chId);
		int dev_id;
};
#endif

