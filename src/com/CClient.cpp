#include "CClient.hpp"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define  ServerPort  (1234)

CClient::CClient()
{
    m_Socketfd=0;
    exitservThrd=false;
    memset(&netsenddataThrd,0,sizeof(netsenddataThrd));
    memset(&reconnectThrd,0,sizeof(reconnectThrd));
}

CClient::~CClient()
{
}

int CClient::create()
{	
    m_Socketfd = openPort();
    NetConnect(m_Socketfd, ServerPort);
	
    return 0;
}

int CClient::openPort()
{
	int option = 1;
	
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd<0)
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
	
    //setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	
    return fd;
}

int CClient::closePort()
{
    return close(m_Socketfd);
}

void CClient::run()
{
    int retv;

    retv=OSA_thrCreate(&netsenddataThrd,
        SendDataFunc,
        0,
        0,
        (void*)this);

    retv=OSA_thrCreate(&reconnectThrd,
        ReConnectFunc,
        0,
        0,
        (void*)this);
}

int CClient::NetConnect(int fd, int port)
{
    int iRet;
    char ipbuf[20];
    char *ipaddr=ipbuf;
    struct ifreq ifr;
    struct sockaddr_in servAddr;

    memset(&servAddr,0,sizeof(servAddr));	
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons( port);
    servAddr.sin_addr.s_addr = inet_addr(serverip); 
    printf(" INFO: connect to server  addr:%s---port:%d\r\n",serverip,port);

    iRet = connect(fd,(struct sockaddr*)&servAddr,sizeof(servAddr));
    if (iRet<0){
        printf("ERR:  Can not connect to server %s---port:%d, error: %s(errno: %d)\r\n ",serverip,port,strerror(errno),errno);
    }

    return  0 ;
}

int CClient::sendData()
{
    static int flag = 0;
	
    while(!exitservThrd)
    {

        //send(m_Socketfd, "hello,i'm zzq", 14, 0);
	OSA_semWait(&CGlobalDate::Instance()->m_semHndl_socket_client,OSA_TIMEOUT_FOREVER);
	if(1 == exitservThrd)
	{
		CGlobalDate::Instance()->set_sendfile_status(0);
		break;
	}

	char filepath[128] = {0};
	memcpy(filepath, (char*)CGlobalDate::Instance()->send_filepath, strlen((char *)CGlobalDate::Instance()->send_filepath));
	sendfile(filepath);
	CGlobalDate::Instance()->set_sendfile_status(0);
    }
    return 0;
}

void seconds_sleep(unsigned seconds)
{
    struct timeval tv;

    int err;
    do{
        tv.tv_sec = seconds;
        tv.tv_usec = 0;
        err = select(0, NULL, NULL, NULL, &tv);
    }while(err<0 && errno==EINTR);
}

int CClient::ReConnectThread()
{
    while(!exitservThrd)
    {
        seconds_sleep(3);
    }

    return 0;
}

int CClient::sendfile(char *filename)
{
	int write_len = 0;
	int len = 0;
	FILE *fp;
	int filesize = 0;
	int sendsize = 0;
	int packet_flag;
	unsigned char checksum = 0;
	unsigned char usocket_send_buf[1024+256] = {0};
	unsigned char buf[1024+256] = {0};
	char namebak[128] = {0};
	char name[128] = {0};
	
	memcpy(namebak, filename, strlen(filename));
	
	if(NULL ==(fp = fopen(filename, "r")))
	{
		perror("fopen\r\n");
		return -1;
	}

	fseek(fp,0,SEEK_END);
	filesize = ftell(fp);
	fseek(fp,0,SEEK_SET);

	usocket_send_buf[0] = 0xEB;
	usocket_send_buf[1] = 0x51;
	usocket_send_buf[4] = 0x39;
	usocket_send_buf[5] = filesize&0xff;
	usocket_send_buf[6] = (filesize>>8)&0xff;
	usocket_send_buf[7] = (filesize>>16)&0xff;
	usocket_send_buf[8] = (filesize>>24)&0xff;


	char *ptr;
	ptr = strtok(namebak, "/");
	while(ptr!=NULL)
	{
		memcpy(name, ptr, strlen(ptr));
		ptr = strtok(NULL, "/");
	}
	printf("%s, %d, name=%s\n",  __FILE__,__LINE__, name);
	len = strlen(name);
	usocket_send_buf[9] = -1;
	usocket_send_buf[2] = (len+8)&0xff;
	usocket_send_buf[3] = ((len+8)>>8)&0xff;
	usocket_send_buf[10] = len&0xff;
	usocket_send_buf[11] = (len>>8)&0xff;
	memcpy(usocket_send_buf+12,name, len);
	for(int m = 1; m<12+len;m++)
		checksum ^= usocket_send_buf[m];
	usocket_send_buf[12+len] = checksum;
	write_len = write(m_Socketfd,usocket_send_buf,len+13);
	
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

		write_len = write(m_Socketfd,usocket_send_buf,len+13);
#if 0
		for(int n = 0; n < write_len; n++)
			printf("%02x ", usocket_send_buf[n]);
		printf("\n");
#endif
		if(-1 == write_len)
		{
			perror("write fail");
		}

	}
	if(sendsize == filesize)
	{
		printf("sendfile %s success\n", filename);
	}
	else
	{
		printf("sendfile %s fail\n", filename);
	}
	
	fclose(fp);
}
