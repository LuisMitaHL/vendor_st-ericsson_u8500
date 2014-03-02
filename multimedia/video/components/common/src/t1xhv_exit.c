/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************
 * Created by Serge Backert
 */
/*****************************************************************************/

void t1xhv_exit(int a)
{
    #ifdef MMDSP
    int forever = 1;
    while (forever);
    #else
    exit(a);
    #endif
} /* t1xhv_exit() */

