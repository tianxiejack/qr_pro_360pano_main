/*
 * eventParsing.cpp
 *
 *  Created on: 2019年5月7日
 *      Author: d
 */
#include "eventParsing.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include "osa_sem.h"
#include <errno.h>
#include "Status.hpp"

CEventParsing* CEventParsing::pThis = NULL;
int evtDbgOn = 0;

CEventParsing::CEventParsing()
{
	pThis = this;
	_globalDate = getDate();
	pM = getpM();

	exit_comParsing = false;
	pCom1 = PortFactory::createProduct(1);
	comfd = pCom1->copen();
	exit_netParsing = false;
	memset(&mutexConn,0,sizeof(mutexConn));
	OSA_mutexCreate(&mutexConn);
	pCom2 = PortFactory::createProduct(2);
	pCom2->copen();

}

CEventParsing::~CEventParsing()
{
	exit_comParsing = true;
	exit_netParsing = true;
	pCom1->cclose();
	pCom2->cclose();
	delete pCom1;
	delete pCom2;
}

CGlobalDate* CEventParsing::getDate()
{
	_globalDate = CGlobalDate::Instance();
	return _globalDate;
}

CMessage * CEventParsing::getpM()
{
    pM =  CMessage::getInstance();
    return pM;
}

void *CEventParsing::thread_comrecvEvent(void *p)
{
	int sizeRcv;
 	uint8_t dest[1024]={0};
	int read_status = 0;
	int dest_cnt = 0;
	unsigned char  tmpRcvBuff[RECV_BUF_SIZE];
	memset(tmpRcvBuff,0,sizeof(tmpRcvBuff));

	while(!pThis->exit_comParsing)
	{
		sizeRcv= pThis->pCom1->crecv(pThis->comfd, (void *)tmpRcvBuff,RECV_BUF_SIZE);
		comtype_t comtype = {pThis->comfd, 1};
		pThis->parsingframe(tmpRcvBuff, sizeRcv, comtype);
	}
}

void *CEventParsing::thread_comsendEvent(void *p)
{
	sendInfo repSendBuffer;
	
	while(!pThis->exit_comParsing)
	{
		OSA_semWait(&pThis->_globalDate->m_semHndl,OSA_TIMEOUT_FOREVER);
		pThis->getSendInfo(&repSendBuffer);
		if(1 == repSendBuffer.comtype.type)
		{
			pThis->pCom1->csend(repSendBuffer.comtype.fd, &repSendBuffer.sendBuff, repSendBuffer.byteSizeSend);
		}

		else if(2 == repSendBuffer.comtype.type)
		{
			OSA_mutexLock(&pThis->mutexConn);
			if((pThis->connetVector.size()>0) && (pThis->connetVector[0]->bConnecting))
			{
				pThis->pCom2->csend(repSendBuffer.comtype.fd, &repSendBuffer.sendBuff, repSendBuffer.byteSizeSend);
				if(evtDbgOn)
				{
					printf("net send %d bytes:\n", repSendBuffer.byteSizeSend);
					for(int i = 0; i < repSendBuffer.byteSizeSend; i++)
						printf("%02x ", repSendBuffer.sendBuff[i]);
					printf("\n");
				}
			}
			OSA_mutexUnlock(&pThis->mutexConn);
		}

	}
}

void *CEventParsing::thread_Getaccept(void *p)
{
	int iret;
	struct sockaddr_in connectAddr;
	memset(&connectAddr, 0, sizeof(connectAddr));
	int lenthConnect = sizeof(connectAddr);
	
    	while(!pThis->exit_netParsing)
    	{
    		int m_connectfd=pThis->pCom2->caccept((struct sockaddr*)&connectAddr,(socklen_t*)&lenthConnect);
		if(m_connectfd<0)
		{
			printf("ERR: Can not  accept  Client connect\r\n");
			continue;
		}
		else
		{
			char *connectIP = inet_ntoa(connectAddr.sin_addr);
			int connectPort = ntohs(connectAddr.sin_port);
			printf("INFO: Accept Connect IP : %s---- Port: %d,connfd:%d\r\n",connectIP, connectPort, m_connectfd);
			
			CConnectVECTOR *pConnect = (CConnectVECTOR *)malloc(sizeof(CConnectVECTOR));
			CClient  *pClient = new CClient();
			
			pConnect->connfd = m_connectfd;
			OSA_mutexLock(&pThis->mutexConn);
			pThis->connetVector.push_back(pConnect);
			memcpy(pClient->serverip, connectIP, strlen(connectIP));
           		pThis->clientVector.push_back(pClient);
			OSA_mutexUnlock(&pThis->mutexConn);

			pConnect->bConnecting=true;
			iret = OSA_thrCreate(&pConnect->getDataThrID,
			        thread_netrecvEvent,
			        0,
			        0,
			        (void*)pConnect);

			 pClient->create();
           		 pClient->run();
		}
	}
}

void *CEventParsing::thread_netrecvEvent(void *p)
{
	int sizeRcv;
 	uint8_t dest[1024]={0};
	int read_status = 0;
	int dest_cnt = 0;
	unsigned char  tmpRcvBuff[RECV_BUF_SIZE];
	memset(tmpRcvBuff,0,sizeof(tmpRcvBuff));

	CConnectVECTOR *pConnect = (CConnectVECTOR *)p;
	while(!pThis->exit_netParsing)
	{
		sizeRcv= pThis->pCom2->crecv(pConnect->connfd, (void *)tmpRcvBuff,RECV_BUF_SIZE);
		if(-2 == sizeRcv)
		{
			pConnect->bConnecting = false;
			break;
		}
		comtype_t comtype = {pConnect->connfd, 2};
		pThis->parsingframe(tmpRcvBuff, sizeRcv, comtype);
	}
}

void *CEventParsing::thread_ReclaimConnect(void *p)
{
	while(!pThis->exit_netParsing)
	{
		OSA_mutexLock(&pThis->mutexConn);
		vector<CConnectVECTOR *>::iterator iter = pThis->connetVector.begin();
 		CClientVECTOR::iterator iterc = pThis->clientVector.begin();
		for (iter; iter != pThis->connetVector.end();)
		{
			CConnectVECTOR *pConnect = *iter;
			CClient *pCClient= (CClient*)*iterc;
			if (!pConnect->bConnecting)
			{
				printf("INFO:  Reclainm connect !!\r\n");
				OSA_thrDelete(&pConnect->getDataThrID);
				pThis->connetVector.erase(iter);
				free(pConnect);

				pCClient->DisConnecting();
				pThis->clientVector.erase(iterc);
				delete pCClient;
				pCClient = NULL;
			}
			else
			{
				iter++;
				iterc++;
			}
		}
		OSA_mutexUnlock(&pThis->mutexConn);
				
		struct timeval tv;
		int err;
		do{
			tv.tv_sec = TIME_FOR_THREAD_END;
			tv.tv_usec = 0;
			err = select(0, NULL, NULL, NULL, &tv);
		}while(err<0 && errno==EINTR);
	}

	if(pThis->exit_netParsing)
	{
		OSA_mutexLock(&pThis->mutexConn);
		vector<CConnectVECTOR *>::iterator iter = pThis->connetVector.begin();
		for (iter; iter != pThis->connetVector.end();)
		{
			CConnectVECTOR *pConnect = *iter;
			if (pConnect->bConnecting)
			{
				pConnect->bConnecting = false;
				OSA_thrDelete(&pConnect->getDataThrID);
				pThis->connetVector.erase(iter);
				printf("*pConnect=%p\n", pConnect);
				free(pConnect);
			}
			else
			{
				++iter;
			}
		}
		OSA_mutexUnlock(&pThis->mutexConn);
		
		struct timeval tv;
		int err;
		do{
			tv.tv_sec = TIME_FOR_THREAD_END;
			tv.tv_usec = 0;
			err = select(0, NULL, NULL, NULL, &tv);
		}while(err<0 && errno==EINTR);
			
	}
    	return 0;
}

void CEventParsing::parsingframe(unsigned char *tmpRcvBuff, int sizeRcv, comtype_t comtype)
{
	unsigned int uartdata_pos = 0;
	unsigned char frame_head[]={0xEB, 0x51};
	
	static struct data_buf
	{
		unsigned int len;
		unsigned int pos;
		unsigned char reading;
		unsigned char buf[RECV_BUF_SIZE];
	}swap_data = {0, 0, 0,{0}};


	uartdata_pos = 0;
	if(sizeRcv>0)
	{
		if(evtDbgOn)
		{
			printf("------------------(fd:%d)start recv data---------------------\n", comtype.fd);
			for(int j=0;j<sizeRcv;j++)
			{
				printf("%02x ",tmpRcvBuff[j]);
			}
			printf("\n");
		}

		while (uartdata_pos< sizeRcv)
		{
	        		if((0 == swap_data.reading) || (2 == swap_data.reading))
	       		{
	            			if(frame_head[swap_data.len] == tmpRcvBuff[uartdata_pos])
	            			{
	                			swap_data.buf[swap_data.pos++] =  tmpRcvBuff[uartdata_pos++];
	                			swap_data.len++;
	                			swap_data.reading = 2;
	                			if(swap_data.len == sizeof(frame_head)/sizeof(char))
	                    				swap_data.reading = 1;
	            			}
		           		 else
		            		{
		                		uartdata_pos++;
		                		if(2 == swap_data.reading)
		                    		memset(&swap_data, 0, sizeof(struct data_buf));
		            		}
			}
		        	else if(1 == swap_data.reading)
			{
				swap_data.buf[swap_data.pos++] = tmpRcvBuff[uartdata_pos++];
				swap_data.len++;
				if(swap_data.len>=4)
				{
					if(swap_data.len==((swap_data.buf[2]|(swap_data.buf[3]<<8))+5))
					{

						for(int i=0;i<swap_data.len;i++)
						{
							_globalDate->rcvBufQue.push_back(swap_data.buf[i]);
						}
						parsingComEvent(comtype);
						memset(&swap_data, 0, sizeof(struct data_buf));
					}
				}
			}
		}
	}
}

int CEventParsing::parsingComEvent(comtype_t comtype)
{
	int ret =  -1;
	int cmdLength= (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8)+5;

	if(cmdLength<6)
	{
        		printf("Warning::Invaild frame\r\n");
        		_globalDate->rcvBufQue.erase(_globalDate->rcvBufQue.begin(),_globalDate->rcvBufQue.begin()+cmdLength);
        		return ret;
    	}
    	unsigned char checkSum = recvcheck_sum(cmdLength);
	int cmdid = _globalDate->rcvBufQue.at(4);
	if(Status::getinstance()==0)
		cmdid=-1;

	OSA_mutexLock(&pThis->mutexConn);
	if(connetVector.size() > 0)
	{
		if(comtype.type == 2 && comtype.fd != connetVector[0]->connfd)
			cmdid = -1;
	}
	OSA_mutexUnlock(&pThis->mutexConn);
	
    	if(checkSum== _globalDate->rcvBufQue.at(cmdLength-1))
    	{	
    		if(cmdid != -1)
				_globalDate->comtype = comtype;
			
        		switch(cmdid)
        		{
            		case 0x03:
				mouseevent(Status::MOUSEBUTTON);
				break;
			case 0x04:
				mouseevent(Status::MOUSEROLLER);
				break;
			case 0x05:
				displaymod();
				break;
			case 0x06:
				workMode();
				break;
			case 0x07:
				StoreMode(Status::STOREGO);
				break;
			case 0x08:
				StoreMode(Status::STORESAVE);
				break;
			case 0x09:
				updatepano();
				break;
			case 0x12:
				ZoomCtrl();
				break;
			case 0x13:
				IrisCtrl();
				break;
			case 0x14:
				FocusCtrl();
				break;
			case 0x15:
				plantctl();
				break;

			case 0x30:
				GetsoftVersion();
				break;
			case 0x32:
				//provided by a single server
				break;
			case 0x33:
				//provided by a single server
				break;
			case 0x35:
				//provided by a single server
				break;

			case 0x40:
				ConfigCurrentSave();
				break;
			case 0x41:
				ConfigLoadDefault();
				break;
			case 0x45:
				choosedev();
				break;
			case 0x46:
				chooseptz();
				break;

			case 0x60:
				playercontrl();
				break;
			case 0x61:
				playerselect();
				break;
			case 0x62:
				playerquery();
				break;
			case 0x63:
				livevideo();
				break;
			case 0x64:
				livephoto();
				break;

			case 0x65:
				panoenable();
				break;
			case 0x66:
				videoclip();
				break;

			case 0x77:
				sensortvconfig();
				break;
			case 0x78:
				sensortrkconfig();
				break;
			case 0x79:
				scan_plantformconfig();
				break;
			case 0x7a:
				radarconfig();
				break;
			case 0x7b:
				trackconfig();
				break;

			case 0x7c:
				adddevconfig();
				break;
			case 0x7d:
				deletedevconfig();
				break;
			case 0x80:
				trk_plantformconfig();
				break;
			case 0x81:
				sensorfrconfig();
				break;
			case 0x82:
				zeroconfig();
				break;

			case 0x83:
				recordconfig();
				break;
			case 0x84:
				movedetectconfig();
				break;
			case 0x85:
				movedetectareaconfig();
				break;
			case 0x86:
				displayconfig();
				break;
			case 0x87:
				correcttimeconfig();
				break;
			case 0x88:
				panoconfig();
				break;
			case 0x8a:
				rebootconfig();
				break;
			case 0x90:
				querryconfig();
				break;

			default:
				printf("INFO: Unknow  Control Command, please check!!!\r\n ");
				ret = 0;
				break;
		}
    	}
    	else
        		printf("%s,%d, checksum error:,cal is %02x,recv is %02x\n",__FILE__,__LINE__,checkSum,_globalDate->rcvBufQue.at(cmdLength-1));
		
	_globalDate->rcvBufQue.erase(_globalDate->rcvBufQue.begin(),_globalDate->rcvBufQue.begin()+cmdLength);
	return 1;

}

unsigned char CEventParsing::recvcheck_sum(int len_t)
{
    unsigned char cksum = 0;
    for(int n=1; n<len_t-1; n++)
    {
        cksum ^= _globalDate->rcvBufQue.at(n);
    }
    return  cksum;
}

int  CEventParsing::getSendInfo(sendInfo * psendBuf)
{
	int respondId = _globalDate->feedback;
	psendBuf->comtype = _globalDate->comtype;
	
	switch(respondId){
		case ACK_softVersion:
			softVersion(psendBuf);
			break;
		case ACK_mainVideoStatus:
			mainVedioChannel(psendBuf);
			break;
		case ACK_updatepano:
			ack_updatepano(psendBuf);
			break;
		case ACK_fullscreenmode:
			ack_fullscreenmode(psendBuf);
			break;
		case ACK_avtErrorOutput:
			trackErrOutput(psendBuf);
			break;
		case ACK_mmtStatus:
			mutilTargetNoticeStatus(psendBuf);
			break;
		case ACK_mmtSelectStatus:
			multilTargetNumSelectStatus(psendBuf);
			break;
		case ACK_EnhStatus:
			imageEnhanceStatus(psendBuf);
			break;
		case ACK_MtdStatus:
			moveTargetDetectedStat(psendBuf);
			break;
		case ACK_TrkSearchStatus:
			trackSearchStat(psendBuf);
			break;
		case ACK_posMoveStatus:
			trackFinetuningStat(psendBuf);
			break;
		case ACK_moveAxisStatus:
			confirmAxisStat(psendBuf);
			break;
		case ACK_ElectronicZoomStatus:
			ElectronicZoomStat(psendBuf);
			break;
		case ACK_picpStatus:
			break;
		case ACK_VideoChannelStatus:
			break;
		case ACK_frameCtrlStatus:
			break;
		case ACK_compression_quality:
			break;
		case ACK_config_Write:
			settingCmdRespon(psendBuf);
			break;
		case ACK_config_Read:
			readConfigSetting(psendBuf);
			break;
		case ACK_jos_Kboard:
			extExtraInputResponse(psendBuf);
			break;
		case ACK_upgradefw:
			//printf("%s,%d, upgradefw response!!!\n",__FILE__,__LINE__);
			upgradefwStat(psendBuf);
			break;
		case ACK_param_todef:
			paramtodef(psendBuf);
			break;
		case ACK_playerquerry:
			recordquerry(psendBuf);
			break;
		case ACK_playertime:
			ackplayertime(psendBuf);
			break;
		case ACK_plantformconfig:
			ackplantformconfig(psendBuf);
			break;
		case ACK_sensortvconfig:
			acksensortvconfig(psendBuf);
			break;
		case ACK_sensortrkconfig:
			acksensortrkconfig(psendBuf);
			break;
		case ACK_sensorfrconfig:
			acksensorfrconfig(psendBuf);
			break;
		case ACK_zeroconfig:
			//recordquerry(psendBuf);
			break;
		case ACK_recordconfig:
			ackrecordconfig(psendBuf,0);
			break;
		case ACK_mvconfig:
			ackmvconfig(psendBuf);
			break;
		case ACK_mvareaconfig:
			//recordquerry(psendBuf);
			break;
		case ACK_displayconfig:
			ackdisplayconfig(psendBuf);
			break;
		case ACK_correcttimeconfig:
			//recordquerry(psendBuf);
			break;
		case ACK_recordconfigmv:
			ackrecordconfig(psendBuf,1);
			break;
		case ACK_panoconfig:
			ackpanoconfig(psendBuf);
			//recordquerry(psendBuf);
			break;
		case ACK_scanplantformconfig:
			ackscanplantformconfig(psendBuf);
			break;
		case ACK_radarconfig:
			ackradarconfig(psendBuf);
			break;
		case ACK_trackconfig:
			acktrackconfig(psendBuf);
			break;
		default:
			break;
	}
	return 0;
}

void CEventParsing::mouseevent(int event)
{
	if(event==Status::MOUSEBUTTON)
		{
			if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->mouseleftright)
				Status::getinstance()->mouseleftright=_globalDate->rcvBufQue.at(6);
			if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->mousebuttonstaus)
				Status::getinstance()->mousebuttonstaus=_globalDate->rcvBufQue.at(7);
			Status::getinstance()->mousex=_globalDate->rcvBufQue.at(8)<<8|_globalDate->rcvBufQue.at(9);
			Status::getinstance()->mousey=_globalDate->rcvBufQue.at(10)<<8|_globalDate->rcvBufQue.at(11);

			
			OSA_printf("%s the x1=%d x2=%d y1=%d y2=%d \n",__func__,_globalDate->rcvBufQue.at(8),_globalDate->rcvBufQue.at(9),
				_globalDate->rcvBufQue.at(10),_globalDate->rcvBufQue.at(11));
			OSA_printf("%s the x=%d y=%d \n",__func__,Status::getinstance()->mousex,Status::getinstance()->mousey);
			
			pM->MSGDRIV_send(MSGID_EXT_INPUT_MouseEvent, (void *)(Status::MOUSEBUTTON));
		}
	else if(event==Status::MOUSEROLLER)
		{
			Status::getinstance()->setrigion(_globalDate->rcvBufQue.at(5));
			Status::getinstance()->setzoomstat(_globalDate->rcvBufQue.at(6));
			pM->MSGDRIV_send(MSGID_EXT_INPUT_MouseEvent, (void *)(Status::MOUSEROLLER));
		}
}
void CEventParsing::displaymod()
{
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->getdisplaymod())
	{
		Status::getinstance()->setdisplaymod(_globalDate->rcvBufQue.at(5));
	}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->getdisplaysensor())
	{
		Status::getinstance()->setdisplaysensor(_globalDate->rcvBufQue.at(6));
	}
		

	if(Status::getinstance()->getdisplaymod()==Status::LIVEMOD)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_DISMOD, (void *)(Status::LIVEMOD));
	else if(Status::getinstance()->getdisplaymod()==Status::PLAYCALLBACK)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_DISMOD, (void *)(Status::PLAYCALLBACK));
}

void CEventParsing::workMode()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 2)
		return ;

	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->getworkmod())
	{
		Status::getinstance()->setworkmod(_globalDate->rcvBufQue.at(5));
		if(Status::getinstance()->getworkmod()==0)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_WorkModeCTRL, (void *)(Status::PANOAUTO));
		else if(Status::getinstance()->getworkmod()==1)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_WorkModeCTRL, (void *)(Status::PANOPTZ));
		else if(Status::getinstance()->getworkmod()==2)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_WorkModeCTRL, (void *)(Status::PANOSELECT));

		//PANOAUTO
	}

}

void CEventParsing::StoreMode(int mod)
{
	if(mod==Status::STOREGO)
	{
		if(Status::getinstance()->getstoremod()!=mod)
			Status::getinstance()->setstoremod(mod);

		if(Status::getinstance()->storegonum!=_globalDate->rcvBufQue.at(5))
			Status::getinstance()->storegonum=_globalDate->rcvBufQue.at(5);
		
		
		OSA_printf("the mod=%d storegonum=%d \n ",Status::getinstance()->getstoremod(),Status::getinstance()->storegonum);
		
		pM->MSGDRIV_send(MSGID_EXT_INPUT_StoreMod, (void *)(Status::STOREGO));
	}
	else if(mod==Status::STORESAVE)
	{
		if(Status::getinstance()->getstoremod()!=mod)
			Status::getinstance()->setstoremod(mod);
		
		if(Status::getinstance()->storesavenum!=_globalDate->rcvBufQue.at(5))
			Status::getinstance()->storesavenum=_globalDate->rcvBufQue.at(5);
		
		if(Status::getinstance()->storesavemod!=_globalDate->rcvBufQue.at(6))
			Status::getinstance()->storesavemod=_globalDate->rcvBufQue.at(6);
		
		OSA_printf("the mod=%d savenum=%d mod=%d\n ",Status::getinstance()->getstoremod(),Status::getinstance()->storesavenum,Status::getinstance()->storesavemod);

		pM->MSGDRIV_send(MSGID_EXT_INPUT_StoreMod, (void *)(Status::STORESAVE));
	}
};

void CEventParsing::updatepano()
{
	pM->MSGDRIV_send(MSGID_EXT_INPUT_Updatapano, 0);
}

void CEventParsing::ZoomCtrl()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 2)
		return ;

	int focallength = _globalDate->rcvBufQue.at(5);
	if(0 == focallength)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCALLENGTHCTRL, (void *)(Status::PTZFOCUSLENGTHSTOP));
	else if(1 == focallength)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCALLENGTHCTRL, (void *)(Status::PTZFOCUSLENGTHOWN));
	else if(2 == focallength)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCALLENGTHCTRL, (void *)(Status::PTZFOCUSLENGTHUP));
	
    return ;
}

void CEventParsing::IrisCtrl()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 2)
		return ;

	int iris = _globalDate->rcvBufQue.at(5);
	if(0 == iris)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_IRISCTRL, (void *)(Status::PTZIRISSTOP));
	else if(1 == iris)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_IRISCTRL, (void *)(Status::PTZIRISDOWN));
	else if(2 == iris)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_IRISCTRL, (void *)(Status::PTZIRISUP));
}

void CEventParsing::FocusCtrl()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 2)
		return ;

	int focus = _globalDate->rcvBufQue.at(5);
	if(0 == focus)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCUSCTRL, (void *)(Status::PTZFOCUSSTOP));
	else if(1 == focus)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCUSCTRL, (void *)(Status::PTZFOCUSSNEAR));
	else if(2 == focus)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCUSCTRL, (void *)(Status::PTZFOCUSSFAR));
}

void CEventParsing::plantctl()
{
	int mod=0;
	int mod1=0;
	int change=0;

	Status::getinstance()->ptzpanodirection=_globalDate->rcvBufQue.at(5);
	Status::getinstance()->ptztitledirection=_globalDate->rcvBufQue.at(7);
	Status::getinstance()->ptzpanspeed=_globalDate->rcvBufQue.at(6);
	Status::getinstance()->ptztitlespeed=_globalDate->rcvBufQue.at(8);
	
	pM->MSGDRIV_send(MSGID_EXT_INPUT_PLATCTRL, 0);
}

void CEventParsing::GetsoftVersion()
{
	GetSoftWareBuildTargetTime();
	pM->MSGDRIV_send(MSGID_EXT_INPUT_Getversion, 0);
	_globalDate->feedback=ACK_softVersion;
	OSA_semSignal(&_globalDate->m_semHndl);
}

void CEventParsing::GetSoftWareBuildTargetTime(void)
{
    char arrDate[20]; //Jul 03 2018
    char arrTime[20]; //06:17:05
    char pDest[20];

    _globalDate->softversion;
    
   // RTC_TIME_DEF stTime;

    sprintf(arrDate,"%s",__DATE__);//Jul 03 2018
    sprintf(arrTime,"%s",__TIME__);//06:17:05
    
    //char *strncpy(char *dest, const char *src, int n)
    //(char*)(&(pDest[0])) = myStrncpy(pDest, arrDate, 3);
    sprintf(pDest, "%s", myStrncpy(pDest, arrDate, 3));

    if (strcmp(pDest, "Jan") == 0) _globalDate->softversion.mon= 1;
    else if (strcmp(pDest, "Feb") == 0) _globalDate->softversion.mon= 2;
    else if (strcmp(pDest, "Mar") == 0) _globalDate->softversion.mon = 3;
    else if (strcmp(pDest, "Apr") == 0) _globalDate->softversion.mon = 4;
    else if (strcmp(pDest, "May") == 0) _globalDate->softversion.mon = 5;
    else if (strcmp(pDest, "Jun") == 0) _globalDate->softversion.mon = 6;
    else if (strcmp(pDest, "Jul") == 0) _globalDate->softversion.mon = 7;
    else if (strcmp(pDest, "Aug") == 0) _globalDate->softversion.mon = 8;
    else if (strcmp(pDest, "Sep") == 0) _globalDate->softversion.mon = 9;
    else if (strcmp(pDest, "Oct") == 0) _globalDate->softversion.mon = 10;
    else if (strcmp(pDest, "Nov") == 0) _globalDate->softversion.mon = 11;
    else if (strcmp(pDest, "Dec") == 0) _globalDate->softversion.mon = 12;
    else _globalDate->softversion.mon = 1;

	


     sprintf(pDest, "%s", myStrncpy(pDest, arrDate+4, 2));
    //int atoi(const char *nptr);
   _globalDate->softversion.day= atoi(pDest);
    sprintf(pDest, "%s", myStrncpy(pDest, arrDate + 4 + 3, 4));
    //int atoi(const char *nptr);
    _globalDate->softversion.year= atoi(pDest);

    _globalDate->softversion.year=_globalDate->softversion.year%100;
    //time
    
    sprintf(pDest, "%s", myStrncpy(pDest, arrTime, 2));
   _globalDate->softversion.hour= atoi(pDest);
    sprintf(pDest, "%s", myStrncpy(pDest, arrTime+3, 2));
    _globalDate->softversion.min= atoi(pDest);
    sprintf(pDest, "%s", myStrncpy(pDest, arrTime + 3 + 3, 2));
    _globalDate->softversion.sec= atoi(pDest);

	_globalDate->softversion.major=MAJORVERSION;
	_globalDate->softversion.secmajor=MAJORVERSION;
	_globalDate->softversion.testversion=MAJORVERSION;
	_globalDate->softversion.softstage=0;
    return ;
}

char *CEventParsing::myStrncpy(char *dest, const char *src, int n) 
{
    int size = sizeof(char)*(n + 1);
    char *tmp = (char*)malloc(size);  
    if (tmp) 
    {
        memset(tmp, '\0', size); 
        memcpy(tmp, src, size - 1);
        memcpy(dest, tmp, size);
        free(tmp);
        return dest;
    }
    else 
    {
        return NULL;
    }
}

void CEventParsing::ConfigCurrentSave()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 1)
		return ;

	Status::getinstance()->saveCurrentConfig(0);
}

void CEventParsing::ConfigLoadDefault()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 1)
		return ;

	Status::getinstance()->resetDefaultConfig(0);
}

void CEventParsing::choosedev()
{
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->getdevid())
		Status::getinstance()->setdevid(_globalDate->rcvBufQue.at(5));
	else
		return;

	if(1==Status::getinstance()->getdevid())
		pM->MSGDRIV_send(MSGID_EXT_CHOOSEDEV, (void *)(Status::DEV1));
	else if(2==Status::getinstance()->getdevid())
		pM->MSGDRIV_send(MSGID_EXT_CHOOSEDEV, (void *)(Status::DEV2));
	else if(3==Status::getinstance()->getdevid())
		pM->MSGDRIV_send(MSGID_EXT_CHOOSEDEV, (void *)(Status::DEV3));

}

void CEventParsing::chooseptz()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 2)
		return ;

	int ptzid = _globalDate->rcvBufQue.at(5);
	if(1 == ptzid)
		pM->MSGDRIV_send(MSGID_EXT_CHOOSEPTZ, (void *)(Status::PTZ_SCAN));
	else if(2 == ptzid)
		pM->MSGDRIV_send(MSGID_EXT_CHOOSEPTZ, (void *)(Status::PTZ_TRK));
}

void CEventParsing::playercontrl()
{
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->playercontrl)
	{
		Status::getinstance()->playercontrl=_globalDate->rcvBufQue.at(5);
	}

	if(Status::getinstance()->playercontrl==Status::PLAYERSTOP)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerCtl, (void *)(Status::PLAYERSTOP));
	if(Status::getinstance()->playercontrl==Status::PLAYERRUN)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerCtl, (void *)(Status::PLAYERRUN));
	if(Status::getinstance()->playercontrl==Status::PLAYERACC)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerCtl, (void *)(Status::PLAYERDEC));
	if(Status::getinstance()->playercontrl==Status::PLAYERDEC)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerCtl, (void *)(Status::PLAYERACC));			
}

void CEventParsing::playerselect()
{
	if((_globalDate->rcvBufQue.at(5)<<8|_globalDate->rcvBufQue.at(6))!=Status::getinstance()->playeryear)
	{
		Status::getinstance()->playeryear=_globalDate->rcvBufQue.at(5)<<8|_globalDate->rcvBufQue.at(6);
	}
	if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->playermonth)
	{
		Status::getinstance()->playermonth=_globalDate->rcvBufQue.at(7);
	}
	if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->playerday)
	{
		Status::getinstance()->playerday=_globalDate->rcvBufQue.at(8);
	}
	if(_globalDate->rcvBufQue.at(9)!=Status::getinstance()->playerhour)
	{
		Status::getinstance()->playerhour=_globalDate->rcvBufQue.at(9);
	}
	if(_globalDate->rcvBufQue.at(10)!=Status::getinstance()->playermin)
	{
		Status::getinstance()->playermin=_globalDate->rcvBufQue.at(10);
	}
	if(_globalDate->rcvBufQue.at(11)!=Status::getinstance()->playersec)
	{
		Status::getinstance()->playersec=_globalDate->rcvBufQue.at(11);
	}

	OSA_printf("%s the h=%d m=%d s=%d\n",__func__,Status::getinstance()->playerhour,Status::getinstance()->playermin,Status::getinstance()->playersec);

	pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerSelect, 0);		
}

void CEventParsing::playerquery()
{
	if((_globalDate->rcvBufQue.at(5)<<8|_globalDate->rcvBufQue.at(6))!=Status::getinstance()->playerqueryyear)
	{
		Status::getinstance()->playerqueryyear=_globalDate->rcvBufQue.at(5)<<8|_globalDate->rcvBufQue.at(6);
	}
	if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->playerquerymon)
	{
		Status::getinstance()->playerquerymon=_globalDate->rcvBufQue.at(7);
	}
	if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->playerqueryday)
	{
		Status::getinstance()->playerqueryday=_globalDate->rcvBufQue.at(8);
	}

	OSA_printf("%s the year=%d mon=%d day=%d\n",__func__,Status::getinstance()->playerqueryyear,Status::getinstance()->playerquerymon,Status::getinstance()->playerqueryday);

	pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerQuerry, 0);
}

void CEventParsing::livevideo()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s livevideo start, cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 2)
		return ;

	if((_globalDate->rcvBufQue.at(5))!=Status::getinstance()->livevideoflg)
	{
		Status::getinstance()->livevideoflg=_globalDate->rcvBufQue.at(5);
		pM->MSGDRIV_send(MSGID_EXT_INPUT_LIVEVIDEO, 0);
	}
}

void CEventParsing::livephoto()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 1)
		return ;

	pM->MSGDRIV_send(MSGID_EXT_INPUT_LIVEPHOTO, 0);
}

void CEventParsing::panoenable()
{
	if((_globalDate->rcvBufQue.at(5))!=Status::getinstance()->mvconfigenable)
		{
			Status::getinstance()->mvconfigenable=_globalDate->rcvBufQue.at(5);
		}
	//printf("Status::getinstance()->mvconfigenable=%d\n",Status::getinstance()->mvconfigenable);
	pM->MSGDRIV_send(MSGID_EXT_INPUT_MVCONFIGENABLE, 0);
}

void CEventParsing::videoclip()
{
	if((_globalDate->rcvBufQue.at(5))!=Status::getinstance()->videoclipflg)
	{
		Status::getinstance()->videoclipflg=_globalDate->rcvBufQue.at(5);
	}

	pM->MSGDRIV_send(MSGID_EXT_INPUT_VIDEOCLIP, 0);
}

void CEventParsing::sensortvconfig()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 13)
		return ;

	int configchange=0;
	int time;
	sensorcfg_t *pSenCfg = &(Status::getinstance()->sensorcfg[0]);
	if(_globalDate->rcvBufQue.at(5)!=pSenCfg->outputresol)
	{
		pSenCfg->outputresol=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=pSenCfg->brightness)
	{
		pSenCfg->brightness=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(7)!=pSenCfg->contract)
	{
		pSenCfg->contract=_globalDate->rcvBufQue.at(7);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(8)!=pSenCfg->autobright)
	{
		pSenCfg->autobright=_globalDate->rcvBufQue.at(8);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(9)!=pSenCfg->backlight)
	{
		pSenCfg->backlight=_globalDate->rcvBufQue.at(9);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(10)!=pSenCfg->whitebalance)
	{
		pSenCfg->whitebalance=_globalDate->rcvBufQue.at(10);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(11)!=pSenCfg->gain)
	{
		pSenCfg->gain=_globalDate->rcvBufQue.at(11);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(12)!=pSenCfg->daynight)
	{
		pSenCfg->daynight=_globalDate->rcvBufQue.at(12);
		configchange=1;
	}

	if(_globalDate->rcvBufQue.at(13)!=pSenCfg->stronglightsup)
	{
		pSenCfg->stronglightsup=_globalDate->rcvBufQue.at(13);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(14)!=pSenCfg->exposuremode)
	{
		pSenCfg->exposuremode=_globalDate->rcvBufQue.at(14);
		configchange=1;
	}

	time = _globalDate->rcvBufQue.at(15) | (_globalDate->rcvBufQue.at(16) << 8);
	if(time!=pSenCfg->elecshutter_time)
	{
		pSenCfg->elecshutter_time=time;
		configchange=1;
	}

	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_SensorTVConfig, 0);
}

void CEventParsing::sensortrkconfig()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 13)
		return ;

	int configchange=0;
	int time;
	sensorcfg_t *pSenCfg = &(Status::getinstance()->sensorcfg[2]);
	if(_globalDate->rcvBufQue.at(5)!=pSenCfg->outputresol)
	{
		pSenCfg->outputresol=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=pSenCfg->brightness)
	{
		pSenCfg->brightness=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(7)!=pSenCfg->contract)
	{
		pSenCfg->contract=_globalDate->rcvBufQue.at(7);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(8)!=pSenCfg->autobright)
	{
		pSenCfg->autobright=_globalDate->rcvBufQue.at(8);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(9)!=pSenCfg->backlight)
	{
		pSenCfg->backlight=_globalDate->rcvBufQue.at(9);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(10)!=pSenCfg->whitebalance)
	{
		pSenCfg->whitebalance=_globalDate->rcvBufQue.at(10);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(11)!=pSenCfg->gain)
	{
		pSenCfg->gain=_globalDate->rcvBufQue.at(11);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(12)!=pSenCfg->daynight)
	{
		pSenCfg->daynight=_globalDate->rcvBufQue.at(12);
		configchange=1;
	}

	if(_globalDate->rcvBufQue.at(13)!=pSenCfg->stronglightsup)
	{
		pSenCfg->stronglightsup=_globalDate->rcvBufQue.at(13);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(14)!=pSenCfg->exposuremode)
	{
		pSenCfg->exposuremode=_globalDate->rcvBufQue.at(14);
		configchange=1;
	}

	time = _globalDate->rcvBufQue.at(15) | (_globalDate->rcvBufQue.at(16) << 8);
	if(time!=pSenCfg->elecshutter_time)
	{
		pSenCfg->elecshutter_time=time;
		configchange=1;
	}

	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_SensorTRKConfig, 0);

}

void CEventParsing::scan_plantformconfig()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 6)
		return ;

	int configchange=0;
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->scan_platformcfg.address)
	{
		Status::getinstance()->scan_platformcfg.address=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->scan_platformcfg.protocol)
	{
		Status::getinstance()->scan_platformcfg.protocol=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->scan_platformcfg.baudrate)
	{
		Status::getinstance()->scan_platformcfg.baudrate=_globalDate->rcvBufQue.at(7);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->scan_platformcfg.start_signal)
	{
		Status::getinstance()->scan_platformcfg.start_signal=_globalDate->rcvBufQue.at(8);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(9)!=Status::getinstance()->scan_platformcfg.pt_check)
	{
		Status::getinstance()->scan_platformcfg.pt_check=_globalDate->rcvBufQue.at(9);
		configchange=1;
	}

	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_ScanPlantfromConfig, 0);
}

void CEventParsing::radarconfig()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 14)
		return ;

	int configchange=0;
	int offset50[2];
	int offset100[2];
	int offset300[2];
	int sensor=0;
	offset50[0] = _globalDate->rcvBufQue.at(6) | (_globalDate->rcvBufQue.at(7) << 8);
	offset100[0] = _globalDate->rcvBufQue.at(8) | (_globalDate->rcvBufQue.at(9) << 8);
	offset300[0] = _globalDate->rcvBufQue.at(10) | (_globalDate->rcvBufQue.at(11) << 8);
	offset50[1] = _globalDate->rcvBufQue.at(12) | (_globalDate->rcvBufQue.at(13) << 8);
	offset100[1] = _globalDate->rcvBufQue.at(14) | (_globalDate->rcvBufQue.at(15) << 8);
	offset300[1] = _globalDate->rcvBufQue.at(16) | (_globalDate->rcvBufQue.at(17) << 8);
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->radarcfg.hideline)
	{
		Status::getinstance()->radarcfg.hideline=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	for(sensor=0; sensor<2; sensor++)
	{
		if(offset50[sensor]!=Status::getinstance()->radarcfg.offset50m[sensor])
		{
			Status::getinstance()->radarcfg.offset50m[sensor]=offset50[sensor];
			configchange=1;
		}
		if(offset100[sensor]!=Status::getinstance()->radarcfg.offset100m[sensor])
		{
			Status::getinstance()->radarcfg.offset100m[sensor]=offset100[sensor];
			configchange=1;
		}
		if(offset300[sensor]!=Status::getinstance()->radarcfg.offset300m[sensor])
		{
			Status::getinstance()->radarcfg.offset300m[sensor]=offset300[sensor];
			configchange=1;
		}
	}

	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_RadarConfig, 0);
}

void CEventParsing::trackconfig()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 5)
		return ;

	int configchange=0;
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->trackcfg.trkpriodis)
	{
		Status::getinstance()->trackcfg.trkpriodis=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->trackcfg.trkpriobright)
	{
		Status::getinstance()->trackcfg.trkpriobright=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->trackcfg.trkpriosize)
	{
		Status::getinstance()->trackcfg.trkpriosize=_globalDate->rcvBufQue.at(7);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->trackcfg.trktime)
	{
		Status::getinstance()->trackcfg.trktime=_globalDate->rcvBufQue.at(8);
		configchange=1;
	}

	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_TrackConfig, 0);
}

void CEventParsing::adddevconfig()
{
	Status::getinstance()->adddevcfg.devid=_globalDate->rcvBufQue.at(5);
	for(int i = 0; i < 16; i++)
	{
		Status::getinstance()->adddevcfg.ip[i] = _globalDate->rcvBufQue.at(i+6);
	}
	
	pM->MSGDRIV_send(MSGID_EXT_INPUT_AdddevConfig, 0);
}

void CEventParsing::deletedevconfig()
{
	Status::getinstance()->deldevid=_globalDate->rcvBufQue.at(5);
	pM->MSGDRIV_send(MSGID_EXT_INPUT_DeldevConfig, 0);
}

void CEventParsing::trk_plantformconfig()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 5)
		return ;

	int configchange=0;
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->trk_platformcfg.address)
	{
		Status::getinstance()->trk_platformcfg.address=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->trk_platformcfg.protocol)
	{
		Status::getinstance()->trk_platformcfg.protocol=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->trk_platformcfg.baudrate)
	{
		Status::getinstance()->trk_platformcfg.baudrate=_globalDate->rcvBufQue.at(7);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->trk_platformcfg.ptzspeed)
	{
		Status::getinstance()->trk_platformcfg.ptzspeed=_globalDate->rcvBufQue.at(8);
		configchange=1;
	}

	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_PlantfromConfig, 0);
	
}

void CEventParsing::sensorfrconfig()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 10)
		return ;

	int configchange=0;
	sensorcfg_t *pSenCfg = &(Status::getinstance()->sensorcfg[1]);
	if(_globalDate->rcvBufQue.at(5)!=pSenCfg->brightness)
	{
		pSenCfg->brightness=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=pSenCfg->contract)
	{
		pSenCfg->contract=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(7)!=pSenCfg->autobright)
	{
		pSenCfg->autobright=_globalDate->rcvBufQue.at(7);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(8)!=pSenCfg->backandwrite)
	{
		pSenCfg->backandwrite=_globalDate->rcvBufQue.at(8);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(9)!=pSenCfg->correct)
	{
		pSenCfg->correct=_globalDate->rcvBufQue.at(9);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(10)!=pSenCfg->digitfilter)
	{
		pSenCfg->digitfilter=_globalDate->rcvBufQue.at(10);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(11)!=pSenCfg->digitenhance)
	{
		pSenCfg->digitenhance=_globalDate->rcvBufQue.at(11);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(12)!=pSenCfg->mirror)
	{
		pSenCfg->mirror=_globalDate->rcvBufQue.at(12);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(13)!=pSenCfg->outputresol)
	{
		pSenCfg->outputresol=_globalDate->rcvBufQue.at(13);
		configchange=1;
	}

	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_SensorFRConfig, 0);

}
 
void CEventParsing::zeroconfig()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 2)
		return ;

	int configchange=0;
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->zeromod)
	{
		Status::getinstance()->zeromod=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	OSA_printf("%s zeromod=%d\n",__func__,Status::getinstance()->zeromod);
	if(configchange)
	{
		if(Status::getinstance()->zeromod==0)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_ZeroConfig, (void *)Status::ZEROSTOP);
		else if(Status::getinstance()->zeromod==1)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_ZeroConfig, (void *)Status::ZEROSTART);
		else if(Status::getinstance()->zeromod==2)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_ZeroConfig, (void *)Status::ZEROSAVE);
	}
}

void CEventParsing::recordconfig()
{
		
		int configchange=1;
		
		int recordclass=_globalDate->rcvBufQue.at(5);
		int bitnum=8;
		
		for(int i=0;i<HELDWEEK;i++)
			{
				for(int j=0;j<HELDHOUR;j++)
					{
						//printf("%d \t",_globalDate->rcvBufQue.at(6+i*3));
						if(j<8)
							{
								Status::getinstance()->recordpositionheld[recordclass][i][bitnum-1-j]=(_globalDate->rcvBufQue.at(6+i*3)>>j)&0x01;
							//printf("test%d \t",Status::getinstance()->recordpositionheld[recordclass][i][bitnum-1-j]);
							}
						else if(j<16)
								Status::getinstance()->recordpositionheld[recordclass][i][2*bitnum-1-(j-8)]=(_globalDate->rcvBufQue.at(6+i*3+1)>>(j-bitnum))&0x01;
						else if(j<24)
							{
								Status::getinstance()->recordpositionheld[recordclass][i][3*bitnum-1-(j-16)]=(_globalDate->rcvBufQue.at(6+i*3+2)>>(j-2*bitnum))&0x01;
							//printf("test_%d \t",Status::getinstance()->recordpositionheld[recordclass][i][3*bitnum-1-(j-24)]);
							}
					}

			}
		
		/*
		printf("the time\n");
		for(int i=0;i<HELDWEEK;i++)
			{
				for(int j=0;j<HELDHOUR;j++)
					{
						if(j<8)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);
						else if(j<16)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);
						else if(j<24)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);					
					}
				printf("\n");

			}
		*/
			
		/*
		printf("the mov\n");
		for(int i=0;i<HELDWEEK;i++)
			{
				for(int j=0;j<HELDHOUR;j++)
					{
						if(j<8)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);
						else if(j<16)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);
						else if(j<24)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);

						
					}
				printf("\n");

			}
			*/
		if(configchange)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_RecordConfig,0);
}


void CEventParsing::movedetectconfig()
 {
 	int configchange=0;
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->mtdcfg.movedetectalgenable)
	{
		Status::getinstance()->mtdcfg.movedetectalgenable=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if((_globalDate->rcvBufQue.at(6))!=Status::getinstance()->mtdcfg.sensitivity)
	{
		Status::getinstance()->mtdcfg.sensitivity=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->mtdcfg.speedpriority)
	{
		Status::getinstance()->mtdcfg.speedpriority=_globalDate->rcvBufQue.at(7);
		configchange=1;
	}
	if((_globalDate->rcvBufQue.at(8)<<8|_globalDate->rcvBufQue.at(9))!=Status::getinstance()->mtdcfg.movmaxwidth)
	{
		Status::getinstance()->mtdcfg.movmaxwidth=_globalDate->rcvBufQue.at(8)<<8|_globalDate->rcvBufQue.at(9);
		configchange=1;
	}
	if((_globalDate->rcvBufQue.at(10)<<8|_globalDate->rcvBufQue.at(11))!=Status::getinstance()->mtdcfg.movmaxheight)
	{
		Status::getinstance()->mtdcfg.movmaxheight=_globalDate->rcvBufQue.at(10)<<8|_globalDate->rcvBufQue.at(11);
		configchange=1;
	}
	if((_globalDate->rcvBufQue.at(12)<<8|_globalDate->rcvBufQue.at(13))!=Status::getinstance()->mtdcfg.movminwidth)
	{
		Status::getinstance()->mtdcfg.movminwidth=_globalDate->rcvBufQue.at(12)<<8|_globalDate->rcvBufQue.at(13);
		configchange=1;
	}
	if((_globalDate->rcvBufQue.at(14)<<8|_globalDate->rcvBufQue.at(15))!=Status::getinstance()->mtdcfg.movminheight)
	{
		Status::getinstance()->mtdcfg.movminheight=_globalDate->rcvBufQue.at(14)<<8|_globalDate->rcvBufQue.at(15);
		configchange=1;
	}	

	if((_globalDate->rcvBufQue.at(16)<<8|_globalDate->rcvBufQue.at(17))!=Status::getinstance()->mtdcfg.moverecordtime)
	{
		Status::getinstance()->mtdcfg.moverecordtime=_globalDate->rcvBufQue.at(16)<<8|_globalDate->rcvBufQue.at(17);
		configchange=1;
	}

	OSA_printf("movedetectalgenable=%d  sensitivity=%d  speedpriority=%d movminwidth=%d movminheight=%d moverecordtime=%d\n",Status::getinstance()->mtdcfg.movedetectalgenable,Status::getinstance()->mtdcfg.sensitivity,Status::getinstance()->mtdcfg.speedpriority,\
	Status::getinstance()->mtdcfg.movminwidth,Status::getinstance()->mtdcfg.movminheight,Status::getinstance()->mtdcfg.moverecordtime);

	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_MoveDetectConfig,0);
		
 }

 void CEventParsing::movedetectareaconfig()
 {
	int configchange=1;
 	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->detectareanum+1)
	{
		Status::getinstance()->detectareanum=_globalDate->rcvBufQue.at(5)-1;
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->detectareaenable)
	{
		Status::getinstance()->detectareaenable=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_MoveDetectAreaConfig,0);
 }

 void CEventParsing::displayconfig()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 2)
		return ;

	int configchange=0;
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->displayresolution)
	{
		Status::getinstance()->displayresolution=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}

	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_DisplayConfig,0);
}

 void CEventParsing::correcttimeconfig()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 8)
		return ;

	int configchange=0;
	correcttime_t correct_tmp;
	correct_tmp.year = (_globalDate->rcvBufQue.at(5)<<8|_globalDate->rcvBufQue.at(6));
	correct_tmp.mon = (_globalDate->rcvBufQue.at(7));
	correct_tmp.day = _globalDate->rcvBufQue.at(8);
	correct_tmp.hour = _globalDate->rcvBufQue.at(9);
	correct_tmp.min = _globalDate->rcvBufQue.at(10);
	correct_tmp.sec = _globalDate->rcvBufQue.at(11);
	if(correct_tmp.year!=Status::getinstance()->correcttime.year)
	{
		Status::getinstance()->correcttime.year=correct_tmp.year;
		configchange=1;
	}
	if(correct_tmp.mon!=Status::getinstance()->correcttime.mon)
	{
		Status::getinstance()->correcttime.mon=correct_tmp.mon;
		configchange=1;
	}
	if(correct_tmp.day!=Status::getinstance()->correcttime.day)
	{
		Status::getinstance()->correcttime.day=correct_tmp.day;
		configchange=1;
	}
	if(correct_tmp.hour!=Status::getinstance()->correcttime.hour)
	{
		Status::getinstance()->correcttime.hour=correct_tmp.hour;
		configchange=1;
	}
	if(correct_tmp.min!=Status::getinstance()->correcttime.min)
	{
		Status::getinstance()->correcttime.min=correct_tmp.min;
		configchange=1;
	}
	if(correct_tmp.sec!=Status::getinstance()->correcttime.sec)
	{
		Status::getinstance()->correcttime.sec=correct_tmp.sec;
		configchange=1;
	}

	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_CorrectTimeConfig,0);

}

 void CEventParsing::panoconfig()
{
	int cmdLen = (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8);
	//printf("%s cmdlen=%d\n",__func__,cmdLen);
	if(cmdLen != 9)
		return ;

	int configchange=0;
	int ptzspeed[2];
	int piexfocus[2];
	int circlefps[2];
	int sensor=0;
	ptzspeed[0]=_globalDate->rcvBufQue.at(5);
	piexfocus[0]=_globalDate->rcvBufQue.at(6)<<8|_globalDate->rcvBufQue.at(7);
	circlefps[0]=_globalDate->rcvBufQue.at(8);
	ptzspeed[1]=_globalDate->rcvBufQue.at(9);
	piexfocus[1]=_globalDate->rcvBufQue.at(10)<<8|_globalDate->rcvBufQue.at(11);
	circlefps[1]=_globalDate->rcvBufQue.at(12);
	for(sensor=0; sensor<2; sensor++)
	{
		if(ptzspeed[sensor]!=Status::getinstance()->panocfg.ptzspeed[sensor])
		{
			Status::getinstance()->panocfg.ptzspeed[sensor]=ptzspeed[sensor];
			configchange=1;
		}
		if(piexfocus[sensor]!=Status::getinstance()->panocfg.piexfocus[sensor])
		{
			Status::getinstance()->panocfg.piexfocus[sensor]=piexfocus[sensor];
			configchange=1;
		}
		if(circlefps[sensor]!=Status::getinstance()->panocfg.circlefps[sensor])
		{
			Status::getinstance()->panocfg.circlefps[sensor]=circlefps[sensor];
			configchange=1;
		}
	}

	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_PanoConfig,0);

}

 void  CEventParsing::rebootconfig()
 {
 	printf("%s\n",__func__);
 	char cmdBuf[128];
	sprintf(cmdBuf, "reboot");
	system(cmdBuf);

 }

 void CEventParsing::querryconfig()
 {
	int configchange=0;
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->querryconfig)
	{
		Status::getinstance()->querryconfig=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}

	switch(Status::getinstance()->querryconfig)
	{
		case Status::CFG_TRKPLARFORM:
			CGlobalDate::Instance()->feedback=ACK_plantformconfig;
			OSA_semSignal(&_globalDate->m_semHndl);	
			break;

		case Status::CFG_SENSORFR:
			CGlobalDate::Instance()->feedback=ACK_sensorfrconfig;
			OSA_semSignal(&_globalDate->m_semHndl);	
			break;
		case Status::CFG_PLAYBACK:
			CGlobalDate::Instance()->feedback=ACK_recordconfig;
			OSA_semSignal(&_globalDate->m_semHndl);	
			break;
		case Status::CFG_MTD:
			CGlobalDate::Instance()->feedback=ACK_mvconfig;
			OSA_semSignal(&_globalDate->m_semHndl);	
			break;
		case Status::CFG_OUTPUTRESOL:
			CGlobalDate::Instance()->feedback=ACK_displayconfig;
			OSA_semSignal(&_globalDate->m_semHndl);
			break;
		case Status::CFG_VIDEO:
			CGlobalDate::Instance()->feedback=ACK_recordconfigmv;
			OSA_semSignal(&_globalDate->m_semHndl);	
			break;
		case Status::CFG_MONTAGE:
			CGlobalDate::Instance()->feedback=ACK_panoconfig;
			OSA_semSignal(&_globalDate->m_semHndl);	
			break;
		case Status::CFG_SCANPLATFORM:
			CGlobalDate::Instance()->feedback=ACK_scanplantformconfig;
			OSA_semSignal(&_globalDate->m_semHndl);	
			break;
		case Status::CFG_SENSORTV:
			CGlobalDate::Instance()->feedback=ACK_sensortvconfig;
			OSA_semSignal(&_globalDate->m_semHndl);	
			break;
		case Status::CFG_SENSORTRK:
			CGlobalDate::Instance()->feedback=ACK_sensortrkconfig;
			OSA_semSignal(&_globalDate->m_semHndl);
			break;
		case Status::CFG_SENSORRADAR:
			CGlobalDate::Instance()->feedback=ACK_radarconfig;
			OSA_semSignal(&_globalDate->m_semHndl);
			break;
		case Status::CFG_TRK:
			CGlobalDate::Instance()->feedback=ACK_trackconfig;
			OSA_semSignal(&_globalDate->m_semHndl);
			break;
		default:
			break;
		}
}

 void  CEventParsing:: softVersion(sendInfo * spBuf)
{
	int msg_length = 11;
	u_int8_t  mainVedioChannel[msg_length+5];
	mainVedioChannel[4] = 0x30;
	mainVedioChannel[5]=_globalDate->softversion.major;
	mainVedioChannel[6]=_globalDate->softversion.secmajor;
	mainVedioChannel[7]=_globalDate->softversion.testversion;
	mainVedioChannel[8]=_globalDate->softversion.year;
	mainVedioChannel[9]=_globalDate->softversion.mon;
	mainVedioChannel[10]=_globalDate->softversion.day;
	mainVedioChannel[11]=_globalDate->softversion.hour;
	mainVedioChannel[12]=_globalDate->softversion.min;
	mainVedioChannel[13]=_globalDate->softversion.sec;
	mainVedioChannel[14]=_globalDate->softversion.softstage;
	
	//mainVedioChannel[5]=_globalDate->avt_status.SensorStat;
	package_frame(msg_length, mainVedioChannel);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,mainVedioChannel,msg_length+5);

}

void  CEventParsing:: mainVedioChannel(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  mainVedioChannel[msg_length+5];
	mainVedioChannel[4] = 0x04;
	//mainVedioChannel[5]=_globalDate->avt_status.SensorStat;
	package_frame(msg_length, mainVedioChannel);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,mainVedioChannel,msg_length+5);
}

void  CEventParsing:: ack_updatepano(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  restartCheckAnswer[msg_length+5];
	restartCheckAnswer[4] = 0x09;
	restartCheckAnswer[5]=Status::getinstance()->ack_updatepano;
	package_frame(msg_length, restartCheckAnswer);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,restartCheckAnswer,msg_length+5);
}

void  CEventParsing:: ack_fullscreenmode(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  retrackStatus[msg_length+5];

	retrackStatus[4] = 0x0a;
	retrackStatus[5]=Status::getinstance()->ack_fullscreenmode;
	package_frame(msg_length, retrackStatus);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,retrackStatus,msg_length+5);
}

void  CEventParsing:: trackErrOutput(sendInfo * spBuf)
{
	int msg_length = 5;
	u_int8_t  retrackErrOutput[msg_length+5];
	retrackErrOutput[4] = 0x08;
	retrackErrOutput[5]= _globalDate->errorOutPut[0];
	retrackErrOutput[5] = retrackErrOutput[5]&0xff;

	retrackErrOutput[6]= _globalDate->errorOutPut[0];
	retrackErrOutput[6] = (retrackErrOutput[6]>>8)&0xff;

	retrackErrOutput[7]= _globalDate->errorOutPut[1];
	retrackErrOutput[7] = retrackErrOutput[7]&0xff;

	retrackErrOutput[8]= _globalDate->errorOutPut[1];
	retrackErrOutput[8] = (retrackErrOutput[8]>>8)&0xff;

	package_frame(msg_length, retrackErrOutput);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,retrackErrOutput,msg_length+5);
}

void  CEventParsing:: mutilTargetNoticeStatus(sendInfo * spBuf)
{
	int msg_length = 2;
	channelTable    mutilTargetSta;
	u_int8_t  remutilTargetNoticeStatus[msg_length+5];
	/*
	mutilTargetSta.channel0=_globalDate->avt_status.MtdState[0];
	mutilTargetSta.channel1=_globalDate->avt_status.MtdState[1];
	mutilTargetSta.channel2=_globalDate->avt_status.MtdState[2];
	mutilTargetSta.channel3=_globalDate->avt_status.MtdState[3];
	mutilTargetSta.channel4=_globalDate->avt_status.MtdState[4];
	mutilTargetSta.channel5=_globalDate->avt_status.MtdState[5];
	*/
	remutilTargetNoticeStatus[4] = 0x09;
	remutilTargetNoticeStatus[5]=*(u_int8_t*)(&mutilTargetSta);
	package_frame(msg_length, remutilTargetNoticeStatus);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,remutilTargetNoticeStatus,msg_length+5);
}

void CEventParsing::multilTargetNumSelectStatus(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  tmpbuf[msg_length+5];
	tmpbuf[4] = 0x0a;
	tmpbuf[5]=(u_int8_t) (_globalDate->mainProStat[ACK_mmtSelect_value]&0xff);
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void  CEventParsing::imageEnhanceStatus(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  tmpbuf[msg_length+5];
	channelTable    enhanceChannelSta;
	/*
	enhanceChannelSta.channel0=_globalDate->avt_status.ImgEnhStat[0];
	enhanceChannelSta.channel1=_globalDate->avt_status.ImgEnhStat[1];
	enhanceChannelSta.channel2=_globalDate->avt_status.ImgEnhStat[2];
	enhanceChannelSta.channel3=_globalDate->avt_status.ImgEnhStat[3];
	enhanceChannelSta.channel4=_globalDate->avt_status.ImgEnhStat[4];
	enhanceChannelSta.channel5=_globalDate->avt_status.ImgEnhStat[5];
	*/
	tmpbuf[4] = 0x0b;
	tmpbuf[5]=*(u_int8_t*)(&enhanceChannelSta);;
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CEventParsing::moveTargetDetectedStat(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  tmpbuf[msg_length+5];
	channelTable    currtmoveTargetDet;
	/*
	currtmoveTargetDet.channel0=_globalDate->avt_status.MtdState[0];
	currtmoveTargetDet.channel1=_globalDate->avt_status.MtdState[1];
	currtmoveTargetDet.channel2=_globalDate->avt_status.MtdState[2];
	currtmoveTargetDet.channel3=_globalDate->avt_status.MtdState[3];
	currtmoveTargetDet.channel4=_globalDate->avt_status.MtdState[4];
	currtmoveTargetDet.channel5=_globalDate->avt_status.MtdState[5];
	*/
	tmpbuf[4] = 0x10;
	tmpbuf[5]=*(u_int8_t*)(&currtmoveTargetDet);
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CEventParsing::trackSearchStat(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  retrackSearchStat[msg_length+5];
	retrackSearchStat[4] = 0x0f;
	unsigned int sectrkstat = 0;//_globalDate->avt_status.AvtTrkStat;
	if(sectrkstat == 4)
		sectrkstat = 1;
	else
		sectrkstat = 2;
	retrackSearchStat[5] = sectrkstat;
	package_frame(msg_length, retrackSearchStat);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,retrackSearchStat,msg_length+5);
}

void CEventParsing::trackFinetuningStat(sendInfo * spBuf)
{
	int msg_length = 3;
	u_int8_t  retrackFinetuningStat[msg_length+5];
	retrackFinetuningStat[4] = 0x0c;
	retrackFinetuningStat[5] = (u_int8_t) (_globalDate->mainProStat[ACK_posMove_value]&0xff);
	retrackFinetuningStat[6] = (u_int8_t) (_globalDate->mainProStat[ACK_posMove_value+1]&0xff);
	package_frame(msg_length, retrackFinetuningStat);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,retrackFinetuningStat,msg_length+5);
}

void CEventParsing::confirmAxisStat(sendInfo * spBuf)
{
	int msg_length = 3;
	u_int8_t  tmpbuf[msg_length+5];
	tmpbuf[4] = 0x0d;
	tmpbuf[5]=(u_int8_t) (_globalDate->mainProStat[ACK_moveAxis_value]&0xff);
	tmpbuf[6]=(u_int8_t) (_globalDate->mainProStat[ACK_moveAxis_value+1]&0xff);
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CEventParsing::ElectronicZoomStat(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  tmpbuf[msg_length+5];
	tmpbuf[4] = 0x0e;
	tmpbuf[5]=(u_int8_t) (_globalDate->mainProStat[ACK_ElectronicZoom_value]&0xff);
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CEventParsing::settingCmdRespon(sendInfo * spBuf)
{
	int msg_length = 7;
	u_int8_t  tmpbuf[msg_length+5];
	tmpbuf[4] = 0x30;
	tmpbuf[5] = (u_int8_t) (_globalDate->mainProStat[ACK_config_Wblock]&0xff);
	tmpbuf[6] = (u_int8_t) (_globalDate->mainProStat[ACK_config_Wfield]&0xff);
	memcpy(tmpbuf+7,&(_globalDate->Host_Ctrl[config_Wvalue]),4);
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CEventParsing::readConfigSetting(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  readCfgSetting[8];
	float tmp ;
	int msglen = 0x07;
	spBuf->byteSizeSend = 0x0c;

	int len ;
	if(_globalDate->choose){
		len = strlen(_globalDate->ACK_read_content);
		msglen = msglen -4 + len;
		spBuf->byteSizeSend = msglen + 5;
	}

	spBuf->sendBuff[0] = 0xEB;
	spBuf->sendBuff[1] = 0x53;
	spBuf->sendBuff[2]= msglen&0xff;
	spBuf->sendBuff[3]= (msglen>>8)&0xff;
	spBuf->sendBuff[4]= 0x31;
	spBuf->sendBuff[5]= (u_int8_t) (_globalDate->mainProStat[ACK_config_Rblock]&0xff);
	spBuf->sendBuff[6]= (u_int8_t) (_globalDate->mainProStat[ACK_config_Rfield]&0xff);

	if(_globalDate->choose){
		memcpy((void*)&spBuf->sendBuff[7],(void*)_globalDate->ACK_read_content,len);
		sumCheck=sendCheck_sum(msglen+3, spBuf->sendBuff+1);
		spBuf->sendBuff[msglen+4]=(sumCheck&0xff);
		_globalDate->choose = 0;
	}else{
		tmp = _globalDate->ACK_read[0];
		printf("%s: %d      block = %d , field = %d ,value = %f \n",__func__,__LINE__,spBuf->sendBuff[5],spBuf->sendBuff[6],tmp);
		memcpy(spBuf->sendBuff+7,&tmp,4);
		sumCheck=sendCheck_sum(msglen+3, spBuf->sendBuff+1);
		spBuf->sendBuff[msglen+4]=(sumCheck&0xff);
		_globalDate->ACK_read.erase(_globalDate->ACK_read.begin());
	}

	//printf("read <=====> is  back\n");
}

void CEventParsing::extExtraInputResponse(sendInfo * spBuf)
{

}

void  CEventParsing:: upgradefwStat(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t trackStatus[3];
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=0x03;
	spBuf->sendBuff[3]=0x00;
	spBuf->sendBuff[4]=0x35;
	spBuf->sendBuff[5]=_globalDate->respupgradefw_stat;
	spBuf->sendBuff[6]=_globalDate->respupgradefw_perc;
	sumCheck=sendCheck_sum(6,spBuf->sendBuff+1);
	spBuf->sendBuff[7]=(sumCheck&0xff);
	spBuf->byteSizeSend=0x08;
}

void  CEventParsing:: paramtodef(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x53;
	spBuf->sendBuff[2]=0x02;
	spBuf->sendBuff[3]=0x00;
	spBuf->sendBuff[4]=0x0d;
	spBuf->sendBuff[5]= (u_int8_t) (_globalDate->mainProStat[ACK_config_Rblock]&0xff);
	sumCheck=sendCheck_sum(5,spBuf->sendBuff+1);
	spBuf->sendBuff[6]=(sumCheck&0xff);
	spBuf->byteSizeSend=0x07;
}

void  CEventParsing:: recordquerry(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=15*_globalDate->querrytime.size()+15*_globalDate->querrylivetime.size()+2;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_playerquerry;
	int i = 0;
	int j = _globalDate->querrytime.size();
	for(i = 0; i < j; i++)
	{
		spBuf->sendBuff[5+i*15]=1;
		spBuf->sendBuff[6+i*15]=(_globalDate->querrytime[i].startyear>>8)&0xff;
		spBuf->sendBuff[7+i*15]=_globalDate->querrytime[i].startyear&0xff;
		spBuf->sendBuff[8+i*15]=_globalDate->querrytime[i].startmon;
		spBuf->sendBuff[9+i*15]=_globalDate->querrytime[i].startday;
		spBuf->sendBuff[10+i*15]=_globalDate->querrytime[i].starthour;
		spBuf->sendBuff[11+i*15]=_globalDate->querrytime[i].startmin;
		spBuf->sendBuff[12+i*15]=_globalDate->querrytime[i].startsec;
		spBuf->sendBuff[13+i*15]=(_globalDate->querrytime[i].endyear>>8)&0xff;
		spBuf->sendBuff[14+i*15]=_globalDate->querrytime[i].endyear&0xff;
		spBuf->sendBuff[15+i*15]=_globalDate->querrytime[i].endmon;
		spBuf->sendBuff[16+i*15]=_globalDate->querrytime[i].endday;
		spBuf->sendBuff[17+i*15]=_globalDate->querrytime[i].endhour;
		spBuf->sendBuff[18+i*15]=_globalDate->querrytime[i].endtmin;
		spBuf->sendBuff[19+i*15]=_globalDate->querrytime[i].endsec;

	}

	for(i = j; i < j + _globalDate->querrylivetime.size(); i++)
	{
			spBuf->sendBuff[5+i*15]=2;
			spBuf->sendBuff[6+i*15]=(_globalDate->querrylivetime[i-j].startyear>>8)&0xff;
			spBuf->sendBuff[7+i*15]=_globalDate->querrylivetime[i-j].startyear&0xff;
			spBuf->sendBuff[8+i*15]=_globalDate->querrylivetime[i-j].startmon;
			spBuf->sendBuff[9+i*15]=_globalDate->querrylivetime[i-j].startday;
			spBuf->sendBuff[10+i*15]=_globalDate->querrylivetime[i-j].starthour;
			spBuf->sendBuff[11+i*15]=_globalDate->querrylivetime[i-j].startmin;
			spBuf->sendBuff[12+i*15]=_globalDate->querrylivetime[i-j].startsec;
			spBuf->sendBuff[13+i*15]=(_globalDate->querrylivetime[i-j].endyear>>8)&0xff;
			spBuf->sendBuff[14+i*15]=_globalDate->querrylivetime[i-j].endyear&0xff;
			spBuf->sendBuff[15+i*15]=_globalDate->querrylivetime[i-j].endmon;
			spBuf->sendBuff[16+i*15]=_globalDate->querrylivetime[i-j].endday;
			spBuf->sendBuff[17+i*15]=_globalDate->querrylivetime[i-j].endhour;
			spBuf->sendBuff[18+i*15]=_globalDate->querrylivetime[i-j].endtmin;
			spBuf->sendBuff[19+i*15]=_globalDate->querrylivetime[i-j].endsec;

		}
	//spBuf->sendBuff[5]= (u_int8_t) (_globalDate->mainProStat[ACK_config_Rblock]&0xff);
	sumCheck=sendCheck_sum(4+1+15*_globalDate->querrytime.size()+15*_globalDate->querrylivetime.size(),spBuf->sendBuff+1);
	spBuf->sendBuff[4+1+15*_globalDate->querrytime.size()+15*_globalDate->querrylivetime.size()+1]=(sumCheck&0xff);
	spBuf->byteSizeSend=4+1+15*_globalDate->querrytime.size()+15*_globalDate->querrylivetime.size()+2;
	printf("%s\n",__func__);
}

void  CEventParsing::ackplayertime(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=8;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_playertime;
	spBuf->sendBuff[5]=(Status::getinstance()->playertime.year>>8)&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->playertime.year&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->playertime.mon&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->playertime.day&0xff;
	spBuf->sendBuff[9]=Status::getinstance()->playertime.hour&0xff;
	spBuf->sendBuff[10]=Status::getinstance()->playertime.min&0xff;
	spBuf->sendBuff[11]=Status::getinstance()->playertime.sec&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void  CEventParsing::ackplantformconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=5;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_plantformconfig;
	spBuf->sendBuff[5]=Status::getinstance()->trk_platformcfg.address&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->trk_platformcfg.protocol&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->trk_platformcfg.baudrate&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->trk_platformcfg.ptzspeed&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
	printf("%s\n",__func__);
}

void  CEventParsing::acksensortvconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=13;
	int senId=0;
	
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_sensortvconfig;

	spBuf->sendBuff[5]=Status::getinstance()->sensorcfg[senId].outputresol&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->sensorcfg[senId].brightness&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->sensorcfg[senId].contract&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->sensorcfg[senId].autobright&0xff;
	spBuf->sendBuff[9]=Status::getinstance()->sensorcfg[senId].backlight&0xff;
	
	spBuf->sendBuff[10]=Status::getinstance()->sensorcfg[senId].whitebalance&0xff;
	spBuf->sendBuff[11]=Status::getinstance()->sensorcfg[senId].gain&0xff;
	spBuf->sendBuff[12]=Status::getinstance()->sensorcfg[senId].daynight&0xff;
	spBuf->sendBuff[13]=Status::getinstance()->sensorcfg[senId].stronglightsup&0xff;
	spBuf->sendBuff[14]=Status::getinstance()->sensorcfg[senId].exposuremode&0xff;
	spBuf->sendBuff[15]=Status::getinstance()->sensorcfg[senId].elecshutter_time&0xff;
	spBuf->sendBuff[16]=(Status::getinstance()->sensorcfg[senId].elecshutter_time>>8)&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void  CEventParsing::acksensortrkconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=13;
	int senId=2;
	
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_sensortrkconfig;

	spBuf->sendBuff[5]=Status::getinstance()->sensorcfg[senId].outputresol&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->sensorcfg[senId].brightness&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->sensorcfg[senId].contract&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->sensorcfg[senId].autobright&0xff;
	spBuf->sendBuff[9]=Status::getinstance()->sensorcfg[senId].backlight&0xff;
	
	spBuf->sendBuff[10]=Status::getinstance()->sensorcfg[senId].whitebalance&0xff;
	spBuf->sendBuff[11]=Status::getinstance()->sensorcfg[senId].gain&0xff;
	spBuf->sendBuff[12]=Status::getinstance()->sensorcfg[senId].daynight&0xff;
	spBuf->sendBuff[13]=Status::getinstance()->sensorcfg[senId].stronglightsup&0xff;
	spBuf->sendBuff[14]=Status::getinstance()->sensorcfg[senId].exposuremode&0xff;
	spBuf->sendBuff[15]=Status::getinstance()->sensorcfg[senId].elecshutter_time&0xff;
	spBuf->sendBuff[16]=(Status::getinstance()->sensorcfg[senId].elecshutter_time>>8)&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void  CEventParsing:: acksensorfrconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=10;
	int senId=1;
	
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_sensorfrconfig;
	
	spBuf->sendBuff[5]=Status::getinstance()->sensorcfg[senId].brightness&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->sensorcfg[senId].contract&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->sensorcfg[senId].autobright&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->sensorcfg[senId].backandwrite&0xff;
	
	spBuf->sendBuff[9]=Status::getinstance()->sensorcfg[senId].correct&0xff;
	spBuf->sendBuff[10]=Status::getinstance()->sensorcfg[senId].digitfilter&0xff;
	spBuf->sendBuff[11]=Status::getinstance()->sensorcfg[senId].digitenhance&0xff;
	spBuf->sendBuff[12]=Status::getinstance()->sensorcfg[senId].mirror&0xff;
	spBuf->sendBuff[13]=Status::getinstance()->sensorcfg[senId].outputresol&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
	printf("%s\n",__func__);
}

void  CEventParsing:: ackrecordconfig(sendInfo * spBuf,int classid)
{
	u_int8_t sumCheck;
	int infosize=23;
	//int classid=0;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_recordconfig;
	spBuf->sendBuff[5]=classid&0xff;
	
	for(int k=0;k<7;k++)
		{
			spBuf->sendBuff[6+k*3+0]=0;
			spBuf->sendBuff[6+k*3+1]=0;
			spBuf->sendBuff[6+k*3+2]=0;
			for(int i=0;i<24;i++)
				{
					if(i<8)
						spBuf->sendBuff[6+k*3+0]|=Status::getinstance()->recordpositionheld[classid][k][i]<<(7-i);
					else if(i<16)
						spBuf->sendBuff[6+k*3+1]|=Status::getinstance()->recordpositionheld[classid][k][i]<<(15-i);
					else if(i<24)
						spBuf->sendBuff[6+k*3+2]|=Status::getinstance()->recordpositionheld[classid][k][i]<<(23-i);
				}

		}
	
	printf("the classid=%d\n",classid);
	for(int i=0;i<HELDWEEK;i++)
		{
			for(int j=0;j<HELDHOUR;j++)
				{
					if(j<8)
						printf("%d \t",Status::getinstance()->recordpositionheld[classid][i][j]);
					else if(j<16)
						printf("%d \t",Status::getinstance()->recordpositionheld[classid][i][j]);
					else if(j<24)
						printf("%d \t",Status::getinstance()->recordpositionheld[classid][i][j]);					
				}
			printf("\n");

		}
	for(int i=0;i<21;i++)
		{
			printf("%d \t",spBuf->sendBuff[6+i]);

		}
	printf("\n");
	
	//spBuf->sendBuff[5]= (u_int8_t) (_globalDate->mainProStat[ACK_config_Rblock]&0xff);
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
	printf("%s\n",__func__);
}

void  CEventParsing:: ackmvconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=14;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_mvconfig;
	spBuf->sendBuff[5]=Status::getinstance()->mtdcfg.movedetectalgenable&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->mtdcfg.sensitivity&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->mtdcfg.speedpriority&0xff;
	spBuf->sendBuff[8]=(Status::getinstance()->mtdcfg.movmaxwidth>>8)&0xff;
	spBuf->sendBuff[9]=Status::getinstance()->mtdcfg.movmaxwidth&0xff;
	spBuf->sendBuff[10]=(Status::getinstance()->mtdcfg.movmaxheight>>8)&0xff;
	spBuf->sendBuff[11]=Status::getinstance()->mtdcfg.movmaxheight&0xff;
	spBuf->sendBuff[12]=(Status::getinstance()->mtdcfg.movminwidth>>8)&0xff;
	spBuf->sendBuff[13]=Status::getinstance()->mtdcfg.movminwidth&0xff;
	spBuf->sendBuff[14]=(Status::getinstance()->mtdcfg.movminheight>>8)&0xff;
	spBuf->sendBuff[15]=Status::getinstance()->mtdcfg.movminheight&0xff;
	spBuf->sendBuff[16]=(Status::getinstance()->mtdcfg.moverecordtime>>8)&0xff;
	spBuf->sendBuff[17]=Status::getinstance()->mtdcfg.moverecordtime&0xff;
	
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
	printf("%s\n",__func__);
}

void CEventParsing::ackdisplayconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=2;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_displayconfig;
	spBuf->sendBuff[5]=Status::getinstance()->displayresolution&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void  CEventParsing:: ackpanoconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=9;
	int sensor=0;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_panoconfig;
	for(sensor=0; sensor<2; sensor++)
	{
		spBuf->sendBuff[5+4*sensor]=Status::getinstance()->panocfg.ptzspeed[sensor]&0xff;
		spBuf->sendBuff[6+4*sensor]=(Status::getinstance()->panocfg.piexfocus[sensor]>>8)&0xff;
		spBuf->sendBuff[7+4*sensor]=(Status::getinstance()->panocfg.piexfocus[sensor])&0xff;
		spBuf->sendBuff[8+4*sensor]=(Status::getinstance()->panocfg.circlefps[sensor])&0xff;
	}

	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
	printf("%s\n",__func__);
}

void  CEventParsing::ackscanplantformconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=6;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_scanplantformconfig;
	spBuf->sendBuff[5]=Status::getinstance()->scan_platformcfg.address&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->scan_platformcfg.protocol&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->scan_platformcfg.baudrate&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->scan_platformcfg.start_signal&0xff;
	spBuf->sendBuff[9]=Status::getinstance()->scan_platformcfg.pt_check&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void CEventParsing::ackradarconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=14;
	int sensor=0;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_radarconfig;
	spBuf->sendBuff[5]=Status::getinstance()->radarcfg.hideline&0xff;
	for(sensor=0; sensor<2; sensor++)
	{
		spBuf->sendBuff[6+sensor*6]=Status::getinstance()->radarcfg.offset50m[sensor]&0xff;
		spBuf->sendBuff[7+sensor*6]=(Status::getinstance()->radarcfg.offset50m[sensor]>>8)&0xff;
		spBuf->sendBuff[8+sensor*6]=Status::getinstance()->radarcfg.offset100m[sensor]&0xff;
		spBuf->sendBuff[9+sensor*6]=(Status::getinstance()->radarcfg.offset100m[sensor]>>8)&0xff;
		spBuf->sendBuff[10+sensor*6]=Status::getinstance()->radarcfg.offset300m[sensor]&0xff;
		spBuf->sendBuff[11+sensor*6]=(Status::getinstance()->radarcfg.offset300m[sensor]>>8)&0xff;
	}

	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void CEventParsing::acktrackconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=5;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_trackconfig;
	spBuf->sendBuff[5]=Status::getinstance()->trackcfg.trkpriodis&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->trackcfg.trkpriobright&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->trackcfg.trkpriosize&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->trackcfg.trktime&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

u_int8_t CEventParsing:: package_frame(uint len, u_int8_t *tmpbuf)
{
	tmpbuf[0] = 0xEB;
	tmpbuf[1] = 0x51;
	tmpbuf[2] = len&0xff;
	tmpbuf[3] = (len>>8)&0xff;
	tmpbuf[len+4]= sendCheck_sum(len+3,tmpbuf+1);
	//printf("%s check sum=%d\n",__func__,tmpbuf[len+4]);
	return 0;
}

u_int8_t CEventParsing:: sendCheck_sum(uint len, u_int8_t *tmpbuf)
{
	u_int8_t  ckeSum=0;
	for(int n=0;n<len;n++)
		{
		ckeSum ^= tmpbuf[n];
		//printf("crcsum=%d\n",ckeSum);
		}
	return ckeSum;
}
