/*
 * udpselect.h
 *
 *  Created on: 2018年4月22日
 *      Author: jet
 */


#ifndef UDPSELECT_H_
#define UDPSELECT_H_

typedef struct {
	int sockfd;
	int ret_bind;
	int ret_select;
}udp_ret;

int udp_init();
int udp_recv(char *buf,int bufsize);
void SendPowerOnSelfTest(); //给载员显示器发送开机自检信息
void sendFarSelfTest(char *farresult); //参数是一个不小于10的数组，按字节保存从0~9相机的状态，0为异常，1为正常

#endif /* UDPSELECT_H_ */

