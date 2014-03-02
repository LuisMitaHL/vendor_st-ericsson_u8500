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

/* Generated C++ skeleton for 'shared_emptythisbuffer' */
#include <host/shared_emptythisbuffer.hpp>
static void sk_shared_emptythisbuffer_emptyThisBuffer(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  shared_emptythisbufferDescriptor *self = (shared_emptythisbufferDescriptor *)itfref;
  t_uint32 buffer;
    /* buffer <t_uint32> marshalling */
  buffer = ((t_uint32)_xyuv_data[0] | ((t_uint32)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->emptyThisBuffer(buffer);
}

static t_jump_method JT_shared_emptythisbuffer[] = {
  (t_jump_method)sk_shared_emptythisbuffer_emptyThisBuffer,
};

/* Generated C++ skeleton for 'shared_fillthisbuffer' */
#include <host/shared_fillthisbuffer.hpp>
static void sk_shared_fillthisbuffer_fillThisBuffer(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  shared_fillthisbufferDescriptor *self = (shared_fillthisbufferDescriptor *)itfref;
  t_uint32 buffer;
    /* buffer <t_uint32> marshalling */
  buffer = ((t_uint32)_xyuv_data[0] | ((t_uint32)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->fillThisBuffer(buffer);
}

static t_jump_method JT_shared_fillthisbuffer[] = {
  (t_jump_method)sk_shared_fillthisbuffer_fillThisBuffer,
};

/* Generated C++ skeleton for 'eventhandler' */
#include <host/eventhandler.hpp>
static void sk_eventhandler_eventHandler(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  eventhandlerDescriptor *self = (eventhandlerDescriptor *)itfref;
  OMX_EVENTTYPE ev;
  t_uint32 data1;
  t_uint32 data2;
    /* ev <OMX_EVENTTYPE> marshalling */
  ev = (OMX_EVENTTYPE)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* data1 <t_uint32> marshalling */
  data1 = ((t_uint32)_xyuv_data[2] | ((t_uint32)_xyuv_data[2+1] << 16));
    /* data2 <t_uint32> marshalling */
  data2 = ((t_uint32)_xyuv_data[4] | ((t_uint32)_xyuv_data[4+1] << 16));
  /* Server calling */
  self->eventHandler(ev, data1, data2);
}

static t_jump_method JT_eventhandler[] = {
  (t_jump_method)sk_eventhandler_eventHandler,
};

static t_nmf_skel_function ens_cpp_skel_functions[] = {
  {"shared_emptythisbuffer", JT_shared_emptythisbuffer},
  {"shared_fillthisbuffer", JT_shared_fillthisbuffer},
  {"eventhandler", JT_eventhandler},
};

/*const */t_nmf_skel_register ens_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(ens_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    ens_cpp_skel_functions
};
