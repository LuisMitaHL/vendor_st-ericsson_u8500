/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   shared_bufout.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <nmf/mpc/shared_bufout.nmf>
#include <sharedbuffer.idt>
#include "inc/archi-wrapper.h"

void METH(fillThisBuffer) (t_uint32 sharedbuffer) {
    Buffer_t *buffer = (Buffer_t *)sharedbuffer;
    MMDSP_FLUSH_DCACHE();
    mpc.fillThisBuffer(buffer);
}

void METH(emptyThisBuffer) (Buffer_t * buffer) {
    SharedBuffer_t *sharedbuffer = (SharedBuffer_t *)buffer;
    host.emptyThisBuffer(sharedbuffer->hostHeaderAddr);
}
