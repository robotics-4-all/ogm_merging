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
#include "ogm_evaluation/ogm_evaluation.h"

namespace ogm_evaluation
{
  /**
  @class MapEvaluator
  @brief Implements the OGM evaluations methods
  **/ 
  MapEvaluator::MapEvaluator(int argc, char** argv)
    {
      _mapEvaluationService =  _nh.advertiseService(
          "ogm_evaluation/map_evaluation", &MapEvaluator::evaluationCallback, this);
    }

    /**
    @brief Service callback for performing map evaluation
    @param req [ogm_msgs::ServerRequestEvaluation::Request&] The service request
    @param res [ogm_msgs::ServerRequestEvaluation::Response&] The service response
    @return bool
    **/
    bool MapEvaluator::evaluationCallback(ogm_msgs::ServerRequestEvaluation::Request& req,
      ogm_msgs::ServerRequestEvaluation::Response& res)
    {
      _groundTruthMap = mapToMat(req.groundTruthMap);
      _slamMap = mapToMat(req.slamMap);
      ROS_INFO_STREAM("GROUND TRUTH MAP SIZE=" << _groundTruthMap.size() << "type=" << _groundTruthMap.type() << "channels="<< _groundTruthMap.channels());
      ROS_INFO_STREAM("SLAM MAP SIZE=" << _slamMap.size()<<  "type=" << _slamMap.type() << "channels=" <<_slamMap.channels());
      _transform = req.transform;

    if (req.manualAlignment)
      alignMaps();
    bool s = req.manualAlignment;
    ROS_INFO_STREAM("manual alignment=" << s);

    if (req.binary)
    {
      for (int i = 0; i < _groundTruthMap.rows; i++)
        for (int j = 0; j < _groundTruthMap.cols; j++)
          if(_groundTruthMap.at<uchar>(i, j) == 127)
            _groundTruthMap.at<uchar>(i, j) = 255;

      for (int i = 0; i < _slamMap.rows; i++)
        for (int j = 0; j < _slamMap.cols; j++)
          if(_slamMap.at<uchar>(i, j) == 127)
            _slamMap.at<uchar>(i, j) = 255;

/*      cv::threshold(_groundTruthMap, _groundTruthMap, 0, 255, cv::THRESH_BINARY);*/
      /*cv::threshold(_slamMap, _slamMap, 0, 255, cv::THRESH_BINARY);*/
    }
  /*  cv::erode(_groundTruthMap, _groundTruthMap, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);*/
    /*cv::erode(_slamMap, _slamMap, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);*/

    _metric =  _metric_factory.createMetricInstance(req.method, _groundTruthMap, _slamMap, req.detector, req.descriptor, req.matcher, req.closestPointMethod, req.distNorm, req.matchingRatio, req.ransacReprjError);
    _metric->calculateMetric();
    res.result = _metric->getResult();
   /* cv::imshow("groundTruthMap", _groundTruthMap );*/
    //cv::waitKey(30);
    //cv::imshow("slamMap", _slamMap);
    /*cv::waitKey(30);*/
 /*   cv::imshow("diff", diff);*/
    /*cv::waitKey(30);*/

    return true;
  }

  /**
  @brief Method that converts nav_msgs/OccupancyGrid to cv::Mat
  @oaram map [const nav_msgs::OccupancyGrid&] the OccupancyGrid map
  @return void
  **/
  cv::Mat MapEvaluator::mapToMat(const nav_msgs::OccupancyGrid& map)
  {
    cv::Mat im(map.info.height, map.info.width, CV_8UC1);
    
    if (map.info.height*map.info.width != map.data.size())
    {
        ROS_ERROR("Data size doesn't match width*height: width = %d, height = %d, data size = %zu", map.info.width, map.info.height, map.data.size());
    }

    for (size_t i = 0; i < map.info.height * map.info.width; i++)
    {
        if (map.data.at(i) == -1)
        {
            im.data[i] = 127;
        }
        else
        {
          im.data[i] = (100.0 - map.data.at(i)) / 100.0 * 255.0;
        }
    }
    cv::Mat flipped;
    cv::flip(im, flipped, 0);
    return flipped;
  }

  /**
  @brief Method that aligns the two maps using the transform received from server
  @return void
  **/
  void MapEvaluator::alignMaps()
  {
    cv::Mat transformMat;

    // resize ground truth map to offset scale
    cv::resize(_groundTruthMap, _groundTruthMap, cv::Size(), _transform.groundTruthOffsetScale, _transform.groundTruthOffsetScale, cv::INTER_NEAREST);

    // construct the transformation Matrix
    cv::Point2f center(_groundTruthMap.cols/2.0,  _groundTruthMap.rows/2.0);
    transformMat = cv::getRotationMatrix2D(center, -_transform.pose.theta, _transform.scale);

    transformMat.at<double>(0,2) += _transform.pose.x;
    transformMat.at<double>(1,2) += _transform.pose.y;

    // apply the transformation
    cv::warpAffine(_groundTruthMap, _groundTruthMap, transformMat, _groundTruthMap.size(), cv::INTER_NEAREST, IPL_BORDER_CONSTANT, cv::Scalar::all(127));

    // resize slam produced map to offset scale
    cv::resize(_slamMap, _slamMap, cv::Size(), _transform.slamOffsetScale,  _transform.slamOffsetScale, cv::INTER_NEAREST);

    ROS_INFO_STREAM("GROUND TRUTH MAP SIZE after scaling=" << _groundTruthMap.size());
    ROS_INFO_STREAM("SLAM MAP SIZE after scaling =" << _slamMap.size());

    //"unknown" padding to fix images size
    int paddedWidth, paddedHeight;
    paddedHeight = abs(_slamMap.rows - _groundTruthMap.rows);
    paddedWidth = abs(_slamMap.cols - _groundTruthMap.cols);

    ROS_INFO_STREAM("PADDED WIDTH/HEIGHT=" << paddedWidth << " " << paddedHeight);

    if(_slamMap.rows >= _groundTruthMap.rows)
      cv::copyMakeBorder(_groundTruthMap, _groundTruthMap, 0, paddedHeight, 0, 0, IPL_BORDER_CONSTANT, cv::Scalar(127));
    else
      cv::copyMakeBorder(_slamMap, _slamMap, 0, paddedHeight, 0, 0, IPL_BORDER_CONSTANT, cv::Scalar(127));

    if(_slamMap.cols >= _groundTruthMap.cols)
      cv::copyMakeBorder(_groundTruthMap, _groundTruthMap, 0, 0, 0, paddedWidth, IPL_BORDER_CONSTANT, cv::Scalar(127));
    else
      cv::copyMakeBorder(_slamMap, _slamMap, 0, 0, 0, paddedWidth, IPL_BORDER_CONSTANT, cv::Scalar(127));

    cv::absdiff(_groundTruthMap, _slamMap, diff);

    ROS_INFO_STREAM("GROUND TRUTH MAP SIZE after padding=" << _groundTruthMap.size());
    ROS_INFO_STREAM("SLAM MAP SIZE after padding =" << _slamMap.size());
    ROS_INFO_STREAM("X=" << _transform.pose.x);
    ROS_INFO_STREAM("Y=" << _transform.pose.y);
    ROS_INFO_STREAM("theta=" << _transform.pose.theta);
    ROS_INFO_STREAM("scale=" << _transform.scale);
    ROS_INFO_STREAM("SlamScale=" << _transform.slamOffsetScale);
    ROS_INFO_STREAM("GroundScale=" << _transform.groundTruthOffsetScale);
  }

}  // namespace ogm_evaluation
