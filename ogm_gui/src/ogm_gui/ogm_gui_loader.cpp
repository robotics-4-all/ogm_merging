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
   * Manos Tsardoulias, etsardou@gmail.com
******************************************************************************/

#include "ogm_gui/ogm_gui_loader.h"

namespace ogm_gui
{
  /**
  @brief Default contructor
  @param argc [int] Number of input arguments
  @param argv [char **] Input arguments
  @return void
  **/
  CGuiLoader::CGuiLoader(int argc,char **argv):
    argc_(argc),
    argv_(argv)
  {
    setupUi(this);

    addToolbarIcons();
    close_signal_ = false;
  }

  /**
  @brief Adds the tool buttons in the main window toolbar
  @return void
  **/
  void CGuiLoader::addToolbarIcons(void)
  {

    actionLoadMap = new QAction(this);
    actionLoadMap->setObjectName(QString::fromUtf8("actionLoadMap"));
    actionLoadMap->setCheckable(false);
    actionLoadMap->setIconText(QString("Load map"));
    QIcon iconLoadMap;
    iconLoadMap.addFile(QString::fromUtf8((
      ogm_gui_tools::getRosPackagePath("ogm_gui") +
        std::string("/resources/images/load_map.png")).c_str()),
      QSize(),
      QIcon::Normal,
      QIcon::Off);
    actionLoadMap->setIcon(iconLoadMap);
    toolBar->addAction(actionLoadMap);

    toolBar->addSeparator();

    actionGrid = new QAction(this);
    actionGrid->setObjectName(QString::fromUtf8("actionGrid"));
    actionGrid->setCheckable(true);
    actionGrid->setChecked(false);
    actionGrid->setIconText(QString("Enable grid"));
    QIcon iconGrid;
    iconGrid.addFile(QString::fromUtf8((
      ogm_gui_tools::getRosPackagePath("ogm_gui") +
        std::string("/resources/images/grid.png")).c_str()),
      QSize(),
      QIcon::Normal,
      QIcon::Off);
    actionGrid->setIcon(iconGrid);
    toolBar->addAction(actionGrid);

    actionZoomIn = new QAction(this);
    actionZoomIn->setObjectName(QString::fromUtf8("actionZoomIn"));
    actionZoomIn->setCheckable(true);
    actionZoomIn->setIconText(QString("Zoom in"));
    QIcon iconZoomIn;
    iconZoomIn.addFile(QString::fromUtf8((
      ogm_gui_tools::getRosPackagePath("ogm_gui") +
        std::string("/resources/images/zoom_in_b.png")).c_str()),
      QSize(),
      QIcon::Normal,
      QIcon::Off);
    actionZoomIn->setIcon(iconZoomIn);
    toolBar->addAction(actionZoomIn);

    actionZoomOut = new QAction(this);
    actionZoomOut->setObjectName(QString::fromUtf8("actionZoomOut"));
    actionZoomOut->setCheckable(true);
    actionZoomOut->setIconText(QString("Zoom out"));
    QIcon iconZoomOut;
    iconZoomOut.addFile(QString::fromUtf8((
      ogm_gui_tools::getRosPackagePath("ogm_gui") +
        std::string("/resources/images/zoom_out_b.png")).c_str()),
      QSize(),
      QIcon::Normal,
      QIcon::Off);
    actionZoomOut->setIcon(iconZoomOut);
    toolBar->addAction(actionZoomOut);

    actionAdjusted = new QAction(this);
    actionAdjusted->setObjectName(QString::fromUtf8("actionAdjusted"));
    actionAdjusted->setCheckable(false);
    actionAdjusted->setChecked(true);
    actionAdjusted->setIconText(QString("Adjust size"));
    QIcon iconAdjust;
    iconAdjust.addFile(QString::fromUtf8((
      ogm_gui_tools::getRosPackagePath("ogm_gui") +
        std::string("/resources/images/adjusted.png")).c_str()),
      QSize(),
      QIcon::Normal,
      QIcon::Off);
    actionAdjusted->setIcon(iconAdjust);
    toolBar->addAction(actionAdjusted);

    toolBar->setIconSize(QSize(30,30));
  }

  /**
  @brief Overloading of closeEvent function from QMainWindow
  @param event [QCloseEvent*] The exit event
  @return void
  **/
  void CGuiLoader::closeEvent(QCloseEvent *event)
  {
    //~ ROS_ERROR("Shutdown signal!");
    if(close_signal_)
    {
      event->accept();
      //~ ROS_ERROR("Shutting down ros...");
      ros::shutdown();
      exit(0);
      return;
    }
    close_signal_ = true;
    event->ignore();
    event_ = event;
  }
  
  /**
  @brief Returns the exit event
  @return QEvent* 
  **/
  QEvent* CGuiLoader::getCloseEvent(void)
  {
    return event_;
  }
  
  /**
  @brief Returns true if a close event was triggered
  @return bool
  **/
  bool CGuiLoader::closeTriggered(void)
  {
    return close_signal_;
  }
  
  /**
  @brief Shuts down the main window
  @return void
  **/
  void CGuiLoader::shutdown(void)
  {
    this->close();
  }
}
