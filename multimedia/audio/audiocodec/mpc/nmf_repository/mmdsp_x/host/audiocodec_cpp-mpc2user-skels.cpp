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

#include <cm/proxy/api/private/stub-requiredapi.h>

/* Generated C++ skeleton for 'dmaout.low_signal' */
#include <host/dmaout/low_signal.hpp>
static void sk_dmaout_low_signal_lowPowerMode(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  dmaout_low_signalDescriptor *self = (dmaout_low_signalDescriptor *)itfref;
  t_uint16 framecount;
    /* framecount <t_uint16> marshalling */
  framecount = ((t_uint16)_xyuv_data[0]);
  /* Server calling */
  self->lowPowerMode(framecount);
}

static void sk_dmaout_low_signal_normalMode(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  dmaout_low_signalDescriptor *self = (dmaout_low_signalDescriptor *)itfref;
  /* Server calling */
  self->normalMode();
}

static void sk_dmaout_low_signal_acknowledgeRmForLowPowerMode(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  dmaout_low_signalDescriptor *self = (dmaout_low_signalDescriptor *)itfref;
  /* Server calling */
  self->acknowledgeRmForLowPowerMode();
}

static void sk_dmaout_low_signal_lowPowerModeSetup(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  dmaout_low_signalDescriptor *self = (dmaout_low_signalDescriptor *)itfref;
  t_uint16 port_idx;
    /* port_idx <t_uint16> marshalling */
  port_idx = ((t_uint16)_xyuv_data[0]);
  /* Server calling */
  self->lowPowerModeSetup(port_idx);
}

static void sk_dmaout_low_signal_fifoEmptied(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  dmaout_low_signalDescriptor *self = (dmaout_low_signalDescriptor *)itfref;
  /* Server calling */
  self->fifoEmptied();
}

static void sk_dmaout_low_signal_digitalMute(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  dmaout_low_signalDescriptor *self = (dmaout_low_signalDescriptor *)itfref;
  t_uint16 mute;
    /* mute <t_uint16> marshalling */
  mute = ((t_uint16)_xyuv_data[0]);
  /* Server calling */
  self->digitalMute(mute);
}

static void sk_dmaout_low_signal_portMuted(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  dmaout_low_signalDescriptor *self = (dmaout_low_signalDescriptor *)itfref;
  t_uint16 port_idx;
    /* port_idx <t_uint16> marshalling */
  port_idx = ((t_uint16)_xyuv_data[0]);
  /* Server calling */
  self->portMuted(port_idx);
}

static t_jump_method JT_dmaout_low_signal[] = {
  (t_jump_method)sk_dmaout_low_signal_lowPowerMode,
  (t_jump_method)sk_dmaout_low_signal_normalMode,
  (t_jump_method)sk_dmaout_low_signal_acknowledgeRmForLowPowerMode,
  (t_jump_method)sk_dmaout_low_signal_lowPowerModeSetup,
  (t_jump_method)sk_dmaout_low_signal_fifoEmptied,
  (t_jump_method)sk_dmaout_low_signal_digitalMute,
  (t_jump_method)sk_dmaout_low_signal_portMuted,
};

/* Generated C++ skeleton for 'msp_dma_ctrl_ab8500.bitclock' */
#include <host/msp_dma_ctrl_ab8500/bitclock.hpp>
static void sk_msp_dma_ctrl_ab8500_bitclock_enable(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  msp_dma_ctrl_ab8500_bitclockDescriptor *self = (msp_dma_ctrl_ab8500_bitclockDescriptor *)itfref;
  t_bool on_off;
    /* on_off <t_bool> marshalling */
  on_off = ((t_bool)_xyuv_data[0]);
  /* Server calling */
  self->enable(on_off);
}

static t_jump_method JT_msp_dma_ctrl_ab8500_bitclock[] = {
  (t_jump_method)sk_msp_dma_ctrl_ab8500_bitclock_enable,
};

static t_nmf_skel_function audiocodec_cpp_skel_functions[] = {
  {"dmaout.low_signal", JT_dmaout_low_signal},
  {"msp_dma_ctrl_ab8500.bitclock", JT_msp_dma_ctrl_ab8500_bitclock},
};

/*const */t_nmf_skel_register audiocodec_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(audiocodec_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    audiocodec_cpp_skel_functions
};
