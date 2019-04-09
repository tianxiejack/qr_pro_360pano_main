/*
 * IFrealRecord.h
 *
 *  Created on: 2019年3月29日
 *      Author: fsmdn121
 */

#ifndef IFREALRECORD_H_
#define IFREALRECORD_H_
using namespace cv;
class IFrealRecord
{
	public:
	virtual	void StartRecord()=0;
	virtual	void PushData(Mat *pmatSrc)=0;
	virtual void StopRecord()=0;
	virtual char *GetFileName()=0;
};







#endif /* IFREALRECORD_H_ */
