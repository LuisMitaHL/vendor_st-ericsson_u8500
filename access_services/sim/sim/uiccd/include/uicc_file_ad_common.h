/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef UICC_FILE_AD_COMMON_H_
#define UICC_FILE_AD_COMMON_H_

#include "catd_modem_types.h"

#define EF_AD_DATA_MAX_LENGTH  4
#define EF_IMSI_DATA_MAX_LENGTH  9

#define EF_AD_MNC_LENGTH_DATA_POS 4      //Fourth byte in data

/* Control data for Administrative file transactions */
typedef enum {
    UICCD_TRANS_AD_INIT,
    UICCD_TRANS_AD_READ_AD_DATA,
    UICCD_TRANS_AD_READ_IMSI_DATA,
    UICCD_TRANS_AD_INVALID
} uiccd_trans_ad_state_t;

typedef struct {
    uiccd_trans_ad_state_t        trans_state;
    int                           ef_ad_read_offset;
    uint8_t                       mnc_length;
    uint8_t                       ef_ad_data[EF_AD_DATA_MAX_LENGTH+1];
    size_t                        ef_ad_data_len;
    uint16_t                      mcc;
    uint16_t                      mnc;
} uiccd_trans_data_ad_t;

const char *plmn_static_operator_list_p = "/system/etc/plmn.latam.list";

/* Note: the order of the columns influence in what order the linked list
 * is sorted. Higher value means higher importance. This applies to numeric
 * fields only. Not Strings.
 */
typedef enum {
    PLMN_LIST_COLUMN_MCC,               /* Mobile Country Code (mandatory) */
    PLMN_LIST_COLUMN_MNC,                           /* Mobile Network Code (mandatory) */
    PLMN_LIST_COLUMN_UNKNOWN,
} plmn_list_column_t;

/* Constants */
const char *str_header_p = "header:";
const char *str_mcc_p    = "mcc";
const char *str_mnc_p    = "mnc";

static const int mandatory_mask = 0x1 << PLMN_LIST_COLUMN_MCC |
                                  0x1 << PLMN_LIST_COLUMN_MNC;



#define PLMN_MCC_MIN                    (0x001)
#define PLMN_MCC_MAX                    (0xFFE)

#define PLMN_MNC_MIN                    (0x000)
#define PLMN_MNC_MAX                    (0xFFE)

#define PLMN_LIST_MNC_2_MAX_STR_LEN     (6)
#define PLMN_LIST_MAX_LINE_LEN          (256)

#endif /* UICC_FILE_AD_COMMON_H_ */
