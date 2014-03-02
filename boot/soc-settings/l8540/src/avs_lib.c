/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
| $RCSfile: /sources/avs_lib.c$
| $Revision: 1.1$
| $Date: Fri Sep 14 08:01:58 2012 GMT$
|
| $Source: /sources/avs_lib.c$
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
| Description: this file contains all functions to compute voltage for AVS
| ------------
|
|
|===========================================================================
|                    Revision History
|===========================================================================
|
|  $Log: /sources/avs_lib.c$
|  
|   Revision: 1.1 Fri Sep 14 08:01:58 2012 GMT nxp11764
|   Config pack Soc settings v3.0 - AVS V1 implementation
|
|  $Aliases: $
|
|
|  $KeysEnd $
|
|
|===========================================================================*/
/*! @file avs_lib.c
    @author laetitia Dijoux
    @version 1.1
    This file contains all functions to decode fuse and compute voltages.
*/

#include <log.h>
#include <float.h>


#include "avs_calc.h"
#include "avs_lib.h"

/*!
 ******************************************************************************
 * @fn      int AVS_Get_Two_Complement(int value,u16 nb_bit,u16 mask)
 * @author  LDI
 * @brief   Calculate the two complement binary of a number
 *
 * @param   int value =  value from register coded in 2's complement
 *          u16 nb_bit = Number of coding bit
 *          u16 mask = mask of bit for the value and the sign
 * @return  int Value ( signed )
 ******************************************************************************
**/
int AVS_Get_Two_Complement(int value,u16 nb_bit,u16 mask)
{
    if ( (value >> (nb_bit-1)) && 0x1)          // 2-comp negative number
    {
        value = (((~(value & mask)) & mask) + 1);
        value = -value;
        return(value);
    }
    else
        return(value);
}

/*!
 ******************************************************************************
 * @fn      short AVS_Voltage_Round_Up_Check(float value,float step)
 * @author  LDI
 * @brief   Calculate the round up of the voltage
 *
 * @param   float value = value computed
 *          float step = Step to use for round up
 * @return  short new_voltage = value rounded up and in 16 bits coded
 ******************************************************************************
**/
short AVS_Voltage_Round_Up_Check(float value,float step)
{
    u16 new_voltage;

    if ((int)(value/step) < (value/step))          //test if need round up
    {
        new_voltage = (short)(((short)(value/step) + 1)* step);
    }
    else
    {
        new_voltage = (short)(((short)(value/step))* step);
    }
    return(new_voltage);

}
/*!
 ******************************************************************************
 * @fn      Opp_status AVS_Max_Check(sPrcmuApi_InitOppData_t *stOpp_data,
 *          u8 config_name)
 * @author  LDI
 * @brief   Verify if the voltage computed is not higher than the MAX
 *          If it is the case, it means that the CHIP is not functional
 *
 * @param   sPrcmuApi_InitOppData_t *stOpp_data = full PRCMU structure to test
 *          u8 config_name = needed to know if OPP0 is @1.7GHz or 1.85GHz
 *          const tab_const_t *const_reg =  constant for computation
 * @return  Opp_status result =  if voltage > max OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL
 *                               else OPP_STATUS_NO_ERROR
 *
 ******************************************************************************
**/
Opp_status AVS_Max_Check(sPrcmuApi_InitOppData_t *stOpp_data,u8 config_name, const tab_const_t *const_reg)
{
    Opp_status result = OPP_STATUS_NO_ERROR;
    int i;
    float max_vsmps;

    /* Check Arm Voltage */
    for(i=ePrcmuApi_ArmOPP1;i<ePrcmuApi_ArmNbMaxOpp;i++)
    {
        switch(i)
        {
            case ePrcmuApi_ArmOPP6:
                if (config_name == 4)
                    max_vsmps = const_reg->vsmps_arm_opp0b_max;
                else
                    max_vsmps = const_reg->vsmps_arm_opp0_max;
                break;
            case ePrcmuApi_ArmOPP5: max_vsmps = const_reg->vsmps_arm_opp1_max; break;
            case ePrcmuApi_ArmOPP4: max_vsmps = const_reg->vsmps_arm_opp2_max; break;
            case ePrcmuApi_ArmOPP3: max_vsmps = const_reg->vsmps_arm_opp3_max; break;
            case ePrcmuApi_ArmOPP2: max_vsmps = const_reg->vsmps_arm_opp4_max; break;
            case ePrcmuApi_ArmOPP1: max_vsmps = const_reg->vsmps_arm_opp5_max; break;
        }
        if ((stOpp_data->armOppData[i].voltage > max_vsmps)
         && (stOpp_data->armOppData[i].oppAllowed != OPP_DISABLE_FLAG))
        {
            result = OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL;
            loginfo("Vsmps Arm > Vmax = > Chip not functional");
        }
    }

    /* Check APE Voltage */
    for(i=eIDX_APE_OPP1;i<ePrcmuApi_ApeNbMaxOpp;i++)
    {
        switch(i)
        {
            case eIDX_APE_OPP1: max_vsmps = const_reg->vsmps_ape_opp2_max; break;
            case eIDX_APE_OPP2: max_vsmps = const_reg->vsmps_ape_opp1_max; break;
        }
        if ((stOpp_data->apeOppData[i].voltage > max_vsmps)
         && (stOpp_data->apeOppData[i].oppAllowed != OPP_DISABLE_FLAG))
        {
            result = OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL;
            loginfo("Vsmps ape > Vmax = > Chip not functional");
        }
    }

    /* Check SAFE Voltage */
    for(i=ePrcmuApi_SafeOPP1;i<ePrcmuApi_SafeNbMaxOpp;i++)
    {
        switch(i)
        {
            case ePrcmuApi_SafeOPP1: max_vsmps = const_reg->vsmps_safe_opp2_max; break;
            case ePrcmuApi_SafeOPP2: max_vsmps = const_reg->vsmps_safe_opp1_max; break;
        }
        if ((stOpp_data->safeOppData[i].voltage > max_vsmps)
         && (stOpp_data->safeOppData[i].oppAllowed != OPP_DISABLE_FLAG))
        {
            result = OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL;
            loginfo("Vsmps safe > Vmax = > Chip not functional");
        }
    }

    /* Check OPP0 Vt < Vt plm rule */
    if ((stOpp_data->armOppData[ePrcmuApi_ArmOPP6].voltageTransistor < const_reg->arm_vt_plm_rule)
     && (stOpp_data->armOppData[ePrcmuApi_ArmOPP6].oppAllowed != OPP_DISABLE_FLAG))
    {
        result = OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL;
        loginfo("Vt OPP0 < Vt plm rule = > Chip not functional");
    }

    // Disable all OPPs
    if (result != OPP_STATUS_NO_ERROR )
    {
        // disable ARM OPP
        for(i=ePrcmuApi_ArmOPP0_RET;i<ePrcmuApi_ArmNbMaxOpp;i++)
        {
            stOpp_data->armOppData[i].oppAllowed = OPP_DISABLE_FLAG;
        }

        //Disable APE OPP
        for(i=eIDX_APE_OPP1;i<ePrcmuApi_ApeNbMaxOpp;i++)
        {
            stOpp_data->apeOppData[i].oppAllowed = OPP_DISABLE_FLAG;
        }
        //Disable SAFE OPP
        for(i=ePrcmuApi_SafeOPP0_RET;i<ePrcmuApi_SafeNbMaxOpp;i++)
        {
            stOpp_data->safeOppData[i].oppAllowed = OPP_DISABLE_FLAG;
        }

    }
    return(result);
}
/*!
 ******************************************************************************
 * @fn      short AVS_Memory_Rules_K2_Check(short vsmps_to_check,
 *          float step, float k_value)
 * @author  LDI
 * @brief   Memory rules check K2
 *
 * @param   short vsmps_to_check = Voltage domain to check with K2 rules against VSAFE
 *          float step = Domain fuse step
 *          float k_value =  constant value for each K2 rules
 *          short vsafe = Safe voltage to compare
 * @return  short = voltage value updated satisfying K2 rules
 *
 ******************************************************************************
**/
short AVS_Memory_Rules_K2_Check(short vsafe, short vsmps_to_check, float step, float k_value)
{
    short delta_N;
    float new_voltage;
    short voltage_updated;

    /* Vsafe OPP1 vs Vsmps_to_check => K2 rules failed need to do a correction*/
    if (((vsafe - vsmps_to_check) > k_value))
    {
        /* compute the delta to add to satify the rules */
        delta_N = (short)((short)((vsafe - vsmps_to_check - k_value)/step)+1);

        /* compute the new voltage with the delta and Add one step margin */
        new_voltage = vsmps_to_check + ((delta_N + 1)*step);

        /* Roundup the voltage */
        voltage_updated = (short)(AVS_Voltage_Round_Up_Check(new_voltage,step));
    }
    /* K2 rules passed => no correction to do */
    else
    {
        voltage_updated = vsmps_to_check;

    }
    return(voltage_updated);
}
/*!
 ******************************************************************************
 * @fn      void AVS_Compute_Vtransistor(
 *          sPrcmuApi_InitOppData_t *stOpp_data,stack_voltage_t *stack_voltage)
 * @author  LDI
 * @brief   Compute v transistor for all domains ARM/APE/SAFE
 *
 * @param   sPrcmuApi_InitOppData_t *stOpp_data = full PRCMU structure
 *          stack_voltage_t *stack_voltage = stack voltage value
 * @return  none
 *
 ******************************************************************************
**/
void AVS_Compute_Vtransistor(sPrcmuApi_InitOppData_t *stOpp_data,stack_voltage_t *stack_voltage)
{
    float arm_vt_opp0,arm_vt_opp1,arm_vt_opp2,arm_vt_opp3,arm_vt_opp4,arm_vt_opp5;
    float ape_vt_opp1,ape_vt_opp2;
    float safe_vt_opp1,safe_vt_opp2;

    /* ARM domain */
    arm_vt_opp5 = stOpp_data->armOppData[ePrcmuApi_ArmOPP1].voltage - stack_voltage->arm_stack_opp5;
    arm_vt_opp4 = stOpp_data->armOppData[ePrcmuApi_ArmOPP2].voltage - stack_voltage->arm_stack_opp4;
    arm_vt_opp3 = stOpp_data->armOppData[ePrcmuApi_ArmOPP3].voltage - stack_voltage->arm_stack_opp3;
    arm_vt_opp2 = stOpp_data->armOppData[ePrcmuApi_ArmOPP4].voltage - stack_voltage->arm_stack_opp2;
    arm_vt_opp1 = stOpp_data->armOppData[ePrcmuApi_ArmOPP5].voltage - stack_voltage->arm_stack_opp1;
    arm_vt_opp0 = stOpp_data->armOppData[ePrcmuApi_ArmOPP6].voltage - stack_voltage->arm_stack_opp0;
    /* APE domain */
    ape_vt_opp1 = stOpp_data->apeOppData[eIDX_APE_OPP2].voltage - stack_voltage->ape_stack_opp1;
    ape_vt_opp2 = stOpp_data->apeOppData[eIDX_APE_OPP1].voltage - stack_voltage->ape_stack_opp2;
    /* SAFE domain */
    safe_vt_opp1 = stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage - stack_voltage->safe_stack_opp1;
    safe_vt_opp2 = stOpp_data->safeOppData[ePrcmuApi_SafeOPP1].voltage - stack_voltage->safe_stack_opp2;

    /* Voltage transistor computation with roundup (+1) before truncature */
    /* ARM domain */
    stOpp_data->armOppData[ePrcmuApi_ArmOPP1].voltageTransistor = (short)(arm_vt_opp5+1);
    stOpp_data->armOppData[ePrcmuApi_ArmOPP2].voltageTransistor = (short)(arm_vt_opp4+1);
    stOpp_data->armOppData[ePrcmuApi_ArmOPP3].voltageTransistor = (short)(arm_vt_opp3+1);
    stOpp_data->armOppData[ePrcmuApi_ArmOPP4].voltageTransistor = (short)(arm_vt_opp2+1);
    stOpp_data->armOppData[ePrcmuApi_ArmOPP5].voltageTransistor = (short)(arm_vt_opp1+1);
    stOpp_data->armOppData[ePrcmuApi_ArmOPP6].voltageTransistor = (short)(arm_vt_opp0+1);

    /* APE domain */
    stOpp_data->apeOppData[eIDX_APE_OPP2].voltageTransistor = (short)(ape_vt_opp1+1);
    stOpp_data->apeOppData[eIDX_APE_OPP1].voltageTransistor = (short)(ape_vt_opp2+1);

    /* SAFE domain */
    stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltageTransistor = (short)(safe_vt_opp1+1);
    stOpp_data->safeOppData[ePrcmuApi_SafeOPP1].voltageTransistor = (short)(safe_vt_opp2+1);
}
/*!
 ******************************************************************************
 * @fn      Opp_status AVS_Memory_Rules_Check(
 *          sPrcmuApi_InitOppData_t *stOpp_data,stack_voltage_t *stack_voltage)
 * @author  LDI
 * @brief   Memory rules check between VASEF/APE/ARM domains
 *
 * @param   sPrcmuApi_InitOppData_t *stOpp_data = full PRCMU structure
 *          stack_voltage_t *stack_voltage = stack voltage value
 *          const tab_const_t *const_reg = constant for computation
 * @return  Opp_status result =  OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL
 *                               else OPP_STATUS_NO_ERROR
 *
 ******************************************************************************
**/
Opp_status AVS_Memory_Rules_Check(sPrcmuApi_InitOppData_t *stOpp_data,stack_voltage_t *stack_voltage, const tab_const_t *const_reg)
{
    float k11,k12,k13,k14,k15,k16_plm,k16,k16_final;
    float k21,k22,k23,k24,k25,k26_plm,k26,k26_final;
    float new_voltage;
    short delta_N;
    Opp_status result = OPP_STATUS_NO_ERROR;
    int i;

    /* init constant Kxx to check */
    k11 = -(const_reg->periph_array_rule -(const_reg->ape_transient_ov_opp1  +
            const_reg->smps_accuracy_pos + const_reg->safe_transient_un_opp1 +
            const_reg->smps_accuracy_neg + const_reg->safe_pcb_drop_opp1 +
            const_reg->safe_epod_drop_opp1 + const_reg->safe_aging_opp1 +
            const_reg->safe_gdrop_opp1 + const_reg->temp_offset));
    k12 = -(const_reg->periph_array_rule -(const_reg->ape_transient_ov_opp2  +
            const_reg->smps_accuracy_pos + const_reg->safe_transient_un_opp1 +
            const_reg->smps_accuracy_neg + const_reg->safe_pcb_drop_opp1 +
            const_reg->safe_epod_drop_opp1 + const_reg->safe_aging_opp1 +
            const_reg->safe_gdrop_opp1 + const_reg->temp_offset));
    k21 = const_reg->array_periph_rule -(const_reg->safe_transient_ov_opp1 +
          const_reg->smps_accuracy_pos + const_reg->ape_transient_un_opp1 +
          const_reg->smps_accuracy_neg + const_reg->ape_pcb_drop_opp1 +
          const_reg->ape_epod_drop_opp1 + const_reg->ape_gdrop_opp1 +
          const_reg->ape_aging_opp1 + const_reg->temp_offset);
    k22 = const_reg->array_periph_rule -(const_reg->safe_transient_ov_opp1 +
          const_reg->smps_accuracy_pos + const_reg->ape_transient_un_opp2 +
          const_reg->smps_accuracy_neg + const_reg->ape_pcb_drop_opp2 +
          const_reg->ape_epod_drop_opp2 + const_reg->ape_gdrop_opp2 +
          const_reg->ape_aging_opp2 + const_reg->temp_offset);
    k13 = -(const_reg->periph_array_rule -(const_reg->arm_transient_ov_opp5 +
            const_reg->smps_accuracy_pos + const_reg->safe_transient_un_opp1 +
            const_reg->smps_accuracy_neg + const_reg->safe_pcb_drop_opp1 +
            const_reg->safe_epod_drop_opp1 + const_reg->safe_gdrop_opp1 +
            const_reg->safe_aging_opp1 + const_reg->temp_offset));
    k14 = -(const_reg->periph_array_rule -(const_reg->arm_transient_ov_opp4 +
            const_reg->smps_accuracy_pos + const_reg->safe_transient_un_opp1 +
            const_reg->smps_accuracy_neg + const_reg->safe_pcb_drop_opp1 +
            const_reg->safe_epod_drop_opp1 + const_reg->safe_gdrop_opp1 +
            const_reg->safe_aging_opp1 + const_reg->temp_offset));
    k15 = -(const_reg->periph_array_rule -(const_reg->arm_transient_ov_opp3 +
            const_reg->smps_accuracy_pos + const_reg->safe_transient_un_opp1 +
            const_reg->smps_accuracy_neg + const_reg->safe_pcb_drop_opp1 +
            const_reg->safe_epod_drop_opp1 + const_reg->safe_gdrop_opp1 +
            const_reg->safe_aging_opp1 + const_reg->temp_offset));
    k23 = const_reg->array_periph_rule -(const_reg->safe_transient_ov_opp1 +
            const_reg->smps_accuracy_pos + const_reg->arm_transient_un_opp5 +
            const_reg->smps_accuracy_neg + const_reg->arm_pcb_drop_opp5 +
            const_reg->arm_epod_drop_opp5 + const_reg->arm_gdrop_opp5 +
            const_reg->arm_aging_opp5 + const_reg->temp_offset);
    k24 = const_reg->array_periph_rule -(const_reg->safe_transient_ov_opp1 +
            const_reg->smps_accuracy_pos + const_reg->arm_transient_un_opp4 +
            const_reg->smps_accuracy_neg + const_reg->arm_pcb_drop_opp4 +
            const_reg->arm_epod_drop_opp4 + const_reg->arm_gdrop_opp4 +
            const_reg->arm_aging_opp4 + const_reg->temp_offset);
    k25 = const_reg->array_periph_rule -(const_reg->safe_transient_ov_opp1 +
            const_reg->smps_accuracy_pos + const_reg->arm_transient_un_opp3 +
            const_reg->smps_accuracy_neg + const_reg->arm_pcb_drop_opp3 +
            const_reg->arm_epod_drop_opp3 + const_reg->arm_gdrop_opp3 +
            const_reg->arm_aging_opp3 + const_reg->temp_offset);

    /* setting constant for PLM rule */
    /* compute constant with plm */
    k16_plm = -(const_reg->plm_rule - (const_reg->smps_accuracy_pos + const_reg->smps_accuracy_neg +
                const_reg->safe_transient_un_opp1 + const_reg->safe_pcb_drop_opp1 +
                const_reg->safe_epod_drop_opp1 + const_reg->safe_gdrop_opp1 +
                const_reg->safe_aging_opp1 + const_reg->temp_offset));
    k26_plm = const_reg->plm_rule-(const_reg->safe_transient_ov_opp1 + const_reg->smps_accuracy_pos +
              const_reg->smps_accuracy_neg + (const_reg->arm_gdrop_opp2 / 2) +
              const_reg->arm_epod_drop_opp2 + const_reg->arm_aging_opp2 + const_reg->temp_offset);
    /* compute constant with array and periph  rule */
    k16 = -(const_reg->periph_array_rule -(const_reg->arm_transient_ov_opp2 +
            const_reg->smps_accuracy_pos + const_reg->safe_transient_un_opp1 +
            const_reg->smps_accuracy_neg + const_reg->safe_pcb_drop_opp1 +
            const_reg->safe_epod_drop_opp1 + const_reg->safe_gdrop_opp1 +
            const_reg->safe_aging_opp1 + const_reg->temp_offset));
    k26 = const_reg->array_periph_rule -(const_reg->safe_transient_ov_opp1 +
          const_reg->smps_accuracy_pos + const_reg->arm_transient_un_opp2 +
          const_reg->smps_accuracy_neg + const_reg->arm_pcb_drop_opp2 +
          const_reg->arm_epod_drop_opp2 + const_reg->arm_gdrop_opp2 +
          const_reg->arm_aging_opp2 + const_reg->temp_offset);

    /* select the most aggressive k16 */
    if (k16_plm < k16)
        k16_final = k16;
    else
        k16_final = k16_plm;

    /* select the most aggressive k26 */
    if (k26_plm < k26)
        k26_final = k26_plm;
    else
        k26_final = k26;


    /* K1 rules check */
    if (((stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage - stOpp_data->apeOppData[eIDX_APE_OPP2].voltage) < k11) ||
        ((stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage - stOpp_data->apeOppData[eIDX_APE_OPP1].voltage) < k12) ||
        ((stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage - stOpp_data->armOppData[ePrcmuApi_ArmOPP1].voltage) < k13) ||
        ((stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage - stOpp_data->armOppData[ePrcmuApi_ArmOPP2].voltage) < k14) ||
        ((stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage - stOpp_data->armOppData[ePrcmuApi_ArmOPP3].voltage) < k15) ||
        ((stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage - stOpp_data->armOppData[ePrcmuApi_ArmOPP4].voltage) < k16_final))
    {
        // disable ARM OPP
        for(i=ePrcmuApi_ArmOPP0_RET;i<ePrcmuApi_ArmNbMaxOpp;i++)
        {
            stOpp_data->armOppData[i].oppAllowed = OPP_DISABLE_FLAG;
        }

        //Disable APE OPP
        for(i=eIDX_APE_OPP1;i<ePrcmuApi_ApeNbMaxOpp;i++)
        {
            stOpp_data->apeOppData[i].oppAllowed = OPP_DISABLE_FLAG;
        }
        //Disable SAFE OPP
        for(i=ePrcmuApi_SafeOPP0_RET;i<ePrcmuApi_SafeNbMaxOpp;i++)
        {
            stOpp_data->safeOppData[i].oppAllowed = OPP_DISABLE_FLAG;
        }

        result = OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL;
        loginfo("K1 rules not satisfied > Chip not functional");
        return (result);
    }

    /* K2 Rules check */

    /* Vsafe OPP1 vs Vape OPP1 (100%) */
    stOpp_data->apeOppData[eIDX_APE_OPP2].voltage =
            AVS_Memory_Rules_K2_Check(stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage,
                                      stOpp_data->apeOppData[eIDX_APE_OPP2].voltage,
                                      const_reg->ape_step_opp1, k21);

    /* Vsafe OPP1 vs Vape OPP2 (50%) */
    stOpp_data->apeOppData[eIDX_APE_OPP1].voltage =
            AVS_Memory_Rules_K2_Check(stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage,
                                      stOpp_data->apeOppData[eIDX_APE_OPP1].voltage,
                                      const_reg->ape_step_opp2, k22);

    /*Vsafe OPP1 vs Varm OPP5 */
    stOpp_data->armOppData[ePrcmuApi_ArmOPP1].voltage =
            AVS_Memory_Rules_K2_Check(stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage,
                                      stOpp_data->armOppData[ePrcmuApi_ArmOPP1].voltage,
                                      const_reg->arm_step_opp5, k23);

    /*Vsafe OPP1 vs Varm OPP4 */
    stOpp_data->armOppData[ePrcmuApi_ArmOPP2].voltage =
            AVS_Memory_Rules_K2_Check(stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage,
                                      stOpp_data->armOppData[ePrcmuApi_ArmOPP2].voltage,
                                      const_reg->arm_step_opp4, k24);

    /*Vsafe OPP1 vs Varm OPP3 */
    stOpp_data->armOppData[ePrcmuApi_ArmOPP3].voltage =
            AVS_Memory_Rules_K2_Check(stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage,
                                      stOpp_data->armOppData[ePrcmuApi_ArmOPP3].voltage,
                                      const_reg->arm_step_opp3, k25);

    /*Vsafe OPP1 vs Varm OPP2 */
    stOpp_data->armOppData[ePrcmuApi_ArmOPP4].voltage =
            AVS_Memory_Rules_K2_Check(stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage,
                                      stOpp_data->armOppData[ePrcmuApi_ArmOPP4].voltage,
                                      const_reg->arm_step_opp2, k26_final);

    /* Specific rules for ARM OPP5 and ARM OPP1 */

    /* For ARM OPP5 vs ARM OPP4 signoff and stack voltage*/
    if ( (float) stOpp_data->armOppData[ePrcmuApi_ArmOPP1].voltage < (const_reg->arm_opp4_vt_signoff + stack_voltage->arm_stack_opp5))
    {
        loginfo(" Apply ARM OPP4 signoff on ARM OPP5 ");
        /* compute the delta to add to satify the rules */
        delta_N = (short)((short)((stOpp_data->armOppData[ePrcmuApi_ArmOPP1].voltage - const_reg->arm_opp4_vt_signoff -
                  stack_voltage->arm_stack_opp5)/const_reg->arm_step_opp1)+1);

        /* compute the new voltage with the delta and Add one step margin */
        new_voltage = stOpp_data->armOppData[ePrcmuApi_ArmOPP1].voltage + ((delta_N + 1)*const_reg->arm_step_opp1);

        /* Roundup the voltage */
        stOpp_data->armOppData[ePrcmuApi_ArmOPP1].voltage = (short)(AVS_Voltage_Round_Up_Check(new_voltage,const_reg->arm_step_opp1));
    }

    /* For ARM OPP1 vs Vt min */
    if (((float)stOpp_data->armOppData[ePrcmuApi_ArmOPP5].voltage < (const_reg->arm_vt_plm_rule + stack_voltage->arm_stack_opp1))
     && (stOpp_data->armOppData[ePrcmuApi_ArmOPP5].oppAllowed != OPP_DISABLE_FLAG))
    {
        loginfo("Clamp ARM OPP1 Vsmps");
        /* Clamp Vsmsp OPP1 */
        new_voltage = const_reg->arm_vt_plm_rule + stack_voltage->arm_stack_opp1;
        /* round up and check new voltage */
        stOpp_data->armOppData[ePrcmuApi_ArmOPP5].voltage = AVS_Voltage_Round_Up_Check(new_voltage,const_reg->arm_step_opp1);
    }
    return(result);
}

/*!
 ******************************************************************************
 * @fn      void AVS_Decoding_Fuses(opp_data_fuses_t *data_fuses, u32 avs_fuses[6])
 * @author  LDI
 * @brief   Read fuses and apply the 2 complement binary
 *
 * @param   opp_data_fuses_t *data_fuses = decoding fuse values
 *          u32 avs_fuses[6] = value of 6 fuse registers
 *          const tab_avs_fuse_t *fuse_reg = fuse reg definitions
 * @return  none
 ******************************************************************************
**/
void AVS_Decoding_Fuses(opp_data_fuses_t *data_fuses, u32 avs_fuses[6], const tab_avs_fuse_t *fuse_reg)
{

    /* FRV 6 read */
    data_fuses->opp0_enable = ( avs_fuses[FRV6_DATA] & fuse_reg->opp0_enable_field ) >> fuse_reg->opp0_enable_shift;
    data_fuses->opp0_mask   = ( avs_fuses[FRV6_DATA] & fuse_reg->opp0_mask_field ) >> fuse_reg->opp0_mask_shift;
    data_fuses->avs_rev     = ( avs_fuses[FRV6_DATA] & fuse_reg->avs_rev_field ) >> fuse_reg->avs_rev_shift;

    /* FRV 7 read */
    data_fuses->chip_config = ( avs_fuses[FRV7_DATA] & fuse_reg->chip_config_field ) >> fuse_reg->chip_config_shift;

    /* FRV 21 read */
    data_fuses->arm_opp0_offset = ( avs_fuses[FRV21_DATA] & fuse_reg->arm_opp0_offset_field ) >> fuse_reg->arm_opp0_offset_shift;
    data_fuses->arm_opp0_offset = AVS_Get_Two_Complement(data_fuses->arm_opp0_offset,fuse_reg->arm_opp0_offset_length,
                                  fuse_reg->arm_opp0_offset_2comp_mask);

    data_fuses->arm_opp1_offset = ( avs_fuses[FRV21_DATA] & fuse_reg->arm_opp1_offset_field ) >> fuse_reg->arm_opp1_offset_shift;
    data_fuses->arm_opp1_offset = AVS_Get_Two_Complement(data_fuses->arm_opp1_offset,fuse_reg->arm_opp1_offset_length,
                                  fuse_reg->arm_opp1_offset_2comp_mask);

    /* start with LSB */
    data_fuses->arm_opp2_offset = ( avs_fuses[FRV21_DATA] & fuse_reg->arm_opp2_offset_lsb_field ) >> fuse_reg->arm_opp2_offset_lsb_shift;

    /* FRV 22 read */
    /* finish decoding arm_opp2_offset in FRV 22 */
    data_fuses->arm_opp2_offset = ((avs_fuses[FRV22_DATA] & fuse_reg->arm_opp2_offset_msb_field) << fuse_reg->arm_opp2_offset_shift) | data_fuses->arm_opp2_offset ;
    data_fuses->arm_opp2_offset = AVS_Get_Two_Complement(data_fuses->arm_opp2_offset,fuse_reg->arm_opp2_offset_length,
                                  fuse_reg->arm_opp2_offset_2comp_mask);

    data_fuses->arm_opp3_offset = ( avs_fuses[FRV22_DATA] & fuse_reg->arm_opp3_offset_field ) >> fuse_reg->arm_opp3_offset_shift;
    data_fuses->arm_opp3_offset = AVS_Get_Two_Complement(data_fuses->arm_opp3_offset,fuse_reg->arm_opp3_offset_length,
                                  fuse_reg->arm_opp3_offset_2comp_mask);

    data_fuses->arm_opp4_offset = ( avs_fuses[FRV22_DATA] & fuse_reg->arm_opp4_offset_field ) >> fuse_reg->arm_opp4_offset_shift;
    data_fuses->arm_opp4_offset = AVS_Get_Two_Complement(data_fuses->arm_opp4_offset,fuse_reg->arm_opp4_offset_length,
                                  fuse_reg->arm_opp4_offset_2comp_mask);

    data_fuses->arm_opp5_offset = ( avs_fuses[FRV22_DATA] & fuse_reg->arm_opp5_offset_field ) >> fuse_reg->arm_opp5_offset_shift;
    data_fuses->arm_opp5_offset = AVS_Get_Two_Complement(data_fuses->arm_opp5_offset,fuse_reg->arm_opp5_offset_length,
                                  fuse_reg->arm_opp5_offset_2comp_mask);

    data_fuses->ape_opp1_offset = ( avs_fuses[FRV22_DATA] & fuse_reg->ape_opp1_offset_field ) >> fuse_reg->ape_opp1_offset_shift;
    data_fuses->ape_opp1_offset = AVS_Get_Two_Complement(data_fuses->ape_opp1_offset,fuse_reg->ape_opp1_offset_length,
                                  fuse_reg->ape_opp1_offset_2comp_mask);

    data_fuses->ape_opp2_offset = ( avs_fuses[FRV22_DATA] & fuse_reg->ape_opp2_offset_field ) >> fuse_reg->ape_opp2_offset_shift;
    data_fuses->ape_opp2_offset = AVS_Get_Two_Complement(data_fuses->ape_opp2_offset,fuse_reg->ape_opp2_offset_length,
                                  fuse_reg->ape_opp2_offset_2comp_mask);

    data_fuses->safe_opp1_offset = ( avs_fuses[FRV22_DATA] & fuse_reg->safe_opp1_offset_field ) >> fuse_reg->safe_opp1_offset_shift;
    data_fuses->safe_opp1_offset = AVS_Get_Two_Complement(data_fuses->safe_opp1_offset,fuse_reg->safe_opp1_offset_length,
                                   fuse_reg->safe_opp1_offset_2comp_mask);

    data_fuses->safe_opp2_offset = ( avs_fuses[FRV22_DATA] & fuse_reg->safe_opp2_offset_field ) >> fuse_reg->safe_opp2_offset_shift;

    data_fuses->safe_opp2_offset = AVS_Get_Two_Complement(data_fuses->safe_opp2_offset,fuse_reg->safe_opp2_offset_length,
                                   fuse_reg->safe_opp2_offset_2comp_mask);

}

/*!
 ******************************************************************************
 * @fn      void AVS_Compute_Voltage(stack_voltage_t *stack_voltage,
 *                              opp_data_fuses_t *data_fuses,u8 config_name,
 *                              sPrcmuApi_InitOppData_t *stOpp_data)
 * @author  LDI
 * @brief   Compute Voltage and stack
 *
 * @param   opp_data_fuses_t *data_fuses = decoding fuse values
 *          stack_voltage_t *stack_voltage = stack voltage values
 *          u8 config_name = config of the chip
 *          sPrcmuApi_InitOppData_t *stOpp_data = structure for the PRCMU
 *          const tab_const_t *const_reg = constant for computation
 * @return  none
 ******************************************************************************
**/
void AVS_Compute_Voltage(stack_voltage_t *stack_voltage, opp_data_fuses_t *data_fuses, u8 config_name,
                         sPrcmuApi_InitOppData_t *stOpp_data, const tab_const_t *const_reg)
{
    float arm_smps_opp0,arm_smps_opp1,arm_smps_opp2,arm_smps_opp3,arm_smps_opp4,arm_smps_opp5;
    float ape_smps_opp1, ape_smps_opp2;
    float safe_smps_opp1, safe_smps_opp2;
    float arm_vtxref_opp0=0;


    /* ARM stack */
    if ((config_name == 5) || (config_name == 6)) /* For 2Ghz , the chip is stuck at 1.85G */
    {
        /* OPP0 @ 1.85Ghz */
        stack_voltage->arm_stack_opp0 = const_reg->arm_gdrop_opp0 + const_reg->arm_transient_un_opp0 + const_reg->smps_accuracy_neg
                + const_reg->temp_offset + const_reg->arm_aging_opp0 + const_reg->arm_epod_drop_opp0 + const_reg->arm_pcb_drop_opp0;
        arm_vtxref_opp0 = const_reg->arm_vtxref_opp0;
    }
    else if (config_name == 4)
    {
        /* OPP0 @ 1.7Ghz */
        stack_voltage->arm_stack_opp0 = const_reg->arm_gdrop_opp0b + const_reg->arm_transient_un_opp0b + const_reg->smps_accuracy_neg
                + const_reg->temp_offset + const_reg->arm_aging_opp0b + const_reg->arm_epod_drop_opp0b + const_reg->arm_pcb_drop_opp0b;
        arm_vtxref_opp0 = const_reg->arm_vtxref_opp0b;
    }

    stack_voltage->arm_stack_opp1 = const_reg->arm_gdrop_opp1 + const_reg->arm_transient_un_opp1 + const_reg->smps_accuracy_neg
                           + const_reg->temp_offset + const_reg->arm_aging_opp1 + const_reg->arm_epod_drop_opp1
                           + const_reg->arm_pcb_drop_opp1;
    stack_voltage->arm_stack_opp2 = const_reg->arm_gdrop_opp2 + const_reg->arm_transient_un_opp2 + const_reg->smps_accuracy_neg
                           + const_reg->temp_offset + const_reg->arm_aging_opp2 + const_reg->arm_epod_drop_opp2
                           + const_reg->arm_pcb_drop_opp2;
    stack_voltage->arm_stack_opp3 = const_reg->arm_gdrop_opp3 + const_reg->arm_transient_un_opp3 + const_reg->smps_accuracy_neg
                           + const_reg->temp_offset + const_reg->arm_aging_opp3 + const_reg->arm_epod_drop_opp3
                           + const_reg->arm_pcb_drop_opp3;
    stack_voltage->arm_stack_opp4 = const_reg->arm_gdrop_opp4 + const_reg->arm_transient_un_opp4 + const_reg->smps_accuracy_neg
                           + const_reg->temp_offset + const_reg->arm_aging_opp4 + const_reg->arm_epod_drop_opp4
                           + const_reg->arm_pcb_drop_opp4;
    stack_voltage->arm_stack_opp5 = const_reg->arm_gdrop_opp5 + const_reg->arm_transient_un_opp5 + const_reg->smps_accuracy_neg
                           + const_reg->temp_offset + const_reg->arm_aging_opp5 + const_reg->arm_epod_drop_opp5
                           + const_reg->arm_pcb_drop_opp5;

    /*APE stack */
    stack_voltage->ape_stack_opp1 = const_reg->ape_gdrop_opp1 + const_reg->ape_transient_un_opp1 + const_reg->smps_accuracy_neg
                           + const_reg->temp_offset + const_reg->ape_aging_opp1 + const_reg->ape_epod_drop_opp1
                           + const_reg->ape_pcb_drop_opp1;
    stack_voltage->ape_stack_opp2 = const_reg->ape_gdrop_opp2 + const_reg->ape_transient_un_opp2 + const_reg->smps_accuracy_neg
                           + const_reg->temp_offset + const_reg->ape_aging_opp2 + const_reg->ape_epod_drop_opp2
                           + const_reg->ape_pcb_drop_opp2;
    /*SAFE stack */
    stack_voltage->safe_stack_opp1 = const_reg->safe_gdrop_opp1 + const_reg->safe_transient_un_opp1 + const_reg->smps_accuracy_neg
                           + const_reg->temp_offset + const_reg->safe_aging_opp1 + const_reg->safe_epod_drop_opp1
                           + const_reg->safe_pcb_drop_opp1;
    stack_voltage->safe_stack_opp2 = const_reg->safe_gdrop_opp2 + const_reg->safe_transient_un_opp2 + const_reg->smps_accuracy_neg
                           + const_reg->temp_offset + const_reg->safe_aging_opp2 + const_reg->safe_epod_drop_opp2
                           + const_reg->safe_pcb_drop_opp2;

    /* Calculate Voltage */
    /* ARM OPP5 */
    arm_smps_opp5 =  const_reg->arm_vtxref_opp5 + (data_fuses->arm_opp5_offset*const_reg->arm_step_opp5) + stack_voltage->arm_stack_opp5;
    stOpp_data->armOppData[ePrcmuApi_ArmOPP1].voltage = AVS_Voltage_Round_Up_Check(arm_smps_opp5,const_reg->arm_step_opp5);
    /* ARM OPP4 */
    arm_smps_opp4 =  const_reg->arm_vtxref_opp4 + (data_fuses->arm_opp4_offset*const_reg->arm_step_opp4) + stack_voltage->arm_stack_opp4;
    stOpp_data->armOppData[ePrcmuApi_ArmOPP2].voltage = AVS_Voltage_Round_Up_Check(arm_smps_opp4,const_reg->arm_step_opp4);
    /* ARM OPP3 */
    arm_smps_opp3 =  const_reg->arm_vtxref_opp3 + (data_fuses->arm_opp3_offset*const_reg->arm_step_opp3) + stack_voltage->arm_stack_opp3;
    stOpp_data->armOppData[ePrcmuApi_ArmOPP3].voltage = AVS_Voltage_Round_Up_Check(arm_smps_opp3,const_reg->arm_step_opp3);
    /* ARM OPP2 */
    arm_smps_opp2 = const_reg->arm_vtxref_opp2 + (data_fuses->arm_opp2_offset*const_reg->arm_step_opp2) + stack_voltage->arm_stack_opp2;
    stOpp_data->armOppData[ePrcmuApi_ArmOPP4].voltage = AVS_Voltage_Round_Up_Check(arm_smps_opp2,const_reg->arm_step_opp2);
    /* ARM OPP1 */
    arm_smps_opp1 =  const_reg->arm_vtxref_opp1 + (data_fuses->arm_opp1_offset*const_reg->arm_step_opp1) + stack_voltage->arm_stack_opp1;
    stOpp_data->armOppData[ePrcmuApi_ArmOPP5].voltage = AVS_Voltage_Round_Up_Check(arm_smps_opp1,const_reg->arm_step_opp1);
    /* ARM OPP0 */
    arm_smps_opp0 =  arm_vtxref_opp0 + (data_fuses->arm_opp0_offset*const_reg->arm_step_opp0) + stack_voltage->arm_stack_opp0;
    stOpp_data->armOppData[ePrcmuApi_ArmOPP6].voltage = AVS_Voltage_Round_Up_Check(arm_smps_opp0,const_reg->arm_step_opp0);

    /* APE OPP1 @500Mhz*/
    ape_smps_opp1 =  const_reg->ape_vtxref_opp1+ (data_fuses->ape_opp1_offset*const_reg->ape_step_opp1) + stack_voltage->ape_stack_opp1;
    stOpp_data->apeOppData[eIDX_APE_OPP2].voltage = AVS_Voltage_Round_Up_Check(ape_smps_opp1,const_reg->ape_step_opp1);

    /* APE OPP2 @250Mhz*/
    ape_smps_opp2 =  const_reg->ape_vtxref_opp2 + (data_fuses->ape_opp2_offset*const_reg->ape_step_opp2) + stack_voltage->ape_stack_opp2;
    stOpp_data->apeOppData[eIDX_APE_OPP1].voltage = AVS_Voltage_Round_Up_Check(ape_smps_opp2,const_reg->ape_step_opp2);

    /* SAFE OPP1 @533MHz*/
    safe_smps_opp1 =  const_reg->safe_vtxref_opp1 + (data_fuses->safe_opp1_offset*const_reg->safe_step_opp1) + stack_voltage->safe_stack_opp1;
    stOpp_data->safeOppData[ePrcmuApi_SafeOPP2].voltage = AVS_Voltage_Round_Up_Check(safe_smps_opp1,const_reg->safe_step_opp1);
    /* SAFE OPP2 @266MHz*/
    safe_smps_opp2 =  const_reg->safe_vtxref_opp2 + (data_fuses->safe_opp2_offset*const_reg->safe_step_opp2) + stack_voltage->safe_stack_opp2;
    stOpp_data->safeOppData[ePrcmuApi_SafeOPP1].voltage = AVS_Voltage_Round_Up_Check(safe_smps_opp2,const_reg->safe_step_opp2);

    /* Set voltage transistor in opp init structure*/
    AVS_Compute_Vtransistor(stOpp_data,stack_voltage);

}





