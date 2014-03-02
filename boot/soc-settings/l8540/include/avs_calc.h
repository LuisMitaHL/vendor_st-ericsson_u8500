/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
| $RCSfile: /sources/avs_calc.h$
| $Revision: 1.7$
| $Date: Fri Sep 14 08:01:58 2012 GMT$
|
| $Source: /sources/avs_calc.h$
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
|  $Log: /sources/avs_calc.h$
|  
|   Revision: 1.7 Fri Sep 14 08:01:58 2012 GMT nxp11764
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
#ifndef __AVS_CALC_H__
#define __AVS_CALC_H__

#include <types.h>
#include <prcmuApi.h>

/*-------------------------------------------------------------------------*/
/*                            Define                                       */
/*-------------------------------------------------------------------------*/

/*! @def Vbb definitions */

/* VBBP/N MAX */
#define VBB_MAX			350
#define VBB_NOT_2_COMP	4

/*! @def FRV register number translation position in avs_fuses table */
#define FRV6_DATA   2
#define FRV7_DATA   3
#define FRV21_DATA  4
#define FRV22_DATA  5

/*! @def OPP enable/disble flag */
/* OPP_enable_flag */
#define OPP_ENABLE_FLAG    1
#define OPP_DISABLE_FLAG   0

/*! @def OPP0 frequency */
/* OPP0 frequency */
#define OPP0_1G85_FREQ      0x1C3A90
#define OPP0_1G70_FREQ      0x19F0A0

/*! @def AVS revision */
/* OPP0 frequency */
#define AVS_REV_0      0
#define AVS_REV_1      1

/*! @enum Opp_status
 */
/* Opp error to kernel */
typedef enum {
    OPP_STATUS_NO_ERROR,                //!< OPP_STATUS_NO_ERROR
    OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL//!< OPP_STATUS_ERROR_CHIP_NOT_FUNCTIONAL
} Opp_status;

/*! @enum APE index
 */
/* APE index opp init table */
enum {
    eIDX_APE_OPP1=ePrcmuApi_ApeOPP1-ePrcmuApi_ApeOPP1,                //!< Index OPP1 ( 50 %) in opp init table
    eIDX_APE_OPP2=ePrcmuApi_ApeOPP2-ePrcmuApi_ApeOPP1                 //!< Index OPP2 ( 100 %) in opp init table
};
/*-------------------------------------------------------------------------*/
/*                        structure definition                             */
/*-------------------------------------------------------------------------*/
/*! @struct opp_data_fuses_t
 *  @brief Internal structure for AVS voltage computation
 */
/* structure of data fuse decoded */
typedef struct{
    u8 opp0_enable; /**< OPP0 enable */
    u8 opp0_mask;   /**< OPP0 mask */
    u8 chip_config; /**< Hw chip configuration */
    u8 avs_rev;     /**< AVs revision */
    int arm_opp0_offset;    /**< ARM OPP0 offset */
    int arm_opp1_offset;    /**< ARM OPP1 offset */
    int arm_opp2_offset;    /**< ARM OPP2 offset */
    int arm_opp3_offset;    /**< ARM OPP3 offset */
    int arm_opp4_offset;    /**< ARM OPP4 offset */
    int arm_opp5_offset;    /**< ARM OPP5 offset */
    int ape_opp1_offset;    /**< APE OPP1 offset */
    int ape_opp2_offset;    /**< APE OPP2 offset */
    int safe_opp1_offset;   /**< SAFE OPP1 offset */
    int safe_opp2_offset;   /**< SAFE OPP2 offset */
}opp_data_fuses_t;

typedef struct{
    float arm_stack_opp0;   /**<ARM stack voltage OPP0 */
    float arm_stack_opp1;   /**<ARM stack voltage OPP1 */
    float arm_stack_opp2;   /**<ARM stack voltage OPP2 */
    float arm_stack_opp3;   /**<ARM stack voltage OPP3 */
    float arm_stack_opp4;   /**<ARM stack voltage OPP4 */
    float arm_stack_opp5;   /**<ARM stack voltage OPP5 */
    float ape_stack_opp1;   /**<APE stack voltage OPP1 */
    float ape_stack_opp2;   /**<APE stack voltage OPP2 */
    float safe_stack_opp1;  /**<SAFE stack voltage OPP1 */
    float safe_stack_opp2;  /**<SAFE stack voltage OPP2 */
}stack_voltage_t;

/*! @struct tab_const_t
    @brief structure of AVS constants
 */
typedef struct{
    /* General parameters */
    float vbbf_max; /**< Max forward biasing voltage in active mode */
    float vbbr_max; /**< Max reverse biasing voltage in active mode */
    float vsmps_arm_opp0_max;   /**< Max allowed SMPS Voltage for ARM OPP0(1.85Ghz) */
    float vsmps_arm_opp0b_max;  /**< Max allowed SMPS Voltage for ARM OPP0(1.7Ghz) */
    float vsmps_arm_opp1_max;   /**< Max allowed SMPS Voltage for ARM OPP1 */
    float vsmps_arm_opp2_max;   /**< Max allowed SMPS Voltage for ARM OPP2 */
    float vsmps_arm_opp3_max;   /**< Max allowed SMPS Voltage for ARM OPP3 */
    float vsmps_arm_opp4_max;   /**< Max allowed SMPS Voltage for ARM OPP4 */
    float vsmps_arm_opp5_max;   /**< Max allowed SMPS Voltage for ARM OPP5 */
    float vsmps_ape_opp1_max;   /**< Max allowed SMPS Voltage for APE OPP1 */
    float vsmps_ape_opp2_max;   /**< Max allowed SMPS Voltage for APE OPP2 */
    float vsmps_safe_opp1_max;  /**< Max allowed SMPS Voltage for SAFE OPP1 */
    float vsmps_safe_opp2_max;  /**< Max allowed SMPS Voltage for SAFE OPP1 */
    float smps_accuracy_pos;    /**< AB9540 Regulator Positive Precision */
    float smps_accuracy_neg;    /**< AB9540 Regulator Negative Precision */
    float temp_offset;          /**< Supply voltage margin taken into account for temperature variations */
    float array_periph_rule;    /**< Maximum voltage between array and periphery */
    float periph_array_rule;    /**< Maximum voltage between periphery and array*/
    float plm_rule;             /**< Maximum voltage between the PLM voltages ( Varm-Vsafe ) */
    /* ARM parameters */
    float arm_step_opp0;        /**< Step used to compute fuse field for OPP0 */
    float arm_step_opp1;        /**< Step used to compute fuse field for OPP1 */
    float arm_step_opp2;        /**< Step used to compute fuse field for OPP2 */
    float arm_step_opp3;        /**< Step used to compute fuse field for OPP3 */
    float arm_step_opp4;        /**< Step used to compute fuse field for OPP4 */
    float arm_step_opp5;        /**< Step used to compute fuse field for OPP5 */
    float arm_vtxref_opp0;      /**< Reference ARM min voltage at transistor level for OPP0 (1.85Ghz) */
    float arm_vtxref_opp0b;     /**< Reference ARM min voltage at transistor level for OPP0 (1.7Ghz) */
    float arm_vtxref_opp1;      /**< Reference ARM min voltage at transistor level for OPP1 */
    float arm_vtxref_opp2;      /**< Reference ARM min voltage at transistor level for OPP2 */
    float arm_vtxref_opp3;      /**< Reference ARM min voltage at transistor level for OPP3 */
    float arm_vtxref_opp4;      /**< Reference ARM min voltage at transistor level for OPP4 */
    float arm_vtxref_opp5;      /**< Reference ARM min voltage at transistor level for OPP5 */
    float arm_epod_drop_opp0;   /**< In die ePOD drop for ARM IP at OPP0(1.85Ghz) */
    float arm_epod_drop_opp0b;  /**< In die ePOD drop for ARM IP at OPP0(1.7Ghz) */
    float arm_epod_drop_opp1;   /**< In die ePOD drop for ARM IP at OPP1 */
    float arm_epod_drop_opp2;   /**< In die ePOD drop for ARM IP at OPP2 */
    float arm_epod_drop_opp3;   /**< In die ePOD drop for ARM IP at OPP3 */
    float arm_epod_drop_opp4;   /**< In die ePOD drop for ARM IP at OPP4 */
    float arm_epod_drop_opp5;   /**< In die ePOD drop for ARM IP at OPP5 */
    float arm_pcb_drop_opp0;    /**< PCB drop for ARM supply at OPP0 (1.85Ghz) */
    float arm_pcb_drop_opp0b;   /**< PCB drop for ARM supply at OPP0 (1.7Ghz) */
    float arm_pcb_drop_opp1;    /**< PCB drop for ARM supply at OPP1 */
    float arm_pcb_drop_opp2;    /**< PCB drop for ARM supply at OPP2 */
    float arm_pcb_drop_opp3;    /**< PCB drop for ARM supply at OPP3 */
    float arm_pcb_drop_opp4;    /**< PCB drop for ARM supply at OPP4 */
    float arm_pcb_drop_opp5;    /**< PCB drop for ARM supply at OPP5 */
    float arm_transient_ov_opp0;    /**< SMPS overshoot for ARM supply at OPP0 (1.85Ghz) */
    float arm_transient_ov_opp0b;   /**< SMPS overshoot for ARM supply at OPP0 (1.7Ghz) */
    float arm_transient_ov_opp1;    /**< SMPS overshoot for ARM supply at OPP1 */
    float arm_transient_ov_opp2;    /**< SMPS overshoot for ARM supply at OPP2 */
    float arm_transient_ov_opp3;    /**< SMPS overshoot for ARM supply at OPP3 */
    float arm_transient_ov_opp4;    /**< SMPS overshoot for ARM supply at OPP4 */
    float arm_transient_ov_opp5;    /**< SMPS overshoot for ARM supply at OPP5 */
    float arm_transient_un_opp0;    /**< SMPS undershoot for ARM supply at OPP0 (1.85Ghz) */
    float arm_transient_un_opp0b;   /**< SMPS undershoot for ARM supply at OPP0 (1.7Ghz) */
    float arm_transient_un_opp1;    /**< SMPS undershoot for ARM supply at OPP1 */
    float arm_transient_un_opp2;    /**< SMPS undershoot for ARM supply at OPP2 */
    float arm_transient_un_opp3;    /**< SMPS undershoot for ARM supply at OPP3 */
    float arm_transient_un_opp4;    /**< SMPS undershoot for ARM supply at OPP4 */
    float arm_transient_un_opp5;    /**< SMPS undershoot for ARM supply at OPP5 */
    float arm_aging_opp0;       /**< ARM supply voltage margin taken into account for temperature variations at OPP0 (1.85Ghz) */
    float arm_aging_opp0b;      /**< ARM supply voltage margin taken into account for temperature variations at OPP0 (1.7Ghz) */
    float arm_aging_opp1;       /**< ARM supply voltage margin taken into account for temperature variations at OPP1 */
    float arm_aging_opp2;       /**< ARM supply voltage margin taken into account for temperature variations at OPP2 */
    float arm_aging_opp3;       /**< ARM supply voltage margin taken into account for temperature variations at OPP3 */
    float arm_aging_opp4;       /**< ARM supply voltage margin taken into account for temperature variations at OPP4 */
    float arm_aging_opp5;       /**< ARM supply voltage margin taken into account for temperature variations at OPP5 */
    float arm_gdrop_opp0;       /**< In die Grip Drop for ARM IP at OPP0 (1.85Ghz) */
    float arm_gdrop_opp0b;      /**< In die Grip Drop for ARM IP at OPP0 (1.7Ghz) */
    float arm_gdrop_opp1;       /**< In die Grip Drop for ARM IP at OPP1 */
    float arm_gdrop_opp2;       /**< In die Grip Drop for ARM IP at OPP2 */
    float arm_gdrop_opp3;       /**< In die Grip Drop for ARM IP at OPP3 */
    float arm_gdrop_opp4;       /**< In die Grip Drop for ARM IP at OPP4 */
    float arm_gdrop_opp5;       /**< In die Grip Drop for ARM IP at OPP5 */
    float arm_vsmps_ret;        /**< Retention supply voltage at SMPS level for ARM */
    /* APE parameters */
    float ape_step_opp1;        /**< Step used to compute fuse field for OPP1 */
    float ape_step_opp2;        /**< Step used to compute fuse field for OPP2 */
    float ape_vtxref_opp1;      /**< Reference APE min voltage at transistor level for OPP1 */
    float ape_vtxref_opp2;      /**< Reference APE min voltage at transistor level for OPP2 */
    float ape_epod_drop_opp1;   /**< In die ePOD drop for APE IP at OPP1 */
    float ape_epod_drop_opp2;   /**< In die ePOD drop for APE IP at OPP2 */
    float ape_pcb_drop_opp1;    /**< PCB drop for APE supply at OPP1 */
    float ape_pcb_drop_opp2;    /**< PCB drop for APE supply at OPP2 */
    float ape_transient_ov_opp1;  /**< SMPS overshoot for APE supply at OPP1 */
    float ape_transient_ov_opp2;  /**< SMPS overshoot for APE supply at OPP2 */
    float ape_transient_un_opp1;  /**< SMPS undershoot for APE supply at OPP1 */
    float ape_transient_un_opp2;  /**< SMPS undershoot for APE supply at OPP2 */
    float ape_aging_opp1;         /**< APE supply voltage margin taken into account for temperature variations at OPP1 */
    float ape_aging_opp2;         /**< APE supply voltage margin taken into account for temperature variations at OPP2 */
    float ape_gdrop_opp1;         /**< In die Grip Drop for APE IP at OPP1 */
    float ape_gdrop_opp2;         /**< In die Grip Drop for APE IP at OPP2 */
    /* SAFE parameters */
    float safe_step_opp1;  /**< Step used to compute fuse field for OPP1 */
    float safe_step_opp2;  /**< Step used to compute fuse field for OPP2 */
    float safe_vtxref_opp1;         /**< Reference SAFE min voltage at transistor level for OPP1 */
    float safe_vtxref_opp2;         /**< Reference SAFE min voltage at transistor level for OPP2 */
    float safe_epod_drop_opp1;      /**< In die ePOD drop for SAFE IP at OPP1 */
    float safe_epod_drop_opp2;      /**< In die ePOD drop for SAFE IP at OPP2 */
    float safe_pcb_drop_opp1;       /**< PCB drop for SAFE supply at OPP1 */
    float safe_pcb_drop_opp2;       /**< PCB drop for SAFE supply at OPP2 */
    float safe_transient_ov_opp1;     /**< SMPS overshoot for SAFE supply at OPP1 */
    float safe_transient_ov_opp2;     /**< SMPS overshoot for SAFE supply at OPP2 */
    float safe_transient_un_opp1;     /**< SMPS undershoot for SAFE supply at OPP1 */
    float safe_transient_un_opp2;     /**< SMPS undershoot for SAFE supply at OPP2 */
    float safe_aging_opp1;            /**< SAFE supply voltage margin taken into account for temperature variations at OPP1 */
    float safe_aging_opp2;            /**< SAFE supply voltage margin taken into account for temperature variations at OPP2 */
    float safe_gdrop_opp1;            /**< In die Grip Drop for SAFE IP at OPP1 */
    float safe_gdrop_opp2;            /**< In die Grip Drop for SAFE IP at OPP2 */
    float arm_opp4_vt_signoff;        /**< Arm OPP4 Vt signoff voltage value */
    float arm_vt_plm_rule;            /**< ARM Vt PLM Rule value */
    float arm_opp0_no_avs_1_7G_smps;  /**< ARM OPP0 NO AVS SMPS @1.7G */
    float arm_opp0_no_avs_1_7G_vt;    /**< ARM OPP0 NO AVS Vt @1.7G */
}tab_const_t;

/*! @struct tab_avs_fuse_t
    @brief structure of fuse constants
 */
typedef struct{
    /* FRV21 [31:0] register */
    unsigned int avs_frv21_field; /**< FRV21 AVS field */
    unsigned int thermal_sensor_offset_field;   /**< thermal sensor field */
    unsigned int thermal_sensor_offset_shift;   /**< thermal sensor shift */
    unsigned int thermal_sensor_offset_length;  /**< thermal sensor lenght */
    unsigned int voltage_sensor_offset_field;   /**< voltage sensor field */
    unsigned int voltage_sensor_offset_shift;   /**< voltage sensor shift */
    unsigned int voltage_sensor_offset_length;  /**< voltage sensor lenght */
    unsigned int voltage_sensor_trim_field;     /**< voltage sensor trim field */
    unsigned int voltage_sensor_trim_shift;     /**< voltage sensor trim shift */
    unsigned int voltage_sensor_trim_length;    /**< voltage sensor trim lenght */
    unsigned int arm_vbbn_field;        /**< Arm Vbbn field */
    unsigned int arm_vbbn_shift;        /**< Arm Vbbn shift */
    unsigned int arm_vbbn_length;       /**< Arm Vbbn lenght */
    unsigned int arm_vbbn_2comp_mask;   /**< Arm Vbbn 2's complement mask */
    unsigned int arm_vbbp_field;        /**< Arm Vbbp field */
    unsigned int arm_vbbp_shift;        /**< Arm Vbbp shift */
    unsigned int arm_vbbp_length;       /**< Arm Vbbp lenght */
    unsigned int arm_vbbp_2comp_mask;   /**< Arm Vbbp 2's complement mask */
    unsigned int arm_opp0_offset_field;         /**< Arm OPP0 offset field */
    unsigned int arm_opp0_offset_shift;         /**< Arm OPP0 offset shift */
    unsigned int arm_opp0_offset_length;        /**< Arm OPP0 offset lenght */
    unsigned int arm_opp0_offset_2comp_mask;    /**< Arm OPP0 offset 2's complement mask */
    unsigned int arm_opp1_offset_field;         /**< Arm OPP1 offset field */
    unsigned int arm_opp1_offset_shift;         /**< Arm OPP1 offset shift */
    unsigned int arm_opp1_offset_length;        /**< Arm OPP1 offset lenght */
    unsigned int arm_opp1_offset_2comp_mask;    /**< Arm OPP1 offset 2's complement mask */
    unsigned int arm_opp2_offset_lsb_field;     /**< Arm OPP2 LSB in FRV21 offset field */
    unsigned int arm_opp2_offset_lsb_shift;     /**< Arm OPP2 LSB in FRV21 offset shift */
    /* FRV22 [63 : 32] register but cast on 32 bits [31:0]*/
    unsigned int avs_frv22_field;               /**< FRV22 AVS field */
    unsigned int arm_opp2_offset_shift;         /**< Arm OPP2 offset shift */
    unsigned int arm_opp2_offset_msb_field;     /**< Arm OPP2 MSB in FRV22 offset field */
    unsigned int arm_opp2_offset_msb_shift;     /**< Arm OPP2 MSB in FRV22 offset shift */
    unsigned int arm_opp2_offset_length;        /**< Arm OPP2 offset lenght */
    unsigned int arm_opp2_offset_2comp_mask;    /**< Arm OPP2 offset 2's complement mask */
    unsigned int arm_opp3_offset_field;         /**< Arm OPP3 offset field */
    unsigned int arm_opp3_offset_shift;         /**< Arm OPP3 offset shift */
    unsigned int arm_opp3_offset_length;        /**< Arm OPP3 offset lenght */
    unsigned int arm_opp3_offset_2comp_mask;    /**< Arm OPP3 offset 2's complement mask */
    unsigned int arm_opp4_offset_field;         /**< Arm OPP4 offset field */
    unsigned int arm_opp4_offset_shift;         /**< Arm OPP4 offset shift */
    unsigned int arm_opp4_offset_length;        /**< Arm OPP4 offset lenght */
    unsigned int arm_opp4_offset_2comp_mask;    /**< Arm OPP4 offset 2's complement mask */
    unsigned int arm_opp5_offset_field;         /**< Arm OPP5 offset field */
    unsigned int arm_opp5_offset_shift;         /**< Arm OPP5 offset shift */
    unsigned int arm_opp5_offset_length;        /**< Arm OPP5 offset lenght */
    unsigned int arm_opp5_offset_2comp_mask;    /**< Arm OPP5 offset 2's complement mask */
    unsigned int safe_opp2_offset_field;        /**< Safe OPP2 offset field */
    unsigned int safe_opp2_offset_shift;        /**< Safe OPP2 offset shift */
    unsigned int safe_opp2_offset_length;       /**< Safe OPP2 offset lenght */
    unsigned int safe_opp2_offset_2comp_mask;   /**< Safe OPP2 offset 2's complement mask */
    unsigned int safe_opp1_offset_field;        /**< Safe OPP1 offset field */
    unsigned int safe_opp1_offset_shift;        /**< Safe OPP1 offset shift */
    unsigned int safe_opp1_offset_length;       /**< Safe OPP1 offset lenght */
    unsigned int safe_opp1_offset_2comp_mask;   /**< Safe OPP1 offset 2's complement mask */
    unsigned int ape_opp2_offset_field;         /**< Ape OPP2 offset field */
    unsigned int ape_opp2_offset_shift;         /**< Ape OPP2 offset shift */
    unsigned int ape_opp2_offset_length;        /**< Ape OPP2 offset lenght */
    unsigned int ape_opp2_offset_2comp_mask;    /**< Ape OPP2 offset 2's complement mask */
    unsigned int ape_opp1_offset_field;         /**< Ape OPP1 offset field */
    unsigned int ape_opp1_offset_shift;         /**< Ape OPP1 offset shift */
    unsigned int ape_opp1_offset_length;        /**< Ape OPP1 offset lenght */
    unsigned int ape_opp1_offset_2comp_mask;    /**< Ape OPP1 offset 2's complement mask */
    unsigned int avs_frv6_field;                /**< FRV6 AVs field */
    unsigned int avs_rev_field;                 /**< AVS Revision field */
    unsigned int avs_rev_shift;                 /**< AVS Revision shift */
    unsigned int avs_rev_length;                /**< AVS Revision length */
    unsigned int opp0_mask_field;               /**< OPP0 mask field */
    unsigned int opp0_mask_shift;               /**< OPP0 mask shift */
    unsigned int opp0_mask_length;              /**< OPP0 mask length */
    unsigned int opp0_enable_field;             /**< OPP0 enable field */
    unsigned int opp0_enable_shift;             /**< OPP0 enable shift */
    unsigned int opp0_enable_length;            /**< OPP0 enable length */
    unsigned int avs_frv7_field;                /**< FRV7 AVs field */
    unsigned int chip_config_field;             /**< Chip config field */
    unsigned int chip_config_shift;             /**< Chip config shift */
    unsigned int chip_config_length;            /**< Chip config length */
}tab_avs_fuse_t;


/*-------------------------------------------------------------------------*/
/*                        function declaration                             */
/*-------------------------------------------------------------------------*/

extern void memcpy(u8 *dest, u8* src, unsigned length);
Opp_status OPP_GetOppConfig(u32 avs_fuses[6], const u16 *Opp_Default, sPrcmuApi_InitOppData_t *stOpp_data);


#endif /*__AVS_CALC_H__*/
