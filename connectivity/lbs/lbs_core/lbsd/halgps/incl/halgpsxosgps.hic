/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSXOSGPS_HIC__
#define __HALGPSXOSGPS_HIC__
/**
* \file halgpsxosgps.hic
* \date 26/05/2009
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all defines used by halgps2spi.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 26.05.09</TD><TD> Archana.B </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/


/** 
* \enum e_halgps_xosgps_ctrl  
*
*define the XOSGPS Ctrl commands
*
*/
typedef enum 
{
    HALGPS_XOSGPS_POWER_ON,
    HALGPS_XOSGPS_POWER_OFF,
    HALGPS_XOSGPS_RESET_ON,
    HALGPS_XOSGPS_RESET_OFF
} e_halgps_xosgps_ctrl;


/**
* \def HALGPS_XOSGPS_MAX_BUF_READ_LEN
* Maximum length of the Read buffer
*/
#define HALGPS_XOSGPS_MAX_BUF_READ_LEN  2048

/**
* \def HALGPS_XOSGPS_MAX_BUF_WRITE_LEN
* Maximum length of the Write buffer
*/
#define HALGPS_XOSGPS_MAX_BUF_WRITE_LEN 4096








#endif //__HALGPSXOSGPS_HIC__
