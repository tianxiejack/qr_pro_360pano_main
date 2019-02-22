/*
 * StichAlg.hpp
 *
 *  Created on: 2018年11月12日
 *      Author: wj
 */

#ifndef STICHALG_HPP_
#define STICHALG_HPP_
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

using namespace cv;

typedef struct {
	bool bTrack;
	bool bMtd;
	bool bBlobDetect;
	int chId;
	int iTrackStat;
	
	//Mat frame;
}MAIN_ProcThrStichObj_cxt;

typedef struct {
	MAIN_ProcThrStichObj_cxt cxt[2];
	OSA_ThrHndl		thrHandleProc;
	OSA_SemHndl	procNotifySem;
	int pp;
	bool bFirst;
	volatile bool	exitProcThread;
	bool						initFlag;
	void 						*pParent;
}MAIN_ProcThrStichObj;
#define MAXFUSON (40)
#define FIXDIS (500)
class StichAlg 
{

	
	public :
		void create();

		int zeroflameupdate;
		void setzeroflameupdate(int flag){zeroflameupdate=flag;};
		int getzeroflameupdate(){return zeroflameupdate;};


		/********camrea********/
			double camerazeroossfet;
			void setcamerazeroossfet(double flag){camerazeroossfet=flag;};
			double getcamerazeroossfet(){return camerazeroossfet;};


			void setzerocalib(int flag){zerocalibflag=flag;};
			int getzerocalib(){return zerocalibflag;};
			void setzeroangle(double flag){zeroangle=flag;};
			double getzeroangle(){return zeroangle;};
			void setzeroflag(int flag){zeroflag=flag;};

			void setzerocalibing(int flag){zerocalibing=flag;};
			int getzerocalibing(){return zerocalibing;};

			void fusiontail(Mat src,int pos);
			void StichFusionSeam(Mat & dst,int seampostion);
			void CColorCorrect(Mat &src);
			void CColorCorrectfeed(Mat &src,Mat & dst,int seampostion);
		static StichAlg *getinstance();
	public:
			/*****************fusion*************************/
	
		int bl_width_;
		int bl_height_;
		 std::vector<Mat_<float> > gain_maps_;
		 double gamma;
		 Point3d gain_c;
		 std::vector<Point3d>	alpha;
	public:
		
		int Fusion[MAXFUSON];
		int fixfusondis[MAXFUSON];
		Mat Fusiontail[MAXFUSON];
		Mat Fusionhead[MAXFUSON];
		int  fusiontailpanopos[MAXFUSON];
		int  fusionheadpanopos[MAXFUSON];
		int getfusiontailpanopos(int chid){return fusiontailpanopos[chid];};
		Mat  getFusiontail(int chid){return Fusiontail[chid];};
	private:
		
		double currentangle;
		double getcurrentangle(){return currentangle;};
		void setcurrentangle(double angle){currentangle=angle;};


		
		int siglecircle;
		double sigleangle;
		void setsigleangle(double angle){sigleangle=angle;}
		double getsigleangle(){return sigleangle;};
		void singlefun();

		void singleinterupt();

		




		Mat currentflame;
		Mat getcurrentflame(){return currentflame;};
		void setcurrentflame(Mat flame){currentflame=flame;};



		Mat disflame;
		Mat getdisflame(){return disflame;};
		void setdisflame(Mat flame){disflame=flame;};


		Mat preframe;
		Mat getpreframe(){return preframe;};
		void setpreframe(Mat frame){preframe=frame;};

		double preangle;
		double getpreangle(){return preangle;};
		void setpreangle(double angle){preangle=angle;};


		unsigned int preflag;
		unsigned int  getpreframeflage(){return preflag;};
		void setpreframeflag(int flag){preflag=flag;};



		unsigned int Seampostion;
		unsigned int getSeamPos(){return Seampostion;};
		void setSeamPos(unsigned int sem){ Seampostion=sem;};


		Mat ProcessPreimage;

		int xoffsetfeat;
		int yoffsetfeat;

		void setpreprocessimage(Mat src){memcpy(ProcessPreimage.data,src.data,ProcessPreimage.cols*ProcessPreimage.rows*ProcessPreimage.channels());};
		Mat getpreprocessimage(){return ProcessPreimage;};


		

			/********frame********/
		Mat zeroflame;
		Mat getzeroflame(){return zeroflame;};
		void setzeroflame(Mat flame){memcpy(zeroflame.data,flame.data,flame.cols*flame.rows*flame.channels());};



				/********angle********/
		double gyroangle;
		void setgyroangle(double flag){gyroangle=flag;};
		double getgyroangle(){return gyroangle;};
		



		double zeroptzangle;
		double zeroptztiangle;


			/********zero********/

		int zerolostcout;
	      int zerolostflag;

		int zeroflag;
		int zerocalibflag;
		double zeroangle;
		int zeroprocessflag;
		int zerocaliboffset;
		int zerocalibing;
		int zerocalibrationstatus;

		int tailcut;

		void setzerocalibrationstatus(int flag){zerocalibrationstatus=flag;};
		int getzerocalibrationstatus(){return zerocalibrationstatus;};
		
		void setzerolostflag(int flag){zerolostflag=flag;};
		int getzerolostflag(){return zerolostflag;};

		void setzerolostcout(int count){zerolostcout=count;};
		void setzerolostcoutadd(){zerolostcout++;};
		int getzerolostcout(){return zerolostcout;};
		

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
		
		
		
		
		void setzeroprocessflag(int flag){zeroprocessflag=flag;};
		int getzeroprocessflag(){return zeroprocessflag;};

		void setzerocaliboffset(int offset){zerocaliboffset=offset;};
		int getzerocaliboffset(){return zerocaliboffset;};

		




		int getzeroflag(){return zeroflag;};
		void zeroprocess();
		int judgezero();


		void Panoprocess();

		void zerolostreset();

		void stichprocess();

		void Zeropreprocess();

		void zerolostjudge();

		void zeroreset();



		




	private:

		
		static StichAlg*instance;
		StichAlg();
		~StichAlg();

		MAIN_ProcThrStichObj	mainProcThrObj;
		void main_proc_func();
		int MAIN_threadCreate(void);
		int MAIN_threadDestroy(void);
		static void *mainProcTsk(void *context)
		{
			MAIN_ProcThrStichObj  * pObj= (MAIN_ProcThrStichObj*) context;
			if(pObj==NULL)
				{

				printf("error \n");

				}
			StichAlg *ctxHdl = (StichAlg *) pObj->pParent;
			ctxHdl->main_proc_func();
		
			return NULL;
		}



};


#endif /* STICHALG_HPP_ */
