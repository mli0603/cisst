/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ali Uneri
  Created on: 2009-11-06

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief An example interface for Claron Micron Tracker.
  \ingroup devicesTutorial
*/

#include <cisstCommon/cmnPath.h>
#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstDevices/devMicronTracker.h>

#include <QApplication>
#include <QMainWindow>

#include "devMicronTrackerControllerQDevice.h"
#include "devMicronTrackerToolQDevice.h"


int main(int argc, char *argv[])
{
    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

    // add a log per thread
    osaThreadedLogFile threadedLog("example7-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // set the log level of detail on select tasks
    cmnClassRegister::SetLoD("devMicronTracker", CMN_LOG_LOD_RUN_WARNING);

    // create a Qt user interface
    QApplication application(argc, argv);

    // create the tasks
    devMicronTracker * taskMicronTracker = new devMicronTracker("devMicronTracker", 50.0 * cmn_ms);
    devMicronTrackerControllerQDevice * taskControllerQDevice = new devMicronTrackerControllerQDevice("taskControllerQDevice");

    // configure the tasks
    cmnPath searchPath = std::string(CISST_SOURCE_ROOT) + "/examples/devicesTutorial/example7";
    taskMicronTracker->Configure(searchPath.Find("config.xml"));

    // add the tasks to the task manager
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    taskManager->AddTask(taskMicronTracker);
    taskManager->AddDevice(taskControllerQDevice);

    // connect the tasks, e.g. RequiredInterface -> ProvidedInterface
    taskManager->Connect("taskControllerQDevice", "RequiresMicronTrackerController",
                         "devMicronTracker", "ProvidesMicronTrackerController");

    // add interfaces for tools and populate controller widget with tool widgets
    const unsigned int numberOfTools = taskMicronTracker->GetNumberOfTools();
    for (unsigned int i = 0; i < numberOfTools; i++) {
        std::string toolName = taskMicronTracker->GetToolName(i);
        devMicronTrackerToolQDevice * taskToolQDevice = new devMicronTrackerToolQDevice(toolName);
        taskControllerQDevice->AddToolWidget(taskToolQDevice->GetWidget(),
                                             taskToolQDevice->GetMarkerLeft(), taskToolQDevice->GetMarkerRight());
        taskManager->AddDevice(taskToolQDevice);
        taskManager->Connect(toolName, toolName,
                             "devMicronTracker", toolName);
    }

    // create and start all tasks
    taskManager->CreateAll();
    taskManager->StartAll();

    // create a main window to hold QWidgets
    QMainWindow * mainWindow = new QMainWindow();
    mainWindow->setCentralWidget(taskControllerQDevice->GetWidget());
    mainWindow->setWindowTitle("MicronTracker Controller");
    mainWindow->show();

    // run Qt user interface
    application.exec();

    // kill all tasks and perform cleanup
    taskManager->KillAll();
    taskManager->Cleanup();

    return 0;
}
