#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "DxTimer.hpp"
#include "osa.h"
DxTimer *DxTimer::instance=NULL;
DxTimer::DxTimer()
{
    
}

DxTimer::~DxTimer()
{
    Dx_destroyTimer();
}

void DxTimer::create()
{
	Dx_initTimer();
	PTH_msgrecv();
	PTH_timerloop(10);

}
DxTimer *DxTimer::getinstance()
{
	if(instance==NULL)
		instance=new DxTimer();
	return instance;

}
/*******************************************************************************
*    Function Name		:    createTimer
*    Create Date		:    2018/10/25        
*    Author/Corporation	:    zzq/Chamrun
*    Description	    	:    get a timer ID
*    Param         		:    No parameter
*    Return Code	    	:    0~DX_TIMER_MAX-1:a timer ID
                		     -1:failed
*******************************************************************************/
int DxTimer::createTimer()
{
    for(int i = 0; i < DX_TIMER_MAX; i++)
    {
        if(eTimer_Stat_unused == timerNum[i])
        {
            timerNum[i] = eTimer_Stat_used;
            return i;
        }
    }
    return -1;
}

/*******************************************************************************
*    Function Name		:    registerTimer
*    Create Date		:    2018/10/25        
*    Author/Corporation	:    zzq/Chamrun
*    Description	    	:    register a callback for a timer
*    Param         		:    timerId:a timer ID
                		     callback:when timer arrives,the callback will be called
                		     p:will be passed to callback as a parameter 
*    Return Code	    	:    0:success
                		     -1:failed
*******************************************************************************/
int DxTimer::registerTimer(unsigned int timerId,void (*callback)(void *), void *p)
{
    if(eTimer_Stat_unused == timerNum[timerId])
    {
        printf("timer %d is has not been created!\n");
        return -1;
    }
		
    TMSGDRIV_Class *handle=&g_MsgDrvObj;

    assert(handle != NULL);
    MSGDRIV_attachMsgFun(handle,timerId,callback,p);

    return 0;
}

/*******************************************************************************
*    Function Name		:    startTimer
*    Create Date		:    2018/10/25        
*    Author/Corporation	:    zzq/Chamrun
*    Description	    	:    start a timer
*    Param         		:    timerId:a timer ID
                		     nMs:the timer cycle,unit is milliseconds
*    Return Code	    	:    0:success
                		     -1:failed
*******************************************************************************/
int DxTimer::startTimer(unsigned int timerId,unsigned int nMs)
{
    if(eTimer_Stat_unused == timerNum[timerId])
    {
        printf("timer %d has not been created!\n");
        return -1;
    }
    Dx_setTimer(timerId, nMs);
    pTimeArray[timerId].nStat = eTimer_Stat_Run;

    return 0;
}

/*******************************************************************************
*    Function Name		:    getTimerStat
*    Create Date		:    2018/10/25        
*    Author/Corporation	:    zzq/Chamrun
*    Description	    	:    get the status of a timer
*    Param         		:    timerId:a timer ID
*    Return Code	    	:    eTimer_Stat_Run:the timer is running
                		     eTimer_Stat_Stop:the timer is stopped
                		     -1:failed
*******************************************************************************/
int DxTimer::getTimerStat(unsigned int timerId)
{
    if(eTimer_Stat_unused == timerNum[timerId])
    {
        printf("timer %d has not been created!\n");
        return -1;
    }
		
    CDTime * pTime =NULL;

    assert(timerId < DX_TIMER_MAX);

    pTime = &pTimeArray[timerId];

    return (pTime->nStat);
}

/*******************************************************************************
*    Function Name		:    stopTimer
*    Create Date		:    2018/10/25        
*    Author/Corporation	:    zzq/Chamrun
*    Description	    	:    stop a timer
*    Param         		:    timerId:a timer ID
*    Return Code	    	:    0:success
                		     -1:failed
*******************************************************************************/
int DxTimer::stopTimer(unsigned int timerId)
{
    if(eTimer_Stat_unused == timerNum[timerId])
    {
        printf("timer %d has not been created!\n");
        return -1;
    }

    if(pTimeArray[timerId].nStat == eTimer_Stat_Run)
    {	
        Dx_stopTimer(timerId);
        pTimeArray[timerId].nStat = eTimer_Stat_Stop;
    }

    return 0;
}

/*******************************************************************************
*    Function Name		:    resetTimer
*    Create Date		:    2018/10/25        
*    Author/Corporation	:    zzq/Chamrun
*    Description	    	:    reset a timer
*    Param         		:    timerId:a timer ID
*    Return Code	    	:    0:success
                		     -1:failed
*******************************************************************************/
int DxTimer::resetTimer(unsigned int timerId, unsigned int nMs)
{
    if(eTimer_Stat_unused == timerNum[timerId])
    {
        printf("timer %d has not been created!\n");
        return -1;
    }

    stopTimer(timerId);
    startTimer( timerId, nMs);

    return 0;
}

/*******************************************************************************
*    Function Name		:    killTimer
*    Create Date		:    2018/10/25        
*    Author/Corporation	:    zzq/Chamrun
*    Description	    	:    destroy a timer
*    Param         		:    timerId:a timer ID
*    Return Code	    	:    0:success
                		     -1:failed
*******************************************************************************/
int DxTimer::killTimer(int timerId)
{
    if(eTimer_Stat_unused == timerNum[timerId])
    {
        printf("timer %d has been killed!\n");
        return -1;
    }
    stopTimer(timerId);
    timerNum[timerId] = eTimer_Stat_unused;

    return 0;
}

void DxTimer::Dx_initTimer()
{
    memset(&g_MsgDrvObj, 0, sizeof(TMSGDRIV_Class));
    g_MsgDrvObj.tskLoop = 1;
    g_MsgDrvObj.bIntial = 1;
    g_MsgDrvObj.istskStopDone = 0;

    memset(pTimeArray,0,sizeof(CDTime)*DX_TIMER_MAX);

    for(int i = 0; i < DX_TIMER_MAX; i++)
    {
        timerNum[i] = eTimer_Stat_unused;
    }
}

int DxTimer::Dx_setTimer(unsigned int timerId, unsigned int nMs)
{
    if(timerId >= DX_TIMER_MAX)
    {
        return OSA_EFAIL;
    }

    gDxObj.tmReg[timerId]	   = nMs;
    gDxObj.tmBegen[timerId]	   = OSA_getCurTimeInMsec();
    gDxObj.tmCnt[timerId]	   = 0;

    return OSA_SOK;
}

int DxTimer::Dx_stopTimer(unsigned int timerId)
{
    if( timerId >= DX_TIMER_MAX )
    {
        return OSA_EFAIL;
    }

    gDxObj.tmReg[timerId] = 0;

    return OSA_SOK;
}

void DxTimer::Dx_destroyTimer()
{
    OSA_waitMsecs(100);
    cTimer.flag = 0;
    OSA_waitMsecs(10);
    while(!cTimer.flag)
    {
        OSA_waitMsecs(10);
    }
    pthread_cancel(thread_timer);
    pthread_join(thread_timer, NULL);

    memset(&cTimer, 0, sizeof(cTimer));
}

void DxTimer::PTH_msgrecv()
{
    int status = 0;
    TMSGDRIV_Class *handle=&g_MsgDrvObj;
    pth.ptimer = this;
    pth.pmsg = handle;

    assert(handle != NULL);
    memset(handle->msgTab, 0, sizeof(TMSGTAB_Class) * MAX_MSG_NUM);
	
    OSA_msgqCreate(&handle->msgQue);
    status = OSA_thrCreate(&handle->tskHndl,
                MSGDRIV_recv_PTH,
                OSA_THR_PRI_DEFAULT+10,
                0,
                &pth
            );
    OSA_assert(status == 0);	
}

int DxTimer::PTH_timerloop(int interval)
{
    //__suseconds_t us;
    memset(&cTimer, 0, sizeof(cTimer));
    cTimer.flag			   = 1;
    cTimer.timeout.tv_sec  = interval / 1000;
    cTimer.timeout.tv_usec = ((interval % 1000)) * 1000;

    printf( "%s : %d %d\n", __func__, (int)cTimer.timeout.tv_sec, (int)cTimer.timeout.tv_usec);

    return pthread_create(&thread_timer, NULL, THSIGFUNC, this);
}

void DxTimer::Dx_OnTimer()
{
    int i;
    unsigned int curTM = OSA_getCurTimeInMsec();

    for(i = 0; i < DX_TIMER_MAX; i++)
    {
        if(gDxObj.tmReg[i] == 0)
        {
            continue;
        }

        while(curTM >= gDxObj.tmBegen[i] + (gDxObj.tmCnt[i] + 1) * gDxObj.tmReg[i])
        {
            MSGDRIV_send(i);
            gDxObj.tmCnt[i]++;
        }
    }
}

void DxTimer::sig_func(void)
{
    struct timeval timeout;
    int err;
    while(cTimer.flag)
    {
        //pthread_testcancel();
        memcpy(&timeout, &cTimer.timeout, sizeof(timeout));
        select(0, NULL, NULL, NULL, &timeout);
	//  printf("%s\n",__func__);

        Dx_OnTimer();
    }
    cTimer.flag = 1;
    printf("%s:: exit !", __func__);
    return;
}

void DxTimer::MSGDRIV_attachMsgFun(TMSGDRIV_Class *handle, int msgId, TMsgApiFun pRtnFun, void *context)
{
    assert(handle != NULL && msgId < MAX_MSG_NUM);

    handle->msgTab[msgId].msgId = msgId;
    handle->msgTab[msgId].pRtnFun = pRtnFun;
    handle->msgTab[msgId].refContext = context;
}

void DxTimer::MSGDRIV_recv(void *pPrm)
{
    
    OSA_MsgHndl msg_recv;
    TMSGTAB_Class msg;
    unsigned int icou = 0;
    TMSGDRIV_Class* pMsgDrvObj = (TMSGDRIV_Class*)pPrm;
    assert(pMsgDrvObj != NULL);
    while (pMsgDrvObj->tskLoop)
    {
        icou++;
        int iret = OSA_msgqRecvMsgEx(&pMsgDrvObj->msgQue, &msg_recv, OSA_TIMEOUT_FOREVER);
        if((iret != -1) && (msg_recv.cmd != 0xFFFF))
        {
            msg.msgId = msg_recv.cmd;
            msg.refContext = msg_recv.pPrm;
	    //  printf("%s\n",__func__);
            assert(msg.msgId >= 0 && msg.msgId < MAX_MSG_NUM);
            if (pMsgDrvObj->msgTab[msg.msgId].pRtnFun != NULL)
            {
                pMsgDrvObj->msgTab[msg.msgId].pRtnFun(pMsgDrvObj->msgTab[msg.msgId].refContext);
            }
        }
        else if (msg_recv.cmd == 0xFFFF)
        {
            OSA_printf(" %s recieve MSGDRIV_FREE_CMD !!! ", __func__);
        }
    }
    pMsgDrvObj->istskStopDone = 1;
}

void DxTimer::MSGDRIV_send(int msgId)
{
    assert(msgId < DX_TIMER_MAX);
    OSA_msgqSendMsg(&g_MsgDrvObj.msgQue, &g_MsgDrvObj.msgQue, msgId, NULL, 0, NULL);
}

