/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "kerneldump.h"
#include "utils.h"

/**
 * create_default_kerneldumpdir
 *
 * Check if /sdcard/COREDUMP_PATH/KERNELDUMP_DIR
 * directory exists. If not, create it.
 *
 * output: 0 on success
 *         other value on error.
 */
int create_default_kerneldumpdir(void)
{
    int ret = 0;
    char temp_dir[NAME_LENGTH];

    memset(temp_dir, 0, NAME_LENGTH);
    snprintf(temp_dir, NAME_LENGTH, "%s%s%s", "/sdcard", COREDUMP_PATH,
             KERNELDUMP_DIR);
    ret = Create_dir(temp_dir);

    if (ret != 0) {
        ALOGE("Can't create %s directory\n", temp_dir);
    }

    return ret;
}

/*
 * coredump_move_kerneldump
 *
 * check if kernel dump files are present on SDcard root or not.
 * If present, then move them to directory specific to kernel
 * dumps.
 *
 * input  : None
 * output : 0  success
 *          other on eer(consrror
 */
int move_kerneldump()
{
    FILE *fd_msg;
    int ret = 0, n = 0;

    struct timeval tv;
    struct timezone tz;
    struct tm *time;

    char cmd_line[CMD_LINE_LENGTH];
    char old_kdump_filepath[NAME_LENGTH];
    char new_kdump_filename[NAME_LENGTH];
    struct dirent **cdumplist;
    char *cdump_prefix;

    /* Check if new kernel dumps are present */
    n = scandir("/sdcard", &cdumplist, dir_entry_filter, 0);

    if (n > 0) {
        EXTRADEBUG("Found %d coredumps", n);

        /* SDCard is mounted, otherwise we would not be in this method since
         * it is called from sdcard_mount_handler. Check if kernel dump directory exists on SDCard,
         * otherwise create a new directory for kernel dumps on SDcard */
        ret = create_default_kerneldumpdir();

        if (ret != 0) {
            while (n--) {
                free(cdumplist[n]);
            }

            free(cdumplist);
            return ret;
        }

        /* get date and time */
        gettimeofday(&tv, &tz);
        time = gmtime(&tv.tv_sec);

        /* For each found cdump - file.. */
        while (n--) {

            memset(old_kdump_filepath, 0, NAME_LENGTH);
            snprintf(old_kdump_filepath, NAME_LENGTH, "%s%s", "/sdcard/",
                     cdumplist[n]->d_name);

            /* Get the prefix to be used as the beginning of the filename */
            cdump_prefix = (char *) malloc(strlen(cdumplist[n]->d_name));

            if (cdump_prefix == NULL) {
                ALOGI("Can't allocate memory for coredump prefix");

                while (n >= 0) {
                    free(cdumplist[n]);
                    n--;
                }

                free(cdumplist);
                return -1;
            }

            ret = get_dump_file_prefix(((char *) cdumplist[n]->d_name),
                                       cdump_prefix);

            if (ret != 0) {
                ALOGI("Could not get prefix from the file. Assigning it default value cdump_x");
                snprintf(cdump_prefix, 8, "%s", "cdump_x");
            }

            /* add timestamp to prefix kernel dump name */
            memset(new_kdump_filename, 0, NAME_LENGTH);
            snprintf(new_kdump_filename, NAME_LENGTH,
                     "%s_%04d-%02d-%02d_%02dh%02dm%02d.elf", cdump_prefix,
                     1900 + time->tm_year, time->tm_mon + 1, time->tm_mday,
                     time->tm_hour, time->tm_min, time->tm_sec);

            free(cdump_prefix);
            ALOGI("Kernel dump file name %s\n", new_kdump_filename);

            /* Move kernel dump file into new directory */
            memset(cmd_line, 0, CMD_LINE_LENGTH);
            snprintf(cmd_line, CMD_LINE_LENGTH, "mv %s %s%s%s%s%s",
                     old_kdump_filepath, "/sdcard", COREDUMP_PATH,
                     KERNELDUMP_DIR, "/", new_kdump_filename);
            ret = system(cmd_line);

            if (ret != 0) {
                ALOGE("Can't move kernel dump to %s%s%s directory\n", "/sdcard", COREDUMP_PATH, KERNELDUMP_DIR);

                while (n >= 0) {
                    free(cdumplist[n]);
                    n--;
                }

                free(cdumplist);
                return ret;
            }

            /* Writing coredump notification to kernel log */
            fd_msg = fopen(KERNEL_MSG_DEV, "w");

            if (fd_msg != 0) {
                fprintf(fd_msg, KERNELDUMP_MSG "/sdcard%s%s/%s \n",
                        COREDUMP_PATH, KERNELDUMP_DIR, new_kdump_filename);
                fclose(fd_msg);
            }

            if (cdumplist[n] != NULL) {
                free(cdumplist[n]);
            }
        } //End of while

        // Change to root directory before leaving
        memset(cmd_line, 0, CMD_LINE_LENGTH);
        snprintf(cmd_line, CMD_LINE_LENGTH, "cd /");
        ret = system(cmd_line);

        if (ret != 0) {
            ALOGE("Can't change directory to root\n");
        }

    } //end of found coredump
    else if (n < 0) {
        EXTRADEBUG("scandir error while checking for cdumps");
    } else { // n == 0
        ALOGI("No kernel dumps available.");
    }

    free(cdumplist);
    ret = errno;
    return ret;
}
