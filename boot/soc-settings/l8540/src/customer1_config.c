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
const u32 ddr_interleaver_config = 0x689;

__attribute__((section(".payload_pasr")))
const u16 PASRSegMaskAtBoot_table[] = {
	0x00, /* Ctrl[0].die[0].bankMask */
	0x3F, /* Ctrl[0].die[0].segMask */
	0x00, /* Ctrl[0].die[1].bankMask */
	0x00, /* Ctrl[0].die[1].segMask */
	0x00, /* Ctrl[1].die[0].bankMask */
	0xBF, /* Ctrl[1].die[0].segMask */
	0x00, /* Ctrl[1].die[1].bankMask */
	0x00 /* Ctrl[1].die[1].segMask */
};

__attribute__((section(".payload_lvl0_firewall")))
const u32 Lvl0Firewall_table[] = {
	0xFFFF0000, /* LVL0_FIREWALL_MOD */
	0xFFFFFFFF, /* LVL0_FIREWALL_A9_ONLY*/
	0xFFFFFFFF, /* LVL0_FIREWALL_MAXSPACE LVL0_FIREWALL_SHAREHIGH */
};

