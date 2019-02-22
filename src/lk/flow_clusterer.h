/* flow_clusterer.h
 *
 * Copyright (C) 2014 Santosh Thoduka
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef FLOW_CLUSTERER_H_
#define FLOW_CLUSTERER_H_

#include <opencv2/core/core.hpp>

class FlowClusterer
{

    public:
        FlowClusterer();
        virtual ~FlowClusterer();

        cv::Mat clusterFlowVectors(const cv::Mat &flow_vectors);

        std::vector<cv::Point2f> getClustersCenters(const cv::Mat &flow_vectors, int pixel_step, double distance_threshold, double angular_threshold);

        std::vector<std::vector<cv::Vec4d> > getClusters(const cv::Mat &flow_vectors, int pixel_step, double distance_threshold, double angular_threshold);
        std::vector<std::vector<cv::Point2f> > clusterEuclidean(const std::vector<cv::Point2f> &points, double distance_threshold);
};
#endif
