/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
|  $RCSfile: /sources/avs_const.h$
|  $Revision: 1.3$
|  $Date: Fri Sep 14 08:01:58 2012 GMT$
|
|  $Source: /sources/avs_const.h$
|
|  Copyright Statement:
|  -------------------
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
| Definition of the constant needed for AVS calculation
|
|===========================================================================
|                    Revision History
|===========================================================================
|
|  $Aliases: $
|
|  $Log: /sources/avs_const.h$
|  
|   Revision: 1.3 Fri Sep 14 08:01:58 2012 GMT nxp11764
|   Config pack Soc settings v3.0 - AVS V1 implementation
|  
|   Revision: 1.2 Mon Mar 26 15:45:44 2012 GMT frq06447
|   Update constant aligned with AVS service FW DOS 1.4b
|  
|   Revision: 1.1 Tue Mar 13 15:06:46 2012 GMT frq06447
|   AVS constant file original
|
|
|  $KeysEnd $
|
|
|===========================================================================*/
/*! @file avs_const.h
    @author laetitia Dijoux
    @version 1.1
    This header file contains all AVS constants.
*/
#ifndef __avs_const_h
#define __avs_const_h

#ifdef __cplusplus
extern "C" {
#endif

#include "avs_calc.h"

/*! @param tab_const_avs_rev
    @brief This  parameter is a table of structure : one table for each AVS revision
 */
/* Parameters are in mV */
const tab_const_t tab_const_avs_rev[]={
/*-----------*/
/* AVS Rev 0 */
/*-----------*/
{
 400,     //VBBF_MAX
 100,     //VBBR_MAX
 1343.75, //VSMPS_ARM_OPP0_MAX
 1356.25, //VSMPS_ARM_OPP0B_MAX
 1362.5,  //VSMPS_ARM_OPP1_MAX
 1375,    //VSMPS_ARM_OPP2_MAX
 1393.75, //VSMPS_ARM_OPP3_MAX
 1393.75, //VSMPS_ARM_OPP4_MAX
 1393.75, //VSMPS_ARM_OPP5_MAX
 1250,    //VSMPS_APE_OPP1_MAX
 1275,    //VSMPS_APE_OPP2_MAX
 1275,    //VSMPS_SAFE_OPP1_MAX
 1275,    //VSMPS_SAFE_OPP2_MAX
 7.5,     //SMPS_ACCURACY_POS
 7.5,     //SMPS_ACCURACY_NEG
 10,      //TEMP_OFFSET
 350,     //ARRAY_PERIPH_RULE
 200,     //PERIPH_ARRAY_RULE
 200,     //PLM_RULE
 6.25,    //ARM_STEP_OPP0
 12.5,    //ARM_STEP_OPP1
 12.5,    //ARM_STEP_OPP2
 12.5,    //ARM_STEP_OPP3
 12.5,    //ARM_STEP_OPP4
 12.5,    //ARM_STEP_OPP5
 1100,    //ARM_VTX_REF_OPP0
 1075,    //ARM_VTX_REF_OPP0B
 1025,    //ARM_VTX_REF_OPP1
 925,     //ARM_VTX_REF_OPP2
 850,     //ARM_VTX_REF_OPP3
 675,     //ARM_VTX_REF_OPP4
 600,     //ARM_VTX_REF_OPP5
 10,      //ARM_EPOD_DROP_OPP0
 10,      //ARM_EPOD_DROP_OPP0B
 9,       //ARM_EPOD_DROP_OPP1
 7,       //ARM_EPOD_DROP_OPP2
 5,       //ARM_EPOD_DROP_OPP3
 3,       //ARM_EPOD_DROP_OPP4
 2,       //ARM_EPOD_DROP_OPP5
 0,       //ARM_PCB_DROP_OPP0
 0,       //ARM_PCB_DROP_OPP0B
 0,       //ARM_PCB_DROP_OPP1
 0,       //ARM_PCB_DROP_OPP2
 0,       //ARM_PCB_DROP_OPP3
 0,       //ARM_PCB_DROP_OPP4
 0,       //ARM_PCB_DROP_OPP5
 93.5,    //ARM_TRANSIENT_OV_OPP0
 86,      //ARM_TRANSIENT_OV_OPP0B
 76,      //ARM_TRANSIENT_OV_OPP1
 61,      //ARM_TRANSIENT_OV_OPP2
 41,      //ARM_TRANSIENT_OV_OPP3
 21,      //ARM_TRANSIENT_OV_OPP4
 8,       //ARM_TRANSIENT_OV_OPP5
 105,     //ARM_TRANSIENT_UN_OPP0
 97,      //ARM_TRANSIENT_UN_OPP0B
 86,      //ARM_TRANSIENT_UN_OPP1
 69,      //ARM_TRANSIENT_UN_OPP2
 46,      //ARM_TRANSIENT_UN_OPP3
 23,      //ARM_TRANSIENT_UN_OPP4
 14,      //ARM_TRANSIENT_UN_OPP5
 39,      //ARM_AGING_OPP0
 38,      //ARM_AGING_OPP0B
 37,      //ARM_AGING_OPP1
 35,      //ARM_AGING_OPP2
 32,      //ARM_AGING_OPP3
 29,      //ARM_AGING_OPP4
 27,      //ARM_AGING_OPP5
 40,      //ARM_GDROP_OPP0
 37,      //ARM_GDROP_OPP0B
 33,      //ARM_GDROP_OPP1
 26,      //ARM_GDROP_OPP2
 18,      //ARM_GDROP_OPP3
 9,       //ARM_GDROP_OPP4
 6,       //ARM_GDROP_OPP5
 650,     //ARM_VSMPS_RET
 12.5,    //APE_STEP_OPP1
 12.5,    //APE_STEP_OPP2
 950,     //APE_VTX_REF_OPP1
 825,     //APE_VTX_REF_OPP2
 10,      //APE_EPOD_DROP_OPP1
 10,      //APE_EPOD_DROP_OPP2
 15,      //APE_PCB_DROP_OPP1
 10,      //APE_PCB_DROP_OPP2
 35,      //APE_TRANSIENT_OV_OPP1
 17.5,    //APE_TRANSIENT_OV_OPP2
 45,      //APE_TRANSIENT_UN_OPP1
 25,      //APE_TRANSIENT_UN_OPP2
 15,      //APE_AGING_OPP1
 15,      //APE_AGING_OPP2
 40,      //APE_GDROP_OPP1
 26,      //APE_GDROP_OPP2
 12.5,    //SAFE_STEP_OPP1
 12.5,    //SAFE_STEP_OPP2
 975,     //SAFE_VTX_REF_OPP1
 950,     //SAFE_VTX_REF_OPP2
 10,      //SAFE_EPOD_DROP_OPP1
 7.5,     //SAFE_EPOD_DROP_OPP2
 10,      //SAFE_PCB_DROP_OPP1
 7.5,     //SAFE_PCB_DROP_OPP2
 17.5,    //SAFE_TRANSIENT_OV_OPP1
 10,      //SAFE_TRANSIENT_OV_OPP2
 25,      //SAFE_TRANSIENT_UN_OPP1
 12.5,    //SAFE_TRANSIENT_UN_OPP2
 15,      //SAFE_AGING_OPP1
 15,      //SAFE_AGING_OPP2
 20,      //SAFE_GDROP_OPP1
 10,      //SAFE_GDROP_OPP2
 690,     //ARM_OPP4_VT_SIGNOFF
 950,     //ARM_VT_PLM_RULE
 1350,    //ARM_NO_AVS_OPP0_1_7G_VSMPS
 1150     //ARM_NO_AVS_OPP0_1_7G_VT
}, \
/*-----------*/
/* AVS Rev 1 */
/*-----------*/
{
 300,     //VBBF_MAX
 100 ,    //VBBR_MAX
 1337.5,  //VSMPS_ARM_OPP0_MAX
 1325,    //VSMPS_ARM_OPP0B_MAX
 1250,    //VSMPS_ARM_OPP1_MAX
 1110,    //VSMPS_ARM_OPP2_MAX
 1025,    //VSMPS_ARM_OPP3_MAX
 1025,    //VSMPS_ARM_OPP4_MAX
 1025,    //VSMPS_ARM_OPP5_MAX
 1137.5,  //VSMPS_APE_OPP1_MAX
 950,     //VSMPS_APE_OPP2_MAX
 1087.5,  //VSMPS_SAFE_OPP1_MAX
 1025,    //VSMPS_SAFE_OPP2_MAX
 7.5,     //SMPS_ACCURACY_POS
 7.5,     //SMPS_ACCURACY_NEG
 10,      //TEMP_OFFSET
 300,     //ARRAY_PERIPH_RULE
 200,     //PERIPH_ARRAY_RULE
 200,     //PLM_RULE
 12.5,    //ARM_STEP_OPP0
 12.5,    //ARM_STEP_OPP1
 12.5,    //ARM_STEP_OPP2
 12.5,    //ARM_STEP_OPP3
 12.5,    //ARM_STEP_OPP4
 12.5,    //ARM_STEP_OPP5
 1075,    //ARM_VTX_REF_OPP0
 1075,    //ARM_VTX_REF_OPP0B
 1000,    //ARM_VTX_REF_OPP1
 875,     //ARM_VTX_REF_OPP2
 750,     //ARM_VTX_REF_OPP3
 625,     //ARM_VTX_REF_OPP4
 625,     //ARM_VTX_REF_OPP5
 10,      //ARM_EPOD_DROP_OPP0
 10,      //ARM_EPOD_DROP_OPP0B
 9,       //ARM_EPOD_DROP_OPP1
 7,       //ARM_EPOD_DROP_OPP2
 5,       //ARM_EPOD_DROP_OPP3
 3,       //ARM_EPOD_DROP_OPP4
 2,       //ARM_EPOD_DROP_OPP5
 0,       //ARM_PCB_DROP_OPP0
 0,       //ARM_PCB_DROP_OPP0B
 0,       //ARM_PCB_DROP_OPP1
 0,       //ARM_PCB_DROP_OPP2
 0,       //ARM_PCB_DROP_OPP3
 0,       //ARM_PCB_DROP_OPP4
 0,       //ARM_PCB_DROP_OPP5
 93.5,    //ARM_TRANSIENT_OV_OPP0
 86,      //ARM_TRANSIENT_OV_OPP0B
 76,      //ARM_TRANSIENT_OV_OPP1
 61,      //ARM_TRANSIENT_OV_OPP2
 41,      //ARM_TRANSIENT_OV_OPP3
 21,      //ARM_TRANSIENT_OV_OPP4
 14,       //ARM_TRANSIENT_OV_OPP5
 105,     //ARM_TRANSIENT_UN_OPP0
 97,      //ARM_TRANSIENT_UN_OPP0B
 86,      //ARM_TRANSIENT_UN_OPP1
 69,      //ARM_TRANSIENT_UN_OPP2
 46,      //ARM_TRANSIENT_UN_OPP3
 23,      //ARM_TRANSIENT_UN_OPP4
 16,      //ARM_TRANSIENT_UN_OPP5
 39,      //ARM_AGING_OPP0
 38,      //ARM_AGING_OPP0B
 37,      //ARM_AGING_OPP1
 35,      //ARM_AGING_OPP2
 32,      //ARM_AGING_OPP3
 29,      //ARM_AGING_OPP4
 27,      //ARM_AGING_OPP5
 40,      //ARM_GDROP_OPP0
 37,      //ARM_GDROP_OPP0B
 33,      //ARM_GDROP_OPP1
 26,      //ARM_GDROP_OPP2
 18,      //ARM_GDROP_OPP3
 9,       //ARM_GDROP_OPP4
 6,       //ARM_GDROP_OPP5
 650,     //ARM_VSMPS_RET
 12.5,    //APE_STEP_OPP1
 12.5,    //APE_STEP_OPP2
 950,     //APE_VTX_REF_OPP1
 825,     //APE_VTX_REF_OPP2
 10,      //APE_EPOD_DROP_OPP1
 10,      //APE_EPOD_DROP_OPP2
 15,      //APE_PCB_DROP_OPP1
 10,      //APE_PCB_DROP_OPP2
 35,      //APE_TRANSIENT_OV_OPP1
 17.5,    //APE_TRANSIENT_OV_OPP2
 45,      //APE_TRANSIENT_UN_OPP1
 25,      //APE_TRANSIENT_UN_OPP2
 15,      //APE_AGING_OPP1
 15,      //APE_AGING_OPP2
 40,      //APE_GDROP_OPP1
 26,      //APE_GDROP_OPP2
 12.5,    //SAFE_STEP_OPP1
 12.5,    //SAFE_STEP_OPP2
 950,     //SAFE_VTX_REF_OPP1
 950,     //SAFE_VTX_REF_OPP2
 10,      //SAFE_EPOD_DROP_OPP1
 7.5,     //SAFE_EPOD_DROP_OPP2
 10,      //SAFE_PCB_DROP_OPP1
 7.5,     //SAFE_PCB_DROP_OPP2
 17.5,    //SAFE_TRANSIENT_OV_OPP1
 10,      //SAFE_TRANSIENT_OV_OPP2
 25,      //SAFE_TRANSIENT_UN_OPP1
 12.5,    //SAFE_TRANSIENT_UN_OPP2
 15,      //SAFE_AGING_OPP1
 15,      //SAFE_AGING_OPP2
 20,      //SAFE_GDROP_OPP1
 10,      //SAFE_GDROP_OPP2
 690,     //ARM_OPP4_SIGNOFF
 950,     //ARM_VT_PLM_RULE
 1350,    //ARM_NO_AVS_OPP0_1_7G_VSMPS
 1150     //ARM_NO_AVS_OPP0_1_7G_VT
}
};

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif

// --============================ End of file ==============================--
