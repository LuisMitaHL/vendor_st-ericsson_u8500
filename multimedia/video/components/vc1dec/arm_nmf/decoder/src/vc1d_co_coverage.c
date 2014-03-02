/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/

#include "vc1d_common.h"

/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/

#ifdef COVERAGE

#define MAX_COV_ENTRIES 1024


/*------------------------------------------------------------------------
 * Variables                                                            
 *----------------------------------------------------------------------*/
char Cov_sequence[1024];
char Cov_frame[1024];
char Cov_bitstream[256];
char Cov_entries[MAX_COV_ENTRIES];
FILE *Cov_file=NULL;
int Frame_num;

/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/

void cov_log_standard_entry(char *entry)
{

    /* We try to not log the same entry many times */
    if (!strstr(Cov_entries,entry))
    {
        if (strlen(entry)  + strlen(Cov_entries) < MAX_COV_ENTRIES -1)
        {
            sprintf(Cov_entries,"%s|%s",Cov_entries,entry);
        }
        else printf("Warning: Maximum coverage entries exceeded\n");
    }
}

void cov_flush()
{
    ASSERT(Cov_file);


    fprintf(Cov_file,"%s%s%s%s\n",
            Cov_bitstream,Cov_sequence,Cov_frame,Cov_entries);

    /* Reset frame and entries for next calls */
    strcpy(Cov_entries,"");
    strcpy(Cov_frame,"");
}

#else /* COVERAGE not defined */

#endif /* COVERAGE */
