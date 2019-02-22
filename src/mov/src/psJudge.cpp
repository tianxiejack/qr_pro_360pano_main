#include  "psJudge.h"
#include <stdio.h>
#include <stdlib.h>

// �жϵ�������ֱ�ߵ���໹���Ҳ�.
// ����ֵ:-1: �����߶����; 0: �����߶���; 1: �����߶��Ҳ�
int PointAtLineLeftRight(cv::Point ptStart, cv::Point ptEnd, cv::Point ptTest)
{
	ptStart.x -= ptTest.x;
	ptStart.y -= ptTest.y;
	ptEnd.x -= ptTest.x;
	ptEnd.y -= ptTest.y;

	int nRet = ptStart.x * ptEnd.y - ptStart.y * ptEnd.x;
	if (nRet == 0)
		return 0;
	else if (nRet > 0)
		return 1;
	else if (nRet < 0)
		return -1;

	return 0;
}

// �ж������߶��Ƿ��ཻ
bool IsTwoLineIntersect(cv::Point ptLine1Start, cv::Point ptLine1End, cv::Point ptLine2Start, cv::Point ptLine2End)
{
	int nLine1Start = PointAtLineLeftRight(ptLine2Start, ptLine2End, ptLine1Start);
	int nLine1End = PointAtLineLeftRight(ptLine2Start, ptLine2End, ptLine1End);
	if (nLine1Start * nLine1End > 0)
		return false;

	int nLine2Start = PointAtLineLeftRight(ptLine1Start, ptLine1End, ptLine2Start);
	int nLine2End = PointAtLineLeftRight(ptLine1Start, ptLine1End, ptLine2End);

	if (nLine2Start * nLine2End > 0)
		return false;

	return true;
}

// �ж��߶��Ƿ�������ཻ
bool IsLineIntersectRect(cv::Point ptStart, cv::Point ptEnd, cv::Rect rect)
{
	// Two point both are in rect
	std::vector<cv::Point2f>		rectVec;
	cv::Point2f	fStart, fEnd;
	rectVec.resize(4);
	rectVec[0] = cv::Point2f(rect.x, rect.y);
	rectVec[1] = cv::Point2f(rect.x+rect.width, rect.y);
	rectVec[2] = cv::Point2f(rect.x+rect.width, rect.y+rect.height);
	rectVec[3] = cv::Point2f(rect.x, rect.y+rect.height);
	fStart = cv::Point2f(ptStart.x,	ptStart.y);
	fEnd = cv::Point2f(ptEnd.x,	ptEnd.y);

	if(cv::pointPolygonTest( rectVec, fStart, false ) == 1.0 || cv::pointPolygonTest( rectVec, fEnd, false ) == 1.0)
		return true;

	// Two point both aren't in rect
	if (IsTwoLineIntersect(ptStart, ptEnd, cv::Point(rect.x, rect.y), cv::Point(rect.x, rect.y+rect.height)))
		return true;
	if (IsTwoLineIntersect(ptStart, ptEnd, cv::Point(rect.x, rect.y+rect.height), cv::Point(rect.x+rect.width, rect.y+rect.height)))
		return true;
	if (IsTwoLineIntersect(ptStart, ptEnd, cv::Point(rect.x+rect.width, rect.y+rect.height), cv::Point(rect.x+rect.width, rect.y)))
		return true;
	if (IsTwoLineIntersect(ptStart, ptEnd, cv::Point(rect.x, rect.y), cv::Point(rect.x+rect.width, rect.y)))
		return true;

	return false;
}

//A(ax,ay),B(px,py)Ϊ������  ��x1,y1��,(x2,y2)Ϊ���ε����ϽǺ����½�����  ,�ж�A��B�����Ƿ�;����ཻ  

bool	lineInRectJudge(cv::Point ptStart, cv::Point ptEnd, cv::Rect rect)
{
	float ax, ay, px, py, x1, y1, x2, y2;
//#ת��Ϊ�����
	ax =   float(ptStart.x);
	ay =  float(ptStart.y); 
	px =  float(ptEnd.x);
	py = float(ptEnd.y);

	x1 =  float(rect.x);
	y1 = float(rect.y);
	x2 = float(rect.x+rect.width);
	y2  =  float(rect.y+rect.height);

//#�жϾ����ϱ��ߺ�����ֱ���ཻ�ĵ�
	float	sx = (y1 - ay) * (px - ax) / (py - ay) + ax;
	if (sx >= x1 && sx <= x2)
		return true;
//#�жϾ����±��ߺ�����ֱ���ཻ�ĵ�
	float	xx = (y1 - ay) * (px - ax) / (py - ay) + ax;
	if (sx >= x1 && sx <= x2)
	return true;

//#�жϾ�������ߺ�����ֱ���ཻ�ĵ�
	float	zy = (y2 - ay) * (x2 - ax) / (px - ax) + ay;
	if (zy >= y1 && zy <= y2)
		return true;
//#�жϾ����ұ��ߺ�����ֱ���ཻ�ĵ�
	float	yy = (y2 - ay) * (x2 - ax) / (px - ax) + ay;
	if (yy <= y1 && yy >= y2)
		return true;

	return false;
}

bool overlapRoi(cv::Rect rec1,	cv::Rect	rec2, cv::Rect &roi)
{
	cv::Point tl1,tl2;
	cv::Size sz1,sz2;
	tl1	= rec1.tl();
	tl2	= rec2.tl();
	sz1	= rec1.size();
	sz2	= rec2.size();
	int x_tl = std::max(tl1.x, tl2.x);
	int y_tl = std::max(tl1.y, tl2.y);
	int x_br = std::min(tl1.x + sz1.width, tl2.x + sz2.width);
	int y_br = std::min(tl1.y + sz1.height, tl2.y + sz2.height);
	if (x_tl < x_br && y_tl < y_br)
	{
		roi = cv::Rect(x_tl, y_tl, x_br - x_tl, y_br - y_tl);
		return true;
	}
	return false;
}

int _bInRect(cv::Rect rec1,	cv::Rect	rec2,	cv::Rect &roi)
{
	bool rtn	= overlapRoi(rec1, rec2,	 roi);
	if(rtn){
		if(roi.x == rec1.x && roi.y == rec1.y&& roi.width == rec1.width&& roi.height == rec1.height)
			return 1;
		else if(roi.x == rec2.x && roi.y == rec2.y&& roi.width == rec2.width&& roi.height == rec2.height)
			return 2;
		else
			return	0;
	}else{
		return -1;
	}
}

float _bbOverlap(const cv::Rect& box1,const cv::Rect& box2)
{
	if (box1.x > box2.x+box2.width) { return 0.0; }
	if (box1.y > box2.y+box2.height) { return 0.0; }
	if (box1.x+box1.width < box2.x) { return 0.0; }
	if (box1.y+box1.height < box2.y) { return 0.0; }

	float colInt =  std::min(box1.x+box1.width,box2.x+box2.width) - std::max(box1.x, box2.x);
	float rowInt =  std::min(box1.y+box1.height,box2.y+box2.height) - std::max(box1.y,box2.y);

	float intersection = colInt * rowInt;
	float area1 = box1.width*box1.height;
	float area2 = box2.width*box2.height;
	return intersection / (area1 + area2 - intersection);
}