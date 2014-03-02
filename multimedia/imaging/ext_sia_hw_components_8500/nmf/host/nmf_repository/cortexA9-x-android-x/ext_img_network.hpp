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

/* 'ext_img_network' header library */
#if !defined(ext_img_network_NMF)
#define ext_img_network_NMF

#include <inc/type.h>
#include <cpp.hpp>
#ifndef HOST_ONLY
#include <cm/inc/cm.hpp>
#endif
#include <sendcommand.hpp>
#include <fsminit.hpp>
#include <armnmf_fillthisbuffer.hpp>
#include <ext_stab/api/configure.hpp>
#include <ext_grabctl/api/configure.hpp>
#include <ext_grab/api/cmd.hpp>
#include <eventhandler.hpp>
#include <ext_grabctl/api/acknowledges.hpp>
#include <ext_grabctl/api/error.hpp>
#include <armnmf_emptythisbuffer.hpp>
#include <ext_grab/api/alert.hpp>
#include <ee/api/trace.hpp>

class ext_img_network: public NMF::Composite {
  public:
    t_cm_error cm_error;
    t_cm_domain_id domain;
  protected:
    virtual ~ext_img_network() {} // Protected, use ext_img_network() instead !!!
};

/*
 * Component Factory
 */
IMPORT_NMF_COMPONENT ext_img_network* ext_img_networkCreate(void);
IMPORT_NMF_COMPONENT void ext_img_networkDestroy(ext_img_network*& instance);

#endif
