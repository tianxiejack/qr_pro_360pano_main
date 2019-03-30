/*
 * ReSplic.h
 *
 *  Created on: 2018年12月26日
 *      Author: fsmdn121
 */

#ifndef RESPLIC_H_
#define RESPLIC_H_
#include "opencv2/opencv.hpp"
#include "MP4cabinCapture.h"
using namespace cv;

class IFSaveVideo
{
public:
	virtual void init()=0;
	virtual void SaveAsMp4(Mat *pmatSrc)=0;
	virtual void EndOfStream()=0;
	virtual void SetMTime(int Tnm)=0;
};

class Pseudo:public IFSaveVideo
{
	 void init(){};
	 void SaveAsMp4(Mat *pmatSrc){};
	 void EndOfStream(){};
	 void SetMTime(int Tnm){};
};


class SaveVideoByGST:public IFSaveVideo
{
	 void init();
	 void SaveAsMp4(Mat *pmatSrc);
	 void EndOfStream();
private:
#if 1
	 RecordHandle * record_handle;
#endif
public:
	void SetMTime(int Tnm);
};

#endif /* RESPLIC_H_ */
