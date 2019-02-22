/* flow_clusterer.cpp
 *
 * Copyright (C) 2014 Santosh Thoduka
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <flow_clusterer.h>
#include <vector_cluster.h>
#include <point_cluster.h>
#include <opencv2/flann/flann_base.hpp>
#include <opencv2/features2d/features2d.hpp>

FlowClusterer::FlowClusterer()
{
}

FlowClusterer::~FlowClusterer()
{
}

cv::Mat FlowClusterer::clusterFlowVectors(const cv::Mat &flow_vectors)
{
    //std::cout << "started" << std::endl;

//    std::cout << "hmm 1" << flow_vectors.rows << ", " << flow_vectors.cols << std::endl;
    cv::Mat samples = cv::Mat::zeros(flow_vectors.rows * flow_vectors.cols, 2, CV_32F);    
  //  std::cout << "hmm " << std::endl;
    int num_samples = 0;
    for (int i = 0; i < flow_vectors.rows; i = i + 20)
    {
        for (int j = 0; j < flow_vectors.cols; j = j + 20)
        {
            //std::cout << "yep " << std::endl;
           // std::cout << flow_vectors.at<cv::Vec4d>(i,j)[3] << std::endl;
            if(flow_vectors.at<cv::Vec4d>(i,j)[3] > 0.0)
            {
               // std::cout << "reached here (" << i <<", " << j << "), "<< num_samples << ": ";          
                for (int f = 0; f < 2; f++)
                {
                    samples.at<float>(num_samples, f) = flow_vectors.at<cv::Vec4d>(i,j)[f];
                    //std::cout << samples.at<float>(num_samples, f) << ", ";
                }
                //std::cout << std::endl;
                num_samples++;
            }
        }
    }
    cvflann::KMeansIndexParams kmeans_params(3000, 100, cvflann::FLANN_CENTERS_KMEANSPP);
 //   std::cout << "done collecting samples" << std::endl;

    samples = samples.rowRange(cv::Range(0,num_samples));
    cvflann::Matrix<float> samplesMatrix((float*)samples.data, samples.rows, samples.cols);
//    std::cout << "done setting samples matrix" << std::endl;
    cv::Mat centers(10, 2, CV_32F);
    cvflann::Matrix<float> centersMatrix((float*)centers.data, centers.rows, centers.cols);


   // std::cout << "started clustering " << std::endl;
    int num_clusters = cvflann::hierarchicalClustering<cvflann::L2<float> >(samplesMatrix, centersMatrix, kmeans_params);
   // std::cout << "finished clustering " << std::endl;

    centers = centers.rowRange(cv::Range(0, num_clusters));
    /*
    std::cout << "centers: " << std::endl;
    for(int i = 0; i < num_clusters; i++)
    {
        for (int j = 0; j < centersMatrix.cols; j++)
        {
            std::cout << centersMatrix[i][j] << ", ";
        }
        std::cout << std::endl;
    }
    std::cout <<"num_clusters: " << num_clusters << std::endl;
    */
    return centers;
}

std::vector<cv::Point2f> FlowClusterer::getClustersCenters(const cv::Mat &flow_vectors, int pixel_step, double distance_threshold, double angular_threshold)
{
    std::vector<VectorCluster> clusters;
    for (int i = 0; i < flow_vectors.rows; i = i + pixel_step)
    {
        for (int j = 0; j < flow_vectors.cols; j = j + pixel_step)
        {
            cv::Vec4d vec = flow_vectors.at<cv::Vec4d>(i, j);
            if (std::abs(vec[2]) > 0.0 || std::abs(vec[3]) > 0.0)
            {
                bool added = false;
                for (int k = 0; k < clusters.size(); k++)
                {
                    if (clusters.at(k).getClosestDistance(vec) < distance_threshold && 
                        clusters.at(k).getClosestOrientation(vec) < angular_threshold)
                    {
                        clusters.at(k).addVector(vec);
                        added = true;
                    }
                }
                if (!added)
                {
                    VectorCluster vc;
                    vc.addVector(vec);
                    clusters.push_back(vc);
                }
            }
        }
    }
    std::vector<cv::Point2f> centroids;
    for (int i = 0; i < clusters.size(); i++)
    {
        centroids.push_back(clusters.at(i).getCentroid());
    }
    return centroids;
}

std::vector<std::vector<cv::Vec4d> > FlowClusterer::getClusters(const cv::Mat &flow_vectors, int pixel_step, double distance_threshold, double angular_threshold)
{
#ifdef NEW_THING 
    std::vector<VectorCluster> clusters;
    for (int i = 0; i < flow_vectors.rows; i = i + pixel_step) 
    {
        for (int j = 0; j < flow_vectors.cols; j = j + pixel_step)
        {
            cv::Vec4d vec = flow_vectors.at<cv::Vec4d>(i, j);
            if (std::abs(vec[2]) > 0.0 || std::abs(vec[3]) > 0.0)
            {
                bool added = false;
                int index = -1;
                
                // TODO: test this, find best weighting for distance and angle
                double best_score = std::numeric_limits<double>::max();
                double closest_distance = std::numeric_limits<double>::max();
                double closest_angle = std::numeric_limits<double>::max();
                for (int k = 0; k < clusters.size(); k++)
                {
                    double distance = clusters.at(k).getClosestDistance(vec);
                    double angle = clusters.at(k).getClosestOrientation(vec);
                    if ((distance + angle * 10.0) < best_score)
                    {
                        best_score = (distance + angle * 10.0);
                        index = k;
                    }
                }
                if (index != -1 && closest_distance < distance_threshold && closest_angle < angular_threshold)
                {
                    clusters.at(index).addVector(vec);
                }
                else
                {
                    VectorCluster vc;
                    vc.addVector(vec);
                    clusters.push_back(vc);
                }
            }
        }
    }
    std::vector<std::vector<cv::Vec4d> > mat_clusters;    
    //std::cout << "clusters_zero: " << std::endl;
    for (int i = 0; i < clusters.size(); i++)
    {
//        cv::Mat cluster_points(clusters.at(i).getCluster(), true);
        if (clusters.at(i).size() > 5)
        {
            //mat_clusters.push_back(clusters.at(i).getMeanVector());
            mat_clusters.push_back(clusters.at(i).getCluster());
        }
    }
    /*
    std::cout << "clusters_first: " << std::endl;
    for (int i = 0; i < mat_clusters.size(); i++)
    {
        std::cout << mat_clusters.at(i) << std::endl;
    }
    */
    return mat_clusters;
#endif
    int count = 0;
    std::vector<VectorCluster> clusters;
    for (int i = 0; i < flow_vectors.rows; i = i + pixel_step)
    {
        for (int j = 0; j < flow_vectors.cols; j = j + pixel_step)
        {
            cv::Vec4d vec = flow_vectors.at<cv::Vec4d>(i, j);
            if (std::abs(vec[2]) > 0.0 || std::abs(vec[3]) > 0.0)
            {
                count++;
                bool added = false;
                for (int k = 0; k < clusters.size(); k++)
                {
                    if (clusters.at(k).getClosestDistance(vec) < distance_threshold && 
                        clusters.at(k).getClosestOrientation(vec) < angular_threshold)
                    {
                        clusters.at(k).addVector(vec);
                        added = true;
                        break;
                    }
                }
                if (!added)
                {
                    VectorCluster vc;
                    vc.addVector(vec);
                    clusters.push_back(vc);
                }
            }
        }
    }
    std::vector<std::vector<cv::Vec4d> > mat_clusters;    
    //std::cout << "clusters_zero: " << std::endl;
    for (int i = 0; i < clusters.size(); i++)
    {
//        cv::Mat cluster_points(clusters.at(i).getCluster(), true);
        if (clusters.at(i).size() > 5)
        {
            //mat_clusters.push_back(clusters.at(i).getMeanVector());
            mat_clusters.push_back(clusters.at(i).getCluster());
        }
    }
    /*
    std::cout << "clusters_first: " << std::endl;
    for (int i = 0; i < mat_clusters.size(); i++)
    {
        std::cout << mat_clusters.at(i) << std::endl;
    }
    */
    std::cout << "num vectors: " << count << std::endl;
    return mat_clusters; 

}

std::vector<std::vector<cv::Point2f> > FlowClusterer::clusterEuclidean(const std::vector<cv::Point2f> &points, double distance_threshold)
{
    std::vector<PointCluster> clusters;
    for (int i = 0; i < points.size(); i++)
    {
        cv::Point2f point = points.at(i);
        bool added = false;
        double closest_distance = std::numeric_limits<double>::max(); 
        int index = -1;
        for (int k = 0; k < clusters.size(); k++)
        {
            double distance = clusters.at(k).getClosestDistance(point);
            if (distance < closest_distance)
            {
                closest_distance = distance;
                index = k;
            }
        }
        if (index != -1 && closest_distance < distance_threshold)
        {
            clusters.at(index).addPoint(point);
        }
        else
        {
            PointCluster pc;
            pc.addPoint(point);
            clusters.push_back(pc);
        }
    }
    std::vector<std::vector<cv::Point2f> > mat_clusters;    
    for (int i = 0; i < clusters.size(); i++)
    {
        if (clusters.at(i).size() > 5)
        {
            mat_clusters.push_back(clusters.at(i).getCluster());
        }
    }
    return mat_clusters;
}
