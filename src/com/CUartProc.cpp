#include "CUartProc.hpp"
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>




CUartProc::CUartProc(const string dev_name,const int baud_rate, const int flow, const int data_bits, const char parity, const int stop_bits)
{
	devname = dev_name;
	baudrate = baud_rate;
	c_flow = flow;
	databits = data_bits;
	paritybits = parity;
	stopbits = stop_bits;
}

CUartProc::~CUartProc()
{

}

int CUartProc::copen()
{
	int m_port = -1;
	 if((m_port = open(devname.c_str(), O_RDWR| O_NOCTTY)) <= 0)
	    {
	        printf("ERR: Can not open the Uart port --(%s)\r\n",devname.c_str());
	        return -1;
	    }
	    else
	        printf("open uart port %s success.fd=%d\n",devname.c_str(), m_port);

	setPort(m_port, baudrate, c_flow, databits, paritybits, stopbits);
	
	return m_port;
}

int CUartProc::cclose()
{

}

int CUartProc::crecv(int fd, void *buf,int len)
{
	int fs_sel;
	fd_set fd_uartRead;
	struct timeval timeout;
	FD_ZERO(&fd_uartRead);
	FD_SET(fd,&fd_uartRead);

	timeout.tv_sec = 0;
	timeout.tv_usec = 50000;

	fs_sel = select(fd+1,&fd_uartRead,NULL,NULL,&timeout);
	if(-1 == fs_sel)
	{
		printf("ERR: Uart Recv  select  Error!!\r\n");
		return -1;
	}
	else if(fs_sel)
	{
		return  read(fd,buf,len);
	}
	else if(0 == fs_sel)
	{
		//printf("Warning: Uart Recv  time  out!!\r\n");
		return 0;
	}
}

int CUartProc::csend(int fd, void *buf,int len)
{
	return write(fd, buf, len);
}

int CUartProc::setPort(int fd, const int baud_rate, const int c_flow, const int data_bits, char parity, const int stop_bits)
{
    struct termios newtio;
    memset(&newtio,  0, sizeof(newtio));

    if(tcgetattr(fd, &newtio))
    {
        printf( "tcgetattr error : %s\n", strerror(errno));
        return -1;
    }
    //set baud rate
    switch (baud_rate)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
	   printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 19200:
            cfsetispeed(&newtio, B19200);
            cfsetospeed(&newtio, B19200);
	   printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 38400:
            cfsetispeed(&newtio, B38400);
            cfsetospeed(&newtio, B38400);
	   printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 57600:
            cfsetispeed(&newtio, B57600);
            cfsetospeed(&newtio, B57600);
	   printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
	   printf("[setPort]baudrate:%d\n", baud_rate);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            printf("[setPort]baudrate:%d\n", baud_rate);
    }


    newtio.c_cflag |= CLOCAL;
    newtio.c_cflag |= CREAD;//3
    

    switch(c_flow)
    {
        default:
        case 0:// none
            newtio.c_cflag &= ~CRTSCTS;
	   printf("[setPort]c_flow:none\n");
            break;
        case 1:// hardware
            newtio.c_cflag |= CRTSCTS;
	   printf("[setPort]c_flow:hardware\n");
            break;
        case 2:// software
            newtio.c_cflag |= IXON|IXOFF|IXANY;
	   printf("[setPort]c_flow:software\n");
            break;
    }


    //set data bits
    newtio.c_cflag &= ~CSIZE;
    switch(data_bits)
    {
        case 5:
	    newtio.c_cflag |= CS5;
	    printf("[setPort]data_bits:5\n");
	    break;
        case 6:
	    newtio.c_cflag |= CS6;
	    printf("[setPort]data_bits:6\n");
	    break;
        case 7:
            newtio.c_cflag |= CS7;
	   printf("[setPort]data_bits:7\n");
            break;
        case 8:
            newtio .c_cflag |= CS8;
	   printf("[setPort]data_bits:8\n");
            break;
        default:
            newtio.c_cflag |= CS8;
            printf("[setPort]data_bits:8\n");
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
	   printf("[setPort]parity:N\n");
            break;
        case 'o':
        case 'O':
            newtio.c_cflag |= (PARODD|PARENB);
            newtio.c_iflag |= INPCK;
	   newtio.c_iflag |= ISTRIP;
	   printf("[setPort]parity:O\n");
            break;
        case 'e':
        case 'E':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            newtio.c_cflag |= INPCK;
            newtio.c_iflag |= ISTRIP;
	   printf("[setPort]parity:E\n");
            break;
        case 's':
        case 'S':
            newtio.c_cflag &= ~PARENB;
            newtio.c_cflag &= ~CSTOPB;
            //newtio.c_cflag |= INPCK;
            printf("[setPort]parity:S\n");
            break;
    }

    //set stop bits
    switch (stop_bits)
    {
        default:
        case 1:
            newtio.c_cflag &= ~CSTOPB;
	    printf("[setPort]stop_bits:1\n");
            break;
        case 2:
            newtio.c_cflag |= CSTOPB;
	   printf("[setPort]stop_bits:2\n");
            break;
    }
	
    newtio.c_cflag |= (CLOCAL | CREAD);
    newtio.c_oflag &= ~OPOST;                 // 杈撳嚭鏁版嵁妯″紡锛屽師濮嬫暟鎹?    Opt.c_oflag &= ~(ONLCR | OCRNL);          //娣诲姞鐨?
    newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
   
    newtio.c_iflag &= ~(ICRNL | INLCR);
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY);   // 涓嶄娇鐢ㄨ蒋浠舵祦鎺э紱
    newtio.c_cflag &= ~CRTSCTS;   //  涓嶄娇鐢ㄦ祦鎺у埗

    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN]  = 1;
    tcflush(fd, TCIOFLUSH);
    if(tcsetattr(fd, TCSANOW, &newtio)){
        return -1;
    }
	
    //printf( "tcsetattr done, baud_rate:%d, c_flow:%d, data_bits:%d, parity:%c, stop_bit:%d\n", baud_rate, c_flow, data_bits, parity, stop_bits);
    return 0;
}
