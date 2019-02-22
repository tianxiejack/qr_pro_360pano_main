#include "CConnect.hpp"
#include <unistd.h>
#include"osa_sem.h"
#define RECVSIZE (32)
#define RECV_BUF_SIZE (1024)

CConnect::CConnect()
{
	m_connect = -1;
    bConnecting=false;
    _globalDate= getDate();
    //OSA_mutexCreate(&sendMux);
}

CConnect::~CConnect()
{
    bConnecting = false;
    OSA_semSignal(&_globalDate->m_semHndl_socket);
    OSA_semSignal(&_globalDate->m_semHndl_socket_s);
    //printf("%s  %d\n",__func__,__LINE__);
    OSA_thrJoin(&getDataThrID);
   // printf("%s  %d\n",__func__,__LINE__);
    OSA_thrJoin(&sendDataThrID);
  //  printf("%s  %d\n",__func__,__LINE__);
    //OSA_mutexDelete(&sendMux);
}

void CConnect::run()
{
    int iret;
    iret = OSA_thrCreate(&getDataThrID,
        getDataThrFun,
        0,
        0,
        (void*)this);

    iret = OSA_thrCreate(&sendDataThrID,
        sendDataThrFun,
        0,
        0,
        (void*)this);
}

int CConnect::recvData()
{
	int sizeRcv;
 	uint8_t dest[1024]={0};
	int read_status = 0;
	int dest_cnt = 0;
	unsigned char  tmpRcvBuff[RECV_BUF_SIZE];
	memset(tmpRcvBuff,0,sizeof(tmpRcvBuff));

	unsigned int uartdata_pos = 0;
	unsigned char frame_head[]={0xEB, 0x51};
	static struct data_buf
	{
		unsigned int len;
		unsigned int pos;
		unsigned char reading;
		unsigned char buf[RECV_BUF_SIZE];
	}swap_data = {0, 0, 0,{0}};
	memset(&swap_data,0,sizeof(swap_data));

	while(bConnecting){
		sizeRcv = NETRecv(tmpRcvBuff,RECVSIZE);
		uartdata_pos = 0;

		if(sizeRcv>0)
		{
#if 0
			printf("%s,%d,recv from socket %d bytes:\n",__FILE__,__LINE__,sizeRcv);
			for(int j=0;j<sizeRcv;j++){
				printf("%02x ",tmpRcvBuff[j]);
			}
			printf("\n");
#endif
			while (uartdata_pos < sizeRcv){
				if((0 == swap_data.reading) || (2 == swap_data.reading)){
					if(frame_head[swap_data.len] == tmpRcvBuff[uartdata_pos]){
						swap_data.buf[swap_data.pos++] =  tmpRcvBuff[uartdata_pos++];
						swap_data.len++;
						swap_data.reading = 2;
						if(swap_data.len == sizeof(frame_head)/sizeof(char))
						swap_data.reading = 1;
					}else{
						uartdata_pos++;
						if(2 == swap_data.reading)
							memset(&swap_data, 0, sizeof(struct data_buf));
					}
				}else if(1 == swap_data.reading){
					swap_data.buf[swap_data.pos++] = tmpRcvBuff[uartdata_pos++];
					swap_data.len++;
					if(swap_data.len>=4){

						
						if(swap_data.len==((swap_data.buf[2]|(swap_data.buf[3]<<8))+5)){


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
									_globalDate->rcvBufQue.push_back(swap_data.buf[i]);

								prcRcvFrameBufQue(2);
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

int CConnect::sendData()
{
    int retVle,n;

	while(bConnecting){
		OSA_semWait(&_globalDate->m_semHndl_socket,OSA_TIMEOUT_FOREVER);
		if(0 == bConnecting)
			break;
		getSendInfo(_globalDate->feedback, &_globalDate->repSendBuffer);
		retVle = write(m_connect, &_globalDate->repSendBuffer.sendBuff,_globalDate->repSendBuffer.byteSizeSend);
		if(_globalDate->feedback == ACK_config_Read || _globalDate->feedback == ACK_param_todef)
			OSA_semSignal(&_globalDate->m_semHndl_socket_s);
		//printf("\n");
		//printf("%s,%d, write to socket %d bytes:\n", __FILE__,__LINE__,retVle);
#if 0
		for(int i = 0; i < retVle; i++)
		{
			printf("%02x ", _globalDate->repSendBuffer.sendBuff[i]);
		}
		printf("-----end-----\n");
#endif
	}
	return 0;
}


int  CConnect::NETRecv(void *rcv_buf,int data_len)
{
		int fs_sel,len;
		fd_set fd_netRead;
		struct timeval timeout;
		FD_ZERO(&fd_netRead);
		FD_SET(m_connect,&fd_netRead);
	    timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		fs_sel = select(m_connect+1,&fd_netRead,NULL,NULL,&timeout);
		if(fs_sel){
			len = read(m_connect,rcv_buf,data_len);
			if((fs_sel==1)&&(len==0))
				bConnecting = false;
			return len;
		}
		else if(-1 == fs_sel){
			printf("ERR: Socket  select  Error!!\r\n");
			return -1;
		}
		else if(0 == fs_sel){
			//printf("Warning: Uart Recv  time  out!!\r\n");
			return 0;
		}
}
int CConnect::expconfig(unsigned char *swap_data_buf, unsigned int swap_data_len)
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
	
	if(NULL ==(fp = fopen("/home/ubuntu/Release/config.xml","r")))
	{
		perror("fopen\r\n");
		return -1;
	}

	fseek(fp,0,SEEK_END);
	filesize = ftell(fp);
	fseek(fp,0,SEEK_SET);

	usocket_send_buf[0] = 0xEB;
	usocket_send_buf[1] = 0x51;
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

		write_len = write(m_connect,usocket_send_buf,len+13);
		printf("write to net %d bytes:\n", write_len);
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











