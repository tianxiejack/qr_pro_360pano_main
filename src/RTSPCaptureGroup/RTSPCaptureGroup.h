/*
 * RTSPCaptureGroup.h
 *
 *  Created on: 2018年5月23日
 *      Author: ubuntu
 */

#ifndef RTSPCAPTUREGROUP_H_
#define RTSPCAPTUREGROUP_H_

#include "CaptureGroup.h"
#include "Camera.h"
#include "GstRTSPClient.h"
#include "InterfaceProcessImageBehavior.h"
#include "testInterfaceProcessImageBehavior.h"

typedef enum{
	RTSP_DEVICE_ID_0=0,
	RTSP_DEVICE_ID_MAX
}RTSP_DEVICE_ID;
/*
typedef enum{
	RTSP_QUEUE_ID_0=0,
	RTSP_QUEUE_ID_MAX
}RTSP_QUEUE_ID;
*/

class RTSPCaptureGroup: public CaptureGroup {

public:
	virtual ~RTSPCaptureGroup();
	virtual void CreateProducers();
	virtual void OpenProducers();
	virtual vector<Consumer>  GetConsumers(int *queueid,int count);
	static RTSPCaptureGroup * GetRTSPInstance();
	

private:
	RTSPCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount=1);
	RTSPCaptureGroup(){};
	vector<Consumer> v_cons;
	static RTSPCaptureGroup rtspCaptureGroup;
};

class IP_cam:public BaseVCap{
public:
	IP_cam(int devId, const char * ip_rtspServerPortNum_streamName="rtsp://192.168.1.26:8554/stream1");
	virtual ~IP_cam();
	bool Open();
	void Close();
	void mainloop(int now_pic_format){ };
	void start_capturing(void);
	void stop_capturing(void);
	void ready_capturing(void);

	/* the producer no need to implement there function. */
	void Capture(char*p){};
	void CaptureFish(char*p){};
	void saveOverLap(){};

private:
	GstRTSPClient gstRTSPClient;

};

class RTSPVCap:public BaseVCap{
public:
	RTSPVCap(int qid,int wide,int height):m_qid(qid){ };
	virtual ~RTSPVCap(){ };
	bool Open(){return true;};
	void Close(){};
	void Capture(char* ptr1);
	void CaptureFish(char* ptr1){};
	void SavePic(const char* name){};
	void saveOverLap(){};
	
private:
	int m_qid;

};



#endif /* RTSPCAPTUREGROUP_H_ */
