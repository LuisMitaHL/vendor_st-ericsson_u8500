/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STEDUMP_H_
#define _STEDUM_H_

typedef struct {
    char name[8];
    char * circ_buf_ptr;
    int * write_ptr_offset;
    int circular_buffer_full;
    int discardable_bytecount;
} mpc_ring_buf;


void log_to_ring_buffer(mpc_ring_buf * struct_p,const char * buf,int read_count); 

void mm_stedump(int admsrv,int mediaserver,int isfatal);
int dump_mpc_circularbuffer(mpc_ring_buf * struct_p);
#define DUMP_MEDIASERVER "/data/ste-debug/dump/mediaserver"
#define DUMP_ADMSRV "/data/ste-debug/dump/admsrv"

#define ADMSRV "/system/bin/admsrv"
#define MEDIASERVER "/system/bin/mediaserver"

#define CHUNK_SIZE 1024
#define PROPERTY_VALUE_MAX 92

#define STE_DEBUG_DIR "/data/ste-debug"
#define STE_DEBUG_DUMP_TRIGGER STE_DEBUG_DIR "/stedump"

#endif


