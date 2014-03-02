/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: David Paris <david.paris at stericsson.com>
 *  for ST-Ericsson.
 *
 * Origin: New
 */

void memcpy(u8 *dest, u8* src, int lenght)
{
	unsigned n;
	const u8 *src = Opp_Default;
	u8 *dst = (u8 *)stOpp_data;

	for (n = 0; n < length; n++)
		dst[n] = src[n];
}
