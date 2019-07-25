/*
 * Detector.hpp
 *
 *  Created on: Feb 18, 2019
 *      Author: wj
 */

#ifndef DETECTOR_HPP_
#define DETECTOR_HPP_

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define DETECTOR_PURE_VIRTUAL =0

struct BoundingBox : public cv::Rect {
		  BoundingBox(){}
		  BoundingBox(cv::Rect r): cv::Rect(r){}
		public:
		  int valid;
		  int trackstatus;
		  /*strategy*/
		  int confidence;
		  int structer;
		  int targetarea;
		  bool overlaytrack;
		  int overlaytarckid;
		  int speed;
		  /*strategy*/
		  int classid;
		};
typedef void (* DetectboxCallback)(std::vector<BoundingBox> &boxs,void *context,int chid);
class Detector{
public:
	typedef enum {
			MAXTRACKNUM,
		}DetectParam;
		typedef enum {
					DETECTMOD,
					DETECTCHANGEMODELID,
					DETECTSCALE100x,
					DETECTNOTRACK,
					DETECTFREQUENCY,
					DETECTCOLOR,/*JUST FOR DEEP DETECT*/
					DETECTTRACKENABLE,
					DETECTDETECTCLOSE,//FOR CHANGE VIDEO CLOSE DETECT
					DETECTMINAREA,
					DETECTMAXAREA,
					DETECTTRACKSTRAGE,
					DETECTTRACKSTOPTIME,
					DETECTTRACKCORRECT,
					DETECTTRACKMAXNUM,
					DETECTTRACKNUM,
			}DetectDyParam;
public:

public:
	Detector(){};
public:
	virtual ~Detector() {};
	virtual void init()DETECTOR_PURE_VIRTUAL;
	virtual void init(std::vector<std::string>& model,std::vector<cv::Size>& modelsize)DETECTOR_PURE_VIRTUAL;
	virtual void init(std::vector<Detector*>& detect,std::vector<int>& weight){};//just for composite
	virtual void detect(cv::Mat src,std::vector<BoundingBox> &detectrect,std::vector<BoundingBox> &trackrect,cv::Rect roi=cv::Rect(0,0,0,0),bool useroi=false) {};//for test
	virtual void detect(cv::Mat src,std::vector<BoundingBox> &detectrect,cv::Rect roi=cv::Rect(0,0,0,0),bool useroi=false) DETECTOR_PURE_VIRTUAL;
	virtual void setparam(DetectParam id,int value) DETECTOR_PURE_VIRTUAL;
	virtual void setparam(DetectParam id,std::string value)DETECTOR_PURE_VIRTUAL;
	virtual void dynamicsetparam(DetectDyParam id,int value)DETECTOR_PURE_VIRTUAL;
	virtual void dynamicgetparam(DetectDyParam id,int &value)DETECTOR_PURE_VIRTUAL;
	virtual void getversion();
	virtual void setasyncdetect(DetectboxCallback detectfun,DetectboxCallback trackfun,void * context){};
	virtual void detectasync(cv::Mat src,int chid=0,cv::Rect roi=cv::Rect(0,0,0,0),bool useroi=false);
	

};


#endif /* DETECTOR_HPP_ */
