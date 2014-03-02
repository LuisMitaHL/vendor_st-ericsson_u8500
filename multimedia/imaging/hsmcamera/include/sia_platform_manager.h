/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _SIA_PLATFORM_MANAGER_H_
#define _SIA_PLATFORM_MANAGER_H_

#include <OMX_Core.h>
#include "host/ispctl/api/cfg.hpp"
#include "sensor.h"
#include "ispctl_communication_manager.h"
#include "TraceObject.h"

typedef enum eDataLane {
	DATA_LANE_MAP_MOP =0xca,  // 0xCA for Toshiba   0xD1 for STV651
	DATA_LANE_MAP_HREF =0xd1 // 0xCA for Toshiba   0xD1 for STV651
}eDataLane;


#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CSiaPlatformManager);
#endif

enum mmio_select_i2c_t {
    MMIO_ACTIVATE_IPI2C2 = 0,
    MMIO_ACTIVATE_I2C_HOST,
    MMIO_DEACTIVATE_I2C };

class CSiaPlatformManager
{

public :
	CSiaPlatformManager(TraceObject *traceobj) ;
	OMX_ERRORTYPE init(enumCameraSlot) ;
	OMX_ERRORTYPE config(enumCameraSlot) ;
	OMX_ERRORTYPE deinit(enumCameraSlot) ;

	void setIspctlCfg(Iispctl_api_cfg);

private :

	Iispctl_api_cfg mIspctlConfigure;

public:
    TraceObject* mTraceObject;
};



#endif /**/
