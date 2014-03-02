/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#ifndef __DDR_H__
#define __DDR_H__

#define DDR_REGISTERS_LEN	16
#define DDR_SETTING_LEN		468
#define DDR_FREQUENCY_LEN	4

struct ddr_data {
	/*
	 * One bit pattern and one mask for each DDR register,
	 * hence the "* 2" below.
	 */
	u8 id_regs_and_mask[DDR_REGISTERS_LEN * 2];
	u32 settings[DDR_SETTING_LEN / 4];
	u32 frequency;
} __attribute__((__packed__));

#endif
