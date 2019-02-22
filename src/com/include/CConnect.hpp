#ifndef __CCONNECT_H__
#define __CCONNECT_H__

#include "CPortBase.hpp"
#include  "osa_thr.h"
#include"osa_sem.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

class CConnect : public CPortBase
{
public:
    CConnect();
    ~CConnect();
    int recvData();
    int sendData();
    void run();
    bool bConnecting;
    int m_connect;
    sockaddr_in m_addr;
    bool IsConnecting(void)
    {
        return bConnecting;
    }
	
    void DisConnecting(void)
    {
        bConnecting = FALSE;
    }
		
private:
    OSA_ThrHndl getDataThrID;
    OSA_ThrHndl sendDataThrID;

    int NETRecv(void *rcv_buf,int data_len);

    int expconfig(unsigned char *swap_data_buf, unsigned int swap_data_len);

	
protected:
    static void *getDataThrFun(void * mContext)
    {
        CConnect* sPthis = (CConnect*)mContext;
        sPthis->recvData();
        return NULL;
    }
	
    static void *sendDataThrFun(void * Context)
    {
        CConnect* sThis = (CConnect*)Context;
        sThis->sendData();
        return NULL;
    }
};

#endif
