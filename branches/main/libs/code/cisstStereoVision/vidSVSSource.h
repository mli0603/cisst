/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vidSVSSource.h,v 1.4 2008/11/04 18:09:04 vagvoba Exp $
  
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

#ifndef _vidSVSSource_h
#define _vidSVSSource_h

#include <cisstStereoVision/svlVideoCaptureSource.h>
#include "svlImageBuffer.h"
#include <svsclass.h>


class CSVSSourceThread;

class CSVSSource : public CVideoCaptureSourceBase
{
friend class CSVSSourceThread;

public:
    CSVSSource();
    ~CSVSSource();

    svlVideoCaptureSource::PlatformType GetPlatformType();
    int SetStreamCount(unsigned int numofstreams);
	int GetDeviceList(svlVideoCaptureSource::DeviceInfo **deviceinfo);
	int Open();
	void Close();
	int Start();
    svlImageRGB* GetLatestFrame(bool waitfornew, unsigned int videoch = 0);
	int Stop();
	bool IsRunning();
    int SetDevice(int devid, int inid, unsigned int videoch = 0);
	int GetWidth(unsigned int videoch = 0);
	int GetHeight(unsigned int videoch = 0);

    int GetFormatList(unsigned int deviceid, svlVideoCaptureSource::ImageFormat **formatlist);
    int GetFormat(svlVideoCaptureSource::ImageFormat& format, unsigned int videoch = 0);

private:
    unsigned int NumOfStreams;
	bool Running;

    CSVSSourceThread* CaptureProc;
    osaThread* CaptureThread;
    svsVideoImages* SVSObj;
    svsStereoImage* SVSImage;
	int DeviceID[2];
    svlImageBuffer** OutputBuffer;

    int CaptureFrame();
    void ConvertRGB32toRGB24(unsigned char* source, unsigned char* dest, const int pixcount);
};


class CSVSSourceThread
{
public:
    CSVSSourceThread() { InitSuccess = false; }
    ~CSVSSourceThread() {}
    void* Proc(CSVSSource* baseref);

    bool WaitForInit() { InitEvent.Wait(); return InitSuccess; }
    bool IsError() { return Error; }

private:
    bool Error;
    osaThreadSignal InitEvent;
    bool InitSuccess;
};


#endif // _vidSVSSource_h

