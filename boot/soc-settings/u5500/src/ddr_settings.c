/*
 *  Copyright (C) 2010 ST-Ericsson AB
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson.
 */

#include <ddr_settings.h>

void set_frequency(struct frequency *set_freq)
{
	set_freq->max_cpu_freq = MCPUF_800MHZ;
#ifdef U5500C
	set_freq->max_aclk_freq = MACLKF_266MHZ;
	set_freq->max_sga_freq = MSGAF_266MHZ;
#else
	set_freq->max_aclk_freq = MACLKF_200MHZ;
	set_freq->max_sga_freq = MSGAF_200MHZ;
#endif
}

void set_memory_speed(struct memory_speed *ddr_speed)
{
#ifdef U5500C
	ddr_speed->frequency = DDRF_333MHZ;
#else
	ddr_speed->frequency = DDRF_266MHZ;
#endif
}

void hook_dmc_ddr(struct ddr_init *dm_ddr)
{
#ifdef U5500C
	hook_dmc_333mhz_multiboot(dm_ddr);
#else
	hook_dmc_266mhz_multiboot(dm_ddr);
#endif
}

