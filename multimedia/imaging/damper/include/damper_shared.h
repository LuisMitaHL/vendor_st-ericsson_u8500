/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _DAMPER_SHARED_H_
#define _DAMPER_SHARED_H_

#include <inc/typedef.h>
#include "damper_func.h"
#include "damper_base.h"
#include "TraceObject.h"

typedef enum
{
   DAMPER_OK = 0,
   DAMPER_NOT_CONSTRUCTED,
   DAMPER_ALREADY_CONSTRUCTED,
   DAMPER_MALLOC_FAILED,
   DAMPER_INVALID_ARGUMENT,
   DAMPER_SIZE_MISMATCH,
   DAMPER_UNSUPPORTED_OPERATIVE_MODE,
   DAMPER_TUNING_DB_ERROR,
   DAMPER_CONFIGURE_FAILED,
   DAMPER_BASE_ERROR,
   DAMPER_INCORRECT_TUNING_VALUE,
   DAMPER_CONFIGURE_NEEDED,
   DAMPER_EVALUATION_FAILED
} t_damper_error_code;

class CSharedDamper
{
   public:
     t_damper_error_code SetBaseA(const t_damper_base_id);
     t_damper_error_code SetBaseAControlPoints(const float*, const int);
     t_damper_error_code SetBaseB(const t_damper_base_id);
     t_damper_error_code SetBaseBControlPoints(const float*, const int);
     bool                ConfigChanged();
     t_damper_error_code Configure();
     t_damper_error_code Destroy();

   protected:
     CSharedDamper();
     CSharedDamper(TraceObject *);
     CSharedDamper(const char*);
     CSharedDamper(const char*, TraceObject *);
     ~CSharedDamper();
     t_damper_error_code DoConstruct();
     t_damper_error_code DoConstruct(const int);
     t_damper_error_code DoSetDamper(const float*, const int, const int);
     t_damper_error_code DoSetDamper(const int, const float*, const int, const int);
     t_damper_error_code DoEvaluate(const t_damper_base_values*, float*);
     t_damper_error_code DoEvaluate(const int, const t_damper_base_values*, float*);

   private:

     typedef struct
     {
        unsigned int numRows;
        unsigned int numCols;
        float values[CDAMPER_CTRLP_MAX_DIM*CDAMPER_CTRLP_MAX_DIM];
     } t_damper_values;

     void                 Initialize();
     const char*          iInstanceName;
     int                  iNumDampers;
     CDamperBase          iBaseA;
     CDamperBase          iBaseB;
     t_damper_values*     pDamperValues;
     CDamperFunc*         pDamperFunc;
     bool                 bConfigChanged;
public:
     TraceObject*         mTraceObject;
};

#endif // _DAMPER_SHARED_H_
