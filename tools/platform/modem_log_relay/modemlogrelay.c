/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <semaphore.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/wait.h>

#include <linux/netlink.h>
#include <linux/db8500-modem-trace.h>

#include "cops.h"
#include "shm_netlnk.h"
#include "libmon.h"
#include "dgramsk.h"
#include "coredump.h"
#include "kerneldump.h"
#include "libmlr.h"
#include "cn_client.h"
#include "modemlogrelay.h"
#include "bass_app.h"
#include "rilhandler.h"

static struct evac_info_s evac_info = {.imei = "", .is_autotrigger = 0} ;

// To keep size of internal FIFO buffer
static volatile long trace_buffer_size = 0;
static pthread_mutex_t lock_trace_buffer_size;

// This will be used to store the modem version information
static char modem_version_info[CN_MAX_STRING_BUFF];

int sdcard_check_mount(void);

static int set_max_trigger_report(char **command, int count);
static int enable_trace_auto_trigger(char **command);
static int autoconf_set_dump_path(void);
static int autoconf_check_file(void);
static int overwrite_trace_autoconf(char **command);


/**
 * \fn int my_write(int fd, void const *buffer, size_t length)
 * \brief
 *
 * \param
 * \return
 */
static int my_write(int fd, void const *buffer, size_t length)
{
    size_t     rlen = length;
    size_t     max_len = 4096;
    size_t     clen;
    char const *ptr = buffer;

    if (fd < 0) {
        ALOGE("Trying to call my_write to fd = %d, exiting method.", fd);
        return -1;
    }

    EXTRADEBUG("IN : %d bytes to write on fd %d", length, fd);

    while (rlen > 0) {

        clen = rlen;

        if (rlen > max_len) {
            clen = max_len;
        }

        ssize_t wsize = write(fd, ptr, clen);

        if (wsize == -1) {
            ALOGW("modemlogrelay:: my_write write error");

            if (errno == ENOSPC) {
                return errno;
            }

            return -1;
        }

        ptr += wsize;
        rlen -= wsize;
    }

    ALOGI("OUT : %d bytes written on fd %d", length, fd);

    return 0;
}

void event_init(struct event_s *evt, int state)
{
    evt->flag = state;
    evt->lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    evt->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
}

int event_get_state(struct event_s *evt)
{
    return evt->flag;
}
void event_wait_set(struct event_s *evt, int state, int state_final)
{
    pthread_mutex_lock(&evt->lock);

    while (evt->flag != state) {
        pthread_cond_wait(&evt->cond, &evt->lock);
    }

    evt->flag = state_final;
    pthread_mutex_unlock(&evt->lock);
}

void event_wait(struct event_s *evt, int state)
{
    pthread_mutex_lock(&evt->lock);

    while (evt->flag != state) {
        pthread_cond_wait(&evt->cond, &evt->lock);
    }

    pthread_mutex_unlock(&evt->lock);
}

void event_notify(struct event_s *evt, int state)
{
    pthread_mutex_lock(&evt->lock);

    evt->flag = state;
    pthread_cond_broadcast(&evt->cond);

    pthread_mutex_unlock(&evt->lock);
}

/**
 * \fn int evac_thread_write(evac_fifo_t *p, char *buf, int size)
 * \brief  Write the internal buffer for modem logging to
 *         trace file.
 *
 * \param evac_fifo_t*  Data structure holding the trace file information
 *        buf           Buffer to be written to the file
 *        size          Size of the buffer to write
 *
 * \return 0 on SUCCESS, -1 on Failure
 */
int evac_thread_write(evac_fifo_t *p, char *buf, int size)
{
    int n = 0;
    size_t r_size, c_size;
    char *p_buf;

    // Non-circular buffer
    if (!p->rol_file.enabled) {
        // Trace file splitting is allowed in SDCARD/FS mode
        if ((evac_info.is_autotrigger) && (evac_info.type != DEST_USB)) {
            p->rol_file.cur_size += size;

        }

        n = my_write(p->fd, buf, size);

        if (n == ENOSPC) {
            ALOGI("No more space on storage device, fd %d will be closed", p->fd);
            event_notify(&evac_info.storage_evt, STORAGE_FULL);
        }

        //Prevent never-ending loop if "trace --rol_file ON 0 0" has been set.
    } else if (p->rol_file.max_size == 0) {
        p->rol_file.cur_size = 0;
        goto out;
    } else {
        r_size =  size;
        p_buf = buf;

        while (r_size > 0) {

            c_size = r_size;

            if ((p->rol_file.cur_size + r_size) > p->rol_file.max_size) {
                c_size = p->rol_file.max_size - p->rol_file.cur_size;
            }

            n = my_write(p->fd, p_buf, c_size);

            // If SD card is full and our current file is larger than 10% of specified rol_file max size
            // New max_size will be set to current file size and file will be rolled
            if (n == ENOSPC) {
                if (p->rol_file.cur_size >= p->rol_file.threshold) {
                    p->rol_file.max_size = p->rol_file.cur_size;
                    ALOGI("Storage media is full. New max size of current file is set to %llu", p->rol_file.max_size);
                    p->rol_file.cur_size = 0;
                    lseek(p->fd, 0L, SEEK_SET);
                    continue;
                } else {
                    event_notify(&evac_info.storage_evt, STORAGE_FULL);
                    goto out;
                }
            } else if (n) {
                goto out;
            }

            r_size -= c_size;
            p_buf += c_size;
            p->rol_file.cur_size += c_size;

            if (p->rol_file.cur_size >= p->rol_file.max_size) {
                p->rol_file.cur_size = 0;
                lseek(p->fd, 0L, SEEK_SET);
            }
        }
    }

out:
    return n;
}


/**
 * \fn void evac_thread(evac_fifo_t *evac_fifo)
 * \brief
 *
 * \param
 * \return
 */
void evac_thread(evac_fifo_t *evac_fifo)
{
    int state, n = 0, end = 0;
    struct evac_buffer *evacbuf;

    while (!end) {
        EXTRADEBUG("Waiting for data... fifo=%x",
                   (unsigned int)evac_fifo);

        /* wait data */
        if (sem_wait(&(evac_fifo->data_ready)) < 0) {
            ALOGE("evac_thread: sem_wait returned error, errno = %d", errno);
        }

        event_wait_set(&evac_info.trigger_evt, EVAC_UNLOCKED, EVAC_LOCKED);

        while (evac_fifo->first != NULL) {
            EXTRADEBUG("Data received");
            /* Lock fifo access */
            pthread_mutex_lock(&(evac_fifo->mutex));

            evacbuf = evac_fifo->first;

            /* update the next buffer to evacuate */
            evac_fifo->first = evacbuf->next;

            /* if it was the last buffer */
            if (evac_fifo->first == NULL) {
                evac_fifo->last = NULL;
            }

            evac_fifo->count--;
            evac_fifo->countrd++;

            pthread_mutex_lock(&lock_trace_buffer_size);
            trace_buffer_size -= evacbuf->size;
            pthread_mutex_unlock(&lock_trace_buffer_size);

            /* Unlock fifo access */
            pthread_mutex_unlock(&(evac_fifo->mutex));

            if (n != ENOSPC) {
                n = evac_thread_write(evac_fifo, evacbuf->buffer, evacbuf->size);
            }

            if (n) {
                end = 1;
            }

            free(evacbuf->buffer);
            free(evacbuf);
        }

        event_notify(&evac_info.trigger_evt, EVAC_UNLOCKED);

        // For non circular buffer, splitting of file is enabled.
        if ((evac_info.is_autotrigger) &&
                (evac_info.type != DEST_USB) &&
                (evac_fifo->rol_file.cur_size > evac_fifo->rol_file.max_size)) {
            ALOGI("Trace file reached maximum, an aumatic trigger report will be generated for non-circular buffer save mode.");
            trace_trigger(HOST_TRIGGER);
        }

        state = event_get_state(&evac_info.trace_evt);

        if (state == EVAC_STOP) {
            EXTRADEBUG("evac_fifo->first == NULL!!!");
            end = 1;
        }
    }

    if (evac_info.type != DEST_USB) {
        close(evac_fifo->fd);
        evac_fifo->fd = -1;
    }

    EXTRADEBUG("EVAC_THREAD END !!!!...");
}


/**
 * \fn int evac_fifo_init(evac_fifo_t *evac_fifo)
 * \brief
 *
 * \param
 * \return
 */
int evac_fifo_init(evac_fifo_t *evac_fifo)
{
    evac_fifo->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    sem_init(&(evac_fifo->data_ready), 0, 0);
    evac_fifo->first = NULL;
    evac_fifo->last  = NULL;

    evac_fifo->rol_file.enabled = 0;
    evac_fifo->rol_file.cur_size = 0;
    evac_fifo->rol_file.max_size = 0;


    evac_fifo->count  = 0;
    evac_fifo->countwr  = 0;
    evac_fifo->countrd  = 0;
    evac_fifo->missed = 0;

    return 0;
}


/**
 * \fn int evac_fifo_put(evac_fifo_t *evac_fifo, struct evac_buffer *evac_buf)
 * \brief Adds the received traces to internal trace buffer
 *
 * \param evac_fifo: fifo where to add new data
 * \param evac_buf: data to add
 *
 * \return 0 on success, 1 if internal buffer threshold is reached
 */
int evac_fifo_put(evac_fifo_t *evac_fifo, struct evac_buffer *evac_buf)
{
    int ret = 0;

    EXTRADEBUG("post one element to fifo=%x", (unsigned int) evac_fifo);

    /* Lock the fifo */
    pthread_mutex_lock(&(evac_fifo->mutex));
    EXTRADEBUG("mutex ok");

    /* If active and internal trace buffer size is larger than TRACE_DISCARD_THRESHOLD
     * first chunk in current buffer queue will be removed and new chunk will be added */
    if (TRACE_DISCARD) {
        pthread_mutex_lock(&lock_trace_buffer_size);

        if ((evac_fifo->first != NULL) && (evac_buf->size + trace_buffer_size >= TRACE_DISCARD_THRESHOLD)) {

            struct evac_buffer *temp_evacbuf;
            ALOGW("Internal trace buffer full, will discard trace (fifo = %x)", (unsigned int) evac_fifo);
            temp_evacbuf = evac_fifo->first;
            evac_fifo->first = temp_evacbuf->next;

            evac_fifo->count--;
            evac_fifo->missed++;
            trace_buffer_size -= temp_evacbuf->size;
            free(temp_evacbuf->buffer);
            free(temp_evacbuf);

            /* if it was the last buffer */
            if (evac_fifo->first == NULL) {
                evac_fifo->last = NULL;
            }

            ret = 1;
        }

        pthread_mutex_unlock(&lock_trace_buffer_size);
    }

    if (!evac_fifo->last) {
        evac_fifo->first = evac_buf;
        evac_fifo->last = evac_buf;
    } else {
        evac_fifo->last->next = evac_buf;
        evac_fifo->last = evac_buf;
    }

    EXTRADEBUG("post sema data ready");

    /* post that there is an another element in the fifo */
    sem_post(&(evac_fifo->data_ready));

    EXTRADEBUG("unlock mutex");

    evac_fifo->count++;
    evac_fifo->countwr++;

    pthread_mutex_lock(&lock_trace_buffer_size);
    trace_buffer_size += evac_buf->size;
    pthread_mutex_unlock(&lock_trace_buffer_size);

    EXTRADEBUG("Internal buffer size %ld", trace_buffer_size);

    /* Unlock the fifo */
    pthread_mutex_unlock(&(evac_fifo->mutex));

    return ret;
}

/**
 * \fn void evac_wait_start(void)
 * \brief
 *
 * \return
 */
void evac_wait_start(void)
{
    event_wait(&evac_info.trace_evt, EVAC_START);

    /* if config is usb */
    if (evac_info.type == DEST_USB) {
        event_wait(&evac_info.dlink_evt, DLINK_CONNECTED);
    }

    else if (evac_info.type == DEST_SDCARD) {
        event_wait(&evac_info.sdcard_evt, SD_MOUNTED);
    }

    else if (evac_info.type == DEST_UNKNOWN) {
        ALOGW("Destination unknown!!!");
    }
}

int get_modem_data(mon_trace_flush_ind_t *p, char *buffer)
{
    struct modem_trace_req req;
    int ret;

    req.phys_addr = p->address;
    req.filler = p->filler;
    req.buff = (__u8 *)buffer;
    req.size = p->size;

    ret = ioctl(evac_info.modem_fd, TM_TRACE_REQ, &req);
    EXTRADEBUG("p->size: 0x%x, req.size: 0x%x, ret: %d\n",
               p->size, req.size, ret);

    if ((ret < 0) || ((uint32_t)req.size != p->size)) {
        ALOGE("correct read is not returning expected size");
        return -1;
    }

    return 0;
}

/**
 * \fn int evac_fifo_insert(int buf_id, int start_addr, * int size)
 * \brief
 *
 * \param
 * \return
 */
int evac_fifo_insert(int buf_id, int start_addr, int size)
{
    int n_buffer;
    struct evac_buffer *evac_buf;

    evac_buf = (struct evac_buffer *)malloc(sizeof(struct evac_buffer));

    if (!evac_buf) {
        ALOGE("Failed to allocate memory for evac_buf!");
        return -1;
    }

    evac_buf->size = size;
    evac_buf->next = NULL;
    evac_buf->buffer = (char *)malloc(size);

    if (!evac_buf->buffer) {
        free(evac_buf);
        ALOGE("Failed to allocate memory for buffer for evac_buf!");
        return -1;
    }

    memcpy(evac_buf->buffer, (void *)start_addr, size);

    /* determine which buffer (then which fifo) */
    if (evac_info.nb_fifo == 1) {
        n_buffer = 0;
    } else {
        n_buffer = buf_id;
    }

    ALOGI("Add element for buffer=%d, fd=%d with size=%d", n_buffer, evac_info.fifo[n_buffer].fd, size);

    if (evac_info.fifo[n_buffer].fd >= 0) {
        /* Insert to fifo */
        if (evac_fifo_put(&(evac_info.fifo[n_buffer]), evac_buf) == 1) {
            ALOGW("Buffer %s data chunk discarded. %d chunks totally discarded for this buffer",
                  filename_suffix[n_buffer], evac_info.fifo[n_buffer].missed);
        }

    } else {
        ALOGW("FD closed. Discarding trace from buffer: %d, fd: %d", n_buffer, evac_info.fifo[n_buffer].fd);
        free(evac_buf->buffer);
        free(evac_buf);
    }

    return 0;
}


/**
 * \fn void read_imei(char *imei, size_t imei)
 * \brief
 *
 * \param
 * \return
 */
int read_imei(char *imei, size_t size)
{
    cops_context_id_t *ctx = NULL;
    cops_return_code_t ret_code;
    cops_imei_t cops_imei;

    ret_code = cops_context_create(&ctx, NULL, NULL);

    if (ret_code != COPS_RC_OK) {
        ALOGW("Unable to connect to COPS");
        return -1;
    }

    ret_code = cops_read_imei(ctx, &cops_imei);

    /* free cops_context */
    cops_context_destroy(&ctx);

    if (ret_code != COPS_RC_OK) {
        ALOGW("Unable to read IMEI");
        return -1;
    }

    snprintf(imei, size, "%1d%1d%1d%1d%1d%1d-%1d%1d-%1d%1d%1d%1d%1d%1d-%1d",
             cops_imei.digits[0],
             cops_imei.digits[1],
             cops_imei.digits[2],
             cops_imei.digits[3],
             cops_imei.digits[4],
             cops_imei.digits[5],
             cops_imei.digits[6],
             cops_imei.digits[7],
             cops_imei.digits[8],
             cops_imei.digits[9],
             cops_imei.digits[10],
             cops_imei.digits[11],
             cops_imei.digits[12],
             cops_imei.digits[13],
             cops_imei.digits[14]);
    return 0;
}

/*
* dir_trace_filter
*
* check if a directory or file entry match report prefix or trace suffix
*
* input  : *dirent
* output : 0 if dir entry name match prefix
*          -1 if not
*/
int dir_trace_filter(const struct dirent *entry)
{
    /* check if directory prefix is mcd__ */
    if (strstr(entry->d_name, PREFIX_REPORT) != 0 ||
            strstr(entry->d_name, filename_suffix[0]) != 0 ||
            strstr(entry->d_name, filename_suffix[1]) != 0 ||
            strstr(entry->d_name, filename_suffix[2]) != 0 ||
            strstr(entry->d_name, filename_suffix[3]) != 0) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * check_for_old_traces
 *
 * Compares modified date on found trace files to check if any of the
 * files/directories are older than user specified trace history
 * Deletes all trace files and directories older than specified days (trace_history).
 *
 * input  : pointer on directory name
 * output : 0 OK
 *          other value on errors
 */
int check_for_old_traces(char *directory_name)
{
    struct dirent **filelist;
    int nb_file;
    struct stat fileinfo;
    struct timeval tv;
    struct timezone tz;
    long elapsed;

    gettimeofday(&tv, &tz);

    if (trace_history != 0) {
        ALOGI("Trace history configured, trace files older than %ld days will be deleted", trace_history);
        /* scan directory    */
        nb_file = scandir(directory_name, &filelist, dir_trace_filter, NULL);

        if (nb_file < 0) {
            ALOGE("Failed to scan trace directory");
            return 1;
        } else if (nb_file == 0) {
            ALOGI("No trace file found");
        } else {
            int i;
            long trace_hist_sec = trace_history * 60 * 60 * 24;

            for (i = 0; i < nb_file; i++) {
                char path[PATH_LENGTH];
                // get the complete path for the file
                snprintf(path, PATH_LENGTH, "%s/%s", directory_name, filelist[i]->d_name);

                if (stat(path, &fileinfo) != -1) {

                    EXTRADEBUG("File Name: %s,  File size = %lld", filelist[i]->d_name, fileinfo.st_size);

                    elapsed = tv.tv_sec - fileinfo.st_mtime;

                    if (elapsed > trace_hist_sec) {
                        ALOGI("Removing trace file/dir %s since it is older than specified trace history.", path);

                        if (S_ISDIR(fileinfo.st_mode)) {
                            delete_dir(path);
                        } else {
                            remove(path);
                        }

                        event_notify(&evac_info.storage_evt, STORAGE_AVAIL);
                    }
                }

                free(filelist[i]);
            }

            free(filelist);
        }
    } else {
        ALOGI("Trace history not configured, will keep all traces");
    }

    return 0;
}

/**
 * \fn void evac_create_file(struct evac_info_s *ev)
 * \brief evac_info_s
 * \param
 * \return
 */

void evac_create_file(struct evac_info_s *ev)
{
    int fd, i, n;
    char name[256];
    char imei[IMEI_SIZE];
    struct tm *time;

    check_for_old_traces(ev->dest_path);

    /*  imei is read one */
    if (ev->imei[0] == 0) {
        /* get imei */
        n = read_imei(imei, sizeof(imei));

        if (n) {
            snprintf(imei, sizeof(imei), "123456-12-123456-1");
        }

        snprintf(ev->imei, IMEI_SIZE, "%s", imei);
    } else {
        /*  call for evac trigger  */
        /* close all file open previously */
        for (i = 0; i < MLR_MAX_EVAC_FILE; i++) {
            if (ev->fifo[i].fd >= 0) {
                close(ev->fifo[i].fd);
            }
        }

        snprintf(imei, IMEI_SIZE, "%s", ev->imei);
    }

    /* get date and time */
    time = get_time_stamp();

    for (i = 0; i < MLR_MAX_EVAC_FILE; i++) {

        snprintf(name, sizeof(name), "%s/%s_%s_%04d%02d%02d%02d%02d%02d",
                 ev->dest_path, modem_version_info, imei,
                 1900 + time->tm_year,
                 time->tm_mon + 1,
                 time->tm_mday,
                 time->tm_hour,
                 time->tm_min,
                 time->tm_sec);

        snprintf(ev->filename, MAX_PATH_LEN, "%s", name);
#ifdef SPLIT_TRACE_FILES
        snprintf(name + strlen(name), sizeof(name) - strlen(name), "%s", filename_suffix[i]);
#endif
        fd = open(name, O_CREAT | O_RDWR);

        if (fd < 0) {
            ALOGE("Cannot create file %s", name);
            exit(1);
        }

        ev->fifo[i].fd = fd;
        ev->fifo[i].rol_file.enabled = ev->rol_ff.enabled;
        ev->fifo[i].rol_file.cur_size = 0;
        ev->fifo[i].rol_file.max_size =
            ev->rol_ff.max_size[mlr_buffer_type[i]];
        // Used when SD card is full
        ev->fifo[i].rol_file.threshold = ev->fifo[i].rol_file.max_size / 10;

        EXTRADEBUG("%d --> rol_enable = %d, max=%lld", i,
                   ev->fifo[i].rol_file.enabled,
                   ev->fifo[i].rol_file.max_size);
    }

    memset(log_complete_status, 0x00, sizeof(log_complete_status));
    snprintf(log_complete_status, sizeof(log_complete_status), "%s/%s_%s_%04d%02d%02d%02d%02d%02d_%s",
             ev->dest_path, modem_version_info, imei,
             1900 + time->tm_year,
             time->tm_mon + 1,
             time->tm_mday,
             time->tm_hour,
             time->tm_min,
             time->tm_sec,
             "log_not_completed");
    fd = open(log_complete_status, O_CREAT | O_RDWR);

    if (fd < 0) {
        ALOGE("Unable to create status file to indicate logging complete %s", log_complete_status);
    }

    if (write(fd, dummy_file_content, sizeof(dummy_file_content)) < 0) {
        ALOGE("Unable to add content to status file");
    }

    close(fd);
}


/**
 * \fn void evac_handler(void)
 * \brief
 *
 * \param
 * \return
 */
void evac_handler(void)
{
    int i;
    pthread_t thread[MLR_MAX_EVAC_FILE];
    struct stat path_stat;

    while (1) {
        EXTRADEBUG("Wait START TRACE IND");
        evac_wait_start();
        EXTRADEBUG("Wait START TRACE IND OK");

        if (evac_info.type == DEST_USB) {
            evac_info.fifo[0].fd = sk_serv[SK_SERV_DATA].fd;
            evac_info.fifo[0].rol_file.enabled = 0;
        } else {
            ALOGI("SDCARD / FS ...: %s", evac_info.dest_path);

            if (!(!stat(evac_info.dest_path, &path_stat)
                    && S_ISDIR(path_stat.st_mode))) {
                /* Path does not exists, so create it */

                if (Create_dir(evac_info.dest_path)) {
                    ALOGE("Cannot create trace folder");
                    exit(EXIT_FAILURE);
                }

            }

            evac_create_file(&evac_info);
        }

        for (i = 0; i < evac_info.nb_fifo; i++)
            pthread_create(&thread[i], NULL, (void *)evac_thread,
                           (void *) & (evac_info.fifo[i]));

        for (i = 0; i < evac_info.nb_fifo; i++) {
            pthread_join(thread[i], NULL);
        }

        // If storage is full we check to see if we can create space by deleting old logs
        if ((evac_info.type == DEST_SDCARD || evac_info.type == DEST_FS)
                && event_get_state(&evac_info.flush_evt) != EVAC_FLUSHING
                && event_get_state(&evac_info.storage_evt) == STORAGE_FULL) {

            check_for_old_traces(evac_info.dest_path);

            if (event_get_state(&evac_info.storage_evt) == STORAGE_FULL) {
                event_notify(&evac_info.trace_evt, EVAC_STOP);
                ALOGW("Stopping trace since selected storage media is full");
            }
        }

        if (event_get_state(&evac_info.flush_evt) == EVAC_FLUSHING) {
            pthread_mutex_lock(&lock_trace_buffer_size);
            trace_buffer_size = 0;
            pthread_mutex_unlock(&lock_trace_buffer_size);

            for (i = 0; i < evac_info.nb_fifo; i++) {
                evac_info.fifo[i].missed = 0;
            }
        }

        EXTRADEBUG("pthread_join end (evac_handler)...");
        event_notify(&evac_info.flush_evt, EVAC_FLUSHED);

    }
}


/**
 * \fn void evac_flush(void)
 * \brief
 *
 * \param
 * \return
 */
void evac_flush(void)
{
    int i;

    event_notify(&evac_info.trace_evt, EVAC_STOP);

    event_notify(&evac_info.flush_evt, EVAC_FLUSHING);

    EXTRADEBUG("evac_flush...");

    for (i = 0; i < evac_info.nb_fifo; i++) {
        sem_post(&(evac_info.fifo[i].data_ready));
    }

}


static inline int mal_sk_req_send(char *msg, int len)
{
    return dgram_sk_send(mal_dgram_sk[MAL_SK_CMD_REQ].conn, msg, len);
}

static inline int mal_sk_rsp_recv(char *msg)
{
    return dgram_sk_recv_timeout(mal_dgram_sk[MAL_SK_CMD_RSP].conn, msg, 4);
}

static inline int mal_sk_ind_recv(char *msg)
{
    return dgram_sk_recv(mal_dgram_sk[MAL_SK_IND].conn, msg);
}

static inline int mal_sk_ind_inv_recv(char *msg)
{
    return dgram_sk_recv(mal_dgram_sk[MAL_SK_IND_INV].conn, msg);
}

/**
 * \fn int overwrite_trace_autoconf(char **command)
 *
 * \brief  Overwrite trace_auto.conf file with file in input path.
 *
 * \param  command The command-line argument provided to the command
 * \return 0 on Success, -1 on Failure
 */
int overwrite_trace_autoconf(char **command)
{
    char source_path[MAX_PATH_LEN];
    char cmd[CMD_LINE_LENGTH];
    struct stat path_stat;
    int ret = 0;

    strncpy(source_path, command[2], MAX_PATH_LEN);
    source_path[MAX_PATH_LEN - 1] = '\0';

    if (!stat(source_path, &path_stat) && !S_ISDIR(path_stat.st_mode)) {
        // Execute system command to overwrite trace_auto.conf with new input file
        memset(cmd, 0, CMD_LINE_LENGTH);
        snprintf(cmd, CMD_LINE_LENGTH, "cat %s >  %s", source_path, AUTOCONF_FILENAME);
        ret = system(cmd);
    } else {
        ALOGW("Source file %s does not exist", source_path);
        ret =  -1;
    }

    return ret;
}

/**
 * \fn int trace_activate(int timestamp, uint8_t entityID, \
 * uint32_t bitmap)
 * \brief
 *
 * \param command The command line argument passed
 * \return
 */
int trace_activate(char **command, int prog, uint8_t bitmap_count)
{
    int n, i;
    int timestamp;
    int offset = 0;
    uint32_t *mon_bitmap_data = NULL;
    tLIB_Packet_Rsp *packet_rsp = NULL;
    tLIB_Packet_Req packet_req;
    uint8_t trace_type;
    uint8_t entityID;

    timestamp = atoi(command[2]);

    if (prog == _TRACE) {
        offset = 1;
        EXTRADEBUG("enable trace: ");

        if (!strncmp(command[3], "OS", 2)) {
            trace_type = MAL_MON_OS_TRACES;
        } else if (!strncmp(command[3], "MASTER", 6)) {
            trace_type = MAL_MON_MASTER_TRACES;
        } else if (!strncmp(command[3], "VENDOR", 6)) {
            trace_type = MAL_MON_VENDOR_DLL_TRACES;
        } else if (!strncmp(command[3], "MCU", 3)) {
            trace_type = MAL_MON_MCU_TRACES;
        } else if (!strncmp(command[3], "INVARIANT", 9)) {
            trace_type = MAL_MON_INVARIANT_TRACES;
        } else {
            return -1;
        }

    } else if (prog == _INV_TRACE) {
        trace_type = MAL_MON_INVARIANT_TRACES;
        EXTRADEBUG("enable invariant trace: ");
    } else {
        return -1;
    }

    entityID = (uint8_t)strtol(command[3 + offset], NULL, 16);

    EXTRADEBUG("timestamp=%d, entityID = 0x%02x", timestamp, entityID);
    EXTRADEBUG("trace_type=%d, bitmap count = 0x%02x", trace_type,
               bitmap_count);

    packet_req.id = MAL_MON_TRACE_ACTIVATE_REQ;

    if (timestamp == 0)
        packet_req.msg.act.sb_act_timestamp.time_stamp =
            MAL_MON_TRACE_NO_TIME_STAMP;
    else
        packet_req.msg.act.sb_act_timestamp.time_stamp =
            MAL_MON_TRACE_EXT_TIME_STAMP_REQUESTED;

    packet_req.msg.act.sb_act_bitmap.trace_type = trace_type;
    packet_req.msg.act.sb_act_bitmap.entity_id = entityID;
    packet_req.msg.act.sb_act_bitmap.bitmap_length = bitmap_count;

    mon_bitmap_data = (uint32_t *)malloc(bitmap_count * sizeof(uint32_t));

    if (!mon_bitmap_data) {
        ALOGE("Failed to allocate memory for bitmap data!");
        return -1;
    }

    for (i = 0; i < bitmap_count; i++) {
        mon_bitmap_data[i] = (uint32_t)strtol(command[4 + offset + i], NULL, 16);
        EXTRADEBUG("bitmap = 0x%x", mon_bitmap_data[i]);
    }

    packet_req.msg.act.sb_act_bitmap.bitmap_array = mon_bitmap_data;

    n = mal_sk_req_send((char *) & packet_req, sizeof(tLIB_Packet_Req));

    ALOGD("n = mal_sk_req_send (n=%d)", n);

    if (n <= 0) {
        free(mon_bitmap_data);
        return -1;
    }

    packet_rsp = (tLIB_Packet_Rsp *)malloc(sizeof(tLIB_Packet_Rsp));

    if (!packet_rsp) {
        ALOGE("Failed to allocate memory for response packet!");
        return -1;
    }

    n = mal_sk_rsp_recv((char *)packet_rsp);

    ALOGD("n = mal_sk_rsp_recv (n=%d)", n);
    EXTRADEBUG("packet_rsp->id= %d (%d), packet_rsp->msg.stat=%d (%d)",
               packet_rsp->id,
               MAL_MON_TRACE_ACTIVATE_RESP, packet_rsp->msg.stat,
               MAL_MON_RESPONSE_OK);

    if ((n > 0) && (packet_rsp->id == MAL_MON_TRACE_ACTIVATE_RESP) &&
            ((packet_rsp->msg.stat == MAL_MON_RESPONSE_OK) ||
             (packet_rsp->msg.stat == MAL_MON_CAUSE_PARTIAL_UPDATE))) {
        free(packet_rsp);
        return 0;
    } else {
        free(packet_rsp);
        return -1;
    }
}


/**
 * \fn int trace_deactivate(void)
 * \brief
 *
 * \param
 * \return
 */
int trace_deactivate(void)
{
    int n = 0;
    tLIB_Packet_Rsp *packet_rsp = NULL;
    tLIB_Packet_Req packet_req;

    packet_req.id = MAL_MON_TRACE_DEACTIVATE_REQ;
    packet_req.msg.deact.deactive = MAL_MON_DEACTIVATE_ISI_INV_TRACES;

    n = mal_sk_req_send((char *) & packet_req, sizeof(tLIB_Packet_Req));

    if (n <= 0) {
        return -1;
    }

    packet_rsp = (tLIB_Packet_Rsp *)malloc(sizeof(tLIB_Packet_Rsp));

    if (!packet_rsp) {
        ALOGE("Memory allocation failed during de-activation of trace!");
        return -1;
    }

    n = mal_sk_rsp_recv((char *)packet_rsp);

    if ((n > 0) && (packet_rsp->id == MAL_MON_TRACE_DEACTIVATE_RESP) &&
            (packet_rsp->msg.stat == MAL_MON_RESPONSE_OK)) {
        free(packet_rsp);
        return 0;
    } else {
        free(packet_rsp);
        return -1;
    }
}

/**
 * \fn int trace_buffer_marker(uint8_t action_name)
 * \brief Send a buffer marker ISI message to modem
 *
 * \param action name for the ISI message
 * \return
 */
int trace_buffer_marker(uint8_t action_name)
{
    int ret, i;
    time_t timestamp;
    struct tm *localTime;

    tLIB_Packet_Rsp *packet_rsp = NULL;
    tLIB_Packet_Req packet_req;

    // Get local time
    localTime = get_time_stamp();

    // POSIX way of converting calendar time to epoch.
    // Clear explanation is provided for each operation.
    // Magic number used are described below.
    // 1 Hour = 60 * 60            = 3600 seconds
    // 1 Day  = 60 * 60 * 24       = 86400 seconds
    // 1 year = 60 * 60 * 24 * 365 = 31536000 seconds
    timestamp = localTime->tm_sec + localTime->tm_min * 60 +       // Convert minute and seconds
                localTime->tm_hour * 3600 +                        // Convert hours to seconds
                localTime->tm_yday * 86400 +                       // days of years to seconds
                (localTime->tm_year - 70) * 31536000 +             // years to seconds
                ((localTime->tm_year - 69) / 4) * 86400 -          // leap year additions
                ((localTime->tm_year - 1) / 100) * 86400 +         // subtracts a day back out every 100 years starting from 2001
                ((localTime->tm_year + 299) / 400) * 86400;        // adds a day back in every 400 years starting from 2001

    ALOGI("Local Epoch : %ju", (uintmax_t)timestamp);

    // Create the ISI message
    packet_req.id = MAL_MON_TRACE_BUFFER_MARKER_REQ;
    packet_req.msg.marker.action = action_name;

    for (i = 7; i >= 0; i--) {
        packet_req.msg.marker.timestamp[i] = (timestamp >> i * 8) & 0xFF;
    }

    ALOGI("Send ISI Buffer marker request to MAL");
    EXTRADEBUG("Set buffer marker timestamp: %ld", timestamp);
    EXTRADEBUG("Set buffer marker action: %02x", (0xff & action_name));

    // Send the ISI message to MAL
    ret = mal_sk_req_send((char *) & packet_req, sizeof(tLIB_Packet_Req));

    if (ret <= 0) {
        ALOGE("MAL not connected to buffer marker");
        return -1;
    }

    ALOGI("Send ISI buffer marker to MAL successful");


    packet_rsp = (tLIB_Packet_Rsp *)malloc(sizeof(tLIB_Packet_Rsp));

    if (!packet_rsp) {
        ALOGE("Memory allocation failed during buffer marker!");
        return -1;
    }

    // Wait for the response for BUFFER MARKER message and validate
    ret = mal_sk_rsp_recv((char *)packet_rsp);

    if ((ret > 0) && (packet_rsp->id == MAL_MON_TRACE_BUFFER_MARKER_RESP) &&
            (packet_rsp->msg.stat == MAL_MON_RESPONSE_OK)) {
        ALOGI("MAL Buffer Marker response OK");
        free(packet_rsp);
        return 0;
    } else {
        ALOGI("MAL Buffer Marker response KO");
        free(packet_rsp);
        return -1;
    }
}

/**
 * \fn int trace_get_activation_status(int entity)
 * \brief
 *
 * \param
 * \return
 */
int trace_get_activation_status(int fd, char **command, int trace_count)
{
    int n, i, j;
    bitmap_t *bitmap = NULL;
    tLIB_Packet_Rsp *packet_rsp = NULL;
    char resp[MAX_CONFIG_RESP_LEN];
    tLIB_Packet_Req packet_req;

    if (fd < 0) {
        return 0;
    }

    EXTRADEBUG("Get activation status");
    packet_req.id = MAL_MON_TRACE_ACTIVATION_READ_STATUS_REQ;
    packet_req.msg.read.nb_bitmap = (uint8_t)trace_count;

    bitmap = (bitmap_t *)malloc(packet_req.msg.read.nb_bitmap *
                                sizeof(bitmap_t));

    if (!bitmap) {
        ALOGE("Memory allocation (bitmap) failed during get activation status!");
        return -1;
    }

    for (i = 0; i < trace_count; i++) {
        bitmap[i].trace_type = MAL_MON_INVARIANT_TRACES;

        if (strncmp(command[2 + i], "0x", 2)) {
            n = my_write(fd, "KO\n", strlen("KO\n"));
            ALOGE("Parameter %d:%s wrong", 2 + i, command[2 + i]);
            free(bitmap);
            return -1;
        }

        bitmap[i].entity_id = (uint8_t)strtol(command[2 + i], NULL, 16);
    }

    packet_req.msg.read.array_bitmap = bitmap;

    n = mal_sk_req_send((char *) & packet_req, sizeof(tLIB_Packet_Req));

    if (n <= 0) {
        n = my_write(fd, "KO\n", strlen("KO\n"));
        ALOGE("Failed to send MAL packet!");
        free(bitmap);
        return -1;
    }

    packet_rsp = (tLIB_Packet_Rsp *)malloc(sizeof(tLIB_Packet_Rsp));

    if (!packet_rsp) {
        ALOGE("Memory allocation (packet_rsp) failed during get activation status!");
        return -1;
    }

    n = mal_sk_rsp_recv((char *)packet_rsp);

    if (n <= 0) {
        ALOGE("Failed to receive MAL packet!");
        goto err;
    }

    if (packet_rsp->id != MAL_MON_TRACE_ACTIVATION_READ_STATUS_RESP) {
        ALOGE("Incorrect response id to activation status request!");
        goto err;
    }

    if (packet_rsp->msg.read.status != MAL_MON_RESPONSE_OK) {
        ALOGE("Incorrect response to activation status request!");
        goto err;
    }

    memset(resp, '\0', MAX_CONFIG_RESP_LEN);

    for (i = 0; i < packet_rsp->msg.read.nb_subblocks; i++) {
        sprintf(resp + strlen(resp), "0x%x 0x%x ",
                packet_rsp->msg.read.array_act[i].trace_type,
                packet_rsp->msg.read.array_act[i].entity_id);

        for (j = 0; j < packet_rsp->msg.read.array_act[i].bitmap_length; j++) {
            sprintf(resp + strlen(resp), "0x%x ",
                    packet_rsp->msg.read.array_act[i].bitmap_array[j]);
        }
    }

    resp[strlen(resp)] = '\n';
    n = my_write(fd, resp, strlen(resp));

    for (i = 0; i < packet_rsp->msg.read.nb_subblocks; i++) {
        free(packet_rsp->msg.read.array_act[i].bitmap_array);
    }

    free(packet_rsp->msg.read.array_act);

    free(packet_rsp);
    return 0;

err:
    my_write(fd, "KO\n", strlen("KO\n"));
    free(packet_rsp);
    return -1;
}


/**
 * \fn int trace_set_config(char *trace_name)
 * \brief
 *
 * \param
 * \return
 */
int trace_set_config(char *trace_name)
{
    int n;
    tLIB_Packet_Rsp *packet_rsp = NULL;
    tLIB_Packet_Req packet_req;
    ALOGI("Set configuration trace_name: %s", trace_name);

    packet_req.id = MAL_MON_TRACE_CONFIG_SET_REQ;
    packet_req.msg.set.ascii = (char *)calloc(1, strlen(trace_name) + 1);
    strncpy(packet_req.msg.set.ascii, trace_name, strlen(trace_name));

    EXTRADEBUG("Send request to MAL");

    n = mal_sk_req_send((char *) & packet_req, sizeof(tLIB_Packet_Req));

    if (n <= 0) {
        ALOGE("MAL not connected");
        return -1;
    }

    EXTRADEBUG("WAIT RESP");

    packet_rsp = (tLIB_Packet_Rsp *)malloc(sizeof(tLIB_Packet_Rsp));

    if (!packet_rsp) {
        ALOGE("Memory allocation (packet_rsp) failed during set configuration!");
        return -1;
    }

    n = mal_sk_rsp_recv((char *)packet_rsp);

    EXTRADEBUG("Response received");

    if ((n > 0) && (packet_rsp->id == MAL_MON_TRACE_CONFIG_SET_RESP) &&
            (packet_rsp->msg.stat == MAL_MON_RESPONSE_OK)) {
        EXTRADEBUG("MAL response OK");
        free(packet_rsp);
        return 0;
    } else {
        EXTRADEBUG("MAL response KO");
        free(packet_rsp);
        return -1;
    }
}


/**
 * \fn int trace_get_config(int fd)
 * \brief
 *
 * \param
 * \return
 */
int trace_get_config(int fd)
{
    int n;
    tLIB_Packet_Rsp *packet_rsp = NULL;
    tLIB_Packet_Req packet_req;
    char resp[MAX_CONFIG_RESP_LEN];

    if (fd < 0) {
        return 0;
    }

    ALOGI("Get configuration");
    packet_req.id = MAL_MON_TRACE_CONFIG_GET_REQ;

    n = mal_sk_req_send((char *) & packet_req, sizeof(tLIB_Packet_Req));

    if (n <= 0) {
        n = my_write(fd, "KO\n", strlen("KO\n"));
        return -1;
    }

    packet_rsp = (tLIB_Packet_Rsp *)malloc(sizeof(tLIB_Packet_Rsp));

    if (!packet_rsp) {
        ALOGE("Memory allocation (packet_rsp) failed during get configuration!");
        return -1;
    }

    n = mal_sk_rsp_recv((char *)packet_rsp);

    if ((n > 0) && (packet_rsp->id == MAL_MON_TRACE_CONFIG_GET_RESP)) {
        /* Send Config */
        memset(resp, '\0', MAX_CONFIG_RESP_LEN);
        strncpy(resp, packet_rsp->msg.get.cur_ascii,
                strlen(packet_rsp->msg.get.cur_ascii));

        resp[strlen(resp)] = '\n';
        ALOGI("Current config: %s", resp);
        n = my_write(fd, resp, strlen(resp));

        free(packet_rsp->msg.get.cur_ascii);
        free(packet_rsp);
        return 1;
    } else {
        n = my_write(fd, "KO\n", strlen("KO\n"));
        free(packet_rsp);
        return -1;
    }
}

/**
 * \fn uint8_t trace_get_buffering_mode()
 * \brief writes which buffering mode is set
 * \return MAL_MON_FLUSHING_MODE or MAL_MON_SDRAM_ONLY_MODE or -1 on error
 * \n
 */
uint8_t trace_get_buffering_mode(void)
{
    int ret = 0;
    tLIB_Packet_Rsp *packet_rsp = NULL;
    tLIB_Packet_Req packet_req;
    uint8_t buffering_mode_resp;

    // Create the ISI message
    packet_req.id = MAL_MON_TRACE_BUFFERING_MODE_READ_REQ;

    ALOGI("Send ISI read buffering mode request to MAL");

    // Send the ISI message to MAL
    ret = mal_sk_req_send((char *) &packet_req, sizeof(tLIB_Packet_Req));

    if (ret <= 0) {
        ALOGE("Error trying to send read buffering mode request ISI message");
        return -1;
    }

    ALOGI("Send ISI read buffering mode request to MAL successful");

    packet_rsp = (tLIB_Packet_Rsp *) malloc(sizeof(tLIB_Packet_Rsp));

    if (!packet_rsp) {
        ALOGE("Memory allocation failed during read buffering mode!");
        return -1;
    }

    // Wait for the response for BUFFERING MODE READ message and validate
    ret = mal_sk_rsp_recv((char *) packet_rsp);

    if ((ret > 0) && (packet_rsp->id == MAL_MON_TRACE_BUFFERING_MODE_READ_RESP)) {
        /* Receive buffering mode and return it */
        buffering_mode_resp = packet_rsp->msg.buffering_mode_get.mode;

        ALOGI("MAL Read buffering mode OK");
        free(packet_rsp);
        return buffering_mode_resp;
    } else {
        ALOGI("MAL Read buffering mode response KO");
        free(packet_rsp);
        return -1;
    }
}

/**
 * \fn int trace_get_status
 * \brief Get status of current trace, such as
 *  if SDCARD-tracing is active, the size of
 *  medium and/or long_buffer (set by trace --rol_file command)
 *  etc
 *
 * \param file descriptor of the tool for replies
 * \param command: the parsed command from the tool
 * \return -1 on error, 1 if ok
 */
int trace_get_status(int tool_fd, char **command, int count)
{
    int l_rol_size, m_rol_size;
    char response[CN_MAX_STRING_BUFF];
    uint8_t buffering_mode;

    if (tool_fd < 0) {
        return -1;
    }

    ALOGI("Trace get status: %s", command[2]);

    m_rol_size = evac_info.rol_ff.max_size[MLR_MEDIUM_BUF_TYPE];
    l_rol_size = evac_info.rol_ff.max_size[MLR_LONG_BUF_TYPE];

    if (!strcmp(command[2], "SDCARD")) {
        //Return true if log to SD-card is active, false otherwise

        //Check if destination set to SDCARD and that
        //trace -t BUFFER has been called.
        if (evac_info.type == DEST_SDCARD && evac_info.trace_route_type
                == BUFFER && event_get_state(&evac_info.sdcard_evt) == SD_MOUNTED) {
            sprintf(response, "%s", "true");
        } else {
            sprintf(response, "%s", "false");
        }

    } else if (!strcmp(command[2], "LEVEL")) {
        //Return "L", "M" or "LM" depending on the size that
        //has been set with trace --rol_file command.
        //(For instance, if medium_buf_size set to 0, return L!)

        if (m_rol_size && l_rol_size) {
            sprintf(response, "%s", "LM");
        } else if (m_rol_size > 0) {
            sprintf(response, "%s", "M");
        } else if (l_rol_size > 0) {
            sprintf(response, "%s", "L");
        } else {
            sprintf(response, "%s", "Not set");
        }

    } else if (!strcmp(command[2], "SIZE") && count >= 4) {

        if (!strcmp(command[3], "M")) {
            //Return the medium_buf_size set from trace --rol_file command
            sprintf(response, "%d", m_rol_size);
        } else if (!strcmp(command[3], "L")) {
            //Return the long_buf_size set from trace --rol_file command
            sprintf(response, "%d", l_rol_size);
        } else {
            goto KO;
        }

    } else if (!strcmp(command[2], "ROUTE")) {
        //Return the current trace route setting
        //(what has been set with trace -t command)
        switch (evac_info.trace_route_type) {
        case OFF:
            sprintf(response, "%s", "OFF");
            break;
        case STP:
            sprintf(response, "%s", "STP");
            break;
        case BUFFER:
            sprintf(response, "%s", "BUFFER");
            break;
        case BOTH:
            sprintf(response, "%s", "BOTH");
            break;
        default:
            sprintf(response, "%s", "Not set");
            break;
        }
    } else if (!strncmp(command[2], "MODEM_VERSION", 13)) {
        // Return the modem version
        EXTRADEBUG("Modem software version ");

        if (!strncmp(modem_version_info, "NA", 2)) {
            // Return KO, if no modem version information available
            snprintf(response, CN_MAX_STRING_BUFF, "%s", "KO\n");
        } else {
            snprintf(response, CN_MAX_STRING_BUFF, "%s%s", MODEM_VERSION_PREFIX_STR, modem_version_info);
        }
    } else if (!strncmp(command[2], "BUFFERING_MODE", 14)) {
        EXTRADEBUG("Modem buffering mode ");
        buffering_mode = trace_get_buffering_mode();

        switch (buffering_mode) {
        case MAL_MON_FLUSHING_MODE:
            sprintf(response, "%s", "FLUSHING");
            break;
        case MAL_MON_SDRAM_ONLY_MODE:
            sprintf(response, "%s", "SDRAM_ONLY");
            break;
        default:
            sprintf(response, "%s", "Read error");
            break;
        }
    } else {
KO:
        //Probably something wrong with input options, return KO
        my_write(tool_fd, "KO\n", strlen("KO\n"));
        return -1;
    }

    snprintf(response + strlen(response), sizeof(response) - strlen(response),
             "\n");
    return my_write(tool_fd, response, strlen(response));
}



/**
 * \fn int trace_route(void)
 * \brief
 *
 * \param
 * \return
 */
int trace_route(char *routing)
{
    int n, trace_route_type;
    tLIB_Packet_Rsp *packet_rsp = NULL;
    tLIB_Packet_Req packet_req;

    ALOGI("Trace route, routing: %s", routing);

    packet_req.id = MAL_MON_TRACE_ROUTING_REQ;

    if (!strncmp(routing, "OFF", 3)) {
        packet_req.msg.routing.routing = MAL_MON_TRACE_ROUTING_NO_TRACE;
        trace_route_type = OFF;
    } else if (!strncmp(routing, "STP", 3)) {
        packet_req.msg.routing.routing = MAL_MON_TRACE_ROUTING_XTI;
        trace_route_type = STP;
    } else if (!strncmp(routing, "BUFFER", 6)) {
        packet_req.msg.routing.routing = MAL_MON_TRACE_ROUTING_BUFFER;
        trace_route_type = BUFFER;
    } else if (!strncmp(routing, "BOTH", 4)) {
        packet_req.msg.routing.routing = MAL_MON_TRACE_ROUTING_XTI_BUFFER;
        trace_route_type = BOTH;
    } else {
        return -1;
    }

    n = mal_sk_req_send((char *) & packet_req, sizeof(tLIB_Packet_Req));

    if (n <= 0) {
        if (n < 0) {
            ALOGE("Error during trace_route");
        }

        return -1;
    }

    packet_rsp = (tLIB_Packet_Rsp *)malloc(sizeof(tLIB_Packet_Rsp));

    if (!packet_rsp) {
        ALOGE("Memory allocation (packet_rsp) failed during trace route!");
        return -1;
    }

    n = mal_sk_rsp_recv((char *)packet_rsp);

    if ((n > 0) && (packet_rsp->id == MAL_MON_TRACE_ROUTING_RESP)
            && (packet_rsp->msg.stat == MAL_MON_RESPONSE_OK)) {
        ALOGI("Successfully routed to : %s", routing);
        evac_info.trace_route_type = trace_route_type;
        free(packet_rsp);
    } else {
        free(packet_rsp);
        return -1;
    }

    // After enabling the trace to BUFFER/BOTH we will send a start trace
    // We don't need to check the return value.
    if ((strncmp(routing, "BUFFER", 6) == 0) || (strncmp(routing, "BOTH", 4) == 0)) {
        // Send a ISI message to modem for buffer marker
        // with action as a MARKER_START. It will indicate a
        // trace start in the trace log MHMM MSG:/RECV; isi:
        trace_buffer_marker(MAL_MON_MARKER_START);
    }

    return 0;
}


/**
 * @brief
 *
 * @param source
 * MODEM_TRIGGER, or HOST_TRIGGER
 *
 * @return
 */
int trace_trigger(int source)
{
    int ret;
    struct stat path_stat;
    char filename[64];
    char path[NAME_LENGTH];

    snprintf(path, NAME_LENGTH, "%s", trigger_directory);
    snprintf(filename, 64, "%s_", trigger_type[source]);

    ALOGI("Trace trigger, %s initiated Trigger successfully \n", trigger_type[source]);

    if (evac_info.type == DEST_USB && event_get_state(&evac_info.reportlink_evt) == DLINK_CONNECTED) {
        event_wait_set(&evac_info.trigger_evt, EVAC_UNLOCKED, EVAC_LOCKED);
        // Send ISI message to modem to activate a
        // buffer marker with action as MARKER_END
        trace_buffer_marker(MAL_MON_MARKER_END);
        ret = report_generation(sk_serv[SK_SERV_RPT].fd, path, NULL, filename, modem_version_info);
    } else {
        if (!(!stat(path, &path_stat) && S_ISDIR(path_stat.st_mode))) {
            /* Path does not exists, so create it */
            if (Create_dir(path)) {
                ALOGE("Cannot create trace folder");
                return -1;
            }
        }

        event_wait_set(&evac_info.trigger_evt, EVAC_UNLOCKED, EVAC_LOCKED);
        // Send ISI message to modem to activate a
        // buffer marker with action as MARKER_END
        trace_buffer_marker(MAL_MON_MARKER_END);

        if (evac_info.type == DEST_USB || evac_info.type == DEST_UNKNOWN) {
            ret = report_generation(0, path, NULL, filename, modem_version_info);
        } else {
            ret = report_generation(0, path, evac_info.filename, filename, modem_version_info);
        }
    }

    if (ret < 0)
        ALOGE("%s initiated Trigger failed : ret = %d\n",
              trigger_type[source], ret);

    if (source != STOP_TRIGGER) {
        // Send ISI message to modem to activate a
        // buffer marker with action as MARKER_START
        trace_buffer_marker(MAL_MON_MARKER_START);

        if (evac_info.type != DEST_USB && evac_info.type != DEST_UNKNOWN) {

            /*  call function with IMEI already initialized , previous files are closed
             *  and new file are created */
            evac_create_file(&evac_info);
        }
    }

    event_notify(&evac_info.trigger_evt, EVAC_UNLOCKED);
    ALOGI("%s Trigger done\n", trigger_type[source]);
    return ret;
}


/**
 * \fn int trace_ids_activate_route(char **command, int trace_count)
 * \brief
 *
 * \param
 * \return
 */
int trace_ids_activate_route(char **command, int trace_count)
{
    int n, i;
    int offset;
    uint16_t *trace_id;
    tLIB_Packet_Rsp *packet_rsp = NULL;
    tLIB_Packet_Req packet_req;

    packet_req.id = MAL_MON_TRACE_IDS_ACTIVATE_ROUTE_REQ;

    if ((!strncmp(command[0], "invariant_trace", 15)) &&
            ((!strncmp(command[1], "-d", 2)) ||
             (!strncmp(command[1], "--deactivate_invariant_trace", 28)))) {
        /* invariant_trace -d command */
        packet_req.msg.route.processor = MAL_MON_TRACE_PROCESSOR_L23;
        packet_req.msg.route.trace_type = MAL_MON_INVARIANT_TRACES;
        packet_req.msg.route.bitmap_entity =
            (uint32_t)strtol(command[2], NULL, 16);
        packet_req.msg.route.action = MAL_MON_TRACE_OFF;
        offset = 0;

        goto fill_bitmap;
    }

    offset = 3;

    if (!strncmp(command[2], "L1", 2)) {
        packet_req.msg.route.processor = MAL_MON_TRACE_PROCESSOR_L1;
    } else if (!strncmp(command[2], "L23", 3)) {
        packet_req.msg.route.processor = MAL_MON_TRACE_PROCESSOR_L23;
    } else {
        return -1;
    }

    if (!strncmp(command[3], "OS", 2)) {
        packet_req.msg.route.trace_type = MAL_MON_OS_TRACES;
    } else if (!strncmp(command[3], "MASTER", 6)) {
        packet_req.msg.route.trace_type = MAL_MON_MASTER_TRACES;
    } else if (!strncmp(command[3], "VENDOR", 6)) {
        packet_req.msg.route.trace_type = MAL_MON_VENDOR_DLL_TRACES;
    } else if (!strncmp(command[3], "MCU", 3)) {
        packet_req.msg.route.trace_type = MAL_MON_MCU_TRACES;
    } else if (!strncmp(command[3], "INVARIANT", 9)) {
        packet_req.msg.route.trace_type = MAL_MON_INVARIANT_TRACES;
    } else {
        return -1;
    }

    packet_req.msg.route.bitmap_entity =
        (uint32_t)strtol(command[4], NULL, 16);

    if (!strncmp(command[5], "OFF", 3)) {
        packet_req.msg.route.action = MAL_MON_TRACE_OFF;
    } else if (!strncmp(command[5], "ON", 2)) {
        packet_req.msg.route.action = MAL_MON_TRACE_ON;
    } else if (!strncmp(command[5], "SHORT", 5)) {
        packet_req.msg.route.action = MAL_MON_TRACE_SHORT;
    } else if (!strncmp(command[5], "EXTENDED", 8)) {
        packet_req.msg.route.action = MAL_MON_TRACE_EXTENDED;
    } else if (!strncmp(command[5], "BUFFER_SHORT", 12)) {
        packet_req.msg.route.action = MAL_MON_TRACE_BUFFER_SHORT;
    } else if (!strncmp(command[5], "BUFFER_MEDIUM", 13)) {
        packet_req.msg.route.action = MAL_MON_TRACE_BUFFER_MEDIUM;
    } else if (!strncmp(command[5], "BUFFER_LONG", 11)) {
        packet_req.msg.route.action = MAL_MON_TRACE_BUFFER_LONG;
    } else {
        return -1;
    }

fill_bitmap:
    packet_req.msg.route.trace_count = trace_count;

    trace_id = (uint16_t *)malloc(trace_count * sizeof(uint16_t));

    if (!trace_id) {
        ALOGE("Memory allocation (trace_id) failed during ids activation route!");
        return -1;
    }

    for (i = 0; i < trace_count; i++) {
        trace_id[i] = (uint16_t)strtol(command[3 + offset + i], NULL, 16);
    }

    packet_req.msg.route.trace_id = trace_id;

    n = mal_sk_req_send((char *) & packet_req, sizeof(tLIB_Packet_Req));

    if (n <= 0) {
        free(trace_id);
        return -1;
    }

    packet_rsp = (tLIB_Packet_Rsp *)malloc(sizeof(tLIB_Packet_Rsp));

    if (!packet_rsp) {
        ALOGE("Memory allocation (packet_rsp) failed during ids activation route!");
        return -1;
    }

    n = mal_sk_rsp_recv((char *)packet_rsp);

    if ((n > 0) && (packet_rsp->id == MAL_MON_TRACE_IDS_ACTIVATE_ROUTE_RESP) &&
            (packet_rsp->msg.stat == MAL_MON_RESPONSE_OK)) {
        free(packet_rsp);
        return 0;
    } else {
        free(packet_rsp);
        return -1;
    }
}

/**
 * \fn int trace_set_buffering_mode(uint8_t mode)
 * \brief Send a set buffering mode ISI message to modem
 *
 * \param mode which buffering mode to set
 * \return -1 on error, 0 if ok
 */
int trace_set_buffering_mode(uint8_t mode)
{
    int ret = 0;
    tLIB_Packet_Rsp *packet_rsp = NULL;
    tLIB_Packet_Req packet_req;

    //Check whether the mode we're trying to switch to is already set
    if (trace_get_buffering_mode() == mode) {
        ALOGI("MAL Buffering mode already set, nothing to do.");
        return 0;
    }

    //First set trace routing to STP (send MAL_MON_TRACE_ROUTE_REQ set to XTI)
    trace_route("STP");

    // Create the ISI message
    packet_req.id = MAL_MON_TRACE_BUFFERING_MODE_REQ;
    packet_req.msg.buffering_mode_set.mode = mode;

    switch (mode) {
    case MAL_MON_FLUSHING_MODE:
        ALOGI("Send ISI Buffering mode = flushing request to MAL");
        break;
    case MAL_MON_SDRAM_ONLY_MODE:
        ALOGI("Send ISI Buffering mode = sdram_only request to MAL");
        break;
    default:
        ALOGE("Trying to create ISI Buffering mode request, but buffering mode is faulty.");
        return -1;
        break;
    }

    // Send the ISI message to MAL
    ret = mal_sk_req_send((char *) &packet_req, sizeof(tLIB_Packet_Req));

    if (ret <= 0) {
        ALOGE("Error trying to send buffering mode request ISI message");
        return -1;
    }

    ALOGI("Send ISI buffering mode request to MAL successful");

    packet_rsp = (tLIB_Packet_Rsp *) malloc(sizeof(tLIB_Packet_Rsp));

    if (!packet_rsp) {
        ALOGE("Memory allocation failed during buffering mode!");
        return -1;
    }

    // Wait for the response for BUFFERING MODE message and validate
    ret = mal_sk_rsp_recv((char *) packet_rsp);

    if ((ret > 0) && (packet_rsp->id == MAL_MON_TRACE_BUFFERING_MODE_RESP)
            && (packet_rsp->msg.stat == MAL_MON_RESPONSE_OK)) {
        ALOGI("MAL Buffering mode response OK");
        free(packet_rsp);
    } else {
        ALOGI("MAL Buffering mode response KO");
        free(packet_rsp);
        return -1;
    }

    //When done, set trace routing to BUFFER; send MAL_MON_TRACE_ROUTE_REQ set to BUFFER
    trace_route("BUFFER");
    return 0;
}

/**
 * \fn int command_set_dest(char **command, int prog)
 * \brief Set the traces destination (USB, FS, SDCARD)
 *
 * \param command: the parsed command from the tool
 * \param the program type
 * \return -1 on error, 1 if ok
 */
int command_set_dest(char **command, int prog)
{
    int type = DEST_UNKNOWN;
    int nb_fifo = 0;

    EXTRADEBUG("Set destination");

    if ((prog == _TRIGGER) && strlen(command[2])) {
        memset(trigger_directory, 0x00, NAME_LENGTH);
        snprintf(trigger_directory, NAME_LENGTH, "%s", command[2]);
        return 0;
    }

    if (!strncasecmp(command[2], "USB", 3)) {
        type = DEST_USB;
        nb_fifo = 1;
    } else if (!strncasecmp(command[2], "fs", 2)) {
        type = DEST_FS;
        nb_fifo = MLR_MAX_EVAC_FILE;
    } else if (!strncasecmp(command[2], "sdcard", 6)) {
        type = DEST_SDCARD;
        nb_fifo = MLR_MAX_EVAC_FILE;
    } else {
        return -1;
    }

    ALOGI("Set destination type: %d", type);

    if (prog == _TRACE) {
        trace_set_buffering_mode(MAL_MON_FLUSHING_MODE);

        // Register for flush indications only when destination is set
        mal_mon_sub();

        if (evac_info.type == DEST_UNKNOWN) {
            /* Not yet configured */
            evac_info.type = type;
            evac_info.nb_fifo = nb_fifo;

            if ((type == DEST_FS) || (type == DEST_SDCARD)) {
                memset(evac_info.dest_path, '\0', MAX_PATH_LEN);
                strncpy(evac_info.dest_path, command[3],
                        strlen(command[3]));
            }
        } else {
            /* Already configured: a reconfiguration could be needed
             * with a stop and restart of the traces */

            if (event_get_state(&evac_info.trace_evt) == EVAC_START ||
                    event_get_state(&evac_info.trace_evt) == EVAC_FLUSHING) {
                evac_flush();
                EXTRADEBUG("Wait END FLUSH");
                event_wait(&evac_info.flush_evt, EVAC_FLUSHED);
            }

            evac_info.type = type;
            evac_info.nb_fifo = nb_fifo;

            if ((type == DEST_FS) || (type == DEST_SDCARD)) {
                memset(evac_info.dest_path, '\0', MAX_PATH_LEN);
                strncpy(evac_info.dest_path, command[3],
                        strlen(command[3]));
            }
        }

    } else if (prog == _DUMP) {
        evac_info.dump_type = type;

        if ((type == DEST_FS) || (type == DEST_SDCARD)) {
            strncpy(evac_info.dump_path, command[3], strlen(command[3]));
        } else if (type == DEST_USB) {
            memset(evac_info.dump_path, '\0', MAX_PATH_LEN);
            strncpy(evac_info.dump_path, PREFIX_COREDUMP_USB, strlen(PREFIX_COREDUMP_USB));
        }
    }

    return 0;
}

/**
 * \fn int command_set_rol_file(char **command, int count)
 * \brief
 *
 * \param
 * \return -1 on error, 0 on success

 */
int command_set_rol_file(char **command, int count)
{
    int n = -1;
    int m_rol_size, l_rol_size;

    if (count > 2) {
        if (!strncmp(command[2], "OFF", 3)) {
            evac_info.rol_ff.enabled = 0;
            n = 0;
        } else if ((!strncmp(command[2], "ON", 2)) && (count == 5)) {
            evac_info.rol_ff.enabled = 1;
            n = 0;
        }
    }

    if (count == 5) {
        m_rol_size = atoi(command[3]) * 1024 * 1024;
        l_rol_size = atoi(command[4]) * 1024 * 1024;
        evac_info.rol_ff.max_size[MLR_MEDIUM_BUF_TYPE] = m_rol_size;
        evac_info.rol_ff.max_size[MLR_LONG_BUF_TYPE] = l_rol_size;

        EXTRADEBUG("m = %d l =%d", m_rol_size, l_rol_size);
    }

    return n;
}

/**
 * \fn int command_set_dump_files(char **command, int count)
 * \brief Set number of core dumps to keep. 0=unlimited
 *
 * \param
 * \return -1 on error, 0 on success
 */
int command_set_dump_files(char **command, int count)
{
    int n = -1;

    if (count > 2) {
        if (atoi(command[2]) >= -1) {
            dump_files = atoi(command[2]);
            n = 0;
            ALOGI("Max number of saved dumps files changed to %d (0 = unlimited)", dump_files);
        } else {
            ALOGE("Command only accept values larger or equal to -1 (-1 disables core dump generation)");
        }
    }

    return n;
}

/**
 * \fn int set_max_trigger_report(char **command, int count)
 * \brief Set number of trigger report to keep. 0=unlimited
 *
 * \param
 * \return -1 on error, 0 on success
 */
int set_max_trigger_report(char **command, int count)
{
    int n = -1;

    if ((count > 2) && atoi(command[2]) >= 0) {
        trigger_files = atoi(command[2]);
        n = 0;
        ALOGI("Maximum number of trigger reports set to %d (0 = unlimited)", trigger_files);
    } else {
        ALOGE("Invalid arguments, 'trace -l, --trigger_files=n', (n >= 0, 0 = unlimited)");
    }

    return n;
}

/**
 * \fn int command_set_trace_history(char **command, int count)
 * \brief Set number to keep old traces. 0=unlimited
 *
 * \param
 * \return -1 on error, 0 on success
 */
int command_set_trace_history(char **command, int count)
{
    int n = -1;

    if (count > 2) {
        if (atoi(command[2]) >= 0) {
            trace_history = atoi(command[2]);
            ALOGI("Number of days to keep old traces changed to %ld (0 = keep all files)", trace_history);
            n = 0;
        } else {
            ALOGE("Command only accept values larger or equal to zero");
        }
    }

    return n;
}


/**
 * \fn int command_send_imei(int fd)
 * \brief Retrieve and send IMEI to trace tool
 *
 * \param file descriptor of the tool for replies
 * \return -1 on error, 1 on success

 */
int command_send_imei(int fd)
{
    int err;
    char imei[20];

    if (fd < 0) {
        return 0;
    }

    err = read_imei(&imei[0], sizeof(imei));

    if (err) {
        my_write(fd, "KO\n", strlen("KO\n"));
        return -1;
    }

    snprintf(imei + strlen(imei), sizeof(imei) - strlen(imei), "\n");
    return my_write(fd, imei, strlen(imei));
}


/**
 * \fn int command_send_bb_id(int fd)
 * \brief Retrieve and send Baseband Unique ID to trace tool
 *
 * \param file descriptor of the tool for replies
 * \return -1 on error, 1 on success

 */
int command_send_bb_id(int fd)
{
    int err = 0, fd_uid;
    char bb_id[50];

    if (fd < 0) {
        return 0;
    }

    fd_uid = open(BASEBAND_UID_PATH, O_RDONLY);

    if (fd_uid < 0) {
        err = -1;
        goto reply;
    }

    err = read(fd_uid, bb_id, 50);

reply:

    if (err < 0) {
        my_write(fd, "KO\n", strlen("KO\n"));
        return -1;
    }

    return my_write(fd, bb_id, err);
}

/**
 * int get_modem_version()
 *
 * Fetch the modem software version by sending
 * a ISI message to modem
 *
 * Return -1 on failure
 */
int get_modem_version()
{
    int ret = 0;
    int request_fd = -1;
    char *position = NULL;
    cn_uint32_t size = 0;
    cn_error_code_t result;
    cn_context_t *cn_context_p = NULL;
    cn_message_t *message_p = NULL;


    // Initiate a CN client connection
    EXTRADEBUG("Init CN Client");
    result = cn_client_init(&cn_context_p);

    if (CN_SUCCESS != result) {
        ALOGE("cn_client_init failed");
        return -1;
    }

    // Get the request fd
    EXTRADEBUG("cn_client_get_request_fd");
    result = cn_client_get_request_fd(cn_context_p, &request_fd);

    if (CN_SUCCESS != result) {
        ALOGE("cn_client_get_request_fd failed!");
        goto cleanup;
    }

    // Send ISI message to get Modem version information
    result = cn_request_baseband_version(cn_context_p, 0);

    if (CN_SUCCESS != result) {
        ALOGE("cn_request_baseband_version failed. ");
        goto cleanup;
    }

    /* Wait for CN_RESPONSE_BASEBAND_VERSION  */
    EXTRADEBUG("waiting for response.");

    while (TRUE) {
        result = cn_get_message_queue_size(request_fd, &size);

        if (CN_SUCCESS != result) {
            ALOGE("cn_get_message_queue_size failed!");
            goto cleanup;
        }

        if (size > 0) {
            /* at least one byte has been received. Ensure that all bytes are received */
            sleep(1);
            break;
        }
    }

    // Receive the response
    EXTRADEBUG("received baseband version response.");
    result = cn_message_receive(request_fd, &size, &message_p);

    if (CN_SUCCESS != result) {
        ALOGE("cn_message_receive failed!");
        goto cleanup;
    }

    // Check for correct response message
    if (CN_RESPONSE_BASEBAND_VERSION != message_p->type) {
        ALOGE("CN_RESPONSE_BASEBAND_VERSION not received!");
        // This is one case where you don't have the modem version,
        // although result is CN_SUCCESS
        snprintf(modem_version_info, CN_MAX_STRING_BUFF, "%s", "NA");
        goto cleanup;
    }

    // Extract modem version from the response
    EXTRADEBUG("parse baseband version response.");
    snprintf(modem_version_info, CN_MAX_STRING_BUFF, "%s", (char *) message_p->payload);
    position = strchr(modem_version_info, '\n');

    if (position != NULL) {
        *position = '\0';
    }

    ALOGI("Modem Version: %s", modem_version_info);

cleanup:

    if (result != CN_SUCCESS) {
        snprintf(modem_version_info, CN_MAX_STRING_BUFF, "%s", "NA");
        ret = -1;
    }

    // Shutdown the CN client
    result = cn_client_shutdown(cn_context_p);

    if (CN_SUCCESS != result) {
        ALOGE("cn_client_shutdown failed!");
        return -1;
    }

    return ret;
}

/**
 * \fn int command_reset_report(void)
 * \brief Call the reset report (dump) of modem
 *
 * \return -1 on error, 1 on success

 */
int command_reset_report(void)
{
    int n;
    int found = 0;
    struct stat path_stat;

    if (evac_info.dump_type == DEST_USB) {
        if (event_get_state(&evac_info.reportlink_evt)) {
            ALOGD("Sending modem coredump via socket connection");
            n = coredump_generation(sk_serv[SK_SERV_RPT].fd,
                                    evac_info.dump_path, NULL);
        } else {
            ALOGE("Dump path set to USB but no listener on report port. Abort coredump generation");
            return -1;
        }
    } else {
        ALOGD("Saving modem coredump to SD-Card");

        if (!((stat(evac_info.dump_path, &path_stat) == 0)
                && S_ISDIR(path_stat.st_mode))) {
            if (evac_info.dump_type == DEST_SDCARD) {
                found = sdcard_check_mount();

                if (found != 1) {
                    ALOGW("SDCARD not mounted");
                    return -1;
                }
            }

            /* Path does not exists, so create it */
            if (Create_dir(evac_info.dump_path)) {
                return -1;
            }
        }

        if (evac_info.type == DEST_USB || evac_info.type == DEST_UNKNOWN) {
            n = coredump_generation(0, evac_info.dump_path, NULL);
        } else {
            n = coredump_generation(0, evac_info.dump_path, evac_info.filename);
        }
    }

    return n;
}

/**
 * \fn int enable_trace_auto_trigger(char **command)
 *
 * \brief  Enable the trace auto trigger mode. This mode is valid only for
 *         when trace files are being saved in non-circular buffer style.
 *         An automatic trigger report will be generated when any of the
 *         trace files reaches it's maximum size, if auto trigger option
 *         is enabled
 *
 *
 * \param  command The command-line argument provided to the command
 * \return 0 on Success, -1 on Failure
 */
int enable_trace_auto_trigger(char **command)
{
    int ret = 0;
    EXTRADEBUG("Trace enable file split for non-circular buffer save mode");

    if (!strncasecmp(command[2], "ON", 2)) {
        evac_info.is_autotrigger = 1;
        ALOGD("Trace file splitting enabled for SDCARD/FS with non-circular buffer save mode");
    } else if (!strncasecmp(command[2], "OFF", 3)) {
        evac_info.is_autotrigger = 0;
    } else {
        ALOGW("Wrong option for command, trace -u, --auto_trigger=ON/OFF");
        ret = -1;
    }

    return ret;
}


/**
 * \fn int parse_tool_cmd(char *buffer, int tool_fd)
 * \brief Analyze the command received from the trace tool
 *
 * \param buffer: the command from the tool
 * \param file descriptor of the tool for replies
 * \return 1
 */
int parse_tool_cmd(char *buffer, int tool_fd)
{
    int i;
    int n = -1;
    int count = 1;
    int nsplit = 0;
    int prog = -1;
    char *bufcopy = strdup(buffer);
    char *pch;
    char **split_command;

    split_command = (char **)malloc(50 * sizeof(char *));

    /* remove newline characters and trailing whitespaces */
    if (bufcopy[strlen(bufcopy) - 1] == '\n') {
        bufcopy[strlen(bufcopy) - 1] = '\0';
    }

    if (bufcopy[strlen(bufcopy) - 1] == '\r') {
        bufcopy[strlen(bufcopy) - 1] = '\0';
    }

    while (bufcopy[strlen(bufcopy) - 1] == ' ') {
        bufcopy[strlen(bufcopy) - 1] = '\0';
    }

    /* count number of words in command line */
    for (i = 0; bufcopy[i] != '\0'; i++) {
        if ((bufcopy[i] == ' ') || (bufcopy[i] == '=')) {
            count++;
        }
    }

    if (count < 2) {
        n = -1;
        goto Exit;
    }

    /* Split the command line */
    pch = strtok(bufcopy, " =");

    do {
        split_command[nsplit++] = strdup(pch);
        pch = strtok(NULL, " =");
    } while (pch != NULL);

    /* Choose type of command */
    if (!strncmp(split_command[0], "dump", 4)) {
        prog = _DUMP;
    }  else if (!strncmp(split_command[0], "invariant_trace", 15)) {
        prog = _INV_TRACE;
    } else if (!strncmp(split_command[0], "trace", 5)) {
        prog = _TRACE;
    } else if (!strncmp(split_command[0], "trigger", 7)) {
        prog = _TRIGGER;
    } else {
        ALOGW("Wrong Command");
        n = -1;
        goto Exit;
    }

    /* Analyze command option */
    if (split_command[1][0] != '-') {
        ALOGW("Wrong Syntax");
        n = -1;
        goto Exit;
    } else {
        switch (split_command[1][1]) {

        case '-':

            if (!strncmp(split_command[1], "--activate_dump", 15)) {
                goto activate_dump;
            } else if (!strncmp(split_command[1], "--activate_invariant_trace", 26)) {
                goto activate_trace;
            } else if (!strncmp(split_command[1], "--auto_trigger", 12)) {
                goto trace_auto_trigger;
            } else if (!strncmp(split_command[1], "--autoconf_file", 15)) {
                goto overwrite_autoconf;
            } else if (!strncmp(split_command[1], "--baseband_id", 13)) {
                goto bb_id;
            } else if (!strncmp(split_command[1], "--buffering_mode", 14)) {
                goto buffering_mode;
            } else if (!strncmp(split_command[1], "--deactivate_invariant_trace", 28)) {
                goto deactivate_invariant_trace;
            }  else if (!strncmp(split_command[1], "--disable_trace", 15)
                        || !strncmp(split_command[1], "--disable_invariant_trace", 25)
                        || !strncmp(split_command[1], "--disable_dump", 14)) {
                goto disable_trace;
            } else if (!strncmp(split_command[1], "--dump_files", 12)) {
                goto dump_files;
            } else if (!strncmp(split_command[1], "--enable_trace", 14)) {
                goto enable_trace;
            } else if (!strncmp(split_command[1], "--get_config", 12)) {
                goto get_conf;
            } else if (!strncmp(split_command[1], "--IMEI", 6)) {
                goto imei;
            } else if (!strncmp(split_command[1], "--query_status", 14)) {
                goto query_status;
            } else if (!strncmp(split_command[1], "--re_init", 9)) {
                goto config_re_init;
            } else if (!strncmp(split_command[1], "--read_status", 13)) {
                goto read_status;
            } else if (!strncmp(split_command[1], "--reset_report", 14)) {
                goto reset_report;
            } else if (!strncmp(split_command[1], "--rol_file", 10)) {
                goto rol_file;
            } else if (!strncmp(split_command[1], "--route", 7)) {
                goto route;
            } else if (!strncmp(split_command[1], "--set_destination", 17)) {
                goto set_destination;
            } else if (!strncmp(split_command[1], "--trace_history", 15)) {
                goto trace_history;
            } else if (!strncmp(split_command[1], "--trace_route", 13)) {
                goto trace_route;
            } else if (!strncmp(split_command[1], "--trigger_files", 15)) {
                goto trigger_files;
            } else if (!strncmp(split_command[1], "--trigger_report", 16)) {
                goto trigger_report;
            } else {
                ALOGW("Wrong option");
                n = -1;
                goto Exit;
            }

            break;

        case 'D':
set_destination:
            EXTRADEBUG("set_destination");

            if ((count < 3) || (count > 5) || (prog == _INV_TRACE)) {
                n = -1;
                goto Exit;
            }

            n = command_set_dest(split_command, prog);

            if ((prog != _TRIGGER) && (prog != _DUMP) && !n) {

                // Signal EVAC_START only if below is true.
                if (((evac_info.type == DEST_SDCARD) && (event_get_state(&evac_info.sdcard_evt) == SD_MOUNTED)) ||
                        ((evac_info.type == DEST_USB) && (event_get_state(&evac_info.dlink_evt) == DLINK_CONNECTED)) ||
                        (evac_info.type == DEST_FS)) {

                    event_notify(&evac_info.trace_evt, EVAC_START);
                    event_notify(&evac_info.storage_evt, STORAGE_AVAIL);
                }
            }

            break;
        case 'T':
enable_trace:

            if ((count != 3) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            /* If trace configured */
            if (evac_info.type != DEST_UNKNOWN) {
                EXTRADEBUG("enable trace: ");

                // Send a ISI message to modem to activate the
                // buffer marker with action as MARKER_MESSAGE
                trace_buffer_marker(MAL_MON_MARKER_MESSAGE);

                // Ready to read msg from mal server
                // Signal EVAC_START only if below is true.
                if (((evac_info.type == DEST_SDCARD) && (event_get_state(&evac_info.sdcard_evt) == SD_MOUNTED)) ||
                        ((evac_info.type == DEST_USB) && (event_get_state(&evac_info.dlink_evt) == DLINK_CONNECTED)) ||
                        (evac_info.type == DEST_FS)) {

                    event_notify(&evac_info.trace_evt, EVAC_START);
                    event_notify(&evac_info.storage_evt, STORAGE_AVAIL);
                }

                n = trace_set_config(split_command[2]);
            }

            break;

        case 'X':
            /* If trace configured */
            EXTRADEBUG("subscription: ");

            /* Ready to read msg from mal server */
            if ((count == 2) && (prog == _TRACE)) {
                n = mal_mon_sub();
            }

            break;

        case 'S':
            /* If trace configured */
            EXTRADEBUG("Unlock reception thread");

            /* Ready to read msg from mal server */
            if ((count == 2) && (prog == _TRACE)) {
                event_notify(&evac_info.trace_evt, EVAC_START);
                event_notify(&evac_info.storage_evt, STORAGE_AVAIL);
            }

            break;

        case 'Q':
            /* If trace configured */
            EXTRADEBUG("QUIT");

            /* Ready to read msg from mal server */
            if ((count == 2) && (prog == _TRACE)) {
                exit(1);
            }

            break;

        case 'c':
disable_trace:

            if (count > 2) {
                n = -1;
                goto Exit;
            }

            switch (prog) {
            case _TRACE:

                if (event_get_state(&evac_info.trace_evt) == EVAC_START ||
                        event_get_state(&evac_info.trace_evt) == EVAC_FLUSHING) {
                    /* Write last chunk to host*/
                    evac_flush();
                    EXTRADEBUG("Wait END FLUSH");
                    event_wait(&evac_info.flush_evt, EVAC_FLUSHED);
                }

                /* Trigger report to also get all the trace in the SHM buffer. */
                n = trace_trigger(STOP_TRIGGER);
                evac_info.type = DEST_UNKNOWN;
                /* Stop trace */
                EXTRADEBUG("disable trace");
                n = trace_route("STP");

                break;

            case _INV_TRACE:
                /* Disable invariant trace */
                EXTRADEBUG("disable invariant trace");
                n = trace_deactivate();
                break;

            case _DUMP:
                /* Disable dump */
                EXTRADEBUG("disable dump");
                evac_info.dump_type = DEST_UNKNOWN;
                n = 0;
                break;

            default:
                n = -1;
                break;
            }

            break;

        case 'a':

            switch (prog) {
            case _TRACE:
            case _INV_TRACE:
activate_trace:
                EXTRADEBUG("activate trace");

                if (count < 5) {
                    n = -1;
                    goto Exit;
                }

                if (prog == _TRACE) {
                    n = trace_activate(split_command, prog, count - 5);
                } else {
                    n = trace_activate(split_command, prog, count - 4);
                }

                break;
            case _DUMP:
activate_dump:
                EXTRADEBUG("activate dump");

                if (count > 2) {
                    n = -1;
                    goto Exit;
                }

                if (strlen(evac_info.dump_path)) {
                    evac_info.dump_type = DEST_FS;
                    n = 0;
                } else {
                    /* Not yet configured, use -D */
                    n = -1;
                }

                break;
            default:
                n = -1;
                break;
            }

            break;

        case 'l':

            switch (prog) {
            case _TRACE:
rol_file:
                EXTRADEBUG("rol file count=%d", count);
                n = command_set_rol_file(split_command, count);
                break;

            case _DUMP:
dump_files:
                EXTRADEBUG("dump_files");
                n = command_set_dump_files(split_command, count);
                break;
            case _TRIGGER:
trigger_files:
                n = set_max_trigger_report(split_command, count);
                break;
            default:
                n = -1;
            }

            break;

        case 'I':
imei:
            EXTRADEBUG("IMEI");

            if ((count > 2) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            n = command_send_imei(tool_fd);
            goto out;
            break;

        case 'B':
bb_id:
            EXTRADEBUG("Baseband ID");

            if ((count > 2) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            n = command_send_bb_id(tool_fd);
            goto out;
            break;

        case 'r':

            switch (prog) {
            case _TRACE:
trigger_report:
                EXTRADEBUG("trigger report");

                if (count > 2) {
                    n = -1;
                    goto Exit;
                }

                n = trace_trigger(HOST_TRIGGER);
                break;

            case _DUMP:
reset_report:
                EXTRADEBUG("reset report");

                if (count > 2) {
                    n = -1;
                    goto Exit;
                }

                if (evac_info.dump_type && strlen(evac_info.dump_path)) {
                    n = request_reset_modem_with_dump();
                } else {
                    n = -1;
                }

                break;

            default:
                n = -1;
                break;
            }

            break;

        case 'R':
route:
            EXTRADEBUG("Route");

            if ((count < 7) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            n = trace_ids_activate_route(split_command,
                                         count - 6);
            break;

        case 'g':
get_conf:
            EXTRADEBUG("Get config");

            if ((count > 2) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            n = trace_get_config(tool_fd);
            goto out;

        case 's':
read_status:
            EXTRADEBUG("Read Status");

            if ((count < 3) || (prog != _INV_TRACE)) {
                n = -1;
                goto Exit;
            }

            n = trace_get_activation_status(tool_fd, split_command, count - 2);
            goto out;

        case 't':
trace_route:
            EXTRADEBUG("Trace Routing");

            if ((count != 3) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            n = trace_route(&split_command[2][0]);
            break;

        case 'd':
deactivate_invariant_trace:
            EXTRADEBUG("Deactivate invariant trace");

            if ((count < 4) || (prog != _INV_TRACE)) {
                n = -1;
                goto Exit;
            }

            n = trace_ids_activate_route(split_command, 0);
            break;

        case 'h':
trace_history:
            EXTRADEBUG("Trace history");

            if ((count < 3) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            n = command_set_trace_history(split_command, count);
            break;

        case 'i':
config_re_init:
            EXTRADEBUG("Config re-init");

            if ((count > 2) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            n = 0;

            // Stop Trace if ongoing
            if (event_get_state(&evac_info.trace_evt) == EVAC_START ||
                    event_get_state(&evac_info.trace_evt) == EVAC_FLUSHING) {
                /* Write last chunk to host*/
                evac_flush();
                EXTRADEBUG("Wait END FLUSH");
                event_wait(&evac_info.flush_evt, EVAC_FLUSHED);

                /* Trigger report to also get all the trace in the SHM buffer. */
                n = trace_trigger(STOP_TRIGGER);
                evac_info.type = DEST_UNKNOWN;
                /* Stop trace */
                EXTRADEBUG("disable trace");
                n = trace_route("STP");

                if (n == -1) {
                    goto Exit;
                }
            }

            autoconf_set_dump_path();
            autoconf_check_file();
            break;

        case 'O':
overwrite_autoconf:
            EXTRADEBUG("Overwrite trace_auto.conf file");

            if ((count < 3) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            n = overwrite_trace_autoconf(split_command);
            break;

        case 'q':
query_status:
            EXTRADEBUG("Get trace status");

            if ((count < 3) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            n = trace_get_status(tool_fd, split_command, count);
            goto out;
            break;
        case 'b':
buffering_mode:
            EXTRADEBUG("Set trace buffering mode");

            if ((count < 3) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            if (!strncasecmp(split_command[2], "FLUSH", 5)) {
                n = trace_set_buffering_mode(MAL_MON_FLUSHING_MODE);
            } else if (!strncasecmp(split_command[2], "SDRAM", 5)) {
                n = trace_set_buffering_mode(MAL_MON_SDRAM_ONLY_MODE);
            } else {
                n = -1;
            }

            goto Exit;
            break;
        case 'u':
trace_auto_trigger:

            if ((count < 3) || (prog != _TRACE)) {
                n = -1;
                goto Exit;
            }

            n = enable_trace_auto_trigger(split_command);
            break;
        default:
            ALOGW("Wrong option");
            n = -1;
            break;
        }
    }

Exit:

    /* ACK The cmd to the tool */
    if (tool_fd >= 0) {
        if (!n) {
            n = my_write(tool_fd, "OK\n", strlen("OK\n"));
        } else {
            n = my_write(tool_fd, "KO\n", strlen("KO\n"));
        }
    }

out:
    free(bufcopy);

    for (i = 0; i < nsplit; i++) {
        if (split_command[i] != NULL) {
            free(split_command[i]);
        }
    }

    free(split_command);
    return n;
}


int parse_msup_cmd(char *cmd)
{
    if (!strncmp(cmd, MSG_GEN_MODEM_COREDUMP,
                 strlen(MSG_GEN_MODEM_COREDUMP))) {
        return MSUP_GEN_MODEM_COREDUMP;
    }

    else if (!strncmp(cmd, MSG_MODEM_READY,
                      strlen(MSG_MODEM_READY))) {
        return MSUP_IND_MODEM_REBOOT;
    }

    return MSUP_UNKNOWN_CMD;
}


/**
 * \fn int parse_mal_cmd(tLIB_Packet_Ind *packet_ind)
 * \brief Parse the ISI message received from MAL
 *
 * \param the ISI message
 * \return 0 on success, -1 on error
 */

int parse_mal_cmd(tLIB_Packet_Ind *packet_ind)
{
    char *flush_buffer, ril_message[RIL_MESSAGE_SIZE];
    int ret;
    mon_trace_flush_ind_t *p_flush_ind;
    uint16_t trigger_reason;

    switch (packet_ind->id) {
    case MAL_MON_TRACE_BUFFER_FLUSH_IND:

        if (event_get_state(&evac_info.trace_evt) == EVAC_START) {
            p_flush_ind = &packet_ind->msg.flush;

            EXTRADEBUG("MAL_MON_TRACE_BUFFER_FLUSH_IND 0x%x, 0x%x", p_flush_ind->address, packet_ind->msg.flush.size);

            /* Retrieve Start address and packet_ind size, and ID */
            flush_buffer = (char *) malloc(packet_ind->msg.flush.size);

            if (!flush_buffer) {
                ALOGE("Malloc failed!");
                return -1;
            }

            get_modem_data(p_flush_ind, (char *)flush_buffer);
            evac_fifo_insert(packet_ind->msg.flush.buffer_id, (int) flush_buffer, packet_ind->msg.flush.size);
            free(flush_buffer);
        }

        break;

    case MAL_MON_TRACE_LOG_TRIGGER_IND:
        EXTRADEBUG("MAL_MON_TRACE_LOG_TRIGGER_IND");
        memset(ril_message, 0x00, RIL_MESSAGE_SIZE);

        trigger_reason = get_uint16(packet_ind->msg.trigger.trigger_id);
        EXTRADEBUG("Modem trigger reason : Au_id(0x%02x), Trigger id (0x%02x)", ((trigger_reason >> 8) & 0xFF), (trigger_reason & 0xFF));

        switch (trigger_reason & 0xFF) {
        case NO_SERVICE_POWER_SAVE:
            snprintf(ril_message, RIL_MESSAGE_SIZE, "Modem trigger reason : No service power save");
            break;
        case LIMITED_SERVICE:
            snprintf(ril_message, RIL_MESSAGE_SIZE, "Modem trigger reason : Limited Service");
            break;
        case NO_SIM:
            snprintf(ril_message, RIL_MESSAGE_SIZE, "Modem trigger reason : No SIM");
            break;
        default:
            ALOGW("Modem trigger reason unknown : Trigger id (0x%02x)", (trigger_reason & 0xFF));
            snprintf(ril_message, RIL_MESSAGE_SIZE, "Modem trigger reason : Unknown(0x%02x)", (trigger_reason & 0xFF));
            break;
        }

        ret = trace_trigger(MODEM_TRIGGER);

        if (ret == 0) {
            send_to_ril(ril_message);
        }

        break;

    default:
        ALOGW("MAL CMD UNKNOWN %d", packet_ind->id);
        return -1;
        break;
    }

    return 0;
}


/**
 * \fn int autoconf_get_value(char search_tag[], char search_value[])
 * \brief get a value in a config file corresponding to a specified tag
 *
 * \param tag
 * \param value found in file
 * \return 0 if value corresponding to tag is found
 */
int autoconf_get_value(FILE *fd, char search_tag[], char search_value[])
{
    ssize_t read;
    int found = 0;
    char tag[64];

    while (((read = fscanf(fd, "%s %s\n", tag, search_value)) >= 1)
            && (!found)) {
        EXTRADEBUG("read %d: %s %s", (int)read, tag, search_value);

        if (!strcmp(tag, search_tag)) {
            found = 1;
            break;
        }
    }

    if (!found) {
        EXTRADEBUG("Can't find %s ", search_tag);
    } else {
        EXTRADEBUG("%s = %s", search_tag, search_value);
    }

    return !found;
}


int autoconf_check_file(void)
{
    int n;
    FILE *fp;
    char command_line[CMD_LINE_LENGTH];
    int autostart = 0;

    fp = fopen(AUTOCONF_FILENAME, "r");

    /* Check Autoconf File exists and is readable */
    if (!fp) {
        ALOGW("Autoconf file not found");
        return -1;
    }

    /* Autostart ON, Set Trace configs */
    while (fgets(command_line, CMD_LINE_LENGTH, fp) != NULL) {
        // dump -D command is already parsed once before creating threads using
        // the function autoconf_set_dump_path() inside main() function. So no
        // need to execute the command once more.
        if ((strstr(command_line, "dump -D") != NULL) && (evac_info.dump_type != DEST_UNKNOWN)) {
            continue;
        }

        if (strstr(command_line, "AUTOSTART") != NULL) {
            int config_value = 0;

            if (sscanf(command_line, "AUTOSTART %d", &config_value) == 1 && config_value == 1) {
                autostart = 1;
            } else {
                break;
            }
        } else {
            ALOGI("cmd from auto conf = %s", command_line);
            n = parse_tool_cmd(command_line, -1);

            if (n) {
                ALOGD("cmd error : %s", command_line);
            }
        }
    }

    fclose(fp);
    return autostart;
}

// Set the core dump path as mentioned in the Configuration file.
// The line in the configuration file reads as mention below
// dump -D SDCARD /path/to/dump/directory
int autoconf_set_dump_path(void)
{
    int ret = -1;
    FILE *fp;
    char command_line[CMD_LINE_LENGTH];

    fp = fopen(AUTOCONF_FILENAME, "r");

    /* Check Autoconf File exists and is readable */
    if (!fp) {
        ALOGW("autoconf_set_dump_path() : Autoconf file not found");
        return -1;
    }

    /* Set the dump destination */
    while (fgets(command_line, CMD_LINE_LENGTH, fp) != NULL) {
        if (strstr(command_line, "dump -D") != NULL) {
            ALOGI("cmd from auto conf = %s", command_line);
            ret = parse_tool_cmd(command_line, -1);

            if (ret) {
                ALOGD("cmd error : %s", command_line);
            }

            break;
        }
    }

    fclose(fp);
    return ret;
}

/**
 * \fn int command_read(int fd, char *buffer)
 * \brief
 *
 * \param file descriptor
 * \param buffer to be filled by the read command
 * \return Number of bytes read
 */
int command_read(int fd, char *buffer)
{
    int n, fdmax, fdpipe = sk_serv[SK_SERV_CTRL].pipe[0];
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    FD_SET(fdpipe, &readfds);

    fdmax = fd > fdpipe ? fd : fdpipe;

    EXTRADEBUG("fd = %d, fdpipe = %d, fdmax = %d", fd, fdpipe, fdmax);
    n = select(fdmax + 1, &readfds, NULL, NULL, NULL);

    if ((n <= 0) || FD_ISSET(fdpipe, &readfds)) {
        ALOGW("USB Link disconnection : %d", n);
        n = -1;
        goto out;
    }

    memset(buffer, '\0', 256);
    n = read(fd, buffer, 255);

    if (n <= 0) {
        goto out;
    }

    /* Replace last '\n' with '\0' */
    if (buffer[n - 1] == '\n') {
        buffer[n - 1] = '\0';
    }

out:
    return n;
}


/**
 * \fn int init_mal_connection(int void)
 * \brief
 *
 * \param
 * \return
 */
int init_mal_connection(void)
{
    int i;

    /* init socket from modemlogrelay point of view */
    for (i = 0; i < MAL_SK_MAX; i++) {
        mal_dgram_sk[i].conn = dgram_sk_init(mal_dgram_sk[i].type,
                                             mal_dgram_sk[i].name,
                                             DGRAM_SK_LENGTH_MSG_MAX);

        if (!mal_dgram_sk[i].conn) {
            return -1;
        }
    }

    /* init socket from mal point of view */
    return mon_trace_init(mal_dgram_sk[0].name, mal_dgram_sk[1].name,
                          mal_dgram_sk[2].name, mal_dgram_sk[3].name);
}


/**
 * \fn void command_handler(int command_hdl_fd)
 * \brief
 *
 * \param
 * \return
 */
void command_handler(int command_hdl_fd)
{
    int n, end = 0;
    char buffer[256];

    ALOGD("command_handler is launched");

    while (!end) {

        /* Wait modem ready indication */
        EXTRADEBUG("Wait modem ready ?");
        event_wait(&evac_info.modem_evt, MODEM_READY);
        EXTRADEBUG("Wait modem ready !");

        end = 0;

        while (!end) {

            EXTRADEBUG("Waiting for command...");

            n = command_read(command_hdl_fd, buffer);

            if (n <= 0) {
                ALOGW("CLIENT DISCONNECTED!!!!!!!");
                end = 1;
            } else {
                ALOGI("cmd from TOOL = %s", buffer);
                n = parse_tool_cmd(buffer, command_hdl_fd);
            }
        }
    }
}

/**
 * \fn void debug_handler(int command_hdl_fd)
 * \brief
 *
 * \param
 * \return
 */
void debug_handler(int dbg_hdl_fd)
{
    int n, end = 0;
    char buf[256];
    char rsp[256];
    char msg[128];
    int state;
    int len;
    int fdmax, fdpipe = sk_serv[SK_SERV_DBG].pipe[0];
    fd_set readfds;

    ALOGD("debug_handler is launched");

    while (!end) {

        FD_ZERO(&readfds);
        FD_SET(dbg_hdl_fd, &readfds);
        FD_SET(fdpipe, &readfds);

        fdmax = dbg_hdl_fd > fdpipe ? dbg_hdl_fd : fdpipe;

        n = select(fdmax + 1, &readfds, NULL, NULL, NULL);

        if ((n <= 0) || FD_ISSET(fdpipe, &readfds)) {
            ALOGW("USB Link disconnection (debug_handler): %d", n);
            break;
        }

        n = read(dbg_hdl_fd, buf, 255);

        rsp[0] = '\0';

        if (n < 0) {
            ALOGW("debug_handler connection lost !");
            break;
        }

        if (!strncmp(buf, "status", 6)) {
            int i;
            len = 0;

            for (i = 0; i < evac_info.nb_fifo; i++) {
                len += sprintf(rsp + len,
                               "fifo (%d) count diff=%d wr=%d rd=%d\n",
                               i,
                               evac_info.fifo[i].count,
                               evac_info.fifo[i].countwr,
                               evac_info.fifo[i].countrd);
            }

            state = event_get_state(&evac_info.dlink_evt);
            len += sprintf(rsp + len,
                           "\ndlink_evt = (%d) ", state);

            switch (state) {
            case DLINK_DISCONNECTED:
                strcpy(msg, "DLINK_DISCONNECTED");
                break;
            case DLINK_CONNECTED:
                strcpy(msg, "DLINK_CONNECTED");
                break;
            default:
                strcpy(msg, "UNKNOWN !!!");
            }

            len += sprintf(rsp + len, "%s\n", msg);


            state = event_get_state(&evac_info.trace_evt);
            len += sprintf(rsp + len,
                           "trace_evt = (%d) ", state);

            switch (state) {
            case EVAC_STOP:
                strcpy(msg, "EVAC_STOP");
                break;
            case EVAC_START:
                strcpy(msg, "EVAC_START");
                break;
            default:
                strcpy(msg, "UNKNOWN !!!");
            }

            len += sprintf(rsp + len, "%s\n", msg);

            state = event_get_state(&evac_info.flush_evt);
            len += sprintf(rsp + len,
                           "flush_evt = (%d) ", state);

            switch (state) {
            case EVAC_FLUSHED:
                strcpy(msg, "EVAC_FLUSHED");
                break;
            case EVAC_FLUSHING:
                strcpy(msg, "EVAC_FLUSHING");
                break;
            default:
                strcpy(msg, "UNKNOWN !!!");
            }

            len += sprintf(rsp + len, "%s\n", msg);

            state = event_get_state(&evac_info.sdcard_evt);
            len += sprintf(rsp + len,
                           "sdcard_evt = (%d) ", state);

            switch (state) {
            case SD_UNMOUNTED:
                strcpy(msg, "SD_UNMOUNTED");
                break;
            case SD_MOUNTED:
                strcpy(msg, "SD_MOUNTED");
                break;
            default:
                strcpy(msg, "UNKNOWN !!!");
            }

            len += sprintf(rsp + len, "%s\n", msg);


            state = event_get_state(&evac_info.modem_evt);
            len += sprintf(rsp + len,
                           "modem_evt = (%d) ", state);

            switch (state) {
            case MODEM_NOT_READY:
                strcpy(msg, "MODEM_NOT_READY");
                break;
            case MODEM_READY:
                strcpy(msg, "MODEM_READY");
                break;
            default:
                strcpy(msg, "UNKNOWN !!!");
            }

            len += sprintf(rsp + len, "%s\n", msg);


        } else if (!strncmp(buf, "debug_on", 8)) {
            printf_trace = 1;
            strcpy(rsp, "OK\n");
        } else if (!strncmp(buf, "debug_off", 9)) {
            printf_trace = 0;
            strcpy(rsp, "OK\n");
        } else {
            strcpy(rsp, "UNKNOWD CMD !\n");
        }

        n = write(dbg_hdl_fd, rsp, strlen(rsp));
    }
}

/**
 * \fn void monitor_client_handler(void)
 * \brief
 *
 * \param
 * \return
 */
void monitor_client_handler(void)
{
    int n;
    int end = 0;
    tLIB_Packet_Ind packet_ind;

    ALOGD("monitor_client_handler is launched");

    while (!end) {
        /* Wait modem ready indication */
        EXTRADEBUG("Wait modem ready");
        event_wait(&evac_info.modem_evt, MODEM_READY);

        EXTRADEBUG("Waiting for command...");
        n = mal_sk_ind_recv((char *) &packet_ind);

        if (n <= 0) {
            EXTRADEBUG("n = mal_sk_ind_recv == %d !", n);

            if (event_get_state(&evac_info.trace_evt) == EVAC_START) {
                evac_flush();
            }
        } else {
            if (parse_mal_cmd(&packet_ind)) {
                EXTRADEBUG("MAL command parsing failed!");
            }
        }
    }

    EXTRADEBUG("MAL (monitor_client_handler) connection is closed");
}

void modem_synchro_thread(void *param)
{
    cn_error_code_t result;
    cn_context_t *cn_context_p;
    int i = 0;

    do {
        ALOGI("Trying to connect to CN_SERVER, try %d...", i + 1);
        result = cn_client_init(&cn_context_p);
        sleep(1);
    } while ((result != CN_SUCCESS) && (i++ < MAX_SYNC_RETRIES));

    if (i >= MAX_SYNC_RETRIES) {
        ALOGE("Modem seems to be dead, exiting...");
        exit(EXIT_FAILURE);
    }

    ALOGI("Connection to CN_SERVER successfull !\n");

    cn_client_shutdown(cn_context_p);

    event_notify(&evac_info.modem_evt, MODEM_READY);

    if (param == BOOT_SYNCHRO) {
        return;
    }

    /* Silent Reboot use-case */
    evac_info.type = DEST_UNKNOWN;
    evac_info.dump_type = DEST_UNKNOWN;
    autoconf_check_file();
}

/**
 * \fn int request_reset_modem_with_dump(void)
 * \brief
 *
 * \return 0 if cn_server succeeded to send request to modem, -1 at failure
 */
int request_reset_modem_with_dump(void)
{
    cn_error_code_t result;
    cn_context_t *cn_context_p_d = NULL;

    int returnvalue = -1;
    EXTRADEBUG("Sending modem reset request to modem");

    result = cn_client_init(&cn_context_p_d);

    if (result == CN_SUCCESS)  {
        result = cn_request_reset_modem_with_dump(cn_context_p_d, CN_CPU_L23, 0);

        if (result == CN_SUCCESS) {
            returnvalue = 0;
        } else {
            ALOGI("Failed to send modem reset request to modem!\n");
        }

        result = cn_client_shutdown(cn_context_p_d);

        if (result != CN_SUCCESS) {
            ALOGI("Failed to disconnect from CN_SERVER!\n");
        }

    } else {
        ALOGI("Failed to connect to CN_SERVER!\n");
    }

    return returnvalue;
}

/**
 * \fn void modem_sup_handler(void)
 * \brief
 *
 * \param fd file descriptor of ModemSupervisor
 * \return n
 */
void modem_sup_handler(void)
{
    char buf[MLR_SUP_DGRAM_SK_LENGTH_MSG_MAX];
    int n, end;
    pthread_t thread;

    ALOGD("modem_sup_handler is launched");

    end = 0;

    while (!end) {

        EXTRADEBUG("Waiting for command...");
        n = mlr_sup_sk_recv(buf);

        if (!n) {
            continue;
        }

        EXTRADEBUG("command received :%s", buf);

        switch (parse_msup_cmd(buf)) {
        case MSUP_GEN_MODEM_COREDUMP:
            ALOGD("Coredump requested by modem-supervisor");

            evac_flush();
            event_notify(&evac_info.modem_evt, MODEM_NOT_READY);

            if ((evac_info.dump_type && strlen(evac_info.dump_path)) && dump_files != -1) {
                n = command_reset_report();

                if (n) {
                    ALOGE("Coredump failed");
                    goto err;
                }

                EXTRADEBUG("Coredump successful");
            } else {
                ALOGD("Coredump feature not configured!");
            }

            /* ACK The cmd to the tool */
            mlr_sup_sk_send("OK\n", strlen("OK"));
            break;

        case MSUP_IND_MODEM_REBOOT:
            ALOGD("Reboot indication received from MSUP.\n"
                  "Waiting modem initialization...\n");
            pthread_create(&thread, NULL, (void *)modem_synchro_thread,
                           (void *)REBOOT_SYNCHRO);

            pthread_join(thread, NULL);
            break;

        case MSUP_UNKNOWN_CMD:
        default:
            ALOGW("Unknown message : %s", buf);
            goto err;
        }

        continue;
err:
        mlr_sup_sk_send("KO\n", strlen("KO"));
    }
}



/**
 * \fn void monitor_client_handler(void)
 * \brief
 *
 * \param
 * \return
 */
void inv_trace_handler(void)
{
    int n;
    int end = 0;
    uint8_t *trace_data;
    mon_trace_inv_ind_t *inv_trace;
    uint16_t len;
    trace_inv_t trace_inv;
    tLIB_Packet_Ind packet_ind;

    ALOGD("inv_trace_handler is launched");

    while (!end) {

        /* Wait modem ready indication */
        EXTRADEBUG("Wait modem ready");
        event_wait(&evac_info.modem_evt, MODEM_READY);

        /* wait start trace indication */
        EXTRADEBUG("evac_wait_start...");
        evac_wait_start();
        EXTRADEBUG("evac_wait_start OK");

        while (event_get_state(&evac_info.trace_evt) == EVAC_START) {

            /* wait invariant msg */
            n = mal_sk_ind_inv_recv((char *) & packet_ind);

            EXTRADEBUG("packet_ind n=%d fd=%d", n,
                       sck_server_Inv_trace_fd);

            if (n <= 0) {
                ALOGE("invariant socket problem!");
                end = 1;
            } else if ((sck_server_Inv_trace_fd >= 0) &&
                       (packet_ind.id == MAL_MON_TRACE_INV_IND)) {

                inv_trace = &(packet_ind.msg.inv);

                trace_inv.header = 0x55555555;
                trace_inv.time_stamp = 0;
                trace_inv.filler = 0xFF;

                if (inv_trace->n_sb == 2)
                    trace_inv.time_stamp = inv_trace->\
                                           mon_sb_trace_ext_time_stamp_info.\
                                           extended_time_stamp;

                len = inv_trace->mon_sb_trace_inv_data.trace_data_len;

                trace_data = inv_trace->mon_sb_trace_inv_data.trace_data;

                trace_inv.trace_entity =
                    inv_trace->mon_sb_trace_inv_data.trace_entity;

                trace_inv.trace_grp =
                    inv_trace->mon_sb_trace_inv_data.trace_grp;

                trace_inv.trace_id = inv_trace->mon_sb_trace_inv_data.trace_id;

                trace_inv.trace_data_len =
                    inv_trace->mon_sb_trace_inv_data.trace_data_len;

                n = write(sck_server_Inv_trace_fd,
                          (char *) & trace_inv, sizeof(trace_inv_t));

                n = write(sck_server_Inv_trace_fd, (char *)trace_data, len);

                free(trace_data);
            }
        }
    }

    ALOGW("MAL (inv_trace_handler) connection is closed !!!");
}



/**
 * \fn int datalink_read(int fd)
 * \brief Read in the DataLink file descriptor
 * Read in the DataLink file descriptor to check if link still alive
 *
 * \param file descriptor of DataLink
 * \return Bytes read
 */
int datalink_read(int fd, int fdpipe)
{
    char buffer[2];
    int n, fdmax;
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    FD_SET(fdpipe, &readfds);

    fdmax = fd > fdpipe ? fd : fdpipe;

    EXTRADEBUG("fd = %d, fdpipe = %d, fdmax = %d", fd, fdpipe, fdmax);
    n = select(fdmax + 1, &readfds, NULL, NULL, NULL);

    if ((n <= 0) || FD_ISSET(fdpipe, &readfds)) {
        ALOGW("USB Link disconnection (datalink_read) : %d", n);
        n = -1;
        goto out;
    }

    n = read(fd, buffer, 1);

out:
    return n;
}

/**
 * \fn int reportlink_read(int fd)
 * \brief Read in the ReportLink file descriptor
 * Read in the ReportLink file descriptor to check if link still alive
 *
 * \param file descriptor of ReportLink
 * \return Bytes read
 */
int reportlink_read(int fd, int fdpipe)
{
    char buffer[2];
    int n, fdmax;
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    FD_SET(fdpipe, &readfds);

    fdmax = fd > fdpipe ? fd : fdpipe;

    EXTRADEBUG("fd = %d, fdpipe = %d, fdmax = %d", fd, fdpipe, fdmax);
    n = select(fdmax + 1, &readfds, NULL, NULL, NULL);

    if ((n <= 0) || FD_ISSET(fdpipe, &readfds)) {
        ALOGW("USB Link disconnection (datalink_read) : %d", n);
        n = -1;
        goto out;
    }

    n = read(fd, buffer, 1);

out:
    return n;
}


/**
 * \fn void datalink_handler(int fd)
 * \brief
 *
 * \param
 * \return
 */
void datalink_handler(int fd)
{
    int n;

    ALOGD("datalink_handler is launched (fd=%d)", fd);

    /* Indicate that the data link is ready */
    event_notify(&evac_info.dlink_evt, DLINK_CONNECTED);

    if (evac_info.type == DEST_USB) {
        event_notify(&evac_info.trace_evt, EVAC_START);
    }

    while (1) {
        n = datalink_read(fd, sk_serv[SK_SERV_DATA].pipe[0]);
        EXTRADEBUG("Datalink_Read : %d", n);

        if (n <= 0)  {
            ALOGE("datalink_handler connection lost !");
            break;
        }
    }

    /* Data link is broken */
    event_notify(&evac_info.dlink_evt, DLINK_DISCONNECTED);

    /* Set trace in stop mode */
    if (evac_info.type == DEST_USB) {
        event_notify(&evac_info.trace_evt, EVAC_STOP);
    }
}

/**
 * \fn void report_handler(int fd)
 * \brief
 *
 * \param
 * \return
 */
void report_handler(int fd)
{
    int n;
    ALOGD("report_handler is launched (fd=%d)", fd);

    /* Indicate that the dump/trigger data link is ready */
    event_notify(&evac_info.reportlink_evt, DLINK_CONNECTED);

    while (1) {
        n = reportlink_read(fd, sk_serv[SK_SERV_RPT].pipe[0]);
        EXTRADEBUG("Reportlink_Read : %d", n);

        if (n <= 0)  {
            ALOGE("report_handler connection lost !");
            break;
        }
    }

    /* Dump/trigger data link is broken */
    event_notify(&evac_info.reportlink_evt, DLINK_DISCONNECTED);

}

int usb_cable_connected(void)
{
    int fd_usb_state;
    char usb_state[3];

    fd_usb_state = open(USB_CONNECTION_PATH, O_RDONLY);

    if (fd_usb_state < 0) {
        EXTRADEBUG("%s open failed!", USB_CONNECTION_PATH);
        return -1;
    }

    if (read(fd_usb_state, usb_state, 3) < 0) {
        EXTRADEBUG("%s read failed!", USB_CONNECTION_PATH);
        close(fd_usb_state);
        return -1;
    }

    close(fd_usb_state);

    if (usb_state[0] == '0') {
        return 0;
    } else if (usb_state[0] == '1') {
        return 1;
    } else
        ALOGW("%s : %d <- unknown value!",
              USB_CONNECTION_PATH, usb_state[0]);

    return -1;

}

/**
 * \fn void usblink_handler(void)
 * \brief
 *
 * \param
 * \return
 */
void usblink_handler(void)
{
    /* Socket uevent */
    struct sockaddr_nl netlink_addr;
    int ue_socket;
    int ue_size = 64 * 1024;
    int usb_status = -1, usb_status_new;
    char ue_buffer[ue_size];
    int len, i;
    int count = 0;

    ALOGD("usblink_handler is launched");

    /* Configure Netlink family address struct */
    memset(&netlink_addr, 0x00, sizeof(struct sockaddr_nl));
    netlink_addr.nl_family = AF_NETLINK;
    netlink_addr.nl_pid = getpid();
    netlink_addr.nl_groups = 1;

    /* Connect to UEVENT socket */
    if ((ue_socket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT)) < 0) {
        ALOGE("Cannot connect to UEVENT socket!");
        return;
    }

    /* Bind UEVENT socket */
    if (bind(ue_socket, (struct sockaddr *) &netlink_addr, sizeof(netlink_addr)) < 0) {
        ALOGE("Cannot bind UEVENT socket!");
        return;
    }

    /* Configure socket options */
    if (setsockopt(ue_socket, SOL_SOCKET, SO_RCVBUF, &ue_size, sizeof(ue_size)) < 0) {
        ALOGE("Cannot set uevent socket options!");
        return;
    }

    while (1) {
        len = recv(ue_socket, &ue_buffer, sizeof(ue_buffer), 0);

        if (len < 0) {
            count++;
            ALOGW("RECV failed on UEVENT socket! Failure number: %d", count);

            if (count == MAX_SOCKET_RECONNECT_TRIES) {
                ALOGE("RECV failed on UEVENT socket too many times! Closing USB link handler to prevent unhandled loop");
                break;
            }

            //Sleep 3 seconds before trying to read again.
            sleep(3);
            continue;
        }

        count = 0;

        if (strstr(ue_buffer, UEVENT_USB_STR)) {
            EXTRADEBUG("USB Connection UEVENT received : %s", ue_buffer);
            usb_status_new = usb_cable_connected();

            if (usb_status_new == usb_status) {
                continue;
            } else {
                usb_status = usb_status_new;
            }

            if (usb_status == 0) {
                ALOGW("USB Cable unplugged!");

                for (i = 0; i < SK_SERV_MAX; i++) {
                    write(sk_serv[i].pipe[1], "1", 1);
                }
            } else if (usb_status == 1) {
                ALOGI("USB Cable plugged!");
            }
        }
    }

    ALOGD("usblink_handler closed");
}

/**
 * \fn int sdcard_check_mount(void)
 * \brief
 *
 * \return
 */
int sdcard_check_mount(void)
{
    int found = -1;
    FILE *fp;
    char line[MOUNT_LINE_LENGTH];

    /* Check sdcard mount */
    fp = fopen("/proc/mounts", "r");

    if (!fp) {
        ALOGW("Cannot open /proc/mounts");
        return -1;
    }

    while (fgets(line, MOUNT_LINE_LENGTH, fp) != NULL) {
        if (strstr(line, "/mnt/sdcard") != NULL) {
            found = 1;
            break;
        }
    }

    fclose(fp);
    return found;
}


/**
 * \fn void sdcard_mount_handler(void)
 * \brief
 *
 * \param
 * \return
 */
void sdcard_mount_handler(void)
{
    int n = 0;
    int count_sleep = 0;

    ALOGD("sdcard_mount_handler is launched");

    /* Wait for SDCARD mount */
    while (count_sleep < 80) {
        sleep(2);
        count_sleep++;
        n = sdcard_check_mount();

        if (n == 1) {
            event_notify(&evac_info.sdcard_evt, SD_MOUNTED);

            if (evac_info.type == DEST_SDCARD) {
                event_notify(&evac_info.trace_evt, EVAC_START);
            }

            ALOGD("Sdcard is mounted");

            /* Check for kernel dump and move to specific folder */
            move_kerneldump();

            break;
        }
    }

    ALOGD("sdcard_mount_handler is closed");
}

/**
 * \fn void sck_server_inv_trace(int fd)
 * \brief
 *
 * \param
 * \return
 */
void sck_server_inv_trace(int fd)
{
    int n;

    ALOGD("sck_server_inv_trace is launched (fd=%d)", fd);

    sck_server_Inv_trace_fd = fd;

    while (1) {
        n = datalink_read(fd, sk_serv[SK_SERV_INV].pipe[0]);
        EXTRADEBUG("Datalink_Read : %d", n);

        if (n <= 0)  {
            ALOGW("sck_server_inv_trace connection lost !");
            break;
        }

    }

    close(fd);
}

/**
 * \fn void ril_connection__handler(void)
 * \brief
 *
 * \param Handles the connection to the RIL
 * \return
 */
void ril_connection_handler(int port)
{
    int ret;
    char message[RIL_MESSAGE_SIZE];

    // Open socket for RIL to connect
    ret = open_ril_server(port);

    if (ret < 0) {
        return;
    }

    while (1) {

        // Connect to RIL client
        ret = connect_client();

        if (ret < 0) {
            continue;
        }

        // Wait for RIL message
        while (1) {
            memset(message, 0x00, RIL_MESSAGE_SIZE);
            ret = receive_from_ril(message);

            if (ret <= 0)  {
                ALOGE("ril_connection_handler connection lost");
                break;
            }

            if ((strlen(message)) && (!strncmp(message, TRIGGER_COMMAND, 8))) {
                trace_trigger(RIL_TRIGGER);
            }
        }

        // Disconnect client
        disconnect_client();
        ALOGI("RIL Client disconnected");
    }

    close_ril_server();

    return;
}

/**
 * \fn int socket_server_open(int port)
 * \brief Open a server socket.
 * Open a server socket on a specific port and return the file descriptor
 * pctool can be detected on ethernet.
 *
 * \param port port to listen
 * \return file descriptor of sock if successfull else -1
 */
int socket_server_open(int port, int nb_client)
{
    int sock, bind_error;
    int yes = 1;
    struct sockaddr_in server_address;

    memset(&server_address, 0, sizeof(struct sockaddr));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        ALOGE("socket error: socket_server_open, file descriptor returned from socket = -1, errno = %d", errno);
        return sock;
    }

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    bind_error = bind(sock, (struct sockaddr *) & server_address,
                      sizeof(struct sockaddr));

    if (bind_error == -1) {
        ALOGE("bind error: socket_server_open, errno = %d", errno);
        sock = -1;
    }

    if (listen(sock, nb_client) == -1) {
        ALOGE("listen error: socket_server_open");
        sock = -1;
    }

    return sock;
}

/**
 * \fn void socket_server_handler(int server_number)
 * \brief
 *
 * \param
 * \return
 */
void socket_server_handler(int server_number)
{
    int sockfd;
    int end;
    int port_number;
    char *server_name;
    func_t function;
    int count = 0;

    port_number = sk_serv[server_number].port;
    function    = sk_serv[server_number].func;
    server_name = sk_serv[server_number].name;

    ALOGD("***** Server (%s) port=%d Waiting client... *****", server_name, port_number);

    end = 0;

    while (!end) {

        sockfd = socket_server_open(port_number, 1);

        if (sockfd < 0) {
            count++;
            ALOGW("Failed to open socket on port %d. Try %d", port_number, count);

            if (count == MAX_SOCKET_RECONNECT_TRIES) {
                ALOGE("Failed too many times to reconnect socket on port %d. MLR will restart", port_number);

                //Don't stop modem_log_relay if modem is down since we could be in the middle of generating a coredump.
                if (event_get_state(&evac_info.modem_evt) == MODEM_NOT_READY) {
                    event_wait(&evac_info.modem_evt, MODEM_READY);
                    //sleep one second to ensure MSUP receives our response
                    sleep(1);
                }

                exit(EXIT_FAILURE);
            }

            //Sleep 3 seconds before trying to reconnect
            sleep(3);
            continue;
        }

        count = 0;

        ALOGD("***** Server (%s) port=%d Waiting client2... *****", server_name, port_number);

        sk_serv[server_number].fd = accept(sockfd, NULL, 0);

        if (sk_serv[server_number].fd < 0) {
            ALOGE("Accept (socket_server_handler)!");
            continue;
        }

        ALOGD("Server (%s) NEW CLIENT !!! fsock=%d *****", server_name, sk_serv[server_number].fd);

        if (pipe(sk_serv[server_number].pipe) < 0) {
            ALOGE("pipe call failed: socket_server_handler, server_number = %d", server_number);
        }

        (function)(sk_serv[server_number].fd);

        ALOGD("CLIENT (%s) is disconnected !!!", server_name);

        close(sk_serv[server_number].fd);
        sk_serv[server_number].fd = -1;
        close(sk_serv[server_number].pipe[0]);
        close(sk_serv[server_number].pipe[1]);
        sk_serv[server_number].pipe[0] = -1;
        sk_serv[server_number].pipe[1] = -1;
        close(sockfd);
    }
}

arg_t arg[] = {
    {.cmd = "--ctrl_port=", .fmt = "%d", .val = "-1", .manda = 1, .adr = &sk_serv[SK_SERV_CTRL].port},
    {.cmd = "--data_port=", .fmt = "%d", .val = "-1", .manda = 1, .adr = &sk_serv[SK_SERV_DATA].port},
    {.cmd = "--inv_port=", .fmt = "%d", .val = "-1", .manda = 0, .adr = &sk_serv[SK_SERV_INV].port},
    {.cmd = "--dbg_port=", .fmt = "%d", .val = "-1", .manda = 0, .adr = &sk_serv[SK_SERV_DBG].port},
    {.cmd = "--report_port=", .fmt = "%d", .val = "-1", .manda = 1, .adr = &sk_serv[SK_SERV_RPT].port},
    {.cmd = "--ril_port=", .fmt = "%d", .val = "-1", .manda = 0, .adr = &ril_port},
    {.cmd = "--debug=", .fmt = "%d", .val = "0" , .manda = 0, .adr = &printf_trace},
    {.cmd = "--hats=", .fmt = "%d", .val = "0" , .manda = 0, .adr = &hatsmode},
    {.cmd = "\0" }
};

char *arg_usage = "modem_log_relay --ctrl_port=<port> --data_port=<port> --inv_port=<port> --dbg_port=<port> --report_port=<port> --ril_port=<port> --debug=1 --hats=0";

void init_arg(void)
{
    int i = 0;
    arg_t *p;

    while (arg[i].cmd[0]) {
        p = &arg[i];
        p->type = ARG_NOTYPE;
        p->found = 0;

        if (!strncmp(p->fmt, "%d", 2)) {
            *((int *)p->adr) = atoi(p->val);
            p->type = ARG_INT;
        }

        i++;
    }
}

int call_arg(int argc, char *argv[])
{
    int i, j, n;
    char cmd[256];
    arg_t *p;

    init_arg();

    /* search argument and affect value */
    for (i = 1; i < argc; i++) {
        j = 0;

        while (arg[j].cmd[0] != '\0') {
            p = &arg[j];
            sprintf(cmd, "%s%s", p->cmd, p->fmt);

            if (p->type == ARG_INT) {
                n = sscanf(argv[i], cmd, ((void *)p->adr));

                if (n == 1) {
                    p->found = 1;
                    break;
                }
            }

            j++;
        }
    }

    /* check if missing mandatory argument */
    n = 1;
    j = 0;

    while (arg[j].cmd[0] != '\0') {
        p = &arg[j];

        if ((p->manda == 1) && (p->found == 0)) {
            n = 0;
            ALOGW("%s is mandatory !\n", p->cmd);
        }

        j++;
    }

    return n;
}

int mlr_initialize(void)
{
    int i, ret;

    dump_files = 0;
    evac_info.nb_fifo = 0;
    evac_info.rol_ff.enabled = 0;

    evac_info.type = DEST_UNKNOWN;
    evac_info.dump_type = DEST_SDCARD;

    // Initialize trigger directory
    snprintf(trigger_directory, NAME_LENGTH, "%s", TRIGGER_PATH);

    /* Initialize events */
    event_init(&evac_info.modem_evt, MODEM_NOT_READY);
    event_init(&evac_info.dlink_evt, DLINK_DISCONNECTED);
    event_init(&evac_info.trace_evt, EVAC_STOP);
    event_init(&evac_info.flush_evt, EVAC_FLUSHED);
    event_init(&evac_info.sdcard_evt, SD_UNMOUNTED);

    // Inittialize mutex for internal FIFO buffer
    lock_trace_buffer_size = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

    /*  Open modem trace device */
    evac_info.modem_fd = open(MODEM_PATH, O_RDONLY);

    if (evac_info.modem_fd < 0) {
        ALOGE("Can't open modem device !");
        return -1;
    }

    /* Initialize evacuation fifos */
    for (i = 0; i < MLR_MAX_EVAC_FILE; i++) {
        evac_fifo_init(&(evac_info.fifo[i]));
    }

    /*  Initialize sockets */
    ret = mlr_sup_init_sk(SK_MODEM_LOG_RELAY);

    if (ret) {
        ALOGE("mlr_sup_init_sk() failed");
        return -1;
    }

    // Initiate MAL connection
    ret = init_mal_connection();

    if (ret) {
        ALOGE("init_mal_connection() failed");
        return -1;
    }

    return 0;
}


/**
 * \fn int main(int argc, char *argv[])
 * \brief ModemLogRelay main function
 *
 * \param argc
 * \param argv
 * \return 0
 */
int main(int argc, char *argv[])
{
    int i, ret, thread_nb = 0, mdebug_enabled = 1;
    pthread_t thread[SK_SERV_MAX + 7];
    struct tee_product_config product_cfg;

    ret = call_arg(argc, argv);

    if (!ret) {
        ALOGW("Invalid command arguments!\n");
        ALOGI("%s\n", arg_usage);
        exit(1);
    }

    OPENLOG(LOG_USER);
    ALOGI("Starting Modem Log Relay...");

    ret = mlr_initialize();

    if (ret) {
        ALOGE("Initialization failed, exiting.");
        exit(EXIT_FAILURE);
    }

    /* Set the dump destination path before creating the modem_sup thread.
       It can happen that modem can crash  before the modem_sup_handler is
       started, so need to set the dump directory path to store the  modem
       core dump.
    */
    ret = autoconf_set_dump_path();

    if (ret == -1) {
        ALOGW("Unable to set the the dump directory path from configuarion file.");
    } else {
        ALOGI("Dump directory path is set successfully.");
    }

    EXTRADEBUG("Creating threads...");
    pthread_create(&thread[thread_nb++], NULL,
                   (void *)modem_sup_handler, (void *)NULL);

    pthread_create(&thread[thread_nb++], NULL,
                   (void *)evac_handler, (void *)NULL);

    pthread_create(&thread[thread_nb++], NULL,
                   (void *)monitor_client_handler, (void *)NULL);

    pthread_create(&thread[thread_nb++], NULL,
                   (void *)inv_trace_handler, (void *)NULL);

    pthread_create(&thread[thread_nb++], NULL,
                   (void *)sdcard_mount_handler, (void *)NULL);

    pthread_create(&thread[thread_nb++], NULL,
                   (void *)usblink_handler, (void *)NULL);

    ret = get_product_config(&product_cfg);

    if (ret != BASS_RC_SUCCESS) {
        ALOGE("Get product configuration failed, exiting...\n");
        exit(1);
    }

    if ((product_cfg.rt_flags & TEE_RT_FLAGS_MODEM_DEBUG) || (product_cfg.rt_flags & TEE_RT_FLAGS_ALLOW_MODEM_DUMP)) {
        ALOGI("Modem debug activated in ISSW.\n");

        for (i = 0; i < SK_SERV_MAX; i++) {
            if (sk_serv[i].port != -1) {
                pthread_create(&thread[thread_nb++], NULL,
                               (void *)socket_server_handler, (void *)i);
            }
        }

        // create RIL server socket
        if (ril_port > 0) {
            pthread_create(&thread[thread_nb++], NULL, (void *)ril_connection_handler, (void *)ril_port);
        }
    } else {
        ALOGW("Modem debug deactivated in ISSW, sockets threads not created\n");
        mdebug_enabled = 0;
    }

    ALOGI("Waiting Modem to be ready...");

    if (hatsmode == 0) {
        // Check for the modem is up and running
        modem_synchro_thread((void *)BOOT_SYNCHRO);
    } else {
        // In HATS mode, sleep for 20 seconds to be sure that modem is up
        ALOGI("In HATS mode, waiting few seconds for the modem to be ready.");
        sleep(15);
    }

    ALOGI("Modem ready");

    EXTRADEBUG("Get Modem software version");
    get_modem_version();

    if (mdebug_enabled) {
        EXTRADEBUG("Subscribing for modem triggers");
        mal_mon_sub_modem_trigger();
    }

    /* Determine if autostart is on and fill coredump path */
    ret = autoconf_check_file();

    if (ret == -1) {
        ALOGW("Auto-configuration not found");
    } else {
        ALOGI("Auto-configuration found");
    }

    for (i = 0; i < thread_nb; i++) {
        pthread_join(thread[i], NULL);
    }

    ALOGI("Exiting Modem Log Relay.");

    return 0;
}
