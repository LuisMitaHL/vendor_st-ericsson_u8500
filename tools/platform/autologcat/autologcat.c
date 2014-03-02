/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

/* Module specific includes */
#include "autologcat_log.h"

#define MOUNT_LINE_LENGTH 256
#define MAX_CMD_LEN 512

#define AUTOCONF_FILENAME "/system/etc/autologcat.conf"

const char *command;    /* command */

/**
 * \fn void usage(void)
 * \brief
 *
 * \param
 * \return
 */
static void usage(void)
{
    printf("Usage: autologcat\n");
    printf("Wrapper for logcat. Takes a logcat command in full from the configuration file \"autologcat.conf\" and executes it."
           "If the log path is on SDcard, it will check to see that it is mounted first before issuing the command.\n\n");
}

/**
 * \fn int parse_autoconf(void)
 * \brief
 *
 * \return
 */
int parse_autoconf(void)
{
    int i = -1;
    FILE *fp;
    char line[MAX_CMD_LEN];

    /* Check sdcard mount */
    fp = fopen(AUTOCONF_FILENAME, "r");

    if (!fp) {
        ALOGW("Cannot open %s\n", AUTOCONF_FILENAME);
        return i;
    }

    while (fgets(line, MAX_CMD_LEN, fp) != NULL) {
        if (strstr(line, "/system/bin/logcat") != NULL) {

            /* remove newline characters and trailing whitespaces */
            while (isspace(line[strlen(line) - 1])) {
                line[strlen(line) - 1] = '\0';
            }

            /* Save the command */
            sprintf((char*) command, "%s", line);
            i = 0;
            break;
        }
    }

    fclose(fp);
    return i;
}

/**
 * \fn int sdcard_check_mount(void)
 * \brief
 *
 * \return
 */
int sdcard_check_mount(void)
{
    int i = -1;
    FILE *fp;
    char line[MOUNT_LINE_LENGTH];

    /* Check sdcard mount */
    fp = fopen("/proc/mounts", "r");

    if (!fp) {
        ALOGW("Cannot open /proc/mounts\n");
        return -1;
    }

    while (fgets(line, MOUNT_LINE_LENGTH, fp) != NULL) {
        if (strstr(line, "/mnt/sdcard") != NULL) {
            i = 0;
            break;
        }
    }

    fclose(fp);
    return i;
}

/**
 * \fn void sdcard_mount_handler(void)
 * \brief
 *
 * \param
 * \return
 */
int sdcard_mount_handler(void)
{
    int i = 0;
    int count_sleep = 0;

    ALOGD("sdcard_mount_handler is launched\n");

    /* Wait for SDCARD mount */
    while (count_sleep < 30) {
        sleep(2);
        count_sleep++;
        i = sdcard_check_mount();

        if (i == 0) {
            ALOGD("Sdcard is mounted\n");
            break;
        }
    }

    ALOGD("sdcard_mount_handler is closed\n");
    return i;
}

/**
 * \fn void cleanup(void)
 * \brief
 *
 * \param
 * \return
 */
static void cleanup(void)
{
    free((char*) command);
}

/**
 * \fn int main()
 * \brief
 *
 * \param
 * \return
 */
int main()
{
    int ret = 0;

    /* Allocate memory for the command */
    command = (char *)malloc(MAX_CMD_LEN);
    if (command == NULL) {
        ALOGE("Failed to allocate memory for command");
        return -1;
    }
    ret = parse_autoconf();

    if (ret != 0) {
        ALOGE("Error parsing \"autologcat.conf\". Aborting..\n\n");
        usage();
        goto exit;
    }

    /* Check if sdcard is in the path */
    if (strstr(command, "sdcard") != NULL)
    {
        ret = sdcard_mount_handler();
        if (ret != 0) {
            ALOGE("SDcard not mounted. Aborting...");
            goto exit;
        }
    }

    /* Execute the logcat command */
    ret = system(command);
    ALOGD("Command issued: %s\nReturn value: %d.\n", command, ret);
    if (ret != 0) {
        ALOGE("Command failed.\nReturn value: %d.\n", ret);
        goto exit;
    }

    cleanup();
    return 0;
exit:
    cleanup();
    return -1;
}
