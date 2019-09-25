/*
 * eventParsing.hpp
 *
 *  Created on: 2019年5月7日
 *      Author: d
 */

#ifndef EVENTPARSING_HPP_
#define EVENTPARSING_HPP_
#include <iostream>
#include "PortFactory.hpp"
#include <vector>
#include  "osa_thr.h"
#include "osa_mutex.h"
#include "globalDate.h"
#include "CClient.hpp"

using namespace std;


#define    RECV_BUF_SIZE   1024
#define  TIME_FOR_THREAD_END 3
#define MAXSENDSIZE 136


typedef struct{
	int connfd;
	bool bConnecting;
    	OSA_ThrHndl getDataThrID;
    	OSA_ThrHndl sendDataThrID;
}CConnectVECTOR;

typedef  vector<CClient*>  CClientVECTOR;

typedef struct {
	comtype_t comtype;
    	int byteSizeSend;
   	unsigned char sendBuff[MAXSENDSIZE];
}sendInfo;

class CEventParsing
{
public:
	CEventParsing();
	~CEventParsing();
	static void *thread_comrecvEvent(void *p);
	static void *thread_comsendEvent(void *p);
	static void *thread_Getaccept(void *p);
	static void *thread_ReclaimConnect(void *p);

public:
	OSA_ThrHndl comrecvEvent_thid, comsendEvent_thid,Getaccept_thid, ReclaimConnect_thid;
	
private:
	CGlobalDate* getDate();
	CMessage *getpM();
	static void *thread_netrecvEvent(void *p);
	void parsingframe(unsigned char *tmpRcvBuff, int sizeRcv, comtype_t comtype);
	int parsingComEvent(comtype_t comtype);
	unsigned char recvcheck_sum(int len_t);
	int getSendInfo(sendInfo * psendBuf);
	void mouseevent(int event);
	void displaymod();
	void workMode();
	void StoreMode(int mod);
	void updatepano();
	void ZoomCtrl();
	void IrisCtrl();
	void FocusCtrl();
	void plantctl();
	void GetsoftVersion();
	void GetSoftWareBuildTargetTime(void);
	char *myStrncpy(char *dest, const char *src, int n) ;
	void ConfigCurrentSave();
	void ConfigLoadDefault();
	void choosedev();
	void chooseptz();
	void playercontrl();
	void playerselect();
	void playerquery_day();
	void playerquery_mon();
	void livevideo();
	void livephoto();
	void panoenable();
	void videoclip();
	void sensortvconfig();
	void sensortrkconfig();
	void scan_plantformconfig();
	void radarconfig();
	void trackconfig();
	void adddevconfig();
	void deletedevconfig();
	void trk_plantformconfig();
	void sensorfrconfig();
	void zeroconfig();
	void recordconfig();
	void movedetectconfig();
	void movedetectareaconfig();
	void displayconfig();
	void correcttimeconfig();
	void panoconfig();
	void  rebootconfig();
	void querryconfig();
	void   softVersion(sendInfo * spBuf);
	void  mainVedioChannel(sendInfo * spBuf);
	void  ack_updatepano(sendInfo * spBuf);
	void  ack_fullscreenmode(sendInfo * spBuf);
	void  trackErrOutput(sendInfo * spBuf);
	void  mutilTargetNoticeStatus(sendInfo * spBuf);
	void multilTargetNumSelectStatus(sendInfo * spBuf);
	void  imageEnhanceStatus(sendInfo * spBuf);
	void moveTargetDetectedStat(sendInfo * spBuf);
	void trackSearchStat(sendInfo * spBuf);
	void trackFinetuningStat(sendInfo * spBuf);
	void confirmAxisStat(sendInfo * spBuf);
	void ElectronicZoomStat(sendInfo * spBuf);
	void settingCmdRespon(sendInfo * spBuf);
	void readConfigSetting(sendInfo * spBuf);
	void extExtraInputResponse(sendInfo * spBuf);
	void  upgradefwStat(sendInfo * spBuf);
	void  paramtodef(sendInfo * spBuf);
	void  recordquerry_day(sendInfo * spBuf);
	void  recordquerry_mon(sendInfo * spBuf);
	void  ackplayertime(sendInfo * spBuf);
	void  ackplantformconfig(sendInfo * spBuf);
	void  acksensortvconfig(sendInfo * spBuf);
	void  acksensortrkconfig(sendInfo * spBuf);
	void   acksensorfrconfig(sendInfo * spBuf);
	void  ackrecordconfig(sendInfo * spBuf,int classid);
	void  ackmvconfig(sendInfo * spBuf);
	void ackdisplayconfig(sendInfo * spBuf);
	void  ackpanoconfig(sendInfo * spBuf);
	void  ackscanplantformconfig(sendInfo * spBuf);
	void ackradarconfig(sendInfo * spBuf);
	void acktrackconfig(sendInfo * spBuf);
	u_int8_t package_frame(uint len, u_int8_t *tmpbuf);
	u_int8_t sendCheck_sum(uint len, u_int8_t *tmpbuf);
	
private:
	static CEventParsing* pThis;
	bool exit_jsParsing,exit_comParsing,exit_netParsing;
	int comfd;
	CPortInterface *pCom1, *pCom2;
	vector<CConnectVECTOR *>  connetVector;
	CClientVECTOR clientVector;
	OSA_MutexHndl mutexConn;

	CGlobalDate* _globalDate;
	CMessage *pM;

};

#endif /* EVENTPARSING_HPP_ */
