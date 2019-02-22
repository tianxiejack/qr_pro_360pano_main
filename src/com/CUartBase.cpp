#include "CUartBase.hpp"
#include "stdio.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <errno.h>


#include <stdio.h>      /*\u6807\u51c6\u8f93\u5165\u8f93\u51fa\u5b9a\u4e49*/
#include <stdlib.h>     /*\u6807\u51c6\u51fd\u6570\u5e93\u5b9a\u4e49*/
#include <unistd.h>     /*Unix \u6807\u51c6\u51fd\u6570\u5b9a\u4e49*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>      /*\u6587\u4ef6\u63a7\u5236\u5b9a\u4e49*/
#include <termios.h>    /*PPSIX \u7ec8\u7aef\u63a7\u5236\u5b9a\u4e49*/
#include <errno.h>      /*\u9519\u8bef\u53f7\u5b9a\u4e49*/
#include <string.h>


#include <sys/ioctl.h>
#include <linux/serial.h>


CUartBase::CUartBase()
{

}

CUartBase::~CUartBase()
{

}

int CUartBase::UartOpen(char *dev_name)
{
	int m_port = -1;
	
    if((m_port = open(dev_name, O_RDWR| O_NOCTTY |O_NDELAY)) <= 0)
    {
        printf("ERR: Can not open the Uart port --(%s)\r\n",dev_name);
        exit(-1);
    }
    else
        printf("open uart port %s success.fd=%d\n",dev_name, m_port);

    return m_port;
}




int set_speci_baud(int fd, int baud)
{



#if 1
	struct serial_struct ss, ss_set;



	if((ioctl(fd, TIOCGSERIAL, &ss)) < 0)
	{
	printf("BAUD: error to get the serial_struct info:%s\n", strerror(errno));
	return -1;
	}


	ss.flags = ASYNC_SPD_CUST;
	ss.custom_divisor = ss.baud_base / baud;
	if((ioctl(fd, TIOCSSERIAL, &ss)) < 0)
	{
	printf("BAUD: error to set serial_struct:%s\n", strerror(errno));
	return -2;
	}


	ioctl(fd, TIOCGSERIAL, &ss_set);
	printf("BAUD: success set baud to %d,custom_divisor=%d,baud_base=%d\n",
	      baud, ss_set.custom_divisor, ss_set.baud_base);
#endif

return 0;
}

int CUartBase::UartSet(const int fd, const int baud_rate, const int data_bits, char parity, const int stop_bits)
{
    struct termios newtio;
    memset(&newtio,  0, sizeof(newtio));

    if(tcgetattr(fd, &newtio))
    {
        printf( "tcgetattr error : %s\n", strerror(errno));
        return -1;
    }
	struct serial_struct ss, ss_set;
	int baud=128000;
    //set baud rate
    switch (baud_rate)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 19200:
            cfsetispeed(&newtio, B19200);
            cfsetospeed(&newtio, B19200);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
	case 1:
		//set_speci_baud(fd,1843200);
		//set_speci_baud(fd,460800);
		//set_speci_baud(fd,128000);
		//baud=128000;
		//cfsetispeed(&newtio, B38400);
		//cfsetospeed(&newtio, B38400);


		//tcflush(fd, TCIFLUSH);/*handle unrecevie char*/
		//tcsetattr(fd, TCSANOW, &newtio);

/*
		if((ioctl(fd, TIOCGSERIAL, &ss)) < 0)
		{
		printf("BAUD: error to get the serial_struct info:%s\n", strerror(errno));
		return -1;
		}


		ss.flags = ASYNC_SPD_CUST;
		ss.custom_divisor = ss.baud_base / baud;
		if((ioctl(fd, TIOCSSERIAL, &ss)) < 0)
		{
		printf("BAUD: error to set serial_struct:%s\n", strerror(errno));
		return -2;
		}


		ioctl(fd, TIOCGSERIAL, &ss_set);
		printf("BAUD: success set baud to %d,custom_divisor=%d,baud_base=%d\n",
		      baud, ss_set.custom_divisor, ss_set.baud_base);
		    */
		break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
    }



    //set data bits
    newtio.c_cflag &= ~CSIZE;
    switch(data_bits)
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio .c_cflag |= CS8;
            break;
        default:
            newtio.c_cflag |= CS8;
            break;
    }
    //set stop bits
    switch (stop_bits)
    {
        default:
        case 1:
            newtio.c_cflag &= ~CSTOPB;
            break;
        case 2:
            newtio.c_cflag |= CSTOPB;
            break;
    }
    //set parity
    switch (parity)
    {
        default:
        case 'n':
        case 'N':
            newtio.c_cflag &= ~PARENB;
            newtio.c_iflag &= ~INPCK;
            break;
        case 'o':
        case 'O':
            newtio.c_cflag |= (PARODD|PARENB);
            newtio.c_iflag |= INPCK;
            break;
        case 'e':
        case 'E':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            newtio.c_cflag |= INPCK;
            break;
        case 's':
        case 'S':
            newtio.c_cflag &= ~PARENB;
            newtio.c_cflag &= ~CSTOPB;
            newtio.c_cflag |= INPCK;
            break;
    }

    newtio.c_cflag |= (CLOCAL | CREAD);
    newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    newtio.c_oflag &= ~OPOST;                 // 杈撳嚭鏁版嵁妯″紡锛屽師濮嬫暟鎹?    Opt.c_oflag &= ~(ONLCR | OCRNL);          //娣诲姞鐨?
    newtio.c_iflag &= ~(ICRNL | INLCR);
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY);   // 涓嶄娇鐢ㄨ蒋浠舵祦鎺э紱
    newtio.c_cflag &= ~CRTSCTS;   //  涓嶄娇鐢ㄦ祦鎺у埗

    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN]  = 1;
    tcflush(fd, TCIOFLUSH);
    if(tcsetattr(fd, TCSANOW, &newtio)){
        return -1;
    }

    if(baud_rate==1)
		set_speci_baud(fd,128000);
	
    printf( "tcsetattr done, baud_rate:%d, data_bits:%d, parity:%c, stop_bit:%d\n", baud_rate, data_bits, parity, stop_bits);
    return 0;
}

int CUartBase::UartSend(int fd, unsigned char *sendBuff, int sendsize)
{
	return write(fd, sendBuff, sendsize);
}

int CUartBase::UartRecv(int fd, unsigned char *recvbuff, int recvsize)
{
    int fs_sel,len;
    fd_set fd_uartRead;
    struct timeval timeout;

	FD_ZERO(&fd_uartRead);
    FD_SET(fd,&fd_uartRead);
    timeout.tv_sec = 50;
    timeout.tv_usec = 50000;
        
    fs_sel = select(fd+1,&fd_uartRead,NULL,NULL,&timeout);
	if(0 == fs_sel)//timeout
    {
        return fs_sel;
    }
    else if(-1 == fs_sel)
    {
        printf("ERR: Uart Recv select Error!\r\n");
        return fs_sel;
    }
	else
	{
		return UartRead(fd ,recvbuff,recvsize);
	}
}

int CUartBase::UartRead(int fd, unsigned char *recvbuff, int recvsize)
{
	return read(fd, recvbuff, recvsize);
}

int CUartBase::UartClose(int fd)
{
    return close(fd);
}
