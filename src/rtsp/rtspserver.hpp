/*
 * rtspserver.hpp
 *
 *  Created on: Dec 20, 2018
 *      Author: wj
 */

#ifndef RTSPSERVER_HPP_
#define RTSPSERVER_HPP_
#include"osa.h"
#include"osa_sem.h"
#include"osa_thr.h"
#include"osa_mutex.h"
#include"osa_buf.h"

class RtspServer
{
public:
	static RtspServer* getinstance();
	void create();
private:
	RtspServer();
	static void * runServer(void * server);
	~RtspServer();
	void createthread();
	static RtspServer *instance;
private:
	pthread_t thread1;
	pthread_t thread2;

};


#endif /* RTSPSERVER_HPP_ */
