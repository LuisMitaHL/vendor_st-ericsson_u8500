/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSGSA_HIC__
#define __CGPSGSA_HIC__
/**
* \file cgpsgsa.hic
* \date 23/07/2009
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contains all defines used by cgps8gsa.c.\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 22.07.08</TD><TD> Anshuman Pandey </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#ifndef __RTK_E__


/**
* \enum e_cgps_cc_state
*
* define the current CC State in CGPS
*
*/
typedef enum
{
    K_CGPS_CC_DEACTIVATED,
    K_CGPS_CC_ACTIVATE_PENDING,
    K_CGPS_CC_ACTIVATED,
} e_cgps_cc_state;


#define K_CGPS_MAX_FREQ_ERROR 100   /*Maximum tolerated freq error delta*/
#define K_CGPS_GSA_FREQ       200


#define MC_CGPS_HZ_TO_MHZ(X) ((double)X/1000000.0)

#endif /* !__RTK_E__ */

#define K_CGPS_CALIB_CLK_PERIOD_CHECK_TIMEOUT 200
#define K_CGPS_CALIB_CLK_TIMEOUT 6000


#endif /* __CGPSGSA_HIC__ */

