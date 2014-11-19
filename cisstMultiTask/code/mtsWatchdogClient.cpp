/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id: $

Author(s):  Marcin Balicki
Created on: 2014

(C) Copyright 2006-2014 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsWatchdogClient.h>
#include <cisstOSAbstraction.h>
#include <cisstCommon.h>


// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES(mtsWatchdogClient);


void mtsWatchdogClient::AddToRequiredInterface(mtsInterfaceRequired &reqInt) {

    // Required interface to match the watchdog on the remote server
    reqInt.AddFunction("GetWatchdogState"   , Watchdog.ReadState);
    reqInt.AddFunction("SetWatchdogState"   , Watchdog.WriteState);
    reqInt.AddFunction("WatchdogReset"      , Watchdog.ResetState);

}

bool mtsWatchdogClient::CheckAndUpdate() {

    if (!Is_OK)
        return false;
    mtsExecutionResult executionResult;
    mtsBool updatedBool = false;

    //check if variable on the other end is updated to true;
    executionResult = Watchdog.ReadState(updatedBool);
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "Watchdog read var failed \""
                                << executionResult << "\"" << std::endl;
        Is_OK = false;
        return false;
    }

    //if it is OK reset watchdogtimer and reset the remote variable to false. return true
    if (updatedBool) {
        if (!Reset())
            return false;
        else
            return true;
    }
    //else check if time has exceeded timeout, if not return true, else return false;
    else if (StopWatch.GetElapsedTime() > Timeout) {
            Is_OK = false;
            return false;
    }
    else
        return true;
}

bool mtsWatchdogClient::Reset() {
    StopWatch.Reset();
    StopWatch.Start();

    mtsExecutionResult executionResult = Watchdog.WriteState(mtsBool(false));
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "Watchdog write var failed \""
                                << executionResult << "\"" << std::endl;
        Is_OK = false;
        return false;
    }

    Is_OK = true;
    return true;
}

bool mtsWatchdogClient::Start() {
    Reset();
    mtsExecutionResult executionResult = Watchdog.ResetState();
    if (!executionResult.IsOK()) {
        CMN_LOG_CLASS_RUN_ERROR << "Watchdog reset var failed \""
                                << executionResult << "\"" << std::endl;
        Is_OK = false;
        return false;
    }
    return true;
}
