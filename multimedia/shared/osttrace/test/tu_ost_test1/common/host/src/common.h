/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef __cplusplus

#include <cm/inc/cm.hpp>

extern "C" t_cm_domain_id mpc2domainId(const char* mpcstring);

#else

#include <cm/inc/cm.h>

extern t_cm_domain_id mpc2domainId(const char* mpcstring);

#endif

