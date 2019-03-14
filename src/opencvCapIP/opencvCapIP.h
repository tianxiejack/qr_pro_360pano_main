/*
 * opencvCapIP.h
 *
 *  Created on: 2019年3月14日
 *      Author: fsmdn121
 */

#ifndef OPENCVCAPIP_H_
#define OPENCVCAPIP_H_

#include <string>
#include <vector>
#include "CaptureGroup.h"
typedef enum{
	CVIP_DEVICE_ID_0=0,
	CVIP_DEVICE_ID_MAX
}CVIP_DEVICE_ID;
using namespace std;
#if 0
class opencvIPCamCaptureGroup: public CaptureGroup {

public:
	virtual ~opencvIPCamCaptureGroup();
	virtual void CreateProducers();
	virtual void OpenProducers();
	virtual vector<Consumer>  GetConsumers(int *queueid,int count);
	static opencvIPCamCaptureGroup * GetCVIPCAMInstance();
	void startcap(){v_cons[0].pcap->Capture(NULL);};


private:
	opencvIPCamCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount);
	opencvIPCamCaptureGroup(){};
	vector<Consumer> v_cons;
	static opencvIPCamCaptureGroup m_opencvIPCamCaptureGroup;
};
#endif


class opencvCapIP{
public:
	opencvCapIP(int devid);
	bool Open();
	void Close();
	void mainloop(int now_pic_format){ };
	void start_capturing(void);
	void stop_capturing(void){};
	/* the producer no need to implement there function. */
	void Capture(char*p);
	void CaptureFish(char*p){};
	void saveOverLap(){};
	void processImg();
	 void SetDefaultImg(char*) {};
	 void SavePic(const char* name) {};

private:
    VideoCapture m_cap;
    string mstrURL;
	Mat capframe;
	int mdevid;
};




#endif /* OPENCVCAPIP_H_ */
