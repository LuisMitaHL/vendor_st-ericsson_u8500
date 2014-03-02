

#ifndef _MATHEMATICS_H_
#define _MATHEMATICS_H_

#if defined(ARM) && defined(NEON)
#include "audiolibs_types.h"
#else /* ARM && NEON */
#define int16_t short
#define int32_t long
#endif /* ARM && NEON */

#ifdef STM_ARM_NEON_MODELS
#define __forceinline 
#endif


/*******************************************************************************
sqrt16 & sqrt16_fast routines :

input :     x (Q15 fractional positive value)
            => x floating point value = *p_x / 2^15

output :    square root of x
            Q15 fractional positive value

                                      sqrt16                sqrt16_fast
max relative error :                  0.000526592943654555  0.000660066006600660
average relative error :              0.000031135146443931  0.000024860203484090
max relative error for x >= 0.5 :     0.000134834490662712  0.000129472185058910
average relative error for x >= 0.5 : 0.000038613990200622  0.000029495869780700
(x >= 0.5 meaning x most left shifted, i.e. bit 14 = 1)
average cycles on CA9 NEON          : 100                   95
*******************************************************************************/
extern int16_t sqrt16     (int16_t x);
extern int16_t sqrt16_fast(int16_t x);


/*******************************************************************************
sqrt32 & sqrt32_fast routines :

input :     x (Q31 fractional positive value)
            => x floating point value = *p_x / 2^31

output :    square root of x
            Q31 fractional positive value

                                      sqrt32                sqrt32_fast
max relative error :                  0.000000053007317130  0.000001907467976871
average relative error :              0.000000000446789739  0.000000956155289238
max relative error for x >= 0.5 :     0.000000002373627778  0.000001906420194263
average relative error for x >= 0.5 : 0.000000000500330221  0.000000956838137010
(x >= 0.5 meaning x most left shifted, i.e. bit 30 = 1)
average cycles on CA9 NEON          : 141                   110
******************************************************************************/
extern int32_t sqrt32     (int32_t x);
extern int32_t sqrt32_fast(int32_t x);


/*******************************************************************************
sqrt16_float & sqrt16_float_fast routines :

input :     *p_x (Q15 fractional positive value)
            *p_right_shift (right shift of x)
            => x floating point value = *p_x / 2^(15 + *p_right_shift)

output :    *p_x (Q15 fractional positive value)
            *p_right_shift (right shift of x)
            => square root of x floating point value = *p_x / 2^(15 + *p_right_shift)

                             sqrt16_float          sqrt16_float_fast
max relative error :         0.000134834490662712  0.000129472185058910
average relative error :     0.000029181008637952  0.000023107708000574
average cycles on CA9 NEON : 105                   97
*******************************************************************************/
extern void sqrt16_float     (int16_t *p_x, int *p_right_shift);
extern void sqrt16_float_fast(int16_t *p_x, int *p_right_shift);


/*******************************************************************************
sqrt32_float & sqrt32_float_fast routines :

input :     *p_x (Q31 fractional positive value)
            *p_right_shift (right shift of x)
            => x floating point value = *p_x / 2^(31 + *p_right_shift)

output :    *p_x (Q31fractional positive value)
            *p_right_shift (right shift of x)
            => square root of x floating point value = *p_x / 2^(31+ *p_right_shift)

                             sqrt32_float          sqrt32_float_fast
max relative error :         0.000000002426828376  0.000001907800282063
average relative error :     0.000000000419400302  0.000000952699951180
average cycles on CA9 NEON : 138                   104
*******************************************************************************/
extern void sqrt32_float     (int32_t *p_x, int *p_right_shift);
extern void sqrt32_float_fast(int32_t *p_x, int *p_right_shift);


#if defined(ARM) && defined(NEON)
extern int16_t sqrt16_no_opt           (int16_t x);
extern int16_t sqrt16_fast_no_opt      (int16_t x);
extern int32_t sqrt32_no_opt           (int32_t x);
extern int32_t sqrt32_fast_no_opt      (int32_t x);
extern void    sqrt16_float_no_opt     (int16_t *p_x, int *p_right_shift);
extern void    sqrt16_float_fast_no_opt(int16_t *p_x, int *p_right_shift);
extern void    sqrt32_float_no_opt     (int32_t *p_x, int *p_right_shift);
extern void    sqrt32_float_fast_no_opt(int32_t *p_x, int *p_right_shift);
#endif /* ARM && NEON */

#endif /* _MATHEMATICS_H_ */
