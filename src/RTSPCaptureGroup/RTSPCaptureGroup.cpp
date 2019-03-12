/*
 * RTSPCaptureGroup.cpp
 *
 *  Created on: 2018年5月23日
 *      Author: ubuntu
 */

#include "CaptureGroup.h"
//#include "../buffer.h"
#include <unistd.h>
#include "RTSPCaptureGroup.h"


static IP_cam* pIP_cam[RTSP_DEVICE_ID_MAX]={0};		//IPCamera相机组

 RTSPCaptureGroup RTSPCaptureGroup::rtspCaptureGroup(DEFAULT_IMAGE_WIDTH, DEFAULT_IMAGE_HEIGHT, 4, 1);

RTSPCaptureGroup::RTSPCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount)
:CaptureGroup(w,h,NCHAN,capCount) {

}

 void RTSPCaptureGroup::CreateProducers()
 {
	 	 int dev_id=RTSP_DEVICE_ID_0;
	 	 if(pIP_cam[dev_id]==NULL)
	 	 {
	 		 pIP_cam[dev_id] = new IP_cam(dev_id, "rtsp://admin:abc12345@192.168.1.26");
	 	 }
 }

 void RTSPCaptureGroup::OpenProducers()
 {
	 int dev_id=RTSP_DEVICE_ID_0;
	 pIP_cam[dev_id] ->Open();
 }

 RTSPCaptureGroup::~RTSPCaptureGroup() {

	 for(int i=1; i<1; i++)//0 is not used
	 	{
	 		if(pIP_cam[i]){
	 					delete pIP_cam[i];
	 					pIP_cam[i]= NULL;
	 		}
	 	}
 }

 vector<Consumer>  RTSPCaptureGroup::GetConsumers(int *queueid,int count)
 {
		 Consumer cons;
		 for(int i=0;i<count;i++)
		 {
			 int qid=queueid[i];
			   cons.pcap = new RTSPVCap(qid,DEFAULT_IMAGE_WIDTH, DEFAULT_IMAGE_HEIGHT);
			   cons.idx = i;
			   v_cons.push_back(cons);
		 }
		 return v_cons;
 }

	RTSPCaptureGroup * RTSPCaptureGroup::GetRTSPInstance()
	{
			int queueid[1]={RTSP_DEVICE_ID_0};
			int count=1;
			static bool once =true;
			if(once){
				rtspCaptureGroup.init(queueid,count);
				once =false;
			}
			return &rtspCaptureGroup;
	}

 //******************************** producer ********************************//
 //need by createProducer()
 IP_cam::IP_cam(int devId, const char * URI)
 {
	 	printf("[IP_cam:%d]set rtsp address %s\n", devId, URI);
		InterfaceProcessImageBehavior * pProcessImageBehavior = new ProcessQueueBehavior(devId);
		gstRTSPClient.SetInterfaceProcess(pProcessImageBehavior);
	 	gstRTSPClient.InitMain();
	 	gstRTSPClient.SetSourceLocation(URI);
	 	//ready_capturing();
 }

 IP_cam::~IP_cam()
 {
	 // so far : do nothing here.
 }

 //need by openProducer()
 bool  IP_cam::Open()
 {
	 usleep(100000);
	 start_capturing();
	 return true;
 }

 void IP_cam::Close()
 {

 }

 void IP_cam::start_capturing(void)
 {
	 gstRTSPClient.Play();
 }

void IP_cam::stop_capturing(void)
{
	 gstRTSPClient.Pause();
}

void IP_cam::ready_capturing(void)
{
	 gstRTSPClient.Ready();
}

 //******************************** consumer ********************************//

 void RTSPVCap::Capture(char* ptr1){
	// printf("get_buffer:ptr1=%p,m_qid=%d\n", ptr1, m_qid);
//	 get_buffer((unsigned char *)ptr1, m_qid);
 }















