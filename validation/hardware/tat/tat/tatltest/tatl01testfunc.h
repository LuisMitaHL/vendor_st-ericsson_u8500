/* ============================================================================
*
*		Filename:  tatl01testfunc.h
*
*		Description:
*
*       Version:  1.0
*       Created:  May 2011
*       Revision:  none
*       Compiler:
*
*       Author:  VDY
*       Copyright ST-Ericsson, 2011. All Rights Reserved
*
* ============================================================================
*/

#ifndef TATLTESTFUNC_H_
#define TATLTESTFUNC_H_

#if defined(TATL01TESTFUNC_C)
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL u8 tatl_debug
#if defined(TATL01TESTFUNC_C)
    = 0
# endif
    ;

GLOBAL u32 asyncval
#if defined(TATL01TESTFUNC_C)
= 0
# endif
;

/* unsigned table index */
#define	UNB (URANGE + 1)

GLOBAL unsigned char TU8[UNB]
#if defined(TATL01TESTFUNC_C)
= { 0x7F, 0x7F, 0x7F }
# endif
;

GLOBAL unsigned short int TU16[UNB]
#if defined(TATL01TESTFUNC_C)
= { 0x7F00, 0x7F00, 0x7F00 }
# endif
;

GLOBAL unsigned int TU32[UNB]
#if defined(TATL01TESTFUNC_C)
= { 0x0L, 0x7F000000UL, 0x7F000000UL }
# endif
;

GLOBAL unsigned long long TU64[UNB]
#if defined(TATL01TESTFUNC_C)
= { 0x7F00000000000000ULL, 0x7F00000000000000ULL, 0x7F00000000000000ULL }
# endif
;

/* signed table index */
#define SNB (RANGE + 1)

GLOBAL char TS8[SNB]
#if defined(TATL01TESTFUNC_C)
= { 0x81, 0x1, 0x81, 0x8F }
# endif
;

GLOBAL short int TS16[SNB]
#if defined(TATL01TESTFUNC_C)
= { 0x8001, 0x1, 0x8001, 0x8F00 }
# endif
;

GLOBAL int TS32[SNB]
#if defined(TATL01TESTFUNC_C)
= { 0x80000001, 0x1, 0x80000001, 0x8F000000 }
# endif
;

GLOBAL long long TS64[SNB]
#if defined(TATL01TESTFUNC_C)
= { 0x0LL, 0x1LL, 0x8000000000000001LL, 0x0LL }
# endif
;

/* bitfield table index */
#define BNB (THREEGROUPS + 1)

GLOBAL u8 TU8B[BNB]
#if defined(TATL01TESTFUNC_C)
= { 0 }
# endif
;

GLOBAL u16 TU16B[BNB]
#if defined(TATL01TESTFUNC_C)
= { 0 }
# endif
;

GLOBAL u32 TU32B[BNB]
#if defined(TATL01TESTFUNC_C)
= { 0 }
# endif
;

GLOBAL u64 TU64B[BNB]
#if defined(TATL01TESTFUNC_C)
= { 0 }
# endif
;

GLOBAL float TF[SNB]
#if defined(TATL01TESTFUNC_C)
= { 0, 1, -1, 0 }
# endif
;

#define STRINGNB (STRINGFULL + 1)

GLOBAL char *TString[STRINGNB]
#if defined(TATL01TESTFUNC_C)
= { NULL }
#endif
;

GLOBAL int TStringSize[STRINGNB]
#if defined(TATL01TESTFUNC_C)
= { 1, 2, 17, 4097 }
#endif
;

GLOBAL u8 u8_1_1[1][1]
#if defined(TATL01TESTFUNC_C)
= { {0} }
#endif
;

GLOBAL u16 u16_1_1[1][1]
#if defined(TATL01TESTFUNC_C)
= { {0} }
#endif
;

GLOBAL u32 u32_1_1[1][1]
#if defined(TATL01TESTFUNC_C)
= { {0} }
#endif
;

GLOBAL u64 u64_1_1[1][1]
#if defined(TATL01TESTFUNC_C)
= { {0} }
#endif
;

GLOBAL u8 u8_2_2[2][2]
#if defined(TATL01TESTFUNC_C)
= { {0, 0}, {0, 0} }
#endif
;

GLOBAL u16 u16_2_2[2][2]
#if defined(TATL01TESTFUNC_C)
= { {0, 0}, {0, 0} }
#endif
;

GLOBAL u32 u32_2_2[2][2]
#if defined(TATL01TESTFUNC_C)
= { {0, 0}, {0, 0} }
#endif
;

GLOBAL u64 u64_2_2[2][2]
#if defined(TATL01TESTFUNC_C)
= { {0, 0}, {0, 0} }
#endif
;

GLOBAL int tatl_TestParam_Exec(struct dth_element *elem);
GLOBAL int tatl_TestParam_Set(struct dth_element *elem, void *Value);
GLOBAL int tatl_TestParam_Get(struct dth_element *elem, void *Value);
GLOBAL int tatl_ElementSize(int type);
GLOBAL int tatl_TestGetDthArray(struct dth_element *pl_elem,
				struct dth_array *pl_array, int table_name);
GLOBAL int tatl_TestSetDthArray(struct dth_element *pl_elem,
				struct dth_array *pl_array, int table_name);

#undef GLOBAL
#endif /* TATLTESTFUNC_H_ */
