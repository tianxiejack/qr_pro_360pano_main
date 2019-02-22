/*
 * videoload.hpp
 *
 *  Created on: Dec 19, 2018
 *      Author: wj
 */

#ifndef VIDEOLOAD_HPP_
#define VIDEOLOAD_HPP_
#include <osa_thr.h>
#include"osa_sem.h"
#include"config.hpp"
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/gstinfo.h>
#include <gst/app/gstappsink.h>
using namespace std;
typedef struct {
	bool bTrack;
	bool bMtd;
	bool bBlobDetect;
	int chId;
	int iTrackStat;
	
	//Mat frame;
}MAIN_VideoLoadCaptureThrObj_cxt;

typedef struct {
	MAIN_VideoLoadCaptureThrObj_cxt cxt[2];
	OSA_ThrHndl		thrHandleProc;
	OSA_ThrHndl		thrHandleProcdouble;
	OSA_SemHndl	procNotifySem;
	int pp;
	bool bFirst;
	volatile bool	exitProcThread;
	bool						initFlag;
	void 						*pParent;
}MAIN_VideoLoadCaptureThrObj;

using namespace cv;
typedef void (* VideoCallBackfun)(void *data,void *info);
typedef struct{
	int event;
	double gyrox;
	double gyroy;
	double gyroz;
}VideoLoadData;
#define FILEXML (1)
#define MAX_LINE (100)
class MyTDataLoad
{
public :
  MyTDataLoad() :
    event(0), gyrox(0), gyroy(0),gyroz(0)
  {
  }
 
  explicit MyTDataLoad(int t,double x,double y,double z) :
    event(t), gyrox(x), gyroy(y),gyroz(z)
  {
  }
  int event;
  double gyrox;
  double gyroy;
  double gyroz;
  FileNodeIterator fni;
  FileNodeIterator fniEnd;
  FileNodeIterator current; 
 FILE *filefp=NULL;
  char buf[MAX_LINE];
   cv::FileStorage filestore;
  void open(const char *name)
  	{
  		//printf("%s l=%d\n",__func__,__LINE__);
  	if(FILEXML)
  		{
  				if ((filefp=fopen(name,"r"))==NULL)			//\u6253\u5f00\u6307\u5b9a\u6587\u4ef6\uff0c\u5982\u679c\u6587\u4ef6\u4e0d\u5b58\u5728\u5219\u65b0\u5efa\u8be5\u6587\u4ef6
					{
						printf("Open Failed.\n");
						return;
					} 
  			
  		}
	else
		{
			try
				{
					filestore=FileStorage(name, FileStorage::READ);
				}
			catch (Exception ex)
				{
					;
					//printf("%s l=%d\n",__func__,__LINE__);
				}
			//printf("%s l=%d\n",__func__,__LINE__);
			FileNode features = filestore["gyro"];
			//printf("%s l=%d\n",__func__,__LINE__);
			fni= features.begin();
	    	      fniEnd = features.end();
		      current=fni;
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
		filestore.release();
  	}

 

  
  VideoLoadData read()  //Read serialization for this class
  {
  	//if(current==)
  	VideoLoadData data;
  	if(FILEXML)
  		{	
  			if(filefp!=NULL)
  				{
  					if(!feof(filefp))
  						{
		  					fgets(buf,MAX_LINE,filefp);
							sscanf(buf,"event:%d_gyrox:%lf_gyroy:%lf_gyroz:%lf",&data.event,&data.gyrox,&data.gyroy,&data.gyroz);
  						}
					else
						fseek(filefp, 0, SEEK_SET);
  				}
  		}
	else
		{
	
	data.event=(int)(*current)["event"];
	data.gyrox=(double)(*current)["gyrox"];
	data.gyroy=(double)(*current)["gyroy"];
	data.gyroz=(double)(*current)["gyroz"];
	if(current==fniEnd)
		current=fni;
	current++;
		}
	return data;
	
  	
   
  }
};





class VideoLoad{

	public:
		
		void create();
		void destory();
		static VideoLoad*getinstance();
		void registerfun(VideoCallBackfun fun);
		void putvideoname(char *name){memcpy(videoname,name,60);};
		char * getvideoname(){return videoname;};

		void playvideo();
	private:
		VideoLoad();
		~VideoLoad();
		void initgstreamer();
		void uninitgstreamer();
		Mat getgstframebegin();
		void getgstframeend();
		static VideoLoad* instance;
		VideoCallBackfun callfun;
		char videoname[60];
		MyTDataLoad mydata;
		Mat record;

		OSA_SemHndl  loadsem;

	public:
		string readname;
		string readavi;
		string readdir;
		int readnewfile;
	private :
		GstElement *pipeline, *sink;
	      GstElement *app_src;
	      gint width, height;

	      GstSample *sample = NULL;

	      gchar *descr;

	      GError *error = NULL;

	      GstStateChangeReturn ret;

	      gboolean res;

	      GstMapInfo map;

	      GMainLoop *loop;

	      GstBus *bus;
	      GstMessage *msg;

		GstBuffer *buffer;

        GstCaps *caps;

        GstStructure *s;
	public:
		void setreadnewfile(int flag){readnewfile=flag;};
		int getreadnewfile(){ return readnewfile;};
		void setreadname(string name){readname=name;};
		string getreadname(){return readname;};
		void setreadavi(string name){readavi=name;};
		string getreadavi(){return readavi;};
	public:
		void initvideo();
		static GstFlowReturn new_buffer(GstAppSink *appsink, gpointer user_data);
		static void appsink_eos(GstAppSink * appsink, gpointer user_data);
	public:
		void initgstreamerrtsp();
		void initgstreamerfile();
	private:
		 GstPipeline *gst_pipeline = NULL;
		 string launch_string;
		 int frame_count = 0;
		 int sleep_count = 0;
		 unsigned char *rtspdata=NULL;
		 unsigned char *rtspdatargb=NULL;
		 int rtspdatelen=0;
		
	public:
		//MyTDataLoad mydata;
		MAIN_VideoLoadCaptureThrObj	mainRecvThrObj;
		void main_Recv_func();
		int MAIN_threadRecvCreate(void);
		int MAIN_threadRecvDestroy(void);
		static void *mainRecvTsk(void *context)
		{
			MAIN_VideoLoadCaptureThrObj  * pObj= (MAIN_VideoLoadCaptureThrObj*) context;
			if(pObj==NULL)
				{

				printf("++++++++++++++++++++++++++\n");

				}
			VideoLoad *ctxHdl = (VideoLoad *) pObj->pParent;
			ctxHdl->main_Recv_func();
			
			return NULL;
		}


		MAIN_VideoLoadCaptureThrObj	mainRecvThrObjdata;
		void main_Recv_funcdata();
		int MAIN_threadRecvCreatedata(void);
		int MAIN_threadRecvDestroydata(void);
		static void *mainRecvTskdata(void *context)
		{
			MAIN_VideoLoadCaptureThrObj  * pObj= (MAIN_VideoLoadCaptureThrObj*) context;
			if(pObj==NULL)
				{

				printf("++++++++++++++++++++++++++\n");

				}
			VideoLoad *ctxHdl = (VideoLoad *) pObj->pParent;
			ctxHdl->main_Recv_funcdata();
			
			return NULL;
		}



};



#endif /* VIDEOLOAD_HPP_ */
