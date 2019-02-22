/* vector_cluster.h
 *
 * Copyright (C) 2014 Santosh Thoduka
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef VECTOR_CLUSTER_H_
#define VECTOR_CLUSTER_H_

#include <opencv2/core/core.hpp>

class VectorCluster
{
    public:
        VectorCluster();
        virtual ~VectorCluster();

        void addVector(const cv::Vec4d &vector);
        double getClosestDistance(const cv::Vec4d &vector);
        double getClosestOrientation(const cv::Vec4d &vector);
        cv::Point2f getCentroid();
        double getMeanOrientation();
        
        std::vector<cv::Vec4d> getCluster();
        std::vector<cv::Point2f> getClusterPoints();
        std::vector<cv::Vec4d> getMeanVector();
        int size();


    private:
        double getDistance(const cv::Vec4d &one, const cv::Vec4d &two);
        double getAngularDistance(const cv::Vec4d &one, const cv::Vec4d &two);
        double getAngle(const cv::Vec4d &vector);
        void getMinMax(double &min_x, double &max_x, double &min_y, double &max_y);


    private:
        std::vector<cv::Vec4d> cluster_;
};
#endif
