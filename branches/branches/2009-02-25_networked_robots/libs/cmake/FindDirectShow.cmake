#
# $Id$
#
# Author(s):  Anton Deguet
# Created on: 2007-02-23
#
# (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
# 
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
# 
# --- end cisst license ---

#
# This file contains code found on the Wiki page of CMake
#

# - Test for DirectShow on Windows.
# Once loaded this will define
#   DIRECTSHOW_FOUND        - system has DirectShow
#   DIRECTSHOW_INCLUDE_DIR  - include directory for DirectShow
#   DIRECTSHOW_LIBRARIES    - libraries you need to link to

SET(DIRECTSHOW_FOUND "NO")

# DirectShow is only available on Windows platforms
IF(WIN32)
  # Find DirectX Include Directory
  FIND_PATH(DIRECTX_INCLUDE_DIR ddraw.h
    "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Include"
    "C:/Program Files/Microsoft DirectX SDK (February 2006)/Include"
    "C:/Program Files/Microsoft DirectX 9.0 SDK (June 2005)/Include"
    "C:/DXSDK/Include"
    DOC "What is the path where the file ddraw.h can be found"
  )

  # if DirectX found, then find DirectShow include directory
  IF(DIRECTX_INCLUDE_DIR)
    FIND_PATH(DIRECTSHOW_INCLUDE_DIR dshow.h
      "C:/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/Include"
      "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Include"
      "C:/Program Files/Microsoft Platform SDK/Include"
      "C:/DXSDK/Include"
      DOC "What is the path where the file dshow.h can be found"
    )

    # if DirectShow include dir found, then find DirectShow libraries
    IF(DIRECTSHOW_INCLUDE_DIR)
      FIND_LIBRARY(DIRECTSHOW_strmiids_LIBRARY strmiids
        "C:/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/Lib"
        "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
        "C:/Program Files/Microsoft Platform SDK/Lib"
        "C:/DXSDK/Include/Lib"
        DOC "Where can the DirectShow strmiids library be found"
      )
      FIND_LIBRARY(DIRECTSHOW_quartz_LIBRARY quartz
        "C:/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/Lib"
        "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
        "C:/Program Files/Microsoft Platform SDK/Lib"
        "C:/DXSDK/Include/Lib"
        DOC "Where can the DirectShow quartz library be found"
      )
      FIND_LIBRARY(DIRECTSHOW_Vfw32_LIBRARY Vfw32
        "C:/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/Lib"
        "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
        "C:/Program Files/Microsoft Platform SDK/Lib"
        "C:/DXSDK/Include/Lib"
        DOC "Where can the DirectShow Vfw32 library be found"
      )
      FIND_LIBRARY(DIRECTSHOW_WinMM_LIBRARY WinMM
        "C:/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/Lib"
        "C:/Program Files/Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib"
        "C:/Program Files/Microsoft Platform SDK/Lib"
        "C:/DXSDK/Include/Lib"
        DOC "Where can the DirectShow WinMM library be found"
      )

      # if DirectShow libraries found, then we're ok
      IF(DIRECTSHOW_strmiids_LIBRARY)
        IF(DIRECTSHOW_quartz_LIBRARY)
          # everything found
          SET(DIRECTSHOW_FOUND "YES")
        ENDIF(DIRECTSHOW_quartz_LIBRARY)
      ENDIF(DIRECTSHOW_strmiids_LIBRARY)
    ENDIF(DIRECTSHOW_INCLUDE_DIR)
  ENDIF(DIRECTX_INCLUDE_DIR)
ENDIF(WIN32)


#---------------------------------------------------------------------
IF(DIRECTSHOW_FOUND)
  SET(DIRECTSHOW_INCLUDE_DIR
    ${DIRECTSHOW_INCLUDE_DIR}
    ${DIRECTX_INCLUDE_DIR}
  )

  SET(DIRECTSHOW_LIBRARIES
    ${DIRECTSHOW_strmiids_LIBRARY}
    ${DIRECTSHOW_quartz_LIBRARY}
  )
ELSE(DIRECTSHOW_FOUND)
  # make FIND_PACKAGE friendly
  IF(NOT DIRECTSHOW_FIND_QUIETLY)
    IF(DIRECTSHOW_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR
              "DirectShow required, please specify it's location.")
    ELSE(DIRECTSHOW_FIND_REQUIRED)
      MESSAGE(STATUS "DirectShow was not found.")
    ENDIF(DIRECTSHOW_FIND_REQUIRED)
  ENDIF(NOT DIRECTSHOW_FIND_QUIETLY)
ENDIF(DIRECTSHOW_FOUND)
