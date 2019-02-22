#ifndef DXTIMER_HPP_
#define DXTIMER_HPP_

#include <pthread.h>
#include <osa_tsk.h>

#define DX_TIMER_MAX    50
#define MAX_MSG_NUM     50

class DxTimer;

typedef enum timer_stat
{
    eTimer_Stat_Stop = 0x00,
    eTimer_Stat_Run	= 0x01
}eTimerStat;

typedef enum timerid_stat
{
    eTimer_Stat_unused = 0x00,
    eTimer_Stat_used = 0x01
}eTimerIdStat;

typedef struct _dx_timer{
    int interval;
    int curTamp;
    unsigned int cnt;
    void (*fnxCall)(void *);
    volatile int flag;
    struct timeval timeout;
}DX_TIMER;

typedef struct Dx_Obj
{
    unsigned int tmReg[DX_TIMER_MAX];
    unsigned int tmCnt[DX_TIMER_MAX];
    unsigned int tmBegen[DX_TIMER_MAX];
}Dx_Obj;

typedef void (*TMsgApiFun)(void *);
typedef struct _TMsg_Tab
{
    int msgId;
    void *refContext;
    TMsgApiFun pRtnFun;
}TMSGTAB_Class;
	
typedef struct tmsgdriv
{
    unsigned short  bIntial;
    OSA_ThrHndl tskHndl;
    unsigned short tskLoop;
    unsigned short istskStopDone;
    OSA_MsgqHndl msgQue;
    TMSGTAB_Class msgTab[MAX_MSG_NUM];
}TMSGDRIV_Class;

typedef struct 
{
    DxTimer *ptimer;
    TMSGDRIV_Class *pmsg;
}Dx_Procth;

typedef struct 
{
    volatile unsigned int nStat;
}CDTime;

class DxTimer 
{
public:
	void create();
	static DxTimer *getinstance();
private:
	DxTimer();
	static DxTimer *instance;
	;
public:
    
    ~DxTimer();
    int createTimer();
    int registerTimer(unsigned int timerId,void (*callback)(void *), void *);   
    int startTimer(unsigned int timerId,unsigned int nMs);
    int getTimerStat(unsigned int timerId);
    int stopTimer(unsigned int timerId);
    int resetTimer(unsigned int timerId, unsigned int nMs);
    int killTimer(int timerId);

private:
    void MSGDRIV_attachMsgFun(TMSGDRIV_Class *handle, int msgId, TMsgApiFun pRtnFun, void *context);
    void MSGDRIV_recv(void *pPrm);
    void MSGDRIV_send(int msgId);
    void Dx_initTimer();
    int Dx_setTimer(unsigned int timerId, unsigned int nMs);
    int Dx_stopTimer(unsigned int timerId);
    void Dx_destroyTimer();
    void PTH_msgrecv();
    int PTH_timerloop(int interval);
    void Dx_OnTimer();
    void sig_func(void);
    static void *THSIGFUNC(void *p)
    {
        DxTimer *pthis = (DxTimer *)p;
        pthis->sig_func();
    }
    static void *MSGDRIV_recv_PTH(void *p)
    {
        Dx_Procth *pth = (Dx_Procth *)p;
        DxTimer *pthis = pth->ptimer;
        pthis->MSGDRIV_recv(pth->pmsg);
    }

    Dx_Obj gDxObj;
    TMSGDRIV_Class g_MsgDrvObj;
    Dx_Procth pth;
    DX_TIMER cTimer;
    pthread_t thread_timer;
    CDTime pTimeArray[DX_TIMER_MAX];
    int timerNum[DX_TIMER_MAX];
};

#endif