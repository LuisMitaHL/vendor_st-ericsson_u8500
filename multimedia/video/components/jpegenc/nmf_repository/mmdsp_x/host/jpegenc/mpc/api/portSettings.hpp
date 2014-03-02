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

/* 'jpegenc.mpc.api.portSettings' interface */
#if !defined(jpegenc_mpc_api_portSettings_IDL)
#define jpegenc_mpc_api_portSettings_IDL

#include <cpp.hpp>


class jpegenc_mpc_api_portSettingsDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void portSettings(t_uint32 size) = 0;
};

class Ijpegenc_mpc_api_portSettings: public NMF::InterfaceReference {
  public:
    Ijpegenc_mpc_api_portSettings(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void portSettings(t_uint32 size) {
      ((jpegenc_mpc_api_portSettingsDescriptor*)reference)->portSettings(size);
    }
};

#endif
