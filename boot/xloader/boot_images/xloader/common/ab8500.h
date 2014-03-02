/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _AB_8500_H
#define _AB_8500_H

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "boot_secure_types.h"

#define AB8500_REGUSYSCLKREQ1HPVALID1 0x0307

typedef union {
	struct {
		t_bitfield Vsmps1SysClkReq1HPValid : 1;
		t_bitfield Vsmps2SysClkReq1HPValid : 1;
		t_bitfield Vsmps3SysClkReq1HPValid : 1;
		t_bitfield VanaSysClkReq1HPValid : 1;
		t_bitfield VpllSysClkReq1HPValid : 1;
		t_bitfield Vaux1SysClkReq1HPValid : 1;
		t_bitfield Vaux2SysClkReq1HPValid : 1;
		t_bitfield Vaux3SysClkReq1HPValid : 1;
	} bit;
	t_uint8  reg;
} t_regu_sys_clk_req1_hp_valid1;

#define AB8500_REGUHWHPREQ1VALID1 0x0309

typedef union {
	struct {
		t_bitfield Vsmps1HwHPReq1Valid : 1;
		t_bitfield Vsmps2HwHPReq1Valid : 1;
		t_bitfield Vsmps3HwHPReq1Valid : 1;
		t_bitfield VanaHwHPReq1Valid : 1;
		t_bitfield VpllHwHPReq1Valid : 1;
		t_bitfield Vaux1HwHPReq1Valid : 1;
		t_bitfield Vaux2HwHPReq1Valid : 1;
		t_bitfield Vaux3HwHPReq1Valid : 1;
	} bit;
	t_uint8  reg;
} t_regu_hw_hp_req_1_valid1;

#define AB8500_REGUHWHPREQ2VALID1 0x030B

typedef union {
	struct {
		t_bitfield Vsmps1HwHPReq2Valid : 1;
		t_bitfield Vsmps2HwHPReq2Valid : 1;
		t_bitfield Vsmps3HwHPReq2Valid : 1;
		t_bitfield VanaHwHPReq2Valid : 1;
		t_bitfield VpllHwHPReq2Valid : 1;
		t_bitfield Vaux1HwHPReq2Valid : 1;
		t_bitfield Vaux2HwHPReq2Valid : 1;
		t_bitfield Vaux3HwHPReq2Valid : 1;
	} bit;
	t_uint8  reg;
} t_regu_hw_hp_req_2_valid1;

#define AB8500_REGUCTRLDISCH2 0x0444

typedef union {
	struct {
		t_bitfield VsimDisch : 1;
		t_bitfield VanaDisch : 1;
		t_bitfield VdmicPullDownEna : 1;
		t_bitfield VpllPullDownEna : 1;
		t_bitfield VdmicDisch : 1;
		t_bitfield Reserved : 3;
	} bit;
	t_uint8  reg;
} t_regu_ctrl_disch_2;

/*
 *VAUX2 Voltage levls for eMMC
 *Voltage@VAUX2 0x0A: 2.7V
 *Voltage@VAUX2 0x0F: 3.3V
 *Voltage@VAUX2 0x0D: 2.9V
 */
#define AB8500_VAUX2_2V7                0x0A
#define AB8500_VAUX2_MAX                0x0F
#define AB8500_VAUX2_2V9                0x0D

#define AB8500_VAUX2SEL_REG             0x0420

#define AB8500_MAINWDOGCTRL             0x0201
#define AB8500_USBPHYCTRL               0x058A
#define AB8500_USBOTGCTRL               0x0587
#define AB9540_USBLINKSTATUS            0x0594
#define AB8500_USBLINESTATUS            0x0580
#define AB8500_REGIDDETCTRL1            0x0BA0
#define AB8500_REGIDDETCTRL2            0x0BA1
#define AB8500_REGIDDETCTRL3            0x0BA2
#define AB8500_REGIDDETCTRL4            0x0BA3
#define AB8500_REGIDDETVTH              0x0BA6
#define AB8500_REGIDDETSTATE            0x0BA7
#define AB8500_ITSOURCE2                0x0E01
#define AB8505_ALTERNATFUNCTION         0x1050
#define AB8505_GPIOSEL2                 0x1001
#define AB8505_GPIOSEL7                 0x1006

#endif
