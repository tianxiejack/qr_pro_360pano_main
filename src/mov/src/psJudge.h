#ifndef		_PS_JUDGE_H_
#define		_PS_JUDGE_H_

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


int PointAtLineLeftRight(cv::Point ptStart, cv::Point ptEnd, cv::Point ptTest);

bool IsTwoLineIntersect(cv::Point ptLine1Start, cv::Point ptLine1End, cv::Point ptLine2Start, cv::Point ptLine2End);

bool IsLineIntersectRect(cv::Point ptStart, cv::Point ptEnd, cv::Rect rect);

bool	lineInRectJudge(cv::Point ptStart, cv::Point ptEnd, cv::Rect rect);

bool overlapRoi(cv::Rect rec1,	cv::Rect	rec2, cv::Rect &roi);

int _bInRect(cv::Rect rec1,	cv::Rect	rec2,	cv::Rect &roi);

float _bbOverlap(const cv::Rect& box1,const cv::Rect& box2);


#endif