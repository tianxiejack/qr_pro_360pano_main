/*
 * ptzProxyMsg.cpp
 *
 *  Created on: 2019年3月19日
 *      Author: fsmdn121
 */


#include "udpselect.h"
#include "sys/socket.h"
#include "opencv2/highgui.hpp"
#include "stdio.h"
#include"netinet/in.h"
#include "ptzProxyMsg.h"
using namespace cv;
 ptzProxyMsg ptzmsg;
typedef enum{
	RECV_MOVE_UP = 0x01,
	RECV_MOVE_DOWN,
	RECV_MOVE_LEFT,
	RECV_MOVE_RIGHT,
	RECV_MOVE_UP_LEFT,
	RECV_MOVE_UP_RIGHT,
	RECV_MOVE_DOWN_LEFT,
	RECV_MOVE_DOWN_RIGHT,
	RECV_STOP,
	RECV_ZOOMIN,
	RECV_ZOOMOUT,
	RECV_FOCUSNEAR,
	RECV_FOCUSFAR,
	RECV_IRISOPEN,
	RECV_IRISCLOSE,
	RECV_SET_PTZ,
	RECV_GET_PTZ
}PTZ_CTL;


extern struct sockaddr_in mine_addr, other_addr, mine_addr1;
extern struct sockaddr_in servaddr;
extern struct sockaddr_in nearaddr;
//char buf[22] = { 0 };
//char buf1[26] = { 0 };
extern	udp_ret mine_ret, mine_ret1;
extern	fd_set readfds, recordfds;
extern	socklen_t len_other;

#if 0
int main()
{
	udp_init();
	while(1)
	{
		char key ;
		scanf("%c",&key);
		switch(key)
		{
		case '1':
		moveDownLeft();
			break;
		case '2':
			moveDown();
			break;
		case '3':
			moveDownRight();
			break;
		case '4':
			moveLeft();
			break;
		case '5':
			Stop();
			break;
		case '6':
			moveRight();
			break;
		case'7':
			moveUpLeft();
			break;
		case '8':
			moveUp();
			break;
		case '9':
			moveUpRight();
			break;
		case 'Z':
			zoomIn();
			break;
		case 'z':
			zoomOut();
			break;
		case 'F':
			focusNear();
			break;
		case 'f':
			focusFar();
			break;
		case 'I':
			irisOpen();
			break;
		case 'i':
			irisClose();
			break;
		default :
			break;
		}
	}

}
#endif

void ptzProxyMsg::initUDP()
{
	//udp_init();
}

void ptzProxyMsg::moveUp()
{
	char buf[22]={0};
	buf[17]=RECV_MOVE_UP;
	PackageToSend(buf);
}
void  ptzProxyMsg::moveDown()
{
	char buf[22]={0};
	buf[17]=RECV_MOVE_DOWN;
	PackageToSend(buf);
}
void  ptzProxyMsg::moveLeft()
{
	char buf[22]={0};
	buf[17]=RECV_MOVE_LEFT;
	PackageToSend(buf);
}
void  ptzProxyMsg::moveRight()
{
	char buf[22]={0};
	buf[17]=RECV_MOVE_RIGHT;
	PackageToSend(buf);
}
void  ptzProxyMsg::moveUpLeft()
{
	char buf[22]={0};
	buf[17]=RECV_MOVE_UP_LEFT;
	PackageToSend(buf);
}
void  ptzProxyMsg::moveUpRight()
{
	char buf[22]={0};
	buf[17]=RECV_MOVE_UP_RIGHT;
	PackageToSend(buf);
}
void  ptzProxyMsg::moveDownLeft()
{
	char buf[22]={0};
	buf[17]=RECV_MOVE_DOWN_LEFT;
	PackageToSend(buf);
}
void  ptzProxyMsg::moveDownRight()
{
	char buf[22]={0};
	buf[17]=RECV_MOVE_DOWN_RIGHT;
	PackageToSend(buf);
}
void  ptzProxyMsg::Stop()
{
	char buf[22]={0};
	buf[17]=RECV_STOP;
	PackageToSend(buf);
}
void  ptzProxyMsg::zoomIn()
{
	char buf[22]={0};
	buf[17]=RECV_ZOOMIN;
	PackageToSend(buf);
}
void  ptzProxyMsg::zoomOut()
{
	char buf[22]={0};
	buf[17]=RECV_ZOOMOUT;
	PackageToSend(buf);
}
void  ptzProxyMsg::focusNear()
{
	char buf[22]={0};
	buf[17]=RECV_FOCUSNEAR;
	PackageToSend(buf);
}
void  ptzProxyMsg::focusFar()
{
	char buf[22]={0};
	buf[17]=RECV_FOCUSFAR;
	PackageToSend(buf);
}
void  ptzProxyMsg::irisOpen()
{
	char buf[22]={0};
	buf[17]=RECV_IRISOPEN;
	PackageToSend(buf);
}
void  ptzProxyMsg::irisClose()
{
	char buf[22]={0};
	buf[17]=RECV_IRISCLOSE;
	PackageToSend(buf);
}


void  ptzProxyMsg::PackageToSend(char *buf)
{
	int ll;
	buf[0]=buf[1]=buf[2]=0;
	buf[3]=0xcb;
	buf[4]=buf[5]=buf[6]=0;
	buf[7]=0xc9;
	buf[8]=buf[9]=buf[10]=0;
	buf[11]=0x02;
	buf[12]=buf[13]=buf[14]=buf[15]=0;
	buf[16]=0;
	buf[18]=buf[19]=buf[20]=0;
	buf[21]=(char)(buf[16]+buf[17]);
	//ll = sendto(mine_ret.sockfd, buf, 22, 0,
	//			(struct sockaddr*) &servaddr, sizeof(servaddr));
}

