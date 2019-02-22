#include"videoload.hpp"
#include<stdio.h>
#include"Queuebuffer.hpp"

VideoLoad* VideoLoad::instance=NULL;
#define DIRRECTDIR  "/home/ubuntu/calib/video/"

#define OPENCVAVI (1)

#define RTSPURL (0)
VideoLoad::VideoLoad():callfun(NULL),readnewfile(0),readname("1.xml"),readavi("1.avi"),readdir(DIRRECTDIR)
{


}
VideoLoad::~VideoLoad()
{
	

}
void VideoLoad::initgstreamer()
{
	 descr =
             //g_strdup_printf ("filesrc location=%s ! qtdemux name=demux ! queue ! h264parse ! omxh264dec ! videoconvert ! appsink name=sink sync=false "
    		  g_strdup_printf ("filesrc  location=/home/ubuntu/calib/video/1.avi ! avidemux name=demux ! queue ! h264parse ! omxh264dec ! videoconvert  ! video/x-raw, format=(string)BGR !  appsink name=sink sync=false "
    		//  g_strdup_printf ("appsrc name=source ! avidemux name=demux ! queue ! h264parse ! omxh264dec ! videoconvert ! 'video/x-raw, format=RGB' ! appsink name=sink sync=false "
    	  );
//
      pipeline = gst_parse_launch (descr, &error);
       
      if (error != NULL) {

        g_print ("could not construct pipeline: %s\n", error->message);

        g_clear_error (&error);

        exit (-1);

      }

         /*get appsrc*/
    	//g_object_set (G_OBJECT (app_src),"stream-type",0,"format",GST_FORMAT_TIME,NULL);
        //    app_src = gst_bin_get_by_name (GST_BIN (pipeline), "source");
    	//g_signal_connect(app_src,"need-data",G_CALLBACK(cb_need_data),app_src);
    	//g_object_set (G_OBJECT (app_src),"stream-type",0,"format",GST_FORMAT_TIME,NULL);
        /* get sink */
            sink = gst_bin_get_by_name (GST_BIN (pipeline), "sink");

       /* set to PAUSED to make the first frame arrive in the sink */
           ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
           switch (ret) {

            case GST_STATE_CHANGE_FAILURE:{
               g_print ("failed to play the file---------\n");
               exit (-1);
       	}
             case GST_STATE_CHANGE_NO_PREROLL:{
               g_print ("live sources not supported yet\n");
               exit (-1);
    	}

            default:

             break;

      }


      /* This can block for up to 5 seconds. If your machine is really overloaded,

       * it might time out before the pipeline prerolled and we generate an error. A

       * better way is to run a mainloop and catch errors there. */

      ret = gst_element_get_state (pipeline, NULL, NULL, 1 * GST_SECOND);

      if (ret == GST_STATE_CHANGE_FAILURE) {

        g_print ("failed to play the file==========\n");

        exit (-1);

      }

     gst_element_send_event (pipeline,gst_event_new_step (GST_FORMAT_BUFFERS, 1, 1, TRUE, FALSE));

      clock_t start,finish;
      int count = 0;
      bool isEnd = false;
    double sum;

}

Mat VideoLoad::getgstframebegin()
{
	g_signal_emit_by_name (sink, "pull-preroll", &sample, NULL);
	Mat show;
	  if(gst_app_sink_is_eos((GstAppSink*)sink)){

             g_print("is end of stream\n");
		return show;
      }
     
      if (sample)
	  	{
		        caps = gst_sample_get_caps (sample);
		        if (!caps) {

		          g_print ("could not get snapshot format\n");

				return show;
		          //exit (-1);

		        }

		        s = gst_caps_get_structure (caps, 0);



		        /* we need to get the final caps on the buffer to get the size */
			/*
		        res = gst_structure_get_int (s, "width", &width);
		        res |= gst_structure_get_int (s, "height", &height);
		        if (!res) {
		             g_print ("could not get snapshot dimension\n");
		             return show;
		        }
				*/
		        //printf("the widht =%d height=%d\n",width,height);
		        /* create pixmap from buffer and save, gstreamer video buffers have a stride

		         * that is rounded up to the nearest multiple of 4 */

		        buffer = gst_sample_get_buffer (sample);

		        /* Mapping a buffer can fail (non-readable) */

		        if (gst_buffer_map (buffer, &map, GST_MAP_READ)) {

		    	//render using map_info.data
		    		printf("********************************8\n");
		           // cv::Mat frame = cv::Mat(1080*3/2, 1920, CV_8UC1, (char *)map.data, cv::Mat::AUTO_STEP);
		    	 //g_print ("sizeof(*map.data) =  %d\n",sizeof(*map.data));
		    	    //show = cv::Mat(1080, 1920, CV_8UC3, (char *)map.data, cv::Mat::AUTO_STEP);	
		    	       memcpy(record.data,(char *)map.data,1920*1080*3);
			       gst_buffer_unmap (buffer, &map);
			       gst_sample_unref (sample);
			       gst_element_send_event (pipeline,gst_event_new_step (GST_FORMAT_BUFFERS, 1, 1, TRUE, FALSE));
		          return record;

        		}
			gst_sample_unref (sample);
	

     }
   
      gst_element_send_event (pipeline,gst_event_new_step (GST_FORMAT_BUFFERS, 1, 1, TRUE, FALSE));
}
void  VideoLoad::getgstframeend()
{
	return ;
	  if (sample)
	  	{
			    gst_buffer_unmap (buffer, &map);
			   gst_sample_unref (sample);
	  	}
	   
      gst_element_send_event (pipeline,gst_event_new_step (GST_FORMAT_BUFFERS, 1, 1, TRUE, FALSE));

}
void VideoLoad::uninitgstreamer()
{
	gst_element_set_state (pipeline, GST_STATE_NULL);

      gst_object_unref (pipeline);

}
//static int eos = 0;
void VideoLoad::appsink_eos(GstAppSink * appsink, gpointer user_data)
{
    printf("app sink receive eos\n");
   // eos = 1;
//    g_main_loop_quit (hpipe->loop);
}
#define MAX(a ,b) ((a > b) ? a : b)
#define MIN(a ,b) ((a < b) ? a : b)
#define CLAP(a) (MAX((MIN(a, 0xff)), 0x00)) 
void NV212BGR( unsigned char *imgY, unsigned char *imgDst,int width, int height )
{
	int w, h;
	int shift = 14, offset = 8192;
	int C0 = 22987, C1 = -11698, C2 = -5636, C3 = 29049;
 
	int y1,y2,u1,v1;
 
	unsigned char * pY1 = imgY;
	unsigned char * pY2 = imgY+width;
	unsigned char * pUV = imgY+width*height;
	
	unsigned char * pD1 = imgDst;
	unsigned char * pD2 = imgDst+width*3;
 
	for ( h = 0; h < height; h +=2 )
	{
		for ( w = 0; w < width; w +=2 )
		{
			u1 = *pUV-128;
			pUV++;
			v1 = *pUV-128;
			pUV++;
 
			
			y1 = *pY1;
			y2 = *pY2;
 
			*pD1++ = CLAP(y1+((u1 * C3 + offset) >> shift));
			*pD1++ = CLAP(y1+((u1 * C2 + v1 * C1 + offset) >> shift));
			*pD1++ = CLAP(y1+((v1 * C0 + offset) >> shift));
			*pD2++ = CLAP(y2+((u1 * C3 + offset) >> shift));
			*pD2++ = CLAP(y2+((u1 * C2 + v1 * C1 + offset) >> shift));
			*pD2++ = CLAP(y2+((v1 * C0 + offset) >> shift));
 
			pY1++;
			pY2++;
			y1 = *pY1;
			y2 = *pY2;
 
			*pD1++ = CLAP(y1+((u1 * C3 + offset) >> shift));
			*pD1++ = CLAP(y1+((u1 * C2 + v1 * C1 + offset) >> shift));
			*pD1++ = CLAP(y1+((v1 * C0 + offset) >> shift));
			*pD2++ = CLAP(y2+((u1 * C3 + offset) >> shift));
			*pD2++ = CLAP(y2+((u1 * C2 + v1 * C1 + offset) >> shift));
			*pD2++ = CLAP(y2+((v1 * C0 + offset) >> shift));
			pY1++;
			pY2++;
 
		}
		pY1 += width;
		pY2 += width;
		pD1 += 3*width;
		pD2 += 3*width;
 
	}
}

GstFlowReturn VideoLoad::new_buffer(GstAppSink *appsink, gpointer user_data)
{
    GstSample *sample = NULL;
    cv::Mat show;
	cv::Mat showrgb;
    g_signal_emit_by_name (appsink, "pull-sample", &sample,NULL);
	VideoLoadData loaddata;
	OSA_BufInfo *bufinfo=NULL;
   // printf("****************************\n");
    if (sample)
    {
        GstBuffer *buffer = NULL;
        GstCaps   *caps   = NULL;
        GstMapInfo map    = {0};

        caps = gst_sample_get_caps (sample);
        if (!caps)
        {
            printf("could not get snapshot format\n");
        }
        gst_caps_get_structure (caps, 0);
        buffer = gst_sample_get_buffer (sample);
        gst_buffer_map (buffer, &map, GST_MAP_READ);
	  bufinfo=(OSA_BufInfo*)(Queue::getinstance()->getempty(Queue::VIDEOLOADRTSP,0,OSA_TIMEOUT_NONE));
	  if(bufinfo!=NULL)
	  	{
			
				memcpy(bufinfo->virtAddr,map.data,map.size);
				//printf("*****************VIDEOLOADRTSP++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
				Queue::getinstance()->putfull(Queue::VIDEOLOADRTSP,0,bufinfo);
	  	}
       
		
      //  printf("map.size = %lu\n", map.size);
        //cv::Mat frame_in(1920, 1080, CV_8UC3);
        static Uint32 pretime1=0;
	Uint32 currenttime=OSA_getCurTimeInMsec();
	//if(currenttime-pretime>50||currenttime-pretime<30)
		{
			;
			//OSA_printf("********lost %d ms %s timeoffset=%d ms**********\n", OSA_getCurTimeInMsec(), __func__,currenttime-pretime1);
		}
	
	pretime1=currenttime;
        
       // show = Mat(1080, 1920, CV_8UC2, instance->rtspdata);
	// showrgb = Mat(1080, 1920, CV_8UC3, instance->rtspdatargb);
      //  cvtColor(show,showrgb,cv::COLOR_YUV2BGR_NV12);
   

        gst_buffer_unmap(buffer, &map);

        gst_sample_unref (sample);
    }
    else
    {
        g_print ("could not make snapshot\n");
    }

    return GST_FLOW_OK;
}

void VideoLoad::initgstreamerfile()
{
    GMainLoop *main_loop;
    main_loop = g_main_loop_new (NULL, FALSE);
    ostringstream launch_stream;
    int w = 1920;
    int h = 1080;
    rtspdata=(unsigned char *)malloc(w*h*3);
    rtspdatargb=(unsigned char *)malloc(w*h*3);
    GstAppSinkCallbacks callbacks = {appsink_eos, NULL, new_buffer};

    launch_stream
	//<< "nvcamerasrc ! "
	//<< "video/x-raw(memory:NVMM), width="<< w <<", height="<< h <<", framerate=30/1 ! " 
    <<"filesrc  location=/home/ubuntu/calib/video/1.avi !"
   // << "rtspsrc location=rtsp://192.168.1.188:8554/test.264 latency=0 ! "
    //<< "filesrc 1.avi ! decodebin ! "
    << "avidemux name=demux !"
    <<" queue !"
    <<"h264parse !"
    <<"omxh264dec !"
    << "videoconvert !"
    <<"video/x-raw, format=(string)NV12 ! "
   // <<"video/x-raw, format=(string)RGB !"
  //  << "video/x-raw, format=NV12, width="<< w <<", height="<< h <<" ! "
  // <<"video/x-raw, format=NV12 !"
   // <<" videoconvert  ! "
  
   // <<"video/x-raw, format=(string)NV12 !"
    << "appsink name=mysink ";

/*
	 descr =
             //g_strdup_printf ("filesrc location=%s ! qtdemux name=demux ! queue ! h264parse ! omxh264dec ! videoconvert ! appsink name=sink sync=false "
    		  g_strdup_printf ("filesrc  location=/home/ubuntu/calib/video/1.avi ! avidemux name=demux ! queue ! h264parse ! omxh264dec ! videoconvert  ! video/x-raw, format=(string)BGR !  appsink name=sink sync=false "
    		//  g_strdup_printf ("appsrc name=source ! avidemux name=demux ! queue ! h264parse ! omxh264dec ! videoconvert ! 'video/x-raw, format=RGB' ! appsink name=sink sync=false "
    	  );
    	  */
//
   //   pipeline = gst_parse_launch (descr, &error);

    launch_string = launch_stream.str();

    g_print("Using launch string: %s\n", launch_string.c_str());

    GError *error = NULL;
    gst_pipeline  = (GstPipeline*) gst_parse_launch(launch_string.c_str(), &error);

    if (gst_pipeline == NULL) {
        g_print( "Failed to parse launch: %s\n", error->message);
        return ;
    }
    if(error) g_error_free(error);

    GstElement *appsink_ = gst_bin_get_by_name(GST_BIN(gst_pipeline), "mysink");
    gst_app_sink_set_callbacks (GST_APP_SINK(appsink_), &callbacks, NULL, NULL);

    gst_element_set_state((GstElement*)gst_pipeline, GST_STATE_PLAYING); 

  
    //sleep(90);
    //g_main_loop_run (main_loop);
/*
    gst_element_set_state((GstElement*)gst_pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(gst_pipeline));
    g_main_loop_unref(main_loop);
*/
    //g_print("going to exit, decode %d frames in %d seconds \n", frame_count, sleep_count);


}
void VideoLoad::initgstreamerrtsp()
{
	GMainLoop *main_loop;
    main_loop = g_main_loop_new (NULL, FALSE);
    ostringstream launch_stream;
    int w = 1920;
    int h = 1080;
    rtspdata=(unsigned char *)malloc(w*h*3);
    rtspdatargb=(unsigned char *)malloc(w*h*3);
    GstAppSinkCallbacks callbacks = {appsink_eos, NULL, new_buffer};

    launch_stream
	//<< "nvcamerasrc ! "
	//<< "video/x-raw(memory:NVMM), width="<< w <<", height="<< h <<", framerate=30/1 ! " 
    <<"rtspsrc location=rtsp://admin:abc12345@192.168.1.26:554/h264/ch0/main/av_stream latency=0 !"
   // << "rtspsrc location=rtsp://192.168.1.188:8554/test.264 latency=0 ! "
    //<< "filesrc 1.avi ! decodebin ! "
    << "decodebin ! "
    << "nvvidconv ! "
   // <<"video/x-raw, format=(string)RGB !"
    << "video/x-raw, format=NV12, width="<< w <<", height="<< h <<" ! "
  // <<"video/x-raw, format=NV12 !"
   // <<" videoconvert  ! "
  
   // <<"video/x-raw, format=(string)NV12 !"
    << "appsink name=mysink ";

    launch_string = launch_stream.str();

    g_print("Using launch string: %s\n", launch_string.c_str());

    GError *error = NULL;
    gst_pipeline  = (GstPipeline*) gst_parse_launch(launch_string.c_str(), &error);

    if (gst_pipeline == NULL) {
        g_print( "Failed to parse launch: %s\n", error->message);
        return ;
    }
    if(error) g_error_free(error);

    GstElement *appsink_ = gst_bin_get_by_name(GST_BIN(gst_pipeline), "mysink");
    gst_app_sink_set_callbacks (GST_APP_SINK(appsink_), &callbacks, NULL, NULL);

    gst_element_set_state((GstElement*)gst_pipeline, GST_STATE_PLAYING); 

  
    //sleep(90);
    //g_main_loop_run (main_loop);
/*
    gst_element_set_state((GstElement*)gst_pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(gst_pipeline));
    g_main_loop_unref(main_loop);
*/
    //g_print("going to exit, decode %d frames in %d seconds \n", frame_count, sleep_count);


}
void VideoLoad::create()
{
	record=Mat(1080,1920,CV_8UC3,cv::Scalar(0));
	MAIN_threadRecvCreate();
	MAIN_threadRecvCreatedata();
	OSA_semCreate(&loadsem,1,0);
	//initgstreamerrtsp();
	//initgstreamerfile();
	//initgstreamer();

}

void VideoLoad::destory()
{
	MAIN_threadRecvDestroy();
	MAIN_threadRecvDestroydata();
	OSA_semDelete(&loadsem);
}



void VideoLoad::registerfun(VideoCallBackfun fun)
{	
	callfun=fun;
}


void VideoLoad::playvideo()
{
	OSA_semSignal(&loadsem);

}
void VideoLoad::initvideo()
{



}
void VideoLoad::main_Recv_func()
{
	OSA_printf("%s: Main Proc Tsk Is Entering...\n",__func__);
	
	unsigned char *data=NULL;
	int angle=0;
	int status;
	VideoCapture videocapture;
	Mat fileframe;
	if(OPENCVAVI)
	videocapture.open(readavi);
	VideoLoadData loaddata;
	mydata.open(readname.c_str());
	string aviname=readdir+readavi;
	string  xmlname=readdir+readname;
	memset(&loaddata,0,sizeof(VideoLoadData));
	static double gyrodata=0;
	
	while(mainRecvThrObj.exitProcThread ==  false)
	{	
		
		int capangle=0;
		//OSA_waitMsecs(30);
		OSA_semWait(&loadsem,OSA_TIMEOUT_FOREVER);
		//printf("********%s bengin******\n",__func__);
		if(RTSPURL)
			return;
		if(getreadnewfile())
			{
				setreadnewfile(0);
				aviname=readdir+getreadavi();
				xmlname=readdir+getreadname();
				if(OPENCVAVI)
				videocapture.release();
				else
				uninitgstreamer();
				
				mydata.close();
				if(OPENCVAVI)
				videocapture.open(aviname);
				else
				initgstreamer();
				mydata.open(xmlname.c_str());

			}
		
		//initvideo();
		//if(getreadnewfile())
			{
				;

			}
		if(OPENCVAVI)
		status=videocapture.read(fileframe);
		else
			fileframe=getgstframebegin();
		if(!fileframe.empty())
			{
				
				//if()
				//resize(fileframe,fileframe,Size(config->getpanoprocesswidth(),config->getpanoprocessheight()),0,0,INTER_LINEAR);
				//printf("********2*file cap ok******\n");
				//fileframe.copyTo(img);

				
				//cvtColor(fileframe,img,CV_GRAY2BGR);
				//printf("********3*file cap ok******\n");

				loaddata=mydata.read();
				
				//printf("********loaddata=%f******\n",loaddata.gyroz);
			}
		else
			{
				//videocapture.set(CV_CAP_PROP_POS_FRAMES,0);
				if(OPENCVAVI)
					{
				videocapture.release();
				videocapture.open(aviname);
					}
				else
					{
						getgstframeend();
						uninitgstreamer();
						initgstreamer();

					}
				mydata.close();
				mydata.open(xmlname.c_str());
				if(OPENCVAVI)
				videocapture.read(fileframe);
				else
					{
						fileframe=getgstframebegin();
					}
				loaddata=mydata.read();

			}
		//printf("********%s end******\n",__func__);
		if(callfun!=NULL)
			{
				if(RTSPURL==0)
				callfun(fileframe.data,&loaddata);
			}
		if(OPENCVAVI)
			;
		else
		getgstframeend();
		//OSA_printf("%s: Main Proc Tsk Is Entering. capture ..\n",__func__);
		/*
		data=(unsigned char *)readfilepicture(&angle);
		//capangle=angle*1000;
		if(capturecallback!=NULL)
		capturecallback(capture.data,&angle);
		*/
	}

}
void VideoLoad::main_Recv_funcdata()
{
	
	OSA_printf("+++++++++++++%s: Main Proc Tsk Is Entering...++++++++++++++++++\n",__func__);
	unsigned char *data=NULL;
	int angle=0;
	int status;
	//memset(&loaddata,0,sizeof(VideoLoadData));
	static double gyrodata=0;
	OSA_BufInfo *bufinfo;
	VideoLoadData loaddata;
	while(mainRecvThrObjdata.exitProcThread ==  false)
		{
			//OSA_semWait(&loadsem,OSA_TIMEOUT_FOREVER);
			bufinfo=(OSA_BufInfo*)Queue::getinstance()->getfull(Queue::VIDEOLOADRTSP,0,OSA_TIMEOUT_FOREVER);
			unsigned char *data=(unsigned char *)bufinfo->virtAddr;
			//printf("(((((((((((((((((((((((((((((((((((((((((((((\n");
			 NV212BGR(data,instance->rtspdatargb,1920,1080);
			//rtspdatelen=map.size;
		      	if(instance->callfun!=NULL)
			{
				//if(RTSPURL)
				instance->callfun(instance->rtspdatargb,&loaddata);
			}


			Queue::getinstance()->putempty(Queue::VIDEOLOADRTSP, 0, bufinfo);
		}
}

int VideoLoad::MAIN_threadRecvCreate(void)
{
	int iRet = OSA_SOK;
	iRet = OSA_semCreate(&mainRecvThrObj.procNotifySem ,1,0) ;
	OSA_assert(iRet == OSA_SOK);


	mainRecvThrObj.exitProcThread = false;

	mainRecvThrObj.initFlag = true;

	mainRecvThrObj.pParent = (void*)this;

	iRet = OSA_thrCreate(&mainRecvThrObj.thrHandleProc, mainRecvTsk, 0, 0, &mainRecvThrObj);
	

	return iRet;
}

int VideoLoad::MAIN_threadRecvCreatedata(void)
{
	int iRet = OSA_SOK;
	iRet = OSA_semCreate(&mainRecvThrObjdata.procNotifySem ,1,0) ;
	OSA_assert(iRet == OSA_SOK);


	mainRecvThrObjdata.exitProcThread = false;

	mainRecvThrObjdata.initFlag = true;

	mainRecvThrObjdata.pParent = (void*)this;

	iRet = OSA_thrCreate(&mainRecvThrObjdata.thrHandleProc, mainRecvTskdata, 0, 0, &mainRecvThrObjdata);
	

	return iRet;
}
int VideoLoad::MAIN_threadRecvDestroy(void)
{
	int iRet = OSA_SOK;

	mainRecvThrObj.exitProcThread = true;
	OSA_semSignal(&mainRecvThrObj.procNotifySem);

	iRet = OSA_thrDelete(&mainRecvThrObj.thrHandleProc);

	mainRecvThrObj.initFlag = false;
	OSA_semDelete(&mainRecvThrObj.procNotifySem);

	return iRet;
}
int VideoLoad::MAIN_threadRecvDestroydata(void)
{
	int iRet = OSA_SOK;

	mainRecvThrObjdata.exitProcThread = true;
	OSA_semSignal(&mainRecvThrObjdata.procNotifySem);

	iRet = OSA_thrDelete(&mainRecvThrObjdata.thrHandleProc);

	mainRecvThrObjdata.initFlag = false;
	OSA_semDelete(&mainRecvThrObjdata.procNotifySem);

	return iRet;
}
VideoLoad *VideoLoad::getinstance()
{
	if(instance==NULL)
		instance=new VideoLoad();
	return instance;

}
