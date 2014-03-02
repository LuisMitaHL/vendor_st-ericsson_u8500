/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _audioutils_h_
#define _audioutils_h_

#include "audiolibs_common.h"
#include "common_interface.h"

typedef struct {
	unsigned int   *curr_pointer;     /* start of ancillary data             */
	unsigned int    nb_valid_bits;    /* valid bits at address curr_pointer,
										 should be [24..0[                   */
	unsigned int    nb_ancillary_bits;/* length in bits of ancillary data    */
	unsigned int   *base_address;     /* first valid address                 */
	unsigned int   *max_address;      /* last valid address                  */

	int             delay;             /* delay in frames between audio and
										  ancillary data 					 */

	/* in case the delay is not zero, provide an alternate pointer to
	   extract ancillary with no delay */
	unsigned int   *alt_curr_pointer;
	unsigned int    alt_nb_valid_bits;
	unsigned int    alt_nb_ancillary_bits;
	/* curr_pointer should be in range [base_address,max_address] */
} AncillaryData;


extern int
audioutils_extract_ancillary_bits(AncillaryData *p_ancillary, int nbit);

#endif /* DO NOT EDIT BELOW THIS LINE */
