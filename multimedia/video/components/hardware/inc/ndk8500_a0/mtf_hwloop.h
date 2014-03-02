/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/****************************************************************************
 * Memory Transfert FIFO commands using hardware loop => limited to transfer
 * size < 1023 16bit words.
 */
/*****************************************************************************/

#ifndef _MTF_HWLOOP_H_
#define _MTF_HWLOOP_H_

/*
 * Includes						       
 */
#include "mtf_api.h"

void mtf_read_table_hwloop(t_uint32 addr_in,
                     t_uint16 *pt_dest,
                     t_uint16 len,
                     t_uint16 incr,
                     t_mtf_swap_cfg swap_cfg);
                     
void mtf_write_table_hwloop(t_uint32 addr_out,
                     t_uint16 *pt_src,
                     t_uint16 len,
                     t_uint16 incr,
                     t_mtf_swap_cfg swap_cfg);

#endif /* _MTF_HWLOOP_H_ */

