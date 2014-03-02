/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TRACE_H_
#define _TRACE_H_



#include "OMX_Index.h"
#include "OMX_Core.h"
#include "ENS_Component.h"
#include "qevent.h"
#include "qhsm.h"
#include "qf_os.h"
#include "ENS_Component_Fsm.h"
#include "IFM_Types.h"




#define DBG_MAX_TRACES 32

#define MAX_NUMBER_OF_PE_IN_A_LIST  32

typedef struct {
	OMX_EVENTTYPE ev;
	t_uint32 data1;
	t_uint32 data2;
}t_omx_event;

/*Read or Write list of Pe related */
typedef enum {
    WRITE_LIST_OF_PE,
    READ_LIST_OF_PE
}t_controling_readwritepe_type;

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTrace);
#endif
class CTrace {

public:

	CTrace();

	void resetReadListPe();
	void resetWriteListPe();

	//MM added to implement the read list pe
	IFM_CONFIG_READPETYPE iListOfPeToRead;
	IFM_CONFIG_WRITEPETYPE iListOfPeToWrite;

	t_controling_readwritepe_type iPEMode;
	OMX_BOOL bCallbackNeeded;
};

#endif



