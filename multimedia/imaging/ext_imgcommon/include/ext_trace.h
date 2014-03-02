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
#include "ext_qevent.h"
#include "ext_qhsm.h"
#include "ext_qf_os.h"
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

/* debugging list of PE */
typedef struct
{
    //OMX_BOOL pipe_Enable;
    //OMX_U16  buffer_of_PE[MAX_NUMBER_OF_PE_IN_A_LIST];
	IFM_CONFIG_READPETYPE iListPEtoRead;
    OMX_U32  nb_of_PE;
}t_reading_listPE;

/* writing list of PE */
typedef struct
{
    //OMX_U16  buffer_of_PE_addr[MAX_NUMBER_OF_PE_IN_A_LIST];
    //OMX_U16  buffer_of_PE_data[MAX_NUMBER_OF_PE_IN_A_LIST];
	IFM_CONFIG_WRITEPETYPE iListPEtoWrite;
    OMX_U32  nb_of_PE;
}t_writing_listPE;


#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTrace);
#endif
class CTrace {

public:

	CTrace();
/*
	void initTraces(void);

	void traceXp70Event(QEvent);
	void traceDspEvent(OMX_EVENTTYPE, t_uint32, t_uint32);
	void traceQState(QHsm::CQSTATE);
*/

	void resetReadListPe();
	void resetWriteListPe();

	//MM added to implement the read list pe
	t_reading_listPE iPrepareListPe;
	t_writing_listPE iListOfPeToWrite;

	t_controling_readwritepe_type iPEMode;

	OMX_U32 *pExtraDataZone;

	OMX_BUFFERHEADERTYPE * pOMXbufferfilldone;


protected:
/*
	t_uint32 xp70EventIdx;
	t_uint32 dspEventIdx;
	t_uint32 qStateIdx;

	QEvent xp70Event[DBG_MAX_TRACES];
	t_omx_event dspEvent[DBG_MAX_TRACES];
	QHsm::CQSTATE qState[DBG_MAX_TRACES];
*/





};

#endif



