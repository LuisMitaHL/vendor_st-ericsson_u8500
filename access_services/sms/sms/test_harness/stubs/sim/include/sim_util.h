#ifndef INCLUSION_GUARD_SIM_UTIL_H
#define INCLUSION_GUARD_SIM_UTIL_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdint.h>
#include "sim.h"
#include "sim_stub.h"


/*------------------------------------------------------------------------------
 * Public function prototypes
 *------------------------------------------------------------------------------
 */
void *sim_util_readsocket(
    int,
    size_t,
    ssize_t *);

int sim_util_writesocket(
    int,
    const char *,
    const char *,
    const size_t);

char *sim_util_enc(
    char *,
    const void *,
    size_t);

const char *sim_util_dec(
    const char *,
    void *,
    size_t,
    const char *);

void *sim_util_encode_generic(
    uint16_t,
    const char *,
    size_t,
    uintptr_t,
    size_t *);

void *sim_util_decode_generic(
    const char *,
    const ssize_t,
    uint16_t *,
    uint16_t *,
    uintptr_t *,
    size_t *);

#endif                          // INCLUSION_GUARD_SIM_UTIL_H
