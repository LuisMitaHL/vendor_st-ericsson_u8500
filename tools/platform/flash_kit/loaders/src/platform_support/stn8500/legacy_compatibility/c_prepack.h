/* c_prepack.h
 *
 * Contents: Macros for structure packing (so no padding bytes
 *           are inserted). This is required on a >8-bit
 *           processor where it is more optimal to put fields
 *           inside a structure on aligned addresses. ARM for
 *           example tries to align things on 32 bits address
 *           boundaries (bits 1:0 = 00).
 *
 * Usage:
 *
 *   PRE_PACKED            - use between "typedef" and "struct".
 *   POST_PACKED           - use between the type name and the semi-colon.
 *   #include "c_prepack.h"   - turns packing on. Include before first usage
 *                           of PRE_PACKED
 *   #include "c_postpack.h" - turns packing off. Include after last usage
 *                           of POST_PACKED.
 *
 * Example:
 *
 *   #include "c_prepack.h"
 *
 *   typedef PRE_PACKED struct
 *   {
 *      char c
 *      int x,y;
 *   }
 *   MyType1_t POST_PACKED;
 *
 *   typedef PRE_PACKED struct
 *   {
 *      uint16 i;
 *      char c;
 *      uint32 d[3];
 *   }
 *   MyType2_t POST_PACKED;
 *
 *   #include "c_postpack.h"
 */

#ifndef _C_PREPACK_H_
#define _C_PREPACK_H_

//We don't need to include c_cmpiler.h as it is included in c_system_v2.h which always is supposed to
//be included first in the .c file
//#include "c_compiler.h" // Get info about the compiler we are using

#ifdef COMPILER_MSDEV

//#include <pshpack1.h>
//Copied contents of pshpack1.h here because of make dependencies problem for AVR
//990526 ECSJNIK
#if ! (defined(lint) || defined(_lint) || defined(RC_INVOKED))
#if ( _MSC_VER >= 800 ) || defined(_PUSHPOP_SUPPORTED)
#pragma warning(disable:4103)
#if !(defined( MIDL_PASS )) || defined( __midl )
#pragma pack(push)
#endif
#pragma pack(1)
#else
#pragma pack(1)
#endif
#endif // ! (defined(lint) || defined(_lint) || defined(RC_INVOKED))

#endif // #ifdef COMPILER_MSDEV

#if (defined(COMPILER_ARM_ARM) && !(defined(lint) || defined(_lint)))
#define PRE_PACKED __packed
#endif

#ifdef COMPILER_BORLAND
#pragma option -a1
#endif

#ifdef COMPILER_METAWARE
#pragma push_align_members(64)
#endif

#ifdef COMPILER_IAR_ARM
#pragma pack(push,1)
#endif

#if defined(COMPILER_GCC_ARM) || defined(COMPILER_GCC)
//#define PRE_PACKED __attribute__((__packed__))
//#define POST_PACKED
#pragma pack(push,1)
#endif

#ifndef PRE_PACKED
#define PRE_PACKED
#endif

#ifndef POST_PACKED
#define POST_PACKED
#endif

// allow inclusion of c_postpack.h again
#ifdef _C_POSTPACK_H_
#undef _C_POSTPACK_H_
#endif

#else //#ifndef _C_PREPACK_H_
#error Packing is active already; do not include c_prepack.h twice!
#endif


/* end of file c_prepack.h */
