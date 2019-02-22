/*
 * Udpbase.hpp
 *
 *  Created on: 2018年11月14日
 *      Author: wj
 */

#ifndef UDPBASE_HPP_
#define UDPBASE_HPP_

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

class UDPBase
{
public:
	UDPBase();
    ~UDPBase();
	int NetOpen();
	int NetBind(int fd, int port);
	int NetSend(int fd, unsigned char *sendBuff, int sendsize);
	int NetRecv(int fd, unsigned char *recvbuff, int recvsize);
	int NetRead(int fd, unsigned char *recvbuff, int recvsize);
	int NetClose(int fd);
};



#endif /* UDPBASE_HPP_ */
