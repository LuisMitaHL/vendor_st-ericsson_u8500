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

#include <cm/inc/cm.h>
#include <cm/proxy/api/private/stub-requiredapi.h>

/* Generated C++ stub for 'grabctl.api.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/grabctl/api/configure.hpp>

class grabctl_api_configure: public grabctl_api_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParams(struct s_grabParams GrabParams, enum e_grabctlPortID port_idx, t_uint16 fifoOutsize) {
      t_uint16 _xyuv_data[57];
      int ___i0, ___j0;
      /* GrabParams <struct s_grabParams> marshalling */
      _xyuv_data[0+0] = (t_uint16)((unsigned int)GrabParams.output_format & 0xFFFFU);
      _xyuv_data[0+0+1] = (t_uint16)((unsigned int)GrabParams.output_format >> 16);
      _xyuv_data[0+2] = (t_uint16)GrabParams.x_window_size;
      _xyuv_data[0+3] = (t_uint16)GrabParams.y_window_size;
      _xyuv_data[0+4] = (t_uint16)GrabParams.x_window_offset;
      _xyuv_data[0+5] = (t_uint16)GrabParams.y_window_offset;
      _xyuv_data[0+6] = (t_uint16)GrabParams.x_frame_size;
      _xyuv_data[0+7] = (t_uint16)GrabParams.y_frame_size;
      _xyuv_data[0+8] = (t_uint16)GrabParams.dest_buf_addr;
      _xyuv_data[0+8+1] = (t_uint16)(GrabParams.dest_buf_addr >> 16);
      _xyuv_data[0+10] = (t_uint16)GrabParams.buf_id;
      _xyuv_data[0+11] = (t_uint16)GrabParams.rotation_cfg;
      _xyuv_data[0+12] = (t_uint16)GrabParams.disable_grab_cache;
      _xyuv_data[0+13] = (t_uint16)GrabParams.cache_buf_addr;
      _xyuv_data[0+13+1] = (t_uint16)(GrabParams.cache_buf_addr >> 16);
      _xyuv_data[0+15] = (t_uint16)GrabParams.enable_stab;
      _xyuv_data[0+16] = (t_uint16)GrabParams.x_stab_size_lr;
      _xyuv_data[0+17] = (t_uint16)GrabParams.y_stab_size_lr;
      _xyuv_data[0+18] = (t_uint16)GrabParams.proj_h_buf_addr;
      _xyuv_data[0+18+1] = (t_uint16)(GrabParams.proj_h_buf_addr >> 16);
      _xyuv_data[0+20] = (t_uint16)GrabParams.proj_v_buf_addr;
      _xyuv_data[0+20+1] = (t_uint16)(GrabParams.proj_v_buf_addr >> 16);
      _xyuv_data[0+22] = (t_uint16)GrabParams.lr_alpha;
      _xyuv_data[0+23] = (t_uint16)GrabParams.keepRatio;
      _xyuv_data[0+24+0] = (t_uint16)((unsigned int)GrabParams.bml_params.pixel_order & 0xFFFFU);
      _xyuv_data[0+24+0+1] = (t_uint16)((unsigned int)GrabParams.bml_params.pixel_order >> 16);
      _xyuv_data[0+24+2] = (t_uint16)GrabParams.bml_params.nb_linetypes;
      for(___i0 = ___j0 = 0; ___i0 < 8; ___i0++) {
        _xyuv_data[0+24+3+___j0] = (t_uint16)((unsigned int)GrabParams.bml_params.line_type[___i0] & 0xFFFFU);
        _xyuv_data[0+24+3+___j0+1] = (t_uint16)((unsigned int)GrabParams.bml_params.line_type[___i0] >> 16);
        ___j0 += 2;
      }
      for(___i0 = ___j0 = 0; ___i0 < 8; ___i0++) {
        _xyuv_data[0+24+19+___j0] = (t_uint16)GrabParams.bml_params.line_type_nb[___i0];
        ___j0 += 1;
      }
      _xyuv_data[0+24+27] = (t_uint16)GrabParams.bml_params.interline_size;
      _xyuv_data[0+24+27+1] = (t_uint16)(GrabParams.bml_params.interline_size >> 16);
      _xyuv_data[0+24+29] = (t_uint16)GrabParams.bml_params.nb_embedded_lines;
      /* port_idx <enum e_grabctlPortID> marshalling */
      _xyuv_data[54] = (t_uint16)((unsigned int)port_idx & 0xFFFFU);
      _xyuv_data[54+1] = (t_uint16)((unsigned int)port_idx >> 16);
      /* fifoOutsize <t_uint16> marshalling */
      _xyuv_data[56] = (t_uint16)fifoOutsize;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 57*2, 0);
    }
    virtual void setConfig(enum e_grabctlPortID port_idx, enum e_grabctlCfgIdx param_idx, enum e_capturing capture_mode, t_uint8 param_data) {
      t_uint16 _xyuv_data[7];
      /* port_idx <enum e_grabctlPortID> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)port_idx & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)port_idx >> 16);
      /* param_idx <enum e_grabctlCfgIdx> marshalling */
      _xyuv_data[2] = (t_uint16)((unsigned int)param_idx & 0xFFFFU);
      _xyuv_data[2+1] = (t_uint16)((unsigned int)param_idx >> 16);
      /* capture_mode <enum e_capturing> marshalling */
      _xyuv_data[4] = (t_uint16)((unsigned int)capture_mode & 0xFFFFU);
      _xyuv_data[4+1] = (t_uint16)((unsigned int)capture_mode >> 16);
      /* param_data <t_uint8> marshalling */
      _xyuv_data[6] = (t_uint16)param_data;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 7*2, 1);
    }
    virtual void setConfigBMS(enum e_grabctlPortID port_idx, t_uint16 bufferAddressesH[6], t_uint16 bufferAddressesL[6], t_uint16 bufferCount, t_uint16 xsize, t_uint16 ysize, t_uint16 linelen, t_uint16 pixel_order) {
      t_uint16 _xyuv_data[19];
      int ___i0, ___j0;
      /* port_idx <enum e_grabctlPortID> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)port_idx & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)port_idx >> 16);
      /* bufferAddressesH <t_uint16*> marshalling */
      for(___i0 = ___j0 = 0; ___i0 < 6; ___i0++) {
        _xyuv_data[2+___j0] = (t_uint16)bufferAddressesH[___i0];
        ___j0 += 1;
      }
      /* bufferAddressesL <t_uint16*> marshalling */
      for(___i0 = ___j0 = 0; ___i0 < 6; ___i0++) {
        _xyuv_data[8+___j0] = (t_uint16)bufferAddressesL[___i0];
        ___j0 += 1;
      }
      /* bufferCount <t_uint16> marshalling */
      _xyuv_data[14] = (t_uint16)bufferCount;
      /* xsize <t_uint16> marshalling */
      _xyuv_data[15] = (t_uint16)xsize;
      /* ysize <t_uint16> marshalling */
      _xyuv_data[16] = (t_uint16)ysize;
      /* linelen <t_uint16> marshalling */
      _xyuv_data[17] = (t_uint16)linelen;
      /* pixel_order <t_uint16> marshalling */
      _xyuv_data[18] = (t_uint16)pixel_order;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 19*2, 2);
    }
    virtual void setClientID(t_uint8 client_id) {
      t_uint16 _xyuv_data[1];
      /* client_id <t_uint8> marshalling */
      _xyuv_data[0] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 3);
    }
    virtual void setPort2PipeMapping(enum e_grabPipeID pipe4port0, enum e_grabPipeID pipe4port1, enum e_grabPipeID pipe4port2) {
      t_uint16 _xyuv_data[6];
      /* pipe4port0 <enum e_grabPipeID> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)pipe4port0 & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)pipe4port0 >> 16);
      /* pipe4port1 <enum e_grabPipeID> marshalling */
      _xyuv_data[2] = (t_uint16)((unsigned int)pipe4port1 & 0xFFFFU);
      _xyuv_data[2+1] = (t_uint16)((unsigned int)pipe4port1 >> 16);
      /* pipe4port2 <enum e_grabPipeID> marshalling */
      _xyuv_data[4] = (t_uint16)((unsigned int)pipe4port2 & 0xFFFFU);
      _xyuv_data[4+1] = (t_uint16)((unsigned int)pipe4port2 >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 4);
    }
    virtual void setClockWaitingForStart(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 5);
    }
    virtual void setClockRunning(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 6);
    }
    virtual void setHiddenBMS(t_uint16 hidden_bms, t_uint16 nFramesBefore) {
      t_uint16 _xyuv_data[2];
      /* hidden_bms <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)hidden_bms;
      /* nFramesBefore <t_uint16> marshalling */
      _xyuv_data[1] = (t_uint16)nFramesBefore;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 7);
    }
    virtual void endAllCaptures(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 8);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_grabctl_api_configure(t_nmf_interface_desc* itf) {
  grabctl_api_configure *stub = new grabctl_api_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'ispctl.api.cmd'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/ispctl/api/cmd.hpp>

class ispctl_api_cmd: public ispctl_api_cmdDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void initISP(t_uint8 client_id) {
      t_uint16 _xyuv_data[1];
      /* client_id <t_uint8> marshalling */
      _xyuv_data[0] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 0);
    }
    virtual void traceInit(TraceInfo_t trace_info, t_uint16 id) {
      t_uint16 _xyuv_data[6];
      /* trace_info <TraceInfo_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)trace_info.parentHandle;
      _xyuv_data[0+0+1] = (t_uint16)(trace_info.parentHandle >> 16);
      _xyuv_data[0+2] = (t_uint16)trace_info.traceEnable;
      _xyuv_data[0+3] = (t_uint16)trace_info.dspAddr;
      _xyuv_data[0+3+1] = (t_uint16)(trace_info.dspAddr >> 16);
      /* id <t_uint16> marshalling */
      _xyuv_data[5] = (t_uint16)id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 1);
    }
    virtual void readPageElement(t_uint16 addr, t_uint8 client_id) {
      t_uint16 _xyuv_data[2];
      /* addr <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)addr;
      /* client_id <t_uint8> marshalling */
      _xyuv_data[1] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 2);
    }
    virtual void WaitExpectedPeValue(t_uint16 addr, t_uint32 value_expected, t_uint32 polling_frequency, t_uint32 timeout, t_uint8 client_id) {
      t_uint16 _xyuv_data[8];
      /* addr <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)addr;
      /* value_expected <t_uint32> marshalling */
      _xyuv_data[1] = (t_uint16)value_expected;
      _xyuv_data[1+1] = (t_uint16)(value_expected >> 16);
      /* polling_frequency <t_uint32> marshalling */
      _xyuv_data[3] = (t_uint16)polling_frequency;
      _xyuv_data[3+1] = (t_uint16)(polling_frequency >> 16);
      /* timeout <t_uint32> marshalling */
      _xyuv_data[5] = (t_uint16)timeout;
      _xyuv_data[5+1] = (t_uint16)(timeout >> 16);
      /* client_id <t_uint8> marshalling */
      _xyuv_data[7] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 8*2, 3);
    }
    virtual void readListPageElement(ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id) {
      t_uint16 _xyuv_data[194];
      int ___i0, ___j0;
      /* tab_pe <ts_PageElement*> marshalling */
      for(___i0 = ___j0 = 0; ___i0 < 64; ___i0++) {
        _xyuv_data[0+___j0+0] = (t_uint16)tab_pe[___i0].pe_addr;
        _xyuv_data[0+___j0+1] = (t_uint16)tab_pe[___i0].pe_data;
        _xyuv_data[0+___j0+1+1] = (t_uint16)(tab_pe[___i0].pe_data >> 16);
        ___j0 += 3;
      }
      /* number_of_pe <t_uint16> marshalling */
      _xyuv_data[192] = (t_uint16)number_of_pe;
      /* client_id <t_uint8> marshalling */
      _xyuv_data[193] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 194*2, 4);
    }
    virtual void writePageElement(t_uint16 addr, t_uint32 value, t_uint8 client_id) {
      t_uint16 _xyuv_data[4];
      /* addr <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)addr;
      /* value <t_uint32> marshalling */
      _xyuv_data[1] = (t_uint16)value;
      _xyuv_data[1+1] = (t_uint16)(value >> 16);
      /* client_id <t_uint8> marshalling */
      _xyuv_data[3] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 4*2, 5);
    }
    virtual void writeListPageElement(ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id) {
      t_uint16 _xyuv_data[194];
      int ___i0, ___j0;
      /* tab_pe <ts_PageElement*> marshalling */
      for(___i0 = ___j0 = 0; ___i0 < 64; ___i0++) {
        _xyuv_data[0+___j0+0] = (t_uint16)tab_pe[___i0].pe_addr;
        _xyuv_data[0+___j0+1] = (t_uint16)tab_pe[___i0].pe_data;
        _xyuv_data[0+___j0+1+1] = (t_uint16)(tab_pe[___i0].pe_data >> 16);
        ___j0 += 3;
      }
      /* number_of_pe <t_uint16> marshalling */
      _xyuv_data[192] = (t_uint16)number_of_pe;
      /* client_id <t_uint8> marshalling */
      _xyuv_data[193] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 194*2, 6);
    }
    virtual void updateGridironTable(t_uint32 addr, t_uint16 size, t_uint8 client_id) {
      t_uint16 _xyuv_data[4];
      /* addr <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)addr;
      _xyuv_data[0+1] = (t_uint16)(addr >> 16);
      /* size <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)size;
      /* client_id <t_uint8> marshalling */
      _xyuv_data[3] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 4*2, 7);
    }
    virtual void subscribeEvent(t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) {
      t_uint16 _xyuv_data[6];
      /* info_events <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)info_events;
      _xyuv_data[0+1] = (t_uint16)(info_events >> 16);
      /* error_events <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)error_events;
      /* debug_events <t_uint32> marshalling */
      _xyuv_data[3] = (t_uint16)debug_events;
      _xyuv_data[3+1] = (t_uint16)(debug_events >> 16);
      /* client_id <t_uint8> marshalling */
      _xyuv_data[5] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 8);
    }
    virtual void unsubscribeEvent(t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) {
      t_uint16 _xyuv_data[6];
      /* info_events <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)info_events;
      _xyuv_data[0+1] = (t_uint16)(info_events >> 16);
      /* error_events <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)error_events;
      /* debug_events <t_uint32> marshalling */
      _xyuv_data[3] = (t_uint16)debug_events;
      _xyuv_data[3+1] = (t_uint16)(debug_events >> 16);
      /* client_id <t_uint8> marshalling */
      _xyuv_data[5] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 9);
    }
    virtual void allowSleep(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 10);
    }
    virtual void preventSleep(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 11);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_ispctl_api_cmd(t_nmf_interface_desc* itf) {
  ispctl_api_cmd *stub = new ispctl_api_cmd();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'ispctl.api.cfg'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/ispctl/api/cfg.hpp>

class ispctl_api_cfg: public ispctl_api_cfgDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
};
static t_cm_bf_host2mpc_handle* INITstub_ispctl_api_cfg(t_nmf_interface_desc* itf) {
  ispctl_api_cfg *stub = new ispctl_api_cfg();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'api.set_debug'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/api/set_debug.hpp>

class api_set_debug: public api_set_debugDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setDebug(t_uint16 debug_mode, t_uint16 param1, t_uint16 param2) {
      t_uint16 _xyuv_data[3];
      /* debug_mode <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)debug_mode;
      /* param1 <t_uint16> marshalling */
      _xyuv_data[1] = (t_uint16)param1;
      /* param2 <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)param2;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 0);
    }
    virtual void readRegister(t_uint16 reg_adress) {
      t_uint16 _xyuv_data[1];
      /* reg_adress <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)reg_adress;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 1);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_api_set_debug(t_nmf_interface_desc* itf) {
  api_set_debug *stub = new api_set_debug();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'grab.api.cmd'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/grab/api/cmd.hpp>

class grab_api_cmd: public grab_api_cmdDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void traceInit(TraceInfo_t trace_info, t_uint16 id) {
      t_uint16 _xyuv_data[6];
      /* trace_info <TraceInfo_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)trace_info.parentHandle;
      _xyuv_data[0+0+1] = (t_uint16)(trace_info.parentHandle >> 16);
      _xyuv_data[0+2] = (t_uint16)trace_info.traceEnable;
      _xyuv_data[0+3] = (t_uint16)trace_info.dspAddr;
      _xyuv_data[0+3+1] = (t_uint16)(trace_info.dspAddr >> 16);
      /* id <t_uint16> marshalling */
      _xyuv_data[5] = (t_uint16)id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 0);
    }
    virtual void abort(enum e_grabPipeID pipe, t_uint16 client_id) {
      t_uint16 _xyuv_data[3];
      /* pipe <enum e_grabPipeID> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)pipe & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)pipe >> 16);
      /* client_id <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 1);
    }
    virtual void execute(enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id) {
      t_uint16 _xyuv_data[57];
      int ___i0, ___j0;
      /* pipe <enum e_grabPipeID> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)pipe & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)pipe >> 16);
      /* params <struct s_grabParams> marshalling */
      _xyuv_data[2+0] = (t_uint16)((unsigned int)params.output_format & 0xFFFFU);
      _xyuv_data[2+0+1] = (t_uint16)((unsigned int)params.output_format >> 16);
      _xyuv_data[2+2] = (t_uint16)params.x_window_size;
      _xyuv_data[2+3] = (t_uint16)params.y_window_size;
      _xyuv_data[2+4] = (t_uint16)params.x_window_offset;
      _xyuv_data[2+5] = (t_uint16)params.y_window_offset;
      _xyuv_data[2+6] = (t_uint16)params.x_frame_size;
      _xyuv_data[2+7] = (t_uint16)params.y_frame_size;
      _xyuv_data[2+8] = (t_uint16)params.dest_buf_addr;
      _xyuv_data[2+8+1] = (t_uint16)(params.dest_buf_addr >> 16);
      _xyuv_data[2+10] = (t_uint16)params.buf_id;
      _xyuv_data[2+11] = (t_uint16)params.rotation_cfg;
      _xyuv_data[2+12] = (t_uint16)params.disable_grab_cache;
      _xyuv_data[2+13] = (t_uint16)params.cache_buf_addr;
      _xyuv_data[2+13+1] = (t_uint16)(params.cache_buf_addr >> 16);
      _xyuv_data[2+15] = (t_uint16)params.enable_stab;
      _xyuv_data[2+16] = (t_uint16)params.x_stab_size_lr;
      _xyuv_data[2+17] = (t_uint16)params.y_stab_size_lr;
      _xyuv_data[2+18] = (t_uint16)params.proj_h_buf_addr;
      _xyuv_data[2+18+1] = (t_uint16)(params.proj_h_buf_addr >> 16);
      _xyuv_data[2+20] = (t_uint16)params.proj_v_buf_addr;
      _xyuv_data[2+20+1] = (t_uint16)(params.proj_v_buf_addr >> 16);
      _xyuv_data[2+22] = (t_uint16)params.lr_alpha;
      _xyuv_data[2+23] = (t_uint16)params.keepRatio;
      _xyuv_data[2+24+0] = (t_uint16)((unsigned int)params.bml_params.pixel_order & 0xFFFFU);
      _xyuv_data[2+24+0+1] = (t_uint16)((unsigned int)params.bml_params.pixel_order >> 16);
      _xyuv_data[2+24+2] = (t_uint16)params.bml_params.nb_linetypes;
      for(___i0 = ___j0 = 0; ___i0 < 8; ___i0++) {
        _xyuv_data[2+24+3+___j0] = (t_uint16)((unsigned int)params.bml_params.line_type[___i0] & 0xFFFFU);
        _xyuv_data[2+24+3+___j0+1] = (t_uint16)((unsigned int)params.bml_params.line_type[___i0] >> 16);
        ___j0 += 2;
      }
      for(___i0 = ___j0 = 0; ___i0 < 8; ___i0++) {
        _xyuv_data[2+24+19+___j0] = (t_uint16)params.bml_params.line_type_nb[___i0];
        ___j0 += 1;
      }
      _xyuv_data[2+24+27] = (t_uint16)params.bml_params.interline_size;
      _xyuv_data[2+24+27+1] = (t_uint16)(params.bml_params.interline_size >> 16);
      _xyuv_data[2+24+29] = (t_uint16)params.bml_params.nb_embedded_lines;
      /* client_id <t_uint16> marshalling */
      _xyuv_data[56] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 57*2, 2);
    }
    virtual void subscribeEvents(t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) {
      t_uint16 _xyuv_data[5];
      /* info_events <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)info_events;
      /* error_events <t_uint16> marshalling */
      _xyuv_data[1] = (t_uint16)error_events;
      /* debug_events <t_uint32> marshalling */
      _xyuv_data[2] = (t_uint16)debug_events;
      _xyuv_data[2+1] = (t_uint16)(debug_events >> 16);
      /* client_id <t_uint16> marshalling */
      _xyuv_data[4] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 3);
    }
    virtual void unsubscribeEvents(t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) {
      t_uint16 _xyuv_data[5];
      /* info_events <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)info_events;
      /* error_events <t_uint16> marshalling */
      _xyuv_data[1] = (t_uint16)error_events;
      /* debug_events <t_uint32> marshalling */
      _xyuv_data[2] = (t_uint16)debug_events;
      _xyuv_data[2+1] = (t_uint16)(debug_events >> 16);
      /* client_id <t_uint16> marshalling */
      _xyuv_data[4] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 4);
    }
    virtual void Overscan_Supported(t_uint16 Adaptive_Overscan_legacy) {
      t_uint16 _xyuv_data[1];
      /* Adaptive_Overscan_legacy <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)Adaptive_Overscan_legacy;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 5);
    }
    virtual void disableDspFineLowPower(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 6);
    }
    virtual void setBMSAdd(t_uint16 bmsHadd, t_uint16 bmsLadd) {
      t_uint16 _xyuv_data[2];
      /* bmsHadd <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)bmsHadd;
      /* bmsLadd <t_uint16> marshalling */
      _xyuv_data[1] = (t_uint16)bmsLadd;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 7);
    }
    virtual void setHiddenBMS(t_uint16 hiddenBMS) {
      t_uint16 _xyuv_data[1];
      /* hiddenBMS <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)hiddenBMS;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 8);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_grab_api_cmd(t_nmf_interface_desc* itf) {
  grab_api_cmd *stub = new grab_api_cmd();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'memgrabctl.api.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/memgrabctl/api/configure.hpp>

class memgrabctl_api_configure: public memgrabctl_api_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParams(struct s_grabParams GrabParams, enum e_grabctlPortID port_idx, t_uint16 fifoOutsize) {
      t_uint16 _xyuv_data[57];
      int ___i0, ___j0;
      /* GrabParams <struct s_grabParams> marshalling */
      _xyuv_data[0+0] = (t_uint16)((unsigned int)GrabParams.output_format & 0xFFFFU);
      _xyuv_data[0+0+1] = (t_uint16)((unsigned int)GrabParams.output_format >> 16);
      _xyuv_data[0+2] = (t_uint16)GrabParams.x_window_size;
      _xyuv_data[0+3] = (t_uint16)GrabParams.y_window_size;
      _xyuv_data[0+4] = (t_uint16)GrabParams.x_window_offset;
      _xyuv_data[0+5] = (t_uint16)GrabParams.y_window_offset;
      _xyuv_data[0+6] = (t_uint16)GrabParams.x_frame_size;
      _xyuv_data[0+7] = (t_uint16)GrabParams.y_frame_size;
      _xyuv_data[0+8] = (t_uint16)GrabParams.dest_buf_addr;
      _xyuv_data[0+8+1] = (t_uint16)(GrabParams.dest_buf_addr >> 16);
      _xyuv_data[0+10] = (t_uint16)GrabParams.buf_id;
      _xyuv_data[0+11] = (t_uint16)GrabParams.rotation_cfg;
      _xyuv_data[0+12] = (t_uint16)GrabParams.disable_grab_cache;
      _xyuv_data[0+13] = (t_uint16)GrabParams.cache_buf_addr;
      _xyuv_data[0+13+1] = (t_uint16)(GrabParams.cache_buf_addr >> 16);
      _xyuv_data[0+15] = (t_uint16)GrabParams.enable_stab;
      _xyuv_data[0+16] = (t_uint16)GrabParams.x_stab_size_lr;
      _xyuv_data[0+17] = (t_uint16)GrabParams.y_stab_size_lr;
      _xyuv_data[0+18] = (t_uint16)GrabParams.proj_h_buf_addr;
      _xyuv_data[0+18+1] = (t_uint16)(GrabParams.proj_h_buf_addr >> 16);
      _xyuv_data[0+20] = (t_uint16)GrabParams.proj_v_buf_addr;
      _xyuv_data[0+20+1] = (t_uint16)(GrabParams.proj_v_buf_addr >> 16);
      _xyuv_data[0+22] = (t_uint16)GrabParams.lr_alpha;
      _xyuv_data[0+23] = (t_uint16)GrabParams.keepRatio;
      _xyuv_data[0+24+0] = (t_uint16)((unsigned int)GrabParams.bml_params.pixel_order & 0xFFFFU);
      _xyuv_data[0+24+0+1] = (t_uint16)((unsigned int)GrabParams.bml_params.pixel_order >> 16);
      _xyuv_data[0+24+2] = (t_uint16)GrabParams.bml_params.nb_linetypes;
      for(___i0 = ___j0 = 0; ___i0 < 8; ___i0++) {
        _xyuv_data[0+24+3+___j0] = (t_uint16)((unsigned int)GrabParams.bml_params.line_type[___i0] & 0xFFFFU);
        _xyuv_data[0+24+3+___j0+1] = (t_uint16)((unsigned int)GrabParams.bml_params.line_type[___i0] >> 16);
        ___j0 += 2;
      }
      for(___i0 = ___j0 = 0; ___i0 < 8; ___i0++) {
        _xyuv_data[0+24+19+___j0] = (t_uint16)GrabParams.bml_params.line_type_nb[___i0];
        ___j0 += 1;
      }
      _xyuv_data[0+24+27] = (t_uint16)GrabParams.bml_params.interline_size;
      _xyuv_data[0+24+27+1] = (t_uint16)(GrabParams.bml_params.interline_size >> 16);
      _xyuv_data[0+24+29] = (t_uint16)GrabParams.bml_params.nb_embedded_lines;
      /* port_idx <enum e_grabctlPortID> marshalling */
      _xyuv_data[54] = (t_uint16)((unsigned int)port_idx & 0xFFFFU);
      _xyuv_data[54+1] = (t_uint16)((unsigned int)port_idx >> 16);
      /* fifoOutsize <t_uint16> marshalling */
      _xyuv_data[56] = (t_uint16)fifoOutsize;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 57*2, 0);
    }
    virtual void setConfig(enum e_grabctlPortID main_port_idx, enum e_grabctlPortID second_port_idx, enum e_grabctlPortID third_port_idx, t_uint32 nbPorts, enum e_grabctlCfgIdx param_idx, t_uint8 param_data) {
      t_uint16 _xyuv_data[11];
      /* main_port_idx <enum e_grabctlPortID> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)main_port_idx & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)main_port_idx >> 16);
      /* second_port_idx <enum e_grabctlPortID> marshalling */
      _xyuv_data[2] = (t_uint16)((unsigned int)second_port_idx & 0xFFFFU);
      _xyuv_data[2+1] = (t_uint16)((unsigned int)second_port_idx >> 16);
      /* third_port_idx <enum e_grabctlPortID> marshalling */
      _xyuv_data[4] = (t_uint16)((unsigned int)third_port_idx & 0xFFFFU);
      _xyuv_data[4+1] = (t_uint16)((unsigned int)third_port_idx >> 16);
      /* nbPorts <t_uint32> marshalling */
      _xyuv_data[6] = (t_uint16)nbPorts;
      _xyuv_data[6+1] = (t_uint16)(nbPorts >> 16);
      /* param_idx <enum e_grabctlCfgIdx> marshalling */
      _xyuv_data[8] = (t_uint16)((unsigned int)param_idx & 0xFFFFU);
      _xyuv_data[8+1] = (t_uint16)((unsigned int)param_idx >> 16);
      /* param_data <t_uint8> marshalling */
      _xyuv_data[10] = (t_uint16)param_data;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 11*2, 1);
    }
    virtual void setClientID(t_uint8 client_id) {
      t_uint16 _xyuv_data[1];
      /* client_id <t_uint8> marshalling */
      _xyuv_data[0] = (t_uint16)client_id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 2);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_memgrabctl_api_configure(t_nmf_interface_desc* itf) {
  memgrabctl_api_configure *stub = new memgrabctl_api_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function imaging_cpp_stub_functions[] = {
  {"grabctl.api.configure", &INITstub_grabctl_api_configure},
  {"ispctl.api.cmd", &INITstub_ispctl_api_cmd},
  {"ispctl.api.cfg", &INITstub_ispctl_api_cfg},
  {"api.set_debug", &INITstub_api_set_debug},
  {"grab.api.cmd", &INITstub_grab_api_cmd},
  {"memgrabctl.api.configure", &INITstub_memgrabctl_api_configure},
};

/*const */t_nmf_stub_register imaging_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(imaging_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    imaging_cpp_stub_functions
};
