#include"Comcontrl.hpp"
#include "netMsgDef.h"
#include"demo_global_def.h"
COM_Contrl *COM_Contrl::instance=NULL;



COM_Contrl::COM_Contrl():udpfd(0),renderkey(NULL)
{
	memset(&Netobj,0,sizeof(Netobj));
}
COM_Contrl::~COM_Contrl()
{
	MAIN_threadRecvDestroy();

}
void COM_Contrl::create()
{
	Netobj.recvBuf=recvbuf;
	Netobj.sendBuf=sendbuf;
	udpfd=UDP.NetOpen();
	UDP.NetBind(udpfd,5560);
	MAIN_threadRecvCreate();
	
}

void COM_Contrl::registkey(COMNOTIFYFUNClk fun)
{
	renderkey=fun;

}
int COM_Contrl::NETCTRL_lookupSync(NetObj*pObj)
{
	int stat 		= -1;
	Uint8 *pCur 	= pObj->recvBuf;
	NET_MSG *pMsg	= (NET_MSG *)pCur;
	
	while(pObj->recvLen >= sizeof(NET_MSG_HEADER))
	{
		if(pMsg->head.SyFlag==0xAA && (pMsg->head.uiSize >= sizeof(NET_MSG_HEADER) && pMsg->head.uiSize <= MAX_RECV_BUF_LEN)){
			stat = 0;
			break;
		}
		else{
			pCur++;
			pObj->recvLen--;
			pMsg= (NET_MSG *)pCur;
		}
	}
	
	if(pCur!=pObj->recvBuf){
		memcpy(pObj->recvBuf, pCur, pObj->recvLen);
	}

	return stat;
}

int COM_Contrl::NETCTRL_checkSum(NetObj* pObj)
{
	UINT iCou=0;
	unsigned char checkSum=0;
	NET_MSG *pMsg= (NET_MSG *)pObj->recvBuf;
	//return SDK_SOK;
	if(pMsg->head.uiSize <= sizeof(NET_MSG_HEADER) || pMsg->head.uiSize >= MAX_RECV_BUF_LEN)
		return -1;
	for(iCou=0;iCou<pMsg->head.uiSize-1;iCou++)
	{
		checkSum+=pObj->recvBuf[iCou];
	}
	//printf(" [DEBUG:%s] check sum %02x %02x\n",__func__, checkSum, pObj->recvBuf[pMsg->head.uiSize-1]);
	return (checkSum==pObj->recvBuf[pMsg->head.uiSize-1])?SDK_SOK:-1;
}

int COM_Contrl::NETCTRL_platformRecv(unsigned char *pData,UINT uLen,int context)
{
	int iLen=0;
	//NPORT_Handle pPort = &g_NetPortObj/*(NPORT_Handle)context*/;
	NET_MSG_HEADER *pMsgHead=NULL;
	unsigned char *pInBuf=NULL;
	int mousestatus=0;
	int mousex=0;
	int mousey=0;
	int mousest=0;
	int mousebutton=0;

	pMsgHead = (NET_MSG_HEADER *)pData;
	pInBuf = (unsigned char *)(pData + sizeof(NET_MSG_HEADER));
	iLen = pMsgHead->uiSize-sizeof(NET_MSG_HEADER)-1;
	//printf("*****************************line=%d***********************\n",__LINE__);
	switch(pMsgHead->eCmdType)
	{
	case eCT_UserData205:
		#if (DEBUG_NET)
		printf(" [DEBUG:] %s recv userdata len=0x%x\r\n", __func__, pMsgHead->uiSize);
		#endif
		break;
		
	case eCT_AVTMain:
		#if (DEBUG_NET)
		printf(" [DEBUG:] %s recv avtmain len=0x%x\r\n", __func__, pMsgHead->uiSize);
		#endif
		break;
		
	case eCT_AVTLogdata:
		#if (DEBUG_NET)
		printf(" [DEBUG:] %s recv avtlog len=0x%x\r\n", __func__, pMsgHead->uiSize);
		#endif
		break;
		
	case eCT_Debug:
		#if (DEBUG_NET)
		printf(" [DEBUG:] %s recv debug len=0x%x\r\n", __func__, pMsgHead->uiSize);
		#endif
		break;
		
	case eCT_Msg:
		#if (DEBUG_NET)
		printf(" [DEBUG:] %s recv msg len=0x%x\r\n", __func__, pMsgHead->uiSize);
		#endif
		if(pInBuf[0]==0x04&&pInBuf[1]==0x01)
			{
				mousestatus=1;
				mousest=0;
				mousebutton=0;
			}
		if(pInBuf[0]==0x04&&pInBuf[1]==0x02)
			{
				mousestatus=1;
				mousest=1;
				mousebutton=0;
			}
		if(pInBuf[0]==0x05&&pInBuf[1]==0x01)
			{
				mousestatus=1;
				mousest=0;
				mousebutton=2;
			}
		if(pInBuf[0]==0x05&&pInBuf[1]==0x02)
			{
				mousestatus=1;
				mousest=1;
				mousebutton=2;

			}
		
		memcpy(&mousex,&pInBuf[2],4);
		memcpy(&mousey,&pInBuf[6],4);
		if(mousestatus==1)
			{
				if(renderkey!=NULL)
					renderkey(mousebutton,mousest,mousex,mousey);

			}
		
		//printf("**********mousestatus=%d*****mousex=%d**mousey=%d******\n",mousestatus,mousex,mousey);
		//netctrl_setInCmd((pData+headLenNetMsg), iLen);
		break;
		
	case eCT_Update:
		#if (DEBUG_NET)
		printf(" [DEBUG:] %s recv update len=0x%x\r\n", __func__, pMsgHead->uiSize);
		#endif
		break;
		
	default :
		printf(" [DEBUG:] %s recv unknown msg type 0x%x\r\n", __func__, pMsgHead->eCmdType);
		break;
	}

	return 0;
}

void COM_Contrl::main_Recv_func()
{
	OSA_printf("%s: Main Proc Tsk Is Entering...\n",__func__);
	int mousex=0;
	int mousey=0;
	int mousestatue=0;
	int headLenNetMsg=sizeof(NET_MSG_HEADER);
	NET_MSG_HEADER *pMsgHead=NULL;
	pMsgHead 	= (NET_MSG_HEADER*)Netobj.recvBuf;
	int iLen=0, nWait=0, recvLen=0, result=0, stat=0;
	while(mainObj.exitProcThread ==  false)
	{
		
			int recvLen=UDP.NetRecv(udpfd, Netobj.recvBuf+ Netobj.recvLen,MAX_RECV_BUF_LEN-Netobj.recvLen);
			if(recvLen<0)
				continue;

			Netobj.recvLen += recvLen;
			if(Netobj.recvLen == MAX_RECV_BUF_LEN)
			{
				printf(" [DEBUG:] %s recv buf is full, clean !!!\n",__func__);
				Netobj.recvLen = 0;
			}

			
			while(Netobj.recvLen >= headLenNetMsg)
			{
				// check msg head and data len
				if(nWait == 0)
				{
					stat = NETCTRL_lookupSync(&Netobj);
					if(stat==SDK_SOK)
					{
						nWait = Netobj.recvLen - pMsgHead->uiSize;
						iLen = pMsgHead->uiSize;
						//printf(" [DEBUG:] %s lookup head ok type=%02x size=%02x\r\n", 
						//		__func__, pMsgHead->eCmdType, pMsgHead->uiSize);
					}
					else
						break;
				}
				// deal data part
				//printf("********LINE=%d*****nWait=%d******************\n",__LINE__,nWait);
				if(nWait >= 0)
				{
					if(NETCTRL_checkSum(&Netobj) == SDK_SOK)
			                {
			//					NETCTRL_platformRecv(pObj->recvBuf, iLen,(int)pPort);
			                   			 NETCTRL_platformRecv(Netobj.recvBuf, iLen,0);
			                }
							else
								printf(" [DEBUG:] %s check sum error type=%02x size=%02x\r\n", 
										__func__, pMsgHead->eCmdType, pMsgHead->uiSize);
	                    
					Netobj.recvLen = nWait;
					if(Netobj.recvLen > 0)
					{
						memcpy(Netobj.recvBuf, Netobj.recvBuf+iLen, Netobj.recvLen);
						nWait = 0;
					}
				}
				else
				{
					nWait = 0;
					break;
				}
			}// while(packet - recvbuf <= buffLen)     

			
	
		
		
		
	}



}
int COM_Contrl::MAIN_threadRecvCreate(void)
{
	int iRet = OSA_SOK;
	iRet = OSA_semCreate(&mainObj.procNotifySem ,1,0) ;
	OSA_assert(iRet == OSA_SOK);


	mainObj.exitProcThread = false;

	mainObj.initFlag = true;

	mainObj.pParent = (void*)this;

	iRet = OSA_thrCreate(&mainObj.thrHandleProc, mainRecvTsk, 0, 0, &mainObj);
	

	return iRet;
}


int COM_Contrl::MAIN_threadRecvDestroy(void)
{
	int iRet = OSA_SOK;

	mainObj.exitProcThread = true;
	OSA_semSignal(&mainObj.procNotifySem);

	iRet = OSA_thrDelete(&mainObj.thrHandleProc);

	mainObj.initFlag = false;
	OSA_semDelete(&mainObj.procNotifySem);

	return iRet;
}


COM_Contrl * COM_Contrl::getinstance()
{
	if(instance==NULL)
		instance=new COM_Contrl();
	return instance;
}
