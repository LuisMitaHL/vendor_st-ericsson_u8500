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

/* Generated grabctl_types.idt defined type */
#if !defined(_grabctl_types_idt)
#define _grabctl_types_idt


enum e_grabctlCfgIdx {
  GRABCTL_CFG_ALPHA=0x1,
  GRABCTL_CFG_CAPTURING=0x2};

enum e_grabctlPortID {
  GRBCTLPID_PORT_0=0,
  GRBCTLPID_PORT_1,
  GRBCTLPID_PORT_2,
  GRBCTLPID_PORT_3};

enum e_capturing {
  GRBCTL_CAPT_NOTCAPTURING=0,
  GRBCTL_CAPT_PENDING_EOS=1,
  GRBCTL_CAPT_CAPTURING=2,
  GRBCTL_CAPT_ONE_SHOT=3,
  GRBCTL_CAPT_STOP_NOW=4,
  GRBCTL_CAPT_CAPTURING_KEEP_BUFFERS=5,
  GRBCTL_CAPT_BURST=6,
  GRBCTL_CAPT_AFTERBML_1=7,
  GRBCTL_CAPT_AFTERBML_2=8};

enum e_AbortRequestType {
  GRABCTL_ABORT_NONE=0,
  GRABCTL_ABORT_CLIENT,
  GRABCTL_ABORT_INTERNAL};

enum e_WhatNextAfterAbort {
  GARBCTL_INFORM_NONE=0,
  GARBCTL_INFORM_CLIENT,
  GRABCTL_PORT_FLUSH,
  GRABCTL_PRECAPTURE,
  GRABCTL_CMD_IDLE};

#endif
