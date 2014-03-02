/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __plmn_list_h__
#define __plmn_list_h__ (1)

#include <stdint.h>
#include <stdbool.h>

/* Constants */
#define PLMN_MCC_MIN                    (0x001)
#define PLMN_MCC_MAX                    (0xFFE)

#define PLMN_MNC_MIN                    (0x000)
#define PLMN_MNC_MAX                    (0xFFE)

#define PLMN_LAC_MIN                    (0x0000)
#define PLMN_LAC_MAX                    (0xFFFE)

/* Structures */
typedef enum {
    PLMN_MATCH_ANY,                     /* Match individual, LAC range or MCC/MNC wild-cards */
    PLMN_MATCH_RANGE,                   /* Match individual, or LAC range, no MCC/MNC wild-cards */
    PLMN_MATCH_EXACT,                   /* Exact match, no LAC range or MCC/MNC wild-cards */
} plmn_match_mode_t;

typedef enum {
    PLMN_ANY_SOURCE,
    PLMN_SIM_FILE,                      /* Retrieved from SIM storage */
    PLMN_NITZ_NAME,                     /* Received OTA with NITZ message */
    PLMN_FILE,                          /* Information retrieved from file */
} plmn_list_source_t;

/* Note: the order of the columns influence in what order the linked list
 * is sorted. Higher value means higher importance. This applies to numeric
 * fields only. Not Strings.
 */
typedef enum {
    PLMN_SOURCE,                        /* See plmn_list_source_t */

    PLMN_MNN,                           /* Mobile Network Name (mandatory) */
    PLMN_SPN,                           /* Service Provider Name (mandatory) */
    PLMN_LAC1,                          /* Location Area Code 1 (optional) */
    PLMN_LAC2,                          /* Location Area Code 2 (optional) */
    PLMN_MNC,                           /* Mobile Network Code (mandatory) */
    PLMN_MCC,                           /* Mobile Country Code (mandatory) */

    PLMN_ADD_ENTRY,                     /* Special: */
    PLMN_UNKNOWN,
} plmn_list_columns_t;

typedef struct plmn_list_entry_s {
    plmn_list_source_t source;          /* Source of entry */
    uint16_t    mcc;                    /* Mobile Country Code (MCC) */
    uint16_t    mcc_wc_mask;            /* MCC wildcard mask */
    uint16_t    mnc;                    /* Mobile Network Code (MNC) */
    uint16_t    mnc_wc_mask;            /* MNC wildcard mask */
    uint16_t    lac1;                   /* Location Area Code 1 (LAC1) */
    uint16_t    lac2;                   /* Location Area Code 2 (LAC2) */
    size_t      mnn_offset;             /* Mobile Network Name (MNN) names array offset */
    struct plmn_list_entry_s *next_p;   /* Link to next entry in chain */
    char        names[1];               /* Dynamically sized compound string array
                                         * holding both SPN and MNN strings, plus a
                                         * terminating '\0' character for each. */
} plmn_list_entry_t;

/* Function prototypes */
plmn_list_entry_t *plmn_list_get_head(void);

int plmn_list_add_entry(const plmn_list_source_t source,
                        const uint16_t mcc, const uint16_t mnc,
                        const uint16_t lac1, const uint16_t lac2,
                        const char *spn_p, const char *mnn_p);

int plmn_list_delete_entry(const plmn_list_source_t source,
                           const uint16_t mcc, const uint16_t mnc,
                           const uint16_t lac1, const uint16_t lac2);

int plmn_list_find_operator(const plmn_list_source_t source,
                            const uint16_t mcc, const uint16_t mnc,
                            const uint16_t lac,
                            const char **spn_pp, const char **mnn_pp);

int plmn_list_find_operator_by_name(const plmn_list_source_t source,
                                    const plmn_list_columns_t column,
                                    const char *name_p,
                                    uint16_t *mcc_p, uint16_t *mnc_p);

int plmn_list_empty(const plmn_list_source_t source);

#endif /* __plmn_list_h__ */
