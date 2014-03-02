/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _OMX_WRAPPER_H_
#define _OMX_WRAPPER_H_

#include <sys/cdefs.h>
#include <OMX_Component.h>

__BEGIN_DECLS

/** Returns an OMX_COMPONENTTYPE structure wrapping the provided component.
 * In case of error NULL is returned. */
OMX_COMPONENTTYPE * OMX_Wrapper_create(OMX_COMPONENTTYPE *comp, OMX_STRING cComponentName);

/** Destroy the wrapper */
void OMX_Wrapper_destroy(OMX_COMPONENTTYPE *comp);

__END_DECLS

#endif /* _OMX_WRAPPER_H_ */


