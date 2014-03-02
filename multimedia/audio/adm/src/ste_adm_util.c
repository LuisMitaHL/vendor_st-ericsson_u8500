#include "ste_adm_util.h"
#include "ste_adm_dbg.h"
#include "errno.h"
#include "ste_adm_omx_log.h" // ste_adm_debug_logbuf_show
#include "ste_adm_srv.h"

#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

int adm_sem_init(adm_sem_t* sem, unsigned int init_val)
{
    ADM_ASSERT(sem);
    int retval = sem_init(sem, 0, init_val);

    if (retval != 0) {
        ALOG_ERR("Failed to create semaphore: %s\n", strerror(errno));
    }

    return retval;
}

int adm_sem_wait(adm_sem_t* sem, unsigned short timeout)
{
    ADM_ASSERT(sem);

    struct timespec ts;
    ts.tv_nsec = 0;
    ts.tv_sec  = (long) (time(NULL) + timeout + 1);
    int retval;
    struct timespec now;
    time_t timespent;
    time_t before;

    clock_gettime(CLOCK_MONOTONIC, &now);
    before = now.tv_sec;

    do {
        retval = sem_timedwait(sem, &ts);

        clock_gettime(CLOCK_MONOTONIC, &now);
        timespent = now.tv_sec - before;
        ts.tv_sec = ts.tv_sec - timespent;

    } while ((retval != 0 && errno == ETIMEDOUT) && timespent < timeout + 1);

    if (retval != 0) {
        if (errno == EINTR) {
            ALOG_WARN("Semaphore wait interrupted by signal\n");
        } else if (errno != ETIMEDOUT) {
            ALOG_ERR("Semaphore error: %s\n", strerror(errno));
        }
        return errno;
    }

#ifndef ADM_DBG_X86
    ALOG_INFO_VERBOSE("Semaphore Down - 0x%08X (%d)", sem, sem->count);
#else
    ALOG_INFO_VERBOSE("Semaphore Down - 0x%08X (%d)", sem, adm_sem_get_count(sem));
#endif
    return 0;
}

void adm_sem_wait_and_printf(adm_sem_t* sem, const char* file, int line, const char* arg, ...)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;

    // ALways do this so we don't get dumps first time a semaphore is late..
    char linebuf[120];
    va_list a;
    va_start(a, arg);
    vsnprintf(linebuf, 120, arg, a);
    va_end(a);
    unsigned short wait_time_secs = 5;

    int result;

    while ((result = adm_sem_wait(sem, wait_time_secs)) != 0) {
        if (result != EINTR) {
            ALOG_WARN("Semaphore seems stuck at %s:%d\n", file, line);
            ALOG_WARN("Semaphore purpose: '%s'\n", linebuf);
            wait_time_secs = 60;

            ste_adm_debug_reboot();
        }
    }
}






void adm_sem_post(adm_sem_t* sem)
{
    ADM_ASSERT(sem);
    int retval = sem_post(sem);
    if (retval != 0) {
        ALOG_ERR("Semaphore error: %s\n", strerror(errno));

        // Post fails if the semaphore is invalid (which is a
        // pre-requisite that it is not), or if it is overflowed,
        // for which error handling is not motivated.
        ADM_ASSERT(0);
    }

#ifndef ADM_DBG_X86
    ALOG_INFO_VERBOSE("Semaphore Up - 0x%08X (%d)", sem, sem->count);
#else
    ALOG_INFO_VERBOSE("Semaphore Up - 0x%08X (%d)", sem, adm_sem_get_count(sem));
#endif
}


void adm_sem_destroy(adm_sem_t* sem)
{
    ADM_ASSERT(sem);
    int retval = sem_destroy(sem);
    (void) retval;
    // This only fails if the semaphore is invalid,
    // which is a pre-requisite that it is not
    ADM_ASSERT(retval == 0);
}


///////////////////////////////////////////////////////////////////////////////
//
// Shared memory
//
///////////////////////////////////////////////////////////////////////////////



int util_shm_create(int shm_fd_to_reuse, int bytes)
{
    int fd = shm_fd_to_reuse;

    if (fd == -1) {
        char filename[] = "/dev/shm/adm-XXXXXX";
        fd = mkstemp(filename);
        if (fd <= -1) {
                ALOG_ERR("util_shm_create - mkstemp(%s) failed, errno %s\n", filename, strerror(errno));
                return -1;
        }

        // Get rid of the file name, so the inode goes away when the last fd/mmap
        // is dropped. Otherwise, the file name would be one more inode reference,
        // keeping the file there.
        unlink(filename);
    }

    if (ftruncate(fd, bytes)) {
            ALOG_ERR("util_shm_create - ftruncate(%d,%d) failed, errno %s\n", fd, bytes, strerror(errno));
            close(fd);
            return -1;
    }


    return fd;
}

// Transmit the specified file descriptor over the socket
ste_adm_res_t util_fd_send_to_socket(int socket_fd, int fd)
{
    struct msghdr msg;
    char control_data[CMSG_SPACE(sizeof(int))];
    char iobuf[8];

    // Avoid Valgrind warnings
    memset(&msg, 0, sizeof(msg));
    memset(control_data, 0, sizeof(control_data));
    memset(iobuf,0,sizeof(iobuf));

    struct iovec iov[1];      /* I/O vector */
    iov[0].iov_base = iobuf;
    iov[0].iov_len = 8;

    msg.msg_control    = &control_data;
    msg.msg_controllen = sizeof(control_data);
    msg.msg_name       = NULL;
    msg.msg_namelen    = 0;
    msg.msg_iov        = iov;
    msg.msg_iovlen     = 1;
    msg.msg_flags      = 0;

    struct cmsghdr *cm = CMSG_FIRSTHDR(&msg);
    cm->cmsg_len   = CMSG_LEN(sizeof(int));
    cm->cmsg_level = SOL_SOCKET;
    cm->cmsg_type  = SCM_RIGHTS;
    msg.msg_controllen = cm->cmsg_len;

    *( (int*) CMSG_DATA(cm)) = fd;
    if (sendmsg(socket_fd, &msg, 0) <= 0) {
        ALOG_ERR("util_fd_send_to_socket(%d, %d): sendmsg failed, errno=%s\n", socket, fd, strerror(errno));
        return STE_ADM_RES_INTERNAL_ERROR;
    }

    return STE_ADM_RES_OK;
}

int adm_sem_get_count(adm_sem_t* sem)
{
    int sval;
    int retval = sem_getvalue(sem, &sval);

    if (retval == 0) {
        if (sval >= 0) return sval;
        return 0; // negative count to specify waiting threads should be ignored
    }

    return -1;
}


ste_adm_modem_type_t adm_get_modem_type()
{

#if defined(STE_PLATFORM_U8540) && defined(ADM_ENABLE_FEATURE_FAT_MODEM)
    return ADM_FULL_FAT_MODEM;
#elif !defined(STE_PLATFORM_U8540) && defined(ADM_ENABLE_FEATURE_FAT_MODEM)
    /* ADM_ENABLE_FEATURE_FAT_MODEM*/
    /* In case of 9540 platform, half slim modem and fat modem are supported*/
    /* It is dynamically known */
    int is_audio_half_slim = 0;
    int is_audio_fat = 0;
    static ste_adm_modem_type_t modem_type  = ADM_UNKNOWN_MODEM_TYPE;
    if(modem_type == ADM_UNKNOWN_MODEM_TYPE)
    {
        FILE * f_audio_half_slim = fopen("/sys/devices/platform/modem-hwcfg/audio_half_slim","r");
        if(f_audio_half_slim){
            if(fscanf(f_audio_half_slim, "%d", &is_audio_half_slim)<=0){
                ALOG_ERR("Unable to read half_slim information");
                is_audio_half_slim = 0;
            }
            fclose(f_audio_half_slim);
        }
        FILE * f_audio_fat = fopen("/sys/devices/platform/modem-hwcfg/audio_fat","r");
        if(f_audio_fat){
            if(fscanf(f_audio_fat, "%d", &is_audio_fat)<=0){
                ALOG_ERR("Unable to read half_slim information");
                is_audio_fat = 0;
            }
            fclose(f_audio_fat);
        }
        ALOG_INFO("is_audio_half_slim = %d  |is_audio_fat = %d ", is_audio_half_slim, is_audio_fat);
        if((is_audio_half_slim==0) && (is_audio_fat==0) ){
            ALOG_WARN("Unable to retrieve Audio Modem Type, set it to default : ADM_FAT_MODEM");
            modem_type = ADM_FAT_MODEM;
        }else if((is_audio_half_slim==0) && (is_audio_fat==1) ){
            modem_type =  ADM_FAT_MODEM;
        } else if((is_audio_half_slim==1) && (is_audio_fat==0) ){
            modem_type =  ADM_HALF_SLIM_MODEM;
        } else if((is_audio_half_slim==1) && (is_audio_fat==1) ){
            ALOG_ERR(" !!! Both Audio Modem type set !!!  : ADM_UNKNOWN_MODEM_TYPE");
            modem_type =  ADM_UNKNOWN_MODEM_TYPE;
        } else{
            ALOG_ERR(" !!!Unknown combinaison !!!  : ADM_UNKNOWN_MODEM_TYPE");
            modem_type =  ADM_UNKNOWN_MODEM_TYPE;
        }
    }

    return modem_type;
    /*ADM_ENABLE_FEATURE_FAT_MODEM*/
#else
    /* In case of 8500 platform, only slim modem is supported*/
    return ADM_SLIM_MODEM;
#endif /*SLIM_MODEM*/
}



