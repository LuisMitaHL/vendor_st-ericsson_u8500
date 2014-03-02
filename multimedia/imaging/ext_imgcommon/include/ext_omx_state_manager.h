/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _OMX_STATE_MANAGER_H
#define _OMX_STATE_MANAGER_H

#include "OMX_Core.h"

#include <cm/inc/cm_type.h>

#include <ENS_DBC.h>


/*****************************/
/* Sensor object declaration */
/*****************************/

/** \brief abstract class implementing Sensor specifics parameters */

class COmxStateMgr
{
    public:
        COmxStateMgr();                                                          /*Standard Constructor*/
		OMX_STATETYPE updateState(void);
		OMX_ERRORTYPE changeState(OMX_STATETYPE stateTobeUpdated);
    protected:
        OMX_STATETYPE       state[2];
		t_uint16 			stateCounter;

};
#endif
