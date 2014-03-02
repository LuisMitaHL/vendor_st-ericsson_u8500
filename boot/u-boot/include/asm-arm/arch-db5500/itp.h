/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Mikael Larsson <mikael.xt.larsson@stericsson.com> for ST-Ericsson.
 *
 * License terms: GNU General Public License (GPL), version 2.
 */
#ifndef __ITP_H__
#define __ITP_H__

#include <part.h>

/* These values are reserved and should not be removed or changed */
#define ITP_LOAD_KERNEL		0x01
#define ITP_LOAD_MODEM		0x02
#define ITP_LOAD_ITP		0x04
#define ITP_LOAD_MODEM_LBP      0x08

#define ITP_CSPSA_KEY		0

int itp_is_itp_in_config(void);
int itp_is_modem_in_config(void);
int itp_read_config(block_dev_desc_t *block_dev);
int itp_load_itp(block_dev_desc_t *block_dev);
int itp_is_lbp_modem_in_config(void);
#endif	/* __ITP_H__ */
