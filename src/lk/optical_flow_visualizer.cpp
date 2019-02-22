/* optical_flow_visualizer.cpp
 *
 * Copyright (C) 2014 Santosh Thoduka
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <optical_flow_visualizer.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>
#include <iostream>

OpticalFlowVisualizer::OpticalFlowVisualizer()
{
}

OpticalFlowVisualizer::~OpticalFlowVisualizer()
{    
}

void OpticalFlowVisualizer::showOpticalFlowVectors(const cv::Mat &original_image, cv::Mat &optical_flow_image, const cv::Mat &optical_flow_vectors, int pixel_step, cv::Scalar colour, double min_vector_size)
{

    original_image.copyTo(optical_flow_image);

    for (int i = 0; i < optical_flow_vectors.rows; i++)
    {
        for (int j = 0; j < optical_flow_vectors.cols; j++)
        {
            cv::Point2f start_point;
            cv::Point2f end_point;

            cv::Vec4d elem = optical_flow_vectors.at<cv::Vec4d>(i, j);

            if ((std::abs(elem[2])> min_vector_size || std::abs(elem[3]) > min_vector_size) && std::abs(elem[2]) < pixel_step*5 && std::abs(elem[3]) < pixel_step*5) 
            {

                start_point.x = elem[0];
                start_point.y = elem[1];            

                /*

                end_point.x = start_point.x + elem[3] * cos(elem[2]);
                end_point.y = start_point.y + elem[3] * sin(elem[2]);
                */

                end_point.x = start_point.x + elem[2];
                end_point.y = start_point.y + elem[3];

                cv::line(optical_flow_image, start_point, end_point, colour, 1, CV_AA, 0);

                double backward_angle = atan2(start_point.y - end_point.y, start_point.x - end_point.x);
                double arrow1_angle = (backward_angle + M_PI / 4.0);
                double arrow2_angle = (backward_angle - M_PI / 4.0);

                cv::Point2f arrow1_end;
                arrow1_end.x = end_point.x + 3.0 * cos(arrow1_angle);
                arrow1_end.y = end_point.y + 3.0 * sin(arrow1_angle);

                cv::Point2f arrow2_end;
                arrow2_end.x = end_point.x + 3.0 * cos(arrow2_angle);
                arrow2_end.y = end_point.y + 3.0 * sin(arrow2_angle);

                cv::line(optical_flow_image, end_point, arrow1_end, colour, 1, CV_AA, 0);
                cv::line(optical_flow_image, end_point, arrow2_end, colour, 1, CV_AA, 0);
            }
        }
    }
}

void OpticalFlowVisualizer::showFlowClusters(const cv::Mat &original_image, cv::Mat &optical_flow_image, const std::vector<cv::Vec4d> &optical_flow_vectors, int pixel_step,  cv::Scalar colour, double min_vector_size)
{
    original_image.copyTo(optical_flow_image);

    std::vector<cv::Point> points;

    for (int i = 0; i < optical_flow_vectors.size(); i++)
    {
        cv::Point2f start_point;
        cv::Point2f end_point;

        cv::Vec4d elem = optical_flow_vectors.at(i);

        if ((std::abs(elem[2])> min_vector_size || std::abs(elem[3]) > min_vector_size) && std::abs(elem[2]) < pixel_step*5 && std::abs(elem[3]) < pixel_step*5) 
        {

            start_point.x = elem[0];
            start_point.y = elem[1];
            cv::Point p;
            p.x = (int)start_point.x;
            p.y = (int)start_point.y;
            points.push_back(p);

            /*

            end_point.x = start_point.x + elem[3] * cos(elem[2]);
            end_point.y = start_point.y + elem[3] * sin(elem[2]);
            */

            end_point.x = start_point.x + elem[2];
            end_point.y = start_point.y + elem[3];

            cv::line(optical_flow_image, start_point, end_point, colour, 1, CV_AA, 0);

            double backward_angle = atan2(start_point.y - end_point.y, start_point.x - end_point.x);
            double arrow1_angle = (backward_angle + M_PI / 4.0);
            double arrow2_angle = (backward_angle - M_PI / 4.0);

            cv::Point2f arrow1_end;
            arrow1_end.x = end_point.x + 3.0 * cos(arrow1_angle);
            arrow1_end.y = end_point.y + 3.0 * sin(arrow1_angle);

            cv::Point2f arrow2_end;
            arrow2_end.x = end_point.x + 3.0 * cos(arrow2_angle);
            arrow2_end.y = end_point.y + 3.0 * sin(arrow2_angle);

            cv::line(optical_flow_image, end_point, arrow1_end, colour, 1, CV_AA, 0);
            cv::line(optical_flow_image, end_point, arrow2_end, colour, 1, CV_AA, 0);
        }
    }
    if (!points.empty())
    {
        cv::Mat points_mat(points);
        std::vector<std::vector<cv::Point> > hull(1);
        cv::convexHull(points_mat, hull[0], false);
        /*
        for (int i = 0; i < hull[0].size(); i++)
        {
            std::cout << hull[i] << std::endl;
        }
        */
        cv::drawContours(optical_flow_image, hull, -1, colour, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
    }
}

void OpticalFlowVisualizer::showFlowOutliers(const cv::Mat &original_image, cv::Mat &outlier_image, const cv::Mat &optical_flow_vectors, const cv::Mat &outlier_mask, int pixel_step, bool print)
{

    original_image.copyTo(outlier_image);

    for (int i = 0; i < optical_flow_vectors.rows; i = i + pixel_step)
    {
        for (int j = 0; j < optical_flow_vectors.cols; j = j + pixel_step)
        {
            cv::Point2f start_point;
            cv::Point2f end_point;

            cv::Vec4d elem = optical_flow_vectors.at<cv::Vec4d>(i, j);
            double outlier = outlier_mask.at<double>(i, j);

            if ((std::abs(elem[2])> 0.0 || std::abs(elem[3]) > 0.0))
            {

                if (print)
                {
                    std::cout << "Processing: " << i << ", " << j << ": "  << outlier_mask.at<double>(i,j) << " also " << elem[0] << ", " << elem[1] << std::endl;
                }
                start_point.x = elem[0];
                start_point.y = elem[1];            

                /*

                end_point.x = start_point.x + elem[3] * cos(elem[2]);
                end_point.y = start_point.y + elem[3] * sin(elem[2]);
                */

                end_point.x = start_point.x + elem[2];
                end_point.y = start_point.y + elem[3];
                cv::Scalar colour;
                if (outlier_mask.at<double>(i,j) > 0.5)
                {
                    colour = CV_RGB(0, 0, 255);
/*                }
                else
                {
                    colour = CV_RGB(0, 255, 0);
                }

                */
                cv::line(outlier_image, start_point, end_point, colour, 1, CV_AA, 0);

                double backward_angle = atan2(start_point.y - end_point.y, start_point.x - end_point.x);
                double arrow1_angle = (backward_angle + M_PI / 4.0);
                double arrow2_angle = (backward_angle - M_PI / 4.0);

                cv::Point2f arrow1_end;
                arrow1_end.x = end_point.x + 3.0 * cos(arrow1_angle);
                arrow1_end.y = end_point.y + 3.0 * sin(arrow1_angle);

                cv::Point2f arrow2_end;
                arrow2_end.x = end_point.x + 3.0 * cos(arrow2_angle);
                arrow2_end.y = end_point.y + 3.0 * sin(arrow2_angle);

                cv::line(outlier_image, end_point, arrow1_end, colour, 1, CV_AA, 0);
                cv::line(outlier_image, end_point, arrow2_end, colour, 1, CV_AA, 0);
            }
            }
        }
    }
}

std::vector<std::vector<cv::Point> > OpticalFlowVisualizer::showClusterContours(const cv::Mat &original_image, cv::Mat &clusters_image, const std::vector<std::vector<cv::Point2f> > &clusters)
{
    std::vector<std::vector<cv::Point> > contours;
    cv::Scalar colour = CV_RGB(0, 0, 255);
    original_image.copyTo(clusters_image);
    for (int i = 0; i < clusters.size(); i++)
    {
        std::vector<std::vector<cv::Point> > hull(1);
        std::vector<cv::Point2f> cluster = clusters.at(i);
        std::vector<cv::Point> clusteri;
        cv::Mat(cluster).copyTo(clusteri);
        cv::Mat points_mat(clusteri);
        cv::convexHull(points_mat, hull[0], false);
        cv::drawContours(clusters_image, hull, -1, colour, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
        contours.push_back(hull[0]);
    }
    return contours;
}

std::vector<cv::Rect> OpticalFlowVisualizer::showBoundingBoxes(const cv::Mat &original_image, cv::Mat &clusters_image, const std::vector<std::vector<cv::Point2f> > &clusters)
{
    std::vector<cv::Rect> rectangles;
    cv::Scalar colour = CV_RGB(0, 0, 255);
    original_image.copyTo(clusters_image);
    for (int i = 0; i < clusters.size(); i++)
    {
        cv::Rect rect;
        std::vector<cv::Point2f> cluster = clusters.at(i);
        std::vector<cv::Point> clusteri;
        cv::Mat(cluster).copyTo(clusteri);
        cv::Mat points_mat(clusteri);
        rect = cv::boundingRect(points_mat);
        rectangles.push_back(rect);
        cv::rectangle(clusters_image, rect.tl(), rect.br(), colour, 2, 8, 0);
    }
    return rectangles;
}
