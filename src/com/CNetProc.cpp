#include "CNetProc.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h> 
#include <net/if.h>

#define  MAXCONNECT 10

CNetProc::CNetProc(int tmpport)
{
	port = tmpport;
}

CNetProc::~CNetProc()
{

}

int CNetProc::copen()
{
	int option = 1;
	
    	m_Socketfd = socket(AF_INET,SOCK_STREAM,0);
    	if(m_Socketfd<0)
    	{
        		printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
				return -1;
    	}
    	setsockopt(m_Socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	
    	NetBind(m_Socketfd, port);
    	NetListen(m_Socketfd, MAXCONNECT);
}

int CNetProc::cclose()
{
	return close(m_Socketfd);
}

int CNetProc::crecv(int fd, void *buf,int len)
{
	int fs_sel,readlen;
	fd_set fd_netRead;
	struct timeval timeout;
	FD_ZERO(&fd_netRead);
	FD_SET(fd,&fd_netRead);
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	fs_sel = select(fd+1,&fd_netRead,NULL,NULL,&timeout);
	if(-1 == fs_sel)
	{
		printf("ERR: Socket  select  Error!!\r\n");
		return -1;
	}
	else if(fs_sel)
	{
		readlen = read(fd, buf, len);
		if((fs_sel==1)&&(readlen==0))
			return -2;
		else
			return readlen;
	}

	else if(0 == fs_sel)
	{
		//printf("Warning: Uart Recv  time  out!!\r\n");
		return 0;
	}
}

int CNetProc::csend(int fd, void *buf,int len)
{
	return write(fd, buf,len);
}

int CNetProc::NetBind(int fd, int port)
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

int CNetProc::NetListen(int fd,int maxnum)
{
    int ret;

    ret =listen(fd,maxnum);
    if(ret<0){
        printf("ERR: Can not listen sockfd ,error: %s(errno: %d)\r\n",strerror(errno),errno);
    }
    return 0;
}

int CNetProc::caccept(struct sockaddr*connectAddr,socklen_t* lenthConnect)
{
	return accept(m_Socketfd, connectAddr, lenthConnect);
}