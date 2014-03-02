/*
  Hybrid Filter Bank header file
*/

#ifndef _HYBRID_H
#define _HYBRID_H

#include "sbr_const.h"

#define HYBRID_FILTER_LENGTH  13
#define HYBRID_FILTER_DELAY    6

typedef enum {

  HYBRID_2_REAL = 2,
  HYBRID_4_CPLX = 4,
  HYBRID_8_CPLX = 8

} HYBRID_RES;

typedef struct
{
  int   nQmfBands;
  int   *pResolution;
  int   qmfBufferMove;

  Float *pWorkReal;
  Float *pWorkImag;

  Float **mQmfBufferReal;
  Float **mQmfBufferImag;
  Float *mTempReal;
  Float *mTempImag;

} HYBRID;

typedef HYBRID *HANDLE_HYBRID;

void
HybridAnalysis ( const Float **mQmfReal,
                 const Float **mQmfImag,
                 Float **mHybridReal,
                 Float **mHybridImag,
                 HANDLE_HYBRID hHybrid
#ifdef MMDSP
                 ,int scale,
                 int ps_scale
#endif
                 );

void
HybridSynthesis ( const Float **mHybridReal,
                  const Float **mHybridImag,
                  Float **mQmfReal,
                  Float **mQmfImag,
                  HANDLE_HYBRID hHybrid );

int
CreateHybridFilterBank ( HANDLE_HYBRID *phHybrid,
                         int noBands,
                         const int *pResolution,
                         Float **pPtr);

void
DeleteHybridFilterBank ( HANDLE_HYBRID *phHybrid );



#endif /* _HYBRID_H */

