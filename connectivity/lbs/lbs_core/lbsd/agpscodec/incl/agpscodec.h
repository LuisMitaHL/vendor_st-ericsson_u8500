/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef _AGPS_CODEC_H
#define _AGPS_CODEC_H

#ifndef DEBUG_LOG_PRINT
#ifndef ARM_LINUX_AGPS_FTR

#define DEBUG_LOG_PRINT( X )                ALOGD X
#else
#define DEBUG_LOG_PRINT( X )                printf X
#endif

#ifdef DEBUG_LOG_LEV2
#define DEBUG_LOG_PRINT_LEV2( X )   DEBUG_LOG_PRINT( X )
#else  /* DEBUG_LOG_LEV2 */
#define DEBUG_LOG_PRINT_LEV2( X )
#endif /* DEBUG_LOG_LEV2 */

#endif

typedef struct
{
    char *p_Buffer;
    int v_Length;
} t_Buffer;

int agpscodec_EncodeCommand(t_agps_Command *command , t_Buffer *p_Buffer );

uint32_t agpscodec_DecodeCommand(t_agps_Command *command, t_Buffer v_Buffer);

/* +ER:270717 */
#define AGPS_CODEC_HEADER                0xA53C7E5A
#define AGPS_CODEC_HEADER_SIZE            4
#define AGPS_CODEC_CS_SIZE                1
#define AGPS_CODEC_HEADER_N_CS_SIZE        (AGPS_CODEC_HEADER_SIZE+AGPS_CODEC_CS_SIZE)
#define AGPS_CODEC_LENGTH_N_TYPE_SIZE    (sizeof(int)*2)
/* -ER:270717 */

#endif /* #ifndef _AGPS_CODEC_H */
