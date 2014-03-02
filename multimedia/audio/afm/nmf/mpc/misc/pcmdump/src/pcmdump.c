/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   pcmdump.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/

#include <pcmdump.nmf>
#include <dbc.h>
#include <fsm/generic/include/FSM.h>
#include <pcmdump/complete.idt>


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_mpc_misc_pcmdump_src_pcmdumpTraces.h"
#endif

typedef struct sIpcmdump_complete {
  void* THIS;
  void (*pcmdump_complete)(PcmDumpCompleteInfo_t completeInfo);
} Ipcmdump_complete;

#define MAXITERCOUNT 256    // 2^n such as 2^n<=1023 (if MAXITERCOUNT=1023, compiler adds a divide which is not optimized)

static void inline c24to16(t_sint16 * dest, t_sint24 * src, int size) {
    int i, n;

#ifdef __flexcc2__
    #pragma loop maxitercount(MAXITERCOUNT)
#endif
    for(n = (int) size; n > MAXITERCOUNT; n -= MAXITERCOUNT)
    {
        for (i = 0; i < MAXITERCOUNT; i++) {
            *dest = wmsr(waddsat(*src, 0x80), 8);
            src++;
            dest++;
        }
    }
#ifdef __flexcc2__
    #pragma loop maxitercount(MAXITERCOUNT)
#endif
    for (i = 0; i < n; i++) {
        *dest = wmsr(waddsat(*src, 0x80), 8);
        src++;
        dest++;
    }
}

#pragma noprefix
#pragma nopid
extern void *THIS;

void configure(PcmDumpConfigure_t sConfigure, void * trace_p, const void* complete_cb, PcmDump_t * config)
{
    if (sConfigure.enable == false)
    {
        // we're disabling this probe, so send
        // the remaining probed samples to the proxy
        void *savedTHIS = THIS;
        Ipcmdump_complete * cb = (Ipcmdump_complete *)config->complete_cb;
        PcmDumpCompleteInfo_t sCompleteInfo;
        sCompleteInfo.omx_port_idx          = sConfigure.omx_port_idx;
        sCompleteInfo.effect_position_idx   = config->effect_position_idx;
        sCompleteInfo.bufferIdx             = config->buffer_write_id;
        sCompleteInfo.offset                = config->buffer_write_offset;
        THIS = cb->THIS;
        cb->pcmdump_complete(sCompleteInfo);
        THIS = savedTHIS;
    }
    else
    {
        config->buffer = sConfigure.buffer;
        config->buffer_size = sConfigure.buffer_size/2;
        config->buffer_write_offset = 0;
        config->trace_p = (TRACE_t*)trace_p;
        config->complete_cb = complete_cb;
        config->buffer_write_id = 0;
        config->omx_port_idx = sConfigure.omx_port_idx;
        config->effect_position_idx = sConfigure.effect_position_idx;
    }
    config->is_enabled = sConfigure.enable;
}

// note: size is in dsp words
BOOL probe(void * buf, t_uint24 size, PcmDump_t * config)
{
    int i;
    t_uint16 * dest;
    t_uint24 * src = (t_uint24 *)buf;
    t_uint24 offset = config->buffer_write_offset;
    t_uint24 dest_buffer_size = config->buffer_size;
    t_uint24 * buffer_start = config->buffer;
    TRACE_t * this = (TRACE_t *)config->trace_p;

    if ((size == 0) || (config->is_enabled == false))
    {
        return false;
    }

    OstTraceFiltInst2(TRACE_DEBUG, "AFM_Mpc: probe: args are buf: 0x%x, size: %d", (t_uint24)buf, size);

   // check for a write request outside of the buffer
    if ((offset + size) > dest_buffer_size)
    {
        OstTraceFiltInst4(TRACE_ERROR, "AFM_Mpc: probe: request to write outside of dump buffer 0x%x offset %u, size %u, total size %u", (t_uint24)config->buffer, offset, size, dest_buffer_size);
        return false;
    }

    dest = buffer_start + offset;

    if (config->buffer_write_id == 1)
    {
        dest += dest_buffer_size;
    }

    OstTraceFiltInst3(TRACE_DEBUG, "AFM_Mpc: probe: copy from src: 0x%x to dest: 0x%x, size: %d", (t_uint24)src, (t_uint24)dest, size);

    // copy the buffer content to the shared buffer
    c24to16(dest, src, size);

    dest += size;

    if ((dest == (t_uint24*)(buffer_start + dest_buffer_size)) || (dest == (t_uint24*)(buffer_start + 2*dest_buffer_size)))
    {
        // we've fullfilled the shared buffer area, signal it to the proxy!
        Ipcmdump_complete * cb = (Ipcmdump_complete *)config->complete_cb;
        void *savedTHIS = THIS;
        PcmDumpCompleteInfo_t sCompleteInfo;
        sCompleteInfo.omx_port_idx          = config->omx_port_idx;
        sCompleteInfo.effect_position_idx   = config->effect_position_idx;
        sCompleteInfo.bufferIdx             = config->buffer_write_id;
        sCompleteInfo.offset                = REGULAR_COMPLETE_CALL;

        OstTraceFiltInst1(TRACE_DEBUG, "AFM_Mpc: probe: call proxy complete: %d", config->buffer_write_offset);

        THIS = cb->THIS;
        (*cb->pcmdump_complete)(sCompleteInfo);
        THIS = savedTHIS;

        // and change write index
        config->buffer_write_id ^= 1;
        config->buffer_write_offset = 0;
        return true;
    }
    else
    {
        // simply increment the write offset
        config->buffer_write_offset += size;
        OstTraceFiltInst1(TRACE_DEBUG, "AFM_Mpc: probe: increment write offset: %d", config->buffer_write_offset);
        return false;
    }
}
