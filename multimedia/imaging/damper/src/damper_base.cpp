/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* +CR337836 */
#define DECLARE_AUTOVAR
/* -CR337836 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "DAMPER_BASE"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include "damper_base.h"
#include "osi_trace.h"
#include <stdio.h>

/*
 * Local definitions
 */
#define HASH_ENTRY(id) {id,#id}

typedef struct
{
   t_damper_base_id id;
   const char* name;
} t_damper_base_name_entry;


static const t_damper_base_name_entry KDamperBaseNamesList[DAMPER_BASE_COUNT] =
{
   { DAMPER_BASE_NONE,                         "None"                      },
   { DAMPER_BASE_INTEGRATION_TIME,             "IntegrationTime"           },
   { DAMPER_BASE_ANALOG_GAIN,                  "AnalogGain"                },
   { DAMPER_BASE_POST_DG_MINIMUM_GAIN,         "PostDgMinimumGain"         },
   { DAMPER_BASE_POST_DG_AVERAGE_GAIN,         "PostDgAverageGain"         },
   { DAMPER_BASE_POST_ER_MINIMUM_GAIN,         "PostErMinimumGain"         },
   { DAMPER_BASE_POST_ER_AVERAGE_GAIN,         "PostErAverageGain"         },
   { DAMPER_BASE_POST_CS_MINIMUM_GAIN,         "PostCsMinimumGain"         },
   { DAMPER_BASE_POST_CS_AVERAGE_GAIN,         "PostCsAverageGain"         },
   { DAMPER_BASE_NORMALIZED_PIXEL_LEVEL,       "NormalizedPixelLevel"      },
   { DAMPER_BASE_NORMALIZED_PIXEL_LEVEL_RED,   "NormalizedPixelLevelRed"   },
   { DAMPER_BASE_NORMALIZED_PIXEL_LEVEL_GREEN, "NormalizedPixelLevelGreen" },
   { DAMPER_BASE_NORMALIZED_PIXEL_LEVEL_BLUE,  "NormalizedPixelLevelBlue"  },
   { DAMPER_BASE_NORMALIZED_SNR,               "NormalizedSnr"             },
   { DAMPER_BASE_NORMALIZED_SNR_RED,           "NormalizedSnrRed"          },
   { DAMPER_BASE_NORMALIZED_SNR_GREEN,         "NormalizedSnrGreen"        },
   { DAMPER_BASE_NORMALIZED_SNR_BLUE,          "NormalizedSnrBlue"         }
};

#define KDamperBaseNamesListSize (sizeof(KDamperBaseNamesList)/sizeof(KDamperBaseNamesList[0]))

/*
 * Debug functions
 */
#if (!defined(__SYMBIAN32__)&&defined(CONFIG_DEBUG))||(defined(__SYMBIAN32__)&&defined(_DEBUG))
static const char* stringifyVector(const float* pVector, unsigned int aNumElements) {
   static char str[512]=""; int n=0;
   n += snprintf(&str[n], sizeof(str)-1, "[");
   for(unsigned int i=0;i<aNumElements;i++) {
      n += snprintf(&str[n], sizeof(str)-1, "%#.2f ", ((const float*)(pVector))[i]);
   }
   n += snprintf(&str[n], sizeof(str)-1, "]");
   return (const char*)str;
}
#endif


/*
 * Constructor
 */
void CDamperBase::Initialize()
{
   iDamperName = "Anonymous";
   iInstanceName = "Anonymous";
   iBase = DAMPER_BASE_NONE;
   iSize = 1;
   iControlPoints[0] = 0.0;
}

/*
 * Constructor
 */
CDamperBase::CDamperBase()
{
   GET_AND_SET_TRACE_LEVEL(damper);
   Initialize();
}

/*
 * Constructor
 */
CDamperBase::CDamperBase(const char* pDamperName, const char* pInstanceName)
{
   GET_AND_SET_TRACE_LEVEL(damper);
   Initialize();
   if(pDamperName!=NULL) {
      iDamperName = pDamperName;
   }
   if(pInstanceName!=NULL) {
      iInstanceName = pInstanceName;
   }
}

/*
 * Damper Base setters
 */
t_damper_base_error_code CDamperBase::SetBase(const t_damper_base_id aBase)
{
   if(aBase<DAMPER_BASE_NONE || aBase>=DAMPER_BASE_COUNT)
   {
      MSG3("[%s][%s] invalid argument: aBase=%d\n", iDamperName, iInstanceName, aBase);
      return DAMPER_BASE_INVALID_ARGUMENT;
   }
   if(aBase==DAMPER_BASE_NONE)
   {
      iSize = 1;
      iControlPoints[0] = 0.0;
   }
   iBase = aBase;
   MSG3("[%s][%s] set to <%s>\n", iDamperName, iInstanceName, GetBaseString());
   return DAMPER_BASE_OK;
}

t_damper_base_error_code CDamperBase::SetControlPoints(const float* pValues, const int aNumValues)
{
   // Sanity Check
   if(pValues==NULL || aNumValues<=0 || aNumValues>CDAMPER_CTRLP_MAX_DIM)
   {
      MSG4("[%s][%s] invalid argument: pValues=%p, aNumValues=%d\n", iDamperName, iInstanceName, pValues, aNumValues);
      return DAMPER_BASE_INVALID_ARGUMENT;
   }
   // Copy values
   for(int i=0; i<aNumValues; i++)
   {
      iControlPoints[i] = pValues[i];
   }
   iSize = (unsigned int)aNumValues;
   MSG3("[%s][%s] iControlPoints = %s\n", iDamperName, iInstanceName, stringifyVector(iControlPoints, iSize));
   return DAMPER_BASE_OK;
}

bool CDamperBase::IsOk()
{
   // Sanity Check
   if( iBase == DAMPER_BASE_NONE && iSize!=1)
   {
      MSG2("[%s][%s] base is DAMPER_BASE_NONE but size!=1\n",iDamperName, iInstanceName);
      MSG2("[%s][%s] possible cause: Control Points have been set but base selector was not set (i.e. call to CDamperBase::SetBase() needed)\n", iDamperName, iInstanceName);
      return false;
   }
   return true;
}

const float* CDamperBase::GetControlPoints()
{
   return (const float*)iControlPoints;
}

unsigned int CDamperBase::GetNumPoints()
{
   return iSize;
}

float CDamperBase::SelectBase(const t_damper_base_values* pBaseValues)
{
   switch(iBase)
   {
      case DAMPER_BASE_NONE:                         return 0.0;
      case DAMPER_BASE_INTEGRATION_TIME:             return pBaseValues->IntegrationTime;
      case DAMPER_BASE_ANALOG_GAIN:                  return pBaseValues->AnalogGain;
      case DAMPER_BASE_POST_DG_MINIMUM_GAIN:         return pBaseValues->PostDgMinimumGain;
      case DAMPER_BASE_POST_DG_AVERAGE_GAIN:         return pBaseValues->PostDgAverageGain;
      case DAMPER_BASE_POST_ER_MINIMUM_GAIN:         return pBaseValues->PostErMinimumGain;
      case DAMPER_BASE_POST_ER_AVERAGE_GAIN:         return pBaseValues->PostErAverageGain;
      case DAMPER_BASE_POST_CS_MINIMUM_GAIN:         return pBaseValues->PostCsMinimumGain;
      case DAMPER_BASE_POST_CS_AVERAGE_GAIN:         return pBaseValues->PostCsAverageGain;
      case DAMPER_BASE_NORMALIZED_PIXEL_LEVEL:       return pBaseValues->NomalizedPixelLevel;
      case DAMPER_BASE_NORMALIZED_PIXEL_LEVEL_RED:   return pBaseValues->NomalizedPixelLevelRed;
      case DAMPER_BASE_NORMALIZED_PIXEL_LEVEL_GREEN: return pBaseValues->NomalizedPixelLevelGreen;
      case DAMPER_BASE_NORMALIZED_PIXEL_LEVEL_BLUE:  return pBaseValues->NomalizedPixelLevelBlue;
      case DAMPER_BASE_NORMALIZED_SNR:               return pBaseValues->NomalizedSnr;
      case DAMPER_BASE_NORMALIZED_SNR_RED:           return pBaseValues->NomalizedSnrRed;
      case DAMPER_BASE_NORMALIZED_SNR_GREEN:         return pBaseValues->NomalizedSnrGreen;
      case DAMPER_BASE_NORMALIZED_SNR_BLUE:          return pBaseValues->NomalizedSnrBlue;
      default:                                       return 0.0;
   }
}

/*
 * This function converts a Base Id into a human readable string
 */
const char* CDamperBase::GetBaseString()
{
   return GetBaseString(iBase);
}

const char* CDamperBase::GetBaseString(t_damper_base_id aBaseId)
{
   for(unsigned int i=0; i<KDamperBaseNamesListSize; i++)
   {
      if(KDamperBaseNamesList[i].id == aBaseId)
         return KDamperBaseNamesList[i].name;
   }
   return "*** DAMPER BASE NAME NOT FOUND ***";
}

