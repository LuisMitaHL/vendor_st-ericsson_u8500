/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

/*------------------------------------------------------------------------
 * Include
 *----------------------------------------------------------------------*/

/*
 * Define
 */
#define TRUE 1
#define FALSE 0

#define NB_INTERRUPT 32

/* Structure for interrupt */
typedef struct {
	t_uint16      registered;
	/*t_uint16      itremap;
	t_uint16      old_itremap;*/
	Iresource_manager_api_signal *cb;
} ts_interrupt_desc, *tsp_interrupt_desc;

#endif /* INTERRUPT_H_ */

