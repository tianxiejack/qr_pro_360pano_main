/*
 * #
 *
 *  Created on: Dec 19, 2018
 *      Author: wj
 */

#ifndef VIDEORECORD_HPP_
#define VIDEORECORD_HPP_
#include "osa.h"
#include <queue>
#include "opencv2/core/core.hpp"
typedef struct {
	double	gyroX;
	double  	gyroY;
	double	gyroZ;
	int event;


}SyncDate;
using namespace std;

using namespace cv;

#define FILEXML (1)
#define MAX_LINE (100)
class MyTData
{
public :
  MyTData() :
    event(0), gyrox(0), gyroy(0),gyroz(0)
  {
  	memset(buf,0,strlen(buf));
  }
 
  explicit MyTData(int t,double x,double y,double z) :
    event(t), gyrox(x), gyroy(y),gyroz(z)
  {
  }
  int event;
  double gyrox;
  double gyroy;
  double gyroz;
  FILE *filefp=NULL;
  char buf[MAX_LINE];
   cv::FileStorage filestore;
  void open(char *name)
  	{
  		if(FILEXML)
  			{
  					if ((filefp=fopen(name,"w"))==NULL)			//\u6253\u5f00\u6307\u5b9a\u6587\u4ef6\uff0c\u5982\u679c\u6587\u4ef6\u4e0d\u5b58\u5728\u5219\u65b0\u5efa\u8be5\u6587\u4ef6
					{
						printf("Open Failed.\n");
						return;
					} 
  			}
		else
			{
				filestore=FileStorage(name, FileStorage::WRITE);
				filestore << "gyro" << "[";
			}
  	}

   void close()
  	{
  	if(FILEXML)
  		{
  			if(filefp!=NULL)
  				fclose(filefp);
			filefp=NULL;
  		}
	else
		{
		filestore << "]";
		filestore.release();
		}
  	}

 
  void write(int event,double gyrox,double gyroy,double gyroz)  //Write serialization for this class
  {
  if(FILEXML)
  	{
  		sprintf(buf,"event:%d_gyrox:%f_gyroy:%f_gyroz:%f",event,gyrox,gyroy,gyroz);
		if(filefp!=NULL)
  		fprintf(filefp,"%s\n",buf);
		fflush(filefp);
  	}
  else
    filestore << "{" << "event" << event << "gyrox" << gyrox << "gyroy" << gyroy << "gyroz" << gyroz << "}";
	// fs << "{"<< "time" << Xspeed  <<  "}";
  }
  
  void read(const FileNode& node)  //Read serialization for this class
  {

    event = (int)node["event"];
    gyrox = (double)node["gyrox"];
    gyroy = (double)node["gyroy"];
    gyroz = (double)node["gyroz"];
   
  }
};

typedef void (*Callbackvideorecord) ();
class VideoRecord{
	#define EVENTRECORD 2
	#define WEEKRECORD 7
	#define HOURSRECORD 24
	public:
		void create();
		static VideoRecord*getinstance();
		static void recordvideo(void *data,void* size);
		void putsync(void *data);
		void getsync(void *data);

		void settimerecordenable(int enable){timeenable=enable;};
		void seteventrecordenable(int enable){eventenable=enable;};

		int getrecordflag(){return timeenable||eventenable;};

		int getforceclose(){return forceclose_;};
		void setforceclose(int flag){forceclose_=flag;};

		int getforcecloseonece(){return forcecloseonece_;};
		
		void setforcecloseonece(int flag){forcecloseonece_=flag;};

		void setmovtimedelay(int enable){timerdelayenable=enable;};
		int getmovtimedelay(){return timerdelayenable;};

		void setdataheldrecord(int a[2][7][24])
			{
				 for(int i=0;i<EVENTRECORD;i++)
	 				for(int j=0;j<WEEKRECORD;j++)
						for(int k=0;k<HOURSRECORD;k++)
							recordpositionheld[i][j][k]=a[i][j][k];
				
			};
		void registermanagercall(Callbackvideorecord call)
					{
						callback=call;

					}
	private:
		void heldrecord();
	private:
		int timerdelayenable;
		int timeenable;
		int eventenable;
		int forceclose_;
		int forcecloseonece_;
		VideoRecord();
		~VideoRecord();
		char avihead[1000];
		int aviheadlen;
		int aviheadenable;
		static VideoRecord* instance;
		
		Callbackvideorecord callback;

		
		
		
		int recordpositionheld[EVENTRECORD][WEEKRECORD][HOURSRECORD];
		

		 FILE *videorecordfb;
		 MyTData mydata;
		 int tm_year;
		 int tm_mon;
		 int tm_mday;
		 int tm_hour;
		 int tm_min;
		 int tm_sec;
		 int g_gst_wrPkt;
		 queue<SyncDate> *_syncdata;



};



#endif /* VIDEORECORD_HPP_ */
