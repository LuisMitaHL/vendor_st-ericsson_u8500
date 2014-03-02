/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "RSO_DAMPER"
#define OMX_TRACE_UID 0x10
#define DBGT_LAYER 1
/*
 * Includes
 */
#include "osi_trace.h"
#include "rso_damper.h"
#include "VhcElementDefs.h"


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_isp_common_rso_damperTraces.h"
#endif

#define HASH_ENTRY(i) i,#i

typedef struct
{
   t_rso_damper_id id;
   const char*     name;
   t_uint32        addr;
} t_rso_damper_info;

static const t_rso_damper_info KRsoDamperInfo[RSO_DAMPERS_NUM] =
{
   { HASH_ENTRY(RSO_DCTERM_RR), RSO_DataCtrl_u16_DcTermR_Byte0 },
   { HASH_ENTRY(RSO_SLANTX_RR), RSO_DataCtrl_u32_XCoefR_Byte0  },
   { HASH_ENTRY(RSO_SLANTY_RR), RSO_DataCtrl_u32_YCoefR_Byte0  },

   { HASH_ENTRY(RSO_DCTERM_GR), RSO_DataCtrl_u16_DcTermGr_Byte0 },
   { HASH_ENTRY(RSO_SLANTX_GR), RSO_DataCtrl_u32_XCoefGr_Byte0  },
   { HASH_ENTRY(RSO_SLANTY_GR), RSO_DataCtrl_u32_YCoefGr_Byte0  },

   { HASH_ENTRY(RSO_DCTERM_GB), RSO_DataCtrl_u16_DcTermGb_Byte0 },
   { HASH_ENTRY(RSO_SLANTX_GB), RSO_DataCtrl_u32_XCoefGb_Byte0  },
   { HASH_ENTRY(RSO_SLANTY_GB), RSO_DataCtrl_u32_YCoefGb_Byte0  },

   { HASH_ENTRY(RSO_DCTERM_BB), RSO_DataCtrl_u16_DcTermB_Byte0 },
   { HASH_ENTRY(RSO_SLANTX_BB), RSO_DataCtrl_u32_XCoefB_Byte0  },
   { HASH_ENTRY(RSO_SLANTY_BB), RSO_DataCtrl_u32_YCoefB_Byte0  }
};

/*
 * Constructor
 */
CRsoDamper::CRsoDamper(TraceObject *traceobj):CSharedDamper("RSO", traceobj)
{
}

/*
 * Actual Constructor
 */
t_damper_error_code CRsoDamper::Construct()
{
   return DoConstruct(RSO_DAMPERS_NUM);
}

/*
 * This function sets the RSO dampers
 */
t_damper_error_code CRsoDamper::SetDamper( const t_rso_damper_id aDamperId,
                                           const float*          pValues,
                                           const int             aNumRows,
                                           const int             aNumCols)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CRsoDamper::SetDamper", (mTraceObject));
   t_damper_error_code err = DAMPER_OK;
   if(aDamperId<0 || aDamperId>=RSO_DAMPERS_NUM)
   {
      DBGT_ERROR("Invalid damper Id: %d\n", aDamperId);
      OstTraceFiltStatic1(TRACE_ERROR, "Invalid damper Id: %d", (mTraceObject), aDamperId);
      OUTR(" ",DAMPER_INVALID_ARGUMENT);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CRsoDamper::SetDamper (%d)", (mTraceObject), DAMPER_INVALID_ARGUMENT);
      return DAMPER_INVALID_ARGUMENT;
   }
   MSG3("[%s], NumRows=%d, NumCols=%d\n", KRsoDamperInfo[aDamperId].name, aNumRows, aNumCols);
   OstTraceFiltStatic3(TRACE_DEBUG, "KRsoDamperInfo[%d].name, NumRows=%d, NumCols=%d", (mTraceObject), aDamperId, aNumRows, aNumCols);
   err = DoSetDamper( aDamperId, pValues, aNumRows, aNumCols);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("[%s] failed to set damper: err=%d\n", KRsoDamperInfo[aDamperId].name, err);
      OstTraceFiltStatic2(TRACE_ERROR, "KRsoDamperInfo[%d].name failed to set damper: err=%d", (mTraceObject), aDamperId, err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CRsoDamper::SetDamper (%d)", (mTraceObject), err);
      return err;
   }
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CRsoDamper::SetDamper (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}

/*
 * This function computes the dampers and queues the Page Elements
 */
t_damper_error_code CRsoDamper::ComputeAndQueuePEs( CIspctlCom*                 pIspctlCom,
                                                    const t_damper_base_values* pBaseValues)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CRsoDamper::ComputeAndQueuePEs", (mTraceObject));
   float pDamperValues[RSO_DAMPERS_NUM] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
   t_damper_error_code err = DAMPER_OK;

   // Evaluate RSO dampers
   err = DoEvaluate( pBaseValues, pDamperValues);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("Evaluation failed: err=%d\n", err);
      OstTraceFiltStatic1(TRACE_ERROR, "Evaluation failed: err=%d", (mTraceObject), err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CRsoDamper::ComputeAndQueuePEs (%d)", (mTraceObject), err);
      return err;
   }

   // Queue the Page Elements
   for(int i=0; i<RSO_DAMPERS_NUM; i++)
   {
      MSG2("%s = %f\n", CIspctlCom::pIspctlSensor->GetPeName(KRsoDamperInfo[i].addr), pDamperValues[i]);
      //OstTraceFiltStatic2(TRACE_DEBUG, "%s = %f", (mTraceObject), CXp70::GetPeName(KRsoDamperInfo[i].addr), pDamperValues[i]);
      pIspctlCom->queuePE( KRsoDamperInfo[i].addr, (t_uint32)pDamperValues[i]);
   }

   // Done
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CRsoDamper::ComputeAndQueuePEs (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}
