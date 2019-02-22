#include "Detectfilter.hpp"
#include "config.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include"osa.h"
#include"osa_sem.h"
#include"osa_thr.h"
#include"osa_mutex.h"
#include"osa_buf.h"

using namespace cv;
using namespace std;
DetectFilter::DetectFilter():backgroundflag(0)
{
	

}
DetectFilter::~DetectFilter()
{
	

}
double  DetectFilter::Templatematch(const cv::Mat & src, const cv::Mat & dst) 
	{
		double exec_time = (double)getTickCount();
		int status=0;
             Mat tempsrc=src;
             Mat tempdst=dst;
		Mat result(tempdst.cols - tempsrc.cols + 1, tempdst.rows - tempsrc.rows + 1, CV_8UC1);
		
		matchTemplate(tempdst, tempsrc, result, CV_TM_CCOEFF_NORMED); 
		double minVal; double maxVal; Point minLoc; Point maxLoc;
		Point matchLoc; 
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat()); 
		//printf("%s   %f\n",__func__,maxVal);
	return maxVal; 
	}
void DetectFilter::process(cv::Mat src,std::vector<TRK_RECT_INFO> & target)
{



	if(Config::getinstance()->getpanocalibration()==0)
		{
			return;
		}
	std::vector<TRK_RECT_INFO> targetstemp;//=target;
	Rect tempsrcrect;
	Mat tempsrc;
	Rect matchrect;
	Mat match;
	double ratio=0.5;
	int centx=0;
	int centy=0;
	int centendx=0;
	int centendy=0;
	for(int i=0;i<target.size();i++)
		{
			tempsrcrect=target[i].targetRect;
			centx=tempsrcrect.x+tempsrcrect.width/2;
			centy=tempsrcrect.y+tempsrcrect.height/2;
			matchrect.x=max((int)(centx-(1+ratio)*tempsrcrect.width/2),0);
			matchrect.y=max((int)(centy-(1+ratio)*tempsrcrect.height/2),0);
			centendx=min((int)(centx+(1+ratio)*tempsrcrect.width/2),src.cols);
			centendy=min((int)(centy+(1+ratio)*tempsrcrect.height/2),src.rows);

			matchrect.width=centendx-matchrect.x;
			matchrect.height=centendy-matchrect.y;
			
			tempsrc=Mat(src,tempsrcrect);
			match=Mat(background,matchrect);
			if(Templatematch(tempsrc,match)<0.7)
				targetstemp.push_back(target[i]);

		}
	if(target.size()>0)
	target.swap(targetstemp);
	//target.swap()
}

void DetectFilter::create()
{


}
void DetectFilter::destroy()
{


}
void DetectFilter::backgroundupdate(cv::Mat src)
{
	if(backgroundflag==0)
		{
			src.copyTo(background);
		}
	backgroundflag=1;
}