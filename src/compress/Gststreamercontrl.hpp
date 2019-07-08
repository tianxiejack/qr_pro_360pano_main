/*
 * Gststreamercontrl.hpp
 *
 *  Created on: Dec 18, 2018
 *      Author: wj
 */

#ifndef GSTSTREAMERCONTRL_HPP_
#define GSTSTREAMERCONTRL_HPP_
#if 0
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/gstclock.h>
#endif
#include "Gststream.hpp"
#include "ReSplic.h"

class GstreaemerContrl
{
public:
	void create();
	
	IFSaveVideo *gstreamer_mp4;
	static GstreaemerContrl*getinstance();
	void gstputmux(cv::Mat src,Privatedata* privatedata);
	void setLiveVideo(int bEnable);
	void setLivePhoto(void);

private:
	
	GstreaemerContrl();
	~GstreaemerContrl();
	int should_video();
	int stop_video();
	int save_gyro_data(Privatedata *privatedata);
	void remove_last_video();
	static GstreaemerContrl* instance;
	int vedioing = 0;
	

};


#endif /* GSTSTREAMERCONTRL_HPP_ */
