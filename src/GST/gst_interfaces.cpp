/***************************************
File name: bitsInterfacesdemo.c
Auther:	Aloysa
CopyRight@ JiaYue/LianShu/QianRun Graduate 2016

***************************************/
 /*
 modification history
 --------------------
 */

/* include files */
#include <demo_global_def.h>
#include <gst_interfaces.h>

/***************************************/
/*  extensional define  */
/***************************************/

/***************************************/
/* module  define  */
/***************************************/
#define MAX_RECV_BUF_LEN 0x200000	// 6M
#define MAX_SEND_BUF_LEN 0x200000	// 6M

/***************************************/
/*  module typedef  */
/***************************************/
typedef enum
{
    ePort_inet_udp,
    ePort_inet_tcp,
} Enum_DevPortType;

typedef struct  _mux_port_handle
{
    int devId;
    char ipAddr[256];

    Enum_DevPortType    devType;
    //void *  openParams;
    //void *  transContext;
    //void *  user;

    //socket
    int connectAlive;
    struct sockaddr_in  sinlocal;
    struct sockaddr_in  sinremote;

    //buffer
    int recvLen;
    int sendLen;
    UInt8*  sendBuf;
    UInt8*  recvBuf;

} mux_port_handle, *MPORT_Handle;

typedef struct  _mux_port_tsk
{
    int  (* TskCreate)();
    int  (* TskDestroy)();

    OSA_ThrHndl tskSendHndl;
    Bool tskSendLoop;
    Bool istskSendStopDone;

	OSA_BufCreate tskSendBufCreate;
	OSA_BufHndl tskSendBuf;

} mux_port_tsk, *MPORT_Tsk;

typedef struct _mux_port_obj
{
	mux_port_handle pPort;

	mux_port_tsk pTsk;

} mux_port_obj, *MPORT_Obj;


/***************************************/
/* module local  */
/***************************************/
static BOOL bInit = FALSE;
static mux_port_obj interobj_net;

/***************************************/
/*  */
/***************************************/
static int clientOpen(MPORT_Handle pPort, char *ip_addr, Uint32 port)
{
	OSA_assert(pPort != NULL);

	int status = SDK_SOK;
	int sockAddrLen = sizeof(struct sockaddr);
	struct hostent *host;

	if(ip_addr != NULL)
	{
		host = gethostbyname(ip_addr);
		strcpy(pPort->ipAddr, ip_addr);
	}
	else
	{
		host = gethostbyname("127.0.0.1");
		strcpy(pPort->ipAddr, "127.0.0.1");
	}
	if(host == NULL)
		return OSA_EFAIL;

	pPort->sinlocal.sin_family     = AF_INET;
	pPort->sinlocal.sin_port       = htons(port);
	pPort->sinlocal.sin_addr.s_addr   = htonl(INADDR_ANY);
	bzero(&(pPort->sinlocal.sin_zero),8);

	pPort->sinremote.sin_family     = AF_INET;
	pPort->sinremote.sin_port       = htons(port);
	//inet_aton("127.0.0.1", &pPort->sinremote.sin_addr);
	pPort->sinremote.sin_addr   = *((struct in_addr *)host->h_addr);
	bzero(&(pPort->sinremote.sin_zero),8);
	OSA_printf(" Remote interface %x:%d", pPort->sinremote.sin_addr.s_addr, pPort->sinremote.sin_port);

	/** < alloc mem buffer */
	//pPort->sendBuf = SDK_MEM_MALLOC(MAX_SEND_BUF_LEN);
	//OSA_assert(pPort->sendBuf != NULL);
	//pPort->recvBuf = SDK_MEM_MALLOC(MAX_RECV_BUF_LEN);
	//OSA_assert(pPort->recvBuf != NULL);

	/** < open port and config */
	if (pPort->devType == ePort_inet_tcp)
	{
		/** < tcp link */
		pPort->devId = socket(AF_INET, SOCK_STREAM, 0);
		OSA_assert(pPort->devId > 0);
		//OSA_printf(" Binding interface %x:%d", pPort->sinlocal.sin_addr.s_addr, pPort->sinlocal.sin_port);
		//status = bind(pPort->devId, (struct sockaddr *)&pPort->sinlocal, sockAddrLen);
		//OSA_assert(status == SDK_SOK);
		//status = listen(pPort->devId, 5);
		//OSA_assert(status == SDK_SOK);
		#if 0
		int optVal = 0;
		int optLen = sizeof(optVal);
		getsockopt(pPort->devId, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, &optLen);
		OSA_printf("sock recv buf is %d\n", optVal);
		getsockopt(pPort->devId, SOL_SOCKET, SO_SNDBUF, (char*)&optVal, &optLen);
		OSA_printf("sock snd buf is %d\n", optVal);
		#endif
	}
	else    /*if(pPort->devType == ePort_inet_udp)*/
	{
		/** < udp link */
		pPort->devId = socket(AF_INET, SOCK_DGRAM, 0);
		OSA_assert(pPort->devId > 0);
		//OSA_printf(" Binding interface %x:%d", pPort->sinlocal.sin_addr.s_addr, pPort->sinlocal.sin_port);
		//status = bind(pPort->devId, (struct sockaddr *)&pPort->sinlocal, sockAddrLen);
		//OSA_assert(status == SDK_SOK);
		pPort->connectAlive = 1;	// udp no need connect
	}

    return SDK_SOK;
}

/***************************************/
/*  */
/***************************************/
static int clientClose(MPORT_Handle pPort)
{
	OSA_assert(pPort != NULL);

	/** < close port */
	/*if(pPort->devskId > 0)
	{
		close(pPort->devskId);
		pPort->devskId = 0;
	}*/
	close(pPort->devId);

	/** < free mem buffer */
	if(pPort->sendBuf != NULL)
		SDK_MEM_FREE(pPort->sendBuf);
	if(pPort->recvBuf != NULL)
		SDK_MEM_FREE(pPort->recvBuf);

    return SDK_SOK;
}

/***************************************/
/*  */
/***************************************/
static int clientConnect(MPORT_Handle pPort)
{
	OSA_assert(pPort != NULL);

	if(pPort->connectAlive)
		return SDK_SOK;

	int status = SDK_SOK;
	int sockAddrLen = sizeof(struct sockaddr);
	struct hostent *host;

	/** < open port and config */
	if (pPort->devType == ePort_inet_tcp)
	{
		/** < tcp link */
		status = connect(pPort->devId, (struct sockaddr *)&pPort->sinremote, sockAddrLen);
		if(status != SDK_SOK)
		{
			sleep(1);
			//OSA_printf(" Conncet interface failed.");
			return OSA_EFAIL;
		}
		else
		{
			pPort->connectAlive = 1;
		}
	}

	return SDK_SOK;
}

/***************************************/
/*  */
/***************************************/
static int clientSendData(MPORT_Handle pPort, Uint8 *dataBuf, Uint32 dataSize)
{
	OSA_assert(pPort != NULL);

	if(!pPort->connectAlive)
		return OSA_EFAIL;

	int actDataSize=0, pktSize=65507; 	// udp sendto pktlen max is 2^16 -1 - 8 -20
	int sockAddrLen = sizeof(struct sockaddr_in);
	Uint8 *pCur = dataBuf;

	if(pPort->devType == ePort_inet_udp)
	{
		while(dataSize > 0)
		{
			if(dataSize > pktSize)
			{
				actDataSize = sendto(pPort->devId, pCur, pktSize, 0,
					(struct sockaddr*)&pPort->sinremote, sockAddrLen);
			}
			else
			{
				actDataSize = sendto(pPort->devId, pCur, dataSize, 0,
					(struct sockaddr*)&pPort->sinremote, sockAddrLen);
			}

			if(actDataSize <= 0)
				break;
			pCur += actDataSize;
			dataSize -= actDataSize;
		}

		if( dataSize > 0 )
			return OSA_EFAIL;
	}
	else /*if(pPort->devType == ePort_inet_tcp)*/
	{
		while(dataSize > 0)
		{
			actDataSize = sendto(pPort->devId, pCur, dataSize, 0,
				(struct sockaddr*)&pPort->sinremote, sockAddrLen);

			if(actDataSize <= 0)
				break;
			pCur += actDataSize;
			dataSize -= actDataSize;
		}

		if( dataSize > 0 )
			return OSA_EFAIL;
	}

	return OSA_SOK;
}

/***************************************/
/*  */
/***************************************/
#define DEMOTEST	0
#define DEMOTEST2	0
static void* senderTask(void *pPrm)
{
	MPORT_Obj pPortObj = NULL;
	MPORT_Handle pPort = NULL;
	MPORT_Tsk pTsk = NULL;

	struct timeval timeout;
	int result = 0, pause = 0;
	int sendLen = 0, sendPkt = 0;
	int rngstat = SDK_EFAIL, rngId = 0, rngLen = 0, headlen = sizeof(DATAHEADER);
	DATAHEADER *pHead = NULL;
	int icnt=0, ichns=1;

	assert(pPrm != NULL);
	pPortObj    = (MPORT_Obj)pPrm;
	pPort   = &(pPortObj->pPort);
	pTsk    = &(pPortObj->pTsk);

	OSA_printf(" %s start.", __func__);
	while (pTsk->tskSendLoop == TRUE)
	{
		#if DEMOTEST
		/*************************************/
		// for test
		demoInterfacesPut(NULL, NULL);
		timeout.tv_sec   = 0;
		timeout.tv_usec  = (33000/ichns);  // 33ms
		select(0, NULL, NULL, NULL, &timeout);
		// for test end
		/*************************************/
		#endif

		rngstat = OSA_bufGetFull(&pTsk->tskSendBuf, &rngId, SDK_TIMEOUT_FOREVER);
		if(rngstat != OSA_SOK)
		{
			OSA_printf(" datarng get failed no fullpkt.");
			continue;
		}
		sendLen = pTsk->tskSendBuf.bufInfo[rngId].size;
		//memcpy(pPort->sendBuf, pTsk->tskSendBuf.bufInfo[rngId].virtAddr, sendLen);
		pPort->sendBuf = ( UInt8* )pTsk->tskSendBuf.bufInfo[rngId].virtAddr;

		if (pTsk->tskSendLoop == FALSE)
		{
			OSA_bufPutEmpty(&pTsk->tskSendBuf, rngId);
			break;
		}

		#if DEMOTEST2
		/*************************************/
		// for test
		//sendLen = (rand()%400) * KB;
		sendLen = 150 * KB;	// 4MBps
		pHead = (DATAHEADER *)pPort->sendBuf;
		pHead->flag = 0xFFFF0001;
		pHead->size = sendLen;
		pHead->strmId = (icnt % ichns);
		pHead->codecType = VCODEC_TYPE_H264;
		pHead->frameType = (icnt % 30 == 0)?0:1;
		pHead->timestamp = OSA_getCurTimeInMsec();
		// for test end
		#endif

		/*************************************/
		if (pause || sendLen <= 0)
		{
			OSA_bufPutEmpty(&pTsk->tskSendBuf, rngId);
			continue;
		}

		/*************************************/
		pPort->sendLen = sendLen;

		// tcp connect
		if(!pPort->connectAlive)
		{
			clientConnect(pPort);
		}

		// out data
		if(pPort->connectAlive)
		{
			result = clientSendData(pPort, pPort->sendBuf, pPort->sendLen);
			if (result != OSA_SOK)
			{
				OSA_printf(" %s send data error %d", __func__, pPort->sendLen);
				//pause = 1;
				pPort->connectAlive=0;
			}
			icnt++;
			if(icnt % 30 == 1)
			{
				//OSA_printf(" %s [%d] send strm=%d len=%d.", __func__, icnt, pHead->strmId, pPort->sendLen);
			}
		}

		OSA_bufPutEmpty(&pTsk->tskSendBuf, rngId);

	}// while(tskSendLoop == TRUE)

	pPort->sendBuf = NULL;

	OSA_printf(" %s exit.", __func__);
	pTsk->istskSendStopDone = TRUE;
	return NULL;
}

/***************************************/
/* module  API  */
/***************************************/
void demoInterfacesCreate(char *ip_addr, Uint32 port)
{
	if(bInit == TRUE)
		return ;

	memset(&interobj_net, 0, sizeof(interobj_net));

	MPORT_Tsk pTsk = &interobj_net.pTsk;
	int status = SDK_SOK;

	//interobj_net.pPort.devType = ePort_inet_udp;
	interobj_net.pPort.devType = ePort_inet_tcp;
	if(clientOpen(&interobj_net.pPort, ip_addr, port) != OSA_SOK)
	{
		OSA_printf(" %s create failed.", __func__);
		return ;
	}

	/** < create sender ringbuf */
	int i;
	pTsk->tskSendBufCreate.numBuf = OSA_BUF_NUM_MAX;
	for (i = 0; i < pTsk->tskSendBufCreate.numBuf; i++)
	{
		pTsk->tskSendBufCreate.bufVirtAddr[i] = SDK_MEM_MALLOC(MAX_SEND_BUF_LEN);
		OSA_assert(pTsk->tskSendBufCreate.bufVirtAddr[i] != NULL);
		memset(pTsk->tskSendBufCreate.bufVirtAddr[i], 0, MAX_SEND_BUF_LEN);
	}
	status = OSA_bufCreate(&pTsk->tskSendBuf, &pTsk->tskSendBufCreate);
	OSA_assert(status == OSA_SOK);

	/** < create sender task loop */
	pTsk->tskSendLoop = TRUE;
	pTsk->istskSendStopDone = FALSE;
	status = OSA_thrCreate(&pTsk->tskSendHndl,
	             senderTask,
	             OSA_THR_PRI_DEFAULT+10,
	             0,
	             &interobj_net
	         );
	OSA_assert(status == OSA_SOK);

	OSA_printf(" %s done.", __func__);
	bInit = TRUE;

	return ;
}

/***************************************/
/*   */
/***************************************/
void demoInterfacesDestroy(void)
{
	if(bInit != TRUE)
		return ;

	MPORT_Tsk pTsk = &interobj_net.pTsk;
	//int status = SDK_SOK;
	int rngId;

	bInit = FALSE;

	/** < stop receive task loop */
	pTsk->tskSendLoop = FALSE;
	OSA_bufGetEmpty(&pTsk->tskSendBuf, &rngId, SDK_TIMEOUT_FOREVER);
	pTsk->tskSendBuf.bufInfo[rngId].size = 0;
	OSA_bufPutFull(&pTsk->tskSendBuf, rngId);
	while (1)
	{
	    OSA_waitMsecs(40);
	    if (pTsk->istskSendStopDone)
	        break;
	}

	OSA_thrDelete(&pTsk->tskSendHndl);

	clientClose(&interobj_net.pPort);

	int i;
	for (i = 0; i < pTsk->tskSendBufCreate.numBuf; i++)
	{
		if (pTsk->tskSendBufCreate.bufVirtAddr[i] != NULL)
		{
			SDK_MEM_FREE(pTsk->tskSendBufCreate.bufVirtAddr[i]);
			pTsk->tskSendBufCreate.bufVirtAddr[i] = NULL;
		}
	}

	OSA_printf(" %s done.\r\n", __func__);
	return ;
}

/***************************************/
/*   */
/***************************************/
int demoInterfacesPut(DATAHEADER *pPrm, BYTE* pData)
{
	if(bInit != TRUE)
		return OSA_EFAIL;

	MPORT_Tsk pTsk = &interobj_net.pTsk;
	int rngstat = SDK_SOK, rngId = 0, rngLen = 0;
	int datalen = 0, headlen = sizeof(DATAHEADER);
	char *pIn = NULL, *pOut = NULL;
	DATAHEADER *pHead = NULL;

	if(pPrm != NULL)
	{
		pIn = (char *)pData;
		datalen = pPrm->size;
	}
	else
	{
		datalen = 0;
	}

	// get rngbuf
	rngstat = OSA_bufGetEmpty(&pTsk->tskSendBuf, &rngId, SDK_TIMEOUT_NONE/*OSA_TIMEOUT_FOREVER*/);
	//rngstat = OSA_bufGetEmpty(&pTsk->tskSendBuf, &rngId, 500);
	if (rngstat != OSA_SOK)
	{
		return OSA_EFAIL;
	}

	pOut = (char *)pTsk->tskSendBuf.bufInfo[rngId].virtAddr;
	rngLen = datalen + headlen;
	//////////////////////////////
	if(pPrm != NULL)
	{
		memcpy(pOut, pPrm, headlen);
	}
	else
	{
		pHead = (DATAHEADER *)pOut;
		pHead->flag = 0xFFFF0001;
		pHead->size = headlen;
		pHead->strmId = 0xFF;
		pHead->codecType = VCODEC_TYPE_H264;
		pHead->frameType = VCODEC_FRAME_TYPE_P_FRAME;
		pHead->timestamp = OSA_getCurTimeInMsec();
	}
	// head correction
	pHead = (DATAHEADER *)pOut;
	pHead->size = rngLen;
	// head correction end

	if (pIn != NULL)
		memcpy(pOut+headlen, pIn, datalen);
	else
		memset(pOut+headlen, 0, datalen);

	//////////////////////////////
	pTsk->tskSendBuf.bufInfo[rngId].size = rngLen;

	OSA_bufPutFull(&pTsk->tskSendBuf, rngId);

	//OSA_printf(" get data\n ");
	return OSA_SOK;
}


