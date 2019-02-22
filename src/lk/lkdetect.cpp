#include"lkdetect.hpp"



LKmove::LKmove():pixel_step_(10),min_vector_size_(0.4)
{
	memset(backgroundmov,0,sizeof(backgroundmov));

}
LKmove::~LKmove()
{

}

cv::Rect LKmove::findRectangle(cv::Mat Sub,
					   cv::Point2f &r1, 
					   cv::Point2f &r2)
{
	cv::Point2f top, bottom, left, right;
	//top most point
	bool set = false;
	for(int i = 0; i < Sub.rows; i++)
	{
		for(int j = 0; j < Sub.cols; j++)
		{
			if( Sub.at<uchar>(i,j) != 0)
			{
				top = cv::Point2f(j,i);
				set = true;
				break;
			}
		}
		if(set == true)
			break;
	}

	// bottom most point
	set = false;
	for(int i = Sub.rows -1; i > 0; i--)
	{
		for(int j = Sub.cols -1; j > 0; j--)
		{
			if( Sub.at<uchar>(i,j) != 0)
			{
				bottom = cv::Point2f(j,i);
				set = true;
				break;
			}
		}
		if(set == true)
			break;
	}

	//right most point
	set = false;
	for(int i = Sub.cols-1; i > 0; i--)
	{
		for(int j = Sub.rows-1; j > 0; j--)
		{
			if( Sub.at<uchar>(j,i) != 0)
			{
				right = cv::Point2f(i,j);
				set = true;
				break;
			}
		}
		if(set == true)
			break;
	}

	//left most point
	set = false;
	for(int i = 0; i < Sub.cols; i++)
	{
		for(int j = 0; j < Sub.rows; j++)
		{
			if( Sub.at<uchar>(j,i) != 0)
			{
				left = cv::Point2f(i,j);
				set = true;
				break;
			}
		}
		if(set == true)
			break;
	}

	r1 = cv::Point2f(left.x, top.y);
	r2 = cv::Point2f(right.x, bottom.y);
	return cv::Rect(r1,r2);
}
void LKmove::lkmovdetectcreate(LPNOTIFYFUNClk fun, void *context)
{
		callback=fun;
		lkcontext=context;
		if(PANOGRAYDETECT)
			background=Mat(MOVDETECTSRCHEIGHT/MOVDETECTDOW,MOVDETECTSRCWIDTH/MOVDETECTDOW,CV_8UC1,cv::Scalar(0));
			else
			background=Mat(MOVDETECTSRCHEIGHT/MOVDETECTDOW,MOVDETECTSRCWIDTH/MOVDETECTDOW,CV_8UC3,cv::Scalar(0));
		for(int i=0;i<MOVELKBLOCKNUM;i++)
			{

				if(PANOGRAYDETECT)
					backgroundmovmat[i]=Mat(MOVDETECTSRCHEIGHT/MOVDETECTDOW,MOVDETECTSRCWIDTH/MOVDETECTDOW,CV_8UC1,cv::Scalar(0));
					else
					backgroundmovmat[i]=Mat(MOVDETECTSRCHEIGHT/MOVDETECTDOW,MOVDETECTSRCWIDTH/MOVDETECTDOW,CV_8UC3,cv::Scalar(0));


			}


}

void LKmove::lkmovdetectgetrect(Mat &src,vector<Rect> &objects)
{
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Point> conterpart;
   // cv::erode(src, src, cv::Mat());
   // cv::dilate(src, src, cv::Mat());


    cv::findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
   objects.clear();
   for(int i=0;i<contours.size();i++)
   	{
		contours[i];
		RotatedRect box = minAreaRect(contours[i]);
		objects.push_back(boundingRect(contours[i]));

   	}
   
    //cv::drawContours(src, contours, -1, cv::Scalar(255,255,255), 2); 


}

int  LKmove::OpticalFlowprocess(const cv::Mat &image1, const cv::Mat &image2, cv::Mat &dst)
{
	
	  int num_vectors = ofc_.calculateOpticalFlowprocess(image1, image2, dst, pixel_step_, min_vector_size_);
	
	return num_vectors;
  	 // printf("the num vec=%d\n",num_vectors);

}
void LKmove::runOpticalFlowtest( cv::Mat &image1,  cv::Mat &image2)
{

ofc_.calculatefeature(image1, image2);
}
void LKmove::runOpticalFlow(const cv::Mat &image1, const cv::Mat &image2, cv::Mat &optical_flow_vectors)
{
	static int countnum=0;
	char bufname[50];
#if 1
    cv::Mat debug_image;
    cv::Mat optical_flow_image;

    optical_flow_vectors = cv::Mat::zeros(image1.rows, image1.cols, CV_32FC4);
    int num_vectors = ofc_.calculateOpticalFlowgray(image1, image2, optical_flow_vectors, pixel_step_, debug_image, min_vector_size_);
    printf("the num vec=%d\n",num_vectors);
    ofv_.showOpticalFlowVectors(image1, optical_flow_image, optical_flow_vectors, pixel_step_, CV_RGB(0, 0, 255), min_vector_size_);

  
   if(!debug_image.empty())
   	{
   		//lkmovdetectgetrect(debug_image,objectsrect);
  		// imshow("debug_image",debug_image);
   	}
   for(int i=0;i<objectsrect.size();i++)
   	{
	//	rectangle(optical_flow_image, objectsrect[i], Scalar(255,255,255),1,  8);
   	}
   sprintf(bufname,"/home/ubuntu/lk/%d.jpg",countnum);
   countnum++;
  // if(!optical_flow_image.empty())
   imwrite(bufname,optical_flow_image);
   // imshow("optical_flow_image",optical_flow_image);
   // waitKey(1);
#else



#endif

}


void LKmove::getMoveTarget(vector<Rect> &objects,int chId)
{
	objects.clear();
	for(int i=0;i<objectsrect.size();i++)
		{
			;
			objects.push_back(objectsrect[i]);
		}
	

}

void LKmove::lkmovdetectreset()
{
	for(int i=0;i<MOVELKBLOCKNUM;i++)
		backgroundmov[i]=0;

}

void LKmove::lkmovdetectpreprocess(Mat &src,Mat &dst,int chid)
{
		double exec_time = (double)getTickCount();
		
		if(backgroundmov[chid]==0)
			{
				memcpy(backgroundmovmat[chid].data,src.data,src.cols*src.rows*src.channels());
				if(dst.data!=NULL)
				memcpy(dst.data,src.data,src.cols*src.rows*src.channels());
				backgroundmov[chid]=1;
				printf("the backgroundmov chid =%d \n",chid);
				return ;
			}
		OpticalFlowprocess(backgroundmovmat[chid], src, dst);

		exec_time = ((double)getTickCount() - exec_time)*1000./getTickFrequency();
	 	//OSA_printf("the lkmovdetectpreprocess=%f MS\n",exec_time);
		

}
void LKmove::lkmovdetect(Mat src,int chid)
{
	static int backg=0;
	if(backgroundmov[chid]==0)
			{
			memcpy(backgroundmovmat[chid].data,src.data,src.cols*src.rows*src.channels());
			backgroundmov[chid]=1;
			return ;
		}
	cv::Mat optical_flow_vectors;
	runOpticalFlow(backgroundmovmat[chid], src, optical_flow_vectors);
	//runOpticalFlowtest(background, src);

	if(callback!=NULL)
		callback(lkcontext,chid);
	//imshow("background ",background);
	//imshow("detect ",src);
	//waitKey(1);
	

}
