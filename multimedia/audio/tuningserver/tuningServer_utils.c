/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "tuningServer_utils.h"

char *itoa(int val, int base)
{

    static char buf[32] = {0};
    int i = 30;

    for (; val && i ; --i, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }

    return &buf[i+1];
}

int trim(char *trimstring, const char *what)
{
    int i = 0;
    int j = 0;

    char newstring[strlen(trimstring)];

    if (strstr(trimstring, what) != NULL) {
        i = strlen(what);
    }

    for (; trimstring[i]; i++, j++) {
        newstring[j] = trimstring[i];
    }

    newstring[j] = '\0';

    strcpy(trimstring, newstring);

    return 0;
}

pid_t pidof(const char *p_processname, pid_t excludepid)
{
    DIR *dir_p;
    struct dirent *dir_entry_p;
    char dir_name[40];
    char target_name[252];
    int target_result;
    char exe_link[252];
    pid_t result = -1;

    dir_p = opendir("/proc/");

    while (NULL != (dir_entry_p = readdir(dir_p))) {
        if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name)) {
            strcpy(dir_name, "/proc/");
            strcat(dir_name, dir_entry_p->d_name);
            strcat(dir_name, "/");
            exe_link[0] = 0;
            strcat(exe_link, dir_name);
            strcat(exe_link, "exe");
            target_result = readlink(exe_link, target_name, sizeof(target_name) - 1);

            if (target_result > 0) {
                target_name[target_result] = 0;

                if (strstr(target_name, p_processname) != NULL) {
                    result = (pid_t) atoi(dir_entry_p->d_name);

                    // We want to exclude ourselves when searching for the pid.
                    if (result != excludepid) {
                        closedir(dir_p);
                        return result;
                    } else {
                        result = -1;
                    }
                }
            }
        }
    }

    closedir(dir_p);
    return result;
}

