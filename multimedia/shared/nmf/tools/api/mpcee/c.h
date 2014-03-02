/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*
 * c.h
 *
 *  Created on: Feb 4, 2009
 *      Author: fassino
 */

#ifndef C_H_
#define C_H_

#include <inc/nmf_type.idt>

extern void Unbinded(void *returnAddr);
#define IS_NULL_INTERFACE(itf, meth) (itf.meth == 0x0)

#endif /* C_H_ */
