/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * cg2900 vendor specific part of the RX FmRadio inteface.
 *
 * Authors: johan.xj.palmaeus@stericsson.com,
 *          stuart.macdonald@stericsson.com
 *          for ST-Ericsson
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <linux/videodev2.h>

#include "fmradio.h"
#include "ste_fmradio_cg2900_rds_parser.h"

#define LOG_TAG "STE_FM_Radio_CG2900"
#define LOG_NDEBUG 1
#include <utils/Log.h>

/* video4linux use 62.5 multiples of frequency */
#define V4L2_FREQ_FACTOR 62.5
#define RDS_MAX_WAIT_TIME_S 5
#ifndef RDS_BLOCKS_TO_READ
#define RDS_BLOCKS_TO_READ 22
#endif
#define RDS_BLOCKS_PER_GROUP 4
#define RDS_BLOCK_SIZE 3

/* how many seconds between eon clenanups */
#define RDS_EON_CLEANUP_INTERVAL 10

/* maximum number of frequencies in block scan chunk */
#define BLOCK_SCAN_MAX_CHUNK_SIZE 198

/* only report changed signal strength if going up or down >= 10% */
#define SIGSTRENGTH_REPORT_THRESHOLD 0.1

#define FREQ_EU_LO      87500
#define FREQ_EU_HI      108000
#define FREQ_US_LO      87900
#define FREQ_US_HI      107900
#define FREQ_JP_LO      76000
#define FREQ_JP_HI      90000
#define FREQ_CH_LO      70000
#define FREQ_CH_HI      108000

/* messages used in pipe communication between poll thread and search threads */
#define POLL_MSG_HUP   1
#define POLL_MSG_ERR   2
#define POLL_MSG_EVENT 3
#define POLL_MSG_EMPTY 4

/* message in poll return pipe */
#define POLL_RET_ACK   1
#define POLL_RET_REQ   2


#define STE_CG2900_BAND_PARAMETER "/sys/module/radio_cg2900/parameters/band"
#define STE_CG2900_GRID_PARAMETER "/sys/module/radio_cg2900/parameters/grid"

#define VENDOR_SIGNAL_MAX 500
#define INTERFACE_SIGNAL_MAX 1000


#define STOP_SCAN_NO_ATTEMPTS 3
#define STOP_SCAN_STARTUP_SLEEP_MS 200
#define STOP_SCAN_NO_SCAN_SLEEP_MS 20

/* translate chip strength to the interface interval */
#define VENDOR_TO_INTERFACE_SIGNAL(x) \
  ((x)<VENDOR_SIGNAL_MAX?(x)*INTERFACE_SIGNAL_MAX/VENDOR_SIGNAL_MAX:INTERFACE_SIGNAL_MAX)

/* translate interface requested signal strength to chip strength */
#define INTERFACE_TO_VENDOR_SIGNAL(x) \
  ((x)<INTERFACE_SIGNAL_MAX?(x)*VENDOR_SIGNAL_MAX/INTERFACE_SIGNAL_MAX:VENDOR_SIGNAL_MAX)


#ifndef FMRADIO_CG2900_SET_TX_ONLY
/* in RX we need to do some regular cleanup of RDS data and check signal strength */
#define FMRADIO_RX_POLL_TIME_MS 2500
/* how manu polls for each signal strength read */
#define SIGNAL_STRENGTH_POLL_INTERVAL 2

#define RDS_MAX_WAIT_TIME_MS 5000

#define RDS_BLOCKS_TO_READ 22
#define RDS_BLOCKS_PER_GROUP 4
#define RDS_BLOCK_SIZE 3

/* how many seconds between eon clenanups */
#define RDS_EON_CLEANUP_INTERVAL 10

/* only report changed signal strength if going up or down >= 10% */
#define SIGSTRENGTH_REPORT_THRESHOLD 0.1

#define DEFAULT_THRESHOLD 250
#define RDS_TA_FREE_READS_BEFORE_SWITCHBACK 3
#endif

#ifndef FMRADIO_CG2900_SET_RX_ONLY
#define POWER_TX_MIN    88
#define POWER_TX_MAX    123
/* maximum number of frequencies in block scan chunk */
#define BLOCK_SCAN_MAX_CHUNK_SIZE 198
#endif

typedef unsigned long long long_time_t;

enum radio_state_t {
    RADIO_STANDBY,
    RADIO_POWERUP
};

enum radio_mode_t {
    RADIO_MODE_RX,
    RADIO_MODE_TX
};

enum scan_type_t {
    RADIO_NO_SCAN,
    RADIO_STARTUP_SCAN, /* before first iotcl has returned */
    RADIO_SEEK_SCAN,
    RADIO_BAND_SCAN,
    RADIO_BLOCK_SCAN
};

#ifndef FMRADIO_CG2900_SET_RX_ONLY
enum rds_type_t {
    RADIO_RDS_PI,
    RADIO_RDS_TP,
    RADIO_RDS_PTY,
    RADIO_RDS_TA,
    RADIO_RDS_MS,
    RADIO_RDS_AF,
    RADIO_RDS_PSN,
    RADIO_RDS_RT,
    RADIO_RDS_CT,
    RADIO_RDS_PTYN,
    RADIO_RDS_TMC,
    RADIO_RDS_TAF,
    RADIO_RDS_SHUTDOWN
};
#endif

#ifndef FMRADIO_CG2900_SET_TX_ONLY
struct fmradio_rds_session_t {
    int ta_switch_original_freq;
    int ta_switch_original_pi;
    int ta_switch_no_ta_counter;
    time_t last_eon_cleanup;
    struct fmradio_rds_bundle_t last_bundle;
    struct rds_session_t rds_session;
};
#endif

struct poll_thread_struct_t {
    enum radio_mode_t radio_mode;
    pthread_t thread_ctrl;
    struct ste_fmradio_cg2900_internal_struct_t *internal_data_p;
    int radio_fd;
    int poll_pipe_search[2];
    int poll_pipe_search_ret[2];
    int poll_pipe_shutdown[2];
    struct fmradio_vendor_callbacks_t callbacks;
#ifndef FMRADIO_CG2900_SET_TX_ONLY
    int last_signal_strength;
    struct fmradio_rds_session_t fmradio_rds_session;
    int no_afs;
    int af[RDS_MAX_AFS];
    int last_pi;
#endif
};

struct ste_fmradio_cg2900_internal_struct_t {
    int fd;
    enum radio_mode_t radio_mode;
    int grid_size;
    int min_freq;
    int max_freq;
    int use_rds;
    int current_frequency;
    int force_mono;
    enum scan_type_t ongoing_scan;
    enum radio_state_t radio_state;
    struct poll_thread_struct_t poll_thread;
#ifndef FMRADIO_CG2900_SET_TX_ONLY
    int automatic_ta_switching;
    int automatic_af_switching;
    int threshold;
#endif
};

/*
 * the following mutex must be held at any manipulation of the struct above,
 * except for poll_thread_struct_t that is only used in poll_thread
*/

static pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifndef FMRADIO_CG2900_SET_TX_ONLY

static int ste_fmradio_cg2900_is_playing_in_stereo(void **data_pp);

static int ste_fmradio_cg2900_get_signal_strength(void **data_pp);

static int ste_fmradio_cg2900_set_threshold(void **data_pp, int threshold);

static int ste_fmradio_cg2900_set_force_mono(void **data_pp, int force_mono);

static int ste_fmradio_cg2900_set_rds_reception(void **data_pp, int use_rds);

/*
 * time_long give millisecond precission reference time
 *
 * @return a reference time (based on  CLOCK_REALTIME), 0 on failure
 */

static long_time_t time_long(void)
{
    struct timespec ts;

    memset(&ts, 0, sizeof(ts));
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        ALOGE("clock_gettime failed");
        return 0;
    }

    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

#endif

/*
 * check_fd just check if there is word to read from fd and in that case
 * its value. Used for the shutdown pipe and return pipe
 *
 * @param [in] fd - the file to check
 *
 * @return Returns value in pipe or -1 if no valid available
 */

static int check_fd(int fd)
{
    struct pollfd poll_fd;

    int retval = 0;

    memset(&poll_fd, 0, sizeof(poll_fd));
    poll_fd.fd = fd;
    poll_fd.events = POLLIN;

    if (poll(&poll_fd, 1, 0) != 1)
        return -1;
    if (read(fd, &retval, sizeof(retval)) != sizeof(retval))
        return -1;

    return retval;
}

/*
 * close_pipe close both file handlers of a pipe
 *
 * @param [in] pipe - pipe to close
 *
 * @return Returns 0 on success, -1 if any fd failed close
 */
static int close_pipe(int pipe[2])
{
    if ((close(pipe[0]) == 0) && (close(pipe[1]) == 0))
        return 0;
    else
        return -1;
}


/*
 * close_all_pipes close all our used pipes
 *
 * @param [in] poll_thread_p - thread structure containing the pipes
 *
 * @return Returns 0 on success, -1 if any pipe failed close
 */
static int close_all_pipes(struct poll_thread_struct_t *poll_thread_p)
{
    if ((close_pipe(poll_thread_p->poll_pipe_search) == 0) &&
        (close_pipe(poll_thread_p->poll_pipe_search_ret) == 0) &&
        (close_pipe(poll_thread_p->poll_pipe_shutdown) == 0))
        return 0;
    else
        return -1;
}

#ifndef FMRADIO_CG2900_SET_TX_ONLY
/*
 * get_signal_strength_for_other_freq
 *
 * @param [in] fd - the radio file descriptor
 * @param [in] freq - frequency to get signal strength
 *
 * @return Returns signal strength or -1
 */
static int get_signal_strength_for_other_freq(int fd, int freq)
{
    struct v4l2_control ctrl;

    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = V4L2_CID_CG2900_RADIO_RDS_AF_UPDATE_START;
    ctrl.value = freq;
    if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
        ALOGE("get_signal_strength_for_other_freq - VIDIOC_S_CTRL failed");
        return -1;
    }

    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = V4L2_CID_CG2900_RADIO_RDS_AF_UPDATE_GET_RESULT;
    if (ioctl(fd, VIDIOC_G_CTRL, &ctrl) < 0) {
        ALOGE("get_signal_strength_for_other_freq - VIDIOC_G_CTRL failed");
        return -1;
    }

    return VENDOR_TO_INTERFACE_SIGNAL(ctrl.value);
}

/*
 * switch_station
 *
 * @param [in] fd - the radio file descriptor
 * @param [in] freq - frequency to switch to
 * @param [in] expectedPi - Programme Identification code on channel we
 *                          want to switch to
 * @param [out] result - chip switch result (0=switched, <0=no switch)
 *
 * @return Returns 0 on success and -1 on failure
 */
static int switch_station(int fd, int freq, int expected_pi, int *result)
{
    int retval = -1;
    struct v4l2_ext_controls ext_ctrl;
    struct v4l2_ext_control ctrl;
    int *p = NULL;

    memset(&ext_ctrl, 0, sizeof(ext_ctrl));
    memset(&ctrl, 0, sizeof(ctrl));
    ext_ctrl.ctrl_class = V4L2_CTRL_CLASS_USER;
    ext_ctrl.controls = &ctrl;

    ext_ctrl.count = 0;
    ctrl.id = V4L2_CID_CG2900_RADIO_RDS_AF_SWITCH_START;
    ctrl.size = 2;
    ctrl.string = (char *) malloc(sizeof(int) * ctrl.size);
    if (ctrl.string == NULL) {
        ALOGE("switch_station: Error, malloc failed");
        return FMRADIO_IO_ERROR;
    }

    p = (int *) ctrl.string;
    *p = freq * 1000 / V4L2_FREQ_FACTOR;
    *(p + 1) = expected_pi;

    retval = ioctl(fd, VIDIOC_S_EXT_CTRLS, &ext_ctrl);

    free(ctrl.string);

    if (retval < 0) {
        ALOGE("switch_station: ERROR, VIDOC_S_EXT_CTRLS failed");
        retval = FMRADIO_IO_ERROR;
    } else {
        /* note - we can't reuse ctrl here, not really sure why */
        struct v4l2_control sctrl;
        memset(&sctrl, 0, sizeof(sctrl));
        sctrl.id = V4L2_CID_CG2900_RADIO_RDS_AF_SWITCH_GET_RESULT;
        retval = ioctl(fd, VIDIOC_G_CTRL, &sctrl);

        if (retval < 0) {
            ALOGE("switch_station: ioctl VIDIOC_G_CTRL error");
            retval = FMRADIO_IO_ERROR;
        } else
            *result = sctrl.value;
    }

    return retval;
}

/*
 * find_and_switch_station
 *
 * @param [in] radio_fd - the radio file descriptor
 * @param [in] expected_pi - pi of frequency to switch to
 * @param [in] current_freq - our current frequency
 * @param [in] freq_list - list of frequencies
 * @param [in] no_freqs - number of frequencies in list
 *
 * @return Returns 0 on success and -1 on failure
 */
static int find_and_switch_station(int radio_fd, int expected_pi, int current_freq,
                                   int *freq_list, int no_freqs)
{
    int best_freq;
    int *copy_freq_list = NULL;

    copy_freq_list = malloc(sizeof(int)*no_freqs);
    if (copy_freq_list == NULL) {
        ALOGI("find_and_swtich_station: malloc failed");
        return -1;
    }
    memcpy(copy_freq_list, freq_list, sizeof(int)*no_freqs);

    do {
        int d;
        int highest = -1;
        best_freq = -1;

        for (d = 0; d < no_freqs; d++) {
            int test_freq = copy_freq_list[d];
            if ((test_freq != current_freq) &&
                (test_freq > 0)) {
                int strength = get_signal_strength_for_other_freq(radio_fd,
                                                                  test_freq);
                if (strength > highest) {
                    highest = strength;
                    best_freq = test_freq;
                }
            }
        }

        if (best_freq != -1) {
            int result;
            ALOGI("find_and_switch_station: attempting switch freq to %d kHz",
                 best_freq);
            if (switch_station(radio_fd, best_freq, expected_pi, &result) < 0) {
                ALOGW("find_and_switch_station: Error, switch station failed");
                /* no use of trying more frequencies if we can't even get ioct to run */
                best_freq = -1;
            } else {
                if (result == 0) {
                    free(copy_freq_list);
                    return best_freq;
                } else {
                    switch (result) {
                    case -1:
                        ALOGW("find_and_switch_station: warning, switch failed due to low RSSI");
                        break;
                    case -2:
                        ALOGW("find_and_switch_station: warning, switch failed due to bad PI");
                        break;
                    case -3:
                        ALOGW("find_and_switch_station: warning, switch failed due to no RDS");
                        break;
                    default:
                        ALOGW("find_and_switch_station: ERROR, switch returned strange value");
                    }
                    /* don't use this frequency again ! */

                    for (d = 0; d < no_freqs; d++) {
                        if (copy_freq_list[d] == best_freq) {
                            copy_freq_list[d] = -1;
                            break;
                        }
                    }
                }
            }
        }
    } while (best_freq != -1);
    free(copy_freq_list);
    ALOGE("find_and_switch_station: Could not switch to any available station");
    return -1;
}

/*
 * update_afs_from bundle
 *
 * AFs - Alternate Frequencies - should only be updated if we actually have
 * AFs, otherwise we should keep any old ones in list
 *
 * @param [out] poll_thread_p - poll internal data struct to be updated
 * @param [in] rds_bundle_p - read rds bundle
 */
static void update_afs_from_bundle(struct poll_thread_struct_t *poll_thread_p,
                                   struct fmradio_rds_bundle_t *rds_bundle_p)
{
    if (rds_bundle_p->num_afs > 0) {
        poll_thread_p->no_afs = rds_bundle_p->num_afs;
        memcpy(poll_thread_p->af, rds_bundle_p->af,
               rds_bundle_p->num_afs * sizeof(int));
        poll_thread_p->last_pi = rds_bundle_p->pi;
    }
}

/*
 * handle_rds_data
 *
 * parse RDS data, format structs and send events
 *
 * @param [in/out] poll_thread_p - poll internal structure
 * @param [in]     buf - rds data to pars
 * @param [in]     size - size of rds data
 *
 */
static void handle_rds_data(struct poll_thread_struct_t *poll_thread_p,
                            int frequency, unsigned char *buf, int size)
{
    int radio_fd = poll_thread_p->radio_fd;
    struct fmradio_rds_session_t *session_p = &poll_thread_p->fmradio_rds_session;
    struct rds_session_t *rds_session_p = &session_p->rds_session;
    struct fmradio_rds_bundle_t rds_bundle;
    struct ste_fmradio_cg2900_internal_struct_t *internal_data_p = poll_thread_p->internal_data_p;
    time_t now = time(NULL);

    if (session_p->last_eon_cleanup + RDS_EON_CLEANUP_INTERVAL < now) {
        cleanup_old_eon_entries(rds_session_p);
        session_p->last_eon_cleanup = now;
    }
    if (size > 0) {
        int i, j;
        int automatic_ta_switching;
        pthread_mutex_lock(&data_mutex);
        automatic_ta_switching = internal_data_p->automatic_ta_switching;
        pthread_mutex_unlock(&data_mutex);
        /* sanity check */
        if ((size % (RDS_BLOCKS_PER_GROUP * RDS_BLOCK_SIZE)) != 0) {
            ALOGE("handle_rds_data: bad rds size %u (should be %u bytes per group)",
                 size, RDS_BLOCKS_PER_GROUP * RDS_BLOCK_SIZE);
            return;
        }
        /*
         * Decode the blocks received and parse information into the
         * RDS struct
         */
        for (i = 0; i < size / RDS_BLOCK_SIZE;
             i += RDS_BLOCKS_PER_GROUP) {
            for (j = 0; j < RDS_BLOCKS_PER_GROUP; j++) {
                if (!decode_rds(rds_session_p,
                                buf[2 + (i + j) * RDS_BLOCK_SIZE],
                                buf[1 + (i + j) * RDS_BLOCK_SIZE],
                                buf[0 + (i + j) * RDS_BLOCK_SIZE]))
                    break;
            }
        }
        if (frequency > 0) {
            fill_rds(rds_session_p, &rds_bundle);
            update_afs_from_bundle(poll_thread_p,
                                   &rds_bundle);
            /* update higher layers, but only if something really is updated */


            if (memcmp(&rds_bundle, &session_p->last_bundle, sizeof(rds_bundle)) != 0) {
                if (poll_thread_p->callbacks.on_rds_data_found != NULL)
                    poll_thread_p->callbacks.on_rds_data_found(&rds_bundle, frequency);
                memcpy(&session_p->last_bundle, &rds_bundle, sizeof(rds_bundle));
            }
        }

        /* check if we received a request for traffic announcement TA */
        if (automatic_ta_switching &&
            (rds_session_p->ta_switch_requested_to_pi != 0) &&
            (session_p->ta_switch_original_pi <= 0)) {
            struct rds_eon_t *eon_p = NULL;
            int *found_freqs_p = NULL;
            int no_found_freqs = 0;

            eon_p = get_eon_from_pi(rds_session_p->eon_list,
                                    rds_session_p->ta_switch_requested_to_pi);

            ALOGI("TA switch requested to pi %4x", rds_session_p->ta_switch_requested_to_pi);
            while (eon_p && !found_freqs_p) {
                if ((eon_p->pi == rds_session_p->ta_switch_requested_to_pi) &&
                    (eon_p->mapped_freqs != NULL || eon_p->num_afs > 0)) {
                    /*
                     * we found a valid eon. First try to find a valid
                     * mapping, if not found use AFs if present
                     */
                    struct rds_mapped_freqs_t *freqs_p = eon_p->mapped_freqs;
                    while (freqs_p) {
                        if (freqs_p->transmitted_freq == frequency) {
                            unsigned int d;
                            /* scan how many mappings there are */
                            for (d = 0;
                                 d < sizeof(freqs_p->mapped_freq) / sizeof(freqs_p->mapped_freq[0]);
                                 d++) {
                                if (freqs_p->mapped_freq[d] != 0)
                                    no_found_freqs++;
                                else
                                    break;
                            }
                            found_freqs_p = freqs_p->mapped_freq;
                            break;
                        }
                        freqs_p = freqs_p->next;
                    }
                    /* if no mapping is found buy we have af's use them */
                    if ((no_found_freqs == 0) &&
                        (eon_p->num_afs > 0)) {
                        no_found_freqs = eon_p->num_afs;
                        found_freqs_p = eon_p->af;
                    }
                }
                eon_p = eon_p->next;
            }

            if (found_freqs_p) {
                int new_freq = find_and_switch_station(radio_fd,
                                                       rds_session_p->ta_switch_requested_to_pi,
                                                       frequency,
                                                       found_freqs_p,
                                                       no_found_freqs);
                if (new_freq > 0) {
                    session_p->ta_switch_original_freq = frequency;
                    frequency = new_freq;

                    pthread_mutex_lock(&data_mutex);
                    internal_data_p->current_frequency = frequency;
                    pthread_mutex_unlock(&data_mutex);
                    session_p->ta_switch_original_pi = rds_bundle.pi;
                    poll_thread_p->callbacks.
                        on_automatic_switch(new_freq, FMRADIO_SWITCH_TA);
                    clear_rds(rds_session_p);
                    session_p->ta_switch_no_ta_counter = RDS_TA_FREE_READS_BEFORE_SWITCHBACK;
                }
            }
        }

        /*
         * check if we temporary are on a TA and should switch back (=
         * TA is cleared).
         *
         * To make sure we really have got packets check that PI is set.
         * to avoid single packets without TA flag at all we use
         * use counter ta_switch_no_ta_counter to decrement to 0 before
         * acutally switch back.
         */

        if ((session_p->ta_switch_original_freq > 0)  &&
            (rds_session_p->rds_bundle.pi != 0)  &&
            (rds_session_p->rds_bundle.ta != 1)  &&
            (session_p->ta_switch_no_ta_counter-- < 1)) {
            int result;
            session_p->ta_switch_no_ta_counter = 0;
            if ((switch_station(radio_fd,
                                session_p->ta_switch_original_freq,
                                session_p->ta_switch_original_pi,
                                &result) < 0) ||
                (result < 0)) {
                ALOGE("handle_rds_data: Error, switch station failed");
            } else {
                poll_thread_p->callbacks.
                    on_automatic_switch(session_p->ta_switch_original_freq, FMRADIO_SWITCH_TA_END);
                if (poll_thread_p->callbacks.on_playing_in_stereo_changed != NULL)
                    poll_thread_p->callbacks.on_playing_in_stereo_changed(0);
                clear_rds(rds_session_p);
                memset(&session_p->last_bundle, 0, sizeof(session_p->last_bundle));

                pthread_mutex_lock(&data_mutex);
                internal_data_p->current_frequency = session_p->ta_switch_original_freq;
                pthread_mutex_unlock(&data_mutex);
            }

            /* any way, clear the switch data */
            session_p->ta_switch_original_freq = -1;
            session_p->ta_switch_original_pi = -1;
        }

    } else {
        /*
         * No RDS data mean we should reset variables and send empty bundle.
         * Do NOT reset AFs, no_afs and last_pi, since we might need to trigger
         * a switch after we lose RDS reception.
         */
        clear_rds(rds_session_p);
        fill_rds(rds_session_p, &rds_bundle);
        memset(&session_p->last_bundle, 0, sizeof(session_p->last_bundle));
        if (poll_thread_p->callbacks.on_rds_data_found != NULL)
            poll_thread_p->callbacks.on_rds_data_found(&rds_bundle, frequency);
    }
}


/*
 * handle_signal_strength_poll
 *
 * do a signal_strenght_poll and, if needed, signal result
 *
 * @param [in/out] poll_thread_p - our internal structure
 *
 */
static void handle_signal_strength_poll(struct poll_thread_struct_t *poll_thread_p)
{
    struct ste_fmradio_cg2900_internal_struct_t *internal_data_p =
        (struct ste_fmradio_cg2900_internal_struct_t*) poll_thread_p->internal_data_p;
    int signal_strength;
    enum scan_type_t ongoing_scan;
    enum radio_state_t radio_state;

    /*
     * check if a scan is ongoing or we are paused, in that case
     * skip the reads to avoid getting error messages in the log
     * (we don't hold the lock for the complete read so theoretically
     * we still might be suspended but that will only result in some
     * prints in the log, no crash etc)
     */

    pthread_mutex_lock(&data_mutex);
    ongoing_scan = poll_thread_p->internal_data_p->ongoing_scan;
    radio_state = poll_thread_p->internal_data_p->radio_state;
    pthread_mutex_unlock(&data_mutex);

    if (ongoing_scan != RADIO_NO_SCAN || radio_state == RADIO_STANDBY)
        return;

    /* check signal strength */

    signal_strength =
        ste_fmradio_cg2900_get_signal_strength
        ((void **) &internal_data_p);

    if (signal_strength >= 0) {
        int threshold;
        int automatic_af_switching;
        int current_freq;
        int no_afs = poll_thread_p->no_afs;
        int exp_pi = poll_thread_p->last_pi;
        /*
         * if two signals in row has been below half threshold and switch
         * is enabled try a switch
         */

        pthread_mutex_lock(&data_mutex);
        threshold = internal_data_p->threshold;
        automatic_af_switching = internal_data_p->automatic_af_switching;
        current_freq = internal_data_p->current_frequency;
        pthread_mutex_unlock(&data_mutex);

        if (automatic_af_switching &&
            (no_afs > 0) &&
            (poll_thread_p->last_signal_strength < threshold / 2) &&
            (signal_strength < threshold / 2)) {

            int new_freq = find_and_switch_station(poll_thread_p->radio_fd,
                                                   exp_pi,
                                                   current_freq,
                                                   poll_thread_p->af,
                                                   no_afs);
            if (new_freq != -1) {
                pthread_mutex_lock(&data_mutex);
                internal_data_p->current_frequency = new_freq;
                pthread_mutex_unlock(&data_mutex);
                poll_thread_p->no_afs = 0;
                poll_thread_p->callbacks.
                    on_automatic_switch(new_freq, FMRADIO_SWITCH_AF);
            }
            poll_thread_p->last_signal_strength = -1;
        }

        if ((poll_thread_p->last_signal_strength == -1) ||
            ((signal_strength <
              (1 - SIGSTRENGTH_REPORT_THRESHOLD) * poll_thread_p->last_signal_strength)
             || (signal_strength >
                 (1 + SIGSTRENGTH_REPORT_THRESHOLD) * poll_thread_p->last_signal_strength))) {
            poll_thread_p->
                callbacks.on_signal_strength_changed(signal_strength);
            poll_thread_p->last_signal_strength = signal_strength;
        }
    } else
        ALOGE("handle_signal_strength_poll: Warning. signalstrength failure %d",
             signal_strength);
}

#endif

/*
 * get_interrupt
 *
 * return current waiting event
 *
 * @param [in] fd - file descriptor
 * @param [out] result - interrupt result
 *
 * @return event, V4L2_CG2900_RADIO_INTERRUPT_UNKNOWN on failure
 */
enum v4l2_cg2900_radio_interrupt get_interrupt(int fd, int *result) {
    enum v4l2_cg2900_radio_interrupt retval;
    struct v4l2_ext_controls intr_result;
    struct v4l2_ext_control controls;
    int err;
    memset(&intr_result, 0, sizeof(intr_result));

    intr_result.count = 0;
    intr_result.ctrl_class = V4L2_CTRL_CLASS_USER;
    intr_result.controls = &controls;
    memset(intr_result.controls, 0, sizeof(*intr_result.controls));
    controls.id = V4L2_CID_CG2900_RADIO_GET_INTERRUPT;
    controls.size = 2;
    controls.string = malloc(controls.size * sizeof(int));

    err = ioctl(fd, VIDIOC_G_EXT_CTRLS, &intr_result);

    if (err < 0) {
        ALOGE("get_interrupt: VIDIOC_G_EXT_CTRLS failed");
        retval = V4L2_CG2900_RADIO_INTERRUPT_UNKNOWN;
        goto func_exit;
    }

    retval = ((int*)controls.string)[0];
    *result = ((int*)controls.string)[1];

 func_exit:
    free(controls.string);
    return retval;
}

/*
 * ex_poll_thread
 *
 * this thread hangs on the radio_fd to get new events (seeks and scans)
 * without needing to poll the driver. Note currenltly this will not work
 * for rds data, hence it has it own thread
 *
 * @param [in/out] args - our internal structure
 *
 * @return Returns NULL when thread is finished
 */
static void *ex_poll_thread(void *args)
{
    int shutdown = 0;
    struct poll_thread_struct_t *thread_data_p =
        (struct poll_thread_struct_t *) args;
    struct pollfd poll_fd[2];
#ifndef FMRADIO_CG2900_SET_TX_ONLY
    time_t rds_receive_time = -1;
    /* select() has a feature returning the used time in timeout variable
       (although not defined by Posix making code less portable), but due to
       several reasons we need to use poll() here forcing us to keep track
       on spent time with remaining_timeout variable and some calls to time()
    */
    int remaining_timeout = 100; /* first read of signal strength */
    int signal_strength_poll_count = 0;
    int frequency = -1;
#endif
    /* we will poll two fds, the shutdown pipe (#0) and the radio (#1) */
    memset(&poll_fd, 0, sizeof(poll_fd));
    /*pipe..[0] is the pipe's read channel*/
    poll_fd[0].fd = thread_data_p->poll_pipe_shutdown[0];
    poll_fd[0].events = POLLIN;
    poll_fd[1].fd = thread_data_p->radio_fd;
    poll_fd[1].events = POLLIN | POLLRDNORM;
    /* poll the main fd */
    while (!shutdown) {
        int search_msg = -1;
        int value;
        int ret = -1;
        int timeout = -1;
#ifndef FMRADIO_CG2900_SET_TX_ONLY
        long_time_t time_before_poll = time_long();
        if (thread_data_p->radio_mode == RADIO_MODE_RX) {
            if (remaining_timeout > 0)
                timeout = remaining_timeout;
            else
                timeout = FMRADIO_RX_POLL_TIME_MS;

        }
#endif
        ret = poll(poll_fd, 2, timeout);

#ifndef FMRADIO_CG2900_SET_TX_ONLY
        if (thread_data_p->radio_mode == RADIO_MODE_RX) {
            if (ret == 0) {
                /* timeout - reset remainign timeout */
                remaining_timeout = 0;
            } else if (ret > 0) {
                /* event occured, lower remaining timeout */
                long_time_t time_after_poll = time_long();
                int time_diff = time_after_poll - time_before_poll;

                if (time_diff <  timeout)
                    remaining_timeout = timeout - time_diff;
                else
                    remaining_timeout = 0;
            }

            pthread_mutex_lock(&data_mutex);
            int new_freq = thread_data_p->internal_data_p->current_frequency;
            pthread_mutex_unlock(&data_mutex);
            if (frequency != new_freq) {
                /* reset rds info and send empty one on old frequency */
                if (frequency != -1)
                    handle_rds_data(thread_data_p, frequency, NULL, 0);
                /* af need to be resetted separately */
                thread_data_p->no_afs = 0;
                rds_receive_time = -1;
                frequency = new_freq;
                if (thread_data_p->callbacks.on_playing_in_stereo_changed != NULL)
                    thread_data_p->callbacks.on_playing_in_stereo_changed(0);
            }
            pthread_mutex_unlock(&data_mutex);
        }

#endif
        if (ret < 0) {
            ALOGE("ex_poll_thread: poll returned error (errno=%d (%s))", errno,
                 strerror(errno));
            if (errno != EINTR) {
                search_msg = POLL_MSG_ERR;
                shutdown = 1;
            }
        }
#ifndef FMRADIO_CG2900_SET_TX_ONLY
        else if (ret == 0 && thread_data_p->radio_mode == RADIO_MODE_RX) {

            /* if we haven't got rds for some time send an empty one */

            if ((rds_receive_time != -1) &&
                (rds_receive_time + RDS_MAX_WAIT_TIME_S < time(NULL))) {
                handle_rds_data(thread_data_p, frequency, NULL, 0);
                rds_receive_time = -1;
            }

            if ((signal_strength_poll_count++ % SIGNAL_STRENGTH_POLL_INTERVAL) == 0) {
                handle_signal_strength_poll(thread_data_p);
            }

            continue;
        }
#endif

        /* first test if we have a pending shutdown, in that case set flag */
        if ((poll_fd[0].revents & POLLIN) &&
            (read(poll_fd[0].fd, &value, sizeof(value)) ==
             sizeof(value)) && (value == 1)) {
            shutdown = 1;
            search_msg = POLL_MSG_HUP;
        }

        /* check for search messages */
        if (shutdown || poll_fd[1].revents & POLLHUP) {
            search_msg = POLL_MSG_HUP;
        } else if (poll_fd[1].revents & POLLNVAL) {
            ALOGE("ex_poll_thread: poll invalid argument, fd probably closed");
            shutdown = 1;
            search_msg = POLL_MSG_ERR;
        } else if (poll_fd[1].revents & POLLERR) {
            ALOGE("ex_poll_thread: poll error, errno %d (%s)", errno,
                 strerror(errno));
            shutdown = 1;
            search_msg = POLL_MSG_ERR;
        } else if (poll_fd[1].revents & POLLRDNORM) {
            int result = 0;
            enum v4l2_cg2900_radio_interrupt interrupt =
                get_interrupt(poll_fd[1].fd, &result);
            switch (interrupt) {
            case V4L2_CG2900_RADIO_INTERRUPT_SEARCH_COMPLETED:
            case V4L2_CG2900_RADIO_INTERRUPT_BAND_SCAN_COMPLETED:
            case V4L2_CG2900_RADIO_INTERRUPT_BLOCK_SCAN_COMPLETED:
                /*
                 * currently cg2900 driver set error flag if nothing was
                 * found, but API want an "empty" result instead
                 */
                if (result == 0)
                    search_msg = POLL_MSG_EVENT;
                else
                    search_msg = POLL_MSG_EMPTY;
                break;
            case V4L2_CG2900_RADIO_INTERRUPT_SCAN_CANCELLED:
                search_msg = POLL_MSG_HUP;
                break;
            case V4L2_CG2900_RADIO_INTERRUPT_DEVICE_RESET:
                ALOGE("ex_poll_thread: device reseted");
                shutdown = 1;
                search_msg = POLL_MSG_ERR;
                break;
#ifndef FMRADIO_CG2900_SET_TX_ONLY
            case V4L2_CG2900_RADIO_INTERRUPT_MONO_STEREO_TRANSITION:
                if (thread_data_p->callbacks.on_playing_in_stereo_changed
                    != NULL) {
                    int new_stereo_value =
                        ste_fmradio_cg2900_is_playing_in_stereo((void*)&(thread_data_p->internal_data_p));
                    thread_data_p->callbacks.on_playing_in_stereo_changed(new_stereo_value);
                    /* it is quite likely the signal strength has changed */
                    handle_signal_strength_poll(thread_data_p);

                    search_msg = -1;
                }
                break;
            case V4L2_CG2900_RADIO_INTERRUPT_RDS_RECEIVED:
                /* read rds data */
                {
                    unsigned char buf[RDS_BLOCK_SIZE * RDS_BLOCKS_PER_GROUP *
                                      RDS_BLOCKS_TO_READ];
                    int ret = read(thread_data_p->radio_fd, buf, sizeof(buf));

                    handle_rds_data(thread_data_p, frequency, buf, ret);

                    if (ret <= 0 && errno != EAGAIN)
                        ALOGE("ex_poll_thread: Error reading RDS data from driver = %d", errno);
                    else
                        rds_receive_time = time(NULL);
                }

                break;
#endif
            default:
                ALOGE("ex_poll_thread:undefined interrupt type %d", interrupt);
                search_msg = POLL_MSG_ERR;
                break;
            }
        } else if (poll_fd[1].revents != 0)
            ALOGE("ex_poll_thread: unknown revents for poll_fd[1] %d",
                 poll_fd[1].revents);

        if (search_msg <= 0)
            continue;

        int size;
        int retval;

        /* check if we have a pending request for data */
        if (check_fd(thread_data_p->poll_pipe_search_ret[0]) ==
            POLL_RET_REQ) {
            (void)write(thread_data_p->poll_pipe_search[1], &search_msg,
                        sizeof(search_msg));
            if (!shutdown) {
                /* get response */
                size =
                    read(thread_data_p->poll_pipe_search_ret[0],
                         &retval, sizeof(retval));
                if ((size != sizeof(retval))
                    || (retval != POLL_RET_ACK))
                    ALOGE("ex_poll_thread: warning, not correct size (%d) or retval (%d)", size, retval);
            }
        }
        search_msg = 0;
    } /* while (...) */

    /* close the pipes, two fd's per pipe */
    close_pipe(thread_data_p->poll_pipe_search);
    close_pipe(thread_data_p->poll_pipe_search_ret);

    pthread_exit(NULL);
    return NULL;
}

/*
 * ste_fmradio_cg2900_get_frequency
 *
 * @param [in] data_pp - our current session struct
 *
 * @return Returns current frequency or negative error code on failure
 */
static int ste_fmradio_cg2900_get_frequency(void **data_pp)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    struct v4l2_frequency freq;

    int fd;

    /* Freq in KHz */

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        fd = (*internal_data_pp)->fd;
    else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("get_frequency: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    memset(&freq, 0, sizeof(freq));

    if (ioctl(fd, VIDIOC_G_FREQUENCY, &freq) != 0) {
        ALOGE("get_frequency: Error getting frequency ");
        return FMRADIO_IO_ERROR;
    }

    return (int) (freq.frequency * V4L2_FREQ_FACTOR / 1000);
}


#ifndef FMRADIO_CG2900_SET_TX_ONLY
/*
 * ste_fmradio_cg2900_set_antenna
 *
 * @param [in] data_pp - our current session struct
 * @param [in] antenna - id of antenna to use
 *
 * @return Returns 0 on success, negative error code on failure
 */
static int ste_fmradio_cg2900_set_antenna(void **data_pp, int antenna)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    int retval = FMRADIO_OK;

    struct v4l2_control ctrl;

    enum radio_mode_t radio_mode = RADIO_MODE_RX;

    int fd;
    /* Set antenna */

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL) {
        fd = (*internal_data_pp)->fd;
        radio_mode = (*internal_data_pp)->radio_mode;
    } else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("set_antenna: invalid file descriptor %d", fd);
        return FMRADIO_IO_ERROR;
    }

    if (radio_mode == RADIO_MODE_TX)
        ALOGE("set_antenna: warning - should not call in TX mode");

    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = V4L2_CID_CG2900_RADIO_SELECT_ANTENNA;
    ctrl.value = antenna;

    if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) != 0) {
        ALOGE("set_antennnna: ioctl VIDIOC_S_CTRL failed");
        retval = FMRADIO_IO_ERROR;
    }
    return retval;
}
#endif

#ifndef FMRADIO_CG2900_SET_RX_ONLY
/*
 * ste_fmradio_cg2900_set_output_power
 *
 * @param [in] data_pp - our current session struct
 * @param [in] power - power to use, should be between 88 and 123
 *
 * @return Returns 0 on success, negative error code on failure
 */
static int ste_fmradio_cg2900_set_output_power(void **data_pp, int power)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    int retval = FMRADIO_OK;

    struct v4l2_ext_controls ext_ctrl;

    enum radio_mode_t radio_mode;

    int fd;
    /* Set antenna */

    if ((power < POWER_TX_MIN) || (power > POWER_TX_MAX)) {
        ALOGE("set_output_power: invalid power %d, should be  %d<->%d", power, POWER_TX_MIN,
             POWER_TX_MAX);
        retval = FMRADIO_INVALID_PARAMETER;
        goto ret;
    }

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL) {
        fd = (*internal_data_pp)->fd;
        radio_mode = (*internal_data_pp)->radio_mode;
    } else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("set_output_power: invalid file descriptor");
        retval = FMRADIO_IO_ERROR;
        goto ret;
    }

    if (radio_mode == RADIO_MODE_RX) {
        ALOGE("set_output_power: error - radio in RX mode");
        retval = FMRADIO_UNSUPPORTED_OPERATION;
        goto ret;
    }


    memset(&ext_ctrl, 0, sizeof(ext_ctrl));

    ext_ctrl.ctrl_class = V4L2_CTRL_CLASS_FM_TX;
    ext_ctrl.count = 0;

    ext_ctrl.controls = (struct v4l2_ext_control *)
        malloc(sizeof(*ext_ctrl.controls));

    if (ext_ctrl.controls == NULL) {
        ALOGE("set_output_power: malloc failed");
        retval = FMRADIO_IO_ERROR;
        goto ret;
    }

    memset(ext_ctrl.controls, 0, sizeof(*ext_ctrl.controls));

    ext_ctrl.controls->id = V4L2_CID_TUNE_POWER_LEVEL;
    ext_ctrl.controls->size = 0;
    ext_ctrl.controls->string = NULL;
    ext_ctrl.controls->value = power;

    if (ioctl(fd, VIDIOC_S_EXT_CTRLS, &ext_ctrl) != 0) {
        ALOGE("set_output_power: ioctl VIDIOC_S_EXT_CTRL failed");
        retval = FMRADIO_IO_ERROR;
    }

    free(ext_ctrl.controls);

  ret:
    return retval;
}
#endif

/*
 * ste_fmradio_cg2900_set_frequency
 *
 * @param [in/out] data_pp - our current session struct
 * @param [in] frequency - new frequency to use
 *
 * @return Returns 0 on success, negative error code on failure
 */
static int ste_fmradio_cg2900_set_frequency(void **data_pp, int frequency)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    struct v4l2_frequency freq;
    int fd;
    int min_freq = 0;
    int max_freq = 0;

    /* Freq in KHz */

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL) {
        fd = (*internal_data_pp)->fd;
        min_freq = (*internal_data_pp)->min_freq;
        max_freq = (*internal_data_pp)->max_freq;
    } else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("set_frequency: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    if ((frequency < min_freq) || (frequency > max_freq)) {
        ALOGE("set_frequency: requested frequency %d not within"
             "limits %d<->%d", frequency, min_freq, max_freq);
        return FMRADIO_INVALID_PARAMETER;
    }

    memset(&freq, 0, sizeof(freq));

    /* set the frequency */
    freq.frequency = frequency * 1000 / V4L2_FREQ_FACTOR;
    freq.type = V4L2_TUNER_RADIO;

    if (ioctl(fd, VIDIOC_S_FREQUENCY, &freq) != 0) {
        ALOGE("set_frequency: Error setting frequency. fd: %d freq:%d filfreq: %u",
             fd, frequency, freq.frequency);
        return FMRADIO_IO_ERROR;
    }

    memset(&freq, 0, sizeof(freq));

    if (ioctl(fd, VIDIOC_G_FREQUENCY, &freq) != 0) {
        ALOGE("set_frequency: Error getting frequency");
        return FMRADIO_IO_ERROR;
    }

    if ((freq.frequency * V4L2_FREQ_FACTOR / 1000) != frequency) {
        ALOGE("set_frequency: Bad frequency read (%d, should be %d)",
             (int) (freq.frequency * V4L2_FREQ_FACTOR / 1000), frequency);
        return FMRADIO_IO_ERROR;
    }

    /* update our current frequency so we don't need to do unnecessary reads */
    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL) {
        (*internal_data_pp)->current_frequency = frequency;
    }
    pthread_mutex_unlock(&data_mutex);

    return FMRADIO_OK;
}

/*
 * set_band_and_grid
 *
 * find and set supported band/grid combinations in chip
 *
 * @param [in] low_freq - requested lower limit in band (in kHz)
 * @param [in] high_freq - requested higher limit in band (in kHz)
 * @param [in] grid - requested grid size (in kHz)
 *
 * @return Returns 0 on success, -1 on failure
 */
static int set_band_and_grid(int low_freq, int high_freq, int grid)
{
    int grid_code;
    int band_code;

    FILE *fd;

    switch (grid) {
    case 50:
        grid_code = 0;          /* China */
        break;
    case 100:
        grid_code = 1;          /* Europe and Japan */
        break;
    case 200:
        grid_code = 2;          /* US */
        break;
    default:
        ALOGE("set_band_and_grid: Invalid grid %u khz, valid values are 50, 100 & 200",
             grid);
        return -1;
    }

    if ((low_freq == FREQ_EU_LO && high_freq == FREQ_EU_HI) ||
        (low_freq == FREQ_US_LO && high_freq == FREQ_US_HI))
        band_code = 0;
    else if (low_freq == FREQ_JP_LO && high_freq == FREQ_JP_HI)
        band_code = 1;
    else if (low_freq == FREQ_CH_LO && high_freq == FREQ_CH_HI)
        band_code = 2;
    else {
        ALOGE("set_band_and_grid: Invalid interval %d->%d kHz", low_freq, high_freq);
        return -1;
    }

    fd = fopen(STE_CG2900_BAND_PARAMETER, "w");

    if (fd == NULL) {
        ALOGE("set_band_and_grid: ERROR - can't open band parameter file %s",
             STE_CG2900_BAND_PARAMETER);
        return -1;
    } else {
        fprintf(fd, "%d", band_code);
        fclose(fd);
    }

    fd = fopen(STE_CG2900_GRID_PARAMETER, "w");

    if (fd == NULL) {
        ALOGE("set_band_and_grid: ERROR - can't open grid parameter file %s",
             STE_CG2900_GRID_PARAMETER);
        return -1;
    } else {
        fprintf(fd, "%d", grid_code);
        fclose(fd);
    }

    return 0;
}

/*
 * ste_fmradio_cg2900_start
 *
 * start chip, set default frequency, default threshold, create session data,
 * start all threads etc
 *
 * @param [out] data_pp - data area to be used
 * @param [in] radio_mode - rx or tx mode
 * @param [in] callbacks_p - callbacks to use to calling layer
 * @param [in] low_freq - requested lower limit in band (in kHz)
 * @param [in] high_freq - requested higher limit in band (in kHz)
 * @param [in] default_freq - request frequency to start at (in kHz)
 * @param [in] grid - requested grid size (in kHz)
 *
 * @return Returns 0 on success, -1 on failure
 */
static int
ste_fmradio_cg2900_start(void **data_pp, enum radio_mode_t radio_mode,
                         const struct fmradio_vendor_callbacks_t
                         *callbacks_p, int low_freq, int high_freq,
                         int default_freq, int grid)
{
    struct v4l2_capability caps;
    struct ste_fmradio_cg2900_internal_struct_t *internal_data_p;
    struct poll_thread_struct_t *poll_thread_p;
    int retval;
    /*
     * check it is unused memory (but note this has to be done again at end
     * of function before updating, the only reason to check it here is to
     * avoid unecessary 4 seconds wait before reporting failure. But higher
     * layers should make sure this doesn't happen
     */

    pthread_mutex_lock(&data_mutex);
    if (*data_pp != NULL) {
        ALOGE("start: Warning! The data is already used, error");
        /* use it anyway */
    }
    pthread_mutex_unlock(&data_mutex);

    if (default_freq < low_freq || default_freq > high_freq) {
        ALOGE("start: Error, requested default frequency %d kHz is outside requested low/high interval %d-%d kHz", default_freq, low_freq, high_freq);
        return FMRADIO_INVALID_PARAMETER;
    }

    if (set_band_and_grid(low_freq, high_freq, grid) < 0) {
        ALOGE("start: Error, unsuported combination of frequency interval (%d-%d kHz) and/or grid (%d kHz)", low_freq, high_freq, grid);
        return FMRADIO_UNSUPPORTED_OPERATION;
    }

    internal_data_p = malloc(sizeof(*internal_data_p));

    if (internal_data_p == NULL) {
        ALOGE("start: Error allocating memory...");
        return FMRADIO_IO_ERROR;
    }

    memset(internal_data_p, 0, sizeof(*internal_data_p));

    poll_thread_p = &internal_data_p->poll_thread;

    internal_data_p->min_freq = low_freq;
    internal_data_p->max_freq = high_freq;

    internal_data_p->fd = open("/dev/radio0", O_RDONLY);
    if (internal_data_p->fd < 0) {
        ALOGE("start: Unable to open FM Radio device");
        ALOGE("start: FD: %d Errno: %d", internal_data_p->fd, errno);
        retval = FMRADIO_IO_ERROR;
        goto free_data;
    }

    internal_data_p->grid_size = grid;
    internal_data_p->radio_state = RADIO_POWERUP;

    memset(&caps, 0, sizeof(caps));
    if (ioctl(internal_data_p->fd, VIDIOC_QUERYCAP, &caps) != 0) {
        ALOGE("start: Error getting VIDIOC_QUERYCAP from device.");
        retval = FMRADIO_IO_ERROR;
        goto close_fd;
    }

    if ((caps.capabilities & V4L2_CAP_TUNER) == 0) {
        ALOGE("start: Error- not radio tuner.");
        retval = FMRADIO_UNSUPPORTED_OPERATION;
        goto close_fd;
    }

    if (radio_mode == RADIO_MODE_RX) {
        struct v4l2_tuner tuner;

        /* Set the Tuner data to Rx Mode */
        memset(&tuner, 0, sizeof(tuner));
        tuner.index = 0;
        tuner.rxsubchans = V4L2_TUNER_SUB_STEREO;
        if (ioctl(internal_data_p->fd, VIDIOC_S_TUNER, &tuner) != 0) {
            ALOGE("start: Error setting VIDIOC_S_TUNER to RX mode in device");
            retval = FMRADIO_IO_ERROR;
            goto close_fd;
        }
    } else {
        struct v4l2_modulator modulator;

        memset(&modulator, 0, sizeof(modulator));
        modulator.index = 0;
        /* do not turn on RDS until we have something to send */
        modulator.txsubchans = V4L2_TUNER_SUB_STEREO;
        if (ioctl(internal_data_p->fd, VIDIOC_S_MODULATOR, &modulator) !=
            0) {
            ALOGE("start: Error setting VIDIOC_S_MODULATOR to TX mode in device");
            retval = FMRADIO_IO_ERROR;
            goto close_fd;
        }
    }


    if (ste_fmradio_cg2900_set_frequency((void **) &internal_data_p,
                                         default_freq) < 0) {
        ALOGE("start: Error setting frequency to %d kHz", default_freq);
        retval = FMRADIO_IO_ERROR;
        goto close_fd;
    }
    /* for rx set antenna and threshold */

#ifndef FMRADIO_CG2900_SET_TX_ONLY
    if (radio_mode == RADIO_MODE_RX) {
        int frequency = 0;
        if (ste_fmradio_cg2900_set_antenna((void **) &internal_data_p,
                                           V4L2_CG2900_RADIO_WIRED_ANTENNA) < 0)
            {
                ALOGE("start: Error when setting wired antenna");
                retval = FMRADIO_IO_ERROR;
                goto close_fd;
            }

        frequency =
            ste_fmradio_cg2900_get_frequency((void **) &internal_data_p);
        if (frequency < 0) {
            ALOGE("start: Error, ste_fmradio_cg2900_get_frequency failed");
            retval = FMRADIO_IO_ERROR;
            goto close_fd;
        }

        if (frequency != default_freq) {
            ALOGE("start: Error, the set frequency %d kHz is not the one later reported %d kHz", default_freq, frequency);
            retval = FMRADIO_IO_ERROR;
        goto close_fd;
        }

        if (ste_fmradio_cg2900_set_threshold((void **) &internal_data_p,
                                             DEFAULT_THRESHOLD) < 0)
            ALOGE("start: Warning, failed to set threshold to %d",
                 DEFAULT_THRESHOLD);
    }
#endif

    /* setup poll pipes and start the poll thread */

    if (callbacks_p != NULL)
        poll_thread_p->callbacks = *callbacks_p;

    /* create the shutdown pipe */
    poll_thread_p->radio_fd = internal_data_p->fd;

    if (pipe(poll_thread_p->poll_pipe_search) == -1) {
        ALOGE("start: pipe creation failed #1");
        retval = FMRADIO_IO_ERROR;
        goto close_fd;
    }

    if (pipe(poll_thread_p->poll_pipe_search_ret) == -1) {
        ALOGE("start: pipe creation failed #2");
        retval = FMRADIO_IO_ERROR;
        close_pipe(poll_thread_p->poll_pipe_search);
        goto close_fd;
    }

    if (pipe(poll_thread_p->poll_pipe_shutdown) == -1) {
        ALOGE("start: pipe creation failed #3");
        retval = FMRADIO_IO_ERROR;
        close_pipe(poll_thread_p->poll_pipe_search);
        close_pipe(poll_thread_p->poll_pipe_search_ret);
        goto close_fd;
    }

    poll_thread_p->radio_mode = radio_mode;
    poll_thread_p->internal_data_p = internal_data_p;

    if (pthread_create(&poll_thread_p->thread_ctrl, NULL, ex_poll_thread,
                       poll_thread_p) != 0) {
        ALOGE("start: pthread_create failed");
        retval = FMRADIO_IO_ERROR;
        goto close_pipes;
    }
    /* update the data */

    pthread_mutex_lock(&data_mutex);
    if (*data_pp != NULL) {
        /*
         * someone else allocated the data while we were running, just free the
         * data we have allocated and return failure.
         * Very unlikely but still possible if higher layer screw up...
         */
        ALOGE("start: data area no longer NULL, someone else has allocated the memory");
        pthread_mutex_unlock(&data_mutex);
        retval = FMRADIO_IO_ERROR;
        goto close_pipes;
    }
    internal_data_p->radio_mode = radio_mode;
    *data_pp = internal_data_p;
    pthread_mutex_unlock(&data_mutex);

    return FMRADIO_OK;

  close_pipes:
    close_all_pipes(poll_thread_p);

  close_fd:
    if (internal_data_p->fd >= 0) {
        ALOGE("start: failure, closing device");
        if (close(internal_data_p->fd) != 0)
            ALOGE("start: error closing device");
    }
  free_data:

    free(internal_data_p);
    return retval;
}

#ifndef FMRADIO_CG2900_SET_TX_ONLY
/*
 * ste_fmradio_cg2900_rx_start
 *
 * start a RX session
 *
 * @param [out] data_pp - data area to be used
 * @param [in] radio_mode - rx or tx mode
 * @param [in] callbacks_p - callbacks to use to calling layer
 * @param [in] low_freq - requested lower limit in band (in kHz)
 * @param [in] high_freq - requested higher limit in band (in kHz)
 * @param [in] default_freq - request frequency to start at (in kHz)
 * @param [in] grid - requested grid size (in kHz)
 *
 * @return Returns 0 on success, -1 on failure
 */

static int ste_fmradio_cg2900_rx_start(void **data_pp, const struct fmradio_vendor_callbacks_t
                                       *callbacks, int low_freq,
                                       int high_freq, int default_freq,
                                       int grid)
{
    return ste_fmradio_cg2900_start(data_pp, RADIO_MODE_RX, callbacks,
                                    low_freq, high_freq, default_freq,
                                    grid);
}
#endif

#ifndef FMRADIO_CG2900_SET_RX_ONLY
/*
 * ste_fmradio_cg2900_tx_start
 *
 * start a TX session
 *
 * @param [out] data_pp - data area to be used
 * @param [in] radio_mode - rx or tx mode
 * @param [in] callbacks_p - callbacks to use to calling layer
 * @param [in] low_freq - requested lower limit in band (in kHz)
 * @param [in] high_freq - requested higher limit in band (in kHz)
 * @param [in] default_freq - request frequency to start at (in kHz)
 * @param [in] grid - requested grid size (in kHz)
 *
 * @return Returns 0 on success, -1 on failure
 */
static int ste_fmradio_cg2900_tx_start(void **data_pp,
                                       const struct fmradio_vendor_callbacks_t
                                       *callbacks, int low_freq,
                                       int high_freq, int default_freq,
                                       int grid)
{
    return ste_fmradio_cg2900_start(data_pp, RADIO_MODE_TX, callbacks,
                                    low_freq, high_freq, default_freq,
                                    grid);
}
#endif

/*
 * ste_fmradio_cg2900_pause
 *
 * set cg2900 driver to suspend mode
 *
 * @param [in/out] data_pp - our session struct
 *
 * @return Returns 0 on success or negative error number
 */
static int ste_fmradio_cg2900_pause(void **data_pp)
{
    struct v4l2_control sctrl;
    int fd;
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        fd = (*internal_data_pp)->fd;
    else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("pause: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    memset(&sctrl, 0, sizeof(sctrl));
    sctrl.id = V4L2_CID_CG2900_RADIO_CHIP_STATE;
    sctrl.value = RADIO_STANDBY;

    if (ioctl(fd, VIDIOC_S_CTRL, &sctrl) != 0) {
        ALOGE("pause: ioctl VIDIOC_S_CTRL failed");
        return FMRADIO_IO_ERROR;
    }

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        (*internal_data_pp)->radio_state = RADIO_STANDBY;
    pthread_mutex_unlock(&data_mutex);

    return FMRADIO_OK;
}

/*
 * ste_fmradio_cg2900_resume
 *
 * set cg2900 driver to active mode
 *
 * @param [in/out] data_pp - our session struct
 *
 * @return Returns 0 on success or negative error number
 */
static int ste_fmradio_cg2900_resume(void **data_pp)
{
    struct v4l2_control sctrl;

    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    int fd;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        fd = (*internal_data_pp)->fd;
    else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("resume: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    memset(&sctrl, 0, sizeof(sctrl));
    sctrl.id = V4L2_CID_CG2900_RADIO_CHIP_STATE;
    sctrl.value = RADIO_POWERUP;

    if (ioctl(fd, VIDIOC_S_CTRL, &sctrl) != 0) {
        ALOGE("resume: ioctl VIDIOC_S_CTRL failed");
        return FMRADIO_IO_ERROR;
    }

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        (*internal_data_pp)->radio_state = RADIO_POWERUP;
    pthread_mutex_unlock(&data_mutex);

    return FMRADIO_OK;
}

/*
 * ste_fmradio_cg2900_reset
 *
 * close our radio device, free session struct, shutdown all threads etc
 *
 * @param [in/out] data_pp - our session struct
 *
 * @return Returns 0 on success or negative error number
 */
static int ste_fmradio_cg2900_reset(void **data_pp)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    struct poll_thread_struct_t *poll_thread_p = NULL;
    int fd;
    int value = 1;
    int retval = FMRADIO_OK;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL) {
        fd = (*internal_data_pp)->fd;
        /* reset fd to make sure no more operations are done */
        (*internal_data_pp)->fd = -1;
        poll_thread_p = &(*internal_data_pp)->poll_thread;
    } else {
        ALOGE("reset: internal_data NULL");
        fd = -1;
    }

    pthread_mutex_unlock(&data_mutex);


    if (poll_thread_p != NULL) {
        /* request shutdown by writing to threads shutdown pipe */
        if (write(poll_thread_p->poll_pipe_shutdown[1], &value, sizeof(value))
            != sizeof(value))
            ALOGE("reset: warning, write failed");

        if (pthread_join(poll_thread_p->thread_ctrl, NULL) != 0)
            ALOGE("reset: warning can't join thread");

        /* close shutdown pipe's fds */

        close_pipe(poll_thread_p->poll_pipe_shutdown);
    }

    /* fd is the radio file descriptor, close if valid */
    if (fd == -1 || close(fd) != 0) {
        ALOGE("reset: Error - close failed (fd=%d)",
             fd);
        retval = FMRADIO_IO_ERROR;
    }
    /* reset struct anyway... */
    pthread_mutex_lock(&data_mutex);
    free(*internal_data_pp);
    *internal_data_pp = NULL;

    pthread_mutex_unlock(&data_mutex);

    return retval;
}

#ifndef FMRADIO_CG2900_SET_TX_ONLY

/*
 * ste_fmradio_cg2900_get_signal_strength
 *
 * get signal strenght on current tuned frequency
 *
 * @param [in] data_pp - our session struct
 *
 * @return Returns signal strenght or negative error number
 */

static int ste_fmradio_cg2900_get_signal_strength(void **data_pp)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    struct v4l2_tuner tuner;

    int fd;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        fd = (*internal_data_pp)->fd;
    else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("get_signal_strength: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    /* Get the Tuner data, including signal strength */
    memset(&tuner, 0, sizeof(tuner));
    if (ioctl(fd, VIDIOC_G_TUNER, &tuner) != 0) {
        ALOGE("get_signal_strength: Error getting VIDIOC_G_TUNER from device");
        return FMRADIO_IO_ERROR;
    }
    return VENDOR_TO_INTERFACE_SIGNAL(tuner.signal);
}

/*
 * ste_fmradio_cg2900_scan
 *
 * do a FM seek with given orientation (up or down)
 *
 * @param [in/out] data_pp - our session struct
 * @param [in] orientation - up or down
 *
 * @return Returns new frequency or negative error number
 */
static int
ste_fmradio_cg2900_scan(void **data_pp,
                        enum fmradio_seek_direction_t orientation)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    struct v4l2_hw_freq_seek seek;
    int new_frequency = -1;
    struct poll_thread_struct_t *poll_thread_p;
    int fd;
    int pollval;
    int retval = -1;
    int ret = 0;
    enum scan_type_t *ongoing_scan_p = NULL;
    /* these are needed to see if we have requested update when finished */
    int force_mono;
    int threshold;
    int use_rds;

    pthread_mutex_lock(&data_mutex);
    if ((*internal_data_pp != NULL) &&
        ((*internal_data_pp)->ongoing_scan == RADIO_NO_SCAN) &&
        ((*internal_data_pp)->fd != -1)) {
        fd = (*internal_data_pp)->fd;
        ongoing_scan_p = &(*internal_data_pp)->ongoing_scan;
        *ongoing_scan_p =  RADIO_STARTUP_SCAN;
        force_mono = (*internal_data_pp)->force_mono;
        threshold = (*internal_data_pp)->threshold;
        use_rds = (*internal_data_pp)->use_rds;
    }
    pthread_mutex_unlock(&data_mutex);

    /* if ongoing_p scan wasn't set we should exit without setting
     * scan state */
    if (ongoing_scan_p == NULL) {
        ALOGE("scan: can't start scan");
        return FMRADIO_IO_ERROR;
    }

    poll_thread_p = &(*internal_data_pp)->poll_thread;

    /* write to poll thread that we are interrested in poll event, must
     * be done before actual seek */

    pollval = POLL_RET_REQ;

    if (write(poll_thread_p->poll_pipe_search_ret[1], &pollval,
              sizeof(pollval)) != sizeof(pollval))
        ALOGE("scan: warning, write failed");

    memset(&seek, 0, sizeof(seek));
    seek.tuner = 0;
    seek.type = V4L2_TUNER_RADIO;

    /* If non-zero, seek upward, else seek downward */
    if (orientation == FMRADIO_SEEK_UP)
        seek.seek_upward = 1;
    else
        seek.seek_upward = 0;

    /* Start seek */
    ret = ioctl(fd, VIDIOC_S_HW_FREQ_SEEK, &seek);

    /* update the scan has started */
    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL) {
        (*internal_data_pp)->ongoing_scan = RADIO_SEEK_SCAN;
    }
    pthread_mutex_unlock(&data_mutex);

    /* On success 0 is returned, on error -1 and the ret variable is set
     * appropriately
     */
    if (ret < 0) {
        ALOGE("scan: Error initiating VIDIOC_S_HW_FREQ_SEEK");
        retval = FMRADIO_IO_ERROR;
        goto unset_ongoing_scan;
    }
    /* read from the poll process pipe */

    ret =
        read(poll_thread_p->poll_pipe_search[0], &pollval,
             sizeof(pollval));

    if (ret != sizeof(int)) {
        ALOGE("scan: error, read bytes %d < requested %u", ret,
             (unsigned int)sizeof(pollval));
        retval = FMRADIO_IO_ERROR;
        goto unset_ongoing_scan;
    }

    switch (pollval) {
    case POLL_MSG_EMPTY:
        /*scan seccessfully but no frequency found*/
        ALOGI("scan: no frequency found");
        /* no break */
    case POLL_MSG_EVENT:
        retval = FMRADIO_OK;
        break;
    case POLL_MSG_HUP:
        ALOGI("scan: POLLHUP - search has probably been interrupted!!!");
        retval = FMRADIO_IO_ERROR;
        break;
    case POLL_MSG_ERR:
        ALOGE("scan: poll error");
        retval = FMRADIO_IO_ERROR;
        break;
    default:
        ALOGE("scan: poll-pipe bad return value %d", pollval);
        retval = FMRADIO_IO_ERROR;
    }

    pollval = POLL_RET_ACK;

    new_frequency = ste_fmradio_cg2900_get_frequency(data_pp);

    if (write(poll_thread_p->poll_pipe_search_ret[1], &pollval,
              sizeof(pollval)) != sizeof(pollval))
        ALOGE("scan: warning, write failed");

  unset_ongoing_scan:
    {
        int update_force_mono = -1;
        int update_threshold = -1;
        int update_use_rds = -1;

        pthread_mutex_lock(&data_mutex);
        if (*internal_data_pp != NULL) {
            (*internal_data_pp)->ongoing_scan = RADIO_NO_SCAN;
            if (new_frequency > 0)
                (*internal_data_pp)->current_frequency = new_frequency;
            if ((*internal_data_pp)->force_mono != force_mono)
                update_force_mono = (*internal_data_pp)->force_mono;
            if ((*internal_data_pp)->threshold != threshold)
                update_threshold = (*internal_data_pp)->threshold;
            if ((*internal_data_pp)->use_rds != use_rds)
                update_use_rds = (*internal_data_pp)->use_rds;
        }
        pthread_mutex_unlock(&data_mutex);

        if (update_force_mono != -1) {
            /* NOTE - this 250 ms sleep before setting mono is due to a bug in kernel
             * making it not send mono events if mono triggered too soon after scan.
               ER will be written about this */
            ALOGI("sleeping delayed forced mono");
            usleep(250000);
            ALOGI("setting delayed force mono %d", update_force_mono);
            (void)ste_fmradio_cg2900_set_force_mono(data_pp, update_force_mono);
        }
        if (update_threshold != -1) {
            ALOGI("setting delayed threshold %d", update_threshold);
            (void)ste_fmradio_cg2900_set_threshold(data_pp, update_threshold);
        }
        if (update_use_rds != -1) {
            ALOGI("setting delayed use rds %d", update_use_rds);
            (void)ste_fmradio_cg2900_set_rds_reception(data_pp, update_use_rds);
        }
    }

    /* if everything sucessfully return the new frequency */
    if (retval >= 0 && new_frequency > 0)
        retval = new_frequency;

    return retval;
}

#endif

/*
 * ste_fmradio_cg2900_get_scan_results
 *
 * get search result for a band or block scan
 *
 * @param [in] fd - our radio file descriptor
 * @param [in] poll_fd - our poll pipe read file descriptor
 * @param [in] ret_fd - our poll pipe write back write descriptor
 * @param [in] scan_type - band or block scan
 * @param [out] found_data - seek dependent returned data. Free after use.
 *
 * @return Returns number of found frequencies or negative error number
 */
static int
ste_fmradio_cg2900_get_scan_results(int fd, int poll_fd, int ret_fd,
                                    enum scan_type_t scan_type,
                                    int **found_data)
{
    struct v4l2_ext_controls scan_result;
    struct v4l2_ext_control controls;
    int pollval;
    int retval;
    int number_of_found = 0;
    int err;
    int size_per_line = 0;

    /* read from the poll process pipe */

    retval = read(poll_fd, &pollval, sizeof(pollval));

    if (retval == 0) {
        /* fd EOF, just return 0 */
        number_of_found = 0;
        goto poll_callback;
    } else if (retval != sizeof(pollval)) {
        ALOGE("get_scan_results: read failure, %d (%s)", errno, strerror(errno));
        number_of_found = FMRADIO_IO_ERROR;
        goto poll_callback;
    }

    if (pollval == POLL_MSG_HUP) {
        ALOGI("get_scan_results: POLLHUP - search has probably been interrupted!!!");
        number_of_found = 0;
        goto poll_callback;
    } else if (pollval == POLL_MSG_ERR) {
        ALOGE("get_scan_results: poll error");
        number_of_found = FMRADIO_IO_ERROR;
        goto poll_callback;
    } else if (pollval == POLL_MSG_EMPTY) {
        ALOGE("get_scan_results: empty message, returning 0 items");
        number_of_found = 0;
        goto poll_callback;
    } else if (pollval != POLL_MSG_EVENT) {
        ALOGE("get_scan_results: bad return value");
        number_of_found = FMRADIO_IO_ERROR;
        goto poll_callback;
    }

    memset(&scan_result, 0, sizeof(scan_result));

    scan_result.count = 0;
    scan_result.ctrl_class = V4L2_CTRL_CLASS_USER;
    scan_result.controls = &controls;
    memset(scan_result.controls, 0, sizeof(*scan_result.controls));
    if (scan_type == RADIO_BLOCK_SCAN) {
        size_per_line = sizeof(int);
        controls.id = V4L2_CID_CG2900_RADIO_BLOCKSCAN_GET_RESULTS;
    } else {
        size_per_line = 2 * sizeof(int);
        controls.id = V4L2_CID_CG2900_RADIO_BANDSCAN_GET_RESULTS;
    }

    err = ioctl(fd, VIDIOC_G_EXT_CTRLS, &scan_result);

    if (err < 0 && errno != ENOSPC) {
        ALOGE("get_scan_results: VIDIOC_G_EXT_CTRLS failed");
        number_of_found = FMRADIO_IO_ERROR;
        goto poll_callback;
    }

    if (controls.size <= 0) {
        number_of_found = 0;    /* nothing found */
        goto poll_callback;
    }

    controls.string = (char *) malloc(size_per_line * controls.size);

    if (controls.string == NULL) {
        ALOGE("get_scan_results: malloc failed");
        number_of_found = FMRADIO_IO_ERROR;
        goto poll_callback;
    }

    number_of_found = controls.size;

    if (ioctl(fd, VIDIOC_G_EXT_CTRLS, &scan_result) < 0) {
        /* this is very strange, first call ok, second failed */
        ALOGE("get_scan_results: RadioGetBandScanResult: Second ioctl call failed... errno %d", errno);
        free(controls.string);
        number_of_found = FMRADIO_IO_ERROR;
        goto poll_callback;
    }

    *found_data = (int *) controls.string;

  poll_callback:

    /* need to send back to poll routine to enable next poll */
    {
        int returnvalue = POLL_RET_ACK;

        if (write(ret_fd, &returnvalue, sizeof(returnvalue)) !=
            sizeof(returnvalue))
            ALOGE("get_scan_results: write failed");
    }

    return number_of_found;
}

#ifndef FMRADIO_CG2900_SET_TX_ONLY
/*
 * ste_fmradio_cg2900_band_scan
 *
 * do a band scan finding all frequencies with a signal strength
 * higher than the current set threshold
 *
 * @param [in/out] data_pp - our session struct
 * @param [out] found_freqs_pp - found frequencies. Free after use.
 * @param [out] signal_strengths_pp - signal strenghts of found frequencies
 *                                    free after use
 *
 * @return Returns number of frequencies or negative error number
 */
static int
ste_fmradio_cg2900_band_scan(void **data_pp, int **found_freqs_pp,
                             int **signal_strengths_pp)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    int fd;
    struct v4l2_control ctrl;
    int retval;
    int pollval;
    enum scan_type_t *ongoing_scan_p = NULL;
    /* these are needed to see if we have requested update when finished */
    int force_mono;
    int threshold;
    int use_rds;

    pthread_mutex_lock(&data_mutex);
    if ((*internal_data_pp != NULL) &&
        ((*internal_data_pp)->ongoing_scan == RADIO_NO_SCAN) &&
        ((*internal_data_pp)->fd != -1)) {
        fd = (*internal_data_pp)->fd;
        ongoing_scan_p = &(*internal_data_pp)->ongoing_scan;
        *ongoing_scan_p =  RADIO_STARTUP_SCAN;
        force_mono = (*internal_data_pp)->force_mono;
        threshold = (*internal_data_pp)->threshold;
        use_rds = (*internal_data_pp)->use_rds;
    }
    pthread_mutex_unlock(&data_mutex);

    /* if ongoing_p scan wasn't set we should exit without setting
     * scan state */

    if (ongoing_scan_p == NULL) {
        ALOGE("band_scan: can't start scan");
        return FMRADIO_IO_ERROR;
    }

    /*
     * write to poll thread that we are interrested in poll event, must
     * be done before actual seek
     */

    pollval = POLL_RET_REQ;

    if (write((*internal_data_pp)->poll_thread.poll_pipe_search_ret[1],
              &pollval, sizeof(pollval)) != sizeof(pollval))
        ALOGE("band_scan: warning, write failed");

    memset(&ctrl, 0, sizeof(ctrl));

    ctrl.id = V4L2_CID_CG2900_RADIO_BANDSCAN;
    ctrl.value = V4L2_CG2900_RADIO_BANDSCAN_START;

    if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) != 0) {
        ALOGE("band_scan: Error when starting band scan");
        retval = FMRADIO_IO_ERROR;
        goto set_no_scan_and_exit;
    }

    /* update the scan has started */
    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL) {
        (*internal_data_pp)->ongoing_scan = RADIO_BAND_SCAN;
    }
    pthread_mutex_unlock(&data_mutex);

    int *data_p = NULL;
    int poll_fd = (*internal_data_pp)->poll_thread.poll_pipe_search[0];
    int ret_fd =
        (*internal_data_pp)->poll_thread.poll_pipe_search_ret[1];

    retval =
        ste_fmradio_cg2900_get_scan_results(fd, poll_fd, ret_fd,
                                            RADIO_BAND_SCAN, &data_p);

    if (retval > 0) {
        int d;

        *found_freqs_pp = malloc(sizeof(int) * retval);
        if (*found_freqs_pp == NULL) {
            ALOGE("band_scan: malloc failed");
            free(data_p);
            retval = FMRADIO_IO_ERROR;
            goto set_no_scan_and_exit;
        }

        *signal_strengths_pp = malloc(sizeof(int) * retval);

        if (*signal_strengths_pp == NULL) {
            ALOGE("band_scan: malloc failed");
            free(data_p);
            free(*found_freqs_pp);
            retval = FMRADIO_IO_ERROR;
            goto set_no_scan_and_exit;
        }

        for (d = 0; d < retval; d++) {
            (*found_freqs_pp)[d] =
                (int) (data_p[2 * d] * V4L2_FREQ_FACTOR / 1000);
            (*signal_strengths_pp)[d] =
                VENDOR_TO_INTERFACE_SIGNAL(data_p[2 * d + 1]);
        }
        free(data_p);
    }

 set_no_scan_and_exit:
    {
        int update_force_mono = -1;
        int update_threshold = -1;
        int update_use_rds = -1;

        pthread_mutex_lock(&data_mutex);
        if (*internal_data_pp != NULL) {
            (*internal_data_pp)->ongoing_scan = RADIO_NO_SCAN;
            if ((*internal_data_pp)->force_mono != force_mono)
                update_force_mono = (*internal_data_pp)->force_mono;
            if ((*internal_data_pp)->threshold != threshold)
                update_threshold = (*internal_data_pp)->threshold;
            if ((*internal_data_pp)->use_rds != use_rds)
                update_use_rds = (*internal_data_pp)->use_rds;
        }
        pthread_mutex_unlock(&data_mutex);
        if (update_force_mono != -1) {
            /* NOTE - this 250 ms sleep before setting mono is due to a bug in kernel
             * making it not send mono events if mono triggered too soon after scan.
               ER will be written about this */
            ALOGI("sleep delayed force mono");
            usleep(250000);
            ALOGI("setting delayed force mono %d", update_force_mono);
            (void)ste_fmradio_cg2900_set_force_mono(data_pp, update_force_mono);
        }
        if (update_threshold != -1) {
            ALOGI("setting delayed threshold %d", update_threshold);
            (void)ste_fmradio_cg2900_set_threshold(data_pp, update_threshold);
        }
        if (update_use_rds != -1) {
            ALOGI("setting delayed rds %d", update_use_rds);
            (void)ste_fmradio_cg2900_set_rds_reception(data_pp, update_use_rds);
        }
    }
    return retval;
}
#endif

/*
 * ste_fmradio_cg2900_stop_band_scan
 *
 * stop any ongoing band scan. Note! Currently also used for stopping
 * seeks and block scan, hence it should be included in TX only builds
 *
 * @param [in/out] data_pp - our session struct
 *
 * @return Returns 0 on success or negative error number
 */
static int ste_fmradio_cg2900_stop_band_scan(void **data_pp)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    int fd;

    struct v4l2_control ctrl;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        fd = (*internal_data_pp)->fd;
    else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("stop_band_scan: get_signal_strength: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    memset(&ctrl, 0, sizeof(ctrl));

    ctrl.id = V4L2_CID_CG2900_RADIO_BANDSCAN;
    ctrl.value = V4L2_CG2900_RADIO_BANDSCAN_STOP;

    if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) != 0) {
        ALOGE("stop_band_scan: Error when stopping band scan");
        return FMRADIO_IO_ERROR;
    }

    return FMRADIO_OK;
}

#ifndef FMRADIO_CG2900_SET_TX_ONLY
/*
 * ste_fmradio_cg2900_is_playing_in_stereo
 *
 * are we currently playing in stereo? We might be mono if we are
 * 1) station sends in mono 2) forced mono is set 3) bad signal strength from
 * station
 *
 * @param [in] data_pp - our session struct
 *
 * @return Returns 1 for stereo, 0 for mono or negative error number
 */
static int ste_fmradio_cg2900_is_playing_in_stereo(void **data_pp)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    struct v4l2_tuner tuner;

    int fd;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        fd = (*internal_data_pp)->fd;
    else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("is_playing_in_stereo: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    /* Get the Tuner data, including signal strength */
    memset(&tuner, 0, sizeof(tuner));
    if (ioctl(fd, VIDIOC_G_TUNER, &tuner) != 0) {
        ALOGE("is_playing_in_stereo: Error getting VIDIOC_G_TUNER from device");
        return FMRADIO_IO_ERROR;
    }
    return ((tuner.rxsubchans & V4L2_TUNER_SUB_STEREO) &&
            (tuner.audmode & V4L2_TUNER_MODE_STEREO));
}

/*
 * ste_fmradio_cg2900_is_rds_data_supported
 *
 * is the chip currenlty in rds enable state
 *
 * @param [in] data_pp - our session struct
 *
 * @return Returns positive number if rds is supported, 0 otherwise and
 * and negative error number if any error occured
 */
static int ste_fmradio_cg2900_is_rds_data_supported(void **data_pp)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    struct v4l2_tuner tuner;

    int fd;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        fd = (*internal_data_pp)->fd;
    else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("is_rds_data_supported: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    /* Get the Tuner data, including signal strength */
    memset(&tuner, 0, sizeof(tuner));
    if (ioctl(fd, VIDIOC_G_TUNER, &tuner) != 0) {
        ALOGE("is_rds_data_supported: Error getting VIDIOC_G_TUNER from device");
        return FMRADIO_IO_ERROR;
    }
    return tuner.rxsubchans && V4L2_TUNER_SUB_RDS;
}

/*
 * ste_fmradio_cg2900_is_tuned_to_valid_channel
 *
 * is the chip currenlty tuned to a valid channel
 *
 * @param [in] data_pp - our session struct
 *
 * @return Returns positive number if tuned to valid channel, 0 otherwise and
 * and negative error number if any error occured
 */
static int ste_fmradio_cg2900_is_tuned_to_valid_channel(void **data_pp)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    struct v4l2_tuner tuner;

    struct v4l2_control ctrl;

    int fd;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        fd = (*internal_data_pp)->fd;
    else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("is_tuned_to_valid_channel: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    /* Get the Tuner data, including signal strength */
    memset(&tuner, 0, sizeof(tuner));
    if (ioctl(fd, VIDIOC_G_TUNER, &tuner) != 0) {
        ALOGE("is_tuned_to_valid_channel: Error getting VIDIOC_G_TUNER from device");
        return FMRADIO_IO_ERROR;
    }

    /* Get the RSSI threshold of the chip */
    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = V4L2_CID_CG2900_RADIO_RSSI_THRESHOLD;
    if (ioctl(fd, VIDIOC_G_CTRL, &ctrl) != 0) {
        ALOGE("is_tuned_to_valid_channel: Error getting VIDIOC_G_CTRL from device");
        return FMRADIO_IO_ERROR;
    }

    return tuner.signal >= ctrl.value;
}
#endif

#ifndef FMRADIO_CG2900_SET_RX_ONLY
/*
 * block_scan_chunk
 *
 * read a block scan chunk
 *
 * @param [in] fd - file descriptor to use
 * @param [in] grid_size - grid size to use (in kHz)
 * @param [in] poll_thread_p - link to poll thread pipes
 * @param [in] start_freq - chunk start frequency (in kHz)
 * @param [in] end_freq - chunk end frequency (in kHz)
 * @param [out] ongoing_scan_p - ptr to current scan_type (lock needed!)
 * @param [out] signal_strengths_pp - returned signal strengths
 *
 * @return Returns 0 on success or negative error number
 */

static int
block_scan_chunk(int fd, int grid_size, struct poll_thread_struct_t *poll_thread_p,
                 int start_freq, int end_freq, enum scan_type_t *ongoing_scan_p,
                 int **signal_strengths_pp)
{
    struct v4l2_ext_controls ext_ctrl;

    int pollval;
    int *p = NULL;
    int retval = FMRADIO_OK;

    /*
     * write to poll thread that we are interested in poll event, must
     * be done before actual seek
     */

    pollval = POLL_RET_REQ;

    if (write(poll_thread_p->poll_pipe_search_ret[1],
              &pollval, sizeof(pollval)) != sizeof(pollval))
        ALOGE("block_scan_chunk: warning, write failed");

    memset(&ext_ctrl, 0, sizeof(ext_ctrl));
    ext_ctrl.ctrl_class = V4L2_CTRL_CLASS_USER;
    ext_ctrl.controls = (struct v4l2_ext_control *)
        malloc(sizeof(*ext_ctrl.controls));
    if (ext_ctrl.controls == NULL) {
        ALOGE("block_scan: malloc failed");
        return FMRADIO_IO_ERROR;
    }

    memset(ext_ctrl.controls, 0, sizeof(*ext_ctrl.controls));

    ext_ctrl.count = 0;
    ext_ctrl.controls->id = V4L2_CID_CG2900_RADIO_BLOCKSCAN_START;
    ext_ctrl.controls->size = 2;

    ext_ctrl.controls->string =
        (char *) malloc(sizeof(int) * (ext_ctrl.controls->size));
    if (ext_ctrl.controls->string == NULL) {
        ALOGE("block_scan_chunk: malloc failed");
        free(ext_ctrl.controls);
        return FMRADIO_IO_ERROR;
    }

    memset(ext_ctrl.controls->string, 0,
           sizeof(int) * (ext_ctrl.controls->size));
    p = (int *) ext_ctrl.controls->string;

    *p = start_freq * 1000 / V4L2_FREQ_FACTOR;
    *(p + 1) = end_freq * 1000 / V4L2_FREQ_FACTOR;

    if (ioctl(fd, VIDIOC_S_EXT_CTRLS, &ext_ctrl) != 0) {
        ALOGE("block_scan_chunk: Error when starting block scan");
        retval = FMRADIO_IO_ERROR;
    } else {
        int poll_fd = poll_thread_p->poll_pipe_search[0];

        int ret_fd =
            poll_thread_p->poll_pipe_search_ret[1];

        /* update the scan has started if still in startup state */
        pthread_mutex_lock(&data_mutex);
        if (*ongoing_scan_p == RADIO_STARTUP_SCAN)
            *ongoing_scan_p = RADIO_BLOCK_SCAN;
        pthread_mutex_unlock(&data_mutex);

        retval =
            ste_fmradio_cg2900_get_scan_results(fd, poll_fd, ret_fd,
                                                RADIO_BLOCK_SCAN,
                                                signal_strengths_pp);

        /*
         * check if we got the correct number of frequencies, otherwise we
         * don't know how to map them
         */

        if (retval > 0) {
            if ((end_freq - start_freq) / grid_size + 1 != retval) {
                ALOGE("block_scan_chunk: block_scan returning too few values %d,"
                     " should be %d(range %d->%d kHz, grid %d kHz)",
                     retval, (end_freq - start_freq) / grid_size + 1,
                     start_freq, end_freq, grid_size);
                free(*signal_strengths_pp);
                *signal_strengths_pp = NULL;
                retval = -1;
            } else {
                int d;

                /* translate signal strengths to interface standard (0-1000) */

                for (d = 0; d < retval; d++) {
                    (*signal_strengths_pp)[d] =
                        VENDOR_TO_INTERFACE_SIGNAL((*signal_strengths_pp)
                                               [d]);
                }
            }
        }
    }

    free(ext_ctrl.controls->string);
    free(ext_ctrl.controls);

    return retval;
}

/*
 * ste_fmradio_cg2900_block_scan
 *
 * do a block scan scan finding all frequencies given the requested band
 * interval and our current grid settings.
 *
 * @param [in/out] data_pp - our session struct
 * @param [in] start_freq
 * @param [in] end_freq - frequency range to scan
 * @param [out] frequencies_pp - found frequencies. Free after use.
 * @param [out] signal_strengths_pp - signal strenghts of found frequencies.
 *                                    Free after use.
 *
 * returns number of frequencies/signal strengths or negative error number
 */
static int
ste_fmradio_cg2900_block_scan(void **data_pp, int start_freq, int end_freq,
                              int **frequencies_pp,
                              int **signal_strengths_pp)
{
    int retval = FMRADIO_OK;
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    int fd;
    int grid_size = 0;
    struct poll_thread_struct_t *poll_thread_p = NULL;
    enum scan_type_t *ongoing_scan_p = NULL;
    int cur_start_freq;
    int cur_end_freq;
    int min_freq;

    pthread_mutex_lock(&data_mutex);
    if ((*internal_data_pp != NULL) &&
        ((*internal_data_pp)->ongoing_scan == RADIO_NO_SCAN) &&
        ((*internal_data_pp)->fd != -1)) {
        fd = (*internal_data_pp)->fd;
        grid_size = (*internal_data_pp)->grid_size;
        (*internal_data_pp)->ongoing_scan = RADIO_STARTUP_SCAN;
        poll_thread_p =  &(*internal_data_pp)->poll_thread;
        ongoing_scan_p = &(*internal_data_pp)->ongoing_scan;
        *ongoing_scan_p =  RADIO_STARTUP_SCAN;
        min_freq = (*internal_data_pp)->min_freq;
    }
    pthread_mutex_unlock(&data_mutex);

    /* if ongoing_p scan wasn't set we should exit without setting
     * scan state */
    if (ongoing_scan_p == NULL) {
        ALOGE("block_scan: can't start scan");
        return FMRADIO_IO_ERROR;
    }

    /* sanity check */

    if (start_freq > end_freq) {
        ALOGE("block_scan: start_freq > end_freq");
        retval = FMRADIO_INVALID_PARAMETER;
        goto unset_ongoing_scan;
    }

    if (((start_freq - min_freq) % grid_size) != 0) {
        ALOGE("block_scan: start_freq not a multiple of grid(%d kHz)", grid_size);
        retval = FMRADIO_INVALID_PARAMETER;
        goto unset_ongoing_scan;
    }

    if (((end_freq - start_freq) % grid_size) != 0) {
        ALOGE("block_scan: end_freq not a multiple of grid(%d kHz)", grid_size);
        retval = FMRADIO_INVALID_PARAMETER;
        goto unset_ongoing_scan;
    }

    *frequencies_pp = malloc(sizeof(int) * ((end_freq - start_freq) / grid_size + 1));

    if (*frequencies_pp == NULL) {
        ALOGE("block_scan: out of memory");
        retval = FMRADIO_IO_ERROR;
        goto unset_ongoing_scan;
    }

    *signal_strengths_pp = malloc(sizeof(int) * ((end_freq - start_freq) / grid_size + 1));

    if (*signal_strengths_pp == NULL)  {
        ALOGE("block_scan: out of memory");
        free(*frequencies_pp);
        *frequencies_pp = NULL;
        retval = FMRADIO_IO_ERROR;
        goto unset_ongoing_scan;
    }

    cur_start_freq = start_freq;

    do {
        int chunk_retval;
        int *chunk_signal_strengths_p = NULL;

        if (cur_start_freq +  (BLOCK_SCAN_MAX_CHUNK_SIZE - 1) * grid_size > end_freq)
            cur_end_freq = end_freq;
        else
            cur_end_freq = cur_start_freq +  (BLOCK_SCAN_MAX_CHUNK_SIZE - 1) * grid_size;

        ALOGI("block_scan:chunk %d->%d", cur_start_freq, cur_end_freq);

        chunk_retval = block_scan_chunk(fd, grid_size, poll_thread_p,
                                        cur_start_freq, cur_end_freq,
                                        ongoing_scan_p, &chunk_signal_strengths_p);

        if (chunk_retval > 0) {
            memcpy(*signal_strengths_pp + (cur_start_freq - start_freq) / grid_size,
                   chunk_signal_strengths_p,
                   sizeof(int) * ((cur_end_freq - cur_start_freq) / grid_size + 1));
            retval += chunk_retval;
        } else
            retval = chunk_retval;

        if (chunk_signal_strengths_p != NULL)
            free(chunk_signal_strengths_p);

        /* point start to next chunk */
        cur_start_freq = cur_end_freq + grid_size;

    } while (retval >= 0 && cur_end_freq < end_freq);

    if (retval > 0) {
        int d;
        for (d = 0; d < retval; d++)
            (*frequencies_pp)[d] = start_freq + d * grid_size;

    } else {
        free(*frequencies_pp);
        free(*signal_strengths_pp);
        *frequencies_pp = NULL;
        *signal_strengths_pp = NULL;
    }


 unset_ongoing_scan:
    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        (*internal_data_pp)->ongoing_scan = RADIO_NO_SCAN;
    pthread_mutex_unlock(&data_mutex);

    return retval;
}

#endif

/*
 * ste_fmradio_cg2900_stop_scan
 *
 * Stop any ongoing scan.
 *
 * @param [in/out] data_pp - our session struct
 *
 * @return Returns FMRADIO_OK on success, or negative error number
 */
static int ste_fmradio_cg2900_stop_scan(void **data_pp)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    int retval = FMRADIO_OK;
    enum scan_type_t ongoing_scan;
    int scan_stop_attempts = 0;

    /*
     * to avoid errors when a seeks first ioctl is executing, we await the seek
     * leave the startup state
     */
    do {
        pthread_mutex_lock(&data_mutex);
        if (*internal_data_pp != NULL) {
            ongoing_scan = (*internal_data_pp)->ongoing_scan;
        } else {
            ongoing_scan = RADIO_NO_SCAN;
        }
        pthread_mutex_unlock(&data_mutex);

        if (ongoing_scan == RADIO_STARTUP_SCAN) {
            /* scan starting up, do some waiting before trying again */
            usleep(STOP_SCAN_STARTUP_SLEEP_MS * 1000);
        } else if (ongoing_scan == RADIO_NO_SCAN) {
            /* no scan, but we might be upstartign. do a minimal wait */
            usleep(STOP_SCAN_NO_SCAN_SLEEP_MS * 1000);
        } else {
            /* ongoing scan, we do not need to wait */
            break;
        }

    } while (scan_stop_attempts++ < STOP_SCAN_NO_ATTEMPTS);

    switch (ongoing_scan) {
        /* currently the same method is used for all scan types */
    case RADIO_BAND_SCAN:
    case RADIO_BLOCK_SCAN:
    case RADIO_SEEK_SCAN:
        retval = ste_fmradio_cg2900_stop_band_scan(data_pp);
        break;
    default:
        retval = FMRADIO_UNSUPPORTED_OPERATION;
        ALOGE("stop_scan: Stop scan not supported for non bandscan (ongoing_scan=%d)!", ongoing_scan);
    }

    return retval;
}

#ifndef FMRADIO_CG2900_SET_RX_ONLY
/*
 * radio_set_rds_tx
 *
 * Add rds data field to be transmitted
 *
 * @param [in] fd - radio file descriptor
 * @param [in]type - information field to be updated
 * @param [in]data - new data to be written to information field
 *
 * @return Returns FMRADIO_OK on success, or negative error number
 */
static int radio_set_rds_tx(struct ste_fmradio_cg2900_internal_struct_t
                            **internal_data_pp, enum rds_type_t type,
                            void *data)
{
    struct v4l2_ext_controls ext_ctrl;
    int fd;
    int rds_tx = 0;
    int ret = FMRADIO_UNSUPPORTED_OPERATION;
    int usestring = 0;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL) {
        fd = (*internal_data_pp)->fd;
        rds_tx = (*internal_data_pp)->use_rds;
    } else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("radio_set_rds_tx: no valid device");
        return FMRADIO_IO_ERROR;
    }

    if (type == RADIO_RDS_SHUTDOWN) {
        /* switch off rds */
        struct v4l2_modulator modulator;

        memset(&modulator, 0, sizeof(modulator));
        modulator.index = 0;
        modulator.txsubchans = V4L2_TUNER_SUB_STEREO;
        if (ioctl(fd, VIDIOC_S_MODULATOR, &modulator) !=
            0) {
            ALOGE("radio_set_rds_tx: Error setting VIDIOC_S_MODULATOR to TX mode in device");
            return FMRADIO_IO_ERROR;
        }

        pthread_mutex_lock(&data_mutex);
        (*internal_data_pp)->use_rds = 0;
        pthread_mutex_unlock(&data_mutex);
        return 0;
    } else if (rds_tx == 0) {
        /* switch on rds  */
        struct v4l2_modulator modulator;

        memset(&modulator, 0, sizeof(modulator));
        modulator.index = 0;
        modulator.txsubchans = V4L2_TUNER_SUB_STEREO | V4L2_TUNER_SUB_RDS ;
        if (ioctl(fd, VIDIOC_S_MODULATOR, &modulator) !=
            0) {
            ALOGE("radio_set_rds_tx: Error setting VIDIOC_S_MODULATOR to TX mode in device");
            return FMRADIO_IO_ERROR;
        }

        pthread_mutex_lock(&data_mutex);
        (*internal_data_pp)->use_rds = 1;
        pthread_mutex_unlock(&data_mutex);
    }

    memset(&ext_ctrl, 0, sizeof(ext_ctrl));
    ext_ctrl.ctrl_class = V4L2_CTRL_CLASS_FM_TX;
    ext_ctrl.controls = (struct v4l2_ext_control *)
        malloc(sizeof(*ext_ctrl.controls));
    if (ext_ctrl.controls == NULL)
        return ret;
    memset(ext_ctrl.controls, 0, sizeof(*ext_ctrl.controls));

    ext_ctrl.count = 0;
    switch (type) {
    case RADIO_RDS_PI:
        ext_ctrl.controls->id = V4L2_CID_RDS_TX_PI;
        ext_ctrl.controls->size = 0;
        ext_ctrl.controls->value = *(int *) data;
        break;
    case RADIO_RDS_PTY:
        ext_ctrl.controls->id = V4L2_CID_RDS_TX_PTY;
        ext_ctrl.controls->size = 0;
        ext_ctrl.controls->value = *(int *) data;
        break;
    case RADIO_RDS_PSN:
        ext_ctrl.controls->id = V4L2_CID_RDS_TX_PS_NAME;
        ext_ctrl.controls->size = strlen(data);
        ext_ctrl.controls->string =
            (char *) malloc(ext_ctrl.controls->size + 1);
        if (ext_ctrl.controls->string == NULL)
            goto early_exit;
        memcpy(ext_ctrl.controls->string, (char *) data,
               ext_ctrl.controls->size);
        usestring = 1;
        break;
    case RADIO_RDS_RT:
        ext_ctrl.controls->id = V4L2_CID_RDS_TX_RADIO_TEXT;
        ext_ctrl.controls->size = strlen(data);
        ext_ctrl.controls->string =
            (char *) malloc(ext_ctrl.controls->size + 1);
        if (ext_ctrl.controls->string == NULL)
            goto early_exit;
        memcpy(ext_ctrl.controls->string, (char *) data,
               ext_ctrl.controls->size);
        usestring = 1;
        break;
    default:
        ret = FMRADIO_UNSUPPORTED_OPERATION;
        goto exit;
        break;
    }
    ret = ioctl(fd, VIDIOC_S_EXT_CTRLS, &ext_ctrl);
    if (ret < 0) {
        ALOGE("set_rds_tx: VIDIOC_S_EXT_CTRLS failed");
    }
  exit:
    if (usestring == 1) {
        free(ext_ctrl.controls->string);
    }
  early_exit:
    free(ext_ctrl.controls);
    return ret;
}

/*
 * ste_fmradio_cg2900_set_rds_data
 *
 * set rds data to be transmitted
 *
 * @param [in] data_pp - our session struct
 * @param [in] key - field to be updated
 * @param [in] value - new data to be written to field
 *
 * @return Returns FMRADIO_OK on success, or negative error number
 */
static int ste_fmradio_cg2900_set_rds_data(void **data_pp, char *key,
                                           void *value)
{
    int retval = FMRADIO_UNSUPPORTED_OPERATION;

    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;

    if (key == NULL) {
        retval = radio_set_rds_tx(internal_data_pp, RADIO_RDS_SHUTDOWN, value);
    } else if (!strcmp(key, "PI")) {
        retval = radio_set_rds_tx(internal_data_pp, RADIO_RDS_PI, value);
    } else  if (!strcmp(key, "PTY")) {
        retval = radio_set_rds_tx(internal_data_pp, RADIO_RDS_PTY, value);
    } else if (!strcmp(key, "PSN")) {
        retval = radio_set_rds_tx(internal_data_pp, RADIO_RDS_PSN, value);
    } else if (!strcmp(key, "RT")) {
        retval = radio_set_rds_tx(internal_data_pp, RADIO_RDS_RT, value);
    }
    return retval;
}
#endif

#ifndef FMRADIO_CG2900_SET_TX_ONLY
/*
 * ste_fmradio_cg2900_set_force_mono
 *
 * force radio into mono mode on/off
 *
 * @param [in] data_pp - our session struct
 * @param [in] forceMono - mono on (1) off (0)
 *
 * @return Returns 0 on success or negative error number
 */
static int ste_fmradio_cg2900_set_force_mono(void **data_pp, int force_mono)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    int fd;
    int use_rds;

    struct v4l2_tuner tuner;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL) {
        fd = (*internal_data_pp)->fd;
        use_rds = (*internal_data_pp)->use_rds;
        if ((*internal_data_pp)->ongoing_scan != RADIO_NO_SCAN) {
            ALOGI("ongoing scan - delaying set force mono");
            (*internal_data_pp)->force_mono = force_mono;
            pthread_mutex_unlock(&data_mutex);
            return FMRADIO_OK;
        }
    } else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("set_force_mono: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    memset(&tuner, 0, sizeof(tuner));
    tuner.index = 0;
    if (use_rds)
        tuner.rxsubchans = V4L2_TUNER_SUB_RDS;

    if (force_mono)
        tuner.rxsubchans |= V4L2_TUNER_SUB_MONO;
    else
        tuner.rxsubchans |= V4L2_TUNER_SUB_STEREO;

    if (ioctl(fd, VIDIOC_S_TUNER, &tuner) != 0) {
        ALOGE("set_force_mono: VIDIOC_S_TUNER failed");
        return FMRADIO_IO_ERROR;
    }

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        (*internal_data_pp)->force_mono = force_mono;
    pthread_mutex_unlock(&data_mutex);

    return FMRADIO_OK;
}

/*
 * ste_fmradio_cg2900_set_threshold
 *
 * set our current signal strenght threshold
 *
 * @param [in/out] data_pp - our session struct
 * @param [in] threshold - signal strenght limit to use (0-1000)
 *
 * @return Returns 0 on success or negative error number
 */
static int ste_fmradio_cg2900_set_threshold(void **data_pp, int threshold)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    int fd;

    struct v4l2_control sctrl;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL) {
        fd = (*internal_data_pp)->fd;
        if ((*internal_data_pp)->ongoing_scan != RADIO_NO_SCAN) {
            ALOGI("ongoing scan - delaying set threshold");
            (*internal_data_pp)->threshold = threshold;
            pthread_mutex_unlock(&data_mutex);
            return FMRADIO_OK;
        }

    } else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("set_threshold: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    memset(&sctrl, 0, sizeof(sctrl));
    sctrl.id = V4L2_CID_CG2900_RADIO_RSSI_THRESHOLD;
    sctrl.value = INTERFACE_TO_VENDOR_SIGNAL(threshold);

    if (ioctl(fd, VIDIOC_S_CTRL, &sctrl) < 0) {
        ALOGE("set_threshold: VIDIOC_S_CTRL failed");
        return FMRADIO_IO_ERROR;
    }

    pthread_mutex_lock(&data_mutex);
    (*internal_data_pp)->threshold = threshold;
    pthread_mutex_unlock(&data_mutex);

    return FMRADIO_OK;
}

/*
 * ste_fmradio_cg2900_rds_reception
 *
 * turn rds reception on/off
 *
 * @param [in/out] data_pp - our session struct
 * @param [in] use_rds - should rds be decoded (1 = on, 0 = off)
 *
 * @return Returns 0 on success or negative error number
 */
static int ste_fmradio_cg2900_set_rds_reception(void **data_pp, int use_rds)
{

    struct v4l2_tuner tuner;
    int fd;
    int force_mono;
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL) {
        fd = (*internal_data_pp)->fd;
        force_mono = (*internal_data_pp)->force_mono;
        if ((*internal_data_pp)->ongoing_scan != RADIO_NO_SCAN) {
            ALOGI("ongoing scan - delaying set rds");
            (*internal_data_pp)->use_rds = use_rds;
            pthread_mutex_unlock(&data_mutex);
            return FMRADIO_OK;
        }
    } else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("set_rds_reception: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    memset(&tuner, 0, sizeof(tuner));
    tuner.index = 0;
    if (force_mono)
        tuner.rxsubchans = V4L2_TUNER_SUB_MONO;
    else
        tuner.rxsubchans = V4L2_TUNER_SUB_STEREO;

    if (use_rds)
        tuner.rxsubchans |= V4L2_TUNER_SUB_RDS;

    if (ioctl(fd, VIDIOC_S_TUNER, &tuner) != 0) {
        ALOGE("set_rds_reception: Error setting VIDIOC_S_TUNER");
        return FMRADIO_IO_ERROR;
    }

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        (*internal_data_pp)->use_rds = use_rds;
    pthread_mutex_unlock(&data_mutex);
    return FMRADIO_OK;
}

/*
 * ste_fmradio_cg2900_get_threshold
 *
 * return the current signal strenght threshold in chip
 *
 * @param [in] data_pp - our session struct
 *
 * @return Returns threshold (0-1000) on success or negative error number
 */
static int ste_fmradio_cg2900_get_threshold(void **data_pp)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    int fd;

    struct v4l2_control sctrl;

    pthread_mutex_lock(&data_mutex);
    if (*internal_data_pp != NULL)
        fd = (*internal_data_pp)->fd;
    else
        fd = -1;
    pthread_mutex_unlock(&data_mutex);

    if (fd == -1) {
        ALOGE("get_threshold: invalid file descriptor");
        return FMRADIO_IO_ERROR;
    }

    memset(&sctrl, 0, sizeof(sctrl));
    sctrl.id = V4L2_CID_CG2900_RADIO_RSSI_THRESHOLD;

    if (ioctl(fd, VIDIOC_G_CTRL, &sctrl) < 0) {
        ALOGE("get_threshold: VIDIOC_G_CTRL failed");
        return FMRADIO_IO_ERROR;
    }

    return VENDOR_TO_INTERFACE_SIGNAL(sctrl.value);
}
#endif

/*
 * ste_fmradio_cg2900_send_extra_command
 *
 * execute commands not supported by the fmradio api
 *
 * @param [in] data_pp - our session struct
 * @param [in] command - comand to use
 * @param [in] parameters - command parameters
 * @param [out] out_parameters - returned value. Free after use.
 *
 * @return Returns 0 on success or negative error number
 */
static int
ste_fmradio_cg2900_send_extra_command(void **data_pp, const char *command,
                                      char **parameters_pp,
                                      struct fmradio_extra_command_ret_item_t
                                      **out_parameters_pp)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;
    int retval;

    if (strcmp(command, "vendor_information") == 0) {
        *out_parameters_pp =
            calloc(3, sizeof(struct fmradio_extra_command_ret_item_t));

        (*out_parameters_pp)[0].key = strdup("device_name");
        (*out_parameters_pp)[0].type = FMRADIO_TYPE_STRING;
        (*out_parameters_pp)[0].data.string_value =
            strdup("ste_fmradio_cg2900");

        (*out_parameters_pp)[1].key = strdup("device_version");
        (*out_parameters_pp)[1].type = FMRADIO_TYPE_STRING;
        (*out_parameters_pp)[1].data.string_value = strdup("4");
        /* mark this being our last parameter */
        (*out_parameters_pp)[2].key = NULL;

        retval = FMRADIO_OK;
#ifndef FMRADIO_CG2900_SET_RX_ONLY
    } else if ((strcmp(command, "SetOutputPower") == 0) &&
               (parameters_pp != NULL) && (*parameters_pp != NULL)) {
        int power;

        *out_parameters_pp = NULL;
        if (sscanf(*parameters_pp, "%d", &power) == 1)
            retval = ste_fmradio_cg2900_set_output_power(data_pp, power);
        else
            retval = FMRADIO_INVALID_PARAMETER;
#endif
#ifndef FMRADIO_CG2900_SET_TX_ONLY
    } else  if ((strcmp(command, "SetAntenna") == 0) && (parameters_pp != NULL)
        && *parameters_pp) {
        int antenna;
        *out_parameters_pp = NULL;

        if (sscanf(*parameters_pp, "%d", &antenna) == 1)
            retval = ste_fmradio_cg2900_set_antenna(data_pp, antenna);
        else
            retval = FMRADIO_INVALID_PARAMETER;
#endif
    } else if (strcmp(command, "sleep") == 0) {
        sleep(2);
        *out_parameters_pp = NULL;

        pthread_mutex_lock(&data_mutex);
        if ((*internal_data_pp == NULL) || (*internal_data_pp)->fd < 0) {
            ALOGE("send_extra_command: bad fd");
            retval = FMRADIO_IO_ERROR;
        } else
            retval = FMRADIO_OK;
        pthread_mutex_unlock(&data_mutex);
    } else {
        ALOGE("send_extra_command: unknown command or bad parameter");
        *out_parameters_pp = NULL;
        retval = FMRADIO_INVALID_STATE;
    }

    return retval;
}

#ifndef FMRADIO_CG2900_SET_TX_ONLY
/*
 * ste_fmradio_cg2900_set_automatic_af_switching
 *
 * enable or disable automatic switching when better alternative frequencies (AFs)
 * are available
 *
 * @param [in/out] data_pp - our session struct
 * @param [in] automatic 0 = no automatic swithcing, 1 = automatic switching
 *
 * @return Returns 0 on success or negative error number
 */
static int
ste_fmradio_cg2900_set_automatic_af_switching(void **data_pp, int automatic)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;

    if (*internal_data_pp != NULL) {
        pthread_mutex_lock(&data_mutex);
        (*internal_data_pp)->automatic_af_switching = automatic;
        pthread_mutex_unlock(&data_mutex);
    } else
        return -1;

    return 0;
}

/*
 * ste_fmradio_cg2900_set_automatic_ta_switching
 *
 * enable or disable automatic Traffic Announcement(TA) switching
 *
 * @param [in/out] data_pp - our session struct
 * @param [in] automatic 0 = no automatic switching, 1 = automatic switching
 *
 * @return Returns 0 on success or negative error number
 */
static int
ste_fmradio_cg2900_set_automatic_ta_switching(void **data_pp, int automatic)
{
    struct ste_fmradio_cg2900_internal_struct_t **internal_data_pp =
        (struct ste_fmradio_cg2900_internal_struct_t **) data_pp;

    if (*internal_data_pp != NULL) {
        pthread_mutex_lock(&data_mutex);
        (*internal_data_pp)->automatic_ta_switching = automatic;
        pthread_mutex_unlock(&data_mutex);
    } else
        return -1;

    return 0;
}
#endif

/*
 * register_fmradio_functions
 *
 * returned our supported methods to jni layer
 *
 * @param [out] signature - sanity check signature to write
 * @param [out] vendor_methods_p - gets filled with our supported functions
 *
 * @return Returns 0 (currently can't fail)
 */
int
register_fmradio_functions(long *signature,
                         struct fmradio_vendor_methods_t *vendor_methods_p)
{
    memset(vendor_methods_p, 0, sizeof(*vendor_methods_p));

    vendor_methods_p->reset = ste_fmradio_cg2900_reset;
    vendor_methods_p->pause = ste_fmradio_cg2900_pause;
    vendor_methods_p->resume = ste_fmradio_cg2900_resume;
    vendor_methods_p->set_frequency = ste_fmradio_cg2900_set_frequency;
    vendor_methods_p->get_frequency = ste_fmradio_cg2900_get_frequency;
    vendor_methods_p->stop_scan = ste_fmradio_cg2900_stop_scan;
    vendor_methods_p->send_extra_command =
        ste_fmradio_cg2900_send_extra_command;

#ifndef FMRADIO_CG2900_SET_TX_ONLY
    vendor_methods_p->rx_start = ste_fmradio_cg2900_rx_start;
    vendor_methods_p->get_signal_strength =
        ste_fmradio_cg2900_get_signal_strength;
    vendor_methods_p->scan = ste_fmradio_cg2900_scan;
    vendor_methods_p->full_scan = ste_fmradio_cg2900_band_scan;
    vendor_methods_p->is_playing_in_stereo =
        ste_fmradio_cg2900_is_playing_in_stereo;
    vendor_methods_p->is_rds_data_supported =
        ste_fmradio_cg2900_is_rds_data_supported;
    vendor_methods_p->is_tuned_to_valid_channel =
        ste_fmradio_cg2900_is_tuned_to_valid_channel;
    vendor_methods_p->set_automatic_af_switching =
        ste_fmradio_cg2900_set_automatic_af_switching;
    vendor_methods_p->set_automatic_ta_switching =
        ste_fmradio_cg2900_set_automatic_ta_switching;
    vendor_methods_p->set_force_mono = ste_fmradio_cg2900_set_force_mono;
    vendor_methods_p->get_threshold = ste_fmradio_cg2900_get_threshold;
    vendor_methods_p->set_threshold = ste_fmradio_cg2900_set_threshold;
    vendor_methods_p->set_rds_reception = ste_fmradio_cg2900_set_rds_reception;
#endif

#ifndef FMRADIO_CG2900_SET_RX_ONLY
    vendor_methods_p->tx_start = ste_fmradio_cg2900_tx_start;
    vendor_methods_p->block_scan = ste_fmradio_cg2900_block_scan;
    vendor_methods_p->set_rds_data = ste_fmradio_cg2900_set_rds_data;
#endif


    /* to signature is just to show caller the correct function was called */
    *signature = FMRADIO_SIGNATURE;
    return 0;
}
