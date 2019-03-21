/*
 * udpselect.c
 *
 *  Created on: 2018年4月22日
 *      Author:xz
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "udpselect.h"

struct timeval tv;
struct sockaddr_in mine_addr, other_addr, mine_addr1;
struct sockaddr_in servaddr;
struct sockaddr_in nearaddr;
//char buf[22] = { 0 };
//char buf1[26] = { 0 };
udp_ret mine_ret, mine_ret1;
fd_set readfds, recordfds;
socklen_t len_other;

int udp_init_one(udp_ret *ret_value, struct sockaddr_in *sock_addr,
		int port_num, char* ip_num) {
	ret_value->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (ret_value->sockfd < 0) {
		perror("fail to socket!");
		return -1;
	}
	bzero(sock_addr, sizeof(struct sockaddr_in));
	sock_addr->sin_family = AF_INET;
	sock_addr->sin_port = htons(port_num);
	sock_addr->sin_addr.s_addr = inet_addr(ip_num);
	ret_value->ret_bind = bind(ret_value->sockfd, (struct sockaddr*) sock_addr,
			sizeof(struct sockaddr_in));
	if (ret_value->ret_bind < 0) {
		perror("fail to bind!\n");
		return -2;
	}
	return 0;
}

int udp_init() {
	udp_init_one(&mine_ret, &mine_addr, 6664, "0.0.0.0");
//	udp_init_one(&mine_ret1, &mine_addr1, 6665, "0.0.0.0");

	len_other = sizeof(other_addr);
	FD_ZERO(&recordfds);
	FD_SET(mine_ret.sockfd, &recordfds);
	FD_SET(mine_ret1.sockfd, &recordfds);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.85"); //载员ip 192.9.200.203
	servaddr.sin_port = htons(6664); //载员网络接收端口
#if 0
	bzero(&nearaddr, sizeof(nearaddr));
	nearaddr.sin_family = AF_INET;
	nearaddr.sin_addr.s_addr = inet_addr("192.168.1.85"); //近景板ip 192.9.200.202
	nearaddr.sin_port = htons(6664); //载员网络接收端口
#endif
	return 0;
}

int udp_recv(char *buf, int bufsize) {
	int maxfd;
	int i = bufsize;
	int len_recv = 0;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	readfds = recordfds;
	maxfd = (mine_ret.sockfd > mine_ret1.sockfd) ?
			mine_ret.sockfd : mine_ret1.sockfd;
	mine_ret.ret_select = select(maxfd + 1, &readfds, NULL, NULL, &tv);
	if (mine_ret.ret_select < 0) {
		perror("fail to select!\n");
		return -3;
	} else if (mine_ret.ret_select == 0) {
	} else {
		bzero(buf, bufsize);
		bzero(&other_addr, sizeof(other_addr));
		if (FD_ISSET(mine_ret.sockfd, &readfds)) {
			len_recv = recvfrom(mine_ret.sockfd, buf, bufsize, 0,
					(struct sockaddr*) &other_addr, &len_other);
		} else if (FD_ISSET(mine_ret1.sockfd, &readfds)) {
			len_recv = recvfrom(mine_ret1.sockfd, buf, bufsize, 0,
					(struct sockaddr*) &other_addr, &len_other);
		}

#if Debug_Test
		printf("IP:%s,port:%d recvfrom:", inet_ntoa(other_addr.sin_addr),
				ntohs(other_addr.sin_port));
#endif
	}
	return len_recv;
}



void SendPowerOnSelfTest() {
	int ll;
/*	buf[0] = buf[1] = buf[2] = 0;
	buf[3] = 0xc9;
	buf[4] = buf[5] = buf[6] = 0;
	buf[7] = 0xcb;
	buf[8] = buf[9] = buf[10] = 0;
	buf[11] = 0x2;
	buf[12] = buf[13] = buf[14] = 0;
	buf[15] = 0x00;
	buf[16] = 0x00;
	buf[17] = 0x01;
	buf[18] = buf[19] = buf[20] = 0;
	buf[21] = 0x01;

	ll = sendto(mine_ret.sockfd, buf, sizeof(buf), 0,
			(struct sockaddr*) &servaddr, sizeof(servaddr));*/
}
void sendFarSelfTest(char *farresult) {
	int len;
	/*buf1[0] = buf1[1] = buf1[2] = 0;
	buf1[3] = 0xc9;
	buf1[4] = buf1[5] = buf1[6] = 0;
	buf1[7] = 0xca;
	buf1[8] = buf1[9] = buf1[10] = 0;
	buf1[11] = 0x0a;
	buf1[12] = buf1[13] = buf1[14] = buf1[15] = 0x01;
	memcpy(&buf1[16], farresult, 10);
	len = sendto(mine_ret.sockfd, buf1, sizeof(buf1), 0,
			(struct sockaddr*) &nearaddr, sizeof(nearaddr));*/
} //参数是一个不小于10的数组，按字节保存从0~9相机的状态，0为异常，1为正常

