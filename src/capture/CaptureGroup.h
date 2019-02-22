/*
 * CaptureGroup.h
 *
 *  Created on: Dec 16, 2016
 *      Author: wang
 */

#ifndef CAPTUREGROUP_H_
#define CAPTUREGROUP_H_

#include "StlGlDefines.h"
#include "Camera.h"
#include"HDV4lcap.h"
#include <vector>
using namespace cv;
using namespace std;
typedef struct _Consumer
{
	int idx;
	BaseVCap * pcap;
}Consumer;

class interfaceCapGroup
{
public:
	virtual void CreateProducers()=0;
	virtual void OpenProducers()=0;
	virtual vector<Consumer>  GetConsumers(int *queueid,int count)=0;
};

class CaptureGroup:public interfaceCapGroup
{
public:
	CaptureGroup(unsigned int capCount);
	CaptureGroup(unsigned int wide,unsigned int height,int NCHAN,unsigned int capCount=1);

	virtual ~CaptureGroup();

	void SetConsumers(vector<Consumer> h);
	void init(int *queueid,int count);

	void captureCam(GLubyte *ptr, int index);
	void captureCamFish(GLubyte *ptr, int index);
	void Open();
	bool Append(pInterface_VCap cap);
	bool getScannerPic(IplImage *pImage);
	bool saveCapImg();
	void saveOverLap();
	virtual void CreateProducers(){};
	bool saveSingleCapImg(int cam_num);
#if USE_12
	bool saveExposureCompensationCapImg();
#endif
protected:
	void Close();
	void fillColorBar();
	const unsigned int m_TotalCamCount;
	unsigned int m_currentIdx;
	pInterface_VCap *capCamera;
	unsigned int width, height, depth;
	unsigned char * Defaultimage;
	CaptureGroup():m_TotalCamCount(0),m_currentIdx(0){};
	vector<Consumer> v_cons;
};

class PseudoCaptureGroup:public CaptureGroup
{
public:
	static	PseudoCaptureGroup *GetInstance();
	PseudoCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount=1):
		CaptureGroup(w,h,NCHAN,capCount){};
	~PseudoCaptureGroup(){};
	virtual vector<Consumer>  GetConsumers(int *queueid,int count)
		{return vector<Consumer>(0);};
	virtual void CreateProducers(){};
	virtual void OpenProducers(){};
private:
	static PseudoCaptureGroup pseudoCaptureGroup;
};

class HDCaptureGroup:public CaptureGroup
{
public:
	HDCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount=1):
		CaptureGroup(w,h,NCHAN,capCount){};
	virtual ~HDCaptureGroup(){};
	virtual vector<Consumer>  GetConsumers(int *queueid,int count);
	HDCaptureGroup(){};

};


#endif /* CAPTUREGROUP_H_ */
