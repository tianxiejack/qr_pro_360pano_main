#include "ImageProcess.hpp"
#include"Homography.hpp"
#include "osa_image_queue.h"
#include"osa.h"
#include"Stich.hpp"
#include"Gyroprocess.hpp"
#include "plantformcontrl.hpp"
#include "FrameDifference.h"
#include "StaticFrameDifference.h"
#include "TwoPoints.h"
#include "LBFuzzyGaussian.h"
#include "config.hpp"
#include"Queuebuffer.hpp"
#include"StichAlg.hpp"
#include"DetectAlg.hpp"
#include"Status.hpp"

#define IMAGEQUEUESIZE 4
#define IMAGCAPEQUEUESIZE 2
#define IMAGEQUEUEWIDTH 1920
#define IMAGEQUEUEHEIGHT 1080
#define IMAGEQUEUECHANNEL 3

#define SIGNALEQUEUEID 0

ImageProcess *ImageProcess::Pthis;

ImageProcess:: ImageProcess():pinpang(0),currentcount(0),AngleStich(-ANGLEINTREVAL),seamid(0),SeamEable(0),Seampostion(0),
			preflag(0),preangle(0),currentangle(0),xoffsetfeat(0),yoffsetfeat(0),camerazeroossfet(0),zeroflag(0),zeroangle(0),zerocalibflag(1),
			zeroflameupdate(1),gyroangle(0),pp(0),blocknum(0),zeroprocessflag(0),zerocalibing(0),calibrationzeroangle(0),zerodropflame(0),
			zerodroreset(0),currentcapangle(0),zeroptzangle(0),tailcut(0),movblocknumpre(0),newframe(0)
{
	memset(m_bufQue, 0, sizeof(m_bufQue));
	memset(mcap_bufQue, 0, sizeof(mcap_bufQue));
	memset(LkAngle,0,sizeof(LkAngle));
	memset(panoblockangle,0,sizeof(panoblockangle));
	memset(LKprocessangle,0,sizeof(LKprocessangle));

	for(int i=0;i<MOVELKBLOCKNUM;i++)
		{
			LKangleoffset[i]==360;

		}
	//memset(LKangleoffset,0,sizeof(LKangleoffset));
	Pthis=this;

}
ImageProcess::~ImageProcess()
{


}

void ImageProcess::Init()
{
	//processgray[0]=
	stichinit();
	for(int i=0;i<FRAMEFIFO;i++)
		{
			processgray[i]=Mat(Config::getinstance()->getcamheight(),Config::getinstance()->getcamwidth(),CV_8UC1,cv::Scalar(0));
			processtest=Mat(Config::getinstance()->getcamheight(),Config::getinstance()->getcamwidth(),CV_8UC1,cv::Scalar(0));
			processgraytemp[i]=Mat(405,720,CV_8UC1);
			//OSA_printf("the  DATA=%p\n",processgray[i].data);		
		}
	for(int chId=0; chId<IMAGEQUEUE; chId++)
		image_queue_create(&m_bufQue[chId], IMAGEQUEUESIZE,
				Config::getinstance()->getcamwidth()*Config::getinstance()->getcamheight()*IMAGEQUEUECHANNEL,
				memtype_normal);
	for(int chId=0; chId<IMAGEQUEUE; chId++)
		image_queue_create(&mcap_bufQue[chId], IMAGCAPEQUEUESIZE,
				Config::getinstance()->getcamwidth()*Config::getinstance()->getcamheight()*IMAGEQUEUECHANNEL,
				memtype_normal);
	processtemp=Mat(Config::getinstance()->getpanoprocessheight(),Config::getinstance()->getpanoprocesswidth(),CV_8UC3,cv::Scalar(0));
	processtempgray=Mat(Config::getinstance()->getpanoprocessheight(),Config::getinstance()->getpanoprocesswidth(),CV_8UC1,cv::Scalar(0));
	
	for(int i=0;i<MAXSEAM;i++)
		{
			Seamframe[i]=Mat(Config::getinstance()->getpanoprocessheight(),Config::getinstance()->getpanoprocesswidth(),CV_8UC3,cv::Scalar(0));
		}
	zeroflame=Mat(Config::getinstance()->getcamheight(),Config::getinstance()->getcamwidth(),CV_8UC3,cv::Scalar(0));

	for(int i=0;i<MOVEBLOCKNUM;i++)
		{
			if(PANOGRAYDETECT)
			panoblock[i]=Mat(Config::getinstance()->getmvprocessheight(),Config::getinstance()->getmvprocesswidth(),CV_8UC1,cv::Scalar(0));
			else
			panoblock[i]=Mat(Config::getinstance()->getmvprocessheight(),Config::getinstance()->getmvprocesswidth(),CV_8UC3,cv::Scalar(0));	
		}
	ProcessPreimage=Mat(Config::getinstance()->getpanoprocessheight(),Config::getinstance()->getpanoprocesswidth(),CV_8UC3,cv::Scalar(0));
	
	for(int i=0;i<2;i++)
	MvtestFRrame[i]=Mat(Config::getinstance()->getpanoprocessheight(),Config::getinstance()->getpanoprocesswidth(),CV_8UC1,cv::Scalar(0));

	
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

	printf("====== ImageProcess::Init  panoblockdown w=%d h=%d \n",Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocesswidth()/(Config::getinstance()->getmvdownup()));
	int board=40;
	int boardh=30;
	if(Config::getinstance()->getmvdownup()<=2)
		board=150;
	
	AngleStich=-Config::getinstance()->getangleinterval();
	blackrect=Rect(board,boardh,Config::getinstance()->getmvprocesswidth()/Config::getinstance()->getmvdownup()-2*board,Config::getinstance()->getmvprocessheight()/(Config::getinstance()->getmvdownup())-2*boardh);
	
	MAIN_threadCreate();
	//MAIN_detectthreadCreate();
}
void ImageProcess::Create()
{

}
void ImageProcess::unInit()
{
	
	MAIN_threadDestroy();
	for(int chId=0; chId<IMAGEQUEUE; chId++)
		image_queue_delete(&m_bufQue[chId]);
	for(int chId=0; chId<IMAGEQUEUE; chId++)
		image_queue_delete(&mcap_bufQue[chId]);
}

void ImageProcess::CaptureThreadProcess(Mat src,OSA_BufInfo* frameinfo,int chid)
{
	int queueid=chid;
	Plantformpzt::getinstance()->setplantformcalibration(frameinfo->calibration);
	Status::getinstance()->calibration=frameinfo->calibration;
	
	OSA_BufInfo* info=NULL;
	info = image_queue_getEmpty(&mcap_bufQue[queueid]);
	if(info == NULL)
		return;
	
	memcpy(info->virtAddr,src.data,src.rows*src.cols*src.channels());
	//OSA_printf("the w=%d h=%d c=%d\n",src.cols,src.rows,src.channels());
	info->channels = frameinfo->channels;
	info->width =frameinfo->width;
	info->height =frameinfo->height;
	info->timestamp =frameinfo->timestamp;
	info->framegyroroll=frameinfo->framegyroroll;
	info->framegyropitch=frameinfo->framegyropitch;
	info->framegyroyaw=frameinfo->framegyroyaw;
	info->calibration=frameinfo->calibration;
	image_queue_putFull(&mcap_bufQue[queueid], info);

	//AngleStich=frameinfo->framegyroyaw*1.0/ANGLESCALE;
}



void ImageProcess::detectprocesstest(Mat src,OSA_BufInfo* frameinfo)
{	

		if(DETECTTEST)
		{
			//src.copyTo(MvtestFRrame[pp]);
			cvtColor(src,MvtestFRrame[pp],CV_BGR2GRAY);
			pp^=1;
			OSA_semSignal(&mainProcThrdetectObj.procNotifySem);
			printf("***********DETECTTEST*********\n");
		}


}

void ImageProcess::detectprocess(Mat src,OSA_BufInfo* frameinfo)
{	

	
	//printf("********8888888888888888888888********\n");
	
	double angle=frameinfo->framegyroyaw*1.0/ANGLESCALE+getcamerazeroossfet();
	if(angle<0)
		angle+=360;
	else if(angle>=360)
		angle-=360;
	setcurrentcapangle(angle);
	if(MULTICPUPANOLK)
		MulticpuLKpanoprocess(src);

}

int ImageProcess::MAIN_threadCreate(void)
{
	int iRet = OSA_SOK;
	iRet = OSA_semCreate(&mainProcThrObj.procNotifySem ,1,0) ;
	OSA_assert(iRet == OSA_SOK);


	mainProcThrObj.exitProcThread = false;

	mainProcThrObj.initFlag = true;

	mainProcThrObj.pParent = (void*)this;

	iRet = OSA_thrCreate(&mainProcThrObj.thrHandleProc, mainProcTsk, 0, 0, &mainProcThrObj);
	




	//OSA_waitMsecs(2);

	return iRet;
}
int ImageProcess::MAIN_detectthreadCreate(void)
{
	int iRet = OSA_SOK;
	iRet = OSA_semCreate(&mainProcThrdetectObj.procNotifySem ,1,0) ;
	OSA_assert(iRet == OSA_SOK);


	mainProcThrdetectObj.exitProcThread = false;

	mainProcThrdetectObj.initFlag = true;

	mainProcThrdetectObj.pParent = (void*)this;

	iRet = OSA_thrCreate(&mainProcThrdetectObj.thrHandleProc, maindetectTsk, 0, 0, &mainProcThrdetectObj);

	return iRet;

}

int ImageProcess::MAIN_threadDestroy(void)
{
	int iRet = OSA_SOK;

	mainProcThrObj.exitProcThread = true;
	OSA_semSignal(&mainProcThrObj.procNotifySem);

	iRet = OSA_thrDelete(&mainProcThrObj.thrHandleProc);

	mainProcThrObj.initFlag = false;
	OSA_semDelete(&mainProcThrObj.procNotifySem);

	return iRet;
}
int ImageProcess::MAIN_detectthreadDestroy(void)
{
	int iRet = OSA_SOK;

	mainProcThrdetectObj.exitProcThread = true;
	OSA_semSignal(&mainProcThrdetectObj.procNotifySem);

	iRet = OSA_thrDelete(&mainProcThrdetectObj.thrHandleProc);

	mainProcThrdetectObj.initFlag = false;
	OSA_semDelete(&mainProcThrdetectObj.procNotifySem);

	return iRet;
}

void ImageProcess::OptiSeam()
{


	Mat src=getPreFame();
	Mat dst=getCurrentFame();
	if(getSeamEnable())
		{	
			//printf("the seam=%d\n",getSeamPos());
			//dst=fusionframe;
			FusionSeam(src,dst,getSeamPos());	
		}

}

int ImageProcess::judgezero()
{
	double currentA=getcurrentangle();
	double zeroA=getzeroangle();

	
	if(currentA>360-ZEROJUEGE)
		{
			//currentA-=360;
			setzeroflag(1);
		}
	//double angleoffset=abs(currentA-zeroA);
	else{
			setzeroflag(0);
			setzerocalib(0);

		}

		
	//if

}

int  ImageProcess::Panorest()
{
	if(getstichreset()==0)
		return 0;

	DetectAlg::getinstance()->lkmove.lkmovdetectreset();
	DetectAlg::getinstance()->m_pMovDetector->mvPause();
	
	Gyrorest();
	StichAlg::getinstance()->setcamerazeroossfet(0);
	StichAlg::getinstance()->setzeroflameupdate(1);
	StichAlg::getinstance()->setzeroangle(0);
	StichAlg::getinstance()->setzeroflag(0);
	setstichreset(0);
	for(int i=0;i<MOVELKBLOCKNUM;i++)
		DetectAlg::getinstance()->LKprocessangle[i]=0;
	return 1;
}
#if 0
void ImageProcess::zeroprocess()
{
	double angle=0;
	double gyroangleoffset=0;
	setzeroprocessflag(0);
	judgezero();
	int zerooffset=0;
	
	int zerobool=getzeroflag();
	int calibool=getzerocalib();
	if(getorcezeroprocess()==0)
		{
			if(zerobool==0||calibool==1)
				{
					setzerocalibing(0);
					return ;
				}
		}
	setforcezeroprocess(0);
	setzerocalibing(1);
	Mat zeroF=getzeroflame();
	Mat currentF=getcurrentflame();

	double currentgyroA=getgyroangle();
	if(currentgyroA>300)
		currentgyroA-=360;
	double zeroA=getzeroangle();
	gyroangleoffset=currentgyroA-zeroA;

	//setyawbase();
	if(zerocalibration(zeroF,currentF,&angle,&zerooffset))
		{
			
			//setcamerazeroossfet(angle-(currentgyroA-zeroA));
			setzerocalibing(0);
			if(ZEROCALIBRATIONMODE)
				{
					/***drop mod***/
					setzeroprocessflag(1);
					setzerocaliboffset(zerooffset);
					setcurrentangle(0);
						/***drop mod***/
					setzerodroreset(1);
					setcalibrationzeroangle(getgyroangle());
					
				}
			
			
			
			
		
			setyawbase(gyroangleoffset-angle);
			setzerocalib(1);
			
			printf("********basioffset=%f********zeroA=%f  gyroangleoffset=%f currentgyroA=%f \n",gyroangleoffset-angle,zeroA,gyroangleoffset,currentgyroA);
			//setzeroflag(0);
		}

	

	
}
#endif
void ImageProcess::Panoprocess(Mat src,Mat dst)
{
	//setNextFrameId();
	double angleoffset=0.0;
	int  piexoffset=0;
	int preoffset=getpanooffet(getpreangle());
	int curoffset=getpanooffet(getcurrentangle());
	angleoffset=getcurrentangle()-getpreangle();

	if(angleoffset<0)
		angleoffset=angleoffset+360;

	piexoffset=getpanooffet(angleoffset);


	Mat Imagepre;
	if(IMAGEPROCESSSTICH)
		{
			Imagepre=getpreprocessimage();
			setpreprocessimage(src);
		}
		

	
	setSeamPos(piexoffset);
	Mat temp;
	if(Config::getinstance()->getpanocylinder())
	temp=src;//getCurrentFame();
	else
	temp=dst;

	
	/*******************************/
	int flag=getzeroprocessflag();
	int getoffst=getzerocaliboffset();
	
	if(flag==1)
		{
			Matcpy(src,src,abs(getoffst));
			Matblack(src,src.cols-abs(getoffst));
			settailcut(src.cols-abs(getoffst));
			
		}


	/******************************/
	
	//Mat dst
	if(src.cols!=Config::getinstance()->getpanoprocesswidth()||src.rows!=Config::getinstance()->getpanoprocessheight())
			resize(src,temp,Size(Config::getinstance()->getpanoprocesswidth(),Config::getinstance()->getpanoprocessheight()),0,0,INTER_LINEAR);
	else
		{
		//memcpy(temp.data,src.data,PANO360WIDTH*PANO360HEIGHT*3);
		if(Config::getinstance()->getpanocylinder()==0)
		Matcpy(src,temp,Config::getinstance()->getpanoprocessshift());
		}
	//return ;
	double exec_time = (double)getTickCount();
	if(Config::getinstance()->getpanocylinder())
		cylinder(temp,dst,1.0*(Config::getinstance()->getcamfx())*Config::getinstance()->getpanoprocesswidth()/Config::getinstance()->getcamwidth(),Config::getinstance()->getpanoprocessshift());

		
	exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
	//OSA_printf("the %s exec_time=%f MS\n",__func__,exec_time);
	if(getpreframeflage()==0)
		return ;
	Mat pre=getpreframe();
	if(FEATURESTICH)
		{
			if(IMAGEPROCESSSTICH)
				getfeaturePanoOffset(Imagepre,dst,&xoffsetfeat,&yoffsetfeat);
			else
				getPanoOffset(dst,pre,&xoffsetfeat,&yoffsetfeat);
				
			//printf();
			setcurrentangle(offet2anglerelative2inter(-xoffsetfeat));
			curoffset=getpanooffet(getcurrentangle());
			setSeamPos(curoffset-preoffset);
			OSA_printf("the xoffsetfeat=%d  yoffsetfeat=%d getcurrentangle=%f\n",xoffsetfeat,yoffsetfeat,getcurrentangle());
		}
	
	if(getfusionenable()&&flag!=1)
		{
			//Mat pre=getpreframe();
			//if((getcurrentangle()<360-ZEROJUEGE)&&(getcurrentangle()>0))
				FusionSeam(pre,dst,getSeamPos());
			//	histequision(dst);
				//equalizeHist(dst, dst);
				//OSA_printf("the getSeamPos=%d  getpreangle=%f getcurrentangle=%f\n",getSeamPos(),getpreangle(),getcurrentangle());
		}

	/***********detect process **********************/
	//if(getcurrentangle()>360-ZEROJUEGE)
	//	return ;
	//if(FEATURETEST)
	//	return ;
	
	if(1)
		;
      else if(MULTICPUPANO)
	Multicpupanoprocess(dst);
	else
	cpupanoprocess(dst);


	//if()

	
	
	

}
void ImageProcess::NotifyFunclk(void *context, int chId)
{
	
	ImageProcess *pParent = (ImageProcess*)context;
	pParent->lkmove.getMoveTarget(pParent->detectlk,chId);
	
	setmvdetect(pParent->detectlk,chId);


}

void ImageProcess::NotifyFunc(void *context, int chId)
{
	std::vector<cv::Rect>	detect;
	ImageProcess *pParent = (ImageProcess*)context;
	pParent->m_pMovDetector->getMoveTarget(pParent->detect_vect,chId);
	for(int i=0;i<pParent->detect_vect.size();i++)
		{
			detect.push_back(pParent->detect_vect[i].targetRect);
		}
	setmvdetect(detect,chId);
	//if()
}

void ImageProcess::setwarndetect(int w,int h,int chid)
{
	int boardw=30;
	printf("w=%d h=%d\n",w,h);
	std::vector<cv::Point2i>	warnRoi;
	warnRoi.push_back(Point2i(boardw,boardw));
	warnRoi.push_back(Point2i(w-boardw,boardw));
	warnRoi.push_back(Point2i(boardw,h-boardw));
	warnRoi.push_back(Point2i(w-boardw,h-boardw));
	m_pMovDetector->setWarningRoi(warnRoi,chid);
	printf("2w=%d h=%d chid=%d\n",w,h,chid);


}

#define MINAREDETECT (20)



void ImageProcess::panomoveprocess()
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
	if(DETECTTEST)
		{
			src=MvtestFRrame[pp^1];
			if(MOVDETECTDOWENABLE)
				{
				
					resize(src,MvtestFRramegray,Size(Config::getinstance()->getmvprocesswidth()/Config::getinstance()->getmvdownup(),Config::getinstance()->getmvprocessheight()/Config::getinstance()->getmvdownup()),0,0,INTER_LINEAR);
					blur( MvtestFRramegray, MvtestFRramegray, Size(3,3) );
					src=MvtestFRramegray;
				}


			if(PANOGRAYDETECT)
			{
				if(lkmove.backgroundmov[0]==0)
					setmvprocessangle(LKprocessangle[0],0);
				#if 1
				lkmove.lkmovdetectpreprocess(src,LKRramegray,0);
				m_pMovDetector->setFrame(LKRramegray,LKRramegray.cols,LKRramegray.rows,0,10,MINAREDETECT,200000,30);
				#else
				lkmove.lkmovdetect(src,0);

				#endif
				//imshow("LKRramegray",LKRramegray);
				//waitKey(1);
				//lkmove.lkmovdetect(src,0);
				//printf("the  ");
			}
			else
				m_pMovDetector->setFrame(src,src.cols,src.rows,0,10,MINAREDETECT,200000,40);
			return ;
		}
/*
	if(PANOGRAYDETECT)
		{
			process=panoblock[blocknum];
			
			//lkmove.lkmovdetect(process,0);
			
			videowriter[blocknum]<<process;
			//OSA_printf("the mov write ok\n");
			//imshow("mov",panoblock[blocknum]);
			//waitKey(1);
			return ;
		}
		*/
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
					//sprintf(recoardnum,"%d.bmp",blocknum);
					//imwrite(recoardnum,process);
					//imshow(recoardnum,process);
					//waitKey(1);
					//if(blocknum==0||blocknum==1)
						{
						//cout<<"************panomoveprocess*************"<<endl;

						
						if(lkmove.backgroundmov[blocknum]==0)
							{
								//setwarndetect(process.cols,process.rows,blocknum);
								setmvprocessangle(LKprocessangle[blocknum],blocknum);
							}

						//if(blocknum==0)
						if(newmat)
							{
							lkmove.lkmovdetectpreprocess(process,LKRramegray,blocknum);
							//printf("******newmatblocknum=%d prenum=%d\n******",blocknum,prenum);
							
							}
						else
							{
								printf("******blocknum=%d prenum=%d\n******",blocknum,prenum);
							}
						//lkmove.lkmovdetect(process,blocknum);

						//cout<<"blackrect"<<blackrect<<endl;
						if(getmodeling())
							{
							
								int num=getmodelnum(blocknum);
								if(num<MODELINGNUM&&newmat)
									{
										premodelnum=num;
										//printf(" newmat proceess ****************num=%d\n",premodelnum);
										LKRramegray(blackrect).copyTo(Modelframe[blocknum][premodelnum](blackrect));
										//lkmove.lkmovdetectpreprocess(process,Modelframe[blocknum][num],blocknum);
										setmodelnum(blocknum);
										
									}
							}
						else	
							{
								//printf(" newmat proceess ****************num=%d\n",premodelnum);
								LKRramegray(blackrect).copyTo(LKRramegrayblackboard(blackrect));
							}

						//int num=getmodelnum(blocknum);
						if(getmodeling()==0)
						videowriter[blocknum]<<LKRramegrayblackboard;
						else
							videowriter[blocknum]<<Modelframe[blocknum][premodelnum];
						/*
						if(blocknum==1)
							{
								    cv::Mat img_mask;
								    cv::Mat img_bkgmodel;
								    bgs->process(LKRramegrayblackboard, img_mask, img_bkgmodel); // by default, it shows automatically the foreground mask image

							}
							waitKey(1);
						*/
						
						//m_pMovDetector->setFrame(LKRramegray,LKRramegray.cols,LKRramegray.rows,0,10,MINAREDETECT,200000,40);
						
						
						//printf("modeling=%d newmat=%d num=%d\n",blocknum,newmat,premodelnum);


						
						if(getmodeling())
							{
								m_pMovDetector->setFrame(Modelframe[blocknum][premodelnum],Modelframe[blocknum][premodelnum].cols,Modelframe[blocknum][premodelnum].rows,blocknum,10,MINAREDETECT,200000,50);
							}
						else
							m_pMovDetector->setFrame(LKRramegrayblackboard,LKRramegrayblackboard.cols,LKRramegrayblackboard.rows,blocknum,10,MINAREDETECT,2000000,50);
						
				}
				}
			return;
		}
	//videocapture
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
			//videowriter<<process;
			//videowriter.write(process);
			//videowriter<<process;
			//waitKey(1);
			//OSA_printf("***********video write***************\n");
			//VideoWriter
			//imwrite("blockpano.bmp",panoblock[0]);
			m_pMovDetector->setFrame(process,process.cols,process.rows,0,10,MINAREDETECT,200000,30);


		}

#else
	imwrite("pano.bmp",pano360);

	
		
		//m_pMovDetector->setFrame(frame_gray,frame_gray.cols,frame_gray.rows,0,2,8,200,30);

#endif
		

}
void ImageProcess::getnumofpano360image(int startx,int endx,int *texturestart,int *textureend)
{
	
	unsigned int pan360w=0;
	unsigned int pan360h=0;

	getpanoparam(&pan360w,&pan360h);

	int num=ceil(1.0*pan360w/Config::getinstance()->getmvprocesswidth());
	*texturestart=(startx/Config::getinstance()->getmvprocesswidth())%num;
	if(endx%Config::getinstance()->getmvprocesswidth()==0)
	  *textureend=*texturestart;
	else
	*textureend=(endx/Config::getinstance()->getmvprocesswidth())%num;
	//if(*texturestart>=num)

}

int ImageProcess::JudgeLk(Mat src)
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

int ImageProcess::JudgeLkFast(Mat src)
{
	int ret=-1;
	double anglepos[MOVELKBLOCKNUM];
	memset(anglepos,0,sizeof(anglepos));
	double angle=getcurrentcapangle();
	//printf("the cap angle =%f\n",angle);
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
void ImageProcess::MulticpuLKpanoprocess(Mat& src)
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

	if(FASTMODE)
		blucknum=JudgeLkFast(processsrc);
	else
		blucknum=JudgeLk(processsrc);
	if(blucknum!=-1)
		{
			
			
			if(movblocknumpre!=blucknum)
				{
					//OSA_printf("the movblocknumpre=%d\n",movblocknumpre);
					movblocknum=movblocknumpre;
					movblocknumpre=blucknum;
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


void ImageProcess::Multicpupanoprocess(Mat& src)
{
	int startnum=0;
	int endnum=0;
	int multipanx=0;
	int multipany=0;
	int multipanw=0;
	int multipanh=0;
	//static int singalstartnum=0;
	unsigned int pan360w=0;
	unsigned int pan360h=0;

	getpanoparam(&pan360w,&pan360h);
	double exec_time = (double)getTickCount();
		Mat processsrc;
	if(PANOGRAYDETECT)
		{
		cvtColor(src,panograysrc,CV_BGR2GRAY);
		processsrc=panograysrc;
		}
	else
		processsrc=src;

	double angle=getcurrentangle();
	int panx=getpanooffet(angle);
	int pany=0;
	int panw=processsrc.cols-Config::getinstance()->getpanoprocessshift();
	int panh=processsrc.rows;
	Rect panorio(panx,pany,panw,panh);
	Rect srcrio(0,0,processsrc.cols-Config::getinstance()->getpanoprocessshift(),processsrc.rows);

	
	
	getnumofpano360image(panx,panx+panw,&startnum,&endnum);

	//cout<<"startnum"<<startnum<<"**endnum"<<endnum<<endl;

	if(startnum==MULTICPUPANONUM-1&&panx+panw>pan360w)
		{
			;
		}
	else if(startnum==endnum)
	{
		multipanx=panx-startnum*Config::getinstance()->getmvprocesswidth();
		multipany=pany;
		multipanw=panw;
		multipanh=panh;
		panorio=Rect(multipanx,multipany,multipanw,multipanh);
		processsrc(srcrio).copyTo(panoblock[startnum](panorio));

	}
	else
	{
		multipanx=panx-startnum*Config::getinstance()->getmvprocesswidth();
		multipany=pany;
		multipanw=Config::getinstance()->getmvprocesswidth()-multipanx;
		multipanh=panh;
		//Rect srcrio(0,0,multipanw,processsrc.rows);
		srcrio=Rect(0,0,multipanw,processsrc.rows);
		panorio=Rect(multipanx,multipany,multipanw,multipanh);
		//cout<<"panorio"<<panorio<<endl;
		//cout<<"srcrio"<<srcrio<<endl;
		processsrc(srcrio).copyTo(panoblock[startnum](panorio));


		multipanx=0;
		multipanw=panw-multipanw;
		srcrio=Rect(panw-multipanw,0,multipanw,processsrc.rows);
		panorio=Rect(0,multipany,multipanw,multipanh);
		//cout<<"2panorio"<<panorio<<endl;
		//cout<<"2srcrio"<<srcrio<<endl;
		processsrc(srcrio).copyTo(panoblock[endnum](panorio));


	}
	//if(abs(blocknum-startnum)==2)
	if(blocknum!=startnum)
		{
			movblocknum=blocknum;
			blocknum=startnum;
			OSA_semSignal(&mainProcThrdetectObj.procNotifySem);
			//OSA_printf("*********detect*OSA_semSignal*movblocknum=%d*startnum=%d**angle=%f****************\n",movblocknum,startnum,angle);


		}



	
	
	


	exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
     //  OSA_printf("the %s exec_time=%f\n",__func__,exec_time);
}
void ImageProcess::cpupanoprocess(Mat& src)
{
	double exec_time = (double)getTickCount();
	Mat processsrc;
	if(PANOGRAYDETECT)
		{
			cvtColor(src,panograysrc,CV_BGR2GRAY);
			processsrc=panograysrc;
		}
	else
		processsrc=src;
	static unsigned int cpupanoonce=0;

	Mat cpupano=getpanomap();
	double angle=getcurrentangle();
	int panx=getpanooffet(angle);
	int pany=PANOEXTRAH/2;
	int panw=processsrc.cols-Config::getinstance()->getpanoprocessshift();
	int panh=processsrc.rows;
	Rect panorio(panx,pany,panw,panh);
	

	
		
		
		
	Rect srcrio(0,0,processsrc.cols-Config::getinstance()->getpanoprocessshift(),processsrc.rows);
	unsigned int pan360w=0;
	unsigned int pan360h=0;

	getpanoparam(&pan360w,&pan360h);

	//OSA_printf("**********%s begin********************\n",__func__);

	
	//OSA_printf("");
	//cout<<"panorio"<<panorio<<endl;
	//cout<<"srcrio"<<srcrio<<endl;
	if(panx+panw>pan360w)
		{
			if(cpupanoonce==0)
				{
					setnextnum();
					cpupanoonce=1;
					if(DETECTTEST==0)
					OSA_semSignal(&mainProcThrdetectObj.procNotifySem);
					OSA_printf("*********detect*OSA_semSignal********************\n");
				}

		}
	else
		{
			cpupanoonce=0;
		      processsrc(srcrio).copyTo(cpupano(panorio));
		}

	//OSA_printf("**********%s end********************\n",__func__);
	//
	 exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
     //  OSA_printf("the %s exec_time=%f\n",__func__,exec_time);
	


}

int ImageProcess::stichenable(OSA_BufInfo* info)
{
	int ret=0;
	if(Config::getinstance()->getpanocalibration()==0)
		return 1;
	//OSA_printf("%s:%d.\n",__func__,__LINE__);
	if(Panorest())
		return ret;
	//OSA_printf("%s:%d.\n",__func__,__LINE__);
	if(getscanpanflag()==0)
		return ret;
	//OSA_printf("%s:%d.\n",__func__,__LINE__);
	if(info->calibration==0)
		return ret;
	//OSA_printf("%s:%d.\n",__func__,__LINE__);
	
	if(StichAlg::getinstance()->getzeroflameupdate()==0)
	if(abs(info->framegyroyaw*1.0/ANGLESCALE-AngleStich)<Config::getinstance()->getangleinterval()&&(StichAlg::getinstance()->getzerocalibing()==0)&&(getpanoflagenable()==1))
		{
			//OSA_printf("%s:%d.\n",__func__,__LINE__);
			return ret;
		}
	//OSA_printf("%s:%d.getzerocalibing=%d  getpanoflagenable=%d\n",__func__,__LINE__,getzerocalibing(),getpanoflagenable());
	AngleStich=info->framegyroyaw*1.0/ANGLESCALE;
	ret=1;
	return ret;
}




void ImageProcess::main_proc_func()
{
	OSA_printf("%s: Main Proc Tsk Is Entering...\n",__func__);
	unsigned int framecount=0;
	OSA_BufInfo* info=NULL;
	OSA_BufInfo* infocap=NULL;
	OSA_BufInfo *outputif=NULL;
	int queueid=0;
	Mat pre;
	int xoffset,yoffset;
	double imageangle=0;

	Queue *queuebuf;
	while(mainProcThrObj.exitProcThread ==  false)
	{
		
		infocap=image_queue_getFullforever(&mcap_bufQue[queueid]);
		if(infocap==NULL)
				continue;
		Mat src1=Mat(infocap->height,infocap->width,CV_8UC3,infocap->virtAddr);
		//OSA_printf("******begin*angle=%f*********************\n",infocap->framegyroyaw*1.0/ANGLESCALE);
		queuebuf=Queue::getinstance();

		if(DetectAlg::getinstance()->getmtdstat())
		{
			DetectAlg::getinstance()->detectprocess(src1,infocap);
		}
		if(stichenable(infocap))
			{
				outputif=(OSA_BufInfo *)queuebuf->getempty(Queue::TOPANOSTICH,0,OSA_TIMEOUT_NONE);
				//OSA_printf("******process*angle=%f*****outputif=%p****************\n",infocap->framegyroyaw*1.0/ANGLESCALE,outputif);
			}
		else
			outputif=NULL;
		
		if(outputif==NULL)
			{
				 image_queue_putEmpty(&mcap_bufQue[queueid],infocap);
				 continue;
			}
		
		Mat dst1=Mat(Config::getinstance()->getpanoprocessheight(),Config::getinstance()->getpanoprocesswidth(),CV_8UC3,outputif->virtAddr);


		
		outputif->channels = infocap->channels;
		outputif->width =Config::getinstance()->getpanoprocesswidth();
		outputif->height =Config::getinstance()->getpanoprocessheight();
		outputif->timestamp =infocap->timestamp;
		outputif->framegyroroll=infocap->framegyroroll;
		outputif->framegyropitch=infocap->framegyropitch;
		outputif->framegyroyaw=infocap->framegyroyaw;

		memcpy(dst1.data,src1.data,Config::getinstance()->getpanoprocessheight()*Config::getinstance()->getpanoprocesswidth()*3);

		//OSA_printf("******end*angle=%f*********************\n",outputif->framegyroyaw*1.0/ANGLESCALE);
		//imshow("dst",dst1);
		//waitKey(1);
		
		 queuebuf->putfull(Queue::TOPANOSTICH,0,outputif);
		 image_queue_putEmpty(&mcap_bufQue[queueid],infocap);
	}
}



void ImageProcess::main_detect_func()
{
	OSA_printf("%s: Main Proc Tsk Is Entering...\n",__func__);
	unsigned int framecount=0;
	OSA_BufInfo* info=NULL;
	OSA_BufInfo* infocap=NULL;
	int queueid=0;
	Mat pre;
	int xoffset,yoffset;
	double imageangle=0;
	while(mainProcThrdetectObj.exitProcThread ==  false)
	{
		OSA_semWait(&mainProcThrdetectObj.procNotifySem, OSA_TIMEOUT_FOREVER);
		//OSA_printf("**********%s***********\n",__func__);
		double exec_time = (double)getTickCount();
		panomoveprocess();
		exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
	 	//OSA_printf("the panomoveprocess=%f MS\n",exec_time);
		
		

	}



}
