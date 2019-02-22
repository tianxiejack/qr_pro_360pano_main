#ifndef __CUARTPROCESS_H__
#define __CUARTPROCESS_H__

#include <pthread.h>
#include "CPortBase.hpp"
#include "osa_thr.h"
#include"osa_sem.h"
#define    RECV_BUF_SIZE   (1024)

class CUartProcess : public CPortBase{
public:
    CUartProcess();
    ~CUartProcess();
	int recvData();
    int closePort();
    void run();
	virtual int create();
    virtual int sendData();
	virtual int openPort(char *dev_name);
	virtual int setPort(const int baud_rate, const int data_bits, char parity, const int stop_bits);
	int m_port;
    bool m_UartRun;
    OSA_ThrHndl uartGetDataThrID;
    OSA_ThrHndl uartSendDataThrID;

    static void *uartGetDataThrFun(void *getContext)
    {
        CUartProcess* sPthis =  (CUartProcess*)getContext;
        sPthis->recvData();
        return NULL;
    }

    static	void *uartSendDataThrFun(void * sendContext)
    {
        CUartProcess* sThis =  (CUartProcess*)sendContext;
        sThis->sendData();
        return NULL;
    }

private:
   int  UartRecv(void *rcv_buf,int data_len);
   int expconfig(unsigned char *swap_data_buf, unsigned int swap_data_len);
};

#endif
