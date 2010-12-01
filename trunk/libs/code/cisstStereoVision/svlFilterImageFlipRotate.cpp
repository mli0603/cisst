/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Balazs Vagvolgyi
  Created on: 2008

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageFlipRotate.h>
#include <cisstStereoVision/svlFilterInput.h>


/***************************************/
/*** svlFilterImageFlipRotate class ****/
/***************************************/

CMN_IMPLEMENT_SERVICES(svlFilterImageFlipRotate)

svlFilterImageFlipRotate::svlFilterImageFlipRotate() :
    svlFilterBase(),
    OutputImage(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageMono16);
    AddInputType("input", svlTypeImageRGBStereo);
    AddInputType("input", svlTypeImageMono8Stereo);
    AddInputType("input", svlTypeImageMono16Stereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);

    CWQuarters[0] = CWQuarters[1] = 0;
    FlipHorizontal[0] = FlipHorizontal[1] = false;
    FlipVertical[0] = FlipVertical[1] = false;
}

svlFilterImageFlipRotate::~svlFilterImageFlipRotate()
{
    Release();
}

int svlFilterImageFlipRotate::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    Release();

    OutputImage = dynamic_cast<svlSampleImage*>(syncInput->GetNewInstance());
    svlSampleImage* input = dynamic_cast<svlSampleImage*>(syncInput);
    const unsigned int channels = input->GetVideoChannels();

    // Initializing image sizes and processing parameters
    for (unsigned int i = 0; i < channels; i ++) {
        // Consolidating rotation
        // after which the rotation will be either -1 or 0 or +1
        if (CWQuarters[i] < 0) CWQuarters[i] = 2 - CWQuarters[i];
        CWQuarters[i] = CWQuarters[i] % 4;
        if (CWQuarters[i] == 2) {
            CWQuarters[i] = 0;
            FlipHorizontal[i] = !FlipHorizontal[i];
            FlipVertical[i] = !FlipVertical[i];
        }
        if (CWQuarters[i] == 3) CWQuarters[i] = -1;

        // Computing rotation & flip parameters
        QuickCopy[i] = 0;
        if (CWQuarters[i] == 0) {
            if (FlipHorizontal[i]) {
                if (FlipVertical[i]) {
                    // rotate 180 degrees
                    StartOffset[i] = input->GetWidth(i) * input->GetHeight(i) - 1;
                    Stride[i] = -1;
                    LineStride[i] = -1 * input->GetWidth(i);
                }
                else {
                    // flip horizontally
                    StartOffset[i] = input->GetWidth(i) - 1;
                    Stride[i] = -1;
                    LineStride[i] = input->GetWidth(i);
                }
            }
            else {
                if (FlipVertical[i]) {
                    // upside down
                    QuickCopy[i] = 2;
                }
                else {
                    // same as input
                    QuickCopy[i] = 1;
                }
            }
        }
        else if (CWQuarters[i] == 1) {
            if (FlipHorizontal[i]) {
                if (FlipVertical[i]) {
                    // rotate -90 degrees
                    StartOffset[i] = input->GetHeight(i) * (input->GetWidth(i) - 1);
                    Stride[i] = -1 * input->GetHeight(i);
                    LineStride[i] = 1;
                }
                else {
                    // rotate 90 degrees and flip horizontally
                    StartOffset[i] = 0;
                    Stride[i] = input->GetHeight(i);
                    LineStride[i] = 1;
                }
            }
            else {
                if (FlipVertical[i]) {
                    // rotate 90 degrees and flip vertically
                    StartOffset[i] = input->GetWidth(i) * input->GetHeight(i) - 1;
                    Stride[i] = -1 * input->GetHeight(i);
                    LineStride[i] = -1;
                }
                else {
                    // rotate 90 degrees
                    StartOffset[i] = input->GetHeight(i) - 1;
                    Stride[i] = input->GetHeight(i);
                    LineStride[i] = -1;
                }
            }
        }
        else { // CWQuarters[i] == -1
            if (FlipHorizontal[i]) {
                if (FlipVertical[i]) {
                    // rotate 90 degrees
                    StartOffset[i] = input->GetHeight(i) - 1;
                    Stride[i] = input->GetHeight(i);
                    LineStride[i] = -1;
                }
                else {
                    // rotate -90 degrees and flip horizontally
                    StartOffset[i] = input->GetWidth(i) * input->GetHeight(i) - 1;
                    Stride[i] = -1 * input->GetHeight(i);
                    LineStride[i] = -1;
                }
            }
            else {
                if (FlipVertical[i]) {
                    // rotate -90 degrees and flip vertically
                    StartOffset[i] = 0;
                    Stride[i] = input->GetHeight(i);
                    LineStride[i] = 1;
                }
                else {
                    // rotate -90 degrees
                    StartOffset[i] = input->GetHeight(i) * (input->GetWidth(i) - 1);
                    Stride[i] = -1 * input->GetHeight(i);
                    LineStride[i] = 1;
                }
            }
        }

        if (CWQuarters[i] == 0) OutputImage->SetSize(i, input->GetWidth(i), input->GetHeight(i));
        else OutputImage->SetSize(i, input->GetHeight(i), input->GetWidth(i));
    }

    syncOutput = OutputImage;

    return SVL_OK;
}

int svlFilterImageFlipRotate::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
    syncOutput = OutputImage;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);

    svlSampleImage* input = dynamic_cast<svlSampleImage*>(syncInput);
    unsigned int videochannels = input->GetVideoChannels();
    unsigned int idx;

    typedef vctFixedSizeVector<unsigned char, 3> RGBPixelType;

    _ParallelLoop(procInfo, idx, videochannels)
    {
        switch (GetInput()->GetType()) {
            case svlTypeImageRGB:
            case svlTypeImageRGBStereo:
                FlipRotate<RGBPixelType>(reinterpret_cast<RGBPixelType *>(input->GetUCharPointer(idx)),
                                         reinterpret_cast<RGBPixelType *>(OutputImage->GetUCharPointer(idx)),
                                         input->GetWidth(idx), input->GetHeight(idx),
                                         QuickCopy[idx], StartOffset[idx], Stride[idx], LineStride[idx]);
            break;

            case svlTypeImageMono8:
            case svlTypeImageMono8Stereo:
                FlipRotate<unsigned char>(input->GetUCharPointer(idx),
                                          OutputImage->GetUCharPointer(idx),
                                          input->GetWidth(idx), input->GetHeight(idx),
                                          QuickCopy[idx], StartOffset[idx], Stride[idx], LineStride[idx]);
            break;

            case svlTypeImageMono16:
            case svlTypeImageMono16Stereo:
                FlipRotate<unsigned short>(reinterpret_cast<unsigned short*>(input->GetUCharPointer(idx)),
                                           reinterpret_cast<unsigned short*>(OutputImage->GetUCharPointer(idx)),
                                           input->GetWidth(idx), input->GetHeight(idx),
                                           QuickCopy[idx], StartOffset[idx], Stride[idx], LineStride[idx]);
            break;

            case svlTypeImageRGBA:
            case svlTypeImageRGBAStereo:
            case svlTypeMatrixInt8:
            case svlTypeMatrixInt16:
            case svlTypeMatrixInt32:
            case svlTypeMatrixInt64:
            case svlTypeMatrixUInt8:
            case svlTypeMatrixUInt16:
            case svlTypeMatrixUInt32:
            case svlTypeMatrixUInt64:
            case svlTypeMatrixFloat:
            case svlTypeMatrixDouble:
            case svlTypeImage3DMap:
            case svlTypeInvalid:
            case svlTypeStreamSource:
            case svlTypeStreamSink:
            case svlTypeTransform3D:
            case svlTypeTargets:
            case svlTypeText:
            return SVL_FAIL;
        }
    }

    return SVL_OK;
}

int svlFilterImageFlipRotate::Release()
{
    if (OutputImage) {
        delete OutputImage;
        OutputImage = 0;
    }
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetHorizontalFlip(bool flip)
{
    if (IsInitialized()) return SVL_FAIL;
    FlipHorizontal[0] = FlipHorizontal[1] = flip;
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetHorizontalFlip(unsigned int videoch, bool flip)
{
    if (IsInitialized()) return SVL_FAIL;
    if (videoch <= 1) FlipHorizontal[videoch] = flip;
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetVerticalFlip(bool flip)
{
    if (IsInitialized()) return SVL_FAIL;
    FlipVertical[0] = FlipVertical[1] = flip;
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetVerticalFlip(unsigned int videoch, bool flip)
{
    if (IsInitialized()) return SVL_FAIL;
    if (videoch <= 1) FlipVertical[videoch] = flip;
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetRotation(int cw_quarters)
{
    if (IsInitialized()) return SVL_FAIL;
    CWQuarters[0] = CWQuarters[1] = cw_quarters;
    return SVL_OK;
}

int svlFilterImageFlipRotate::SetRotation(unsigned int videoch, int cw_quarters)
{
    if (IsInitialized()) return SVL_FAIL;
    if (videoch <= 1) CWQuarters[videoch] = cw_quarters;
    return SVL_OK;
}

