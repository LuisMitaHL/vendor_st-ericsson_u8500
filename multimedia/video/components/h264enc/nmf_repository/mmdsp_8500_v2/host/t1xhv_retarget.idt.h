/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Generated t1xhv_retarget.idt defined type */
#if !defined(_t1xhv_vdc_retarget_idt)
#define _t1xhv_vdc_retarget_idt


typedef t_uint32 t_ulong_value;

typedef t_sint32 t_long_value;

typedef t_uint16 t_ushort_value;

typedef t_sint16 t_short_value;

typedef t_uint32 t_ahb_address;

typedef t_long_value t_time_stamp;

struct rational {
  t_long_value div;
  t_ulong_value rem;
  t_ushort_value base;
  t_ushort_value padding;
};

typedef struct rational ts_rational;

struct rational_32b {
  t_uint32 div;
  t_uint32 rem;
  t_ushort_value base;
  t_ushort_value padding;
};

typedef struct rational_32b ts_rational_32b;

struct fps {
  t_ulong_value num;
  t_ulong_value den;
};

typedef struct fps ts_fps;

#endif
