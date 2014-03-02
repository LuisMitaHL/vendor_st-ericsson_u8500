/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson.
 */

#ifndef __VERSION_H__
#define __VERSION_H__

/**
 * Will print the version string
 */
void version_print(void);

/**
 * This function will write the board version into backup RAM
 */
void version_write(void);

#endif /* __VERSION_H__ */

