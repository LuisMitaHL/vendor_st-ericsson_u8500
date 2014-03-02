/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "hsmcam.h" // rename_me sc_cam.h

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "hsmcamera_src_isp_features_mirroring_statemachineTraces.h"
#endif

SCF_STATE CAM_SM::Mirroring(s_scf_event const *e)
{
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
			MSG0("Mirroring-ENTRY;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Mirroring-ENTRY;", (&mENSComponent));
			return 0;
		case SCF_STATE_EXIT_SIG:
			MSG0("Mirroring-EXIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Mirroring-EXIT;", (&mENSComponent));
			return 0;
		case SCF_STATE_INIT_SIG:
			MSG0("Mirroring-INIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Mirroring-INIT;", (&mENSComponent));
			SCF_INIT(&CAM_SM::MirroringExecute);
			return 0;
		case Q_DEFERRED_EVENT_SIG:
			MSG0("Mirroring-DEFERRED_EVENT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Mirroring-DEFERRED_EVENT;", (&mENSComponent));
			return 0;
		default:
			break;
	}

	MSG0("call SCF_STATE_PTR(&CAM_SM::SetFeature)\n");
	OstTraceFiltStatic0(TRACE_DEBUG, "call SCF_STATE_PTR(&CAM_SM::SetFeature)", (&mENSComponent));
	return SCF_STATE_PTR(&CAM_SM::SetFeature);
}

SCF_STATE CAM_SM::MirroringExecute(s_scf_event const *e)
{
	Flag_te eflip = Flag_e_FALSE;
	Flag_te emirror = Flag_e_FALSE;
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
			MSG0("MirroringExecute-ENTRY;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "MirroringExecute-ENTRY;", (&mENSComponent));
			switch (eMirror)
			{
				case OMX_MirrorNone:
					MSG0("MirroringExecute MirrorNone\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "MirroringExecute MirrorNone", (&mENSComponent));
					eflip = Flag_e_FALSE;
					emirror = Flag_e_FALSE;
					break;
				case OMX_MirrorVertical:
					MSG0("MirroringExecute MirrorVertical\n");	
					OstTraceFiltStatic0(TRACE_DEBUG, "MirroringExecute MirrorVertical", (&mENSComponent));	
					eflip = Flag_e_TRUE;
					emirror = Flag_e_FALSE;
					break;
				case OMX_MirrorHorizontal:
					MSG0("MirroringExecute MirrorHorizontal\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "MirroringExecute MirrorHorizontal", (&mENSComponent));
					eflip = Flag_e_FALSE;
					emirror = Flag_e_TRUE;
					break;
				case OMX_MirrorBoth:
					MSG0("MirroringExecute MirrorBoth\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "MirroringExecute MirrorBoth", (&mENSComponent));
					eflip = Flag_e_TRUE;
					emirror = Flag_e_TRUE;
					break;
				default:
					break;
			}
			pIspctlCom->queuePE(SystemSetup_e_Flag_flip_Byte0,(t_uint32)eflip);
			pIspctlCom->queuePE(SystemSetup_e_Flag_mirror_Byte0,(t_uint32)emirror);
			/*The mirroring is updated only after s3a update*/
			/*In the next release FW, we are a specific coin for this */
			pIspctlCom->processQueue();
			return 0;
		case EVT_ISPCTL_LIST_INFO_SIG :
		{
			MSG0("MirroringExecute-EVT_ISPCTL_LIST_INFO_SIG;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "MirroringExecute-EVT_ISPCTL_LIST_INFO_SIG;", (&mENSComponent));
			if (ISP_WRITELIST_DONE == e->type.ispctlInfo.info_id)
			{
				SCF_TRANSIT_TO(&CAM_SM::ProcessPendingEvents);
			}
			return 0;
		}
		case SCF_STATE_EXIT_SIG:
			MSG0("MirroringExecute-EXIT;\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "MirroringExecute-EXIT;", (&mENSComponent));
			return 0;
		default:
			break;
	}
	MSG0("call SCF_STATE_PTR(&CAM_SM::Mirroring)\n");
	OstTraceFiltStatic0(TRACE_DEBUG, "call SCF_STATE_PTR(&CAM_SM::Mirroring)", (&mENSComponent));
	return SCF_STATE_PTR(&CAM_SM::Mirroring);
}
