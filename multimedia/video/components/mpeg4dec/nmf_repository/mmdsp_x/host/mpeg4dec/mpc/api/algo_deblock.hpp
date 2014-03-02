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

/* 'mpeg4dec.mpc.api.algo_deblock' interface */
#if !defined(mpeg4dec_mpc_api_algo_deblock_IDL)
#define mpeg4dec_mpc_api_algo_deblock_IDL

#include <cpp.hpp>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vdc_mpeg4.idt.h>


class mpeg4dec_mpc_api_algo_deblockDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void configureAlgo(t_uint16 Width, t_uint16 Height, t_uint32 PostFilterLevel, t_uint32 MQ_Mode, t_uint32 QP_p, t_uint32 Input_p, t_uint32 Output_p, t_uint32 Temp_p) = 0;
    virtual void controlAlgo(t_t1xhv_command command, t_uint16 param) = 0;
};

class Impeg4dec_mpc_api_algo_deblock: public NMF::InterfaceReference {
  public:
    Impeg4dec_mpc_api_algo_deblock(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void configureAlgo(t_uint16 Width, t_uint16 Height, t_uint32 PostFilterLevel, t_uint32 MQ_Mode, t_uint32 QP_p, t_uint32 Input_p, t_uint32 Output_p, t_uint32 Temp_p) {
      ((mpeg4dec_mpc_api_algo_deblockDescriptor*)reference)->configureAlgo(Width, Height, PostFilterLevel, MQ_Mode, QP_p, Input_p, Output_p, Temp_p);
    }
    void controlAlgo(t_t1xhv_command command, t_uint16 param) {
      ((mpeg4dec_mpc_api_algo_deblockDescriptor*)reference)->controlAlgo(command, param);
    }
};

#endif
