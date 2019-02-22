/*
 * Homography.hpp
 *
 *  Created on: 2018年9月20日
 *      Author: wj
 */

#ifndef HOMOGRAPHY_HPP_
#define HOMOGRAPHY_HPP_

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
using namespace cv;
using namespace std;
void FindHomographyfromPicture(cv::Mat& src,cv::Mat & dst,cv::Mat &H);
int  getPano360Rotation(cv::Mat & src,cv::Mat & dst,double *rotation );
int  getPano360Offset(cv::Mat & src,cv::Mat & dst,int *xoffset ,int* yoffset);
int  getPano360OffsetT(cv::Mat & src,cv::Mat & dst,int *xoffset ,int* yoffset) ;
cv::Mat  FindHomography(cv::Mat& src,cv::Mat & dst);

int find_feature_matches ( const Mat& img_1, const Mat& img_2,
                            std::vector<KeyPoint>& keypoints_1,
                            std::vector<KeyPoint>& keypoints_2,
                            std::vector< DMatch >& matches );

#endif /* HOMOGRAPHY_HPP_ */
