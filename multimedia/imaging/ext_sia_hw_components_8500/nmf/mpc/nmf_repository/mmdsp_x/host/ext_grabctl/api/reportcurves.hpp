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

/* 'ext_grabctl.api.reportcurves' interface */
#if !defined(ext_grabctl_api_reportcurves_IDL)
#define ext_grabctl_api_reportcurves_IDL

#include <cpp.hpp>


class ext_grabctl_api_reportcurvesDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void reportCurves(t_uint32 h_curve, t_uint32 v_curve, t_uint32 timestamp) = 0;
};

class Iext_grabctl_api_reportcurves: public NMF::InterfaceReference {
  public:
    Iext_grabctl_api_reportcurves(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void reportCurves(t_uint32 h_curve, t_uint32 v_curve, t_uint32 timestamp) {
      ((ext_grabctl_api_reportcurvesDescriptor*)reference)->reportCurves(h_curve, v_curve, timestamp);
    }
};

#endif
