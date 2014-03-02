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

/* Generated host stub for 'resource_manager.api.mtf_resource' */
#include <host/resource_manager/api/mtf_resource.h>

static t_cm_error st_resource_manager_api_mtf_resource_mtfWrite(void* THIS, t_uint32 dstDataAddress, t_uint16 Size, t_uint32 srcDataAddress) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[5];
    /* dstDataAddress <t_uint32> marshalling */
  _xyuv_data[0] = (t_uint16)dstDataAddress;
  _xyuv_data[0+1] = (t_uint16)(dstDataAddress >> 16);
    /* Size <t_uint16> marshalling */
  _xyuv_data[2] = (t_uint16)Size;
    /* srcDataAddress <t_uint32> marshalling */
  _xyuv_data[3] = (t_uint16)srcDataAddress;
  _xyuv_data[3+1] = (t_uint16)(srcDataAddress >> 16);
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 0);
}

static t_cm_error st_resource_manager_api_mtf_resource_mtfRead(void* THIS, t_uint32 srcDataAddress, t_uint16 Size, t_uint32 dstDataAddress) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[5];
    /* srcDataAddress <t_uint32> marshalling */
  _xyuv_data[0] = (t_uint16)srcDataAddress;
  _xyuv_data[0+1] = (t_uint16)(srcDataAddress >> 16);
    /* Size <t_uint16> marshalling */
  _xyuv_data[2] = (t_uint16)Size;
    /* dstDataAddress <t_uint32> marshalling */
  _xyuv_data[3] = (t_uint16)dstDataAddress;
  _xyuv_data[3+1] = (t_uint16)(dstDataAddress >> 16);
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 1);
}

static t_cm_bf_host2mpc_handle* INITstub_resource_manager_api_mtf_resource(t_nmf_interface_desc* itf) {
  ((Iresource_manager_api_mtf_resource *)itf)->mtfWrite = st_resource_manager_api_mtf_resource_mtfWrite;
  ((Iresource_manager_api_mtf_resource *)itf)->mtfRead = st_resource_manager_api_mtf_resource_mtfRead;
  return (t_cm_bf_host2mpc_handle*)&((Iresource_manager_api_mtf_resource *)itf)->THIS;
}

/* Generated host stub for 'resource_manager.api.common_interface_exports' */
#include <host/resource_manager/api/common_interface_exports.h>

static t_cm_error st_resource_manager_api_common_interface_exports_common_interface_exports(void* THIS) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 0);
}

static t_cm_bf_host2mpc_handle* INITstub_resource_manager_api_common_interface_exports(t_nmf_interface_desc* itf) {
  ((Iresource_manager_api_common_interface_exports *)itf)->common_interface_exports = st_resource_manager_api_common_interface_exports_common_interface_exports;
  return (t_cm_bf_host2mpc_handle*)&((Iresource_manager_api_common_interface_exports *)itf)->THIS;
}

static t_nmf_stub_function video_resource_manager_stub_functions[] = {
  {"resource_manager.api.mtf_resource", &INITstub_resource_manager_api_mtf_resource},
  {"resource_manager.api.common_interface_exports", &INITstub_resource_manager_api_common_interface_exports},
};

/*const */t_nmf_stub_register video_resource_manager_stub_register = {
    NMF_BC_IN_C,
    sizeof(video_resource_manager_stub_functions)/sizeof(t_nmf_stub_function),
    video_resource_manager_stub_functions
};
