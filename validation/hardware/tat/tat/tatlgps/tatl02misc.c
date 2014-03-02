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
#include "tatlgps.h"

#ifdef _DEBUG

void tatl02_01DumpBinhex(void *block, size_t size)
{
	u32 i;
	u8 *ptr = (u8 *) block;
	fprintf(stdout, "block @%p (%u bytes)\n", block, size);
	for (i = 0; i < size; i++, ptr++) {
		if (i == 0)
			fprintf(stdout, "%8p - ", ptr);
		else if ((i > 0) && ((i % 16) == 0))
			fprintf(stdout, "\n%8p - ", ptr);
		else if ((i > 0) && ((i % 4) == 0))
			fprintf(stdout, " ");

		fprintf(stdout, "%02X", *ptr);
	}
	fprintf(stdout, "\n");
}

#endif /* _DEBUG */
