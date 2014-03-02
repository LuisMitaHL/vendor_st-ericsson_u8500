/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*******************************************************************************
 * Created by Philippe Rochette on Tue Aug  6 15:16:20 2002
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "t1xhv_exit.h"

/*****************************************************************************/
void error(char *msg)
/*-----------------------------------------------------------------------------
* IN  : msg = message to print
* OUT : none
* Display error message and exit of the program
*-----------------------------------------------------------------------------
******************************************************************************/
{
#ifdef T1XHV_DEBUG
    printf("Error: %s\b", msg);
#endif
    t1xhv_exit(2);
}


/* END of t1xhv_error.c */
