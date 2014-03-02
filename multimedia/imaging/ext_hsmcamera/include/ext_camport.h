/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_CAM_PORT_H_
#define _EXT_CAM_PORT_H_

#include <cm/inc/cm.hpp>
#include "ifmport.h"
#include "ext_grab_types.idt.h"

#define CAMERA_NB_OUT_PORTS 2
#define CAMERA_NB_IN_PORTS 0
#define CAMERA_NB_PORTS (CAMERA_NB_OUT_PORTS+CAMERA_NB_IN_PORTS)

#define CAMERA_PORT_OUT1 (CAMERA_NB_OUT_PORTS-1)
#define CAMERA_PORT_OUT0 (CAMERA_PORT_OUT1-1)

class camport: public ifmport {
public:
    static const int DEFAULT_WIDTH     = 320;
    static const int DEFAULT_HEIGHT    = 240;
    static const OMX_COLOR_FORMATTYPE DEFAULT_COLOR_FORMAT = OMX_COLOR_FormatCbYCrY;
    static const int DEFAULT_FRAMERATE = 10;

    camport(const EnsCommonPortData& commonPortData, ENS_Component& ensComp)
        : ifmport(commonPortData, ensComp){
            mBytesPerPixel = 4;
            extradataSize = 0;
            portSettingsChanged=OMX_FALSE;
        }
    virtual ~camport() {};

    virtual OMX_ERRORTYPE setFormatInPortDefinition(
            const OMX_PARAM_PORTDEFINITIONTYPE &portdef) ;

    void setDefaultFormatInPortDefinition(
            OMX_PARAM_PORTDEFINITIONTYPE *);

    virtual OMX_ERRORTYPE setParameter(
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure);

    virtual OMX_ERRORTYPE getParameter(
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure) const;

    virtual OMX_ERRORTYPE checkFormatInPortDefinition(
            const OMX_PARAM_PORTDEFINITIONTYPE &portdef);

    void setExtraDataSize(OMX_U32 size)
    {
        extradataSize = size;
    }

    OMX_BOOL portSettingsChanged;

    t_uint16 getMaxRes(
            t_uint16 portId);

    OMX_BOOL isSupportedFmt(
            t_uint16 portID,
            OMX_COLOR_FORMATTYPE omxformat,
            t_uint32 omxcodingformat);

    void getHwConstraints(
            t_uint16 portId,
            OMX_COLOR_FORMATTYPE omxformat,
            t_uint16 * p_multiple_width,
            t_uint16 * p_multiple_height,
            t_uint16 * p_multiple_stride);

    OMX_BOOL bOneShot;

private:
    float    mBytesPerPixel;
    OMX_U32	 extradataSize;
};

#endif /* _EXT_CAM_PORT_H_ */
