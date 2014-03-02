/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   DTH filesystem
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTH_UTILS_INTERNAL_H_
#define DTH_UTILS_INTERNAL_H_

#include <dthfs/dth_utils.h>

/**
 * Generate a compiler warning if two variables are of different types.
 *
 * This macro is useful for checking types of parameters from other macros.
 *
 * @param a the first variable
 * @param b the second variable
 */
#define dth_check_type(a, b) dth_unused((&(a)) == (&(b)))

/**
 * @internal
 */
#define _DTH_STRING(x) #x

/**
 * Turn an expression into a string during C pre-processing.
 */
#define DTH_STRING(x) _DTH_STRING(x)

/**
 *  Concatenate two expressions during C pre-processing.
 */
#define DTH_CONCAT(x, y) x ## y

extern void dth_panic(const char *, ...);

#ifndef NDEBUG
#define dth_assert(predicate)										\
	do																\
		if (dth_unlikely(!(predicate)))								\
			dth_panic("%s:%d: assertion `%s' failed in function "	\
					"`%s'.", __FILE__, __LINE__, #predicate,		\
					__func__);										\
	while (0)
#else /* NDEBUG */
#define dth_assert(predicate)
#endif /* NDEBUG */

#define dth_check(predicate)                                    \
	do                                                          \
		if (dth_unlikely(!(predicate)))                         \
			dth_panic("%s:%d: check `%s' failed in function "   \
					"`%s'.", __FILE__, __LINE__, #predicate,	\
					__func__);									\
	while (0)

#endif /* DTH_UTILS_INTERNAL_H_ */
