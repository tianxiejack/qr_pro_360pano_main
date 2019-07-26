/*
 * DetectAlg.hpp
 *
 *  Created on: 2018年11月13日
 *      Author: wj
 */

#ifndef DETECTALG_HPP_
#define DETECTALG_HPP_


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
#include"classifydetect.hpp"
#include "StlGlDefines.h"
#if USE_DETECTV2
#include"DetecterFactory.hpp"
#include "Detector.hpp"
#endif

using namespace cv;
using namespace ibgs;

#if USE_DETECTV2
typedef struct{
	vector<BoundingBox> detectbox_;
	double angle;
}detectbox_angle_t;

typedef struct
{
	std::vector<cv::Rect> mvtect;
	double angle;
}mvtect_angle_t;
#endif
		
typedef struct {
	bool bTrack;
	bool bMtd;
	bool bBlobDetect;
	int chId;
	int iTrackStat;
	
	//Mat frame;
}MAIN_ProcThrDetectObj_cxt;

typedef struct {
	MAIN_ProcThrDetectObj_cxt cxt[2];
	OSA_ThrHndl		thrHandleProc;
	OSA_SemHndl	procNotifySem;
	int pp;
	bool bFirst;
	volatile bool	exitProcThread;
	bool						initFlag;
	void 						*pParent;
}MAIN_ProcThrDetectObj;

class DetectAlg 
{
	

	public :
		void create();
#if USE_DETECTV2
		void initmtdparam(int texturewidth);
		void createV2();
		int JudgeLkFastV2(Mat src);
		void panomoveprocessV2();
		void panomoveprocessV2(Mat src, int chid);
		static  void detectcall(vector<BoundingBox>& trackbox,void *context,int chid);
		static  void trackcall(vector<BoundingBox>& trackbox,void *context,int chid);
#endif
		static DetectAlg *getinstance();
	public:
		void setmtdstat(int stat){movdetectenable = stat;}
		int getmtdstat(){return movdetectenable;}
		void detectprocess(Mat src,OSA_BufInfo* frameinfo);
		
		

		LKmove lkmove;
		double LKprocessangle[MOVELKBLOCKNUM];
		CMvDectInterface *m_pMovDetector;
		ClassifyDetect *classifydetect;
	

	private:
			/*******************detect******************/
		 IBGS *bgs;

		Mat panograysrc;
		Mat detedtgraysrc;
		Mat panoblock[MOVEBLOCKNUM];
		Mat panoblockdown;
#if USE_DETECTV2
		int mtdpositonnum;
		vector<double> angleposv2;
		vector<cv::Mat> panoblockv2;
	
		OSA_MutexHndl mulock;
		OSA_MutexHndl mulocktrack;
		Detector *detectornew;
		double mtd_curangle[MULTICPUPANONUM];
		vector<BoundingBox> trackbox_;
		detectbox_angle_t detectbox_angle;
#endif
		int movdetectenable;
		int newframe;
		void setnewframe(int flag){newframe=flag;};
		int getnewframe(){return newframe;};
		int mk_dir(char *dir);

		
		Mat Modelframe[MOVEBLOCKNUM][MODELINGNUM];
		void panomoveprocess();
		void getnumofpano360image(int startx,int endx,int *texturestart,int *textureend);
		static void NotifyFunc(void *context, int chId);
		static void NotifyFunclk(void *context, int chId);
		static void NotifyFunccd(void *context, int chId);
		void Multicpupanoprocess(Mat& src);
		void MulticpuLKpanoprocess(Mat& src);

		void registorfun();


		void equalize(Mat& src);

		void detectprocesstest(Mat src,OSA_BufInfo* frameinfo);
		int JudgeLk(Mat src);
		int JudgeLkFast(Mat src);
		int LkAngle[MOVELKBLOCKNUM];
		double LKangleoffset[MOVELKBLOCKNUM];

		//static DetectAlg *Pthis;
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


		std::vector<cv::Rect>	detectlk;


			double currentcapangle;
		double getcurrentcapangle(){return currentcapangle;};
		void setcurrentcapangle(double angle){currentcapangle=angle;};

		void Multicpufilepanoprocess(Mat& src);
		void detectprocesstest(Mat src);
		int JudgeLkFastNew(Mat src);
		
		MAIN_ProcThrDetectObj	mainProcThrdetectObj;
		
		static DetectAlg*instance;
		DetectAlg();
		~DetectAlg();

		//MAIN_ProcThrDetectObj	mainProcThrObj;
		void main_proc_func();
		int MAIN_threadCreate(void);
		int MAIN_threadDestroy(void);
		static void *mainProcTsk(void *context)
		{
			MAIN_ProcThrDetectObj  * pObj= (MAIN_ProcThrDetectObj*) context;
			if(pObj==NULL)
				{

				printf("error \n");

				}
			DetectAlg *ctxHdl = (DetectAlg *) pObj->pParent;
			ctxHdl->main_proc_func();
		
			return NULL;
		}
	private:
		static void updatemtdparam(long param);



};


#endif /* DETECTALG_HPP_ */
