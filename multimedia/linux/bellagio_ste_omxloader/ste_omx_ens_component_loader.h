/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STE_OMXCOMPONENT_LOADER_H_
#define _STE_OMXCOMPONENT_LOADER_H_

class ENS_Component;
#include "OMX_Core.h"

/** The maximum numbers of roles a single component is able to register */
#define STE_OMX_ENS_COMPONENT_LOADER_MAX_ROLES 4

/** The maximum numbers of components a given shared library is able to register */
#define STE_OMX_ENS_COMPONENT_LOADER_MAX_COMPONENTS 15

/** Stucture to register a given component along with its OMX std roles and
 * the constructor setting up an ENS component */
struct ste_omx_ens_component_def {
    /** The component name */
    const char name[OMX_MAX_STRINGNAME_SIZE];
    /** The component roles */
    const char roles[STE_OMX_ENS_COMPONENT_LOADER_MAX_ROLES][OMX_MAX_STRINGNAME_SIZE];
    /** The component constructor */
    OMX_ERRORTYPE (*constructor)(ENS_Component ** ppENSComponent);
};

/** Stucture to register a set of components */
struct ste_omx_ens_component_def_array {
    /** The array of components a given shared library is registering */
    struct ste_omx_ens_component_def array[STE_OMX_ENS_COMPONENT_LOADER_MAX_COMPONENTS];
};

typedef void (*ste_omx_ens_component_register_t)(struct ste_omx_ens_component_def_array **refs);


#endif /* _STE_OMXCOMPONENT_LOADER_H_ */


