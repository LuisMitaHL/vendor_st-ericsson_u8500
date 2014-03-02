
#ifndef _softpfloat_h_
#define _softpfloat_h_

#include "audiolibs_common.h"

#define SPFLOAT_MEM __EXTERN /* all tables moved to external memory */

//#define INLINE __inline__
//#define SPFLOAT_LONGLONGTYPE

#ifdef SPFLOAT_LONGLONGTYPE

typedef WORD48 spfloat;   
typedef WORD24 spfmant;
typedef WORD24 spfexp;
typedef WORD56 spfwork;


#define  SPFLOAT_BITS 48
#define  SPFLOAT_MANT_LENGTH 24
#define  SPFLOAT_EXP_LENGTH  24

#define  SPFLOAT_MIN_EXP            (-(1LL<<(SPFLOAT_EXP_LENGTH-1))+1)
#define  SPFLOAT_MAX_EXP            ((1LL<<(SPFLOAT_EXP_LENGTH-1))-1)
#define  SPFLOAT_MIN_MANT           (-(1LL<<(SPFLOAT_MANT_LENGTH-1))+1)
#define  SPFLOAT_MAX_MANT           ((1LL<<(SPFLOAT_MANT_LENGTH-1))-1)
#define  SPFLOAT_MIN_UNS_MANT (1LL<<(SPFLOAT_MANT_LENGTH-2))//(1<<SPFLOAT_MANT_LENGTH)
#define  SPFLOAT_MAX_UNS_MANT ((1LL<<(SPFLOAT_MANT_LENGTH-1))-1)//((1<<(SPFLOAT_MANT_LENGTH+1))-1)

#define  SPFLOAT_MASK_MSP           0xFFFFFF000000
#define  SPFLOAT_MASK_LSP           0xFFFFFF
#define  SPFLOAT_MASK_EXP           0xFFFFFF
#define  MSPFLOAT_MASK_ROUND_MSP     (1LL<<(SPFLOAT_EXP_LENGTH-1)) //0x000000800000

#define spfloat_get_exponent(a)         (wextract_l((a)))
#define spfloat_get_mant(a)             (wextract_h((a)))
#define spfloat_get_mant_msp(dest, a)   (wX_depmsp(wextract_h(a)))
#define spfloat_make_acc(dest, a)       spfloat_get_mant_msp(dest, a)
#define spfloat_get_mant_left(a)        spfloat_get_mant((a))									 
#define spfloat_msp_mant_exp(dest,a,b)       ( winsert_h(winsert_l((dest), (b) ) , (a) ) )
#define spfloat_clip_exp(a)             
#define spfloat_round_mant(a)          


#else

/*
 ----------------------- 
 | mantisse | exposant |
 -----------------------
 |  16      |  8       |
 -----------------------
*/
typedef WORD24 spfloat;
typedef WORD16 spfmant;
typedef WORD16/*char*/ spfexp;
typedef WORD56 spfwork;


#define  SPFLOAT_BITS 24
#define  SPFLOAT_MANT_LENGTH 16
#define  SPFLOAT_EXP_LENGTH   8

#define  SPFLOAT_MIN_EXP            (-(1<<(SPFLOAT_EXP_LENGTH-1))+1)
#define  SPFLOAT_MAX_EXP            ((1<<(SPFLOAT_EXP_LENGTH-1))-1)
#define  SPFLOAT_MIN_MANT           (-(1<<(SPFLOAT_MANT_LENGTH-1))+1)
#define  SPFLOAT_MAX_MANT           ((1<<(SPFLOAT_MANT_LENGTH-1))-1)
#define  SPFLOAT_MIN_UNS_MANT (1<<(SPFLOAT_MANT_LENGTH-2))//(1<<SPFLOAT_MANT_LENGTH)
#define  SPFLOAT_MAX_UNS_MANT ((1<<(SPFLOAT_MANT_LENGTH-1))-1)//((1<<(SPFLOAT_MANT_LENGTH+1))-1)

#define  SPFLOAT_MASK_MSP           0xFFFF00
#define  SPFLOAT_MASK_LSP           0xFFFF
#define  SPFLOAT_MASK_EXP           0x00FF
#define  SPFLOAT_MASK_ROUND_MSP     ( 1 << (SPFLOAT_EXP_LENGTH-1))//0x80

#define spfloat_get_exponent(a)         (wextract((a),bit_field(0,SPFLOAT_EXP_LENGTH)))
#define spfloat_get_mant(a)             (wextract((a),bit_field(SPFLOAT_EXP_LENGTH,SPFLOAT_MANT_LENGTH)))
#ifndef __flexcc2__ 
#define spfloat_get_mant_left(a)        (((a)<<8)>>8)
#else
#define spfloat_get_mant_left(a)        (a)
#endif
#define spfloat_get_mant_msp(dest, a)         (wX_depmsp((a)))
#define spfloat_make_acc(dest, a)             (wX_depmsp((a)<<SPFLOAT_EXP_LENGTH))

#define spfloat_msp_mant_exp(dest, a,b)       (( (a)&SPFLOAT_MASK_MSP ) | ((b)&SPFLOAT_MASK_EXP))
#define spfloat_clip_exp(a)              ( (a) = wmin( wmax( (a), SPFLOAT_MIN_EXP), SPFLOAT_MAX_EXP) )
//#define spfloat_round_mant(a)            ( (a) = waddsat( (a) , SPFLOAT_MASK_ROUND_MSP ))
#define spfloat_round_mant(a)           

#if defined(__cplusplus) && !defined(DO_NOT_USE_PFLOAT_CLASS)
/* rely on constructor */
#define INIT_SPFLOAT(a , mant, exp)      pfloat((exp),(mant))
#else
#define INIT_SPFLOAT(a , mant, exp)      ( (((spfloat)(mant))<<SPFLOAT_EXP_LENGTH) | ((spfloat)(exp)& SPFLOAT_MASK_EXP) )
#endif

#define NormMantExp2spf(mant, exp)       ( (((spfloat)(mant))<<SPFLOAT_EXP_LENGTH) | ((spfloat)(exp)& SPFLOAT_MASK_EXP) )

#define SPF_MAX_VAL(a)                    ( a = ( (SPFLOAT_MAX_MANT<<SPFLOAT_EXP_LENGTH) | (SPFLOAT_MAX_EXP& SPFLOAT_MASK_EXP)) )



#endif


extern CPPCALL spfloat float2spf(float value);
extern CPPCALL float   spf2float(spfloat a);

extern CPPCALL spfloat spfloat_add(spfloat a, spfloat b);
extern CPPCALL spfloat spfloat_mul(spfloat a, spfloat b);
extern CPPCALL spfloat spfloat_imul(int a, spfloat b);
extern CPPCALL spfloat spfloat_mul3(spfloat a, spfloat b, spfloat c);
extern CPPCALL spfloat spfloat_square(spfloat a);
extern CPPCALL spfloat spfloat_sub(spfloat a, spfloat b);
extern CPPCALL spfloat spfloat_div(spfloat a, spfloat b);
extern CPPCALL spfloat spfloat_invert(spfloat a);
extern CPPCALL spfloat spfloat_div_2power(spfloat a, int b);

extern CPPCALL spfloat int2spf(int value);
extern CPPCALL int     spf2int(spfloat value);
extern CPPCALL int     spf2intNoRound(spfloat value);
extern CPPCALL int     spf_floor_int(spfloat a);
extern CPPCALL spfloat spf_floor(spfloat a);
extern CPPCALL int     spf_ceil_int(spfloat a);

extern CPPCALL spfloat fract2spf(int value);
extern CPPCALL spfloat dfract2spf(long long value);
extern CPPCALL int     spf2fract(spfloat a);
extern CPPCALL int     spf2fractNoRound(spfloat a);
extern CPPCALL WORD48  spf2dfract(spfloat a);

extern CPPCALL spfloat spfloat_abs(spfloat a);
extern CPPCALL spfloat spfloat_neg(spfloat a);
extern CPPCALL spfloat spfloat_max(spfloat a, spfloat b);
extern CPPCALL spfloat spfloat_min(spfloat a, spfloat b);
extern CPPCALL int     spfloat_eq(spfloat a, spfloat b);
extern CPPCALL int     spfloat_gt(spfloat a, spfloat b);
extern CPPCALL int     spfloat_ge(spfloat a, spfloat b);
extern CPPCALL int     spfloat_lt(spfloat a, spfloat b);
extern CPPCALL int     spfloat_gt_one(spfloat a);
extern CPPCALL int     spfloat_eq_zero(spfloat a);
extern CPPCALL int     spfloat_ge_zero(spfloat a);
extern CPPCALL int     spfloat_gt_zero(spfloat a);
extern CPPCALL int     spfloat_lt_zero(spfloat a);
extern CPPCALL int     spfloat_neq_zero(spfloat a);

extern CPPCALL spfloat spfloat_log2(spfloat a);
extern CPPCALL spfloat spfloat_sqrt(spfloat a);
extern CPPCALL spfloat spfloat_atan(spfloat a);
extern CPPCALL spfloat spfloat_power(spfloat a, spfloat b);
extern CPPCALL spfloat spfloat_2power(spfloat a);


extern CPPCALL int test_spf(void);


#endif /* _softpflost_h_ */ /* Do not edit below this line */







