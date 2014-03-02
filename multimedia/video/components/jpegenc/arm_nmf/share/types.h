/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef TYPES_H
#define TYPES_H

#include <limits.h>

#ifndef NULL
#  define NULL    0
#endif

typedef  unsigned char       t_uint8; /**< \brief unsigned 8 bits type   */
typedef    signed char       t_sint8; /**< \brief signed 8 bits type   */

typedef  unsigned short     t_uint16; /**< \brief unsigned 16 bits type   */
typedef    signed short     t_sint16; /**< \brief signed 16 bits type   */

typedef  unsigned long int  t_uint32; /**< \brief unsigned 32 bits type   */
typedef    signed long int  t_sint32; /**< \brief signed 32 bits type   */
typedef  t_sint16              T_BOOL; /**< \brief boolean type   */

typedef t_uint32              t_address; /**< \brief used to exchange addresses between hamac and host   */

#ifndef __NMF
typedef t_uint32 t_ulong_value;
typedef t_sint32 t_long_value;
typedef t_uint16 t_ushort_value;
typedef t_sint16 t_short_value;
typedef t_uint32 t_ahb_address;
typedef t_long_value t_time_stamp;
#endif

#define ON      1
#define OFF     0

#define TRUE    1
#define FALSE   0


#endif  /* TYPES_H */
