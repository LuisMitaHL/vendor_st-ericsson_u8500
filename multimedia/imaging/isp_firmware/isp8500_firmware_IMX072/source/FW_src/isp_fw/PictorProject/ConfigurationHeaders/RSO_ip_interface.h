/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
\file RSO_ip_interface.h
 */

#ifndef RSO_IP_INTERFACE_H_
#define RSO_IP_INTERFACE_H_

#include "PictorhwReg.h"
#include "RSO_OPInterface.h"
#include "RSO_platform_specific.h"
#include "Damper.h"

// invoke the damper with correct parameters

#define Set_RSO_ENABLE()                       Set_ISP_RSO_ISP_RSO_SLANT_CTRL_rso_en__ENABLE()
#define Set_RSO_DISABLE()                      Set_ISP_RSO_ISP_RSO_SLANT_CTRL_rso_en__DISABLE()

/// gir channel
#define Set_RSO_DC_TERM_GR(dc_term)            Set_ISP_RSO_ISP_RSO_DC_TERM_GR(dc_term)
#define Set_RSO_X_COEF_GR(x_coeff)             Set_ISP_RSO_ISP_RSO_X_COEF_GR(x_coeff)
#define Set_RSO_Y_COEF_GR(y_coeff)             Set_ISP_RSO_ISP_RSO_Y_COEF_GR(y_coeff)

/// r channel
#define Set_RSO_DC_TERM_RR(dc_term)            Set_ISP_RSO_ISP_RSO_DC_TERM_RR(dc_term)
#define Set_RSO_X_COEF_RR(x_coeff)             Set_ISP_RSO_ISP_RSO_X_COEF_RR(x_coeff)
#define Set_RSO_Y_COEF_RR(y_coeff)             Set_ISP_RSO_ISP_RSO_Y_COEF_RR(y_coeff)

/// b channel
#define Set_RSO_DC_TERM_BB(dc_term)            Set_ISP_RSO_ISP_RSO_DC_TERM_BB(dc_term)
#define Set_RSO_X_COEF_BB(x_coeff)             Set_ISP_RSO_ISP_RSO_X_COEF_BB(x_coeff)
#define Set_RSO_Y_COEF_BB(y_coeff)             Set_ISP_RSO_ISP_RSO_Y_COEF_BB(y_coeff)

/// gib channel
#define Set_RSO_DC_TERM_GB(dc_term)            Set_ISP_RSO_ISP_RSO_DC_TERM_GB(dc_term)
#define Set_RSO_X_COEF_GB(x_coeff)             Set_ISP_RSO_ISP_RSO_X_COEF_GB(x_coeff)
#define Set_RSO_Y_COEF_GB(y_coeff)             Set_ISP_RSO_ISP_RSO_Y_COEF_GB(y_coeff)

/// slant origin
#define Set_RSO_X_SLANT_ORIGIN(x_slant_origin) Set_ISP_RSO_ISP_RSO_X_SLANT_ORIGIN(x_slant_origin)
#define Set_RSO_Y_SLANT_ORIGIN(y_slant_origin) Set_ISP_RSO_ISP_RSO_Y_SLANT_ORIGIN(y_slant_origin)


#endif /* RSO_IP_INTERFACE_H_ */
