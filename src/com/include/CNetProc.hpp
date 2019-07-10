#ifndef __CNETPROC_H__
#define __CNETPROC_H__

#include "CPortInterface.hpp"

class CNetProc : public CPortInterface
{
public:
	CNetProc(int tmpport);
	~CNetProc();
	int copen();
	int cclose();
	int crecv(int fd, void *buf,int len);
	int csend(int fd, void *buf,int len);
	int caccept(struct sockaddr*connectAddr,socklen_t* lenthConnect);

private:
	int NetBind(int fd, int port);
	int NetListen(int fd,int maxnum);
	
 private:
 	int port;
	int m_Socketfd;
};

#endif
