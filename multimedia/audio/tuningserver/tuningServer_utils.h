/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ATS_UTILS_H
#define ATS_UTILS_H 1

#include <unistd.h>
#include <sys/types.h>

// Use to prevent annoying warnings if parameters are not used.
#define IDENTIFIER_NOT_USED(x) (void)x;

// I like to have booleans!
#define false 0
#define true (!false)
typedef unsigned int boolean;


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * Converts a number to a string given the base.,
     */
    char *itoa(int val, int base);

    /*
     * Crops "what" starting at the beginning of the strimg to be trimmed.
     */
    int trim(char *trimstring, const char *what);

    /*
     * Returns the pid of the process name which is given as the function
     * argument. In case no process found with the given name -1 will be returned.
     * The excluded PID is used to exclude your own process id from the search.
     */
    pid_t pidof(const char *p_processname, pid_t excludedpid);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* utils.h  */
