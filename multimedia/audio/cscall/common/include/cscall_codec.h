/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __cscall_codech
#define __cscall_codech

// FIXME: need to be removed
#include <amr/nmfil/host/encoder/config.idt.h>
#include <amr/nmfil/host/decoder/config.idt.h>
#include <amrwb/nmfil/host/encoder/config.idt.h>
#include <amrwb/nmfil/host/decoder/config.idt.h>
#include <gsmfr/nmfil/host/encoder/config.idt.h>
#include <gsmfr/nmfil/host/decoder/config.idt.h>
#include <gsmhr/nmfil/host/encoder/config.idt.h>
#include <gsmhr/nmfil/host/decoder/config.idt.h>
#include <cscall/nmf/host/pcm_codec/common/config.idt>

// end FIXME

typedef struct {
    AmrDecParams_t         amrnbDecParam;
    AmrDecConfig_t         amrnbDecConfig;
    AmrEncParam_t          amrnbEncParam;
    AmrEncConfig_t	       amrnbEncConfig;
    NmfGsmfrDecParams_t    gsmfrDecParam;
	NmfGsmfrDecConfig_t    gsmfrDecConfig;
    NmfGsmfrEncParams_t    gsmfrEncParam;
	NmfGsmfrEncConfig_t    gsmfrEncConfig;
    GsmhrDecParams_t       gsmhrDecParam;  
	GsmhrDecConfig_t       gsmhrDecConfig;
    GsmhrEncParams_t       gsmhrEncParam;
	GsmhrEncConfig_t       gsmhrEncConfig;
    AmrwbDecParams_t       amrwbDecParam;
    AmrwbDecConfig_t       amrwbDecConfig;
    AmrwbEncParam_t        amrwbEncParam;
    AmrwbEncConfig_t	   amrwbEncConfig;
    PcmCodecConfig_t       pcmDecConfig;
    PcmCodecConfig_t       pcmEncConfig;
} Cscall_Codec_Config_t;




typedef struct {
    t_sint32		NextSFN;
    t_sint32		RequestedSFN;
    t_sint32		Nsync;
	t_sint32		encDtx;
	
} Cscall_Codec_Status_t;

#endif // __cscall_codech

