/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * RDS parser for the RX FmRadio Inteface.
 * Reference: [1]The RDS standard
 * http://www.rds.org.uk/rds98/pdf/IEC%2062106-E_no%20print.pdf
 *
 * Authors: johan.xj.palmaeus@stericsson.com,
 *          stuart.macdonald@stericsson.com
 *          for ST-Ericsson
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include "ste_fmradio_cg2900_rds_parser.h"
#include <utils/Log.h>

#define FALSE 0
#define TRUE 1

/*
 * The standard says all content must be resent within 2 minutes =
 * 120 seconds. Remove any eons not sent for five times this time,
 * = 600 seconds, too allow for quite severe packet loss.
 */
#define RDS_EON_KEEP_TIME 600

/* Both RDS and RBDS are based on the EU band and its frequency limits  */
#define RDS_FIRST_FREQ 87500
#define RDS_CHANNEL_OFFSET 100
#define RDS_AF_FIRST_CHANNEL 0 /* = 87500 kHz */
#define RDS_AF_LAST_CHANNEL 204 /* = 107900 kHz */
#define RDS_AF_COUNT_OFFSET 224 /* = 0 AF's */
#define RDS_AF_COUNT_MAX 249 /* = 25 AF's */

/* EON 14A packet can map 16 different variants, not all are currently used */
#define RDS_EON_VARIANT_PSN_FIRST 0
#define RDS_EON_VARIANT_PSN_LAST 3
#define RDS_EON_VARIANT_AF 4
#define RDS_EON_VARIANT_MAPPED_FIRST 5
#define RDS_EON_VARIANT_MAPPED_LAST 8
#define RDS_EON_VARIANT_MAPPED_AM 9
#define RDS_EON_VARIANT_LINKAGE 12
#define RDS_EON_VARIANT_PTY_TA 13
#define RDS_EON_VARIANT_PIN 14

/*
 * valid_modified_UTF
 *
 * check if byte string is really valid modified UTF
 *
 * @param [in] bytes - byte string to check
 *
 * @return Returns TRUE if string is valide, FALSE otherwise
 */
static int valid_modified_UTF(char * bytes){
    while (*bytes != '\0') {
        char utf8 = *(bytes++);
        /* Switch on the high four bits. */
        switch (utf8 >> 4) {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
                /* Bit pattern 0xxx. No need for any extra bytes.*/
                break;
            case 0x08:
            case 0x09:
            case 0x0a:
            case 0x0b:
            case 0x0f:
                /*
                 * Bit pattern 10xx or 1111, which are illegal start bytes.
                 * Note: 1111 is valid for normal UTF-8, but not the
                 * modified UTF-8 used here.
                 */
                ALOGI("JNI WARNING: illegal start byte 0x%x\n", utf8);
                goto fail;
                /*
                 * no break, actually block will always goto fail but that is
                 * missed by coverity
                 */
            case 0x0e:
                utf8 = *(bytes++);
                if ((utf8 & 0xc0) != 0x80) {
                    ALOGI("JNI WARNING: illegal continuation byte 0x%x\n",
                         utf8);
                    goto fail;
                }
                /* no break */
            case 0x0c:
            case 0x0d:
                utf8 = *(bytes++);
                if ((utf8 & 0xc0) != 0x80) {
                    ALOGI("JNI WARNING: illegal continuation byte 0x%x\n",
                         utf8);
                    goto fail;
                }
                break;
        }
    }
    return TRUE;
  fail:
    return FALSE;
}

/*
 * fill_rds
 *
 * Copy data to given rds bundle
 *
 * @param [out] bundle - bundle to update
 */

void fill_rds(struct rds_session_t *session_p, struct fmradio_rds_bundle_t *bundle)
{
    memcpy(bundle, &session_p->rds_bundle, sizeof(session_p->rds_bundle));
}

static void update_mapped_freqs(struct rds_mapped_freqs_t ** head,
                                int number,
                                int transmitted_freq,
                                int mapped_freq)
{
    /* search for this transmitted frequency */
    struct rds_mapped_freqs_t **ptr = head;
    struct rds_mapped_freqs_t *new_item = NULL;

    while (*ptr != NULL && (*ptr)->transmitted_freq < transmitted_freq) {
        ptr = &(*ptr)->next;
    }

    if ((*ptr != NULL) &&
        ((*ptr)->transmitted_freq == transmitted_freq)) {
        (*ptr)->mapped_freq[number] = mapped_freq;
    } else {
        new_item = malloc(sizeof(*new_item));
        memset(new_item, 0, sizeof(*new_item));
        new_item->next = *ptr;
        new_item->transmitted_freq = transmitted_freq;
        new_item->mapped_freq[number] = mapped_freq;
        *ptr = new_item;
    }

}

/* Decodes the A block of RDS data */
static void decode_A(struct rds_session_t *session_p, int byte1, int byte0)
{
    unsigned short pi = (byte1 << 8) | byte0;

    if (pi) {
        session_p->rds_bundle.pi = pi;
    }
}

/* Decodes the B block of RDS data */
static void decode_B(struct rds_session_t *session_p, int byte1, int byte0)
{
    int b_bytes = (byte1 << 8) | byte0;

    session_p->blockb_header = b_bytes & 0x1f;
    session_p->rds_bundle.pty = (b_bytes >> 5) & 0x1f;
    session_p->rds_bundle.tp = (b_bytes >> 10) & 0x1;
    session_p->group_type = (b_bytes >> 11) & 0x1;
    session_p->group_number = (b_bytes >> 12) & 0xf;
}

/* Decodes the C block of RDS data */
static void decode_C(struct rds_session_t *session_p, int byte1, int byte0)
{
    /* Data saved for processing in D block. */
    session_p->blockc_byte1 = byte1;
    session_p->blockc_byte0 = byte0;
}

static int get_freq_from_code(int code) {
    if (code >= RDS_AF_FIRST_CHANNEL && code <= RDS_AF_LAST_CHANNEL)
        return RDS_FIRST_FREQ + code * RDS_CHANNEL_OFFSET;
    else
        return -1;
}

/*
 * Decodes the frequency code to frequency and adds it to the AF list
 * currently only AF method A is supported (max 25 stations)
 */

static void decode_af(int code, struct rds_af_build_t *af_build, int *afs, short *num_afs)
{
    if (code >= RDS_AF_FIRST_CHANNEL && code <= RDS_AF_LAST_CHANNEL) {
        /* Read the AF frequencies and store them in the AF frequency list */
        if (af_build->decoded_nr_of_af < RDS_MAX_AFS) {
            af_build->af[af_build->decoded_nr_of_af++] =
                get_freq_from_code(code);
            if (af_build->decoded_nr_of_af == af_build->received_nr_of_af) {
                /* All AFs in list received, copy to official list */
                *num_afs = af_build->decoded_nr_of_af;
                memcpy(afs, af_build->af, sizeof(*afs) * (*num_afs));
            }
        }
    } else if (code >= RDS_AF_COUNT_OFFSET && code <= RDS_AF_COUNT_MAX) {
        /* new list of AF, reset old one */
        af_build->decoded_nr_of_af = 0;
        af_build->received_nr_of_af = code - RDS_AF_COUNT_OFFSET;
    }
}

/* Decodes the D block of 0 type RDS data */
static void decode_group0(struct rds_session_t *session_p, int byte1, int byte0)
{
    /* Check for the type 0A RDS block */
    if (session_p->group_type == 0) {
        decode_af(session_p->blockc_byte1, &session_p->af_build,
                  session_p->rds_bundle.af, &session_p->rds_bundle.num_afs);
        decode_af(session_p->blockc_byte0, &session_p->af_build,
                  session_p->rds_bundle.af, &session_p->rds_bundle.num_afs);
    }
    /* Use the last 2 bits of the block B header to assemble the
     * PSN, once complete, copy this to the RDS struct. */
    session_p->psn_buffer[2 * (session_p->blockb_header & 0x03) + 0] = byte1;
    session_p->psn_buffer[2 * (session_p->blockb_header & 0x03) + 1] = byte0;

    session_p->rds_bundle.ms = (session_p->blockb_header >> 3) & 0x01;
    session_p->rds_bundle.ta = (session_p->blockb_header >> 4) & 0x01;

    if ((session_p->blockb_header & 0x03) == 0x03 && session_p->psn_buffer[0] != '\0') {
        if (valid_modified_UTF(session_p->psn_buffer)) {
            strncpy(session_p->rds_bundle.psn, session_p->psn_buffer,
                    RDS_PSN_MAX_LENGTH);
            session_p->rds_bundle.psn[RDS_PSN_MAX_LENGTH] = '\0';
        }
        memset(session_p->psn_buffer, 0, sizeof(session_p->psn_buffer));
    }
}

/* Decodes the D block of 2 type RDS data */
static void decode_group2(struct rds_session_t *session_p, int byte1, int byte0)
{
    /*
     * The A/B flag is toggled when a text has been sent completely
     */
  int rt_ab = session_p->blockb_header & 0x10;
  int group_type = session_p->group_type;

  /*
   * 2A packets have 4 chars in each packet, 2B just 2. Define a multiplicator
   * to know how many chars each packet contains.
   */
  int group_mul = 4;
  if (group_type == 1)  {
      group_mul = 2;
  }

  /* Check for the type 2A RDS block and use the last 4 bits of the block
  * B header to determine the positioning. */
  int d = 0;
  if (group_type == 0) {
      session_p->rt_buffer[group_mul * (session_p->blockb_header & 0x0f) + d++] = session_p->blockc_byte1;
      session_p->rt_buffer[group_mul * (session_p->blockb_header & 0x0f) + d++] = session_p->blockc_byte0;
  }
  session_p->rt_buffer[group_mul * (session_p->blockb_header & 0x0f) + d++] = byte1;
  session_p->rt_buffer[group_mul * (session_p->blockb_header & 0x0f) + d++] = byte0;

  if (rt_ab != session_p->rt_flag && session_p->rt_flag != -1) {
      if (valid_modified_UTF(session_p->rt_buffer)) {
          strncpy(session_p->rds_bundle.rt, session_p->rt_buffer, RDS_RT_MAX_LENGTH);
          session_p->rds_bundle.rt[RDS_RT_MAX_LENGTH] = '\0';
      }
      memset(session_p->rt_buffer, 0, sizeof(session_p->rt_buffer));
  }
  session_p->rt_flag = rt_ab;
}

/* Decodes the D block of 4 type RDS data */
static void decode_group4(struct rds_session_t *session_p, int byte1, int byte0)
{
    if (session_p->group_type == 0) {
        /* All variables as defined in the RDS standard[1]. See Annex G
        * for detailed instructions regarding the calculations. */
        int MJD, Y, M, D, K, H, N, F;
        /* Date */
        MJD =
            ((session_p->blockb_header & 0x03) << 15) | (session_p->blockc_byte1 << 7) |
            ((session_p->blockc_byte0 >> 1) & 0x7F);
        Y = (int) (((double) MJD - 15078.2) / 365.25);
        M = (int) (((double) MJD - 14956.1 - (int) (Y * 365.25)) / 30.6001);
        D = MJD - 14956 - (int) (Y * 365.25) - (int) (M * 30.6001);
        if ((M == 14) || (M == 15))
            K = 1;
        else
            K = 0;
        Y = Y + K + 1900;
        M = M - 1 - (K * 12);
        /* Time */
        H = ((session_p->blockc_byte0 & 0x01) << 4) | ((byte1 & 0xF0) >> 4);
        N = ((byte1 & 0x0F) << 2) | ((byte0 & 0xC0) >> 6);
        F = byte0 & 0x1F;
        if (F & 0x01) {
            if (byte0 & 0x20) {
                N = N - 30;
                if (N < 0) {
                    N += 60;
                    H++;
                }
            } else {
                N = N + 30;
                if (N > 59) {
                    N -= 60;
                    H--;
                }
            }
        }
        if (byte0 & 0x20)
            F = -F;
        H += F / 2;
        if (H < 0)
            H += 24;
        if (H > 23)
            H -= 24;

        sprintf(session_p->rds_bundle.ct, "%02d:%02d:%02d:%02d:%02d", Y, M, D, H, N);
    }
}

/* Decodes the D block of 10 type RDS data */
static void decode_group10(struct rds_session_t *session_p, int byte1, int byte0)
{
    /* only 10A is supported */
    if (session_p->group_type == 0) {
        int ptyn_ab = session_p->blockb_header & 0x10;


        session_p->ptyn_buffer[4 * (session_p->blockb_header & 0x01) + 0] = session_p->blockc_byte1;
        session_p->ptyn_buffer[4 * (session_p->blockb_header & 0x01) + 1] = session_p->blockc_byte0;

        session_p->ptyn_buffer[4 * (session_p->blockb_header & 0x01) + 2] = byte1;
        session_p->ptyn_buffer[4 * (session_p->blockb_header & 0x01) + 3] = byte0;

        if (ptyn_ab != session_p->ptyn_flag && session_p->ptyn_flag != -1) {
            if (valid_modified_UTF(session_p->ptyn_buffer)) {
                strncpy(session_p->rds_bundle.ptyn, session_p->ptyn_buffer, RDS_PTYN_MAX_LENGTH);
                session_p->rds_bundle.ptyn[RDS_PTYN_MAX_LENGTH] = '\0';
            }
            memset(session_p->ptyn_buffer, 0, sizeof(session_p->ptyn_buffer));
        }
        session_p->ptyn_flag = ptyn_ab;
    }
}

/* Decodes the D block of 14A type RDS data - EON groups */
static void decode_group14A(struct rds_session_t *session_p, int byte1, int byte0)
{
    unsigned short pi_on = (byte1 << 8) | byte0;
    unsigned short variant = session_p->blockb_header & 0x0F;
    struct rds_eon_build_t *eb_p =  session_p->eon_build_p;
    struct rds_eon_build_t *eb_prev = NULL;
    struct rds_eon_t *eon_p = NULL;

    /* check if we already have this in our list */

    while (eb_p != NULL) {
        if (eb_p->eon_p->pi == pi_on) {
            break;
        }
        eb_prev = eb_p;
        eb_p = eb_p->next;
    }

    /* if we didn't find any create a new */
    if (eb_p == NULL) {
        eb_p = malloc(sizeof(*eb_p));
        if (eb_p == NULL) {
            return;
        }
        memset(eb_p, 0, sizeof(*eb_p));

        eb_p->eon_p = malloc(sizeof(*(eb_p->eon_p)));
        if (eb_p == NULL) {
            ALOGE("rds: malloc failed!\n");
            free(eb_p);
            return;
        }
        memset(eb_p->eon_p, 0, sizeof(*(eb_p->eon_p)));

        if (eb_prev != NULL) {
            eb_prev->next = eb_p;
            eb_prev->eon_p->next = eb_p->eon_p;
        } else   {
            /* first in list, update session */
            session_p->eon_build_p = eb_p;
            session_p->eon_list = eb_p->eon_p;
        }
    }

    eon_p = eb_p->eon_p;

    /* now populate */
    eon_p->pi = pi_on;
    eon_p->tp = (session_p->blockb_header >> 4) & 0x001;
    /* select variant, we don't support linkage or PIN */
    if (variant >= RDS_EON_VARIANT_PSN_FIRST &&
        variant <= RDS_EON_VARIANT_PSN_LAST) {
        /* psn */
        eon_p->psn[2 * variant + 0] = session_p->blockc_byte1;
        eon_p->psn[2 * variant + 1] = session_p->blockc_byte0;
    } else if (variant == RDS_EON_VARIANT_AF) {
        decode_af(session_p->blockc_byte1, &eb_p->af_build,
                  eon_p->af, &eon_p->num_afs);
        decode_af(session_p->blockc_byte0, &eb_p->af_build,
                    eon_p->af, &eon_p->num_afs);
    } else if (variant >= RDS_EON_VARIANT_MAPPED_FIRST &&
               variant <= RDS_EON_VARIANT_MAPPED_LAST) {
        // mapped frequency
        update_mapped_freqs(&eon_p->mapped_freqs,
                            variant - 5,
                            get_freq_from_code(session_p->blockc_byte1),
                            get_freq_from_code(session_p->blockc_byte0));

    } else if (variant == RDS_EON_VARIANT_MAPPED_AM) {
        /* chip does not support AM */
        ALOGW("warning - 14A variant %u (mapped AM) not supported\n", RDS_EON_VARIANT_MAPPED_AM);
    } else if (variant == RDS_EON_VARIANT_LINKAGE) {
        // linkage information ignored right now
    } else if (variant == RDS_EON_VARIANT_PTY_TA) {
        eon_p->pty = (session_p->blockc_byte1 >> 3) & 0x1F;
        eon_p->ta = session_p->blockc_byte0 & 0x01;
    } else if (variant == RDS_EON_VARIANT_PIN) {
        // pin ignored for now
    } else {
        ALOGW("decode_14A - warning - unknown variant %d, data %2x%2x\n",
             variant, session_p->blockc_byte1, session_p->blockc_byte0);
    }
    /* mark current eon build with a timestamp to know when it is too old */
    eb_p->timestamp = time(NULL);
}

/* Decodes the D block of 14B type RDS data */
static void decode_group14B(struct rds_session_t *session_p, int byte1, int byte0)
{
    /* 14B mean changed TA of other network and might trigger switch */

    if ((session_p->blockb_header >> 4) & 0x01) {
        unsigned short pi_on = (byte1 << 8) | byte0;
        // requested PPI is in this block
        session_p->ta_switch_requested_to_pi = pi_on;
    } else
        // clearing ta
        session_p->ta_switch_requested_to_pi = 0;
}

/* Decodes the D block of RDS data */
static void decode_D(struct rds_session_t *session_p, int byte1, int byte0)
{
    switch (session_p->group_number) {
    case 0:
        decode_group0(session_p, byte1, byte0);
        break;
    case 2:
        decode_group2(session_p, byte1, byte0);
        break;
    case 4:
        decode_group4(session_p, byte1, byte0);
        break;
    case 10:
        decode_group10(session_p, byte1, byte0);
        break;
    case 14:
        if (session_p->group_type == 0) {
            decode_group14A(session_p, byte1, byte0);
        } else {
            decode_group14B(session_p, byte1, byte0);
        }
        break;
    default:
        break;
    }
}

/* Selects which decode block will parse the current RDS block */
int decode_rds(struct rds_session_t *session_p, int byte2, int byte1, int byte0)
{
    if ((byte2 & 0x80) != 0) {
        /* Bad packet */
        return FALSE;
    }

    int block = byte2 & 0x07;

    switch (block) {
    case 0:
        decode_A(session_p, byte1, byte0);
        break;
    case 1:
        decode_B(session_p, byte1, byte0);
        break;
    case 2:
    case 4:
        /* block = 4 is the RDS C' block, it is handled the same as the
         * block = 2 RDS C block */
        decode_C(session_p, byte1, byte0);
        break;
    case 3:
        decode_D(session_p, byte1, byte0);
        break;
    default:
        break;
    }
    return TRUE;
}

/*
 * clear_rds_data
 *
 * Clears the current aggregated rds_data and frees eon list if existing, to be
 * used after frequency switch or after a shutdown request.
 */
void clear_rds(struct rds_session_t *session_p)
{
    struct rds_eon_t *eon_p = session_p->eon_list;
    struct rds_eon_build_t *eb_p =  session_p->eon_build_p;

    while (eon_p != NULL) {
        struct rds_eon_t *tmp_p = eon_p;
        struct rds_mapped_freqs_t *mf_p = eon_p->mapped_freqs;
        while (mf_p != NULL) {
            struct rds_mapped_freqs_t *tmp_mf_p = mf_p;
            mf_p = mf_p->next;
            free(tmp_mf_p);
        }

        eon_p = eon_p->next;
        free(tmp_p);
    }

    session_p->eon_list = NULL;

    while (eb_p != NULL) {
        struct rds_eon_build_t *tmp_p =  eb_p;
        eb_p = eb_p->next;
        free(tmp_p);
    }

    init_rds(session_p);
}

/*
 * init_rds
 *
 * Initialize different rds parameters. NOTE! Does not free anyting, should NOT
 * be used to clear the struct (use clear_rds)
 */
void init_rds(struct rds_session_t *session_p)
{
    memset(session_p, 0, sizeof(*session_p));
    session_p->rt_flag = -1;
    session_p->ptyn_flag = -1;
}

/*
 * search for a eon group given a pi value. Used in ta switching.
 */
struct rds_eon_t* get_eon_from_pi(struct rds_eon_t *eon_list,
                                  unsigned short pi)
{
    struct rds_eon_t *eon_p = eon_list;
    while (eon_p != NULL) {
        if (eon_p->pi == pi) {
            break;
        }
        eon_p = eon_p->next;
    }
    return eon_p;
}

/*
 * cleanup_old_eon_entries
 *
 * this function is used to remove too old entries in eon list, the only way
 * to detect entries no longer available is by detect when their last
 * transmitted time is more than RDS_EON_KEEP_TIME old.
 */
void cleanup_old_eon_entries(struct rds_session_t *session_p)
{
    struct rds_eon_build_t *eb_p = session_p->eon_build_p;
    struct rds_eon_build_t *eb_prev_p = NULL;

    while (eb_p != NULL) {
        struct rds_eon_build_t *eb_next_p = eb_p->next;
        if (eb_p->timestamp + RDS_EON_KEEP_TIME < time(NULL)) {

            /* re point the elements pointing to these elements */
            if (eb_prev_p != NULL) {
                eb_prev_p->eon_p->next = eb_p->eon_p->next;
                eb_prev_p->next = eb_p->next;
            } else {
                session_p->eon_list = eb_p->eon_p->next;
                session_p->eon_build_p = eb_p->next;
            }
            free(eb_p->eon_p);
            free(eb_p);
        } else
            eb_prev_p = eb_p; /* only update this if current element was not deleted */
        eb_p = eb_next_p;
    }

}
