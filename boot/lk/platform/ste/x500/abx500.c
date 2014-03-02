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
#include <kernel/thread.h>
#include <kernel/timer.h>
#include <platform/timer.h>
#include "abx500.h"
#include "prcmu.h"


#define CHIP_ID_INVALID	-1
#define WDOG_EN_BIT		1
#define WDOG_KICK_BIT	2
#define WDOG_DISABLE	0
#define EVER			;;

typedef struct {
	int			chip_id;
	uint8_t		wd_ctrl_reg;
	bool		wd_timer_active;
	timer_t		timer;
} ab8500_device_t;

static ab8500_device_t ab8500_dev;

/*
 * ab8500_reg_mask_and_set - AB8500 - mask and set AB8500 reg
 * @bank_reg: - ab8500 bank and register to be accessed
 * @mask    :  -ab8500 register mask
 * @val     :  -ab8500 register value
 * Returns:  status of operation performed
 */
int ab8500_reg_mask_and_set(uint16_t bank_reg,
	uint8_t mask, uint8_t val)
{
	uint8_t bank, reg;
	int rc;

	bank = (uint8_t)(bank_reg >> 8u);
	reg= (uint8_t)bank_reg;

	rc = ab8500_read(bank, reg);
	if (rc < 0) {
		dprintf(SPEW, "ab8500: failed to read bank %#x reg %#x: %d\n",
			bank, reg, rc);
		return rc;
	}

	val = (~mask & (uint8_t)rc) | (mask & val);

	rc = ab8500_write(bank, reg, val);
	if (rc < 0) {
		dprintf(SPEW, "ab8500: failed to write bank %#x reg %#x: %d\n",
			bank, reg, rc);
	}

	return rc;
}

/*
 * ab8500_reg_read - AB8500 - read AB8500 reg
 * @bank_reg: - ab8500 bank and register to be accessed
 * Returns:  status of operation performed or register's value
 */
int ab8500_reg_read(uint16_t bank_reg)
{
	int rc;
	uint8_t bank, reg;

	bank = (uint8_t)(bank_reg >> 8u);
	reg= (uint8_t)bank_reg;

	enter_critical_section();
	rc = ab8500_read(bank, reg);
	exit_critical_section();

	return rc;
}

/*
 * ab8500_reg_write - AB8500 - write to AB8500 reg
 * @bank_reg: - ab8500 bank and register to be accessed
 * @val     :  -ab8500 register value
 * Returns:  status of operation performed
 */
int ab8500_reg_write(uint16_t bank_reg, uint8_t val)
{
	int rc;
	uint8_t bank, reg;

	bank = (uint8_t)(bank_reg >> 8u);
	reg= (uint8_t)bank_reg;

	enter_critical_section();
	rc = ab8500_write(bank, reg, val);
	exit_critical_section();

	return rc;
}

static int chip_id_get(void)
{
	int rc;
	int ab8500_cutid;
	int ab_ic_name;

	/* Get CutID */
	rc = ab8500_reg_read(AB8500_REV_REG);
	if(rc < 0)
		goto chip_id_get_err;
	ab8500_cutid = rc;

	/* Get AB IC_NAME (for AB8505 and upwards) */
	rc = ab8500_reg_read(AB8500_IC_NAME_REG);
	if(rc < 0) {
		ab_ic_name = 0;
	} else {
		ab_ic_name = rc;
	}

	/* Concatenate cutid and ic_name */
	rc = (ab_ic_name << 8) | ab8500_cutid;
	return rc;

chip_id_get_err:
	dprintf(CRITICAL, "ab8500: invalid chip_id: %d\n",rc);
	return rc;
}

static enum handler_return
main_wdog_kick(struct timer *timer, time_t now, void *arg)
{
	int rc;

	/* kick the dog */
	rc = ab8500_reg_write(AB8500_MAIN_WDOG_CTRL_REG, ab8500_dev.wd_ctrl_reg | WDOG_KICK_BIT);
	if (rc < 0)
		dprintf(SPEW,"main_wdog_kick: failed to write wdog ctrl reg\n");
	return INT_RESCHEDULE;
}

static int main_wdog_check(void)
{
	int rc;

	/* read wdog control reg */
	rc = ab8500_reg_read(AB8500_MAIN_WDOG_CTRL_REG);
	if (rc < 0) {
		dprintf(SPEW,"main_wdog_check: failed to read wdog ctrl reg\n");
		return -1;
	}

	/* check if the wdog is enabled */
	if (!(rc & WDOG_EN_BIT)) {
		dprintf(INFO,"main_wdog_check: wdog is off, enable and kick it once\n");
		/* enable WD */
		rc = ab8500_reg_write(AB8500_MAIN_WDOG_CTRL_REG,WDOG_EN_BIT);
		if (rc < 0) {
			dprintf(SPEW,"main_wdog_check: failed to write wdog ctrl reg\n");
			return -1;
		}
		udelay(100);
		/* kick WD */
		rc = ab8500_reg_write(AB8500_MAIN_WDOG_CTRL_REG,WDOG_EN_BIT|WDOG_KICK_BIT);
		if (rc < 0) {
			dprintf(SPEW,"main_wdog_check: failed to write wdog ctrl reg\n");
			return -1;
		}
		udelay(100);
		/* disable WD*/
		rc = ab8500_reg_write(AB8500_MAIN_WDOG_CTRL_REG,WDOG_DISABLE);
		if (rc < 0) {
			dprintf(SPEW,"main_wdog_check: failed to write wdog ctrl reg\n");
			return -1;
		}
		return 0;
	}
	ab8500_dev.wd_ctrl_reg = (uint8_t)rc;

	/* read wdog timer reg */
	rc = ab8500_reg_read(AB8500_MAIN_WDOG_TIMER_REG);
	if (rc < 0) {
		dprintf(SPEW,"main_wdog_check: failed to read wdog timer reg\n");
		return -1;
	}

	/* calculate kick timeout in msec */
	rc = ((unsigned)rc >> 1u) * 1000u;
	dprintf(INFO,"main_wdog_check: kick timeout: %d\n", rc);

	/* create and init the timer object */
	timer_initialize(&ab8500_dev.timer);
	timer_set_periodic(&ab8500_dev.timer,rc,main_wdog_kick,NULL);
	ab8500_dev.wd_timer_active = true;

	return 0;
}

/*
 * ab8500_dev_init - AB8500 - initialization of AB8500 device
 * Returns:  status of operation performed
 */
int ab8500_dev_init(void)
{
	int rc = 0;

	/* init context variables */
	ab8500_dev.wd_timer_active = false;

	/* init prcmu driver */
	void prcmu_init();

	/* read out chip id */
	rc = chip_id_get();
	if(rc < 0) {
		dprintf(SPEW,"ab8500_dev_init: failed read chip id: %d\n", rc);
		ab8500_dev.chip_id = CHIP_ID_INVALID;
		return rc;
	}
	ab8500_dev.chip_id = rc;

	/*
	 * Check the stait of main wdog. If it is ON,
	 * setup the thread which kicks it periodically.
	 */
	rc = main_wdog_check();
	if (rc)
		return rc;

	dprintf(INFO,"ab8500_dev_init: done\n");
	return 0;
}

/*
 * ab8500_dev_close - AB8500 - finalize usage of AB8500 device
 * Returns:  status of operation performed
 */
int ab8500_dev_close(void)
{
	if (ab8500_dev.wd_timer_active) {
		enter_critical_section();
		timer_cancel(&ab8500_dev.timer);
		exit_critical_section();
		ab8500_dev.wd_timer_active = false;
		dprintf(INFO,"ab8500_dev_close: timer disabled\n");
	}
	dprintf(INFO,"ab8500_dev_close: done\n");
	return 0;
}

/*
 * ab8500_chip_id_read - AB8500 - read AB8500 chip id
 * Returns:  chip id
 */
int ab8500_chip_id_read(void)
{
	return ab8500_dev.chip_id;
}
