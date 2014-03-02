/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef UTILS_H
#define UTILS_H

#include <dirent.h>
#include <stdint.h>

#define PREFIX_MODEM_COREDUMP       "mcd_"
#define PREFIX_SYSTEM_COREDUMP      "scd_"
#define PREFIX_KERNEL_COREDUMP      "cdump"

#define TEMP_DIR                    "/coredump_temp"
#define TEMP_PATH                   "/tmp"
#define COREDUMP_PATH               "/core_dumps"

#define NAME_LENGTH                 150
#define CMD_LINE_LENGTH             256
#define PATH_LENGTH                 128
#define TIMESTAMP_LENGTH            25

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define LITTLEENDIAN                0
#define BIGENDIAN                   1

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define BINNAME "MLR"
int syslog_trace;
int printf_trace;

#ifdef HAVE_ANDROID_OS
#define LOG_TAG BINNAME
#include <cutils/log.h>
#define OPENLOG(facility) ((void)0)
#define EXTRADEBUG(fmt , __args__...) \
        do { \
                if (printf_trace) \
                    ALOGD(fmt, ##__args__); \
            } while (0)
#else
#include <syslog.h>
#define SYSLOG(prio, fmt , __args__...) \
    do { \
        if (syslog_trace) \
            syslog(prio, "%s: " fmt "\r\n", __func__, ##__args__); \
        if (printf_trace) \
            printf("%s: " fmt "\r\n", __func__, ##__args__); \
    } while (0)

#define OPENLOG(facility) openlog(BINNAME, LOG_PID | LOG_CONS, facility)
#define LOG(priority, format, ...) SYSLOG(priority, format, ##__VA_ARGS__)
#define ALOGV(format, ...)   LOG(LOG_INFO, format, ##__VA_ARGS__)
#define ALOGD(format, ...)   LOG(LOG_DEBUG, format, ##__VA_ARGS__)
#define ALOGI(format, ...)   LOG(LOG_INFO, format, ##__VA_ARGS__)
#define ALOGW(format, ...)   LOG(LOG_WARNING, format, ##__VA_ARGS__)
#define ALOGE(format, ...)   LOG(LOG_ERR, format, ##__VA_ARGS__)
#define EXTRADEBUG(format, ...)  LOG(LOG_DEBUG, format, ##__VA_ARGS__)
#endif

int delete_dir(char *dir_path);
int Create_dir(char *path);
int dir_entry_dump_filter(const struct dirent *entry);
int dir_entry_filter(const struct dirent *entry);
int filecompare(const struct dirent **first_file, const struct dirent **second_file);
int filecompare_latest(const struct dirent **first_file, const struct dirent **second_file);
struct tm *get_time_stamp();
int get_dump_file_prefix(char *dumpfilename, char *prefix);
int get_endian(void);
uint16_t get_uint16(uint16_t val);
void set_predefine_path_filecompare_latest(const char *path);
void set_filter_string(int filetype, const char *filter);
int  scandir_filter(const struct dirent *entry);
uint32_t swap_endian_32(uint32_t val);

#endif /* ifndef UTILS_H */
