/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shared_bufin.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <nmf/mpc/shared_bufin.nmf>
#include <sharedbuffer.idt>
#include "inc/archi-wrapper.h"

void METH(emptyThisBuffer) (t_uint32 sharedbuffer) {
    Buffer_t *buffer = (Buffer_t *)sharedbuffer;
    MMDSP_FLUSH_DCACHE();
    mpc.emptyThisBuffer(buffer);
}

void METH(fillThisBuffer) (Buffer_t * buffer) {
    SharedBuffer_t *sharedbuffer = (SharedBuffer_t *)buffer;
    host.fillThisBuffer(sharedbuffer->hostHeaderAddr);
}
