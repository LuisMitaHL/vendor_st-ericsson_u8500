/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _EXT_HOSTNMF_CB_H_
#define _EXT_HOSTNMF_CB_H_

#include "host/armnmf_emptythisbuffer.hpp"
#include "ENS_Component.h"

class Camera;

class CHostNmf_CB : public armnmf_emptythisbufferDescriptor
{
public :
    CHostNmf_CB(Camera * cam) { mCam = cam ;}

    // methods inherited from armnmf_emptythisbufferDescriptor
    ENS_API_IMPORT virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) ;

private :
    Camera * mCam;
};

#endif /* _EXT_HOSTNMF_CB_H_ */
