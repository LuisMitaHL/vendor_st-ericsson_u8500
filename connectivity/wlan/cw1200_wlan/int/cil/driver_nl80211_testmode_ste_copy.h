/*
 * ST-Ericsson hostap/wpa_supplicant changes and additions.
 *
 * Copyright (C) ST-Ericsson AB 2010
 *
 */

#ifndef _DRIVER_NL80211_TESTMODE_STE_H_
#define _DRIVER_NL80211_TESTMODE_STE_H_

/**
 * nl80211 testmode data additions
 */

enum ste_nl80211_testmode_data_attributes {
	STE_TM_MSG_ID = 0x0001,  /* u32 type containing the STE message ID */
	STE_TM_MSG_DATA,         /* message payload */

	/* Add new attributes here*/

	STE_TM_MSG_ATTR_MAX,
	/* Max indicator so module test may add its own attributes*/
};

#define STE_TM_MAX_ATTRIB_SIZE 1024

#endif /* _DRIVER_NL80211_TESTMODE_STE_H_ */

