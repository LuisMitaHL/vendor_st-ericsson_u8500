/*
 * Positioning Manager
 *
 * submodule_test_main.c.bak
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <agpsosa.h>

#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

#include <test_engine_process.h>

/*** MAIN ******/
int main(int argc, char **argv)
{

    argc=argc;
    argv=argv;
    INF("*** Test Engine main entry ***\n");

    TSTENG_init();

    while (1) {
    OSA_Sleep(1000);
    }

    INF("*** Test Engine exit ***\n");
    return 0;
}
