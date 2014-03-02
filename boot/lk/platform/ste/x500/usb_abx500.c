/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include "abx500.h"
#include "prcmu.h"
#include "usb_abx500.h"

#ifdef  DEBUG
#define debug(lvl,fmt,args...)		dprintf (lvl,fmt ,##args)
#else
#define debug(lvl,fmt,args...)
#endif

/*
 * v-intcore regulator (ReguMisc1):
 * VTVoutLP      : inactive
 * VintCore12LP  : inactive
 * VintCore12Sel : 1.2V
 * VintCore12Ena : enable
 * VTVoutEna     : disable
 */
#define AB8500_REGU_MISC1_REG_MASK	0x04
#define AB8500_REGU_MISC1_REG_VAL	0x04

#define DRV_ID "ab8500_usb"

#define AB8500_BIT_PHY_CTRL_DEVICE_EN (1 << 1)

int ab8500_usb_init(bool phy_tune)
{
	int rc;

	/* set gpios */

	/* request sysclk */
	rc= ab8500_sysclk_req(true);
	if(rc) {
		debug(CRITICAL,"Error: %s sysclk request failed\n",DRV_ID);
		return rc;
	}
	debug(INFO,"%s sysclk request done\n",DRV_ID);

	/*
	 * Enable regulators.
	 * VapeRegu and Vsmps2Regu should be enabled by prcmu at this point.
	 * So, enable only ReguMisc1 (v-intcore)
	 */
	rc = ab8500_reg_mask_and_set(AB8500_REGU_MISC1_REG,
		AB8500_REGU_MISC1_REG_MASK,
		AB8500_REGU_MISC1_REG_VAL);

	if (rc < 0) {
		debug(CRITICAL,"%s: enable regulator failed\n",DRV_ID);
		return rc;
	}
	debug(INFO,"%s regulator ReguMisc1 enabled\n",DRV_ID);

	/* Write Phy tuning values */
	if (phy_tune) {
		/* Enable the PBT/Bank 0x12 access */
		rc = ab8500_reg_write(AB8500_DEBUG_TESTMODE_REG, 0x01);
		if (rc < 0)
			debug(INFO,"%s: bank12 access enable"
					" failed ret=%d\n", DRV_ID, rc);

		rc = ab8500_reg_write(AB8500_USB_PHY_TUNE1_REG, 0xC8);
		if (rc < 0)
			debug(INFO,"%s: PHY_TUNE1 reg set"
					" failed ret=%d\n", DRV_ID, rc);

		rc = ab8500_reg_write(AB8500_USB_PHY_TUNE2_REG, 0x00);
		if (rc < 0)
			debug(INFO,"%s: PHY_TUNE2 reg set"
					" failed ret=%d\n", DRV_ID, rc);

		rc = ab8500_reg_write(AB8500_USB_PHY_TUNE3_REG, 0x78);
		if (rc < 0)
			debug(INFO,"%s: PHY_TUNE3 reg set"
						" failed ret=%d\n", DRV_ID, rc);

		/* Switch to normal mode/disable Bank 0x12 access */
		rc = ab8500_reg_write(AB8500_DEBUG_TESTMODE_REG, 0x00);
		if (rc < 0)
			debug(INFO,"%s: bank12 access disable"
					" failed ret=%d\n", DRV_ID, rc);
	}

	/* enable phy in peripherial (device) mode */
	rc = ab8500_reg_mask_and_set(AB8500_USB_PHY_CTRL_REG,
			AB8500_BIT_PHY_CTRL_DEVICE_EN,
			AB8500_BIT_PHY_CTRL_DEVICE_EN);
	if (rc < 0) {
		debug(CRITICAL,"%s: phy enable failed, ret=%d\n", DRV_ID, rc);
		return -1;
	}

	debug(INFO,"%s: init done\n",  DRV_ID);
	return USB_AB8500_RC_OK;
}
