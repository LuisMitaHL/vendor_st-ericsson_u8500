/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*	no_control.h	*/

#ifndef __NO_CONTROL
#define __NO_CONTROL

void			mp4e_NoBRC_InitPict(void);
t_uint16		mp4e_NoBRC_PostPict(void);
t_sint32		mp4e_NoBRC_StuffingMB(t_sint32 inter, t_sint32 used_bits, t_sint32 MB_index, t_sint32 VP_occupancy, t_sint32 VP_stuff);


#endif /*	__NO_CONTROL	*/
