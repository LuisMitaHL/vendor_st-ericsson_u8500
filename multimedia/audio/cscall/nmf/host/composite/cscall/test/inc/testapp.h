/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   testapp.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _TESTAPP_H_
#define _TESTAPP_H_

#include <armnmf_dbc.h>

#define MAX_NMF_MESSAGE_SIZE 1024
#define NMF_CALL_BACK_STACK_SIZE (32*1024)
#define NMF_CM_CB_FIFO_SIZE 128

#ifdef _OS_THINK_

#include "los/api/los_api.h"

#define TESTAPP_FILE t_los_file

#define TESTAPP_FOPEN LOS_fopen
#define TESTAPP_FREAD LOS_fread
#define TESTAPP_FWRITE LOS_fwrite
#define TESTAPP_FSEEK  LOS_fseek
#define TESTAPP_FTELL  LOS_ftell
#define TESTAPP_EXIT(a)
#define TESTAPP_PRINTF LOS_Log
#define TESTAPP_FCLOSE LOS_fclose
#define TESTAPP_MALLOC(a) LOS_Alloc(a,4,LOS_MAPPING_DEFAULT)
#define TESTAPP_FREE(a) LOS_Free(t_los_memory_handle(a))
#define TESTAPP_SEEK_END LOS_SEEK_END
#define TESTAPP_SEEK_SET LOS_SEEK_SET


#define IMPORT_C 


#else

#include <stdio.h>
#include <stdlib.h>
#include <inc/type.h>

#define TESTAPP_FILE FILE

#define TESTAPP_FOPEN fopen
#define TESTAPP_FREAD fread
#define TESTAPP_FWRITE fwrite
#define TESTAPP_FCLOSE fclose
#define TESTAPP_FSEEK  fseek
#define TESTAPP_FTELL  ftell
#define TESTAPP_EXIT(a) exit(a)

#define TESTAPP_PRINTF NMF_LOG
#define TESTAPP_MALLOC(a) malloc(a)
#define TESTAPP_FREE(a) free((void *)a)

#define TESTAPP_SEEK_END SEEK_END
#define TESTAPP_SEEK_SET SEEK_SET

#ifndef IMPORT_C
#define IMPORT_C
#endif

#endif

#endif //_TESTAPP_H_
