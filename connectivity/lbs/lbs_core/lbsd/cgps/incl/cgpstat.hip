/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSTAT_HIP__
#define __CGPSTAT_HIP__
/**
* \file cgpstat.hip
* \date 23/06/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by cgps6tat.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 23.06.08 </TD><TD> M.BELOU</TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/**
*\fn const t_OperationDescriptor* CGPS6_00TATTimerExpired(t_RtkObject* p_FsmObject)
*\brief function called when the loop Timer expired
*
* loop to update GPS library
* 
*\param p_FsmObject parameter use with fsm table, sent to CGPS0_00TATTimerExpired
*/
const t_OperationDescriptor* CGPS6_00TATTimerExpired(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS6_01TATActivate(t_RtkObject* p_FsmObject)
*\brief Activate or deactivate GPS module
*
* Activate or deactivate the GPS via TAT.
* 
*\param p_FsmObject parameter use with fsm table, sent to HALGPS0_12TATActivate
*/
const t_OperationDescriptor* CGPS6_01TATActivate(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS6_02TATGetInfo(t_RtkObject* p_FsmObject)
*\brief get some hardware information 
*
* Get sysol and GPS version
* 
*\param p_FsmObject parameter use with fsm table, sent to HALGPS0_13TATGetInfo
*/
const t_OperationDescriptor* CGPS6_02TATGetInfo(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS6_03TATGetLocation(t_RtkObject* p_FsmObject)
*\brief get navigation data
*
* Get fix information from library
* 
*\param p_FsmObject parameter use with fsm table, sent to HALGPS0_14TATGetLocation
*/
const t_OperationDescriptor* CGPS6_03TATGetLocation(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS6_04TATMode(t_RtkObject* p_FsmObject)
*\brief change GPS mode
*
* Switch between different GPS mode: Normal, Sleep, Coma, Deepcoma.
* 
*\param p_FsmObject parameter use with fsm table, sent to HALGPS0_15TATMode
*/
const t_OperationDescriptor* CGPS6_04TATMode(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS6_05TATTransparentMode(t_RtkObject* p_FsmObject)
*\brief switch to transparent mode
*
* Set the hardware layer to switch in trabsparent mode
* In transparent mode, the data from uart are directly sent to TAT.
* 
*\param p_FsmObject parameter use with fsm table, sent to HALGPS0_16TATTransparentMode
*/
const t_OperationDescriptor* CGPS6_05TATTransparentMode(t_RtkObject* p_FsmObject);


const t_OperationDescriptor* CGPS6_11TATTransparentDataReq(t_RtkObject* p_FsmObject);

/**
*\fn void CGPS6_06TATGoToNormalMode(e_cgps_tat_mode vp_state)
*\brief Go to normal mode
*
* From a state given vp_state the cgps go to normal mode
* 
*\param vp_state initial state
*/
e_cgps_tat_mode CGPS6_06TATGoToNormalMode(e_cgps_tat_mode vp_state);

/**
*\fn void CGPS6_07TATGoToSleepMode(e_cgps_tat_mode vp_state)
*\brief Go to normal mode
*
* From a state given vp_state the cgps go to normal mode
* 
*\param vp_state initial state
*/
e_cgps_tat_mode CGPS6_07TATGoToSleepMode(e_cgps_tat_mode vp_state);

/**
*\fn void CGPS6_08TATGoToComaMode(e_cgps_tat_mode vp_state)
*\brief Go to normal mode
*
* From a state given vp_state the cgps go to normal mode
* 
*\param vp_state initial state
*/
e_cgps_tat_mode CGPS6_08TATGoToComaMode(e_cgps_tat_mode vp_state);

/**
*\fn void CGPS6_09TATGoToDeepComaMode(e_cgps_tat_mode vp_state)
*\brief Go to normal mode
*
* From a state given vp_state the cgps go to normal mode
* 
*\param vp_state initial state
*/
e_cgps_tat_mode CGPS6_09TATGoToDeepComaMode(e_cgps_tat_mode vp_state);

/**
*\fn const t_OperationDescriptor* CGPS6_10SleepTimeExpired(t_RtkObject* p_FsmObject)
*\brief Relaunch sleep mode
*
* the max timeout of the sleep mode is 60s 
* to maintain the sleep mode we need to used a timer set with max value.
* When the timer expired the GPS is wke up and sleep.
* 
*\param vp_state initial state
*/
const t_OperationDescriptor* CGPS6_10SleepTimeExpired(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS6_11TATTransparentDataReq(t_RtkObject* p_FsmObject)
*\brief 
*
*/
const t_OperationDescriptor* CGPS6_11TATTransparentDataReq(t_RtkObject* p_FsmObject);

/**
*\fn const t_OperationDescriptor* CGPS6_13TATInitCnf(t_RtkObject* p_FsmObject)
*\brief 
*
*/
const t_OperationDescriptor* CGPS6_13TATInitCnf(t_RtkObject* p_FsmObject);

/* + LMSQC06481 : Production test */
#ifdef CGPS_CNO_VALUE_FTR
/**
*\fn const t_OperationDescriptor* CGPS6_14GetCnValueReq(t_RtkObject* p_FsmObject)
*\brief 
*
*/
const t_OperationDescriptor* CGPS6_14GetCnValueReq(t_RtkObject* p_FsmObject);
#endif /* CGPS_CNO_VALUE_FTR */
/* - LMSQC06481 : Production test */

#endif /* __CGPSTAT_HIP__ */
