/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ALSACTRL_HWH_U85XX_H
#define ALSACTRL_HWH_U85XX_H

#include <sqlite3.h>

#include "alsactrl_hwh.h"

enum direction_type {
	DIRECTION_IN = 0,
	DIRECTION_OUT = 1
};

enum regulator {
	REGULATOR_VAMIC1 = 0,
	REGULATOR_VAMIC2 = 1
};

enum fm_type {
	FM_TYPE_ANALOG = 0,
	FM_TYPE_DIGITAL = 1
};

// FM-radio
static const enum fm_type FMRX_TYPE = FM_TYPE_DIGITAL;

// Mic-biases
static const enum regulator MIC_1A_REGULATOR = REGULATOR_VAMIC1;
static const enum regulator MIC_1B_REGULATOR = REGULATOR_VAMIC1;
static const enum regulator MIC_2_REGULATOR = REGULATOR_VAMIC2;

typedef struct _u85xx_device_t {
	int id;
	const char* name;
	enum direction_type direction;
	int active;
	int (*set_state)(struct _u85xx_device_t *dev);
	int (*device_handler)(struct _u85xx_device_t *dev, sqlite3* db_p);
	void* pdata;
} u85xx_device_t;

typedef struct _u85xx_switch_t {
	int id;
	const char* name;
	int (*set_enabled)(struct _u85xx_switch_t *sw, u85xx_device_t *devices, sqlite3* db_p);
} u85xx_switch_t;

typedef struct _u85xx_d2d_t {
	const char* name_src;
	const char* name_dst;
	int active;
	int (*d2d_handler)(struct _u85xx_d2d_t *d2d);
} u85xx_d2d_t;

/*
* When applying sidetone FIR-filter, this value
* sets the maximum number of tries before giving up
*/
#define MAX_TRIES_SID 5

#define ALSACTRL_IDX_DEFAULT_STATE 0

// Public interface
int IsComboDevice(const char* name);
void SetActive(u85xx_device_t *dev, bool active);
int SetSwitch(u85xx_switch_t *sw, bool enable);
int SetSidetoneRoute(sqlite3* db_p, const char *indev_p, bool *sidetone_left, bool *sidetone_right);
int SetSidetoneGain(sqlite3_stmt *stmt, int *sidetone_gain);
int SetSidetoneCoeffs(sqlite3_stmt *stmt);
int SetSidetoneEnable(bool sidetone_left, bool sidetone_right);
int SetANC(sqlite3_stmt *stmt);
int SetFilters(sqlite3* db_p, const char* indev_p, const char* outdev_p, int fs);

#endif
