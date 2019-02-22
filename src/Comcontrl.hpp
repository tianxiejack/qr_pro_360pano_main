/*
 * Comcontrl.hpp
 *
 *  Created on: 2018年11月14日
 *      Author: wj
 */

#ifndef COMCONTRL_HPP_
#define COMCONTRL_HPP_
#include "Udpbase.hpp"
#include <osa_thr.h>
#include"osa_sem.h"
#include"config.hpp"
typedef struct {
	bool bTrack;
	bool bMtd;
	bool bBlobDetect;
	int chId;
	int iTrackStat;
	
	//Mat frame;
}MAIN_COMThrObj_cxt;

typedef struct {
	MAIN_COMThrObj_cxt cxt[2];
	OSA_ThrHndl		thrHandleProc;
	OSA_SemHndl	procNotifySem;
	int pp;
	bool bFirst;
	volatile bool	exitProcThread;
	bool						initFlag;
	void 						*pParent;
}MAIN_COMThrObj;


typedef struct _dev_obj {
	int  devId;
	int  recvLen;
	int  sendLen;
	//eUartStat uartStat;
	//int  devPortType;		// Enum_MuxPortType

	
	Bool tskLoop;
	Bool istskStopDone;

	UInt8* sendBuf;
	UInt8* recvBuf;
}NetObj;
#define MAX_RECV_BUF_LEN 256

typedef void ( *COMNOTIFYFUNClk)(int button, int state, int x, int y);
class COM_Contrl
{
	public:
		static COM_Contrl *instance;
		static COM_Contrl *getinstance();
		void registkey(COMNOTIFYFUNClk fun);
		void create();
	private:
	COM_Contrl();
	~COM_Contrl();
	COMNOTIFYFUNClk renderkey;
	UDPBase UDP;
	int udpfd;
	unsigned char recvbuf[1000];
	unsigned char sendbuf[100];
	NetObj Netobj;
	MAIN_COMThrObj	mainObj;

	int NETCTRL_lookupSync(NetObj*pObj);
	int NETCTRL_checkSum(NetObj* pObj);
	int NETCTRL_platformRecv(unsigned char  *pData,unsigned int  uLen,int context);
	void main_Recv_func();
	int MAIN_threadRecvCreate(void);
	int MAIN_threadRecvDestroy(void);
	static void *mainRecvTsk(void *context)
	{
		MAIN_COMThrObj  * pObj= (MAIN_COMThrObj*) context;
		if(pObj==NULL)
			{

			printf("++++++++++++++++++++++++++\n");

			}
		COM_Contrl *ctxHdl = (COM_Contrl *) pObj->pParent;
		ctxHdl->main_Recv_func();
		
		return NULL;
	}
	



};




#endif /* COMCONTRL_HPP_ */
