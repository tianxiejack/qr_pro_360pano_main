#include"DetectAlg.hpp"
#include"Queuebuffer.hpp"
#include"Stich.hpp"
#include "plantformcontrl.hpp"
#include"StichAlg.hpp"
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include "CMessage.hpp"
#include"Status.hpp"
#include "Render.hpp"

DetectAlg*DetectAlg::instance=NULL;

#if USE_DETECTV2
void DetectAlg::initmtdparam(int texturewidth)
{
	mtdpositonnum = texturewidth / Config::getinstance()->getmvprocesswidth();
	if(texturewidth % Config::getinstance()->getmvprocesswidth())
		mtdpositonnum += 1;
	printf("%s,%d, mtdpositonnum=%d\n",__FILE__,__LINE__, mtdpositonnum);

	angleposv2.resize(mtdpositonnum);
	panoblockv2.resize(mtdpositonnum);

	Mat testmat;
	for(int i = 0; i<mtdpositonnum; i++)
	{
		angleposv2[i] = i * 360 * Config::getinstance()->getmvprocesswidth() /  texturewidth;
		if(PANOGRAYDETECT)
			panoblockv2[i]=Mat(Config::getinstance()->getmvprocessheight(),Config::getinstance()->getmvprocesswidth(),CV_8UC1,cv::Scalar(0));
		else
			panoblockv2[i]=Mat(Config::getinstance()->getmvprocessheight(),Config::getinstance()->getmvprocesswidth(),CV_8UC3,cv::Scalar(0));	
	}
}

void DetectAlg::createV2()
{
	mtdcfg_t mtdcfg_alg = Status::getinstance()->mtdcfg;
	std::vector<std::string> model;
	std::vector<cv::Size> modelsize;

	OSA_mutexCreate(&mulock);
	OSA_mutexCreate(&mulocktrack);
#ifdef DEEPDETECT
//	readclassnmaeformtxt("config/labels.txt");
	model.push_back(string("config/yolo_v3_tiny.yml"));
	modelsize.push_back(Size(1920,1080));
	detectornew=DetectorFactory::getinstance()->createDetector(DetectorFactory::DEEPLEARNDETECTOR);
#else
	model.push_back(string("car.xml"));
	modelsize.push_back(Size(32,32));
	detectornew=DetectorFactory::getinstance()->createDetector(DetectorFactory::HOGDETECTOR);
#endif

	detectornew->setparam(Detector::MAXTRACKNUM,20);
	detectornew->init(model,modelsize);

#ifdef DEEPDETECT
	detectornew->dynamicsetparam(Detector::DETECTSCALE100x,100);
#else
	detectornew->dynamicsetparam(Detector::DETECTSCALE100x,200);
#endif

	detectornew->dynamicsetparam(Detector::DETECTMOD,1);
	detectornew->dynamicsetparam(Detector::DETECTFREQUENCY,1);
	detectornew->dynamicsetparam(Detector::DETECTTRACKSTOPTIME,100);
	detectornew->dynamicsetparam(Detector::DETECTTRACKCORRECT,0);
	detectornew->dynamicsetparam(Detector::DETECTNOTRACK,0);
	detectornew->dynamicsetparam(Detector::DETECTMINAREA,mtdcfg_alg.movminwidth*mtdcfg_alg.movminheight);
	detectornew->dynamicsetparam(Detector::DETECTMAXAREA,mtdcfg_alg.movmaxwidth*mtdcfg_alg.movmaxheight);
	
	detectornew->getversion();
	detectornew->setasyncdetect(detectcall,trackcall, NULL);

	setmtdstat(mtdcfg_alg.movedetectalgenable);
}

int DetectAlg::JudgeLkFastV2(Mat src)
{
	int ret=-1;
	double angle=getcurrentcapangle();
	int postionid=-1;

	double angleoffset=0;
	static double angleoffsetpre=0;
	
	for(int i=0;i<angleposv2.size();i++)
	{
		angleoffset=fabs(angle-angleposv2[i]);

		if(angleoffset<LKMOVANGLE)
		{
			postionid=i;	
		}
	}
	
	if(postionid!=-1)
	{
		Mat dst=panoblockv2[postionid];
		memcpy(dst.data,src.data,dst.cols*dst.rows*dst.channels());
		ret=postionid;
	}

	return ret;
}

void DetectAlg::panomoveprocessV2()
{
	Mat process;
	char recoardnum[50];
	int blocknum=movblocknum;
	Mat src;
	static int premodelnum=0;
	static int prenum=-1;
	if(prenum!=blocknum)
	{
		prenum=blocknum;
		setnewframe(1);
	}
	else
		setnewframe(0);
	
	int newmat=getnewframe();

	if(MULTICPUPANO)
	{
		for(int i=0;i<=MULDETECTTRAIN;i++)
		{
			blocknum=(blocknum+mtdpositonnum-i)%mtdpositonnum;
			if(MOVDETECTDOWENABLE)
			{
				resize(panoblockv2[blocknum],panoblockdown,Size(Config::getinstance()->getmvprocesswidth()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup()),0,0,INTER_LINEAR);
				process=panoblockdown;
			}
			else
				process=panoblockv2[blocknum];

			//if(blocknum==0||blocknum==1)
			{
				//if(lkmove.backgroundmov[blocknum]==0)
				{
					//setwarndetect(process.cols,process.rows,blocknum);
					//setmvprocessangle(LKprocessangle[blocknum],blocknum);			
				}

				if(newmat)
				{
					if(Config::getinstance()->getpanocalibration())
					{
						//lkmove.lkmovdetectpreprocess(process,LKRramegray,blocknum);
						//lkmove.backgroundmov[blocknum]=1;
						memcpy(LKRramegray.data,process.data,process.cols*process.rows*process.channels());
					}
					else
					{
						//lkmove.backgroundmov[blocknum]=1;
						memcpy(LKRramegray.data,process.data,LKRramegray.cols*LKRramegray.rows*LKRramegray.channels());
					}
				}
				else
				{
					printf("******blocknum=%d prenum=%d\n******",blocknum,prenum);
				}

				if(getmodeling())
				{
					int num=getmodelnum(blocknum);
					if(num<MODELINGNUM&&newmat)
					{
						premodelnum=num;
						//LKRramegray(blackrect).copyTo(Modelframe[blocknum][premodelnum](blackrect));
						LKRramegray.copyTo(Modelframe[blocknum][premodelnum]);
						setmodelnum(blocknum);	
					}
				}
				else	
				{
					//LKRramegray(blackrect).copyTo(LKRramegrayblackboard(blackrect));
					LKRramegray.copyTo(LKRramegrayblackboard);
				}

				if(getmodeling()==0)
					videowriter[blocknum]<<LKRramegrayblackboard;
				else
					videowriter[blocknum]<<Modelframe[blocknum][premodelnum];
						

				if(getmodeling())
				{
						m_pMovDetector->setFrame(Modelframe[blocknum][premodelnum],Modelframe[blocknum][premodelnum].cols,Modelframe[blocknum][premodelnum].rows,blocknum,10,Config::getinstance()->getminarea(),Config::getinstance()->getmaxarea(),Config::getinstance()->getdetectthread());
				}
				else
				{		
					Rect roi=Rect(0,0,LKRramegrayblackboard.cols,LKRramegrayblackboard.rows);
					bool roienable=false;
					mtd_curangle[blocknum] = getcurrentcapangle();
					if(Render::pthis->mtdareapoints[0][blocknum])
						detectornew->detectasync(LKRramegrayblackboard, blocknum, roi, roienable);
				}
			}
		}
		return;
	}

}

void DetectAlg::panomoveprocessV2(Mat src, int chid)
{
	vector<BoundingBox> trackbox;
	vector<BoundingBox> algbox;
	Mat dst;
	//src.copyTo(dst);
	cvtColor(src,dst,COLOR_BGR2RGB);
	resize(dst,dst,Size(1920,1080));

	Rect roi=Rect(dst.cols*1/4,dst.rows*1/4,dst.cols*2/4,dst.rows*2/4);
	bool roienable=false;
#ifdef DEEPDETECT
	detectornew->detectasync(dst, chid, roi, roienable);
	//detectornew->detect(src,algbox,trackbox);
#else
		Mat src1;
		cvtColor(src,src1,COLOR_RGB2GRAY);
		printf("the w=%d h=%d \n",src1.cols,src1.rows);
		detectornew->detect(src1,algbox,trackbox);
#endif
}

void DetectAlg::trackcall(vector<BoundingBox>& trackbox,void *context,int chid)
{
	printf("%s,%d, detectcall start\n",__FILE__, __LINE__);
	instance->trackbox_.clear();
	OSA_mutexLock(&instance->mulocktrack);
	instance->trackbox_.insert(instance->trackbox_.begin(),trackbox.begin(),trackbox.end());
	OSA_mutexUnlock(&instance->mulocktrack);
	//swap(detectbox_,trackbox);
}

void DetectAlg::detectcall(vector<BoundingBox>& trackbox,void *context,int chid)
{
	instance->detectbox_angle.detectbox_.clear();
	OSA_mutexLock(&instance->mulock);
	instance->detectbox_angle.detectbox_.insert(instance->detectbox_angle.detectbox_.begin(),trackbox.begin(),trackbox.end());
	instance->detectbox_angle.angle = instance->mtd_curangle[chid];
	OSA_mutexUnlock(&instance->mulock);
	//swap(detectbox_,trackbox);
	if(instance->detectbox_angle.detectbox_.size() >0)
	{
		printf("%s,%d, detectcall chid=%d, angle=%f\n",__FILE__, __LINE__, chid, instance->detectbox_angle.angle);
		for(int i  = 0; i < instance->detectbox_angle.detectbox_.size(); i++)
		{
			Rect r = instance->detectbox_angle.detectbox_[i];
			printf("(%d,%d,%d,%d),", r.x, r.y, r.width, r.height);
		}
		printf("\n");
	}
	setmvdetectV2(instance->detectbox_angle,chid);
}

#endif
DetectAlg::DetectAlg():newframe(0),currentcapangle(0),movblocknum(0),movblocknumpre(-1),movdetectenable(0)
	{
	};
DetectAlg::~DetectAlg()
	{
	};
#define MINAREDETECT (100)


void DetectAlg::equalize(Mat& src)
{
	int Total = src.total(); 
	vector<int>piexl(256, 0); 
	for (int i = 0; i < src.total();++i)
		piexl[src.data[i]]++; 
	for (int i = 1; i < 256; ++i)
		piexl[i]+=piexl[i-1]; 
	vector<int>piexlnew(256, 0); 
		for (int i = 0; i < 256; ++i) { 
			piexlnew[i] = double(piexl[i]) / Total * 255;
			} 
		for (int i = 0; i < src.total(); ++i)
			src.data[i] =piexlnew[src.data[i]];

}

void DetectAlg::detectprocesstest(Mat src)
{	

		if(DETECTTEST)
		{
			//src.copyTo(MvtestFRrame[pp]);
			cvtColor(src,MvtestFRrame[pp],CV_BGR2GRAY);
			pp^=1;
			OSA_semSignal(&mainProcThrdetectObj.procNotifySem);
			//printf("***********DETECTTEST*********\n");
		}


}

void DetectAlg::panomoveprocess()
{
	Mat process;
	char recoardnum[50];
	int blocknum=movblocknum;
	Mat src;
	static int premodelnum=0;
	static int prenum=-1;
	if(prenum!=blocknum)
		{
			prenum=blocknum;
			setnewframe(1);
		}
	else
		setnewframe(0);
	
	int newmat=getnewframe();

	if(MULTICPUPANO)
		{

			for(int i=0;i<=MULDETECTTRAIN;i++)
				{
					blocknum=(blocknum+MULTICPUPANONUM-i)%MULTICPUPANONUM;
					if(MOVDETECTDOWENABLE)
						{
							resize(panoblock[blocknum],panoblockdown,Size(Config::getinstance()->getmvprocesswidth()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup()),0,0,INTER_LINEAR);
							process=panoblockdown;
						}
					else
						process=panoblock[blocknum];
					//blur(panoblockdown,panoblockdown,Size(3,3));
					//equalize(panoblockdown);
					//equalizeHist(panoblockdown, panoblockdown);
					//sprintf(recoardnum,"%d.bmp",blocknum);
					//imwrite(recoardnum,process);
					//imshow(recoardnum,process);
					//waitKey(1);
					//if(blocknum==0||blocknum==1)
						{
	

						if(lkmove.backgroundmov[blocknum]==0)
							{
								//setwarndetect(process.cols,process.rows,blocknum);
								setmvprocessangle(LKprocessangle[blocknum],blocknum);
								
							}

						if(newmat)
							{
								if(Config::getinstance()->getpanocalibration())
								{
									lkmove.lkmovdetectpreprocess(process,LKRramegray,blocknum);
								}
								else
								{
									lkmove.backgroundmov[blocknum]=1;
									memcpy(LKRramegray.data,process.data,LKRramegray.cols*LKRramegray.rows*LKRramegray.channels());
								}
							}
						else
							{
								printf("******blocknum=%d prenum=%d\n******",blocknum,prenum);
							}

						if(getmodeling())
							{
								int num=getmodelnum(blocknum);
								if(num<MODELINGNUM&&newmat)
									{
										premodelnum=num;
										LKRramegray(blackrect).copyTo(Modelframe[blocknum][premodelnum](blackrect));
										setmodelnum(blocknum);	
									}
							}
						else	
							{
								LKRramegray(blackrect).copyTo(LKRramegrayblackboard(blackrect));
							}


						if(getmodeling()==0)
							videowriter[blocknum]<<LKRramegrayblackboard;
						else
							videowriter[blocknum]<<Modelframe[blocknum][premodelnum];
						

						if(getmodeling())
							{
								m_pMovDetector->setFrame(Modelframe[blocknum][premodelnum],Modelframe[blocknum][premodelnum].cols,Modelframe[blocknum][premodelnum].rows,blocknum,10,Config::getinstance()->getminarea(),Config::getinstance()->getmaxarea(),Config::getinstance()->getdetectthread());
							}
						else
						{
							m_pMovDetector->setFrame(LKRramegrayblackboard,LKRramegrayblackboard.cols,LKRramegrayblackboard.rows,blocknum,10,Config::getinstance()->getminarea(),Config::getinstance()->getmaxarea(),Config::getinstance()->getdetectthread());
						}

				}
				}
			return;
		}

	Mat pano360=getpanolastmap();
	
#if PANOGRAYDETECT
	if(MOVEBLOCKENABLE)
		{
			pano360(Rect(1920*5+15000,0,PANO360WIDTH*2,PANO360HEIGHT)).copyTo(panoblock[0]);
			if(MOVDETECTDOWENABLE)
				{
					resize(panoblock[0],panoblockdown,Size(PANO360WIDTH*2/MOVDETECTDOW,PANO360HEIGHT/MOVDETECTDOW),0,0,INTER_LINEAR);
					process=panoblockdown;
				}
			else
				process=panoblock[0];

			printf("%s,%d, setFrame\n", __FILE__, __LINE__);
			m_pMovDetector->setFrame(process,process.cols,process.rows,0,10,Config::getinstance()->getminarea(),Config::getinstance()->getmaxarea(),Config::getinstance()->getdetectthread());


		}

#else
	imwrite("pano.bmp",pano360);

	
		
		//m_pMovDetector->setFrame(frame_gray,frame_gray.cols,frame_gray.rows,0,2,8,200,30);

#endif
		

}
void DetectAlg::main_proc_func()
{

	
	OSA_printf("DetectAlg %s: Main Proc Tsk Is Entering...\n",__func__);
	unsigned int framecount=0;
	OSA_BufInfo* info=NULL;
	OSA_BufInfo* infocap=NULL;
	Queue *queuebuf;
	Mat pre;
	while(mainProcThrdetectObj.exitProcThread ==  false)
	{

		OSA_semWait(&mainProcThrdetectObj.procNotifySem, OSA_TIMEOUT_FOREVER);
		//OSA_printf("**********%s***********\n",__func__);
		double exec_time = (double)getTickCount();
#if USE_DETECTV2
		panomoveprocessV2();
#else
		panomoveprocess();
#endif
		exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
	}
}

int DetectAlg::MAIN_threadCreate(void)
{
	int iRet = OSA_SOK;
	iRet = OSA_semCreate(&mainProcThrdetectObj.procNotifySem ,1,0) ;
	OSA_assert(iRet == OSA_SOK);


	mainProcThrdetectObj.exitProcThread = false;

	mainProcThrdetectObj.initFlag = true;

	mainProcThrdetectObj.pParent = (void*)this;

	iRet = OSA_thrCreate(&mainProcThrdetectObj.thrHandleProc, mainProcTsk, 0, 0, &mainProcThrdetectObj);
	





	return iRet;
}
int DetectAlg::MAIN_threadDestroy(void)
{
	int iRet = OSA_SOK;

	mainProcThrdetectObj.exitProcThread = true;
	OSA_semSignal(&mainProcThrdetectObj.procNotifySem);

	iRet = OSA_thrDelete(&mainProcThrdetectObj.thrHandleProc);

	mainProcThrdetectObj.initFlag = false;
	OSA_semDelete(&mainProcThrdetectObj.procNotifySem);

	return iRet;
}


void DetectAlg::NotifyFunclk(void *context, int chId)
{
	
	DetectAlg *pParent = (DetectAlg*)context;
	pParent->lkmove.getMoveTarget(pParent->detectlk,chId);
	
	setmvdetect(pParent->detectlk,chId);


}
void DetectAlg::NotifyFunccd(void *context, int chId)
{
	
	DetectAlg *pParent = (DetectAlg*)context;
	pParent->classifydetect->getMoveTarget(pParent->detectlk,chId);
	
	setmvdetect(pParent->detectlk,chId);


}

void DetectAlg::NotifyFunc(void *context, int chId)
{
	std::vector<cv::Rect>	detect;
	DetectAlg *pParent = (DetectAlg*)context;
	pParent->m_pMovDetector->getMoveTarget(pParent->detect_vect,chId);
	for(int i=0;i<pParent->detect_vect.size();i++)
		{
			detect.push_back(pParent->detect_vect[i].targetRect);
		}
	setmvdetect(detect,chId);
	//if()
}
#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)
int DetectAlg::mk_dir(char *dir)
{

    DIR *mydir = NULL;
    if((mydir= opendir(dir))==NULL)
    {
      int ret = mkdir(dir, MODE);
      if (ret != 0)
      {
          return -1;
      }
      printf("%s created sucess!/n", dir);
    }
    else
    {
        printf("%s exist!/n", dir);
    }
 
    return 0;
}

void DetectAlg::create()
{
	for(int i=0;i<2;i++)
	MvtestFRrame[i]=Mat(Config::getinstance()->getpanoprocessheight(),Config::getinstance()->getpanoprocesswidth(),CV_8UC1,cv::Scalar(0));
	MvtestFRramegray=Mat(Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocesswidth()/(Config::getinstance()->getmvdownup()),CV_8UC1,cv::Scalar(0));

	for(int i=0;i<MOVEBLOCKNUM;i++)
		{
			if(PANOGRAYDETECT)
			panoblock[i]=Mat(Config::getinstance()->getmvprocessheight(),Config::getinstance()->getmvprocesswidth(),CV_8UC1,cv::Scalar(0));
			else
			panoblock[i]=Mat(Config::getinstance()->getmvprocessheight(),Config::getinstance()->getmvprocesswidth(),CV_8UC3,cv::Scalar(0));	
		}

	panograysrc=Mat(Config::getinstance()->getpanoprocessheight(),Config::getinstance()->getpanoprocesswidth(),CV_8UC1,cv::Scalar(0));

	LKRramegray=Mat(Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocesswidth()/(Config::getinstance()->getmvdownup()),CV_8UC1,cv::Scalar(0));
	
	detedtgraysrc=Mat(Config::getinstance()->getpanoprocessheight(),Config::getinstance()->getpanoprocesswidth(),CV_8UC1,cv::Scalar(0));

	LKRramegrayblackboard=Mat(Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocesswidth()/(Config::getinstance()->getmvdownup()),CV_8UC1,cv::Scalar(0));

	for(int i=0;i<MOVEBLOCKNUM;i++)
		for(int j=0;j<MODELINGNUM;j++)
			Modelframe[i][j]=Mat(Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocesswidth()/(Config::getinstance()->getmvdownup()),CV_8UC1,cv::Scalar(0));
	//Modelframe
	
	MvtestFRramegray=Mat(Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocesswidth()/(Config::getinstance()->getmvdownup()),CV_8UC1,cv::Scalar(0));

	panoblockdown=Mat(Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocesswidth()/(Config::getinstance()->getmvdownup()),CV_8UC1,cv::Scalar(0));

	printf("panoblockdown w=%d h=%d \n",Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocesswidth()/(Config::getinstance()->getmvdownup()));
	int board=40;
	int boardh=30;
	if(Config::getinstance()->getmvdownup()<=2)
		board=150;
	if(Config::getinstance()->getpanocalibration()==0)
		{
			board=0;
			 boardh=0;

		}
	
	
	blackrect=Rect(board,boardh,Config::getinstance()->getmvprocesswidth()/Config::getinstance()->getmvdownup()-2*board,Config::getinstance()->getmvprocessheight()/(Config::getinstance()->getmvdownup())-2*boardh);
	char bufname[50];
	m_pMovDetector=MvDetector_Create();
	m_pMovDetector->init(NotifyFunc,( void *) this);

	lkmove.lkmovdetectcreate(NotifyFunclk,( void *) this);

	classifydetect=new ClassifyDetect();
	
	classifydetect->movdetectcreate(NotifyFunccd,( void *)this);
	classifydetect->create();
	//bgs = new FrameDifference;
	//bgs = new StaticFrameDifference;
	//bgs = new TwoPoints;
	//bgs = new LBFuzzyGaussian;
	double rate = 2.0;
	#if MULTICPUPANOLK
	Size videoSize(Config::getinstance()->getmvprocesswidth()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup());
	#else
	Size videoSize(Config::getinstance()->getmvprocesswidth()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup());
	#endif
	//videowriter=VideoWriter("mov.avi", CV_FOURCC('M', 'J', 'P', 'G'), rate, videoSize);
	//bool status=videowriter.open("mov.avi", CV_FOURCC('X', 'V', 'I', 'D'),rate, videoSize, false);
	//mk_dir("/home/nvidia/calib/");
	for(int i=0;i<MULTICPUPANONUM;i++)
		{
			//sprintf(bufname,"/home/nvidia/calib/mov%d.avi",i);
			//videowriter[i].open(bufname, CV_FOURCC('M', 'J', 'P', 'G'),rate, videoSize, false);
		}
	
	OSA_printf("***********Create* end=**************\n");

	MAIN_threadCreate();

	registorfun();

}

void DetectAlg::detectprocess(Mat src,OSA_BufInfo* frameinfo)
{	

	
	//printf("********8888888888888888888888********\n");
	
	double angle=frameinfo->framegyroyaw*1.0/ANGLESCALE+StichAlg::getinstance()->getcamerazeroossfet();
	
	if(angle<0)
		angle+=360;
	else if(angle>=360)
		angle-=360;
	
	setcurrentcapangle(angle);

	
	if(DETECTTEST)
		{
			detectprocesstest(src);
		}
	else if(MULTICPUPANOLK)
		{
		if(Config::getinstance()->getpanocalibration())
		{
			MulticpuLKpanoprocess(src);
		}
		else
		   Multicpufilepanoprocess(src);
		}

}
int DetectAlg::JudgeLk(Mat src)
{
	int ret=-1;
	double anglepos[MOVELKBLOCKNUM];
	memset(anglepos,0,sizeof(anglepos));
	double angle=getcurrentcapangle();
	//printf("the cap angle =%f\n",angle);
	int postionid=-1;
	for(int i=0;i<MOVELKBLOCKNUM;i++)
		{
			anglepos[i]=360.0/MOVELKBLOCKNUM*i;
			if(i==0)
				{
					if(LkAngle[i]==0&&angle!=0&&abs(angle-anglepos[i])<LKMOVANGLE)
						{
							LkAngle[i]=1;
							LKprocessangle[i]=angle;
							postionid=i;

						}

				}
			else if(abs(angle-anglepos[i])<LKMOVANGLE)
				{
					if(LkAngle[i]==0)
						{
							LkAngle[i]=1;
							LKprocessangle[i]=angle;
							postionid=i;
						}
				}
			else
				LkAngle[i]=0;

		}
	
	//if(angle)


	
	
	if(postionid!=-1)
		{
			//printf("[postionid=%d]the angle =%f LKprocessangle[i]=%f\n",postionid,angle,LKprocessangle[postionid]);
			Mat dst=panoblock[postionid];
			memcpy(dst.data,src.data,dst.cols*dst.rows*dst.channels());
			ret=postionid;
		}
	return ret;

}

int DetectAlg::JudgeLkFastNew(Mat src)
{
	int ret=-1;
	double anglepos[MOVELKBLOCKNUM];
	memset(anglepos,0,sizeof(anglepos));
	double angle=getcurrentcapangle();
	//printf("the cap angle =%f\n",angle);
	int postionid=-1;

	double angleoffset=0;
	static double angleoffsetpre=0;

	static int currentid=0;


	#if 0
	for(int i=0;i<MOVELKBLOCKNUM;i++)
		{
			anglepos[i]=360.0/MOVELKBLOCKNUM*i;

			angleoffset=abs(angle-anglepos[i]);

			if(angleoffset<LKMOVANGLE)
				{

					/*
					if(LKangleoffset[i]<angleoffset)
						{	
							LKangleoffset[i]=angleoffset;
						}
					else
						{
							continue;
						}
					*/
						LKprocessangle[i]=angle;
						postionid=i;
						
						
				}
			else
				LKangleoffset[i]=360;

		}
	
	#else

	//double angle=Status::getinstance()->getmvreachangle();

	if(Status::getinstance()->getmvreach())
		{
			Status::getinstance()->setmvreach(0);
			Status::getinstance()->nextid=Status::getinstance()->getnextmvdetectnum();
			//OSA_printf("*****the reach nexid=%d\n",Status::getinstance()->nextid);
			CMessage::getInstance()->MSGDRIV_send(MSGID_EXT_INPUT_MVDETECTGO,0);
			postionid=Status::getinstance()->getmvdetectnum();
			LKprocessangle[postionid]=angle;

			
			
			
			
			
		}
	

	#endif
	
	
	if(postionid!=-1)
		{
			//printf("[postionid=%d]the [angle =%f pos=%f]\n",postionid,angle,360.0/MOVELKBLOCKNUM*postionid);
			Mat dst=panoblock[postionid];
			memcpy(dst.data,src.data,dst.cols*dst.rows*dst.channels());
			ret=postionid;
		}

	
	return ret;

}

int DetectAlg::JudgeLkFast(Mat src)
{
	int ret=-1;
	double anglepos[MOVELKBLOCKNUM];
	memset(anglepos,0,sizeof(anglepos));
	double angle=getcurrentcapangle();
	int postionid=-1;

	double angleoffset=0;
	static double angleoffsetpre=0;
	
	for(int i=0;i<MOVELKBLOCKNUM;i++)
		{
			anglepos[i]=360.0/MOVELKBLOCKNUM*i;

			angleoffset=abs(angle-anglepos[i]);

			if(angleoffset<LKMOVANGLE)
				{

					/*
					if(LKangleoffset[i]<angleoffset)
						{	
							LKangleoffset[i]=angleoffset;
						}
					else
						{
							continue;
						}
					*/
						LKprocessangle[i]=angle;
						postionid=i;
						
						
				}
			else
				LKangleoffset[i]=360;

		}
	

	
	if(postionid!=-1)
		{
			//printf("[postionid=%d]the [angle =%f pos=%f]\n",postionid,angle,360.0/MOVELKBLOCKNUM*postionid);
			Mat dst=panoblock[postionid];
			memcpy(dst.data,src.data,dst.cols*dst.rows*dst.channels());
			ret=postionid;
		}

	
	return ret;

}
void DetectAlg::Multicpufilepanoprocess(Mat& src)
{
	static int modelingnum=0;
	static int enablemodel=0;
	double exec_time = (double)getTickCount();
	Mat processsrc;
	int blucknum=-1;
	if(PANOGRAYDETECT)
		{
			cvtColor(src,detedtgraysrc,CV_BGR2GRAY);
			processsrc=detedtgraysrc;
		}
	else
		processsrc=src;

	int angle=getcurrentcapangle()*1000;
	int fixangle=Config::getinstance()->getcam_fixcamereafov()*1000;
	blucknum=angle/fixangle;

	

	//printf("the blucknum=%d angle=%f\n",blucknum,angle);
	if(blucknum!=-1)
		{
			Mat dst=panoblock[blucknum];
			memcpy(dst.data,processsrc.data,dst.cols*dst.rows*dst.channels());	
			if(movblocknumpre!=blucknum)
				{
					//OSA_printf("the movblocknumpre=%d\n",movblocknumpre);
					movblocknum=movblocknumpre;
					movblocknumpre=blucknum;
					LKprocessangle[blucknum]=1.0*angle/1000;
					OSA_semSignal(&mainProcThrdetectObj.procNotifySem);
					modelingnum++;
					enablemodel=1;
					
					//setnewframe(1);
				}
			

		}
	

}
void DetectAlg::MulticpuLKpanoprocess(Mat& src)
{
	if(getpanoflagenable()==0)
		return ;
	static int modelingnum=0;
	static int enablemodel=0;
	double exec_time = (double)getTickCount();
	Mat processsrc;
	int blucknum=-1;
	if(PANOGRAYDETECT)
		{
			cvtColor(src,detedtgraysrc,CV_BGR2GRAY);
			processsrc=detedtgraysrc;
		}
	else
		processsrc=src;

	if(Status::getinstance()->getusestepdetect())
		blucknum=JudgeLkFastNew(processsrc);
	else if(1)
	{
#if USE_DETECTV2
		blucknum=JudgeLkFastV2(processsrc);
#else
		blucknum=JudgeLkFast(processsrc);
#endif
	}
	else
		blucknum=JudgeLk(processsrc);

	if(blucknum!=-1)
		{
			if(movblocknumpre!=blucknum)
				{
					movblocknum=blucknum;
					movblocknumpre=movblocknum;
					OSA_semSignal(&mainProcThrdetectObj.procNotifySem);
					modelingnum++;
					enablemodel=1;
					//setnewframe(1);
				}		
		}
	else
		{
			//setnewframe(0);
			
			if(getmodeling()&&enablemodel)
				{
					//setnewframe(0);
					OSA_printf("jian mo doing \n");
					OSA_semSignal(&mainProcThrdetectObj.procNotifySem);

				}
			if(modelingnum>=100)
				{
				setmodeling(0);
				modelingnum=100;
				}

		}
	


exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
     //  OSA_printf("the %s exec_time=%f\n",__func__,exec_time);

}


void DetectAlg::registorfun()
{
	
	
}

void DetectAlg::updatemtdparam()
{
	mtdcfg_t mtdcfg_alg = Status::getinstance()->mtdcfg;
#if USE_DETECTV2
	getinstance()->detectornew->dynamicsetparam(Detector::DETECTMINAREA, mtdcfg_alg.movminwidth * mtdcfg_alg.movminheight);
	getinstance()->detectornew->dynamicsetparam(Detector::DETECTMAXAREA, mtdcfg_alg.movmaxwidth * mtdcfg_alg.movmaxheight);
#endif
	getinstance()->setmtdstat(mtdcfg_alg.movedetectalgenable);
}

DetectAlg *DetectAlg::getinstance()
{
	if(instance==NULL)
		instance=new DetectAlg();



	return instance;
	


}

