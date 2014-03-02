/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef R_MM_PROBE_H_
#define R_MM_PROBE_H_

#include "t_mm_probe.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Writes the array of data to the FIFO. (Version 1)
 * NOTE: Legacy not to be used for new probes!
 */
extern int mmprobe_probe(int id, void* data, int length);

/**
 * Writes the array of data to the FIFO. (Version 2)
 */
extern int mmprobe_probe_V2(int id, void* data, int length, DataFormat_t* format_p);

/**
 * Check if a probe is active
 * NOTE: Only valid for speech probes i.e. id < MM_PROBE_NUMBER_OF_SPEECH_PROBES
 */
extern int mmprobe_status(int id);


#ifdef __cplusplus
}
#endif

#endif /* R_MM_PROBE_H_ */
