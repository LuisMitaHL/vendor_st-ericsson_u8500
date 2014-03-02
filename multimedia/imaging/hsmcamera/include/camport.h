/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _CAM_PORT_H_
#define _CAM_PORT_H_

#include <cm/inc/cm.hpp>
#include "ifmport.h"
#include "IFM_Types.h"
#include "grab_types.idt.h"
#include "IFM_Types.h"

#define CAMERA_NB_OUT_PORTS 3
#define CAMERA_NB_IN_PORTS 0
#define CAMERA_NB_PORTS (CAMERA_NB_OUT_PORTS+CAMERA_NB_IN_PORTS)

#define CAMERA_PORT_OUT2 (CAMERA_NB_OUT_PORTS-1)
#define CAMERA_PORT_OUT1 (CAMERA_PORT_OUT2-1)
#define CAMERA_PORT_OUT0 (CAMERA_PORT_OUT1-1)

#define CAM_OVERSCAN_NOM 11
#define CAM_OVERSCAN_DEN 10

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(camport);
#endif
class camport: public ifmport {
   public:
      static const int DEFAULT_VPB0_WIDTH = 640;
      static const int DEFAULT_VPB0_HEIGHT = 360;
      static const int DEFAULT_VPB2_WIDTH_PRIMARY = 1280;
      static const int DEFAULT_VPB2_HEIGHT_PRIMARY = 720;
      static const int DEFAULT_VPB2_WIDTH_SECONDARY = 640;
      static const int DEFAULT_VPB2_HEIGHT_SECONDARY = 480;
      static const OMX_COLOR_FORMATTYPE DEFAULT_COLOR_FORMAT = OMX_COLOR_FormatCbYCrY;
      static const OMX_COLOR_FORMATTYPE DEFAULT_VPB2_COLOR_FORMAT = (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar;
      static const  OMX_COLOR_FORMATTYPE DEFAULT_BAYER_COLOR_FORMAT = OMX_COLOR_FormatRawBayer8bit;
      static const int DEFAULT_VPB0_FRAMERATE = 0;          // Port-specific, according to SAS §16.1; Q16 format in OMX
      static const int DEFAULT_VPB1_FRAMERATE = 0;          // Port-specific, according to SAS §16.1; Q16 format in OMX
      static const int DEFAULT_VPB2_FRAMERATE = 30 << 16;   // Port-specific, according to SAS §16.1; Q16 format in OMX

      camport(const EnsCommonPortData& commonPortData, ENS_Component& ensComp)
         : ifmport(commonPortData, ensComp){
            mBytesPerPixel = 4;
            extradataSize = 0;
            portSettingsChanged=OMX_FALSE;
            mInternalFrameWidth = DEFAULT_VPB0_WIDTH;
            mInternalFrameHeight = DEFAULT_VPB0_HEIGHT;
            nNumberOfFramesGrabbed = 0;
            nNumberOfFramesStreamedOut = 0;
            bOneShot = OMX_TRUE;
            mMaxOverscannedHeight = 0;
            mMaxOverscannedWidth = 0;
         }
      virtual ~camport() {};

      virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef) ;

      virtual OMX_ERRORTYPE setParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure);

      virtual OMX_ERRORTYPE getParameter(
                OMX_INDEXTYPE nParamIndex,  
                OMX_PTR pComponentParameterStructure) const;

      void setDefaultFormatInPortDefinition(OMX_PARAM_PORTDEFINITIONTYPE *);

      virtual OMX_ERRORTYPE checkFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef);

      void setExtraDataSize(OMX_U32 size) { extradataSize = size; }

      OMX_BOOL portSettingsChanged;

      t_uint16 getMaxRes(t_uint16 portId);
      OMX_BOOL isSupportedFmt(t_uint16 portID, OMX_COLOR_FORMATTYPE omxformat);
      void getHwConstraints(t_uint16 portId,OMX_COLOR_FORMATTYPE omxformat, t_uint16 * p_multiple_width, t_uint16 * p_multiple_height, t_uint16 * p_multiple_stride);
      OMX_U32 getMaxOverscannedWidth() {return mMaxOverscannedWidth;}
      OMX_U32 getMaxOverscannedHeight() {return mMaxOverscannedHeight; }
      inline void incrementNumberOfFramesGrabbed() { nNumberOfFramesGrabbed++; }
      inline OMX_U32 getNumberOfFramesGrabbed() { return nNumberOfFramesGrabbed; };
      inline void incrementNumberOfFramesStreamedOut() { nNumberOfFramesStreamedOut++; }
      inline OMX_U32 getNumberOfFramesStreamedOut() { return nNumberOfFramesStreamedOut; }

      OMX_BOOL bOneShot;

   private:
      OMX_U32 extradataSize;

      OMX_U32 mMaxOverscannedWidth;
      OMX_U32 mMaxOverscannedHeight;

      OMX_U32 nNumberOfFramesGrabbed;
      OMX_U32 nNumberOfFramesStreamedOut;
};

#endif // _CAM_PORT_H_
