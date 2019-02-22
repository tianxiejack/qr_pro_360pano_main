#include "CUartProcess.hpp"

CUartProcess::CUartProcess()
{
    m_UartRun=false;
    m_port=-1;
    _globalDate= getDate();
}

CUartProcess::~CUartProcess()
{
    m_UartRun=false;
    OSA_thrJoin(&uartGetDataThrID);
    OSA_thrJoin(&uartSendDataThrID);
}

int CUartProcess::create()
{
    m_UartRun=true;
    openPort("/dev/ttyTHS1");
    setPort(115200, 8, 'N', 1);

    return 0;
}

int CUartProcess::openPort(char *dev_name)
{
    if((m_port = open(dev_name, O_RDWR| O_NOCTTY)) <= 0)
    {
        printf("ERR: Can not open the Uart port --(%s)\r\n",dev_name);
        exit(-1);
    }
    else
        printf("open uart port %s success.fd=%d\n",dev_name, m_port);

    return 0;
}

int CUartProcess::closePort()
{
    return close(m_port);
}

int CUartProcess::setPort(const int baud_rate, const int data_bits, char parity, const int stop_bits)
{
    struct termios newtio;
    memset(&newtio,  0, sizeof(newtio));

    if(tcgetattr(m_port, &newtio))
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
    tcflush(m_port, TCIOFLUSH);
    if(tcsetattr(m_port, TCSANOW, &newtio)){
        return -1;
    }
	
    printf( "tcsetattr done, baud_rate:%d, data_bits:%d, parity:%c, stop_bit:%d\n", baud_rate, data_bits, parity, stop_bits);
    return 0;
}

void CUartProcess::run()
{
    int iret;

    iret = OSA_thrCreate(&uartGetDataThrID,
        uartGetDataThrFun,
        0,
        0,
        (void*)this);

    iret = OSA_thrCreate(&uartSendDataThrID,
        uartSendDataThrFun,
        0,
        0,
        (void*)this);
}

int CUartProcess::recvData()
{
	int sizeRcv;
 	uint8_t dest[1024]={0};
	int read_status = 0;
	int dest_cnt = 0;
	unsigned char  tmpRcvBuff[RECV_BUF_SIZE];
	memset(tmpRcvBuff,0,sizeof(tmpRcvBuff));

	unsigned int uartdata_pos = 0;
	unsigned char frame_head[]={0xEB, 0x53};
	static struct data_buf
	{
		unsigned int len;
		unsigned int pos;
		unsigned char reading;
		unsigned char buf[RECV_BUF_SIZE];
	}swap_data = {0, 0, 0,{0}};

	memset(&swap_data,0,sizeof(swap_data));

	while(m_UartRun)
	{
		sizeRcv= UartRecv(tmpRcvBuff,RECV_BUF_SIZE);
		
		uartdata_pos = 0;
		if(sizeRcv>0)
		{
			printf("------------------Uart start recv date---------------------\n");
			for(int j=0;j<sizeRcv;j++)
			{
				   printf("%02x ",tmpRcvBuff[j]);
			}
			printf("\n");
			while (uartdata_pos< sizeRcv)
	    		{
	        		if((0 == swap_data.reading) || (2 == swap_data.reading))
	       		{
	            			if(frame_head[swap_data.len] == tmpRcvBuff[uartdata_pos])
	            			{
	                			swap_data.buf[swap_data.pos++] =  tmpRcvBuff[uartdata_pos++];
	                			swap_data.len++;
	                			swap_data.reading = 2;
	                			if(swap_data.len == sizeof(frame_head)/sizeof(char))
	                    				swap_data.reading = 1;
	            			}
		           		 else
		            		{
		                		uartdata_pos++;
		                		if(2 == swap_data.reading)
		                    		memset(&swap_data, 0, sizeof(struct data_buf));
		            		}
	    			}
		        	else if(1 == swap_data.reading)
			        {
			      		swap_data.buf[swap_data.pos++] = tmpRcvBuff[uartdata_pos++];
						swap_data.len++;
					if(swap_data.len>=4)
					{
						if(swap_data.len==((swap_data.buf[2]|(swap_data.buf[3]<<8))+5))
						{
							if(swap_data.buf[4]==0x35)
							{
								printf("%s,%d,it is upgrade fw\n",__FILE__,__LINE__);
								upgradefw(swap_data.buf,swap_data.len);
							}
							else if(swap_data.buf[4]==0x33)
							{
								printf("%s,%d,it is export config file\n",__FILE__,__LINE__);
								expconfig(swap_data.buf,swap_data.len);
							}
							else
							{
								for(int i=0;i<swap_data.len;i++)
								{
									_globalDate->rcvBufQue.push_back(swap_data.buf[i]);
								}
								prcRcvFrameBufQue(1);
							}
							memset(&swap_data, 0, sizeof(struct data_buf));
						}
					}
			        }
			}
		}
	}
	return 0;
}

int CUartProcess::sendData()
{
    int mRespCmd;
    int n;
    int result,retVle;
    struct timeval    sendTimeout;
	while(m_UartRun){
		printf("****************************%s**********************************************************\n",__func__);
		printf("****************************%s**********************************************************\n",__func__);
		OSA_semWait(&_globalDate->m_semHndl,OSA_TIMEOUT_FOREVER);
		printf("****************************%s*********start*************************************************\n",__func__);
		printf("****************************%s*********start*************************************************\n",__func__);
		getSendInfo(_globalDate->feedback, &_globalDate->repSendBuffer);
		retVle = write(m_port, &_globalDate->repSendBuffer.sendBuff, _globalDate->repSendBuffer.byteSizeSend);
		if(_globalDate->feedback == ACK_config_Read)
			OSA_semSignal(&_globalDate->m_semHndl_s);
		printf("%s,%d, write to uart(fd:%d) %d bytes:\n", __FILE__,__LINE__,m_port, retVle);
		for(int i = 0; i < retVle; i++)
		{
			printf("%02x ", _globalDate->repSendBuffer.sendBuff[i]);
		}
		printf("\n");
		
	  }
	return 0;
}

int  CUartProcess::UartRecv(void *rcv_buf,int data_len)
{
		int fs_sel,len;
		fd_set fd_uartRead;
		struct timeval timeout;
		FD_ZERO(&fd_uartRead);
		FD_SET(m_port,&fd_uartRead);

	    timeout.tv_sec = 0;
		timeout.tv_usec = 50000;

		fs_sel = select(m_port+1,&fd_uartRead,NULL,NULL,&timeout);
		if(fs_sel){
			len = read(m_port,rcv_buf,data_len);
			return len;
		}
		else if(-1 == fs_sel){
			printf("ERR: Uart Recv  select  Error!!\r\n");
			return -1;
		}
		else if(0 == fs_sel){
			//printf("Warning: Uart Recv  time  out!!\r\n");
			return 0;
		}
}

int CUartProcess::expconfig(unsigned char *swap_data_buf, unsigned int swap_data_len)
{
	printf("it is export config\n");
	int write_len = 0;
	int len = 0;
	FILE *fp;
	int filesize = 0;
	int sendsize = 0;
	int packet_flag;
	unsigned char checksum = 0;
	unsigned char usocket_send_buf[1024+256] = {0};
	unsigned char buf[1024+256] = {0};

	unsigned char cksum = 0;
	for(int n=1;n<swap_data_len-1;n++)
	{
		cksum ^= swap_data_buf[n];
	}
	if(cksum !=swap_data_buf[swap_data_len-1])
	{
		printf("checksum error,cal is %02x, recv is %02x\n",cksum,swap_data_buf[swap_data_len-1]);
		return -1;
	}
	
	if(NULL ==(fp = fopen("/home/ubuntu/dss_bin/Profile.yml","r")))
	{
		perror("fopen\r\n");
		return -1;
	}

	fseek(fp,0,SEEK_END);
	filesize = ftell(fp);
	fseek(fp,0,SEEK_SET);

	usocket_send_buf[0] = 0xEB;
	usocket_send_buf[1] = 0x53;
	usocket_send_buf[4] = 0x33;
	usocket_send_buf[5] = filesize&0xff;
	usocket_send_buf[6] = (filesize>>8)&0xff;
	usocket_send_buf[7] = (filesize>>16)&0xff;
	usocket_send_buf[8] = (filesize>>24)&0xff;
	packet_flag = 0;

	while(len = fread(buf,1,1024,fp))
	{
		checksum = 0;
		if(len<0)
		{
			printf("read error!\n");
			break;
		}
		sendsize += len;
		if(packet_flag == 0)
		{
			if(sendsize == filesize)
			{
				usocket_send_buf[9] = 2;
			}
			else
			{
				usocket_send_buf[9] = 0;
			}
			packet_flag = 1;
		}
		else if(sendsize == filesize)
		{
			usocket_send_buf[9] = 2;
			packet_flag = 0;
		}
		else
		{
			usocket_send_buf[9] = 1;
		}
		usocket_send_buf[2] = (len+8)&0xff;
		usocket_send_buf[3] = ((len+8)>>8)&0xff;
		usocket_send_buf[10] = len&0xff;
		usocket_send_buf[11] = (len>>8)&0xff;
		memcpy(usocket_send_buf+12,buf, len);
		for(int m = 1; m<12+len;m++)
			checksum ^= usocket_send_buf[m];
		usocket_send_buf[12+len] = checksum;

		write_len = write(m_port,usocket_send_buf,len+13);
		printf("write to uart %d bytes:\n", write_len);
		for(int n = 0; n < write_len; n++)
			printf("%02x ", usocket_send_buf[n]);
		printf("\n");
		if(-1 == write_len)
		{
			perror("write fail");
		}

	}
	if(sendsize == filesize)
	{
		printf("export success\n");
	}
	else
	{
		printf("export fail\n");
	}
	fclose(fp);
}
