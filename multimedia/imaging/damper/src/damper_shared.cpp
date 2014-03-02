/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "DAMPER_SHARED"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include "osi_trace.h"
#include <stdio.h>
#include "damper_shared.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "damper_src_damper_sharedTraces.h"
#endif


/*
 * Debug functions
 */

#if (!defined(__SYMBIAN32__)&&defined(CONFIG_DEBUG))||(defined(__SYMBIAN32__)&&defined(_DEBUG))
#define RowCol2FlatIndex(NumCols,Row,Col) ((Row*NumCols)+Col)
static const char* stringifyMatrix(const float* pMatrix, int aRows, int aCols) {
   static char str[512]=""; int n=0;
   for(int i=0;i<aRows;i++) {
      n += snprintf(&str[n], sizeof(str)-1, "[");
      for(int j=0;j<aCols;j++) {
         n += snprintf(&str[n], sizeof(str)-1, "%#.2f ", ((const float*)(pMatrix))[RowCol2FlatIndex(aCols,i,j)]);
      }
      n += snprintf(&str[n], sizeof(str)-1, "]");
   }
   return (const char*)str;
}
#endif

/*
 * Initializer
 */
void CSharedDamper::Initialize()
{
   iInstanceName = "Anonymous";
   iNumDampers = 0;
   pDamperValues = NULL;
   pDamperFunc = NULL;
   bConfigChanged = false;
}

/*
 * Constructor
 */
CSharedDamper::CSharedDamper()
{
   Initialize();
   MSG1("[%s] instantiated\n", iInstanceName);
}

/*
 * Constructor
 */
CSharedDamper::CSharedDamper(TraceObject *traceobj):
		mTraceObject(traceobj)
{
   Initialize();
   MSG1("[%s] instantiated\n", iInstanceName);
   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% instantiated", (mTraceObject));
}

/*
 * Constructor
 */
CSharedDamper::CSharedDamper(const char* pInstanceName):
   iBaseA(pInstanceName, "Base A"),
   iBaseB(pInstanceName, "Base B")
{
   Initialize();
   if(pInstanceName != NULL) {
      iInstanceName = pInstanceName;
   }
   MSG1("[%s] instantiated\n", iInstanceName);
}

/*
 * Constructor
 */
CSharedDamper::CSharedDamper(const char* pInstanceName, TraceObject *traceobj):
   iBaseA(pInstanceName, "Base A"),
   iBaseB(pInstanceName, "Base B"),
   mTraceObject(traceobj)
{
   Initialize();
   if(pInstanceName != NULL) {
      iInstanceName = pInstanceName;
   }
   MSG1("[%s] instantiated\n", iInstanceName);
   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% instantiated", (mTraceObject));
}

/*
 * Destructor
 */
CSharedDamper::~CSharedDamper()
{
   if(Destroy()!= DAMPER_OK) {
      MSG1("[%s] destruction failed\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "CSharedDamper destruction failed", (mTraceObject));
      return;
   }
   MSG1("[%s] deleted\n", iInstanceName);
   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% deleted", (mTraceObject));
}

/*
 * Actual Constructor
 */
t_damper_error_code CSharedDamper::DoConstruct()
{
   // We assume that only 1 damper is driven
   return DoConstruct(1);
}

/*
 * Actual Destructor
 */
t_damper_error_code CSharedDamper::Destroy()
{
   if(pDamperValues != NULL) {
       delete [] pDamperValues;
       pDamperValues = NULL;
   }
   if(pDamperFunc != NULL) {
       delete [] pDamperFunc;
       pDamperFunc = NULL;
   }
   iNumDampers = 0;
   MSG1("[%s] destroyed\n", iInstanceName);
   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% destroyed", (mTraceObject));
   return DAMPER_OK;
}
   /*
 * Actual Constructor
 */
t_damper_error_code CSharedDamper::DoConstruct(const int aNumDampers)
{
   // Sanity check
   if(iNumDampers!=0)
   {
      MSG1("[%s] already constructed\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% already constructed", (mTraceObject));
      return DAMPER_ALREADY_CONSTRUCTED;
   }
   if(aNumDampers <=0)
   {
      MSG2("[%s] invalid number of dampers: %d\n", iInstanceName, aNumDampers);
      OstTraceFiltStatic1(TRACE_ERROR, "%DamperName% invalid number of dampers: %d", (mTraceObject), aNumDampers);
      return DAMPER_INVALID_ARGUMENT;
   }
   // Allocated Dampers Values arrays
   MSG2("[%s] allocating %d slots for values\n", iInstanceName, aNumDampers);
   OstTraceFiltStatic1(TRACE_DEBUG, "%DamperName% allocating %d slots for values", (mTraceObject), aNumDampers);
   pDamperValues = new t_damper_values[aNumDampers];
   if(pDamperValues == NULL) {
       MSG0("Failed to allocate space for damper values\n");
       OstTraceFiltStatic0(TRACE_ERROR, "Failed to allocate space for damper values", (mTraceObject));
       return DAMPER_MALLOC_FAILED;
   }
   // Alocated Dampers functions
   MSG2("[%s] allocating %d slots for functions\n", iInstanceName, aNumDampers);
   OstTraceFiltStatic1(TRACE_DEBUG, "%DamperName% allocating %d slots for functions", (mTraceObject), aNumDampers);
   pDamperFunc = new CDamperFunc[aNumDampers];
   if(pDamperFunc == NULL) {
       MSG0("Failed to allocate space for damper functions\n");
       OstTraceFiltStatic0(TRACE_ERROR, "Failed to allocate space for damper functions", (mTraceObject));
       return DAMPER_MALLOC_FAILED;
   }
   // Defaults
   const float iDefaultCtrlPoints[1] = { 0.0 };
   for(int i=0; i<aNumDampers; i++) {
      pDamperValues[i].numRows = 1;
      pDamperValues[i].numCols = 1;
      pDamperValues[i].values[0] = 0.0;
      int res = pDamperFunc[i].Configure( pDamperValues[i].numRows,
                                          pDamperValues[i].numCols,
                                          iDefaultCtrlPoints,
                                          iDefaultCtrlPoints,
                                          pDamperValues[i].values);
      if(res!=0)
      {
          MSG2("[%s][%d] configuration failed\n", iInstanceName, i);
          OstTraceFiltStatic1(TRACE_ERROR, "%DamperName%[%d] configuration failed", (mTraceObject), i);
          return DAMPER_CONFIGURE_FAILED;
      }
   }
   // Construction done
   iNumDampers = aNumDampers;
   return DAMPER_OK;
}

/*
 * Setters
 */
t_damper_error_code CSharedDamper::SetBaseA(const t_damper_base_id aId)
{
   t_damper_base_error_code baseErr = DAMPER_BASE_OK;
   if(iNumDampers==0)
   {
      MSG1("[%s] not constructed\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% not constructed", (mTraceObject));
      return DAMPER_NOT_CONSTRUCTED;
   }
   MSG1("[%s] setting Base A\n", iInstanceName);
   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% setting Base A", (mTraceObject));
   baseErr = iBaseA.SetBase(aId);
   if(baseErr != DAMPER_BASE_OK)
   {
      MSG2("[%s] failed to set Base A: err=%d\n", iInstanceName, baseErr);
      OstTraceFiltStatic1(TRACE_ERROR, "%DamperName% failed to set Base A: err=%d", (mTraceObject), baseErr);
      return DAMPER_BASE_ERROR;
   }
   bConfigChanged = true;
   return DAMPER_OK;
}

t_damper_error_code CSharedDamper::SetBaseB(const t_damper_base_id aId)
{
   t_damper_base_error_code baseErr = DAMPER_BASE_OK;
   if(iNumDampers==0)
   {
      MSG1("[%s] not constructed\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% not constructed", (mTraceObject));
      return DAMPER_NOT_CONSTRUCTED;
   }
   MSG1("[%s] setting Base B\n", iInstanceName);
   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% setting Base B", (mTraceObject));
   baseErr = iBaseB.SetBase(aId);
   if(baseErr != DAMPER_BASE_OK)
   {
      MSG2("[%s] failed to set Base B: err=%d\n", iInstanceName, baseErr);
      OstTraceFiltStatic1(TRACE_ERROR, "%DamperName% failed to set Base B: err=%d", (mTraceObject), baseErr);
      return DAMPER_BASE_ERROR;
   }
   bConfigChanged = true;
   return DAMPER_OK;
}

t_damper_error_code CSharedDamper::SetBaseAControlPoints(const float* pValues, const int aNumValues)
{
   t_damper_base_error_code baseErr = DAMPER_BASE_OK;
   if(iNumDampers==0)
   {
      MSG1("[%s] not constructed\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% not constructed", (mTraceObject));
      return DAMPER_NOT_CONSTRUCTED;
   }
   MSG1("[%s] setting Base A control points\n", iInstanceName);
   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% setting Base A control points", (mTraceObject));
   baseErr = iBaseA.SetControlPoints(pValues, aNumValues);
   if(baseErr != DAMPER_BASE_OK)
   {
      MSG2("[%s] failed to set Base A control points: err=%d\n", iInstanceName, baseErr);
      OstTraceFiltStatic1(TRACE_ERROR, "%DamperName% failed to set Base A control points: err=%d", (mTraceObject), baseErr);
      return DAMPER_BASE_ERROR;
   }
   bConfigChanged = true;
   return DAMPER_OK;
}

t_damper_error_code CSharedDamper::SetBaseBControlPoints(const float* pValues, const int aNumValues)
{
   t_damper_base_error_code baseErr = DAMPER_BASE_OK;
   if(iNumDampers==0)
   {
      MSG1("[%s] not constructed\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% not constructed", (mTraceObject));
      return DAMPER_NOT_CONSTRUCTED;
   }
   MSG1("[%s] setting Base B control points\n", iInstanceName);
   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% setting Base B control points", (mTraceObject));
   baseErr = iBaseB.SetControlPoints(pValues, aNumValues);
   if(baseErr != DAMPER_BASE_OK)
   {
      MSG2("[%s] failed to set Base B control points: err=%d\n", iInstanceName, baseErr);
      OstTraceFiltStatic1(TRACE_ERROR, "%DamperName% failed to set Base B control points: err=%d", (mTraceObject), baseErr);
      return DAMPER_BASE_ERROR;
   }
   bConfigChanged = true;
   return DAMPER_OK;
}

t_damper_error_code CSharedDamper::DoSetDamper( const float* pValues,
                                                const int    aNumRows,
                                                const int    aNumCols)
{
   // We assume that only 1 damper is driven (index 0)
   return DoSetDamper( 0, pValues, aNumRows, aNumCols);
}

t_damper_error_code CSharedDamper::DoSetDamper( const int    aDamperIndex,
                                                const float* pValues,
                                                const int    aNumRows,
                                                const int    aNumCols)
{
   // Sanity Checks
   if(iNumDampers==0)
   {
      MSG1("[%s] not constructed\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% not constructed", (mTraceObject));
      return DAMPER_NOT_CONSTRUCTED;
   }
   if(aDamperIndex< 0 || aDamperIndex>=iNumDampers)
   {
      MSG2("[%s] invalid index: aDamperIndex=%d\n", iInstanceName, aDamperIndex);
      OstTraceFiltStatic1(TRACE_ERROR, "%DamperName% invalid index: aDamperIndex=%d", (mTraceObject), aDamperIndex);
      return DAMPER_INVALID_ARGUMENT;
   }
   if( pValues==NULL || aNumRows<=0 || aNumCols<=0)
   {
      MSG4("[%s] invalid argument: pValues=%p, aNumRows=%d, aNumCols=%d\n", iInstanceName, pValues, aNumRows, aNumCols);
      OstTraceFiltStatic3(TRACE_ERROR, "%DamperName% invalid argument: pValues=%x, aNumRows=%d, aNumCols=%d", (mTraceObject), (t_uint32)pValues, aNumRows, aNumCols);
      return DAMPER_INVALID_ARGUMENT;
   }
   if( aNumRows > CDAMPER_CTRLP_MAX_DIM || aNumCols > CDAMPER_CTRLP_MAX_DIM )
   {
      MSG3("[%s] dimensions too big: aNumRows=%d, aNumCols=%d\n", iInstanceName, aNumRows, aNumCols);
      OstTraceFiltStatic2(TRACE_ERROR, "%DamperName% dimensions too big: aNumRows=%d, aNumCols=%d", (mTraceObject), aNumRows, aNumCols);
      return DAMPER_INVALID_ARGUMENT;
   }
   // Check damper against Bases dimensions
   if(aNumRows != (int)iBaseA.GetNumPoints())
   {
      MSG3("[%s] number of rows does not match Base A size: aNumRows=%d, BaseA=%d \n", iInstanceName, aNumRows, iBaseA.GetNumPoints());
      OstTraceFiltStatic2(TRACE_ERROR, "%DamperName% number of rows does not match Base A size: aNumRows=%d, BaseA=%d ", (mTraceObject), aNumRows, iBaseA.GetNumPoints());
      return DAMPER_SIZE_MISMATCH;
   }
   if(aNumCols != (int)iBaseB.GetNumPoints())
   {
      MSG3("[%s] number of columns does not match Base B size: aNumCols=%d, BaseB=%d \n", iInstanceName, aNumCols, iBaseB.GetNumPoints());
      OstTraceFiltStatic2(TRACE_ERROR, "%DamperName% number of columns does not match Base B size: aNumCols=%d, BaseB=%d ", (mTraceObject), aNumCols, iBaseB.GetNumPoints());
      return DAMPER_SIZE_MISMATCH;
   }
   // Copy values
   for(int i=0; i<aNumRows*aNumCols; i++)
   {
      pDamperValues[aDamperIndex].values[i] = pValues[i];
   }
   // Store Damper dimension
   pDamperValues[aDamperIndex].numRows = (unsigned int)aNumRows;
   pDamperValues[aDamperIndex].numCols = (unsigned int)aNumCols;

   MSG3("[%s][%d] Values: %s\n", iInstanceName, aDamperIndex, stringifyMatrix(pDamperValues[aDamperIndex].values, pDamperValues[aDamperIndex].numRows, pDamperValues[aDamperIndex].numCols));
   //OstTraceFiltStatic2(TRACE_DEBUG, "%DamperName%[%d] Values: %s", (mTraceObject), aDamperIndex, stringifyMatrix(pDamperValues[aDamperIndex].values, pDamperValues[aDamperIndex].numRows, pDamperValues[aDamperIndex].numCols));

   // Done
   bConfigChanged = true;
   return DAMPER_OK;
}

t_damper_error_code CSharedDamper::Configure()
{
   int res = 0;

   // Sanity Checks
   if(iNumDampers==0)
   {
      MSG1("[%s] not constructed\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% not constructed", (mTraceObject));
      return DAMPER_NOT_CONSTRUCTED;
   }

   MSG2("[%s] Base A: <%s>\n", iInstanceName, iBaseA.GetBaseString());
   //OstTraceFiltStatic2(TRACE_DEBUG, "%DamperName% Base A: <%s>", (mTraceObject), iBaseA.GetBaseString());
   MSG2("[%s] Base B: <%s>\n", iInstanceName, iBaseB.GetBaseString());
   //OstTraceFiltStatic2(TRACE_DEBUG, "%DamperName% Base B: <%s>", (mTraceObject), iBaseB.GetBaseString());

   MSG1("[%s] checking Base A sanity\n", iInstanceName);
   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% checking Base A sanity", (mTraceObject));
   if( iBaseA.IsOk() == false)
   {
      MSG1("[%s] Base A is not OK\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% Base A is not OK", (mTraceObject));
      return DAMPER_BASE_ERROR;
   }

   MSG1("[%s] checking Base B sanity\n", iInstanceName);
   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% checking Base B sanity", (mTraceObject));
   if( iBaseB.IsOk() == false)
   {
      MSG1("[%s] Base B is not OK\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% Base B is not OK", (mTraceObject));
      return DAMPER_BASE_ERROR;
   }

   // Get Dampers bases
   unsigned int baseASize    = iBaseA.GetNumPoints();
   unsigned int baseBSize    = iBaseB.GetNumPoints();
   const float* pBaseAPoints = iBaseA.GetControlPoints();
   const float* pBaseBPoints = iBaseB.GetControlPoints();

   // Configure all the dampers
   for(int damperIndex=0; damperIndex<iNumDampers; damperIndex++)
   {
      // Check the damper sanity
      if( pDamperValues[damperIndex].numRows != baseASize)
      {
         MSG4("[%s][%d] number of rows (%d) does not match number of control points of Base A (%d)\n", iInstanceName, damperIndex, pDamperValues[damperIndex].numRows, baseASize);
         OstTraceFiltStatic3(TRACE_ERROR, "%DamperName%[%d] number of rows (%d) does not match number of control points of Base A (%d)", (mTraceObject), damperIndex, pDamperValues[damperIndex].numRows, baseASize);
         return DAMPER_SIZE_MISMATCH;
      }
      if( pDamperValues[damperIndex].numCols != baseBSize)
      {
         MSG4("[%s][%d] number of cols (%d) does not match number of control points of Base B (%d)\n", iInstanceName, damperIndex, pDamperValues[damperIndex].numCols, baseBSize);
         OstTraceFiltStatic3(TRACE_ERROR, "%DamperName%[%d] number of cols (%d) does not match number of control points of Base B (%d)", (mTraceObject), damperIndex, pDamperValues[damperIndex].numCols, baseBSize);
         return DAMPER_SIZE_MISMATCH;
      }
      // Configure the damper
      res = pDamperFunc[damperIndex].Configure( baseASize, baseBSize, pBaseAPoints, pBaseBPoints, pDamperValues[damperIndex].values);
      if(res!=0)
      {
          MSG2("[%s][%d] configuration failed\n", iInstanceName, damperIndex);
          OstTraceFiltStatic1(TRACE_ERROR, "%DamperName%[%d] configuration failed", (mTraceObject), damperIndex);
          return DAMPER_CONFIGURE_FAILED;
      }
      MSG2("[%s][%d] configuration done\n", iInstanceName, damperIndex);
      OstTraceFiltStatic1(TRACE_DEBUG, "%DamperName%[%d] configuration done", (mTraceObject), damperIndex);
   }
   // Done
   bConfigChanged = false;
   return DAMPER_OK;
}

/*
 * Getters
 */
t_damper_error_code CSharedDamper::DoEvaluate( const t_damper_base_values* pBaseValues,
                                               float*                      pValues)
{
   int   res = 0;
   float fBase_A_Value = 0.0;
   float fBase_B_Value = 0.0;

   // Sanity Checks
   if(iNumDampers==0)
   {
      MSG1("[%s] not constructed\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% not constructed", (mTraceObject));
      return DAMPER_NOT_CONSTRUCTED;
   }
   if(bConfigChanged==true)
   {
      MSG1("[%s] configuration changed: call to DoConfigure() needed to apply the changes\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% configuration changed: call to DoConfigure() needed to apply the changes", (mTraceObject));
      return DAMPER_CONFIGURE_NEEDED;
   }

   // Select Damper Bases
   fBase_A_Value = iBaseA.SelectBase(pBaseValues);
   MSG3("[%s] Base A: <%s> = %f\n", iInstanceName, iBaseA.GetBaseString(),fBase_A_Value);
   //OstTraceFiltStatic2(TRACE_DEBUG, "%DamperName% Base A: <%s> = %f", (mTraceObject), iBaseA.GetBaseString(),fBase_A_Value);

   fBase_B_Value = iBaseB.SelectBase(pBaseValues);
   MSG3("[%s] Base B: <%s> = %f\n", iInstanceName, iBaseB.GetBaseString(), fBase_B_Value);
   //OstTraceFiltStatic2(TRACE_DEBUG, "%DamperName% Base B: <%s> = %f", (mTraceObject), iBaseB.GetBaseString(), fBase_B_Value);

   // Do evaluate all dampers
   for(int damperIndex=0; damperIndex<iNumDampers; damperIndex++)
   {
      res = pDamperFunc[damperIndex].Evaluate( &pValues[damperIndex], fBase_A_Value, fBase_B_Value);
      if(res != 0)
      {
         MSG2("[%s][%d] evaluation failed\n", iInstanceName, damperIndex);
         OstTraceFiltStatic1(TRACE_ERROR, "%DamperName%[%d] evaluation failed", (mTraceObject), damperIndex);
         return DAMPER_EVALUATION_FAILED;
      }
      MSG3("[%s][%d]=%f\n", iInstanceName, damperIndex, pValues[damperIndex]);
      OstTraceFiltStatic2(TRACE_DEBUG, "%DamperName%[%d]=%f", (mTraceObject), damperIndex, pValues[damperIndex]);
   }
   return DAMPER_OK;
}

t_damper_error_code CSharedDamper::DoEvaluate( const int                   aDamperIndex,
                                               const t_damper_base_values* pBaseValues,
                                               float*                      pValue)
{
   int   res = 0;
   float fBase_A_Value = 0.0;
   float fBase_B_Value = 0.0;

   // Sanity Checks
   if(iNumDampers==0)
   {
      MSG1("[%s] not constructed\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% not constructed", (mTraceObject));
      return DAMPER_NOT_CONSTRUCTED;
   }
   if(aDamperIndex< 0 || aDamperIndex>=iNumDampers)
   {
      MSG2("[%s] invalid index: aDamperIndex=%d\n", iInstanceName, aDamperIndex);
      OstTraceFiltStatic1(TRACE_ERROR, "%DamperName% invalid index: aDamperIndex=%d", (mTraceObject), aDamperIndex);
      return DAMPER_INVALID_ARGUMENT;
   }
   if(bConfigChanged==true)
   {
      MSG1("[%s] configuration changed: call to DoConfigure() needed to apply the changes\n", iInstanceName);
      OstTraceFiltStatic0(TRACE_ERROR, "%DamperName% configuration changed: call to DoConfigure() needed to apply the changes", (mTraceObject));
      return DAMPER_CONFIGURE_NEEDED;
   }

   // Select Damper Bases
   fBase_A_Value = iBaseA.SelectBase(pBaseValues);
   MSG3("[%s] Base A: <%s> = %f\n", iInstanceName, iBaseA.GetBaseString(),fBase_A_Value);
   //OstTraceFiltStatic2(TRACE_DEBUG, "%DamperName% Base A: <%s> = %f", (mTraceObject), iBaseA.GetBaseString(),fBase_A_Value);

   fBase_B_Value = iBaseB.SelectBase(pBaseValues);
   MSG3("[%s] Base B: <%s> = %f\n", iInstanceName, iBaseB.GetBaseString(), fBase_B_Value);
   //OstTraceFiltStatic2(TRACE_DEBUG, "%DamperName% Base B: <%s> = %f", (mTraceObject), iBaseB.GetBaseString(), fBase_B_Value);

   res = pDamperFunc[aDamperIndex].Evaluate( pValue, fBase_A_Value, fBase_B_Value);
   if(res != 0)
   {
      MSG2("[%s][%d] evaluation failed\n", iInstanceName, aDamperIndex);
      OstTraceFiltStatic1(TRACE_ERROR, "%DamperName%[%d] evaluation failed", (mTraceObject), aDamperIndex);
      return DAMPER_EVALUATION_FAILED;
   }
   MSG3("[%s][%d]=%f\n", iInstanceName, aDamperIndex, pValue[aDamperIndex]);
   OstTraceFiltStatic2(TRACE_DEBUG, "%DamperName%[%d]=%f", (mTraceObject), aDamperIndex, pValue[aDamperIndex]);
   return DAMPER_OK;
}

bool CSharedDamper::ConfigChanged()
{
   MSG2("[%s] config changed: %s\n", iInstanceName, bConfigChanged ? "yes" : "no");
   if (bConfigChanged){
	   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% config changed: yes", (mTraceObject));
	}
   else{
	   OstTraceFiltStatic0(TRACE_DEBUG, "%DamperName% config changed: no", (mTraceObject));
   }
   return bConfigChanged;
}
