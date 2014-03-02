/*
 * Copyright (C) ST-Ericsson SA 2009
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _DB8500_COMMON_H_
#define _DB8500_COMMON_H_

#include <common.h>

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

/*-----------------------------------------------------------------------------
 * Bit mask definition
 *---------------------------------------------------------------------------*/

#define MASK_BIT0	(1UL<<0)
#define MASK_BIT1	(1UL<<1)
#define MASK_BIT2	(1UL<<2)
#define MASK_BIT3	(1UL<<3)
#define MASK_BIT4	(1UL<<4)
#define MASK_BIT5	(1UL<<5)
#define MASK_BIT6	(1UL<<6)
#define MASK_BIT7	(1UL<<7)
#define MASK_BIT8	(1UL<<8)
#define MASK_BIT9	(1UL<<9)
#define MASK_BIT10	(1UL<<10)
#define MASK_BIT11	(1UL<<11)
#define MASK_BIT12	(1UL<<12)
#define MASK_BIT13	(1UL<<13)
#define MASK_BIT14	(1UL<<14)
#define MASK_BIT15	(1UL<<15)
#define MASK_BIT16	(1UL<<16)
#define MASK_BIT17	(1UL<<17)
#define MASK_BIT18	(1UL<<18)
#define MASK_BIT19	(1UL<<19)
#define MASK_BIT20	(1UL<<20)
#define MASK_BIT21	(1UL<<21)
#define MASK_BIT22	(1UL<<22)
#define MASK_BIT23	(1UL<<23)
#define MASK_BIT24	(1UL<<24)
#define MASK_BIT25	(1UL<<25)
#define MASK_BIT26	(1UL<<26)
#define MASK_BIT27	(1UL<<27)
#define MASK_BIT28	(1UL<<28)
#define MASK_BIT29	(1UL<<29)
#define MASK_BIT30	(1UL<<30)
#define MASK_BIT31	(1UL<<31)

/* function prototypes */
int board_early_access(block_dev_desc_t *block_dev);

#endif  /* _DB8500_COMMON_H_ */
