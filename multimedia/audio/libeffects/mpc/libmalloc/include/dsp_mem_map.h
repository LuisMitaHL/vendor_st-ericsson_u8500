/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */


#ifndef __dsp_mem_map_h__
#define __dsp_mem_map_h__

#define U8500_TCM_BASE_ADDRESS      0x00000000UL
#define U8500_TCM_SIZE              ((unsigned long)(32 * 1024))

#define U8500_ESRAM_24_BASE_ADDRESS 0x00400000UL
#define U8500_ESRAM_24_SIZE         ((unsigned long)(64 * 1024))

#define U8500_ESRAM_16_BASE_ADDRESS 0x00BC0000UL
#define U8500_ESRAM_16_SIZE         ((unsigned long)(64 * 1024))

#define U8500_DDR_24_BASE_ADDRESS   0x00010000UL
#define U8500_DDR_24_SIZE           ((unsigned long)(1024 * 1024))

#define U8500_DDR_16_BASE_ADDRESS   0x00800000UL
#define U8500_DDR_16_SIZE           ((unsigned long)(2048 * 1024))

#endif //__dsp_mem_map_h__



