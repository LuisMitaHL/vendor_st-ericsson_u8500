/*
 * st-ericsson_config.c
 *
 * Copyright (C) ST-Ericsson SA 2012
 * Author:  <david.paris@stericsson.com> for ST-Ericsson.
 * License terms:  GNU General Public License (GPL), version 2
 */
#include <types.h>
#include <soc_settings.h>

__attribute__((section(".type")))
const u32 pasr_type = SECTION_TYPE_CUSTOM_CONFIG;

extern u32 linker_payload_length;

__attribute__((section(".length")))
const u32 payload_length = (u32)&linker_payload_length;

__attribute__((section(".payload_interlv")))
const u32 ddr_interleaver_config = 0xE89;

__attribute__((section(".payload_pasr")))
const u16 PASRSegMaskAtBoot_table[] = {
	0x00, /* Ctrl[0].die[0].bankMask */
	0xFF, /* Ctrl[0].die[0].segMask */
	0x00, /* Ctrl[0].die[1].bankMask */
	0x3F, /* Ctrl[0].die[1].segMask */
	0x00, /* Ctrl[1].die[0].bankMask */
	0xFF, /* Ctrl[1].die[0].segMask */
	0x00, /* Ctrl[1].die[1].bankMask */
	0xBF  /* Ctrl[1].die[1].segMask */
};

__attribute__((section(".payload_lvl0_firewall")))
const u32 Lvl0Firewall_table[] = {
	0xFB000000, /* LVL0_FIREWALL_DDRSECLOW */
	0xFC000000, /* LVL0_FIREWALL_DDRSECHIGH */
	0x3BE00000, /* LVL0_FIREWALL_MODLOW */
	0x40000000, /* LVL0_FIREWALL_MODHIGH */
	0x3C000000, /* LVL0_FIREWALL_SHAREHIGH */
	0xFFF00000, /* LVL0_FIREWALL_DDRMAXSPACE */
	0x00000038, /* LVL0_FIREWALL_C2COFFSETADDR */
};

