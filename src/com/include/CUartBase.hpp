#ifndef __CUARTBASE_H__
#define __CUARTBASE_H__


class CUartBase{
public:
    CUartBase();
    ~CUartBase();
	int UartOpen(char *dev_name);
	int UartSet(const int m_port, const int baud_rate, const int data_bits, char parity, const int stop_bits);
	int UartRecv(int fd, unsigned char *recvbuff, int recvsize);
	int UartRead(int fd, unsigned char *recvbuff, int recvsize);
	int UartSend(int fd, unsigned char *sendbuff, int sendsize);
    int UartClose(int fd);
};

#endif