/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef MMPROBE_CLIENT_H_
#define MMPROBE_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "../api/t_mm_probe.h"
#include "../api/r_mm_probe.h"

#define probeHeader_t MM_ProbeHeader_V2_t
#define probe_t       MM_ProbeDataCB_V2_t

/**
 * Creates an empty probe
 */
extern probe_t* create_probe(int id);

/**
 * Delete and free up probe resources
 */
extern void delete_probe(probe_t* p);

/**
 * Sends the probe
 */
extern int send_probe(probe_t* p);

#ifdef __cplusplus
}
#endif

#endif /* MMPROBE_CLIENT_H_ */

