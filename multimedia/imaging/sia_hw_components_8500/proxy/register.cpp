/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \brief   register C/C++ NMF Interfaces, used by NMF clients
*/
#include "cm/proxy/api/communication_proxy.h"
#include "register.h"


#ifndef EXPORT_C
#define EXPORT_C
#endif

EXPORT_C void registerStubsAndSkels(void)
{
    CM_REGISTER_STUBS_SKELS(imaging);
    CM_REGISTER_STUBS_SKELS(imaging_cpp);
}

EXPORT_C void unregisterStubsAndSkels(void)
{
    CM_UNREGISTER_STUBS_SKELS(imaging_cpp);
    CM_UNREGISTER_STUBS_SKELS(imaging);
}
