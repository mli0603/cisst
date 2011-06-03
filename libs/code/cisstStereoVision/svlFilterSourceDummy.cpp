/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Balazs Vagvolgyi
  Created on: 2006

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterSourceDummy.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <string.h>
#include "time.h"


/*************************************/
/*** svlFilterSourceDummy class ******/
/*************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterSourceDummy, svlFilterSourceBase)

svlFilterSourceDummy::svlFilterSourceDummy() :
    svlFilterSourceBase(),
    OutputSample(0),
    Width(320),
    Height(240),
    Noise(true)
{
    AddOutput("output", true);
    SetAutomaticOutputType(false);
    SetTargetFrequency(30.0);
}

svlFilterSourceDummy::svlFilterSourceDummy(svlStreamType type) :
    svlFilterSourceBase(),
    OutputSample(0),
    Width(320),
    Height(240),
    Noise(true)
{
    AddOutput("output", true);
    SetAutomaticOutputType(false);
    SetType(type);
    SetTargetFrequency(30.0);
}

svlFilterSourceDummy::svlFilterSourceDummy(const svlSampleImage & image) :
    svlFilterSourceBase(),
    OutputSample(0),
    Width(320),
    Height(240),
    Noise(true)
{
    AddOutput("output", true);
    SetAutomaticOutputType(false);
    SetImage(image);
    SetTargetFrequency(30.0);
}

svlFilterSourceDummy::~svlFilterSourceDummy()
{
    if (OutputSample) delete OutputSample;
}

int svlFilterSourceDummy::SetType(svlStreamType type)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    // Other types may be added in the future
    if (type != svlTypeImageRGB && type != svlTypeImageRGBStereo) return SVL_FAIL;

    if (OutputSample && OutputSample->GetType() != type) {
        delete OutputSample;
        OutputSample = svlSample::GetNewFromType(type);
    }
    else if (!OutputSample) OutputSample = svlSample::GetNewFromType(type);

    if (Width > 0 && Height > 0) dynamic_cast<svlSampleImage*>(OutputSample)->SetSize(Width, Height);

    GetOutput()->SetType(type);

    return SVL_OK;
}

int svlFilterSourceDummy::SetImage(const svlSampleImage & image)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    svlStreamType type = image.GetType();

    // Other types may be added in the future
    if (type != svlTypeImageRGB && type != svlTypeImageRGBStereo) return SVL_FAIL;

    if (OutputSample && OutputSample->GetType() != type) {
        delete OutputSample;
        OutputSample = svlSample::GetNewFromType(type);
    }
    else if (!OutputSample) OutputSample = svlSample::GetNewFromType(type);

    OutputSample->CopyOf(image);

    GetOutput()->SetType(type);

    Noise = false;

    return SVL_OK;
}

int svlFilterSourceDummy::SetDimensions(unsigned int width, unsigned int height)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    Width = width;
    Height = height;

    if (OutputSample) {
        dynamic_cast<svlSampleImage*>(OutputSample)->SetSize(width, height);
    }

    return SVL_OK;
}

void svlFilterSourceDummy::EnableNoiseImage(bool noise)
{
    Noise = noise;
}

unsigned int svlFilterSourceDummy::GetWidth(unsigned int CMN_UNUSED(videoch)) const
{
    return Width;
}

unsigned int svlFilterSourceDummy::GetHeight(unsigned int CMN_UNUSED(videoch)) const
{
    return Height;
}

int svlFilterSourceDummy::Initialize(svlSample* &syncOutput)
{
    if (OutputSample == 0) return SVL_FAIL;

    srand(static_cast<unsigned int>(time(0)));

    syncOutput = OutputSample;

    return SVL_OK;
}

int svlFilterSourceDummy::Process(svlProcInfo* procInfo, svlSample* &syncOutput)
{
    syncOutput = OutputSample;

    // Try to keep target frequency
    _OnSingleThread(procInfo) WaitForTargetTimer();

    if (Noise) {

        svlSampleImage* img = dynamic_cast<svlSampleImage*>(OutputSample);
        const unsigned int videochannels = img->GetVideoChannels();
        unsigned int channel, idx;
        unsigned char* ptr;

        for (channel = 0; channel < videochannels; channel ++) {

            ptr = img->GetUCharPointer(channel);

            _ParallelLoop(procInfo, idx, img->GetDataSize(channel))
            {
                ptr[idx] = static_cast<unsigned char>(rand());
            }
        }
    }

    return SVL_OK;
}

