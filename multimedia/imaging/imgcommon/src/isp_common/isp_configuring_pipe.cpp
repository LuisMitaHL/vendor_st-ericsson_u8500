/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "hsm.h" // rename_me "common_sm.h"
#include "host/grabctl_types.idt.h"
#include "host/grab_types.idt.h"
#include "VhcElementDefs.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_isp_common_isp_configuring_pipeTraces.h"
#endif


/*
 * Is the parent for Configuring the ISP Pipe
 * Each state machine (CAM, ISP) inherits from this state machine to configure the ISP PIPE
 * Some dedicated functions are pure virtual and have to be overwritten by the childs
 */
SCF_STATE COM_SM::SHARED_ConfiguringISPPipe(s_scf_event const *e) {
   switch (e->sig)
   {
	   case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
	   case SCF_STATE_EXIT_SIG:  EXIT;  return 0;
	   case SCF_STATE_INIT_SIG:  INIT;  SCF_INIT(&COM_SM::SHARED_ConfiguringISPPipe_FormatResolution); return 0;
	   case SCF_PARENT_SIG:break;
	   case EVT_ISPCTL_ERROR_SIG:
	   {
		   /* Today assert in case of Error coming from the ISP : no error recovery is implemented */
			DBGT_ERROR("SHARED_ConfiguringISPPipe-EVT_ISPCTL_ERROR_SIG\n");
			OstTraceFiltStatic0(TRACE_ERROR, "SHARED_ConfiguringISPPipe-EVT_ISPCTL_ERROR_SIG", (&mENSComponent));
			if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR)  {
				DBGT_ERROR("ControlingISP-ISP_POLLING_TIMEOUT_ERROR;\n");
				OstTraceFiltStatic0(TRACE_ERROR, "ControlingISP-ISP_POLLING_TIMEOUT_ERROR;", (&mENSComponent));
				DBC_ASSERT(0);
			}
			if (e->type.ispctlError.error_id == ISP_DOES_NOT_RESPOND_ERROR)  {
				DBGT_ERROR("ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;\n");
				OstTraceFiltStatic0(TRACE_ERROR, "ControlingISP-ISP_DOES_NOT_RESPOND_ERROR;", (&mENSComponent));
				DBC_ASSERT(0);
			}
            return 0;
	   }
	   case Q_DEFERRED_EVENT_SIG:
	   {
		   MSG0("ConfiguringISPPipe-DEFERRED_EVENT;\n");
		   OstTraceFiltStatic0(TRACE_DEBUG, "ConfiguringISPPipe-DEFERRED_EVENT;", (&mENSComponent));
		   return 0;
	   }
	   default:
		   break;
   }
   return SCF_STATE_PTR(&COM_SM::OMX_Executing);
}



/*
 * This allows to configure a format & a resolution.
 * This is based on	ConfigureResolution() that has to be overwritten by who inherits.
 */
SCF_STATE COM_SM::SHARED_ConfiguringISPPipe_FormatResolution(s_scf_event const *e) {
    switch (e->sig)
    {
        case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;
            OMX_ERRORTYPE error = OMX_ErrorUndefined;
            error = configureResolution();
            if (OMX_ErrorNone != error) {
                DBGT_ERROR("COM_SM::SHARED_ConfiguringISPPipe_FormatResolution configureResolution error %d\n", error);
                OstTraceFiltStatic1(TRACE_ERROR, "COM_SM::SHARED_ConfiguringISPPipe_FormatResolution configureResolution error %d", (&mENSComponent), error);
                DBC_ASSERT(0);
                return 0;
            }
			pIspctlCom->processQueue();
			return 0;
	   }
	   case SCF_STATE_EXIT_SIG:  EXIT;  return 0;
	   case EVT_ISPCTL_LIST_INFO_SIG:
	   {
		    MSG0("ConfiguringISPipe_FormatResolution-EVT_ISPCTL_INFO_SIG\n");
		    OstTraceFiltStatic0(TRACE_DEBUG, "ConfiguringISPipe_FormatResolution-EVT_ISPCTL_INFO_SIG", (&mENSComponent));
		    if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break; /* Ignore this message, it will be handled by the parent*/

			if(ConfiguringISPPipe_ConfigType!=ConfiguringISPPipe_ResolutionOnly)
				SCF_TRANSIT_TO(&COM_SM::SHARED_ConfiguringISPPipe_Ena_Disa);
			else 
            {
				SCF_TRANSIT_TO_PSTATE(whatNext);
                whatNext = NULL;    // To avoid any confusion.
            }

			return 0;
        }
        default: break;
    }
    return SCF_STATE_PTR(&COM_SM::SHARED_ConfiguringISPPipe);
}


/*
 * This allows to enable or disable the pipes.
 * This is based on	configurePipes() that has to be overwritten by who inherits.
 */
SCF_STATE COM_SM::SHARED_ConfiguringISPPipe_Ena_Disa(s_scf_event const *e) {
	switch (e->sig)
	{
		case SCF_STATE_ENTRY_SIG:
		{
			ENTRY;
            OMX_ERRORTYPE error = OMX_ErrorUndefined;
            error = configurePipes();
            if (OMX_ErrorNone != error) {
                DBGT_ERROR("COM_SM::SHARED_ConfiguringISPPipe_Ena_Disa configurePipes error %d\n", error);
                OstTraceFiltStatic1(TRACE_ERROR, "COM_SM::SHARED_ConfiguringISPPipe_Ena_Disa configurePipes error %d", (&mENSComponent), error);
                DBC_ASSERT(0);
                return 0;
            }
			pIspctlCom->processQueue();
			return 0;
		}
		case SCF_STATE_EXIT_SIG:  EXIT; return 0;
		case EVT_ISPCTL_LIST_INFO_SIG :
		{
			MSG0("ConfiguringISPipe_Ena_Disa-EVT_ISPCTL_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ConfiguringISPipe_Ena_Disa-EVT_ISPCTL_INFO_SIG", (&mENSComponent));
			if (e->type.ispctlInfo.info_id != ISP_WRITELIST_DONE) break; //Ignore the message, it will be handled by the parent.
			SCF_TRANSIT_TO_PSTATE(whatNext);
            whatNext = NULL;    // To avoid any confusion.
			return 0;
		}
        default: break;
	}
	return SCF_STATE_PTR(&COM_SM::SHARED_ConfiguringISPPipe);
}

OutputFormat_te COM_SM::convertFormatForISP(OMX_COLOR_FORMATTYPE omxformat)
{
    MSG1("ISP format is x%x\n",omxformat);
    OstTraceFiltStatic1(TRACE_DEBUG, "ISP format is x%x", (&mENSComponent),omxformat);
    switch ((t_uint32)omxformat)
    {
		case OMX_COLOR_Format16bitRGB565 :
			MSG0("Selected color format : RGB565\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Selected color format : RGB565", (&mENSComponent));
			return OutputFormat_e_RGB565_PEAKED;
		case OMX_COLOR_FormatYUV420Planar : // for backward compatibility
		case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
		case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:
		case OMX_COLOR_FormatCbYCrY:
		case OMX_COLOR_FormatYUV420PackedPlanar:
        case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV21:
        case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV12:
			MSG0("Selected color format : YUV\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "Selected color format : YUV", (&mENSComponent));
			return OutputFormat_e_YUV;
		case OMX_COLOR_Format16bitARGB4444:
			return OutputFormat_e_RGB444_PACKED_PEAKED;
		case OMX_COLOR_Format16bitARGB1555:
			return OutputFormat_e_RGB555_PEAKED;
		case OMX_COLOR_Format24bitRGB888:
		case OMX_COLOR_Format32bitARGB8888:
			return OutputFormat_e_RGB888_PEAKED;
		default :
			DBC_ASSERT(0);//omxformat has been corrupted
			return (OutputFormat_te)-1;
    }
}

/* */
/* COM_SM::colorPrimary2TransformType
 * This function converts ColorPrimary OMX setting into FW PE value
 */
/**/
Transform_te COM_SM::colorPrimary2TransformType(OMX_SYMBIAN_COLORPRIMARYTYPE aColorPrimary)
{
	Transform_te eTransformType = Transform_e_YCbCr_JFIF;
	switch(aColorPrimary)
	{
		case OMX_SYMBIAN_ColorPrimaryBT601FullRange:
			eTransformType = Transform_e_YCbCr_JFIF;
			break;
		case OMX_SYMBIAN_ColorPrimaryBT601LimitedRange:
			eTransformType = Transform_e_YCbCr_Rec601;
			break;
		case OMX_SYMBIAN_ColorPrimaryBT709FullRange:
			// eTransformType = ; TODO implement when FW exposes enum value
			break;
		case OMX_SYMBIAN_ColorPrimaryBT709LimitedRange:
			// eTransformType = ; TODO implement when FW exposes enum value
			break;
		default:
			DBC_ASSERT(0);
			break;
	}
	return eTransformType;
}
