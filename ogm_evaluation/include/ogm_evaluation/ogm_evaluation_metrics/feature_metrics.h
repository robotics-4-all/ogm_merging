/******************************************************************************
   OGM Validator - Occupancy Grid Map Validator
   Copyright (C) 2015 OGM Validator
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

   Authors :
   * Marios Protopapas, protopapasmarios@gmail.com
******************************************************************************/

#ifndef FEATURE_METRICS_H
#define FEATURE_METRICS_H

//#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include "ogm_evaluation/ogm_evaluation_metrics/metric_base.h"

/**
@namespace ogm_evaluation
@brief The main namespace for OGM Evaluation
**/ 
namespace ogm_evaluation
{
  /**
  @class FeatureMetrics
  @brief A class that provides Metrics Implementation for various feature Descriptors.
  Inherits publicly Metric
  **/ 
  class FeatureMetrics : public Metric
  {
    public:
      /**
      @brief Default Constructor
      @param groundTruthMap [const cv::Mat& ] the ground truth map
      @param slamMap[const cv::Mat&] the slam produced Map
      @param int [method] the omse method to be used
      @param int [distNorm] the distance norm to be used
      @return void
      **/
      FeatureMetrics(const cv::Mat& groundTruthMap,
                 const cv::Mat& slamMap,
                 int method,
                 int distNorm);

      /**
      @brief Default destructor
      @return void
      **/
      virtual ~FeatureMetrics(void) 
      {
      }

      /**
      @brief calculate the obstacle metric.
      @return void
      **/
      virtual void calculateMetric(void);

    private:

      //!< the omse method to be used
      int _omse_method;

      //!< the distance norm to be used
      int _distNorm;
      
      //!< the instance of opencv's FeatureDetector class to be used
      cv::Ptr<cv::FeatureDetector> _featureDetector;

      //!< the instance of opencv's DescriptorExtractor class to be used
      cv::Ptr<cv::DescriptorExtractor> _descriptorExtractor;

      //!< the instance of opencv's DescriptorMatcher class to be used
      cv::Ptr<cv::DescriptorMatcher> _matcher;

      //!< the groundTruthMap feature keypoints
      std::vector<cv::KeyPoint> _groundTruthKeypoints;
      
      //!< the slamMap feature keypoints
      std::vector<cv::KeyPoint> _slamKeypoints;

      //!< the groundTruthMap feature descriptors
      cv::Mat _groundTruthDescriptors;

      //!< the slamMap feature descriptors
      cv::Mat _slamDescriptors;
     
     //!< the Matching descriptor vectors
     std::vector< cv::DMatch > _matches;
  };
}
#endif
