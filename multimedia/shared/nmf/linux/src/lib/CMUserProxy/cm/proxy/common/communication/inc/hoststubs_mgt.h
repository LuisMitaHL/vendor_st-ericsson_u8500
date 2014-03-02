/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/**
 * \internal
 */
#ifndef COMMON_HOSTSTUBS_MGT_H
#define COMMON_HOSTSTUBS_MGT_H

#include <cm/proxy/api/private/stub-requiredapi.h>

PUBLIC t_cm_error cm_HSM_getItfStubInitFct(t_nmf_bc_coding_style style, const char *itfType, t_nmf_init_stub_fct *pInitStubFct);
PUBLIC t_cm_error cm_HSM_getItfSkelInitFct(t_nmf_bc_coding_style style, const char *itfType, t_jump_method **jumpMethods);

#endif /* COMMON_HOSTSTUBS_MGT_H */
