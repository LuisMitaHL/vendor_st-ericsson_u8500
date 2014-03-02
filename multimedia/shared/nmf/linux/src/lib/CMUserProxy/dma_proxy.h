/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef COMMON_DMA_PROXY_H
#define COMMON_DMA_PROXY_H

#include <cm/inc/cm_type.h>
#include <cm/engine/memory/inc/memory_type.h>

PUBLIC IMPORT_SHARED t_cm_error CM_SetupRelinkAreaMem2Per(
    t_cm_memory_handle from_addr,
    unsigned int to_addr,
    unsigned int segments,
    unsigned int segmentsize,
    unsigned int LOS);

PUBLIC IMPORT_SHARED t_cm_error CM_SetupRelinkAreaPer2Mem(
    t_cm_memory_handle from_addr,
    unsigned int to_addr,
    unsigned int segments,
    unsigned int segmentsize,
    unsigned int LOS);
    
#endif /* COMMON_DMA_PROXY_H */
