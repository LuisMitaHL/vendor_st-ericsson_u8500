/*****************************************************************************\
*  Copyright (C) 2012 - Ericsson AB - All Rights Reserved
*
*      Filename:    mpalib.h
*
*      Created:     1/19/12 by Ekdahl Patrik
*
*      Project:     Multi Precision Arithmetics Library.
*
*
*      Authors:     Ekdahl Patrik (Ericsson AB)
*
*      Description: Main header file for the mpalib
*
\*****************************************************************************/
#ifndef GUARD_MPALIB_H
#define GUARD_MPALIB_H

/*************************************************************
 *
 *   How functions are exported.
 *
 *************************************************************/
#define MPALIB_EXPORT




/*************************************************************
 *
 *   Include common configuration definitions
 *
 *************************************************************/
#include "mpalib_config.h"



/*************************************************************
 *
 *   TYPE DEFINITIONS
 *
 *************************************************************/



#if defined(MPA_SUPPORT_DWORD_T)
typedef unsigned long long mpa_dword_t;
#endif


/*! \struct mpa_numbase_struct
 * The internal representation of a multi precision integer.
 *
 *  \param alloc  The size of the allocated array d in number of mpa_word_t.
 *
 *  \param size   The number of used words in *d to represent the number, or minus the number
 *                if the mpa_numbase is representing a negative number. The mpa_numbase
 *                is representing zero if and only if size == 0.
 *
 *  \param d      The digits of the integer. The digits are in radix 2^WORD_SIZE.
 *                The digits are stored in a little endian format, i.e.
 *                the least significant word_t is stored in d[0].
 *
 * \internal ** NOTE **
 * If you change this struct, you must update the const variables
 * in mpa_misc.c and mpa_primetest.c
 * And the
 * MPA_NUMBASE_METADATA_SIZE_IN_U32
 * below.
 */
typedef struct mpa_numbase_struct {
    mpa_asize_t alloc;
    mpa_usize_t size;
    mpa_word_t d[];
} mpa_num_base;

/// mpanum is the type we use as parameters to function calls in this library
typedef mpa_num_base * mpanum;


/*!
 * The Context struct for a Montgomery multiplication
 *
 * \internal ** NOTE **
 * If you change this struct, you must update the
 * MPA_FMM_CONTEXT_METADATA_SIZE_IN_U32
 * below.
 *
 */
typedef struct mpa_fmm_context_struct {
    mpanum         r_ptr;
    mpanum         r2_ptr;
    mpa_word_t     n_inv;
    uint32_t       m[];
} mpa_fmm_context_base;

typedef mpa_fmm_context_base * mpa_fmm_context;



/*
 *
 * bit_size is the number of bits of the regular vars
 * that this pool was created for.
 */
typedef struct mpa_scratch_mem_struct {
    uint32_t nrof_vars;
    uint32_t alloc_size;
    uint32_t bit_size;
    uint32_t m[];
} mpa_scratch_mem_base;

typedef mpa_scratch_mem_base * mpa_scratch_mem;



/*************************************************************
 *
 *   EXPORTED VARIABLES
 *
 *************************************************************/



/*************************************************************
 *
 *   MACROS
 *
 *************************************************************/

#define MPA_STRING_MODE_HEX_UC  16
#define MPA_STRING_MODE_HEX_LC  17

#define MPA_EVEN_PARITY 0
#define MPA_ODD_PARITY  1

/*! Returns true if the mpanum 'a' is even (zero included) */
#define mpa_IsEven(a) ( mpa_Parity((a)) == MPA_EVEN_PARITY )
/*! Returns true if the mpanum 'a' is odd */
#define mpa_IsOdd(a)  ( mpa_Parity((a)) == MPA_ODD_PARITY )
/* Short hand for setting the value of 'a' to the value of 'b' */
#define mpa_Set(a,b) mpa_Copy((a),(b))


//
// Define how to convert between sizes given in uint32_t and mpa_word_t
//
#define ASIZE_TO_U32(x) (((sizeof(mpa_word_t) * (x)) + 3) / 4)
#define U32_TO_ASIZE(x) ((mpa_asize_t)(((4 * (x)) + (sizeof(mpa_word_t) - 1)) / sizeof(mpa_word_t)))


/*************************************************************
 *
 *   STATIC MEMORY MODE DEFINES
 *
 *************************************************************/


/*
 *   The number of extra uint32_t in the internal representation.
 *   This is used in the static memory mode.
 *   It is chosen to be complient with the requirments of GlobalPlatform.
 */
#define MPA_NUMBASE_METADATA_SIZE_IN_U32 2

#define MPA_SCRATCHMEM_METADATA_SIZE_IN_U32  (sizeof(mpa_scratch_mem_base)/ 4)


/*
 * The size (in uint32_t) of the constituent variables
 * of mpa_fmm_context apart from m[]
 */

#define MPA_FMM_CONTEXT_METADATA_SIZE_IN_U32 (sizeof(mpa_fmm_context_base)/4)


/*
 * This macro returns the size of the complete mpa_num_base struct that
 * can hold n-bits integers. This is used in the static memory mode.
 */
#define mpa_StaticVarSizeInU32(n)  \
((((n)+31)/32) + MPA_NUMBASE_METADATA_SIZE_IN_U32)


/*
 *
 */
#define mpa_StaticTempVarSizeInU32(max_bits) \
(2 * mpa_StaticVarSizeInU32((max_bits)) - MPA_NUMBASE_METADATA_SIZE_IN_U32)


/*
 *
 */
#define mpa_ScratchMemSizeInU32(nr_temp_vars, max_bits) \
((nr_temp_vars) * mpa_StaticTempVarSizeInU32(max_bits) + MPA_SCRATCHMEM_METADATA_SIZE_IN_U32)


/*
 *
 */
#define mpa_FMMContextSizeInU32(n) \
(2 * (mpa_StaticVarSizeInU32((n)) + 2) + MPA_FMM_CONTEXT_METADATA_SIZE_IN_U32)



/*************************************************************
 *
 *   FUNCTION PROTOTYPES
 *
 *  All externally available functions from this lib.
 *
 *************************************************************/



/*
 * From mpa_init.c
 */

/*!  mpa_LibInit
 *------------------------------------------------------------
 *  Initiate the mpa library. Only needed if we use debug printouts
 *  or if we use a software generated psuedo-random generator.
 *
 *
 */
MPALIB_EXPORT
void mpa_LibInit(void);


/*!  mpa_InitScratchMem
 *------------------------------------------------------------
 *  Initiate a chunk of memory to be used as a scratch pool.
 *  The size of the pool (in uint32_t) must corresponde to the
 *  size returned by the macro mpa_ScratchMemSizeInU32
 *  with the same parameters 'nr_vars' and 'max_bits'
 *
 *  \param pool         The pool to initialize
 *  \param nr_vars      The number of temp vars that it is sized to hold
 *  \param max_bits     The maximum number of bits that the library
 *                      can handle.
 *
 */
MPALIB_EXPORT
void mpa_InitScratchMem(mpa_scratch_mem pool,
                        int             nr_vars,
                        int             max_bits);




/*!  mpa_InitStatic
 *------------------------------------------------------------
 *  Initiate a mpanum to hold an integer of a certain size.
 *  The parameter 'len' is the return value of the macro
 *  mpa_StaticVarSizeInU32 called with the max bit size as parameter.
 *
 *  \param src  The mpanum to be initialized
 *  \param len  The allocated size in uint32_t of src
 *
 *
 */
MPALIB_EXPORT
void mpa_InitStatic(mpanum src,
                    uint32_t len);

MPALIB_EXPORT
void mpa_InitStaticFMMContext(mpa_fmm_context_base* context,
                              uint32_t                len);

/*
 * From mpa_addsub.c
 */

MPALIB_EXPORT
void mpa_Add(      mpanum dest,
             const mpanum op1,
             const mpanum op2,
             mpa_scratch_mem pool);


MPALIB_EXPORT
void mpa_Sub(      mpanum dest,
             const mpanum op1,
             const mpanum op2,
             mpa_scratch_mem pool);


MPALIB_EXPORT
void mpa_AddWord(       mpanum     dest,
                 const  mpanum     op1,
                        mpa_word_t op2,
                mpa_scratch_mem pool);

MPALIB_EXPORT
void mpa_SubWord(       mpanum     dest,
                 const  mpanum     op1,
                        mpa_word_t op2,
                mpa_scratch_mem pool);


MPALIB_EXPORT
void mpa_Neg(       mpanum dest,
             const  mpanum src);

/*
 * From mpa_mul.c
 */

MPALIB_EXPORT
void mpa_Mul(      mpanum dest,
             const mpanum op1,
             const mpanum op2,
             mpa_scratch_mem pool);

MPALIB_EXPORT
void mpa_MulWord(       mpanum     dest,
                 const  mpanum     op1,
                        mpa_word_t op2,
                 mpa_scratch_mem pool);


/*
 * From mpa_div.c
 */

MPALIB_EXPORT
void mpa_Div(       mpanum q,
                    mpanum r,
             const  mpanum op1,
             const  mpanum op2,
             mpa_scratch_mem pool);




/*
 * From mpa_modulus.c
 */

MPALIB_EXPORT
void mpa_Mod(      mpanum dest,
             const mpanum op,
             const mpanum n,
             mpa_scratch_mem pool);


MPALIB_EXPORT
void mpa_AddMod(       mpanum dest,
                const  mpanum op1,
                const  mpanum op2,
                const  mpanum n,
                mpa_scratch_mem pool);

MPALIB_EXPORT
void mpa_SubMod(       mpanum dest,
                const  mpanum op1,
                const  mpanum op2,
                const  mpanum n,
                mpa_scratch_mem pool);


MPALIB_EXPORT
void mpa_MulMod(       mpanum dest,
                const  mpanum op1,
                const  mpanum op2,
                const  mpanum n,
                mpa_scratch_mem pool);

MPALIB_EXPORT
int mpa_InvMod(       mpanum dest,
               const  mpanum op,
               const  mpanum n,
               mpa_scratch_mem pool);


/*
 * From mpa_cmp.c
 */

MPALIB_EXPORT
int mpa_Cmp(const mpanum op1,
            const mpanum op2);

MPALIB_EXPORT
int mpa_CmpShort(const  mpanum  op1,
                        int32_t op2);


/*
 * From mpa_conv.c
 */

MPALIB_EXPORT
void mpa_SetS32(mpanum    dest,
                int32_t   shortVal);

MPALIB_EXPORT
int32_t mpa_GetS32(int32_t* dest,
                   mpanum   src);

MPALIB_EXPORT
void mpa_SetWord(mpanum     dest,
                 mpa_word_t src);

MPALIB_EXPORT
mpa_word_t mpa_GetWord(mpanum src);



/*
 * From mpa_shift.c
 */

MPALIB_EXPORT
void mpa_ShiftLeft(         mpanum     dest,
                   const    mpanum     src,
                            mpa_word_t steps);


MPALIB_EXPORT
void mpa_ShiftRight(        mpanum     dest,
                    const   mpanum     src,
                            mpa_word_t steps);


/*
 * From mpa_gcd.c
 */
MPALIB_EXPORT
void mpa_GCD(      mpanum dest,
             const mpanum src1,
             const mpanum src2,
             mpa_scratch_mem pool);


MPALIB_EXPORT
void mpa_Extended_GCD(      mpanum gcd,
                            mpanum dest1,
                            mpanum dest2,
                      const mpanum src1,
                      const mpanum src2,
                      mpa_scratch_mem pool);


/*
 * From mpa_io.c
 */
MPALIB_EXPORT
int mpa_SetStr(      mpanum dest,
               const char*  digitstr);

MPALIB_EXPORT
char* mpa_GetStr(       char*       str,
                        int         mode,
                        int         groupsize,
                 const  mpanum      n);


MPALIB_EXPORT
int mpa_SetOctStr(mpanum dest, const uint8_t *buffer, size_t buffer_len,
                bool negative);

MPALIB_EXPORT
int mpa_GetOctStr(uint8_t *buffer, size_t *buffer_len, const  mpanum n);




/*
 * From mpa_expmod.c
 */
MPALIB_EXPORT
void mpa_ExpMod(        mpanum          dest,
                const   mpanum          op1,
                const   mpanum          op2,
                const   mpanum          n,
                const   mpanum          r_modn,
                const   mpanum          r2_modn,
                const   mpa_word_t      n_inv,
                mpa_scratch_mem         pool);






/*
 * From mpa_misc.c
 */


MPALIB_EXPORT
void mpa_Wipe(mpanum src);


MPALIB_EXPORT
void mpa_Copy(      mpanum dest,
              const mpanum src);

MPALIB_EXPORT
void mpa_Abs(        mpanum     dest,
             const   mpanum     src);

MPALIB_EXPORT
int mpa_HighestBitIndex(const mpanum src);

MPALIB_EXPORT
uint32_t mpa_GetBit(const   mpanum   src,
                    uint32_t idx);

MPALIB_EXPORT
int mpa_CanHold(       mpanum dest,
                 const  mpanum src);


MPALIB_EXPORT
int mpa_Parity(const mpanum src);

MPALIB_EXPORT
mpanum mpa_ConstantOne(void);



/*
 * From mpa_Random.c
 */

MPALIB_EXPORT
void mpa_GetRandom(mpanum dest, mpanum limit);


/*
 * From mpa_montgomery.c
 */


MPALIB_EXPORT
int mpa_ComputeFMMContext(const mpanum                 modulus,
                                mpanum                 r_modn,
                                mpanum                 r2_modn,
                                mpa_word_t*            n_inv,
                                mpa_scratch_mem        pool);


MPALIB_EXPORT
void mpa_MontgomeryMul(mpanum       dest,
                       mpanum       op1,
                       mpanum       op2,
                       mpanum       n,
                       mpa_word_t   n_inv,
                       mpa_scratch_mem pool);


/*
 * From mpa_mem_static.c
 */
MPALIB_EXPORT
mpanum mpa_AllocStaticTempVar(mpanum*           var,
                              mpa_scratch_mem   pool);

MPALIB_EXPORT
void mpa_FreeStaticTempVar(mpanum*          var,
                           mpa_scratch_mem  pool);




/*
 * From mpa_primetest.c
 */
MPALIB_EXPORT
int mpa_IsProbPrime(mpanum  n,
                    int     confLevel,
                    mpa_scratch_mem pool);


#endif /* include guard */
