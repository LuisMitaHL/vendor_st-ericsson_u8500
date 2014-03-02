/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file issw_printf.h
 * \author ST Ericsson
 *
 * This header file provides printf support.
 *
 */
/*---------------------------------------------------------------------------*/
#ifndef _ISSW_PRINTF_H
#define _ISSW_PRINTF_H

#ifdef __ENABLE_PRINTF

#include <stdarg.h>
#include "hcl_defs.h"

extern int issw_printf_init(void);
extern void issw_puts(const char *s);
extern void issw_put_hex32(t_uint32 v, int width);
extern void issw_printf(const char *fmt, ...);

extern void issw_hex_print_buf(const t_uint8 *buf, t_uint32 bsize);

#define PRINTF_INIT issw_printf_init()
#define PUTS(_x_) issw_puts(_x_)
#define PUT_HEX32(_x_) issw_put_hex32(_x_, 8)
#define PUT_PTR(_x_) issw_put_hex32((t_uint32)(_x_), 8)
#define PRINTF(...) issw_printf(__VA_ARGS__)
#define HEX_PRINT_BUF(buf, bsize) issw_hex_print_buf((buf), (bsize))

#else /* No printfs */

#define PRINTF_INIT
#define PUTS(_x_)
#define PUT_HEX32(_x_)
#define PUT_PTR(_x_)
#define PRINTF(...)
#define HEX_PRINT_BUF(buf, bsize)

#endif /* !__ENABLE_PRINTF*/

#define init_board() PRINTF_INIT

#endif /* _ISSW_PRINTF_H*/
