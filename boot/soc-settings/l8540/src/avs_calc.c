/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
| $RCSfile: /sources/avs_calc.c$
| $Revision: 1.8$
| $Date: Fri Sep 14 08:01:58 2012 GMT$
|
| $Source: /sources/avs_calc.c$
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
|  $Log: /sources/avs_calc.c$
|  
|   Revision: 1.8 Fri Sep 14 08:01:58 2012 GMT nxp11764
|   Config pack Soc settings v3.0 - AVS V1 implementation
|
|   Revision: 1.7 Thu Apr 05 07:28:11 2012 GMT frq06447
|   Implementation of the ER 427896 : [SocSettings] : Error in Body Bias computation and ER 427889 : [SocSettings] : Error in decoding OPP2 voltage
|
|   Revision: 1.6 Tue Apr 03 12:04:57 2012 GMT frq06447
|   Correction in boby bias calculation and fuse decoding field
|
|   Revision: 1.5 Fri Mar 30 12:30:58 2012 GMT frq06447
|   Update the Avs functions to be aligned with AVS FW DOS 1.4b
|
|   Revision: 1.4 Tue Mar 13 15:09:28 2012 GMT frq06447
|   Update to support AVS feature
|
|   Revision: 1.3 Mon Nov 28 13:37:15 2011 GMT frq02595
|   Move BodyBias abs avs calculation function to static function
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
/*! @file avs_calc.c
    @author laetitia Dijoux
    @version 1.1
    This file contains all functions to decode fuse and compute voltages.
*/

#include <log.h>
#include <float.h>


#include "avs_calc.h"
#include "avs_lib.h"
#include "avs_const.h"
#include "avs_fuse.h"


/*! @param tab_const
 *  @brief pointer to avs constant regarding the AVS revision
 */
/* Pointer to AVS constant */
const tab_const_t *tab_const=NULL;

/*! @param tab_fuse
 *  @brief pointer to fuse constant regarding the AVS revision
 */
/* Pointer to AVS fuse */
const tab_avs_fuse_t *tab_fuse=NULL;

/*!
 ******************************************************************************
 * @fn      static void AVS_ConfigBits_Management(u32 avs_fuses_FRV6,
 *              u32 avs_fuses_FRV7,sPrcmuApi_InitOppData_t *stOpp_data)
 * @author  LDI
 * @brief   Management of Config Bits
 *
 * @param   u32 avs_fuses_FRV6 = value of the fuse register FRV6
 *          u32 avs_fuses_FRV7 = value of the fuse register FRV7
 *          sPrcmuApi_InitOppData_t *stOpp_data = structure for the PRCMU
 * @return  Opp_status result : result of the maximum check
 ******************************************************************************
**/
static Opp_status AVS_ConfigBits_Management(u32 avs_fuses_FRV6,u32 avs_fuses_FRV7,sPrcmuApi_InitOppData_t *stOpp_data)
{
    u8 opp0_enable,opp0_mask,chip_config;
    u8 opp0_enable_flag;
    Opp_status result = OPP_STATUS_NO_ERROR;
    u8 config_name;
    int i;

    /* FRV 6 read */
    opp0_enable = ( avs_fuses_FRV6 & tab_fuse->opp0_enable_field ) >> tab_fuse->opp0_enable_shift;
    opp0_mask   = ( avs_fuses_FRV6 & tab_fuse->opp0_mask_field ) >> tab_fuse->opp0_mask_shift;

    /* FRV 7 read */
    chip_config = ( avs_fuses_FRV7 & tab_fuse->chip_config_field ) >> tab_fuse->chip_config_shift;


    opp0_enable_flag = opp0_enable & (~opp0_mask);

    /* Decode Config Name */
    config_name = chip_config | (opp0_enable_flag << 2);

    /* Apply OPP Mapping table regarding FRV7 an OPP0_enable */
    switch(config_name)
    {
       case 0:
           /* OPP0 not relevant => set to 0 oppAllowed */
           stOpp_data->armOppData[ePrcmuApi_ArmOPP6].oppAllowed = OPP_DISABLE_FLAG;
           loginfo("OPP0 disable");
           break;
       case 1:
           /* OPP0 & 1 not relevant => set to 0 oppAllowed */
           stOpp_data->armOppData[ePrcmuApi_ArmOPP6].oppAllowed = OPP_DISABLE_FLAG;
           stOpp_data->armOppData[ePrcmuApi_ArmOPP5].oppAllowed = OPP_DISABLE_FLAG;
           loginfo("OPP0 & OPP1 disable");
           break;
       case 4:
           /* OPP 0 @ 1.7Ghz */
           stOpp_data->armOppData[ePrcmuApi_ArmOPP6].freq = OPP0_1G70_FREQ;
           stOpp_data->armOppData[ePrcmuApi_ArmOPP6].oppAllowed = OPP_ENABLE_FLAG;
           stOpp_data->armOppData[ePrcmuApi_ArmOPP6].voltage = tab_const->arm_opp0_no_avs_1_7G_smps;
           stOpp_data->armOppData[ePrcmuApi_ArmOPP6].voltageTransistor = tab_const->arm_opp0_no_avs_1_7G_vt;
           loginfo("OPP0 Enable @ 1.7G");
           break;
       case 5:
           /* OPP 0 @ 1.85Ghz */
           /* No freq set because already set @ 1.85G in No-AVS table */
           stOpp_data->armOppData[ePrcmuApi_ArmOPP6].oppAllowed = OPP_ENABLE_FLAG;
           loginfo("OPP0 Enable @ 1.85G");
           break;
       case 6: /* For 2 G, set the frequency @1.85Ghz */
           /* OPP 0 @ 1.85Ghz */
           stOpp_data->armOppData[ePrcmuApi_ArmOPP6].freq = OPP0_1G85_FREQ;
           stOpp_data->armOppData[ePrcmuApi_ArmOPP6].oppAllowed = OPP_ENABLE_FLAG;
           loginfo("OPP0 Enable @ 1.85G");
           break;
       case 2: case 3:  case 7:
           loginfo("Config chip not possible");
            /* OPP not supported by the config */
           for(i=ePrcmuApi_ArmOPP0_RET;i<ePrcmuApi_ArmNbMaxOpp;i++)
           {
               stOpp_data->armOppData[i].oppAllowed = OPP_DISABLE_FLAG;
           }
           result = OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL;
           break;
    }
    return (result);
}

/*!
 ******************************************************************************
 * @fn      static Opp_status AVS_Opp_Calculate(u32 avs_fuses[6], const u16 *Opp_Default
 *          , sPrcmuApi_InitOppData_t *stOpp_data)
 * @author	MLA/LDI
 * @brief	Calculate all Operating point (power and frequency) from the fuses
 *			values.
 *
 * @param   u32 avs_fuses[6] = Value of the six fuse registers
 *          const u16 *Opp_Default = OPP default table in No AVS
 *          sPrcmuApi_InitOppData_t *stOpp_data = structure for the PRCMU
 * @return  Opp_status result : result of the maximum check
 ******************************************************************************
**/
static Opp_status AVS_Opp_Calculate(u32 avs_fuses[6], const u16 *Opp_Default, sPrcmuApi_InitOppData_t *stOpp_data)
{
    /* structure of data read from fuses */
    opp_data_fuses_t opp_data_fuses;
    stack_voltage_t stack_voltage;
    u8 config_name, opp0_enable_flag;
    Opp_status result = OPP_STATUS_NO_ERROR;


    /* Copy the default table into the PRCMU table */
    /* In this function, we will change only the field customize by fuse */
    memcpy((u8*)stOpp_data, (u8*)Opp_Default, sizeof(sPrcmuApi_InitOppData_t));

    /* read fuses */
    AVS_Decoding_Fuses(&opp_data_fuses, avs_fuses,tab_fuse);
    loginfo("Decoding fuse");

    /* Chip config management */
    loginfo("Chip config management");
    result = AVS_ConfigBits_Management(avs_fuses[FRV6_DATA],avs_fuses[FRV7_DATA],stOpp_data);

    /* Decode Config Name */
    opp0_enable_flag = opp_data_fuses.opp0_enable & (~opp_data_fuses.opp0_mask);
    config_name = opp_data_fuses.chip_config | (opp0_enable_flag << 2);

    if (result != OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL)
    {
        /* Calculate stack voltage for OPP voltage */
        AVS_Compute_Voltage(&stack_voltage,&opp_data_fuses,config_name,stOpp_data,tab_const);
        loginfo("Computing voltage");

        /* check VSMPS computed < Max VSMPS  => else CHIP NOT functional */
        result = AVS_Max_Check(stOpp_data,config_name,tab_const);
        loginfo("Check voltage vs maximum");

        /* Memory rules check */
        /* else no memory rules check because chip not functional */
        if ((opp_data_fuses.avs_rev != 0) && (result != OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL))
        {
            loginfo( "Check Memory Rules");
            /* Memory rules check */
            result = AVS_Memory_Rules_Check(stOpp_data,&stack_voltage,tab_const);
            /* Compute Vtransistor */
            AVS_Compute_Vtransistor(stOpp_data,&stack_voltage);
            loginfo("Compute Vtransistor");
            if (result != OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL)
            {
                /* check VSMPS computed < Max VSMPS  => else CHIP NOT functional */
                result = AVS_Max_Check(stOpp_data,config_name,tab_const);
                loginfo("Check voltage vs maximum after memory rules correction");
            }
        }
    }
    return(result);
}


/*!
 ******************************************************************************
 * @fn      static void AVS_BodyBias_Calculate(u32 avs_fuses_bb,sPrcmuApi_InitOppData_t *stOpp_data)
 * @author	MLA/LDI
 * @brief	Calculate Body Bias voltage from the fuses values
 *
 * @param   u32 avs_fuses_bb = value of the field BB in fuse register FRV21
 *          sPrcmuApi_InitOppData_t *stOpp_data = structure for the PRCMU
 * @return  none
 ******************************************************************************
**/
static void AVS_BodyBias_Calculate(u32 avs_fuses_bb, sPrcmuApi_InitOppData_t *stOpp_data)
{
    signed short Vbbn, Vbbp;
    u16 Vbbn_reg, Vbbp_reg;
    u8 i;

    /* Read fuses in FRV 21 */
    Vbbn_reg = (avs_fuses_bb & tab_fuse->arm_vbbn_field) >> tab_fuse->arm_vbbn_shift;
    Vbbp_reg = (avs_fuses_bb & tab_fuse->arm_vbbp_field) >> tab_fuse->arm_vbbp_shift;

    /* Apply the translation table to have the voltage */
    Vbbn = 100 * AVS_Get_Two_Complement(Vbbn_reg,tab_fuse->arm_vbbn_length,tab_fuse->arm_vbbn_2comp_mask);
    Vbbp = 100 * AVS_Get_Two_Complement(Vbbp_reg,tab_fuse->arm_vbbp_length,tab_fuse->arm_vbbp_2comp_mask);

    /* Correct the value 4 => because not 2's complement */
    if ( Vbbn_reg == VBB_NOT_2_COMP )
        Vbbn = VBB_MAX;
    if ( Vbbp_reg == VBB_NOT_2_COMP )
        Vbbp = VBB_MAX;

    /* set the structure */
    /* RBB */
    if( Vbbn < 0 )
    {
        Vbbn = -(Vbbn);
        if ( Vbbn >  tab_const->vbbr_max )
        {
            Vbbn = tab_const->vbbr_max;
            loginfo("Clamp Vbbn");
        }
        for(i=1;i<7;i++)
        {
            stOpp_data->armOppData[i].Vbbn = -Vbbn;
        }

        /* Set the Vbbn at the current Vbbn decoding */
        stOpp_data->armOppData[ePrcmuApi_ArmOPP0_RET].Vbbn = -Vbbn;
    }
    else
    {
        /* FBB */
        if ( Vbbn >  tab_const->vbbf_max )
        {
            Vbbn = tab_const->vbbf_max;
            loginfo("Clamp Vbbn");
        }
        for(i=1;i<7;i++)
        {
            stOpp_data->armOppData[i].Vbbn = Vbbn;
        }
        /* Set the Vbbn at 0 (BBmux OFF) */
        stOpp_data->armOppData[ePrcmuApi_ArmOPP0_RET].Vbbn = 0;
    }
    /* RBB */
    if( Vbbp < 0 )
    {
        Vbbp = -(Vbbp);
        if ( Vbbp >  tab_const->vbbr_max )
        {
            Vbbp = tab_const->vbbr_max;
            loginfo("Clamp Vbbp");
        }
        for(i=1;i<7;i++) /* Retention mode at the same BB to confirm TBC */
        {
            stOpp_data->armOppData[i].Vbbp = stOpp_data->armOppData[i].voltage + Vbbp;
        }
    }
    else
    {
        /* FBB */
        if ( Vbbp >  tab_const->vbbf_max )
        {
            Vbbp = tab_const->vbbf_max;
            loginfo("Clamp Vbbp");
        }
        for(i=1;i<7;i++) /* Retention mode at the same BB to confirm TBC */
        {
            stOpp_data->armOppData[i].Vbbp = stOpp_data->armOppData[i].voltage - Vbbp;
        }
    }
    /* Set the Vbbp at Varm => no biasing */
    stOpp_data->armOppData[ePrcmuApi_ArmOPP0_RET].Vbbp = stOpp_data->armOppData[ePrcmuApi_ArmOPP0_RET].voltage;
}

/*!
 ******************************************************************************
 * @fn      static void AVS_Revision_Management(u32 avs_fuses_avs_rev)
 * @author  LDI
 * @brief   Management of AVS Revision for constant AVS
 *
 * @param   u32 avs_fuses_avs_rev = value of the avs revision in fuse register FRV6
 * @return  none
 ******************************************************************************
**/
static void AVS_Revision_Management(u32 avs_fuses_avs_rev)
{
    u8 avs_rev;

    /* Read fuses in FRV 6 */
    avs_rev = (avs_fuses_avs_rev & tab_fuse->avs_rev_field) >> tab_fuse->avs_rev_shift;
    tab_const = &tab_const_avs_rev[avs_rev];
    tab_fuse = &tab_fuse_avs_rev[avs_rev];
}
/*!
 ******************************************************************************
 * @fn      Opp_status OPP_GetOppConfig(u32 avs_fuses[6], const u16 *opp_default,
 *          sPrcmuApi_InitOppData_t *opp_data)
 * @author	MLA/LDI
 * @brief	Load the AVS values for voltage setting in the xP70 shared
 *			memory to prepare the AVS service call.
 *			If AVS values are not fused:
 *		        - Load default voltage config
 *			If AVS values are fused and AVS disabled:
 *			    If BB enabled
 *				    - Load default voltage config
 *				    - Load Body Bias values from fuses
 *				Else
 *				    - Load default voltage config
 *			If AVS values are fused and AVS enabled:
 *				- Load calculated voltage config from AVS algo
 *				- Load Body Bias values from fuses
 *
 * @param   u32 avs_fuses[6] = avs_fuses[0] = FRV4 register
 *                              avs_fuses[1] = FRV5 register
 *                              avs_fuses[2] = FRV6 register
 *                              avs_fuses[3] = FRV7 register
 *                              avs_fuses[4] = FRV21 register
 *                              avs_fuses[5] = FRV22 register
 *          const u16 *Opp_Default = Table by default No AVS
 *          sPrcmuApi_InitOppData_t *stOpp_data = table to send to PRCMU
 * @return  Opp_status result = Status of AVS computation
 ******************************************************************************
**/

Opp_status OPP_GetOppConfig(u32 avs_fuses[6], const u16 *Opp_Default, sPrcmuApi_InitOppData_t *stOpp_data)
{
    u32 avs_fuses_masked[4];
    Opp_status result = OPP_STATUS_NO_ERROR;
    /* the values pointed at the beginning are the same between AVS revisions */
    tab_fuse = &tab_fuse_avs_rev[0];

    /* mask the value not used in AVS */
    avs_fuses_masked[0] = avs_fuses[FRV6_DATA] & ( tab_fuse->avs_rev_field | tab_fuse->opp0_mask_field | tab_fuse->opp0_enable_field );
    avs_fuses_masked[1] = avs_fuses[FRV7_DATA] & tab_fuse->chip_config_field;
    avs_fuses_masked[2] = avs_fuses[FRV21_DATA]; /* all bits used in this register for AVS */
    avs_fuses_masked[3] = avs_fuses[FRV22_DATA]; /* all bits used in this register for AVS */

	if ((avs_fuses_masked[0] == 0) && (avs_fuses_masked[1] == 0)
	 && (avs_fuses_masked[2] == 0) && (avs_fuses_masked[3] == 0))
	{
	        // *************************************
	        // ***     AVS values not Fused      ***
	        // *************************************
	        // Load Default config:
	        //      - Default voltage values
	        //      - Body Bias disabled ( BBmux OFF )
	        memcpy((u8*)stOpp_data, (u8*)Opp_Default, sizeof(sPrcmuApi_InitOppData_t));
	        loginfo("AVS values not Fused : set with No AVS table");
	}
	else
	{
	    // AVS Revision management to choose the AVS constant
	    AVS_Revision_Management(avs_fuses[FRV6_DATA]);

		// *************************************
		// ***       AVS values Fused        ***
		// *************************************
		if (COMPILE_AVS)
		{
			// *** AVS Enabled *****************
			// Load voltage config:
			// 		- Calculated voltage values
			//		- Body Bias values from fuse



		    // Calculate and Load Voltage values from AVS algo
			result=AVS_Opp_Calculate(avs_fuses, Opp_Default, stOpp_data);

			// Calculate and Load BodyBias values from FRV21 register
			AVS_BodyBias_Calculate(avs_fuses[FRV21_DATA], stOpp_data);

			loginfo("AVS values Fused and calculate BodyBias from Fuses");
		}
		else
		{
		    // Load Default voltage values
		    memcpy((u8*)stOpp_data, (u8*)Opp_Default, sizeof(sPrcmuApi_InitOppData_t));

		    // Config Chip Management to update the No-AVS table
		    result = AVS_ConfigBits_Management(avs_fuses[FRV6_DATA],avs_fuses[FRV7_DATA],stOpp_data);

		    if (COMPILE_BB)
		    {
		        // *** No AVS **********************
		        // Load voltage config:
		        //		- Default voltage values
		        //		- Body Bias values from fuse

		        // Calculate and Load BodyBias values from FRV21 register
		        AVS_BodyBias_Calculate(avs_fuses[FRV21_DATA],stOpp_data);
		        loginfo("AVS flag not Set => set with No AVS table");
		        loginfo("BB flag set => calculate BodyBias from fuses");
		    }
		    else
		    {
		        // *** No AVS **********************
		        // Load voltage config:
		        //      - Default voltage values
		        //      - Body Bias disabled ( BBmux OFF )
		        loginfo("AVS and BB flags not Set => set with No AVS table");
		    }
		}
	}
	return (result);
}




