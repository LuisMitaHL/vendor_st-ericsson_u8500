/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Mikael Larsson <mikael.xt.larsson@stericsson.com> for ST-Ericsson.
 *
 * License terms: GNU General Public License (GPL), version 2.
 */
#include <common.h>
#include <part.h>
#include <asm/arch/itp.h>
#include <cspsa_fp.h>

static u32 cspsa_key;

/*
 * itp_load_itp - Loads itp.
 * If itp is loaded ok it will be executed and u-boot execution will stop
 */
int itp_load_itp(block_dev_desc_t *block_dev)
{
	u32 offset;
	u32 size;
	u32 loadaddress;

	debug("\nitp_load_itp\n");

	if (toc_load_toc_entry(block_dev, CONFIG_ITP_TOC_ITP_NAME, 0, 0, 0))
		return 1;

	if (get_entry_info_toc(block_dev, CONFIG_ITP_TOC_ITP_NAME, &offset,
			       &size, &loadaddress))
		return 1;

	((void(*)(void))loadaddress)(); /* U-boot execution will end here */

	return 1; /* Should not get here */
}

int itp_is_itp_in_config(void)
{
	return cspsa_key & ITP_LOAD_ITP;
}


int itp_is_modem_in_config(void)
{
	return cspsa_key & ITP_LOAD_MODEM;
}

int itp_is_lbp_modem_in_config(void)
{
        return cspsa_key & ITP_LOAD_MODEM_LBP;
}

int itp_read_config(block_dev_desc_t *block_dev)
{
	printf("  ITP_CONFIG:  ");
	if (cspsa_fp_read(block_dev,
			  ITP_CSPSA_KEY,
			  &cspsa_key)) {
		printf("not present. Using default values");
		cspsa_key = ITP_LOAD_KERNEL;
	} else {
		if (cspsa_key & ITP_LOAD_KERNEL)
			printf("load kernel ");
		if (cspsa_key & ITP_LOAD_MODEM)
			printf("load modem ");
		if (cspsa_key & ITP_LOAD_ITP)
			printf("load itp ");
		if (cspsa_key & ITP_LOAD_MODEM_LBP)
			printf("load modem in lbp mode ");
	}
	printf("\n");
	return 0;
}
