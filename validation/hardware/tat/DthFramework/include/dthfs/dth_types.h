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
 * @file   dth_types.h
 *
 * @brief Definition of base types.
 */
#include <stdint.h>
#ifndef DTH_TYPES_H_
#define DTH_TYPES_H_

typedef uint8_t     dth_u8;  /**< 9P unsigned one byte field.    */
typedef int8_t	    dth_s8;  /**< 9P signed one byte field.      */
typedef uint16_t    dth_u16; /**< 9P unsigned two bytes field.   */
typedef int16_t	    dth_s16; /**< 9P signed two bytes field.   */
typedef uint32_t    dth_u32; /**< 9P unsigned four bytes field.  */
typedef int32_t     dth_s32; /**< 9P signed four bytes field.  */
typedef uint64_t    dth_u64; /**< 9P unsigned eight bytes field. */
typedef int64_t     dth_s64; /**< 9P signed eight bytes field.   */
typedef float       dth_float; /**< 9P signed eight bytes field.   */


#endif /* DTH_TYPES_H_ */
