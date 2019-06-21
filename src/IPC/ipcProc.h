#ifndef _IPCPROC_H_
#define _IPCPROC_H_

#include "osa_thr.h"
#include "ipc_custom_head.hpp"
#include <string>
#pragma once
using namespace std;

typedef struct{
	int status;
	float errx;
	float erry;
}GstRectParm_t;
	
class  CIPCProc{

public:
	CIPCProc();
	~CIPCProc();

	void IPCCreate();
	void IPCDestory();
	static void *thread_ipcEvent(void *p);
	int IPCSendMsg(CMD_ID cmd, void* prm, int len);
	int IPCRecvMsg(void* prm);
	int IPCRecvGstMsg(void* prm);
	
	GstRectParm_t m_gstRectParm;
private:
	static CIPCProc* pThis;
	pthread_mutex_t mutex;

};
#endif
