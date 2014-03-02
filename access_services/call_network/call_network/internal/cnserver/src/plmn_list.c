/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "cn_log.h"
#include "cn_macros.h"
#include "plmn_list.h"

/* Constants */

/* Definitions */
typedef struct {
    int value;
    bool wc_match;
} plmn_list_compare_t;


/* PLMN list anchor */
static plmn_list_entry_t *plmn_list_head_p = NULL;

/* Local function prototypes */
static plmn_list_compare_t plmn_list_compare_entry(const plmn_list_entry_t *list_entry_p,
        const plmn_match_mode_t mode,
        const plmn_list_source_t source,
        const uint16_t mcc, const uint16_t mnc,
        const uint16_t lac, const uint16_t lac2);

static uint16_t plmn_list_get_wildcard_mask(uint16_t value);


/**
 * Return list head
 */
plmn_list_entry_t *plmn_list_get_head()
{
    return plmn_list_head_p;
}


/**
 * Add or update PLMN list entry
 * Add new entry in sorted order according to value of country-code, network-code, lac1, lac2 & source.
 * Avoid duplicates by updating existing entries.
 *
 * Note: The wild-card entries need to be sorted in at the lower end. This is why the
 * compare function is invoked with the MCC and MNC values with the wild-cards masked out.
 *
 * Note: The entries without a LAC range (LAC1=0x0, LAC2=0xFFE) need to be sorted in at the
 * end of the range so they are seen last.
 */
int plmn_list_add_entry(const plmn_list_source_t source, const uint16_t mcc,
                        const uint16_t mnc, const uint16_t lac1, const uint16_t lac2,
                        const char *spn_p, const char *mnn_p)
{
    plmn_list_entry_t *prev_entry_p;
    plmn_list_entry_t *new_entry_p;
    plmn_list_entry_t *curr_entry_p;
    uint16_t mcc_wc_mask, mnc_wc_mask;
    size_t spn_length, mnn_length;
    plmn_list_compare_t compare;
    int added = 0;


    /* Set MCC wildcard mask */
    mcc_wc_mask = plmn_list_get_wildcard_mask(mcc);

    /* Set MNC wildcard mask */
    mnc_wc_mask = plmn_list_get_wildcard_mask(mnc);

    CN_LOG_V("source=%d, mcc=0x%03X(0x%X), mnc=0x%02X(0x%X), lac=0x%X-0x%X, spn=\"%s\", mnn=\"%s\"",
             source, mcc, mcc_wc_mask, mnc, mnc_wc_mask, lac1, lac2, spn_p, mnn_p);

    /* Get string lengths */
    if (spn_p) {
        spn_length = strlen(spn_p);
    } else {
        spn_length = 0;
    }

    if (mnn_p) {
        mnn_length = strlen(mnn_p);
    } else {
        mnn_length = 0;
    }

    /* Find suitable location to insert entry in */
    for (new_entry_p = prev_entry_p = NULL, curr_entry_p = plmn_list_head_p;
            new_entry_p == NULL;
            prev_entry_p = curr_entry_p, curr_entry_p = curr_entry_p->next_p) {

        if (curr_entry_p) {
            compare = plmn_list_compare_entry(curr_entry_p, PLMN_MATCH_EXACT, source,
                                              (mcc & mcc_wc_mask), (mnc & mnc_wc_mask), lac1, lac2);

        } else {
            /* End of list reached, charge the dice */
            compare.value = 1;
        }

        if (compare.value < 0) {
            /* Entry has lower comparison value. Keep scanning. */
            continue;

        } else if (compare.value > 0) {
            /* First non-matching entry with higher comparison value found.
             * Create new entry, populate it and chain it in before this entry or at list-head. */
            new_entry_p = calloc(1, sizeof(plmn_list_entry_t) + spn_length + mnn_length + 2);

            if (!new_entry_p) {
                CN_LOG_E("Memory allocation failure!");
                added = -1;
                goto exit;
            }

            new_entry_p->source = source;
            new_entry_p->mcc = mcc;
            new_entry_p->mcc_wc_mask = mcc_wc_mask;
            new_entry_p->mnc = mnc;
            new_entry_p->mnc_wc_mask = mnc_wc_mask;
            new_entry_p->lac1 = lac1;
            new_entry_p->lac2 = lac2;
            new_entry_p->mnn_offset = spn_length + 1;

            if (spn_p) {
                strncpy(&new_entry_p->names[0], spn_p, spn_length + 1);
            }

            if (mnn_p) {
                strncpy(&new_entry_p->names[spn_length+1], mnn_p, mnn_length + 1);
            }

            if (NULL == prev_entry_p) {

                /* Chain in at list head */
                new_entry_p->next_p = curr_entry_p;
                plmn_list_head_p = new_entry_p;

            } else {

                /* Chain in before curr_entry_p */
                new_entry_p->next_p = curr_entry_p;
                prev_entry_p->next_p = new_entry_p;

            }

            /* Count added entries */
            added++;

            break;

        } else {
            CN_LOG_E("Duplicate source=%d, mcc=0x%03X(0x%X), mnc=0x%02X(0x%X), lac=0x%X-0x%X, spn=\"%s\", mnn=\"%s\"",
                     curr_entry_p->source,
                     curr_entry_p->mcc, curr_entry_p->mcc_wc_mask,
                     curr_entry_p->mnc, curr_entry_p->mnc_wc_mask,
                     curr_entry_p->lac1, curr_entry_p->lac2,
                     &curr_entry_p->names[0],
                     &curr_entry_p->names[curr_entry_p->mnn_offset]);

            /* Must reallocate entry in case string lengths have increased */
            new_entry_p = realloc(curr_entry_p, sizeof(plmn_list_entry_t) + spn_length + mnn_length + 2);

            if (!new_entry_p) {
                CN_LOG_E("Memory allocation failure!");
                added = -1;
                goto exit;
            }

            /* Update entry */
            new_entry_p->mnn_offset = spn_length + 1;

            if (spn_p) {
                strncpy(&new_entry_p->names[0], spn_p, spn_length + 1);
            } else {
                new_entry_p->names[0] = '\0';
            }

            if (mnn_p) {
                strncpy(&new_entry_p->names[spn_length+1], mnn_p, mnn_length + 1);
            } else {
                new_entry_p->names[spn_length+1] = '\0';
            }

            /* Update pointer on previous entry, or list head */
            if (NULL == prev_entry_p) {
                plmn_list_head_p = new_entry_p;
            } else {
                prev_entry_p->next_p = new_entry_p;
            }

            /* Only count added entries, not updated */
            break;
        }

    }

exit:
    return added;
}


int plmn_list_delete_entry(const plmn_list_source_t source,
                           const uint16_t mcc, const uint16_t mnc,
                           const uint16_t lac1, const uint16_t lac2)
{
    plmn_list_entry_t *prev_entry_p;
    plmn_list_entry_t *curr_entry_p;
    uint16_t mcc_wc_mask, mnc_wc_mask;
    plmn_list_compare_t compare;
    int deleted = 0;


    /* Set MCC wildcard mask */
    mcc_wc_mask = plmn_list_get_wildcard_mask(mcc);

    /* Set MNC wildcard mask */
    mnc_wc_mask = plmn_list_get_wildcard_mask(mnc);

    CN_LOG_D("source=%d, mcc=0x%03X(0x%X), mnc=0x%02X(0x%X), lac=0x%X-0x%X",
             source, mcc, mcc_wc_mask, mnc, mnc_wc_mask, lac1, lac2);

    for (prev_entry_p = NULL, curr_entry_p = plmn_list_head_p;
            curr_entry_p != NULL;
            prev_entry_p = curr_entry_p, curr_entry_p = curr_entry_p->next_p) {

        compare = plmn_list_compare_entry(curr_entry_p, PLMN_MATCH_EXACT, source,
                                          (mcc & mcc_wc_mask), (mnc & mnc_wc_mask), lac1, lac2);

        if (compare.value < 0) {
            continue;

        } else if (compare.value > 0) {
            /* Not found */
            break;

        } else {
            /* Matching entry, unchain and free */
            if (prev_entry_p == NULL) {
                /* List head */
                plmn_list_head_p = curr_entry_p->next_p;
            } else {
                /* Not list head */
                prev_entry_p->next_p = curr_entry_p->next_p;
            }

            free(curr_entry_p);

            /* Count deleted entries */
            deleted++;

            break;
        }

    }

    return deleted;
}


int plmn_list_find_operator(const plmn_list_source_t source,
                            const uint16_t mcc, const uint16_t mnc, const uint16_t lac,
                            const char **spn_pp, const char **mnn_pp)
{
    plmn_list_entry_t *curr_entry_p;
    int lac_range = 0x1 << PLMN_LAC1;
    bool wc_match = false;
    plmn_list_compare_t compare;
    int matches = 0;


    CN_LOG_D("source=%d, mcc=0x%03X, mnc=0x%02X, lac=0x%X",
             source, mcc, mnc, lac);

    if (PLMN_ANY_SOURCE != source) {
        lac_range += 0x1 << PLMN_SOURCE;
    }

    for (curr_entry_p = plmn_list_head_p;
            curr_entry_p != NULL;
            curr_entry_p = curr_entry_p->next_p) {

        compare = plmn_list_compare_entry(curr_entry_p, PLMN_MATCH_ANY, source, mcc, mnc, lac, 0 /* No LAC range */);

        if (compare.value < 0) {
            continue;

        } else if (compare.value > 0) {

            if ((compare.value - lac_range) > 0) {
                /* Nothing found. All the entries in the list beyond this point have
                 * a higher value than what we are looking for. Stop searching */
                break;
            }

            /* Continue search within same MCC+MNC, even if the LAC range is exceeded.
             * Entries with a matching LAC range may still exist further down the list. */
            continue;

        } else {
            /* Matching entry found, if first found, or if previously found
             * entry was a wild-card match - then copy the name pointers. */
            if (0 == matches || wc_match) {
                *spn_pp = &curr_entry_p->names[0];
                *mnn_pp = &curr_entry_p->names[curr_entry_p->mnn_offset];

                /* Track if this was a wild-card match or not */
                wc_match = compare.wc_match;
            }

            /* Count number of matches */
            matches++;

            /* Keep on scanning to count all matching entries. */
            continue;
        }
    }

    return matches;
}


int plmn_list_find_operator_by_name(const plmn_list_source_t source,
                                    const plmn_list_columns_t column,
                                    const char *name_p,
                                    uint16_t *mcc_p, uint16_t *mnc_p)
{
    plmn_list_entry_t *curr_entry_p;
    int matches = 0;
    int compare;


    CN_LOG_D("source=%d, column=%d, name_p=\"%s\"",
             source, column, name_p);

    for (curr_entry_p = plmn_list_head_p;
            curr_entry_p != NULL;
            curr_entry_p = curr_entry_p->next_p) {

        if (PLMN_ANY_SOURCE != source && curr_entry_p->source != source) {
            continue;
        }

        /* Wildcard entries are not real networks */
        if (curr_entry_p->mcc_wc_mask != 0xFFF ||
                curr_entry_p->mnc_wc_mask != 0xFFF) {
            continue;
        }

        switch (column) {
        case PLMN_MNN:

            if (0 != curr_entry_p->mnn_offset) {
                compare = strcmp(name_p, &curr_entry_p->names[curr_entry_p->mnn_offset]);
            } else {
                compare = -1;
            }

            break;
        case PLMN_SPN:
            compare = strcmp(name_p, &curr_entry_p->names[0]);
            break;
        default:
            compare = -1;
            break;
        }

        /* Exit on first match */
        if (0 == compare) {
            *mcc_p = curr_entry_p->mcc;
            *mnc_p = curr_entry_p->mnc;
            matches++;
            break;
        }
    }

    return matches;
}


/**
 * Delete entries from PLMN operator list
 * Delete all entries, or all from a specific source.
 */
int plmn_list_empty(const plmn_list_source_t source)
{
    plmn_list_entry_t *prev_entry_p;
    plmn_list_entry_t *curr_entry_p;
    plmn_list_entry_t *next_entry_p;
    int deleted = 0;


    /* Walk through list and free each matching entry to the end */
    for (prev_entry_p = NULL, curr_entry_p = plmn_list_head_p;
            curr_entry_p != NULL;
            curr_entry_p = next_entry_p) {

        /* Need to copy pointer to next before deleting current */
        next_entry_p = curr_entry_p->next_p;

        if (PLMN_ANY_SOURCE != source && curr_entry_p->source != source) {
            prev_entry_p = curr_entry_p;
            continue;
        }

        /* Matching entry, unchain and free */
        if (prev_entry_p == NULL) {
            /* List head */
            plmn_list_head_p = next_entry_p;
        } else {
            /* Not list head */
            prev_entry_p->next_p = next_entry_p;
        }

        free(curr_entry_p);

        /* Count deleted entries */
        deleted++;
    }

    return deleted;
}


plmn_list_compare_t plmn_list_compare_entry(const plmn_list_entry_t *list_entry_p,
        const plmn_match_mode_t mode,
        const plmn_list_source_t source,
        const uint16_t mcc, const uint16_t mnc,
        const uint16_t lac, const uint16_t lac2)
{
    plmn_list_compare_t compare = { .value = 0, .wc_match = false };


    if (source != PLMN_ANY_SOURCE) {
        if (list_entry_p->source < source) {
            compare.value -= 0x1 << PLMN_SOURCE;
        } else if (list_entry_p->source > source) {
            compare.value += 0x1 << PLMN_SOURCE;
        }
    }

    /* Compare: Location Area Code */
    if ((PLMN_MATCH_EXACT == mode) || lac2) {
        /* Test LAC ranges */
        if (list_entry_p->lac1 < lac) {
            compare.value -= 0x1 << PLMN_LAC1;
        } else if (list_entry_p->lac1 > lac) {
            compare.value += 0x1 << PLMN_LAC1;
        }

        if (list_entry_p->lac2 < lac2) {
            compare.value -= 0x1 << PLMN_LAC2;
        } else if (list_entry_p->lac2 > lac2) {
            compare.value += 0x1 << PLMN_LAC2;
        }
    } else {
        /* LAC2=0 compare individual LAC */
        /* Test if individual LAC is within LAC1-LAC2 range */
        if (list_entry_p->lac2 < lac) {
            compare.value -= 0x1 << PLMN_LAC1;
        } else if (list_entry_p->lac1 > lac) {
            compare.value += 0x1 << PLMN_LAC1;
        }
    }

    /* Compare: Mobile Network Code */
    if (PLMN_MATCH_EXACT == mode) {
        if ((list_entry_p->mnc & list_entry_p->mnc_wc_mask) < mnc) {
            compare.value -= 0x1 << PLMN_MNC;
        } else if ((list_entry_p->mnc & list_entry_p->mnc_wc_mask) > mnc) {
            compare.value += 0x1 << PLMN_MNC;
        }
    } else {
        if ((list_entry_p->mnc & list_entry_p->mnc_wc_mask) < mnc) {
            compare.value -= 0x1 << PLMN_MNC;
        } else if (((list_entry_p->mnc & list_entry_p->mnc_wc_mask) | (~list_entry_p->mnc_wc_mask & 0x999)) > mnc) {
            compare.value += 0x1 << PLMN_MNC;
        }

        if (list_entry_p->mnc_wc_mask != 0xFFF) {
            compare.wc_match = true;
        }
    }

    /* Compare: Mobile Country Code */
    if (PLMN_MATCH_EXACT == mode) {
        if ((list_entry_p->mcc & list_entry_p->mcc_wc_mask) < mcc) {
            compare.value -= 0x1 << PLMN_MCC;
        } else if ((list_entry_p->mcc & list_entry_p->mcc_wc_mask) > mcc) {
            compare.value += 0x1 << PLMN_MCC;
        }
    } else {
        if ((list_entry_p->mcc & list_entry_p->mcc_wc_mask) < mcc) {
            compare.value -= 0x1 << PLMN_MCC;
        } else if (((list_entry_p->mcc & list_entry_p->mcc_wc_mask) | (~list_entry_p->mcc_wc_mask & 0x999)) > mcc) {
            compare.value += 0x1 << PLMN_MCC;
        }

        if (list_entry_p->mcc_wc_mask != 0xFFF) {
            compare.wc_match = true;
        }
    }

    return compare;
}


uint16_t plmn_list_get_wildcard_mask(uint16_t value)
{
    uint16_t mask = 0x0;


    if ((value & 0xF) != 0xD) {
        mask |= 0xF;
    }

    if ((value & 0xF0) != 0xD0) {
        mask |= 0xF0;
    }

    if ((value & 0xF00) != 0xD00) {
        mask |= 0xF00;
    }

    return mask;
}

