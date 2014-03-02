/* c_postpack.h
 *
 * Contents: This routine reverses the packing enabled by c_prepack.h
 *           See that file for more details.
 */

#ifndef _C_POSTPACK_H_
#define _C_POSTPACK_H_

//We don't need to include c_cmpiler.h as it is included in c_system_v2.h which always is supposed to
//be included first in the .c file
//#include "c_compiler.h" // Get info about the compiler we are using

#ifndef _C_PREPACK_H_
#error Packing is not active, no need to include c_postpack.h
#endif

#ifdef COMPILER_MSDEV

//#include <poppack.h>
//Copied contents of poppack.h here because of make dependencies problem for AVR
//990526 ECSJNIK
#if ! (defined(lint) || defined(_lint) || defined(RC_INVOKED))
#if ( _MSC_VER >= 800 ) || defined(_PUSHPOP_SUPPORTED)
#pragma warning(disable:4103)
#if !(defined( MIDL_PASS )) || defined( __midl )
#pragma pack(pop)
#else
#pragma pack()
#endif
#else
#pragma pack()
#endif
#endif // ! (defined(lint) || defined(_lint) || defined(RC_INVOKED))

#endif


#ifdef COMPILER_BORLAND
#pragma option -a-
#endif

#ifdef COMPILER_METAWARE
#pragma pop_align_members()
#endif

#ifdef COMPILER_IAR_ARM
#pragma pack(pop)
#endif

#if defined(COMPILER_GCC_ARM) || defined(COMPILER_GCC)
//#undef PRE_PACKED
//#undef POST_PACKED
#pragma pack(pop)
#endif

#undef PRE_PACKED
#undef POST_PACKED

// allow to include c_prepack.h again
#undef _C_PREPACK_H_

#endif

/* end of file c_postpack.h */
