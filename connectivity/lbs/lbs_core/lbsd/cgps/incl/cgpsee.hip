/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSEE_HIP__
#define __CGPSEE_HIP__
/**
* \file cgpsee.hip
* \date 19/01/2012
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by cgps4utils.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 19.01.12 </TD><TD> Saswatha </TD><TD> Extended Ephemeris  </TD>
*     </TR>
* </TABLE>
*/

/**
* \fn const t_OperationDescriptor* CGPS11_03ExtendedEphRsp(t_RtkObject* p_FsmObject)
* \brief This function is to copy the Extended Ephemeris data to Global buffer
* \return State
*/
const t_OperationDescriptor* CGPS11_03ExtendedEphRsp(t_RtkObject* p_FsmObject);

/*XYBRID Integration :194997*/

const t_OperationDescriptor* CGPS11_04GetRefLocationRsp(t_RtkObject* p_FsmObject);

/*XYBRID Integration :194997*/


#endif /* __CGPSEE_HIP__ */
