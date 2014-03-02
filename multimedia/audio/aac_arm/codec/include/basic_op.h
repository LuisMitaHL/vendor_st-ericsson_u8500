/*
  Envelope calculation prototypes
*/
#ifndef __BASIC_OP_H
#define __BASIC_OP_H

#include "audiolibs_common.h"

#define L_abssat(a) ( (a) > 0 ? (a) : wL_negsat(a) ) 
#define L_abs(a) ( (a) > 0 ? (a) : wL_neg(a) ) 
#define INVSQRT2 5931642

#ifdef __flexcc2__
#define INLINED
#endif

#ifdef INLINED
struct long_long_pair {
	long long first;
	long long second;
};

#pragma ckf wX_div	250001
asm struct long_long_pair
wX_div( long long a, long long b, long long c )
{
	dmv @{c}, xr2
	X_div @{a}, @{b}, @{first}
	dmv xr2, @{second}
}

#pragma inline
static WORD56 L_div(WORD56 a, WORD56 b)
{
  int i;
  int sign_flag=0;
  WORD56 tmp;
  struct long_long_pair div;


  if(a<0)
  {
    sign_flag = 1;
    a =-a;
  }
  tmp = 0;

  div.first  = a;
  div.second = 0LL;
  
  for(i=0;i<47;i++)
  {
      div = wX_div( div.first, b, div.second );
  }

  tmp = div.second;
  
  if(sign_flag)
  {
      tmp = -tmp;
  }
  return tmp;
}

#pragma inline
static WORD24 short_div(WORD24 a, WORD24 b)
{
  int i;
  int sign_flag=0;
  WORD56 tmp,var1;
  struct long_long_pair div;

  var1 = a;
  tmp = 0;

  if(var1<0)
  {
    sign_flag = 1;
    var1 = -var1;
  }

  div.first  = var1;
  div.second = 0LL;

  for(i=0;i<23;i++)
  {
      div = wX_div( div.first, b, div.second );
  }

  tmp = div.second;

  if(sign_flag)
  {
      tmp = -tmp;
  }
  return (WORD24)tmp;
}

#else
WORD56 L_div(WORD56 a, WORD56 b);
WORD24 short_div(WORD24 a, WORD24 b);
#endif // INLINED
void add_MantExp(Float a_m,Float a_e,Float b_m,Float b_e,Float *ptrSum_m,Float *ptrSum_e);
Float aDivb_powInvc(Word24 a, Word24 b, Word24 c);

Word48 Mpy_48_24(Word48 a , Float b);

Float  get_max_value( Float **Real,
                     Float **Imag,
                    int start,
                    int stop,
                    int len);

int Rescale_Samples(Float **QmfBufferReal,
                    Float **QmfBufferImag,
                    int StartBand,
                    int StopBand,
                    int StartSlot,
                    int StopSlot,
                    int input_scaling,
                    int bUseLP
                    );

#endif /* __BASIC_OP_H */
