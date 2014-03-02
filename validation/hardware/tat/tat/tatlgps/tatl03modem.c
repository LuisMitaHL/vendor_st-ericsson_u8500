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

void tatl03_02DumpIsiHeader(t_isi_header *hdr)
{
	fprintf(stdout, "ISI header: mediaID = %u, rxDev = %u, txDev = %u, resourceID = %u, length = %u, rxObj = %u, txObj = %u.\n",
			hdr->mediaID, hdr->rxDev, hdr->txDev, hdr->resourceID,
			hdr->length, hdr->rxObj, hdr->txObj);
}

void tatl03_03DumpIsi(void *buf, size_t len)
{
	size_t i;
	u8 *ptr = (u8 *) buf;

	fprintf(stdout, "\nisi buffer @%p (%u bytes)\n", buf, len);
	for (i = 0; i < len; i++, ptr++) {
		if (i > 0)
			fprintf(stdout, ",%02X", *ptr);
		else
			fprintf(stdout, "%02X", *ptr);
	}

	fprintf(stdout, "\n\n");
}

#endif /* _DEBUG */
