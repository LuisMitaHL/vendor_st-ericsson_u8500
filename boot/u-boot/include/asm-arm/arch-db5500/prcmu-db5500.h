/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * License Terms: GNU General Public License v2
 *
 * U5500 PRCMU API.
 */
#ifndef __PRCMU_U5500_H
#define __PRCMU_U5500_H

int prcmu_init(void);
int prcmu_abb_read(u8 slave, u8 reg, u8 *value, u8 size);
int prcmu_abb_write(u8 slave, u8 reg, u8 *value, u8 size);

#endif /* __PRCMU_U5500_H */

