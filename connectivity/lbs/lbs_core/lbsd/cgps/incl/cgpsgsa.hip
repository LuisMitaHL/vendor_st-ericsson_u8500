/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSGSA_HIP__
#define __CGPSGSA_HIP__
/**
* \file cgpsgsa.hip
* \date 23/07/2009
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by cgps7cp.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 21.07.08 </TD><TD> Anshuman Pandey </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/



/* ++ LMSQC06781 */
/* void  CGPS8_00GsaInit( void ); */
/* +- LMSQC06781 */

void  CGPS8_01ActivateGsa( void );

void  CGPS8_02DeActivateGsa( void );

void CGPS8_04CheckGsaTimerExp(uint32_t vl_CurrentTime);

#ifndef __RTK_E__
/**
* \fn const t_OperationDescriptor* CGPS8_05HandleClockCalibInd(t_RtkObject* p_FsmObject)
* \brief Handle CC status indicator
* \param p_FsmObject information sent with the message
*/
const t_OperationDescriptor* CGPS8_05HandleClockCalibInd(t_RtkObject* p_FsmObject);

/**
* \fn void CGPS8_06StopFreqAidingComputation(void)
* \brief Stops frequency aiding computation being done in GPS
* \param None
*/
void CGPS8_06StopFreqAidingComputation(void);


#else

void CGPS8_03CheckGsa( void );

#endif /* __RTK_E__ */

#endif /* __CGPSGSA_HIP__ */


