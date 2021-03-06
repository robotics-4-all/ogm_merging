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

#include "ogm_gui/ogm_gui_connector.h"

namespace ogm_gui
{

  /**
  @brief Default contructor
  @param argc [int] Number of input arguments
  @param argv [char **] Input arguments
  @return void
  **/
  CGuiConnector::CGuiConnector(int argc, char **argv):
    QObject(),
    loader_(argc,argv),
    argc_(argc),
    argv_(argv)
  {
    bool map_initialized_ = false;

    QObject::connect(
      loader_.actionAbout_map_validator,SIGNAL(triggered(bool)),
      this,SLOT(actionAboutTriggered()));

    QObject::connect(
      loader_.actionExit,SIGNAL(triggered(bool)),
      this,SLOT(actionExitTriggered()));

    QObject::connect(
      loader_.actionLoadMap,SIGNAL(triggered(bool)),
      this,SLOT(actionLoadMapTriggered()));

    QObject::connect(
      loader_.actionLoadSlamMap,SIGNAL(triggered(bool)),
      this,SLOT(actionLoadSlamMapTriggered()));

    QObject::connect(
      loader_.actionLoadMapsFromServer,SIGNAL(triggered(bool)),
      this,SLOT(actionLoadMapsFromServerTriggered()));

    QObject::connect(
      loader_.actionGrid,SIGNAL(triggered(bool)),
      this,SLOT(actionGridTriggered()));

   grid_enabled_ = false;
  }

  /**
  @brief Qt slot that is called when the Exit action is triggered
  @return void
  **/
  void CGuiConnector::actionExitTriggered(void)
  {
    ROS_INFO("Exiting GUI...");
    exit(0);
  }


  /**
  @brief Qt slot that is called when the LoadMap tool button is pressed
  @return void
  **/
  void CGuiConnector::actionLoadMapTriggered(void)
  {
     QString file_name = QFileDialog::getOpenFileName( 
       &loader_,
       tr("Load map"),
       QString().fromStdString(
         ogm_gui_tools::getRosPackagePath("ogm_resources") +
           std::string("/maps")),
         tr("Yaml map files (*.yaml)"));
     if(file_name.isEmpty() || file_name.isNull())
     {
       return;
     }

     Q_EMIT requestMap(file_name, true);

  }

  /**
  @brief Qt slot that is called when the LoadSlamMap tool button is pressed
  @return void
  **/
  void CGuiConnector::actionLoadSlamMapTriggered(void)
  {
     QString file_name = QFileDialog::getOpenFileName( 
       &loader_,
       tr("Load SLAM-produced map"),
       QString().fromStdString(
         ogm_gui_tools::getRosPackagePath("ogm_resources") +
           std::string("/maps")),
         tr("Yaml map files (*.yaml)"));
     if(file_name.isEmpty() || file_name.isNull())
     {
       return;
     }

     Q_EMIT requestMap(file_name, false);
  }

  /**
  @brief Qt slot that is called when the LoadMapsfromServer tool button is pressed
  @return void
  **/
  void CGuiConnector::actionLoadMapsFromServerTriggered(void)
  {
    Q_EMIT loadDefaultMaps();
  }

  /**
  @brief Qt slot that is called when the About tool button is pressed
  @return void
  **/
  void CGuiConnector::actionAboutTriggered(void)
  {
    QMessageBox msg(static_cast<QMainWindow *>(&this->loader_));
    msg.setWindowTitle(QString("OGM Validator - About"));
    msg.setText(QString(" "));
    msg.exec();
  }

  /**
  @brief Qt slot that is called when the grid status has changed
  @return void
  **/
  void CGuiConnector::actionGridTriggered(void)
  {
    if ( ! map_initialized_ )
    {
      return;
    }
    grid_enabled_ =! grid_enabled_;
  }

  /**
  @brief Returns the grid enabled state
  @return bool : True if grid is enabled
  **/
  bool CGuiConnector::isGridEnabled(void)
  {
    return grid_enabled_;
  }

  /**
  @brief Adds a widget to the main window Qt grid
  @param w [QWidget*] The widget to be placed
  @param row [int] The row of the grid
  @param column [int] The column of the grid
  @return void
  **/
  void CGuiConnector::addToGrid(QWidget *w, int row, int column, 
                                int rowSpan, int columnSpan)
  {
    if(rowSpan != 0 || columnSpan != 0)
    {
      loader_.gridLayout->addWidget(w, row, column, rowSpan, columnSpan, 0);
    }
    else
      loader_.gridLayout->addWidget(w, row, column, 0);
  }

  /**
  @brief Wraps the Qt gridColumnStretch function
  @param cell [int] The specific column
  @param stretch [int] The relative stretch coefficient
  @return void
  **/
  void CGuiConnector::setGridColumnStretch(int cell, int stretch)
  {
    loader_.gridLayout->setColumnStretch(cell, stretch);
  }

  /**
  @brief Shows the main window
  @return void
  **/
  void CGuiConnector::show(void)
  {
    loader_.show();
    loader_.showMaximized();
  }

  /**
  @brief Displays a message in the QMainWindow status bar
  @param s [QString] The message
  @return void
  **/
  void CGuiConnector::setStatusBarMessage(QString s)
  {
    loader_.statusbar->showMessage(s, 0);
  }

  /**
  @brief Returns the exit event captured
  @return QEvent* The captured event
  **/
  QEvent* CGuiConnector::getCloseEvent(void)
  {
    return loader_.getCloseEvent();
  }

  /**
  @brief Returns the exit triggered status
  @return bool True if exit has been triggered
  **/
  bool CGuiConnector::closeTriggered(void)
  {
    return loader_.closeTriggered();
  }

  /**
  @brief Shuts down the main window
  @return void
  **/
  void CGuiConnector::shutdown(void)
  {
    loader_.shutdown();
  }

  /**
  @brief Sets the map_initialized_ private variable
  @param mi [bool] The new value
  @return void
  **/
  void CGuiConnector::setMapInitialized(bool mi)
  {
    map_initialized_ = mi;
  }

  /**
  @brief Raises a message box with a specific message
  @param title [QString] The message box title
  @param s [QString] The message
  @return void
  **/
  void CGuiConnector::raiseMessage(QString title, QString s)
  {
    QMessageBox msg(static_cast<QMainWindow *>(&this->loader_));
    msg.setWindowTitle(title);
    msg.setText(s);
    msg.exec();
  }
}
