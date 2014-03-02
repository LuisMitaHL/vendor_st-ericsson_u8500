/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage audio dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#define TATL03PARAM_C
#include "tatlaudio.h"
#undef  TATL03PARAM_C


/* Non public audio parameters list */
typedef enum {
    /* numbering must begin after the public audio parameters */
    AUDIO_PARAM_EAR_DIG_PATH_PWR = AUDIO_PARAM_INTERNALS,
    AUDIO_PARAM_HSL_PATH_PWR,
    AUDIO_PARAM_HSL_DAC_DRIVER_PWR,
    AUDIO_PARAM_HSR_PATH_PWR,
    AUDIO_PARAM_HSR_DAC_DRIVER_PWR,
    AUDIO_PARAM_HFL_PATH_PWR,
    AUDIO_PARAM_HFR_PATH_PWR,
    AUDIO_PARAM_VIB1_PATH_PWR,
    AUDIO_PARAM_VIB1_CTRL_INT,
    AUDIO_PARAM_ADC1_PWR,
    AUDIO_PARAM_ADC2_PWR,
    AUDIO_PARAM_ADC3_PWR,
    AUDIO_PARAM_EPWM2TOGPIOSEL

} e_dthaudio_private_param;


/* pre-declarations of private functions */
int DthAudio_mapParam(uint32_t param, uint32_t*, int);
int DthAudio_setParam(uint32_t, uint32_t);
int DthAudio_getParam(uint32_t, uint32_t*);

/* guards for reentrant functions */
int vg_dthaudio_setParam_guard = 0;
int vg_dthaudio_getParam_guard = 0;

#define DTH_DIGITAL_MULTIPLEXERS_REG DIGITAL_MULTIPLEXERS_REG
#define DTH_DIGITAL_MULTIPLEXERS_MASK /*0x3E*/DIG_MICRO_1_FOR_OUT_1_MASK|DIG_MICRO_2_FOR_OUT_2_MASK|DIG_MICRO_3_FOR_OUT_3_MASK|DIG_MICRO_5_FOR_OUT_5_MASK|DIG_MICRO_6_FOR_OUT_6_MASK

typedef enum dth_usb_interface {
    DTH_ANALOG = 0x0C,
    DTH_HEADSET = 0X03,
    DTH_CARKIT = 0XFC
} DTH_USB_INTERFACE_T ;

typedef enum dth_usb_mic {
    DTH_NO_ONE = 0x0,
    DTH_USBSWCAP = 0x1,
    DTH_USB_DP = 0x2,
    DTH_USB_ID = 0x3
} DTH_USB_MIC_T ;

typedef enum dth_headset_analog_gains {
    DTH_HS_ANALOG_GAIN_4_DB = 0x0,
    DTH_HS_ANALOG_GAIN_2_DB = 0x1,
    DTH_HS_ANALOG_GAIN_0_DB = 0x2,
    DTH_HS_ANALOG_GAIN_MINUS_2_DB = 0x3,
    DTH_HS_ANALOG_GAIN_MINUS_4_DB = 0x4,
    DTH_HS_ANALOG_GAIN_MINUS_6_DB = 0x5,
    DTH_HS_ANALOG_GAIN_MINUS_8_DB = 0x6,
    DTH_HS_ANALOG_GAIN_MINUS_10_DB = 0x7,
    DTH_HS_ANALOG_GAIN_MINUS_12_DB = 0x8,
    DTH_HS_ANALOG_GAIN_MINUS_14_DB = 0x9,
    DTH_HS_ANALOG_GAIN_MINUS_16_DB = 0xA,
    DTH_HS_ANALOG_GAIN_MINUS_18_DB = 0xB,
    DTH_HS_ANALOG_GAIN_MINUS_20_DB = 0xC,
    DTH_HS_ANALOG_GAIN_MINUS_24_DB = 0xD,
    DTH_HS_ANALOG_GAIN_MINUS_28_POINT_2_DB = 0xE,
    DTH_HS_ANALOG_GAIN_MINUS_31_POINT_8_DB = 0xF
} HS_ANALOG_GAINS_T ;

typedef enum dthear_analog_gains {
    DTHEAR_ANALOG_GAIN_8_DB = 0x0,
    DTHEAR_ANALOG_GAIN_6_DB = 0x1,
    DTHEAR_ANALOG_GAIN_4_DB = 0x2,
    DTHEAR_ANALOG_GAIN_2_DB = 0x3,
    DTHEAR_ANALOG_GAIN_0_DB = 0x4,
    DTHEAR_ANALOG_GAIN_MINUS_2_DB = 0x5,
    DTHEAR_ANALOG_GAIN_MINUS_4_DB = 0x6,
    DTHEAR_ANALOG_GAIN_MINUS_6_DB = 0x7,
    DTHEAR_ANALOG_GAIN_MINUS_8_DB = 0x8,
    DTHEAR_ANALOG_GAIN_MINUS_8_DB_ = 0x9,
    DTHEAR_ANALOG_GAIN_MINUS_8_DB__ = 0xa,
    DTHEAR_ANALOG_GAIN_MINUS_8_DB___ = 0xB,
    DTHEAR_ANALOG_GAIN_MINUS_8_DB____ = 0xC,
    DTHEAR_ANALOG_GAIN_MINUS_8_DB_____ = 0xD,
    DTHEAR_ANALOG_GAIN_MINUS_8_DB______ = 0xE,
    DTHEAR_ANALOG_GAIN_MINUS_8_DB_______ = 0xF
} EAR_GAINS_T ;


typedef enum dth_hf_analog_gains {
	DTH_HF_ANALOG_GAIN_20_DB = 0,
	DTH_HF_ANALOG_GAIN_19_DB = 1,
	DTH_HF_ANALOG_GAIN_18_DB = 2,
	DTH_HF_ANALOG_GAIN_17_DB = 3,
	DTH_HF_ANALOG_GAIN_16_DB = 4,
	DTH_HF_ANALOG_GAIN_15_DB = 5,
	DTH_HF_ANALOG_GAIN_14_DB = 6,
	DTH_HF_ANALOG_GAIN_13_DB = 7,
	DTH_HF_ANALOG_GAIN_12_DB = 8,
	DTH_HF_ANALOG_GAIN_11_DB = 9,
	DTH_HF_ANALOG_GAIN_10_DB = 10,
	DTH_HF_ANALOG_GAIN_9_DB = 11,
	DTH_HF_ANALOG_GAIN_8_DB = 12,
	DTH_HF_ANALOG_GAIN_7_DB = 13,
	DTH_HF_ANALOG_GAIN_6_DB = 14,
	DTH_HF_ANALOG_GAIN_5_DB = 15,
	DTH_HF_ANALOG_GAIN_4_DB = 16,
	DTH_HF_ANALOG_GAIN_3_DB = 17,
	DTH_HF_ANALOG_GAIN_2_DB = 18,
	DTH_HF_ANALOG_GAIN_1_DB = 19,
	DTH_HF_ANALOG_GAIN_0_DB = 20,
	DTH_HF_ANALOG_GAIN_MINUS_1_DB = 21,
	DTH_HF_ANALOG_GAIN_MINUS_2_DB = 22,
	DTH_HF_ANALOG_GAIN_MINUS_3_DB = 23
} HF_GAINS_T ;

typedef enum dth_vib_analog_gains {
	DTH_VIB_ANALOG_GAIN_20_DB = 0,
	DTH_VIB_ANALOG_GAIN_19_DB = 1,
	DTH_VIB_ANALOG_GAIN_18_DB = 2,
	DTH_VIB_ANALOG_GAIN_17_DB = 3,
	DTH_VIB_ANALOG_GAIN_16_DB = 4,
	DTH_VIB_ANALOG_GAIN_15_DB = 5,
	DTH_VIB_ANALOG_GAIN_14_DB = 6,
	DTH_VIB_ANALOG_GAIN_13_DB = 7,
	DTH_VIB_ANALOG_GAIN_12_DB = 8,
	DTH_VIB_ANALOG_GAIN_11_DB = 9,
	DTH_VIB_ANALOG_GAIN_10_DB = 10,
	DTH_VIB_ANALOG_GAIN_9_DB = 11,
	DTH_VIB_ANALOG_GAIN_8_DB = 12,
	DTH_VIB_ANALOG_GAIN_7_DB = 13,
	DTH_VIB_ANALOG_GAIN_6_DB = 14,
	DTH_VIB_ANALOG_GAIN_5_DB = 15,
	DTH_VIB_ANALOG_GAIN_4_DB = 16,
	DTH_VIB_ANALOG_GAIN_3_DB = 17,
	DTH_VIB_ANALOG_GAIN_2_DB = 18,
	DTH_VIB_ANALOG_GAIN_1_DB = 19,
	DTH_VIB_ANALOG_GAIN_0_DB = 20,
	DTH_VIB_ANALOG_GAIN_MINUS_1_DB = 21,
	DTH_VIB_ANALOG_GAIN_MINUS_2_DB = 22,
	DTH_VIB_ANALOG_GAIN_MINUS_3_DB = 23
} VIB_GAINS_T ;

typedef enum dth_headset_gains {
    DTH_HEADSET_GAIN_8_DB = 0x0,
    DTH_HEADSET_GAIN_7_DB = 0x1,
    DTH_HEADSET_GAIN_6_DB = 0x2,
    DTH_HEADSET_GAIN_5_DB = 0x3,
    DTH_HEADSET_GAIN_4_DB = 0x4,
    DTH_HEADSET_GAIN_3_DB = 0x5,
    DTH_HEADSET_GAIN_2_DB = 0x6,
    DTH_HEADSET_GAIN_1_DB = 0x7,
    DTH_HEADSET_GAIN_0_DB = 0x8,
    DTH_HEADSET_GAIN_MINUS_INF = 0x9,
    DTH_HEADSET_GAIN_MINUS_INF_ = 0xA,
    DTH_HEADSET_GAIN_MINUS_INF__ = 0xB,
    DTH_HEADSET_GAIN_MINUS_INF___ = 0xC,
    DTH_HEADSET_GAIN_MINUS_INF____ = 0xD,
    DTH_HEADSET_GAIN_MINUS_INF_____ = 0xE,
    DTH_HEADSET_GAIN_MINUS_INF______ = 0xF

} HEADSET_GAINS_T ;

typedef enum dth_linein_to_headset {
    DTH_LINEIN_TO_HS_GAIN_0_DB = 0x0,
    DTH_LINEIN_TO_HS_GAIN_MINUS_2_DB = 0x1,
    DTH_LINEIN_TO_HS_GAIN_MINUS_4_DB = 0x2,
    DTH_LINEIN_TO_HS_GAIN_MINUS_6_DB = 0x3,
    DTH_LINEIN_TO_HS_GAIN_MINUS_8_DB = 0x4,
    DTH_LINEIN_TO_HS_GAIN_MINUS_10_DB = 0x5,
    DTH_LINEIN_TO_HS_GAIN_MINUS_12_DB = 0x6,
    DTH_LINEIN_TO_HS_GAIN_MINUS_14_DB = 0x7,
    DTH_LINEIN_TO_HS_GAIN_MINUS_16_DB = 0x8,
    DTH_LINEIN_TO_HS_GAIN_MINUS_18_DB = 0x9,
    DTH_LINEIN_TO_HS_GAIN_MINUS_20_DB = 0xA,
    DTH_LINEIN_TO_HS_GAIN_MINUS_22_DB = 0xB,
    DTH_LINEIN_TO_HS_GAIN_MINUS_24_DB = 0xC,
    DTH_LINEIN_TO_HS_GAIN_MINUS_26_DB = 0xD,
    DTH_LINEIN_TO_HS_GAIN_MINUS_28_DB = 0xE,
    DTH_LINEIN_TO_HS_GAIN_MINUS_30_DB = 0xF,
    DTH_LINEIN_TO_HS_GAIN_MINUS_32_DB = 0x10,
    DTH_LINEIN_TO_HS_GAIN_MINUS_34_DB = 0x11,
    DTH_LINEIN_TO_HS_GAIN_MINUS_36_DB = 0x12,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF = 0x13,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF_ = 0x14,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF__ = 0x15,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF___ = 0x16,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF____ = 0x17,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF_____ = 0x18,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF______ = 0x19,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF_______ = 0x1A,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF________ = 0x1B,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF_________ = 0x1C,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF__________ = 0x1D,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF___________ = 0x1E,
    DTH_LINEIN_TO_HS_GAIN_MINUS_INF____________ = 0x1F
} LINEIN_TO_HEADSET_T ;

typedef enum dth_digital_gain_side_tone {
    DTH_SIDETONE_GAIN_0_DB = 0x0,
    DTH_SIDETONE_GAIN_MINUS_1_DB = 0x1,
    DTH_SIDETONE_GAIN_MINUS_2_DB = 0x2,
    DTH_SIDETONE_GAIN_MINUS_3_DB = 0x3,
    DTH_SIDETONE_GAIN_MINUS_4_DB = 0x4,
    DTH_SIDETONE_GAIN_MINUS_5_DB = 0x5,
    DTH_SIDETONE_GAIN_MINUS_6_DB = 0x6,
    DTH_SIDETONE_GAIN_MINUS_7_DB = 0x7,
    DTH_SIDETONE_GAIN_MINUS_8_DB = 0x8,
    DTH_SIDETONE_GAIN_MINUS_9_DB = 0x9,
    DTH_SIDETONE_GAIN_MINUS_10_DB = 0xA,
    DTH_SIDETONE_GAIN_MINUS_11_DB = 0xB,
    DTH_SIDETONE_GAIN_MINUS_12_DB = 0xC,
    DTH_SIDETONE_GAIN_MINUS_13_DB = 0xD,
    DTH_SIDETONE_GAIN_MINUS_14_DB = 0xE,
    DTH_SIDETONE_GAIN_MINUS_15_DB = 0xF,
    DTH_SIDETONE_GAIN_MINUS_16_DB = 0x10,
    DTH_SIDETONE_GAIN_MINUS_17_DB = 0x11,
    DTH_SIDETONE_GAIN_MINUS_18_DB = 0x12,
    DTH_SIDETONE_GAIN_MINUS_19_DB = 0x13,
    DTH_SIDETONE_GAIN_MINUS_20_DB = 0x14,
    DTH_SIDETONE_GAIN_MINUS_21_DB = 0x15,
    DTH_SIDETONE_GAIN_MINUS_22_DB = 0x16,
    DTH_SIDETONE_GAIN_MINUS_23_DB = 0x17,
    DTH_SIDETONE_GAIN_MINUS_24_DB = 0x18,
    DTH_SIDETONE_GAIN_MINUS_25_DB = 0x19,
    DTH_SIDETONE_GAIN_MINUS_26_DB = 0x1A,
    DTH_SIDETONE_GAIN_MINUS_27_DB = 0x1B,
    DTH_SIDETONE_GAIN_MINUS_28_DB = 0x1C,
    DTH_SIDETONE_GAIN_MINUS_29_DB = 0x1D,
    DTH_SIDETONE_GAIN_MINUS_30_DB = 0x1E,
    DTH_SIDETONE_GAIN_MINUS_INF = 0x1F
} DTH_DIGITAL_GAIN_SIDE_TONE_T ;

typedef enum dth_ad_digital_gain {
    DTH_AD_DIGITAL_GAIN_31_DB = 0x0,
    DTH_AD_DIGITAL_GAIN_30_DB = 0x1,
    DTH_AD_DIGITAL_GAIN_29_DB = 0x2,
    DTH_AD_DIGITAL_GAIN_28_DB = 0x3,
    DTH_AD_DIGITAL_GAIN_27_DB = 0x4,
    DTH_AD_DIGITAL_GAIN_26_DB = 0x5,
    DTH_AD_DIGITAL_GAIN_25_DB = 0x6,
    DTH_AD_DIGITAL_GAIN_24_DB = 0x7,
    DTH_AD_DIGITAL_GAIN_23_DB = 0x8,
    DTH_AD_DIGITAL_GAIN_22_DB = 0x9,
    DTH_AD_DIGITAL_GAIN_21_DB = 0xA,
    DTH_AD_DIGITAL_GAIN_20_DB = 0xB,
    DTH_AD_DIGITAL_GAIN_19_DB = 0xC,
    DTH_AD_DIGITAL_GAIN_18_DB = 0xD,
    DTH_AD_DIGITAL_GAIN_17_DB = 0xE,
    DTH_AD_DIGITAL_GAIN_16_DB = 0xF,
    DTH_AD_DIGITAL_GAIN_15_DB = 0x10,
    DTH_AD_DIGITAL_GAIN_14_DB = 0x11,
    DTH_AD_DIGITAL_GAIN_13_DB = 0x12,
    DTH_AD_DIGITAL_GAIN_12_DB = 0x13,
    DTH_AD_DIGITAL_GAIN_11_DB = 0x14,
    DTH_AD_DIGITAL_GAIN_10_DB = 0x15,
    DTH_AD_DIGITAL_GAIN_9_DB = 0x16,
    DTH_AD_DIGITAL_GAIN_8_DB = 0x17,
    DTH_AD_DIGITAL_GAIN_7_DB = 0x18,
    DTH_AD_DIGITAL_GAIN_6_DB = 0x19,
    DTH_AD_DIGITAL_GAIN_5_DB = 0x1A,
    DTH_AD_DIGITAL_GAIN_4_DB = 0x1B,
    DTH_AD_DIGITAL_GAIN_3_DB = 0x1C,
    DTH_AD_DIGITAL_GAIN_2_DB = 0x1D,
    DTH_AD_DIGITAL_GAIN_1_DB = 0x1E,
    DTH_AD_DIGITAL_GAIN_0_DB = 0x1F,
    DTH_AD_DIGITAL_GAIN_MINUS_1_DB = 0x20,
    DTH_AD_DIGITAL_GAIN_MINUS_2_DB = 0x21,
    DTH_AD_DIGITAL_GAIN_MINUS_3_DB = 0x22,
    DTH_AD_DIGITAL_GAIN_MINUS_4_DB = 0x23,
    DTH_AD_DIGITAL_GAIN_MINUS_5_DB = 0x24,
    DTH_AD_DIGITAL_GAIN_MINUS_6_DB = 0x25,
    DTH_AD_DIGITAL_GAIN_MINUS_7_DB = 0x26,
    DTH_AD_DIGITAL_GAIN_MINUS_8_DB = 0x27,
    DTH_AD_DIGITAL_GAIN_MINUS_9_DB = 0x28,
    DTH_AD_DIGITAL_GAIN_MINUS_10_DB = 0x29,
    DTH_AD_DIGITAL_GAIN_MINUS_11_DB = 0x2A,
    DTH_AD_DIGITAL_GAIN_MINUS_12_DB = 0x2B,
    DTH_AD_DIGITAL_GAIN_MINUS_13_DB = 0x2C,
    DTH_AD_DIGITAL_GAIN_MINUS_14_DB = 0x2D,
    DTH_AD_DIGITAL_GAIN_MINUS_15_DB = 0x2E,
    DTH_AD_DIGITAL_GAIN_MINUS_16_DB = 0x2F,
    DTH_AD_DIGITAL_GAIN_MINUS_17_DB = 0x30,
    DTH_AD_DIGITAL_GAIN_MINUS_18_DB = 0x31,
    DTH_AD_DIGITAL_GAIN_MINUS_19_DB = 0x32,
    DTH_AD_DIGITAL_GAIN_MINUS_20_DB = 0x33,
    DTH_AD_DIGITAL_GAIN_MINUS_21_DB = 0x34,
    DTH_AD_DIGITAL_GAIN_MINUS_22_DB = 0x35,
    DTH_AD_DIGITAL_GAIN_MINUS_23_DB = 0x36,
    DTH_AD_DIGITAL_GAIN_MINUS_24_DB = 0x37,
    DTH_AD_DIGITAL_GAIN_MINUS_25_DB = 0x38,
    DTH_AD_DIGITAL_GAIN_MINUS_26_DB = 0x39,
    DTH_AD_DIGITAL_GAIN_MINUS_27_DB = 0x3A,
    DTH_AD_DIGITAL_GAIN_MINUS_28_DB = 0x3B,
    DTH_AD_DIGITAL_GAIN_MINUS_29_DB = 0x3C,
    DTH_AD_DIGITAL_GAIN_MINUS_30_DB = 0x3D,
    DTH_AD_DIGITAL_GAIN_MINUS_31_DB = 0x3E,
    DTH_AD_DIGITAL_GAIN_MINUS_INF = 0x3F
} DTH_AD_DIGITAL_GAIN_T ;

typedef enum dth_da_digital_gain {
    DTH_DA_DIGITAL_GAIN_0_DB = 0x0,
    DTH_DA_DIGITAL_GAIN_MINUS_1_DB = 0x1,
    DTH_DA_DIGITAL_GAIN_MINUS_2_DB = 0x2,
    DTH_DA_DIGITAL_GAIN_MINUS_3_DB = 0x3,
    DTH_DA_DIGITAL_GAIN_MINUS_4_DB = 0x4,
    DTH_DA_DIGITAL_GAIN_MINUS_5_DB = 0x5,
    DTH_DA_DIGITAL_GAIN_MINUS_6_DB = 0x6,
    DTH_DA_DIGITAL_GAIN_MINUS_7_DB = 0x7,
    DTH_DA_DIGITAL_GAIN_MINUS_8_DB = 0x8,
    DTH_DA_DIGITAL_GAIN_MINUS_9_DB = 0x9,
    DTH_DA_DIGITAL_GAIN_MINUS_10_DB = 0xA,
    DTH_DA_DIGITAL_GAIN_MINUS_11_DB = 0xB,
    DTH_DA_DIGITAL_GAIN_MINUS_12_DB = 0xC,
    DTH_DA_DIGITAL_GAIN_MINUS_13_DB = 0xD,
    DTH_DA_DIGITAL_GAIN_MINUS_14_DB = 0xE,
    DTH_DA_DIGITAL_GAIN_MINUS_15_DB = 0xF,
    DTH_DA_DIGITAL_GAIN_MINUS_16_DB = 0x10,
    DTH_DA_DIGITAL_GAIN_MINUS_17_DB = 0x11,
    DTH_DA_DIGITAL_GAIN_MINUS_18_DB = 0x12,
    DTH_DA_DIGITAL_GAIN_MINUS_19_DB = 0x13,
    DTH_DA_DIGITAL_GAIN_MINUS_20_DB = 0x14,
    DTH_DA_DIGITAL_GAIN_MINUS_21_DB = 0x15,
    DTH_DA_DIGITAL_GAIN_MINUS_22_DB = 0x16,
    DTH_DA_DIGITAL_GAIN_MINUS_23_DB = 0x17,
    DTH_DA_DIGITAL_GAIN_MINUS_24_DB = 0x18,
    DTH_DA_DIGITAL_GAIN_MINUS_25_DB = 0x19,
    DTH_DA_DIGITAL_GAIN_MINUS_26_DB = 0x1A,
    DTH_DA_DIGITAL_GAIN_MINUS_27_DB = 0x1B,
    DTH_DA_DIGITAL_GAIN_MINUS_28_DB = 0x1C,
    DTH_DA_DIGITAL_GAIN_MINUS_29_DB = 0x1D,
    DTH_DA_DIGITAL_GAIN_MINUS_30_DB = 0x1E,
    DTH_DA_DIGITAL_GAIN_MINUS_31_DB = 0x1F,
    DTH_DA_DIGITAL_GAIN_MINUS_32_DB = 0x20,
    DTH_DA_DIGITAL_GAIN_MINUS_33_DB = 0x21,
    DTH_DA_DIGITAL_GAIN_MINUS_34_DB = 0x22,
    DTH_DA_DIGITAL_GAIN_MINUS_35_DB = 0x23,
    DTH_DA_DIGITAL_GAIN_MINUS_36_DB = 0x24,
    DTH_DA_DIGITAL_GAIN_MINUS_37_DB = 0x25,
    DTH_DA_DIGITAL_GAIN_MINUS_38_DB = 0x26,
    DTH_DA_DIGITAL_GAIN_MINUS_39_DB = 0x27,
    DTH_DA_DIGITAL_GAIN_MINUS_40_DB = 0x28,
    DTH_DA_DIGITAL_GAIN_MINUS_41_DB = 0x29,
    DTH_DA_DIGITAL_GAIN_MINUS_42_DB = 0x2A,
    DTH_DA_DIGITAL_GAIN_MINUS_43_DB = 0x2B,
    DTH_DA_DIGITAL_GAIN_MINUS_44_DB = 0x2C,
    DTH_DA_DIGITAL_GAIN_MINUS_45_DB = 0x2D,
    DTH_DA_DIGITAL_GAIN_MINUS_46_DB = 0x2E,
    DTH_DA_DIGITAL_GAIN_MINUS_47_DB = 0x2F,
    DTH_DA_DIGITAL_GAIN_MINUS_48_DB = 0x30,
    DTH_DA_DIGITAL_GAIN_MINUS_49_DB = 0x31,
    DTH_DA_DIGITAL_GAIN_MINUS_50_DB = 0x32,
    DTH_DA_DIGITAL_GAIN_MINUS_51_DB = 0x33,
    DTH_DA_DIGITAL_GAIN_MINUS_52_DB = 0x34,
    DTH_DA_DIGITAL_GAIN_MINUS_53_DB = 0x35,
    DTH_DA_DIGITAL_GAIN_MINUS_54_DB = 0x36,
    DTH_DA_DIGITAL_GAIN_MINUS_55_DB = 0x37,
    DTH_DA_DIGITAL_GAIN_MINUS_56_DB = 0x38,
    DTH_DA_DIGITAL_GAIN_MINUS_57_DB = 0x39,
    DTH_DA_DIGITAL_GAIN_MINUS_58_DB = 0x3A,
    DTH_DA_DIGITAL_GAIN_MINUS_59_DB = 0x3B,
    DTH_DA_DIGITAL_GAIN_MINUS_60_DB = 0x3C,
    DTH_DA_DIGITAL_GAIN_MINUS_61_DB = 0x3D,
    DTH_DA_DIGITAL_GAIN_MINUS_62_DB = 0x3E,
    DTH_DA_DIGITAL_GAIN_MINUS_INF = 0x3F
} DTH_DA_DIGITAL_GAIN_T ;


#if !defined VIBRATOR_1_CLASS_D_POWER_MASK
#define VIBRATOR_1_CLASS_D_POWER_MASK VIBRATOR_CLASS_D_POWER_MASK
#endif

#if !defined VIB_1_CONTROLLED_INTERNAL_MASK
#define VIB_1_CONTROLLED_INTERNAL_MASK VIB_CONTROLLED_INTERNAL_MASK
#endif

#if !defined VIBRATOR_1_CLASS_D_PATH_POWER_MASK
#define VIBRATOR_1_CLASS_D_PATH_POWER_MASK VIBRATOR_CLASS_D_PATH_POWER_MASK
#endif

#if !defined VIBRATOR_1_CLASS_D_PATH_POWER_UP_E
#define VIBRATOR_1_CLASS_D_PATH_POWER_UP_E VIBRATOR_CLASS_D_PATH_POWER_UP_E
#endif

#if !defined VIBRATOR_1_CLASS_D_PATH_POWER_DOWN_E
#define VIBRATOR_1_CLASS_D_PATH_POWER_DOWN_E VIBRATOR_CLASS_D_PATH_POWER_DOWN_E
#endif


#if !defined SLOT_ALLOCATION_1_REG
#define SLOT_ALLOCATION_1_REG SLOT_ALLOCATION_0_REG
#endif

#if !defined SLOT_ALLOCATION_3_REG
#define SLOT_ALLOCATION_3_REG SLOT_ALLOCATION_2_REG
#endif

#if !defined SLOT_ALLOCATION_5_REG
#define SLOT_ALLOCATION_5_REG SLOT_ALLOCATION_4_REG
#endif

#if !defined SLOT_ALLOCATION_7_REG
#define SLOT_ALLOCATION_7_REG SLOT_ALLOCATION_6_REG
#endif


int DthAudio_01ParamGet(struct dth_element *elem, void *value)
{
    DECLARE_ERR();

    VERIFY_DTH_TYPE(elem, DTH_TYPE_U32);
    vg_dthaudio_getParam_guard = 0;

    TAT_TRY(DthAudio_getParam(elem->user_data, (uint32_t*)value));

    RETURN_ERR();
}

int DthAudio_02ParamSet(struct dth_element *elem, void *value)
{
    DECLARE_ERR();

    VERIFY_DTH_TYPE(elem, DTH_TYPE_U32);
    if (TAT_LAST_OK()) {
        vg_dthaudio_setParam_guard = 0;
        TAT_TRY(DthAudio_setParam(elem->user_data, DEREF_PTR(value, uint32_t)));
        SYSLOG(LOG_DEBUG, "set param %u to value %u ends with code %d", elem->user_data, DEREF_PTR(value, uint32_t), LAST_ERR);
    } else {
        SYSLOG(LOG_ERR, "Set param returns error %d", LAST_ERR);
    }

    RETURN_ERR();
}

int DthAudio_mapParam(uint32_t vp_param, uint32_t* pp_value, int vp_isSet)
{
    DECLARE_ERR();

    BEGIN_AUDIO_PARAM_MAP(vp_param)

    AUDIO_PARAM(AUDIO_PARAM_ANALOG_DIGITAL_SEL,     DTH_DIGITAL_MULTIPLEXERS_REG, DTH_DIGITAL_MULTIPLEXERS_MASK,     enum micro,                     pp_value, vp_isSet)

    /* Analog inputs */
    AUDIO_PARAM(AUDIO_PARAM_MIC1,              LINE_IN_CONF_REG, MICRO_1_POWER_MASK,                                 enum micro1_power,                  pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_MIC2,              LINE_IN_CONF_REG, MICRO_2_POWER_MASK,                                 enum micro2_power,                  pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_LINL,              LINE_IN_CONF_REG, LINE_IN_LEFT_POWER_MASK,                            enum line_in_left_power,            pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_LINR,              LINE_IN_CONF_REG, LINE_IN_RIGHT_POWER_MASK,                           enum line_in_right_power,           pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_MICSEL,            ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, ADC_1_CONNECTED_TO_MIC_MASK,   enum adc1_connected_to_mic_a,       pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_LINR_MIC2_SEL,     ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, ADC_2_CONNECTED_TO_MIC_MASK,   enum adc2_connected_to_mic,         pp_value, vp_isSet)

    /* Outputs */
    AUDIO_PARAM(AUDIO_PARAM_EAR,               ANALOG_OUTPUT_ENABLE_REG, EAR_CLASS_AB_POWER_MASK,                    enum ear_class_ab_power,            pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HSL,               ANALOG_OUTPUT_ENABLE_REG, HEADSET_LEFT_POWER_MASK,                    enum headset_left_power,            pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HSR,               ANALOG_OUTPUT_ENABLE_REG, HEADSET_RIGHT_POWER_MASK,                   enum headset_right_power,           pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HFL,               ANALOG_OUTPUT_ENABLE_REG, HF_CLASS_D_LEFT_POWER_MASK,             enum hf_left_class_d_power,         pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HFR,               ANALOG_OUTPUT_ENABLE_REG, HF_CLASS_D_RIGTH_POWER_MASK,                enum hf_right_class_d_power,        pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_VIB1,              ANALOG_OUTPUT_ENABLE_REG, VIBRATOR_1_CLASS_D_POWER_MASK,              enum vibrator_power,               pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_LINEOUT,           0xD0C, 0x0C,     enum vibrator_power,               pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_PDMOUT,            0xD76, 0x03,     enum vibrator_power,               pp_value, vp_isSet)

    /* Output configuration */
    AUDIO_PARAM(AUDIO_PARAM_CLASSD_CONF,       CONFIGURATION_CLASS_D_DRIVERS_REG, VIB_1_CONTROLLED_INTERNAL_MASK,        enum vib1,                          pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_VIB1N_PWM,         CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG, VIB_1_NEGATIVE_DUTY_CYLE_MASK,   enum duty_cycle,                    pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_VIB1P_PWM,         CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG, VIB_1_POSITIVE_DUTY_CYLE_MASK,   enum duty_cycle,                    pp_value, vp_isSet)

    /* Analog and digital gains (analog) */
    AUDIO_PARAM(AUDIO_PARAM_HSL_ANALOG_GAIN,   0XD16, 0xF0,          enum dth_headset_analog_gains,  pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HSR_ANALOG_GAIN,   0XD16, 0x0F,          enum dth_headset_analog_gains,  pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_EAR_ANALOG_GAIN,   0xD75, 0x0F,          enum dthear_analog_gains,   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HFL_ANALOG_GAIN,   0xD80, 0x1F,          enum dth_hf_analog_gains,   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HFR_ANALOG_GAIN,   0xD81, 0x1F,          enum dth_hf_analog_gains,   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_VIB_ANALOG_GAIN,   0xD82, 0x1F,          enum dth_vib_analog_gains,  pp_value, vp_isSet)


    /* Analog and digital gains (digital) */
    AUDIO_PARAM(AUDIO_PARAM_HSL_EAR_GAIN,      0xD4F, 0x0F,      enum dth_headset_gains,     pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HSR_GAIN,          0xD50, 0x0F,      enum dth_headset_gains,         pp_value, vp_isSet)

    /* DA gains */
    AUDIO_PARAM(AUDIO_PARAM_DA1_GAIN,          DIGITAL_GAIN_DA_1_REG, DA_1_GAIN_MASK,         enum dth_da_digital_gain,               pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_DA2_GAIN,          DIGITAL_GAIN_DA_2_REG, DA_2_GAIN_MASK,         enum dth_da_digital_gain,               pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_DA3_GAIN,          DIGITAL_GAIN_DA_3_REG, DA_3_GAIN_MASK,         enum dth_da_digital_gain,               pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_DA4_GAIN,          DIGITAL_GAIN_DA_4_REG, DA_4_GAIN_MASK,         enum dth_da_digital_gain,               pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_DA5_GAIN,          DIGITAL_GAIN_DA_5_REG, DA_5_GAIN_MASK,         enum dth_da_digital_gain,               pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_DA6_GAIN,          DIGITAL_GAIN_DA6_REG, DA_6_GAIN_MASK,          enum dth_da_digital_gain,               pp_value, vp_isSet)

    /* DA mux */
    AUDIO_PARAM(AUDIO_PARAM_SLOT_DA1,          SLOT_SELECTION_DA_PATH_1_REG, SLOT_DSP_01_MASK,         enum slot_dsp,                      pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_DA2,          SLOT_SELECTION_DA_PATH_2_REG, SLOT_DSP_02_MASK,         enum slot_dsp,                      pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_DA3,          SLOT_SELECTION_DA_PATH_3_REG, SLOT_DSP_03_MASK,         enum slot_dsp,                      pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_DA4,          SLOT_SELECTION_DA_PATH_4_REG, SLOT_DSP_04_MASK,         enum slot_dsp,                      pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_DA5,          SLOT_SELECTION_DA_PATH_5_REG, SLOT_DSP_05_MASK,         enum slot_dsp,                      pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_DA6,          SLOT_SELECTION_DA_PATH_6_REG, SLOT_DSP_06_MASK,         enum slot_dsp,                      pp_value, vp_isSet)

    /* Digital inputs */
    AUDIO_PARAM(AUDIO_PARAM_DMIC1,             DIGITAL_INPUTS_ENABLE_REG, DIGITAL_MICRO_1_MUTE_MASK,        enum digital1_micro_mute,           pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_DMIC2,             DIGITAL_INPUTS_ENABLE_REG, DIGITAL_MICRO_2_MUTE_MASK,        enum digital2_micro_mute,           pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_DMIC3,             DIGITAL_INPUTS_ENABLE_REG, DIGITAL_MICRO_3_MUTE_MASK,        enum digital3_micro_mute,           pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_DMIC4,             DIGITAL_INPUTS_ENABLE_REG, DIGITAL_MICRO_4_MUTE_MASK,        enum digital4_micro_mute,           pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_DMIC5,             DIGITAL_INPUTS_ENABLE_REG, DIGITAL_MICRO_5_MUTE_MASK,        enum digital5_micro_mute,           pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_DMIC6,             DIGITAL_INPUTS_ENABLE_REG, DIGITAL_MICRO_6_MUTE_MASK,        enum digital6_micro_mute,           pp_value, vp_isSet)

    /* Analog gains */
    AUDIO_PARAM(AUDIO_PARAM_MIC1_CONFIG,       ANALOG_MIC_1_GAIN_REG, MIC_1_SINGLE_ENDED_CONF_MASK,        enum single_or_differential,        pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_AMIC1_GAIN,        ANALOG_MIC_1_GAIN_REG, MIC_1_GAIN_MASK,                    enum analog_gain,                   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_MIC2_CONFIG,       ANALOG_MIC_2_GAIN_REG, MIC_2_SINGLE_ENDED_CONF_MASK,       enum single_or_differential,        pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_AMIC2_GAIN,        ANALOG_MIC_2_GAIN_REG, MIC_2_GAIN_MASK,                    enum analog_gain,                   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_LINL_GAIN,         RIGHT_LINE_IN_GAIN_REG, LEFT_LINE_ANALOG_GAIN_MASK,        enum line_in_gain,                  pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_LINR_GAIN,         RIGHT_LINE_IN_GAIN_REG, RIGHT_LINE_ANALOG_GAIN_MASK,      enum line_in_gain,                  pp_value, vp_isSet)

    /* AD gains */
    AUDIO_PARAM(AUDIO_PARAM_AD1_GAIN,          DIGITAL_GAIN_AD1_REG, AD_1_GAIN_MASK,           enum dth_ad_digital_gain,                  pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_AD2_GAIN,          DIGITAL_GAIN_AD2_REG, AD_2_GAIN_MASK,           enum dth_ad_digital_gain,                  pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_AD3_GAIN,          DIGITAL_GAIN_AD3_REG, AD_3_GAIN_MASK,           enum dth_ad_digital_gain,                  pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_AD4_GAIN,          DIGITAL_GAIN_AD4_REG, AD_4_GAIN_MASK,           enum dth_ad_digital_gain,                  pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_AD5_GAIN,          DIGITAL_GAIN_AD5_REG, AD_5_GAIN_MASK,           enum dth_ad_digital_gain,                  pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_AD6_GAIN,          DIGITAL_GAIN_AD6_REG, AD_6_GAIN_MASK,           enum dth_ad_digital_gain,                  pp_value, vp_isSet)

    /* AD mux */
    AUDIO_PARAM(AUDIO_PARAM_SLOT_AD0,          SLOT_ALLOCATION_0_REG, SLOT_OUTPUT_0_MASK,        enum slot_output,                   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_AD1,          SLOT_ALLOCATION_1_REG, SLOT_OUTPUT_1_MASK,        enum slot_output,                   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_AD2,          SLOT_ALLOCATION_2_REG, SLOT_OUTPUT_2_MASK,        enum slot_output,                   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_AD3,          SLOT_ALLOCATION_3_REG, SLOT_OUTPUT_3_MASK,    enum slot_output,                   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_AD4,          SLOT_ALLOCATION_4_REG, SLOT_OUTPUT_4_MASK,    enum slot_output,                   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_AD5,          SLOT_ALLOCATION_5_REG, SLOT_OUTPUT_5_MASK,    enum slot_output,                   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_AD6,          SLOT_ALLOCATION_6_REG, SLOT_OUTPUT_6_MASK,    enum slot_output,                   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SLOT_AD7,          SLOT_ALLOCATION_7_REG, SLOT_OUTPUT_7_MASK,    enum slot_output,                   pp_value, vp_isSet)

    /* Analog loop gain */
    AUDIO_PARAM(AUDIO_PARAM_ANALOG_LOOP_GAIN_L, 0xD18, 0x1F,         enum dth_linein_to_headset,           pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_ANALOG_LOOP_GAIN_R, 0xD19, 0x1F,             enum dth_linein_to_headset,           pp_value, vp_isSet)

    /* Side tone */
    AUDIO_PARAM(AUDIO_PARAM_SIDETONE_GAIN_1,   0xD51, 0x1F,              enum dth_digital_gain_side_tone,        pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SIDETONE_GAIN_2,   0xD52, 0x1F,          enum dth_digital_gain_side_tone,        pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SIDETONE_SRC_1,    DIGITAL_MULTIPLEXERS_MSB_REG, FIR_1_SELECTION_MASK,                enum selection_fir1,                pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_SIDETONE_SRC_2,    DIGITAL_MULTIPLEXERS_MSB_REG, FIR_2_SELECTION_MASK,                enum selection_fir2,                pp_value, vp_isSet)

    /* USB */
    AUDIO_PARAM(AUDIO_PARAM_USB_INTERFACE,  0xD74, 0xFF,         enum dth_usb_interface, pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_USB_MIC,    0xD75, 0x30,         enum dth_usb_mic,   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_USB_GAIN_L,     0xD73, 0xF0,         enum dth_headset_analog_gains,     pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_USB_GAIN_R,     0xD73, 0x0F,             enum dth_headset_analog_gains,     pp_value, vp_isSet)

    /* INTERNALS ONLY */
    AUDIO_PARAM(AUDIO_PARAM_EAR_DIG_PATH_PWR,  DIGITAL_OUTPUT_ENABLE_REG, EAR_AND_DIGITAL_PATH_POWER_MASK,       enum ear_and_digital_path_power,    pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HSL_PATH_PWR,      DIGITAL_OUTPUT_ENABLE_REG, HEADSET_LEFT_PATH_POWER_MASK,          enum headset_left_path_power,       pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HSR_PATH_PWR,      DIGITAL_OUTPUT_ENABLE_REG, HEADSET_RIGHT_PATH_POWER_MASK,                 enum headset_right_path_power,      pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HFL_PATH_PWR,     DIGITAL_OUTPUT_ENABLE_REG, HF_CLASS_D_LEFT_PATH_POWER_MASK,           enum hf_class_d_left_path_power,    pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HFR_PATH_PWR,      DIGITAL_OUTPUT_ENABLE_REG, HF_CLASS_D_RIGTH_PATH_POWER_MASK,          enum hf_class_d_rigth_path_power,   pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_VIB1_PATH_PWR,     DIGITAL_OUTPUT_ENABLE_REG, VIBRATOR_1_CLASS_D_PATH_POWER_MASK,        enum vibrator_class_d_path_power,  pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_VIB1_CTRL_INT,     CONFIGURATION_CLASS_D_DRIVERS_REG, VIB_1_CONTROLLED_INTERNAL_MASK,      enum vib1,                          pp_value, vp_isSet)

    AUDIO_PARAM(AUDIO_PARAM_HSL_DAC_DRIVER_PWR, ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, HSL_DAC_DRIVER_POWER_MASK,          enum hsl_dac_driver_power,          pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_HSR_DAC_DRIVER_PWR, ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, HSR_DAC_DRIVER_POWER_MASK,      enum hsr_dac_driver_power,          pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_ADC1_PWR,          ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, ADC_1_POWER_MASK,                 enum adc1_power,                    pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_ADC2_PWR,          ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, ADC_2_POWER_MASK,                 enum adc2_power,                    pp_value, vp_isSet)
    AUDIO_PARAM(AUDIO_PARAM_ADC3_PWR,          ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, ADC_3_POWER_MASK,                 enum adc3_power,                    pp_value, vp_isSet)

    AUDIO_PARAM(AUDIO_PARAM_EPWM2TOGPIOSEL,    0xD71, 0x20,                enum adc3_power,                    pp_value, vp_isSet)

    END_AUDIO_PARAM_MAP(vp_param)

    RETURN_ERR();
}

int DthAudio_setParam(uint32_t param, uint32_t value)
{
    DECLARE_ERR();
    Dthaudio_path  dthaudio_listpath[NB_MAX] = {""};

    /* reetrant: check guard */
    if (10 < vg_dthaudio_setParam_guard) {
        /* probable deadlock detected! */
        TAT_FAIL(TAT_ERROR_DEADLOCK);
    }

    vg_dthaudio_setParam_guard++;

    /* special treatment for the above parameters because the value to write doesn't fit with enum values defined in the xml*/
    switch (param) {
    case  AUDIO_PARAM_SLOT_AD0:
    case  AUDIO_PARAM_SLOT_AD1:
    case  AUDIO_PARAM_SLOT_AD2:
    case  AUDIO_PARAM_SLOT_AD3:
    case  AUDIO_PARAM_SLOT_AD4:
    case  AUDIO_PARAM_SLOT_AD5:
    case  AUDIO_PARAM_SLOT_AD6:
    case  AUDIO_PARAM_SLOT_AD7:
        if (value == SLOT_OUT_ZEROS_E) {
            value = DTH_SLOT_OUT_ZEROS;
        }
        break;

    case  AUDIO_PARAM_ANALOG_DIGITAL_SEL:
        if (value == 1) {
            value = 0x1F;
        }
        SYSLOG(LOG_DEBUG, "SET ANALOG or DIGITAL INPUT:%d\n", value);
        break;

    case AUDIO_PARAM_LINEOUT:
        if (value == 1) {
            value = 0x03;    /* Left and right power up*/
        }
        break;

    case AUDIO_PARAM_PDMOUT:
        if (value == 1) {
            value = 0x03;    /* PDM1 and PDM2 power up*/
        }
        break;

    case AUDIO_PARAM_USB_INTERFACE:
        if (value == 0) {
            value = DTH_ANALOG;
        } else if (value == 1) {
            value = DTH_HEADSET;
        } else if (value == 2) {
            value = DTH_CARKIT;
        }
        break;
    }

    /* Write Parameters*/
    TAT_TRY(DthAudio_mapParam(param, &value, 1));

    if (TAT_LAST_OK()) {
        /* additionnal operations go here. */
        switch (param) {
        case AUDIO_PARAM_EAR:

            if (EAR_CLASS_AB_POWER_UP_E == value) {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_EAR_DIG_PATH_PWR, EAR_AND_DIGITAL_PATH_POWER_UP_E));
            } else {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_EAR_DIG_PATH_PWR, EAR_AND_DIGITAL_PATH_POWER_DOWN_E));
            }
            break;

        case AUDIO_PARAM_HSL:

            if (HEADSET_LEFT_POWER_UP_E == value) {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HSL_PATH_PWR, HEADSET_LEFT_PATH_POWER_UP_E));
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HSL_DAC_DRIVER_PWR, HSL_DAC_DRIVER_POWER_UP_E));
            } else {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HSL_PATH_PWR, HEADSET_LEFT_PATH_POWER_DOWN_E));
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HSL_DAC_DRIVER_PWR, HSL_DAC_DRIVER_POWER_DOWN_E));
            }
            break;

        case AUDIO_PARAM_HSR:

            if (HEADSET_RIGHT_POWER_UP_E == value) {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HSR_PATH_PWR, HEADSET_RIGHT_PATH_POWER_UP_E));
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HSR_DAC_DRIVER_PWR, HSR_DAC_DRIVER_POWER_UP_E));
            } else {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HSR_PATH_PWR, HEADSET_RIGHT_PATH_POWER_DOWN_E));
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HSR_DAC_DRIVER_PWR, HSR_DAC_DRIVER_POWER_DOWN_E));
            }
            break;

        case AUDIO_PARAM_HFL:

            if (HF_CLASS_D_LEFT_PATH_POWER_UP_E == value) {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HFL_PATH_PWR, HF_CLASS_D_LEFT_PATH_POWER_UP_E));
            } else {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HFL_PATH_PWR, HF_CLASS_D_LEFT_PATH_POWER_DOWN_E));
            }
            break;

        case AUDIO_PARAM_HFR:

            if (HF_CLASS_D_RIGTH_PATH_POWER_UP_E == value) {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HFR_PATH_PWR, HF_CLASS_D_RIGTH_PATH_POWER_UP_E));
            } else {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HFR_PATH_PWR, HF_CLASS_D_RIGTH_PATH_POWER_DOWN_E));
            }
            break;

        case AUDIO_PARAM_VIB1:

            if (VIBRATOR_1_CLASS_D_PATH_POWER_UP_E == value) {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_VIB1_PATH_PWR, VIBRATOR_1_CLASS_D_PATH_POWER_UP_E));
            } else {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_VIB1_PATH_PWR, VIBRATOR_1_CLASS_D_PATH_POWER_DOWN_E));
            }
            break;

        case AUDIO_PARAM_LINEOUT:
            sprintf(dthaudio_listpath[0], "/DTH/AUDIO/Param/out");

            /*  Left Line-Out driver is enabled (if EnHfL=0 in AnaConf4 register).
                Right Line-Out driver is enabled (if EnHfR=0 in AnaConf4 register).*/
            if (value == 0x03) {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HFL, 0));
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_HFR, 0));
                DthAudio_SendAsyncEventParameters(dthaudio_listpath, VALUE_CHANGED, (mode_t)0);
            }
            break;

        case AUDIO_PARAM_PDMOUT:
            /* need update of Epwm2ToGpioSel*/
            if (value == 03) {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_EPWM2TOGPIOSEL, 0));
            } else {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_EPWM2TOGPIOSEL, 1));
            }
            break;

        case AUDIO_PARAM_CLASSD_CONF:

            TAT_TRY(DthAudio_setParam(AUDIO_PARAM_VIB1_CTRL_INT, value));
            break;

        case AUDIO_PARAM_MIC1:

            if (MICRO_1_POWER_UP_E == value) {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_ADC1_PWR, ADC_1_POWER_UP_E));
            } else {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_ADC1_PWR, ADC_1_POWER_DOWN_E));
            }
            break;

        case AUDIO_PARAM_MIC2:
        case AUDIO_PARAM_LINL:

        {
            uint32_t vl_mic2, vl_linl;
            TAT_TRY(DthAudio_getParam(AUDIO_PARAM_MIC2, &vl_mic2));
            TAT_TRY(DthAudio_getParam(AUDIO_PARAM_LINL, &vl_linl));

            if (TAT_LAST_OK()) {
                if ((vl_mic2 == MICRO_2_POWER_UP_E) || (vl_linl == LINE_IN_LEFT_POWER_UP_E)) {
                    TAT_TRY(DthAudio_setParam(AUDIO_PARAM_ADC2_PWR, ADC_2_POWER_UP_E));
                } else {
                    TAT_TRY(DthAudio_setParam(AUDIO_PARAM_ADC2_PWR, ADC_2_POWER_DOWN_E));
                }
            }
        }
        break;

        case AUDIO_PARAM_LINR:

            if (LINE_IN_RIGHT_POWER_UP_E == value) {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_ADC3_PWR, ADC_3_POWER_UP_E));
            } else {
                TAT_TRY(DthAudio_setParam(AUDIO_PARAM_ADC3_PWR, ADC_3_POWER_DOWN_E));
            }
            break;


        case AUDIO_PARAM_USB_INTERFACE:
            VERIFY_ENUM(value, 2); /* 0 , 1, or 2 only */

            break;

        }

    }

    RETURN_ERR();
}

int DthAudio_getParam(uint32_t param, uint32_t* pp_value)
{
    /* reetrant: check guard */
    if (10 < vg_dthaudio_getParam_guard) {
        /* probable deadlock detected! */
        TAT_FAIL(TAT_ERROR_DEADLOCK);
    }

    vg_dthaudio_getParam_guard++;

    *pp_value = 0;
    int vl_error = DthAudio_mapParam(param, pp_value, 0);

    if (TAT_OK(vl_error)) {
        /* additionnal operations go here. */
        switch (param) {
        case AUDIO_PARAM_ANALOG_DIGITAL_SEL:
            /* supported value are only: 0b11111 or 0b00000*/
            /* and displayed value are only resp 1 or 0*/
            if (*pp_value == 0b11111) {
                *pp_value = 1;
            } else { /* return 0 and put all bit values to 0*/
                *pp_value = 0;
                DthAudio_setParam(AUDIO_PARAM_ANALOG_DIGITAL_SEL, *pp_value);
            }
            SYSLOG(LOG_DEBUG, "GET ANALOG or DIGITAL INPUT:%d\n", *pp_value);

            break;

        case  AUDIO_PARAM_SLOT_AD0:
        case  AUDIO_PARAM_SLOT_AD1:
        case  AUDIO_PARAM_SLOT_AD2:
        case  AUDIO_PARAM_SLOT_AD3:
        case  AUDIO_PARAM_SLOT_AD4:
        case  AUDIO_PARAM_SLOT_AD5:
        case  AUDIO_PARAM_SLOT_AD6:
        case  AUDIO_PARAM_SLOT_AD7:
            if (*pp_value == DTH_SLOT_OUT_ZEROS) { /* value which doesn't match with the enum values of xml file*/
                *pp_value = SLOT_OUT_ZEROS_E;
            }
            break;

        case AUDIO_PARAM_LINEOUT:
            if (*pp_value == 0x03) {
                *pp_value = 1;    /* Left and right power up*/
            }
            break;

        case AUDIO_PARAM_PDMOUT:
            if (*pp_value == 0x03) {
                *pp_value = 1;    /* PDM1 and PDM2 power up*/
            }
            break;

        case AUDIO_PARAM_USB_INTERFACE:
            if (*pp_value == DTH_ANALOG) {
                *pp_value = 0;
            } else if (*pp_value == DTH_HEADSET) {
                *pp_value = 1;
            } else if (*pp_value == DTH_CARKIT) {
                *pp_value = 2;
            }
            break;

        }
    }

    return vl_error;
}



/*  Get the offset of a register field from the field mask*/
int DthAudio_GetOffsetFromMask(uint8_t mask, uint8_t* poffset)
{
    uint8_t i = 0;
    uint8_t startbitmask = 0x01;
    uint8_t end = 0;
    DECLARE_ERR();

    while (end == 0) {
        if ((mask&startbitmask) == 1) {
            end = 1;
        } else {
            i++;
            mask = mask >> 1;
            /*mask!=0)*/
        }
    }

    *poffset = i;
    RETURN_ERR();
}



/*  Write into an AB register*/
int DthAudio_WriteInReg(uint16_t addr, uint8_t mask, uint8_t* pp_value)
{
    uint8_t valR = 0, valW = 0, subvalW = *pp_value;
    uint8_t offset = 0;
    DECLARE_ERR();

    if ((addr != 0)&(mask != 0)) {
        DthAudio_GetOffsetFromMask(mask, &offset);

        SYSLOG(LOG_DEBUG, "call of abxxxx_read ... ");
        TAT_TRY(abxxxx_read(addr, &valR));

        if (TAT_LAST_OK()) {
            valW = valR & ~mask;
            valW = valW | (subvalW << offset);
            TAT_TRY(abxxxx_write(addr, valW));
            SYSLOG(LOG_DEBUG, "abxxxx_write %2X %2X\n", addr, valW);
        } else
            SYSLOG(LOG_ERR, "abxxxx_write error");

    }

    RETURN_ERR();
}

/*  Read into an AB register*/
int DthAudio_ReadInReg(uint16_t addr, uint8_t mask, uint8_t* pp_value)
{
    uint8_t valR = 0;
    uint8_t offset = 0;
    DECLARE_ERR();

    if ((addr != 0)&(mask != 0)) {
        DthAudio_GetOffsetFromMask(mask, &offset);

        SYSLOG(LOG_DEBUG, "call of abxxxx_read ... ");
        TAT_TRY(abxxxx_read(addr, &valR));

        if (TAT_LAST_OK()) {
            *pp_value = ((valR & mask) >> offset);;
            SYSLOG(LOG_DEBUG, "abxxxx_read %2X %2X\n", addr, valR);
        } else
            SYSLOG(LOG_ERR, "abxxxx_read error");
    }

    RETURN_ERR();
}


/*  Initiate an asynchronous event for all audio parameters*/
int DthAudio_SendAsyncEventParameters(Dthaudio_path* listpath, unsigned char type, mode_t mode)
{
    DECLARE_ERR();
    int status = 1234;
    char command[100] = "";
//  char command2[100]="";
    int i = 0;

    while (strlen(listpath[i])) {
        if (type == PERM_CHANGED) {

//          pid_t pid=1, npid=0;
//          FILE *file;

//          snprintf(command, 100, "chmod %o /mnt/9p/%s/*/value&", mode, listpath[i]);
//          status=system(command);

            snprintf(command, 100, "find /mnt/9p/%s -type f -name 'value' -exec chmod %o {} \\; &", listpath[i], mode);
            status = system(command);

            /*Wait for the end of the process launched above*/
            //system("rm -f /tmp/dthaudiopid.txt");
            // while (pid)
            // {
            // snprintf(command2,100,"pgrep find > /tmp/dthaudiopid.txt");
            // system(command2);
            // file= fopen("/tmp/dthaudiopid.txt","r");
            // if(file != NULL)
            // {
            // fscanf(file,"%d\n",&pid);
            // fclose(file);
            // }
            // if (pid==0) printf ("invalid pid!\n");
            // else
            // printf (" pid=%d\n",pid );

            // }

            //system("rm -f /tmp/dthaudiopid.txt");
            // if (pid>0)
            // {
            // npid=waitpid(pid, &status,0);
            // printf ("waitpid %d returns %d %d", pid, npid, status);
            // }

            // else
            // printf ("invalid pid!\n");

#if 0
            DIR * rep = NULL;
            char FilePath[100] = "";
            char DirPath[100] = "";

            snprintf(DirPath, 100, "/mnt/9p%s", listpath[i]);
            DIR * rep = opendir(DirPath);
            printf("opendir: %s\n", DirPath);

            if (rep != NULL) {
                struct dirent * ent;
                printf("readdir: while...\n");

                while ((ent = readdir(rep)) != NULL) {
                    if (ent->d_type == DT_DIR) {
                        snprintf(FilePath, 100, "%s/%s/value", listpath[i], ent->d_name);
                        /*change mode for value file */
                        status = chmod(FilePath, mode);
                        printf("change mode %o %s, returns %d\n", mode, FilePath, status);
                    } else
                        printf("readdir: %s\n", ent->d_name);
                }
                closedir(rep);
            }
#endif
        }

        SYSLOG(LOG_INFO, "libtatasync_msg_send (%d, %s)", type,  listpath[i]);
        TAT_TRY(libtatasync_msg_send(type, listpath[i]));

        i++;
    }

    RETURN_ERR();
}

int DthAudio_GpioConfig(int AddressDir, int AddressVal, int AddressPud, int Mask, uint8_t Direction, uint8_t Value, uint8_t Pud)
{
    DECLARE_ERR();

    /* 1- Set GPIO Direction */

	TAT_TRY(DthAudio_WriteInReg(AddressDir, Mask , &Direction))

    if (TAT_LAST_OK()) {
        /* 2- Set GPIO value */
        TAT_TRY(DthAudio_WriteInReg(AddressVal, Mask , &Value))
        if (TAT_LAST_OK()) {
            /* 3- Set GPIO Pull Down value */
            TAT_TRY(DthAudio_WriteInReg(AddressPud, Mask , &Pud))
            if (TAT_LAST_OK()) {
                SYSLOG(LOG_INFO, "Gpio configured ( @=%x, %x, %x)", AddressDir,  AddressVal,  AddressPud);
            } else {
                SYSLOG(LOG_ERR, "Error Gpio configuration:%d ( @=%x)", LAST_ERR, AddressPud);
            }

        } else {
            SYSLOG(LOG_ERR, "Error Gpio configuration:%d ( @=%x)", LAST_ERR, AddressVal);
        }
    } else {
        SYSLOG(LOG_ERR, "Error Gpio configuration:%d ( @=%x)", LAST_ERR, AddressDir);
    }

	RETURN_ERR();
}
