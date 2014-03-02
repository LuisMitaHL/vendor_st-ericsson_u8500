/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

/**
 * @file   dth_utils.h
 *
 * @brief Useful macros for type conversions.
 */

#ifndef DTH_UTILS_H_
#define DTH_UTILS_H_

#ifndef NULL

/**
 * NIL pointer.
 */
#define NULL (void *) 0L

#endif /* NULL */

/**
 * Return the offset of a field within a composite type.
 */
#define dth_offset_of(t, f) (int) (&(((t *) 0)->f))

/**
 * Return the composite object that actually contains a field, which the
 * address is known.
 *
 * @param p specifies the address of the field.
 * @param t specifies the composite type.
 * @param f specifies the name of the field within the composite type.
 *
 * Suppose a C structure such as follows:
 *
 * @code
 * struct my_struct {
 *	int i;
 *	unsigned u;
 * };
 * @endcode
 *
 * It is possible to deduce the address of the container structure my_struct
 * having the address of its u field. The following test (hopefully) never
 * fails:
 *
 * @code
 * struct my_struct s;
 * if (&s != dth_container_of(&s.u, struct my_struct, u))
 *	puts ("dth_container_of is buggy\n");
 * @endcode
 */
#define dth_container_of(p, t, f) (t *) (((char *)p) - dth_offset_of(t, f))

/**
 * Return the number of elements of an array.
 * @param a specifies the array.
 */
#define dth_card_of(a) (sizeof(a) / sizeof(a[0]))

/**
 * @def dth_likely
 * @brief Inform the compiler the condition is expected to be true most of the
 * time.
 * @param x specifies the condition
 *
 * @def dth_unlikely
 * @brief Inform the compiler the condition is expected to be false most of the
 * time.
 * @param x specifies the condition
 */
#if defined(__GNUC__) && \
    (__GNUC__ >= 3 || __GNUC__ == 2 && __GNUC_MINOR__ >= 96)
#define dth_likely(x)	__builtin_expect(!!(x), 1)
#define dth_unlikely(x)	__builtin_expect(!!(x), 0)
#else
#define dth_likely(x) (x)
#define dth_unlikely(x) (x)
#endif

/**
 * Specify a variable or a function as unused so as to avoid compiler warnings.
 *
 * This macro must be used in a function context.
 *
 * @param x specifies the function or the variable.
 */
#define dth_unused(x) (void) (x)

/**
 * Prevent the software from compiling if specified conditions are not met.
 *
 * This macro is mainly used to verify that the underlying compiler matches
 * implementation choices. It must be used in a function context.
 *
 * @param predicate specifies the condition to analyze. The condition must
 * reduce to a constant during compilation.
 */
#define dth_static_assert(predicate)					      \
	do								      \
		switch (0) {						      \
		case 0:							      \
		case (predicate):					      \
			break	;					      \
		}							      \
	while (0)

#endif /* DTH_UTILS_H_ */
