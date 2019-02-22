/* optical_flow_calculator.cpp
 *
 * Copyright (C) 2014 Santosh Thoduka
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <optical_flow_calculator.h>
#include <iostream>
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <VarFlow.h>
#include <slic.h>
#include <fstream>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
 #include"Homography.hpp"
 #include"config.h"
using namespace cv;
OpticalFlowCalculator::OpticalFlowCalculator()
{

}

OpticalFlowCalculator::~OpticalFlowCalculator()
{

}

int  OpticalFlowCalculator::getOffsetT(const cv::Mat & src, const cv::Mat & dst,int *xoffset ,int* yoffset) 
	{
		double exec_time = (double)getTickCount();
		 int status=0;
             Mat tempsrc=src;
		static unsigned int tempcount=0;
		static unsigned int tempcounterror=0;
		char bufname[50];   
             // resize(src,tempsrc,Size(1920,1080),0,0,INTER_LINEAR);
             Mat tempdst=dst;
		Rect temprect;
		temprect=Rect(tempsrc.cols*0.5-0.3*tempsrc.cols,0.5*tempsrc.rows-0.45*tempsrc.rows,0.6*tempsrc.cols, 0.9*tempsrc.rows);
		Mat templ(tempsrc, temprect); 
		Mat result(tempdst.cols - templ.cols + 1, tempdst.rows - templ.rows + 1, CV_8UC1);
		matchTemplate(tempdst, templ, result, CV_TM_CCOEFF_NORMED); 
		double minVal; double maxVal; Point minLoc; Point maxLoc;
		Point matchLoc; 
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat()); 
		//if(FEATURETEST==0)
		sprintf(bufname,"/home/ubuntu/calib/lk%d.bmp",tempcount);
		tempcounterror++;
		if(maxVal<0.85)
			{
				//sprintf(bufname,"/home/ubuntu/calib/error%d.bmp",tempcount);
				//imwrite(bufname,tempdst);
			return -1;
			}
		//printf("the %s  maxVal=%f  minVal=%f\n",__func__,maxVal,minVal);
		normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat()); 
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat()); 
		matchLoc = maxLoc;
		int dx = matchLoc.x-temprect.x; 
		int dy = matchLoc.y - temprect.y;
	
		*xoffset=dx;
		*yoffset=dy;
		//rectangle(tempdst, Rect(matchLoc.x,matchLoc.y,temprect.width,temprect.height), Scalar(255,0,0),1,  8);
		//imwrite(bufname,tempdst);
		tempcount++;
		 exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
		// printf("the %s exec_time=%f  maxVal=%f  minVal=%f\n",__func__,exec_time,maxVal,minVal);
	//Point2i a(dx, dy); 
	return status; 
	}
int OpticalFlowCalculator::clibrationformshift(const cv::Mat & src,const cv::Mat & dst,int xoffset ,int yoffset)
{
	 Rect rectsrc;
	   Rect rectdst;
	   if(xoffset>=0)
		{
			rectsrc.x=xoffset;
			rectsrc.width=src.cols-xoffset;

			rectdst.x=0;
			rectdst.width=src.cols-xoffset;



		}
	   else
		{
			rectsrc.x=0;
			rectsrc.width=src.cols+xoffset;

			rectdst.x=-xoffset;
			rectdst.width=src.cols+xoffset;



		}


		 if(yoffset>=0)
		{
			rectsrc.y=yoffset;
			rectsrc.height=src.rows-yoffset;

			rectdst.y=0;
			rectdst.height=src.rows-yoffset;



		}
	   else
		{
			rectsrc.y=0;
			rectsrc.height=src.rows+yoffset;

			rectdst.y=-yoffset;
			rectdst.height=src.rows+yoffset;

		}

	
		  src(rectsrc).copyTo(dst(rectdst));
	 



}

int OpticalFlowCalculator::calculateOpticalFlowprocess(const cv::Mat &gray_image1, const cv::Mat &gray_image2, cv::Mat &dst, int pixel_step, double min_vector_size)
{
    /*
    const int MAX_LEVEL = 2;
    cv::Size winSize(40, 40);
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.03);
    */

    const int MAX_LEVEL = 4;
    cv::Size winSize(30, 30);
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.03);

    static int pointnum=0;
    int tempnum=0;
    static int mathcount=0;
    char matchname[20];

    const int MAX_COUNT = 500;
    Size subPixWinSize(10,10);
    
    std::vector<cv::Point2f> points_image2;

   int xoffset=0;
   int yoffset=0;
   double percent=0.6;
   int fixoffset=1;
   int trackok=getOffsetT(gray_image1,gray_image2,&xoffset,&yoffset);
   int featureok=0;
   int xbaseoffet=0;
   int ybaseoffet=0;
   double upx=xoffset+fixoffset*percent+xbaseoffet;
   double downx=xoffset-fixoffset*percent-xbaseoffet;
   double upy=yoffset+fixoffset*percent+ybaseoffet;
   double downy=yoffset-fixoffset*percent-ybaseoffet;

   double featurexoffet=0;
   double featureyoffet=0;

  // printf("the xoffset=%d yoffset=%d [%d~%d] [%d~%d]\n",xoffset,yoffset,downx,upx,downy,upy);

    static int index=0;

  #if 1
    if(index==0)
	{
		index++;
		    for (int i = 0; i < gray_image1.cols; i = i + pixel_step)
		    {
		        for (int j = 0; j < gray_image1.rows; j = j + pixel_step)
		        {
		            cv::Point2f point(i, j);
		            //std::cout << "adding point " << i << ", " << j << std::endl;
		            points_image1.push_back(point);
			      pointnum++;
		        }
		    }
	}


	#else
	if(index==0)
	{
		index++;
		goodFeaturesToTrack(gray_image1, points_image1, MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
	      cornerSubPix(gray_image1, points_image1, subPixWinSize, Size(-1,-1), termcrit);
	 }
	

	#endif

    std::vector<cv::Mat> pyramids;
    int num_vectors = 0;
   //  if (MVDETECTALG)
   if(0)
     {
#if 1
		cv::buildOpticalFlowPyramid(gray_image1, pyramids, winSize, MAX_LEVEL, true);
		//cv::calcOpticalFlowPyrLK(gray_image1, gray_image2, points_image1, points_image2, status, err, winSize, MAX_LEVEL, termcrit, 0, 0.001);
		cv::calcOpticalFlowPyrLK(pyramids, gray_image2, points_image1, points_image2, status, err, winSize, MAX_LEVEL, termcrit, 0, 0.001);
		std::vector<cv::Point2f> src_points;
		std::vector<cv::Point2f> dst_points;
#else

		vector<KeyPoint> keypoints_1, keypoints_2;
		vector<DMatch> matches;
		featureok= find_feature_matches ( gray_image1, gray_image2, keypoints_1, keypoints_2, matches );

		points_image1.clear();
		points_image2.clear();
		for ( int i = 0; i < ( int ) matches.size(); i++ )
		{
		    points_image1.push_back ( keypoints_1[matches[i].queryIdx].pt );
		    points_image2.push_back ( keypoints_2[matches[i].trainIdx].pt );
		    featurexoffet+=keypoints_2[matches[i].trainIdx].pt.x- keypoints_1[matches[i].queryIdx].pt.x;
		    featureyoffet+=keypoints_2[matches[i].trainIdx].pt.y- keypoints_1[matches[i].queryIdx].pt.y;
		}
		featurexoffet=featurexoffet/matches.size();
		featureyoffet=featureyoffet/matches.size();
		
		/*
		Mat first_match;
		drawMatches(gray_image1, keypoints_1, gray_image2, keypoints_2, matches, first_match);
		sprintf(matchname,"%d.jpg",mathcount);
		//imshow("first_match ", first_match);
		imwrite(matchname,first_match);
		mathcount++;
		//waitKey(1);
		*/

		std::vector<cv::Point2f> src_points;
		std::vector<cv::Point2f> dst_points;
#endif

	 //   printf("points_image2.size()=%d\n",points_image2.size());

	
		for (int i = 0; i < points_image2.size(); i++)
		{
			if (status[i])
			//if(1)
			{

			//printf("the opt ok\n");
			cv::Point2f start_point = points_image1.at(i);
			cv::Point2f end_point = points_image2.at(i);
			float x_diff = end_point.x - start_point.x;
			float y_diff = end_point.y - start_point.y;
			//printf("the opt x_diff=%f y_diff=%f\n",x_diff,y_diff);
			 double backward_angle = atan2(start_point.y - end_point.y, start_point.x - end_point.x)*180/3.141592653;
			//printf("x_diff=%f y_diff=%f upx=%f,downx=%f,upy=%f,downy=%f \n",x_diff,y_diff,upx,downx,upy,downy);

			
			if(featureok==0&&trackok==0)
				{
					if((x_diff<upx)&&(x_diff>downx)&&(y_diff<upy)&&(y_diff>downy))
						tempnum++;
					else
						continue;
				}
			else
				break;
				if (((std::abs(x_diff) > min_vector_size || std::abs(y_diff) > min_vector_size))&&(abs(backward_angle)<1)) // THIS USED TO BE 1.0
		   //  if (((std::abs(x_diff) > min_vector_size || std::abs(y_diff) > min_vector_size))) // THIS USED TO BE 1.0
				{
					src_points.push_back(start_point);
					dst_points.push_back(end_point);
					num_vectors++;
				}
			//printf("x_diff=%f y_diff=%f backward_angle=%f \n",x_diff,y_diff,backward_angle);

			}

		}
		//printf("featureok=%d num_vectors=%d tempnum=%d \n",featureok,num_vectors,tempnum);
		if (num_vectors > 50)
		{
			vector<uchar> inliersMask(src_points.size());
   			 cv::Mat H = findHomography(dst_points, src_points, CV_RANSAC, 3, inliersMask);
			//cv::Mat H = findHomography(dst_points, src_points, CV_RANSAC);
			//cv::Mat compensated;
			#if 1
			warpPerspective(gray_image2, dst, H, cv::Size(gray_image2.cols, gray_image2.rows));
			#else
			double xshift=0;
			double yshift=0;
			int count=0;
			for(int j=0;j<src_points.size();j++)
				{
					if(inliersMask[j])
						{
							xshift+=dst_points[j].x-src_points[j].x;
							yshift+=dst_points[j].y-src_points[j].y;
							count++;
						}

				}
			xshift=xshift/count;
			yshift=yshift/count;
			clibrationformshift(gray_image2,dst,(int )xshift,(int )yshift);
			#endif
			//cv::absdiff(compensated, gray_image1, comp);
			//cv::imshow("compensated",compensated);
			//cv::imshow("gray_image2",gray_image1);
			//cv::waitKey(1);
			//cv::threshold(comp, comp, 50, 255, CV_THRESH_BINARY);

		}
	else if(featureok==0&&trackok==0)
		{
			// xoffset=featurexoffet;
			// yoffset=featureyoffet;
			//printf("the *********************1\n");
			 if(trackok==0)
				  clibrationformshift(gray_image2,dst,xoffset,yoffset);
			   else
				gray_image1.copyTo(dst);
			 


		}
	else if(trackok==0)
		{
			printf("the *********************2\n");
			 if(trackok==0)
				  clibrationformshift(gray_image2,dst,xoffset,yoffset);
			   else
				gray_image1.copyTo(dst);

		}
	else
		{
			printf("no feature\n");
			memcpy(dst.data,gray_image1.data,dst.cols*dst.rows*dst.channels());

		}

		//printf("********************tempnum=%d *****************\n",tempnum);
     }
   else
   {
	    if(trackok==0)
		  clibrationformshift(gray_image2,dst,xoffset,yoffset);
	   else
		gray_image1.copyTo(dst);
 }
    
    return num_vectors;
}


int OpticalFlowCalculator::calculatefeature( cv::Mat &gray_image1,  cv::Mat &gray_image2)
{

	Mat H=FindHomography(gray_image1,gray_image2);
	Mat compensated;
	warpPerspective(gray_image2, compensated, H, cv::Size(gray_image2.cols, gray_image2.rows));
	//warpAffine(gray_image2, compensated, H, cv::Size(gray_image1.cols, gray_image1.rows));
	imshow("compensated",compensated);
	imshow("feature",gray_image1);
	waitKey(1);

}
int OpticalFlowCalculator::calculateOpticalFlowgray(const cv::Mat &gray_image1, const cv::Mat &gray_image2, cv::Mat &optical_flow_vectors, int pixel_step,  cv::Mat &comp, double min_vector_size)
{
    /*
    const int MAX_LEVEL = 2;
    cv::Size winSize(40, 40);
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.03);
    */

    const int MAX_LEVEL = 5;
    cv::Size winSize(40, 40);
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.03);

    



    const int MAX_COUNT = 500;
    Size subPixWinSize(10,10);
    
    std::vector<cv::Point2f> points_image2;


#if 0
	 int index=0;
	points_image1.clear();
	if(index==0)
		{
			index++;
			vector<cv::KeyPoint> kps;
  	             cv::FastFeatureDetector detector = cv::FastFeatureDetector();
  	             detector.detect( gray_image1, kps );
  	            // for ( auto kp:kps )
  	            printf("kps.size()=%d\n",kps.size());
  	            for(int i=0;i<kps.size();i++)
  	                points_image1.push_back( kps[i].pt );
			

		}
#elif 1
 int index=0;
points_image1.clear();
if(index==0)
		{
		index++;
		    for (int i = 0; i < gray_image1.cols; i = i + pixel_step)
		    {
		        for (int j = 0; j < gray_image1.rows; j = j + pixel_step)
		        {
		            cv::Point2f point(i, j);
		            //std::cout << "adding point " << i << ", " << j << std::endl;
		            points_image1.push_back(point);
		        }
		    }
}
#else
	points_image1.clear();
	goodFeaturesToTrack(gray_image1, points_image1, MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
      cornerSubPix(gray_image1, points_image1, subPixWinSize, Size(-1,-1), termcrit);


#endif
	

    std::vector<cv::Mat> pyramids;
    cv::buildOpticalFlowPyramid(gray_image1, pyramids, winSize, MAX_LEVEL, true);
    //cv::calcOpticalFlowPyrLK(gray_image1, gray_image2, points_image1, points_image2, status, err, winSize, MAX_LEVEL, termcrit, 0, 0.001);
    cv::calcOpticalFlowPyrLK(pyramids, gray_image2, points_image1, points_image2, status, err, winSize, MAX_LEVEL, termcrit, 0, 0.001);
    int num_vectors = 0;
    std::vector<cv::Point2f> src_points;
    std::vector<cv::Point2f> dst_points;


 //   printf("points_image2.size()=%d\n",points_image2.size());
    for (int i = 0; i < points_image2.size(); i++)
    {
        if (status[i])
        {

		//printf("the opt ok\n");
            cv::Point2f start_point = points_image1.at(i);
            cv::Point2f end_point = points_image2.at(i);
            float x_diff = end_point.x - start_point.x;
            float y_diff = end_point.y - start_point.y;

		 double backward_angle = atan2(start_point.y - end_point.y, start_point.x - end_point.x)*180/3.141592653;
		 printf("the angle =%f \n",backward_angle);
		 //&&(abs(backward_angle)<20)
            if ((std::abs(x_diff) > min_vector_size || std::abs(y_diff) > min_vector_size)&&(abs(backward_angle)<1)) // THIS USED TO BE 1.0
            {
                cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
                elem[0] = start_point.x;
                elem[1] = start_point.y;
                elem[2] = x_diff;
                elem[3] = y_diff;
                //elem[2] = atan2(y_diff, x_diff);
                //elem[3] = sqrt(x_diff*x_diff + y_diff*y_diff);
                src_points.push_back(start_point);
                dst_points.push_back(end_point);
                num_vectors++;
            }
            else
            {
            /*
                 cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
                elem[0] = start_point.x;
                elem[1] = start_point.y;
                elem[2] = 0.0;
                elem[3] = 0.0;
                */
            }
        }
        else
        {
        /*
            cv::Point2f start_point = points_image1.at(i);
            cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
            elem[0] = -1.0;
            elem[1] = -1.0;
            elem[2] = 0.0;
            elem[3] = 0.0;
            */
        }
    }
    if (num_vectors > 100)
    {



	
        //cv::Mat pers_transform = cv::getPerspectiveTransform(&src_points[0], &dst_points[0]);
       // cv::Mat pers_transform = cv::getAffineTransform(&src_points[0], &dst_points[0]);
        cv::Mat H = findHomography(dst_points, src_points, CV_RANSAC);
        cv::Mat compensated;

	
        gray_image1.copyTo(compensated);
        gray_image1.copyTo(comp);

     //  cv::warpPerspective(gray_image1, compensated, pers_transform, cv::Size(gray_image1.cols, gray_image1.rows));
     	warpPerspective(gray_image2, compensated, H, cv::Size(gray_image2.cols, gray_image2.rows));
    //   warpAffine(gray_image1, compensated, pers_transform, cv::Size(gray_image1.cols, gray_image1.rows));
        cv::absdiff(compensated, gray_image1, comp);
        //std::cout << comp << std::endl;
    //   cv::imshow("compensated",compensated);
	// cv::imshow("gray_image2",gray_image1);
	// cv::waitKey(1);;
        cv::threshold(comp, comp, 50, 255, CV_THRESH_BINARY);
	  
    }
   
    
    return num_vectors;
}


int OpticalFlowCalculator::calculateOpticalFlow(const cv::Mat &image1, const cv::Mat &image2, cv::Mat &optical_flow_vectors, int pixel_step, cv::Mat &comp, double min_vector_size)
{
    /*
    const int MAX_LEVEL = 2;
    cv::Size winSize(40, 40);
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.03);
    */

    const int MAX_LEVEL = 5;
    cv::Size winSize(40, 40);
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.03);

    
    cv::Mat gray_image1;
    cv::Mat gray_image2;

    cvtColor(image1, gray_image1, CV_BGR2GRAY);
    cvtColor(image2, gray_image2, CV_BGR2GRAY);

    std::vector<cv::Point2f> points_image1;
    std::vector<cv::Point2f> points_image2;

    for (int i = 0; i < image1.cols; i = i + pixel_step)
    {
        for (int j = 0; j < image1.rows; j = j + pixel_step)
        {
            cv::Point2f point(i, j);
            //std::cout << "adding point " << i << ", " << j << std::endl;
            points_image1.push_back(point);
        }
    }

    std::vector<cv::Mat> pyramids;
    cv::buildOpticalFlowPyramid(gray_image1, pyramids, winSize, MAX_LEVEL, true);

    //cv::calcOpticalFlowPyrLK(gray_image1, gray_image2, points_image1, points_image2, status, err, winSize, MAX_LEVEL, termcrit, 0, 0.001);

    cv::calcOpticalFlowPyrLK(pyramids, gray_image2, points_image1, points_image2, status, err, winSize, MAX_LEVEL, termcrit, 0, 0.001);

    int num_vectors = 0;

    std::vector<cv::Point2f> src_points;
    std::vector<cv::Point2f> dst_points;

    for (int i = 0; i < points_image2.size(); i++)
    {
        if (status[i])
        {
            cv::Point2f start_point = points_image1.at(i);
            cv::Point2f end_point = points_image2.at(i);
            float x_diff = end_point.x - start_point.x;
            float y_diff = end_point.y - start_point.y;
            if ((std::abs(x_diff) > min_vector_size || std::abs(y_diff) > min_vector_size)) // THIS USED TO BE 1.0
            {
                cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
                elem[0] = start_point.x;
                elem[1] = start_point.y;
                elem[2] = x_diff;
                elem[3] = y_diff;
                //elem[2] = atan2(y_diff, x_diff);
                //elem[3] = sqrt(x_diff*x_diff + y_diff*y_diff);
                src_points.push_back(start_point);
                dst_points.push_back(end_point);
                num_vectors++;
            }
            else
            {
                cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
                elem[0] = start_point.x;
                elem[1] = start_point.y;
                elem[2] = 0.0;
                elem[3] = 0.0;
            }
        }
        else
        {
            cv::Point2f start_point = points_image1.at(i);
            cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
            elem[0] = -1.0;
            elem[1] = -1.0;
            elem[2] = 0.0;
            elem[3] = 0.0;
        }
    }
    if (num_vectors > 0)
    {
        cv::Mat pers_transform = cv::getPerspectiveTransform(&src_points[0], &dst_points[0]);
        cv::Mat compensated;
        gray_image2.copyTo(compensated);
        gray_image2.copyTo(comp);
        cv::warpPerspective(gray_image1, compensated, pers_transform, cv::Size(gray_image1.cols, gray_image1.rows));
        cv::absdiff(compensated, gray_image2, comp);
        //std::cout << comp << std::endl;
        cv::threshold(comp, comp, 190, 255, CV_THRESH_BINARY);
    }
    return num_vectors;
}


int OpticalFlowCalculator::calculateOpticalFlowTrajectory(const std::vector<cv::Mat> &images, cv::Mat &optical_flow_vectors, 
                                        std::vector<std::vector<cv::Point2f> > &trajectories, int pixel_step, cv::Mat &comp, double min_vector_size)
{
    
    const int MAX_LEVEL = 5;
    cv::Size winSize(40, 40);
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.03);

    std::vector<std::vector<cv::Point2f> > init_traj_list; 

    // initialize points we want to track
    std::vector<cv::Point2f> points_image1;
    std::vector<cv::Point2f> points_image2;
    for (int i = 0; i < images[0].cols; i = i + pixel_step)
    {
        for (int j = 0; j < images[0].rows; j = j + pixel_step)
        {
            cv::Point2f point(i, j);
            points_image1.push_back(point);
            std::vector<cv::Point2f> traj;
            traj.push_back(point);
            init_traj_list.push_back(traj);
        }
    }

    int num_vectors = 0;
    for (int j = 0; j < images.size() - 1; j++)
    {
        cv::Mat gray_image1;
        cv::Mat gray_image2;

        cvtColor(images.at(j), gray_image1, CV_BGR2GRAY);
        cvtColor(images.at(j+1), gray_image2, CV_BGR2GRAY);

        std::vector<cv::Mat> pyramids;
        cv::buildOpticalFlowPyramid(gray_image1, pyramids, winSize, MAX_LEVEL, true);

        cv::calcOpticalFlowPyrLK(pyramids, gray_image2, points_image1, points_image2, status, err, winSize, MAX_LEVEL, termcrit, 0, 0.001);


        std::vector<cv::Point2f> temp;
        
        int found_vectors = 0;
        for (int i = 0; i < points_image2.size(); i++)
        {
            if (status[i])
            {
                found_vectors++;
                if (j == images.size() - 2)
                {
                    cv::Point2f start_point = points_image1.at(i);
                    cv::Point2f end_point = points_image2.at(i);
                    float x_diff = end_point.x - start_point.x;
                    float y_diff = end_point.y - start_point.y;
                    if (std::abs(x_diff) > min_vector_size || std::abs(y_diff) > min_vector_size) // THIS USED TO BE 1.0
                    {
                        cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
                        elem[0] = start_point.x;
                        elem[1] = start_point.y;
                        elem[2] = x_diff;
                        elem[3] = y_diff;
                        num_vectors++;
                    }
                    else
                    {
                        cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
                        elem[0] = start_point.x;
                        elem[1] = start_point.y;
                        elem[2] = 0.0;
                        elem[3] = 0.0;
                    }
                }
                if (points_image2.at(i).x > 10.0 && points_image2.at(i).y > 10.0
                    && points_image2.at(i).x < images[0].cols-10 && points_image2.at(i).y < images[0].rows-10)
                {
                    temp.push_back(points_image2.at(i));
                    init_traj_list.at(i).push_back(points_image2.at(i));
                }
                else
                {
                    temp.push_back(points_image1.at(i));
                }
            }
            else
            {
                if (j == images.size() - 2)
                {
                    //std::cout << "in here " << std::endl;
                    cv::Point2f start_point = points_image1.at(i);
                    //std::cout << "start_point " << start_point.x << ", " << start_point.y << std::endl;
                    cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
                    elem[0] = -1.0;
                    elem[1] = -1.0;
                    elem[2] = 0.0;
                    elem[3] = 0.0;
                }
                if (points_image1.at(i).x < 0.0 || points_image1.at(i).y < 0.0)
                {
                }
                temp.push_back(points_image1.at(i));
            }
        }
        points_image1.clear();
        for (int xyz=0;xyz<temp.size();xyz++)
        {
            points_image1.push_back(temp.at(xyz));
        }
        points_image2.clear();
    }
    for (int i = 0; i < init_traj_list.size(); i++)
    {
        if (init_traj_list.at(i).size() == images.size())
        {
            trajectories.push_back(init_traj_list.at(i));
        }
        else 
        {
            //std::cout << "size: " << init_traj_list.at(i).size() << std::endl;
        }
    }
    
    return num_vectors;
}






int OpticalFlowCalculator::calculateCompensatedFlow(const cv::Mat &image1, const cv::Mat &image2, cv::Mat &optical_flow_vectors, int pixel_step)
{
    const int MAX_LEVEL = 2;
    cv::Size winSize(40, 40);
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.03);

    
    cv::Mat gray_image1;
    cv::Mat gray_image2;

    cvtColor(image1, gray_image1, CV_BGR2GRAY);
    cvtColor(image2, gray_image2, CV_BGR2GRAY);

    std::vector<cv::Point2f> points_image1;
    std::vector<cv::Point2f> points_image2;

    for (int i = 0; i < image1.cols; i = i + pixel_step)
    {
        for (int j = 0; j < image1.rows; j = j + pixel_step)
        {
            cv::Point2f point(i, j);
            //std::cout << "adding point " << i << ", " << j << std::endl;
            points_image1.push_back(point);
        }
    }

    cv::calcOpticalFlowPyrLK(gray_image1, gray_image2, points_image1, points_image2, status, err, winSize, MAX_LEVEL, termcrit, 0, 0.001);


    int num_vectors = 0;

    for (int i = 0; i < points_image2.size(); i++)
    {
        if (status[i])
        {
            cv::Point2f start_point = points_image1.at(i);
            cv::Point2f end_point = points_image2.at(i);
            float x_diff = end_point.x - start_point.x;
            float y_diff = end_point.y - start_point.y;
            if (std::abs(x_diff) > 1.0 || std::abs(y_diff) > 1.0)
            {
                cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
                elem[0] = start_point.x;
                elem[1] = start_point.y;
                elem[2] = x_diff;
                elem[3] = y_diff;
                //elem[2] = atan2(y_diff, x_diff);
                //elem[3] = sqrt(x_diff*x_diff + y_diff*y_diff);
                num_vectors++;
            }
            else
            {
                cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
                elem[0] = start_point.x;
                elem[1] = start_point.y;
                elem[2] = 0.0;
                elem[3] = 0.0;
            }
        }
        else
        {
            cv::Point2f start_point = points_image1.at(i);
            cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
            elem[0] = -1.0;
            elem[1] = -1.0;
            elem[2] = 0.0; 
            elem[3] = 0.0;
        }
    }
}

int OpticalFlowCalculator::superPixelFlow(const cv::Mat &image1, const cv::Mat &image2, cv::Mat &optical_flow_image, cv::Mat &optical_flow_vectors)
{
    int width = image1.cols;
    int height = image1.rows;
    

    IplImage *im1 = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U,3); 
    im1->imageData = (char *)image1.data;

    
    int number_of_superpixels = 50;
    int nc = 40;

    double step = sqrt((width * height) / (double) number_of_superpixels);


    Slic slic;
    IplImage *im1_lab = cvCloneImage(im1);
    cvCvtColor(im1, im1_lab, CV_BGR2Lab);
    slic.generate_superpixels(im1_lab, step, nc);

    std::vector<std::vector<double> > centers = slic.get_centers();

    const int MAX_LEVEL = 2;
    cv::Size winSize(40, 40);
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.03);

    image1.copyTo(optical_flow_image);
    
    cv::Mat gray_image1;
    cv::Mat gray_image2;

    cvtColor(image1, gray_image1, CV_BGR2GRAY);
    cvtColor(image2, gray_image2, CV_BGR2GRAY);

    std::vector<cv::Point2f> points_image1;
    std::vector<cv::Point2f> points_image2;

    std::cout << "adding " << centers.size() << " points" << std::endl;
    for (int i = 0; i < centers.size(); i++)
    {
        cv::Point2f point(centers[i][3], centers[i][4]);
        //std::cout << "adding point " << i << ", " << j << std::endl;
        points_image1.push_back(point);
    }

    cv::calcOpticalFlowPyrLK(gray_image1, gray_image2, points_image1, points_image2, status, err, winSize, MAX_LEVEL, termcrit, 0, 0.001);


    int num_vectors = 0;
    for (int i = 0; i < points_image2.size(); i++)
    {
        if (status[i])
        {
            cv::Point2f start_point = points_image1.at(i);
            cv::Point2f end_point = points_image2.at(i);
            float x_diff = end_point.x - start_point.x;
            float y_diff = end_point.y - start_point.y;
            if (x_diff > 1.0 || y_diff > 1.0)
            {
                cv::Vec4d &elem = optical_flow_vectors.at<cv::Vec4d> ((int)start_point.y, (int)start_point.x);
                elem[0] = start_point.x;
                elem[1] = start_point.y;
               // elem[2] = end_point.x - start_point.x;
              //  elem[3] = end_point.y - start_point.y;
              //  elem[4] = sqrt(elem[2] * elem[2] + elem[3] * elem[3]);
              //  elem[5] = atan2(elem[3], elem[2]);
                elem[2] = atan2(y_diff, x_diff);
                elem[3] = sqrt(x_diff*x_diff + y_diff*y_diff);
                //std::cout << "elem is : " << optical_flow_vectors.at<cv::Vec4d>((int)start_point.y, (int)start_point.x) << std::endl;
                //std::cout << "Point " << start_point.x << ", " << start_point.y << " moved to " << end_point.x << ", " << end_point.y << std::endl;
                cv::line(optical_flow_image, start_point, end_point, CV_RGB(255,0,0), 1, CV_AA, 0);
                num_vectors++;
            }
        }
    }
    return num_vectors;
}
void OpticalFlowCalculator::varFlow(const cv::Mat &image1, const cv::Mat &image2, cv::Mat &optical_flow, cv::Mat &optical_flow_vectors)
{

    int width = image1.cols;
    int height = image1.rows;
    int max_level = 4;
    int start_level = 0;
    int n1 = 2;
    int n2 = 2;

    float rho = 2.8;
    float alpha = 1400;
    float sigma = 1.5;    

    //std::cout << "before var flow" << std::endl;
    VarFlow var_flow(width, height, max_level, start_level, n1, n2, rho, alpha, sigma);


    IplImage *im1 = cvCreateImage(cvSize(width, height), 8,1); 
    im1->imageData = (char *)image1.data;

    IplImage *im2 = cvCreateImage(cvSize(width, height), 8,1); 
    im2->imageData = (char *)image2.data;



//    IplImage *im1 = cvCloneImage(&(IplImage)image1);
//    IplImage *im2 = cvCloneImage(&(IplImage)image2);
    IplImage* imgU = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
    IplImage* imgV = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
    //std::cout << "created IplImages" << std::endl;
    cvZero(imgU);
    cvZero(imgV);
    //std::cout << "zeroed images " << std::endl;

    var_flow.CalcFlow(im1, im2, imgU, imgV, 0);
    //std::cout << "ran calc flow " << std::endl;

    IplImage* imgMotion = cvCreateImage(cvSize(width, height), 8, 3);
    imgMotion->imageData = (char *)image2.data;
    
    //cvZero(imgMotion);
    drawMotionField(imgU, imgV, imgMotion, 10, 10, 5, 1, CV_RGB(255,0,0));
    

    cv::Mat m = cv::cvarrToMat(imgMotion);
    m.copyTo(optical_flow);
}

// Draw a vector field based on horizontal and vertical flow fields
void OpticalFlowCalculator::drawMotionField(IplImage* imgU, IplImage* imgV, IplImage* imgMotion, int xSpace, int ySpace, float cutoff, int multiplier, CvScalar color)
{
     int x, y;
    
     CvPoint p0 = cvPoint(0,0);
     CvPoint p1 = cvPoint(0,0);
     
     float deltaX, deltaY, angle, hyp;
     
     for(y = ySpace; y < imgU->height; y+= ySpace ) {
        for(x = xSpace; x < imgU->width; x+= xSpace ){
         
            p0.x = x;
            p0.y = y;
            
            deltaX = *((float*)(imgU->imageData + y*imgU->widthStep)+x);
            deltaY = -(*((float*)(imgV->imageData + y*imgV->widthStep)+x));
            
            angle = atan2(deltaY, deltaX);
            hyp = sqrt(deltaX*deltaX + deltaY*deltaY);
   
            if(hyp > cutoff){
                   
                p1.x = p0.x + cvRound(multiplier*hyp*cos(angle));
                p1.y = p0.y + cvRound(multiplier*hyp*sin(angle));
                   
                cvLine( imgMotion, p0, p1, color,1, CV_AA, 0);
                
                p0.x = p1.x + cvRound(3*cos(angle-M_PI + M_PI/4));
                p0.y = p1.y + cvRound(3*sin(angle-M_PI + M_PI/4));
                cvLine( imgMotion, p0, p1, color,1, CV_AA, 0);
                
                p0.x = p1.x + cvRound(3*cos(angle-M_PI - M_PI/4));
                p0.y = p1.y + cvRound(3*sin(angle-M_PI - M_PI/4));
                cvLine( imgMotion, p0, p1, color,1, CV_AA, 0);
            }
      
        }
    }
    
}

void OpticalFlowCalculator::writeFlow(const cv::Mat &optical_flow_vectors, const std::string &filename, int pixel_step)
{
    std::string horizontal_f = filename + "_h";
    std::string vertical_f = filename + "_f";
    ofstream hfile(horizontal_f.c_str());
    ofstream vfile(vertical_f.c_str());
    for (int i = 0; i < optical_flow_vectors.rows; i = i + pixel_step)
    {
        for (int j = 0; j < optical_flow_vectors.cols; j = j + pixel_step)
        {
            if (j != 0)
            {
                hfile << ", ";
                vfile << ", ";
            }
            cv::Vec4d elem = optical_flow_vectors.at<cv::Vec4d>(i, j);
            if (elem[0] == -1.0)
            {
                hfile << 0.0;
                vfile << 0.0;
            }
            else
            {
                hfile << elem[2];
                vfile << elem[3];
            }
        }
        hfile << endl;
        vfile << endl;
    }
    hfile.close();
    vfile.close();
}

void OpticalFlowCalculator::writeTrajectories(const std::vector<std::vector<cv::Point2f> > &trajectories, const std::string &filename)
{

    ofstream tfile(filename.c_str());

    for (int i = 0; i < trajectories.size(); i++)
    {
        std::vector<cv::Point2f> traj = trajectories.at(i);
        for (int j = 0; j < traj.size(); j++)
        {
            if (j != 0)
            {
                tfile << ", ";                
            }
            tfile << traj.at(j).x << ", " << traj.at(j).y;
        }
        tfile << std::endl;
    }
    tfile.close();
}
