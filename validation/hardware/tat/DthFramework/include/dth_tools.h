/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTH_TOOLS_H_
#define DTH_TOOLS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dthsrvhelper/dthsrvhelper.h>
#include <float.h>

typedef uint8_t      u8;
typedef int8_t		 s8;
typedef uint16_t     u16;
typedef int16_t      s16;
typedef uint32_t     u32;
typedef int32_t      s32;
typedef uint64_t     u64;
typedef int64_t      s64;

#define NO_GET	NULL
#define NO_SET	NULL
#define NO_EXEC	NULL
#define NO_ENUM	NULL
#define NO_INFO	NULL
#define SEP ","
#define MAX_CHAR_PER_LINE 4096
#define MAX_LOCAL_STRING_SIZE 256

#endif /* DTH_TOOLS_H_ */

