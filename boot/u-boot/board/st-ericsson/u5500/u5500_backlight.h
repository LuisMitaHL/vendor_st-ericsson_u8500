/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Sesahgiri.Holi <seshagiri.holi@stericsson.com> for ST-Ericsson
 * License terms: GNU General Public License (GPL), version 2.
 */

#ifndef __U5500_BACKLIGHT_H__
#define __U5500_BACKLIGHT_H__

#define LM3530_BL_MODE_MANUAL 0

#define LM3530_FS_CURR_5mA		(0) /* Full Scale Current */
#define LM3530_FS_CURR_8mA		(1)
#define LM3530_FS_CURR_12mA		(2)
#define LM3530_FS_CURR_15mA		(3)
#define LM3530_FS_CURR_19mA		(4)
#define LM3530_FS_CURR_22mA		(5)
#define LM3530_FS_CURR_26mA		(6)
#define LM3530_FS_CURR_29mA		(7)

#define LM3530_ALS_AVRG_TIME_32ms	(0) /* ALS Averaging Time */
#define LM3530_ALS_AVRG_TIME_64ms	(1)
#define LM3530_ALS_AVRG_TIME_128ms	(2)
#define LM3530_ALS_AVRG_TIME_256ms	(3)
#define LM3530_ALS_AVRG_TIME_512ms	(4)
#define LM3530_ALS_AVRG_TIME_1024ms	(5)
#define LM3530_ALS_AVRG_TIME_2048ms	(6)
#define LM3530_ALS_AVRG_TIME_4096ms	(7)

#define LM3530_RAMP_TIME_1ms		(0) /* Brigtness Ramp Time */
#define LM3530_RAMP_TIME_130ms		(1) /* Max to 0 and vice versa */
#define LM3530_RAMP_TIME_260ms		(2)
#define LM3530_RAMP_TIME_520ms		(3)
#define LM3530_RAMP_TIME_1s		(4)
#define LM3530_RAMP_TIME_2s		(5)
#define LM3530_RAMP_TIME_4s		(6)
#define LM3530_RAMP_TIME_8s		(7)

/* ALS Resistor Select */
#define LM3530_ALS_IMPD_Z		(0x00) /* ALS Impedance */
#define LM3530_ALS_IMPD_13_53kOhm	(0x01)
#define LM3530_ALS_IMPD_9_01kOhm	(0x02)
#define LM3530_ALS_IMPD_5_41kOhm	(0x03)
#define LM3530_ALS_IMPD_2_27kOhm	(0x04)
#define LM3530_ALS_IMPD_1_94kOhm	(0x05)
#define LM3530_ALS_IMPD_1_81kOhm	(0x06)
#define LM3530_ALS_IMPD_1_6kOhm		(0x07)
#define LM3530_ALS_IMPD_1_138kOhm	(0x08)
#define LM3530_ALS_IMPD_1_05kOhm	(0x09)
#define LM3530_ALS_IMPD_1_011kOhm	(0x0A)
#define LM3530_ALS_IMPD_941Ohm		(0x0B)
#define LM3530_ALS_IMPD_759Ohm		(0x0C)
#define LM3530_ALS_IMPD_719Ohm		(0x0D)
#define LM3530_ALS_IMPD_700Ohm		(0x0E)
#define LM3530_ALS_IMPD_667Ohm		(0x0F)



#endif /* __U5500_BACKLIGHT_H__ */
