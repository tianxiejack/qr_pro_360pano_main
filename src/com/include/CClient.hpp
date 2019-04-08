#ifndef __CCLIENT_H__
#define __CCLIENT_H__

#include "CPortBase.hpp"
#include "CClient.hpp"
#include "osa_mutex.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

class CClient:public CPortBase
{
public:
    CClient();
    ~CClient();
    int create();
    int closePort();
    void run();
    void DisConnecting(void)
    {
        exitservThrd = TRUE;
    }
	
public:
    char serverip[20] = "127.0.0.1";

private:
    int openPort();
    int NetConnect(int fd, int port);
    int sendData();
    int ReConnectThread();
    int sendfile(char *filename);
    static void *SendDataFunc(void* AcceptContext)
    {
        CClient *m_thrd = (CClient *)AcceptContext;
        m_thrd->sendData();
        return NULL;
    }
    static void *ReConnectFunc(void* CleanContext)
    {
        CClient *pThrd = (CClient *)CleanContext;
        pThrd->ReConnectThread();
        return NULL;
    }
	
private:
    OSA_ThrHndl netsenddataThrd,reconnectThrd;
    bool exitservThrd;
    int m_Socketfd;
	
};

#endif
