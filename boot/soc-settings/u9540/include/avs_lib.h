/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
| $RCSfile: /sources/avs_lib.h$
| $Revision: 1.1$
| $Date: Fri Sep 14 08:01:58 2012 GMT$
|
| $Source: /sources/avs_lib.h$
|
| Copyright Statement:
| -------------------
| The confidential and proprietary information contained in this file may
| only be used by a person authorized under and to the extent permitted
| by a subsisting licensing agreement from ST-Ericsson S.A.
|
| Copyright (C) ST-Ericsson S.A. 2011. All rights reserved.
|
| This entire notice must be reproduced on all copies of this file
| and copies of this file may only be made by a person if such person is
| permitted to do so under the terms of a subsisting license agreement
| from ST-Ericsson S.A..
|
|
| Project :  AP9540
| -------
|
| Description:
| ------------
|
|
|===========================================================================
|                    Revision History
|===========================================================================
|
|  $Log: /sources/avs_lib.h$
|  
|   Revision: 1.1 Fri Sep 14 08:01:58 2012 GMT nxp11764
|   Config pack Soc settings v3.0 - AVS V1 implementation
|
|   Revision: 1.6 Thu Apr 05 07:26:43 2012 GMT frq06447
|   New define relative to ER 427896 : [SocSettings] : Error in Body Bias computation
|
|   Revision: 1.5 Fri Mar 30 12:29:08 2012 GMT frq06447
|   Update for AVS feature implementation
|
|   Revision: 1.4 Tue Mar 13 15:08:31 2012 GMT frq06447
|   Update OPP_GetOppConfig function to support 6 regifuse registers read
|
|   Revision: 1.3 Mon Nov 28 13:35:27 2011 GMT frq02595
|   Update include link of prcmuApi.h
|
|   Revision: 1.2 Wed Nov 09 14:15:00 2011 GMT frq02595
|   Wake-Up: First delivery for AB9540
|
|   Revision: 1.1 Wed Nov 09 10:54:13 2011 GMT frq02595
|
|  $Aliases: $
|
|
|  $KeysEnd $
|
|
|===========================================================================*/
/*! @file avs_calc.h
    @author laetitia Dijoux
    @version 1.1
    This header file contains all definitions for AVS functions.
*/
#ifndef __AVS_LIB_H__
#define __AVS_LIB_H__

#include <types.h>
#include <prcmuApi.h>


int AVS_Get_Two_Complement(int value,u16 nb_bit,u16 mask);
short AVS_Voltage_Round_Up_Check(float value,float step);
Opp_status AVS_Max_Check(sPrcmuApi_InitOppData_t *stOpp_data,u8 config_name, const tab_const_t *const_reg);
short AVS_Memory_Rules_K2_Check(short vsafe, short vsmps_to_check, float step, float k_value);
void AVS_Compute_Vtransistor(sPrcmuApi_InitOppData_t *stOpp_data,stack_voltage_t *stack_voltage);
Opp_status AVS_Memory_Rules_Check(sPrcmuApi_InitOppData_t *stOpp_data,stack_voltage_t *stack_voltage, const tab_const_t *const_reg);
void AVS_Decoding_Fuses(opp_data_fuses_t *data_fuses, u32 avs_fuses[6],const tab_avs_fuse_t *fuse_reg);
void AVS_Compute_Voltage(stack_voltage_t *stack_voltage, opp_data_fuses_t *data_fuses, u8 config_name,sPrcmuApi_InitOppData_t *stOpp_data, const tab_const_t *const_reg);


#endif /*__AVS_CALC_H__*/
