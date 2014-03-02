/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _pcm_local_h_
#define _pcm_local_h_

/* get 16 signed bits, left aligned, byte swapped */
extern int
pcmfile_get_16_bits_swapped(void);

/* get 16 signed bits, left aligned */
extern int
pcmfile_get_16_bits(void);

/* get 8 bits, signed */
extern int
pcmfile_get_8_bits(void);

/* get 16 unsigned bits, right aligned */
extern unsigned int
pcmfile_get_16_unsigned_bits(void);

/* get 32 unsigned bits. right aligned */
extern unsigned long
pcmfile_get_32_unsigned_bits(void);

/* get 16 unsigned bits, right aligned */
extern unsigned int
pcmfile_get_16_unsigned_bits_swapped(void);

/* get 32 unsigned bits. right aligned */
extern unsigned long
pcmfile_get_32_unsigned_bits_swapped(void);

#endif /* Do not edit below this line */
