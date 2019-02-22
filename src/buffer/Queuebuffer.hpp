/*
 * Queuebuffer.hpp
 *
 *  Created on: 2018年11月9日
 *      Author: wj
 */

#ifndef QUEUEBUFFER_HPP_
#define QUEUEBUFFER_HPP_
#include "osa_image_queue.h"
#include"osa.h"
#include"config.hpp"
#include"config.h"
#include"stdio.h"


class Queue 
{

	public:
	#define MAXCHANNEL (1)
	#define PROCESSQUEUENUM (4)
	enum
		{
			IMAGEPROCESS,
			MOVDETECT,
			TOPANOSTICH,
			FROMEPANOSTICH,
			DISPALYPROCESS,
			DISPALYTORTP,
			VIDEOLOADRTSP,
			MAXQUEUE,

		};


	public :
		void create();
		void *getfull(int index,int chid,int forever);
		void  putempty(int index,int chid ,void *info);
		int getfullcount(int index,int chid);


		void *getempty(int index,int chid ,int forever);
		void  putfull(int index,int chid ,void *info);

		void *getprefull(int index,int chid,void *curentif);
		void *getprefull(int index,int chid);


		
		OSA_BufHndl bufQue[MAXQUEUE][MAXCHANNEL];
		
		static Queue *getinstance();
	private:
		static Queue*instance;
		Queue(){};
		~Queue(){};



};



#endif /* QUEUEBUFFER_HPP_ */
