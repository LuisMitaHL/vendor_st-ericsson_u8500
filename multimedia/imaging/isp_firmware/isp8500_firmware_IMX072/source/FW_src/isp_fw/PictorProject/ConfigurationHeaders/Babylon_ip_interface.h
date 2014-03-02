/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
\file Babylon_ip_interface.h
 */
#ifndef BABYLON_IP_INTERFACE_H_
#   define BABYLON_IP_INTERFACE_H_

#   include "PictorhwReg.h"
#   include "Babylon_platform_specific.h"
#   include "Damper.h"

/// macro to commit the enable/disable of the block
#   define Set_BABYLON_ENABLE(enable_babylon, soft_reset)  Set_ISP_BABYLON_ISP_BABYLON_ENABLE(enable_babylon, soft_reset)   //enable_babylon,soft_reset
    
    /// macro to set the zipperkill control
#   define Set_BABYLON_ZIPPERKILL(zipperkill)              Set_ISP_BABYLON_ISP_BABYLON_ZIPPERKILL(zipperkill)

/// macro to set the flat threshold
#   define Set_BABYLON_FLAT_TH(flat_th)    Set_ISP_BABYLON_ISP_BABYLON_FLAT_TH(flat_th)

/// calls the appropriate damper function with appropriate parameters to damp the block

#endif /* BABYLON_IP_INTERFACE_H_ */

