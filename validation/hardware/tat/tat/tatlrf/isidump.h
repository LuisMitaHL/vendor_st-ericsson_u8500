/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   isidump.h
* \brief   routines to print ISI messages and subblocks in a human readable way
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef ISIDUMP_H_
#define ISIDUMP_H_

#include "isimsg.h"

#define tatrf_isi_dump(pp_st, pp_text, vp_prio) \
    (tatl17_10dump((pp_st), (pp_text), (vp_prio)))

void tatl17_10dump(struct tatrf_isi_msg_t *pp_st, const char *desc,
		   int vp_prio);

#endif /* ISIDUMP_H_ */
