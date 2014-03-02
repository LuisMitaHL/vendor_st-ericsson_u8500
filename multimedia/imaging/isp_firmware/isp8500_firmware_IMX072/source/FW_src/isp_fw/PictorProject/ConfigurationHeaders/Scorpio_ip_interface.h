/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
\file Scorpio_ip_interface.h
 */

#ifndef SCORPIO_IP_INTERFACE_H_
#define SCORPIO_IP_INTERFACE_H_

#include "Platform.h"
#include "PictorhwReg.h"
#include "Scorpio_platform_specific.h"
#include "Scorpio.h"
#include "Damper.h"




/// interface to enable/disable the hw block
#define Set_SCORPIO_ENABLE(scorpio_enable) Set_ISP_SCORPIO_ISP_SCORPIO_ENABLE(scorpio_enable)

/// interface to set required coring level
#define Set_SCORPIO_CORING_LEVEL(coring_level) Set_ISP_SCORPIO_ISP_SCORPIO_CORING_LVL_REQ(coring_level)


#endif /* SCORPIO_IP_INTERFACE_H_ */
