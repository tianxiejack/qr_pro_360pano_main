#ifndef		_BG_FG_TRACK_H_
#define		_BG_FG_TRACK_H_

#include	"Kalman.h"
#include	"infoHead.h"

class CBGFGTracker
{
public:
	CBGFGTracker();
	virtual	~CBGFGTracker();

	void	InitCBFG(	int x0, int y0, double DeltaT,int DP, int MP, int CP);
	void	DeInitCBFG();

	void	Kalman(double *measure, double *control);
	void	KalmanPredict(int xout, int yout);

	void	SetTrkTarget(std::vector<TRK_RECT_INFO>	warnTarget);
	void	TrackProcess(Pattern  *curPatterns,	 int	numPatterns);
	int		TrackAnalyse(std::vector<cv::Point2i>	warnRoi);
	void	GetTrackTarget(std::vector<TRK_RECT_INFO> &lostTarget, std::vector<TRK_RECT_INFO> &invadeTarget, std::vector<TRK_RECT_INFO> &warnTarget);
	void	ClearTrkTarget(int	Idx);

	void	SetTrkThred(TRK_THRED	 trkThred);
	void	DrawWarnTarget(cv::Mat	frame);

public:

	TRK_RECT_INFO		m_warnTarget[SAMPLE_NUMBER];
	TRK_RECT_INFO		m_warnTargetBK[SAMPLE_NUMBER];

	CKalman_mv* m_pKalmanProc;
	double*  m_pMeasure;
	double*  m_pControl;

protected:
	TRK_THRED		m_thredParam;

private:

	BOOL		m_bInited;
};

#endif
