#include"Homography.hpp"
#include <iostream>
#include "stdio.h"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "osa.h"
#include "config.h"
#include "extra.h" 
using namespace cv;
using namespace std;
//unsigned int Templatex=0;
void FindHomographyfromPicture(Mat& src,Mat & dst,Mat &Ha)
{
/*
	  //Mat img_object = imread( "1.png", CV_LOAD_IMAGE_GRAYSCALE );  
	 // Mat img_scene = imread( "2.png", CV_LOAD_IMAGE_GRAYSCALE );  
	 Mat img_object = imread( "Bridge/01.jpg", CV_LOAD_IMAGE_GRAYSCALE );  
	 Mat img_scene = imread( "Bridge/02.jpg", CV_LOAD_IMAGE_GRAYSCALE );  
	  
	 //-- Step 1: Detect the keypoints using SURF Detector
	  int minHessian = 400;

	  SurfFeatureDetector detector( minHessian );

	  std::vector<KeyPoint> keypoints_object, keypoints_scene;

	  detector.detect( img_object, keypoints_object );
	  detector.detect( img_scene, keypoints_scene );

	  //-- Step 2: Calculate descriptors (feature vectors)
	  SurfDescriptorExtractor extractor;

	  Mat descriptors_object, descriptors_scene;

	  extractor.compute( img_object, keypoints_object, descriptors_object );
	  extractor.compute( img_scene, keypoints_scene, descriptors_scene );

	  //-- Step 3: Matching descriptor vectors using FLANN matcher
	  FlannBasedMatcher matcher;
	  std::vector< DMatch > matches;
	  matcher.match( descriptors_object, descriptors_scene, matches );

	  double max_dist = 0; double min_dist = 100;

	  //-- Quick calculation of max and min distances between keypoints
	  for( int i = 0; i < descriptors_object.rows; i++ )
	  { double dist = matches[i].distance;
	    if( dist < min_dist ) min_dist = dist;
	    if( dist > max_dist ) max_dist = dist;
	  }

	  printf("-- Max dist : %f \n", max_dist );
	  printf("-- Min dist : %f \n", min_dist );

	  //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	  std::vector< DMatch > good_matches;

	  for( int i = 0; i < descriptors_object.rows; i++ )
	  { if( matches[i].distance < 3*min_dist )
	     { good_matches.push_back( matches[i]); }
	  }

	  Mat img_matches;
	  drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
	               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
	               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

	  //-- Localize the object
	  std::vector<Point2f> obj;
	  std::vector<Point2f> scene;

	  for( int i = 0; i < good_matches.size(); i++ )
	  {
	    //-- Get the keypoints from the good matches
	    obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
	    scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	  }

	  Mat H = findHomography( obj, scene, CV_RANSAC );

	  //-- Get the corners from the image_1 ( the object to be "detected" )
	  std::vector<Point2f> obj_corners(4);
	  obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
	  obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
	  std::vector<Point2f> scene_corners(4);

	  perspectiveTransform( obj_corners, scene_corners, H);

#if 0
	 

	  Mat dst1;

	  Mat shftMat=(Mat_<double>(3,3)<<1.0,0,img_object.cols, 0,1.0,0, 0,0,1.0);
	  warpPerspective(img_scene,dst1,shftMat*H,Size(img_scene.cols*2, img_scene.rows));

	  img_object.copyTo(Mat(dst1,Rect(0,0,img_object.cols,img_object.rows)));
	    imshow( "dst", dst1 );
#endif
	  //-- Draw lines between the corners (the mapped object in the scene - image_2 )
	  line( img_matches, scene_corners[0] + Point2f( img_object.cols, 0), scene_corners[1] + Point2f( img_object.cols, 0), Scalar(0, 255, 0), 4 );
	  line( img_matches, scene_corners[1] + Point2f( img_object.cols, 0), scene_corners[2] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
	  line( img_matches, scene_corners[2] + Point2f( img_object.cols, 0), scene_corners[3] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
	  line( img_matches, scene_corners[3] + Point2f( img_object.cols, 0), scene_corners[0] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );

	  //line(img_matches,Point2f(0,0),Point2f(img_object.cols,img_object.rows),Scalar(0, 255, 0), 4);
	  //-- Show detected matches

	  imshow( "Good Matches & Object detection", img_matches );

	  waitKey(0);
*/
}

Vec3f rotationMatrixToEulerAngles(Mat &R)
{



    float sy = sqrt(R.at<double>(0,0) * R.at<double>(0,0) +  R.at<double>(1,0) * R.at<double>(1,0) );

    bool singular = sy < 1e-6; // If

    float x, y, z;
    if (!singular)
    {
    	 //printf("***************usr right*****************\n");
        x = atan2(R.at<double>(2,1) , R.at<double>(2,2));
        y = atan2(-R.at<double>(2,0), sy);
        z = atan2(R.at<double>(1,0), R.at<double>(0,0));
    }
    else
    {
        x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
        y = atan2(-R.at<double>(2,0), sy);
        z = 0;
    }
    return Vec3f(x, y, z);
}
Point2i getOffset(Mat img, Mat img1) {
	Mat templ(img1, Rect(0, 0.4*img1.rows, 0.2*img1.cols, 0.2*img1.rows)); 
	Mat result(img.cols - templ.cols + 1, img.rows - templ.rows + 1, CV_8UC1);
	
	matchTemplate(img, templ, result, CV_TM_CCORR_NORMED); 
	
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat()); 
	
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc; 
	
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat()); 
	matchLoc = maxLoc;
	int dx = matchLoc.x; 
	int dy = matchLoc.y - 0.4*img1.rows;
	Point2i a(dx, dy); 
	return a; 
	}
double dotProduct(const vector<float>& v1, const vector<float>& v2) 
	{
		assert(v1.size() == v2.size());
		double ret = 0.0; 
		for (vector<float>::size_type i = 0; i != v1.size(); ++i)
		{ 
			ret += v1[i] * v2[i]; 
		} 
		return ret;
	} 
	double module(const vector<float>& v) 
	{ double ret = 0.0;
		for (vector<float>::size_type i = 0; i != v.size(); ++i) 
		{ 
			ret += v[i] * v[i]; 
		}
		return sqrt(ret);
	} 
	double cosine(const vector<float>& v1, const vector<float>& v2) 
	{ 
		assert(v1.size() == v2.size()); 
		return dotProduct(v1, v2) / (module(v1) * module(v2));
	}


void median_filter_matches(vector<DMatch>& match,  std::vector<KeyPoint>& keypoints_1,std::vector<KeyPoint>& keypoints_2)
{
 	double min_dist=10000, max_dist=0;
 	int midindex=0;
      std::vector< DMatch > matches;
	vector<float> diffs;
	 for ( int i = 0; i < ( int ) match.size(); i++ )
	    {
	    	    float px_diff =keypoints_1[match[i].queryIdx].pt.x-keypoints_2[match[i].trainIdx].pt.x;
	    	   diffs.push_back(px_diff);
	    }
	 vector<float> diffs_sorted = diffs;
      std::sort(diffs_sorted.begin(), diffs_sorted.end());
       float median = diffs[diffs_sorted.size()/2];
	 for(int i = 0; i < match.size(); i++)
	 	{
			float px_diff =keypoints_1[match[i].queryIdx].pt.x-keypoints_2[match[i].trainIdx].pt.x;
			if(px_diff/median<1.2)
				matches.push_back(match[i]);
	 	}
    		
	 match.swap(matches);
  
	
    

    
}



void angle_filter_matches(vector<DMatch>& match,  std::vector<KeyPoint>& keypoints_1,std::vector<KeyPoint>& keypoints_2)
{
 	double min_dist=10000, max_dist=0;
 	int midindex=0;
      std::vector< DMatch > matches;

	
	 for(int i = 0; i < match.size(); i++)
	 	{
			float px_diff =keypoints_1[match[i].queryIdx].pt.x-keypoints_2[match[i].trainIdx].pt.x;
			float py_diff =keypoints_1[match[i].queryIdx].pt.y-keypoints_2[match[i].trainIdx].pt.y;
			double backward_angle = atan2(py_diff, px_diff)*180/3.141592653;
			if(abs(backward_angle)<2)
				matches.push_back(match[i]);
	 	}
    		
	 match.swap(matches);
  
	
    

    
}
void retinewithdistance(vector<DMatch>& match,   Mat descriptors_1,Mat descriptors_2)
{

		double min_dist=10000, max_dist=0;

    std::vector< DMatch > matches;
    for ( int i = 0; i < match.size(); i++ )
    {
        double dist = match[i].distance;
        if ( dist < min_dist ) min_dist = dist;
        if ( dist > max_dist ) max_dist = dist;
    }

   


    sort(match.begin(), match.end()); 
   // printf ( "-- Max dist : %f \n", max_dist );
  //  printf ( "-- Min dist : %f \n", min_dist );

  
  //  for ( int i = 0; i < descriptors_1.rows; i++ )

   for ( int i = 0; i < match.size(); i++ )
    {
        if ( match[i].distance <= max ( 2*min_dist,30.0 ) )
        {
            matches.push_back ( match[i] );
        }
    }




    match.swap(matches);


}
void retinefeature(vector<DMatch>& goodMatches,   Mat descriptors1,Mat descriptors2)
{
	vector<float> vec_all1;
	vector<float> vec_all2; 
	vector<float>vec_all3;
	double cosine_sum = 0; 
	double cosine_average; 
	double value; 
	vector<double>value_all;

	for (int i = 0; i < goodMatches.size(); i++) 
	{ 
	
		int *data1 = descriptors1.ptr<int>(goodMatches[i].queryIdx); 
		int *data2 = descriptors2.ptr<int>(goodMatches[i].trainIdx); 
		//cout << "******************1***********" << endl;
		for (int j = 0; j < 128; j++) 
		{
			int vec1 = data1[j];
			int vec2 = data2[j]; 
			vec_all1.push_back(vec1);
			vec_all2.push_back(vec2); 
		} 
		
		value = cosine(vec_all1, vec_all2);
		
		value_all.push_back(value); 
		cosine_sum = cosine_sum + value;
		
		vec_all1.clear();
		vec_all2.clear();
		//vec_all1.swap(vec_all3); 
		//vec_all2.swap(vec_all3); 
	} 

	cosine_average = cosine_sum / goodMatches.size();
	cout << "22" << cosine_average << endl; 
	vector<DMatch>angle_matches; 
	for (int n = 0; n < goodMatches.size(); n++) 
	{
		if (value_all[n] > cosine_average) 
		{
			angle_matches.push_back(goodMatches[n]); 
		}
	} 
	cout << "12" << angle_matches.size() << endl; 
	cout << "12" << goodMatches.size() - angle_matches.size() << endl;
	goodMatches.swap(angle_matches);

}
void refineMatcheswithHomography(vector<DMatch>& matches, double reprojectionThreshold ,std::vector<KeyPoint>& keypoints_1,
                            std::vector<KeyPoint>& keypoints_2){
    const int minNumbermatchesAllowed = 8;
    Mat homography;
    if (matches.size() < minNumbermatchesAllowed)
        return;

    //Prepare data for findHomography
    vector<Point2f> srcPoints(matches.size());
    vector<Point2f> dstPoints(matches.size());

    for (size_t i = 0; i < matches.size(); i++) {
        srcPoints[i] = keypoints_1[matches[i].trainIdx].pt;
        dstPoints[i] =keypoints_2[matches[i].queryIdx].pt;
    }

    //find homography matrix and get inliers mask
    vector<uchar> inliersMask(srcPoints.size());
    homography = findHomography(srcPoints, dstPoints, CV_FM_RANSAC, reprojectionThreshold, inliersMask);

    vector<DMatch> inliers;
    for (size_t i = 0; i < inliersMask.size(); i++){
        if (inliersMask[i])
            inliers.push_back(matches[i]);
    }
    matches.swap(inliers);
}


int find_feature_matches ( const Mat& img_1, const Mat& img_2,
                            std::vector<KeyPoint>& keypoints_1,
                            std::vector<KeyPoint>& keypoints_2,
                            std::vector< DMatch >& matches )
{
    
    Mat descriptors_1, descriptors_2;
   double exec_time=0;
    // used in OpenCV3 
  #if 0
   Ptr<FeatureDetector> detector = FeatureDetector::create ( "ORB" );
   Ptr<DescriptorExtractor> descriptor = DescriptorExtractor::create ( "ORB" );
   Ptr<DescriptorMatcher> matcher  = DescriptorMatcher::create ( "BruteForce-Hamming" );

   #elif 1
  Ptr<FeatureDetector> detector = FeatureDetector::create ( "ORB" );
  // OrbFeatureDetector detector(500);
   Ptr<DescriptorExtractor> descriptor = DescriptorExtractor::create ( "ORB" );
   Ptr<DescriptorMatcher> matcher  = DescriptorMatcher::create ( "BruteForce-Hamming" );
  #else
  //initModule_nonfree();//if use SIFT or SURF
//   Ptr<FeatureDetector> detector = FeatureDetector::create ( "SURF" );
 const int minHessian = 700;
    SurfFeatureDetector detector(minHessian);

   Ptr<DescriptorExtractor> descriptor = DescriptorExtractor::create ( "SURF" );
   Ptr<DescriptorMatcher> matcher  = DescriptorMatcher::create ( "BruteForce" );
  #endif
     exec_time = (double)getTickCount();
   // detector.removeInvalidPoints(const Mat & mask, vector < KeyPoint > & keypoints)
    detector->detect ( img_1,keypoints_1 );
    detector->detect ( img_2,keypoints_2 );
     exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();


    if(keypoints_1.size()<=10||keypoints_2.size()<=10)
		return -1;

   // OSA_printf("the %s detect exec_time=%f\n",__func__,exec_time);
    exec_time = (double)getTickCount();
    descriptor->compute ( img_1, keypoints_1, descriptors_1 );
    descriptor->compute ( img_2, keypoints_2, descriptors_2 );
     exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
   //  OSA_printf("the %s compute exec_time=%f\n",__func__,exec_time);
   
    vector<DMatch> match;
    //BFMatcher matcher ( NORM_HAMMING );
    exec_time = (double)getTickCount();
    matcher->match ( descriptors_1, descriptors_2, match );

/*
   FlannBasedMatcher matcherknn; 

   vector<vector<DMatch> > m_knnMatches; 

   const float minRatio=1.f / 1.5f; 
   matcherknn.knnMatch(descriptors_1,descriptors_2,m_knnMatches,2);

   cout << "total match points: " << m_knnMatches.size() << endl;
	

	for (int i = 0; i < m_knnMatches.size(); i++)
	{
		if (m_knnMatches[i][0].distance < 0.2 * m_knnMatches[i][1].distance)
		{
			match.push_back(m_knnMatches[i][0]);
		}
	}

*/
/*
	 flann::Index flannIndex(descriptors_1, flann::LshIndexParams(12, 20, 2), cvflann::FLANN_DIST_HAMMING);
    //  cv::flann::Index flannIndex(descriptors_1, cv::flann::KDTreeIndexParams(4));
  //  vector<DMatch> GoodMatchePoints;
    
    Mat macthIndex(descriptors_2.rows, 2, CV_32SC1);
    Mat matchDistance(descriptors_2.rows, 2, CV_32FC1);
    flannIndex.knnSearch(descriptors_2, macthIndex, matchDistance, 2, flann::SearchParams(64));

    // Lowe's algorithm
    for (int i = 0; i < matchDistance.rows; i++)
    {
        if (matchDistance.at<float>(i,0) < 0.6 * matchDistance.at<float>(i, 1))
        {
            DMatch dmatches(i, macthIndex.at<int>(i, 0), matchDistance.at<float>(i, 0));
            match.push_back(dmatches);
        }
    }

*/
	
	 exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
   //  OSA_printf("the %s match exec_time=%f\n",__func__,exec_time);
	if(match.size()==0)
		return -1;
	for(int i=0; i<match.size(); i++)  
	{  
		
			matches.push_back(match[i]);  
	 
	} 

     //refineMatcheswithHomography(matches,3,keypoints_1,keypoints_2);
  
     //retinefeature(matches,descriptors_1,descriptors_2);
     retinewithdistance(matches,descriptors_1,descriptors_2);
     median_filter_matches(matches,keypoints_1,keypoints_2);
     angle_filter_matches(matches,keypoints_1,keypoints_2);
	if(matches.size()<=10)
		return -1;

	return 0;
    
}
cv::Mat  FindHomography(Mat& src,Mat & dst)
{
	 int status=0;

    vector<KeyPoint> keypoints_1, keypoints_2;
    vector<DMatch> matches;
    find_feature_matches ( src, dst, keypoints_1, keypoints_2, matches );

    Mat R,t;
    vector<Point2f> points1;
    vector<Point2f> points2;
    for ( int i = 0; i < ( int ) matches.size(); i++ )
    {
        points1.push_back ( keypoints_1[matches[i].queryIdx].pt );
        points2.push_back ( keypoints_2[matches[i].trainIdx].pt );
    }
 cv::Mat H = findHomography(points2, points1, CV_RANSAC);

return H;

}
void pose_estimation_2d2d ( std:: vector<Point2f> points1,
                            vector<Point2f> points2,
                         
                            Mat& R, Mat& t )
{
 
   // Mat K = ( Mat_<double> ( 3,3 ) << 520.9, 0, 325.1, 0, 521.0, 249.7, 0, 0, 1 );
#if 0
    double kx=2.666666;
    double ky=2.666666;
#else
	double kx=1;
    double ky=1;
#endif
    double fx=4671.959882/kx;
    double fy=4669.274908/ky;
    double ox=971.9267629/kx;
    double oy=545.9823086/ky;
    Mat K = ( Mat_<double> ( 3,3 ) << fx, 0, ox, 0, fy, oy, 0, 0, 1 );

    /*
    vector<Point2f> points1;
    vector<Point2f> points2;

    for ( int i = 0; i < ( int ) matches.size(); i++ )
    {
        points1.push_back ( keypoints_1[matches[i].queryIdx].pt );
        points2.push_back ( keypoints_2[matches[i].trainIdx].pt );
    }

  */
  /*
    Mat fundamental_matrix;
    fundamental_matrix = findFundamentalMat ( points1, points2, CV_FM_8POINT );
    cout<<"fundamental_matrix is "<<endl<< fundamental_matrix<<endl;
*/
    
    Point2d principal_point (ox, oy);	
    double focal_length = fy;			
    Mat essential_matrix;
    essential_matrix = findEssentialMat ( points1, points2, focal_length, principal_point );
    cout<<"essential_matrix is "<<endl<< essential_matrix<<endl;

 /*
    Mat homography_matrix;
    homography_matrix = findHomography ( points1, points2, RANSAC, 3 );
    cout<<"homography_matrix is "<<endl<<homography_matrix<<endl;
  
*/
   
    recoverPose ( essential_matrix, points1, points2, R, t, focal_length, principal_point );

 
   /*
    Vec3f sita;
    printf("___________________________________________________________________________\n");
    sita=rotationMatrixToEulerAngles(R);
   
    cout<<"sitais "<<endl<<sita<<endl;
    cout<<"X="<<sita[0]*180/3.141592653<<"**************"<<"Y="<<sita[1]*180/3.141592653<<"**************"<<"Z="<<sita[2]*180/3.141592653<<endl;
 
    
    cout<<"R is "<<endl<<R<<endl;
    cout<<"t is "<<endl<<t<<endl;
    printf("___________________________________________________________________________\n");
    */
    
}




int  getPano360Rotation(cv::Mat & src,cv::Mat & dst,double *rotation )
{
    int status=0;
   
    vector<KeyPoint> keypoints_1, keypoints_2;
    vector<DMatch> matches;
    find_feature_matches ( src, dst, keypoints_1, keypoints_2, matches );
     
    Mat R,t;
    vector<Point2f> points1;
    vector<Point2f> points2;
    for ( int i = 0; i < ( int ) matches.size(); i++ )
    {
        points1.push_back ( keypoints_1[matches[i].queryIdx].pt );
        points2.push_back ( keypoints_2[matches[i].trainIdx].pt );
    }

/*
     Mat first_match;
	drawMatches(src, keypoints_1, dst, keypoints_2, matches, first_match);
	imshow("first_match ", first_match);
  
	waitKey(1);
*/
    if(matches.size()<10)
    	{
		cout<<"the matches num is low="<<matches.size()<<endl;
		status= -1;
		 return status;

    	}
    pose_estimation_2d2d ( points1, points2, R, t );
    Vec3f sita;
    sita=rotationMatrixToEulerAngles(R);
    cout<<"X="<<sita[0]*180/3.141592653<<"**************"<<"Y="<<sita[1]*180/3.141592653<<"**************"<<"Z="<<sita[2]*180/3.141592653<<endl;
     *rotation=sita[1]*180/3.141592653;
    return status;
}
void getoffsetfromfeaturepoint(std::vector<Point2f> points1,std::vector<Point2f> points2,int* xoffset,int* yoffset,int width)
{
	int pointnum=20;
	Point2f src,dst;
	int sumx=0;
	int sumy=0;
	if(points1.size()<pointnum)
		pointnum=points1.size();
	for(int i=0;i<pointnum;i++)
		{
			sumx+=points1[i].x;
			sumy+=points1[i].y;

		}

	src.x=floor(sumx/pointnum);
	src.y=floor(sumy/pointnum);
	sumx=0;
	sumy=0;
	for(int i=0;i<pointnum;i++)
		{
			sumx+=points2[i].x;
			sumy+=points2[i].y;

		}

	dst.x=floor(sumx/pointnum);
	dst.y=floor(sumy/pointnum);

	*yoffset=dst.y-src.y;
	*xoffset=src.x-dst.x;
	//*xoffset=width-src.x+dst.x;

}

#define RESIZE 1
int  getPano360Offset(cv::Mat & src,cv::Mat & dst,int *xoffset ,int* yoffset)
{
	  int status=0;
   double exec_time = (double)getTickCount();
    vector<KeyPoint> keypoints_1, keypoints_2;
    vector<DMatch> matches;
	//(processgray[frameid],processgraytemp[frameid],Size(720,405),0,0,INTER_LINEAR)
    Mat tempsrc;
    if(RESIZE)
    resize(src,tempsrc,Size(960,540),0,0,INTER_LINEAR);
  
   // resize(src,tempsrc,Size(1920,1080),0,0,INTER_LINEAR);
    if(RESIZE)
    cvtColor(tempsrc,tempsrc,CV_BGR2GRAY);
    else
     cvtColor(src,tempsrc,CV_BGR2GRAY);
    
    Mat tempdst;
    if(RESIZE)
    resize(dst,tempdst,Size(960,540),0,0,INTER_LINEAR);
  
  //resize(dst,tempdst,Size(1920,1080),0,0,INTER_LINEAR);
    if(RESIZE)
    cvtColor(tempdst,tempdst,CV_BGR2GRAY);
    else
     cvtColor(dst,tempdst,CV_BGR2GRAY);
    find_feature_matches ( tempsrc, tempdst, keypoints_1, keypoints_2, matches );
     
    Mat R,t;
    vector<Point2f> points1;
    vector<Point2f> points2;
    for ( int i = 0; i < ( int ) matches.size(); i++ )
    {
        points1.push_back ( keypoints_1[matches[i].queryIdx].pt );
        points2.push_back ( keypoints_2[matches[i].trainIdx].pt );
    }

/*
     Mat first_match;
	drawMatches(src, keypoints_1, dst, keypoints_2, matches, first_match);
	imshow("first_match ", first_match);
  
	waitKey(1);
*/
    if(matches.size()<3)
    	{
		cout<<"the matches num is low="<<matches.size()<<endl;
		status= -1;
		 return status;

    	}
    getoffsetfromfeaturepoint(points1,points2,xoffset,yoffset,src.cols);
	   if(RESIZE)
	   	{
	*xoffset=*xoffset*2;
	*yoffset=*yoffset*2;
	   	}


    exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();

    OSA_printf("the %s exec_time=%f\n",__func__,exec_time);
    return status;

}


int  getPano360OffsetT(cv::Mat & src,cv::Mat & dst,int *xoffset ,int* yoffset) 
	{
		double exec_time = (double)getTickCount();
		 int status=0;
             Mat tempsrc;
		static unsigned int tempcount=0;
		static unsigned int tempcounterror=0;
		char bufname[100];
             if(RESIZE)
             resize(src,tempsrc,Size(960,540),0,0,INTER_LINEAR);
             
             // resize(src,tempsrc,Size(1920,1080),0,0,INTER_LINEAR);
             if(RESIZE)
             cvtColor(tempsrc,tempsrc,CV_BGR2GRAY);
             else
             cvtColor(src,tempsrc,CV_BGR2GRAY);
             
             Mat tempdst;
             if(RESIZE)
             resize(dst,tempdst,Size(960,540),0,0,INTER_LINEAR);
             
             //resize(dst,tempdst,Size(1920,1080),0,0,INTER_LINEAR);
             if(RESIZE)
             cvtColor(tempdst,tempdst,CV_BGR2GRAY);
             else
             cvtColor(dst,tempdst,CV_BGR2GRAY);

		Rect temprect;
		if(FEATURETEST==0)
		temprect=Rect(0,0.5*tempsrc.rows-0.35*tempsrc.rows,0.3*tempsrc.cols, 0.7*tempsrc.rows);
		else
		temprect=Rect(0,0.5*tempsrc.rows-0.4*tempsrc.rows,0.2*tempsrc.cols, 0.8*tempsrc.rows);
		Mat templ(tempsrc, temprect); 
		Mat result(tempdst.cols - templ.cols + 1, tempdst.rows - templ.rows + 1, CV_8UC1);
		
		matchTemplate(tempdst, templ, result, CV_TM_CCOEFF_NORMED); 
		
		
		
		
		double minVal; double maxVal; Point minLoc; Point maxLoc;
		Point matchLoc; 
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat()); 

		//if(FEATURETEST==0)
		sprintf(bufname,"/home/ubuntu/calib/%d.bmp",tempcount);
		tempcounterror++;
		if(maxVal<0.80)
			{
			
				matchLoc = maxLoc;
				rectangle(tempdst, Rect(matchLoc.x,matchLoc.y,temprect.width,temprect.height), Scalar(255,0,0),1,  8);
				sprintf(bufname,"/home/ubuntu/calib/error%f_%d.bmp",maxVal,tempcount);
				//imwrite(bufname,tempdst);
			return -1;
			}
	
		
		
		//OSA_printf("the %s  maxVal=%f  minVal=%f\n",__func__,maxVal,minVal);

		normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat()); 
		
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat()); 
		matchLoc = maxLoc;
		int dx = matchLoc.x-temprect.x; 
		int dy = matchLoc.y - temprect.y;
		 if(RESIZE)
		 	{
				dx=dx*2;
				dy=dy*2;
		 	}
		*xoffset=-dx;
		*yoffset=-dy;

		rectangle(tempdst, Rect(matchLoc.x,matchLoc.y,temprect.width,temprect.height), Scalar(255,0,0),1,  8);
		
		imwrite(bufname,tempdst);
		tempcount++;
		 exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
		 OSA_printf("the %s exec_time=%f  maxVal=%f  minVal=%f\n",__func__,exec_time,maxVal,minVal);
	//Point2i a(dx, dy); 
	return status; 
	}

