#include <sys/time.h>
#include <assert.h>
#include <stdint.h>
#include "ipcProc.h"
#include "configtable.h"
#include "plantformcontrl.hpp"

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
	ipc_sendmsg(IPC_TOIMG_MSG ,&sendData);
	pthread_mutex_unlock(&mutex);

	return 0;
}


int CIPCProc::IPCRecvGstMsg(void* prm)
{
	SENDST recvData;
	IPC_PRM_INT *pIn = (IPC_PRM_INT *)&recvData.param;
	ipc_recvmsg( IPC_GSTREAM_PTZ , &recvData );
	int flag = -1;
	switch(recvData.cmd_ID)
	{
		case 0:
			m_gstRectParm.status = pIn->intPrm[0];	
			flag = 0;
			break;
		case 1:
			m_gstRectParm.status = pIn->intPrm[0];
			memcpy((void*)&m_gstRectParm.errx ,(void*)&pIn->intPrm[1],sizeof(float));
			memcpy((void*)&m_gstRectParm.erry ,(void*)&pIn->intPrm[2],sizeof(float));
			flag = 1;
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
	ipc_recvmsg(IPC_FRIMG_MSG,&recvData);
	switch(recvData.cmd_ID)
	{
	case a:
			Plantformpzt::getinstance()->getpanopanpos();
			Plantformpzt::getinstance()->getpanotitlepos();
			
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
	while(!pThis->exit_ipcthread)
	{
		cfgid = pThis->m_ipc->IPCRecvMsg(NULL);
		switch(cfgid)
		{
			case CFGID_RTS_mainch:
				pThis->signalFeedBack(4, pThis->outcomtype, ACK_Sensor, 1, (int)pThis->cfg_value[CFGID_RTS_mainch]);
				break;
			case CFGID_RTS_mainch2:
				pThis->signalFeedBack(4, pThis->outcomtype, ACK_Sensor, 2, (int)pThis->cfg_value[CFGID_RTS_mainch]);
				break;
			case CFGID_RTS_trken:
				pThis->signalFeedBack(3, pThis->outcomtype, ACK_TrkStat, (int)pThis->cfg_value[CFGID_RTS_trken]);
				break;
			case CFGID_RTS_sectrkstat:
			{
				int sectrkstat = pThis->cfg_value[CFGID_RTS_sectrkstat];
				if((0 == sectrkstat) || (3 == sectrkstat))
					pThis->signalFeedBack(3, pThis->outcomtype, ACK_SectrkStat, 3);
				else if(1 == sectrkstat)
					pThis->signalFeedBack(3, pThis->outcomtype, ACK_SectrkStat, 1);
				else if(2 == sectrkstat)
					pThis->signalFeedBack(3, pThis->outcomtype, ACK_SectrkStat, 2);
			}
				break;	
			case CFGID_RTS_trkstat:

				pThis->m_pixelErr.status = pThis->cfg_value[CFGID_RTS_trkstat];
				memcpy(&value, pThis->cfg_value + CFGID_RTS_trkerrx, 4);
				pThis->m_pixelErr.errx = value;
				memcpy(&value, pThis->cfg_value + CFGID_RTS_trkerry, 4);
				pThis->m_pixelErr.erry = value;
				
				pThis->_Msg->MSGDRIV_send(MSGID_COM_INPUT_TRKCONTROL, 0);

				pThis->_StateManager->_state->recvTrkmsg(pThis->cfg_value[CFGID_RTS_trkstat]);
					
				if(1 == pThis->outtype)
				{
					memcpy(&errorx, pThis->cfg_value + CFGID_RTS_trkerrx, 4);
					memcpy(&errory, pThis->cfg_value + CFGID_RTS_trkerry, 4);
					pThis->signalFeedBack_output(pThis->outcomtype, (int)pThis->cfg_value[CFGID_RTS_trkstat], pThis->outtype, errorx, errory);
				}else if(2 == pThis->outtype){
					float x,y;
					pThis->_StateManager->_state->getcurRate(x,y);
					pThis->signalFeedBack_output(pThis->outcomtype, (int)pThis->cfg_value[CFGID_RTS_trkstat], pThis->outtype, x, y);
				}
				break;

			case CFGID_RTS_mtddet:	
					pThis->_StateManager->_state->mtdhandle(pThis->cfg_value[CFGID_RTS_mtddet]);
				break;

			default:
				break;
		}
	}
}