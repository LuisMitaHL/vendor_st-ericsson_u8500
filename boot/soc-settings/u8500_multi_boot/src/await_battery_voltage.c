/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Paer-Olof Haakansson <par-olof.hakansson at stericsson.com>
 *  for ST-Ericsson.
 *
 */
#include <types.h>
#include <io.h>
#include <log.h>
#include <prcmu.h>
#include <soc_settings.h>
#include <await_battery_voltage.h>
#include <ab8500.h>

#define PRCM_ARM_PLLDIVPS	0x118
#define PRCM_TIMER_4_DOWNCOUNT	0x454
#define PRCM_YYCLKEN0_MGT_SET	0x510
#define PRCM_YYCLKEN0_MGT_CLR	0x518
#define PRCM_YYCLKEN0_MGT_VAL	0x520
#define PRCM_ARM_PLLDIVPS_REQ	0x5B0
#define PRCM_ARM_PLLDIVPS_ACK	0x5B4

/* Use Binary Rate Multiplier (BRM)of the ARM clock generator
 * to run clock at 1/32nd frequency
 */
#define RATIO_OUT_OF_32  (1)

#define ONE_32KHZ_SECOND 32768
#define ONE_TENTH_32KHZ_SECOND 3277
#define ONE_HUNDREDTH_32KHZ_SECOND 328
#define ONE_MILLI_32KHZ_SECOND 33
#define RESET_IF_DISCONNECTED 30
#define CHECK_PERIOD 8

#define WALL_CHARGER 0x00
#define USB_CHARGER  0x01
#define BATT_VOLT_WALL 0x1C3 /* 3.4V */
#define BATT_VOLT_USB  0x1C3 /* 3.4V */

#define BATT_VOLT_STOP 0x1C3 /* 3.4V */

#define AB8500_CH_BATTOVV_REG_VAL		(BATTOVV_ENA | BATTOVVTH_475)
#define AB8500_CH_VOLT_LVL_REG_VAL_39		0x10
#define AB8500_CH_VOLT_LVL_REG_VAL_41		0x18
#define AB8500_CH_VOLT_LVL_REG_VAL		AB8500_CH_VOLT_LVL_REG_VAL_41

#define CURRENT_LVL_MAX				CURRENT_LVL_600MA

#define NO_INPUT_VAL 0xFF
#define CURR_BACKED_OFF 0x01
#define CURR_OK 0x00

static void turn_on_charging_led(void)
{
	/* 2 mA, enable */
	hwi2c_write(AB8500_LEDINDICATORPWMCTRL, LEDINDICATORPWMENA);
	/* 100% PWM */
	hwi2c_write(AB8500_LEDINDICATORPWMDUTY, 0xFF);
}

static void turn_off_charging_led(void)
{
	hwi2c_write(AB8500_LEDINDICATORPWMCTRL, 0x00);
}

static void khz32_delay(int count)
{
	int now;
	int start;

	now = read32(PRCMU_BASE + PRCM_TIMER_4_DOWNCOUNT);
	start = now;
	while (start - now  < count+2)
		now = read32(PRCMU_BASE + PRCM_TIMER_4_DOWNCOUNT);
}

static void flash_charging_led(void)
{
	u8 i;

	for (i = 0; i < 10; i++) {
		turn_on_charging_led();
		khz32_delay(ONE_32KHZ_SECOND >> 3);
		turn_off_charging_led();
		khz32_delay(ONE_32KHZ_SECOND >> 3);
	}
}

static void power_off(void)
{
	u8 bits;

	bits = AB8500_STW4500CTRL1_SWOFF | AB8500_STW4500CTRL1_SWRESET;

	hwi2c_masked_write(AB8500_STW4500CTRL1,
			   bits,
			   bits);
}

static void khz32_delay_check_charging(int count,
				       int *time_when_disconnected,
				       u8 *check_disconnect,
				       u8 *check_connect,
				       u8 *wait_stable,
				       u8 *stable_count)
{
	int now;
	int start;
	int check_usb;
	u8 charger_status;
	u8 check_val1;
	u8 check_val2;

	now = read32(PRCMU_BASE + PRCM_TIMER_4_DOWNCOUNT);
	start = now;
	check_usb = now;
	while (start - now  < count+2) {
		now = read32(PRCMU_BASE + PRCM_TIMER_4_DOWNCOUNT);
		if (check_usb - now > ONE_TENTH_32KHZ_SECOND) {
			/* Check USB cable and status 10 times / second */
			check_usb = now;

			/* Check BattOVV BattOVVth=3.75V, battOVV enabled */
			hwi2c_read(AB8500_CH_BATTOVV, &check_val1);

			/* ChVoltLevel = 3.9V */
			hwi2c_read(AB8500_CH_VOLT_LVL_REG, &check_val2);

			if ((check_val1 != AB8500_CH_BATTOVV_REG_VAL) ||
			    (check_val2 != AB8500_CH_VOLT_LVL_REG_VAL)) {
				loginfo("Battery protection registers"
					" have changed: %x %x",
					check_val1, check_val2);
				loginfo("Shutting down to protect"
					" platform...");
				power_off();
				while (1)
					;
			}

			hwi2c_read(AB8500_CH_USBCH_STAT1_REG, &charger_status);
			if (*check_disconnect &&
			    ((charger_status & USBCHON) == 0)) {
				loginfo("USB charging stopped: %x",
					charger_status);
				/* Clear charging bits  */
				hwi2c_write(AB8500_USBCH_CTRL1_REG, 0x00);
				turn_off_charging_led();
				*check_disconnect = 0;
				*wait_stable = 1;
			} else if (*check_connect) {
				if ((charger_status & 0x03) ==
				    (VBUSDETDBNCFLT | VBUSDETDBNC)) {
					enable_usb_charging();
					loginfo("USB charging restarted");

					*check_connect = 0;
					*check_disconnect = 1;
				} else if (*time_when_disconnected - now >
					 RESET_IF_DISCONNECTED  * ONE_32KHZ_SECOND) {
					loginfo("USB detatched %x seconds",
						RESET_IF_DISCONNECTED);
					loginfo("Resetting in 1 second"
						" unless cable attached again");
					power_off();
					*time_when_disconnected =
						read32(PRCMU_BASE +
						       PRCM_TIMER_4_DOWNCOUNT);
				}
			} else if (*wait_stable) {
				/* Wait for VBus to stabilize to avoid
				 * false reconnect detection.
				 * Also makes it possible to restart
				 * charging if there is an erroneous
				 * batt temp interrupt and the VBus
				 * status stays high.
				 */
				(*stable_count)++;

				if (((charger_status & 0x03) == 0) ||
				    ((*stable_count) > 10)) {
					*wait_stable = 0;
					*stable_count = 0;
					*check_connect = 1;
					*time_when_disconnected =
						read32(PRCMU_BASE + PRCM_TIMER_4_DOWNCOUNT);
				}
			}
		}
	}
}

static int khz32_get_timeout_time(u32 seconds)
{
	seconds = seconds << 15;
	return (int)(read32(PRCMU_BASE + PRCM_TIMER_4_DOWNCOUNT) - seconds);
}

static u8 khz32_check_timeout_time(int count)
{
	return (int)read32(PRCMU_BASE + PRCM_TIMER_4_DOWNCOUNT) < count;
}

static void enable_brm(u32 ratio_out_of_32)
{
	/* Configure the Binary Rate Multiplier (BRM)of the ARM clock generator
	 * to make the CPU run as slowly as possible to save power
	 */
	write32(PRCMU_BASE + PRCM_ARM_PLLDIVPS_REQ, 0);

	while ((read32(PRCMU_BASE + PRCM_ARM_PLLDIVPS_ACK) & 0x01) != 0)
		;

	write32(PRCMU_BASE + PRCM_ARM_PLLDIVPS,
		(read32(PRCMU_BASE + PRCM_ARM_PLLDIVPS) & ~(0x3f)) |
		(ratio_out_of_32));

	write32(PRCMU_BASE + PRCM_ARM_PLLDIVPS_REQ, 1);

	while ((read32(PRCMU_BASE + PRCM_ARM_PLLDIVPS_ACK) & 0x01) != 1)
		;
}

static void disable_brm(void)

{
	write32(PRCMU_BASE + PRCM_ARM_PLLDIVPS_REQ, 0);

	while ((read32(PRCMU_BASE + PRCM_ARM_PLLDIVPS_ACK) & 0x01) != 0)
		;

	write32(PRCMU_BASE + PRCM_ARM_PLLDIVPS,
		(read32(PRCMU_BASE + PRCM_ARM_PLLDIVPS) & ~(0x3f)) | 0x20);

	write32(PRCMU_BASE + PRCM_ARM_PLLDIVPS_REQ, 1);

	while ((read32(PRCMU_BASE + PRCM_ARM_PLLDIVPS_ACK) & 0x01) != 1)
		;
}

static void prepare_adc_vbat_measurement(void)
{
	/* VTVoutEna (bit1 = 1) Keep it turned on, turning it on/off
	 * to save power will disturb btemp measurments and result in
	 * false btemp interrupts stopping the charging.
	 */
	hwi2c_masked_write(AB8500_REGU_MISC1, 0x2, 0x2);

	/* Wait for it to take effect */
	khz32_delay(ONE_MILLI_32KHZ_SECOND);

	/* Enable ADC */
	hwi2c_masked_write(AB8500_GPADC_CTRL1_REG, ADCENA, ADCENA);

	/* Wait for it to take effect */
	khz32_delay(ONE_MILLI_32KHZ_SECOND);

	/* Select channel and # of average samples */
	hwi2c_masked_write(AB8500_GPADC_CTRL2_REG,
			   0x7F,
			   ADCSWAVERAGE_4 | VBATA_CH);
	/* Enable buffer */
	hwi2c_masked_write(AB8500_GPADC_CTRL1_REG, BUFENA, BUFENA);

}

static u16 measure_vbat(void)
{
	u8 itsource_5;
	u8 low_data;
	u8 high_data;

	/* Enable GPADC */
	hwi2c_masked_write(AB8500_GPADC_CTRL1_REG, ADCENA, ADCENA);

	/* Wait for it to take effect */
	khz32_delay(ONE_MILLI_32KHZ_SECOND);

	/* Launch an ADC conversion */
	hwi2c_masked_write(AB8500_GPADC_CTRL1_REG,
			   ADCSWCONVERT,
			   ADCSWCONVERT);

	do {
		hwi2c_read(AB8500_ITSOURCE_5, &itsource_5);
	} while (!(itsource_5 & GPADCSWCONVEND));

	hwi2c_read(AB8500_GPADC_MANDATAL_REG, &low_data);
	hwi2c_read(AB8500_GPADC_MANDATAH_REG, &high_data);

	/* Disable GPADC */
	hwi2c_masked_write(AB8500_REGU_MISC1, ADCENA, 0x0);

	return (high_data << 8) | low_data;
}

static u8 check_charger_type(u32 *batt_volt)
{
	u8  usb_line_status = 0;
	u8 batt_ok_val;

	hwi2c_read(AB8500_BATTOK, &batt_ok_val);
	loginfo("BattOK reg %x", batt_ok_val);

	hwi2c_read(AB8500_USBLINESTATUS, &usb_line_status);

	u8 usb_link_status = (usb_line_status & 0x78) >> 3;
	if (((usb_line_status & AB8500_USBLINESTATUS_VdatDet) != 0) ||
	    usb_link_status == 0x4 ||
	    usb_link_status == 0x5 ||
	    usb_link_status == 0x6 ||
	    usb_link_status == 0x7) {
		loginfo("VBATT_VOLT_WALL usb line %x %x",
			usb_line_status, usb_link_status);
		if (batt_volt != NULL)
			*batt_volt = BATT_VOLT_WALL;
		return WALL_CHARGER;
	} else {
		loginfo("VBATT_VOLT_USB usb line %x %x", usb_line_status,
			usb_link_status);
		if (batt_volt != NULL)
			*batt_volt = BATT_VOLT_USB;
		return USB_CHARGER;
	}
}

void await_required_battery_voltage(void)
{
	int time_to_feed_wdogs;
	u32 wdogs_feed_period;
	u8 charg_wd_timer_l;
	u8 charg_wd_timer_h;
	u16 charg_wd_timer;
	u8 main_wd_timer;
	u16 loop_count = 0;
	u32 prcm_yyclken0_mgt_val_save;
	u16 vbat;
	u32 batt_volt = 0;

	(void)check_charger_type(&batt_volt);

	/* Declare these variable here instead of in
	 * khz32_delay_check_charging() since current
	 * linker script can't handle static variables
	 * properly...
	 */
	int time_when_disconnected = 0;
	u8 check_disconnect = 1;
	u8 check_connect = 0;
	u8 wait_stable = 0;
	u8 stable_count = 0;

	prcm_yyclken0_mgt_val_save =
		read32(PRCMU_BASE + PRCM_YYCLKEN0_MGT_VAL);

	/* Disable what is not need during charging */
	write32(PRCMU_BASE + PRCM_YYCLKEN0_MGT_CLR, 0xC0115E80);

	enable_brm(RATIO_OUT_OF_32);

	/* Get timeout settings for the watchdogs */
	hwi2c_read(AB8500_CH_WD_TIMER, &charg_wd_timer_l);
	hwi2c_read(AB8500_CH_WD_CTRL, &charg_wd_timer_h);

	charg_wd_timer = ((charg_wd_timer_h & 0x02) << 7) | charg_wd_timer_l;

	hwi2c_read(AB8500_MAINWDTIMER, &main_wd_timer);

	/* Feed watch dogs at half the lowest period of the two */
	wdogs_feed_period =
		charg_wd_timer < main_wd_timer ?
		charg_wd_timer : main_wd_timer;

	wdogs_feed_period = wdogs_feed_period / 2;

	time_to_feed_wdogs = khz32_get_timeout_time(wdogs_feed_period);

	prepare_adc_vbat_measurement();

	do {
		vbat = measure_vbat();

		if (khz32_check_timeout_time(time_to_feed_wdogs)) {
			time_to_feed_wdogs =
				khz32_get_timeout_time(wdogs_feed_period);
			hwi2c_write(AB8500_MAINWD_REG, 0x03);
			hwi2c_write(AB8500_CH_WD_CTRL, 0x1);
		}

		loginfo("VBat %x %x",
			loop_count * CHECK_PERIOD,
			vbat);

		loop_count++;

		/* Don't wait if voltage already OK.
		 * Also don't try disconnecting / connecting in case the USB
		 * cable was inserted to flash the device and not charge it
		 */
		if (vbat < batt_volt) {
			khz32_delay_check_charging(ONE_32KHZ_SECOND *
						   CHECK_PERIOD,
						   &time_when_disconnected,
						   &check_disconnect,
						   &check_connect,
						   &wait_stable,
						   &stable_count);
		}

	} while (vbat < batt_volt);

	disable_brm();

	/* Turn off watch dogs when we are finished */
	hwi2c_write(AB8500_MAINWD_REG, 0x0);
	hwi2c_write(AB8500_CH_WD_CTRL, 0x0);
	hwi2c_write(AB8500_CH_WD_TIMER, 0x01);

	/* Enable again what was set before in the boot */
	write32(PRCMU_BASE + PRCM_YYCLKEN0_MGT_SET, prcm_yyclken0_mgt_val_save);

}

static void start_charging(u8 usbch_ipt_crntlvl_reg_val, u8 ch_opt_crntlvl_reg_val)
{
	u8 charger_status;

	/* If rising battery voltage triggered VBUSCHNOK,
	 * clear interrupt
	 */
	hwi2c_read(AB8500_CH_USBCH_STAT2_REG, &charger_status);

	if (charger_status & VBUSCHNOK)
		hwi2c_read(AB8500_ITLATCH_3, &charger_status);

	/* Rewrite current values when taking over
	 * from HW (advice from HW engi)
	 */
	if (usbch_ipt_crntlvl_reg_val == NO_INPUT_VAL)
		hwi2c_read(AB8500_USBCH_IPT_CRNTLVL_REG,
			   &usbch_ipt_crntlvl_reg_val);

	hwi2c_write(AB8500_USBCH_IPT_CRNTLVL_REG, usbch_ipt_crntlvl_reg_val);

	if (ch_opt_crntlvl_reg_val == NO_INPUT_VAL)
		hwi2c_read(AB8500_CH_OPT_CRNTLVL_REG, &ch_opt_crntlvl_reg_val);
	hwi2c_write(AB8500_CH_OPT_CRNTLVL_REG, ch_opt_crntlvl_reg_val);

	hwi2c_write(AB8500_CH_BATTOVV,
		    AB8500_CH_BATTOVV_REG_VAL);

	hwi2c_write(AB8500_CH_VOLT_LVL_REG,
		    AB8500_CH_VOLT_LVL_REG_VAL);

	/* mainwatchdogtimer = 127  */
	hwi2c_write(AB8500_MAINWDTIMER, 0x7F);

	khz32_delay(2);

	/* MainWDogEna=1,  WDexpTurnOnValid = 1 */
	hwi2c_write(AB8500_MAINWD_REG, 0x13);

	/*
	 * Due to internal synchronisation, Enable and Kick watchdog bits
	 * cannot be enabled in a single write.
	 * A minimum delay of 2*32 kHz period (62.5 microseconds) must be
	 * inserted between writing Enable then Kick bits.
	 */
	khz32_delay(2);

	/* Kick charger wd */
	hwi2c_write(AB8500_CH_WD_CTRL, 0x1);
	hwi2c_write(AB8500_MAINWD_REG, 0x013);

	/* UsbChCtrl1 UsbChgNoOverShootEnaN=1, UsbChEna=1 */
	hwi2c_write(AB8500_USBCH_CTRL1_REG, 0x01);
}

static u8 check_if_back_off(u8 usbch_ipt_crntlvl_reg_val,
		     u8 ch_opt_crntlvl_reg_val)
{
	u8 charger_status_usb;

	hwi2c_read(AB8500_CH_USBCH_STAT1_REG, &charger_status_usb);
	loginfo("check_if_back_off %x %x %x",
		usbch_ipt_crntlvl_reg_val,
		ch_opt_crntlvl_reg_val,
		charger_status_usb);

	if ((charger_status_usb & USBCHON) == 0) {
		hwi2c_write(AB8500_USBCH_CTRL1_REG, 0x00);
		loginfo("Maxed out %x %x",
			usbch_ipt_crntlvl_reg_val,
			ch_opt_crntlvl_reg_val);
		start_charging(usbch_ipt_crntlvl_reg_val,
			       0x00);
		return CURR_BACKED_OFF;
	} else {
		return CURR_OK;
	}
}

static void increase_ch_current(u8 use_hw_set_crntlvl)
{
	u8 usbch_ipt_crntlvl_reg_val;
	u8 ch_opt_crntlvl_reg_val;
	u8 ch_opt_crntlvl_reg_val_max = CH_OUTPUT_CURRENT_LEVEL_1000MA;

	hwi2c_read(AB8500_USBCH_IPT_CRNTLVL_REG, &usbch_ipt_crntlvl_reg_val);

	/* Increase current in 100 mA steps */
	while (usbch_ipt_crntlvl_reg_val < CURRENT_LVL_MAX) {
		usbch_ipt_crntlvl_reg_val =
			usbch_ipt_crntlvl_reg_val + IPT_CURRENT_LVL_INCREASE;
		hwi2c_write(AB8500_USBCH_IPT_CRNTLVL_REG,
			    usbch_ipt_crntlvl_reg_val);
		khz32_delay(ONE_TENTH_32KHZ_SECOND);
		hwi2c_read(AB8500_USBCH_IPT_CRNTLVL_REG,
			   &usbch_ipt_crntlvl_reg_val);
	}

	if (use_hw_set_crntlvl)	{
		/* Only first time when booted, reuse what was
		 * already set
		 */
		hwi2c_read(AB8500_CH_OPT_CRNTLVL_REG, &ch_opt_crntlvl_reg_val);
	} else {
		hwi2c_write(AB8500_CH_OPT_CRNTLVL_REG, 0);
		ch_opt_crntlvl_reg_val = 0;
	}

start_ramping:
	/* Increase current in 100 mA steps */
	while (ch_opt_crntlvl_reg_val < ch_opt_crntlvl_reg_val_max) {
		ch_opt_crntlvl_reg_val += CRNTLVL_INCREASE;
		hwi2c_write(AB8500_CH_OPT_CRNTLVL_REG, ch_opt_crntlvl_reg_val);

		if (check_if_back_off(
			    usbch_ipt_crntlvl_reg_val,
			    ch_opt_crntlvl_reg_val) == CURR_BACKED_OFF) {
			/* The highest current seems to be achieved if
			 * the whole ramping is done again from the beginning
			 */
			ch_opt_crntlvl_reg_val_max =
				ch_opt_crntlvl_reg_val - 1;
			if (ch_opt_crntlvl_reg_val_max == 0)
				return;
			ch_opt_crntlvl_reg_val = 0;
			goto start_ramping;
		}

		khz32_delay(ONE_TENTH_32KHZ_SECOND);
		hwi2c_read(AB8500_CH_OPT_CRNTLVL_REG, &ch_opt_crntlvl_reg_val);
	}
}

u8 enable_usb_charging(void)
{
	u8 charger_status_usb;
	u8 charger_status_main;
	u8 use_hw_set_crntlvl = 0;

	hwi2c_read(AB8500_CH_USBCH_STAT1_REG, &charger_status_usb);

	hwi2c_read(AB8500_CH_MAINCHSTATUS2, &charger_status_main);

	if (charger_status_usb & USBCHON) {
		hwi2c_write(AB8500_USBCH_CTRL1_REG, 0x00);
		use_hw_set_crntlvl = 1;
	}

	/* If USB cable connected, try starting charging */
	if ((charger_status_usb & VBUSDETDBNCFLT) ||
	    (charger_status_main & MAINCHARGERDETDBNC)) {
		start_charging(NO_INPUT_VAL, NO_INPUT_VAL);

		turn_on_charging_led();
		if (check_charger_type(NULL) == WALL_CHARGER)
			increase_ch_current(use_hw_set_crntlvl);

		return 1;
	} else
		return 0;

}

void vbat_ok_to_boot_or_reset(void)
{
	u16 vbat;

	prepare_adc_vbat_measurement();

	vbat = measure_vbat();

	loginfo("Vbat check %x", vbat);
	if (vbat < BATT_VOLT_STOP) {
		loginfo("VBat below %x, resetting...",
			BATT_VOLT_STOP);
		flash_charging_led();
		power_off();
		while (1)
			;
	}
}
