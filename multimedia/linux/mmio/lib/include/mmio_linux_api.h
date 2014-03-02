/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _MMIO_LINUX_API_H_
#define _MMIO_LINUX_API_H_

#include "OMX_Core.h"

	/*APIs which are linux specific, not used in think currently.
	 *If they are useful for think as well they can be moved to MMIO_Camera.h
	 */

	/*
	 * Activate the I2C2 gpio alternate function for detecting the Camera sensors available on the board
	 * Parameter : OMX_TRUE  : Activate I2C2 and deactivate IPI2C
	 *           : OMX_FALSE : Activate IPI2C and deactivate I2C2
 	 */
	OMX_ERRORTYPE activate_i2c2(int activate);

	/*
 	 * Enable/Disable XSHUTDOWN control from host
	 * Parameter
	 * enable	 : OMX_TRUE : Enable Xshutdown control from host,
	 *						  disable XSHUTDOWN control from f/w
	 *           : OMX_FALSE: Disable Xshutdown control from host,
	 *						  enable Xshutdown control from f/w
 	 * is_active_high : OMX_TRUE : sensor's xshutdown is active high
	 * 					OMX_FALSE: sensor's xshutdown is active low.
	 */
	OMX_ERRORTYPE enable_xshutdown_from_host(int enable, int is_active_high);
#endif



