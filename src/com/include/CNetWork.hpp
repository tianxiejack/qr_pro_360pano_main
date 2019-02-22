#ifndef __CNETWORK_H__
#define __CNETWORK_H__

#include "CPortBase.hpp"
#include "CConnect.hpp"
#include "osa_mutex.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

typedef  vector<CConnect*>  CConnectVECTOR;

class CNetWork:public CPortBase
{
public:
    CNetWork();
    ~CNetWork();
    int create();
    int closePort();
    void run();

private:
	int openPort();
    int NetBind(int fd, int port);
    int NetListen(int fd, int maxnum);
	CConnect *curConnect;
    int m_Socketfd;
    int m_connectfd;
    struct sockaddr_in connectAddr;
    OSA_ThrHndl netAcceptThrd,netCleanThrd;
    OSA_MutexHndl mutexConn;
    bool exitservThrd;
    bool clientConnect;
    int AccpetLinkThread();
    int ReclaimConnectThread();
    CConnectVECTOR  connetVector;
    static void *NetAcceptThrdFunc(void* AcceptContext)
    {
        CNetWork *m_thrd = (CNetWork *)AcceptContext;
        m_thrd->AccpetLinkThread();
        return NULL;
    }
    static void *NetCleanThrdFunc(void* CleanContext)
    {
        CNetWork *pThrd = (CNetWork *)CleanContext;
        pThrd->ReclaimConnectThread();
        return NULL;
    }
};

#endif
