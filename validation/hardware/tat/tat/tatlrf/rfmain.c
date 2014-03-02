/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   rfmain.c
* \brief   HATS RF main file
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "dthrf.h"
#include "rfdef.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "misc.h"
#include "pmmidmap.h"
#include "rfparams.h"


int dth_init_service()
{
	char vl_cmd[512];
    int vl_result = 0;
	int pfd;

	/* backup IQ sample data file */
    pfd = open(RX_IQ_SAMPLES_FILE, O_WRONLY | O_EXCL,
	    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if ( pfd != -1 )
    {
		close(pfd);

		/* make a backup of the IQ samples file */
		snprintf(vl_cmd, 512, "cp %s %s", RX_IQ_SAMPLES_FILE,
			 RX_IQ_SAMPLES_BACKUP);
		system(vl_cmd);

		/* clear IQ samples file */
		snprintf(vl_cmd, 512, "echo \"\" > %s", RX_IQ_SAMPLES_FILE);
		system(vl_cmd);
	}

	/* standard service DTH structure registration */
	vl_result = tatlrf_register_dth_struct();

	if ( TAT_OK(vl_result) )
    {
    	vl_result = tatl14_00GetAllRfParam();
        if ( TAT_OK(vl_result) )
        {
            if ( load_rf_param_file(PATH_PARAMETER_ELEMENT) < 0 )
            {
                SYSLOG(LOG_ERR, "fail to load RF parameters file");
                vl_result = TAT_ERROR_MISC;
            }
        }
        else
        {
            SYSLOG(LOG_ERR, "fail to retrieve RF parameters from modem");
        }
    }
    else
    {
        SYSLOG(LOG_ERR, "fail to register RF service");
	}

	/* ISI link must be closed or it may be unavailable to other processes even
       if this one ends! For exemple, modem_ape.exe may not be usable because 
       HATS was booted. */
	tat_modem_close();

	return vl_result;
}

/* service termination */
void dth_uninit_service()
{
	/* stop RX/TX 2G or 3G if active */
	tatl3_08ForceStopRfActivity();

	/* unregister regular DTH elements */
	tatlrf_unregister_dth_struct();

	/* free allocated resources */
    free_rf_params();
    tatl14_04ReleaseParamInfo();
}

