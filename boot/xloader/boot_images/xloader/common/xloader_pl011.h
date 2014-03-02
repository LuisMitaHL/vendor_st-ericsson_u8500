/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _XLOADER_PL011_H
#define _XLOADER_PL011_H

#include "nomadik_registers.h"
#include "nomadik_mapping.h"


#define UART_PUTC(X) pl011_putc(X)
#define UART_INIT pl011_init


extern int pl011_init(void);
extern void pl011_putc(char c);
extern t_bool pl011_is_tx_fifo_empty(volatile t_uart_registers *p_Regs);

#endif
