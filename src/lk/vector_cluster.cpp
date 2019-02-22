/* vector_cluster.cpp
 *
 * Copyright (C) 2014 Santosh Thoduka
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <vector_cluster.h>
#include <iostream>

VectorCluster::VectorCluster()
{
}

VectorCluster::~VectorCluster()
{
}

void VectorCluster::addVector(const cv::Vec4d & vector)
{
    cluster_.push_back(vector);
}

double VectorCluster::getClosestDistance(const cv::Vec4d &vector)
{
    double min_distance = std::numeric_limits<double>::max();
    for (int i = 0; i < cluster_.size(); i++)
    {
        double distance = getDistance(vector, cluster_.at(i));
        if (distance < min_distance)
        {
            min_distance = distance;
        }
    }
    return min_distance;
}
double VectorCluster::getClosestOrientation(const cv::Vec4d &vector)
{
    double min_angular_distance = std::numeric_limits<double>::max();
    for (int i = 0; i < cluster_.size(); i++)
    {
        double angular_distance = abs(getAngularDistance(vector, cluster_.at(i)));
        if (angular_distance < min_angular_distance)
        {
            min_angular_distance = angular_distance;
        }
    }
    return min_angular_distance;
}

cv::Point2f VectorCluster::getCentroid()
{
    double x_sum = 0.0;
    double y_sum = 0.0;
    for (int i = 0; i < cluster_.size(); i++)
    {
        x_sum += cluster_.at(i)[0];
        y_sum += cluster_.at(i)[1];
    }
    x_sum /= cluster_.size();
    y_sum /= cluster_.size();
    cv::Point2f centroid(x_sum, y_sum);
    return centroid;
}

double VectorCluster::getMeanOrientation()
{
    double angle_sum = 0.0;
    for (int i = 0; i < cluster_.size(); i++)
    {
        angle_sum += getAngle(cluster_.at(i));
    }
    angle_sum /= cluster_.size();
    return angle_sum;
}

std::vector<cv::Vec4d> VectorCluster::getCluster()
{
    return cluster_;
}

std::vector<cv::Point2f> VectorCluster::getClusterPoints()
{
    std::vector<cv::Point2f> cluster_points;
    for (int i = 0; i < cluster_.size(); i++)
    {
        cv::Point2f p(cluster_.at(i)[0], cluster_.at(i)[1]);
        cluster_points.push_back(p);
    }
    return cluster_points;
}

std::vector<cv::Vec4d> VectorCluster::getMeanVector()
{
    cv::Point2f centroid = getCentroid();
    double orientation = getMeanOrientation();
    double magnitude = 50.0;
    cv::Point2f diff;    
    diff.x = magnitude * cos(orientation);
    diff.y = magnitude * sin(orientation);
    std::vector<cv::Vec4d> mean_vector;
    cv::Vec4d v;
    v[0] = centroid.x;
    v[1] = centroid.y;
    v[2] = diff.x;
    v[3] = diff.y;
    //std::cout << "centroid: " << v[0] << ", " << v[1] << std::endl;
    mean_vector.push_back(v);
    return mean_vector;

}

int VectorCluster::size()
{
    return cluster_.size();
}
double VectorCluster::getDistance(const cv::Vec4d &one, const cv::Vec4d &two)
{
    return sqrt((one[0] - two[0])*(one[0] - two[0]) + (one[1] - two[1]) * (one[1] - two[1]));
}

double VectorCluster::getAngularDistance(const cv::Vec4d &one, const cv::Vec4d &two)
{

    double angle1 = getAngle(one);
    double angle2 = getAngle(two);
    return atan2(sin(angle1 - angle2), cos(angle1 - angle2));    
}

double VectorCluster::getAngle(const cv::Vec4d &vector)
{
    double ang = atan2(vector[3], vector[2]);
    if (ang < 0.0)
    {
        ang += 2 * M_PI;
    }
    return ang;
}

void VectorCluster::getMinMax(double &min_x, double &max_x, double &min_y, double &max_y)
{
    min_x = std::numeric_limits<double>::max();
    max_x = -1.0;
    min_y = std::numeric_limits<double>::max();
    max_y = -1.0;
    for (int i = 0; i < cluster_.size(); i++)
    {
        double x = cluster_.at(i)[0];
        double y = cluster_.at(i)[1];
        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
        if (x > max_x) max_x = x;
        if (y > max_y) max_y = y;
    }
}
