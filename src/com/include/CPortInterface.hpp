#ifndef __CPORTINTERFACE_H__
#define __CPORTINTERFACE_H__

#include <netinet/in.h> 

class CPortInterface
{
public:
	virtual int copen()=0;
	virtual int  cclose()=0;
	virtual int crecv(int fd, void *buf,int len)=0;
	virtual int csend(int fd, void *buf,int len)=0;
	virtual int caccept(struct sockaddr*connectAddr,socklen_t* lenthConnect){return 0;};
};
#endif
