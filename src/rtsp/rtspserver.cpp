#include"rtspserver.hpp"
#include"media.h"
#include"rtspmediaserver.h"

RtspServer *RtspServer::instance=NULL;
RtspServer::RtspServer()
{



}



RtspServer::~RtspServer()
{



}


void RtspServer::createthread()
{
    pthread_attr_t attr1;
    pthread_attr_init(&attr1);
    pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);
    int rc1 = pthread_create(&thread1, &attr1,  runServer, NULL);
	
}
void RtspServer::create()
{
	createthread();


}

void * RtspServer::runServer(void * server)
{
	rtspmediaserver();
	//main1();
   // ((MESAI::LiveRTSPServer * ) server)->run();
  //  pthread_exit(NULL);
}
RtspServer* RtspServer::getinstance()
{
	if(instance==NULL)
		instance=new RtspServer;

	
	return instance;


}
