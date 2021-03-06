/******************************************************************************
   OGM Validator - Occupancy Grid Map Validator
   Copyright (C) 2016 OGM Validator
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
   * Manos Tsardoulias, etsardou@gmail.com
******************************************************************************/

#include "feature_evaluation/feature_evaluation_descriptors/radius_statistics_descriptor.h"

namespace feature_evaluation
{
    /**
    @brief Default Constructor
    @return void
    **/ 
    RadiusStatisticsDescriptor::RadiusStatisticsDescriptor():DescriptorExtractor()
    {
        ROS_INFO_STREAM("Created RadiusStatisticsDescriptor instance");
    }
    
    /**
    @brief function for description extract computation.
    @param image [const cv::Mat&] the image 
    @param keypoints [std::vector<cv::Keypoint>&] the image detected keypoints
    @param descriptors [cv::Mat&] the descriptors to be extracted
    @return void
    **/
    void RadiusStatisticsDescriptor::compute(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, cv::Mat* descriptors)
    {
      int radius;
      int obstacles;
      int frees;
      float sse, dst;
      cv::Mat img;
      cv::cvtColor(image, img, CV_GRAY2RGB);
      //image.copyTo(img);
      cv::Mat desc = cv::Mat(keypoints.size(), 12, CV_32FC1);
      std::vector<float> rowFeatures;
      for (int k = 0; k < keypoints.size(); k++)
      {
        radius = 0;
        rowFeatures.clear();
        for (int l = 0; l < 6; l++)
        {
          radius+= 25;
          obstacles = 0;
          frees = 0;
          sse = 0;
          cv::Mat mask = cv::Mat::zeros(image.rows, image.cols, CV_8U);
          cv::circle(mask, keypoints[k].pt, radius, cv::Scalar(255), -1);
       /*  cv::imshow("mask", mask);*/
         /*cv::waitKey(0);*/
          for(unsigned int i = 0; i < image.rows; i++)
            for(unsigned int j = 0; j < image.cols; j++)
              if(mask.at<unsigned char>(i, j) > 0)
              {
                //pixel (i,j) in original image is within that circle so do whatever you want.
                if(image.at<unsigned char>(i, j) == 0)
                {
                  obstacles++;
                  dst = cv::sqrt(pow((i-keypoints[k].pt.x), 2) + pow((j-keypoints[k].pt.y), 2));
                  //sse += dst*dst;
                }
                else if(image.at<unsigned char>(i, j) == 255)
                  frees++;
              }
          //sse = sse / obstacles;
          rowFeatures.push_back((float)obstacles / (obstacles + frees));
          rowFeatures.push_back((float)frees / (obstacles + frees));
          //rowFeatures.push_back(sse);
          cv::circle(img, keypoints[k].pt, radius, cv::Scalar(255, 0, 0), 1, 8);
        }
        for (int i = 0; i < rowFeatures.size(); i++)
        {
          desc.at<float>(k, i) = rowFeatures[i];
          //std::cout << rowFeatures[i] << " ";
        }
        //std::cout << std::endl;
      }
      cv::imshow("Radius Descriptors", img);
      cv::waitKey(1000);
      //ROS_INFO_STREAM("DESC=" <<desc.rows << " " << desc.cols);
      desc.copyTo(*descriptors);
    }
} // namespace feature_evaluation
