/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * RDS parser for the RX FmRadio Interface.
 *
 * Author: stuart.macdonald@stericsson.com for ST-Ericsson
 */

#ifndef _RDS_PARSER_H_
#define _RDS_PARSER_H_

#include "fmradio.h"

/* internal struct to build up the AF lists */
struct rds_af_build_t {
    int received_nr_of_af;
    int decoded_nr_of_af;
    int af[RDS_MAX_AFS];
};

struct rds_mapped_freqs_t {
    int transmitted_freq;
    int mapped_freq[4]; /* variant 5-8 */
    struct rds_mapped_freqs_t *next;
};

/* one EON entry */
struct rds_eon_t {
    unsigned short pi;
    short pty;
    short ta;
    short tp;
    short num_afs;
    int af[RDS_MAX_AFS];
    char psn[RDS_PSN_MAX_LENGTH + 1];
    struct rds_mapped_freqs_t *mapped_freqs;
    struct rds_eon_t *next;
};

/* internal struct to build up the eons */
struct rds_eon_build_t {
    struct rds_eon_t *eon_p; /* link to eon in bundle list */
    struct rds_af_build_t af_build;
    time_t timestamp;
    struct rds_eon_build_t *next;
};

struct rds_session_t {
    struct fmradio_rds_bundle_t rds_bundle;
    struct rds_af_build_t af_build;
    struct rds_eon_build_t *eon_build_p;
    struct rds_eon_t *eon_list;
    /* 0 - A type group, 1 - B type group */
    int group_type;
    /* Block number 0- 15 */
    int group_number;
    /* 5 bits from the start of the B block */
    int blockb_header;
    /* Used for saving the C block for processing in the D block decode.*/
    int blockc_byte0;
    int blockc_byte1;
    /* Buffers used until the data is complete.*/
    char psn_buffer[RDS_PSN_MAX_LENGTH];
    char rt_buffer[RDS_RT_MAX_LENGTH];
    char ptyn_buffer[RDS_PTYN_MAX_LENGTH];
    short rt_flag;
    short ptyn_flag;
    unsigned short ta_switch_requested_to_pi;
};

/* Parses the RDS block filling out the RDS struct */
int decode_rds(struct rds_session_t *session_p, int byte2, int byte1, int byte0);

/* Initializes the RDS struct */
void init_rds(struct rds_session_t *session_p);

/* reset the RDS struct */
void clear_rds(struct rds_session_t *session_p);

/* Fills the RDS bundle with the current values of the RDS struct */

void fill_rds(struct rds_session_t *session_p, struct fmradio_rds_bundle_t *rds);

/*
 * search for a eon group given a pi value. Used in ta switching.
 */
struct rds_eon_t* get_eon_from_pi(struct rds_eon_t *eon_list,
                                  unsigned short pi);

/*
 * cleanup_old_eon_entries
 *
 * this function is used to remove too old entries in eon list, the only way
 * to detect entries no longer available is by detect when their last
 * transmitted time is more than RDS_EON_KEEP_TIME old.
 */
void cleanup_old_eon_entries(struct rds_session_t *session_p);

#endif
