/*
 * realRecord.h
 *
 *  Created on: 2019年3月29日
 *      Author: fsmdn121
 */

#ifndef REALRECORD_H_
#define REALRECORD_H_
#include "opencv2/opencv.hpp"
#include "IFrealRecord.h"
using namespace cv;

class RealRecordByCV:public IFrealRecord
{
public:
	RealRecordByCV(int idx,int w,int h,int framerate=25);
	void SaveVideoByTime();
	 void StartRecord();
	 void PushData(Mat *pmatSrc);
	 void StopRecord();
private:
	 int videoIdx;
	 int videoW;
	 int videoH;
	 int vdieoFrameRate;
	 Mat frame;
	 char *mpfilename;
	 VideoWriter outputVideo;

};






#endif /* REALRECORD_H_ */
