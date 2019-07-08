#include "Gststreamercontrl.hpp"
#include "videorecord.hpp"
#include "Queuebuffer.hpp"

extern int gst_videnc_create(void);
extern int gst_videnc_enable(int bEnable);
extern int gst_shotenc_record(void);

GstreaemerContrl* GstreaemerContrl::instance=NULL;

GstreaemerContrl::GstreaemerContrl()
{

}

GstreaemerContrl::~GstreaemerContrl()
{

}

void GstreaemerContrl::create()
{
	vedioing = 0;
	// HDv4l_cam::read_frame push data-gst enc-gst save mp4
	gstreamer_mp4 = new SaveVideoByGST();
	gstreamer_mp4->init();
	// Queue::DISPALYTORTP push data-gst enc-live555 rtsp
	gst_videnc_create();
}

void GstreaemerContrl::gstputmux(cv::Mat src,Privatedata *privatedata)
{
	if(should_video())	
	{
		vedioing = 1;
		gstreamer_mp4->SaveAsMp4(&src);
		save_gyro_data(privatedata);
		set_data_cnt(get_data_cnt() + 1);
	}
	else
	{
		stop_video();
	}

}

GstreaemerContrl*GstreaemerContrl::getinstance()
{
	if(instance==NULL)
		instance=new GstreaemerContrl();
	return instance;

}

int GstreaemerContrl::should_video()
{
	VideoRecord::getinstance()->heldrecord();
	
	if(VideoRecord::getinstance()->getrecordflag())
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

int GstreaemerContrl::stop_video()
{
	if(vedioing == 1)
	{
		if(get_video_cnt()>0)
		{
			remove_last_video();
			//gstreamer_mp4->SetEos();
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

void GstreaemerContrl::setLiveVideo(int bEnable)
{
	gst_videnc_enable(bEnable);

}

void GstreaemerContrl::setLivePhoto(void)
{
	gst_shotenc_record();

}


