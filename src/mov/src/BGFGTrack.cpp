#include <stdio.h>
#include <stdlib.h>
#include "BGFGTrack.hpp"
#include	"psJudge.h"

CBGFGTracker::CBGFGTracker()
{
	m_pKalmanProc = NULL;
	m_bInited	=FALSE;
	m_pMeasure	=	NULL;
	m_pControl	=	NULL;
	memset(m_warnTarget,	0x00,	sizeof(TRK_RECT_INFO)*SAMPLE_NUMBER);
	memset(m_warnTargetBK,	0x00,	sizeof(TRK_RECT_INFO)*SAMPLE_NUMBER);
	m_thredParam.searchThed	= 0.2;
	m_thredParam.trkThred = 0.2;
	m_thredParam.dispFrames = 15;
	m_thredParam.totalFrames = 100;
	m_thredParam.targetSize	= 200;
	m_thredParam.distRoi = 2.0;

}

CBGFGTracker::~CBGFGTracker()
{
	DeInitCBFG();
}

void	CBGFGTracker::InitCBFG(	int x0, int y0, double DeltaT,int DP, int MP, int CP)
{
	if (m_bInited){
		DeInitCBFG();
	}

	if (m_pKalmanProc == NULL){
		m_pKalmanProc = new CKalman_mv();
		if ( m_pKalmanProc == NULL){
			m_bInited = FALSE;
			return;
		}
	}
	m_pKalmanProc->KalmanOpen(DP, MP, CP);
	if (!m_pKalmanProc->m_bInited){
		m_bInited = FALSE;
		return;
	}
	m_pKalmanProc->KalmanInitParam(x0, y0, DeltaT);

	if (m_pMeasure == NULL){
		m_pMeasure = new double[MP * 1];
		memset(m_pMeasure, 0, sizeof(*m_pMeasure));
	}
	if (m_pControl == NULL && CP > 0){
		m_pControl = new double[CP * 1];
		memset(m_pControl, 0, sizeof(*m_pControl));
	}

	m_bInited = TRUE;
}

void	CBGFGTracker::DeInitCBFG()
{
	if (m_pKalmanProc != NULL)
	{
		delete m_pKalmanProc;
		m_pKalmanProc = NULL;
	}
	if (m_pMeasure != NULL)
	{
		delete m_pMeasure;
		m_pMeasure = NULL;
	}
	if (m_pControl != NULL)
	{
		delete m_pControl;
		m_pControl = NULL;
	}
	m_bInited = FALSE;
}

void CBGFGTracker::KalmanPredict(int xout, int yout)
{
	/* Kalman滤波：更新yk，进行滤波，得到新的xk估计（k时刻）*/
	m_pMeasure[0] = (double)xout;
	m_pMeasure[1] = (double)yout;
	Kalman(m_pMeasure,	NULL);/* 进行Kalman滤波*/
}

void CBGFGTracker::Kalman(double *measure, double *control)
{
	m_pKalmanProc->KalmanPredict(control);
	m_pKalmanProc->KalmanCorrect(measure);
}

void	CBGFGTracker::SetTrkThred(TRK_THRED	 trkThred)
{
	memcpy(&m_thredParam, &trkThred, sizeof(TRK_THRED));
}

void	CBGFGTracker::SetTrkTarget(std::vector<TRK_RECT_INFO>	warnTarget)
{
	int	i,	j,	k;
	int		nTargetNum	=	warnTarget.size();
	TRK_RECT_INFO	*pTrkInfo,	tgtInfo;
	cv::Rect	tgtRect,	trkRect;
	float	foverlap;
	bool	bContinue;

	i = 0;
	for(k=0; k<nTargetNum; k++)
	{
		tgtInfo	=	warnTarget[k];
		tgtRect	= tgtInfo.targetRect;
		if(tgtRect.width*tgtRect.height <m_thredParam.targetSize)//目标太小，不要跟踪
			continue;
		bContinue = false;
		for(j=0;j<SAMPLE_NUMBER;j++)
		{
			pTrkInfo	=	&m_warnTarget[j];
			if(pTrkInfo->trkState	== TRK_STATE_TRACK)
			{
				trkRect	=	pTrkInfo->targetRect;
				foverlap	= _bbOverlap(tgtRect, trkRect);
				if(foverlap > m_thredParam.searchThed/*0.2*/)//找到正在跟踪的重合目标，不需添加新目标
				{
					bContinue = true;
				}
			}
		}
		if(bContinue)
			continue;
		for(;i<SAMPLE_NUMBER;i++)
		{
			pTrkInfo	=	&m_warnTarget[i];
			if(pTrkInfo->trkState	== TRK_STATE_IDLE)
			{
				*pTrkInfo	= tgtInfo;//赋值
				pTrkInfo->trkState	=	TRK_STATE_ACQ;
				break;
			}
		}
		if(i == SAMPLE_NUMBER)
			break;
	}
}

static	void	_trackprocess(Pattern  *curPatterns,	 int	numPatterns,	TRK_RECT_INFO	*pTrkInfo, float trkThred)
{
	int	k;
	cv::Rect	tgtRect,	trkRect	;
	Pattern	*pPat, *pBakPat;
	float	foverlap,	maxoverlap = 0.0;
	trkRect= pTrkInfo->targetRect;

	for(k=0; k<numPatterns;	k++)
	{
		pPat = &curPatterns[k];
		if(!pPat->bValid)
			continue;
		tgtRect	=	cv::Rect(pPat->lefttop.x,	pPat->lefttop.y, pPat->rightbottom.x-pPat->lefttop.x,	pPat->rightbottom.y-pPat->lefttop.y);
		foverlap	= _bbOverlap(tgtRect, trkRect);
		if(foverlap > maxoverlap)
		{
			maxoverlap	= foverlap;
			pBakPat = pPat;
		}
	}
	if(maxoverlap	> trkThred/*0.2*/)//表示跟踪到了目标
	{
		pBakPat->bValid	= false;
		pTrkInfo->targetRect	= cv::Rect(pBakPat->lefttop.x,	pBakPat->lefttop.y, pBakPat->rightbottom.x-pBakPat->lefttop.x,	pBakPat->rightbottom.y-pBakPat->lefttop.y);
		pTrkInfo->trk_frames++;
	}
	else//没有跟踪到目标
	{
		pTrkInfo->trk_frames++;//等待处理
		pTrkInfo->trkState	= TRK_STATE_IDLE;
	}
}

void	CBGFGTracker::TrackProcess(Pattern  *curPatterns,	 int	numPatterns)
{
	int	i,	k;
	TRK_RECT_INFO	*pTrkInfo;
	CV_Assert(numPatterns<SAMPLE_NUMBER);
	for(i=0;i<numPatterns;	i++)
	{
		curPatterns[i].bValid	= true;
	}
	for(i=0;	i<SAMPLE_NUMBER;	i++)
	{
		pTrkInfo	= &m_warnTarget[i];
		if(pTrkInfo->trkState	==TRK_STATE_ACQ )
		{
			pTrkInfo->trkState	= TRK_STATE_TRACK;
			pTrkInfo->disp_frames	= 0;
		}
		else	if (pTrkInfo->trkState	==TRK_STATE_TRACK)
		{
			_trackprocess(curPatterns,	numPatterns,	pTrkInfo, m_thredParam.trkThred);
		}
	}
}

void	CBGFGTracker::ClearTrkTarget(int	Idx)
{
	TRK_RECT_INFO	*pTrkInfo  = &m_warnTarget[Idx];
	pTrkInfo->trkState	= TRK_STATE_IDLE;
	pTrkInfo->warnType	= WARN_STATE_IDLE;
	pTrkInfo->trk_frames = 0;
	pTrkInfo->disp_frames	= 0;
}

int	CBGFGTracker::TrackAnalyse(std::vector<cv::Point2i>		warnRoi)
{
	int	i,	k;
	int	dispFrames, totalFrames;
	float distRoi;
	cv::Scalar	color;
	std::vector<cv::Point2f>		polyRoi;
	cv::Point2f	rc_center;
	TRK_RECT_INFO	*pTrkInfo;
	int	warnState = WARN_STATE_IDLE;

	int	nsize =	warnRoi.size();
	CV_Assert(nsize	>2);
	polyRoi.resize(nsize);
	for(i=0; i<nsize; i++){
		polyRoi[i]	= cv::Point2f((float)warnRoi[i].x, (float)warnRoi[i].y);
	}
	dispFrames = m_thredParam.dispFrames;
	totalFrames = m_thredParam.totalFrames;
	distRoi	= m_thredParam.distRoi;

	for(k=0; k<SAMPLE_NUMBER;	k++){
		pTrkInfo	= &m_warnTarget[k];
		color	= cv::Scalar(0x00,0x00,0xFF);//red color
		if(pTrkInfo->trkState	== TRK_STATE_TRACK){

			int	nTrkFrames	= pTrkInfo->trk_frames;
			cv::Rect	targetRec	= pTrkInfo->targetRect;
			rc_center	= cv::Point2f((float)(targetRec.x+targetRec.width/2), (float)(targetRec.y+targetRec.height/2));

			double	distance	= cv::pointPolygonTest( polyRoi, rc_center, true );
			double	tgw	= targetRec.width;
			double	tgh	=  targetRec.height;
			double	diagd	 = sqrt(tgw*tgw+tgh*tgh);
			if(pTrkInfo->targetType	== TARGET_IN_POLYGON){
				if(distance<0.0	&& pTrkInfo->disp_frames == 0){//丢失
					pTrkInfo->warnType	=	WARN_STATE_LOST;
					pTrkInfo->disp_frames	=  pTrkInfo->disp_frames == 0?dispFrames:pTrkInfo->disp_frames;
					warnState	|=	WARN_STATE_LOST;
				}
			}else if(pTrkInfo->targetType	== TARGET_OUT_POLYGON){
				if(distance>0.0 && pTrkInfo->disp_frames == 0){//入侵
					pTrkInfo->warnType	=	WARN_STATE_INVADE;
					pTrkInfo->disp_frames	=  pTrkInfo->disp_frames == 0?dispFrames:pTrkInfo->disp_frames;
					warnState	|=	WARN_STATE_INVADE;
				}
			}else if(pTrkInfo->targetType	== TARGET_IN_EDGE){
				if(pTrkInfo->distance>0.0 && distance<0.0 && pTrkInfo->disp_frames == 0){//丢失
					pTrkInfo->warnType	=	WARN_STATE_LOST;
					pTrkInfo->disp_frames	=  pTrkInfo->disp_frames == 0?dispFrames:pTrkInfo->disp_frames;
					warnState	|=	WARN_STATE_LOST;
				}else	if(pTrkInfo->distance<0.0 && distance>0.0 && pTrkInfo->disp_frames == 0){//入侵
					pTrkInfo->warnType	=	WARN_STATE_INVADE;
					pTrkInfo->disp_frames	=  pTrkInfo->disp_frames == 0?dispFrames:pTrkInfo->disp_frames;	
					warnState	|=	WARN_STATE_INVADE;
				}
			}
			if(pTrkInfo->disp_frames >	0){
				pTrkInfo->disp_frames--;
				if(pTrkInfo->disp_frames	== 0)
					ClearTrkTarget(k);
			}
			if(nTrkFrames	> totalFrames){//清除跟踪
				ClearTrkTarget(k);
			}
			if(fabs(distance) >diagd*distRoi){//离警戒区超2倍对角线
				ClearTrkTarget(k);
			}
		}
	}
	return warnState;
}

void	CBGFGTracker::GetTrackTarget(std::vector<TRK_RECT_INFO> &lostTarget, std::vector<TRK_RECT_INFO> &invadeTarget, std::vector<TRK_RECT_INFO> &warnTarget)
{
	lostTarget.clear();
	invadeTarget.clear();
	warnTarget.clear();

	int	k;
	TRK_RECT_INFO	*pTrkInfo;

	for(k=0; k<SAMPLE_NUMBER;	k++)
	{
		pTrkInfo	= &m_warnTarget[k];
		pTrkInfo->index = k;
		if(pTrkInfo->trkState	== TRK_STATE_TRACK)
		{
			if(pTrkInfo->warnType	==	WARN_STATE_LOST)
			{
				lostTarget.push_back(*pTrkInfo);
			}
			else if(pTrkInfo->warnType	==	WARN_STATE_INVADE)
			{
				invadeTarget.push_back(*pTrkInfo);
			}
			warnTarget.push_back(*pTrkInfo);
		}
	}
}

static void _drawWarn(cv::Mat frame, TRK_RECT_INFO *warnTarget, bool bshow)
{
	int	k;
	cv::Scalar	color;
	TRK_RECT_INFO	*pTrkInfo;
	unsigned char fcolor = bshow?0xFF:0x00;
	for(k=0; k<SAMPLE_NUMBER;	k++)
	{
		pTrkInfo	= &warnTarget[k];
		color	= cv::Scalar(0x00,0x00,0xFF, fcolor);//red color
		if(pTrkInfo->trkState	== TRK_STATE_TRACK)
		{
			cv::Rect	targetRec	= pTrkInfo->targetRect;
			if(pTrkInfo->warnType	==	WARN_STATE_LOST)
				color	= cv::Scalar(0xFF,0x00,0x00,fcolor);//blue color
			else	if(pTrkInfo->warnType	==	WARN_STATE_INVADE)
				color	= cv::Scalar(0x00,0xFF,0x00,fcolor);//green color
			
			cv::Rect result = targetRec;
			char strDisplay[128];
			rectangle( frame, cv::Point( result.x, result.y ), cv::Point( result.x+result.width, result.y+result.height), color, 4, 8 );
			sprintf(strDisplay, "%d ", k) ;
			putText(frame, strDisplay,cv::Point( result.x+4, result.y+4 ),1,2,cv::Scalar(25,200,25,fcolor));
		}
	}
}

void	CBGFGTracker::DrawWarnTarget(cv::Mat	frame)
{
	_drawWarn(frame, m_warnTargetBK, false);
	_drawWarn(frame, m_warnTarget, true);
	memcpy(m_warnTargetBK, m_warnTarget, sizeof(TRK_RECT_INFO)*SAMPLE_NUMBER);
}
