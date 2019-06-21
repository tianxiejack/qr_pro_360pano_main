#include <sys/time.h>
#include <assert.h>
#include <stdint.h>
#include "ipcProc.h"

CIPCProc* CIPCProc::pThis = 0;

CIPCProc::CIPCProc()
{
	pthread_mutex_init(&mutex,NULL);
}


CIPCProc::~CIPCProc()
{
}

void CIPCProc::IPCCreate()
{
	Ipc_init();
   	int ret = Ipc_create(tmpIpc);
	if(ret == -1)
	{
		printf("[%s] %d ipc create error \n", __func__, __LINE__);
	}
	return ;
}

void CIPCProc::IPCDestory()
{
	Ipc_destory();
	return ;
}

int CIPCProc::IPCSendMsg(CMD_ID cmd, void* prm, int len)
{
	if(len >= PARAMLEN || (prm == NULL && len > 0))
		return -1;

	SENDST sendData;
	memset(sendData.param, 0, PARAMLEN);
	sendData.cmd_ID = cmd;
	if(len > 0)
		memcpy(sendData.param, prm, len);

	pthread_mutex_lock(&mutex);
	//ipc_sendmsg(IPC_TOIMG_MSG ,&sendData);
	pthread_mutex_unlock(&mutex);

	return 0;
}


int CIPCProc::IPCRecvGstMsg(void* prm)
{
	SENDST recvData;
	IPC_PRM_INT *pIn = (IPC_PRM_INT *)&recvData.param;
	//ipc_recvmsg( IPC_GSTREAM_PTZ , &recvData );
	int flag = -1;
	switch(recvData.cmd_ID)
	{
		case 0:
			break;
		case 1:
			break;
		default:
			break;
	}
	return flag;
}


int CIPCProc::IPCRecvMsg(void* prm)
{
	SENDST recvData;
	IPC_PRM_INT *pIn = (IPC_PRM_INT *)&recvData.param;
	ipc_recvmsg(IPC_1,&recvData);
	switch(recvData.cmd_ID)
	{
	case a:

		break;
	case b:
				
		break;
	case c:
				
		break;

	default:
		break;
	}
	return 0;
}

void *CIPCProc::thread_ipcEvent(void *p)
{
	float value;
	float errorx, errory;
	int cfgid = -1;

}
