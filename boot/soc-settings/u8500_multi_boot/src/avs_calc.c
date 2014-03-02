/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Jens Wiklander <jens.wiklander@stericsson.com>
 *  for ST-Ericsson.
 * Jonas Aaberg <jonas.aberg@stericsson.com>
 *  for ST-Ericsson.
 */

#include <log.h>
#include <types.h>
#include <config.h>

#define VERSION_STR_8420 "@(#)SOC-SETTINGS v1.0.2 AVS db8420"
#define VERSION_STR_8500 "@(#)SOC-SETTINGS v4.0.5-(B) AVS db8500"
#define VERSION_STR_8500_BOOST "@(#)SOC-SETTINGS v1.0.0 AVS db8500 BOOST"
#define VERSION_STR_8520 "@(#)SOC-SETTINGS v1.0.2 AVS db8520"

extern void avs8500_calculate_settings(u32 avs_fuses[2], void *avs_data);
extern void avs8500_boost_calculate_settings(u32 avs_fuses[2], void *avs_data);
extern void avs8420_calculate_settings(u32 avs_fuses[2], void *avs_data);
extern void avs8520_calculate_settings(u32 avs_fuses[2], void *avs_data);

extern void avs8500_no_avs_calculate_settings(u32 avs_fuses[2], void *avs_data);
extern void avs8500_boost_no_avs_calculate_settings(u32 avs_fuses[2], void *avs_data);
extern void avs8420_no_avs_calculate_settings(u32 avs_fuses[2], void *avs_data);
extern void avs8520_no_avs_calculate_settings(u32 avs_fuses[2], void *avs_data);

extern int avs8500_is_maxopp_fused(u32 avs_fuses[2]);
extern int avs8500_boost_is_maxopp_fused(u32 avs_fuses[2]);
extern int avs8420_is_maxopp_fused(u32 avs_fuses[2]);
extern int avs8520_is_maxopp_fused(u32 avs_fuses[2]);

int has_avs(void)
{
	struct config *config = get_config();
	return config->avs_enable == true;
}

void avs_calculate_settings(u32 avs_fuses[2], void *avs_data)
{
	struct config *config = get_config();

	switch(config->avs_type) {
	case AVS_TYPE_8500:
		avs8500_calculate_settings(avs_fuses, avs_data);
		break;
	case AVS_TYPE_8500_BOOST:
		avs8500_boost_calculate_settings(avs_fuses, avs_data);
		break;
	case AVS_TYPE_8420:
		avs8420_calculate_settings(avs_fuses, avs_data);
		break;
	case AVS_TYPE_8520:
		avs8520_calculate_settings(avs_fuses, avs_data);
		break;
	case AVS_TYPE_NONE:
		break;
	default:
		logwarn("Unknown avs type.");
		return;
	}
}

void no_avs_calculate_settings(u32 avs_fuses[2], void *avs_data)
{
	struct config *config = get_config();

	switch(config->avs_type) {
	case AVS_TYPE_8500:
		avs8500_no_avs_calculate_settings(avs_fuses, avs_data);
		break;
	case AVS_TYPE_8500_BOOST:
		avs8500_boost_no_avs_calculate_settings(avs_fuses, avs_data);
		break;
	case AVS_TYPE_8420:
		avs8420_no_avs_calculate_settings(avs_fuses, avs_data);
		break;
	case AVS_TYPE_8520:
		avs8520_no_avs_calculate_settings(avs_fuses, avs_data);
		break;
	case AVS_TYPE_NONE:
	default:
		logwarn("Unknown avs type.");
		return;
	}
}

int is_maxopp_fused(u32 avs_fuses[2])
{
	struct config *config = get_config();

	switch(config->avs_type) {
	case AVS_TYPE_8500:
		return avs8500_is_maxopp_fused(avs_fuses);
	case AVS_TYPE_8500_BOOST:
		return avs8500_boost_is_maxopp_fused(avs_fuses);
	case AVS_TYPE_8420:
		return avs8420_is_maxopp_fused(avs_fuses);
	case AVS_TYPE_8520:
		return avs8520_is_maxopp_fused(avs_fuses);
	case AVS_TYPE_NONE:
	default:
		return 0 ;
	}
}

int is_fused(u32 avs_fuses[2])
{
	int unfused = ((avs_fuses[0] == 0) || (avs_fuses[0] == 0xFFFFFF)) && (avs_fuses[1] == 0);
	return unfused == 0;
}

void avs_init(char* build_date, u32 bin, u32 bin_len)
{
	struct config *config = get_config();
	char *version_str;

	switch(config->avs_type) {
	case AVS_TYPE_8500:
		version_str = VERSION_STR_8500;
		break;
	case AVS_TYPE_8500_BOOST:
		version_str = VERSION_STR_8500_BOOST;
		break;
	case AVS_TYPE_8420:
		version_str = VERSION_STR_8420;
		break;
	case AVS_TYPE_8520:
		version_str = VERSION_STR_8520;
		break;
	case AVS_TYPE_NONE:
	default:
		return;
	}
	loginfo("%s %uMhz %s size %u @ %x",
		version_str, config->overclock, build_date,
		bin_len, bin);
}
