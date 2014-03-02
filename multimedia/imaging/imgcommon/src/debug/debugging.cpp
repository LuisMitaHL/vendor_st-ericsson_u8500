/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "hsm.h"
#include "trace.h"
#include "IFM_Index.h"
#include "OMX_CoreExt.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_debug_debuggingTraces.h"
#endif

/*
 * Is the parent of the Debugging State machine.
 * This state machine allows the user trough a SetConfig to read or write a list of PE.
 */
SCF_STATE COM_SM::SHARED_Debugging(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG: return 0;
		case SCF_STATE_EXIT_SIG: return 0;
		case SCF_STATE_INIT_SIG:
			SCF_INIT(&COM_SM::SHARED_DebuggingGetorWriteListPe);
			return 0;
		case Q_DEFERRED_EVENT_SIG: return 0;
		case SCF_PARENT_SIG:break;
		default:
			break;
	}
	return SCF_STATE_PTR(&COM_SM::OMX_Executing);
}

/*
 * Is the one which actually writes or read the requested PE list
 */
SCF_STATE COM_SM::SHARED_DebuggingGetorWriteListPe(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
			MSG0("SHARED_DebuggingGetorWriteListPe-SCF_STATE_ENTRY_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_DebuggingGetorWriteListPe-SCF_STATE_ENTRY_SIG", (&mENSComponent));
			switch(pTrace->iPEMode)
			{
				/* the user has requested a read of a list of PE */
				case READ_LIST_OF_PE:
				{
					MSG0("ReadListPe-switch\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "ReadListPe-switch", (&mENSComponent));
					
					if (pTrace->iListOfPeToRead.nPEToRead > MAX_NUMBER_OF_PE_IN_A_LIST)
					{
						pTrace->iListOfPeToRead.nPEToRead = MAX_NUMBER_OF_PE_IN_A_LIST;
					}
					
					for(int i=pTrace->iListOfPeToRead.nPEToRead; i<MAX_NUMBER_OF_PE_IN_A_LIST; i++)
					{
						pTrace->iListOfPeToRead.nPEList_addr[i] = 0;
						pTrace->iListOfPeToRead.nPEList_data[i] = 0;
					}
										
					for(int i=0; i<pTrace->iListOfPeToRead.nPEToRead; i++)
					{
						pIspctlCom->queuePE( pTrace->iListOfPeToRead.nPEList_addr[i], 0);
					}					
					pIspctlCom->readQueue();
					
					break;
				}
				/* the user has requested a write of a list of PE */
				case WRITE_LIST_OF_PE:
				{
					MSG0("WriteListPe-switch\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "WriteListPe-switch", (&mENSComponent));
					
					if (pTrace->iListOfPeToWrite.nPEToWrite > MAX_NUMBER_OF_PE_IN_A_LIST)
					{
						pTrace->iListOfPeToWrite.nPEToWrite = MAX_NUMBER_OF_PE_IN_A_LIST;
					}

					for(int i=pTrace->iListOfPeToWrite.nPEToWrite; i<MAX_NUMBER_OF_PE_IN_A_LIST; i++)
					{
						pTrace->iListOfPeToWrite.nPEList_addr[i] = 0;
						pTrace->iListOfPeToWrite.nPEList_data[i] = 0;
					}
										
					for(int i=0; i<pTrace->iListOfPeToWrite.nPEToWrite; i++)
					{
						pIspctlCom->queuePE( pTrace->iListOfPeToWrite.nPEList_addr[i], pTrace->iListOfPeToWrite.nPEList_data[i]);
					}
					pIspctlCom->processQueue();
					break;
				}
				default:
					DBC_ASSERT(0); // the iPEMode has been corrupted or the it has not been set.
			}
			return 0;

		case EVT_ISPCTL_LIST_INFO_SIG:
			MSG0("SHARED_DebuggingGetorWriteListPe-EVT_ISPCTL_LIST_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_DebuggingGetorWriteListPe-EVT_ISPCTL_LIST_INFO_SIG", (&mENSComponent));
			if ((e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) && (e->type.ispctlInfo.info_id != ISP_READLIST_DONE)) break;
			
			/* the request read list has been retrieved */
        	 	if (e->type.ispctlInfo.info_id == ISP_READLIST_DONE)
        	 	{
				for (int i=0; i<pTrace->iListOfPeToRead.nPEToRead; i++)
				{
					pTrace->iListOfPeToRead.nPEList_data[i] = e->type.ispctlListInfo.Listvalue[i].pe_data;
				}
				
				if (OMX_TRUE == pTrace->bCallbackNeeded)
				{
					mENSComponent.eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, OMX_ALL, IFM_IndexConfigCommonReadPe, NULL);
				}
			}
			else if (OMX_TRUE == pTrace->bCallbackNeeded)
			{
				mENSComponent.eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, OMX_ALL, IFM_IndexConfigCommonWritePe, NULL);
			}
			
            		SCF_TRANSIT_TO_PSTATE(whatNext);
                    whatNext = NULL;    // To avoid any confusion.
           		return 0;
			
		case SCF_STATE_EXIT_SIG:
			MSG0("SHARED_DebuggingGetorWriteListPe-EXIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_DebuggingGetorWriteListPe-EXIT;", (&mENSComponent));
			return 0;

		default:
			MSG0("SHARED_DebuggingGetorWriteListPe-default;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "SHARED_DebuggingGetorWriteListPe-default;", (&mENSComponent));
			break;
	}

	return SCF_STATE_PTR(&COM_SM::SHARED_Debugging);
}
