/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSMAIN_HIP__
#define __HALGPSMAIN_HIP__
/**
* \file halgpsmain.hip
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by halgps0main.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 25.01.08 </TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/**
*\fn void HALGPS0_04UTCCorruptedCallback()
*\brief UTC corrupted callback
*
* Callback used by gsecurity module if the UTC is corrupted by another application or module.
*/
void HALGPS0_04UTCCorruptedCallback(void);


#endif /* __HALGPSMAIN_HIP__ */
