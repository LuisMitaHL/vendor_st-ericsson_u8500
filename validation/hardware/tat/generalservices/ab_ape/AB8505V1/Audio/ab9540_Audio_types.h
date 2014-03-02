/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Audio/Linux/ab9540_Audio_types.h
 * 
 *
 * Generated on the 19/12/2011 11:27 by the 'super-cool' code generator 
 *
 * Generator written in Common Lisp, created by  Remi PRUD'HOMME - STEricsson 
 * with the help of : 
 *
 ***************************************************************************
 *  Copyright STEricsson  2011
 *
 *  Reference document : User Manual ref : CD00291561 Rev 3, May 2011
 *
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_AUDIO_LINUX_AB9540_AUDIO_TYPES_H
#define C__LISP_PRODUCTION_AB9540_AB9540_AUDIO_LINUX_AB9540_AUDIO_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif


//  
//  0: All the audio analog parts are in power down 
//  1: All the audio analog parts are in power up.
//  Register PowerUpControl 0x0D00, Bits 3:3, typedef analog_power_up
//   
//  
#define ANALOG_POWER_UP_MASK 0x8U
#define ANALOG_POWER_UP_OFFSET 3
#define ANALOG_POWER_UP_STOP_BIT 3
#define ANALOG_POWER_UP_START_BIT 3
#define ANALOG_POWER_UP_WIDTH 1

#define POWER_UP_CONTROL_REG 0xD00

typedef enum analog_power_up {
    ANALOG_POWER_DOWN_E,
    ANALOG_POWER_UP_E
} ANALOG_POWER_UP_T ;
#define ANALOG_POWER_UP_PARAM_MASK  0x8



//  
//  0: The audio feature is in power down 
//  1: The audio feature is in power up.
//  Register PowerUpControl 0x0D00, Bits 7:7, typedef digital_power_up
//   
//  
#define DIGITAL_POWER_UP_MASK 0x80U
#define DIGITAL_POWER_UP_OFFSET 7
#define DIGITAL_POWER_UP_STOP_BIT 7
#define DIGITAL_POWER_UP_START_BIT 7
#define DIGITAL_POWER_UP_WIDTH 1


typedef enum digital_power_up {
    DIGITAL_POWER_DOWN_E,
    DIGITAL_POWER_UP_E
} DIGITAL_POWER_UP_T ;
#define DIGITAL_POWER_UP_PARAM_MASK  0x80



//  
//  0: Normal operation 
//  1: Sets all the audio registers to their default state, including itself.
//  Register SoftwareReset 0x0D01, Bits 7:7, typedef audio_software_reset
//   
//  
#define AUDIO_SOFTWARE_RESET_MASK 0x80U
#define AUDIO_SOFTWARE_RESET_OFFSET 7
#define AUDIO_SOFTWARE_RESET_STOP_BIT 7
#define AUDIO_SOFTWARE_RESET_START_BIT 7
#define AUDIO_SOFTWARE_RESET_WIDTH 1

#define SOFTWARE_RESET_REG 0xD01

typedef enum audio_software_reset {
    AUDIO_SOFTWARE_UN_RESET_E,
    AUDIO_SOFTWARE_RESET_E
} AUDIO_SOFTWARE_RESET_T ;
#define AUDIO_SOFTWARE_RESET_PARAM_MASK  0x80



//  
//  0: AD5/7 and AD6/8 paths are disabled 
//  1: AD5/7 and AD6/8 paths are enabled.
//  Register DigitalChannelsEnable 0x0D02, Bits 3:3, typedef ad5678_enable
//   
//  
#define AD_5768_DIGITAL_AUDIO_PATHS_MASK 0x8U
#define AD_5768_DIGITAL_AUDIO_PATHS_OFFSET 3
#define AD_5768_DIGITAL_AUDIO_PATHS_STOP_BIT 3
#define AD_5768_DIGITAL_AUDIO_PATHS_START_BIT 3
#define AD_5768_DIGITAL_AUDIO_PATHS_WIDTH 1

#define DIGITAL_CHANNELS_ENABLE_REG 0xD02

typedef enum ad5678_enable {
    AD_5678_DISABLE_E,
    AD_5678_ENABLE_E
} AD_5678_ENABLE_T ;
#define AD_5768_DIGITAL_AUDIO_PATHS_PARAM_MASK  0x8



//  
//  0: AD3 and AD4 paths are disabled 
//  1: AD3 and AD4 paths are enabled.
//  Register DigitalChannelsEnable 0x0D02, Bits 5:5, typedef ad34_enable
//   
//  
#define AD_34_DIGITAL_AUDIO_PATHS_MASK 0x20U
#define AD_34_DIGITAL_AUDIO_PATHS_OFFSET 5
#define AD_34_DIGITAL_AUDIO_PATHS_STOP_BIT 5
#define AD_34_DIGITAL_AUDIO_PATHS_START_BIT 5
#define AD_34_DIGITAL_AUDIO_PATHS_WIDTH 1


typedef enum ad34_enable {
    AD_34_DISABLE_E,
    AD_34_ENABLE_E
} AD_34_ENABLE_T ;
#define AD_34_DIGITAL_AUDIO_PATHS_PARAM_MASK  0x20



//  
//  0: AD1 and AD2 paths are disabled 
//  1: AD1 and AD2 paths are enabled.
//  Register DigitalChannelsEnable 0x0D02, Bits 7:7, typedef ad12_enable
//   
//  
#define AD_12_DIGITAL_AUDIO_PATHS_MASK 0x80U
#define AD_12_DIGITAL_AUDIO_PATHS_OFFSET 7
#define AD_12_DIGITAL_AUDIO_PATHS_STOP_BIT 7
#define AD_12_DIGITAL_AUDIO_PATHS_START_BIT 7
#define AD_12_DIGITAL_AUDIO_PATHS_WIDTH 1


typedef enum ad12_enable {
    AD_12_DISABLE_E,
    AD_12_ENABLE_E
} AD_12_ENABLE_T ;
#define AD_12_DIGITAL_AUDIO_PATHS_PARAM_MASK  0x80



//  
//  0: High pass filter for Earpiece is disabled 
//  1: High pass filter for Earpiece is enabled.
//  Register DigitalDaAudioPaths 0x0D03, Bits 1:1, typedef high_pass_filter_enable
//   
//  
#define HIGH_PASS_FILTER_MASK 0x2U
#define HIGH_PASS_FILTER_OFFSET 1
#define HIGH_PASS_FILTER_STOP_BIT 1
#define HIGH_PASS_FILTER_START_BIT 1
#define HIGH_PASS_FILTER_WIDTH 1

#define DIGITAL_DA_AUDIO_PATHS_REG 0xD03

typedef enum high_pass_filter_enable {
    HIGH_PASS_FILTER_DISABLE_E,
    HIGH_PASS_FILTER_ENABLE_E
} HIGH_PASS_FILTER_ENABLE_T ;
#define HIGH_PASS_FILTER_PARAM_MASK  0x2



//  
//  0: DA6 path is disabled 
//  1: DA6 path is enabled.
//  Register DigitalDaAudioPaths 0x0D03, Bits 2:2, typedef da6_enable
//   
//  
#define DA_6_DIGITAL_AUDIO_PATHS_MASK 0x4U
#define DA_6_DIGITAL_AUDIO_PATHS_OFFSET 2
#define DA_6_DIGITAL_AUDIO_PATHS_STOP_BIT 2
#define DA_6_DIGITAL_AUDIO_PATHS_START_BIT 2
#define DA_6_DIGITAL_AUDIO_PATHS_WIDTH 1


typedef enum da6_enable {
    DA_6_DISABLE_E,
    DA_6_ENABLE_E
} DA_6_ENABLE_T ;
#define DA_6_DIGITAL_AUDIO_PATHS_PARAM_MASK  0x4



//  
//  0: DA5 path is disabled 
//  1: DA5 path is enabled.
//  Register DigitalDaAudioPaths 0x0D03, Bits 3:3, typedef da5_enable
//   
//  
#define DA_5_DIGITAL_AUDIO_PATHS_MASK 0x8U
#define DA_5_DIGITAL_AUDIO_PATHS_OFFSET 3
#define DA_5_DIGITAL_AUDIO_PATHS_STOP_BIT 3
#define DA_5_DIGITAL_AUDIO_PATHS_START_BIT 3
#define DA_5_DIGITAL_AUDIO_PATHS_WIDTH 1


typedef enum da5_enable {
    DA_5_DISABLE_E,
    DA_5_ENABLE_E
} DA_5_ENABLE_T ;
#define DA_5_DIGITAL_AUDIO_PATHS_PARAM_MASK  0x8



//  
//  0: DA4 path is disabled 
//  1: DA4 path is enabled.
//  Register DigitalDaAudioPaths 0x0D03, Bits 4:4, typedef da4_enable
//   
//  
#define DA_4_DIGITAL_AUDIO_PATHS_MASK 0x10U
#define DA_4_DIGITAL_AUDIO_PATHS_OFFSET 4
#define DA_4_DIGITAL_AUDIO_PATHS_STOP_BIT 4
#define DA_4_DIGITAL_AUDIO_PATHS_START_BIT 4
#define DA_4_DIGITAL_AUDIO_PATHS_WIDTH 1


typedef enum da4_enable {
    DA_4_DISABLE_E,
    DA_4_ENABLE_E
} DA_4_ENABLE_T ;
#define DA_4_DIGITAL_AUDIO_PATHS_PARAM_MASK  0x10



//  
//  0: DA3 path is disabled 
//  1: DA3 path is enabled.
//  Register DigitalDaAudioPaths 0x0D03, Bits 5:5, typedef da3_enable
//   
//  
#define DA_3_DIGITAL_AUDIO_PATHS_MASK 0x20U
#define DA_3_DIGITAL_AUDIO_PATHS_OFFSET 5
#define DA_3_DIGITAL_AUDIO_PATHS_STOP_BIT 5
#define DA_3_DIGITAL_AUDIO_PATHS_START_BIT 5
#define DA_3_DIGITAL_AUDIO_PATHS_WIDTH 1


typedef enum da3_enable {
    DA_3_DISABLE_E,
    DA_3_ENABLE_E
} DA_3_ENABLE_T ;
#define DA_3_DIGITAL_AUDIO_PATHS_PARAM_MASK  0x20



//  
//  0: DA2 path is disabled 
//  1: DA2 path is enabled.
//  Register DigitalDaAudioPaths 0x0D03, Bits 6:6, typedef da2_enable
//   
//  
#define DA_2_DIGITAL_AUDIO_PATHS_MASK 0x40U
#define DA_2_DIGITAL_AUDIO_PATHS_OFFSET 6
#define DA_2_DIGITAL_AUDIO_PATHS_STOP_BIT 6
#define DA_2_DIGITAL_AUDIO_PATHS_START_BIT 6
#define DA_2_DIGITAL_AUDIO_PATHS_WIDTH 1


typedef enum da2_enable {
    DA_2_DISABLE_E,
    DA_2_ENABLE_E
} DA_2_ENABLE_T ;
#define DA_2_DIGITAL_AUDIO_PATHS_PARAM_MASK  0x40



//  
//  0: DA1 path is disabled 
//  1: DA1 path is enabled.
//  Register DigitalDaAudioPaths 0x0D03, Bits 7:7, typedef da1_enable
//   
//  
#define DA_1_DIGITAL_AUDIO_PATHS_MASK 0x80U
#define DA_1_DIGITAL_AUDIO_PATHS_OFFSET 7
#define DA_1_DIGITAL_AUDIO_PATHS_STOP_BIT 7
#define DA_1_DIGITAL_AUDIO_PATHS_START_BIT 7
#define DA_1_DIGITAL_AUDIO_PATHS_WIDTH 1


typedef enum da1_enable {
    DA_1_DISABLE_E,
    DA_1_ENABLE_E
} DA_1_ENABLE_T ;
#define DA_1_DIGITAL_AUDIO_PATHS_PARAM_MASK  0x80



//  
//  0: Normal Operation 
//  1: Ear DAC driver in Low Power.
//  Register AnalogDMicControls 0x0D04, Bits 0:0, typedef ear_driver_enable
//   
//  
#define EAR_DRIVER_LOW_POWER_MASK 0x1U
#define EAR_DRIVER_LOW_POWER_OFFSET 0
#define EAR_DRIVER_LOW_POWER_STOP_BIT 0
#define EAR_DRIVER_LOW_POWER_START_BIT 0
#define EAR_DRIVER_LOW_POWER_WIDTH 1

#define ANALOG_D_MIC_CONTROLS_REG 0xD04

typedef enum ear_driver_enable {
    EAR_DRIVER_DISABLE_E,
    EAR_DRIVER_ENABLE_E
} EAR_DRIVER_ENABLE_T ;
#define EAR_DRIVER_LOW_POWER_PARAM_MASK  0x1



//  
//  Offset cancellation for headset driver..
//  Register AnalogDMicControls 0x0D04, Bits 1:1, typedef headset_offset_cancelation
//   
//  
#define HEADSET_OFFSET_CANCELATION_MASK 0x2U
#define HEADSET_OFFSET_CANCELATION_OFFSET 1
#define HEADSET_OFFSET_CANCELATION_STOP_BIT 1
#define HEADSET_OFFSET_CANCELATION_START_BIT 1
#define HEADSET_OFFSET_CANCELATION_WIDTH 1


typedef enum headset_offset_cancelation {
    HEADSET_OFFSET_CANCELATION_OFF_E,
    HEADSET_OFFSET_CANCELATION_ON_E
} HEADSET_OFFSET_CANCELATION_T ;
#define HEADSET_OFFSET_CANCELATION_PARAM_MASK  0x2



//  
//  Ear Selection Common Mode.
//  Register AnalogDMicControls 0x0D04, Bits 2:3, typedef ear_selection
//   
//  
#define EAR_SELECTION_MASK 0xCU
#define EAR_SELECTION_OFFSET 3
#define EAR_SELECTION_STOP_BIT 3
#define EAR_SELECTION_START_BIT 2
#define EAR_SELECTION_WIDTH 2

#define  COMMON_MODE_EAR_DRIVER_0_95V 0
#define  COMMON_MODE_EAR_DRIVER_1_1V 1
#define  COMMON_MODE_EAR_DRIVER_1_27V 2
#define  COMMON_MODE_EAR_DRIVER_1_58V 3

typedef enum ear_selection {
    COMMON_MODE_EAR_DRIVER_0_95V_E,
    COMMON_MODE_EAR_DRIVER_1_1V_E,
    COMMON_MODE_EAR_DRIVER_1_27V_E,
    COMMON_MODE_EAR_DRIVER_1_58V_E
} EAR_SELECTION_T ;
#define EAR_SELECTION_PARAM_MASK  0xC



//  
//  Low Power Ear.
//  Register AnalogDMicControls 0x0D04, Bits 4:4, typedef low_power_ear
//   
//  
#define LOW_POWER_EAR_MASK 0x10U
#define LOW_POWER_EAR_OFFSET 4
#define LOW_POWER_EAR_STOP_BIT 4
#define LOW_POWER_EAR_START_BIT 4
#define LOW_POWER_EAR_WIDTH 1


typedef enum low_power_ear {
    NORMAL_OPERATION_EAR_E,
    LOW_POWER_EAR_E
} LOW_POWER_EAR_T ;
#define LOW_POWER_EAR_PARAM_MASK  0x10



//  
//  Low Power DAC headset.
//  Register AnalogDMicControls 0x0D04, Bits 5:6, typedef low_power_dac_headset
//   
//  
#define LOW_POWER_DAC_HEADSET_MASK 0x60U
#define LOW_POWER_DAC_HEADSET_OFFSET 6
#define LOW_POWER_DAC_HEADSET_STOP_BIT 6
#define LOW_POWER_DAC_HEADSET_START_BIT 5
#define LOW_POWER_DAC_HEADSET_WIDTH 2

#define  HS_DAC_NORMAL_OPERATION 0
#define  HS_DAC_DRIVERS_LOWPOWER 1
#define  HS_DAC_LOWPOWER 2
#define  HS_DAC_DAC_DRIVERS_LOWPOWER 3

typedef enum low_power_dac_headset {
    HS_DAC_NORMAL_OPERATION_E,
    HS_DAC_DRIVERS_LOWPOWER_E,
    HS_DAC_LOWPOWER_E,
    HS_DAC_DAC_DRIVERS_LOWPOWER_E
} LOW_POWER_DAC_HEADSET_T ;
#define LOW_POWER_DAC_HEADSET_PARAM_MASK  0x60



//  
//  Low Power Headset.
//  Register AnalogDMicControls 0x0D04, Bits 7:7, typedef audio_low_power_mode
//   
//  
#define LOW_POWER_HEADSET_MASK 0x80U
#define LOW_POWER_HEADSET_OFFSET 7
#define LOW_POWER_HEADSET_STOP_BIT 7
#define LOW_POWER_HEADSET_START_BIT 7
#define LOW_POWER_HEADSET_WIDTH 1


typedef enum audio_low_power_mode {
    NORMAL_OPERATION_E,
    LOW_POWER_E
} AUDIO_LOW_POWER_MODE_T ;
#define LOW_POWER_HEADSET_PARAM_MASK  0x80



//  
//  Analog line in Right is active.
//  Register LineInConf 0x0D05, Bits 0:0, typedef analog_line_in_right_mute
//   
//  
#define ANALOG_LINE_IN_RIGHT_MUTE_MASK 0x1U
#define ANALOG_LINE_IN_RIGHT_MUTE_OFFSET 0
#define ANALOG_LINE_IN_RIGHT_MUTE_STOP_BIT 0
#define ANALOG_LINE_IN_RIGHT_MUTE_START_BIT 0
#define ANALOG_LINE_IN_RIGHT_MUTE_WIDTH 1

#define LINE_IN_CONF_REG 0xD05

typedef enum analog_line_in_right_mute {
    ANALOG_LINE_IN_RIGHT_UNMUTE_E,
    ANALOG_LINE_IN_RIGHT_MUTE_E
} ANALOG_LINE_IN_RIGHT_MUTE_T ;
#define ANALOG_LINE_IN_RIGHT_MUTE_PARAM_MASK  0x1



//  
//  Analog line in Left is unmuted.
//  Register LineInConf 0x0D05, Bits 1:1, typedef analog_line_in_left_mute
//   
//  
#define ANALOG_LINE_IN_LEFT_MUTE_MASK 0x2U
#define ANALOG_LINE_IN_LEFT_MUTE_OFFSET 1
#define ANALOG_LINE_IN_LEFT_MUTE_STOP_BIT 1
#define ANALOG_LINE_IN_LEFT_MUTE_START_BIT 1
#define ANALOG_LINE_IN_LEFT_MUTE_WIDTH 1


typedef enum analog_line_in_left_mute {
    ANALOG_LINE_IN_LEFT_UNMUTE_E,
    ANALOG_LINE_IN_LEFT_MUTE_E
} ANALOG_LINE_IN_LEFT_MUTE_T ;
#define ANALOG_LINE_IN_LEFT_MUTE_PARAM_MASK  0x2



//  
//  Analog micropnone MIC2 is active.
//  Register LineInConf 0x0D05, Bits 2:2, typedef analog_micro2_mute
//   
//  
#define ANALOG_MICRO_2_MUTE_MASK 0x4U
#define ANALOG_MICRO_2_MUTE_OFFSET 2
#define ANALOG_MICRO_2_MUTE_STOP_BIT 2
#define ANALOG_MICRO_2_MUTE_START_BIT 2
#define ANALOG_MICRO_2_MUTE_WIDTH 1


typedef enum analog_micro2_mute {
    ANALOG_MICRO_2_UNMUTE_E,
    ANALOG_MICRO_2_MUTE_E
} ANALOG_MICRO_2_MUTE_T ;
#define ANALOG_MICRO_2_MUTE_PARAM_MASK  0x4



//  
//  Analog micropnone MIC1A/B is active.
//  Register LineInConf 0x0D05, Bits 3:3, typedef analog_micro1_mute
//   
//  
#define ANALOG_MICRO_1_MUTE_MASK 0x8U
#define ANALOG_MICRO_1_MUTE_OFFSET 3
#define ANALOG_MICRO_1_MUTE_STOP_BIT 3
#define ANALOG_MICRO_1_MUTE_START_BIT 3
#define ANALOG_MICRO_1_MUTE_WIDTH 1


typedef enum analog_micro1_mute {
    ANALOG_MICRO_1_UNMUTE_E,
    ANALOG_MICRO_1_MUTE_E
} ANALOG_MICRO_1_MUTE_T ;
#define ANALOG_MICRO_1_MUTE_PARAM_MASK  0x8



//  
//  0: Line-in right input is in power down 
//  1: Line-in left input is in power up.
//  Register LineInConf 0x0D05, Bits 4:4, typedef line_in_right_power
//   
//  
#define LINE_IN_RIGHT_POWER_MASK 0x10U
#define LINE_IN_RIGHT_POWER_OFFSET 4
#define LINE_IN_RIGHT_POWER_STOP_BIT 4
#define LINE_IN_RIGHT_POWER_START_BIT 4
#define LINE_IN_RIGHT_POWER_WIDTH 1


typedef enum line_in_right_power {
    LINE_IN_RIGHT_POWER_DOWN_E,
    LINE_IN_RIGHT_POWER_UP_E
} LINE_IN_RIGHT_POWER_T ;
#define LINE_IN_RIGHT_POWER_PARAM_MASK  0x10



//  
//  0: Line-in left input is in power down 
//  1: Line-in left input is in power up.
//  Register LineInConf 0x0D05, Bits 5:5, typedef line_in_left_power
//   
//  
#define LINE_IN_LEFT_POWER_MASK 0x20U
#define LINE_IN_LEFT_POWER_OFFSET 5
#define LINE_IN_LEFT_POWER_STOP_BIT 5
#define LINE_IN_LEFT_POWER_START_BIT 5
#define LINE_IN_LEFT_POWER_WIDTH 1


typedef enum line_in_left_power {
    LINE_IN_LEFT_POWER_DOWN_E,
    LINE_IN_LEFT_POWER_UP_E
} LINE_IN_LEFT_POWER_T ;
#define LINE_IN_LEFT_POWER_PARAM_MASK  0x20



//  
//  0: Mic2 microphone is in power down 
//  1: Mic2 microphone is in power up.
//  Register LineInConf 0x0D05, Bits 6:6, typedef micro2_power
//   
//  
#define MICRO_2_POWER_MASK 0x40U
#define MICRO_2_POWER_OFFSET 6
#define MICRO_2_POWER_STOP_BIT 6
#define MICRO_2_POWER_START_BIT 6
#define MICRO_2_POWER_WIDTH 1


typedef enum micro2_power {
    MICRO_2_POWER_DOWN_E,
    MICRO_2_POWER_UP_E
} MICRO_2_POWER_T ;
#define MICRO_2_POWER_PARAM_MASK  0x40



//  
//  Micro 1a and 1b Enable (power up).
//  Register LineInConf 0x0D05, Bits 7:7, typedef micro1_power
//   
//  
#define MICRO_1_POWER_MASK 0x80U
#define MICRO_1_POWER_OFFSET 7
#define MICRO_1_POWER_STOP_BIT 7
#define MICRO_1_POWER_START_BIT 7
#define MICRO_1_POWER_WIDTH 1


typedef enum micro1_power {
    MICRO_1_POWER_DOWN_E,
    MICRO_1_POWER_UP_E
} MICRO_1_POWER_T ;
#define MICRO_1_POWER_PARAM_MASK  0x80



//  
//  Approximate Time between each 1dB step Gain Change in digital gain controls.
//  Register DigitalInputsEnable 0x0D06, Bits 0:1, typedef fade_speed
//   
//  
#define FADE_SPEED_MASK 0x3U
#define FADE_SPEED_OFFSET 1
#define FADE_SPEED_STOP_BIT 1
#define FADE_SPEED_START_BIT 0
#define FADE_SPEED_WIDTH 2

#define DIGITAL_INPUTS_ENABLE_REG 0xD06
#define  TIME_2MS 0
#define  TIME_0_5_MS 1
#define  TIME_10_1_MS 2
#define  TIME_55MS 3

typedef enum fade_speed {
    TIME_2MS_E,
    TIME_0_5_MS_E,
    TIME_10_1_MS_E,
    TIME_55MS_E
} FADE_SPEED_T ;
#define FADE_SPEED_PARAM_MASK  0x3



//  
//  1: Digital Micro 6 input is active and Dmic12Clk is running, 
//  0: Digital Micro 6 input is muted.
//  Register DigitalInputsEnable 0x0D06, Bits 2:2, typedef digital6_micro_mute
//   
//  
#define DIGITAL_MICRO_6_MUTE_MASK 0x4U
#define DIGITAL_MICRO_6_MUTE_OFFSET 2
#define DIGITAL_MICRO_6_MUTE_STOP_BIT 2
#define DIGITAL_MICRO_6_MUTE_START_BIT 2
#define DIGITAL_MICRO_6_MUTE_WIDTH 1


typedef enum digital6_micro_mute {
    DIGITAL_6_MICRO_MUTE_E,
    DIGITAL_6_MICRO_UNMUTE_E
} DIGITAL_6_MICRO_MUTE_T ;
#define DIGITAL_MICRO_6_MUTE_PARAM_MASK  0x4



//  
//  1: Digital Micro 5 input is active and Dmic12Clk is running, 
//  0: Digital Micro 5 input is muted.
//  Register DigitalInputsEnable 0x0D06, Bits 3:3, typedef digital5_micro_mute
//   
//  
#define DIGITAL_MICRO_5_MUTE_MASK 0x8U
#define DIGITAL_MICRO_5_MUTE_OFFSET 3
#define DIGITAL_MICRO_5_MUTE_STOP_BIT 3
#define DIGITAL_MICRO_5_MUTE_START_BIT 3
#define DIGITAL_MICRO_5_MUTE_WIDTH 1


typedef enum digital5_micro_mute {
    DIGITAL_5_MICRO_MUTE_E,
    DIGITAL_5_MICRO_UNMUTE_E
} DIGITAL_5_MICRO_MUTE_T ;
#define DIGITAL_MICRO_5_MUTE_PARAM_MASK  0x8



//  
//  1: Digital Micro 4 input is active and Dmic12Clk is running, 
//  0: Digital Micro 4 input is muted.
//  Register DigitalInputsEnable 0x0D06, Bits 4:4, typedef digital4_micro_mute
//   
//  
#define DIGITAL_MICRO_4_MUTE_MASK 0x10U
#define DIGITAL_MICRO_4_MUTE_OFFSET 4
#define DIGITAL_MICRO_4_MUTE_STOP_BIT 4
#define DIGITAL_MICRO_4_MUTE_START_BIT 4
#define DIGITAL_MICRO_4_MUTE_WIDTH 1


typedef enum digital4_micro_mute {
    DIGITAL_4_MICRO_MUTE_E,
    DIGITAL_4_MICRO_UNMUTE_E
} DIGITAL_4_MICRO_MUTE_T ;
#define DIGITAL_MICRO_4_MUTE_PARAM_MASK  0x10



//  
//  1: Digital Micro 3 input is active and Dmic12Clk is running, 
//  0: Digital Micro 3 input is muted.
//  Register DigitalInputsEnable 0x0D06, Bits 5:5, typedef digital3_micro_mute
//   
//  
#define DIGITAL_MICRO_3_MUTE_MASK 0x20U
#define DIGITAL_MICRO_3_MUTE_OFFSET 5
#define DIGITAL_MICRO_3_MUTE_STOP_BIT 5
#define DIGITAL_MICRO_3_MUTE_START_BIT 5
#define DIGITAL_MICRO_3_MUTE_WIDTH 1


typedef enum digital3_micro_mute {
    DIGITAL_3_MICRO_MUTE_E,
    DIGITAL_3_MICRO_UNMUTE_E
} DIGITAL_3_MICRO_MUTE_T ;
#define DIGITAL_MICRO_3_MUTE_PARAM_MASK  0x20



//  
//  1: Digital Micro 2 input is active and Dmic12Clk is running, 
//  0: Digital Micro 2 input is muted.
//  Register DigitalInputsEnable 0x0D06, Bits 6:6, typedef digital2_micro_mute
//   
//  
#define DIGITAL_MICRO_2_MUTE_MASK 0x40U
#define DIGITAL_MICRO_2_MUTE_OFFSET 6
#define DIGITAL_MICRO_2_MUTE_STOP_BIT 6
#define DIGITAL_MICRO_2_MUTE_START_BIT 6
#define DIGITAL_MICRO_2_MUTE_WIDTH 1


typedef enum digital2_micro_mute {
    DIGITAL_2_MICRO_MUTE_E,
    DIGITAL_2_MICRO_UNMUTE_E
} DIGITAL_2_MICRO_MUTE_T ;
#define DIGITAL_MICRO_2_MUTE_PARAM_MASK  0x40



//  
//  1: Digital Micro 1 input is active and Dmic12Clk is running, 
//  0: Digital Micro 1 input is muted.
//  Register DigitalInputsEnable 0x0D06, Bits 7:7, typedef digital1_micro_mute
//   
//  
#define DIGITAL_MICRO_1_MUTE_MASK 0x80U
#define DIGITAL_MICRO_1_MUTE_OFFSET 7
#define DIGITAL_MICRO_1_MUTE_STOP_BIT 7
#define DIGITAL_MICRO_1_MUTE_START_BIT 7
#define DIGITAL_MICRO_1_MUTE_WIDTH 1


typedef enum digital1_micro_mute {
    DIGITAL_1_MICRO_MUTE_E,
    DIGITAL_1_MICRO_UNMUTE_E
} DIGITAL_1_MICRO_MUTE_T ;
#define DIGITAL_MICRO_1_MUTE_PARAM_MASK  0x80



//  
//  0: ADC2 is power down 
//  1: ADC2 is power-up.
//  Register AnalogDigitalConvertersEnable 0x0D07, Bits 0:0, typedef adc2_power
//   
//  
#define ADC_2_POWER_MASK 0x1U
#define ADC_2_POWER_OFFSET 0
#define ADC_2_POWER_STOP_BIT 0
#define ADC_2_POWER_START_BIT 0
#define ADC_2_POWER_WIDTH 1

#define ANALOG_DIGITAL_CONVERTERS_ENABLE_REG 0xD07

typedef enum adc2_power {
    ADC_2_POWER_DOWN_E,
    ADC_2_POWER_UP_E
} ADC_2_POWER_T ;
#define ADC_2_POWER_PARAM_MASK  0x1



//  
//  0: ADC1 is power down 
//  1: ADC1 is power-up.
//  Register AnalogDigitalConvertersEnable 0x0D07, Bits 1:1, typedef adc1_power
//   
//  
#define ADC_1_POWER_MASK 0x2U
#define ADC_1_POWER_OFFSET 1
#define ADC_1_POWER_STOP_BIT 1
#define ADC_1_POWER_START_BIT 1
#define ADC_1_POWER_WIDTH 1


typedef enum adc1_power {
    ADC_1_POWER_DOWN_E,
    ADC_1_POWER_UP_E
} ADC_1_POWER_T ;
#define ADC_1_POWER_PARAM_MASK  0x2



//  
//  0: ADC3 is power down 
//  1: ADC3 is power-up.
//  Register AnalogDigitalConvertersEnable 0x0D07, Bits 2:2, typedef adc3_power
//   
//  
#define ADC_3_POWER_MASK 0x4U
#define ADC_3_POWER_OFFSET 2
#define ADC_3_POWER_STOP_BIT 2
#define ADC_3_POWER_START_BIT 2
#define ADC_3_POWER_WIDTH 1


typedef enum adc3_power {
    ADC_3_POWER_DOWN_E,
    ADC_3_POWER_UP_E
} ADC_3_POWER_T ;
#define ADC_3_POWER_PARAM_MASK  0x4



//  
//  HeadSetRight Dac driver is power up.
//  Register AnalogDigitalConvertersEnable 0x0D07, Bits 4:4, typedef 
//  hsr_dac_driver_power
//   
//  
#define HSR_DAC_DRIVER_POWER_MASK 0x10U
#define HSR_DAC_DRIVER_POWER_OFFSET 4
#define HSR_DAC_DRIVER_POWER_STOP_BIT 4
#define HSR_DAC_DRIVER_POWER_START_BIT 4
#define HSR_DAC_DRIVER_POWER_WIDTH 1


typedef enum hsr_dac_driver_power {
    HSR_DAC_DRIVER_POWER_DOWN_E,
    HSR_DAC_DRIVER_POWER_UP_E
} HSR_DAC_DRIVER_POWER_T ;
#define HSR_DAC_DRIVER_POWER_PARAM_MASK  0x10



//  
//  HeadSetLelf Dac driver is power up.
//  Register AnalogDigitalConvertersEnable 0x0D07, Bits 5:5, typedef 
//  hsl_dac_driver_power
//   
//  
#define HSL_DAC_DRIVER_POWER_MASK 0x20U
#define HSL_DAC_DRIVER_POWER_OFFSET 5
#define HSL_DAC_DRIVER_POWER_STOP_BIT 5
#define HSL_DAC_DRIVER_POWER_START_BIT 5
#define HSL_DAC_DRIVER_POWER_WIDTH 1


typedef enum hsl_dac_driver_power {
    HSL_DAC_DRIVER_POWER_DOWN_E,
    HSL_DAC_DRIVER_POWER_UP_E
} HSL_DAC_DRIVER_POWER_T ;
#define HSL_DAC_DRIVER_POWER_PARAM_MASK  0x20



//  
//  0: connected to MIC2B, 1 : connected to LINR.
//  Register AnalogDigitalConvertersEnable 0x0D07, Bits 6:6, typedef 
//  adc2_connected_to_mic
//   
//  
#define ADC_2_CONNECTED_TO_MIC_MASK 0x40U
#define ADC_2_CONNECTED_TO_MIC_OFFSET 6
#define ADC_2_CONNECTED_TO_MIC_STOP_BIT 6
#define ADC_2_CONNECTED_TO_MIC_START_BIT 6
#define ADC_2_CONNECTED_TO_MIC_WIDTH 1


typedef enum adc2_connected_to_mic {
    CONNECTED_TO_MIC2_E,
    CONNECTED_LINR_E
} ADC2_CONNECTED_TO_MIC_T ;
#define ADC_2_CONNECTED_TO_MIC_PARAM_MASK  0x40



//  
//  1: connected to MIC1B, 0 : connected to MIC1A.
//  Register AnalogDigitalConvertersEnable 0x0D07, Bits 7:7, typedef 
//  adc1_connected_to_mic_a
//   
//  
#define ADC_1_CONNECTED_TO_MIC_MASK 0x80U
#define ADC_1_CONNECTED_TO_MIC_OFFSET 7
#define ADC_1_CONNECTED_TO_MIC_STOP_BIT 7
#define ADC_1_CONNECTED_TO_MIC_START_BIT 7
#define ADC_1_CONNECTED_TO_MIC_WIDTH 1


typedef enum adc1_connected_to_mic_a {
    CONNECTED_TO_MIC1A_E,
    CONNECTED_TO_MIC1B_E
} ADC1_CONNECTED_TO_MIC_A_T ;
#define ADC_1_CONNECTED_TO_MIC_PARAM_MASK  0x80



//  
//  1: The Vibrator Class-D driver is powered up.
//  Register AnalogOutputEnable 0x0D08, Bits 1:1, typedef vibrator_power
//   
//  
#define VIBRATOR_CLASS_D_POWER_MASK 0x2U
#define VIBRATOR_CLASS_D_POWER_OFFSET 1
#define VIBRATOR_CLASS_D_POWER_STOP_BIT 1
#define VIBRATOR_CLASS_D_POWER_START_BIT 1
#define VIBRATOR_CLASS_D_POWER_WIDTH 1

#define ANALOG_OUTPUT_ENABLE_REG 0xD08

typedef enum vibrator_power {
    VIBRATOR_POWER_DOWN_E,
    VIBRATOR_POWER_UP_E
} VIBRATOR_POWER_T ;
#define VIBRATOR_CLASS_D_POWER_PARAM_MASK  0x2



//  
//  1: The HFR Class-D driver is powered up.
//  Register AnalogOutputEnable 0x0D08, Bits 2:2, typedef hf_right_class_d_power
//   
//  
#define HF_CLASS_D_RIGTH_POWER_MASK 0x4U
#define HF_CLASS_D_RIGTH_POWER_OFFSET 2
#define HF_CLASS_D_RIGTH_POWER_STOP_BIT 2
#define HF_CLASS_D_RIGTH_POWER_START_BIT 2
#define HF_CLASS_D_RIGTH_POWER_WIDTH 1


typedef enum hf_right_class_d_power {
    HF_RIGHT_CLASS_D_POWER_DOWN_E,
    HF_RIGHT_CLASS_D_POWER_UP_E
} HF_RIGHT_CLASS_D_POWER_T ;
#define HF_CLASS_D_RIGTH_POWER_PARAM_MASK  0x4



//  
//  1: The HFL Class-D driver is powered up.
//  Register AnalogOutputEnable 0x0D08, Bits 3:3, typedef hf_left_class_d_power
//   
//  
#define HF_CLASS_D_LEFT_POWER_MASK 0x8U
#define HF_CLASS_D_LEFT_POWER_OFFSET 3
#define HF_CLASS_D_LEFT_POWER_STOP_BIT 3
#define HF_CLASS_D_LEFT_POWER_START_BIT 3
#define HF_CLASS_D_LEFT_POWER_WIDTH 1


typedef enum hf_left_class_d_power {
    HF_LEFT_CLASS_D_POWER_DOWN_E,
    HF_LEFT_CLASS_D_POWER_UP_E
} HF_LEFT_CLASS_D_POWER_T ;
#define HF_CLASS_D_LEFT_POWER_PARAM_MASK  0x8



//  
//  1: The Headset Right is powered up.
//  Register AnalogOutputEnable 0x0D08, Bits 4:4, typedef headset_right_power
//   
//  
#define HEADSET_RIGHT_POWER_MASK 0x10U
#define HEADSET_RIGHT_POWER_OFFSET 4
#define HEADSET_RIGHT_POWER_STOP_BIT 4
#define HEADSET_RIGHT_POWER_START_BIT 4
#define HEADSET_RIGHT_POWER_WIDTH 1


typedef enum headset_right_power {
    HEADSET_RIGHT_POWER_DOWN_E,
    HEADSET_RIGHT_POWER_UP_E
} HEADSET_RIGHT_POWER_T ;
#define HEADSET_RIGHT_POWER_PARAM_MASK  0x10



//  
//  1: The Headset Left is powered up.
//  Register AnalogOutputEnable 0x0D08, Bits 5:5, typedef headset_left_power
//   
//  
#define HEADSET_LEFT_POWER_MASK 0x20U
#define HEADSET_LEFT_POWER_OFFSET 5
#define HEADSET_LEFT_POWER_STOP_BIT 5
#define HEADSET_LEFT_POWER_START_BIT 5
#define HEADSET_LEFT_POWER_WIDTH 1


typedef enum headset_left_power {
    HEADSET_LEFT_POWER_DOWN_E,
    HEADSET_LEFT_POWER_UP_E
} HEADSET_LEFT_POWER_T ;
#define HEADSET_LEFT_POWER_PARAM_MASK  0x20



//  
//  1: The EAR Class-AB driver is powered up.
//  Register AnalogOutputEnable 0x0D08, Bits 6:6, typedef ear_class_ab_power
//   
//  
#define EAR_CLASS_AB_POWER_MASK 0x40U
#define EAR_CLASS_AB_POWER_OFFSET 6
#define EAR_CLASS_AB_POWER_STOP_BIT 6
#define EAR_CLASS_AB_POWER_START_BIT 6
#define EAR_CLASS_AB_POWER_WIDTH 1


typedef enum ear_class_ab_power {
    EAR_CLASS_AB_POWER_DOWN_E,
    EAR_CLASS_AB_POWER_UP_E
} EAR_CLASS_AB_POWER_T ;
#define EAR_CLASS_AB_POWER_PARAM_MASK  0x40



//  
//  0: Internal pull down on VssVcpHs is enabled. 
//  1: Internal pull down on VssVcpHs is disabled..
//  Register AnalogOutputEnable 0x0D08, Bits 7:7, typedef internal_pull_down_enabled
//   
//  
#define INTERNAL_PULL_DOWN_MASK 0x80U
#define INTERNAL_PULL_DOWN_OFFSET 7
#define INTERNAL_PULL_DOWN_STOP_BIT 7
#define INTERNAL_PULL_DOWN_START_BIT 7
#define INTERNAL_PULL_DOWN_WIDTH 1


typedef enum internal_pull_down_enabled {
    INTERNAL_PULL_DOWN_ENABLE_E,
    INTERNAL_PULL_DOWN_DISABLE_E
} INTERNAL_PULL_DOWN_ENABLED_T ;
#define INTERNAL_PULL_DOWN_PARAM_MASK  0x80



//  
//  1: The Vibrator and digital dedicated path are powered up.
//  Register DigitalOutputEnable 0x0D09, Bits 1:1, typedef vibrator_class_d_path_power
//   
//  
#define VIBRATOR_CLASS_D_PATH_POWER_MASK 0x2U
#define VIBRATOR_CLASS_D_PATH_POWER_OFFSET 1
#define VIBRATOR_CLASS_D_PATH_POWER_STOP_BIT 1
#define VIBRATOR_CLASS_D_PATH_POWER_START_BIT 1
#define VIBRATOR_CLASS_D_PATH_POWER_WIDTH 1

#define DIGITAL_OUTPUT_ENABLE_REG 0xD09

typedef enum vibrator_class_d_path_power {
    VIBRATOR_CLASS_D_PATH_POWER_DOWN_E,
    VIBRATOR_CLASS_D_PATH_POWER_UP_E
} VIBRATOR_CLASS_D_PATH_POWER_T ;
#define VIBRATOR_CLASS_D_PATH_POWER_PARAM_MASK  0x2



//  
//  1: The HFR Class-D end digital dedicated path are powered up.
//  Register DigitalOutputEnable 0x0D09, Bits 2:2, typedef hf_class_d_rigth_path_power
//   
//  
#define HF_CLASS_D_RIGTH_PATH_POWER_MASK 0x4U
#define HF_CLASS_D_RIGTH_PATH_POWER_OFFSET 2
#define HF_CLASS_D_RIGTH_PATH_POWER_STOP_BIT 2
#define HF_CLASS_D_RIGTH_PATH_POWER_START_BIT 2
#define HF_CLASS_D_RIGTH_PATH_POWER_WIDTH 1


typedef enum hf_class_d_rigth_path_power {
    HF_CLASS_D_RIGTH_PATH_POWER_DOWN_E,
    HF_CLASS_D_RIGTH_PATH_POWER_UP_E
} HF_CLASS_D_RIGTH_PATH_POWER_T ;
#define HF_CLASS_D_RIGTH_PATH_POWER_PARAM_MASK  0x4



//  
//  1 :The HFL Class-D and digital dedicated path are powered up.
//  Register DigitalOutputEnable 0x0D09, Bits 3:3, typedef hf_class_d_left_path_power
//   
//  
#define HF_CLASS_D_LEFT_PATH_POWER_MASK 0x8U
#define HF_CLASS_D_LEFT_PATH_POWER_OFFSET 3
#define HF_CLASS_D_LEFT_PATH_POWER_STOP_BIT 3
#define HF_CLASS_D_LEFT_PATH_POWER_START_BIT 3
#define HF_CLASS_D_LEFT_PATH_POWER_WIDTH 1


typedef enum hf_class_d_left_path_power {
    HF_CLASS_D_LEFT_PATH_POWER_DOWN_E,
    HF_CLASS_D_LEFT_PATH_POWER_UP_E
} HF_CLASS_D_LEFT_PATH_POWER_T ;
#define HF_CLASS_D_LEFT_PATH_POWER_PARAM_MASK  0x8



//  
//  1: HSR DAC and digital dedicated path are powered up.
//  Register DigitalOutputEnable 0x0D09, Bits 4:4, typedef headset_right_path_power
//   
//  
#define HEADSET_RIGHT_PATH_POWER_MASK 0x10U
#define HEADSET_RIGHT_PATH_POWER_OFFSET 4
#define HEADSET_RIGHT_PATH_POWER_STOP_BIT 4
#define HEADSET_RIGHT_PATH_POWER_START_BIT 4
#define HEADSET_RIGHT_PATH_POWER_WIDTH 1


typedef enum headset_right_path_power {
    HEADSET_RIGHT_PATH_POWER_DOWN_E,
    HEADSET_RIGHT_PATH_POWER_UP_E
} HEADSET_RIGHT_PATH_POWER_T ;
#define HEADSET_RIGHT_PATH_POWER_PARAM_MASK  0x10



//  
//  1: HSL DAC and digital dedicated path are powered up.
//  Register DigitalOutputEnable 0x0D09, Bits 5:5, typedef headset_left_path_power
//   
//  
#define HEADSET_LEFT_PATH_POWER_MASK 0x20U
#define HEADSET_LEFT_PATH_POWER_OFFSET 5
#define HEADSET_LEFT_PATH_POWER_STOP_BIT 5
#define HEADSET_LEFT_PATH_POWER_START_BIT 5
#define HEADSET_LEFT_PATH_POWER_WIDTH 1


typedef enum headset_left_path_power {
    HEADSET_LEFT_PATH_POWER_DOWN_E,
    HEADSET_LEFT_PATH_POWER_UP_E
} HEADSET_LEFT_PATH_POWER_T ;
#define HEADSET_LEFT_PATH_POWER_PARAM_MASK  0x20



//  
//  1: EAR DAC and digital dedicated path are powered up.
//  Register DigitalOutputEnable 0x0D09, Bits 6:6, typedef ear_and_digital_path_power
//   
//  
#define EAR_AND_DIGITAL_PATH_POWER_MASK 0x40U
#define EAR_AND_DIGITAL_PATH_POWER_OFFSET 6
#define EAR_AND_DIGITAL_PATH_POWER_STOP_BIT 6
#define EAR_AND_DIGITAL_PATH_POWER_START_BIT 6
#define EAR_AND_DIGITAL_PATH_POWER_WIDTH 1


typedef enum ear_and_digital_path_power {
    EAR_AND_DIGITAL_PATH_POWER_DOWN_E,
    EAR_AND_DIGITAL_PATH_POWER_UP_E
} EAR_AND_DIGITAL_PATH_POWER_T ;
#define EAR_AND_DIGITAL_PATH_POWER_PARAM_MASK  0x40



//  
//  0: Charge Pump is working in normal mode. 
//  1: Enable low frequency limit for Charge Pump (NCP frequency cannot be in audio 
//  band).
//  Register DigitalOutputEnable 0x0D09, Bits 7:7, typedef low_frequency_lim_enable
//   
//  
#define LOW_FREQUENCY_LIM_MASK 0x80U
#define LOW_FREQUENCY_LIM_OFFSET 7
#define LOW_FREQUENCY_LIM_STOP_BIT 7
#define LOW_FREQUENCY_LIM_START_BIT 7
#define LOW_FREQUENCY_LIM_WIDTH 1


typedef enum low_frequency_lim_enable {
    LOW_FREQUENCY_LIM_DISABLE_E,
    LOW_FREQUENCY_LIM_ENABLE_E
} LOW_FREQUENCY_LIM_ENABLE_T ;
#define LOW_FREQUENCY_LIM_PARAM_MASK  0x80



//  
//  0: The HsR DAC is in normal mode 
//  1: The HsR DAC is muted.
//  Register AnalogOutputMute 0x0D0A, Bits 0:0, typedef headset_right_dac_mute
//   
//  
#define HEADSET_RIGHT_DAC_MUTE_MASK 0x1U
#define HEADSET_RIGHT_DAC_MUTE_OFFSET 0
#define HEADSET_RIGHT_DAC_MUTE_STOP_BIT 0
#define HEADSET_RIGHT_DAC_MUTE_START_BIT 0
#define HEADSET_RIGHT_DAC_MUTE_WIDTH 1

#define ANALOG_OUTPUT_MUTE_REG 0xD0A

typedef enum headset_right_dac_mute {
    HEADSET_RIGHT_DAC_UNMUTE_E,
    HEADSET_RIGHT_DAC_MUTE_E
} HEADSET_RIGHT_DAC_MUTE_T ;
#define HEADSET_RIGHT_DAC_MUTE_PARAM_MASK  0x1



//  
//  0: The HsL DAC is in normal mode 
//  1: The HsL DAC is muted.
//  Register AnalogOutputMute 0x0D0A, Bits 1:1, typedef headset_left_dac_mute
//   
//  
#define HEADSET_LEFT_DAC_MUTE_MASK 0x2U
#define HEADSET_LEFT_DAC_MUTE_OFFSET 1
#define HEADSET_LEFT_DAC_MUTE_STOP_BIT 1
#define HEADSET_LEFT_DAC_MUTE_START_BIT 1
#define HEADSET_LEFT_DAC_MUTE_WIDTH 1


typedef enum headset_left_dac_mute {
    HEADSET_LEFT_DAC_UNMUTE_E,
    HEADSET_LEFT_DAC_MUTE_E
} HEADSET_LEFT_DAC_MUTE_T ;
#define HEADSET_LEFT_DAC_MUTE_PARAM_MASK  0x2



//  
//  0: The Ear DAC is in normal mode 
//  1: The Ear DAC is muted.
//  Register AnalogOutputMute 0x0D0A, Bits 2:2, typedef ear_dac_mute
//   
//  
#define EAR_DAC_MUTE_MASK 0x4U
#define EAR_DAC_MUTE_OFFSET 2
#define EAR_DAC_MUTE_STOP_BIT 2
#define EAR_DAC_MUTE_START_BIT 2
#define EAR_DAC_MUTE_WIDTH 1


typedef enum ear_dac_mute {
    EAR_DAC_UNMUTE_E,
    EAR_DAC_MUTE_E
} EAR_DAC_MUTE_T ;
#define EAR_DAC_MUTE_PARAM_MASK  0x4



//  
//  1: The Headset Right is muted.
//  Register AnalogOutputMute 0x0D0A, Bits 4:4, typedef headset_right_mute
//   
//  
#define HEADSET_RIGHT_MUTE_MASK 0x10U
#define HEADSET_RIGHT_MUTE_OFFSET 4
#define HEADSET_RIGHT_MUTE_STOP_BIT 4
#define HEADSET_RIGHT_MUTE_START_BIT 4
#define HEADSET_RIGHT_MUTE_WIDTH 1


typedef enum headset_right_mute {
    HEADSET_RIGHT_UNMUTE_E,
    HEADSET_RIGHT_MUTE_E
} HEADSET_RIGHT_MUTE_T ;
#define HEADSET_RIGHT_MUTE_PARAM_MASK  0x10



//  
//  1: The Headset Left is muted.
//  Register AnalogOutputMute 0x0D0A, Bits 5:5, typedef headset_left_mute
//   
//  
#define HEADSET_LEFT_MUTE_MASK 0x20U
#define HEADSET_LEFT_MUTE_OFFSET 5
#define HEADSET_LEFT_MUTE_STOP_BIT 5
#define HEADSET_LEFT_MUTE_START_BIT 5
#define HEADSET_LEFT_MUTE_WIDTH 1


typedef enum headset_left_mute {
    HEADSET_LEFT_UNMUTE_E,
    HEADSET_LEFT_MUTE_E
} HEADSET_LEFT_MUTE_T ;
#define HEADSET_LEFT_MUTE_PARAM_MASK  0x20



//  
//  1: The EAR Class-AB driver is muted.
//  Register AnalogOutputMute 0x0D0A, Bits 6:6, typedef ear_class_ab_mute
//   
//  
#define EAR_CLASS_AB_MUTE_MASK 0x40U
#define EAR_CLASS_AB_MUTE_OFFSET 6
#define EAR_CLASS_AB_MUTE_STOP_BIT 6
#define EAR_CLASS_AB_MUTE_START_BIT 6
#define EAR_CLASS_AB_MUTE_WIDTH 1


typedef enum ear_class_ab_mute {
    EAR_CLASS_AB_UNMUTE_E,
    EAR_CLASS_AB_MUTE_E
} EAR_CLASS_AB_MUTE_T ;
#define EAR_CLASS_AB_MUTE_PARAM_MASK  0x40



//  
//  0: HS gain changes on signal zero cross (unless time-out occurs) 
//  1: HS gain is changed without zero cross control.
//  Register ShortCircuitDetection 0x0D0B, Bits 0:0, typedef zerocross_enable
//   
//  
#define HEADSET_ZEROCROSS_MASK 0x1U
#define HEADSET_ZEROCROSS_OFFSET 0
#define HEADSET_ZEROCROSS_STOP_BIT 0
#define HEADSET_ZEROCROSS_START_BIT 0
#define HEADSET_ZEROCROSS_WIDTH 1

#define SHORT_CIRCUIT_DETECTION_REG 0xD0B

typedef enum zerocross_enable {
    ZEROCROSS_DISABLE_E,
    ZEROCROSS_ENABLE_E
} ZEROCROSS_ENABLE_T ;
#define HEADSET_ZEROCROSS_PARAM_MASK  0x1



//  
//  0: All intermediate steps are applied between 2 programmed gains (fading) 
//  1: Gain on HS is applied immediately.
//  Register ShortCircuitDetection 0x0D0B, Bits 1:1, typedef headset_fading_enable
//   
//  
#define HEADSET_FADING_MASK 0x2U
#define HEADSET_FADING_OFFSET 1
#define HEADSET_FADING_STOP_BIT 1
#define HEADSET_FADING_START_BIT 1
#define HEADSET_FADING_WIDTH 1


typedef enum headset_fading_enable {
    HEADSET_FADING_DISABLE_E,
    HEADSET_FADING_ENABLE_E
} HEADSET_FADING_ENABLE_T ;
#define HEADSET_FADING_PARAM_MASK  0x2



//  
//  0: The HS drivers use the system clock 
//  1: The HS drivers use a local oscillator (system clock absent: analog path 
//  only).
//  Register ShortCircuitDetection 0x0D0B, Bits 2:2, typedef oscillator_enable
//   
//  
#define HEADSET_OSCILLATOR_MASK 0x4U
#define HEADSET_OSCILLATOR_OFFSET 2
#define HEADSET_OSCILLATOR_STOP_BIT 2
#define HEADSET_OSCILLATOR_START_BIT 2
#define HEADSET_OSCILLATOR_WIDTH 1


typedef enum oscillator_enable {
    OSCILLATOR_DISABLE_E,
    OSCILLATOR_ENABLE_E
} OSCILLATOR_ENABLE_T ;
#define HEADSET_OSCILLATOR_PARAM_MASK  0x4



//  
//  1: HSL and HSR outputs are pulled down to ground 
//  0: HSL and HSR outputs are in high impedance.
//  Register ShortCircuitDetection 0x0D0B, Bits 4:4, typedef headset_pull_down_enable
//   
//  
#define HEADSET_PULL_DOWN_MASK 0x10U
#define HEADSET_PULL_DOWN_OFFSET 4
#define HEADSET_PULL_DOWN_STOP_BIT 4
#define HEADSET_PULL_DOWN_START_BIT 4
#define HEADSET_PULL_DOWN_WIDTH 1


typedef enum headset_pull_down_enable {
    HEADSET_PULL_DOWN_DISABLE_E,
    HEADSET_PULL_DOWN_ENABLE_E
} HEADSET_PULL_DOWN_ENABLE_T ;
#define HEADSET_PULL_DOWN_PARAM_MASK  0x10



//  
//  1: Short circuit detected on headset Left and Right is enabled.
//  Register ShortCircuitDetection 0x0D0B, Bits 5:5, typedef headset_detection_enable
//   
//  
#define HEADSET_DETECTION_MASK 0x20U
#define HEADSET_DETECTION_OFFSET 5
#define HEADSET_DETECTION_STOP_BIT 5
#define HEADSET_DETECTION_START_BIT 5
#define HEADSET_DETECTION_WIDTH 1


typedef enum headset_detection_enable {
    HEADSET_DETECTION_ENABLE_E,
    HEADSET_DETECTION_DISABLE_E
} HEADSET_DETECTION_ENABLE_T ;
#define HEADSET_DETECTION_PARAM_MASK  0x20



//  
//  1: Short circuit detected on EAR driver enabled.
//  Register ShortCircuitDetection 0x0D0B, Bits 6:6, typedef ear_detection_enable
//   
//  
#define EAR_DETECTION_MASK 0x40U
#define EAR_DETECTION_OFFSET 6
#define EAR_DETECTION_STOP_BIT 6
#define EAR_DETECTION_START_BIT 6
#define EAR_DETECTION_WIDTH 1


typedef enum ear_detection_enable {
    EAR_DETECTION_ENABLE_E,
    EAR_DETECTION_DISABLE_E
} EAR_DETECTION_ENABLE_T ;
#define EAR_DETECTION_PARAM_MASK  0x40



//  
//  This bit act on Earpiece and Headset drive. 
//  1: Automatic switch off on short circuit detection is enabled.
//  Register ShortCircuitDetection 0x0D0B, Bits 7:7, typedef switch_off_enable
//   
//  
#define EAR_AUTO_SWITCH_OFF_MASK 0x80U
#define EAR_AUTO_SWITCH_OFF_OFFSET 7
#define EAR_AUTO_SWITCH_OFF_STOP_BIT 7
#define EAR_AUTO_SWITCH_OFF_START_BIT 7
#define EAR_AUTO_SWITCH_OFF_WIDTH 1


typedef enum switch_off_enable {
    SWITCH_OFF_DISABLE_E,
    SWITCH_OFF_ENABLE_E
} SWITCH_OFF_ENABLE_T ;
#define EAR_AUTO_SWITCH_OFF_PARAM_MASK  0x80



//  
//  For a complete auto power-up sequence, It should be enabled or disabled together or 
//  after ENCPHS..
//  Register PowerUpHeadSet 0x0D0C, Bits 0:0, typedef auto_headset_power
//   
//  
#define AUTO_HEADSET_POWER_UP_MASK 0x1U
#define AUTO_HEADSET_POWER_UP_OFFSET 0
#define AUTO_HEADSET_POWER_UP_STOP_BIT 0
#define AUTO_HEADSET_POWER_UP_START_BIT 0
#define AUTO_HEADSET_POWER_UP_WIDTH 1

#define POWER_UP_HEAD_SET_REG 0xD0C

typedef enum auto_headset_power {
    AUTO_HEADSET_POWER_DOWN_E,
    AUTO_HEADSET_POWER_UP_E
} AUTO_HEADSET_POWER_T ;
#define AUTO_HEADSET_POWER_UP_PARAM_MASK  0x1



//  
//  0: Right Line-Out driver is disabled. 
//  1: Right Line-Out driver is enabled.
//  Register PowerUpHeadSet 0x0D0C, Bits 2:2, typedef right_line_out_enable
//   
//  
#define RIGHT_LINE_OUT_DRIVER_MASK 0x4U
#define RIGHT_LINE_OUT_DRIVER_OFFSET 2
#define RIGHT_LINE_OUT_DRIVER_STOP_BIT 2
#define RIGHT_LINE_OUT_DRIVER_START_BIT 2
#define RIGHT_LINE_OUT_DRIVER_WIDTH 1


typedef enum right_line_out_enable {
    RIGHT_LINE_OUT_DISABLE_E,
    RIGHT_LINE_OUT_ENABLE_E
} RIGHT_LINE_OUT_ENABLE_T ;
#define RIGHT_LINE_OUT_DRIVER_PARAM_MASK  0x4



//  
//  0: Left Line-Out driver is disabled. 
//  1: Left Line-Out driver is enabled.
//  Register PowerUpHeadSet 0x0D0C, Bits 3:3, typedef left_line_out_enable
//   
//  
#define LEFT_LINE_OUT_DRIVER_MASK 0x8U
#define LEFT_LINE_OUT_DRIVER_OFFSET 3
#define LEFT_LINE_OUT_DRIVER_STOP_BIT 3
#define LEFT_LINE_OUT_DRIVER_START_BIT 3
#define LEFT_LINE_OUT_DRIVER_WIDTH 1


typedef enum left_line_out_enable {
    LEFT_LINE_OUT_DISABLE_E,
    LEFT_LINE_OUT_ENABLE_E
} LEFT_LINE_OUT_ENABLE_T ;
#define LEFT_LINE_OUT_DRIVER_PARAM_MASK  0x8



//  
//  0: Right Line-Out driver input is Ear-DAC. 
//  1: Right Line-Out driver input is HsR-DAC..
//  Register PowerUpHeadSet 0x0D0C, Bits 4:4, typedef right_line_input
//   
//  
#define RIGHT_LINE_OUT_INPUT_MASK 0x10U
#define RIGHT_LINE_OUT_INPUT_OFFSET 4
#define RIGHT_LINE_OUT_INPUT_STOP_BIT 4
#define RIGHT_LINE_OUT_INPUT_START_BIT 4
#define RIGHT_LINE_OUT_INPUT_WIDTH 1


typedef enum right_line_input {
    RIGTH_LINEOUT_INPUT_EAD_DAC_E,
    RIGTH_LINEOUT_INPUT_HEADSET_DAC_E
} RIGHT_LINE_INPUT_T ;
#define RIGHT_LINE_OUT_INPUT_PARAM_MASK  0x10



//  
//  0: Left Line-Out driver input is Ear-DAC. 
//  1: Left Line-Out driver input is HsL-DAC..
//  Register PowerUpHeadSet 0x0D0C, Bits 5:5, typedef left_line_input
//   
//  
#define LEFT_LINE_OUT_INPUT_MASK 0x20U
#define LEFT_LINE_OUT_INPUT_OFFSET 5
#define LEFT_LINE_OUT_INPUT_STOP_BIT 5
#define LEFT_LINE_OUT_INPUT_START_BIT 5
#define LEFT_LINE_OUT_INPUT_WIDTH 1


typedef enum left_line_input {
    LEFT_LINEOUT_INPUT_EAD_DAC_E,
    LEFT_LINEOUT_INPUT_HEADSET_DAC_E
} LEFT_LINE_INPUT_T ;
#define LEFT_LINE_OUT_INPUT_PARAM_MASK  0x20



//  
//  0: Charge Pump disabled. If HSAUTOENSEL=1 also HSAUTOEN must me resetted. 
//  1: Charge Pump enabled..
//  Register PowerUpHeadSet 0x0D0C, Bits 7:7, typedef charge_pump_enable
//   
//  
#define CHARGE_PUMP_MASK 0x80U
#define CHARGE_PUMP_OFFSET 7
#define CHARGE_PUMP_STOP_BIT 7
#define CHARGE_PUMP_START_BIT 7
#define CHARGE_PUMP_WIDTH 1


typedef enum charge_pump_enable {
    CHARGE_PUMP_DISABLE_E,
    CHARGE_PUMP_ENABLE_E
} CHARGE_PUMP_ENABLE_T ;
#define CHARGE_PUMP_PARAM_MASK  0x80


#define LOW_THRESHOLD_PARAM_MASK  0xF
#define LOW_THRESHOLD_MASK 0xFU
#define LOW_THRESHOLD_OFFSET 3
#define LOW_THRESHOLD_STOP_BIT 3
#define LOW_THRESHOLD_START_BIT 0
#define LOW_THRESHOLD_WIDTH 4

typedef enum threshold {
    HIGH_THRESOLD_0_25_V = 0x0,
    HIGH_THRESOLD_0_30_V = 0x1,
    HIGH_THRESOLD_0_35_V = 0x2,
    HIGH_THRESOLD_0_40_V = 0x3,
    HIGH_THRESOLD_0_45_V = 0x4,
    HIGH_THRESOLD_0_50_V = 0x5,
    HIGH_THRESOLD_0_55_V = 0x6,
    HIGH_THRESOLD_0_60_V = 0x7,
    HIGH_THRESOLD_0_65_V = 0x8,
    HIGH_THRESOLD_0_70_V = 0x9,
    HIGH_THRESOLD_0_75_V = 0xA,
    HIGH_THRESOLD_0_80_V = 0xB,
    HIGH_THRESOLD_0_85_V = 0xC,
    HIGH_THRESOLD_0_90_V = 0xD,
    HIGH_THRESOLD_0_95_V = 0xE,
    HIGH_THRESOLD_1_V = 0xF
} THRESHOLD_T ;
#define HIGH_THRESHOLD_PARAM_MASK  0xF0
#define HIGH_THRESHOLD_MASK 0xF0U
#define HIGH_THRESHOLD_OFFSET 7
#define HIGH_THRESHOLD_STOP_BIT 7
#define HIGH_THRESHOLD_START_BIT 4
#define HIGH_THRESHOLD_WIDTH 4

#define ENVELOPE_CONTROL_REG 0xD0D
#define ENVELOPE_DECAY_TIME_PARAM_MASK  0xF
#define ENVELOPE_DECAY_TIME_MASK 0xFU
#define ENVELOPE_DECAY_TIME_OFFSET 3
#define ENVELOPE_DECAY_TIME_STOP_BIT 3
#define ENVELOPE_DECAY_TIME_START_BIT 0
#define ENVELOPE_DECAY_TIME_WIDTH 4

typedef enum decay_time {
    MICRO_SECOND_27_DECAY_TIME = 0x0,
    MICRO_SECOND_54_DECAY_TIME = 0x1,
    MICRO_SECOND_108_DECAY_TIME = 0x2,
    MICRO_SECOND_216_DECAY_TIME = 0x3,
    MICRO_SECOND_432_DECAY_TIME = 0x4,
    MICRO_SECOND_864_DECAY_TIME = 0x5,
    MICRO_SECOND_1728_DECAY_TIME = 0x6,
    MICRO_SECOND_3456_DECAY_TIME = 0x7,
    MICRO_SECOND_6912_DECAY_TIME = 0x8,
    MICRO_SECOND_13824_DECAY_TIME = 0x9,
    MICRO_SECOND_27648_DECAY_TIME = 0xA,
    MICRO_SECOND_55296_DECAY_TIME = 0xB,
    MICRO_SECOND_110592_DECAY_TIME = 0xC,
    MICRO_SECOND_221184_DECAY_TIME = 0xD,
    MICRO_SECOND_442368_DECAY_TIME = 0xE
} DECAY_TIME_T ;

//  
//  Envelope detection for charge pump.
//  Register EnvelopeThreshold 0x0D0E, Bits 4:4, typedef envelope_detection_c_p_enable
//   
//  
#define ENVELOPE_DETECTION_CP_MASK 0x10U
#define ENVELOPE_DETECTION_CP_OFFSET 4
#define ENVELOPE_DETECTION_CP_STOP_BIT 4
#define ENVELOPE_DETECTION_CP_START_BIT 4
#define ENVELOPE_DETECTION_CP_WIDTH 1

#define ENVELOPE_THRESHOLD_REG 0xD0E

typedef enum envelope_detection_c_p_enable {
    ENVELOPE_DETECTION_CP_DISABLE_E,
    ENVELOPE_DETECTION_CP_ENABLE_E
} ENVELOPE_DETECTION_CP_ENABLE_T ;
#define ENVELOPE_DETECTION_CP_PARAM_MASK  0x10



//  
//  Input voltage control for Charge Pump. Active just when envelope detection is 
//  disabled. 
//  0: VinVcphs ball supplies negative charge Pump. 
//  1: SmpsVcphs ball supplies negative charge Pump..
//  Register EnvelopeThreshold 0x0D0E, Bits 5:5, typedef input_voltage_control
//   
//  
#define INPUT_VOLTAGE_CONTROL_MASK 0x20U
#define INPUT_VOLTAGE_CONTROL_OFFSET 5
#define INPUT_VOLTAGE_CONTROL_STOP_BIT 5
#define INPUT_VOLTAGE_CONTROL_START_BIT 5
#define INPUT_VOLTAGE_CONTROL_WIDTH 1


typedef enum input_voltage_control {
    INPUT_LOW_VOLTAGE_E,
    INPUT_HIGH_VOLTAGE_E
} INPUT_VOLTAGE_CONTROL_T ;
#define INPUT_VOLTAGE_CONTROL_PARAM_MASK  0x20



//  
//  When PWM generator is enable positive is started separatetly from negative (used in 
//  differential).
//  Register ConfigurationClassDDrivers 0x0D0F, Bits 2:2, typedef pwm_generator_positive
//   
//  
#define PWM_GENERATOR_POS_MASK 0x4U
#define PWM_GENERATOR_POS_OFFSET 2
#define PWM_GENERATOR_POS_STOP_BIT 2
#define PWM_GENERATOR_POS_START_BIT 2
#define PWM_GENERATOR_POS_WIDTH 1

#define CONFIGURATION_CLASS_D_DRIVERS_REG 0xD0F

typedef enum pwm_generator_positive {
    PWM_POSITIVE_GENERATOR_POL_E,
    PWM_GENERATOR_POSITIVE_DUTYCYCLE_E
} PWM_GENERATOR_POSITIVE_T ;
#define PWM_GENERATOR_POS_PARAM_MASK  0x4



//  
//  When PWM generator is enable negative is started separatetly from positive (not used 
//  in differential).
//  Register ConfigurationClassDDrivers 0x0D0F, Bits 3:3, typedef pwm_generator_negative
//   
//  
#define PWM_GENERATOR_NEG_MASK 0x8U
#define PWM_GENERATOR_NEG_OFFSET 3
#define PWM_GENERATOR_NEG_STOP_BIT 3
#define PWM_GENERATOR_NEG_START_BIT 3
#define PWM_GENERATOR_NEG_WIDTH 1


typedef enum pwm_generator_negative {
    PWM_NEGATIVE_GENERATOR_POL_E,
    PWM_GENERATOR_NEGATIVE_DUTYCYCLE_E
} PWM_GENERATOR_NEGATIVE_T ;
#define PWM_GENERATOR_NEG_PARAM_MASK  0x8



//  
//  When PWM generator is enable positive and negative are started together (used in 
//  differential).
//  Register ConfigurationClassDDrivers 0x0D0F, Bits 5:5, typedef pwm_started_together
//   
//  
#define PWM_GENERATOR_CONTROL_MASK 0x20U
#define PWM_GENERATOR_CONTROL_OFFSET 5
#define PWM_GENERATOR_CONTROL_STOP_BIT 5
#define PWM_GENERATOR_CONTROL_START_BIT 5
#define PWM_GENERATOR_CONTROL_WIDTH 1


typedef enum pwm_started_together {
    PWM_VALUE_E,
    PWM_DUTY_CYCLE_E
} PWM_STARTED_TOGETHER_T ;
#define PWM_GENERATOR_CONTROL_PARAM_MASK  0x20



//  
//  0: VIB driver is connected to corresponding DA path, 
//  1: VIB driver is controlled by internal PWM Generator.
//  Register ConfigurationClassDDrivers 0x0D0F, Bits 7:7, typedef vib1
//   
//  
#define VIB_CONTROLLED_INTERNAL_MASK 0x80U
#define VIB_CONTROLLED_INTERNAL_OFFSET 7
#define VIB_CONTROLLED_INTERNAL_STOP_BIT 7
#define VIB_CONTROLLED_INTERNAL_START_BIT 7
#define VIB_CONTROLLED_INTERNAL_WIDTH 1


typedef enum vib1 {
    VIB_DRIVER_CONNECTED_DA_PATH_E,
    VIB_DRIVER_CONNECTED_INTERNAL_GENERATOR_E
} VIB_1_T ;
#define VIB_CONTROLLED_INTERNAL_PARAM_MASK  0x80


#define VIB_1_POSITIVE_DUTY_CYLE_PARAM_MASK  0x7F
#define VIB_1_POSITIVE_DUTY_CYLE_MASK 0x7FU
#define VIB_1_POSITIVE_DUTY_CYLE_OFFSET 6
#define VIB_1_POSITIVE_DUTY_CYLE_STOP_BIT 6
#define VIB_1_POSITIVE_DUTY_CYLE_START_BIT 0
#define VIB_1_POSITIVE_DUTY_CYLE_WIDTH 7

typedef enum duty_cycle {
    DUTY_CYCLE_0_PER_CENT = 0x0,
    DUTY_CYCLE_1_PER_CENT = 0x1,
    DUTY_CYCLE_2_PER_CENT = 0x2,
    DUTY_CYCLE_3_PER_CENT = 0x3,
    DUTY_CYCLE_4_PER_CENT = 0x4,
    DUTY_CYCLE_5_PER_CENT = 0x5,
    DUTY_CYCLE_6_PER_CENT = 0x6,
    DUTY_CYCLE_7_PER_CENT = 0x7,
    DUTY_CYCLE_8_PER_CENT = 0x8,
    DUTY_CYCLE_9_PER_CENT = 0x9,
    DUTY_CYCLE_10_PER_CENT = 0xA,
    DUTY_CYCLE_11_PER_CENT = 0xB,
    DUTY_CYCLE_12_PER_CENT = 0xC,
    DUTY_CYCLE_13_PER_CENT = 0xD,
    DUTY_CYCLE_14_PER_CENT = 0xE,
    DUTY_CYCLE_15_PER_CENT = 0xF,
    DUTY_CYCLE_16_PER_CENT = 0x10,
    DUTY_CYCLE_17_PER_CENT = 0x11,
    DUTY_CYCLE_18_PER_CENT = 0x12,
    DUTY_CYCLE_19_PER_CENT = 0x13,
    DUTY_CYCLE_20_PER_CENT = 0x14,
    DUTY_CYCLE_21_PER_CENT = 0x15,
    DUTY_CYCLE_22_PER_CENT = 0x16,
    DUTY_CYCLE_23_PER_CENT = 0x17,
    DUTY_CYCLE_24_PER_CENT = 0x18,
    DUTY_CYCLE_25_PER_CENT = 0x19,
    DUTY_CYCLE_26_PER_CENT = 0x1A,
    DUTY_CYCLE_27_PER_CENT = 0x1B,
    DUTY_CYCLE_28_PER_CENT = 0x1C,
    DUTY_CYCLE_29_PER_CENT = 0x1D,
    DUTY_CYCLE_30_PER_CENT = 0x1E,
    DUTY_CYCLE_31_PER_CENT = 0x1F,
    DUTY_CYCLE_32_PER_CENT = 0x20,
    DUTY_CYCLE_33_PER_CENT = 0x21,
    DUTY_CYCLE_34_PER_CENT = 0x22,
    DUTY_CYCLE_35_PER_CENT = 0x23,
    DUTY_CYCLE_36_PER_CENT = 0x24,
    DUTY_CYCLE_37_PER_CENT = 0x25,
    DUTY_CYCLE_38_PER_CENT = 0x26,
    DUTY_CYCLE_39_PER_CENT = 0x27,
    DUTY_CYCLE_40_PER_CENT = 0x28,
    DUTY_CYCLE_41_PER_CENT = 0x29,
    DUTY_CYCLE_42_PER_CENT = 0x2A,
    DUTY_CYCLE_43_PER_CENT = 0x2B,
    DUTY_CYCLE_44_PER_CENT = 0x2C,
    DUTY_CYCLE_45_PER_CENT = 0x2D,
    DUTY_CYCLE_46_PER_CENT = 0x2E,
    DUTY_CYCLE_47_PER_CENT = 0x2F,
    DUTY_CYCLE_48_PER_CENT = 0x30,
    DUTY_CYCLE_49_PER_CENT = 0x31,
    DUTY_CYCLE_50_PER_CENT = 0x32,
    DUTY_CYCLE_51_PER_CENT = 0x33,
    DUTY_CYCLE_52_PER_CENT = 0x34,
    DUTY_CYCLE_53_PER_CENT = 0x35,
    DUTY_CYCLE_54_PER_CENT = 0x36,
    DUTY_CYCLE_55_PER_CENT = 0x37,
    DUTY_CYCLE_56_PER_CENT = 0x38,
    DUTY_CYCLE_57_PER_CENT = 0x39,
    DUTY_CYCLE_58_PER_CENT = 0x3A,
    DUTY_CYCLE_59_PER_CENT = 0x3B,
    DUTY_CYCLE_60_PER_CENT = 0x3C,
    DUTY_CYCLE_61_PER_CENT = 0x3D,
    DUTY_CYCLE_62_PER_CENT = 0x3E,
    DUTY_CYCLE_63_PER_CENT = 0x3F,
    DUTY_CYCLE_64_PER_CENT = 0x40,
    DUTY_CYCLE_65_PER_CENT = 0x41,
    DUTY_CYCLE_66_PER_CENT = 0x42,
    DUTY_CYCLE_67_PER_CENT = 0x43,
    DUTY_CYCLE_68_PER_CENT = 0x44,
    DUTY_CYCLE_69_PER_CENT = 0x45,
    DUTY_CYCLE_70_PER_CENT = 0x46,
    DUTY_CYCLE_71_PER_CENT = 0x47,
    DUTY_CYCLE_72_PER_CENT = 0x48,
    DUTY_CYCLE_73_PER_CENT = 0x49,
    DUTY_CYCLE_74_PER_CENT = 0x4A,
    DUTY_CYCLE_75_PER_CENT = 0x4B,
    DUTY_CYCLE_76_PER_CENT = 0x4C,
    DUTY_CYCLE_77_PER_CENT = 0x4D,
    DUTY_CYCLE_78_PER_CENT = 0x4E,
    DUTY_CYCLE_79_PER_CENT = 0x4F,
    DUTY_CYCLE_80_PER_CENT = 0x50,
    DUTY_CYCLE_81_PER_CENT = 0x51,
    DUTY_CYCLE_82_PER_CENT = 0x52,
    DUTY_CYCLE_83_PER_CENT = 0x53,
    DUTY_CYCLE_84_PER_CENT = 0x54,
    DUTY_CYCLE_85_PER_CENT = 0x55,
    DUTY_CYCLE_86_PER_CENT = 0x56,
    DUTY_CYCLE_87_PER_CENT = 0x57,
    DUTY_CYCLE_88_PER_CENT = 0x58,
    DUTY_CYCLE_89_PER_CENT = 0x59,
    DUTY_CYCLE_90_PER_CENT = 0x5A,
    DUTY_CYCLE_91_PER_CENT = 0x5B,
    DUTY_CYCLE_92_PER_CENT = 0x5C,
    DUTY_CYCLE_93_PER_CENT = 0x5D,
    DUTY_CYCLE_94_PER_CENT = 0x5E,
    DUTY_CYCLE_95_PER_CENT = 0x5F,
    DUTY_CYCLE_96_PER_CENT = 0x60,
    DUTY_CYCLE_97_PER_CENT = 0x61,
    DUTY_CYCLE_98_PER_CENT = 0x62,
    DUTY_CYCLE_99_PER_CENT = 0x63,
    DUTY_CYCLE_100_PER_CENT = 0x64
} DUTY_CYCLE_T ;

//  
//  0: Vib1p pin is forced to GndVib voltage, 
//  1: Vib1p pin is forced to VinVib voltage.
//  Register ConfigurationPositiveOutputVib1 0x0D10, Bits 7:7, typedef forced_to_voltage
//   
//  
#define VIB_1_POSITIVE_FORCED_TO_VOLTAGE_MASK 0x80U
#define VIB_1_POSITIVE_FORCED_TO_VOLTAGE_OFFSET 7
#define VIB_1_POSITIVE_FORCED_TO_VOLTAGE_STOP_BIT 7
#define VIB_1_POSITIVE_FORCED_TO_VOLTAGE_START_BIT 7
#define VIB_1_POSITIVE_FORCED_TO_VOLTAGE_WIDTH 1

#define CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG 0xD10

typedef enum forced_to_voltage {
    GND_VOLTAGE_E,
    VIN_VOLTAGE_E
} FORCED_TO_VOLTAGE_T ;
#define VIB_1_POSITIVE_FORCED_TO_VOLTAGE_PARAM_MASK  0x80


#define VIB_1_NEGATIVE_DUTY_CYLE_PARAM_MASK  0x7F
#define VIB_1_NEGATIVE_DUTY_CYLE_MASK 0x7FU
#define VIB_1_NEGATIVE_DUTY_CYLE_OFFSET 6
#define VIB_1_NEGATIVE_DUTY_CYLE_STOP_BIT 6
#define VIB_1_NEGATIVE_DUTY_CYLE_START_BIT 0
#define VIB_1_NEGATIVE_DUTY_CYLE_WIDTH 7


//  
//  0: Vibn pin is forced to GndVib voltage, 
//  1: Vibn pin is forced to VinVib voltage.
//  Register ConfigurationNegativeOutputVib1 0x0D11, Bits 7:7, typedef forced_to_voltage
//   
//  
#define VIB_NEGATIVE_PIN_FORCED_MASK 0x80U
#define VIB_NEGATIVE_PIN_FORCED_OFFSET 7
#define VIB_NEGATIVE_PIN_FORCED_STOP_BIT 7
#define VIB_NEGATIVE_PIN_FORCED_START_BIT 7
#define VIB_NEGATIVE_PIN_FORCED_WIDTH 1

#define CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG 0xD11

#define VIB_NEGATIVE_PIN_FORCED_PARAM_MASK  0x80


#define VIB_2_POSITIVE_DUTY_CYLE_PARAM_MASK  0x7F
#define VIB_2_POSITIVE_DUTY_CYLE_MASK 0x7FU
#define VIB_2_POSITIVE_DUTY_CYLE_OFFSET 6
#define VIB_2_POSITIVE_DUTY_CYLE_STOP_BIT 6
#define VIB_2_POSITIVE_DUTY_CYLE_START_BIT 0
#define VIB_2_POSITIVE_DUTY_CYLE_WIDTH 7


//  
//  0: Vib2p pin is forced to GndVib voltage, 
//  1: Vib2p pin is forced to VinVib voltage.
//  Register ConfigurationPositiveOutputVib2 0x0D12, Bits 7:7, typedef forced_to_voltage
//   
//  
#define VIB_2_POSITIVE_FORCED_TO_VOLTAGE_MASK 0x80U
#define VIB_2_POSITIVE_FORCED_TO_VOLTAGE_OFFSET 7
#define VIB_2_POSITIVE_FORCED_TO_VOLTAGE_STOP_BIT 7
#define VIB_2_POSITIVE_FORCED_TO_VOLTAGE_START_BIT 7
#define VIB_2_POSITIVE_FORCED_TO_VOLTAGE_WIDTH 1

#define CONFIGURATION_POSITIVE_OUTPUT_VIB_2_REG 0xD12

#define VIB_2_POSITIVE_FORCED_TO_VOLTAGE_PARAM_MASK  0x80


#define MIC_1_GAIN_PARAM_MASK  0x1F
#define MIC_1_GAIN_MASK 0x1FU
#define MIC_1_GAIN_OFFSET 4
#define MIC_1_GAIN_STOP_BIT 4
#define MIC_1_GAIN_START_BIT 0
#define MIC_1_GAIN_WIDTH 5

typedef enum analog_gain {
    ANALOG_GAIN_0_DB = 0x0,
    ANALOG_GAIN_1_DB = 0x1,
    ANALOG_GAIN_2_DB = 0x2,
    ANALOG_GAIN_3_DB = 0x3,
    ANALOG_GAIN_4_DB = 0x4,
    ANALOG_GAIN_5_DB = 0x5,
    ANALOG_GAIN_6_DB = 0x6,
    ANALOG_GAIN_7_DB = 0x7,
    ANALOG_GAIN_8_DB = 0x8,
    ANALOG_GAIN_9_DB = 0x9,
    ANALOG_GAIN_10_DB = 0xA,
    ANALOG_GAIN_11_DB = 0xB,
    ANALOG_GAIN_12_DB = 0xC,
    ANALOG_GAIN_13_DB = 0xD,
    ANALOG_GAIN_14_DB = 0xE,
    ANALOG_GAIN_15_DB = 0xF,
    ANALOG_GAIN_16_DB = 0x10,
    ANALOG_GAIN_17_DB = 0x11,
    ANALOG_GAIN_18_DB = 0x12,
    ANALOG_GAIN_19_DB = 0x13,
    ANALOG_GAIN_20_DB = 0x14,
    ANALOG_GAIN_21_DB = 0x15,
    ANALOG_GAIN_22_DB = 0x16,
    ANALOG_GAIN_23_DB = 0x17,
    ANALOG_GAIN_24_DB = 0x18,
    ANALOG_GAIN_25_DB = 0x19,
    ANALOG_GAIN_26_DB = 0x1A,
    ANALOG_GAIN_27_DB = 0x1B,
    ANALOG_GAIN_28_DB = 0x1C,
    ANALOG_GAIN_29_DB = 0x1D,
    ANALOG_GAIN_30_DB = 0x1E,
    ANALOG_GAIN_31_DB = 0x1F
} ANALOG_GAIN_T ;

//  
//  New in cut 1.0 Low Power for Micro 1.
//  Register AnalogMic1Gain 0x0D14, Bits 6:6, typedef low_power_micro_enable
//   
//  
#define LOW_POWER_MICRO_1_MASK 0x40U
#define LOW_POWER_MICRO_1_OFFSET 6
#define LOW_POWER_MICRO_1_STOP_BIT 6
#define LOW_POWER_MICRO_1_START_BIT 6
#define LOW_POWER_MICRO_1_WIDTH 1

#define ANALOG_MIC_1_GAIN_REG 0xD14

typedef enum low_power_micro_enable {
    LOW_POWER_DOWN_E,
    LOW_POWER_UP_E
} LOW_POWER_MICRO_ENABLE_T ;
#define LOW_POWER_MICRO_1_PARAM_MASK  0x40



//  
//  0: DIfferential microphone is connected to MIC1 inputs , 
//  1: Single ended microphone is connected to MIC1 inputs.
//  Register AnalogMic1Gain 0x0D14, Bits 7:7, typedef single_or_differential
//   
//  
#define MIC_1_SINGLE_ENDED_CONF_MASK 0x80U
#define MIC_1_SINGLE_ENDED_CONF_OFFSET 7
#define MIC_1_SINGLE_ENDED_CONF_STOP_BIT 7
#define MIC_1_SINGLE_ENDED_CONF_START_BIT 7
#define MIC_1_SINGLE_ENDED_CONF_WIDTH 1


typedef enum single_or_differential {
    DIFFERENTIAL_E,
    SINGLE_ENDED_E
} SINGLE_OR_DIFFERENTIAL_T ;
#define MIC_1_SINGLE_ENDED_CONF_PARAM_MASK  0x80


#define MIC_2_GAIN_PARAM_MASK  0x1F
#define MIC_2_GAIN_MASK 0x1FU
#define MIC_2_GAIN_OFFSET 4
#define MIC_2_GAIN_STOP_BIT 4
#define MIC_2_GAIN_START_BIT 0
#define MIC_2_GAIN_WIDTH 5


//  
//  New in cut 1.0 Low Power for Micro 2.
//  Register AnalogMic2Gain 0x0D15, Bits 6:6, typedef low_power_micro_enable
//   
//  
#define LOW_POWER_MICRO_2_MASK 0x40U
#define LOW_POWER_MICRO_2_OFFSET 6
#define LOW_POWER_MICRO_2_STOP_BIT 6
#define LOW_POWER_MICRO_2_START_BIT 6
#define LOW_POWER_MICRO_2_WIDTH 1

#define ANALOG_MIC_2_GAIN_REG 0xD15

#define LOW_POWER_MICRO_2_PARAM_MASK  0x40



//  
//  0: Differential microphone is connected to MIC2 inputs , 
//  1: Single ended microphone is connected to MIC2 inputs.
//  Register AnalogMic2Gain 0x0D15, Bits 7:7, typedef single_or_differential
//   
//  
#define MIC_2_SINGLE_ENDED_CONF_MASK 0x80U
#define MIC_2_SINGLE_ENDED_CONF_OFFSET 7
#define MIC_2_SINGLE_ENDED_CONF_STOP_BIT 7
#define MIC_2_SINGLE_ENDED_CONF_START_BIT 7
#define MIC_2_SINGLE_ENDED_CONF_WIDTH 1


#define MIC_2_SINGLE_ENDED_CONF_PARAM_MASK  0x80


#define RIGHT_HEADSET_ANALOG_GAIN_PARAM_MASK  0xF
#define RIGHT_HEADSET_ANALOG_GAIN_MASK 0xFU
#define RIGHT_HEADSET_ANALOG_GAIN_OFFSET 3
#define RIGHT_HEADSET_ANALOG_GAIN_STOP_BIT 3
#define RIGHT_HEADSET_ANALOG_GAIN_START_BIT 0
#define RIGHT_HEADSET_ANALOG_GAIN_WIDTH 4

typedef enum headset_analog_gain {
    HEADSET_ANALOG_GAIN_4_DB = 0x0,
    HEADSET_ANALOG_GAIN_2_DB = 0x1,
    HEADSET_ANALOG_GAIN_0_DB = 0x2,
    HEADSET_ANALOG_GAIN_MINUS_2_DB = 0x3,
    HEADSET_ANALOG_GAIN_MINUS_4_DB = 0x4,
    HEADSET_ANALOG_GAIN_MINUS_6_DB = 0x5,
    HEADSET_ANALOG_GAIN_MINUS_8_DB = 0x6,
    HEADSET_ANALOG_GAIN_MINUS_10_DB = 0x7,
    HEADSET_ANALOG_GAIN_MINUS_12_DB = 0x8,
    HEADSET_ANALOG_GAIN_MINUS_14_DB = 0x9,
    HEADSET_ANALOG_GAIN_MINUS_16_DB = 0xA,
    HEADSET_ANALOG_GAIN_MINUS_18_DB = 0xB,
    HEADSET_ANALOG_GAIN_MINUS_20_DB = 0xC,
    HEADSET_ANALOG_GAIN_MINUS_24_DB = 0xD,
    HEADSET_ANALOG_GAIN_MINUS_28_DB = 0xE,
    HEADSET_ANALOG_GAIN_MINUS_32_DB = 0xF
} HEADSET_ANALOG_GAIN_T ;
#define LEFT_HEADSET_ANALOG_GAIN_PARAM_MASK  0xF0
#define LEFT_HEADSET_ANALOG_GAIN_MASK 0xF0U
#define LEFT_HEADSET_ANALOG_GAIN_OFFSET 7
#define LEFT_HEADSET_ANALOG_GAIN_STOP_BIT 7
#define LEFT_HEADSET_ANALOG_GAIN_START_BIT 4
#define LEFT_HEADSET_ANALOG_GAIN_WIDTH 4

#define LEFT_LINE_IN_GAIN_REG 0xD16
#define RIGHT_LINE_ANALOG_GAIN_PARAM_MASK  0xF
#define RIGHT_LINE_ANALOG_GAIN_MASK 0xFU
#define RIGHT_LINE_ANALOG_GAIN_OFFSET 3
#define RIGHT_LINE_ANALOG_GAIN_STOP_BIT 3
#define RIGHT_LINE_ANALOG_GAIN_START_BIT 0
#define RIGHT_LINE_ANALOG_GAIN_WIDTH 4

typedef enum line_in_gain {
    LINE_ANALOG_GAIN_MINUS_10_DB = 0x0,
    LINE_ANALOG_GAIN_MINUS_8_DB = 0x1,
    LINE_ANALOG_GAIN_MINUS_6_DB = 0x2,
    LINE_ANALOG_GAIN_MINUS_4_DB = 0x3,
    LINE_ANALOG_GAIN_MINUS_2_DB = 0x4,
    LINE_ANALOG_GAIN_0_DB = 0x5,
    LINE_ANALOG_GAIN_2_DB = 0x6,
    LINE_ANALOG_GAIN_4_DB = 0x7,
    LINE_ANALOG_GAIN_6_DB = 0x8,
    LINE_ANALOG_GAIN_8_DB = 0x9,
    LINE_ANALOG_GAIN_10_DB = 0xA,
    LINE_ANALOG_GAIN_12_DB = 0xB,
    LINE_ANALOG_GAIN_14_DB = 0xC,
    LINE_ANALOG_GAIN_16_DB = 0xD,
    LINE_ANALOG_GAIN_18_DB = 0xE,
    LINE_ANALOG_GAIN_20_DB = 0xF
} LINE_IN_GAIN_T ;
#define LEFT_LINE_ANALOG_GAIN_PARAM_MASK  0xF0
#define LEFT_LINE_ANALOG_GAIN_MASK 0xF0U
#define LEFT_LINE_ANALOG_GAIN_OFFSET 7
#define LEFT_LINE_ANALOG_GAIN_STOP_BIT 7
#define LEFT_LINE_ANALOG_GAIN_START_BIT 4
#define LEFT_LINE_ANALOG_GAIN_WIDTH 4

#define RIGHT_LINE_IN_GAIN_REG 0xD17
#define LEFT_LINE_IN_TO_HEADSET_GAIN_PARAM_MASK  0x1F
#define LEFT_LINE_IN_TO_HEADSET_GAIN_MASK 0x1FU
#define LEFT_LINE_IN_TO_HEADSET_GAIN_OFFSET 4
#define LEFT_LINE_IN_TO_HEADSET_GAIN_STOP_BIT 4
#define LEFT_LINE_IN_TO_HEADSET_GAIN_START_BIT 0
#define LEFT_LINE_IN_TO_HEADSET_GAIN_WIDTH 5

typedef enum line_in_to_head_set {
    LINE_IN_TO_HEADSET_0_DB = 0x0,
    LINE_IN_TO_HEADSET_MINUS_2_DB = 0x1,
    LINE_IN_TO_HEADSET_MINUS_4_DB = 0x2,
    LINE_IN_TO_HEADSET_MINUS_6_DB = 0x3,
    LINE_IN_TO_HEADSET_MINUS_8_DB = 0x4,
    LINE_IN_TO_HEADSET_MINUS_10_DB = 0x5,
    LINE_IN_TO_HEADSET_MINUS_12_DB = 0x6,
    LINE_IN_TO_HEADSET_MINUS_14_DB = 0x7,
    LINE_IN_TO_HEADSET_MINUS_16_DB = 0x8,
    LINE_IN_TO_HEADSET_MINUS_18_DB = 0x9,
    LINE_IN_TO_HEADSET_MINUS_20_DB = 0xA,
    LINE_IN_TO_HEADSET_MINUS_22_DB = 0xB,
    LINE_IN_TO_HEADSET_MINUS_24_DB = 0xC,
    LINE_IN_TO_HEADSET_MINUS_26_DB = 0xD,
    LINE_IN_TO_HEADSET_MINUS_28_DB = 0xE,
    LINE_IN_TO_HEADSET_MINUS_30_DB = 0xF,
    LINE_IN_TO_HEADSET_MINUS_32_DB = 0x10,
    LINE_IN_TO_HEADSET_MINUS_34_DB = 0x11,
    LINE_IN_TO_HEADSET_MINUS_36_DB = 0x12,
    LINE_IN_TO_HEADSET_MINUS_38_DB = 0x13,
    LINE_IN_TO_HEADSET_MINUS_40_DB = 0x14
} LINE_IN_TO_HEAD_SET_T ;
#define LEFT_LINE_IN_TO_HEADSET_GAIN_REG 0xD18
#define RIGHT_LINE_IN_TO_HEADSET_GAIN_PARAM_MASK  0x1F
#define RIGHT_LINE_IN_TO_HEADSET_GAIN_MASK 0x1FU
#define RIGHT_LINE_IN_TO_HEADSET_GAIN_OFFSET 4
#define RIGHT_LINE_IN_TO_HEADSET_GAIN_STOP_BIT 4
#define RIGHT_LINE_IN_TO_HEADSET_GAIN_START_BIT 0
#define RIGHT_LINE_IN_TO_HEADSET_GAIN_WIDTH 5

#define RIGHT_LINE_IN_TO_HEADSET_GAIN_REG 0xD19

//  
//  0: AD4 channel path use Audio Filters 
//  1: AD4 channel path use 48kHz Low Latency Filters for Voice.
//  Register AudioVoiceFilters 0x0D1A, Bits 0:0, typedef ad4_voice_filter_enable
//   
//  
#define AD_4_VOICE_FILTER_MASK 0x1U
#define AD_4_VOICE_FILTER_OFFSET 0
#define AD_4_VOICE_FILTER_STOP_BIT 0
#define AD_4_VOICE_FILTER_START_BIT 0
#define AD_4_VOICE_FILTER_WIDTH 1

#define AUDIO_VOICE_FILTERS_REG 0xD1A

typedef enum ad4_voice_filter_enable {
    AD_4_VOICE_FILTER_DISABLE_E,
    AD_4_VOICE_FILTER_ENABLE_E
} AD_4_VOICE_FILTER_ENABLE_T ;
#define AD_4_VOICE_FILTER_PARAM_MASK  0x1



//  
//  0: AD3 channel path use Audio Filters 
//  1: AD3 channel path use 48kHz Low Latency Filters for Voice.
//  Register AudioVoiceFilters 0x0D1A, Bits 1:1, typedef ad3_voice_filter_enable
//   
//  
#define AD_3_VOICE_FILTER_MASK 0x2U
#define AD_3_VOICE_FILTER_OFFSET 1
#define AD_3_VOICE_FILTER_STOP_BIT 1
#define AD_3_VOICE_FILTER_START_BIT 1
#define AD_3_VOICE_FILTER_WIDTH 1


typedef enum ad3_voice_filter_enable {
    AD_3_VOICE_FILTER_DISABLE_E,
    AD_3_VOICE_FILTER_ENABLE_E
} AD_3_VOICE_FILTER_ENABLE_T ;
#define AD_3_VOICE_FILTER_PARAM_MASK  0x2



//  
//  0: AD2 channel path use Audio Filters 
//  1: AD2 channel path use 48kHz Low Latency Filters for Voice.
//  Register AudioVoiceFilters 0x0D1A, Bits 2:2, typedef ad2_voice_filter_enable
//   
//  
#define AD_2_VOICE_FILTER_MASK 0x4U
#define AD_2_VOICE_FILTER_OFFSET 2
#define AD_2_VOICE_FILTER_STOP_BIT 2
#define AD_2_VOICE_FILTER_START_BIT 2
#define AD_2_VOICE_FILTER_WIDTH 1


typedef enum ad2_voice_filter_enable {
    AD_2_VOICE_FILTER_DISABLE_E,
    AD_2_VOICE_FILTER_ENABLE_E
} AD_2_VOICE_FILTER_ENABLE_T ;
#define AD_2_VOICE_FILTER_PARAM_MASK  0x4



//  
//  0: AD1 channel path use Audio Filters 
//  1: AD1 channel path use 48kHz Low Latency Filters for Voice.
//  Register AudioVoiceFilters 0x0D1A, Bits 3:3, typedef ad1_voice_filter_enable
//   
//  
#define AD_1_VOICE_FILTER_MASK 0x8U
#define AD_1_VOICE_FILTER_OFFSET 3
#define AD_1_VOICE_FILTER_STOP_BIT 3
#define AD_1_VOICE_FILTER_START_BIT 3
#define AD_1_VOICE_FILTER_WIDTH 1


typedef enum ad1_voice_filter_enable {
    AD_1_VOICE_FILTER_DISABLE_E,
    AD_1_VOICE_FILTER_ENABLE_E
} AD_1_VOICE_FILTER_ENABLE_T ;
#define AD_1_VOICE_FILTER_PARAM_MASK  0x8



//  
//  0: AD4 Path with audio Offset Cancellation filter enabled 
//  1: AD4 Path with audio Offset Cancellation filter disabled.
//  Register AudioVoiceFilters 0x0D1A, Bits 4:4, typedef ad4_offset_cancel
//   
//  
#define AD_4_OFFSET_CANCEL_MASK 0x10U
#define AD_4_OFFSET_CANCEL_OFFSET 4
#define AD_4_OFFSET_CANCEL_STOP_BIT 4
#define AD_4_OFFSET_CANCEL_START_BIT 4
#define AD_4_OFFSET_CANCEL_WIDTH 1


typedef enum ad4_offset_cancel {
    AD4_OFFSET_ENABLE_E,
    AD4_OFFSET_CANCEL_E
} AD4_OFFSET_CANCEL_T ;
#define AD_4_OFFSET_CANCEL_PARAM_MASK  0x10



//  
//  0: AD3 Path with audio Offset Cancellation filter enabled 
//  1: AD3 Path with audio Offset Cancellation filter disabled.
//  Register AudioVoiceFilters 0x0D1A, Bits 5:5, typedef ad3_offset_cancel
//   
//  
#define AD_3_OFFSET_CANCEL_MASK 0x20U
#define AD_3_OFFSET_CANCEL_OFFSET 5
#define AD_3_OFFSET_CANCEL_STOP_BIT 5
#define AD_3_OFFSET_CANCEL_START_BIT 5
#define AD_3_OFFSET_CANCEL_WIDTH 1


typedef enum ad3_offset_cancel {
    AD3_OFFSET_ENABLE_E,
    AD3_OFFSET_CANCEL_E
} AD3_OFFSET_CANCEL_T ;
#define AD_3_OFFSET_CANCEL_PARAM_MASK  0x20



//  
//  0: AD2 Path with audio Offset Cancellation filter enabled 
//  1: AD2 Path with audio Offset Cancellation filter disabled.
//  Register AudioVoiceFilters 0x0D1A, Bits 6:6, typedef ad2_offset_cancel
//   
//  
#define AD_2_OFFSET_CANCEL_MASK 0x40U
#define AD_2_OFFSET_CANCEL_OFFSET 6
#define AD_2_OFFSET_CANCEL_STOP_BIT 6
#define AD_2_OFFSET_CANCEL_START_BIT 6
#define AD_2_OFFSET_CANCEL_WIDTH 1


typedef enum ad2_offset_cancel {
    AD2_OFFSET_ENABLE_E,
    AD2_OFFSET_CANCEL_E
} AD2_OFFSET_CANCEL_T ;
#define AD_2_OFFSET_CANCEL_PARAM_MASK  0x40



//  
//  0: AD1 Path with audio Offset Cancellation filter enabled 
//  1: AD1 Path with audio Offset Cancellation filter disabled.
//  Register AudioVoiceFilters 0x0D1A, Bits 7:7, typedef ad1_offset_cancel
//   
//  
#define AD_1_OFFSET_CANCEL_MASK 0x80U
#define AD_1_OFFSET_CANCEL_OFFSET 7
#define AD_1_OFFSET_CANCEL_STOP_BIT 7
#define AD_1_OFFSET_CANCEL_START_BIT 7
#define AD_1_OFFSET_CANCEL_WIDTH 1


typedef enum ad1_offset_cancel {
    AD1_OFFSET_ENABLE_E,
    AD1_OFFSET_CANCEL_E
} AD1_OFFSET_CANCEL_T ;
#define AD_1_OFFSET_CANCEL_PARAM_MASK  0x80



//  
//  0: The FSync0 and BitClk0 bits are stopped; 
//  1: The FSync0 and BitClk0 bits are enabled.
//  Register TdmInterface 0x0D1B, Bits 0:0, typedef bit_clock0_enable
//   
//  
#define BIT_CLOCK_0_MASK 0x1U
#define BIT_CLOCK_0_OFFSET 0
#define BIT_CLOCK_0_STOP_BIT 0
#define BIT_CLOCK_0_START_BIT 0
#define BIT_CLOCK_0_WIDTH 1

#define TDM_INTERFACE_REG 0xD1B

typedef enum bit_clock0_enable {
    BIT_CLOCK_0_DISABLE_E,
    BIT_CLOCK_0_ENABLE_E
} BIT_CLOCK_0_ENABLE_T ;
#define BIT_CLOCK_0_PARAM_MASK  0x1



//  
//  BitClk0 = 48kHz x N; 00: N = 32 ; 01: N = 64 ; 10: N = 128; 11: N = 256.
//  Register TdmInterface 0x0D1B, Bits 1:2, typedef tdm_bit_clock
//   
//  
#define BIT_CLOCK_0_SETTING_MASK 0x6U
#define BIT_CLOCK_0_SETTING_OFFSET 2
#define BIT_CLOCK_0_SETTING_STOP_BIT 2
#define BIT_CLOCK_0_SETTING_START_BIT 1
#define BIT_CLOCK_0_SETTING_WIDTH 2

#define  CLK_48KHZ_32 0
#define  CLK_48KHZ_64 1
#define  CLK_48KHZ_128 2
#define  CLK_48KHZ_256 3

typedef enum tdm_bit_clock {
    CLK_48KHZ_32_E,
    CLK_48KHZ_64_E,
    CLK_48KHZ_128_E,
    CLK_48KHZ_256_E
} TDM_BIT_CLOCK_T ;
#define BIT_CLOCK_0_SETTING_PARAM_MASK  0x6



//  
//  0: The FSync1 and BitClk1 bits are stopped; 
//  1: The FSync1 and BitClk1 bits are enabled.
//  Register TdmInterface 0x0D1B, Bits 4:4, typedef bit_clock1_enable
//   
//  
#define BIT_CLOCK_1_MASK 0x10U
#define BIT_CLOCK_1_OFFSET 4
#define BIT_CLOCK_1_STOP_BIT 4
#define BIT_CLOCK_1_START_BIT 4
#define BIT_CLOCK_1_WIDTH 1


typedef enum bit_clock1_enable {
    BIT_CLOCK_1_DISABLE_E,
    BIT_CLOCK_1_ENABLE_E
} BIT_CLOCK_1_ENABLE_T ;
#define BIT_CLOCK_1_PARAM_MASK  0x10



//  
//  BitClk1 = 48kHz x N; 00: N = 32 ; 01: N = 64 ; 10: N = 128; 11: N = 256.
//  Register TdmInterface 0x0D1B, Bits 5:6, typedef tdm_bit_clock
//   
//  
#define BIT_CLOCK_1_SETTING_MASK 0x60U
#define BIT_CLOCK_1_SETTING_OFFSET 6
#define BIT_CLOCK_1_SETTING_STOP_BIT 6
#define BIT_CLOCK_1_SETTING_START_BIT 5
#define BIT_CLOCK_1_SETTING_WIDTH 2

#define  CLK_48KHZ_32 0
#define  CLK_48KHZ_64 1
#define  CLK_48KHZ_128 2
#define  CLK_48KHZ_256 3

#define BIT_CLOCK_1_SETTING_PARAM_MASK  0x60



//  
//  0: The Master Generator for AUD_IF1/0 is stopped, 
//  1: The Master Generator for AUD_IF1/0 is active.
//  Register TdmInterface 0x0D1B, Bits 7:7, typedef master0_mode_enable
//   
//  
#define MASTER_0_MODE_MASK 0x80U
#define MASTER_0_MODE_OFFSET 7
#define MASTER_0_MODE_STOP_BIT 7
#define MASTER_0_MODE_START_BIT 7
#define MASTER_0_MODE_WIDTH 1


typedef enum master0_mode_enable {
    MASTER_0_MODE_DISABLE_E,
    MASTER_0_MODE_ENABLE_E
} MASTER_0_MODE_ENABLE_T ;
#define MASTER_0_MODE_PARAM_MASK  0x80



//  
//  00: IF0 Word Length is 16 bits; 
//  01: IF0 Word Length is 20 bits; 
//  10: IF0 Word Length is 24 bits, 
//  11: IF0 Word Length is 32 bit.
//  Register TdmConfiguration 0x0D1C, Bits 0:1, typedef word_length
//   
//  
#define IF_0_WORD_LENGTH_MASK 0x3U
#define IF_0_WORD_LENGTH_OFFSET 1
#define IF_0_WORD_LENGTH_STOP_BIT 1
#define IF_0_WORD_LENGTH_START_BIT 0
#define IF_0_WORD_LENGTH_WIDTH 2

#define TDM_CONFIGURATION_REG 0xD1C
#define  WORD_LENGHT_16_BITS 0
#define  WORD_LENGHT_20_BITS 1
#define  WORD_LENGHT_24_BITS 2
#define  WORD_LENGHT_32_BITS 3

typedef enum word_length {
    WORD_LENGHT_16_BITS_E,
    WORD_LENGHT_20_BITS_E,
    WORD_LENGHT_24_BITS_E,
    WORD_LENGHT_32_BITS_E
} WORD_LENGTH_T ;
#define IF_0_WORD_LENGTH_PARAM_MASK  0x3



//  
//  0: IF0 is disabled (no data is read from the interface). 
//  1: IF0 format is TDM, 1x: IF0 format is I2S Left Aligned.
//  Register TdmConfiguration 0x0D1C, Bits 2:3, typedef format_mode
//   
//  
#define IF_0_MODE_MASK 0xCU
#define IF_0_MODE_OFFSET 3
#define IF_0_MODE_STOP_BIT 3
#define IF_0_MODE_START_BIT 2
#define IF_0_MODE_WIDTH 2

#define  DISABLE 0
#define  TDM 1
#define  I2S_LEFT_ALIGNED 2

typedef enum format_mode {
    DISABLE_E,
    TDM_E,
    I2S_LEFT_ALIGNED_E
} FORMAT_MODE_T ;
#define IF_0_MODE_PARAM_MASK  0xC



//  
//  0: IF0 format is not delayed; 
//  1: IF0 format is delayed.
//  Register TdmConfiguration 0x0D1C, Bits 4:4, typedef delayed
//   
//  
#define IF_0_DELAYED_MASK 0x10U
#define IF_0_DELAYED_OFFSET 4
#define IF_0_DELAYED_STOP_BIT 4
#define IF_0_DELAYED_START_BIT 4
#define IF_0_DELAYED_WIDTH 1


typedef enum delayed {
    NOT_DELAYED_E,
    DELAYED_E
} DELAYED_T ;
#define IF_0_DELAYED_PARAM_MASK  0x10



//  
//  0: The input bits of AUD_IF0 are sampled with BitClk rising edge; 
//  1: The input bits of AUD_IF0 are sampled with BitClk falling edge.
//  Register TdmConfiguration 0x0D1C, Bits 5:5, typedef input0_bit_sampled
//   
//  
#define INPUT_0_BIT_SAMPLED_MASK 0x20U
#define INPUT_0_BIT_SAMPLED_OFFSET 5
#define INPUT_0_BIT_SAMPLED_STOP_BIT 5
#define INPUT_0_BIT_SAMPLED_START_BIT 5
#define INPUT_0_BIT_SAMPLED_WIDTH 1


typedef enum input0_bit_sampled {
    BIT_CLOCK0_RISING_EDGE_E,
    BIT_CLOCK0_FALLING_EDGE_E
} INPUT_0_BIT_SAMPLED_T ;
#define INPUT_0_BIT_SAMPLED_PARAM_MASK  0x20



//  
//  0: The first slot of AUD_IF0 is synchronized with Fsync rising edge; 
//  1: The first slot of AUD_IF0 is synchronized with Fsync falling edge.
//  Register TdmConfiguration 0x0D1C, Bits 6:6, typedef first_slot_synchro0
//   
//  
#define FIRST_SLOT_0_SYNCHRO_MASK 0x40U
#define FIRST_SLOT_0_SYNCHRO_OFFSET 6
#define FIRST_SLOT_0_SYNCHRO_STOP_BIT 6
#define FIRST_SLOT_0_SYNCHRO_START_BIT 6
#define FIRST_SLOT_0_SYNCHRO_WIDTH 1


typedef enum first_slot_synchro0 {
    FSYNC_RISING_EDGE_E,
    FSYNC_FALLING_EDGE_E
} FIRST_SLOT_SYNCHRO_0_T ;
#define FIRST_SLOT_0_SYNCHRO_PARAM_MASK  0x40



//  
//  0: TDM Interface IF0 is configured in normal mode; 
//  1: TDM Interface IF0 is configured in burst mode.
//  Register TdmLoopbackControl 0x0D1D, Bits 0:0, typedef burst_mode_enable
//   
//  
#define BURST_MODE_MASK 0x1U
#define BURST_MODE_OFFSET 0
#define BURST_MODE_STOP_BIT 0
#define BURST_MODE_START_BIT 0
#define BURST_MODE_WIDTH 1

#define TDM_LOOPBACK_CONTROL_REG 0xD1D

typedef enum burst_mode_enable {
    BURST_MODE_DISABLE_E,
    BURST_MODE_ENABLE_E
} BURST_MODE_ENABLE_T ;
#define BURST_MODE_PARAM_MASK  0x1



//  
//  0: Pins FSync0 and BitClk0 are set as input ; 
//  1: Pins FSync0 and BitClk0 are set as output.
//  Register TdmLoopbackControl 0x0D1D, Bits 1:1, typedef master_clock_if0_enable
//   
//  
#define MASTER_CLOCK_IF_0_MASK 0x2U
#define MASTER_CLOCK_IF_0_OFFSET 1
#define MASTER_CLOCK_IF_0_STOP_BIT 1
#define MASTER_CLOCK_IF_0_START_BIT 1
#define MASTER_CLOCK_IF_0_WIDTH 1


typedef enum master_clock_if0_enable {
    MASTER_CLOCK_IF_0_DISABLE_E,
    MASTER_CLOCK_IF_0_ENABLE_E
} MASTER_CLOCK_IF_0_ENABLE_T ;
#define MASTER_CLOCK_IF_0_PARAM_MASK  0x2



//  
//  0: BITCLK0, FSYNC0 pins are used for the default purpose, 
//  1: Clocks on BITCLK1,FSYNC1 pins are sent to BITCLK0, FSYNC0.
//  Register TdmLoopbackControl 0x0D1D, Bits 2:2, typedef if1_on_if0_enable
//   
//  
#define IF_1_ON_IF_0_MASK 0x4U
#define IF_1_ON_IF_0_OFFSET 2
#define IF_1_ON_IF_0_STOP_BIT 2
#define IF_1_ON_IF_0_START_BIT 2
#define IF_1_ON_IF_0_WIDTH 1


typedef enum if1_on_if0_enable {
    IF_1_ON_IF_0_DISABLE_E,
    IF_1_ON_IF_0_ENABLE_E
} IF_1_ON_IF_0_ENABLE_T ;
#define IF_1_ON_IF_0_PARAM_MASK  0x4



//  
//  0: AD_DATA0 pin is used for the default purpose ; 
//  1: Data on DA_DATA1 pin is sent to AD_DATA0 pin.
//  Register TdmLoopbackControl 0x0D1D, Bits 3:3, typedef da_data1_to_ad_data0_enable
//   
//  
#define DA_DATA_1_TO_AD_DATA_0_MASK 0x8U
#define DA_DATA_1_TO_AD_DATA_0_OFFSET 3
#define DA_DATA_1_TO_AD_DATA_0_STOP_BIT 3
#define DA_DATA_1_TO_AD_DATA_0_START_BIT 3
#define DA_DATA_1_TO_AD_DATA_0_WIDTH 1


typedef enum da_data1_to_ad_data0_enable {
    DA_DATA_1_TO_AD_DATA_0_DISABLE_E,
    DA_DATA_1_TO_AD_DATA_0_ENABLE_E
} DA_DATA_1_TO_AD_DATA_0_ENABLE_T ;
#define DA_DATA_1_TO_AD_DATA_0_PARAM_MASK  0x8



//  
//  0: Pins FSync1 and BitClk1 are set as input ; 
//  1: Pins FSync1 and BitClk1 are set as output.
//  Register TdmLoopbackControl 0x0D1D, Bits 5:5, typedef master_clock_if1_enable
//   
//  
#define MASTER_CLOCK_IF_1_MASK 0x20U
#define MASTER_CLOCK_IF_1_OFFSET 5
#define MASTER_CLOCK_IF_1_STOP_BIT 5
#define MASTER_CLOCK_IF_1_START_BIT 5
#define MASTER_CLOCK_IF_1_WIDTH 1


typedef enum master_clock_if1_enable {
    MASTER_CLOCK_IF_1_DISABLE_E,
    MASTER_CLOCK_IF_1_ENABLE_E
} MASTER_CLOCK_IF_1_ENABLE_T ;
#define MASTER_CLOCK_IF_1_PARAM_MASK  0x20



//  
//  0: BITCLK1, FSYNC1 pins are used for the default purpose, 
//  1: Clocks on BITCLK0,FSYNC0 pins are sent to BITCLK1, FSYNC1.
//  Register TdmLoopbackControl 0x0D1D, Bits 6:6, typedef if0_on_if1_enable
//   
//  
#define IF_0_ON_IF_1_MASK 0x40U
#define IF_0_ON_IF_1_OFFSET 6
#define IF_0_ON_IF_1_STOP_BIT 6
#define IF_0_ON_IF_1_START_BIT 6
#define IF_0_ON_IF_1_WIDTH 1


typedef enum if0_on_if1_enable {
    IF_0_ON_IF_1_DISABLE_E,
    IF_0_ON_IF_1_ENABLE_E
} IF_0_ON_IF_1_ENABLE_T ;
#define IF_0_ON_IF_1_PARAM_MASK  0x40



//  
//  0: AD_DATA1 pin is used for the default purpose ; 
//  1: Data on DA_DATA0 pin is sent to AD_DATA1 pin.
//  Register TdmLoopbackControl 0x0D1D, Bits 7:7, typedef da_data0_to_ad_data1_enable
//   
//  
#define DA_DATA_0_TO_AD_DATA_1_MASK 0x80U
#define DA_DATA_0_TO_AD_DATA_1_OFFSET 7
#define DA_DATA_0_TO_AD_DATA_1_STOP_BIT 7
#define DA_DATA_0_TO_AD_DATA_1_START_BIT 7
#define DA_DATA_0_TO_AD_DATA_1_WIDTH 1


typedef enum da_data0_to_ad_data1_enable {
    DA_DATA_0_TO_AD_DATA_1_DISABLE_E,
    DA_DATA_0_TO_AD_DATA_1_ENABLE_E
} DA_DATA_0_TO_AD_DATA_1_ENABLE_T ;
#define DA_DATA_0_TO_AD_DATA_1_PARAM_MASK  0x80



//  
//  00: IF1 Word Length is 16 bits; 
//  01: IF1 Word Length is 20 bits; 
//  10: IF1 Word Length is 24 bits, 
//  11: IF1 Word Length is 32 bit.
//  Register TdmConfiguration1 0x0D1E, Bits 0:1, typedef word_length
//   
//  
#define IF_1_WORD_LENGTH_MASK 0x3U
#define IF_1_WORD_LENGTH_OFFSET 1
#define IF_1_WORD_LENGTH_STOP_BIT 1
#define IF_1_WORD_LENGTH_START_BIT 0
#define IF_1_WORD_LENGTH_WIDTH 2

#define TDM_CONFIGURATION_1_REG 0xD1E
#define  WORD_LENGHT_16_BITS 0
#define  WORD_LENGHT_20_BITS 1
#define  WORD_LENGHT_24_BITS 2
#define  WORD_LENGHT_32_BITS 3

#define IF_1_WORD_LENGTH_PARAM_MASK  0x3



//  
//  0: IF1 is disabled (no data is read from the interface). 
//  1: IF1 format is TDM, 1x: IF1 format is I2S Left Aligned.
//  Register TdmConfiguration1 0x0D1E, Bits 3:3, typedef format_mode
//   
//  
#define IF_1_MODE_MASK 0x8U
#define IF_1_MODE_OFFSET 3
#define IF_1_MODE_STOP_BIT 3
#define IF_1_MODE_START_BIT 3
#define IF_1_MODE_WIDTH 1

#define  DISABLE 0
#define  TDM 1
#define  I2S_LEFT_ALIGNED 2

#define IF_1_MODE_PARAM_MASK  0x8



//  
//  0: IF1 format is not delayed; 
//  1: IF1 format is delayed.
//  Register TdmConfiguration1 0x0D1E, Bits 4:4, typedef delayed
//   
//  
#define IF_1_DELAYED_MASK 0x10U
#define IF_1_DELAYED_OFFSET 4
#define IF_1_DELAYED_STOP_BIT 4
#define IF_1_DELAYED_START_BIT 4
#define IF_1_DELAYED_WIDTH 1


#define IF_1_DELAYED_PARAM_MASK  0x10



//  
//  0: The input bits of AUD_IF1 are sampled with BitClk rising edge; 
//  1: The input bits of AUD_IF1 are sampled with BitClk falling edge.
//  Register TdmConfiguration1 0x0D1E, Bits 5:5, typedef input1_bit_sampled
//   
//  
#define INPUT_1_BIT_SAMPLED_MASK 0x20U
#define INPUT_1_BIT_SAMPLED_OFFSET 5
#define INPUT_1_BIT_SAMPLED_STOP_BIT 5
#define INPUT_1_BIT_SAMPLED_START_BIT 5
#define INPUT_1_BIT_SAMPLED_WIDTH 1


typedef enum input1_bit_sampled {
    BIT_CLOCK1_RISING_EDGE_E,
    BIT_CLOCK1_FALLING_EDGE_E
} INPUT_1_BIT_SAMPLED_T ;
#define INPUT_1_BIT_SAMPLED_PARAM_MASK  0x20



//  
//  1: The first slot of AUD_IF1 is synchronized with Fsync rising edge; 
//  0: The first slot of AUD_IF1 is synchronized with Fsync falling edge.
//  Register TdmConfiguration1 0x0D1E, Bits 6:6, typedef first_slot_synchro0
//   
//  
#define FIRST_SLOT_1_SYNCHRO_MASK 0x40U
#define FIRST_SLOT_1_SYNCHRO_OFFSET 6
#define FIRST_SLOT_1_SYNCHRO_STOP_BIT 6
#define FIRST_SLOT_1_SYNCHRO_START_BIT 6
#define FIRST_SLOT_1_SYNCHRO_WIDTH 1


#define FIRST_SLOT_1_SYNCHRO_PARAM_MASK  0x40



//  
//  Slot Output State.
//  Register SlotAllocation0 0x0D1F, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_1_MASK 0xF0U
#define SLOT_OUTPUT_1_OFFSET 7
#define SLOT_OUTPUT_1_STOP_BIT 7
#define SLOT_OUTPUT_1_START_BIT 4
#define SLOT_OUTPUT_1_WIDTH 4

#define SLOT_ALLOCATION_0_REG 0xD1F
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

typedef enum slot_output {
    SLOT_OUT_FROM_AD_OUT1_E,
    SLOT_OUT_FROM_AD_OUT2_E,
    SLOT_OUT_FROM_AD_OUT3_E,
    SLOT_OUT_FROM_AD_OUT4_E,
    SLOT_OUT_FROM_AD_OUT5_E,
    SLOT_OUT_FROM_AD_OUT6_E,
    SLOT_OUT_FROM_AD_OUT7_E,
    SLOT_OUT_FROM_AD_OUT8_E,
    SLOT_OUT_TRISTATE_E,
    SLOT_OUT_ZEROS_E
} SLOT_OUTPUT_T ;
#define SLOT_OUTPUT_1_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation0 0x0D1F, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_0_MASK 0xFU
#define SLOT_OUTPUT_0_OFFSET 3
#define SLOT_OUTPUT_0_STOP_BIT 3
#define SLOT_OUTPUT_0_START_BIT 0
#define SLOT_OUTPUT_0_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_0_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation2 0x0D20, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_3_MASK 0xF0U
#define SLOT_OUTPUT_3_OFFSET 7
#define SLOT_OUTPUT_3_STOP_BIT 7
#define SLOT_OUTPUT_3_START_BIT 4
#define SLOT_OUTPUT_3_WIDTH 4

#define SLOT_ALLOCATION_2_REG 0xD20
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_3_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation2 0x0D20, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_2_MASK 0xFU
#define SLOT_OUTPUT_2_OFFSET 3
#define SLOT_OUTPUT_2_STOP_BIT 3
#define SLOT_OUTPUT_2_START_BIT 0
#define SLOT_OUTPUT_2_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_2_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation4 0x0D21, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_5_MASK 0xF0U
#define SLOT_OUTPUT_5_OFFSET 7
#define SLOT_OUTPUT_5_STOP_BIT 7
#define SLOT_OUTPUT_5_START_BIT 4
#define SLOT_OUTPUT_5_WIDTH 4

#define SLOT_ALLOCATION_4_REG 0xD21
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_5_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation4 0x0D21, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_4_MASK 0xFU
#define SLOT_OUTPUT_4_OFFSET 3
#define SLOT_OUTPUT_4_STOP_BIT 3
#define SLOT_OUTPUT_4_START_BIT 0
#define SLOT_OUTPUT_4_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_4_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation6 0x0D22, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_7_MASK 0xF0U
#define SLOT_OUTPUT_7_OFFSET 7
#define SLOT_OUTPUT_7_STOP_BIT 7
#define SLOT_OUTPUT_7_START_BIT 4
#define SLOT_OUTPUT_7_WIDTH 4

#define SLOT_ALLOCATION_6_REG 0xD22
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_7_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation6 0x0D22, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_6_MASK 0xFU
#define SLOT_OUTPUT_6_OFFSET 3
#define SLOT_OUTPUT_6_STOP_BIT 3
#define SLOT_OUTPUT_6_START_BIT 0
#define SLOT_OUTPUT_6_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_6_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation8 0x0D23, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_9_MASK 0xF0U
#define SLOT_OUTPUT_9_OFFSET 7
#define SLOT_OUTPUT_9_STOP_BIT 7
#define SLOT_OUTPUT_9_START_BIT 4
#define SLOT_OUTPUT_9_WIDTH 4

#define SLOT_ALLOCATION_8_REG 0xD23
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_9_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation8 0x0D23, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_8_MASK 0xFU
#define SLOT_OUTPUT_8_OFFSET 3
#define SLOT_OUTPUT_8_STOP_BIT 3
#define SLOT_OUTPUT_8_START_BIT 0
#define SLOT_OUTPUT_8_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_8_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation10 0x0D24, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_11_MASK 0xF0U
#define SLOT_OUTPUT_11_OFFSET 7
#define SLOT_OUTPUT_11_STOP_BIT 7
#define SLOT_OUTPUT_11_START_BIT 4
#define SLOT_OUTPUT_11_WIDTH 4

#define SLOT_ALLOCATION_10_REG 0xD24
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_11_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation10 0x0D24, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_10_MASK 0xFU
#define SLOT_OUTPUT_10_OFFSET 3
#define SLOT_OUTPUT_10_STOP_BIT 3
#define SLOT_OUTPUT_10_START_BIT 0
#define SLOT_OUTPUT_10_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_10_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation12 0x0D25, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_13_MASK 0xF0U
#define SLOT_OUTPUT_13_OFFSET 7
#define SLOT_OUTPUT_13_STOP_BIT 7
#define SLOT_OUTPUT_13_START_BIT 4
#define SLOT_OUTPUT_13_WIDTH 4

#define SLOT_ALLOCATION_12_REG 0xD25
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_13_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation12 0x0D25, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_12_MASK 0xFU
#define SLOT_OUTPUT_12_OFFSET 3
#define SLOT_OUTPUT_12_STOP_BIT 3
#define SLOT_OUTPUT_12_START_BIT 0
#define SLOT_OUTPUT_12_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_12_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation14 0x0D26, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_15_MASK 0xF0U
#define SLOT_OUTPUT_15_OFFSET 7
#define SLOT_OUTPUT_15_STOP_BIT 7
#define SLOT_OUTPUT_15_START_BIT 4
#define SLOT_OUTPUT_15_WIDTH 4

#define SLOT_ALLOCATION_14_REG 0xD26
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_15_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation14 0x0D26, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_14_MASK 0xFU
#define SLOT_OUTPUT_14_OFFSET 3
#define SLOT_OUTPUT_14_STOP_BIT 3
#define SLOT_OUTPUT_14_START_BIT 0
#define SLOT_OUTPUT_14_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_14_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation16 0x0D27, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_17_MASK 0xF0U
#define SLOT_OUTPUT_17_OFFSET 7
#define SLOT_OUTPUT_17_STOP_BIT 7
#define SLOT_OUTPUT_17_START_BIT 4
#define SLOT_OUTPUT_17_WIDTH 4

#define SLOT_ALLOCATION_16_REG 0xD27
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_17_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation16 0x0D27, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_16_MASK 0xFU
#define SLOT_OUTPUT_16_OFFSET 3
#define SLOT_OUTPUT_16_STOP_BIT 3
#define SLOT_OUTPUT_16_START_BIT 0
#define SLOT_OUTPUT_16_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_16_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation18 0x0D28, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_19_MASK 0xF0U
#define SLOT_OUTPUT_19_OFFSET 7
#define SLOT_OUTPUT_19_STOP_BIT 7
#define SLOT_OUTPUT_19_START_BIT 4
#define SLOT_OUTPUT_19_WIDTH 4

#define SLOT_ALLOCATION_18_REG 0xD28
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_19_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation18 0x0D28, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_18_MASK 0xFU
#define SLOT_OUTPUT_18_OFFSET 3
#define SLOT_OUTPUT_18_STOP_BIT 3
#define SLOT_OUTPUT_18_START_BIT 0
#define SLOT_OUTPUT_18_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_18_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation20 0x0D29, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_21_MASK 0xF0U
#define SLOT_OUTPUT_21_OFFSET 7
#define SLOT_OUTPUT_21_STOP_BIT 7
#define SLOT_OUTPUT_21_START_BIT 4
#define SLOT_OUTPUT_21_WIDTH 4

#define SLOT_ALLOCATION_20_REG 0xD29
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_21_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation20 0x0D29, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_20_MASK 0xFU
#define SLOT_OUTPUT_20_OFFSET 3
#define SLOT_OUTPUT_20_STOP_BIT 3
#define SLOT_OUTPUT_20_START_BIT 0
#define SLOT_OUTPUT_20_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_20_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation22 0x0D2A, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_23_MASK 0xF0U
#define SLOT_OUTPUT_23_OFFSET 7
#define SLOT_OUTPUT_23_STOP_BIT 7
#define SLOT_OUTPUT_23_START_BIT 4
#define SLOT_OUTPUT_23_WIDTH 4

#define SLOT_ALLOCATION_22_REG 0xD2A
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_23_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation22 0x0D2A, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_22_MASK 0xFU
#define SLOT_OUTPUT_22_OFFSET 3
#define SLOT_OUTPUT_22_STOP_BIT 3
#define SLOT_OUTPUT_22_START_BIT 0
#define SLOT_OUTPUT_22_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_22_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation24 0x0D2B, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_25_MASK 0xF0U
#define SLOT_OUTPUT_25_OFFSET 7
#define SLOT_OUTPUT_25_STOP_BIT 7
#define SLOT_OUTPUT_25_START_BIT 4
#define SLOT_OUTPUT_25_WIDTH 4

#define SLOT_ALLOCATION_24_REG 0xD2B
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_25_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation24 0x0D2B, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_24_MASK 0xFU
#define SLOT_OUTPUT_24_OFFSET 3
#define SLOT_OUTPUT_24_STOP_BIT 3
#define SLOT_OUTPUT_24_START_BIT 0
#define SLOT_OUTPUT_24_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_24_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation26 0x0D2C, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_27_MASK 0xF0U
#define SLOT_OUTPUT_27_OFFSET 7
#define SLOT_OUTPUT_27_STOP_BIT 7
#define SLOT_OUTPUT_27_START_BIT 4
#define SLOT_OUTPUT_27_WIDTH 4

#define SLOT_ALLOCATION_26_REG 0xD2C
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_27_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation26 0x0D2C, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_26_MASK 0xFU
#define SLOT_OUTPUT_26_OFFSET 3
#define SLOT_OUTPUT_26_STOP_BIT 3
#define SLOT_OUTPUT_26_START_BIT 0
#define SLOT_OUTPUT_26_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_26_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation28 0x0D2D, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_29_MASK 0xF0U
#define SLOT_OUTPUT_29_OFFSET 7
#define SLOT_OUTPUT_29_STOP_BIT 7
#define SLOT_OUTPUT_29_START_BIT 4
#define SLOT_OUTPUT_29_WIDTH 4

#define SLOT_ALLOCATION_28_REG 0xD2D
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_29_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation28 0x0D2D, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_28_MASK 0xFU
#define SLOT_OUTPUT_28_OFFSET 3
#define SLOT_OUTPUT_28_STOP_BIT 3
#define SLOT_OUTPUT_28_START_BIT 0
#define SLOT_OUTPUT_28_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_28_PARAM_MASK  0xF



//  
//  Slot Output State.
//  Register SlotAllocation30 0x0D2E, Bits 4:7, typedef slot_output
//   
//  
#define SLOT_OUTPUT_31_MASK 0xF0U
#define SLOT_OUTPUT_31_OFFSET 7
#define SLOT_OUTPUT_31_STOP_BIT 7
#define SLOT_OUTPUT_31_START_BIT 4
#define SLOT_OUTPUT_31_WIDTH 4

#define SLOT_ALLOCATION_30_REG 0xD2E
#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_31_PARAM_MASK  0xF0



//  
//  Slot Output State.
//  Register SlotAllocation30 0x0D2E, Bits 0:3, typedef slot_output
//   
//  
#define SLOT_OUTPUT_30_MASK 0xFU
#define SLOT_OUTPUT_30_OFFSET 3
#define SLOT_OUTPUT_30_STOP_BIT 3
#define SLOT_OUTPUT_30_START_BIT 0
#define SLOT_OUTPUT_30_WIDTH 4

#define  SLOT_OUT_FROM_AD_OUT1 0
#define  SLOT_OUT_FROM_AD_OUT2 1
#define  SLOT_OUT_FROM_AD_OUT3 2
#define  SLOT_OUT_FROM_AD_OUT4 3
#define  SLOT_OUT_FROM_AD_OUT5 4
#define  SLOT_OUT_FROM_AD_OUT6 5
#define  SLOT_OUT_FROM_AD_OUT7 6
#define  SLOT_OUT_FROM_AD_OUT8 7
#define  SLOT_OUT_TRISTATE 8
#define  SLOT_OUT_ZEROS 12

#define SLOT_OUTPUT_30_PARAM_MASK  0xF



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl0 0x0D2F, Bits 7:7, typedef slot_hiz
//   
//  
#define SLOT_HIZ_7_MASK 0x80U
#define SLOT_HIZ_7_OFFSET 7
#define SLOT_HIZ_7_STOP_BIT 7
#define SLOT_HIZ_7_START_BIT 7
#define SLOT_HIZ_7_WIDTH 1

#define A_DSLOT_TRISTATE_CONTROL_0_REG 0xD2F

typedef enum slot_hiz {
    SLOT_LOW_IMPEDANCE_E,
    SLOT_HIGH_IMPEDANCE_E
} SLOT_HIZ_T ;
#define SLOT_HIZ_7_PARAM_MASK  0x80



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl0 0x0D2F, Bits 6:6, typedef slot_hiz
//   
//  
#define SLOT_HIZ_6_MASK 0x40U
#define SLOT_HIZ_6_OFFSET 6
#define SLOT_HIZ_6_STOP_BIT 6
#define SLOT_HIZ_6_START_BIT 6
#define SLOT_HIZ_6_WIDTH 1


#define SLOT_HIZ_6_PARAM_MASK  0x40



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl0 0x0D2F, Bits 5:5, typedef slot_hiz
//   
//  
#define SLOT_HIZ_5_MASK 0x20U
#define SLOT_HIZ_5_OFFSET 5
#define SLOT_HIZ_5_STOP_BIT 5
#define SLOT_HIZ_5_START_BIT 5
#define SLOT_HIZ_5_WIDTH 1


#define SLOT_HIZ_5_PARAM_MASK  0x20



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl0 0x0D2F, Bits 4:4, typedef slot_hiz
//   
//  
#define SLOT_HIZ_4_MASK 0x10U
#define SLOT_HIZ_4_OFFSET 4
#define SLOT_HIZ_4_STOP_BIT 4
#define SLOT_HIZ_4_START_BIT 4
#define SLOT_HIZ_4_WIDTH 1


#define SLOT_HIZ_4_PARAM_MASK  0x10



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl0 0x0D2F, Bits 3:3, typedef slot_hiz
//   
//  
#define SLOT_HIZ_3_MASK 0x8U
#define SLOT_HIZ_3_OFFSET 3
#define SLOT_HIZ_3_STOP_BIT 3
#define SLOT_HIZ_3_START_BIT 3
#define SLOT_HIZ_3_WIDTH 1


#define SLOT_HIZ_3_PARAM_MASK  0x8



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl0 0x0D2F, Bits 2:2, typedef slot_hiz
//   
//  
#define SLOT_HIZ_2_MASK 0x4U
#define SLOT_HIZ_2_OFFSET 2
#define SLOT_HIZ_2_STOP_BIT 2
#define SLOT_HIZ_2_START_BIT 2
#define SLOT_HIZ_2_WIDTH 1


#define SLOT_HIZ_2_PARAM_MASK  0x4



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl0 0x0D2F, Bits 1:1, typedef slot_hiz
//   
//  
#define SLOT_HIZ_1_MASK 0x2U
#define SLOT_HIZ_1_OFFSET 1
#define SLOT_HIZ_1_STOP_BIT 1
#define SLOT_HIZ_1_START_BIT 1
#define SLOT_HIZ_1_WIDTH 1


#define SLOT_HIZ_1_PARAM_MASK  0x2



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl0 0x0D2F, Bits 0:0, typedef slot_hiz
//   
//  
#define SLOT_HIZ_0_MASK 0x1U
#define SLOT_HIZ_0_OFFSET 0
#define SLOT_HIZ_0_STOP_BIT 0
#define SLOT_HIZ_0_START_BIT 0
#define SLOT_HIZ_0_WIDTH 1


#define SLOT_HIZ_0_PARAM_MASK  0x1



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl1 0x0D30, Bits 7:7, typedef slot_hiz
//   
//  
#define SLOT_HIZ_15_MASK 0x80U
#define SLOT_HIZ_15_OFFSET 7
#define SLOT_HIZ_15_STOP_BIT 7
#define SLOT_HIZ_15_START_BIT 7
#define SLOT_HIZ_15_WIDTH 1

#define A_DSLOT_TRISTATE_CONTROL_1_REG 0xD30

#define SLOT_HIZ_15_PARAM_MASK  0x80



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl1 0x0D30, Bits 6:6, typedef slot_hiz
//   
//  
#define SLOT_HIZ_14_MASK 0x40U
#define SLOT_HIZ_14_OFFSET 6
#define SLOT_HIZ_14_STOP_BIT 6
#define SLOT_HIZ_14_START_BIT 6
#define SLOT_HIZ_14_WIDTH 1


#define SLOT_HIZ_14_PARAM_MASK  0x40



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl1 0x0D30, Bits 5:5, typedef slot_hiz
//   
//  
#define SLOT_HIZ_13_MASK 0x20U
#define SLOT_HIZ_13_OFFSET 5
#define SLOT_HIZ_13_STOP_BIT 5
#define SLOT_HIZ_13_START_BIT 5
#define SLOT_HIZ_13_WIDTH 1


#define SLOT_HIZ_13_PARAM_MASK  0x20



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl1 0x0D30, Bits 4:4, typedef slot_hiz
//   
//  
#define SLOT_HIZ_12_MASK 0x10U
#define SLOT_HIZ_12_OFFSET 4
#define SLOT_HIZ_12_STOP_BIT 4
#define SLOT_HIZ_12_START_BIT 4
#define SLOT_HIZ_12_WIDTH 1


#define SLOT_HIZ_12_PARAM_MASK  0x10



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl1 0x0D30, Bits 3:3, typedef slot_hiz
//   
//  
#define SLOT_HIZ_11_MASK 0x8U
#define SLOT_HIZ_11_OFFSET 3
#define SLOT_HIZ_11_STOP_BIT 3
#define SLOT_HIZ_11_START_BIT 3
#define SLOT_HIZ_11_WIDTH 1


#define SLOT_HIZ_11_PARAM_MASK  0x8



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl1 0x0D30, Bits 2:2, typedef slot_hiz
//   
//  
#define SLOT_HIZ_10_MASK 0x4U
#define SLOT_HIZ_10_OFFSET 2
#define SLOT_HIZ_10_STOP_BIT 2
#define SLOT_HIZ_10_START_BIT 2
#define SLOT_HIZ_10_WIDTH 1


#define SLOT_HIZ_10_PARAM_MASK  0x4



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl1 0x0D30, Bits 1:1, typedef slot_hiz
//   
//  
#define SLOT_HIZ_9_MASK 0x2U
#define SLOT_HIZ_9_OFFSET 1
#define SLOT_HIZ_9_STOP_BIT 1
#define SLOT_HIZ_9_START_BIT 1
#define SLOT_HIZ_9_WIDTH 1


#define SLOT_HIZ_9_PARAM_MASK  0x2



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl1 0x0D30, Bits 0:0, typedef slot_hiz
//   
//  
#define SLOT_HIZ_8_MASK 0x1U
#define SLOT_HIZ_8_OFFSET 0
#define SLOT_HIZ_8_STOP_BIT 0
#define SLOT_HIZ_8_START_BIT 0
#define SLOT_HIZ_8_WIDTH 1


#define SLOT_HIZ_8_PARAM_MASK  0x1



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl2 0x0D31, Bits 7:7, typedef slot_hiz
//   
//  
#define SLOT_HIZ_23_MASK 0x80U
#define SLOT_HIZ_23_OFFSET 7
#define SLOT_HIZ_23_STOP_BIT 7
#define SLOT_HIZ_23_START_BIT 7
#define SLOT_HIZ_23_WIDTH 1

#define A_DSLOT_TRISTATE_CONTROL_2_REG 0xD31

#define SLOT_HIZ_23_PARAM_MASK  0x80



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl2 0x0D31, Bits 6:6, typedef slot_hiz
//   
//  
#define SLOT_HIZ_22_MASK 0x40U
#define SLOT_HIZ_22_OFFSET 6
#define SLOT_HIZ_22_STOP_BIT 6
#define SLOT_HIZ_22_START_BIT 6
#define SLOT_HIZ_22_WIDTH 1


#define SLOT_HIZ_22_PARAM_MASK  0x40



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl2 0x0D31, Bits 5:5, typedef slot_hiz
//   
//  
#define SLOT_HIZ_21_MASK 0x20U
#define SLOT_HIZ_21_OFFSET 5
#define SLOT_HIZ_21_STOP_BIT 5
#define SLOT_HIZ_21_START_BIT 5
#define SLOT_HIZ_21_WIDTH 1


#define SLOT_HIZ_21_PARAM_MASK  0x20



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl2 0x0D31, Bits 4:4, typedef slot_hiz
//   
//  
#define SLOT_HIZ_20_MASK 0x10U
#define SLOT_HIZ_20_OFFSET 4
#define SLOT_HIZ_20_STOP_BIT 4
#define SLOT_HIZ_20_START_BIT 4
#define SLOT_HIZ_20_WIDTH 1


#define SLOT_HIZ_20_PARAM_MASK  0x10



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl2 0x0D31, Bits 3:3, typedef slot_hiz
//   
//  
#define SLOT_HIZ_19_MASK 0x8U
#define SLOT_HIZ_19_OFFSET 3
#define SLOT_HIZ_19_STOP_BIT 3
#define SLOT_HIZ_19_START_BIT 3
#define SLOT_HIZ_19_WIDTH 1


#define SLOT_HIZ_19_PARAM_MASK  0x8



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl2 0x0D31, Bits 2:2, typedef slot_hiz
//   
//  
#define SLOT_HIZ_18_MASK 0x4U
#define SLOT_HIZ_18_OFFSET 2
#define SLOT_HIZ_18_STOP_BIT 2
#define SLOT_HIZ_18_START_BIT 2
#define SLOT_HIZ_18_WIDTH 1


#define SLOT_HIZ_18_PARAM_MASK  0x4



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl2 0x0D31, Bits 1:1, typedef slot_hiz
//   
//  
#define SLOT_HIZ_17_MASK 0x2U
#define SLOT_HIZ_17_OFFSET 1
#define SLOT_HIZ_17_STOP_BIT 1
#define SLOT_HIZ_17_START_BIT 1
#define SLOT_HIZ_17_WIDTH 1


#define SLOT_HIZ_17_PARAM_MASK  0x2



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl2 0x0D31, Bits 0:0, typedef slot_hiz
//   
//  
#define SLOT_HIZ_16_MASK 0x1U
#define SLOT_HIZ_16_OFFSET 0
#define SLOT_HIZ_16_STOP_BIT 0
#define SLOT_HIZ_16_START_BIT 0
#define SLOT_HIZ_16_WIDTH 1


#define SLOT_HIZ_16_PARAM_MASK  0x1



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl3 0x0D32, Bits 7:7, typedef slot_hiz
//   
//  
#define SLOT_HIZ_31_MASK 0x80U
#define SLOT_HIZ_31_OFFSET 7
#define SLOT_HIZ_31_STOP_BIT 7
#define SLOT_HIZ_31_START_BIT 7
#define SLOT_HIZ_31_WIDTH 1

#define A_DSLOT_TRISTATE_CONTROL_3_REG 0xD32

#define SLOT_HIZ_31_PARAM_MASK  0x80



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl3 0x0D32, Bits 6:6, typedef slot_hiz
//   
//  
#define SLOT_HIZ_30_MASK 0x40U
#define SLOT_HIZ_30_OFFSET 6
#define SLOT_HIZ_30_STOP_BIT 6
#define SLOT_HIZ_30_START_BIT 6
#define SLOT_HIZ_30_WIDTH 1


#define SLOT_HIZ_30_PARAM_MASK  0x40



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl3 0x0D32, Bits 5:5, typedef slot_hiz
//   
//  
#define SLOT_HIZ_29_MASK 0x20U
#define SLOT_HIZ_29_OFFSET 5
#define SLOT_HIZ_29_STOP_BIT 5
#define SLOT_HIZ_29_START_BIT 5
#define SLOT_HIZ_29_WIDTH 1


#define SLOT_HIZ_29_PARAM_MASK  0x20



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl3 0x0D32, Bits 4:4, typedef slot_hiz
//   
//  
#define SLOT_HIZ_28_MASK 0x10U
#define SLOT_HIZ_28_OFFSET 4
#define SLOT_HIZ_28_STOP_BIT 4
#define SLOT_HIZ_28_START_BIT 4
#define SLOT_HIZ_28_WIDTH 1


#define SLOT_HIZ_28_PARAM_MASK  0x10



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl3 0x0D32, Bits 3:3, typedef slot_hiz
//   
//  
#define SLOT_HIZ_27_MASK 0x8U
#define SLOT_HIZ_27_OFFSET 3
#define SLOT_HIZ_27_STOP_BIT 3
#define SLOT_HIZ_27_START_BIT 3
#define SLOT_HIZ_27_WIDTH 1


#define SLOT_HIZ_27_PARAM_MASK  0x8



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl3 0x0D32, Bits 2:2, typedef slot_hiz
//   
//  
#define SLOT_HIZ_26_MASK 0x4U
#define SLOT_HIZ_26_OFFSET 2
#define SLOT_HIZ_26_STOP_BIT 2
#define SLOT_HIZ_26_START_BIT 2
#define SLOT_HIZ_26_WIDTH 1


#define SLOT_HIZ_26_PARAM_MASK  0x4



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl3 0x0D32, Bits 1:1, typedef slot_hiz
//   
//  
#define SLOT_HIZ_25_MASK 0x2U
#define SLOT_HIZ_25_OFFSET 1
#define SLOT_HIZ_25_STOP_BIT 1
#define SLOT_HIZ_25_START_BIT 1
#define SLOT_HIZ_25_WIDTH 1


#define SLOT_HIZ_25_PARAM_MASK  0x2



//  
//  Slot impedance after last half-lsb.
//  Register ADslotTristateControl3 0x0D32, Bits 0:0, typedef slot_hiz
//   
//  
#define SLOT_HIZ_24_MASK 0x1U
#define SLOT_HIZ_24_OFFSET 0
#define SLOT_HIZ_24_STOP_BIT 0
#define SLOT_HIZ_24_START_BIT 0
#define SLOT_HIZ_24_WIDTH 1


#define SLOT_HIZ_24_PARAM_MASK  0x1


#define SLOT_DSP_01_PARAM_MASK  0x1F
#define SLOT_DSP_01_MASK 0x1FU
#define SLOT_DSP_01_OFFSET 4
#define SLOT_DSP_01_STOP_BIT 4
#define SLOT_DSP_01_START_BIT 0
#define SLOT_DSP_01_WIDTH 5

typedef enum slot_dsp {
    SLOT_0_ = 0x0,
    SLOT_1_ = 0x1,
    SLOT_2_ = 0x2,
    SLOT_3_ = 0x3,
    SLOT_4_ = 0x4,
    SLOT_5_ = 0x5,
    SLOT_6_ = 0x6,
    SLOT_7_ = 0x7,
    SLOT_8_ = 0x8,
    SLOT_9_ = 0x9,
    SLOT_10_ = 0xA,
    SLOT_11_ = 0xB,
    SLOT_12_ = 0xC,
    SLOT_13_ = 0xD,
    SLOT_14_ = 0xE,
    SLOT_15_ = 0xF,
    SLOT_16_ = 0x10,
    SLOT_17_ = 0x11,
    SLOT_18_ = 0x12,
    SLOT_19_ = 0x13,
    SLOT_20_ = 0x14,
    SLOT_21_ = 0x15,
    SLOT_22_ = 0x16,
    SLOT_23_ = 0x17,
    SLOT_24_ = 0x18,
    SLOT_25_ = 0x19,
    SLOT_26_ = 0x1A,
    SLOT_27_ = 0x1B,
    SLOT_28_ = 0x1C,
    SLOT_29_ = 0x1D,
    SLOT_30_ = 0x1E,
    SLOT_31_ = 0x1F
} SLOT_DSP_T ;

//  
//  0: AD_OUT1 is connected to the corresponding AD path; 
//  1: DA_IN7 is looped back to AD_OUT1.
//  Register SlotSelectionDAPath1 0x0D33, Bits 5:5, typedef da_in_to_ad_out
//   
//  
#define DA_IN_7_TO_ADOUT_1_MASK 0x20U
#define DA_IN_7_TO_ADOUT_1_OFFSET 5
#define DA_IN_7_TO_ADOUT_1_STOP_BIT 5
#define DA_IN_7_TO_ADOUT_1_START_BIT 5
#define DA_IN_7_TO_ADOUT_1_WIDTH 1

#define SLOT_SELECTION_DA_PATH_1_REG 0xD33

typedef enum da_in_to_ad_out {
    AD_OUT_CONNECTED_AD_PATH_E,
    DA_IN_LOOPED_TO_AD_OUT_E
} DA_IN_TO_AD_OUT_T ;
#define DA_IN_7_TO_ADOUT_1_PARAM_MASK  0x20



//  
//  Swap chanel 1 and 2 with 3 and 4.
//  Register SlotSelectionDAPath1 0x0D33, Bits 6:6, typedef swap12_34
//   
//  
#define SWAP_1234_MASK 0x40U
#define SWAP_1234_OFFSET 6
#define SWAP_1234_STOP_BIT 6
#define SWAP_1234_START_BIT 6
#define SWAP_1234_WIDTH 1


typedef enum swap12_34 {
    NO_SWAP12_34_E,
    SWAP12_34_E
} SWAP_1234_T ;
#define SWAP_1234_PARAM_MASK  0x40



//  
//  0: DA1 and DA2 channel paths use Audio Filters 
//  1: DA1 and DA2 channel paths use 48kHz Low Latency Filters for Voice.
//  Register SlotSelectionDAPath1 0x0D33, Bits 7:7, typedef da12_voice_filter_enable
//   
//  
#define DA_12_VOICE_FILTER_MASK 0x80U
#define DA_12_VOICE_FILTER_OFFSET 7
#define DA_12_VOICE_FILTER_STOP_BIT 7
#define DA_12_VOICE_FILTER_START_BIT 7
#define DA_12_VOICE_FILTER_WIDTH 1


typedef enum da12_voice_filter_enable {
    DA_12_VOICE_FILTER_DISABLE_E,
    DA_12_VOICE_FILTER_ENABLE_E
} DA_12_VOICE_FILTER_ENABLE_T ;
#define DA_12_VOICE_FILTER_PARAM_MASK  0x80


#define SLOT_DSP_02_PARAM_MASK  0x1F
#define SLOT_DSP_02_MASK 0x1FU
#define SLOT_DSP_02_OFFSET 4
#define SLOT_DSP_02_STOP_BIT 4
#define SLOT_DSP_02_START_BIT 0
#define SLOT_DSP_02_WIDTH 5


//  
//  0: AD_OUT2 is connected to the corresponding AD path; 
//  1: DA_IN8 is looped back to AD_OUT2.
//  Register SlotSelectionDAPath2 0x0D34, Bits 5:5, typedef da_in_to_ad_out
//   
//  
#define DA_IN_8_TO_ADOUT_2_MASK 0x20U
#define DA_IN_8_TO_ADOUT_2_OFFSET 5
#define DA_IN_8_TO_ADOUT_2_STOP_BIT 5
#define DA_IN_8_TO_ADOUT_2_START_BIT 5
#define DA_IN_8_TO_ADOUT_2_WIDTH 1

#define SLOT_SELECTION_DA_PATH_2_REG 0xD34

#define DA_IN_8_TO_ADOUT_2_PARAM_MASK  0x20


#define SLOT_DSP_03_PARAM_MASK  0x1F
#define SLOT_DSP_03_MASK 0x1FU
#define SLOT_DSP_03_OFFSET 4
#define SLOT_DSP_03_STOP_BIT 4
#define SLOT_DSP_03_START_BIT 0
#define SLOT_DSP_03_WIDTH 5


//  
//  0: AD_OUT3 is connected to the corresponding AD path; 
//  1: DA_IN7 is looped back to AD_OUT3.
//  Register SlotSelectionDAPath3 0x0D35, Bits 5:5, typedef da_in_to_ad_out
//   
//  
#define DA_IN_7_TO_ADOUT_3_MASK 0x20U
#define DA_IN_7_TO_ADOUT_3_OFFSET 5
#define DA_IN_7_TO_ADOUT_3_STOP_BIT 5
#define DA_IN_7_TO_ADOUT_3_START_BIT 5
#define DA_IN_7_TO_ADOUT_3_WIDTH 1

#define SLOT_SELECTION_DA_PATH_3_REG 0xD35

#define DA_IN_7_TO_ADOUT_3_PARAM_MASK  0x20



//  
//  0: DA3 and DA3 channel paths use Audio Filters 
//  1: DA3 and DA3 channel paths use 48kHz Low Latency Filters for Voice.
//  Register SlotSelectionDAPath3 0x0D35, Bits 7:7, typedef da34_voice_filter_enable
//   
//  
#define DA_34_VOICE_FILTER_MASK 0x80U
#define DA_34_VOICE_FILTER_OFFSET 7
#define DA_34_VOICE_FILTER_STOP_BIT 7
#define DA_34_VOICE_FILTER_START_BIT 7
#define DA_34_VOICE_FILTER_WIDTH 1


typedef enum da34_voice_filter_enable {
    DA_34_VOICE_FILTER_DISABLE_E,
    DA_34_VOICE_FILTER_ENABLE_E
} DA_34_VOICE_FILTER_ENABLE_T ;
#define DA_34_VOICE_FILTER_PARAM_MASK  0x80


#define SLOT_DSP_04_PARAM_MASK  0x1F
#define SLOT_DSP_04_MASK 0x1FU
#define SLOT_DSP_04_OFFSET 4
#define SLOT_DSP_04_STOP_BIT 4
#define SLOT_DSP_04_START_BIT 0
#define SLOT_DSP_04_WIDTH 5


//  
//  0: AD_OUT4 is connected to the corresponding AD path; 
//  1: DA_IN8 is looped back to AD_OUT4.
//  Register SlotSelectionDAPath4 0x0D36, Bits 5:5, typedef da_in_to_ad_out
//   
//  
#define DA_IN_8_TO_ADOUT_4_MASK 0x20U
#define DA_IN_8_TO_ADOUT_4_OFFSET 5
#define DA_IN_8_TO_ADOUT_4_STOP_BIT 5
#define DA_IN_8_TO_ADOUT_4_START_BIT 5
#define DA_IN_8_TO_ADOUT_4_WIDTH 1

#define SLOT_SELECTION_DA_PATH_4_REG 0xD36

#define DA_IN_8_TO_ADOUT_4_PARAM_MASK  0x20


#define SLOT_DSP_05_PARAM_MASK  0x1F
#define SLOT_DSP_05_MASK 0x1FU
#define SLOT_DSP_05_OFFSET 4
#define SLOT_DSP_05_STOP_BIT 4
#define SLOT_DSP_05_START_BIT 0
#define SLOT_DSP_05_WIDTH 5


//  
//  0: AD_OUT8 is connected to the corresponding AD path; 
//  1: DA_IN7 is looped back to AD_OUT5.
//  Register SlotSelectionDAPath5 0x0D37, Bits 5:5, typedef da_in_to_ad_out
//   
//  
#define DA_IN_7_TO_ADOUT_5_MASK 0x20U
#define DA_IN_7_TO_ADOUT_5_OFFSET 5
#define DA_IN_7_TO_ADOUT_5_STOP_BIT 5
#define DA_IN_7_TO_ADOUT_5_START_BIT 5
#define DA_IN_7_TO_ADOUT_5_WIDTH 1

#define SLOT_SELECTION_DA_PATH_5_REG 0xD37

#define DA_IN_7_TO_ADOUT_5_PARAM_MASK  0x20



//  
//  0: DA5 and DA6 channel paths use Audio Filters 
//  1: DA5 and DA6 channel paths use 48kHz Low Latency Filters for Voice.
//  Register SlotSelectionDAPath5 0x0D37, Bits 7:7, typedef da56_voice_filter_enable
//   
//  
#define DA_56_VOICE_FILTER_MASK 0x80U
#define DA_56_VOICE_FILTER_OFFSET 7
#define DA_56_VOICE_FILTER_STOP_BIT 7
#define DA_56_VOICE_FILTER_START_BIT 7
#define DA_56_VOICE_FILTER_WIDTH 1


typedef enum da56_voice_filter_enable {
    DA_56_VOICE_FILTER_DISABLE_E,
    DA_56_VOICE_FILTER_ENABLE_E
} DA_56_VOICE_FILTER_ENABLE_T ;
#define DA_56_VOICE_FILTER_PARAM_MASK  0x80


#define SLOT_DSP_06_PARAM_MASK  0x1F
#define SLOT_DSP_06_MASK 0x1FU
#define SLOT_DSP_06_OFFSET 4
#define SLOT_DSP_06_STOP_BIT 4
#define SLOT_DSP_06_START_BIT 0
#define SLOT_DSP_06_WIDTH 5


//  
//  0: AD_OUT6 is connected to the corresponding AD path; 
//  1: DA_IN8 is looped back to AD_OUT6.
//  Register SlotSelectionDAPath6 0x0D38, Bits 5:5, typedef da_in_to_ad_out
//   
//  
#define DA_IN_8_TO_ADOUT_6_MASK 0x20U
#define DA_IN_8_TO_ADOUT_6_OFFSET 5
#define DA_IN_8_TO_ADOUT_6_STOP_BIT 5
#define DA_IN_8_TO_ADOUT_6_START_BIT 5
#define DA_IN_8_TO_ADOUT_6_WIDTH 1

#define SLOT_SELECTION_DA_PATH_6_REG 0xD38

#define DA_IN_8_TO_ADOUT_6_PARAM_MASK  0x20


#define SLOT_DSP_07_PARAM_MASK  0x1F
#define SLOT_DSP_07_MASK 0x1FU
#define SLOT_DSP_07_OFFSET 4
#define SLOT_DSP_07_STOP_BIT 4
#define SLOT_DSP_07_START_BIT 0
#define SLOT_DSP_07_WIDTH 5


//  
//  0: AD_OUT7 is connected to the corresponding AD path; 
//  1: DA_IN8 is looped back to AD_OUT7.
//  Register SlotSelectionDAPath7 0x0D39, Bits 5:5, typedef da_in_to_ad_out
//   
//  
#define DA_IN_8_TO_ADOUT_7_MASK 0x20U
#define DA_IN_8_TO_ADOUT_7_OFFSET 5
#define DA_IN_8_TO_ADOUT_7_STOP_BIT 5
#define DA_IN_8_TO_ADOUT_7_START_BIT 5
#define DA_IN_8_TO_ADOUT_7_WIDTH 1

#define SLOT_SELECTION_DA_PATH_7_REG 0xD39

#define DA_IN_8_TO_ADOUT_7_PARAM_MASK  0x20


#define SLOT_DSP_08_PARAM_MASK  0x1F
#define SLOT_DSP_08_MASK 0x1FU
#define SLOT_DSP_08_OFFSET 4
#define SLOT_DSP_08_STOP_BIT 4
#define SLOT_DSP_08_START_BIT 0
#define SLOT_DSP_08_WIDTH 5


//  
//  0: AD_OUT8 is connected to the corresponding AD path; 
//  1: DA_IN8 is looped back to AD_OUT6.
//  Register SlotSelectionDAPath8 0x0D3A, Bits 5:5, typedef da_in_to_ad_out
//   
//  
#define DA_IN_8_TO_ADOUT_8_MASK 0x20U
#define DA_IN_8_TO_ADOUT_8_OFFSET 5
#define DA_IN_8_TO_ADOUT_8_STOP_BIT 5
#define DA_IN_8_TO_ADOUT_8_START_BIT 5
#define DA_IN_8_TO_ADOUT_8_WIDTH 1

#define SLOT_SELECTION_DA_PATH_8_REG 0xD3A

#define DA_IN_8_TO_ADOUT_8_PARAM_MASK  0x20



//  
//  0: The HFL and HFR Outputs are independent; 
//  1: The HFL and HFR Outputs are bridged. HFL output is used for both.
//  Register ClassDEMIControl 0x0D3B, Bits 7:7, typedef hf_output_bridged_enable
//   
//  
#define HF_OUTPUT_BRIDGED_MASK 0x80U
#define HF_OUTPUT_BRIDGED_OFFSET 7
#define HF_OUTPUT_BRIDGED_STOP_BIT 7
#define HF_OUTPUT_BRIDGED_START_BIT 7
#define HF_OUTPUT_BRIDGED_WIDTH 1

#define CLASS_DEMI_CONTROL_REG 0xD3B

typedef enum hf_output_bridged_enable {
    HF_OUTPUT_BRIDGED_DISABLE_E,
    HF_OUTPUT_BRIDGED_ENABLE_E
} HF_OUTPUT_BRIDGED_ENABLE_T ;
#define HF_OUTPUT_BRIDGED_PARAM_MASK  0x80



//  
//  0000: High volume mode disabled; xxx1: Left HF channel in high volume mode; xx1x: 
//  Right HF channel in high volume mode; x1xx: Vibra1 channel in high volume mode; 1xxx: 
//  Vibra2 channel in high volume mode.
//  Register ClassDPath 0x0D3C, Bits 0:3, typedef high_volume
//   
//  
#define HIGH_VOLUME_MASK 0xFU
#define HIGH_VOLUME_OFFSET 3
#define HIGH_VOLUME_STOP_BIT 3
#define HIGH_VOLUME_START_BIT 0
#define HIGH_VOLUME_WIDTH 4

#define CLASS_D_PATH_REG 0xD3C
#define  HIGH_VOLUME_DISABLED 0
#define  LEFT_HF_HIGH_VOLUME 1
#define  RIGHT_HF_HIGH_VOLUME 2
#define  VIB1_HIGH_VOLUME 4
#define  VIB2_HIGH_VOLUME 8

typedef enum high_volume {
    HIGH_VOLUME_DISABLED_E,
    LEFT_HF_HIGH_VOLUME_E,
    RIGHT_HF_HIGH_VOLUME_E,
    VIB1_HIGH_VOLUME_E,
    VIB2_HIGH_VOLUME_E
} HIGH_VOLUME_T ;
#define HIGH_VOLUME_PARAM_MASK  0xF



//  
//  0000: All PWM FIRs are enabled; xxx1: Left HF PWM FIR filter is bypassed; xx1x: Right 
//  HF PWM FIR filter is bypassed; x1xx: Left Vibra PWM FIR filter is bypassed; 1xxx: 
//  Right Vibra PWM FIR filter is bypassed.
//  Register ClassDPath 0x0D3C, Bits 4:7, typedef class_d_fir_bypass
//   
//  
#define CLASS_D_FIR_BYPASS_MASK 0xF0U
#define CLASS_D_FIR_BYPASS_OFFSET 7
#define CLASS_D_FIR_BYPASS_STOP_BIT 7
#define CLASS_D_FIR_BYPASS_START_BIT 4
#define CLASS_D_FIR_BYPASS_WIDTH 4

#define  ALL_PWM_FIR_ENABLED 0
#define  LEFT_HF_BYPASSED 1
#define  RIGHT_HF_BYPASSED 2
#define  VIB1_BYPASSED 4
#define  VIB2_BYPASSED 8

typedef enum class_d_fir_bypass {
    ALL_PWM_FIR_ENABLED_E,
    LEFT_HF_BYPASSED_E,
    RIGHT_HF_BYPASSED_E,
    VIB1_BYPASSED_E,
    VIB2_BYPASSED_E
} CLASS_D_FIR_BYPASS_T ;
#define CLASS_D_FIR_BYPASS_PARAM_MASK  0xF0



//  
//  Gain control for the white component of dither.
//  Register ClassDControlGain 0x0D3D, Bits 0:3, typedef high_white_component_dither
//   
//  
#define HIGHWHITE_COMPONENT_DITHER_MASK 0xFU
#define HIGHWHITE_COMPONENT_DITHER_OFFSET 3
#define HIGHWHITE_COMPONENT_DITHER_STOP_BIT 3
#define HIGHWHITE_COMPONENT_DITHER_START_BIT 0
#define HIGHWHITE_COMPONENT_DITHER_WIDTH 4

#define CLASS_D_CONTROL_GAIN_REG 0xD3D



//  
//  Gain control for the high pass component of dither.
//  Register ClassDControlGain 0x0D3D, Bits 4:7, typedef high_pass_component_dither
//   
//  
#define HIGH_PASS_COMPONENT_DITHER_MASK 0xF0U
#define HIGH_PASS_COMPONENT_DITHER_OFFSET 7
#define HIGH_PASS_COMPONENT_DITHER_STOP_BIT 7
#define HIGH_PASS_COMPONENT_DITHER_START_BIT 4
#define HIGH_PASS_COMPONENT_DITHER_WIDTH 4




//  
//  0: Sinc5 decimator filter is selected for DMIC6; 
//  1: Sinc3 decimator filter is selected for DMIC6.
//  Register DecimatorFilter 0x0D3E, Bits 0:0, typedef decimator_mic
//   
//  
#define DECIMATOR_MIC_6_MASK 0x1U
#define DECIMATOR_MIC_6_OFFSET 0
#define DECIMATOR_MIC_6_STOP_BIT 0
#define DECIMATOR_MIC_6_START_BIT 0
#define DECIMATOR_MIC_6_WIDTH 1

#define DECIMATOR_FILTER_REG 0xD3E

typedef enum decimator_mic {
    SINC5_DECIMATOR_FILTER_E,
    SINC3_DECIMATOR_FILTER_E
} DECIMATOR_MIC_T ;
#define DECIMATOR_MIC_6_PARAM_MASK  0x1



//  
//  0: Sinc5 decimator filter is selected for DMIC5; 
//  1: Sinc3 decimator filter is selected for DMIC5.
//  Register DecimatorFilter 0x0D3E, Bits 1:1, typedef decimator_mic
//   
//  
#define DECIMATOR_MIC_5_MASK 0x2U
#define DECIMATOR_MIC_5_OFFSET 1
#define DECIMATOR_MIC_5_STOP_BIT 1
#define DECIMATOR_MIC_5_START_BIT 1
#define DECIMATOR_MIC_5_WIDTH 1


#define DECIMATOR_MIC_5_PARAM_MASK  0x2



//  
//  0: Sinc5 decimator filter is selected for DMIC4; 
//  1: Sinc3 decimator filter is selected for DMIC4.
//  Register DecimatorFilter 0x0D3E, Bits 2:2, typedef decimator_mic
//   
//  
#define DECIMATOR_MIC_4_MASK 0x4U
#define DECIMATOR_MIC_4_OFFSET 2
#define DECIMATOR_MIC_4_STOP_BIT 2
#define DECIMATOR_MIC_4_START_BIT 2
#define DECIMATOR_MIC_4_WIDTH 1


#define DECIMATOR_MIC_4_PARAM_MASK  0x4



//  
//  0: Sinc5 decimator filter is selected for DMIC3; 
//  1: Sinc3 decimator filter is selected for DMIC3.
//  Register DecimatorFilter 0x0D3E, Bits 3:3, typedef decimator_mic
//   
//  
#define DECIMATOR_MIC_3_MASK 0x8U
#define DECIMATOR_MIC_3_OFFSET 3
#define DECIMATOR_MIC_3_STOP_BIT 3
#define DECIMATOR_MIC_3_START_BIT 3
#define DECIMATOR_MIC_3_WIDTH 1


#define DECIMATOR_MIC_3_PARAM_MASK  0x8



//  
//  0: Sinc5 decimator filter is selected for DMIC2; 
//  1: Sinc3 decimator filter is selected for DMIC2.
//  Register DecimatorFilter 0x0D3E, Bits 4:4, typedef decimator_mic
//   
//  
#define DECIMATOR_MIC_2_MASK 0x10U
#define DECIMATOR_MIC_2_OFFSET 4
#define DECIMATOR_MIC_2_STOP_BIT 4
#define DECIMATOR_MIC_2_START_BIT 4
#define DECIMATOR_MIC_2_WIDTH 1


#define DECIMATOR_MIC_2_PARAM_MASK  0x10



//  
//  0: Sinc5 decimator filter is selected for DMIC1; 
//  1: Sinc3 decimator filter is selected for DMIC1.
//  Register DecimatorFilter 0x0D3E, Bits 5:5, typedef decimator_mic
//   
//  
#define DECIMATOR_MIC_1_MASK 0x20U
#define DECIMATOR_MIC_1_OFFSET 5
#define DECIMATOR_MIC_1_STOP_BIT 5
#define DECIMATOR_MIC_1_START_BIT 5
#define DECIMATOR_MIC_1_WIDTH 1


#define DECIMATOR_MIC_1_PARAM_MASK  0x20



//  
//  0: EarDAC signal comes from DA1 path 
//  1: EarDAC signal comes from DA3 path.
//  Register DecimatorFilter 0x0D3E, Bits 6:6, typedef ear_dac_source
//   
//  
#define EAR_DAC_SOURCE_MASK 0x40U
#define EAR_DAC_SOURCE_OFFSET 6
#define EAR_DAC_SOURCE_STOP_BIT 6
#define EAR_DAC_SOURCE_START_BIT 6
#define EAR_DAC_SOURCE_WIDTH 1


typedef enum ear_dac_source {
    SIGNAL_FORM_DA1_PATH_E,
    SIGNAL_FORM_DA3_PATH_E
} EAR_DAC_SOURCE_T ;
#define EAR_DAC_SOURCE_PARAM_MASK  0x40



//  
//  0: ANC input is AD6 
//  1: ANC input is AD5.
//  Register DecimatorFilter 0x0D3E, Bits 7:7, typedef anc_input
//   
//  
#define ANC_INPUT_MASK 0x80U
#define ANC_INPUT_OFFSET 7
#define ANC_INPUT_STOP_BIT 7
#define ANC_INPUT_START_BIT 7
#define ANC_INPUT_WIDTH 1


typedef enum anc_input {
    ANC_INPUT_AD6_E,
    ANC_INPUT_AD5_E
} ANC_INPUT_T ;
#define ANC_INPUT_PARAM_MASK  0x80



//  
//  0: ANC loop not mixed driver. 
//  1: ANC loop mixed in earpiece driver..
//  Register DigitalMultiplexers 0x0D3F, Bits 0:0, typedef anc_loop_mixed
//   
//  
#define ANC_LOOP_MIXED_MASK 0x1U
#define ANC_LOOP_MIXED_OFFSET 0
#define ANC_LOOP_MIXED_STOP_BIT 0
#define ANC_LOOP_MIXED_START_BIT 0
#define ANC_LOOP_MIXED_WIDTH 1

#define DIGITAL_MULTIPLEXERS_REG 0xD3F

typedef enum anc_loop_mixed {
    NOT_MIXED_E,
    MIXED_E
} ANC_LOOP_MIXED_T ;
#define ANC_LOOP_MIXED_PARAM_MASK  0x1



//  
//  0: Analog microphone on ADR analog input is selected for AD_OUT6/ANC output path; 
//  1: Digital microphone DMIC6 is selected for AD_OUT6/ANC output path.
//  Register DigitalMultiplexers 0x0D3F, Bits 1:1, typedef micro
//   
//  
#define DIG_MICRO_6_FOR_OUT_6_MASK 0x2U
#define DIG_MICRO_6_FOR_OUT_6_OFFSET 1
#define DIG_MICRO_6_FOR_OUT_6_STOP_BIT 1
#define DIG_MICRO_6_FOR_OUT_6_START_BIT 1
#define DIG_MICRO_6_FOR_OUT_6_WIDTH 1


typedef enum micro {
    MICRO_ANALOG_E,
    MICRO_DIGITAL_E
} MICRO_T ;
#define DIG_MICRO_6_FOR_OUT_6_PARAM_MASK  0x2



//  
//  0: Analog microphone on ADR analog input is selected for AD_OUT5 output path; 
//  1: Digital microphone DMIC5 is selected for AD_OUT5 ANC output path.
//  Register DigitalMultiplexers 0x0D3F, Bits 2:2, typedef micro
//   
//  
#define DIG_MICRO_5_FOR_OUT_5_MASK 0x4U
#define DIG_MICRO_5_FOR_OUT_5_OFFSET 2
#define DIG_MICRO_5_FOR_OUT_5_STOP_BIT 2
#define DIG_MICRO_5_FOR_OUT_5_START_BIT 2
#define DIG_MICRO_5_FOR_OUT_5_WIDTH 1


#define DIG_MICRO_5_FOR_OUT_5_PARAM_MASK  0x4



//  
//  0: Analog microphone ADMO analog input is selected for AD_OUT3 output path; 
//  1: Digital microphone DMIC3 is selected for AD_OUT3 output path.
//  Register DigitalMultiplexers 0x0D3F, Bits 3:3, typedef micro
//   
//  
#define DIG_MICRO_3_FOR_OUT_3_MASK 0x8U
#define DIG_MICRO_3_FOR_OUT_3_OFFSET 3
#define DIG_MICRO_3_FOR_OUT_3_STOP_BIT 3
#define DIG_MICRO_3_FOR_OUT_3_START_BIT 3
#define DIG_MICRO_3_FOR_OUT_3_WIDTH 1


#define DIG_MICRO_3_FOR_OUT_3_PARAM_MASK  0x8



//  
//  0: Right Linein ADL analog input is selected for AD_OUT2 output path, 
//  1: Digital microphone DMIC2 is selected for AD_OUT2 output path.
//  Register DigitalMultiplexers 0x0D3F, Bits 4:4, typedef micro
//   
//  
#define DIG_MICRO_2_FOR_OUT_2_MASK 0x10U
#define DIG_MICRO_2_FOR_OUT_2_OFFSET 4
#define DIG_MICRO_2_FOR_OUT_2_STOP_BIT 4
#define DIG_MICRO_2_FOR_OUT_2_START_BIT 4
#define DIG_MICRO_2_FOR_OUT_2_WIDTH 1


#define DIG_MICRO_2_FOR_OUT_2_PARAM_MASK  0x10



//  
//  0: Left Linein ADL analog input is selected for AD_OUT1 output path, 
//  1: Digital microphone DMIC1 is selected for AD_OUT1 output path.
//  Register DigitalMultiplexers 0x0D3F, Bits 5:5, typedef micro
//   
//  
#define DIG_MICRO_1_FOR_OUT_1_MASK 0x20U
#define DIG_MICRO_1_FOR_OUT_1_OFFSET 5
#define DIG_MICRO_1_FOR_OUT_1_STOP_BIT 5
#define DIG_MICRO_1_FOR_OUT_1_START_BIT 5
#define DIG_MICRO_1_FOR_OUT_1_WIDTH 1


#define DIG_MICRO_1_FOR_OUT_1_PARAM_MASK  0x20



//  
//  0: DA_IN2 direct path disabled. Path from sidetone FIR connected to HSR, 
//  1: DA_IN2 path mixed together with sidetone FIR..
//  Register DigitalMultiplexers 0x0D3F, Bits 6:6, typedef path_in2
//   
//  
#define OUTPUT_DIGITAL_IN_2_MASK 0x40U
#define OUTPUT_DIGITAL_IN_2_OFFSET 6
#define OUTPUT_DIGITAL_IN_2_STOP_BIT 6
#define OUTPUT_DIGITAL_IN_2_START_BIT 6
#define OUTPUT_DIGITAL_IN_2_WIDTH 1


typedef enum path_in2 {
    IN2_CONNECTED_TO_HSR_E,
    IN2_MIXED_WITH_SIDETONE_E
} PATH_IN_2_T ;
#define OUTPUT_DIGITAL_IN_2_PARAM_MASK  0x40



//  
//  0: DA_IN1 direct path disabled. Path from sidetone FIR connected to HSL, 
//  1: DA_IN1 path mixed together with sidetone FIR..
//  Register DigitalMultiplexers 0x0D3F, Bits 7:7, typedef path_in1
//   
//  
#define OUTPUT_DIGITAL_IN_1_MASK 0x80U
#define OUTPUT_DIGITAL_IN_1_OFFSET 7
#define OUTPUT_DIGITAL_IN_1_STOP_BIT 7
#define OUTPUT_DIGITAL_IN_1_START_BIT 7
#define OUTPUT_DIGITAL_IN_1_WIDTH 1


typedef enum path_in1 {
    IN1_CONNECTED_TO_HSL_E,
    IN1_MIXED_WITH_SIDETONE_E
} PATH_IN_1_T ;
#define OUTPUT_DIGITAL_IN_1_PARAM_MASK  0x80



//  
//  00: FIR2 data comes from AD_OUT2 
//  01: FIR2 data comes from AD_OUT3 
//  10: FIR2 data comes from AD_OUT4 
//  11: FIR2 data comes from DA_IN2.
//  Register DigitalMultiplexersMsb 0x0D40, Bits 0:1, typedef selection_fir2
//   
//  
#define FIR_2_SELECTION_MASK 0x3U
#define FIR_2_SELECTION_OFFSET 1
#define FIR_2_SELECTION_STOP_BIT 1
#define FIR_2_SELECTION_START_BIT 0
#define FIR_2_SELECTION_WIDTH 2

#define DIGITAL_MULTIPLEXERS_MSB_REG 0xD40
#define  FIR2_AD_OUT2_SELECTED 0
#define  FIR2_AD_OUT3_SELECTED 1
#define  FIR2_AD_OUT4_SELECTED 2
#define  FIR2_DA_IN2_SELECTED 3

typedef enum selection_fir2 {
    FIR2_AD_OUT2_SELECTED_E,
    FIR2_AD_OUT3_SELECTED_E,
    FIR2_AD_OUT4_SELECTED_E,
    FIR2_DA_IN2_SELECTED_E
} SELECTION_FIR_2_T ;
#define FIR_2_SELECTION_PARAM_MASK  0x3



//  
//  00: FIR1 data comes from AD_OUT1 
//  01: FIR1 data comes from AD_OUT2 
//  10: FIR1 data comes from AD_OUT3 
//  11: FIR1 data comes from DA_IN1.
//  Register DigitalMultiplexersMsb 0x0D40, Bits 2:3, typedef selection_fir1
//   
//  
#define FIR_1_SELECTION_MASK 0xCU
#define FIR_1_SELECTION_OFFSET 3
#define FIR_1_SELECTION_STOP_BIT 3
#define FIR_1_SELECTION_START_BIT 2
#define FIR_1_SELECTION_WIDTH 2

#define  FIR1_AD_OUT1_SELECTED 0
#define  FIR1_AD_OUT2_SELECTED 1
#define  FIR1_AD_OUT3_SELECTED 2
#define  FIR1_DA_IN1_SELECTED 3

typedef enum selection_fir1 {
    FIR1_AD_OUT1_SELECTED_E,
    FIR1_AD_OUT2_SELECTED_E,
    FIR1_AD_OUT3_SELECTED_E,
    FIR1_DA_IN1_SELECTED_E
} SELECTION_FIR_1_T ;
#define FIR_1_SELECTION_PARAM_MASK  0xC



//  
//  0: HFL data comes from DA3 if DAToHfLEn=1 
//  1: HFL data comes from ANC if DAToHfLEn=1.
//  Register DigitalMultiplexersMsb 0x0D40, Bits 4:4, typedef da3_mixing_enable
//   
//  
#define DA_3_MIXING_MASK 0x10U
#define DA_3_MIXING_OFFSET 4
#define DA_3_MIXING_STOP_BIT 4
#define DA_3_MIXING_START_BIT 4
#define DA_3_MIXING_WIDTH 1


typedef enum da3_mixing_enable {
    DA_3_MIXING_ENABLE_E,
    DA_3_MIXING_DISABLE_E
} DA_3_MIXING_ENABLE_T ;
#define DA_3_MIXING_PARAM_MASK  0x10



//  
//  0: HFR data comes from DA4 if DAToHfREn=1 
//  1: HFR data comes from ANC if DAToHfREn=.
//  Register DigitalMultiplexersMsb 0x0D40, Bits 5:5, typedef da4_mixing_enable
//   
//  
#define DA_4_MIXING_MASK 0x20U
#define DA_4_MIXING_OFFSET 5
#define DA_4_MIXING_STOP_BIT 5
#define DA_4_MIXING_START_BIT 5
#define DA_4_MIXING_WIDTH 1


typedef enum da4_mixing_enable {
    DA_4_MIXING_ENABLE_E,
    DA_4_MIXING_DISABLE_E
} DA_4_MIXING_ENABLE_T ;
#define DA_4_MIXING_PARAM_MASK  0x20



//  
//  0: DA3 or ANC path to HfL is muted 
//  1: DA3 or ANC path to HfL is enabled.
//  Register DigitalMultiplexersMsb 0x0D40, Bits 6:6, typedef da3_or_anc_mixing_enable
//   
//  
#define DA_3_OR_ANC_LOOP_PATH_MASK 0x40U
#define DA_3_OR_ANC_LOOP_PATH_OFFSET 6
#define DA_3_OR_ANC_LOOP_PATH_STOP_BIT 6
#define DA_3_OR_ANC_LOOP_PATH_START_BIT 6
#define DA_3_OR_ANC_LOOP_PATH_WIDTH 1


typedef enum da3_or_anc_mixing_enable {
    DA_3_OR_ANC_MIXING_DISABLE_E,
    DA_3_OR_ANC_MIXING_ENABLE_E
} DA_3_OR_ANC_MIXING_ENABLE_T ;
#define DA_3_OR_ANC_LOOP_PATH_PARAM_MASK  0x40



//  
//  0: DA4 or ANC path to HfR is muted 
//  1: DA4 or ANC path to HfR is enabled.
//  Register DigitalMultiplexersMsb 0x0D40, Bits 7:7, typedef da4_or_anc_mixing_enable
//   
//  
#define DA_4_OR_ANC_LOOP_PATH_MASK 0x80U
#define DA_4_OR_ANC_LOOP_PATH_OFFSET 7
#define DA_4_OR_ANC_LOOP_PATH_STOP_BIT 7
#define DA_4_OR_ANC_LOOP_PATH_START_BIT 7
#define DA_4_OR_ANC_LOOP_PATH_WIDTH 1


typedef enum da4_or_anc_mixing_enable {
    DA_4_OR_ANC_MIXING_DISABLE_E,
    DA_4_OR_ANC_MIXING_ENABLE_E
} DA_4_OR_ANC_MIXING_ENABLE_T ;
#define DA_4_OR_ANC_LOOP_PATH_PARAM_MASK  0x80


#define AD_1_GAIN_PARAM_MASK  0x3F
#define AD_1_GAIN_MASK 0x3FU
#define AD_1_GAIN_OFFSET 5
#define AD_1_GAIN_STOP_BIT 5
#define AD_1_GAIN_START_BIT 0
#define AD_1_GAIN_WIDTH 6

typedef enum digital_gain {
    AD_DIGITAL_GAIN_31_DB = 0x0,
    AD_DIGITAL_GAIN_30_DB = 0x1,
    AD_DIGITAL_GAIN_29_DB = 0x2,
    AD_DIGITAL_GAIN_28_DB = 0x3,
    AD_DIGITAL_GAIN_27_DB = 0x4,
    AD_DIGITAL_GAIN_26_DB = 0x5,
    AD_DIGITAL_GAIN_25_DB = 0x6,
    AD_DIGITAL_GAIN_24_DB = 0x7,
    AD_DIGITAL_GAIN_23_DB = 0x8,
    AD_DIGITAL_GAIN_22_DB = 0x9,
    AD_DIGITAL_GAIN_21_DB = 0xA,
    AD_DIGITAL_GAIN_20_DB = 0xB,
    AD_DIGITAL_GAIN_19_DB = 0xC,
    AD_DIGITAL_GAIN_18_DB = 0xD,
    AD_DIGITAL_GAIN_17_DB = 0xE,
    AD_DIGITAL_GAIN_16_DB = 0xF,
    AD_DIGITAL_GAIN_15_DB = 0x10,
    AD_DIGITAL_GAIN_14_DB = 0x11,
    AD_DIGITAL_GAIN_13_DB = 0x12,
    AD_DIGITAL_GAIN_12_DB = 0x13,
    AD_DIGITAL_GAIN_11_DB = 0x14,
    AD_DIGITAL_GAIN_10_DB = 0x15,
    AD_DIGITAL_GAIN_9_DB = 0x16,
    AD_DIGITAL_GAIN_8_DB = 0x17,
    AD_DIGITAL_GAIN_7_DB = 0x18,
    AD_DIGITAL_GAIN_6_DB = 0x19,
    AD_DIGITAL_GAIN_5_DB = 0x1A,
    AD_DIGITAL_GAIN_4_DB = 0x1B,
    AD_DIGITAL_GAIN_3_DB = 0x1C,
    AD_DIGITAL_GAIN_2_DB = 0x1D,
    AD_DIGITAL_GAIN_1_DB = 0x1E,
    AD_DIGITAL_GAIN_0_DB = 0x1F,
    AD_DIGITAL_GAIN_MINUS_1_DB = 0x20,
    AD_DIGITAL_GAIN_MINUS_2_DB = 0x21,
    AD_DIGITAL_GAIN_MINUS_3_DB = 0x22,
    AD_DIGITAL_GAIN_MINUS_4_DB = 0x23,
    AD_DIGITAL_GAIN_MINUS_5_DB = 0x24,
    AD_DIGITAL_GAIN_MINUS_6_DB = 0x25,
    AD_DIGITAL_GAIN_MINUS_7_DB = 0x26,
    AD_DIGITAL_GAIN_MINUS_8_DB = 0x27,
    AD_DIGITAL_GAIN_MINUS_9_DB = 0x28,
    AD_DIGITAL_GAIN_MINUS_10_DB = 0x29,
    AD_DIGITAL_GAIN_MINUS_11_DB = 0x2A,
    AD_DIGITAL_GAIN_MINUS_12_DB = 0x2B,
    AD_DIGITAL_GAIN_MINUS_13_DB = 0x2C,
    AD_DIGITAL_GAIN_MINUS_14_DB = 0x2D,
    AD_DIGITAL_GAIN_MINUS_15_DB = 0x2E,
    AD_DIGITAL_GAIN_MINUS_16_DB = 0x2F,
    AD_DIGITAL_GAIN_MINUS_17_DB = 0x30,
    AD_DIGITAL_GAIN_MINUS_18_DB = 0x31,
    AD_DIGITAL_GAIN_MINUS_19_DB = 0x32,
    AD_DIGITAL_GAIN_MINUS_20_DB = 0x33,
    AD_DIGITAL_GAIN_MINUS_21_DB = 0x34,
    AD_DIGITAL_GAIN_MINUS_22_DB = 0x35,
    AD_DIGITAL_GAIN_MINUS_23_DB = 0x36,
    AD_DIGITAL_GAIN_MINUS_24_DB = 0x37,
    AD_DIGITAL_GAIN_MINUS_25_DB = 0x38,
    AD_DIGITAL_GAIN_MINUS_26_DB = 0x39,
    AD_DIGITAL_GAIN_MINUS_27_DB = 0x3A,
    AD_DIGITAL_GAIN_MINUS_28_DB = 0x3B,
    AD_DIGITAL_GAIN_MINUS_29_DB = 0x3C,
    AD_DIGITAL_GAIN_MINUS_30_DB = 0x3D,
    AD_DIGITAL_GAIN_MINUS_31_DB = 0x3E,
    AD_DIGITAL_GAIN_MINUS_32_DB = 0x3F
} DIGITAL_GAIN_T ;

//  
//  0: Fading (anti-click) on AD1 path is active; 
//  1: Fading (anti-click) on AD1 path is disabled.
//  Register DigitalGainAD1 0x0D41, Bits 6:6, typedef fading
//   
//  
#define AD_1_ANTI_CLICK_MASK 0x40U
#define AD_1_ANTI_CLICK_OFFSET 6
#define AD_1_ANTI_CLICK_STOP_BIT 6
#define AD_1_ANTI_CLICK_START_BIT 6
#define AD_1_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_AD1_REG 0xD41

typedef enum fading {
    FADING_ACTIVE_E,
    FADING_DISABLE_E
} FADING_T ;
#define AD_1_ANTI_CLICK_PARAM_MASK  0x40


#define AD_2_GAIN_PARAM_MASK  0x3F
#define AD_2_GAIN_MASK 0x3FU
#define AD_2_GAIN_OFFSET 5
#define AD_2_GAIN_STOP_BIT 5
#define AD_2_GAIN_START_BIT 0
#define AD_2_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on AD2 path is active; 
//  1: Fading (anti-click) on AD2 path is disabled.
//  Register DigitalGainAD2 0x0D42, Bits 6:6, typedef fading
//   
//  
#define AD_2_ANTI_CLICK_MASK 0x40U
#define AD_2_ANTI_CLICK_OFFSET 6
#define AD_2_ANTI_CLICK_STOP_BIT 6
#define AD_2_ANTI_CLICK_START_BIT 6
#define AD_2_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_AD2_REG 0xD42

#define AD_2_ANTI_CLICK_PARAM_MASK  0x40


#define AD_3_GAIN_PARAM_MASK  0x3F
#define AD_3_GAIN_MASK 0x3FU
#define AD_3_GAIN_OFFSET 5
#define AD_3_GAIN_STOP_BIT 5
#define AD_3_GAIN_START_BIT 0
#define AD_3_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on AD3 path is active; 
//  1: Fading (anti-click) on AD3 path is disabled.
//  Register DigitalGainAD3 0x0D43, Bits 6:6, typedef fading
//   
//  
#define AD_3_ANTI_CLICK_MASK 0x40U
#define AD_3_ANTI_CLICK_OFFSET 6
#define AD_3_ANTI_CLICK_STOP_BIT 6
#define AD_3_ANTI_CLICK_START_BIT 6
#define AD_3_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_AD3_REG 0xD43

#define AD_3_ANTI_CLICK_PARAM_MASK  0x40


#define AD_4_GAIN_PARAM_MASK  0x3F
#define AD_4_GAIN_MASK 0x3FU
#define AD_4_GAIN_OFFSET 5
#define AD_4_GAIN_STOP_BIT 5
#define AD_4_GAIN_START_BIT 0
#define AD_4_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on AD4 path is active; 
//  1: Fading (anti-click) on AD4 path is disabled.
//  Register DigitalGainAD4 0x0D44, Bits 6:6, typedef fading
//   
//  
#define AD_4_ANTI_CLICK_MASK 0x40U
#define AD_4_ANTI_CLICK_OFFSET 6
#define AD_4_ANTI_CLICK_STOP_BIT 6
#define AD_4_ANTI_CLICK_START_BIT 6
#define AD_4_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_AD4_REG 0xD44

#define AD_4_ANTI_CLICK_PARAM_MASK  0x40


#define AD_5_GAIN_PARAM_MASK  0x3F
#define AD_5_GAIN_MASK 0x3FU
#define AD_5_GAIN_OFFSET 5
#define AD_5_GAIN_STOP_BIT 5
#define AD_5_GAIN_START_BIT 0
#define AD_5_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on AD5 path is active; 
//  1: Fading (anti-click) on AD5 path is disabled.
//  Register DigitalGainAD5 0x0D45, Bits 6:6, typedef fading
//   
//  
#define AD_5_ANTI_CLICK_MASK 0x40U
#define AD_5_ANTI_CLICK_OFFSET 6
#define AD_5_ANTI_CLICK_STOP_BIT 6
#define AD_5_ANTI_CLICK_START_BIT 6
#define AD_5_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_AD5_REG 0xD45

#define AD_5_ANTI_CLICK_PARAM_MASK  0x40


#define AD_6_GAIN_PARAM_MASK  0x3F
#define AD_6_GAIN_MASK 0x3FU
#define AD_6_GAIN_OFFSET 5
#define AD_6_GAIN_STOP_BIT 5
#define AD_6_GAIN_START_BIT 0
#define AD_6_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on AD6 path is active; 
//  1: Fading (anti-click) on AD6 path is disabled.
//  Register DigitalGainAD6 0x0D46, Bits 6:6, typedef fading
//   
//  
#define AD_6_ANTI_CLICK_MASK 0x40U
#define AD_6_ANTI_CLICK_OFFSET 6
#define AD_6_ANTI_CLICK_STOP_BIT 6
#define AD_6_ANTI_CLICK_START_BIT 6
#define AD_6_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_AD6_REG 0xD46

#define AD_6_ANTI_CLICK_PARAM_MASK  0x40


#define DA_1_GAIN_PARAM_MASK  0x3F
#define DA_1_GAIN_MASK 0x3FU
#define DA_1_GAIN_OFFSET 5
#define DA_1_GAIN_STOP_BIT 5
#define DA_1_GAIN_START_BIT 0
#define DA_1_GAIN_WIDTH 6

typedef enum da_digital_gain {
    DA_DIGITAL_GAIN_0_DB = 0x0,
    DA_DIGITAL_GAIN_MINUS_1_DB = 0x1,
    DA_DIGITAL_GAIN_MINUS_2_DB = 0x2,
    DA_DIGITAL_GAIN_MINUS_3_DB = 0x3,
    DA_DIGITAL_GAIN_MINUS_4_DB = 0x4,
    DA_DIGITAL_GAIN_MINUS_5_DB = 0x5,
    DA_DIGITAL_GAIN_MINUS_6_DB = 0x6,
    DA_DIGITAL_GAIN_MINUS_7_DB = 0x7,
    DA_DIGITAL_GAIN_MINUS_8_DB = 0x8,
    DA_DIGITAL_GAIN_MINUS_9_DB = 0x9,
    DA_DIGITAL_GAIN_MINUS_10_DB = 0xA,
    DA_DIGITAL_GAIN_MINUS_11_DB = 0xB,
    DA_DIGITAL_GAIN_MINUS_12_DB = 0xC,
    DA_DIGITAL_GAIN_MINUS_13_DB = 0xD,
    DA_DIGITAL_GAIN_MINUS_14_DB = 0xE,
    DA_DIGITAL_GAIN_MINUS_15_DB = 0xF,
    DA_DIGITAL_GAIN_MINUS_16_DB = 0x10,
    DA_DIGITAL_GAIN_MINUS_17_DB = 0x11,
    DA_DIGITAL_GAIN_MINUS_18_DB = 0x12,
    DA_DIGITAL_GAIN_MINUS_19_DB = 0x13,
    DA_DIGITAL_GAIN_MINUS_20_DB = 0x14,
    DA_DIGITAL_GAIN_MINUS_21_DB = 0x15,
    DA_DIGITAL_GAIN_MINUS_22_DB = 0x16,
    DA_DIGITAL_GAIN_MINUS_23_DB = 0x17,
    DA_DIGITAL_GAIN_MINUS_24_DB = 0x18,
    DA_DIGITAL_GAIN_MINUS_25_DB = 0x19,
    DA_DIGITAL_GAIN_MINUS_26_DB = 0x1A,
    DA_DIGITAL_GAIN_MINUS_27_DB = 0x1B,
    DA_DIGITAL_GAIN_MINUS_28_DB = 0x1C,
    DA_DIGITAL_GAIN_MINUS_29_DB = 0x1D,
    DA_DIGITAL_GAIN_MINUS_30_DB = 0x1E,
    DA_DIGITAL_GAIN_MINUS_31_DB = 0x1F,
    DA_DIGITAL_GAIN_MINUS_32_DB = 0x20,
    DA_DIGITAL_GAIN_MINUS_33_DB = 0x21,
    DA_DIGITAL_GAIN_MINUS_34_DB = 0x22,
    DA_DIGITAL_GAIN_MINUS_35_DB = 0x23,
    DA_DIGITAL_GAIN_MINUS_36_DB = 0x24,
    DA_DIGITAL_GAIN_MINUS_37_DB = 0x25,
    DA_DIGITAL_GAIN_MINUS_38_DB = 0x26,
    DA_DIGITAL_GAIN_MINUS_39_DB = 0x27,
    DA_DIGITAL_GAIN_MINUS_40_DB = 0x28,
    DA_DIGITAL_GAIN_MINUS_41_DB = 0x29,
    DA_DIGITAL_GAIN_MINUS_42_DB = 0x2A,
    DA_DIGITAL_GAIN_MINUS_43_DB = 0x2B,
    DA_DIGITAL_GAIN_MINUS_44_DB = 0x2C,
    DA_DIGITAL_GAIN_MINUS_45_DB = 0x2D,
    DA_DIGITAL_GAIN_MINUS_46_DB = 0x2E,
    DA_DIGITAL_GAIN_MINUS_47_DB = 0x2F,
    DA_DIGITAL_GAIN_MINUS_48_DB = 0x30,
    DA_DIGITAL_GAIN_MINUS_49_DB = 0x31,
    DA_DIGITAL_GAIN_MINUS_50_DB = 0x32,
    DA_DIGITAL_GAIN_MINUS_51_DB = 0x33,
    DA_DIGITAL_GAIN_MINUS_52_DB = 0x34,
    DA_DIGITAL_GAIN_MINUS_53_DB = 0x35,
    DA_DIGITAL_GAIN_MINUS_54_DB = 0x36,
    DA_DIGITAL_GAIN_MINUS_55_DB = 0x37,
    DA_DIGITAL_GAIN_MINUS_56_DB = 0x38,
    DA_DIGITAL_GAIN_MINUS_57_DB = 0x39,
    DA_DIGITAL_GAIN_MINUS_58_DB = 0x3A,
    DA_DIGITAL_GAIN_MINUS_59_DB = 0x3B,
    DA_DIGITAL_GAIN_MINUS_60_DB = 0x3C,
    DA_DIGITAL_GAIN_MINUS_61_DB = 0x3D,
    DA_DIGITAL_GAIN_MINUS_62_DB = 0x3E,
    DA_DIGITAL_GAIN_MINUS_63_DB = 0x3F
} DA_DIGITAL_GAIN_T ;

//  
//  0: Fading (anti-click) on DA1 path is active; 
//  1: Fading (anti-click) on DA1 path is disabled.
//  Register DigitalGainDa1 0x0D47, Bits 6:6, typedef fading
//   
//  
#define DA_1_ANTI_CLICK_MASK 0x40U
#define DA_1_ANTI_CLICK_OFFSET 6
#define DA_1_ANTI_CLICK_STOP_BIT 6
#define DA_1_ANTI_CLICK_START_BIT 6
#define DA_1_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_DA_1_REG 0xD47

#define DA_1_ANTI_CLICK_PARAM_MASK  0x40


#define DA_2_GAIN_PARAM_MASK  0x3F
#define DA_2_GAIN_MASK 0x3FU
#define DA_2_GAIN_OFFSET 5
#define DA_2_GAIN_STOP_BIT 5
#define DA_2_GAIN_START_BIT 0
#define DA_2_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on DA2 path is active; 
//  1: Fading (anti-click) on DA2 path is disabled.
//  Register DigitalGainDa2 0x0D48, Bits 6:6, typedef fading
//   
//  
#define DA_2_ANTI_CLICK_MASK 0x40U
#define DA_2_ANTI_CLICK_OFFSET 6
#define DA_2_ANTI_CLICK_STOP_BIT 6
#define DA_2_ANTI_CLICK_START_BIT 6
#define DA_2_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_DA_2_REG 0xD48

#define DA_2_ANTI_CLICK_PARAM_MASK  0x40


#define DA_3_GAIN_PARAM_MASK  0x3F
#define DA_3_GAIN_MASK 0x3FU
#define DA_3_GAIN_OFFSET 5
#define DA_3_GAIN_STOP_BIT 5
#define DA_3_GAIN_START_BIT 0
#define DA_3_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on DA3 path is active; 
//  1: Fading (anti-click) on DA3 path is disabled.
//  Register DigitalGainDa3 0x0D49, Bits 6:6, typedef fading
//   
//  
#define DA_3_ANTI_CLICK_MASK 0x40U
#define DA_3_ANTI_CLICK_OFFSET 6
#define DA_3_ANTI_CLICK_STOP_BIT 6
#define DA_3_ANTI_CLICK_START_BIT 6
#define DA_3_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_DA_3_REG 0xD49

#define DA_3_ANTI_CLICK_PARAM_MASK  0x40


#define DA_4_GAIN_PARAM_MASK  0x3F
#define DA_4_GAIN_MASK 0x3FU
#define DA_4_GAIN_OFFSET 5
#define DA_4_GAIN_STOP_BIT 5
#define DA_4_GAIN_START_BIT 0
#define DA_4_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on DA4 path is active; 
//  1: Fading (anti-click) on DA4 path is disabled.
//  Register DigitalGainDa4 0x0D4A, Bits 6:6, typedef fading
//   
//  
#define DA_4_ANTI_CLICK_MASK 0x40U
#define DA_4_ANTI_CLICK_OFFSET 6
#define DA_4_ANTI_CLICK_STOP_BIT 6
#define DA_4_ANTI_CLICK_START_BIT 6
#define DA_4_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_DA_4_REG 0xD4A

#define DA_4_ANTI_CLICK_PARAM_MASK  0x40


#define DA_5_GAIN_PARAM_MASK  0x3F
#define DA_5_GAIN_MASK 0x3FU
#define DA_5_GAIN_OFFSET 5
#define DA_5_GAIN_STOP_BIT 5
#define DA_5_GAIN_START_BIT 0
#define DA_5_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on DA5 path is active; 
//  1: Fading (anti-click) on DA5 path is disabled.
//  Register DigitalGainDa5 0x0D4B, Bits 6:6, typedef fading
//   
//  
#define DA_5_ANTI_CLICK_MASK 0x40U
#define DA_5_ANTI_CLICK_OFFSET 6
#define DA_5_ANTI_CLICK_STOP_BIT 6
#define DA_5_ANTI_CLICK_START_BIT 6
#define DA_5_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_DA_5_REG 0xD4B

#define DA_5_ANTI_CLICK_PARAM_MASK  0x40


#define DA_6_GAIN_PARAM_MASK  0x3F
#define DA_6_GAIN_MASK 0x3FU
#define DA_6_GAIN_OFFSET 5
#define DA_6_GAIN_STOP_BIT 5
#define DA_6_GAIN_START_BIT 0
#define DA_6_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on DA6 path is active; 
//  1: Fading (anti-click) on DA6 path is disabled.
//  Register DigitalGainDA6 0x0D4C, Bits 6:6, typedef fading
//   
//  
#define DA_6_ANTI_CLICK_MASK 0x40U
#define DA_6_ANTI_CLICK_OFFSET 6
#define DA_6_ANTI_CLICK_STOP_BIT 6
#define DA_6_ANTI_CLICK_START_BIT 6
#define DA_6_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_DA6_REG 0xD4C

#define DA_6_ANTI_CLICK_PARAM_MASK  0x40


#define AD_1_HFL_GAIN_PARAM_MASK  0x3F
#define AD_1_HFL_GAIN_MASK 0x3FU
#define AD_1_HFL_GAIN_OFFSET 5
#define AD_1_HFL_GAIN_STOP_BIT 5
#define AD_1_HFL_GAIN_START_BIT 0
#define AD_1_HFL_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on AD1_HFL path is active; 
//  1: Fading (anti-click) on AD1_HFL path is disabled.
//  Register DigitalGainAD1_HFL 0x0D4D, Bits 6:6, typedef fading
//   
//  
#define AD_1_HFL_ANTI_CLICK_MASK 0x40U
#define AD_1_HFL_ANTI_CLICK_OFFSET 6
#define AD_1_HFL_ANTI_CLICK_STOP_BIT 6
#define AD_1_HFL_ANTI_CLICK_START_BIT 6
#define AD_1_HFL_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_AD1HFL_REG 0xD4D

#define AD_1_HFL_ANTI_CLICK_PARAM_MASK  0x40


#define AD_2_HFR_GAIN_PARAM_MASK  0x3F
#define AD_2_HFR_GAIN_MASK 0x3FU
#define AD_2_HFR_GAIN_OFFSET 5
#define AD_2_HFR_GAIN_STOP_BIT 5
#define AD_2_HFR_GAIN_START_BIT 0
#define AD_2_HFR_GAIN_WIDTH 6


//  
//  0: Fading (anti-click) on AD2_HFR path is active; 
//  1: Fading (anti-click) on AD2_HFR path is disabled.
//  Register DigitalGainAD2_HFR 0x0D4E, Bits 6:6, typedef fading
//   
//  
#define AD_2_HFR_R_ANTI_CLICK_MASK 0x40U
#define AD_2_HFR_R_ANTI_CLICK_OFFSET 6
#define AD_2_HFR_R_ANTI_CLICK_STOP_BIT 6
#define AD_2_HFR_R_ANTI_CLICK_START_BIT 6
#define AD_2_HFR_R_ANTI_CLICK_WIDTH 1

#define DIGITAL_GAIN_AD2HFR_REG 0xD4E

#define AD_2_HFR_R_ANTI_CLICK_PARAM_MASK  0x40


#define LEFT_HEADSET_GAIN_PARAM_MASK  0xF
#define LEFT_HEADSET_GAIN_MASK 0xFU
#define LEFT_HEADSET_GAIN_OFFSET 3
#define LEFT_HEADSET_GAIN_STOP_BIT 3
#define LEFT_HEADSET_GAIN_START_BIT 0
#define LEFT_HEADSET_GAIN_WIDTH 4

typedef enum headset_gain {
    HEADSET_DIGITAL_GAIN_8_DB = 0x0,
    HEADSET_DIGITAL_GAIN_7_DB = 0x1,
    HEADSET_DIGITAL_GAIN_6_DB = 0x2,
    HEADSET_DIGITAL_GAIN_5_DB = 0x3,
    HEADSET_DIGITAL_GAIN_4_DB = 0x4,
    HEADSET_DIGITAL_GAIN_3_DB = 0x5,
    HEADSET_DIGITAL_GAIN_2_DB = 0x6,
    HEADSET_DIGITAL_GAIN_1_DB = 0x7,
    HEADSET_DIGITAL_GAIN_0_DB = 0x8,
    HEADSET_DIGITAL_GAIN_MINUS_1_DB = 0x9,
    HEADSET_DIGITAL_GAIN_MINUS_2_DB = 0xA
} HEADSET_GAIN_T ;

//  
//  0: Fading (anti-click) on HFL digital gain is active 
//  1: Fading (anti-click) on HFL digital gain is disabled.
//  Register DigitalGainHeadsetLeft 0x0D4F, Bits 4:4, typedef fading
//   
//  
#define FADING_HEADSET_LEFT_MASK 0x10U
#define FADING_HEADSET_LEFT_OFFSET 4
#define FADING_HEADSET_LEFT_STOP_BIT 4
#define FADING_HEADSET_LEFT_START_BIT 4
#define FADING_HEADSET_LEFT_WIDTH 1

#define DIGITAL_GAIN_HEADSET_LEFT_REG 0xD4F

#define FADING_HEADSET_LEFT_PARAM_MASK  0x10



//  
//  0: sinc3 intepolator choosen for HS or Earpiece drivers 
//  1: sinc1 (hold) intepolator choosen for HS or Earpiece driver..
//  Register DigitalGainHeadsetLeft 0x0D4F, Bits 7:7, typedef sinc1_interpolator_enable
//   
//  
#define SINC_1_INTERPOLATOR_MASK 0x80U
#define SINC_1_INTERPOLATOR_OFFSET 7
#define SINC_1_INTERPOLATOR_STOP_BIT 7
#define SINC_1_INTERPOLATOR_START_BIT 7
#define SINC_1_INTERPOLATOR_WIDTH 1


typedef enum sinc1_interpolator_enable {
    SINC_1_INTERPOLATOR_DISABLE_E,
    SINC_1_INTERPOLATOR_ENABLE_E
} SINC_1_INTERPOLATOR_ENABLE_T ;
#define SINC_1_INTERPOLATOR_PARAM_MASK  0x80


#define RIGHT_HEADSET_GAIN_PARAM_MASK  0xF
#define RIGHT_HEADSET_GAIN_MASK 0xFU
#define RIGHT_HEADSET_GAIN_OFFSET 3
#define RIGHT_HEADSET_GAIN_STOP_BIT 3
#define RIGHT_HEADSET_GAIN_START_BIT 0
#define RIGHT_HEADSET_GAIN_WIDTH 4


//  
//  0: Fading (anti-click) on HFR digital gain is active 
//  1: Fading (anti-click) on HFR digital gain is disabled.
//  Register DigitalGainHeadsetRight 0x0D50, Bits 4:4, typedef fading
//   
//  
#define FADING_HEADSET_RIGHT_MASK 0x10U
#define FADING_HEADSET_RIGHT_OFFSET 4
#define FADING_HEADSET_RIGHT_STOP_BIT 4
#define FADING_HEADSET_RIGHT_START_BIT 4
#define FADING_HEADSET_RIGHT_WIDTH 1

#define DIGITAL_GAIN_HEADSET_RIGHT_REG 0xD50

#define FADING_HEADSET_RIGHT_PARAM_MASK  0x10



//  
//  Approximate Time between each 1dB step Gain Change in digital gain controls.
//  Register DigitalGainHeadsetRight 0x0D50, Bits 6:7, typedef headset_fade_speed
//   
//  
#define HEADSET_FADE_SPEED_MASK 0xC0U
#define HEADSET_FADE_SPEED_OFFSET 7
#define HEADSET_FADE_SPEED_STOP_BIT 7
#define HEADSET_FADE_SPEED_START_BIT 6
#define HEADSET_FADE_SPEED_WIDTH 2

#define  HEADSET_TIME_1MS 0
#define  HEADSET_TIME_4MS 1
#define  HEADSET_TIME_8MS 2
#define  HEADSET_TIME_16MS 3

typedef enum headset_fade_speed {
    HEADSET_TIME_1MS_E,
    HEADSET_TIME_4MS_E,
    HEADSET_TIME_8MS_E,
    HEADSET_TIME_16MS_E
} HEADSET_FADE_SPEED_T ;
#define HEADSET_FADE_SPEED_PARAM_MASK  0xC0


#define DIGITAL_GAIN_SIDE_TONE_1_PARAM_MASK  0x1F
#define DIGITAL_GAIN_SIDE_TONE_1_MASK 0x1FU
#define DIGITAL_GAIN_SIDE_TONE_1_OFFSET 4
#define DIGITAL_GAIN_SIDE_TONE_1_STOP_BIT 4
#define DIGITAL_GAIN_SIDE_TONE_1_START_BIT 0
#define DIGITAL_GAIN_SIDE_TONE_1_WIDTH 5

typedef enum digital_gain_side_tone {
    DIGITAL_GAIN_SIDE_TONE_0_DB = 0x0,
    DIGITAL_GAIN_SIDE_TONE_MINUS_1_DB = 0x1,
    DIGITAL_GAIN_SIDE_TONE_MINUS_2_DB = 0x2,
    DIGITAL_GAIN_SIDE_TONE_MINUS_3_DB = 0x3,
    DIGITAL_GAIN_SIDE_TONE_MINUS_4_DB = 0x4,
    DIGITAL_GAIN_SIDE_TONE_MINUS_5_DB = 0x5,
    DIGITAL_GAIN_SIDE_TONE_MINUS_6_DB = 0x6,
    DIGITAL_GAIN_SIDE_TONE_MINUS_7_DB = 0x7,
    DIGITAL_GAIN_SIDE_TONE_MINUS_8_DB = 0x8,
    DIGITAL_GAIN_SIDE_TONE_MINUS_9_DB = 0x9,
    DIGITAL_GAIN_SIDE_TONE_MINUS_10_DB = 0xA,
    DIGITAL_GAIN_SIDE_TONE_MINUS_11_DB = 0xB,
    DIGITAL_GAIN_SIDE_TONE_MINUS_12_DB = 0xC,
    DIGITAL_GAIN_SIDE_TONE_MINUS_13_DB = 0xD,
    DIGITAL_GAIN_SIDE_TONE_MINUS_14_DB = 0xE,
    DIGITAL_GAIN_SIDE_TONE_MINUS_15_DB = 0xF,
    DIGITAL_GAIN_SIDE_TONE_MINUS_16_DB = 0x10,
    DIGITAL_GAIN_SIDE_TONE_MINUS_17_DB = 0x11,
    DIGITAL_GAIN_SIDE_TONE_MINUS_18_DB = 0x12,
    DIGITAL_GAIN_SIDE_TONE_MINUS_19_DB = 0x13,
    DIGITAL_GAIN_SIDE_TONE_MINUS_20_DB = 0x14,
    DIGITAL_GAIN_SIDE_TONE_MINUS_21_DB = 0x15,
    DIGITAL_GAIN_SIDE_TONE_MINUS_22_DB = 0x16,
    DIGITAL_GAIN_SIDE_TONE_MINUS_23_DB = 0x17,
    DIGITAL_GAIN_SIDE_TONE_MINUS_24_DB = 0x18,
    DIGITAL_GAIN_SIDE_TONE_MINUS_25_DB = 0x19,
    DIGITAL_GAIN_SIDE_TONE_MINUS_26_DB = 0x1A,
    DIGITAL_GAIN_SIDE_TONE_MINUS_27_DB = 0x1B,
    DIGITAL_GAIN_SIDE_TONE_MINUS_28_DB = 0x1C,
    DIGITAL_GAIN_SIDE_TONE_MINUS_29_DB = 0x1D,
    DIGITAL_GAIN_SIDE_TONE_MINUS_30_DB = 0x1E,
    DIGITAL_GAIN_SIDE_TONE_MINUS_31_DB = 0x1F,
    DIGITAL_GAIN_SIDE_TONE_MINUS_32_DB = 0x20
} DIGITAL_GAIN_SIDE_TONE_T ;
#define DIGITAL_GAIN_SIDE_TONE_1_REG 0xD51
#define DIGITAL_GAIN_SIDE_TONE_2_PARAM_MASK  0x1F
#define DIGITAL_GAIN_SIDE_TONE_2_MASK 0x1FU
#define DIGITAL_GAIN_SIDE_TONE_2_OFFSET 4
#define DIGITAL_GAIN_SIDE_TONE_2_STOP_BIT 4
#define DIGITAL_GAIN_SIDE_TONE_2_START_BIT 0
#define DIGITAL_GAIN_SIDE_TONE_2_WIDTH 5

#define DIGITAL_GAIN_SIDE_TONE_2_REG 0xD52

//  
//  0: ANC FIR coefficient pointer is resetted.1: ANC FIR coefficient pointer is removed 
//  from reset and IIR coefficients can be updated..
//  Register AncDigitalGain 0x0D53, Bits 0:0, typedef fir_coefficient_pointer
//   
//  
#define FIR_COEFFICIENT_POINTER_MASK 0x1U
#define FIR_COEFFICIENT_POINTER_OFFSET 0
#define FIR_COEFFICIENT_POINTER_STOP_BIT 0
#define FIR_COEFFICIENT_POINTER_START_BIT 0
#define FIR_COEFFICIENT_POINTER_WIDTH 1

#define ANC_DIGITAL_GAIN_REG 0xD53

typedef enum fir_coefficient_pointer {
    POINTER_RESET_E,
    POINTER_REMOVED_E
} FIR_COEFFICIENT_POINTER_T ;
#define FIR_COEFFICIENT_POINTER_PARAM_MASK  0x1



//  
//  0: IIR initialization cannot start; 
//  1: Indicate the start of IIR initialization. IIR coefficients can be written for the 
//  first time..
//  Register AncDigitalGain 0x0D53, Bits 1:1, typedef iir_status
//   
//  
#define IIR_STATUS_MASK 0x2U
#define IIR_STATUS_OFFSET 1
#define IIR_STATUS_STOP_BIT 1
#define IIR_STATUS_START_BIT 1
#define IIR_STATUS_WIDTH 1


typedef enum iir_status {
    IIR_CANNOT_START_E,
    IIR_STARTED_E
} IIR_STATUS_T ;
#define IIR_STATUS_PARAM_MASK  0x2



//  
//  0: ANC operation are disabled. FIR and IIR coefficients cannot be accessed for 
//  writing 
//  1: ANC operation are enabled.
//  Register AncDigitalGain 0x0D53, Bits 2:2, typedef anc_operation_enable
//   
//  
#define ANC_OPERATION_MASK 0x4U
#define ANC_OPERATION_OFFSET 2
#define ANC_OPERATION_STOP_BIT 2
#define ANC_OPERATION_START_BIT 2
#define ANC_OPERATION_WIDTH 1


typedef enum anc_operation_enable {
    ANC_OPERATION_DISABLE_E,
    ANC_OPERATION_ENABLE_E
} ANC_OPERATION_ENABLE_T ;
#define ANC_OPERATION_PARAM_MASK  0x4



//  
//  0: ANC IIR coefficient pointer is reset (unless ANCIIRInit is used). 
//  1: ANC IIR coefficient pointer is removed from reset and IIR coefficients can be 
//  updated..
//  Register AncDigitalGain 0x0D53, Bits 3:3, typedef anc_iir_pointer
//   
//  
#define ANC_IIR_UPDATE_MASK 0x8U
#define ANC_IIR_UPDATE_OFFSET 3
#define ANC_IIR_UPDATE_STOP_BIT 3
#define ANC_IIR_UPDATE_START_BIT 3
#define ANC_IIR_UPDATE_WIDTH 1


typedef enum anc_iir_pointer {
    ANC_IIR_POINTER_RESET_E,
    ANC_IIR_POINTER_UPDATED_E
} ANC_IIR_POINTER_T ;
#define ANC_IIR_UPDATE_PARAM_MASK  0x8


#define WARPED_SHIFT_IN_PARAM_MASK  0x1F
#define WARPED_SHIFT_IN_MASK 0x1FU
#define WARPED_SHIFT_IN_OFFSET 4
#define WARPED_SHIFT_IN_STOP_BIT 4
#define WARPED_SHIFT_IN_START_BIT 0
#define WARPED_SHIFT_IN_WIDTH 5

typedef enum dig_anc_step_gain {
    SHIFT_15_STEPS = 0x0,
    SHIFT_14_STEPS = 0x1,
    SHIFT_13_STEPS = 0x2,
    SHIFT_12_STEPS = 0x3,
    SHIFT_11_STEPS = 0x4,
    SHIFT_10_STEPS = 0x5,
    SHIFT_9_STEPS = 0x6,
    SHIFT_8_STEPS = 0x7,
    SHIFT_7_STEPS = 0x8,
    SHIFT_6_STEPS = 0x9,
    SHIFT_5_STEPS = 0xA,
    SHIFT_4_STEPS = 0xB,
    SHIFT_3_STEPS = 0xC,
    SHIFT_2_STEPS = 0xD,
    SHIFT_1_STEPS = 0xE,
    SHIFT_0_STEPS = 0xF,
    SHIFT_MINUS_1_STEPS = 0x10,
    SHIFT_MINUS_2_STEPS = 0x11,
    SHIFT_MINUS_3_STEPS = 0x12,
    SHIFT_MINUS_4_STEPS = 0x13,
    SHIFT_MINUS_5_STEPS = 0x14,
    SHIFT_MINUS_6_STEPS = 0x15,
    SHIFT_MINUS_7_STEPS = 0x16,
    SHIFT_MINUS_8_STEPS = 0x17,
    SHIFT_MINUS_9_STEPS = 0x18,
    SHIFT_MINUS_10_STEPS = 0x19,
    SHIFT_MINUS_11_STEPS = 0x1A,
    SHIFT_MINUS_12_STEPS = 0x1B,
    SHIFT_MINUS_13_STEPS = 0x1C,
    SHIFT_MINUS_14_STEPS = 0x1D,
    SHIFT_MINUS_15_STEPS = 0x1E
} DIG_ANC_STEP_GAIN_T ;
#define WARPED_DELAY_LINE_REG 0xD54
#define FIR_SHIFT_OUT_PARAM_MASK  0x1F
#define FIR_SHIFT_OUT_MASK 0x1FU
#define FIR_SHIFT_OUT_OFFSET 4
#define FIR_SHIFT_OUT_STOP_BIT 4
#define FIR_SHIFT_OUT_START_BIT 0
#define FIR_SHIFT_OUT_WIDTH 5

#define GAIN_FOR_FIR_OUTPUT_REG 0xD55
#define IIR_SHIFT_OUT_PARAM_MASK  0x1F
#define IIR_SHIFT_OUT_MASK 0x1FU
#define IIR_SHIFT_OUT_OFFSET 4
#define IIR_SHIFT_OUT_STOP_BIT 4
#define IIR_SHIFT_OUT_START_BIT 0
#define IIR_SHIFT_OUT_WIDTH 5

#define GAIN_FOR_IRR_OUTPUT_REG 0xD56

//  
//  Sets the MSB of the ANC FIR coefficients, the 16 bits coefficient is updated when the 
//  LSB part is written. See ANCFIRCoeffLSB[7:0] for details.
//  Register AncFirCoeffMsb 0x0D57, Bits 0:7, typedef ancfir_coeff_m_s_b
//   
//  
#define ANC_FIR_COEFF_MSB_MASK 0xFFU
#define ANC_FIR_COEFF_MSB_OFFSET 7
#define ANC_FIR_COEFF_MSB_STOP_BIT 7
#define ANC_FIR_COEFF_MSB_START_BIT 0
#define ANC_FIR_COEFF_MSB_WIDTH 8

#define ANC_FIR_COEFF_MSB_REG 0xD57


#define ANC_FIR_COEFF_LSB_REG 0xD58
#define ANC_IIR_COEFF_MSB_REG 0xD59
#define ANC_IIR_COEFF_LSB_REG 0xD5A
#define ANC_WARP_DELAY_MSB_REG 0xD5B
#define ANC_WARP_DELAY_LSB_REG 0xD5C
#define ANC_FIR_PEAK_MSB_REG 0xD5D
#define ANC_FIR_PEAK_LSB_REG 0xD5E
#define ANC_IIR_PEAK_MSB_REG 0xD5F
#define ANC_IIR_PEAK_LSB_REG 0xD60

//  
//  Set the 7 bits address of FIR coefficient that will be written.
//  Register SideToneControl 0x0D61, Bits 0:6, typedef address_coeffocient
//   
//  
#define ADDRESS_COEFFICIENT_MASK 0x7FU
#define ADDRESS_COEFFICIENT_OFFSET 6
#define ADDRESS_COEFFICIENT_STOP_BIT 6
#define ADDRESS_COEFFICIENT_START_BIT 0
#define ADDRESS_COEFFICIENT_WIDTH 7

#define SIDE_TONE_CONTROL_REG 0xD61



//  
//  0: Last FIR coefficients not applied; 
//  1: Applies the last programmed set of FIR coefficients..
//  Register SideToneControl 0x0D61, Bits 7:7, typedef side_tone_control
//   
//  
#define SIDE_TONE_CONTROL_MASK 0x80U
#define SIDE_TONE_CONTROL_OFFSET 7
#define SIDE_TONE_CONTROL_STOP_BIT 7
#define SIDE_TONE_CONTROL_START_BIT 7
#define SIDE_TONE_CONTROL_WIDTH 1


typedef enum side_tone_control {
    FIR_COFF_NOT_APPLIED_E,
    FIR_COFF_APPLIED_E
} SIDE_TONE_CONTROL_T ;
#define SIDE_TONE_CONTROL_PARAM_MASK  0x80


#define SIDE_TONE_COFF_MSB_REG 0xD62
#define SIDE_TONE_COFF_LSB_REG 0xD63

//  
//  0: The ST FIR is ready for new coefficients. 
//  1: The new ST FIR coefficients are not applied yet..
//  Register FirFiltersControl 0x0D64, Bits 0:0, typedef fir_busy
//   
//  
#define FIR_BUSY_MASK 0x1U
#define FIR_BUSY_OFFSET 0
#define FIR_BUSY_STOP_BIT 0
#define FIR_BUSY_START_BIT 0
#define FIR_BUSY_WIDTH 1

#define FIR_FILTERS_CONTROL_REG 0xD64

typedef enum fir_busy {
    FIR_READY_E,
    FIR_BUSY_E
} FIR_BUSY_T ;
#define FIR_BUSY_PARAM_MASK  0x1



//  
//  0: FIR1 and FIR2 Programmable filters run at AUD_IF0 data rate. 
//  1: FIR1 and FIR2 Programmable filters run at AUD_IF1 data rate..
//  Register FirFiltersControl 0x0D64, Bits 1:1, typedef fir_data_rate
//   
//  
#define FIR_DATA_RATE_MASK 0x2U
#define FIR_DATA_RATE_OFFSET 1
#define FIR_DATA_RATE_STOP_BIT 1
#define FIR_DATA_RATE_START_BIT 1
#define FIR_DATA_RATE_WIDTH 1


typedef enum fir_data_rate {
    AUD_IF0_DATA_RATE_E,
    AUD_IF1_DATA_RATE_E
} FIR_DATA_RATE_T ;
#define FIR_DATA_RATE_PARAM_MASK  0x2



//  
//  0: ST FIR Filters are disabled. 
//  1: ST FIR Filters are enabled..
//  Register FirFiltersControl 0x0D64, Bits 2:2, typedef fir_filters_enable
//   
//  
#define FIR_FILTERS_MASK 0x4U
#define FIR_FILTERS_OFFSET 2
#define FIR_FILTERS_STOP_BIT 2
#define FIR_FILTERS_START_BIT 2
#define FIR_FILTERS_WIDTH 1


typedef enum fir_filters_enable {
    FIR_FILTERS_DISABLE_E,
    FIR_FILTERS_ENABLE_E
} FIR_FILTERS_ENABLE_T ;
#define FIR_FILTERS_PARAM_MASK  0x4



//  
//  0: Bit FIR_EV cannot generate an Interrupt Request; 
//  1: Bit FIR_EV can generate an Interrupt Request.
//  Register InterruptEnable0 0x0D65, Bits 0:0, typedef fir_enable
//   
//  
#define INTERRUPT_FIR_MASK 0x1U
#define INTERRUPT_FIR_OFFSET 0
#define INTERRUPT_FIR_STOP_BIT 0
#define INTERRUPT_FIR_START_BIT 0
#define INTERRUPT_FIR_WIDTH 1

#define INTERRUPT_ENABLE_0_REG 0xD65

typedef enum fir_enable {
    FIR_DISABLE_E,
    FIR_ENABLE_E
} FIR_ENABLE_T ;
#define INTERRUPT_FIR_PARAM_MASK  0x1



//  
//  0: Bit DSPDA_EV cannot generate an Interrupt Request; 
//  1: Bit DSPDA_EV can generate an Interrupt Request.
//  Register InterruptEnable0 0x0D65, Bits 1:1, typedef dsp_da_enable
//   
//  
#define INTERRUPT_DSP_DZ_MASK 0x2U
#define INTERRUPT_DSP_DZ_OFFSET 1
#define INTERRUPT_DSP_DZ_STOP_BIT 1
#define INTERRUPT_DSP_DZ_START_BIT 1
#define INTERRUPT_DSP_DZ_WIDTH 1




//  
//  0: Bit DSPAD_EV cannot generate an Interrupt Request; 
//  1: Bit DSPAD_EV can generate an Interrupt Request.
//  Register InterruptEnable0 0x0D65, Bits 2:2, typedef dsp_ad_enable
//   
//  
#define INTERRUPT_DSP_AF_MASK 0x4U
#define INTERRUPT_DSP_AF_OFFSET 2
#define INTERRUPT_DSP_AF_STOP_BIT 2
#define INTERRUPT_DSP_AF_START_BIT 2
#define INTERRUPT_DSP_AF_WIDTH 1




//  
//  0: Bit GAIN_EV cannot generate an Interrupt Request; 
//  1: Bit GAIN_EV can generate an Interrupt Request.
//  Register InterruptEnable0 0x0D65, Bits 3:3, typedef gain
//   
//  
#define INTERRUPT_GAIN_MASK 0x8U
#define INTERRUPT_GAIN_OFFSET 3
#define INTERRUPT_GAIN_STOP_BIT 3
#define INTERRUPT_GAIN_START_BIT 3
#define INTERRUPT_GAIN_WIDTH 1




//  
//  0: Bit DACHAN_EV cannot generate an Interrupt Request; 
//  1: Bit DACHAN_EV can generate an Interrupt Request.
//  Register InterruptEnable0 0x0D65, Bits 4:4, typedef da_chan
//   
//  
#define INTERRUPT_DA_CHAN_MASK 0x10U
#define INTERRUPT_DA_CHAN_OFFSET 4
#define INTERRUPT_DA_CHAN_STOP_BIT 4
#define INTERRUPT_DA_CHAN_START_BIT 4
#define INTERRUPT_DA_CHAN_WIDTH 1




//  
//  0: Bit BFIEMPT_EV cannot generate an Interrupt Request; 
//  1: Bit BFIEMPT_EV can generate an Interrupt Request.
//  Register InterruptEnable0 0x0D65, Bits 5:5, typedef buffer_empty
//   
//  
#define INTERRUPT_BUFFER_EMPTY_MASK 0x20U
#define INTERRUPT_BUFFER_EMPTY_OFFSET 5
#define INTERRUPT_BUFFER_EMPTY_STOP_BIT 5
#define INTERRUPT_BUFFER_EMPTY_START_BIT 5
#define INTERRUPT_BUFFER_EMPTY_WIDTH 1




//  
//  0: Bit BFIFULL_EV cannot generate an Interrupt Request; 
//  1: Bit BFIFULL_EV can generate an Interrupt Request.
//  Register InterruptEnable0 0x0D65, Bits 6:6, typedef buffer_full
//   
//  
#define INTERRUPT_BUFFER_FULL_MASK 0x40U
#define INTERRUPT_BUFFER_FULL_OFFSET 6
#define INTERRUPT_BUFFER_FULL_STOP_BIT 6
#define INTERRUPT_BUFFER_FULL_START_BIT 6
#define INTERRUPT_BUFFER_FULL_WIDTH 1




//  
//  0: Bit IT_HsOffSt cannot generate an Interrupt Request. 
//  1:Bit IT_HsOffSt can generate an Interrupt Request.
//  Register InterruptEnable0 0x0D65, Bits 7:7, typedef hs_offset_status
//   
//  
#define INTERRUPT_HS_OFFSET_STATUS_MASK 0x80U
#define INTERRUPT_HS_OFFSET_STATUS_OFFSET 7
#define INTERRUPT_HS_OFFSET_STATUS_STOP_BIT 7
#define INTERRUPT_HS_OFFSET_STATUS_START_BIT 7
#define INTERRUPT_HS_OFFSET_STATUS_WIDTH 1




//  
//  0: Bit FIR_EV cannot generate an Interrupt Request; 
//  1: Bit FIR_EV can generate an Interrupt Request.
//  Register InterruptStatus0 0x0D66, Bits 0:0, typedef status_fir
//   
//  
#define STATUS_FIR_MASK 0x1U
#define STATUS_FIR_OFFSET 0
#define STATUS_FIR_STOP_BIT 0
#define STATUS_FIR_START_BIT 0
#define STATUS_FIR_WIDTH 1

#define INTERRUPT_STATUS_0_REG 0xD66



//  
//  0: Bit DSPDA_EV cannot generate an Interrupt Request; 
//  1: Bit DSPDA_EV can generate an Interrupt Request.
//  Register InterruptStatus0 0x0D66, Bits 1:1, typedef status_dsp_da
//   
//  
#define STATUS_DSP_DA_MASK 0x2U
#define STATUS_DSP_DA_OFFSET 1
#define STATUS_DSP_DA_STOP_BIT 1
#define STATUS_DSP_DA_START_BIT 1
#define STATUS_DSP_DA_WIDTH 1




//  
//  0: No saturation occurred; 
//  1:A saturation has occurred in AD DSP.
//  Register InterruptStatus0 0x0D66, Bits 2:2, typedef status_dsp_ad
//   
//  
#define STATUS_DSP_AD_MASK 0x4U
#define STATUS_DSP_AD_OFFSET 2
#define STATUS_DSP_AD_STOP_BIT 2
#define STATUS_DSP_AD_START_BIT 2
#define STATUS_DSP_AD_WIDTH 1




//  
//  0: No saturation occurred; 
//  1:A saturation has occurred in AD gain.
//  Register InterruptStatus0 0x0D66, Bits 3:3, typedef status_gain
//   
//  
#define STATUS_GAIN_MASK 0x8U
#define STATUS_GAIN_OFFSET 3
#define STATUS_GAIN_STOP_BIT 3
#define STATUS_GAIN_START_BIT 3
#define STATUS_GAIN_WIDTH 1




//  
//  0: No saturation occurred; 
//  1:A saturation has occurred in DA gain.
//  Register InterruptStatus0 0x0D66, Bits 4:4, typedef status_da_chan
//   
//  
#define STATUS_DA_CHAN_MASK 0x10U
#define STATUS_DA_CHAN_OFFSET 4
#define STATUS_DA_CHAN_STOP_BIT 4
#define STATUS_DA_CHAN_START_BIT 4
#define STATUS_DA_CHAN_WIDTH 1




//  
//  0: Burst FIFO is not empty.1: Burst FIFO event has occured..
//  Register InterruptStatus0 0x0D66, Bits 5:5, typedef status_buffer_empty
//   
//  
#define STATUS_BUFFER_EMPTY_MASK 0x20U
#define STATUS_BUFFER_EMPTY_OFFSET 5
#define STATUS_BUFFER_EMPTY_STOP_BIT 5
#define STATUS_BUFFER_EMPTY_START_BIT 5
#define STATUS_BUFFER_EMPTY_WIDTH 1




//  
//  0: Burst FIFO is not full. 
//  1: Burst FIFO full event has occuredt.
//  Register InterruptStatus0 0x0D66, Bits 6:6, typedef status_buffer_full
//   
//  
#define STATUS_BUFFER_FULL_MASK 0x40U
#define STATUS_BUFFER_FULL_OFFSET 6
#define STATUS_BUFFER_FULL_STOP_BIT 6
#define STATUS_BUFFER_FULL_START_BIT 6
#define STATUS_BUFFER_FULL_WIDTH 1




//  
//  0: Bit IT_HsOffSt cannot generate an Interrupt Request. 
//  1:Bit IT_HsOffSt can generate an Interrupt Request.
//  Register InterruptStatus0 0x0D66, Bits 7:7, typedef hs_offset_status
//   
//  
#define INTERRUPT_HS_OFFSET_STATUS_MASK 0x80U
#define INTERRUPT_HS_OFFSET_STATUS_OFFSET 7
#define INTERRUPT_HS_OFFSET_STATUS_STOP_BIT 7
#define INTERRUPT_HS_OFFSET_STATUS_START_BIT 7
#define INTERRUPT_HS_OFFSET_STATUS_WIDTH 1




//  
//  0: Bit SHRTEAR_EV cannot generate an Interrupt Request; 
//  1: Bit SHRTEAR_EV can generate an Interrupt Request.
//  Register InterruptEnable1 0x0D67, Bits 0:0, typedef 
//  interrupt_short_circuit_detected_ear_enable
//   
//  
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_MASK 0x1U
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_OFFSET 0
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_STOP_BIT 0
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_START_BIT 0
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_WIDTH 1

#define INTERRUPT_ENABLE_1_REG 0xD67

typedef enum interrupt_short_circuit_detected_ear_enable {
    INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_DISABLE_E,
    INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_ENABLE_E
} INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_ENABLE_T ;
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_PARAM_MASK  0x1



//  
//  0: Bit SHRTHSR_EV cannot generate an Interrupt Request; 
//  1: Bit SHRTHSR_EV can generate an Interrupt Request.
//  Register InterruptEnable1 0x0D67, Bits 1:1, typedef 
//  interrupt_short_circuit_hsr_enable
//   
//  
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSR_MASK 0x2U
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSR_OFFSET 1
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSR_STOP_BIT 1
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSR_START_BIT 1
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSR_WIDTH 1


typedef enum interrupt_short_circuit_hsr_enable {
    INTERRUPT_SHORT_CIRCUIT_HSR_DISABLE_E,
    INTERRUPT_SHORT_CIRCUIT_HSR_ENABLE_E
} INTERRUPT_SHORT_CIRCUIT_HSR_ENABLE_T ;
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSR_PARAM_MASK  0x2



//  
//  0: Bit SHRTHSL_EV cannot generate an Interrupt Request; 
//  1: Bit SHRTHSL_EV can generate an Interrupt Request.
//  Register InterruptEnable1 0x0D67, Bits 2:2, typedef 
//  interrupt_short_circuit_hsl_enable
//   
//  
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSL_MASK 0x4U
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSL_OFFSET 2
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSL_STOP_BIT 2
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSL_START_BIT 2
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSL_WIDTH 1


typedef enum interrupt_short_circuit_hsl_enable {
    INTERRUPT_SHORT_CIRCUIT_HSL_DISABLE_E,
    INTERRUPT_SHORT_CIRCUIT_HSL_ENABLE_E
} INTERRUPT_SHORT_CIRCUIT_HSL_ENABLE_T ;
#define INTERRUPT_SHORT_CIRCUIT_DETECTED_HSL_PARAM_MASK  0x4



//  
//  0: Bit BFIFULL_EV cannot generate an Interrupt Request; 
//  1: Bit BFIFULL_EV can generate an Interrupt Request.
//  Register InterruptEnable1 0x0D67, Bits 7:7, typedef interrupt_vss_ready_enable
//   
//  
#define INTERRUPT_VSS_READY_MASK 0x80U
#define INTERRUPT_VSS_READY_OFFSET 7
#define INTERRUPT_VSS_READY_STOP_BIT 7
#define INTERRUPT_VSS_READY_START_BIT 7
#define INTERRUPT_VSS_READY_WIDTH 1


typedef enum interrupt_vss_ready_enable {
    INTERRUPT_VSS_READY_DISABLE_E,
    INTERRUPT_VSS_READY_ENABLE_E
} INTERRUPT_VSS_READY_ENABLE_T ;
#define INTERRUPT_VSS_READY_PARAM_MASK  0x80



//  
//  0: No short circuit detected, 
//  1: Short circuit detected on Ear Output driver..
//  Register InterruptStatus1 0x0D68, Bits 0:0, typedef 
//  status_short_circuit_detected_ear
//   
//  
#define STATUS_SHORT_CIRCUIT_DETECTED_EAR_MASK 0x1U
#define STATUS_SHORT_CIRCUIT_DETECTED_EAR_OFFSET 0
#define STATUS_SHORT_CIRCUIT_DETECTED_EAR_STOP_BIT 0
#define STATUS_SHORT_CIRCUIT_DETECTED_EAR_START_BIT 0
#define STATUS_SHORT_CIRCUIT_DETECTED_EAR_WIDTH 1

#define INTERRUPT_STATUS_1_REG 0xD68



//  
//  0: No short circuit detected, 
//  1: Short circuit detected on HSR Output driver..
//  Register InterruptStatus1 0x0D68, Bits 1:1, typedef 
//  status_short_circuit_detected_hsr
//   
//  
#define STATUS_SHORT_CIRCUIT_DETECTED_HSR_MASK 0x2U
#define STATUS_SHORT_CIRCUIT_DETECTED_HSR_OFFSET 1
#define STATUS_SHORT_CIRCUIT_DETECTED_HSR_STOP_BIT 1
#define STATUS_SHORT_CIRCUIT_DETECTED_HSR_START_BIT 1
#define STATUS_SHORT_CIRCUIT_DETECTED_HSR_WIDTH 1




//  
//  0: No short circuit detected, 
//  1: Short circuit detected on HSL Output driver..
//  Register InterruptStatus1 0x0D68, Bits 2:2, typedef 
//  status_short_circuit_detected_hsl
//   
//  
#define STATUS_SHORT_CIRCUIT_DETECTED_HSL_MASK 0x4U
#define STATUS_SHORT_CIRCUIT_DETECTED_HSL_OFFSET 2
#define STATUS_SHORT_CIRCUIT_DETECTED_HSL_STOP_BIT 2
#define STATUS_SHORT_CIRCUIT_DETECTED_HSL_START_BIT 2
#define STATUS_SHORT_CIRCUIT_DETECTED_HSL_WIDTH 1




//  
//  0: VSS from charge pump is not ready. 
//  1: VSS from charge pump is ready..
//  Register InterruptStatus1 0x0D68, Bits 7:7, typedef status_vss_ready
//   
//  
#define STATUS_VSS_READY_MASK 0x80U
#define STATUS_VSS_READY_OFFSET 7
#define STATUS_VSS_READY_STOP_BIT 7
#define STATUS_VSS_READY_START_BIT 7
#define STATUS_VSS_READY_WIDTH 1




//  
//  Wakeup signal is generated when burst.
//  Register FifoInterruptControl 0x0D69, Bits 0:5, typedef wakeup_signal_level
//   
//  
#define WAKEUP_SIGNAL_LEVEL_MASK 0x3FU
#define WAKEUP_SIGNAL_LEVEL_OFFSET 5
#define WAKEUP_SIGNAL_LEVEL_STOP_BIT 5
#define WAKEUP_SIGNAL_LEVEL_START_BIT 0
#define WAKEUP_SIGNAL_LEVEL_WIDTH 6

#define FIFO_INTERRUPT_CONTROL_REG 0xD69
#define  NO_WAKEUP_SIGNAL 0
#define  FIFO_8_SAMPLES 1
#define  FIFO_16_SAMPLES 2
#define  FIFO_32_SAMPLES 4
#define  FIFO_64_SAMPLES 8
#define  FIFO_128_SAMPLES 16
#define  FIFO_256_SAMPLES 32
#define  FIFO_508_SAMPLES 64

typedef enum wakeup_signal_level {
    NO_WAKEUP_SIGNAL_E,
    FIFO_8_SAMPLES_E,
    FIFO_16_SAMPLES_E,
    FIFO_32_SAMPLES_E,
    FIFO_64_SAMPLES_E,
    FIFO_128_SAMPLES_E,
    FIFO_256_SAMPLES_E,
    FIFO_508_SAMPLES_E
} WAKEUP_SIGNAL_LEVEL_T ;
#define WAKEUP_SIGNAL_LEVEL_PARAM_MASK  0x3F



//  
//  0: BitClk0 frequency in burst mode is 38.4 MHz 
//  1: BitClk0 frequency in burst mode is 19.2 MHz.
//  Register FifoInterruptControl 0x0D69, Bits 6:6, typedef burst_frequency
//   
//  
#define BURST_BIT_CLOCK_MASK 0x40U
#define BURST_BIT_CLOCK_OFFSET 6
#define BURST_BIT_CLOCK_STOP_BIT 6
#define BURST_BIT_CLOCK_START_BIT 6
#define BURST_BIT_CLOCK_WIDTH 1


typedef enum burst_frequency {
    BUSRT_FREQUENCY_38_4_MHZ_E,
    BUSRT_FREQUENCY_19_2_MHZ_E
} BURST_FREQUENCY_T ;
#define BURST_BIT_CLOCK_PARAM_MASK  0x40



//  
//  0: AD_DATA0 is unmasked in burst mode, it can wake up the digital audio data source 
//  1: AD_DATA0 is masked in burst mode, it cannot wake up the digital audio data 
//  source.
//  Register FifoInterruptControl 0x0D69, Bits 7:7, typedef fifo_mask_enable
//   
//  
#define FIFO_MASK_MASK 0x80U
#define FIFO_MASK_OFFSET 7
#define FIFO_MASK_STOP_BIT 7
#define FIFO_MASK_START_BIT 7
#define FIFO_MASK_WIDTH 1


typedef enum fifo_mask_enable {
    FIFO_MASK_DISABLE_E,
    FIFO_MASK_ENABLE_E
} FIFO_MASK_ENABLE_T ;
#define FIFO_MASK_PARAM_MASK  0x80



//  
//  Burst FIFO length register. It define the burst tranfer length in master 
//  mode..
//  Register FifoLength 0x0D6A, Bits 0:7, typedef fifo_burst_length
//   
//  
#define FIFO_BURST_LENGTH_MASK 0xFFU
#define FIFO_BURST_LENGTH_OFFSET 7
#define FIFO_BURST_LENGTH_STOP_BIT 7
#define FIFO_BURST_LENGTH_START_BIT 0
#define FIFO_BURST_LENGTH_WIDTH 8

#define FIFO_LENGTH_REG 0xD6A
#define  FIFO_BURST_LEN_NO_DATA 0
#define  FIFO_BURST_LEN_8_SAMPLES 1
#define  FIFO_BURST_LEN_16_SAMPLES 2
#define  FIFO_BURST_LEN_32_SAMPLES 4
#define  FIFO_BURST_LEN_64_SAMPLES 8
#define  FIFO_BURST_LEN_128_SAMPLES 16
#define  FIFO_BURST_LEN_256_SAMPLES 32
#define  FIFO_BURST_LEN_512_SAMPLES 64
#define  FIFO_BURST_LEN_1024_SAMPLES 128
#define  FIFO_BURST_LEN_1536_SAMPLES 192
#define  FIFO_BURST_LEN_2044_SAMPLES 255

typedef enum fifo_burst_length {
    FIFO_BURST_LEN_NO_DATA_E,
    FIFO_BURST_LEN_8_SAMPLES_E,
    FIFO_BURST_LEN_16_SAMPLES_E,
    FIFO_BURST_LEN_32_SAMPLES_E,
    FIFO_BURST_LEN_64_SAMPLES_E,
    FIFO_BURST_LEN_128_SAMPLES_E,
    FIFO_BURST_LEN_256_SAMPLES_E,
    FIFO_BURST_LEN_512_SAMPLES_E,
    FIFO_BURST_LEN_1024_SAMPLES_E,
    FIFO_BURST_LEN_1536_SAMPLES_E,
    FIFO_BURST_LEN_2044_SAMPLES_E
} FIFO_BURST_LENGTH_T ;
#define FIFO_BURST_LENGTH_PARAM_MASK  0xFF



//  
//  0: Burst FIFO interface is stopped 
//  1: Burst FIFO interface is running.
//  Register FifoControl 0x0D6B, Bits 0:0, typedef bust_mode_run
//   
//  
#define BUST_MODE_RUN_MASK 0x1U
#define BUST_MODE_RUN_OFFSET 0
#define BUST_MODE_RUN_STOP_BIT 0
#define BUST_MODE_RUN_START_BIT 0
#define BUST_MODE_RUN_WIDTH 1

#define FIFO_CONTROL_REG 0xD6B

typedef enum bust_mode_run {
    BURST_MODE_STOPPED_E,
    BURST_MODE_RUNNING_E
} BUST_MODE_RUN_T ;
#define BUST_MODE_RUN_PARAM_MASK  0x1



//  
//  0: Burst FIFO interface is configured in slave mode 
//  1: Burst FIFO interface is configured in master mode.
//  Register FifoControl 0x0D6B, Bits 1:1, typedef master_fifo_mode_enable
//   
//  
#define MASTER_FIFO_MODE_MASK 0x2U
#define MASTER_FIFO_MODE_OFFSET 1
#define MASTER_FIFO_MODE_STOP_BIT 1
#define MASTER_FIFO_MODE_START_BIT 1
#define MASTER_FIFO_MODE_WIDTH 1


typedef enum master_fifo_mode_enable {
    MASTER_FIFO_MODE_DISABLE_E,
    MASTER_FIFO_MODE_ENABLE_E
} MASTER_FIFO_MODE_ENABLE_T ;
#define MASTER_FIFO_MODE_PARAM_MASK  0x2



//  
//  Number of extra bit clocks that precede and follow the first Frame Sync on Aud_IF0 
//  startup or Burst startup.
//  Register FifoControl 0x0D6B, Bits 2:4, typedef number_extra_clocks
//   
//  
#define NUMBER_EXTRA_CLOCKS_MASK 0x1CU
#define NUMBER_EXTRA_CLOCKS_OFFSET 4
#define NUMBER_EXTRA_CLOCKS_STOP_BIT 4
#define NUMBER_EXTRA_CLOCKS_START_BIT 2
#define NUMBER_EXTRA_CLOCKS_WIDTH 3




//  
//  Number of extra clock in burst mode at the end of each frame.
//  Register FifoControl 0x0D6B, Bits 5:7, typedef fifo_extra_clock
//   
//  
#define FIFO_EXTRA_CLOCK_MASK 0xE0U
#define FIFO_EXTRA_CLOCK_OFFSET 7
#define FIFO_EXTRA_CLOCK_STOP_BIT 7
#define FIFO_EXTRA_CLOCK_START_BIT 5
#define FIFO_EXTRA_CLOCK_WIDTH 3

#define  NO_EXTRA_CLOCK 0
#define  MODE_1_SLOT_EXTRA 1
#define  MODE_2_SLOT_EXTRA 2
#define  MODE_3_SLOT_EXTRA 3
#define  MODE_4_SLOT_EXTRA 4
#define  MODE_5_SLOT_EXTRA 5
#define  MODE_6_SLOT_EXTRA 6

typedef enum fifo_extra_clock {
    NO_EXTRA_CLOCK_E,
    MODE_1_SLOT_EXTRA_E,
    MODE_2_SLOT_EXTRA_E,
    MODE_3_SLOT_EXTRA_E,
    MODE_4_SLOT_EXTRA_E,
    MODE_5_SLOT_EXTRA_E,
    MODE_6_SLOT_EXTRA_E
} FIFO_EXTRA_CLOCK_T ;
#define FIFO_EXTRA_CLOCK_PARAM_MASK  0xE0


#define FIFO_SWITCH_FRAME_REG 0xD6C
#define FIFO_WAKE_UP_DELAY_REG 0xD6D
#define FIFO_SAMPLES_NUMBER_REG 0xD6E

//  
//  Audio module revision 01001: AB9540 cut 1.0.
//  Register Revision 0x0D6F, Bits 0:4, typedef ab9540_revison
//   
//  
#define AB_9540_REVISON_MASK 0x1FU
#define AB_9540_REVISON_OFFSET 4
#define AB_9540_REVISON_STOP_BIT 4
#define AB_9540_REVISON_START_BIT 0
#define AB_9540_REVISON_WIDTH 5

#define REVISION_REG 0xD6F


#endif
