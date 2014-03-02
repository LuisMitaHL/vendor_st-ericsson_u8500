/*-----------------------------------------------------------------------------
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: jens.wiklander@stericsson.com
 *---------------------------------------------------------------------------*/
#ifndef COPS_TA_H
#define COPS_TA_H

#include <stdint.h>

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

extern const uint8_t *cops_ta_list[];
extern const uint32_t cops_ta_size_list[];

extern const uint8_t  number_of_tas;

#endif /*COPS_TA_H*/
