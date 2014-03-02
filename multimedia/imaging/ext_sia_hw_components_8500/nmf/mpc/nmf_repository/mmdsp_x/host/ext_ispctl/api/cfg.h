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

/* 'ext_ispctl.api.cfg' interface */
#if !defined(ext_ispctl_api_cfg_IDL)
#define ext_ispctl_api_cfg_IDL

#include <cm/inc/cm_type.h>
#include <host/ext_ispctl_types.idt.h>


typedef struct sIext_ispctl_api_cfg {
  void* THIS;
  t_cm_error (*Primary_Interface_Configuration)(void* THIS, struct s_CsiConfig Primary_Csi_Config);
  t_cm_error (*Secondary_Interface_Configuration)(void* THIS, t_SensorType_e Interface_Type, struct s_CsiConfig Primary_Csi_Config, struct s_CcpConfig Secondary_Ccp_Config);
  t_cm_error (*Reset_Camera_DataPipe)(void* THIS, t_uint16 CsiBitrateMbps);
} Iext_ispctl_api_cfg;

typedef struct {
  void* THIS;
  void (*Primary_Interface_Configuration)(void* THIS, struct s_CsiConfig Primary_Csi_Config);
  void (*Secondary_Interface_Configuration)(void* THIS, t_SensorType_e Interface_Type, struct s_CsiConfig Primary_Csi_Config, struct s_CcpConfig Secondary_Ccp_Config);
  void (*Reset_Camera_DataPipe)(void* THIS, t_uint16 CsiBitrateMbps);
} CBext_ispctl_api_cfg;

#endif
