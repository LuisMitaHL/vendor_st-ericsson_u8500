/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mount.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "utils.h"
#include "kerneldump.h"

#define UNUSED(expr) ((void)(expr))

/**
 * \fn int sdcard_check_mount(void)
 * \brief
 *
 * \return 1 if sdcard is mounted
 *  0 if sdcard is not mounted
 * -1 in case of error.
 */
int sdcard_check_mount(void)
{
    int found = 0;
    FILE *fp;
    char line[CMD_LINE_LENGTH];

    /* Check sdcard mount */
    fp = fopen("/proc/mounts", "r");

    if (!fp) {
        ALOGW("Cannot open /proc/mounts");
        return -1;
    }

    while (fgets(line, CMD_LINE_LENGTH, fp) != NULL) {
        if (strstr(line, "/mnt/sdcard") != NULL) {
            found = 1;
            break;
        }
    }

    fclose(fp);
    return found;
}

/**
 * \fn int sdcard_unmount(void)
 * \brief
 *
 * \return 0 if sdcard is unmounted successfully
 * -1 otherwise.
 */
int sdcard_unmount(void)
{
    int ret = -1;
    ret = umount2("/mnt/sdcard", MNT_DETACH);

    if (ret == 0) {
        //Remove dir and symlink
        rmdir("/mnt/sdcard");
        remove("/sdcard");
    }

    return ret;
}

/**
 * \fn int sdcard_mount(void)
 * \brief
 *
 * \return 0 if sdcard is mounted successfully
 * -1 otherwise.
 */
int sdcard_mount(void)
{
    int ret = -1;
    mkdir("/mnt/sdcard", ALLPERMS);
    ret = mount("/dev/mmcblk2p1", "/mnt/sdcard", "vfat", 0, "");

    if (ret == 0) {
        //Add symlink
        symlink("/mnt/sdcard", "/sdcard");
    }

    return ret;
}

/**
 * \fn int main(int argc, char *argv[])
 * \brief coredumphandler main function
 *
 * \param argc
 * \param argv
 * \return 0
 */
int main(int argc, char *argv[])
{
    int ret = 0, mounted_here = 0;
    // Remove compiler warning
    UNUSED(argc);
    UNUSED(argv);

    ALOGD("Starting coredump-handler...\n");

    ret = sdcard_check_mount();

    switch (ret) {
    case 0:
        ALOGD("Sdcard not mounted, trying to mount it...\n");
        ret = sdcard_mount();

        if (ret != 0) {
            ALOGD("Failed to mount sdcard.\n");
        } else {
            ALOGD("Sdcard mounted!\n");
            mounted_here = 1;
            move_kerneldump();
        }

        break;
    case 1:
        ALOGD("Sdcard already mounted, check for coredumps...\n");
        move_kerneldump();
        break;
    default:
        // Error.
        ALOGD("coredump-handler failed to check sdcard mount.\n");
        break;
    }

    if (mounted_here == 1) {
        ALOGD("Unmounting sdcard since it was mounted by us.\n");
        ret = sdcard_unmount();

        if (ret != 0) {
            ALOGD("coredump-handler failed to unmount sdcard.\n");
        }
    }

    ALOGD("Leaving coredump-handler.\n");

    return 0;
}

