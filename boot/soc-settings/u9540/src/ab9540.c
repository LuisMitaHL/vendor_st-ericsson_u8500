/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: David Paris <david.paris at stericsson.com>
 *  for ST-Ericsson.
 */
#include <config.h>
#include <types.h>
#include <prcmu.h>
#include <io.h>
#include <wait.h>
#include <ab9540_regs.h>

struct ab9540_data {
  u16 access_type;
  u16 reg_addr;
  u16 reg_val;
  u16 mask;
};

static u8 hwi2c_read(u16 addr, u8 *value)
{
	u8 regbank = addr >> 8;
	u8 regaddr = addr & 0xFF;

	TCDMnSec->ReqMb5.un.I2CopType = I2CRead;
	TCDMnSec->ReqMb5.un.SlaveAdd = (regbank & 0x1F) | 0x20;
	TCDMnSec->ReqMb5.un.HwGCEn = 0;
	TCDMnSec->ReqMb5.un.StopEn = 1;
	TCDMnSec->ReqMb5.un.RegAdd = regaddr;
	TCDMnSec->ReqMb5.un.RegVal = 0;

	/* Check IT for ackMB5 is 0*/
	wait(PRCM_ARM_IT1_VAL, PRCM_MBOX_CPU_IT17,
		0x0, "hwi2c_read");

	/* Send mb5 (IT17) interrupt */
	write32(PRCM_MBOX_CPU_SET, PRCM_MBOX_CPU_IT17);

	/* Check IT for ackMB5 */
	wait(PRCM_ARM_IT1_VAL, PRCM_MBOX_CPU_IT17,
		PRCM_MBOX_CPU_IT17, "hwi2c_read");

	if (TCDMnSec->AckMb5.un.AckMb5Status != I2C_Rd_Ok)
		return 1;

	*value = TCDMnSec->AckMb5.un.RegVal;

	/* Clear IT1 ackmb5 */
	write32(PRCM_ARM_IT1_CLR, PRCM_MBOX_CPU_IT17);
	return 0;
}

static u8 hwi2c_write(u16 addr, u8 value)
{
	u8 regbank = addr >> 8;
	u8 regaddr = addr & 0xFF;

	TCDMnSec->ReqMb5.un.I2CopType = I2CWrite;
	TCDMnSec->ReqMb5.un.SlaveAdd = (regbank & 0x1F) | 0x20;
	TCDMnSec->ReqMb5.un.HwGCEn = 0;
	TCDMnSec->ReqMb5.un.StopEn = 1;
	TCDMnSec->ReqMb5.un.RegAdd = regaddr;
	TCDMnSec->ReqMb5.un.RegVal = value;

	/* Check IT for ackMB5 is 0*/
	wait(PRCM_ARM_IT1_VAL, PRCM_MBOX_CPU_IT17,
		0x0, "hwi2c_write");

	/* Send mb5 (IT17) interrupt */
	write32(PRCM_MBOX_CPU_SET, PRCM_MBOX_CPU_IT17);

	/* Check IT for ackMB5 */
	wait(PRCM_ARM_IT1_VAL, PRCM_MBOX_CPU_IT17,
		PRCM_MBOX_CPU_IT17, "hwi2c_write");

	if (TCDMnSec->AckMb5.un.AckMb5Status != I2C_Wr_Ok)
	return 1;

	/* Clear IT1 ackmb5 */
	write32(PRCM_ARM_IT1_CLR, PRCM_MBOX_CPU_IT17);
	return 0;
}

static u8 regulator_config(struct ab9540_data *reg_config)
{
  u8 abx500_value = 0;
  u8 ret = 0;
  
  if (reg_config->access_type == 0) /* WRITE_ONLY */
    {
      ret = hwi2c_write(reg_config->reg_addr, reg_config->reg_val);
    }
  else
    {
      ret |= hwi2c_read(reg_config->reg_addr, &abx500_value);
      abx500_value = (abx500_value & ~reg_config->mask) | reg_config->reg_val;
      ret |= hwi2c_write(reg_config->reg_addr, abx500_value);
    }
  
  return ret;
}


u8 AB9540_init_fct(u8 *ab9540_settings)
{
  struct ab9540_data *reg_data = (struct ab9540_data *)ab9540_settings;
  u8 ret = 0;
  
  while (reg_data->access_type != MAX_REG)
    {
      ret = regulator_config(reg_data);
      if (ret)
	break;
      reg_data ++;
    }
  
  return ret;
}
