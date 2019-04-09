#include"Gststreamercontrl.hpp"
#include "StlGlDefines.h"
#include"videorecord.hpp"
#include "screenCapture.h"

static char TARGET_IP[32] = "192.168.1.26";
#if 0
GstCapture_data gstCapture_data[5];
#endif
RecordHandle * record_handle[5];
#define PORT_RTP 16000
static char strFormat[16] = "BGR";//"YUY2"//"GRAY8"
#define SAVEDIR "/home/nvidia/calib"

static int g_gst_recrod=true;
static FILE *g_gst_wrfp=NULL;
static int g_gst_wrPkt=0;

GstreaemerContrl* GstreaemerContrl::instance=NULL;
GstreaemerContrl::GstreaemerContrl():gstrecordfun(NULL)
{

}

GstreaemerContrl::~GstreaemerContrl()
{

}
int GstreaemerContrl::syncdatafun(void *data)
{
	Privatedata *pridata=(Privatedata *)data;
	SyncDate syncdata;
	//printf("%s\n",__func__);
	syncdata.gyroX=pridata->gyrox;
	syncdata.gyroY=pridata->gyroy;
	syncdata.gyroZ=pridata->gyroz;
	VideoRecord::getinstance()->putsync(&syncdata);


}

int GstreaemerContrl::sync422_ontime_video(int dtype, unsigned char *buf, int size)
{
	//printf("%s dtype=%d \n",__func__,dtype);
	
	if(instance!=NULL&&instance->gstrecordfun!=NULL)
		instance->gstrecordfun(buf,&size);
	
	/*
	if(g_gst_recrod)
	{
		if(g_gst_wrfp == NULL)
		{

			
			char filename[64];
			struct tm tm_set;
			struct timeval tv_set;
			struct timezone tz_set;
			time_t tt_set=0;
			gettimeofday(&tv_set, &tz_set);
			tt_set = tv_set.tv_sec;
			memcpy(&tm_set, localtime(&tt_set), sizeof(struct tm));
			sprintf(filename, "%s/local_%04d%02d%02d-%02d%02d%02d.avi", 
				SAVEDIR, 
				tm_set.tm_year+1900, tm_set.tm_mon+1, tm_set.tm_mday,
				tm_set.tm_hour, tm_set.tm_min,tm_set.tm_sec);
			g_gst_wrfp = fopen(filename,"wb");
			g_gst_wrPkt = 0;
			printf(" open local file %s\n", filename);
		}

		if(g_gst_wrfp != NULL)
		{
			fwrite(buf, size, 1, g_gst_wrfp);
			g_gst_wrPkt++;
			if((g_gst_wrPkt % 50) == 0)
			{
				fflush(g_gst_wrfp);
			}
		}
	}
	else
	{
		if(g_gst_wrfp != NULL)
		{
			fclose(g_gst_wrfp);
			g_gst_wrfp = NULL;
			printf(" close local file\n");
		}
	}
	*/
	

}


void GstreaemerContrl::registrecordfun(CallBackfun fun)
{
	gstrecordfun=fun;

}

void GstreaemerContrl::create()
{

#if 0
	memset(gstCapture_data, 0, sizeof(gstCapture_data));
	gstCapture_data[H265COMPRESS].width = TV_WIDTH;
	gstCapture_data[H265COMPRESS].height = TV_HEIGHT;
	gstCapture_data[H265COMPRESS].framerate = 30;
	gstCapture_data[H265COMPRESS].bitrate = 1500000;
	gstCapture_data[H265COMPRESS].ip_port = PORT_RTP+TV_DEV_ID;
	gstCapture_data[H265COMPRESS].filp_method = FLIP_METHOD_VERTICAL_FLIP;
	gstCapture_data[H265COMPRESS].capture_src = APPSRC;
	//gstCapture_data[TV_DEV_ID].format = "YUY2";
	gstCapture_data[H265COMPRESS].format = strFormat;//"I420";
	gstCapture_data[H265COMPRESS].ip_addr =TARGET_IP;
	gstCapture_data[H265COMPRESS].sd_cb=sync422_ontime_video;
	for(int i=0;i<ENC_QP_PARAMS_COUNT;i++)
		gstCapture_data[H265COMPRESS].Q_PIB[i]=-1;
	//if(notifys!=NULL)
	//	gstCapture_data[TV_DEV_ID].notify = notifys[TV_DEV_ID];

	//record_handle[H265COMPRESS] = gstreamer.gstpipeadd(gstCapture_data[H265COMPRESS]);



	gstCapture_data[H265RTP].width = TV_WIDTH;
	gstCapture_data[H265RTP].height = TV_HEIGHT;
	gstCapture_data[H265RTP].framerate = 30;
	gstCapture_data[H265RTP].bitrate = 1500000;
	gstCapture_data[H265RTP].ip_port = PORT_RTP;
	gstCapture_data[H265RTP].filp_method = FLIP_METHOD_VERTICAL_FLIP;
	gstCapture_data[H265RTP].capture_src = APPSRC;
	//gstCapture_data[TV_DEV_ID].format = "YUY2";
	gstCapture_data[H265RTP].format = strFormat;//"I420";
	gstCapture_data[H265RTP].ip_addr =TARGET_IP;
	gstCapture_data[H265RTP].sd_cb=sync422_ontime_video;
	gstCapture_data[H265RTP].sy_cb=syncdatafun;
	for(int i=0;i<ENC_QP_PARAMS_COUNT;i++)
		gstCapture_data[H265RTP].Q_PIB[i]=-1;

	record_handle[H265RTP] = gstreamer.gstpipeadd(gstCapture_data[H265RTP]);
#endif	
	//gstreamer.create();
	gstreamer_mp4 = new SaveVideoByGST();
	gstreamer_mp4->init();
	gst_videnc_create();
	gst_videnc_bitrate(2);

}



void GstreaemerContrl::gstputmat(cv::Mat src)
{
#if 0
	gstreamer.gstCapturePushData(record_handle[H265RTP],(char *)src.data,src.cols*src.rows*src.channels());
#endif
}

void GstreaemerContrl::gstputmux(cv::Mat src,Privatedata *privatedata)
{
	//gstreamer.gstCapturePushDataMux(record_handle[H265RTP], src, privatedata);
	//gstreamer.gstCapturePushDataMux(record_handle[H265COMPRESS],(char *)src.data,src.cols*src.rows*src.channels(),privatedata);
	
	if(should_video())	
	{
		gstreamer_mp4->SaveAsMp4(&src);
		save_gyro_data(privatedata);
		set_data_cnt(get_data_cnt() + 1);
	}
	else
	{
		stop_video();
	}
}

void GstreaemerContrl::gstputmat(char* buf,int size)
{
#if 0
	gstreamer.gstCapturePushData(record_handle[H265COMPRESS],buf,size);
#endif
}

GstreaemerContrl*GstreaemerContrl::getinstance()
{
	if(instance==NULL)
		{
			instance=new GstreaemerContrl();
			printf("************%s***********************\n",__func__);
		}
	return instance;

}


int GstreaemerContrl::should_video()
{
	VideoRecord::getinstance()->heldrecord();
	
	if(VideoRecord::getinstance()->getrecordflag())
	{
		vedioing = 1;
		return 1;
	}
	else
	{
		return 0;
	}
		
	//VideoRecord::getinstance()->recordvideo();
}

int GstreaemerContrl::stop_video()
{
	if(vedioing == 1)
	{
		if(get_video_cnt()>0)
		{
			remove_last_video();
			set_video_cnt(0);
		}

		set_data_cnt(0);
		vedioing = 0;
	}
}

int GstreaemerContrl::save_gyro_data(Privatedata *privatedata)
{
	VideoRecord::getinstance()->mydata.write(privatedata->event, privatedata->gyrox, privatedata->gyroy, privatedata->gyroz);
}

void GstreaemerContrl::remove_last_video()
{
	char buf[sizeof(filename_bak) + 16]={0};
	sprintf(buf,"sudo rm %s",filename_bak);
	system(buf);


	VideoRecord::getinstance()->mydata.close();
	sprintf(buf,"%s",filename_gyro_bak);
	remove(buf);
}