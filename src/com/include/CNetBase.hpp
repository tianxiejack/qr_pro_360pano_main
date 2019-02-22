#ifndef __CNETBASE_H__
#define __CNETBASE_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

class CNetBase
{
public:
    CNetBase();
    ~CNetBase();
	int NetOpen();
    int NetBind(int fd, int port);
    int NetListen(int fd, int maxnum);
	int NetAccept(int fd, struct sockaddr* addr, socklen_t* length);
	int NetSend(int fd, unsigned char *sendBuff, int sendsize);
	int NetRecv(int fd, unsigned char *recvbuff, int recvsize);
	int NetRead(int fd, unsigned char *recvbuff, int recvsize);
	int NetClose(int fd);
};
#endif