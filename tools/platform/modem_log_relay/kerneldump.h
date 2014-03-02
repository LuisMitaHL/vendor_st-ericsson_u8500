/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef KERNELDUMP_H
#define KERNELDUMP_H

#define KERNELDUMP_MSG              "Kernel coredump available "
#define KERNEL_MSG_DEV              "/dev/kmsg"

#define KERNELDUMP_DIR              "/kernel"

int move_kerneldump();

#endif /* ifndef KERNELDUMP_H */
