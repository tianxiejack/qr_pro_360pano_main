/*
 * Gststreamercontrl.hpp
 *
 *  Created on: Dec 18, 2018
 *      Author: wj
 */

#ifndef GSTSTREAMERCONTRL_HPP_
#define GSTSTREAMERCONTRL_HPP_
#include"Gststream.hpp"

class GstreaemerContrl
{
public:
	enum{
	H265COMPRESS,
	H265RTP,
	QUE_CHID_COUNT
	};
	typedef void (* CallBackfun)(void *data,void *info);
	void create();
	
	Gstreamer gstreamer;
	static int sync422_ontime_video(int dtype, unsigned char *buf, int size);

	CallBackfun gstrecordfun;

	void registrecordfun(CallBackfun fun);
	static int syncdatafun(void *data);
	
	static GstreaemerContrl*getinstance();
	void gstputmat(cv::Mat src);
	void gstputmux(cv::Mat src,Privatedata* privatedata);

	
	void gstputmat(char *buf,int size);
private:
	
	GstreaemerContrl();
	~GstreaemerContrl();
	static GstreaemerContrl* instance;
	

};


#endif /* GSTSTREAMERCONTRL_HPP_ */
