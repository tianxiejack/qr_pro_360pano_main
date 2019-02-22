#include"Stich.hpp"
#include"config.h"
#include"Homography.hpp"
#include"osa.h"
#include <osa_mutex.h>
#include"math.h"
#include "plantformcontrl.hpp"
#include "config.hpp"

unsigned int panowidth=0;
unsigned int panoheight=0;
double angle=0.0;


static unsigned int zeroimagpenum=0;
bool imgprocessenable =false;
int filestoreflag=0;
int panoflag=0;
int scanpanflag=0;
int modeling=0;

/*******************************************/
Mat PANO[PANODETECTNUM];
unsigned int panonum=0;
OSA_MutexHndl disLock[MULTICPUPANONUM];
std::vector<cv::Rect> mvtect[MULTICPUPANONUM];

Mat cylinderremapx;
Mat cylinderremapy;

Mat combitionmat;
int fullflame=0;
#define ODD (0)
#define EVEN (1)
#define FULL (2)

int orcezeroprocess=0;
double ptzzeroangle=0.0;
double ptzzerotitleangle=0.0;

double mvprocessangle[MULTICPUPANONUM];
int stichreset=0;

int modelnum[MULTICPUPANONUM];


void setmodelnum(int chid)
{
	modelnum[chid]++;

}
int getmodelnum(int chid)
{
	return modelnum[chid];
}

void setmodeling(int mod){modeling=mod;};
int getmodeling(){return modeling;};
void setstichreset(int flag)
{
	stichreset=flag;

}

int getstichreset()
{

	return stichreset;
}

void setptzzeroangle(double angle)
{
	ptzzeroangle=angle;

}

double getptzzeroangle()
{

	return ptzzeroangle;
}


void setptzzerotitleangle(double angle)
{
	ptzzerotitleangle=angle;

}

double getptzzerotitleangle()
{

	return ptzzerotitleangle;
}

void setmvprocessangle(double angle,int chid)
{
	mvprocessangle[chid]=angle;

}

double getmvprocessangle(int chid)
{
	return mvprocessangle[chid];

}
void deinterlanceinit()
{

	combitionmat=Mat(FIRHEIGHT,FIRWIDTH,CV_8UC2);

}

void cylinderremapinit()
{
      cylinderremapx.create(Size(Config::getinstance()->getpanoprocesswidth(),Config::getinstance()->getpanoprocessheight()), CV_32FC1 );
      cylinderremapy.create(Size(Config::getinstance()->getpanoprocesswidth(),Config::getinstance()->getpanoprocessheight()), CV_32FC1 );



	double R=Config::getinstance()->getcamfx();

    	int width =Config::getinstance()->getpanoprocesswidth(), height = Config::getinstance()->getpanoprocessheight();
	double x, y;
	int drcpoint;
	double fovAngle=2*atan(width/(2.0*R));
	int cylinderw=fovAngle*Config::getinstance()->getcamfx();
	printf("^^^^^^^^^^^^^^^^cylinderw=%d^^^^^^^^^^^^^^\n",cylinderw);
	for (int hnum = 0; hnum < height;hnum++)
	{
		for (int wnum = Config::getinstance()->getpanoprocessshift(); wnum < width;wnum++)
		{
			double k = R / sqrt(R*R + (wnum - width / 2)*(wnum - width / 2));
			//x = (wnum - width / 2) / k + width / 2;
			#if 1
			x= width/2.0 + R * tan((wnum-R * fovAngle/2)/R) ;
			y=hnum;
			#elif 0
			x=width/2+R*tan((wnum-width/2)/R);
			y=hnum;
			
			#else
			//x==  width/2.0 - R  * tan((R  * fovAngle/2 - wnum)/R ) + 0.5;
			x=width/2+R*tan((wnum-width/2)/R);
			y = (hnum - height / 2) / k + height / 2;
			#endif
			cylinderremapx.at<float>(hnum,wnum-Config::getinstance()->getpanoprocessshift())= static_cast<float>(x);
			cylinderremapy.at<float>(hnum,wnum-Config::getinstance()->getpanoprocessshift())= static_cast<float>(y);
		
		}
	}

}
void stichinit()
{
	for(int i=0;i<MULTICPUPANONUM;i++)
	OSA_mutexCreate(&disLock[i]);
	memset(mvprocessangle,0,sizeof(mvprocessangle));
	memset(modelnum,0,sizeof(modelnum));
	
	cylinderremapinit();
	deinterlanceinit();

}
void setpanomap(Mat pano[])
{
	for(int i=0;i<PANODETECTNUM;i++)
		{
			PANO[i]=pano[i];

		}

}


void setscanpanflag(int flag)
{
	scanpanflag=flag;

}

void setforcezeroprocess(int flag)
{
	orcezeroprocess=flag;
}
int getorcezeroprocess()
{
	return orcezeroprocess;

}
int  getscanpanflag()
{
	return scanpanflag;

}


void setpanoflagenable(int flag)
{
	panoflag=flag;

}
int  getpanoflagenable()
{
	return panoflag;

}

void setfilestoreenable(int flag)
{
	filestoreflag=flag;

}
int  getfilestoreenable()
{
	return filestoreflag;

}

Mat getpanomap()
{
	return PANO[panonum];

}
Mat getpanolastmap()
{
	return PANO[(panonum-1+PANODETECTNUM)%PANODETECTNUM];

}


void setpanonum(unsigned int num)
{
	panonum=num;
}

unsigned int getpanonum()
{
	return panonum;

}

void setnextnum()
{
	panonum=(panonum+1)%PANODETECTNUM;
}

/*******************************************/
void setfusionenalge(bool enable)
{
	imgprocessenable=enable;
}
bool getfusionenable()
{
	return imgprocessenable;
}
void setpanoparam(unsigned int w,unsigned int h)
{
	panowidth=w;
	panoheight=h;

}

void getpanoparam(unsigned int * w,unsigned int * h)
{
	*w=panowidth;
	*h=panoheight;
}

int getpanooffet(double angle)
{
	int xoffset=PANOSCALE*angle*panowidth/360;
	return xoffset;

}
double offet2angle(int  offsetx)
{
	//int xoffset=PANOSCALE*angle*panowidth/360;
	angle+=offsetx*360.0/(panowidth*PANOSCALE);
	if(angle>360)
		angle-=360;
	return angle;

}
double offet2anglepano(int  offsetx)
{
	//int xoffset=PANOSCALE*angle*panowidth/360;
	double angle=0;
	angle =offsetx*360.0/(panowidth*PANOSCALE);
	if(angle>360)
		angle-=360;
	return angle;

}
double offet2anglerelative2inter(int  offsetx)
{
	int halgoffset=offsetx/2;
	//double angle;
	angle+=2*atan2(1.0*halgoffset,Config::getinstance()->getcamfx())*180/3.141592653;
	if(angle>360)
		angle-=360;
	//int xoffset=PANOSCALE*angle*panowidth/360;
	//angle+=offsetx*360.0/(panowidth*PANOSCALE);
	return angle;

}

double offet2anglerelative2(int  offsetx)
{
	int halgoffset=offsetx/2;
	double angle;
	angle=2*atan2(1.0*halgoffset,Config::getinstance()->getcamfx())*180/3.141592653;
	//int xoffset=PANOSCALE*angle*panowidth/360;
	//angle+=offsetx*360.0/(panowidth*PANOSCALE);
	return angle;

}

double offet2anglerelative(int  offsetx)
{
	//int xoffset=PANOSCALE*angle*panowidth/360;
	double angle;
	angle=offsetx*360.0/(panowidth*PANOSCALE);
	return angle;

}

void cylinder(Mat& src,Mat & dst,double R,int shift)
{


#if 0
	int width = src.cols, height = src.rows;
	double x, y;
	//double R = width / 2;
	int drcpoint;
	double fovAngle=2*atan(width/(2.0*R));
	for (int hnum = 0; hnum < height;hnum++)
	{
		for (int wnum = shift; wnum < width;wnum++)
		{
			double k = R / sqrt(R*R + (wnum - width / 2)*(wnum - width / 2));
			//x = (wnum - width / 2) / k + width / 2;
			#if 0
			x= width/2.0 + R * tan((wnum-R * fovAngle/2)/R) ;
			y=hnum;
			#else
			//x==  width/2.0 - R  * tan((R  * fovAngle/2 - wnum)/R ) + 0.5;
			x=width/2+R*tan((wnum-width/2)/R);
			y = (hnum - height / 2) / k + height / 2;
			#endif
			
			if (0 < x && x < width && 0 < y &&y < height)
			{
				dst.at<Vec3b>(hnum, wnum-shift) = src.at<Vec3b>(int(y), int(x));
			}
		}
	}
#else
	remap(src, dst, cylinderremapx, cylinderremapy, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0) );

#endif

}
void histequision(Mat& src)
{
	Mat image_eq;
	image_eq.create(src.rows, src.cols, CV_8UC3); //separate channels, equalize histograms and them merge them
	vector<Mat> channels, channels_eq; 
	split(src, channels); 
	Mat eq; 
	for(int i = 0; i < channels.size(); i++) 
	{ //Mat eq; //eq.release(); 
		equalizeHist(channels[i], eq); 
		channels_eq.push_back(eq); 
	} 
	merge(channels_eq, image_eq);
	imshow("image_eq",image_eq);
	waitKey(1);


}



void FusionSeam(Mat& src,Mat & dst,int seampostion)
{
	double processWidth = src.cols - seampostion;
	if(seampostion<src.cols/2)
		{
			//printf("1FusionSeam w=%f  seampostion=%d\n",processWidth,seampostion);
			processWidth=seampostion;
			if(processWidth<100)
				processWidth=100;
		}
	else
		{
		if(Config::getinstance()->getangleinterval()>10)
			{
				if(processWidth>350)
				processWidth=350;
			}
		else
			{
				if(processWidth>50)
				processWidth=50;
			}

		}
	
	int rows = src.rows;
	int cols = src.cols; 
	double alpha = 1;

	//printf("2FusionSeam w=%f  seampostion=%d\n",processWidth,seampostion);
	for (int i = 0; i < rows; i++)
	{
		uchar* p  = src.ptr<uchar>(i);  
		uchar* t  = dst.ptr<uchar>(i);
		uchar* d = dst.ptr<uchar>(i);
		for (int j = 0; j < processWidth; j++)
		{
			if (p[(j+seampostion)* 3] == 0 && p[(j+seampostion) * 3 + 1] == 0 && p[(j+seampostion) * 3 + 2] == 0)
			{
				alpha = 0;
			}
			else
			alpha =(processWidth- j)*1.0/ processWidth;
			
			d[j * 3] = p[(j+seampostion) * 3] * alpha + t[j * 3] * (1 - alpha);
			d[j * 3 + 1] = p[(j+seampostion) * 3 + 1] * alpha + t[j * 3 + 1] * (1 - alpha);
			d[j * 3 + 2] = p[(j+seampostion)* 3 + 2] * alpha + t[j * 3 + 2] * (1 - alpha);
		}
	}

	

}

int  getPanoOffset(cv::Mat & src,cv::Mat & dst,int *xoffset ,int* yoffset)
{
	//return getPano360Offset( src, dst,xoffset ,yoffset);
	return getPano360OffsetT( src, dst,xoffset ,yoffset);
}


int  getfeaturePanoOffset(cv::Mat & src,cv::Mat & dst,int *xoffset ,int* yoffset)
{



	
	return 0;
}

void setmvdetect(std::vector<cv::Rect> &mv,int chid)
{
	OSA_mutexLock(&disLock[chid]);
	mvtect[chid].clear();
	for(int i=0;i<mv.size();i++)
		mvtect[chid].push_back(mv[i]);
	OSA_mutexUnlock(&disLock[chid]);
}
void  getmvdetect(std::vector<cv::Rect> &mv,int chid)
{
	OSA_mutexLock(&disLock[chid]);
	mv.clear();
	for(int i=0;i<mvtect[chid].size();i++)
		mv.push_back(mvtect[chid][i]);
	OSA_mutexUnlock(&disLock[chid]);
	
}
void mvdetectup(std::vector<cv::Rect> &mv)
{
	if(MOVDETECTDOWENABLE)
		{
			for(int i=0;i<mv.size();i++)
				{
					mv[i].x=mv[i].x*Config::getinstance()->getmvdownup();
					mv[i].y=mv[i].y*Config::getinstance()->getmvdownup();
					mv[i].width=mv[i].width*Config::getinstance()->getmvdownup();
					mv[i].height=mv[i].height*Config::getinstance()->getmvdownup();

				}
		}


}
void Multipotionto360(std::vector<cv::Rect> &mv,int chid)
{
	int size=mv.size();
	if(size==0)
		return;
	
	double anglepos=getmvprocessangle(chid);
	int offset=getpanooffet(anglepos);
	for(int i=0;i<mv.size();i++)
		{
			//mv[i].x=chid*MOVDETECTSRCWIDTH+mv[i].x;
			mv[i].x=offset+mv[i].x;
			//mv.push_back();
			//cout<<"***mv360*****"<<mv[i]<<"   "<<anglepos<<endl;
			

		}

}

void mvclassification(std::vector<cv::Rect> &mv,std::vector<cv::Rect> &mv180,std::vector<cv::Rect> &mv360)
{
	int size=mv.size();
	if(size==0)
		return;
	for(int i=0;i<mv.size();i++)
		{
			if(mv[i].x<panowidth/2)
				mv180.push_back(mv[i]);
			else
				{
					mv[i].x-=panowidth/2;
					mv360.push_back(mv[i]);

				}

		}

}

bool zerocalibration(Mat src,Mat dst,double *angleoffset,int *xoffset)
{
	bool ret=false;
	int status=0;
	int xoffsetfeat=0;
	int yoffsetfeat=0;
	char dstbuf[50];
	memset(dstbuf,0,sizeof(char)*50);
	sprintf(dstbuf,"zero_%d.jpg",zeroimagpenum);
	OSA_printf("zerocalibration begin\n");
	//imwrite("src.jpg",src);
	//imwrite(dstbuf,dst);
	
	status=getPanoOffset(src,dst,&xoffsetfeat,&yoffsetfeat);
	if(status==-1)
		return ret;
	*xoffset=xoffsetfeat;
	*angleoffset=offet2anglerelative2(xoffsetfeat);
	
	OSA_printf("the zeroimagpenum=%d oofset=%d  angle=%f\n",zeroimagpenum,xoffsetfeat,*angleoffset);
	zeroimagpenum++;
	return true;
}

void Matcpy(Mat src,Mat dst,int offset)
{
	int width = src.cols, height = src.rows;
	double x, y;
	//double R = width / 2;
	int drcpoint;

	for (int hnum = 0; hnum < height;hnum++)
	{
		for (int wnum = 0; wnum < width;wnum++)
		{	
			if(wnum+offset<width)
				dst.at<Vec3b>(hnum, wnum) = src.at<Vec3b>(int(hnum), int(wnum+offset));
			
		}
	}


}

void Matblack(Mat src,int offset)
{
	int width = src.cols, height = src.rows;
	double x, y;
	//double R = width / 2;
	int drcpoint;

	for (int hnum = 0; hnum < height;hnum++)
	{
		for (int wnum = offset; wnum < width;wnum++)
		{	
			
			src.at<Vec3b>(int(hnum), int(wnum))=0;
			
		}
	}


}



int  combition(Mat src,int flag)
{
	int status =0;
	int width=src.cols;
	int height=src.rows;

	unsigned char * srcdata=src.data;
	unsigned char * dstdata=combitionmat.data;
	if(flag==ODD)
		{
			fullflame=1;

		}
	else if(flag==EVEN&&fullflame==1)
		{
			fullflame=2;
			status=1;

		}
	for (int i = 0; i < height;i++)
	{
		memcpy(dstdata+(i+fullflame-1)*width*2*2,srcdata+i*width*2,width*2);
	}
	

	return status;

}

Mat getcombition()
{
	
	return combitionmat;
}

