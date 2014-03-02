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

/* Generated ee/api/panic.idt defined type */
#if !defined(__INC_PANIC_IDT)
#define __INC_PANIC_IDT


typedef t_uint8 t_panic_reason;

typedef enum t_xyuv_t_panic_reasonDescription {
  INTERNAL_PANIC=1,
  MPC_NOT_RESPONDING_PANIC=2,
  USER_STACK_OVERFLOW=3,
  SYSTEM_STACK_OVERFLOW=4,
  UNALIGNED_LONG_ACCESS=5,
  EVENT_FIFO_OVERFLOW=6,
  PARAM_FIFO_OVERFLOW=7,
  INTERFACE_NOT_BINDED=8,
  USER_PANIC=9,
  UNBIND_INTERRUPT=10,
  EVENT_FIFO_IN_USE=11,
  RESERVED_PANIC=2} t_panic_reasonDescription;

typedef enum t_xyuv_t_panic_source {
  HOST_EE,
  MPC_EE} t_panic_source;

#endif
