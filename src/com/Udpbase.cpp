#include "Udpbase.hpp"

UDPBase::UDPBase()
{

}

UDPBase::~UDPBase()
{

}

int UDPBase::NetOpen()
{
	int option = 1;
	
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0)
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
	

	
    return fd;
}

int UDPBase::NetBind(int fd, int port)
{
    int iRet;
    char ipbuf[20];
    char *ipaddr=ipbuf;

    struct sockaddr_in servAddr;

    memset(&servAddr,0,sizeof(servAddr));	
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons( port);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    printf("INFO: server  addr:%d---port: %d\r\n",servAddr.sin_addr.s_addr,servAddr.sin_port);

    iRet = bind(fd,(struct sockaddr*)&servAddr,sizeof(servAddr));
    if (iRet<0){
        printf("ERR:  Can not bind  Socketfd, error: %s(errno: %d)\r\n ",strerror(errno),errno);
    }

    return  0 ;
}





int UDPBase::NetSend(int fd, unsigned char *sendBuff, int sendsize)
{
	return write(fd, sendBuff, sendsize);
}

int UDPBase::NetRecv(int fd, unsigned char *recvbuff, int recvsize)
{
    int fs_sel,len;
    fd_set fd_netRead;
    struct timeval timeout;
	int sizeRcv = -1;

	FD_ZERO(&fd_netRead);
    FD_SET(fd,&fd_netRead);
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    fs_sel = select(fd+1,&fd_netRead,NULL,NULL,&timeout);
    if(0 == fs_sel)//timeout
    {
        return fs_sel;
    }
    else if(-1 == fs_sel){
        printf("ERR: Socket select Error!!\n");
        return fs_sel;
    }
	else
	{
		sizeRcv = NetRead(fd ,recvbuff,recvsize);
		if((fs_sel==1)&&(sizeRcv==0))		
		{
			return -2;
		}
		else
			return sizeRcv;
	}
}

int UDPBase::NetRead(int fd, unsigned char *recvbuff, int recvsize)
{
	return read(fd, recvbuff, recvsize);
}

int UDPBase::NetClose(int fd)
{
    return close(fd);
}
