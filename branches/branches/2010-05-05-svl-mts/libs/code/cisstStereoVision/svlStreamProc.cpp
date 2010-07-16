/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlStreamProc.h>
#include <cisstStereoVision/svlStreamManager.h>
#include <cisstStereoVision/svlFilterSourceBase.h>
#include <cisstStereoVision/svlStreamBranchSource.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstOSAbstraction/osaSleep.h>


/*****************************/
/*** svlStreamProc class *****/
/*****************************/

svlStreamProc::svlStreamProc(unsigned int threadcount, unsigned int threadid) :
    ThreadID(threadid),
    ThreadCount(threadcount)
{
}

double svlStreamProc::GetAbsoluteTime(osaTimeServer* timeserver)
{
    if (!timeserver) return -1.0;

    osaAbsoluteTime abstime;

    timeserver->RelativeToAbsolute(timeserver->GetRelativeTime(), abstime);

    // Calculate time since UNIX creation
    return abstime.sec + abstime.nsec / 1000000000.0;
}

void* svlStreamProc::Proc(svlStreamManager* baseref)
{
    svlSample *inputsample, *outputsample;
    svlFilterBase *filter, *prevfilter;
    svlFilterSourceBase* source = baseref->StreamSource;
    svlFilterOutput* output;
    svlFilterInput* input;
    svlProcInfo info;
    svlSyncPoint *sync = baseref->SyncPoint;
    unsigned int counter = 0;
    osaTimeServer* timeserver = 0;
    double timestamp;
    int status = SVL_OK;

    // Initializing thread info structure
    info.count = ThreadCount;
    info.id    = ThreadID;
    info.sync  = sync;
    info.cs    = baseref->CS;

    if (ThreadID == 0) {
    // Execute only on one thread - BEGIN

        // Initialize time server for accessing absolute time
        timeserver = new osaTimeServer;
        timeserver->SetTimeOrigin();

    // Execute only on one thread - END
    }

    while (baseref->StopThread == false) {
        source->FrameCounter = counter;
        outputsample = 0;

    ///////////////////////////////////////
    // Handle stream control (pause/play)

        if (source->PauseAtFrameID == static_cast<int>(counter)) {
            if (ThreadID == 0) {
                // Wait until playback resumed or stream stopped
                while (source->PlayCounter == 0 && baseref->StopThread == false) {
                    osaSleep(0.1); // check 10 times a second
                }
                if (baseref->StopThread) break;
            }

            if (ThreadCount > 1) {
            // Execute only if multi-threaded - BEGIN

                // Synchronization point, wait for other threads
                if (sync->Sync(ThreadID) != SVL_SYNC_OK) break;

            // Execute only if multi-threaded - END
            }
        }

        if (ThreadID == 0) {
            if (source->PlayCounter > 0) source->PlayCounter --;
            if (source->PlayCounter == 0) {
                // Pause when the next frame arrives
                source->PauseAtFrameID = static_cast<int>(counter) + 1;
            }
        }

    ////////////////////////////////////
    // Starting from the stream source

        status = source->Process(&info, outputsample);
        if (status < 0 || status == SVL_STOP_REQUEST) break;

        if (ThreadID == 0) {
        // Execute only on one thread - BEGIN

            if (outputsample && (source->AutoTimestamp || outputsample->GetTimestamp() < 0.0)) {
                // Get fresh timestamp and assign it to the output sample
                outputsample->SetTimestamp(GetAbsoluteTime(timeserver));
            }

        // Execute only on one thread - END
        }

        if (ThreadCount > 1) {
        // Execute only if multi-threaded - BEGIN

            // Synchronization point, wait for other threads
            if (sync->Sync(ThreadID) != SVL_SYNC_OK) break;

        // Execute only if multi-threaded - END
        }

        // Check for errors and stop request
        if (baseref->StopThread || baseref->StreamStatus != SVL_OK) break;

        prevfilter = source;

        // Get next filter in the chain
        output = source->GetOutput();
        filter = 0;
        // Check if trunk output exists
        if (output) {
            input = output->Connection;
            // Check if trunk output is connected
            if (input) {
                // If connected input is trunk
                if (input->Trunk) filter = input->Filter;
                // If connected input is not trunk
                else if (ThreadID == 0 && outputsample) input->Buffer->Push(outputsample);
                // Store timestamps on both the filter input and the filter output
                timestamp = outputsample->GetTimestamp();
                output->Timestamp = timestamp;
                input->Timestamp = timestamp;
            }
        }

    ////////////////////////////////////////////
    // Going downstream filter by filter

        while (filter != 0) {
            filter->FrameCounter = counter;

            // Pass samples downstream
            inputsample = outputsample; outputsample = 0;

            // Check if the previous output is valid input for the next filter
            status = filter->IsDataValid(filter->GetInput()->Type, inputsample);
            if (status != SVL_OK) break;

            status = filter->Process(&info, inputsample, outputsample);
            if (status < 0) break;

            if (ThreadCount > 1) {
            // Execute only if multi-threaded - BEGIN

                // Synchronization point, wait for other threads
                if (sync->Sync(ThreadID) != SVL_SYNC_OK) break;

            // Execute only if multi-threaded - END
            }

            // Check for errors and stop request
            if (baseref->StopThread || baseref->StreamStatus != SVL_OK) break;

            // Store input time stamp
            filter->PrevInputTimestamp = inputsample->GetTimestamp();

            // Pass input timestamp to output sample
            if (outputsample) outputsample->SetTimestamp(filter->PrevInputTimestamp);

            prevfilter = filter;

            // Get next filter in the chain
            output = filter->GetOutput();
            filter = 0;
            // Check if trunk output exists
            if (output) {
                input = output->Connection;
                // Check if trunk output is connected
                if (input) {
                    // If connected input is trunk
                    if (input->Trunk) filter = input->Filter;
                    // If connected input is not trunk
                    else if (ThreadID == 0 && outputsample) input->Buffer->Push(outputsample);
                    // Store timestamps on both the filter input and the filter output
                    timestamp = outputsample->GetTimestamp();
                    output->Timestamp = timestamp;
                    input->Timestamp = timestamp;
                }
            }
        }
        if (status < 0) break;

        // incrementing frame counter
        counter ++;
    }

    if (ThreadID == 0) {
    // Execute only on one thread - BEGIN

        // Delete time server
        if (timeserver) delete timeserver;

    // Execute only on one thread - END
    }

    // Signal the error status
    if (baseref->StopThread == false) {
        // Internal shutdown
        baseref->StreamStatus = status;
    }

    if (ThreadCount > 1) {
    // Execute only if multi-threaded - BEGIN

        sync->ReleaseAll();

    // Execute only if multi-threaded - END
    }

    // Run InternalStop() method in case of internal shutdown
    if (baseref->StopThread == false && ThreadID == 0) {
        baseref->InternalStop(ThreadID);
    }

    return this;
}

