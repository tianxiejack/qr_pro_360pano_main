/* point_cluster.cpp
 *
 * Copyright (C) 2014 Santosh Thoduka
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <point_cluster.h>
#include <iostream>

PointCluster::PointCluster()
{
}

PointCluster::~PointCluster()
{
}

void PointCluster::addPoint(const cv::Point2f &point)
{
    cluster_.push_back(point);
}

double PointCluster::getClosestDistance(const cv::Point2f &point)
{
    double min_distance = std::numeric_limits<double>::max();
    for (int i = 0; i < cluster_.size(); i++)
    {
        double distance = getDistance(point, cluster_.at(i));
        if (distance < min_distance)
        {
            min_distance = distance;
        }
    }
    return min_distance;
}

cv::Point2f PointCluster::getCentroid()
{
    double x_sum = 0.0;
    double y_sum = 0.0;
    for (int i = 0; i < cluster_.size(); i++)
    {
        x_sum += cluster_.at(i).x;
        y_sum += cluster_.at(i).y;
    }
    x_sum /= cluster_.size();
    y_sum /= cluster_.size();
    cv::Point2f centroid(x_sum, y_sum);
    return centroid;
}

std::vector<cv::Point2f> PointCluster::getCluster()
{
    return cluster_;
}

int PointCluster::size()
{
    return cluster_.size();
}
double PointCluster::getDistance(const cv::Point2f &one, const cv::Point2f &two)
{
    return sqrt((one.x - two.x)*(one.x - two.x) + (one.y - two.y) * (one.y - two.y));
}

void PointCluster::getMinMax(double &min_x, double &max_x, double &min_y, double &max_y)
{
    min_x = std::numeric_limits<double>::max();
    max_x = -1.0;
    min_y = std::numeric_limits<double>::max();
    max_y = -1.0;
    for (int i = 0; i < cluster_.size(); i++)
    {
        double x = cluster_.at(i).x;
        double y = cluster_.at(i).y;
        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
        if (x > max_x) max_x = x;
        if (y > max_y) max_y = y;
    }
}
