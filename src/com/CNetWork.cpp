#include "CNetWork.hpp"

#define  _LisPort  (6666)
#define  MAXCONNECT   (10)
#define  TIME_FOR_THREAD_END      (3)

CNetWork::CNetWork()
{
    m_Socketfd=0;
    exitservThrd=false;
    clientConnect =false;
    m_connectfd=0;
    memset(&connectAddr,0,sizeof(connectAddr));
    memset(&mutexConn,0,sizeof(mutexConn));
    memset(&netAcceptThrd,0,sizeof(netAcceptThrd));
    memset(&netCleanThrd,0,sizeof(netCleanThrd));
}

CNetWork::~CNetWork()
{
}

int CNetWork::create()
{	
    OSA_mutexCreate(&mutexConn);
    m_Socketfd = openPort();
    NetBind(m_Socketfd, _LisPort);
    NetListen(m_Socketfd, MAXCONNECT);
    return 0;
}

int CNetWork::openPort()
{
	int option = 1;
	
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd<0)
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
	
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	
    return fd;
}

int CNetWork::closePort()
{
    return close(m_Socketfd);
}

void CNetWork::run()
{
    int retv;

    retv=OSA_thrCreate(&netAcceptThrd,
        NetAcceptThrdFunc,
        0,
        0,
        (void*)this);

    retv=OSA_thrCreate(&netCleanThrd,
        NetCleanThrdFunc,
        0,
        0,
        (void*)this);
}

int CNetWork::NetBind(int fd, int port)
{
    int iRet;
    char ipbuf[20];
    char *ipaddr=ipbuf;
    struct ifreq ifr;
    struct sockaddr_in servAddr;

    memset(&servAddr,0,sizeof(servAddr));	
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons( port);
    servAddr.sin_addr.s_addr = INADDR_ANY; 
    printf("INFO: server  addr:%d---port: %d\r\n",servAddr.sin_addr.s_addr,servAddr.sin_port);

    iRet = bind(fd,(struct sockaddr*)&servAddr,sizeof(servAddr));
    if (iRet<0){
        printf("ERR:  Can not bind  Socketfd, error: %s(errno: %d)\r\n ",strerror(errno),errno);
    }

    return  0 ;
}

int CNetWork::NetListen(int fd,int maxnum)
{
    int ret;

    ret =listen(fd,maxnum);
    if(ret<0){
        printf("ERR: Can not listen sockfd ,error: %s(errno: %d)\r\n",strerror(errno),errno);
    }
    return 0;
}

int CNetWork::AccpetLinkThread()
{
    int lenthConnect = sizeof(connectAddr);
	
    while(!exitservThrd)
    {
        m_connectfd=accept(m_Socketfd, (struct sockaddr*)&connectAddr,(socklen_t*)&lenthConnect);
        if(m_connectfd<0)
        {
            printf("ERR: Can not  accept  Client connect\r\n");
            continue;
        }
        else
        {
            clientConnect=true;;
            //CConnect  *pConnect = new CConnect (m_connectfd, connectAddr);
            CConnect  *pConnect = new CConnect();

            pConnect->m_connect = m_connectfd;
            pConnect->m_addr= connectAddr;
            curConnect = pConnect;
            OSA_mutexLock(&mutexConn);
            char *connectIP = inet_ntoa(connectAddr.sin_addr);
            int connectPort = ntohs(connectAddr.sin_port);
            printf("INFO: Accept Connect IP : %s---- Port: %d\r\n",connectIP, connectPort);
            connetVector.push_back(pConnect);
            pConnect->bConnecting=true;
            OSA_mutexUnlock(&mutexConn);
            pConnect->run();
        }
    }
    return 0;
}

int CNetWork::ReclaimConnectThread()
{
    //客户端退出
    while(!exitservThrd)
    {
        OSA_mutexLock(&mutexConn);
        CConnectVECTOR::iterator iter = connetVector.begin();
        for (iter; iter != connetVector.end();)
        {
            CConnect *pCLink= (CConnect*)*iter;
            if (!pCLink->IsConnecting())
            {
                //printf("INFO:  Reclainm connect begin!!\r\n");
                connetVector.erase(iter);
		  // printf("INFO:  Reclainm connect 1!!\r\n");
                delete pCLink;
                pCLink = NULL;
		  // printf("INFO:  Reclainm connect end!!\r\n");
            }
            else
            {
                iter++;						//指针下移
            }
        }
        if(connetVector.size() == 0)
        {
            clientConnect = false;
        }
        OSA_mutexUnlock(&mutexConn);
	// printf("%s\n",__func__);
        seconds_sleep(TIME_FOR_THREAD_END);
    }

    //服务器端退出
    if(exitservThrd)
    {
        OSA_mutexLock(&mutexConn);
        CConnectVECTOR::iterator iter = connetVector.begin();
        for (iter; iter != connetVector.end();)
        {
            CConnect *pClient = (CConnect*)*iter;
            //如果客户端的连接还存在，则断开连接，线程退出
            if (pClient->IsConnecting())
            {
                pClient->DisConnecting();
            }
            ++iter;
        }
        OSA_mutexUnlock(&mutexConn);
        seconds_sleep(TIME_FOR_THREAD_END);
    }
    return 0;
}

