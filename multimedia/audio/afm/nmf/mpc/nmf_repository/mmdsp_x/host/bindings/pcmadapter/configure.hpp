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

/* 'bindings.pcmadapter.configure' interface */
#if !defined(bindings_pcmadapter_configure_IDL)
#define bindings_pcmadapter_configure_IDL

#include <cpp.hpp>

typedef struct t_xyuv_PcmAdapterParam_t {
  void* pBuffer;
  t_uint24 nBufferSize;
  t_uint16 nInputBlockSize;
  t_uint16 nChansIn;
  t_uint16 nOutputBlockSize;
  t_uint16 nChansOut;
  t_uint16 nNbBuffersIn;
  t_uint16 nNbBuffersOut;
  t_uint16 nBitPerSampleIn;
  t_uint16 nBitPerSampleOut;
  t_uint16 nSampleRate;
} PcmAdapterParam_t;


class bindings_pcmadapter_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(PcmAdapterParam_t params) = 0;
};

class Ibindings_pcmadapter_configure: public NMF::InterfaceReference {
  public:
    Ibindings_pcmadapter_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(PcmAdapterParam_t params) {
      ((bindings_pcmadapter_configureDescriptor*)reference)->setParameter(params);
    }
};

#endif
