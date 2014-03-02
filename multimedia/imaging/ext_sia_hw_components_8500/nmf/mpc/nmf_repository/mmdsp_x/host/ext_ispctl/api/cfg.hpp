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

#include <cpp.hpp>
#include <host/ext_ispctl_types.idt.h>


class ext_ispctl_api_cfgDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void Primary_Interface_Configuration(struct s_CsiConfig Primary_Csi_Config) = 0;
    virtual void Secondary_Interface_Configuration(t_SensorType_e Interface_Type, struct s_CsiConfig Primary_Csi_Config, struct s_CcpConfig Secondary_Ccp_Config) = 0;
    virtual void Reset_Camera_DataPipe(t_uint16 CsiBitrateMbps) = 0;
};

class Iext_ispctl_api_cfg: public NMF::InterfaceReference {
  public:
    Iext_ispctl_api_cfg(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void Primary_Interface_Configuration(struct s_CsiConfig Primary_Csi_Config) {
      ((ext_ispctl_api_cfgDescriptor*)reference)->Primary_Interface_Configuration(Primary_Csi_Config);
    }
    void Secondary_Interface_Configuration(t_SensorType_e Interface_Type, struct s_CsiConfig Primary_Csi_Config, struct s_CcpConfig Secondary_Ccp_Config) {
      ((ext_ispctl_api_cfgDescriptor*)reference)->Secondary_Interface_Configuration(Interface_Type, Primary_Csi_Config, Secondary_Ccp_Config);
    }
    void Reset_Camera_DataPipe(t_uint16 CsiBitrateMbps) {
      ((ext_ispctl_api_cfgDescriptor*)reference)->Reset_Camera_DataPipe(CsiBitrateMbps);
    }
};

#endif
