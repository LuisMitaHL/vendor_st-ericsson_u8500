/*
 * Copyright (C) ST-Ericsson SA 2011 All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_SIA_PLATFORM_MANAGER_H_
#define _EXT_SIA_PLATFORM_MANAGER_H_

#include <OMX_Core.h>
#include "host/ext_ispctl/api/cfg.hpp"
#include "ext_ispctl_communication_manager.h"

class CSiaPlatformManager
{

public :
    CSiaPlatformManager() ;
    OMX_ERRORTYPE init(enumExtCameraSlot) ;
    OMX_ERRORTYPE config(enumExtCameraSlot) ;
    OMX_ERRORTYPE deinit(enumExtCameraSlot) ;

    void setExtIspctlCfg(Iext_ispctl_api_cfg);

private :
    Iext_ispctl_api_cfg mExtIspctlConfigure;
};

#endif /* _EXT_SIA_PLATFORM_MANAGER_H_ */
