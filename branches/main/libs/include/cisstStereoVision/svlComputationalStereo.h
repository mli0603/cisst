/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlComputationalStereo.h,v 1.9 2008/10/17 17:44:38 vagvoba Exp $
  
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

#ifndef _svlComputationalStereo_h
#define _svlComputationalStereo_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SVL_STEREO_INPUT_MISMATCH       -5000
#define SVL_STEREO_INIT_ERROR           -5001

enum svlStereoMethod
{
    svlStereoMethodDP
};

class CISST_EXPORT svlStereoMethodBase
{
public:
    svlStereoMethodBase() {}
    virtual ~svlStereoMethodBase() {}

    virtual int Initialize() { return SVL_FAIL; }
    virtual int Process(svlSampleImageBase *images, int *depthmap) { return SVL_FAIL; }
    virtual void Free() {}
};

class CISST_EXPORT svlComputationalStereo : public svlFilterBase
{
public:
    svlComputationalStereo();
    virtual ~svlComputationalStereo();

    int SetCrossCheck(bool enabled);
    bool GetCrossCheck();
    void DisparityOutput(bool enable) { DisparityOutputEnabled  = enable; }
    bool DisparityOutput() { return DisparityOutputEnabled; }
    void DisparityInterpolation(bool enable) { DisparityInterpolationEnabled  = enable; }
    bool DisparityInterpolation() { return DisparityInterpolationEnabled; }
    int SetFocalLength(double focallength);
    double GetFocalLength();
    int SetStereoBaseline(double baseline);
    double GetStereoBaseline();
    void SetPrincipalPoints(double ppx_left, double ppx_right, double ppy);
    void GetPrincipalPoints(double &ppx_left, double &ppx_right, double &ppy);
    void SetDisparityRange(unsigned int mindisparity, unsigned int maxdisparity);
    void GetDisparityRange(unsigned int& mindisparity, unsigned int& maxdisparity);
    void SetBlockSize(unsigned int blocksize);
    unsigned int GetBlockSize();
    void SetScalingFactor(unsigned int scalefactor);
    unsigned int GetScalingFactor();
    void SetQuickSearchRadius(unsigned int searchradius);
    unsigned int GetQuickSearchRadius();
    void SetSmoothnessFactor(unsigned int smoothness);
    unsigned int GetSmoothnessFactor();
    void SetTemporalFiltering(double tempfilt);
    double GetTemporalFiltering();
    void SetSpatialFiltering(unsigned int radius);
    unsigned int GetSpatialFiltering();
    void SetValidRect(int left, int top, int right, int bottom);

private:
    // Work in progress...
    // Other methods not available yet.
    void SetMethod(svlStereoMethod method);
    svlStereoMethod GetMethod();

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    svlStereoMethodBase* StereoAlgorithm;
    svlStereoMethodBase* XCheckStereoAlgorithm;

    svlSampleImageBase* XCheckImage;
    vctDynamicMatrix<int> DisparityBuffer;
    vctDynamicMatrix<int> XCheckDisparityBuffer;

    vctDynamicMatrix<float> SpatialFilterBuffer;

    vctDynamicMatrix<float> UnitSurfaceVectors;
    vctDynamicVector<unsigned int> SurfaceImageMap;

    int ValidAreaLeft;
    int ValidAreaRight;
    int ValidAreaTop;
    int ValidAreaBottom;
    float FocalLength;
    float Baseline;
    float PPX_Left_flt;
    float PPX_Right_flt;
    float PPY_flt;
    int PPX_Left;
    int PPX_Right;
    int PPY;
    int MinDisparity;
    int MaxDisparity;
    int ScaleFactor;
    int BlockSize;
    int NarrowedSearchRadius;
    int Smoothness;
    double TemporalFilter;
    int SpatialFilterRadius;
    bool DisparityOutputEnabled;
    bool DisparityInterpolationEnabled;
    svlStereoMethod StereoMethod;
    bool XCheckEnabled;

    template <class _paramType>
    void CreateXCheckImageMono(_paramType* source, _paramType* target, const unsigned int width, const unsigned int height);
    void CreateXCheckImageColor(unsigned char* source, unsigned char* target, const unsigned int width, const unsigned int height);

    void PerformXCheck();
    void ConvertDisparityToDistance(int* disparitymap, float* depthmap, const int mapwidth, const int mapheight);
    void ApplySpatialFilter(const int radius,
                            float* depthmap, float* tempbuffer,
                            const int mapwidth, const int mapheight, const int linestride);
};

// *******************************************************************
// CreateXCheckImageMono PRIVATE method
// arguments:
//           source         - input image
//           target         - output image
//           width          - width of images
//           height         - height of images
// function:
//    For single channel input images
//    Creates mirrored image for left-right check
// *******************************************************************
template <class _paramType>
void svlComputationalStereo::CreateXCheckImageMono(_paramType* source, _paramType* target, const unsigned int width, const unsigned int height)
{
    unsigned int i, j;
    _paramType* dst = target + width * height - 1;
    for (j = 0; j < height; j ++) {
        for (i = 0; i < width; i ++) {
            *dst = *source; source ++; dst --;
        }
    }
}

#endif // _svlComputationalStereo_h

