/*
 * ImageProcess.hpp
 *
 *  Created on: 2018年9月29日
 *      Author: wj
 */

#ifndef IMAGEPROCESS_HPP_
#define IMAGEPROCESS_HPP_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include"osa.h"
#include"osa_sem.h"
#include"osa_thr.h"
#include"osa_mutex.h"
#include"osa_buf.h"
#include"config.h"
#include "mvdectInterface.hpp"
#include"lkdetect.hpp"
#include "IBGS.h"
using namespace cv;
using namespace ibgs;
//#define DS_CHAN_MAX         (4)
typedef struct
{
	double intervalsita;
	int validintervalsita;

}FrameINFO;

#define FRAMEFIFO 2
typedef struct _main_thr_obj_cxt{
	bool bTrack;
	bool bMtd;
	bool bBlobDetect;
	int chId;
	int iTrackStat;
	
	//Mat frame;
}MAIN_ProcThrObj_cxt;

typedef struct _main_thr_obj{
	MAIN_ProcThrObj_cxt cxt[2];
	OSA_ThrHndl		thrHandleProc;
	OSA_SemHndl	procNotifySem;
	int pp;
	bool bFirst;
	volatile bool	exitProcThread;
	bool						initFlag;
	void 						*pParent;
}MAIN_ProcThrObj;


class ImageProcess
{
public:
	ImageProcess();
	~ImageProcess();

	void Init();
	void Create();
	
	void unInit();
	void CaptureThreadProcess(Mat src,OSA_BufInfo* frameinfo);


public:
		MAIN_ProcThrObj	mainProcThrObj;
		MAIN_ProcThrObj	mainProcThrdetectObj;
		Mat processgray[FRAMEFIFO];
		Mat processtest;
		Mat processgraytemp[FRAMEFIFO];
		unsigned int currentcount;
		unsigned int pinpang;
		void setImagePinpang(unsigned int id){pinpang=id;};
		void setnextImagePinpang(){pinpang=(pinpang+1)%FRAMEFIFO;};
		unsigned int getImagePinpang(){return pinpang;};
		unsigned int getImagePrePinpang(){return (FRAMEFIFO+pinpang-1)%FRAMEFIFO;};

		void Panoprocess(Mat src,Mat dst);


		int Panorest();

		int detectenable(OSA_BufInfo* info);


		//////////////////ring buffer/////////////////////
		OSA_BufHndl m_bufQue[IMAGEQUEUE];
		OSA_BufHndl mcap_bufQue[IMAGEQUEUE];

		///////////////////FRAME/////////////////////////
		double AngleStich;
		Mat processtemp;
		Mat processtempgray;

		/////////////////////panoprocess////////////////////////
		int xoffsetfeat;
		int yoffsetfeat;
		double preangle;
		double currentangle;
		double getcurrentangle(){return currentangle;};
		void setcurrentangle(double angle){currentangle=angle;};

		double currentcapangle;
		double getcurrentcapangle(){return currentcapangle;};
		void setcurrentcapangle(double angle){currentcapangle=angle;};

		int stichenable();
		int stichenable(OSA_BufInfo* info);
		
		double getpreangle(){return preangle;};
		void setpreangle(double angle){preangle=angle;};
		void OptiSeam();
		Mat ProcessPreimage;

		void setpreprocessimage(Mat src){memcpy(ProcessPreimage.data,src.data,ProcessPreimage.cols*ProcessPreimage.rows*ProcessPreimage.channels());};
		Mat getpreprocessimage(){return ProcessPreimage;};
		
			/******************seaminit***********************/
		#define MAXSEAM 2
		Mat Seamframe[MAXSEAM];
		Mat fusionframe;
		unsigned int seamid;
		unsigned int SeamEable;
		unsigned int Seampostion;

		unsigned int preflag;
		Mat preframe;
		Mat getpreframe(){return preframe;};
		void setpreframe(Mat frame){preframe=frame;};
		unsigned int  getpreframeflage(){return preflag;};
		void setpreframeflag(int flag){preflag=flag;};

		Mat getCurrentFame(){return Seamframe[seamid%MAXSEAM];};
		Mat getPreFame(){return Seamframe[(seamid+1)%MAXSEAM];};

		unsigned int getcurrentFrameId(){return seamid;};
		void setcurrentFrameId(unsigned int id){ seamid=id;};
		void setNextFrameId(){seamid=(seamid+1)%MAXSEAM;};
		
		bool getSeamEnable(){return SeamEable;};
		void setSeamEnable(bool enable){SeamEable=enable;};
		
		unsigned int getSeamPos(){return Seampostion;};
		void setSeamPos(unsigned int sem){ Seampostion=sem;};

		void setwarndetect(int w,int h,int chid);


		/*******************detect******************/
		 IBGS *bgs;
		CMvDectInterface *m_pMovDetector;
		Mat panograysrc;
		Mat detedtgraysrc;
		Mat panoblock[MOVEBLOCKNUM];
		Mat panoblockdown;

		int newframe;
		void setnewframe(int flag){newframe=flag;};
		int getnewframe(){return newframe;};


		
		Mat Modelframe[MOVEBLOCKNUM][MODELINGNUM];
		void panomoveprocess();
		void getnumofpano360image(int startx,int endx,int *texturestart,int *textureend);
		static void NotifyFunc(void *context, int chId);
		static void NotifyFunclk(void *context, int chId);
		void Multicpupanoprocess(Mat& src);
		void MulticpuLKpanoprocess(Mat& src);


		void detectprocess(Mat src,OSA_BufInfo* frameinfo);

		void detectprocesstest(Mat src,OSA_BufInfo* frameinfo);
		int JudgeLk(Mat src);
		int JudgeLkFast(Mat src);
		int LkAngle[MOVELKBLOCKNUM];
		double LKangleoffset[MOVELKBLOCKNUM];
		double LKprocessangle[MOVELKBLOCKNUM];
		static ImageProcess *Pthis;
		std::vector<TRK_RECT_INFO>	detect_vect;
		int blocknum;
		int movblocknum;
		int movblocknumpre;
		double panoblockangle[MOVELKBLOCKNUM];

		
		
		Mat MvtestFRrame[2];
		Mat MvtestFRramegray;
		Mat LKRramegray;
		Mat LKRramegrayblackboard;
		Rect blackrect;
		unsigned int pp;
		VideoWriter videowriter[MULTICPUPANONUM];
		VideoCapture videocapture;

		double zeroptzangle;
		double zeroptztiangle;

		LKmove lkmove;
		std::vector<cv::Rect>	detectlk;
		/********angle********/
		double gyroangle;
		void setgyroangle(double flag){gyroangle=flag;};
		double getgyroangle(){return gyroangle;};
		

		/********camrea********/
		double camerazeroossfet;
		void setcamerazeroossfet(double flag){camerazeroossfet=flag;};
		double getcamerazeroossfet(){return camerazeroossfet;};


		/********zero********/
		int zeroflameupdate;
		int zeroflag;
		int zerocalibflag;
		double zeroangle;
		int zeroprocessflag;
		int zerocaliboffset;
		int zerocalibing;

		int tailcut;

		void settailcut(int cut){tailcut=cut;};
		int gettailcut(){return tailcut;};

		double calibrationzeroangle;
		void setcalibrationzeroangle(double angle){calibrationzeroangle=angle;};
		double getcalibrationzeroangle(){return calibrationzeroangle;};
		
		int zerodropflame;
		void setzerodropflame(int flag){zerodropflame=flag;};
		int getzerodropflame(){return zerodropflame;};

		int zerodroreset;
		void setzerodroreset(int flag){zerodroreset=flag;};
		int getzzerodroreset(){return zerodroreset;};
		
		
		void setzerocalibing(int flag){zerocalibing=flag;};
		int getzerocalibing(){return zerocalibing;};
		
		void setzeroprocessflag(int flag){zeroprocessflag=flag;};
		int getzeroprocessflag(){return zeroprocessflag;};

		void setzerocaliboffset(int offset){zerocaliboffset=offset;};
		int getzerocaliboffset(){return zerocaliboffset;};

		

		void setzeroflameupdate(int flag){zeroflameupdate=flag;};
		int getzeroflameupdate(){return zeroflameupdate;};

		void setzerocalib(int flag){zerocalibflag=flag;};
		int getzerocalib(){return zerocalibflag;};
		void setzeroangle(double flag){zeroangle=flag;};
		double getzeroangle(){return zeroangle;};
		void setzeroflag(int flag){zeroflag=flag;};
		int getzeroflag(){return zeroflag;};
		void zeroprocess();
		int judgezero();


		/********frame********/
		Mat zeroflame;
		Mat getzeroflame(){return zeroflame;};
		void setzeroflame(Mat flame){memcpy(zeroflame.data,flame.data,flame.cols*flame.rows*flame.channels());};

		Mat currentflame;
		Mat getcurrentflame(){return currentflame;};
		void setcurrentflame(Mat flame){currentflame=flame;};


		/********cpu  process********/
		void cpupanoprocess(Mat& src);
	
		
private:
	OSA_MutexHndl m_mutex;
//	unsigned char *m_grayMem[2];

	void main_proc_func();
	int MAIN_threadCreate(void);
	int MAIN_threadDestroy(void);
	static void *mainProcTsk(void *context)
	{
		MAIN_ProcThrObj  * pObj= (MAIN_ProcThrObj*) context;
		if(pObj==NULL)
			{

			printf("++++++++++++++++++++++++++\n");

			}
		ImageProcess *ctxHdl = (ImageProcess *) pObj->pParent;
		ctxHdl->main_proc_func();
		printf("****************************************************\n");
		return NULL;
	}

	void main_detect_func();
	int MAIN_detectthreadCreate(void);
	int MAIN_detectthreadDestroy(void);
	static void *maindetectTsk(void *context)
	{
		MAIN_ProcThrObj  * pObj= (MAIN_ProcThrObj*) context;
		if(pObj==NULL)
			{

			printf("++++++++++++++++++++++++++\n");

			}
		ImageProcess *ctxHdl = (ImageProcess *) pObj->pParent;
		ctxHdl->main_detect_func();
		printf("****************************************************\n");
		return NULL;
	}
		

};


#endif /* IMAGEPROCESS_HPP_ */
