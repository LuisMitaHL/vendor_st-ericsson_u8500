/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_Audio/Linux/ab9540_Audio.c
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

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_AUDIO_LINUX_AB9540_AUDIO_C
#define C__LISP_PRODUCTION_AB9540_AB9540_AUDIO_LINUX_AB9540_AUDIO_C


#ifdef __cplusplus
extern "C" {
#endif
#include "ab9540_Audio.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_analog_power_up
 *
 * IN   : param, a value to write to the regiter PowerUpControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xPowerUpControl
 *
 * Notes : From register 0xD00, bits 3:3
 *
 **************************************************************************/
unsigned char set_analog_power_up( enum analog_power_up param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(POWER_UP_CONTROL_REG);

    /* 
     * 0: All the audio analog parts are in power down 
     * 1: All the audio analog parts are in power up 
     */ 
    switch( param ){
        case ANALOG_POWER_UP_E: 
           value = old_value | ANALOG_POWER_UP_MASK; 
           break;
        case ANALOG_POWER_DOWN_E: 
           value = old_value & ~ ANALOG_POWER_UP_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_power_up
 *
 * RET  : Return the value of register 0xPowerUpControl
 *
 * Notes : From register 0xD00, bits 3:3
 *
 **************************************************************************/
unsigned char get_analog_power_up()
  {
    unsigned char value;


    /* 
     * 0: All the audio analog parts are in power down 
     * 1: All the audio analog parts are in power up 
     */ 
    value = (I2CRead(POWER_UP_CONTROL_REG) & ANALOG_POWER_UP_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_power_up
 *
 * IN   : param, a value to write to the regiter PowerUpControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xPowerUpControl
 *
 * Notes : From register 0xD00, bits 7:7
 *
 **************************************************************************/
unsigned char set_digital_power_up( enum digital_power_up param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(POWER_UP_CONTROL_REG);

    /* 
     * 0: The audio feature is in power down 
     * 1: The audio feature is in power up 
     */ 
    switch( param ){
        case DIGITAL_POWER_UP_E: 
           value = old_value | DIGITAL_POWER_UP_MASK; 
           break;
        case DIGITAL_POWER_DOWN_E: 
           value = old_value & ~ DIGITAL_POWER_UP_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_power_up
 *
 * RET  : Return the value of register 0xPowerUpControl
 *
 * Notes : From register 0xD00, bits 7:7
 *
 **************************************************************************/
unsigned char get_digital_power_up()
  {
    unsigned char value;


    /* 
     * 0: The audio feature is in power down 
     * 1: The audio feature is in power up 
     */ 
    value = (I2CRead(POWER_UP_CONTROL_REG) & DIGITAL_POWER_UP_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_audio_software_reset
 *
 * IN   : param, a value to write to the regiter SoftwareReset
 * OUT  : 
 *
 * RET  : Return the value of register 0xSoftwareReset
 *
 * Notes : From register 0xD01, bits 7:7
 *
 **************************************************************************/
unsigned char set_audio_software_reset( enum audio_software_reset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SOFTWARE_RESET_REG);

    /* 
     * 0: Normal operation 
     * 1: Sets all the audio registers to their default state, 
     * including itself 
     */ 
    switch( param ){
        case AUDIO_SOFTWARE_RESET_E: 
           value = old_value | AUDIO_SOFTWARE_RESET_MASK; 
           break;
        case AUDIO_SOFTWARE_UN_RESET_E: 
           value = old_value & ~ AUDIO_SOFTWARE_RESET_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_RESET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_audio_software_reset
 *
 * RET  : Return the value of register 0xSoftwareReset
 *
 * Notes : From register 0xD01, bits 7:7
 *
 **************************************************************************/
unsigned char get_audio_software_reset()
  {
    unsigned char value;


    /* 
     * 0: Normal operation 
     * 1: Sets all the audio registers to their default state, 
     * including itself 
     */ 
    value = (I2CRead(SOFTWARE_RESET_REG) & AUDIO_SOFTWARE_RESET_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad5768_digital_audio_paths
 *
 * IN   : param, a value to write to the regiter DigitalChannelsEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalChannelsEnable
 *
 * Notes : From register 0xD02, bits 3:3
 *
 **************************************************************************/
unsigned char set_ad5768_digital_audio_paths( enum ad5678_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_CHANNELS_ENABLE_REG);

    /* 
     * 0: AD5/7 and AD6/8 paths are disabled 
     * 1: AD5/7 and AD6/8 paths are enabled 
     */ 
    switch( param ){
        case AD_5678_ENABLE_E: 
           value = old_value | AD_5768_DIGITAL_AUDIO_PATHS_MASK; 
           break;
        case AD_5678_DISABLE_E: 
           value = old_value & ~ AD_5768_DIGITAL_AUDIO_PATHS_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_CHANNELS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad5768_digital_audio_paths
 *
 * RET  : Return the value of register 0xDigitalChannelsEnable
 *
 * Notes : From register 0xD02, bits 3:3
 *
 **************************************************************************/
unsigned char get_ad5768_digital_audio_paths()
  {
    unsigned char value;


    /* 
     * 0: AD5/7 and AD6/8 paths are disabled 
     * 1: AD5/7 and AD6/8 paths are enabled 
     */ 
    value = (I2CRead(DIGITAL_CHANNELS_ENABLE_REG) & AD_5768_DIGITAL_AUDIO_PATHS_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad34_digital_audio_paths
 *
 * IN   : param, a value to write to the regiter DigitalChannelsEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalChannelsEnable
 *
 * Notes : From register 0xD02, bits 5:5
 *
 **************************************************************************/
unsigned char set_ad34_digital_audio_paths( enum ad34_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_CHANNELS_ENABLE_REG);

    /* 
     * 0: AD3 and AD4 paths are disabled 
     * 1: AD3 and AD4 paths are enabled 
     */ 
    switch( param ){
        case AD_34_ENABLE_E: 
           value = old_value | AD_34_DIGITAL_AUDIO_PATHS_MASK; 
           break;
        case AD_34_DISABLE_E: 
           value = old_value & ~ AD_34_DIGITAL_AUDIO_PATHS_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_CHANNELS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad34_digital_audio_paths
 *
 * RET  : Return the value of register 0xDigitalChannelsEnable
 *
 * Notes : From register 0xD02, bits 5:5
 *
 **************************************************************************/
unsigned char get_ad34_digital_audio_paths()
  {
    unsigned char value;


    /* 
     * 0: AD3 and AD4 paths are disabled 
     * 1: AD3 and AD4 paths are enabled 
     */ 
    value = (I2CRead(DIGITAL_CHANNELS_ENABLE_REG) & AD_34_DIGITAL_AUDIO_PATHS_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad12_digital_audio_paths
 *
 * IN   : param, a value to write to the regiter DigitalChannelsEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalChannelsEnable
 *
 * Notes : From register 0xD02, bits 7:7
 *
 **************************************************************************/
unsigned char set_ad12_digital_audio_paths( enum ad12_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_CHANNELS_ENABLE_REG);

    /* 
     * 0: AD1 and AD2 paths are disabled 
     * 1: AD1 and AD2 paths are enabled 
     */ 
    switch( param ){
        case AD_12_ENABLE_E: 
           value = old_value | AD_12_DIGITAL_AUDIO_PATHS_MASK; 
           break;
        case AD_12_DISABLE_E: 
           value = old_value & ~ AD_12_DIGITAL_AUDIO_PATHS_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_CHANNELS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad12_digital_audio_paths
 *
 * RET  : Return the value of register 0xDigitalChannelsEnable
 *
 * Notes : From register 0xD02, bits 7:7
 *
 **************************************************************************/
unsigned char get_ad12_digital_audio_paths()
  {
    unsigned char value;


    /* 
     * 0: AD1 and AD2 paths are disabled 
     * 1: AD1 and AD2 paths are enabled 
     */ 
    value = (I2CRead(DIGITAL_CHANNELS_ENABLE_REG) & AD_12_DIGITAL_AUDIO_PATHS_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_high_pass_filter
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 1:1
 *
 **************************************************************************/
unsigned char set_high_pass_filter( enum high_pass_filter_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_DA_AUDIO_PATHS_REG);

    /* 
     * 0: High pass filter for Earpiece is disabled 
     * 1: High pass filter for Earpiece is enabled 
     */ 
    switch( param ){
        case HIGH_PASS_FILTER_ENABLE_E: 
           value = old_value | HIGH_PASS_FILTER_MASK; 
           break;
        case HIGH_PASS_FILTER_DISABLE_E: 
           value = old_value & ~ HIGH_PASS_FILTER_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_high_pass_filter
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 1:1
 *
 **************************************************************************/
unsigned char get_high_pass_filter()
  {
    unsigned char value;


    /* 
     * 0: High pass filter for Earpiece is disabled 
     * 1: High pass filter for Earpiece is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & HIGH_PASS_FILTER_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da6_digital_audio_paths
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 2:2
 *
 **************************************************************************/
unsigned char set_da6_digital_audio_paths( enum da6_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_DA_AUDIO_PATHS_REG);

    /* 
     * 0: DA6 path is disabled 
     * 1: DA6 path is enabled 
     */ 
    switch( param ){
        case DA_6_ENABLE_E: 
           value = old_value | DA_6_DIGITAL_AUDIO_PATHS_MASK; 
           break;
        case DA_6_DISABLE_E: 
           value = old_value & ~ DA_6_DIGITAL_AUDIO_PATHS_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da6_digital_audio_paths
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 2:2
 *
 **************************************************************************/
unsigned char get_da6_digital_audio_paths()
  {
    unsigned char value;


    /* 
     * 0: DA6 path is disabled 
     * 1: DA6 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_6_DIGITAL_AUDIO_PATHS_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da5_digital_audio_paths
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 3:3
 *
 **************************************************************************/
unsigned char set_da5_digital_audio_paths( enum da5_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_DA_AUDIO_PATHS_REG);

    /* 
     * 0: DA5 path is disabled 
     * 1: DA5 path is enabled 
     */ 
    switch( param ){
        case DA_5_ENABLE_E: 
           value = old_value | DA_5_DIGITAL_AUDIO_PATHS_MASK; 
           break;
        case DA_5_DISABLE_E: 
           value = old_value & ~ DA_5_DIGITAL_AUDIO_PATHS_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da5_digital_audio_paths
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 3:3
 *
 **************************************************************************/
unsigned char get_da5_digital_audio_paths()
  {
    unsigned char value;


    /* 
     * 0: DA5 path is disabled 
     * 1: DA5 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_5_DIGITAL_AUDIO_PATHS_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da4_digital_audio_paths
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 4:4
 *
 **************************************************************************/
unsigned char set_da4_digital_audio_paths( enum da4_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_DA_AUDIO_PATHS_REG);

    /* 
     * 0: DA4 path is disabled 
     * 1: DA4 path is enabled 
     */ 
    switch( param ){
        case DA_4_ENABLE_E: 
           value = old_value | DA_4_DIGITAL_AUDIO_PATHS_MASK; 
           break;
        case DA_4_DISABLE_E: 
           value = old_value & ~ DA_4_DIGITAL_AUDIO_PATHS_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da4_digital_audio_paths
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 4:4
 *
 **************************************************************************/
unsigned char get_da4_digital_audio_paths()
  {
    unsigned char value;


    /* 
     * 0: DA4 path is disabled 
     * 1: DA4 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_4_DIGITAL_AUDIO_PATHS_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da3_digital_audio_paths
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 5:5
 *
 **************************************************************************/
unsigned char set_da3_digital_audio_paths( enum da3_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_DA_AUDIO_PATHS_REG);

    /* 
     * 0: DA3 path is disabled 
     * 1: DA3 path is enabled 
     */ 
    switch( param ){
        case DA_3_ENABLE_E: 
           value = old_value | DA_3_DIGITAL_AUDIO_PATHS_MASK; 
           break;
        case DA_3_DISABLE_E: 
           value = old_value & ~ DA_3_DIGITAL_AUDIO_PATHS_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da3_digital_audio_paths
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 5:5
 *
 **************************************************************************/
unsigned char get_da3_digital_audio_paths()
  {
    unsigned char value;


    /* 
     * 0: DA3 path is disabled 
     * 1: DA3 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_3_DIGITAL_AUDIO_PATHS_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da2_digital_audio_paths
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 6:6
 *
 **************************************************************************/
unsigned char set_da2_digital_audio_paths( enum da2_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_DA_AUDIO_PATHS_REG);

    /* 
     * 0: DA2 path is disabled 
     * 1: DA2 path is enabled 
     */ 
    switch( param ){
        case DA_2_ENABLE_E: 
           value = old_value | DA_2_DIGITAL_AUDIO_PATHS_MASK; 
           break;
        case DA_2_DISABLE_E: 
           value = old_value & ~ DA_2_DIGITAL_AUDIO_PATHS_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da2_digital_audio_paths
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 6:6
 *
 **************************************************************************/
unsigned char get_da2_digital_audio_paths()
  {
    unsigned char value;


    /* 
     * 0: DA2 path is disabled 
     * 1: DA2 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_2_DIGITAL_AUDIO_PATHS_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da1_digital_audio_paths
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 7:7
 *
 **************************************************************************/
unsigned char set_da1_digital_audio_paths( enum da1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_DA_AUDIO_PATHS_REG);

    /* 
     * 0: DA1 path is disabled 
     * 1: DA1 path is enabled 
     */ 
    switch( param ){
        case DA_1_ENABLE_E: 
           value = old_value | DA_1_DIGITAL_AUDIO_PATHS_MASK; 
           break;
        case DA_1_DISABLE_E: 
           value = old_value & ~ DA_1_DIGITAL_AUDIO_PATHS_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da1_digital_audio_paths
 *
 * RET  : Return the value of register 0xDigitalDaAudioPaths
 *
 * Notes : From register 0xD03, bits 7:7
 *
 **************************************************************************/
unsigned char get_da1_digital_audio_paths()
  {
    unsigned char value;


    /* 
     * 0: DA1 path is disabled 
     * 1: DA1 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_1_DIGITAL_AUDIO_PATHS_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_driver_low_power
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 0:0
 *
 **************************************************************************/
unsigned char set_ear_driver_low_power( enum ear_driver_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_D_MIC_CONTROLS_REG);

    /* 
     * 0: Normal Operation 
     * 1: Ear DAC driver in Low Power 
     */ 
    switch( param ){
        case EAR_DRIVER_ENABLE_E: 
           value = old_value | EAR_DRIVER_LOW_POWER_MASK; 
           break;
        case EAR_DRIVER_DISABLE_E: 
           value = old_value & ~ EAR_DRIVER_LOW_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_D_MIC_CONTROLS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_driver_low_power
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 0:0
 *
 **************************************************************************/
unsigned char get_ear_driver_low_power()
  {
    unsigned char value;


    /* 
     * 0: Normal Operation 
     * 1: Ear DAC driver in Low Power 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & EAR_DRIVER_LOW_POWER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_offset_cancelation
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 1:1
 *
 **************************************************************************/
unsigned char set_headset_offset_cancelation( enum headset_offset_cancelation param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_D_MIC_CONTROLS_REG);

    /* 
     * Offset cancellation for headset driver. 
     */ 
    switch( param ){
        case HEADSET_OFFSET_CANCELATION_ON_E: 
           value = old_value | HEADSET_OFFSET_CANCELATION_MASK; 
           break;
        case HEADSET_OFFSET_CANCELATION_OFF_E: 
           value = old_value & ~ HEADSET_OFFSET_CANCELATION_MASK;
           break;
    }
  

    I2CWrite(ANALOG_D_MIC_CONTROLS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_offset_cancelation
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 1:1
 *
 **************************************************************************/
unsigned char get_headset_offset_cancelation()
  {
    unsigned char value;


    /* 
     * Offset cancellation for headset driver. 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & HEADSET_OFFSET_CANCELATION_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_selection
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 3:2
 *
 **************************************************************************/
unsigned char set_ear_selection( enum ear_selection param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_D_MIC_CONTROLS_REG);

    /* 
     * Ear Selection Common Mode 
     */ 
  

     value =  old_value & ~EAR_SELECTION_MASK ;

    switch(  param ){

           case COMMON_MODE_EAR_DRIVER_0_95V_E:
                value  = value  | (COMMON_MODE_EAR_DRIVER_0_95V << 0x2);
                break;
           case COMMON_MODE_EAR_DRIVER_1_1V_E:
                value  = value  | (COMMON_MODE_EAR_DRIVER_1_1V << 0x2);
                break;
           case COMMON_MODE_EAR_DRIVER_1_27V_E:
                value  = value  | (COMMON_MODE_EAR_DRIVER_1_27V << 0x2);
                break;
           case COMMON_MODE_EAR_DRIVER_1_58V_E:
                value  = value  | (COMMON_MODE_EAR_DRIVER_1_58V << 0x2);
                break;
    }
  

    I2CWrite(ANALOG_D_MIC_CONTROLS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_selection
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 3:2
 *
 **************************************************************************/
unsigned char get_ear_selection()
  {
    unsigned char value;


    /* 
     * Ear Selection Common Mode 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & EAR_SELECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_ear
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 4:4
 *
 **************************************************************************/
unsigned char set_low_power_ear( enum low_power_ear param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_D_MIC_CONTROLS_REG);

    /* 
     * Low Power Ear 
     */ 
    switch( param ){
        case LOW_POWER_EAR_E: 
           value = old_value | LOW_POWER_EAR_MASK; 
           break;
        case NORMAL_OPERATION_EAR_E: 
           value = old_value & ~ LOW_POWER_EAR_MASK;
           break;
    }
  

    I2CWrite(ANALOG_D_MIC_CONTROLS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_power_ear
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 4:4
 *
 **************************************************************************/
unsigned char get_low_power_ear()
  {
    unsigned char value;


    /* 
     * Low Power Ear 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & LOW_POWER_EAR_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_dac_headset
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 6:5
 *
 **************************************************************************/
unsigned char set_low_power_dac_headset( enum low_power_dac_headset param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_D_MIC_CONTROLS_REG);

    /* 
     * Low Power DAC headset 
     */ 
  

     value =  old_value & ~LOW_POWER_DAC_HEADSET_MASK ;

    switch(  param ){

           case HS_DAC_NORMAL_OPERATION_E:
                value  = value  | (HS_DAC_NORMAL_OPERATION << 0x5);
                break;
           case HS_DAC_DRIVERS_LOWPOWER_E:
                value  = value  | (HS_DAC_DRIVERS_LOWPOWER << 0x5);
                break;
           case HS_DAC_LOWPOWER_E:
                value  = value  | (HS_DAC_LOWPOWER << 0x5);
                break;
           case HS_DAC_DAC_DRIVERS_LOWPOWER_E:
                value  = value  | (HS_DAC_DAC_DRIVERS_LOWPOWER << 0x5);
                break;
    }
  

    I2CWrite(ANALOG_D_MIC_CONTROLS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_power_dac_headset
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 6:5
 *
 **************************************************************************/
unsigned char get_low_power_dac_headset()
  {
    unsigned char value;


    /* 
     * Low Power DAC headset 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & LOW_POWER_DAC_HEADSET_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_headset
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 7:7
 *
 **************************************************************************/
unsigned char set_low_power_headset( enum audio_low_power_mode param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_D_MIC_CONTROLS_REG);

    /* 
     * Low Power Headset 
     */ 
    switch( param ){
        case LOW_POWER_E: 
           value = old_value | LOW_POWER_HEADSET_MASK; 
           break;
        case NORMAL_OPERATION_E: 
           value = old_value & ~ LOW_POWER_HEADSET_MASK;
           break;
    }
  

    I2CWrite(ANALOG_D_MIC_CONTROLS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_power_headset
 *
 * RET  : Return the value of register 0xAnalogDMicControls
 *
 * Notes : From register 0xD04, bits 7:7
 *
 **************************************************************************/
unsigned char get_low_power_headset()
  {
    unsigned char value;


    /* 
     * Low Power Headset 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & LOW_POWER_HEADSET_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_analog_line_in_right_mute
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 0:0
 *
 **************************************************************************/
unsigned char set_analog_line_in_right_mute( enum analog_line_in_right_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LINE_IN_CONF_REG);

    /* 
     * Analog line in Right is active 
     */ 
    switch( param ){
        case ANALOG_LINE_IN_RIGHT_MUTE_E: 
           value = old_value | ANALOG_LINE_IN_RIGHT_MUTE_MASK; 
           break;
        case ANALOG_LINE_IN_RIGHT_UNMUTE_E: 
           value = old_value & ~ ANALOG_LINE_IN_RIGHT_MUTE_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_line_in_right_mute
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 0:0
 *
 **************************************************************************/
unsigned char get_analog_line_in_right_mute()
  {
    unsigned char value;


    /* 
     * Analog line in Right is active 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & ANALOG_LINE_IN_RIGHT_MUTE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_analog_line_in_left_mute
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 1:1
 *
 **************************************************************************/
unsigned char set_analog_line_in_left_mute( enum analog_line_in_left_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LINE_IN_CONF_REG);

    /* 
     * Analog line in Left is unmuted 
     */ 
    switch( param ){
        case ANALOG_LINE_IN_LEFT_MUTE_E: 
           value = old_value | ANALOG_LINE_IN_LEFT_MUTE_MASK; 
           break;
        case ANALOG_LINE_IN_LEFT_UNMUTE_E: 
           value = old_value & ~ ANALOG_LINE_IN_LEFT_MUTE_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_line_in_left_mute
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 1:1
 *
 **************************************************************************/
unsigned char get_analog_line_in_left_mute()
  {
    unsigned char value;


    /* 
     * Analog line in Left is unmuted 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & ANALOG_LINE_IN_LEFT_MUTE_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_analog_micro2_mute
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 2:2
 *
 **************************************************************************/
unsigned char set_analog_micro2_mute( enum analog_micro2_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LINE_IN_CONF_REG);

    /* 
     * Analog micropnone MIC2 is active 
     */ 
    switch( param ){
        case ANALOG_MICRO_2_MUTE_E: 
           value = old_value | ANALOG_MICRO_2_MUTE_MASK; 
           break;
        case ANALOG_MICRO_2_UNMUTE_E: 
           value = old_value & ~ ANALOG_MICRO_2_MUTE_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_micro2_mute
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 2:2
 *
 **************************************************************************/
unsigned char get_analog_micro2_mute()
  {
    unsigned char value;


    /* 
     * Analog micropnone MIC2 is active 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & ANALOG_MICRO_2_MUTE_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_analog_micro1_mute
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 3:3
 *
 **************************************************************************/
unsigned char set_analog_micro1_mute( enum analog_micro1_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LINE_IN_CONF_REG);

    /* 
     * Analog micropnone MIC1A/B is active 
     */ 
    switch( param ){
        case ANALOG_MICRO_1_MUTE_E: 
           value = old_value | ANALOG_MICRO_1_MUTE_MASK; 
           break;
        case ANALOG_MICRO_1_UNMUTE_E: 
           value = old_value & ~ ANALOG_MICRO_1_MUTE_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_micro1_mute
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 3:3
 *
 **************************************************************************/
unsigned char get_analog_micro1_mute()
  {
    unsigned char value;


    /* 
     * Analog micropnone MIC1A/B is active 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & ANALOG_MICRO_1_MUTE_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_line_in_right_power
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 4:4
 *
 **************************************************************************/
unsigned char set_line_in_right_power( enum line_in_right_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LINE_IN_CONF_REG);

    /* 
     * 0: Line-in right input is in power down 
     * 1: Line-in left input is in power up 
     */ 
    switch( param ){
        case LINE_IN_RIGHT_POWER_UP_E: 
           value = old_value | LINE_IN_RIGHT_POWER_MASK; 
           break;
        case LINE_IN_RIGHT_POWER_DOWN_E: 
           value = old_value & ~ LINE_IN_RIGHT_POWER_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_line_in_right_power
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 4:4
 *
 **************************************************************************/
unsigned char get_line_in_right_power()
  {
    unsigned char value;


    /* 
     * 0: Line-in right input is in power down 
     * 1: Line-in left input is in power up 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & LINE_IN_RIGHT_POWER_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_line_in_left_power
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 5:5
 *
 **************************************************************************/
unsigned char set_line_in_left_power( enum line_in_left_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LINE_IN_CONF_REG);

    /* 
     * 0: Line-in left input is in power down 
     * 1: Line-in left input is in power up 
     */ 
    switch( param ){
        case LINE_IN_LEFT_POWER_UP_E: 
           value = old_value | LINE_IN_LEFT_POWER_MASK; 
           break;
        case LINE_IN_LEFT_POWER_DOWN_E: 
           value = old_value & ~ LINE_IN_LEFT_POWER_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_line_in_left_power
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 5:5
 *
 **************************************************************************/
unsigned char get_line_in_left_power()
  {
    unsigned char value;


    /* 
     * 0: Line-in left input is in power down 
     * 1: Line-in left input is in power up 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & LINE_IN_LEFT_POWER_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_micro2_power
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 6:6
 *
 **************************************************************************/
unsigned char set_micro2_power( enum micro2_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LINE_IN_CONF_REG);

    /* 
     * 0: Mic2 microphone is in power down 
     * 1: Mic2 microphone is in power up 
     */ 
    switch( param ){
        case MICRO_2_POWER_UP_E: 
           value = old_value | MICRO_2_POWER_MASK; 
           break;
        case MICRO_2_POWER_DOWN_E: 
           value = old_value & ~ MICRO_2_POWER_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_micro2_power
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 6:6
 *
 **************************************************************************/
unsigned char get_micro2_power()
  {
    unsigned char value;


    /* 
     * 0: Mic2 microphone is in power down 
     * 1: Mic2 microphone is in power up 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & MICRO_2_POWER_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_micro1_power
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 7:7
 *
 **************************************************************************/
unsigned char set_micro1_power( enum micro1_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LINE_IN_CONF_REG);

    /* 
     * Micro 1a and 1b Enable (power up) 
     */ 
    switch( param ){
        case MICRO_1_POWER_UP_E: 
           value = old_value | MICRO_1_POWER_MASK; 
           break;
        case MICRO_1_POWER_DOWN_E: 
           value = old_value & ~ MICRO_1_POWER_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_micro1_power
 *
 * RET  : Return the value of register 0xLineInConf
 *
 * Notes : From register 0xD05, bits 7:7
 *
 **************************************************************************/
unsigned char get_micro1_power()
  {
    unsigned char value;


    /* 
     * Micro 1a and 1b Enable (power up) 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & MICRO_1_POWER_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fade_speed
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 1:0
 *
 **************************************************************************/
unsigned char set_fade_speed( enum fade_speed param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_INPUTS_ENABLE_REG);

    /* 
     * Approximate Time between each 1dB step Gain Change in 
     * digital gain controls 
     */ 
  

     value =  old_value & ~FADE_SPEED_MASK ;

    switch(  param ){

           case TIME_2MS_E:
                value =  value | TIME_2MS ;
                break;
           case TIME_0_5_MS_E:
                value =  value | TIME_0_5_MS ;
                break;
           case TIME_10_1_MS_E:
                value =  value | TIME_10_1_MS ;
                break;
           case TIME_55MS_E:
                value =  value | TIME_55MS ;
                break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fade_speed
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 1:0
 *
 **************************************************************************/
unsigned char get_fade_speed()
  {
    unsigned char value;


    /* 
     * Approximate Time between each 1dB step Gain Change in 
     * digital gain controls 
     */ 
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & FADE_SPEED_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro6_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 2:2
 *
 **************************************************************************/
unsigned char set_digital_micro6_mute( enum digital6_micro_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_INPUTS_ENABLE_REG);

    /* 
     * 1: Digital Micro 6 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 6 input is muted 
     */ 
    switch( param ){
        case DIGITAL_6_MICRO_MUTE_E: 
           value = old_value | DIGITAL_MICRO_6_MUTE_MASK; 
           break;
        case DIGITAL_6_MICRO_UNMUTE_E: 
           value = old_value & ~ DIGITAL_MICRO_6_MUTE_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro6_mute
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 2:2
 *
 **************************************************************************/
unsigned char get_digital_micro6_mute()
  {
    unsigned char value;


    /* 
     * 1: Digital Micro 6 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 6 input is muted 
     */ 
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_MICRO_6_MUTE_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro5_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 3:3
 *
 **************************************************************************/
unsigned char set_digital_micro5_mute( enum digital5_micro_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_INPUTS_ENABLE_REG);

    /* 
     * 1: Digital Micro 5 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 5 input is muted 
     */ 
    switch( param ){
        case DIGITAL_5_MICRO_MUTE_E: 
           value = old_value | DIGITAL_MICRO_5_MUTE_MASK; 
           break;
        case DIGITAL_5_MICRO_UNMUTE_E: 
           value = old_value & ~ DIGITAL_MICRO_5_MUTE_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro5_mute
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 3:3
 *
 **************************************************************************/
unsigned char get_digital_micro5_mute()
  {
    unsigned char value;


    /* 
     * 1: Digital Micro 5 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 5 input is muted 
     */ 
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_MICRO_5_MUTE_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro4_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 4:4
 *
 **************************************************************************/
unsigned char set_digital_micro4_mute( enum digital4_micro_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_INPUTS_ENABLE_REG);

    /* 
     * 1: Digital Micro 4 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 4 input is muted 
     */ 
    switch( param ){
        case DIGITAL_4_MICRO_MUTE_E: 
           value = old_value | DIGITAL_MICRO_4_MUTE_MASK; 
           break;
        case DIGITAL_4_MICRO_UNMUTE_E: 
           value = old_value & ~ DIGITAL_MICRO_4_MUTE_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro4_mute
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 4:4
 *
 **************************************************************************/
unsigned char get_digital_micro4_mute()
  {
    unsigned char value;


    /* 
     * 1: Digital Micro 4 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 4 input is muted 
     */ 
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_MICRO_4_MUTE_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro3_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 5:5
 *
 **************************************************************************/
unsigned char set_digital_micro3_mute( enum digital3_micro_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_INPUTS_ENABLE_REG);

    /* 
     * 1: Digital Micro 3 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 3 input is muted 
     */ 
    switch( param ){
        case DIGITAL_3_MICRO_MUTE_E: 
           value = old_value | DIGITAL_MICRO_3_MUTE_MASK; 
           break;
        case DIGITAL_3_MICRO_UNMUTE_E: 
           value = old_value & ~ DIGITAL_MICRO_3_MUTE_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro3_mute
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 5:5
 *
 **************************************************************************/
unsigned char get_digital_micro3_mute()
  {
    unsigned char value;


    /* 
     * 1: Digital Micro 3 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 3 input is muted 
     */ 
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_MICRO_3_MUTE_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro2_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 6:6
 *
 **************************************************************************/
unsigned char set_digital_micro2_mute( enum digital2_micro_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_INPUTS_ENABLE_REG);

    /* 
     * 1: Digital Micro 2 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 2 input is muted 
     */ 
    switch( param ){
        case DIGITAL_2_MICRO_MUTE_E: 
           value = old_value | DIGITAL_MICRO_2_MUTE_MASK; 
           break;
        case DIGITAL_2_MICRO_UNMUTE_E: 
           value = old_value & ~ DIGITAL_MICRO_2_MUTE_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro2_mute
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 6:6
 *
 **************************************************************************/
unsigned char get_digital_micro2_mute()
  {
    unsigned char value;


    /* 
     * 1: Digital Micro 2 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 2 input is muted 
     */ 
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_MICRO_2_MUTE_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro1_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 7:7
 *
 **************************************************************************/
unsigned char set_digital_micro1_mute( enum digital1_micro_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_INPUTS_ENABLE_REG);

    /* 
     * 1: Digital Micro 1 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 1 input is muted 
     */ 
    switch( param ){
        case DIGITAL_1_MICRO_MUTE_E: 
           value = old_value | DIGITAL_MICRO_1_MUTE_MASK; 
           break;
        case DIGITAL_1_MICRO_UNMUTE_E: 
           value = old_value & ~ DIGITAL_MICRO_1_MUTE_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro1_mute
 *
 * RET  : Return the value of register 0xDigitalInputsEnable
 *
 * Notes : From register 0xD06, bits 7:7
 *
 **************************************************************************/
unsigned char get_digital_micro1_mute()
  {
    unsigned char value;


    /* 
     * 1: Digital Micro 1 input is active and Dmic12Clk is 
     * running, 
     * 0: Digital Micro 1 input is muted 
     */ 
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_MICRO_1_MUTE_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc2_power
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 0:0
 *
 **************************************************************************/
unsigned char set_adc2_power( enum adc2_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG);

    /* 
     * 0: ADC2 is power down 
     * 1: ADC2 is power-up 
     */ 
    switch( param ){
        case ADC_2_POWER_UP_E: 
           value = old_value | ADC_2_POWER_MASK; 
           break;
        case ADC_2_POWER_DOWN_E: 
           value = old_value & ~ ADC_2_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc2_power
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 0:0
 *
 **************************************************************************/
unsigned char get_adc2_power()
  {
    unsigned char value;


    /* 
     * 0: ADC2 is power down 
     * 1: ADC2 is power-up 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & ADC_2_POWER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc1_power
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 1:1
 *
 **************************************************************************/
unsigned char set_adc1_power( enum adc1_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG);

    /* 
     * 0: ADC1 is power down 
     * 1: ADC1 is power-up 
     */ 
    switch( param ){
        case ADC_1_POWER_UP_E: 
           value = old_value | ADC_1_POWER_MASK; 
           break;
        case ADC_1_POWER_DOWN_E: 
           value = old_value & ~ ADC_1_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc1_power
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 1:1
 *
 **************************************************************************/
unsigned char get_adc1_power()
  {
    unsigned char value;


    /* 
     * 0: ADC1 is power down 
     * 1: ADC1 is power-up 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & ADC_1_POWER_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc3_power
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 2:2
 *
 **************************************************************************/
unsigned char set_adc3_power( enum adc3_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG);

    /* 
     * 0: ADC3 is power down 
     * 1: ADC3 is power-up 
     */ 
    switch( param ){
        case ADC_3_POWER_UP_E: 
           value = old_value | ADC_3_POWER_MASK; 
           break;
        case ADC_3_POWER_DOWN_E: 
           value = old_value & ~ ADC_3_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc3_power
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 2:2
 *
 **************************************************************************/
unsigned char get_adc3_power()
  {
    unsigned char value;


    /* 
     * 0: ADC3 is power down 
     * 1: ADC3 is power-up 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & ADC_3_POWER_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hsr_dac_driver_power
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 4:4
 *
 **************************************************************************/
unsigned char set_hsr_dac_driver_power( enum hsr_dac_driver_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG);

    /* 
     * HeadSetRight Dac driver is power up 
     */ 
    switch( param ){
        case HSR_DAC_DRIVER_POWER_UP_E: 
           value = old_value | HSR_DAC_DRIVER_POWER_MASK; 
           break;
        case HSR_DAC_DRIVER_POWER_DOWN_E: 
           value = old_value & ~ HSR_DAC_DRIVER_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hsr_dac_driver_power
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 4:4
 *
 **************************************************************************/
unsigned char get_hsr_dac_driver_power()
  {
    unsigned char value;


    /* 
     * HeadSetRight Dac driver is power up 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & HSR_DAC_DRIVER_POWER_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hsl_dac_driver_power
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 5:5
 *
 **************************************************************************/
unsigned char set_hsl_dac_driver_power( enum hsl_dac_driver_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG);

    /* 
     * HeadSetLelf Dac driver is power up 
     */ 
    switch( param ){
        case HSL_DAC_DRIVER_POWER_UP_E: 
           value = old_value | HSL_DAC_DRIVER_POWER_MASK; 
           break;
        case HSL_DAC_DRIVER_POWER_DOWN_E: 
           value = old_value & ~ HSL_DAC_DRIVER_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hsl_dac_driver_power
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 5:5
 *
 **************************************************************************/
unsigned char get_hsl_dac_driver_power()
  {
    unsigned char value;


    /* 
     * HeadSetLelf Dac driver is power up 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & HSL_DAC_DRIVER_POWER_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc2_connected_to_mic
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 6:6
 *
 **************************************************************************/
unsigned char set_adc2_connected_to_mic( enum adc2_connected_to_mic param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG);

    /* 
     * 0: connected to MIC2B, 1 : connected to LINR 
     */ 
    switch( param ){
        case CONNECTED_LINR_E: 
           value = old_value | ADC_2_CONNECTED_TO_MIC_MASK; 
           break;
        case CONNECTED_TO_MIC2_E: 
           value = old_value & ~ ADC_2_CONNECTED_TO_MIC_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc2_connected_to_mic
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 6:6
 *
 **************************************************************************/
unsigned char get_adc2_connected_to_mic()
  {
    unsigned char value;


    /* 
     * 0: connected to MIC2B, 1 : connected to LINR 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & ADC_2_CONNECTED_TO_MIC_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc1_connected_to_mic
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 7:7
 *
 **************************************************************************/
unsigned char set_adc1_connected_to_mic( enum adc1_connected_to_mic_a param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG);

    /* 
     * 1: connected to MIC1B, 0 : connected to MIC1A 
     */ 
    switch( param ){
        case CONNECTED_TO_MIC1B_E: 
           value = old_value | ADC_1_CONNECTED_TO_MIC_MASK; 
           break;
        case CONNECTED_TO_MIC1A_E: 
           value = old_value & ~ ADC_1_CONNECTED_TO_MIC_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc1_connected_to_mic
 *
 * RET  : Return the value of register 0xAnalogDigitalConvertersEnable
 *
 * Notes : From register 0xD07, bits 7:7
 *
 **************************************************************************/
unsigned char get_adc1_connected_to_mic()
  {
    unsigned char value;


    /* 
     * 1: connected to MIC1B, 0 : connected to MIC1A 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & ADC_1_CONNECTED_TO_MIC_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vibrator_class_d_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 1:1
 *
 **************************************************************************/
unsigned char set_vibrator_class_d_power( enum vibrator_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_ENABLE_REG);

    /* 
     * 1: The Vibrator Class-D driver is powered up 
     */ 
    switch( param ){
        case VIBRATOR_POWER_UP_E: 
           value = old_value | VIBRATOR_CLASS_D_POWER_MASK; 
           break;
        case VIBRATOR_POWER_DOWN_E: 
           value = old_value & ~ VIBRATOR_CLASS_D_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vibrator_class_d_power
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 1:1
 *
 **************************************************************************/
unsigned char get_vibrator_class_d_power()
  {
    unsigned char value;


    /* 
     * 1: The Vibrator Class-D driver is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & VIBRATOR_CLASS_D_POWER_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hf_class_d_rigth_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 2:2
 *
 **************************************************************************/
unsigned char set_hf_class_d_rigth_power( enum hf_right_class_d_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_ENABLE_REG);

    /* 
     * 1: The HFR Class-D driver is powered up 
     */ 
    switch( param ){
        case HF_RIGHT_CLASS_D_POWER_UP_E: 
           value = old_value | HF_CLASS_D_RIGTH_POWER_MASK; 
           break;
        case HF_RIGHT_CLASS_D_POWER_DOWN_E: 
           value = old_value & ~ HF_CLASS_D_RIGTH_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hf_class_d_rigth_power
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 2:2
 *
 **************************************************************************/
unsigned char get_hf_class_d_rigth_power()
  {
    unsigned char value;


    /* 
     * 1: The HFR Class-D driver is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & HF_CLASS_D_RIGTH_POWER_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hf_class_d_left_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 3:3
 *
 **************************************************************************/
unsigned char set_hf_class_d_left_power( enum hf_left_class_d_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_ENABLE_REG);

    /* 
     * 1: The HFL Class-D driver is powered up 
     */ 
    switch( param ){
        case HF_LEFT_CLASS_D_POWER_UP_E: 
           value = old_value | HF_CLASS_D_LEFT_POWER_MASK; 
           break;
        case HF_LEFT_CLASS_D_POWER_DOWN_E: 
           value = old_value & ~ HF_CLASS_D_LEFT_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hf_class_d_left_power
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 3:3
 *
 **************************************************************************/
unsigned char get_hf_class_d_left_power()
  {
    unsigned char value;


    /* 
     * 1: The HFL Class-D driver is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & HF_CLASS_D_LEFT_POWER_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_right_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 4:4
 *
 **************************************************************************/
unsigned char set_headset_right_power( enum headset_right_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_ENABLE_REG);

    /* 
     * 1: The Headset Right is powered up 
     */ 
    switch( param ){
        case HEADSET_RIGHT_POWER_UP_E: 
           value = old_value | HEADSET_RIGHT_POWER_MASK; 
           break;
        case HEADSET_RIGHT_POWER_DOWN_E: 
           value = old_value & ~ HEADSET_RIGHT_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_right_power
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 4:4
 *
 **************************************************************************/
unsigned char get_headset_right_power()
  {
    unsigned char value;


    /* 
     * 1: The Headset Right is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & HEADSET_RIGHT_POWER_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_left_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 5:5
 *
 **************************************************************************/
unsigned char set_headset_left_power( enum headset_left_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_ENABLE_REG);

    /* 
     * 1: The Headset Left is powered up 
     */ 
    switch( param ){
        case HEADSET_LEFT_POWER_UP_E: 
           value = old_value | HEADSET_LEFT_POWER_MASK; 
           break;
        case HEADSET_LEFT_POWER_DOWN_E: 
           value = old_value & ~ HEADSET_LEFT_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_left_power
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 5:5
 *
 **************************************************************************/
unsigned char get_headset_left_power()
  {
    unsigned char value;


    /* 
     * 1: The Headset Left is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & HEADSET_LEFT_POWER_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_class_ab_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 6:6
 *
 **************************************************************************/
unsigned char set_ear_class_ab_power( enum ear_class_ab_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_ENABLE_REG);

    /* 
     * 1: The EAR Class-AB driver is powered up 
     */ 
    switch( param ){
        case EAR_CLASS_AB_POWER_UP_E: 
           value = old_value | EAR_CLASS_AB_POWER_MASK; 
           break;
        case EAR_CLASS_AB_POWER_DOWN_E: 
           value = old_value & ~ EAR_CLASS_AB_POWER_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_class_ab_power
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 6:6
 *
 **************************************************************************/
unsigned char get_ear_class_ab_power()
  {
    unsigned char value;


    /* 
     * 1: The EAR Class-AB driver is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & EAR_CLASS_AB_POWER_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_internal_pull_down
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 7:7
 *
 **************************************************************************/
unsigned char set_internal_pull_down( enum internal_pull_down_enabled param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_ENABLE_REG);

    /* 
     * 0: Internal pull down on VssVcpHs is enabled. 
     * 1: Internal pull down on VssVcpHs is disabled. 
     */ 
    switch( param ){
        case INTERNAL_PULL_DOWN_ENABLE_E: 
           value = old_value | INTERNAL_PULL_DOWN_MASK; 
           break;
        case INTERNAL_PULL_DOWN_DISABLE_E: 
           value = old_value & ~ INTERNAL_PULL_DOWN_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_internal_pull_down
 *
 * RET  : Return the value of register 0xAnalogOutputEnable
 *
 * Notes : From register 0xD08, bits 7:7
 *
 **************************************************************************/
unsigned char get_internal_pull_down()
  {
    unsigned char value;


    /* 
     * 0: Internal pull down on VssVcpHs is enabled. 
     * 1: Internal pull down on VssVcpHs is disabled. 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & INTERNAL_PULL_DOWN_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vibrator_class_d_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 1:1
 *
 **************************************************************************/
unsigned char set_vibrator_class_d_path_power( enum vibrator_class_d_path_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_OUTPUT_ENABLE_REG);

    /* 
     * 1: The Vibrator and digital dedicated path are powered up 
     */ 
    switch( param ){
        case VIBRATOR_CLASS_D_PATH_POWER_UP_E: 
           value = old_value | VIBRATOR_CLASS_D_PATH_POWER_MASK; 
           break;
        case VIBRATOR_CLASS_D_PATH_POWER_DOWN_E: 
           value = old_value & ~ VIBRATOR_CLASS_D_PATH_POWER_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vibrator_class_d_path_power
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 1:1
 *
 **************************************************************************/
unsigned char get_vibrator_class_d_path_power()
  {
    unsigned char value;


    /* 
     * 1: The Vibrator and digital dedicated path are powered up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & VIBRATOR_CLASS_D_PATH_POWER_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hf_class_d_rigth_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 2:2
 *
 **************************************************************************/
unsigned char set_hf_class_d_rigth_path_power( enum hf_class_d_rigth_path_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_OUTPUT_ENABLE_REG);

    /* 
     * 1: The HFR Class-D end digital dedicated path are powered 
     * up 
     */ 
    switch( param ){
        case HF_CLASS_D_RIGTH_PATH_POWER_UP_E: 
           value = old_value | HF_CLASS_D_RIGTH_PATH_POWER_MASK; 
           break;
        case HF_CLASS_D_RIGTH_PATH_POWER_DOWN_E: 
           value = old_value & ~ HF_CLASS_D_RIGTH_PATH_POWER_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hf_class_d_rigth_path_power
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 2:2
 *
 **************************************************************************/
unsigned char get_hf_class_d_rigth_path_power()
  {
    unsigned char value;


    /* 
     * 1: The HFR Class-D end digital dedicated path are powered 
     * up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & HF_CLASS_D_RIGTH_PATH_POWER_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hf_class_d_left_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 3:3
 *
 **************************************************************************/
unsigned char set_hf_class_d_left_path_power( enum hf_class_d_left_path_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_OUTPUT_ENABLE_REG);

    /* 
     * 1 :The HFL Class-D and digital dedicated path are powered 
     * up 
     */ 
    switch( param ){
        case HF_CLASS_D_LEFT_PATH_POWER_UP_E: 
           value = old_value | HF_CLASS_D_LEFT_PATH_POWER_MASK; 
           break;
        case HF_CLASS_D_LEFT_PATH_POWER_DOWN_E: 
           value = old_value & ~ HF_CLASS_D_LEFT_PATH_POWER_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hf_class_d_left_path_power
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 3:3
 *
 **************************************************************************/
unsigned char get_hf_class_d_left_path_power()
  {
    unsigned char value;


    /* 
     * 1 :The HFL Class-D and digital dedicated path are powered 
     * up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & HF_CLASS_D_LEFT_PATH_POWER_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_right_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 4:4
 *
 **************************************************************************/
unsigned char set_headset_right_path_power( enum headset_right_path_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_OUTPUT_ENABLE_REG);

    /* 
     * 1: HSR DAC and digital dedicated path are powered up 
     */ 
    switch( param ){
        case HEADSET_RIGHT_PATH_POWER_UP_E: 
           value = old_value | HEADSET_RIGHT_PATH_POWER_MASK; 
           break;
        case HEADSET_RIGHT_PATH_POWER_DOWN_E: 
           value = old_value & ~ HEADSET_RIGHT_PATH_POWER_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_right_path_power
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 4:4
 *
 **************************************************************************/
unsigned char get_headset_right_path_power()
  {
    unsigned char value;


    /* 
     * 1: HSR DAC and digital dedicated path are powered up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & HEADSET_RIGHT_PATH_POWER_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_left_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 5:5
 *
 **************************************************************************/
unsigned char set_headset_left_path_power( enum headset_left_path_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_OUTPUT_ENABLE_REG);

    /* 
     * 1: HSL DAC and digital dedicated path are powered up 
     */ 
    switch( param ){
        case HEADSET_LEFT_PATH_POWER_UP_E: 
           value = old_value | HEADSET_LEFT_PATH_POWER_MASK; 
           break;
        case HEADSET_LEFT_PATH_POWER_DOWN_E: 
           value = old_value & ~ HEADSET_LEFT_PATH_POWER_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_left_path_power
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 5:5
 *
 **************************************************************************/
unsigned char get_headset_left_path_power()
  {
    unsigned char value;


    /* 
     * 1: HSL DAC and digital dedicated path are powered up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & HEADSET_LEFT_PATH_POWER_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_and_digital_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 6:6
 *
 **************************************************************************/
unsigned char set_ear_and_digital_path_power( enum ear_and_digital_path_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_OUTPUT_ENABLE_REG);

    /* 
     * 1: EAR DAC and digital dedicated path are powered up 
     */ 
    switch( param ){
        case EAR_AND_DIGITAL_PATH_POWER_UP_E: 
           value = old_value | EAR_AND_DIGITAL_PATH_POWER_MASK; 
           break;
        case EAR_AND_DIGITAL_PATH_POWER_DOWN_E: 
           value = old_value & ~ EAR_AND_DIGITAL_PATH_POWER_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_and_digital_path_power
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 6:6
 *
 **************************************************************************/
unsigned char get_ear_and_digital_path_power()
  {
    unsigned char value;


    /* 
     * 1: EAR DAC and digital dedicated path are powered up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & EAR_AND_DIGITAL_PATH_POWER_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_frequency_lim
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 7:7
 *
 **************************************************************************/
unsigned char set_low_frequency_lim( enum low_frequency_lim_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_OUTPUT_ENABLE_REG);

    /* 
     * 0: Charge Pump is working in normal mode. 
     * 1: Enable low frequency limit for Charge Pump (NCP 
     * frequency cannot be in audio band) 
     */ 
    switch( param ){
        case LOW_FREQUENCY_LIM_ENABLE_E: 
           value = old_value | LOW_FREQUENCY_LIM_MASK; 
           break;
        case LOW_FREQUENCY_LIM_DISABLE_E: 
           value = old_value & ~ LOW_FREQUENCY_LIM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_frequency_lim
 *
 * RET  : Return the value of register 0xDigitalOutputEnable
 *
 * Notes : From register 0xD09, bits 7:7
 *
 **************************************************************************/
unsigned char get_low_frequency_lim()
  {
    unsigned char value;


    /* 
     * 0: Charge Pump is working in normal mode. 
     * 1: Enable low frequency limit for Charge Pump (NCP 
     * frequency cannot be in audio band) 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & LOW_FREQUENCY_LIM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_right_dac_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 0:0
 *
 **************************************************************************/
unsigned char set_headset_right_dac_mute( enum headset_right_dac_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_MUTE_REG);

    /* 
     * 0: The HsR DAC is in normal mode 
     * 1: The HsR DAC is muted 
     */ 
    switch( param ){
        case HEADSET_RIGHT_DAC_MUTE_E: 
           value = old_value | HEADSET_RIGHT_DAC_MUTE_MASK; 
           break;
        case HEADSET_RIGHT_DAC_UNMUTE_E: 
           value = old_value & ~ HEADSET_RIGHT_DAC_MUTE_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_right_dac_mute
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 0:0
 *
 **************************************************************************/
unsigned char get_headset_right_dac_mute()
  {
    unsigned char value;


    /* 
     * 0: The HsR DAC is in normal mode 
     * 1: The HsR DAC is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & HEADSET_RIGHT_DAC_MUTE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_left_dac_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 1:1
 *
 **************************************************************************/
unsigned char set_headset_left_dac_mute( enum headset_left_dac_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_MUTE_REG);

    /* 
     * 0: The HsL DAC is in normal mode 
     * 1: The HsL DAC is muted 
     */ 
    switch( param ){
        case HEADSET_LEFT_DAC_MUTE_E: 
           value = old_value | HEADSET_LEFT_DAC_MUTE_MASK; 
           break;
        case HEADSET_LEFT_DAC_UNMUTE_E: 
           value = old_value & ~ HEADSET_LEFT_DAC_MUTE_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_left_dac_mute
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 1:1
 *
 **************************************************************************/
unsigned char get_headset_left_dac_mute()
  {
    unsigned char value;


    /* 
     * 0: The HsL DAC is in normal mode 
     * 1: The HsL DAC is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & HEADSET_LEFT_DAC_MUTE_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_dac_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 2:2
 *
 **************************************************************************/
unsigned char set_ear_dac_mute( enum ear_dac_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_MUTE_REG);

    /* 
     * 0: The Ear DAC is in normal mode 
     * 1: The Ear DAC is muted 
     */ 
    switch( param ){
        case EAR_DAC_MUTE_E: 
           value = old_value | EAR_DAC_MUTE_MASK; 
           break;
        case EAR_DAC_UNMUTE_E: 
           value = old_value & ~ EAR_DAC_MUTE_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_dac_mute
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 2:2
 *
 **************************************************************************/
unsigned char get_ear_dac_mute()
  {
    unsigned char value;


    /* 
     * 0: The Ear DAC is in normal mode 
     * 1: The Ear DAC is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & EAR_DAC_MUTE_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_right_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 4:4
 *
 **************************************************************************/
unsigned char set_headset_right_mute( enum headset_right_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_MUTE_REG);

    /* 
     * 1: The Headset Right is muted 
     */ 
    switch( param ){
        case HEADSET_RIGHT_MUTE_E: 
           value = old_value | HEADSET_RIGHT_MUTE_MASK; 
           break;
        case HEADSET_RIGHT_UNMUTE_E: 
           value = old_value & ~ HEADSET_RIGHT_MUTE_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_right_mute
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 4:4
 *
 **************************************************************************/
unsigned char get_headset_right_mute()
  {
    unsigned char value;


    /* 
     * 1: The Headset Right is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & HEADSET_RIGHT_MUTE_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_left_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 5:5
 *
 **************************************************************************/
unsigned char set_headset_left_mute( enum headset_left_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_MUTE_REG);

    /* 
     * 1: The Headset Left is muted 
     */ 
    switch( param ){
        case HEADSET_LEFT_MUTE_E: 
           value = old_value | HEADSET_LEFT_MUTE_MASK; 
           break;
        case HEADSET_LEFT_UNMUTE_E: 
           value = old_value & ~ HEADSET_LEFT_MUTE_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_left_mute
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 5:5
 *
 **************************************************************************/
unsigned char get_headset_left_mute()
  {
    unsigned char value;


    /* 
     * 1: The Headset Left is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & HEADSET_LEFT_MUTE_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_class_ab_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 6:6
 *
 **************************************************************************/
unsigned char set_ear_class_ab_mute( enum ear_class_ab_mute param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_MUTE_REG);

    /* 
     * 1: The EAR Class-AB driver is muted 
     */ 
    switch( param ){
        case EAR_CLASS_AB_MUTE_E: 
           value = old_value | EAR_CLASS_AB_MUTE_MASK; 
           break;
        case EAR_CLASS_AB_UNMUTE_E: 
           value = old_value & ~ EAR_CLASS_AB_MUTE_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_class_ab_mute
 *
 * RET  : Return the value of register 0xAnalogOutputMute
 *
 * Notes : From register 0xD0A, bits 6:6
 *
 **************************************************************************/
unsigned char get_ear_class_ab_mute()
  {
    unsigned char value;


    /* 
     * 1: The EAR Class-AB driver is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & EAR_CLASS_AB_MUTE_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_zerocross
 *
 * IN   : param, a value to write to the regiter ShortCircuitDetection
 * OUT  : 
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 0:0
 *
 **************************************************************************/
unsigned char set_headset_zerocross( enum zerocross_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SHORT_CIRCUIT_DETECTION_REG);

    /* 
     * 0: HS gain changes on signal zero cross (unless time-out 
     * occurs) 
     * 1: HS gain is changed without zero cross control 
     */ 
    switch( param ){
        case ZEROCROSS_ENABLE_E: 
           value = old_value | HEADSET_ZEROCROSS_MASK; 
           break;
        case ZEROCROSS_DISABLE_E: 
           value = old_value & ~ HEADSET_ZEROCROSS_MASK;
           break;
    }
  

    I2CWrite(SHORT_CIRCUIT_DETECTION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_zerocross
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 0:0
 *
 **************************************************************************/
unsigned char get_headset_zerocross()
  {
    unsigned char value;


    /* 
     * 0: HS gain changes on signal zero cross (unless time-out 
     * occurs) 
     * 1: HS gain is changed without zero cross control 
     */ 
    value = (I2CRead(SHORT_CIRCUIT_DETECTION_REG) & HEADSET_ZEROCROSS_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_fading
 *
 * IN   : param, a value to write to the regiter ShortCircuitDetection
 * OUT  : 
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 1:1
 *
 **************************************************************************/
unsigned char set_headset_fading( enum headset_fading_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SHORT_CIRCUIT_DETECTION_REG);

    /* 
     * 0: All intermediate steps are applied between 2 
     * programmed gains (fading) 
     * 1: Gain on HS is applied immediately 
     */ 
    switch( param ){
        case HEADSET_FADING_ENABLE_E: 
           value = old_value | HEADSET_FADING_MASK; 
           break;
        case HEADSET_FADING_DISABLE_E: 
           value = old_value & ~ HEADSET_FADING_MASK;
           break;
    }
  

    I2CWrite(SHORT_CIRCUIT_DETECTION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_fading
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 1:1
 *
 **************************************************************************/
unsigned char get_headset_fading()
  {
    unsigned char value;


    /* 
     * 0: All intermediate steps are applied between 2 
     * programmed gains (fading) 
     * 1: Gain on HS is applied immediately 
     */ 
    value = (I2CRead(SHORT_CIRCUIT_DETECTION_REG) & HEADSET_FADING_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_oscillator
 *
 * IN   : param, a value to write to the regiter ShortCircuitDetection
 * OUT  : 
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 2:2
 *
 **************************************************************************/
unsigned char set_headset_oscillator( enum oscillator_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SHORT_CIRCUIT_DETECTION_REG);

    /* 
     * 0: The HS drivers use the system clock 
     * 1: The HS drivers use a local oscillator (system clock 
     * absent: analog path only) 
     */ 
    switch( param ){
        case OSCILLATOR_ENABLE_E: 
           value = old_value | HEADSET_OSCILLATOR_MASK; 
           break;
        case OSCILLATOR_DISABLE_E: 
           value = old_value & ~ HEADSET_OSCILLATOR_MASK;
           break;
    }
  

    I2CWrite(SHORT_CIRCUIT_DETECTION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_oscillator
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 2:2
 *
 **************************************************************************/
unsigned char get_headset_oscillator()
  {
    unsigned char value;


    /* 
     * 0: The HS drivers use the system clock 
     * 1: The HS drivers use a local oscillator (system clock 
     * absent: analog path only) 
     */ 
    value = (I2CRead(SHORT_CIRCUIT_DETECTION_REG) & HEADSET_OSCILLATOR_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_pull_down
 *
 * IN   : param, a value to write to the regiter ShortCircuitDetection
 * OUT  : 
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 4:4
 *
 **************************************************************************/
unsigned char set_headset_pull_down( enum headset_pull_down_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SHORT_CIRCUIT_DETECTION_REG);

    /* 
     * 1: HSL and HSR outputs are pulled down to ground 
     * 0: HSL and HSR outputs are in high impedance 
     */ 
    switch( param ){
        case HEADSET_PULL_DOWN_ENABLE_E: 
           value = old_value | HEADSET_PULL_DOWN_MASK; 
           break;
        case HEADSET_PULL_DOWN_DISABLE_E: 
           value = old_value & ~ HEADSET_PULL_DOWN_MASK;
           break;
    }
  

    I2CWrite(SHORT_CIRCUIT_DETECTION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_pull_down
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 4:4
 *
 **************************************************************************/
unsigned char get_headset_pull_down()
  {
    unsigned char value;


    /* 
     * 1: HSL and HSR outputs are pulled down to ground 
     * 0: HSL and HSR outputs are in high impedance 
     */ 
    value = (I2CRead(SHORT_CIRCUIT_DETECTION_REG) & HEADSET_PULL_DOWN_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_detection
 *
 * IN   : param, a value to write to the regiter ShortCircuitDetection
 * OUT  : 
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 5:5
 *
 **************************************************************************/
unsigned char set_headset_detection( enum headset_detection_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SHORT_CIRCUIT_DETECTION_REG);

    /* 
     * 1: Short circuit detected on headset Left and Right is 
     * enabled 
     */ 
    switch( param ){
        case HEADSET_DETECTION_ENABLE_E: 
           value = old_value | HEADSET_DETECTION_MASK; 
           break;
        case HEADSET_DETECTION_DISABLE_E: 
           value = old_value & ~ HEADSET_DETECTION_MASK;
           break;
    }
  

    I2CWrite(SHORT_CIRCUIT_DETECTION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_detection
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 5:5
 *
 **************************************************************************/
unsigned char get_headset_detection()
  {
    unsigned char value;


    /* 
     * 1: Short circuit detected on headset Left and Right is 
     * enabled 
     */ 
    value = (I2CRead(SHORT_CIRCUIT_DETECTION_REG) & HEADSET_DETECTION_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_detection
 *
 * IN   : param, a value to write to the regiter ShortCircuitDetection
 * OUT  : 
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 6:6
 *
 **************************************************************************/
unsigned char set_ear_detection( enum ear_detection_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SHORT_CIRCUIT_DETECTION_REG);

    /* 
     * 1: Short circuit detected on EAR driver enabled 
     */ 
    switch( param ){
        case EAR_DETECTION_ENABLE_E: 
           value = old_value | EAR_DETECTION_MASK; 
           break;
        case EAR_DETECTION_DISABLE_E: 
           value = old_value & ~ EAR_DETECTION_MASK;
           break;
    }
  

    I2CWrite(SHORT_CIRCUIT_DETECTION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_detection
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 6:6
 *
 **************************************************************************/
unsigned char get_ear_detection()
  {
    unsigned char value;


    /* 
     * 1: Short circuit detected on EAR driver enabled 
     */ 
    value = (I2CRead(SHORT_CIRCUIT_DETECTION_REG) & EAR_DETECTION_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_auto_switch_off
 *
 * IN   : param, a value to write to the regiter ShortCircuitDetection
 * OUT  : 
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 7:7
 *
 **************************************************************************/
unsigned char set_ear_auto_switch_off( enum switch_off_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SHORT_CIRCUIT_DETECTION_REG);

    /* 
     * This bit act on Earpiece and Headset drive. 
     * 1: Automatic switch off on short circuit detection is 
     * enabled 
     */ 
    switch( param ){
        case SWITCH_OFF_ENABLE_E: 
           value = old_value | EAR_AUTO_SWITCH_OFF_MASK; 
           break;
        case SWITCH_OFF_DISABLE_E: 
           value = old_value & ~ EAR_AUTO_SWITCH_OFF_MASK;
           break;
    }
  

    I2CWrite(SHORT_CIRCUIT_DETECTION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_auto_switch_off
 *
 * RET  : Return the value of register 0xShortCircuitDetection
 *
 * Notes : From register 0xD0B, bits 7:7
 *
 **************************************************************************/
unsigned char get_ear_auto_switch_off()
  {
    unsigned char value;


    /* 
     * This bit act on Earpiece and Headset drive. 
     * 1: Automatic switch off on short circuit detection is 
     * enabled 
     */ 
    value = (I2CRead(SHORT_CIRCUIT_DETECTION_REG) & EAR_AUTO_SWITCH_OFF_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_auto_headset_power_up
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 0:0
 *
 **************************************************************************/
unsigned char set_auto_headset_power_up( enum auto_headset_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(POWER_UP_HEAD_SET_REG);

    /* 
     * For a complete auto power-up sequence, It should be 
     * enabled or disabled together or after ENCPHS. 
     */ 
    switch( param ){
        case AUTO_HEADSET_POWER_UP_E: 
           value = old_value | AUTO_HEADSET_POWER_UP_MASK; 
           break;
        case AUTO_HEADSET_POWER_DOWN_E: 
           value = old_value & ~ AUTO_HEADSET_POWER_UP_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_auto_headset_power_up
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 0:0
 *
 **************************************************************************/
unsigned char get_auto_headset_power_up()
  {
    unsigned char value;


    /* 
     * For a complete auto power-up sequence, It should be 
     * enabled or disabled together or after ENCPHS. 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & AUTO_HEADSET_POWER_UP_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_line_out_driver
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 2:2
 *
 **************************************************************************/
unsigned char set_right_line_out_driver( enum right_line_out_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(POWER_UP_HEAD_SET_REG);

    /* 
     * 0: Right Line-Out driver is disabled. 
     * 1: Right Line-Out driver is enabled 
     */ 
    switch( param ){
        case RIGHT_LINE_OUT_ENABLE_E: 
           value = old_value | RIGHT_LINE_OUT_DRIVER_MASK; 
           break;
        case RIGHT_LINE_OUT_DISABLE_E: 
           value = old_value & ~ RIGHT_LINE_OUT_DRIVER_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_line_out_driver
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 2:2
 *
 **************************************************************************/
unsigned char get_right_line_out_driver()
  {
    unsigned char value;


    /* 
     * 0: Right Line-Out driver is disabled. 
     * 1: Right Line-Out driver is enabled 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & RIGHT_LINE_OUT_DRIVER_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_line_out_driver
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 3:3
 *
 **************************************************************************/
unsigned char set_left_line_out_driver( enum left_line_out_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(POWER_UP_HEAD_SET_REG);

    /* 
     * 0: Left Line-Out driver is disabled. 
     * 1: Left Line-Out driver is enabled 
     */ 
    switch( param ){
        case LEFT_LINE_OUT_ENABLE_E: 
           value = old_value | LEFT_LINE_OUT_DRIVER_MASK; 
           break;
        case LEFT_LINE_OUT_DISABLE_E: 
           value = old_value & ~ LEFT_LINE_OUT_DRIVER_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_line_out_driver
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 3:3
 *
 **************************************************************************/
unsigned char get_left_line_out_driver()
  {
    unsigned char value;


    /* 
     * 0: Left Line-Out driver is disabled. 
     * 1: Left Line-Out driver is enabled 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & LEFT_LINE_OUT_DRIVER_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_line_out_input
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 4:4
 *
 **************************************************************************/
unsigned char set_right_line_out_input( enum right_line_input param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(POWER_UP_HEAD_SET_REG);

    /* 
     * 0: Right Line-Out driver input is Ear-DAC. 
     * 1: Right Line-Out driver input is HsR-DAC. 
     */ 
    switch( param ){
        case RIGTH_LINEOUT_INPUT_HEADSET_DAC_E: 
           value = old_value | RIGHT_LINE_OUT_INPUT_MASK; 
           break;
        case RIGTH_LINEOUT_INPUT_EAD_DAC_E: 
           value = old_value & ~ RIGHT_LINE_OUT_INPUT_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_line_out_input
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 4:4
 *
 **************************************************************************/
unsigned char get_right_line_out_input()
  {
    unsigned char value;


    /* 
     * 0: Right Line-Out driver input is Ear-DAC. 
     * 1: Right Line-Out driver input is HsR-DAC. 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & RIGHT_LINE_OUT_INPUT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_line_out_input
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 5:5
 *
 **************************************************************************/
unsigned char set_left_line_out_input( enum left_line_input param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(POWER_UP_HEAD_SET_REG);

    /* 
     * 0: Left Line-Out driver input is Ear-DAC. 
     * 1: Left Line-Out driver input is HsL-DAC. 
     */ 
    switch( param ){
        case LEFT_LINEOUT_INPUT_HEADSET_DAC_E: 
           value = old_value | LEFT_LINE_OUT_INPUT_MASK; 
           break;
        case LEFT_LINEOUT_INPUT_EAD_DAC_E: 
           value = old_value & ~ LEFT_LINE_OUT_INPUT_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_line_out_input
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 5:5
 *
 **************************************************************************/
unsigned char get_left_line_out_input()
  {
    unsigned char value;


    /* 
     * 0: Left Line-Out driver input is Ear-DAC. 
     * 1: Left Line-Out driver input is HsL-DAC. 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & LEFT_LINE_OUT_INPUT_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charge_pump
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 7:7
 *
 **************************************************************************/
unsigned char set_charge_pump( enum charge_pump_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(POWER_UP_HEAD_SET_REG);

    /* 
     * 0: Charge Pump disabled. If HSAUTOENSEL=1 also HSAUTOEN 
     * must me resetted. 
     * 1: Charge Pump enabled. 
     */ 
    switch( param ){
        case CHARGE_PUMP_ENABLE_E: 
           value = old_value | CHARGE_PUMP_MASK; 
           break;
        case CHARGE_PUMP_DISABLE_E: 
           value = old_value & ~ CHARGE_PUMP_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charge_pump
 *
 * RET  : Return the value of register 0xPowerUpHeadSet
 *
 * Notes : From register 0xD0C, bits 7:7
 *
 **************************************************************************/
unsigned char get_charge_pump()
  {
    unsigned char value;


    /* 
     * 0: Charge Pump disabled. If HSAUTOENSEL=1 also HSAUTOEN 
     * must me resetted. 
     * 1: Charge Pump enabled. 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & CHARGE_PUMP_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_threshold
 *
 * IN   : param, a value to write to the regiter EnvelopeControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xEnvelopeControl
 *
 * Notes : From register 0xD0D, bits 3:0
 *
 **************************************************************************/
unsigned char set_low_threshold( enum threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ENVELOPE_CONTROL_REG);

    /* 
     * Low thresold for Charge Pump Low Voltage selection 
     */ 
  
    value =  old_value & ~LOW_THRESHOLD_MASK;


    value |=  param ;  
    I2CWrite(ENVELOPE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_threshold
 *
 * RET  : Return the value of register 0xEnvelopeControl
 *
 * Notes : From register 0xD0D, bits 3:0
 *
 **************************************************************************/
unsigned char get_low_threshold()
  {
    unsigned char value;


    /* 
     * Low thresold for Charge Pump Low Voltage selection 
     */ 
    value = (I2CRead(ENVELOPE_CONTROL_REG) & LOW_THRESHOLD_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_high_threshold
 *
 * IN   : param, a value to write to the regiter EnvelopeControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xEnvelopeControl
 *
 * Notes : From register 0xD0D, bits 7:4
 *
 **************************************************************************/
unsigned char set_high_threshold( enum threshold param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ENVELOPE_CONTROL_REG);

    /* 
     * High thresold for Charge Pump Low Voltage selection 
     */ 
  
    value =  old_value & ~HIGH_THRESHOLD_MASK;


    value |= ( param << 0x4);  
    I2CWrite(ENVELOPE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_high_threshold
 *
 * RET  : Return the value of register 0xEnvelopeControl
 *
 * Notes : From register 0xD0D, bits 7:4
 *
 **************************************************************************/
unsigned char get_high_threshold()
  {
    unsigned char value;


    /* 
     * High thresold for Charge Pump Low Voltage selection 
     */ 
    value = (I2CRead(ENVELOPE_CONTROL_REG) & HIGH_THRESHOLD_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_envelope_decay_time
 *
 * IN   : param, a value to write to the regiter EnvelopeThreshold
 * OUT  : 
 *
 * RET  : Return the value of register 0xEnvelopeThreshold
 *
 * Notes : From register 0xD0E, bits 3:0
 *
 **************************************************************************/
unsigned char set_envelope_decay_time( enum decay_time param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ENVELOPE_THRESHOLD_REG);

    /* 
     * Low thresold for Charge Pump Low Voltage selection 
     */ 
    I2CWrite(ENVELOPE_THRESHOLD_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_envelope_decay_time
 *
 * RET  : Return the value of register 0xEnvelopeThreshold
 *
 * Notes : From register 0xD0E, bits 3:0
 *
 **************************************************************************/
unsigned char get_envelope_decay_time()
  {
    unsigned char value;


    /* 
     * Low thresold for Charge Pump Low Voltage selection 
     */ 
    value = (I2CRead(ENVELOPE_THRESHOLD_REG) & ENVELOPE_DECAY_TIME_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_envelope_detection_c_p
 *
 * IN   : param, a value to write to the regiter EnvelopeThreshold
 * OUT  : 
 *
 * RET  : Return the value of register 0xEnvelopeThreshold
 *
 * Notes : From register 0xD0E, bits 4:4
 *
 **************************************************************************/
unsigned char set_envelope_detection_c_p( enum envelope_detection_c_p_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ENVELOPE_THRESHOLD_REG);

    /* 
     * Envelope detection for charge pump 
     */ 
    switch( param ){
        case ENVELOPE_DETECTION_CP_ENABLE_E: 
           value = old_value | ENVELOPE_DETECTION_CP_MASK; 
           break;
        case ENVELOPE_DETECTION_CP_DISABLE_E: 
           value = old_value & ~ ENVELOPE_DETECTION_CP_MASK;
           break;
    }
  

    I2CWrite(ENVELOPE_THRESHOLD_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_envelope_detection_c_p
 *
 * RET  : Return the value of register 0xEnvelopeThreshold
 *
 * Notes : From register 0xD0E, bits 4:4
 *
 **************************************************************************/
unsigned char get_envelope_detection_c_p()
  {
    unsigned char value;


    /* 
     * Envelope detection for charge pump 
     */ 
    value = (I2CRead(ENVELOPE_THRESHOLD_REG) & ENVELOPE_DETECTION_CP_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_input_voltage_control
 *
 * IN   : param, a value to write to the regiter EnvelopeThreshold
 * OUT  : 
 *
 * RET  : Return the value of register 0xEnvelopeThreshold
 *
 * Notes : From register 0xD0E, bits 5:5
 *
 **************************************************************************/
unsigned char set_input_voltage_control( enum input_voltage_control param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ENVELOPE_THRESHOLD_REG);

    /* 
     * Input voltage control for Charge Pump. Active just when 
     * envelope detection is disabled. 
     * 0: VinVcphs ball supplies negative charge Pump. 
     * 1: SmpsVcphs ball supplies negative charge Pump. 
     */ 
    switch( param ){
        case INPUT_HIGH_VOLTAGE_E: 
           value = old_value | INPUT_VOLTAGE_CONTROL_MASK; 
           break;
        case INPUT_LOW_VOLTAGE_E: 
           value = old_value & ~ INPUT_VOLTAGE_CONTROL_MASK;
           break;
    }
  

    I2CWrite(ENVELOPE_THRESHOLD_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_input_voltage_control
 *
 * RET  : Return the value of register 0xEnvelopeThreshold
 *
 * Notes : From register 0xD0E, bits 5:5
 *
 **************************************************************************/
unsigned char get_input_voltage_control()
  {
    unsigned char value;


    /* 
     * Input voltage control for Charge Pump. Active just when 
     * envelope detection is disabled. 
     * 0: VinVcphs ball supplies negative charge Pump. 
     * 1: SmpsVcphs ball supplies negative charge Pump. 
     */ 
    value = (I2CRead(ENVELOPE_THRESHOLD_REG) & INPUT_VOLTAGE_CONTROL_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_generator_pos
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register 0xConfigurationClassDDrivers
 *
 * Notes : From register 0xD0F, bits 2:2
 *
 **************************************************************************/
unsigned char set_pwm_generator_pos( enum pwm_generator_positive param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * When PWM generator is enable positive is started 
     * separatetly from negative (used in differential) 
     */ 
    switch( param ){
        case PWM_GENERATOR_POSITIVE_DUTYCYCLE_E: 
           value = old_value | PWM_GENERATOR_POS_MASK; 
           break;
        case PWM_POSITIVE_GENERATOR_POL_E: 
           value = old_value & ~ PWM_GENERATOR_POS_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_generator_pos
 *
 * RET  : Return the value of register 0xConfigurationClassDDrivers
 *
 * Notes : From register 0xD0F, bits 2:2
 *
 **************************************************************************/
unsigned char get_pwm_generator_pos()
  {
    unsigned char value;


    /* 
     * When PWM generator is enable positive is started 
     * separatetly from negative (used in differential) 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & PWM_GENERATOR_POS_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_generator_neg
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register 0xConfigurationClassDDrivers
 *
 * Notes : From register 0xD0F, bits 3:3
 *
 **************************************************************************/
unsigned char set_pwm_generator_neg( enum pwm_generator_negative param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * When PWM generator is enable negative is started 
     * separatetly from positive (not used in differential) 
     */ 
    switch( param ){
        case PWM_GENERATOR_NEGATIVE_DUTYCYCLE_E: 
           value = old_value | PWM_GENERATOR_NEG_MASK; 
           break;
        case PWM_NEGATIVE_GENERATOR_POL_E: 
           value = old_value & ~ PWM_GENERATOR_NEG_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_generator_neg
 *
 * RET  : Return the value of register 0xConfigurationClassDDrivers
 *
 * Notes : From register 0xD0F, bits 3:3
 *
 **************************************************************************/
unsigned char get_pwm_generator_neg()
  {
    unsigned char value;


    /* 
     * When PWM generator is enable negative is started 
     * separatetly from positive (not used in differential) 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & PWM_GENERATOR_NEG_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm_generator_control
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register 0xConfigurationClassDDrivers
 *
 * Notes : From register 0xD0F, bits 5:5
 *
 **************************************************************************/
unsigned char set_pwm_generator_control( enum pwm_started_together param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * When PWM generator is enable positive and negative are 
     * started together (used in differential) 
     */ 
    switch( param ){
        case PWM_DUTY_CYCLE_E: 
           value = old_value | PWM_GENERATOR_CONTROL_MASK; 
           break;
        case PWM_VALUE_E: 
           value = old_value & ~ PWM_GENERATOR_CONTROL_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm_generator_control
 *
 * RET  : Return the value of register 0xConfigurationClassDDrivers
 *
 * Notes : From register 0xD0F, bits 5:5
 *
 **************************************************************************/
unsigned char get_pwm_generator_control()
  {
    unsigned char value;


    /* 
     * When PWM generator is enable positive and negative are 
     * started together (used in differential) 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & PWM_GENERATOR_CONTROL_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib_controlled_internal
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register 0xConfigurationClassDDrivers
 *
 * Notes : From register 0xD0F, bits 7:7
 *
 **************************************************************************/
unsigned char set_vib_controlled_internal( enum vib1 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * 0: VIB driver is connected to corresponding DA path, 
     * 1: VIB driver is controlled by internal PWM Generator 
     */ 
    switch( param ){
        case VIB_DRIVER_CONNECTED_INTERNAL_GENERATOR_E: 
           value = old_value | VIB_CONTROLLED_INTERNAL_MASK; 
           break;
        case VIB_DRIVER_CONNECTED_DA_PATH_E: 
           value = old_value & ~ VIB_CONTROLLED_INTERNAL_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib_controlled_internal
 *
 * RET  : Return the value of register 0xConfigurationClassDDrivers
 *
 * Notes : From register 0xD0F, bits 7:7
 *
 **************************************************************************/
unsigned char get_vib_controlled_internal()
  {
    unsigned char value;


    /* 
     * 0: VIB driver is connected to corresponding DA path, 
     * 1: VIB driver is controlled by internal PWM Generator 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & VIB_CONTROLLED_INTERNAL_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib1_positive_duty_cyle
 *
 * IN   : param, a value to write to the regiter ConfigurationPositiveOutputVib1
 * OUT  : 
 *
 * RET  : Return the value of register 0xConfigurationPositiveOutputVib1
 *
 * Notes : From register 0xD10, bits 6:0
 *
 **************************************************************************/
unsigned char set_vib1_positive_duty_cyle( enum duty_cycle param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG);

    /* 
     * PWMPLDUTYCYCLE(6:0) 
     */ 
  
    value =  old_value & ~VIB_1_POSITIVE_DUTY_CYLE_MASK;


    value |=  param ;  
    I2CWrite(CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib1_positive_duty_cyle
 *
 * RET  : Return the value of register 0xConfigurationPositiveOutputVib1
 *
 * Notes : From register 0xD10, bits 6:0
 *
 **************************************************************************/
unsigned char get_vib1_positive_duty_cyle()
  {
    unsigned char value;


    /* 
     * PWMPLDUTYCYCLE(6:0) 
     */ 
    value = (I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG) & VIB_1_POSITIVE_DUTY_CYLE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib1_positive_forced_to_voltage
 *
 * IN   : param, a value to write to the regiter ConfigurationPositiveOutputVib1
 * OUT  : 
 *
 * RET  : Return the value of register 0xConfigurationPositiveOutputVib1
 *
 * Notes : From register 0xD10, bits 7:7
 *
 **************************************************************************/
unsigned char set_vib1_positive_forced_to_voltage( enum forced_to_voltage param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG);

    /* 
     * 0: Vib1p pin is forced to GndVib voltage, 
     * 1: Vib1p pin is forced to VinVib voltage 
     */ 
    switch( param ){
        case VIN_VOLTAGE_E: 
           value = old_value | VIB_1_POSITIVE_FORCED_TO_VOLTAGE_MASK; 
           break;
        case GND_VOLTAGE_E: 
           value = old_value & ~ VIB_1_POSITIVE_FORCED_TO_VOLTAGE_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib1_positive_forced_to_voltage
 *
 * RET  : Return the value of register 0xConfigurationPositiveOutputVib1
 *
 * Notes : From register 0xD10, bits 7:7
 *
 **************************************************************************/
unsigned char get_vib1_positive_forced_to_voltage()
  {
    unsigned char value;


    /* 
     * 0: Vib1p pin is forced to GndVib voltage, 
     * 1: Vib1p pin is forced to VinVib voltage 
     */ 
    value = (I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG) & VIB_1_POSITIVE_FORCED_TO_VOLTAGE_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib1_negative_duty_cyle
 *
 * IN   : param, a value to write to the regiter ConfigurationNegativeOutputVib1
 * OUT  : 
 *
 * RET  : Return the value of register 0xConfigurationNegativeOutputVib1
 *
 * Notes : From register 0xD11, bits 6:0
 *
 **************************************************************************/
unsigned char set_vib1_negative_duty_cyle( enum duty_cycle param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG);

    /* 
     * PWMPLDUTYCYCLE(6:0) 
     */ 
  
    value =  old_value & ~VIB_1_NEGATIVE_DUTY_CYLE_MASK;


    value |=  param ;  
    I2CWrite(CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib1_negative_duty_cyle
 *
 * RET  : Return the value of register 0xConfigurationNegativeOutputVib1
 *
 * Notes : From register 0xD11, bits 6:0
 *
 **************************************************************************/
unsigned char get_vib1_negative_duty_cyle()
  {
    unsigned char value;


    /* 
     * PWMPLDUTYCYCLE(6:0) 
     */ 
    value = (I2CRead(CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG) & VIB_1_NEGATIVE_DUTY_CYLE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib_negative_pin_forced
 *
 * IN   : param, a value to write to the regiter ConfigurationNegativeOutputVib1
 * OUT  : 
 *
 * RET  : Return the value of register 0xConfigurationNegativeOutputVib1
 *
 * Notes : From register 0xD11, bits 7:7
 *
 **************************************************************************/
unsigned char set_vib_negative_pin_forced( enum forced_to_voltage param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG);

    /* 
     * 0: Vibn pin is forced to GndVib voltage, 
     * 1: Vibn pin is forced to VinVib voltage 
     */ 
    switch( param ){
        case VIN_VOLTAGE_E: 
           value = old_value | VIB_NEGATIVE_PIN_FORCED_MASK; 
           break;
        case GND_VOLTAGE_E: 
           value = old_value & ~ VIB_NEGATIVE_PIN_FORCED_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib_negative_pin_forced
 *
 * RET  : Return the value of register 0xConfigurationNegativeOutputVib1
 *
 * Notes : From register 0xD11, bits 7:7
 *
 **************************************************************************/
unsigned char get_vib_negative_pin_forced()
  {
    unsigned char value;


    /* 
     * 0: Vibn pin is forced to GndVib voltage, 
     * 1: Vibn pin is forced to VinVib voltage 
     */ 
    value = (I2CRead(CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG) & VIB_NEGATIVE_PIN_FORCED_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib2_positive_duty_cyle
 *
 * IN   : param, a value to write to the regiter ConfigurationPositiveOutputVib2
 * OUT  : 
 *
 * RET  : Return the value of register 0xConfigurationPositiveOutputVib2
 *
 * Notes : From register 0xD12, bits 6:0
 *
 **************************************************************************/
unsigned char set_vib2_positive_duty_cyle( enum duty_cycle param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_2_REG);

    /* 
     * PWMNRDUTYCYCLE(6:0) 
     */ 
  
    value =  old_value & ~VIB_2_POSITIVE_DUTY_CYLE_MASK;


    value |=  param ;  
    I2CWrite(CONFIGURATION_POSITIVE_OUTPUT_VIB_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib2_positive_duty_cyle
 *
 * RET  : Return the value of register 0xConfigurationPositiveOutputVib2
 *
 * Notes : From register 0xD12, bits 6:0
 *
 **************************************************************************/
unsigned char get_vib2_positive_duty_cyle()
  {
    unsigned char value;


    /* 
     * PWMNRDUTYCYCLE(6:0) 
     */ 
    value = (I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_2_REG) & VIB_2_POSITIVE_DUTY_CYLE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib2_positive_forced_to_voltage
 *
 * IN   : param, a value to write to the regiter ConfigurationPositiveOutputVib2
 * OUT  : 
 *
 * RET  : Return the value of register 0xConfigurationPositiveOutputVib2
 *
 * Notes : From register 0xD12, bits 7:7
 *
 **************************************************************************/
unsigned char set_vib2_positive_forced_to_voltage( enum forced_to_voltage param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_2_REG);

    /* 
     * 0: Vib2p pin is forced to GndVib voltage, 
     * 1: Vib2p pin is forced to VinVib voltage 
     */ 
    switch( param ){
        case VIN_VOLTAGE_E: 
           value = old_value | VIB_2_POSITIVE_FORCED_TO_VOLTAGE_MASK; 
           break;
        case GND_VOLTAGE_E: 
           value = old_value & ~ VIB_2_POSITIVE_FORCED_TO_VOLTAGE_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_POSITIVE_OUTPUT_VIB_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib2_positive_forced_to_voltage
 *
 * RET  : Return the value of register 0xConfigurationPositiveOutputVib2
 *
 * Notes : From register 0xD12, bits 7:7
 *
 **************************************************************************/
unsigned char get_vib2_positive_forced_to_voltage()
  {
    unsigned char value;


    /* 
     * 0: Vib2p pin is forced to GndVib voltage, 
     * 1: Vib2p pin is forced to VinVib voltage 
     */ 
    value = (I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_2_REG) & VIB_2_POSITIVE_FORCED_TO_VOLTAGE_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_mic1_gain
 *
 * IN   : param, a value to write to the regiter AnalogMic1Gain
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogMic1Gain
 *
 * Notes : From register 0xD14, bits 4:0
 *
 **************************************************************************/
unsigned char set_mic1_gain( enum analog_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_MIC_1_GAIN_REG);

    /* 
     * Microphone 1 channel Analog Gain 
     */ 
  
    value =  old_value & ~MIC_1_GAIN_MASK;


    value |=  param ;  
    I2CWrite(ANALOG_MIC_1_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mic1_gain
 *
 * RET  : Return the value of register 0xAnalogMic1Gain
 *
 * Notes : From register 0xD14, bits 4:0
 *
 **************************************************************************/
unsigned char get_mic1_gain()
  {
    unsigned char value;


    /* 
     * Microphone 1 channel Analog Gain 
     */ 
    value = (I2CRead(ANALOG_MIC_1_GAIN_REG) & MIC_1_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_micro1
 *
 * IN   : param, a value to write to the regiter AnalogMic1Gain
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogMic1Gain
 *
 * Notes : From register 0xD14, bits 6:6
 *
 **************************************************************************/
unsigned char set_low_power_micro1( enum low_power_micro_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_MIC_1_GAIN_REG);

    /* 
     * New in cut 1.0 Low Power for Micro 1 
     */ 
    switch( param ){
        case LOW_POWER_UP_E: 
           value = old_value | LOW_POWER_MICRO_1_MASK; 
           break;
        case LOW_POWER_DOWN_E: 
           value = old_value & ~ LOW_POWER_MICRO_1_MASK;
           break;
    }
  

    I2CWrite(ANALOG_MIC_1_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_power_micro1
 *
 * RET  : Return the value of register 0xAnalogMic1Gain
 *
 * Notes : From register 0xD14, bits 6:6
 *
 **************************************************************************/
unsigned char get_low_power_micro1()
  {
    unsigned char value;


    /* 
     * New in cut 1.0 Low Power for Micro 1 
     */ 
    value = (I2CRead(ANALOG_MIC_1_GAIN_REG) & LOW_POWER_MICRO_1_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_mic1_single_ended_conf
 *
 * IN   : param, a value to write to the regiter AnalogMic1Gain
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogMic1Gain
 *
 * Notes : From register 0xD14, bits 7:7
 *
 **************************************************************************/
unsigned char set_mic1_single_ended_conf( enum single_or_differential param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_MIC_1_GAIN_REG);

    /* 
     * 0: DIfferential microphone is connected to MIC1 inputs , 
     * 1: Single ended microphone is connected to MIC1 inputs 
     */ 
    switch( param ){
        case SINGLE_ENDED_E: 
           value = old_value | MIC_1_SINGLE_ENDED_CONF_MASK; 
           break;
        case DIFFERENTIAL_E: 
           value = old_value & ~ MIC_1_SINGLE_ENDED_CONF_MASK;
           break;
    }
  

    I2CWrite(ANALOG_MIC_1_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mic1_single_ended_conf
 *
 * RET  : Return the value of register 0xAnalogMic1Gain
 *
 * Notes : From register 0xD14, bits 7:7
 *
 **************************************************************************/
unsigned char get_mic1_single_ended_conf()
  {
    unsigned char value;


    /* 
     * 0: DIfferential microphone is connected to MIC1 inputs , 
     * 1: Single ended microphone is connected to MIC1 inputs 
     */ 
    value = (I2CRead(ANALOG_MIC_1_GAIN_REG) & MIC_1_SINGLE_ENDED_CONF_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_mic2_gain
 *
 * IN   : param, a value to write to the regiter AnalogMic2Gain
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogMic2Gain
 *
 * Notes : From register 0xD15, bits 4:0
 *
 **************************************************************************/
unsigned char set_mic2_gain( enum analog_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_MIC_2_GAIN_REG);

    /* 
     * Microphone 1 channel Analog Gain 
     */ 
  
    value =  old_value & ~MIC_2_GAIN_MASK;


    value |=  param ;  
    I2CWrite(ANALOG_MIC_2_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mic2_gain
 *
 * RET  : Return the value of register 0xAnalogMic2Gain
 *
 * Notes : From register 0xD15, bits 4:0
 *
 **************************************************************************/
unsigned char get_mic2_gain()
  {
    unsigned char value;


    /* 
     * Microphone 1 channel Analog Gain 
     */ 
    value = (I2CRead(ANALOG_MIC_2_GAIN_REG) & MIC_2_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_micro2
 *
 * IN   : param, a value to write to the regiter AnalogMic2Gain
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogMic2Gain
 *
 * Notes : From register 0xD15, bits 6:6
 *
 **************************************************************************/
unsigned char set_low_power_micro2( enum low_power_micro_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_MIC_2_GAIN_REG);

    /* 
     * New in cut 1.0 Low Power for Micro 2 
     */ 
    switch( param ){
        case LOW_POWER_UP_E: 
           value = old_value | LOW_POWER_MICRO_2_MASK; 
           break;
        case LOW_POWER_DOWN_E: 
           value = old_value & ~ LOW_POWER_MICRO_2_MASK;
           break;
    }
  

    I2CWrite(ANALOG_MIC_2_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_power_micro2
 *
 * RET  : Return the value of register 0xAnalogMic2Gain
 *
 * Notes : From register 0xD15, bits 6:6
 *
 **************************************************************************/
unsigned char get_low_power_micro2()
  {
    unsigned char value;


    /* 
     * New in cut 1.0 Low Power for Micro 2 
     */ 
    value = (I2CRead(ANALOG_MIC_2_GAIN_REG) & LOW_POWER_MICRO_2_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_mic2_single_ended_conf
 *
 * IN   : param, a value to write to the regiter AnalogMic2Gain
 * OUT  : 
 *
 * RET  : Return the value of register 0xAnalogMic2Gain
 *
 * Notes : From register 0xD15, bits 7:7
 *
 **************************************************************************/
unsigned char set_mic2_single_ended_conf( enum single_or_differential param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_MIC_2_GAIN_REG);

    /* 
     * 0: Differential microphone is connected to MIC2 inputs , 
     * 1: Single ended microphone is connected to MIC2 inputs 
     */ 
    switch( param ){
        case SINGLE_ENDED_E: 
           value = old_value | MIC_2_SINGLE_ENDED_CONF_MASK; 
           break;
        case DIFFERENTIAL_E: 
           value = old_value & ~ MIC_2_SINGLE_ENDED_CONF_MASK;
           break;
    }
  

    I2CWrite(ANALOG_MIC_2_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mic2_single_ended_conf
 *
 * RET  : Return the value of register 0xAnalogMic2Gain
 *
 * Notes : From register 0xD15, bits 7:7
 *
 **************************************************************************/
unsigned char get_mic2_single_ended_conf()
  {
    unsigned char value;


    /* 
     * 0: Differential microphone is connected to MIC2 inputs , 
     * 1: Single ended microphone is connected to MIC2 inputs 
     */ 
    value = (I2CRead(ANALOG_MIC_2_GAIN_REG) & MIC_2_SINGLE_ENDED_CONF_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_headset_analog_gain
 *
 * IN   : param, a value to write to the regiter LeftLineInGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xLeftLineInGain
 *
 * Notes : From register 0xD16, bits 3:0
 *
 **************************************************************************/
unsigned char set_right_headset_analog_gain( enum headset_analog_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LEFT_LINE_IN_GAIN_REG);

    /* 
     * Right headset analog gain 
     */ 
  
    value =  old_value & ~RIGHT_HEADSET_ANALOG_GAIN_MASK;


    value |=  param ;  
    I2CWrite(LEFT_LINE_IN_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_headset_analog_gain
 *
 * RET  : Return the value of register 0xLeftLineInGain
 *
 * Notes : From register 0xD16, bits 3:0
 *
 **************************************************************************/
unsigned char get_right_headset_analog_gain()
  {
    unsigned char value;


    /* 
     * Right headset analog gain 
     */ 
    value = (I2CRead(LEFT_LINE_IN_GAIN_REG) & RIGHT_HEADSET_ANALOG_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_headset_analog_gain
 *
 * IN   : param, a value to write to the regiter LeftLineInGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xLeftLineInGain
 *
 * Notes : From register 0xD16, bits 7:4
 *
 **************************************************************************/
unsigned char set_left_headset_analog_gain( enum headset_analog_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LEFT_LINE_IN_GAIN_REG);

    /* 
     * Left headset analog gain 
     */ 
  
    value =  old_value & ~LEFT_HEADSET_ANALOG_GAIN_MASK;


    value |= ( param << 0x4);  
    I2CWrite(LEFT_LINE_IN_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_headset_analog_gain
 *
 * RET  : Return the value of register 0xLeftLineInGain
 *
 * Notes : From register 0xD16, bits 7:4
 *
 **************************************************************************/
unsigned char get_left_headset_analog_gain()
  {
    unsigned char value;


    /* 
     * Left headset analog gain 
     */ 
    value = (I2CRead(LEFT_LINE_IN_GAIN_REG) & LEFT_HEADSET_ANALOG_GAIN_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_line_analog_gain
 *
 * IN   : param, a value to write to the regiter RightLineInGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xRightLineInGain
 *
 * Notes : From register 0xD17, bits 3:0
 *
 **************************************************************************/
unsigned char set_right_line_analog_gain( enum line_in_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RIGHT_LINE_IN_GAIN_REG);

    /* 
     * Gain for Right Line-In 
     */ 
  
    value =  old_value & ~RIGHT_LINE_ANALOG_GAIN_MASK;


    value |=  param ;  
    I2CWrite(RIGHT_LINE_IN_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_line_analog_gain
 *
 * RET  : Return the value of register 0xRightLineInGain
 *
 * Notes : From register 0xD17, bits 3:0
 *
 **************************************************************************/
unsigned char get_right_line_analog_gain()
  {
    unsigned char value;


    /* 
     * Gain for Right Line-In 
     */ 
    value = (I2CRead(RIGHT_LINE_IN_GAIN_REG) & RIGHT_LINE_ANALOG_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_line_analog_gain
 *
 * IN   : param, a value to write to the regiter RightLineInGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xRightLineInGain
 *
 * Notes : From register 0xD17, bits 7:4
 *
 **************************************************************************/
unsigned char set_left_line_analog_gain( enum line_in_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RIGHT_LINE_IN_GAIN_REG);

    /* 
     * Gain for Left Line-In 
     */ 
  
    value =  old_value & ~LEFT_LINE_ANALOG_GAIN_MASK;


    value |= ( param << 0x4);  
    I2CWrite(RIGHT_LINE_IN_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_line_analog_gain
 *
 * RET  : Return the value of register 0xRightLineInGain
 *
 * Notes : From register 0xD17, bits 7:4
 *
 **************************************************************************/
unsigned char get_left_line_analog_gain()
  {
    unsigned char value;


    /* 
     * Gain for Left Line-In 
     */ 
    value = (I2CRead(RIGHT_LINE_IN_GAIN_REG) & LEFT_LINE_ANALOG_GAIN_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_line_in_to_headset_gain
 *
 * IN   : param, a value to write to the regiter LeftLineInToHeadsetGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xLeftLineInToHeadsetGain
 *
 * Notes : From register 0xD18, bits 4:0
 *
 **************************************************************************/
unsigned char set_left_line_in_to_headset_gain( enum line_in_to_head_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(LEFT_LINE_IN_TO_HEADSET_GAIN_REG);

    /* 
     * Gain for Line-In Left to Headset loop 
     */ 
  
    value =  old_value & ~LEFT_LINE_IN_TO_HEADSET_GAIN_MASK;


    value |=  param ;  
    I2CWrite(LEFT_LINE_IN_TO_HEADSET_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_line_in_to_headset_gain
 *
 * RET  : Return the value of register 0xLeftLineInToHeadsetGain
 *
 * Notes : From register 0xD18, bits 4:0
 *
 **************************************************************************/
unsigned char get_left_line_in_to_headset_gain()
  {
    unsigned char value;


    /* 
     * Gain for Line-In Left to Headset loop 
     */ 
    value = (I2CRead(LEFT_LINE_IN_TO_HEADSET_GAIN_REG) & LEFT_LINE_IN_TO_HEADSET_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_line_in_to_headset_gain
 *
 * IN   : param, a value to write to the regiter RightLineInToHeadsetGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xRightLineInToHeadsetGain
 *
 * Notes : From register 0xD19, bits 4:0
 *
 **************************************************************************/
unsigned char set_right_line_in_to_headset_gain( enum line_in_to_head_set param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(RIGHT_LINE_IN_TO_HEADSET_GAIN_REG);

    /* 
     * Gain for Line-In Right to Headset loop 
     */ 
  
    value =  old_value & ~RIGHT_LINE_IN_TO_HEADSET_GAIN_MASK;


    value |=  param ;  
    I2CWrite(RIGHT_LINE_IN_TO_HEADSET_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_line_in_to_headset_gain
 *
 * RET  : Return the value of register 0xRightLineInToHeadsetGain
 *
 * Notes : From register 0xD19, bits 4:0
 *
 **************************************************************************/
unsigned char get_right_line_in_to_headset_gain()
  {
    unsigned char value;


    /* 
     * Gain for Line-In Right to Headset loop 
     */ 
    value = (I2CRead(RIGHT_LINE_IN_TO_HEADSET_GAIN_REG) & RIGHT_LINE_IN_TO_HEADSET_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad4_voice_filter
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 0:0
 *
 **************************************************************************/
unsigned char set_ad4_voice_filter( enum ad4_voice_filter_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_VOICE_FILTERS_REG);

    /* 
     * 0: AD4 channel path use Audio Filters 
     * 1: AD4 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    switch( param ){
        case AD_4_VOICE_FILTER_ENABLE_E: 
           value = old_value | AD_4_VOICE_FILTER_MASK; 
           break;
        case AD_4_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ AD_4_VOICE_FILTER_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad4_voice_filter
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 0:0
 *
 **************************************************************************/
unsigned char get_ad4_voice_filter()
  {
    unsigned char value;


    /* 
     * 0: AD4 channel path use Audio Filters 
     * 1: AD4 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_4_VOICE_FILTER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad3_voice_filter
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 1:1
 *
 **************************************************************************/
unsigned char set_ad3_voice_filter( enum ad3_voice_filter_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_VOICE_FILTERS_REG);

    /* 
     * 0: AD3 channel path use Audio Filters 
     * 1: AD3 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    switch( param ){
        case AD_3_VOICE_FILTER_ENABLE_E: 
           value = old_value | AD_3_VOICE_FILTER_MASK; 
           break;
        case AD_3_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ AD_3_VOICE_FILTER_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad3_voice_filter
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 1:1
 *
 **************************************************************************/
unsigned char get_ad3_voice_filter()
  {
    unsigned char value;


    /* 
     * 0: AD3 channel path use Audio Filters 
     * 1: AD3 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_3_VOICE_FILTER_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_voice_filter
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 2:2
 *
 **************************************************************************/
unsigned char set_ad2_voice_filter( enum ad2_voice_filter_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_VOICE_FILTERS_REG);

    /* 
     * 0: AD2 channel path use Audio Filters 
     * 1: AD2 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    switch( param ){
        case AD_2_VOICE_FILTER_ENABLE_E: 
           value = old_value | AD_2_VOICE_FILTER_MASK; 
           break;
        case AD_2_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ AD_2_VOICE_FILTER_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_voice_filter
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 2:2
 *
 **************************************************************************/
unsigned char get_ad2_voice_filter()
  {
    unsigned char value;


    /* 
     * 0: AD2 channel path use Audio Filters 
     * 1: AD2 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_2_VOICE_FILTER_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_voice_filter
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 3:3
 *
 **************************************************************************/
unsigned char set_ad1_voice_filter( enum ad1_voice_filter_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_VOICE_FILTERS_REG);

    /* 
     * 0: AD1 channel path use Audio Filters 
     * 1: AD1 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    switch( param ){
        case AD_1_VOICE_FILTER_ENABLE_E: 
           value = old_value | AD_1_VOICE_FILTER_MASK; 
           break;
        case AD_1_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ AD_1_VOICE_FILTER_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_voice_filter
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 3:3
 *
 **************************************************************************/
unsigned char get_ad1_voice_filter()
  {
    unsigned char value;


    /* 
     * 0: AD1 channel path use Audio Filters 
     * 1: AD1 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_1_VOICE_FILTER_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad4_offset_cancel
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 4:4
 *
 **************************************************************************/
unsigned char set_ad4_offset_cancel( enum ad4_offset_cancel param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_VOICE_FILTERS_REG);

    /* 
     * 0: AD4 Path with audio Offset Cancellation filter enabled 
     * 1: AD4 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    switch( param ){
        case AD4_OFFSET_ENABLE_E: 
           value = old_value | AD_4_OFFSET_CANCEL_MASK; 
           break;
        case AD4_OFFSET_CANCEL_E: 
           value = old_value & ~ AD_4_OFFSET_CANCEL_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad4_offset_cancel
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 4:4
 *
 **************************************************************************/
unsigned char get_ad4_offset_cancel()
  {
    unsigned char value;


    /* 
     * 0: AD4 Path with audio Offset Cancellation filter enabled 
     * 1: AD4 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_4_OFFSET_CANCEL_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad3_offset_cancel
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 5:5
 *
 **************************************************************************/
unsigned char set_ad3_offset_cancel( enum ad3_offset_cancel param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_VOICE_FILTERS_REG);

    /* 
     * 0: AD3 Path with audio Offset Cancellation filter enabled 
     * 1: AD3 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    switch( param ){
        case AD3_OFFSET_ENABLE_E: 
           value = old_value | AD_3_OFFSET_CANCEL_MASK; 
           break;
        case AD3_OFFSET_CANCEL_E: 
           value = old_value & ~ AD_3_OFFSET_CANCEL_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad3_offset_cancel
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 5:5
 *
 **************************************************************************/
unsigned char get_ad3_offset_cancel()
  {
    unsigned char value;


    /* 
     * 0: AD3 Path with audio Offset Cancellation filter enabled 
     * 1: AD3 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_3_OFFSET_CANCEL_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_offset_cancel
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 6:6
 *
 **************************************************************************/
unsigned char set_ad2_offset_cancel( enum ad2_offset_cancel param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_VOICE_FILTERS_REG);

    /* 
     * 0: AD2 Path with audio Offset Cancellation filter enabled 
     * 1: AD2 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    switch( param ){
        case AD2_OFFSET_ENABLE_E: 
           value = old_value | AD_2_OFFSET_CANCEL_MASK; 
           break;
        case AD2_OFFSET_CANCEL_E: 
           value = old_value & ~ AD_2_OFFSET_CANCEL_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_offset_cancel
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad2_offset_cancel()
  {
    unsigned char value;


    /* 
     * 0: AD2 Path with audio Offset Cancellation filter enabled 
     * 1: AD2 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_2_OFFSET_CANCEL_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_offset_cancel
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 7:7
 *
 **************************************************************************/
unsigned char set_ad1_offset_cancel( enum ad1_offset_cancel param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(AUDIO_VOICE_FILTERS_REG);

    /* 
     * 0: AD1 Path with audio Offset Cancellation filter enabled 
     * 1: AD1 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    switch( param ){
        case AD1_OFFSET_ENABLE_E: 
           value = old_value | AD_1_OFFSET_CANCEL_MASK; 
           break;
        case AD1_OFFSET_CANCEL_E: 
           value = old_value & ~ AD_1_OFFSET_CANCEL_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_offset_cancel
 *
 * RET  : Return the value of register 0xAudioVoiceFilters
 *
 * Notes : From register 0xD1A, bits 7:7
 *
 **************************************************************************/
unsigned char get_ad1_offset_cancel()
  {
    unsigned char value;


    /* 
     * 0: AD1 Path with audio Offset Cancellation filter enabled 
     * 1: AD1 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_1_OFFSET_CANCEL_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_bit_clock0
 *
 * IN   : param, a value to write to the regiter TdmInterface
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmInterface
 *
 * Notes : From register 0xD1B, bits 0:0
 *
 **************************************************************************/
unsigned char set_bit_clock0( enum bit_clock0_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_INTERFACE_REG);

    /* 
     * 0: The FSync0 and BitClk0 bits are stopped; 
     * 1: The FSync0 and BitClk0 bits are enabled 
     */ 
    switch( param ){
        case BIT_CLOCK_0_ENABLE_E: 
           value = old_value | BIT_CLOCK_0_MASK; 
           break;
        case BIT_CLOCK_0_DISABLE_E: 
           value = old_value & ~ BIT_CLOCK_0_MASK;
           break;
    }
  

    I2CWrite(TDM_INTERFACE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_bit_clock0
 *
 * RET  : Return the value of register 0xTdmInterface
 *
 * Notes : From register 0xD1B, bits 0:0
 *
 **************************************************************************/
unsigned char get_bit_clock0()
  {
    unsigned char value;


    /* 
     * 0: The FSync0 and BitClk0 bits are stopped; 
     * 1: The FSync0 and BitClk0 bits are enabled 
     */ 
    value = (I2CRead(TDM_INTERFACE_REG) & BIT_CLOCK_0_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_bit_clock0_setting
 *
 * IN   : param, a value to write to the regiter TdmInterface
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmInterface
 *
 * Notes : From register 0xD1B, bits 2:1
 *
 **************************************************************************/
unsigned char set_bit_clock0_setting( enum tdm_bit_clock param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_INTERFACE_REG);

    /* 
     * BitClk0 = 48kHz x N; 00: N = 32 ; 01: N = 64 ; 10: N = 
     * 128; 11: N = 256 
     */ 
  

     value =  old_value & ~BIT_CLOCK_0_SETTING_MASK ;

    switch(  param ){

           case CLK_48KHZ_32_E:
                value  = value  | (CLK_48KHZ_32 << 0x1);
                break;
           case CLK_48KHZ_64_E:
                value  = value  | (CLK_48KHZ_64 << 0x1);
                break;
           case CLK_48KHZ_128_E:
                value  = value  | (CLK_48KHZ_128 << 0x1);
                break;
           case CLK_48KHZ_256_E:
                value  = value  | (CLK_48KHZ_256 << 0x1);
                break;
    }
  

    I2CWrite(TDM_INTERFACE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_bit_clock0_setting
 *
 * RET  : Return the value of register 0xTdmInterface
 *
 * Notes : From register 0xD1B, bits 2:1
 *
 **************************************************************************/
unsigned char get_bit_clock0_setting()
  {
    unsigned char value;


    /* 
     * BitClk0 = 48kHz x N; 00: N = 32 ; 01: N = 64 ; 10: N = 
     * 128; 11: N = 256 
     */ 
    value = (I2CRead(TDM_INTERFACE_REG) & BIT_CLOCK_0_SETTING_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_bit_clock1
 *
 * IN   : param, a value to write to the regiter TdmInterface
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmInterface
 *
 * Notes : From register 0xD1B, bits 4:4
 *
 **************************************************************************/
unsigned char set_bit_clock1( enum bit_clock1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_INTERFACE_REG);

    /* 
     * 0: The FSync1 and BitClk1 bits are stopped; 
     * 1: The FSync1 and BitClk1 bits are enabled 
     */ 
    switch( param ){
        case BIT_CLOCK_1_ENABLE_E: 
           value = old_value | BIT_CLOCK_1_MASK; 
           break;
        case BIT_CLOCK_1_DISABLE_E: 
           value = old_value & ~ BIT_CLOCK_1_MASK;
           break;
    }
  

    I2CWrite(TDM_INTERFACE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_bit_clock1
 *
 * RET  : Return the value of register 0xTdmInterface
 *
 * Notes : From register 0xD1B, bits 4:4
 *
 **************************************************************************/
unsigned char get_bit_clock1()
  {
    unsigned char value;


    /* 
     * 0: The FSync1 and BitClk1 bits are stopped; 
     * 1: The FSync1 and BitClk1 bits are enabled 
     */ 
    value = (I2CRead(TDM_INTERFACE_REG) & BIT_CLOCK_1_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_bit_clock1_setting
 *
 * IN   : param, a value to write to the regiter TdmInterface
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmInterface
 *
 * Notes : From register 0xD1B, bits 6:5
 *
 **************************************************************************/
unsigned char set_bit_clock1_setting( enum tdm_bit_clock param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_INTERFACE_REG);

    /* 
     * BitClk1 = 48kHz x N; 00: N = 32 ; 01: N = 64 ; 10: N = 
     * 128; 11: N = 256 
     */ 
  

     value =  old_value & ~BIT_CLOCK_1_SETTING_MASK ;

    switch(  param ){

           case CLK_48KHZ_32_E:
                value  = value  | (CLK_48KHZ_32 << 0x5);
                break;
           case CLK_48KHZ_64_E:
                value  = value  | (CLK_48KHZ_64 << 0x5);
                break;
           case CLK_48KHZ_128_E:
                value  = value  | (CLK_48KHZ_128 << 0x5);
                break;
           case CLK_48KHZ_256_E:
                value  = value  | (CLK_48KHZ_256 << 0x5);
                break;
    }
  

    I2CWrite(TDM_INTERFACE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_bit_clock1_setting
 *
 * RET  : Return the value of register 0xTdmInterface
 *
 * Notes : From register 0xD1B, bits 6:5
 *
 **************************************************************************/
unsigned char get_bit_clock1_setting()
  {
    unsigned char value;


    /* 
     * BitClk1 = 48kHz x N; 00: N = 32 ; 01: N = 64 ; 10: N = 
     * 128; 11: N = 256 
     */ 
    value = (I2CRead(TDM_INTERFACE_REG) & BIT_CLOCK_1_SETTING_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_master0_mode
 *
 * IN   : param, a value to write to the regiter TdmInterface
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmInterface
 *
 * Notes : From register 0xD1B, bits 7:7
 *
 **************************************************************************/
unsigned char set_master0_mode( enum master0_mode_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_INTERFACE_REG);

    /* 
     * 0: The Master Generator for AUD_IF1/0 is stopped, 
     * 1: The Master Generator for AUD_IF1/0 is active 
     */ 
    switch( param ){
        case MASTER_0_MODE_ENABLE_E: 
           value = old_value | MASTER_0_MODE_MASK; 
           break;
        case MASTER_0_MODE_DISABLE_E: 
           value = old_value & ~ MASTER_0_MODE_MASK;
           break;
    }
  

    I2CWrite(TDM_INTERFACE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_master0_mode
 *
 * RET  : Return the value of register 0xTdmInterface
 *
 * Notes : From register 0xD1B, bits 7:7
 *
 **************************************************************************/
unsigned char get_master0_mode()
  {
    unsigned char value;


    /* 
     * 0: The Master Generator for AUD_IF1/0 is stopped, 
     * 1: The Master Generator for AUD_IF1/0 is active 
     */ 
    value = (I2CRead(TDM_INTERFACE_REG) & MASTER_0_MODE_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if0_word_length
 *
 * IN   : param, a value to write to the regiter TdmConfiguration
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmConfiguration
 *
 * Notes : From register 0xD1C, bits 1:0
 *
 **************************************************************************/
unsigned char set_if0_word_length( enum word_length param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_CONFIGURATION_REG);

    /* 
     * 00: IF0 Word Length is 16 bits; 
     * 01: IF0 Word Length is 20 bits; 
     * 10: IF0 Word Length is 24 bits, 
     * 11: IF0 Word Length is 32 bit 
     */ 
  

     value =  old_value & ~IF_0_WORD_LENGTH_MASK ;

    switch(  param ){

           case WORD_LENGHT_16_BITS_E:
                value =  value | WORD_LENGHT_16_BITS ;
                break;
           case WORD_LENGHT_20_BITS_E:
                value =  value | WORD_LENGHT_20_BITS ;
                break;
           case WORD_LENGHT_24_BITS_E:
                value =  value | WORD_LENGHT_24_BITS ;
                break;
           case WORD_LENGHT_32_BITS_E:
                value =  value | WORD_LENGHT_32_BITS ;
                break;
    }
  

    I2CWrite(TDM_CONFIGURATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if0_word_length
 *
 * RET  : Return the value of register 0xTdmConfiguration
 *
 * Notes : From register 0xD1C, bits 1:0
 *
 **************************************************************************/
unsigned char get_if0_word_length()
  {
    unsigned char value;


    /* 
     * 00: IF0 Word Length is 16 bits; 
     * 01: IF0 Word Length is 20 bits; 
     * 10: IF0 Word Length is 24 bits, 
     * 11: IF0 Word Length is 32 bit 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_REG) & IF_0_WORD_LENGTH_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_if0_mode
 *
 * IN   : param, a value to write to the regiter TdmConfiguration
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmConfiguration
 *
 * Notes : From register 0xD1C, bits 3:2
 *
 **************************************************************************/
unsigned char set_if0_mode( enum format_mode param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_CONFIGURATION_REG);

    /* 
     * 0: IF0 is disabled (no data is read from the interface). 
     * 1: IF0 format is TDM, 1x: IF0 format is I2S Left Aligned 
     */ 
  

     value =  old_value & ~IF_0_MODE_MASK ;

    switch(  param ){

           case DISABLE_E:
                value  = value  | (DISABLE << 0x2);
                break;
           case TDM_E:
                value  = value  | (TDM << 0x2);
                break;
           case I2S_LEFT_ALIGNED_E:
                value  = value  | (I2S_LEFT_ALIGNED << 0x2);
                break;
    }
  

    I2CWrite(TDM_CONFIGURATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if0_mode
 *
 * RET  : Return the value of register 0xTdmConfiguration
 *
 * Notes : From register 0xD1C, bits 3:2
 *
 **************************************************************************/
unsigned char get_if0_mode()
  {
    unsigned char value;


    /* 
     * 0: IF0 is disabled (no data is read from the interface). 
     * 1: IF0 format is TDM, 1x: IF0 format is I2S Left Aligned 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_REG) & IF_0_MODE_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if0_delayed
 *
 * IN   : param, a value to write to the regiter TdmConfiguration
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmConfiguration
 *
 * Notes : From register 0xD1C, bits 4:4
 *
 **************************************************************************/
unsigned char set_if0_delayed( enum delayed param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_CONFIGURATION_REG);

    /* 
     * 0: IF0 format is not delayed; 
     * 1: IF0 format is delayed 
     */ 
    switch( param ){
        case DELAYED_E: 
           value = old_value | IF_0_DELAYED_MASK; 
           break;
        case NOT_DELAYED_E: 
           value = old_value & ~ IF_0_DELAYED_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if0_delayed
 *
 * RET  : Return the value of register 0xTdmConfiguration
 *
 * Notes : From register 0xD1C, bits 4:4
 *
 **************************************************************************/
unsigned char get_if0_delayed()
  {
    unsigned char value;


    /* 
     * 0: IF0 format is not delayed; 
     * 1: IF0 format is delayed 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_REG) & IF_0_DELAYED_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_input0_bit_sampled
 *
 * IN   : param, a value to write to the regiter TdmConfiguration
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmConfiguration
 *
 * Notes : From register 0xD1C, bits 5:5
 *
 **************************************************************************/
unsigned char set_input0_bit_sampled( enum input0_bit_sampled param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_CONFIGURATION_REG);

    /* 
     * 0: The input bits of AUD_IF0 are sampled with BitClk 
     * rising edge; 
     * 1: The input bits of AUD_IF0 are sampled with BitClk 
     * falling edge 
     */ 
    switch( param ){
        case BIT_CLOCK0_FALLING_EDGE_E: 
           value = old_value | INPUT_0_BIT_SAMPLED_MASK; 
           break;
        case BIT_CLOCK0_RISING_EDGE_E: 
           value = old_value & ~ INPUT_0_BIT_SAMPLED_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_input0_bit_sampled
 *
 * RET  : Return the value of register 0xTdmConfiguration
 *
 * Notes : From register 0xD1C, bits 5:5
 *
 **************************************************************************/
unsigned char get_input0_bit_sampled()
  {
    unsigned char value;


    /* 
     * 0: The input bits of AUD_IF0 are sampled with BitClk 
     * rising edge; 
     * 1: The input bits of AUD_IF0 are sampled with BitClk 
     * falling edge 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_REG) & INPUT_0_BIT_SAMPLED_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_first_slot0_synchro
 *
 * IN   : param, a value to write to the regiter TdmConfiguration
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmConfiguration
 *
 * Notes : From register 0xD1C, bits 6:6
 *
 **************************************************************************/
unsigned char set_first_slot0_synchro( enum first_slot_synchro0 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_CONFIGURATION_REG);

    /* 
     * 0: The first slot of AUD_IF0 is synchronized with Fsync 
     * rising edge; 
     * 1: The first slot of AUD_IF0 is synchronized with Fsync 
     * falling edge 
     */ 
    switch( param ){
        case FSYNC_FALLING_EDGE_E: 
           value = old_value | FIRST_SLOT_0_SYNCHRO_MASK; 
           break;
        case FSYNC_RISING_EDGE_E: 
           value = old_value & ~ FIRST_SLOT_0_SYNCHRO_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_first_slot0_synchro
 *
 * RET  : Return the value of register 0xTdmConfiguration
 *
 * Notes : From register 0xD1C, bits 6:6
 *
 **************************************************************************/
unsigned char get_first_slot0_synchro()
  {
    unsigned char value;


    /* 
     * 0: The first slot of AUD_IF0 is synchronized with Fsync 
     * rising edge; 
     * 1: The first slot of AUD_IF0 is synchronized with Fsync 
     * falling edge 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_REG) & FIRST_SLOT_0_SYNCHRO_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_burst_mode
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 0:0
 *
 **************************************************************************/
unsigned char set_burst_mode( enum burst_mode_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_LOOPBACK_CONTROL_REG);

    /* 
     * 0: TDM Interface IF0 is configured in normal mode; 
     * 1: TDM Interface IF0 is configured in burst mode 
     */ 
    switch( param ){
        case BURST_MODE_ENABLE_E: 
           value = old_value | BURST_MODE_MASK; 
           break;
        case BURST_MODE_DISABLE_E: 
           value = old_value & ~ BURST_MODE_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_burst_mode
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 0:0
 *
 **************************************************************************/
unsigned char get_burst_mode()
  {
    unsigned char value;


    /* 
     * 0: TDM Interface IF0 is configured in normal mode; 
     * 1: TDM Interface IF0 is configured in burst mode 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & BURST_MODE_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_master_clock_if0
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 1:1
 *
 **************************************************************************/
unsigned char set_master_clock_if0( enum master_clock_if0_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_LOOPBACK_CONTROL_REG);

    /* 
     * 0: Pins FSync0 and BitClk0 are set as input ; 
     * 1: Pins FSync0 and BitClk0 are set as output 
     */ 
    switch( param ){
        case MASTER_CLOCK_IF_0_ENABLE_E: 
           value = old_value | MASTER_CLOCK_IF_0_MASK; 
           break;
        case MASTER_CLOCK_IF_0_DISABLE_E: 
           value = old_value & ~ MASTER_CLOCK_IF_0_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_master_clock_if0
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 1:1
 *
 **************************************************************************/
unsigned char get_master_clock_if0()
  {
    unsigned char value;


    /* 
     * 0: Pins FSync0 and BitClk0 are set as input ; 
     * 1: Pins FSync0 and BitClk0 are set as output 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & MASTER_CLOCK_IF_0_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if1_on_if0
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 2:2
 *
 **************************************************************************/
unsigned char set_if1_on_if0( enum if1_on_if0_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_LOOPBACK_CONTROL_REG);

    /* 
     * 0: BITCLK0, FSYNC0 pins are used for the default purpose, 
     * 1: Clocks on BITCLK1,FSYNC1 pins are sent to BITCLK0, 
     * FSYNC0 
     */ 
    switch( param ){
        case IF_1_ON_IF_0_ENABLE_E: 
           value = old_value | IF_1_ON_IF_0_MASK; 
           break;
        case IF_1_ON_IF_0_DISABLE_E: 
           value = old_value & ~ IF_1_ON_IF_0_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if1_on_if0
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 2:2
 *
 **************************************************************************/
unsigned char get_if1_on_if0()
  {
    unsigned char value;


    /* 
     * 0: BITCLK0, FSYNC0 pins are used for the default purpose, 
     * 1: Clocks on BITCLK1,FSYNC1 pins are sent to BITCLK0, 
     * FSYNC0 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & IF_1_ON_IF_0_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_data1_to_ad_data0
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 3:3
 *
 **************************************************************************/
unsigned char set_da_data1_to_ad_data0( enum da_data1_to_ad_data0_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_LOOPBACK_CONTROL_REG);

    /* 
     * 0: AD_DATA0 pin is used for the default purpose ; 
     * 1: Data on DA_DATA1 pin is sent to AD_DATA0 pin 
     */ 
    switch( param ){
        case DA_DATA_1_TO_AD_DATA_0_ENABLE_E: 
           value = old_value | DA_DATA_1_TO_AD_DATA_0_MASK; 
           break;
        case DA_DATA_1_TO_AD_DATA_0_DISABLE_E: 
           value = old_value & ~ DA_DATA_1_TO_AD_DATA_0_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_data1_to_ad_data0
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 3:3
 *
 **************************************************************************/
unsigned char get_da_data1_to_ad_data0()
  {
    unsigned char value;


    /* 
     * 0: AD_DATA0 pin is used for the default purpose ; 
     * 1: Data on DA_DATA1 pin is sent to AD_DATA0 pin 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & DA_DATA_1_TO_AD_DATA_0_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_master_clock_if1
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 5:5
 *
 **************************************************************************/
unsigned char set_master_clock_if1( enum master_clock_if1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_LOOPBACK_CONTROL_REG);

    /* 
     * 0: Pins FSync1 and BitClk1 are set as input ; 
     * 1: Pins FSync1 and BitClk1 are set as output 
     */ 
    switch( param ){
        case MASTER_CLOCK_IF_1_ENABLE_E: 
           value = old_value | MASTER_CLOCK_IF_1_MASK; 
           break;
        case MASTER_CLOCK_IF_1_DISABLE_E: 
           value = old_value & ~ MASTER_CLOCK_IF_1_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_master_clock_if1
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 5:5
 *
 **************************************************************************/
unsigned char get_master_clock_if1()
  {
    unsigned char value;


    /* 
     * 0: Pins FSync1 and BitClk1 are set as input ; 
     * 1: Pins FSync1 and BitClk1 are set as output 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & MASTER_CLOCK_IF_1_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if0_on_if1
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 6:6
 *
 **************************************************************************/
unsigned char set_if0_on_if1( enum if0_on_if1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_LOOPBACK_CONTROL_REG);

    /* 
     * 0: BITCLK1, FSYNC1 pins are used for the default purpose, 
     * 1: Clocks on BITCLK0,FSYNC0 pins are sent to BITCLK1, 
     * FSYNC1 
     */ 
    switch( param ){
        case IF_0_ON_IF_1_ENABLE_E: 
           value = old_value | IF_0_ON_IF_1_MASK; 
           break;
        case IF_0_ON_IF_1_DISABLE_E: 
           value = old_value & ~ IF_0_ON_IF_1_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if0_on_if1
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 6:6
 *
 **************************************************************************/
unsigned char get_if0_on_if1()
  {
    unsigned char value;


    /* 
     * 0: BITCLK1, FSYNC1 pins are used for the default purpose, 
     * 1: Clocks on BITCLK0,FSYNC0 pins are sent to BITCLK1, 
     * FSYNC1 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & IF_0_ON_IF_1_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_data0_to_ad_data1
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 7:7
 *
 **************************************************************************/
unsigned char set_da_data0_to_ad_data1( enum da_data0_to_ad_data1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_LOOPBACK_CONTROL_REG);

    /* 
     * 0: AD_DATA1 pin is used for the default purpose ; 
     * 1: Data on DA_DATA0 pin is sent to AD_DATA1 pin 
     */ 
    switch( param ){
        case DA_DATA_0_TO_AD_DATA_1_ENABLE_E: 
           value = old_value | DA_DATA_0_TO_AD_DATA_1_MASK; 
           break;
        case DA_DATA_0_TO_AD_DATA_1_DISABLE_E: 
           value = old_value & ~ DA_DATA_0_TO_AD_DATA_1_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_data0_to_ad_data1
 *
 * RET  : Return the value of register 0xTdmLoopbackControl
 *
 * Notes : From register 0xD1D, bits 7:7
 *
 **************************************************************************/
unsigned char get_da_data0_to_ad_data1()
  {
    unsigned char value;


    /* 
     * 0: AD_DATA1 pin is used for the default purpose ; 
     * 1: Data on DA_DATA0 pin is sent to AD_DATA1 pin 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & DA_DATA_0_TO_AD_DATA_1_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if1_word_length
 *
 * IN   : param, a value to write to the regiter TdmConfiguration1
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmConfiguration1
 *
 * Notes : From register 0xD1E, bits 1:0
 *
 **************************************************************************/
unsigned char set_if1_word_length( enum word_length param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_CONFIGURATION_1_REG);

    /* 
     * 00: IF1 Word Length is 16 bits; 
     * 01: IF1 Word Length is 20 bits; 
     * 10: IF1 Word Length is 24 bits, 
     * 11: IF1 Word Length is 32 bit 
     */ 
  

     value =  old_value & ~IF_1_WORD_LENGTH_MASK ;

    switch(  param ){

           case WORD_LENGHT_16_BITS_E:
                value =  value | WORD_LENGHT_16_BITS ;
                break;
           case WORD_LENGHT_20_BITS_E:
                value =  value | WORD_LENGHT_20_BITS ;
                break;
           case WORD_LENGHT_24_BITS_E:
                value =  value | WORD_LENGHT_24_BITS ;
                break;
           case WORD_LENGHT_32_BITS_E:
                value =  value | WORD_LENGHT_32_BITS ;
                break;
    }
  

    I2CWrite(TDM_CONFIGURATION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if1_word_length
 *
 * RET  : Return the value of register 0xTdmConfiguration1
 *
 * Notes : From register 0xD1E, bits 1:0
 *
 **************************************************************************/
unsigned char get_if1_word_length()
  {
    unsigned char value;


    /* 
     * 00: IF1 Word Length is 16 bits; 
     * 01: IF1 Word Length is 20 bits; 
     * 10: IF1 Word Length is 24 bits, 
     * 11: IF1 Word Length is 32 bit 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_1_REG) & IF_1_WORD_LENGTH_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_if1_mode
 *
 * IN   : param, a value to write to the regiter TdmConfiguration1
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmConfiguration1
 *
 * Notes : From register 0xD1E, bits 3:3
 *
 **************************************************************************/
unsigned char set_if1_mode( enum format_mode param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_CONFIGURATION_1_REG);

    /* 
     * 0: IF1 is disabled (no data is read from the interface). 
     * 1: IF1 format is TDM, 1x: IF1 format is I2S Left Aligned 
     */ 
  

     value =  old_value & ~IF_1_MODE_MASK ;

    switch(  param ){

           case DISABLE_E:
                value  = value  | (DISABLE << 0x3);
                break;
           case TDM_E:
                value  = value  | (TDM << 0x3);
                break;
           case I2S_LEFT_ALIGNED_E:
                value  = value  | (I2S_LEFT_ALIGNED << 0x3);
                break;
    }
  

    I2CWrite(TDM_CONFIGURATION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if1_mode
 *
 * RET  : Return the value of register 0xTdmConfiguration1
 *
 * Notes : From register 0xD1E, bits 3:3
 *
 **************************************************************************/
unsigned char get_if1_mode()
  {
    unsigned char value;


    /* 
     * 0: IF1 is disabled (no data is read from the interface). 
     * 1: IF1 format is TDM, 1x: IF1 format is I2S Left Aligned 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_1_REG) & IF_1_MODE_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if1_delayed
 *
 * IN   : param, a value to write to the regiter TdmConfiguration1
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmConfiguration1
 *
 * Notes : From register 0xD1E, bits 4:4
 *
 **************************************************************************/
unsigned char set_if1_delayed( enum delayed param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_CONFIGURATION_1_REG);

    /* 
     * 0: IF1 format is not delayed; 
     * 1: IF1 format is delayed 
     */ 
    switch( param ){
        case DELAYED_E: 
           value = old_value | IF_1_DELAYED_MASK; 
           break;
        case NOT_DELAYED_E: 
           value = old_value & ~ IF_1_DELAYED_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if1_delayed
 *
 * RET  : Return the value of register 0xTdmConfiguration1
 *
 * Notes : From register 0xD1E, bits 4:4
 *
 **************************************************************************/
unsigned char get_if1_delayed()
  {
    unsigned char value;


    /* 
     * 0: IF1 format is not delayed; 
     * 1: IF1 format is delayed 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_1_REG) & IF_1_DELAYED_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_input1_bit_sampled
 *
 * IN   : param, a value to write to the regiter TdmConfiguration1
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmConfiguration1
 *
 * Notes : From register 0xD1E, bits 5:5
 *
 **************************************************************************/
unsigned char set_input1_bit_sampled( enum input1_bit_sampled param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_CONFIGURATION_1_REG);

    /* 
     * 0: The input bits of AUD_IF1 are sampled with BitClk 
     * rising edge; 
     * 1: The input bits of AUD_IF1 are sampled with BitClk 
     * falling edge 
     */ 
    switch( param ){
        case BIT_CLOCK1_FALLING_EDGE_E: 
           value = old_value | INPUT_1_BIT_SAMPLED_MASK; 
           break;
        case BIT_CLOCK1_RISING_EDGE_E: 
           value = old_value & ~ INPUT_1_BIT_SAMPLED_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_input1_bit_sampled
 *
 * RET  : Return the value of register 0xTdmConfiguration1
 *
 * Notes : From register 0xD1E, bits 5:5
 *
 **************************************************************************/
unsigned char get_input1_bit_sampled()
  {
    unsigned char value;


    /* 
     * 0: The input bits of AUD_IF1 are sampled with BitClk 
     * rising edge; 
     * 1: The input bits of AUD_IF1 are sampled with BitClk 
     * falling edge 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_1_REG) & INPUT_1_BIT_SAMPLED_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_first_slot1_synchro
 *
 * IN   : param, a value to write to the regiter TdmConfiguration1
 * OUT  : 
 *
 * RET  : Return the value of register 0xTdmConfiguration1
 *
 * Notes : From register 0xD1E, bits 6:6
 *
 **************************************************************************/
unsigned char set_first_slot1_synchro( enum first_slot_synchro0 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_CONFIGURATION_1_REG);

    /* 
     * 1: The first slot of AUD_IF1 is synchronized with Fsync 
     * rising edge; 
     * 0: The first slot of AUD_IF1 is synchronized with Fsync 
     * falling edge 
     */ 
    switch( param ){
        case FSYNC_FALLING_EDGE_E: 
           value = old_value | FIRST_SLOT_1_SYNCHRO_MASK; 
           break;
        case FSYNC_RISING_EDGE_E: 
           value = old_value & ~ FIRST_SLOT_1_SYNCHRO_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_first_slot1_synchro
 *
 * RET  : Return the value of register 0xTdmConfiguration1
 *
 * Notes : From register 0xD1E, bits 6:6
 *
 **************************************************************************/
unsigned char get_first_slot1_synchro()
  {
    unsigned char value;


    /* 
     * 1: The first slot of AUD_IF1 is synchronized with Fsync 
     * rising edge; 
     * 0: The first slot of AUD_IF1 is synchronized with Fsync 
     * falling edge 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_1_REG) & FIRST_SLOT_1_SYNCHRO_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output1
 *
 * IN   : param, a value to write to the regiter SlotAllocation0
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation0
 *
 * Notes : From register 0xD1F, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output1( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_0_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_1_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output1
 *
 * RET  : Return the value of register 0xSlotAllocation0
 *
 * Notes : From register 0xD1F, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output1()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_0_REG) & SLOT_OUTPUT_1_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output0
 *
 * IN   : param, a value to write to the regiter SlotAllocation0
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation0
 *
 * Notes : From register 0xD1F, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output0( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_0_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_0_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output0
 *
 * RET  : Return the value of register 0xSlotAllocation0
 *
 * Notes : From register 0xD1F, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output0()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_0_REG) & SLOT_OUTPUT_0_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output3
 *
 * IN   : param, a value to write to the regiter SlotAllocation2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation2
 *
 * Notes : From register 0xD20, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output3( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_2_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_3_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output3
 *
 * RET  : Return the value of register 0xSlotAllocation2
 *
 * Notes : From register 0xD20, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output3()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_2_REG) & SLOT_OUTPUT_3_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output2
 *
 * IN   : param, a value to write to the regiter SlotAllocation2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation2
 *
 * Notes : From register 0xD20, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output2( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_2_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_2_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output2
 *
 * RET  : Return the value of register 0xSlotAllocation2
 *
 * Notes : From register 0xD20, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output2()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_2_REG) & SLOT_OUTPUT_2_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output5
 *
 * IN   : param, a value to write to the regiter SlotAllocation4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation4
 *
 * Notes : From register 0xD21, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output5( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_4_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_5_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output5
 *
 * RET  : Return the value of register 0xSlotAllocation4
 *
 * Notes : From register 0xD21, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output5()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_4_REG) & SLOT_OUTPUT_5_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output4
 *
 * IN   : param, a value to write to the regiter SlotAllocation4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation4
 *
 * Notes : From register 0xD21, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output4( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_4_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_4_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output4
 *
 * RET  : Return the value of register 0xSlotAllocation4
 *
 * Notes : From register 0xD21, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output4()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_4_REG) & SLOT_OUTPUT_4_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output7
 *
 * IN   : param, a value to write to the regiter SlotAllocation6
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation6
 *
 * Notes : From register 0xD22, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output7( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_6_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_7_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output7
 *
 * RET  : Return the value of register 0xSlotAllocation6
 *
 * Notes : From register 0xD22, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output7()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_6_REG) & SLOT_OUTPUT_7_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output6
 *
 * IN   : param, a value to write to the regiter SlotAllocation6
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation6
 *
 * Notes : From register 0xD22, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output6( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_6_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_6_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output6
 *
 * RET  : Return the value of register 0xSlotAllocation6
 *
 * Notes : From register 0xD22, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output6()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_6_REG) & SLOT_OUTPUT_6_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output9
 *
 * IN   : param, a value to write to the regiter SlotAllocation8
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation8
 *
 * Notes : From register 0xD23, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output9( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_8_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_9_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output9
 *
 * RET  : Return the value of register 0xSlotAllocation8
 *
 * Notes : From register 0xD23, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output9()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_8_REG) & SLOT_OUTPUT_9_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output8
 *
 * IN   : param, a value to write to the regiter SlotAllocation8
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation8
 *
 * Notes : From register 0xD23, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output8( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_8_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_8_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output8
 *
 * RET  : Return the value of register 0xSlotAllocation8
 *
 * Notes : From register 0xD23, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output8()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_8_REG) & SLOT_OUTPUT_8_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output11
 *
 * IN   : param, a value to write to the regiter SlotAllocation10
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation10
 *
 * Notes : From register 0xD24, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output11( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_10_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_11_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output11
 *
 * RET  : Return the value of register 0xSlotAllocation10
 *
 * Notes : From register 0xD24, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output11()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_10_REG) & SLOT_OUTPUT_11_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output10
 *
 * IN   : param, a value to write to the regiter SlotAllocation10
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation10
 *
 * Notes : From register 0xD24, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output10( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_10_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_10_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_10_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output10
 *
 * RET  : Return the value of register 0xSlotAllocation10
 *
 * Notes : From register 0xD24, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output10()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_10_REG) & SLOT_OUTPUT_10_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output13
 *
 * IN   : param, a value to write to the regiter SlotAllocation12
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation12
 *
 * Notes : From register 0xD25, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output13( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_12_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_13_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_12_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output13
 *
 * RET  : Return the value of register 0xSlotAllocation12
 *
 * Notes : From register 0xD25, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output13()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_12_REG) & SLOT_OUTPUT_13_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output12
 *
 * IN   : param, a value to write to the regiter SlotAllocation12
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation12
 *
 * Notes : From register 0xD25, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output12( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_12_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_12_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_12_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output12
 *
 * RET  : Return the value of register 0xSlotAllocation12
 *
 * Notes : From register 0xD25, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output12()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_12_REG) & SLOT_OUTPUT_12_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output15
 *
 * IN   : param, a value to write to the regiter SlotAllocation14
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation14
 *
 * Notes : From register 0xD26, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output15( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_14_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_15_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_14_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output15
 *
 * RET  : Return the value of register 0xSlotAllocation14
 *
 * Notes : From register 0xD26, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output15()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_14_REG) & SLOT_OUTPUT_15_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output14
 *
 * IN   : param, a value to write to the regiter SlotAllocation14
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation14
 *
 * Notes : From register 0xD26, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output14( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_14_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_14_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_14_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output14
 *
 * RET  : Return the value of register 0xSlotAllocation14
 *
 * Notes : From register 0xD26, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output14()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_14_REG) & SLOT_OUTPUT_14_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output17
 *
 * IN   : param, a value to write to the regiter SlotAllocation16
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation16
 *
 * Notes : From register 0xD27, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output17( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_16_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_17_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_16_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output17
 *
 * RET  : Return the value of register 0xSlotAllocation16
 *
 * Notes : From register 0xD27, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output17()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_16_REG) & SLOT_OUTPUT_17_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output16
 *
 * IN   : param, a value to write to the regiter SlotAllocation16
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation16
 *
 * Notes : From register 0xD27, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output16( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_16_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_16_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_16_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output16
 *
 * RET  : Return the value of register 0xSlotAllocation16
 *
 * Notes : From register 0xD27, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output16()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_16_REG) & SLOT_OUTPUT_16_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output19
 *
 * IN   : param, a value to write to the regiter SlotAllocation18
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation18
 *
 * Notes : From register 0xD28, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output19( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_18_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_19_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_18_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output19
 *
 * RET  : Return the value of register 0xSlotAllocation18
 *
 * Notes : From register 0xD28, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output19()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_18_REG) & SLOT_OUTPUT_19_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output18
 *
 * IN   : param, a value to write to the regiter SlotAllocation18
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation18
 *
 * Notes : From register 0xD28, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output18( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_18_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_18_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_18_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output18
 *
 * RET  : Return the value of register 0xSlotAllocation18
 *
 * Notes : From register 0xD28, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output18()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_18_REG) & SLOT_OUTPUT_18_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output21
 *
 * IN   : param, a value to write to the regiter SlotAllocation20
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation20
 *
 * Notes : From register 0xD29, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output21( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_20_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_21_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output21
 *
 * RET  : Return the value of register 0xSlotAllocation20
 *
 * Notes : From register 0xD29, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output21()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_20_REG) & SLOT_OUTPUT_21_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output20
 *
 * IN   : param, a value to write to the regiter SlotAllocation20
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation20
 *
 * Notes : From register 0xD29, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output20( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_20_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_20_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_20_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output20
 *
 * RET  : Return the value of register 0xSlotAllocation20
 *
 * Notes : From register 0xD29, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output20()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_20_REG) & SLOT_OUTPUT_20_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output23
 *
 * IN   : param, a value to write to the regiter SlotAllocation22
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation22
 *
 * Notes : From register 0xD2A, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output23( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_22_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_23_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output23
 *
 * RET  : Return the value of register 0xSlotAllocation22
 *
 * Notes : From register 0xD2A, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output23()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_22_REG) & SLOT_OUTPUT_23_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output22
 *
 * IN   : param, a value to write to the regiter SlotAllocation22
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation22
 *
 * Notes : From register 0xD2A, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output22( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_22_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_22_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_22_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output22
 *
 * RET  : Return the value of register 0xSlotAllocation22
 *
 * Notes : From register 0xD2A, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output22()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_22_REG) & SLOT_OUTPUT_22_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output25
 *
 * IN   : param, a value to write to the regiter SlotAllocation24
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation24
 *
 * Notes : From register 0xD2B, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output25( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_24_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_25_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_24_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output25
 *
 * RET  : Return the value of register 0xSlotAllocation24
 *
 * Notes : From register 0xD2B, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output25()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_24_REG) & SLOT_OUTPUT_25_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output24
 *
 * IN   : param, a value to write to the regiter SlotAllocation24
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation24
 *
 * Notes : From register 0xD2B, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output24( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_24_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_24_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_24_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output24
 *
 * RET  : Return the value of register 0xSlotAllocation24
 *
 * Notes : From register 0xD2B, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output24()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_24_REG) & SLOT_OUTPUT_24_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output27
 *
 * IN   : param, a value to write to the regiter SlotAllocation26
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation26
 *
 * Notes : From register 0xD2C, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output27( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_26_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_27_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_26_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output27
 *
 * RET  : Return the value of register 0xSlotAllocation26
 *
 * Notes : From register 0xD2C, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output27()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_26_REG) & SLOT_OUTPUT_27_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output26
 *
 * IN   : param, a value to write to the regiter SlotAllocation26
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation26
 *
 * Notes : From register 0xD2C, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output26( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_26_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_26_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_26_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output26
 *
 * RET  : Return the value of register 0xSlotAllocation26
 *
 * Notes : From register 0xD2C, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output26()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_26_REG) & SLOT_OUTPUT_26_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output29
 *
 * IN   : param, a value to write to the regiter SlotAllocation28
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation28
 *
 * Notes : From register 0xD2D, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output29( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_28_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_29_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_28_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output29
 *
 * RET  : Return the value of register 0xSlotAllocation28
 *
 * Notes : From register 0xD2D, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output29()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_28_REG) & SLOT_OUTPUT_29_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output28
 *
 * IN   : param, a value to write to the regiter SlotAllocation28
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation28
 *
 * Notes : From register 0xD2D, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output28( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_28_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_28_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_28_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output28
 *
 * RET  : Return the value of register 0xSlotAllocation28
 *
 * Notes : From register 0xD2D, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output28()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_28_REG) & SLOT_OUTPUT_28_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output31
 *
 * IN   : param, a value to write to the regiter SlotAllocation30
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation30
 *
 * Notes : From register 0xD2E, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output31( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_30_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_31_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT1 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT2 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT3 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT4 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT5 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT6 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT7 << 0x4);
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value  = value  | (SLOT_OUT_FROM_AD_OUT8 << 0x4);
                break;
           case SLOT_OUT_TRISTATE_E:
                value  = value  | (SLOT_OUT_TRISTATE << 0x4);
                break;
           case SLOT_OUT_ZEROS_E:
                value  = value  | (SLOT_OUT_ZEROS << 0x4);
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_30_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output31
 *
 * RET  : Return the value of register 0xSlotAllocation30
 *
 * Notes : From register 0xD2E, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output31()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_30_REG) & SLOT_OUTPUT_31_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output30
 *
 * IN   : param, a value to write to the regiter SlotAllocation30
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotAllocation30
 *
 * Notes : From register 0xD2E, bits 3:0
 *
 **************************************************************************/
unsigned char set_slot_output30( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_30_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_30_MASK ;

    switch(  param ){

           case SLOT_OUT_FROM_AD_OUT1_E:
                value =  value | SLOT_OUT_FROM_AD_OUT1 ;
                break;
           case SLOT_OUT_FROM_AD_OUT2_E:
                value =  value | SLOT_OUT_FROM_AD_OUT2 ;
                break;
           case SLOT_OUT_FROM_AD_OUT3_E:
                value =  value | SLOT_OUT_FROM_AD_OUT3 ;
                break;
           case SLOT_OUT_FROM_AD_OUT4_E:
                value =  value | SLOT_OUT_FROM_AD_OUT4 ;
                break;
           case SLOT_OUT_FROM_AD_OUT5_E:
                value =  value | SLOT_OUT_FROM_AD_OUT5 ;
                break;
           case SLOT_OUT_FROM_AD_OUT6_E:
                value =  value | SLOT_OUT_FROM_AD_OUT6 ;
                break;
           case SLOT_OUT_FROM_AD_OUT7_E:
                value =  value | SLOT_OUT_FROM_AD_OUT7 ;
                break;
           case SLOT_OUT_FROM_AD_OUT8_E:
                value =  value | SLOT_OUT_FROM_AD_OUT8 ;
                break;
           case SLOT_OUT_TRISTATE_E:
                value =  value | SLOT_OUT_TRISTATE ;
                break;
           case SLOT_OUT_ZEROS_E:
                value =  value | SLOT_OUT_ZEROS ;
                break;
    }
  

    I2CWrite(SLOT_ALLOCATION_30_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output30
 *
 * RET  : Return the value of register 0xSlotAllocation30
 *
 * Notes : From register 0xD2E, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output30()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_30_REG) & SLOT_OUTPUT_30_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz7
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 7:7
 *
 **************************************************************************/
unsigned char set_slot_hiz7( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_7_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_7_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz7
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 7:7
 *
 **************************************************************************/
unsigned char get_slot_hiz7()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_7_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz6
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 6:6
 *
 **************************************************************************/
unsigned char set_slot_hiz6( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_6_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_6_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz6
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 6:6
 *
 **************************************************************************/
unsigned char get_slot_hiz6()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_6_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz5
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 5:5
 *
 **************************************************************************/
unsigned char set_slot_hiz5( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_5_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_5_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz5
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 5:5
 *
 **************************************************************************/
unsigned char get_slot_hiz5()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_5_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz4
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 4:4
 *
 **************************************************************************/
unsigned char set_slot_hiz4( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_4_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_4_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz4
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 4:4
 *
 **************************************************************************/
unsigned char get_slot_hiz4()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_4_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz3
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 3:3
 *
 **************************************************************************/
unsigned char set_slot_hiz3( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_3_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_3_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz3
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 3:3
 *
 **************************************************************************/
unsigned char get_slot_hiz3()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_3_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz2
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 2:2
 *
 **************************************************************************/
unsigned char set_slot_hiz2( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_2_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_2_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz2
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 2:2
 *
 **************************************************************************/
unsigned char get_slot_hiz2()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_2_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz1
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 1:1
 *
 **************************************************************************/
unsigned char set_slot_hiz1( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_1_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_1_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz1
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 1:1
 *
 **************************************************************************/
unsigned char get_slot_hiz1()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_1_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz0
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 0:0
 *
 **************************************************************************/
unsigned char set_slot_hiz0( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_0_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_0_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz0
 *
 * RET  : Return the value of register 0xADslotTristateControl0
 *
 * Notes : From register 0xD2F, bits 0:0
 *
 **************************************************************************/
unsigned char get_slot_hiz0()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_0_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz15
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 7:7
 *
 **************************************************************************/
unsigned char set_slot_hiz15( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_15_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_15_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz15
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 7:7
 *
 **************************************************************************/
unsigned char get_slot_hiz15()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_15_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz14
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 6:6
 *
 **************************************************************************/
unsigned char set_slot_hiz14( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_14_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_14_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz14
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 6:6
 *
 **************************************************************************/
unsigned char get_slot_hiz14()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_14_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz13
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 5:5
 *
 **************************************************************************/
unsigned char set_slot_hiz13( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_13_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_13_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz13
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 5:5
 *
 **************************************************************************/
unsigned char get_slot_hiz13()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_13_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz12
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 4:4
 *
 **************************************************************************/
unsigned char set_slot_hiz12( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_12_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_12_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz12
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 4:4
 *
 **************************************************************************/
unsigned char get_slot_hiz12()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_12_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz11
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 3:3
 *
 **************************************************************************/
unsigned char set_slot_hiz11( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_11_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_11_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz11
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 3:3
 *
 **************************************************************************/
unsigned char get_slot_hiz11()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_11_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz10
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 2:2
 *
 **************************************************************************/
unsigned char set_slot_hiz10( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_10_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_10_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz10
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 2:2
 *
 **************************************************************************/
unsigned char get_slot_hiz10()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_10_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz9
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 1:1
 *
 **************************************************************************/
unsigned char set_slot_hiz9( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_9_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_9_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz9
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 1:1
 *
 **************************************************************************/
unsigned char get_slot_hiz9()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_9_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz8
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 0:0
 *
 **************************************************************************/
unsigned char set_slot_hiz8( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_8_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_8_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz8
 *
 * RET  : Return the value of register 0xADslotTristateControl1
 *
 * Notes : From register 0xD30, bits 0:0
 *
 **************************************************************************/
unsigned char get_slot_hiz8()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_8_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz23
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 7:7
 *
 **************************************************************************/
unsigned char set_slot_hiz23( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_23_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_23_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz23
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 7:7
 *
 **************************************************************************/
unsigned char get_slot_hiz23()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_23_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz22
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 6:6
 *
 **************************************************************************/
unsigned char set_slot_hiz22( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_22_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_22_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz22
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 6:6
 *
 **************************************************************************/
unsigned char get_slot_hiz22()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_22_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz21
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 5:5
 *
 **************************************************************************/
unsigned char set_slot_hiz21( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_21_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_21_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz21
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 5:5
 *
 **************************************************************************/
unsigned char get_slot_hiz21()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_21_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz20
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 4:4
 *
 **************************************************************************/
unsigned char set_slot_hiz20( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_20_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_20_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz20
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 4:4
 *
 **************************************************************************/
unsigned char get_slot_hiz20()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_20_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz19
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 3:3
 *
 **************************************************************************/
unsigned char set_slot_hiz19( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_19_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_19_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz19
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 3:3
 *
 **************************************************************************/
unsigned char get_slot_hiz19()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_19_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz18
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 2:2
 *
 **************************************************************************/
unsigned char set_slot_hiz18( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_18_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_18_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz18
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 2:2
 *
 **************************************************************************/
unsigned char get_slot_hiz18()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_18_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz17
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 1:1
 *
 **************************************************************************/
unsigned char set_slot_hiz17( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_17_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_17_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz17
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 1:1
 *
 **************************************************************************/
unsigned char get_slot_hiz17()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_17_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz16
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 0:0
 *
 **************************************************************************/
unsigned char set_slot_hiz16( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_16_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_16_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz16
 *
 * RET  : Return the value of register 0xADslotTristateControl2
 *
 * Notes : From register 0xD31, bits 0:0
 *
 **************************************************************************/
unsigned char get_slot_hiz16()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_16_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz31
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 7:7
 *
 **************************************************************************/
unsigned char set_slot_hiz31( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_31_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_31_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz31
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 7:7
 *
 **************************************************************************/
unsigned char get_slot_hiz31()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_31_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz30
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 6:6
 *
 **************************************************************************/
unsigned char set_slot_hiz30( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_30_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_30_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz30
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 6:6
 *
 **************************************************************************/
unsigned char get_slot_hiz30()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_30_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz29
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 5:5
 *
 **************************************************************************/
unsigned char set_slot_hiz29( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_29_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_29_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz29
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 5:5
 *
 **************************************************************************/
unsigned char get_slot_hiz29()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_29_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz28
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 4:4
 *
 **************************************************************************/
unsigned char set_slot_hiz28( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_28_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_28_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz28
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 4:4
 *
 **************************************************************************/
unsigned char get_slot_hiz28()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_28_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz27
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 3:3
 *
 **************************************************************************/
unsigned char set_slot_hiz27( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_27_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_27_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz27
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 3:3
 *
 **************************************************************************/
unsigned char get_slot_hiz27()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_27_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz26
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 2:2
 *
 **************************************************************************/
unsigned char set_slot_hiz26( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_26_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_26_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz26
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 2:2
 *
 **************************************************************************/
unsigned char get_slot_hiz26()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_26_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz25
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 1:1
 *
 **************************************************************************/
unsigned char set_slot_hiz25( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_25_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_25_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz25
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 1:1
 *
 **************************************************************************/
unsigned char get_slot_hiz25()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_25_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz24
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 0:0
 *
 **************************************************************************/
unsigned char set_slot_hiz24( enum slot_hiz param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG);

    /* 
     * Slot impedance after last half-lsb 
     */ 
    switch( param ){
        case SLOT_HIGH_IMPEDANCE_E: 
           value = old_value | SLOT_HIZ_24_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_24_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz24
 *
 * RET  : Return the value of register 0xADslotTristateControl3
 *
 * Notes : From register 0xD32, bits 0:0
 *
 **************************************************************************/
unsigned char get_slot_hiz24()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_24_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp01
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath1
 *
 * Notes : From register 0xD33, bits 4:0
 *
 **************************************************************************/
unsigned char set_slot_dsp01( enum slot_dsp param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_1_REG);

    /* 
     * Data sent to DA1 input of DA filter (DSP) 
     */ 
  
    value =  old_value & ~SLOT_DSP_01_MASK;


    value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp01
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath1
 *
 * Notes : From register 0xD33, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp01()
  {
    unsigned char value;


    /* 
     * Data sent to DA1 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_1_REG) & SLOT_DSP_01_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in7_to_adout1
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath1
 *
 * Notes : From register 0xD33, bits 5:5
 *
 **************************************************************************/
unsigned char set_da_in7_to_adout1( enum da_in_to_ad_out param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_1_REG);

    /* 
     * 0: AD_OUT1 is connected to the corresponding AD path; 
     * 1: DA_IN7 is looped back to AD_OUT1 
     */ 
    switch( param ){
        case DA_IN_LOOPED_TO_AD_OUT_E: 
           value = old_value | DA_IN_7_TO_ADOUT_1_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_7_TO_ADOUT_1_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in7_to_adout1
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath1
 *
 * Notes : From register 0xD33, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in7_to_adout1()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT1 is connected to the corresponding AD path; 
     * 1: DA_IN7 is looped back to AD_OUT1 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_1_REG) & DA_IN_7_TO_ADOUT_1_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_swap12_34
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath1
 *
 * Notes : From register 0xD33, bits 6:6
 *
 **************************************************************************/
unsigned char set_swap12_34( enum swap12_34 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_1_REG);

    /* 
     * Swap chanel 1 and 2 with 3 and 4 
     */ 
    switch( param ){
        case SWAP12_34_E: 
           value = old_value | SWAP_1234_MASK; 
           break;
        case NO_SWAP12_34_E: 
           value = old_value & ~ SWAP_1234_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_swap12_34
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath1
 *
 * Notes : From register 0xD33, bits 6:6
 *
 **************************************************************************/
unsigned char get_swap12_34()
  {
    unsigned char value;


    /* 
     * Swap chanel 1 and 2 with 3 and 4 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_1_REG) & SWAP_1234_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da12_voice_filter
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath1
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath1
 *
 * Notes : From register 0xD33, bits 7:7
 *
 **************************************************************************/
unsigned char set_da12_voice_filter( enum da12_voice_filter_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_1_REG);

    /* 
     * 0: DA1 and DA2 channel paths use Audio Filters 
     * 1: DA1 and DA2 channel paths use 48kHz Low Latency 
     * Filters for Voice 
     */ 
    switch( param ){
        case DA_12_VOICE_FILTER_ENABLE_E: 
           value = old_value | DA_12_VOICE_FILTER_MASK; 
           break;
        case DA_12_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ DA_12_VOICE_FILTER_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da12_voice_filter
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath1
 *
 * Notes : From register 0xD33, bits 7:7
 *
 **************************************************************************/
unsigned char get_da12_voice_filter()
  {
    unsigned char value;


    /* 
     * 0: DA1 and DA2 channel paths use Audio Filters 
     * 1: DA1 and DA2 channel paths use 48kHz Low Latency 
     * Filters for Voice 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_1_REG) & DA_12_VOICE_FILTER_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp02
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath2
 *
 * Notes : From register 0xD34, bits 4:0
 *
 **************************************************************************/
unsigned char set_slot_dsp02( enum slot_dsp param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_2_REG);

    /* 
     * Data sent to DA2 input of DA filter (DSP) 
     */ 
  
    value =  old_value & ~SLOT_DSP_02_MASK;


    value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp02
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath2
 *
 * Notes : From register 0xD34, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp02()
  {
    unsigned char value;


    /* 
     * Data sent to DA2 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_2_REG) & SLOT_DSP_02_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in8_to_adout2
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath2
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath2
 *
 * Notes : From register 0xD34, bits 5:5
 *
 **************************************************************************/
unsigned char set_da_in8_to_adout2( enum da_in_to_ad_out param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_2_REG);

    /* 
     * 0: AD_OUT2 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT2 
     */ 
    switch( param ){
        case DA_IN_LOOPED_TO_AD_OUT_E: 
           value = old_value | DA_IN_8_TO_ADOUT_2_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_8_TO_ADOUT_2_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in8_to_adout2
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath2
 *
 * Notes : From register 0xD34, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in8_to_adout2()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT2 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT2 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_2_REG) & DA_IN_8_TO_ADOUT_2_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp03
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath3
 *
 * Notes : From register 0xD35, bits 4:0
 *
 **************************************************************************/
unsigned char set_slot_dsp03( enum slot_dsp param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_3_REG);

    /* 
     * Data sent to DA3 input of DA filter (DSP) 
     */ 
  
    value =  old_value & ~SLOT_DSP_03_MASK;


    value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp03
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath3
 *
 * Notes : From register 0xD35, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp03()
  {
    unsigned char value;


    /* 
     * Data sent to DA3 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_3_REG) & SLOT_DSP_03_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in7_to_adout3
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath3
 *
 * Notes : From register 0xD35, bits 5:5
 *
 **************************************************************************/
unsigned char set_da_in7_to_adout3( enum da_in_to_ad_out param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_3_REG);

    /* 
     * 0: AD_OUT3 is connected to the corresponding AD path; 
     * 1: DA_IN7 is looped back to AD_OUT3 
     */ 
    switch( param ){
        case DA_IN_LOOPED_TO_AD_OUT_E: 
           value = old_value | DA_IN_7_TO_ADOUT_3_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_7_TO_ADOUT_3_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in7_to_adout3
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath3
 *
 * Notes : From register 0xD35, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in7_to_adout3()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT3 is connected to the corresponding AD path; 
     * 1: DA_IN7 is looped back to AD_OUT3 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_3_REG) & DA_IN_7_TO_ADOUT_3_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da34_voice_filter
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath3
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath3
 *
 * Notes : From register 0xD35, bits 7:7
 *
 **************************************************************************/
unsigned char set_da34_voice_filter( enum da34_voice_filter_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_3_REG);

    /* 
     * 0: DA3 and DA3 channel paths use Audio Filters 
     * 1: DA3 and DA3 channel paths use 48kHz Low Latency 
     * Filters for Voice 
     */ 
    switch( param ){
        case DA_34_VOICE_FILTER_ENABLE_E: 
           value = old_value | DA_34_VOICE_FILTER_MASK; 
           break;
        case DA_34_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ DA_34_VOICE_FILTER_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da34_voice_filter
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath3
 *
 * Notes : From register 0xD35, bits 7:7
 *
 **************************************************************************/
unsigned char get_da34_voice_filter()
  {
    unsigned char value;


    /* 
     * 0: DA3 and DA3 channel paths use Audio Filters 
     * 1: DA3 and DA3 channel paths use 48kHz Low Latency 
     * Filters for Voice 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_3_REG) & DA_34_VOICE_FILTER_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp04
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath4
 *
 * Notes : From register 0xD36, bits 4:0
 *
 **************************************************************************/
unsigned char set_slot_dsp04( enum slot_dsp param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_4_REG);

    /* 
     * Data sent to DA4 input of DA filter (DSP) 
     */ 
  
    value =  old_value & ~SLOT_DSP_04_MASK;


    value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp04
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath4
 *
 * Notes : From register 0xD36, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp04()
  {
    unsigned char value;


    /* 
     * Data sent to DA4 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_4_REG) & SLOT_DSP_04_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in8_to_adout4
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath4
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath4
 *
 * Notes : From register 0xD36, bits 5:5
 *
 **************************************************************************/
unsigned char set_da_in8_to_adout4( enum da_in_to_ad_out param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_4_REG);

    /* 
     * 0: AD_OUT4 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT4 
     */ 
    switch( param ){
        case DA_IN_LOOPED_TO_AD_OUT_E: 
           value = old_value | DA_IN_8_TO_ADOUT_4_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_8_TO_ADOUT_4_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in8_to_adout4
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath4
 *
 * Notes : From register 0xD36, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in8_to_adout4()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT4 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT4 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_4_REG) & DA_IN_8_TO_ADOUT_4_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp05
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath5
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath5
 *
 * Notes : From register 0xD37, bits 4:0
 *
 **************************************************************************/
unsigned char set_slot_dsp05( enum slot_dsp param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_5_REG);

    /* 
     * Data sent to DA5 input of DA filter (DSP) 
     */ 
  
    value =  old_value & ~SLOT_DSP_05_MASK;


    value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp05
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath5
 *
 * Notes : From register 0xD37, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp05()
  {
    unsigned char value;


    /* 
     * Data sent to DA5 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_5_REG) & SLOT_DSP_05_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in7_to_adout5
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath5
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath5
 *
 * Notes : From register 0xD37, bits 5:5
 *
 **************************************************************************/
unsigned char set_da_in7_to_adout5( enum da_in_to_ad_out param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_5_REG);

    /* 
     * 0: AD_OUT8 is connected to the corresponding AD path; 
     * 1: DA_IN7 is looped back to AD_OUT5 
     */ 
    switch( param ){
        case DA_IN_LOOPED_TO_AD_OUT_E: 
           value = old_value | DA_IN_7_TO_ADOUT_5_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_7_TO_ADOUT_5_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in7_to_adout5
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath5
 *
 * Notes : From register 0xD37, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in7_to_adout5()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT8 is connected to the corresponding AD path; 
     * 1: DA_IN7 is looped back to AD_OUT5 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_5_REG) & DA_IN_7_TO_ADOUT_5_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da56_voice_filter
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath5
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath5
 *
 * Notes : From register 0xD37, bits 7:7
 *
 **************************************************************************/
unsigned char set_da56_voice_filter( enum da56_voice_filter_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_5_REG);

    /* 
     * 0: DA5 and DA6 channel paths use Audio Filters 
     * 1: DA5 and DA6 channel paths use 48kHz Low Latency 
     * Filters for Voice 
     */ 
    switch( param ){
        case DA_56_VOICE_FILTER_ENABLE_E: 
           value = old_value | DA_56_VOICE_FILTER_MASK; 
           break;
        case DA_56_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ DA_56_VOICE_FILTER_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da56_voice_filter
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath5
 *
 * Notes : From register 0xD37, bits 7:7
 *
 **************************************************************************/
unsigned char get_da56_voice_filter()
  {
    unsigned char value;


    /* 
     * 0: DA5 and DA6 channel paths use Audio Filters 
     * 1: DA5 and DA6 channel paths use 48kHz Low Latency 
     * Filters for Voice 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_5_REG) & DA_56_VOICE_FILTER_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp06
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath6
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath6
 *
 * Notes : From register 0xD38, bits 4:0
 *
 **************************************************************************/
unsigned char set_slot_dsp06( enum slot_dsp param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_6_REG);

    /* 
     * Data sent to DA6 input of DA filter (DSP) 
     */ 
  
    value =  old_value & ~SLOT_DSP_06_MASK;


    value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp06
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath6
 *
 * Notes : From register 0xD38, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp06()
  {
    unsigned char value;


    /* 
     * Data sent to DA6 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_6_REG) & SLOT_DSP_06_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in8_to_adout6
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath6
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath6
 *
 * Notes : From register 0xD38, bits 5:5
 *
 **************************************************************************/
unsigned char set_da_in8_to_adout6( enum da_in_to_ad_out param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_6_REG);

    /* 
     * 0: AD_OUT6 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT6 
     */ 
    switch( param ){
        case DA_IN_LOOPED_TO_AD_OUT_E: 
           value = old_value | DA_IN_8_TO_ADOUT_6_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_8_TO_ADOUT_6_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in8_to_adout6
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath6
 *
 * Notes : From register 0xD38, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in8_to_adout6()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT6 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT6 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_6_REG) & DA_IN_8_TO_ADOUT_6_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp07
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath7
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath7
 *
 * Notes : From register 0xD39, bits 4:0
 *
 **************************************************************************/
unsigned char set_slot_dsp07( enum slot_dsp param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_7_REG);

    /* 
     * Data sent to DA7 input of DA filter (DSP) 
     */ 
  
    value =  old_value & ~SLOT_DSP_07_MASK;


    value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp07
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath7
 *
 * Notes : From register 0xD39, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp07()
  {
    unsigned char value;


    /* 
     * Data sent to DA7 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_7_REG) & SLOT_DSP_07_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in8_to_adout7
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath7
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath7
 *
 * Notes : From register 0xD39, bits 5:5
 *
 **************************************************************************/
unsigned char set_da_in8_to_adout7( enum da_in_to_ad_out param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_7_REG);

    /* 
     * 0: AD_OUT7 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT7 
     */ 
    switch( param ){
        case DA_IN_LOOPED_TO_AD_OUT_E: 
           value = old_value | DA_IN_8_TO_ADOUT_7_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_8_TO_ADOUT_7_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in8_to_adout7
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath7
 *
 * Notes : From register 0xD39, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in8_to_adout7()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT7 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT7 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_7_REG) & DA_IN_8_TO_ADOUT_7_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp08
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath8
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath8
 *
 * Notes : From register 0xD3A, bits 4:0
 *
 **************************************************************************/
unsigned char set_slot_dsp08( enum slot_dsp param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_8_REG);

    /* 
     * Data sent to DA8 input of DA filter (DSP) 
     */ 
  
    value =  old_value & ~SLOT_DSP_08_MASK;


    value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp08
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath8
 *
 * Notes : From register 0xD3A, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp08()
  {
    unsigned char value;


    /* 
     * Data sent to DA8 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_8_REG) & SLOT_DSP_08_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in8_to_adout8
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath8
 * OUT  : 
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath8
 *
 * Notes : From register 0xD3A, bits 5:5
 *
 **************************************************************************/
unsigned char set_da_in8_to_adout8( enum da_in_to_ad_out param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_8_REG);

    /* 
     * 0: AD_OUT8 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT6 
     */ 
    switch( param ){
        case DA_IN_LOOPED_TO_AD_OUT_E: 
           value = old_value | DA_IN_8_TO_ADOUT_8_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_8_TO_ADOUT_8_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in8_to_adout8
 *
 * RET  : Return the value of register 0xSlotSelectionDAPath8
 *
 * Notes : From register 0xD3A, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in8_to_adout8()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT8 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT6 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_8_REG) & DA_IN_8_TO_ADOUT_8_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hf_output_bridged
 *
 * IN   : param, a value to write to the regiter ClassDEMIControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xClassDEMIControl
 *
 * Notes : From register 0xD3B, bits 7:7
 *
 **************************************************************************/
unsigned char set_hf_output_bridged( enum hf_output_bridged_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLASS_DEMI_CONTROL_REG);

    /* 
     * 0: The HFL and HFR Outputs are independent; 
     * 1: The HFL and HFR Outputs are bridged. HFL output is 
     * used for both 
     */ 
    switch( param ){
        case HF_OUTPUT_BRIDGED_ENABLE_E: 
           value = old_value | HF_OUTPUT_BRIDGED_MASK; 
           break;
        case HF_OUTPUT_BRIDGED_DISABLE_E: 
           value = old_value & ~ HF_OUTPUT_BRIDGED_MASK;
           break;
    }
  

    I2CWrite(CLASS_DEMI_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hf_output_bridged
 *
 * RET  : Return the value of register 0xClassDEMIControl
 *
 * Notes : From register 0xD3B, bits 7:7
 *
 **************************************************************************/
unsigned char get_hf_output_bridged()
  {
    unsigned char value;


    /* 
     * 0: The HFL and HFR Outputs are independent; 
     * 1: The HFL and HFR Outputs are bridged. HFL output is 
     * used for both 
     */ 
    value = (I2CRead(CLASS_DEMI_CONTROL_REG) & HF_OUTPUT_BRIDGED_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_high_volume
 *
 * IN   : param, a value to write to the regiter ClassDPath
 * OUT  : 
 *
 * RET  : Return the value of register 0xClassDPath
 *
 * Notes : From register 0xD3C, bits 3:0
 *
 **************************************************************************/
unsigned char set_high_volume( enum high_volume param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLASS_D_PATH_REG);

    /* 
     * 0000: High volume mode disabled; xxx1: Left HF channel in 
     * high volume mode; xx1x: Right HF channel in high volume 
     * mode; x1xx: Vibra1 channel in high volume mode; 1xxx: 
     * Vibra2 channel in high volume mode 
     */ 
  

     value =  old_value & ~HIGH_VOLUME_MASK ;

    switch(  param ){

           case HIGH_VOLUME_DISABLED_E:
                value =  value | HIGH_VOLUME_DISABLED ;
                break;
           case LEFT_HF_HIGH_VOLUME_E:
                value =  value | LEFT_HF_HIGH_VOLUME ;
                break;
           case RIGHT_HF_HIGH_VOLUME_E:
                value =  value | RIGHT_HF_HIGH_VOLUME ;
                break;
           case VIB1_HIGH_VOLUME_E:
                value =  value | VIB1_HIGH_VOLUME ;
                break;
           case VIB2_HIGH_VOLUME_E:
                value =  value | VIB2_HIGH_VOLUME ;
                break;
    }
  

    I2CWrite(CLASS_D_PATH_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_high_volume
 *
 * RET  : Return the value of register 0xClassDPath
 *
 * Notes : From register 0xD3C, bits 3:0
 *
 **************************************************************************/
unsigned char get_high_volume()
  {
    unsigned char value;


    /* 
     * 0000: High volume mode disabled; xxx1: Left HF channel in 
     * high volume mode; xx1x: Right HF channel in high volume 
     * mode; x1xx: Vibra1 channel in high volume mode; 1xxx: 
     * Vibra2 channel in high volume mode 
     */ 
    value = (I2CRead(CLASS_D_PATH_REG) & HIGH_VOLUME_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_class_d_fir_bypass
 *
 * IN   : param, a value to write to the regiter ClassDPath
 * OUT  : 
 *
 * RET  : Return the value of register 0xClassDPath
 *
 * Notes : From register 0xD3C, bits 7:4
 *
 **************************************************************************/
unsigned char set_class_d_fir_bypass( enum class_d_fir_bypass param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLASS_D_PATH_REG);

    /* 
     * 0000: All PWM FIRs are enabled; xxx1: Left HF PWM FIR 
     * filter is bypassed; xx1x: Right HF PWM FIR filter is 
     * bypassed; x1xx: Left Vibra PWM FIR filter is bypassed; 
     * 1xxx: Right Vibra PWM FIR filter is bypassed 
     */ 
  

     value =  old_value & ~CLASS_D_FIR_BYPASS_MASK ;

    switch(  param ){

           case ALL_PWM_FIR_ENABLED_E:
                value  = value  | (ALL_PWM_FIR_ENABLED << 0x4);
                break;
           case LEFT_HF_BYPASSED_E:
                value  = value  | (LEFT_HF_BYPASSED << 0x4);
                break;
           case RIGHT_HF_BYPASSED_E:
                value  = value  | (RIGHT_HF_BYPASSED << 0x4);
                break;
           case VIB1_BYPASSED_E:
                value  = value  | (VIB1_BYPASSED << 0x4);
                break;
           case VIB2_BYPASSED_E:
                value  = value  | (VIB2_BYPASSED << 0x4);
                break;
    }
  

    I2CWrite(CLASS_D_PATH_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_class_d_fir_bypass
 *
 * RET  : Return the value of register 0xClassDPath
 *
 * Notes : From register 0xD3C, bits 7:4
 *
 **************************************************************************/
unsigned char get_class_d_fir_bypass()
  {
    unsigned char value;


    /* 
     * 0000: All PWM FIRs are enabled; xxx1: Left HF PWM FIR 
     * filter is bypassed; xx1x: Right HF PWM FIR filter is 
     * bypassed; x1xx: Left Vibra PWM FIR filter is bypassed; 
     * 1xxx: Right Vibra PWM FIR filter is bypassed 
     */ 
    value = (I2CRead(CLASS_D_PATH_REG) & CLASS_D_FIR_BYPASS_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_highwhite_component_dither
 *
 * IN   : param, a value to write to the regiter ClassDControlGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xClassDControlGain
 *
 * Notes : From register 0xD3D, bits 3:0
 *
 **************************************************************************/
unsigned char set_highwhite_component_dither( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLASS_D_CONTROL_GAIN_REG);

    /* 
     * Gain control for the white component of dither 
     */ 
  
    value =  old_value & ~HIGHWHITE_COMPONENT_DITHER_MASK;


    value |=  param ;  

    I2CWrite(CLASS_D_CONTROL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_highwhite_component_dither
 *
 * RET  : Return the value of register 0xClassDControlGain
 *
 * Notes : From register 0xD3D, bits 3:0
 *
 **************************************************************************/
unsigned char get_highwhite_component_dither()
  {
    unsigned char value;


    /* 
     * Gain control for the white component of dither 
     */ 
    value = (I2CRead(CLASS_D_CONTROL_GAIN_REG) & HIGHWHITE_COMPONENT_DITHER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_high_pass_component_dither
 *
 * IN   : param, a value to write to the regiter ClassDControlGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xClassDControlGain
 *
 * Notes : From register 0xD3D, bits 7:4
 *
 **************************************************************************/
unsigned char set_high_pass_component_dither( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLASS_D_CONTROL_GAIN_REG);

    /* 
     * Gain control for the high pass component of dither 
     */ 
  
    value =  old_value & ~HIGH_PASS_COMPONENT_DITHER_MASK;


    value |= ( param << 0x4);  

    I2CWrite(CLASS_D_CONTROL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_high_pass_component_dither
 *
 * RET  : Return the value of register 0xClassDControlGain
 *
 * Notes : From register 0xD3D, bits 7:4
 *
 **************************************************************************/
unsigned char get_high_pass_component_dither()
  {
    unsigned char value;


    /* 
     * Gain control for the high pass component of dither 
     */ 
    value = (I2CRead(CLASS_D_CONTROL_GAIN_REG) & HIGH_PASS_COMPONENT_DITHER_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_decimator_mic6
 *
 * IN   : param, a value to write to the regiter DecimatorFilter
 * OUT  : 
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 0:0
 *
 **************************************************************************/
unsigned char set_decimator_mic6( enum decimator_mic param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DECIMATOR_FILTER_REG);

    /* 
     * 0: Sinc5 decimator filter is selected for DMIC6; 
     * 1: Sinc3 decimator filter is selected for DMIC6 
     */ 
    switch( param ){
        case SINC3_DECIMATOR_FILTER_E: 
           value = old_value | DECIMATOR_MIC_6_MASK; 
           break;
        case SINC5_DECIMATOR_FILTER_E: 
           value = old_value & ~ DECIMATOR_MIC_6_MASK;
           break;
    }
  

    I2CWrite(DECIMATOR_FILTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic6
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 0:0
 *
 **************************************************************************/
unsigned char get_decimator_mic6()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC6; 
     * 1: Sinc3 decimator filter is selected for DMIC6 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_6_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_decimator_mic5
 *
 * IN   : param, a value to write to the regiter DecimatorFilter
 * OUT  : 
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 1:1
 *
 **************************************************************************/
unsigned char set_decimator_mic5( enum decimator_mic param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DECIMATOR_FILTER_REG);

    /* 
     * 0: Sinc5 decimator filter is selected for DMIC5; 
     * 1: Sinc3 decimator filter is selected for DMIC5 
     */ 
    switch( param ){
        case SINC3_DECIMATOR_FILTER_E: 
           value = old_value | DECIMATOR_MIC_5_MASK; 
           break;
        case SINC5_DECIMATOR_FILTER_E: 
           value = old_value & ~ DECIMATOR_MIC_5_MASK;
           break;
    }
  

    I2CWrite(DECIMATOR_FILTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic5
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 1:1
 *
 **************************************************************************/
unsigned char get_decimator_mic5()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC5; 
     * 1: Sinc3 decimator filter is selected for DMIC5 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_5_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_decimator_mic4
 *
 * IN   : param, a value to write to the regiter DecimatorFilter
 * OUT  : 
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 2:2
 *
 **************************************************************************/
unsigned char set_decimator_mic4( enum decimator_mic param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DECIMATOR_FILTER_REG);

    /* 
     * 0: Sinc5 decimator filter is selected for DMIC4; 
     * 1: Sinc3 decimator filter is selected for DMIC4 
     */ 
    switch( param ){
        case SINC3_DECIMATOR_FILTER_E: 
           value = old_value | DECIMATOR_MIC_4_MASK; 
           break;
        case SINC5_DECIMATOR_FILTER_E: 
           value = old_value & ~ DECIMATOR_MIC_4_MASK;
           break;
    }
  

    I2CWrite(DECIMATOR_FILTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic4
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 2:2
 *
 **************************************************************************/
unsigned char get_decimator_mic4()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC4; 
     * 1: Sinc3 decimator filter is selected for DMIC4 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_4_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_decimator_mic3
 *
 * IN   : param, a value to write to the regiter DecimatorFilter
 * OUT  : 
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 3:3
 *
 **************************************************************************/
unsigned char set_decimator_mic3( enum decimator_mic param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DECIMATOR_FILTER_REG);

    /* 
     * 0: Sinc5 decimator filter is selected for DMIC3; 
     * 1: Sinc3 decimator filter is selected for DMIC3 
     */ 
    switch( param ){
        case SINC3_DECIMATOR_FILTER_E: 
           value = old_value | DECIMATOR_MIC_3_MASK; 
           break;
        case SINC5_DECIMATOR_FILTER_E: 
           value = old_value & ~ DECIMATOR_MIC_3_MASK;
           break;
    }
  

    I2CWrite(DECIMATOR_FILTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic3
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 3:3
 *
 **************************************************************************/
unsigned char get_decimator_mic3()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC3; 
     * 1: Sinc3 decimator filter is selected for DMIC3 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_3_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_decimator_mic2
 *
 * IN   : param, a value to write to the regiter DecimatorFilter
 * OUT  : 
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 4:4
 *
 **************************************************************************/
unsigned char set_decimator_mic2( enum decimator_mic param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DECIMATOR_FILTER_REG);

    /* 
     * 0: Sinc5 decimator filter is selected for DMIC2; 
     * 1: Sinc3 decimator filter is selected for DMIC2 
     */ 
    switch( param ){
        case SINC3_DECIMATOR_FILTER_E: 
           value = old_value | DECIMATOR_MIC_2_MASK; 
           break;
        case SINC5_DECIMATOR_FILTER_E: 
           value = old_value & ~ DECIMATOR_MIC_2_MASK;
           break;
    }
  

    I2CWrite(DECIMATOR_FILTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic2
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 4:4
 *
 **************************************************************************/
unsigned char get_decimator_mic2()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC2; 
     * 1: Sinc3 decimator filter is selected for DMIC2 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_2_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_decimator_mic1
 *
 * IN   : param, a value to write to the regiter DecimatorFilter
 * OUT  : 
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 5:5
 *
 **************************************************************************/
unsigned char set_decimator_mic1( enum decimator_mic param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DECIMATOR_FILTER_REG);

    /* 
     * 0: Sinc5 decimator filter is selected for DMIC1; 
     * 1: Sinc3 decimator filter is selected for DMIC1 
     */ 
    switch( param ){
        case SINC3_DECIMATOR_FILTER_E: 
           value = old_value | DECIMATOR_MIC_1_MASK; 
           break;
        case SINC5_DECIMATOR_FILTER_E: 
           value = old_value & ~ DECIMATOR_MIC_1_MASK;
           break;
    }
  

    I2CWrite(DECIMATOR_FILTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic1
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 5:5
 *
 **************************************************************************/
unsigned char get_decimator_mic1()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC1; 
     * 1: Sinc3 decimator filter is selected for DMIC1 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_1_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_dac_source
 *
 * IN   : param, a value to write to the regiter DecimatorFilter
 * OUT  : 
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 6:6
 *
 **************************************************************************/
unsigned char set_ear_dac_source( enum ear_dac_source param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DECIMATOR_FILTER_REG);

    /* 
     * 0: EarDAC signal comes from DA1 path 
     * 1: EarDAC signal comes from DA3 path 
     */ 
    switch( param ){
        case SIGNAL_FORM_DA3_PATH_E: 
           value = old_value | EAR_DAC_SOURCE_MASK; 
           break;
        case SIGNAL_FORM_DA1_PATH_E: 
           value = old_value & ~ EAR_DAC_SOURCE_MASK;
           break;
    }
  

    I2CWrite(DECIMATOR_FILTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_dac_source
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 6:6
 *
 **************************************************************************/
unsigned char get_ear_dac_source()
  {
    unsigned char value;


    /* 
     * 0: EarDAC signal comes from DA1 path 
     * 1: EarDAC signal comes from DA3 path 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & EAR_DAC_SOURCE_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_input
 *
 * IN   : param, a value to write to the regiter DecimatorFilter
 * OUT  : 
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 7:7
 *
 **************************************************************************/
unsigned char set_anc_input( enum anc_input param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DECIMATOR_FILTER_REG);

    /* 
     * 0: ANC input is AD6 
     * 1: ANC input is AD5 
     */ 
    switch( param ){
        case ANC_INPUT_AD5_E: 
           value = old_value | ANC_INPUT_MASK; 
           break;
        case ANC_INPUT_AD6_E: 
           value = old_value & ~ ANC_INPUT_MASK;
           break;
    }
  

    I2CWrite(DECIMATOR_FILTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_anc_input
 *
 * RET  : Return the value of register 0xDecimatorFilter
 *
 * Notes : From register 0xD3E, bits 7:7
 *
 **************************************************************************/
unsigned char get_anc_input()
  {
    unsigned char value;


    /* 
     * 0: ANC input is AD6 
     * 1: ANC input is AD5 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & ANC_INPUT_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_loop_mixed
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 0:0
 *
 **************************************************************************/
unsigned char set_anc_loop_mixed( enum anc_loop_mixed param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_REG);

    /* 
     * 0: ANC loop not mixed driver. 
     * 1: ANC loop mixed in earpiece driver. 
     */ 
    switch( param ){
        case MIXED_E: 
           value = old_value | ANC_LOOP_MIXED_MASK; 
           break;
        case NOT_MIXED_E: 
           value = old_value & ~ ANC_LOOP_MIXED_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_anc_loop_mixed
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 0:0
 *
 **************************************************************************/
unsigned char get_anc_loop_mixed()
  {
    unsigned char value;


    /* 
     * 0: ANC loop not mixed driver. 
     * 1: ANC loop mixed in earpiece driver. 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & ANC_LOOP_MIXED_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_dig_micro6_for_out6
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 1:1
 *
 **************************************************************************/
unsigned char set_dig_micro6_for_out6( enum micro param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_REG);

    /* 
     * 0: Analog microphone on ADR analog input is selected for 
     * AD_OUT6/ANC output path; 
     * 1: Digital microphone DMIC6 is selected for AD_OUT6/ANC 
     * output path 
     */ 
    switch( param ){
        case MICRO_DIGITAL_E: 
           value = old_value | DIG_MICRO_6_FOR_OUT_6_MASK; 
           break;
        case MICRO_ANALOG_E: 
           value = old_value & ~ DIG_MICRO_6_FOR_OUT_6_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dig_micro6_for_out6
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 1:1
 *
 **************************************************************************/
unsigned char get_dig_micro6_for_out6()
  {
    unsigned char value;


    /* 
     * 0: Analog microphone on ADR analog input is selected for 
     * AD_OUT6/ANC output path; 
     * 1: Digital microphone DMIC6 is selected for AD_OUT6/ANC 
     * output path 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & DIG_MICRO_6_FOR_OUT_6_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dig_micro5_for_out5
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 2:2
 *
 **************************************************************************/
unsigned char set_dig_micro5_for_out5( enum micro param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_REG);

    /* 
     * 0: Analog microphone on ADR analog input is selected for 
     * AD_OUT5 output path; 
     * 1: Digital microphone DMIC5 is selected for AD_OUT5 ANC 
     * output path 
     */ 
    switch( param ){
        case MICRO_DIGITAL_E: 
           value = old_value | DIG_MICRO_5_FOR_OUT_5_MASK; 
           break;
        case MICRO_ANALOG_E: 
           value = old_value & ~ DIG_MICRO_5_FOR_OUT_5_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dig_micro5_for_out5
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 2:2
 *
 **************************************************************************/
unsigned char get_dig_micro5_for_out5()
  {
    unsigned char value;


    /* 
     * 0: Analog microphone on ADR analog input is selected for 
     * AD_OUT5 output path; 
     * 1: Digital microphone DMIC5 is selected for AD_OUT5 ANC 
     * output path 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & DIG_MICRO_5_FOR_OUT_5_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dig_micro3_for_out3
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 3:3
 *
 **************************************************************************/
unsigned char set_dig_micro3_for_out3( enum micro param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_REG);

    /* 
     * 0: Analog microphone ADMO analog input is selected for 
     * AD_OUT3 output path; 
     * 1: Digital microphone DMIC3 is selected for AD_OUT3 
     * output path 
     */ 
    switch( param ){
        case MICRO_DIGITAL_E: 
           value = old_value | DIG_MICRO_3_FOR_OUT_3_MASK; 
           break;
        case MICRO_ANALOG_E: 
           value = old_value & ~ DIG_MICRO_3_FOR_OUT_3_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dig_micro3_for_out3
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 3:3
 *
 **************************************************************************/
unsigned char get_dig_micro3_for_out3()
  {
    unsigned char value;


    /* 
     * 0: Analog microphone ADMO analog input is selected for 
     * AD_OUT3 output path; 
     * 1: Digital microphone DMIC3 is selected for AD_OUT3 
     * output path 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & DIG_MICRO_3_FOR_OUT_3_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dig_micro2_for_out2
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 4:4
 *
 **************************************************************************/
unsigned char set_dig_micro2_for_out2( enum micro param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_REG);

    /* 
     * 0: Right Linein ADL analog input is selected for AD_OUT2 
     * output path, 
     * 1: Digital microphone DMIC2 is selected for AD_OUT2 
     * output path 
     */ 
    switch( param ){
        case MICRO_DIGITAL_E: 
           value = old_value | DIG_MICRO_2_FOR_OUT_2_MASK; 
           break;
        case MICRO_ANALOG_E: 
           value = old_value & ~ DIG_MICRO_2_FOR_OUT_2_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dig_micro2_for_out2
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 4:4
 *
 **************************************************************************/
unsigned char get_dig_micro2_for_out2()
  {
    unsigned char value;


    /* 
     * 0: Right Linein ADL analog input is selected for AD_OUT2 
     * output path, 
     * 1: Digital microphone DMIC2 is selected for AD_OUT2 
     * output path 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & DIG_MICRO_2_FOR_OUT_2_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dig_micro1_for_out1
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 5:5
 *
 **************************************************************************/
unsigned char set_dig_micro1_for_out1( enum micro param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_REG);

    /* 
     * 0: Left Linein ADL analog input is selected for AD_OUT1 
     * output path, 
     * 1: Digital microphone DMIC1 is selected for AD_OUT1 
     * output path 
     */ 
    switch( param ){
        case MICRO_DIGITAL_E: 
           value = old_value | DIG_MICRO_1_FOR_OUT_1_MASK; 
           break;
        case MICRO_ANALOG_E: 
           value = old_value & ~ DIG_MICRO_1_FOR_OUT_1_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dig_micro1_for_out1
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 5:5
 *
 **************************************************************************/
unsigned char get_dig_micro1_for_out1()
  {
    unsigned char value;


    /* 
     * 0: Left Linein ADL analog input is selected for AD_OUT1 
     * output path, 
     * 1: Digital microphone DMIC1 is selected for AD_OUT1 
     * output path 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & DIG_MICRO_1_FOR_OUT_1_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_output_digital_in2
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 6:6
 *
 **************************************************************************/
unsigned char set_output_digital_in2( enum path_in2 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_REG);

    /* 
     * 0: DA_IN2 direct path disabled. Path from sidetone FIR 
     * connected to HSR, 
     * 1: DA_IN2 path mixed together with sidetone FIR. 
     */ 
    switch( param ){
        case IN2_MIXED_WITH_SIDETONE_E: 
           value = old_value | OUTPUT_DIGITAL_IN_2_MASK; 
           break;
        case IN2_CONNECTED_TO_HSR_E: 
           value = old_value & ~ OUTPUT_DIGITAL_IN_2_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_output_digital_in2
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 6:6
 *
 **************************************************************************/
unsigned char get_output_digital_in2()
  {
    unsigned char value;


    /* 
     * 0: DA_IN2 direct path disabled. Path from sidetone FIR 
     * connected to HSR, 
     * 1: DA_IN2 path mixed together with sidetone FIR. 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & OUTPUT_DIGITAL_IN_2_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_output_digital_in1
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 7:7
 *
 **************************************************************************/
unsigned char set_output_digital_in1( enum path_in1 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_REG);

    /* 
     * 0: DA_IN1 direct path disabled. Path from sidetone FIR 
     * connected to HSL, 
     * 1: DA_IN1 path mixed together with sidetone FIR. 
     */ 
    switch( param ){
        case IN1_MIXED_WITH_SIDETONE_E: 
           value = old_value | OUTPUT_DIGITAL_IN_1_MASK; 
           break;
        case IN1_CONNECTED_TO_HSL_E: 
           value = old_value & ~ OUTPUT_DIGITAL_IN_1_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_output_digital_in1
 *
 * RET  : Return the value of register 0xDigitalMultiplexers
 *
 * Notes : From register 0xD3F, bits 7:7
 *
 **************************************************************************/
unsigned char get_output_digital_in1()
  {
    unsigned char value;


    /* 
     * 0: DA_IN1 direct path disabled. Path from sidetone FIR 
     * connected to HSL, 
     * 1: DA_IN1 path mixed together with sidetone FIR. 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & OUTPUT_DIGITAL_IN_1_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir2_selection
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 1:0
 *
 **************************************************************************/
unsigned char set_fir2_selection( enum selection_fir2 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG);

    /* 
     * 00: FIR2 data comes from AD_OUT2 
     * 01: FIR2 data comes from AD_OUT3 
     * 10: FIR2 data comes from AD_OUT4 
     * 11: FIR2 data comes from DA_IN2 
     */ 
  

     value =  old_value & ~FIR_2_SELECTION_MASK ;

    switch(  param ){

           case FIR2_AD_OUT2_SELECTED_E:
                value =  value | FIR2_AD_OUT2_SELECTED ;
                break;
           case FIR2_AD_OUT3_SELECTED_E:
                value =  value | FIR2_AD_OUT3_SELECTED ;
                break;
           case FIR2_AD_OUT4_SELECTED_E:
                value =  value | FIR2_AD_OUT4_SELECTED ;
                break;
           case FIR2_DA_IN2_SELECTED_E:
                value =  value | FIR2_DA_IN2_SELECTED ;
                break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir2_selection
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 1:0
 *
 **************************************************************************/
unsigned char get_fir2_selection()
  {
    unsigned char value;


    /* 
     * 00: FIR2 data comes from AD_OUT2 
     * 01: FIR2 data comes from AD_OUT3 
     * 10: FIR2 data comes from AD_OUT4 
     * 11: FIR2 data comes from DA_IN2 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & FIR_2_SELECTION_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir1_selection
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 3:2
 *
 **************************************************************************/
unsigned char set_fir1_selection( enum selection_fir1 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG);

    /* 
     * 00: FIR1 data comes from AD_OUT1 
     * 01: FIR1 data comes from AD_OUT2 
     * 10: FIR1 data comes from AD_OUT3 
     * 11: FIR1 data comes from DA_IN1 
     */ 
  

     value =  old_value & ~FIR_1_SELECTION_MASK ;

    switch(  param ){

           case FIR1_AD_OUT1_SELECTED_E:
                value  = value  | (FIR1_AD_OUT1_SELECTED << 0x2);
                break;
           case FIR1_AD_OUT2_SELECTED_E:
                value  = value  | (FIR1_AD_OUT2_SELECTED << 0x2);
                break;
           case FIR1_AD_OUT3_SELECTED_E:
                value  = value  | (FIR1_AD_OUT3_SELECTED << 0x2);
                break;
           case FIR1_DA_IN1_SELECTED_E:
                value  = value  | (FIR1_DA_IN1_SELECTED << 0x2);
                break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir1_selection
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 3:2
 *
 **************************************************************************/
unsigned char get_fir1_selection()
  {
    unsigned char value;


    /* 
     * 00: FIR1 data comes from AD_OUT1 
     * 01: FIR1 data comes from AD_OUT2 
     * 10: FIR1 data comes from AD_OUT3 
     * 11: FIR1 data comes from DA_IN1 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & FIR_1_SELECTION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da3_mixing
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 4:4
 *
 **************************************************************************/
unsigned char set_da3_mixing( enum da3_mixing_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG);

    /* 
     * 0: HFL data comes from DA3 if DAToHfLEn=1 
     * 1: HFL data comes from ANC if DAToHfLEn=1 
     */ 
    switch( param ){
        case DA_3_MIXING_ENABLE_E: 
           value = old_value | DA_3_MIXING_MASK; 
           break;
        case DA_3_MIXING_DISABLE_E: 
           value = old_value & ~ DA_3_MIXING_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da3_mixing
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 4:4
 *
 **************************************************************************/
unsigned char get_da3_mixing()
  {
    unsigned char value;


    /* 
     * 0: HFL data comes from DA3 if DAToHfLEn=1 
     * 1: HFL data comes from ANC if DAToHfLEn=1 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & DA_3_MIXING_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da4_mixing
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 5:5
 *
 **************************************************************************/
unsigned char set_da4_mixing( enum da4_mixing_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG);

    /* 
     * 0: HFR data comes from DA4 if DAToHfREn=1 
     * 1: HFR data comes from ANC if DAToHfREn= 
     */ 
    switch( param ){
        case DA_4_MIXING_ENABLE_E: 
           value = old_value | DA_4_MIXING_MASK; 
           break;
        case DA_4_MIXING_DISABLE_E: 
           value = old_value & ~ DA_4_MIXING_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da4_mixing
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 5:5
 *
 **************************************************************************/
unsigned char get_da4_mixing()
  {
    unsigned char value;


    /* 
     * 0: HFR data comes from DA4 if DAToHfREn=1 
     * 1: HFR data comes from ANC if DAToHfREn= 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & DA_4_MIXING_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da3_or_anc_loop_path
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 6:6
 *
 **************************************************************************/
unsigned char set_da3_or_anc_loop_path( enum da3_or_anc_mixing_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG);

    /* 
     * 0: DA3 or ANC path to HfL is muted 
     * 1: DA3 or ANC path to HfL is enabled 
     */ 
    switch( param ){
        case DA_3_OR_ANC_MIXING_ENABLE_E: 
           value = old_value | DA_3_OR_ANC_LOOP_PATH_MASK; 
           break;
        case DA_3_OR_ANC_MIXING_DISABLE_E: 
           value = old_value & ~ DA_3_OR_ANC_LOOP_PATH_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da3_or_anc_loop_path
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 6:6
 *
 **************************************************************************/
unsigned char get_da3_or_anc_loop_path()
  {
    unsigned char value;


    /* 
     * 0: DA3 or ANC path to HfL is muted 
     * 1: DA3 or ANC path to HfL is enabled 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & DA_3_OR_ANC_LOOP_PATH_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da4_or_anc_loop_path
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 7:7
 *
 **************************************************************************/
unsigned char set_da4_or_anc_loop_path( enum da4_or_anc_mixing_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG);

    /* 
     * 0: DA4 or ANC path to HfR is muted 
     * 1: DA4 or ANC path to HfR is enabled 
     */ 
    switch( param ){
        case DA_4_OR_ANC_MIXING_ENABLE_E: 
           value = old_value | DA_4_OR_ANC_LOOP_PATH_MASK; 
           break;
        case DA_4_OR_ANC_MIXING_DISABLE_E: 
           value = old_value & ~ DA_4_OR_ANC_LOOP_PATH_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da4_or_anc_loop_path
 *
 * RET  : Return the value of register 0xDigitalMultiplexersMsb
 *
 * Notes : From register 0xD40, bits 7:7
 *
 **************************************************************************/
unsigned char get_da4_or_anc_loop_path()
  {
    unsigned char value;


    /* 
     * 0: DA4 or ANC path to HfR is muted 
     * 1: DA4 or ANC path to HfR is enabled 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & DA_4_OR_ANC_LOOP_PATH_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD1
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD1
 *
 * Notes : From register 0xD41, bits 5:0
 *
 **************************************************************************/
unsigned char set_ad1_gain( enum digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD1_REG);

    /* 
     * AD1 path digital gain 
     */ 
  
    value =  old_value & ~AD_1_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_gain
 *
 * RET  : Return the value of register 0xDigitalGainAD1
 *
 * Notes : From register 0xD41, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad1_gain()
  {
    unsigned char value;


    /* 
     * AD1 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD1_REG) & AD_1_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD1
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD1
 *
 * Notes : From register 0xD41, bits 6:6
 *
 **************************************************************************/
unsigned char set_ad1_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD1_REG);

    /* 
     * 0: Fading (anti-click) on AD1 path is active; 
     * 1: Fading (anti-click) on AD1 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | AD_1_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ AD_1_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainAD1
 *
 * Notes : From register 0xD41, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad1_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD1 path is active; 
     * 1: Fading (anti-click) on AD1 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD1_REG) & AD_1_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD2
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD2
 *
 * Notes : From register 0xD42, bits 5:0
 *
 **************************************************************************/
unsigned char set_ad2_gain( enum digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD2_REG);

    /* 
     * AD2 path digital gain 
     */ 
  
    value =  old_value & ~AD_2_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_gain
 *
 * RET  : Return the value of register 0xDigitalGainAD2
 *
 * Notes : From register 0xD42, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad2_gain()
  {
    unsigned char value;


    /* 
     * AD2 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD2_REG) & AD_2_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD2
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD2
 *
 * Notes : From register 0xD42, bits 6:6
 *
 **************************************************************************/
unsigned char set_ad2_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD2_REG);

    /* 
     * 0: Fading (anti-click) on AD2 path is active; 
     * 1: Fading (anti-click) on AD2 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | AD_2_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ AD_2_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainAD2
 *
 * Notes : From register 0xD42, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad2_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD2 path is active; 
     * 1: Fading (anti-click) on AD2 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD2_REG) & AD_2_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad3_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD3
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD3
 *
 * Notes : From register 0xD43, bits 5:0
 *
 **************************************************************************/
unsigned char set_ad3_gain( enum digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD3_REG);

    /* 
     * AD3 path digital gain 
     */ 
  
    value =  old_value & ~AD_3_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad3_gain
 *
 * RET  : Return the value of register 0xDigitalGainAD3
 *
 * Notes : From register 0xD43, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad3_gain()
  {
    unsigned char value;


    /* 
     * AD3 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD3_REG) & AD_3_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad3_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD3
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD3
 *
 * Notes : From register 0xD43, bits 6:6
 *
 **************************************************************************/
unsigned char set_ad3_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD3_REG);

    /* 
     * 0: Fading (anti-click) on AD3 path is active; 
     * 1: Fading (anti-click) on AD3 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | AD_3_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ AD_3_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad3_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainAD3
 *
 * Notes : From register 0xD43, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad3_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD3 path is active; 
     * 1: Fading (anti-click) on AD3 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD3_REG) & AD_3_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad4_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD4
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD4
 *
 * Notes : From register 0xD44, bits 5:0
 *
 **************************************************************************/
unsigned char set_ad4_gain( enum digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD4_REG);

    /* 
     * AD4 path digital gain 
     */ 
  
    value =  old_value & ~AD_4_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad4_gain
 *
 * RET  : Return the value of register 0xDigitalGainAD4
 *
 * Notes : From register 0xD44, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad4_gain()
  {
    unsigned char value;


    /* 
     * AD4 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD4_REG) & AD_4_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad4_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD4
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD4
 *
 * Notes : From register 0xD44, bits 6:6
 *
 **************************************************************************/
unsigned char set_ad4_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD4_REG);

    /* 
     * 0: Fading (anti-click) on AD4 path is active; 
     * 1: Fading (anti-click) on AD4 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | AD_4_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ AD_4_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad4_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainAD4
 *
 * Notes : From register 0xD44, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad4_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD4 path is active; 
     * 1: Fading (anti-click) on AD4 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD4_REG) & AD_4_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad5_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD5
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD5
 *
 * Notes : From register 0xD45, bits 5:0
 *
 **************************************************************************/
unsigned char set_ad5_gain( enum digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD5_REG);

    /* 
     * AD5 path digital gain 
     */ 
  
    value =  old_value & ~AD_5_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad5_gain
 *
 * RET  : Return the value of register 0xDigitalGainAD5
 *
 * Notes : From register 0xD45, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad5_gain()
  {
    unsigned char value;


    /* 
     * AD5 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD5_REG) & AD_5_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad5_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD5
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD5
 *
 * Notes : From register 0xD45, bits 6:6
 *
 **************************************************************************/
unsigned char set_ad5_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD5_REG);

    /* 
     * 0: Fading (anti-click) on AD5 path is active; 
     * 1: Fading (anti-click) on AD5 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | AD_5_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ AD_5_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad5_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainAD5
 *
 * Notes : From register 0xD45, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad5_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD5 path is active; 
     * 1: Fading (anti-click) on AD5 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD5_REG) & AD_5_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad6_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD6
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD6
 *
 * Notes : From register 0xD46, bits 5:0
 *
 **************************************************************************/
unsigned char set_ad6_gain( enum digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD6_REG);

    /* 
     * AD6 path digital gain 
     */ 
  
    value =  old_value & ~AD_6_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad6_gain
 *
 * RET  : Return the value of register 0xDigitalGainAD6
 *
 * Notes : From register 0xD46, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad6_gain()
  {
    unsigned char value;


    /* 
     * AD6 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD6_REG) & AD_6_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad6_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD6
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD6
 *
 * Notes : From register 0xD46, bits 6:6
 *
 **************************************************************************/
unsigned char set_ad6_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD6_REG);

    /* 
     * 0: Fading (anti-click) on AD6 path is active; 
     * 1: Fading (anti-click) on AD6 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | AD_6_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ AD_6_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad6_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainAD6
 *
 * Notes : From register 0xD46, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad6_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD6 path is active; 
     * 1: Fading (anti-click) on AD6 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD6_REG) & AD_6_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da1_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDa1
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDa1
 *
 * Notes : From register 0xD47, bits 5:0
 *
 **************************************************************************/
unsigned char set_da1_gain( enum da_digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA_1_REG);

    /* 
     * DA1 path digital gain 
     */ 
  
    value =  old_value & ~DA_1_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da1_gain
 *
 * RET  : Return the value of register 0xDigitalGainDa1
 *
 * Notes : From register 0xD47, bits 5:0
 *
 **************************************************************************/
unsigned char get_da1_gain()
  {
    unsigned char value;


    /* 
     * DA1 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_1_REG) & DA_1_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da1_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDa1
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDa1
 *
 * Notes : From register 0xD47, bits 6:6
 *
 **************************************************************************/
unsigned char set_da1_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA_1_REG);

    /* 
     * 0: Fading (anti-click) on DA1 path is active; 
     * 1: Fading (anti-click) on DA1 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | DA_1_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ DA_1_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da1_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainDa1
 *
 * Notes : From register 0xD47, bits 6:6
 *
 **************************************************************************/
unsigned char get_da1_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA1 path is active; 
     * 1: Fading (anti-click) on DA1 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_1_REG) & DA_1_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da2_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDa2
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDa2
 *
 * Notes : From register 0xD48, bits 5:0
 *
 **************************************************************************/
unsigned char set_da2_gain( enum da_digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA_2_REG);

    /* 
     * DA2 path digital gain 
     */ 
  
    value =  old_value & ~DA_2_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da2_gain
 *
 * RET  : Return the value of register 0xDigitalGainDa2
 *
 * Notes : From register 0xD48, bits 5:0
 *
 **************************************************************************/
unsigned char get_da2_gain()
  {
    unsigned char value;


    /* 
     * DA2 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_2_REG) & DA_2_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da2_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDa2
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDa2
 *
 * Notes : From register 0xD48, bits 6:6
 *
 **************************************************************************/
unsigned char set_da2_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA_2_REG);

    /* 
     * 0: Fading (anti-click) on DA2 path is active; 
     * 1: Fading (anti-click) on DA2 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | DA_2_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ DA_2_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da2_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainDa2
 *
 * Notes : From register 0xD48, bits 6:6
 *
 **************************************************************************/
unsigned char get_da2_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA2 path is active; 
     * 1: Fading (anti-click) on DA2 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_2_REG) & DA_2_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da3_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDa3
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDa3
 *
 * Notes : From register 0xD49, bits 5:0
 *
 **************************************************************************/
unsigned char set_da3_gain( enum da_digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA_3_REG);

    /* 
     * DA3 path digital gain 
     */ 
  
    value =  old_value & ~DA_3_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da3_gain
 *
 * RET  : Return the value of register 0xDigitalGainDa3
 *
 * Notes : From register 0xD49, bits 5:0
 *
 **************************************************************************/
unsigned char get_da3_gain()
  {
    unsigned char value;


    /* 
     * DA3 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_3_REG) & DA_3_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da3_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDa3
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDa3
 *
 * Notes : From register 0xD49, bits 6:6
 *
 **************************************************************************/
unsigned char set_da3_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA_3_REG);

    /* 
     * 0: Fading (anti-click) on DA3 path is active; 
     * 1: Fading (anti-click) on DA3 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | DA_3_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ DA_3_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da3_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainDa3
 *
 * Notes : From register 0xD49, bits 6:6
 *
 **************************************************************************/
unsigned char get_da3_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA3 path is active; 
     * 1: Fading (anti-click) on DA3 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_3_REG) & DA_3_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da4_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDa4
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDa4
 *
 * Notes : From register 0xD4A, bits 5:0
 *
 **************************************************************************/
unsigned char set_da4_gain( enum da_digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA_4_REG);

    /* 
     * DA4 path digital gain 
     */ 
  
    value =  old_value & ~DA_4_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da4_gain
 *
 * RET  : Return the value of register 0xDigitalGainDa4
 *
 * Notes : From register 0xD4A, bits 5:0
 *
 **************************************************************************/
unsigned char get_da4_gain()
  {
    unsigned char value;


    /* 
     * DA4 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_4_REG) & DA_4_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da4_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDa4
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDa4
 *
 * Notes : From register 0xD4A, bits 6:6
 *
 **************************************************************************/
unsigned char set_da4_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA_4_REG);

    /* 
     * 0: Fading (anti-click) on DA4 path is active; 
     * 1: Fading (anti-click) on DA4 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | DA_4_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ DA_4_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da4_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainDa4
 *
 * Notes : From register 0xD4A, bits 6:6
 *
 **************************************************************************/
unsigned char get_da4_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA4 path is active; 
     * 1: Fading (anti-click) on DA4 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_4_REG) & DA_4_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da5_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDa5
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDa5
 *
 * Notes : From register 0xD4B, bits 5:0
 *
 **************************************************************************/
unsigned char set_da5_gain( enum da_digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA_5_REG);

    /* 
     * DA5 path digital gain 
     */ 
  
    value =  old_value & ~DA_5_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da5_gain
 *
 * RET  : Return the value of register 0xDigitalGainDa5
 *
 * Notes : From register 0xD4B, bits 5:0
 *
 **************************************************************************/
unsigned char get_da5_gain()
  {
    unsigned char value;


    /* 
     * DA5 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_5_REG) & DA_5_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da5_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDa5
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDa5
 *
 * Notes : From register 0xD4B, bits 6:6
 *
 **************************************************************************/
unsigned char set_da5_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA_5_REG);

    /* 
     * 0: Fading (anti-click) on DA5 path is active; 
     * 1: Fading (anti-click) on DA5 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | DA_5_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ DA_5_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da5_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainDa5
 *
 * Notes : From register 0xD4B, bits 6:6
 *
 **************************************************************************/
unsigned char get_da5_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA5 path is active; 
     * 1: Fading (anti-click) on DA5 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_5_REG) & DA_5_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da6_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDA6
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDA6
 *
 * Notes : From register 0xD4C, bits 5:0
 *
 **************************************************************************/
unsigned char set_da6_gain( enum da_digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA6_REG);

    /* 
     * DA6 path digital gain 
     */ 
  
    value =  old_value & ~DA_6_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da6_gain
 *
 * RET  : Return the value of register 0xDigitalGainDA6
 *
 * Notes : From register 0xD4C, bits 5:0
 *
 **************************************************************************/
unsigned char get_da6_gain()
  {
    unsigned char value;


    /* 
     * DA6 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA6_REG) & DA_6_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da6_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDA6
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainDA6
 *
 * Notes : From register 0xD4C, bits 6:6
 *
 **************************************************************************/
unsigned char set_da6_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_DA6_REG);

    /* 
     * 0: Fading (anti-click) on DA6 path is active; 
     * 1: Fading (anti-click) on DA6 path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | DA_6_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ DA_6_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da6_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainDA6
 *
 * Notes : From register 0xD4C, bits 6:6
 *
 **************************************************************************/
unsigned char get_da6_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA6 path is active; 
     * 1: Fading (anti-click) on DA6 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA6_REG) & DA_6_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_hfl_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD1_HFL
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD1_HFL
 *
 * Notes : From register 0xD4D, bits 5:0
 *
 **************************************************************************/
unsigned char set_ad1_hfl_gain( enum da_digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD1HFL_REG);

    /* 
     * AD1_HFL path digital gain 
     */ 
  
    value =  old_value & ~AD_1_HFL_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD1HFL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_hfl_gain
 *
 * RET  : Return the value of register 0xDigitalGainAD1_HFL
 *
 * Notes : From register 0xD4D, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad1_hfl_gain()
  {
    unsigned char value;


    /* 
     * AD1_HFL path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD1HFL_REG) & AD_1_HFL_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_hfl_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD1_HFL
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD1_HFL
 *
 * Notes : From register 0xD4D, bits 6:6
 *
 **************************************************************************/
unsigned char set_ad1_hfl_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD1HFL_REG);

    /* 
     * 0: Fading (anti-click) on AD1_HFL path is active; 
     * 1: Fading (anti-click) on AD1_HFL path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | AD_1_HFL_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ AD_1_HFL_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD1HFL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_hfl_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainAD1_HFL
 *
 * Notes : From register 0xD4D, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad1_hfl_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD1_HFL path is active; 
     * 1: Fading (anti-click) on AD1_HFL path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD1HFL_REG) & AD_1_HFL_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_hfr_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD2_HFR
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD2_HFR
 *
 * Notes : From register 0xD4E, bits 5:0
 *
 **************************************************************************/
unsigned char set_ad2_hfr_gain( enum da_digital_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD2HFR_REG);

    /* 
     * AD2_HFR path digital gain 
     */ 
  
    value =  old_value & ~AD_2_HFR_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD2HFR_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_hfr_gain
 *
 * RET  : Return the value of register 0xDigitalGainAD2_HFR
 *
 * Notes : From register 0xD4E, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad2_hfr_gain()
  {
    unsigned char value;


    /* 
     * AD2_HFR path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD2HFR_REG) & AD_2_HFR_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_hfr_r_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD2_HFR
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainAD2_HFR
 *
 * Notes : From register 0xD4E, bits 6:6
 *
 **************************************************************************/
unsigned char set_ad2_hfr_r_anti_click( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_AD2HFR_REG);

    /* 
     * 0: Fading (anti-click) on AD2_HFR path is active; 
     * 1: Fading (anti-click) on AD2_HFR path is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | AD_2_HFR_R_ANTI_CLICK_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ AD_2_HFR_R_ANTI_CLICK_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD2HFR_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_hfr_r_anti_click
 *
 * RET  : Return the value of register 0xDigitalGainAD2_HFR
 *
 * Notes : From register 0xD4E, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad2_hfr_r_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD2_HFR path is active; 
     * 1: Fading (anti-click) on AD2_HFR path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD2HFR_REG) & AD_2_HFR_R_ANTI_CLICK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_headset_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetLeft
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetLeft
 *
 * Notes : From register 0xD4F, bits 3:0
 *
 **************************************************************************/
unsigned char set_left_headset_gain( enum headset_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_HEADSET_LEFT_REG);

    /* 
     * HSL or Earpiece path digital gain 
     */ 
  
    value =  old_value & ~LEFT_HEADSET_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_HEADSET_LEFT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_headset_gain
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetLeft
 *
 * Notes : From register 0xD4F, bits 3:0
 *
 **************************************************************************/
unsigned char get_left_headset_gain()
  {
    unsigned char value;


    /* 
     * HSL or Earpiece path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_HEADSET_LEFT_REG) & LEFT_HEADSET_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_fading_headset_left
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetLeft
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetLeft
 *
 * Notes : From register 0xD4F, bits 4:4
 *
 **************************************************************************/
unsigned char set_fading_headset_left( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_HEADSET_LEFT_REG);

    /* 
     * 0: Fading (anti-click) on HFL digital gain is active 
     * 1: Fading (anti-click) on HFL digital gain is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | FADING_HEADSET_LEFT_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_HEADSET_LEFT_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_HEADSET_LEFT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fading_headset_left
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetLeft
 *
 * Notes : From register 0xD4F, bits 4:4
 *
 **************************************************************************/
unsigned char get_fading_headset_left()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on HFL digital gain is active 
     * 1: Fading (anti-click) on HFL digital gain is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_HEADSET_LEFT_REG) & FADING_HEADSET_LEFT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_sinc1_interpolator
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetLeft
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetLeft
 *
 * Notes : From register 0xD4F, bits 7:7
 *
 **************************************************************************/
unsigned char set_sinc1_interpolator( enum sinc1_interpolator_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_HEADSET_LEFT_REG);

    /* 
     * 0: sinc3 intepolator choosen for HS or Earpiece drivers 
     * 1: sinc1 (hold) intepolator choosen for HS or Earpiece 
     * driver. 
     */ 
    switch( param ){
        case SINC_1_INTERPOLATOR_ENABLE_E: 
           value = old_value | SINC_1_INTERPOLATOR_MASK; 
           break;
        case SINC_1_INTERPOLATOR_DISABLE_E: 
           value = old_value & ~ SINC_1_INTERPOLATOR_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_HEADSET_LEFT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sinc1_interpolator
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetLeft
 *
 * Notes : From register 0xD4F, bits 7:7
 *
 **************************************************************************/
unsigned char get_sinc1_interpolator()
  {
    unsigned char value;


    /* 
     * 0: sinc3 intepolator choosen for HS or Earpiece drivers 
     * 1: sinc1 (hold) intepolator choosen for HS or Earpiece 
     * driver. 
     */ 
    value = (I2CRead(DIGITAL_GAIN_HEADSET_LEFT_REG) & SINC_1_INTERPOLATOR_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_headset_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetRight
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetRight
 *
 * Notes : From register 0xD50, bits 3:0
 *
 **************************************************************************/
unsigned char set_right_headset_gain( enum headset_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_HEADSET_RIGHT_REG);

    /* 
     * Headset Rigth path digital gain 
     */ 
  
    value =  old_value & ~RIGHT_HEADSET_GAIN_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_HEADSET_RIGHT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_headset_gain
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetRight
 *
 * Notes : From register 0xD50, bits 3:0
 *
 **************************************************************************/
unsigned char get_right_headset_gain()
  {
    unsigned char value;


    /* 
     * Headset Rigth path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_HEADSET_RIGHT_REG) & RIGHT_HEADSET_GAIN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_fading_headset_right
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetRight
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetRight
 *
 * Notes : From register 0xD50, bits 4:4
 *
 **************************************************************************/
unsigned char set_fading_headset_right( enum fading param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_HEADSET_RIGHT_REG);

    /* 
     * 0: Fading (anti-click) on HFR digital gain is active 
     * 1: Fading (anti-click) on HFR digital gain is disabled 
     */ 
    switch( param ){
        case FADING_DISABLE_E: 
           value = old_value | FADING_HEADSET_RIGHT_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_HEADSET_RIGHT_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_HEADSET_RIGHT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fading_headset_right
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetRight
 *
 * Notes : From register 0xD50, bits 4:4
 *
 **************************************************************************/
unsigned char get_fading_headset_right()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on HFR digital gain is active 
     * 1: Fading (anti-click) on HFR digital gain is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_HEADSET_RIGHT_REG) & FADING_HEADSET_RIGHT_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_fade_speed
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetRight
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetRight
 *
 * Notes : From register 0xD50, bits 7:6
 *
 **************************************************************************/
unsigned char set_headset_fade_speed( enum headset_fade_speed param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_HEADSET_RIGHT_REG);

    /* 
     * Approximate Time between each 1dB step Gain Change in 
     * digital gain controls 
     */ 
  

     value =  old_value & ~HEADSET_FADE_SPEED_MASK ;

    switch(  param ){

           case HEADSET_TIME_1MS_E:
                value  = value  | (HEADSET_TIME_1MS << 0x6);
                break;
           case HEADSET_TIME_4MS_E:
                value  = value  | (HEADSET_TIME_4MS << 0x6);
                break;
           case HEADSET_TIME_8MS_E:
                value  = value  | (HEADSET_TIME_8MS << 0x6);
                break;
           case HEADSET_TIME_16MS_E:
                value  = value  | (HEADSET_TIME_16MS << 0x6);
                break;
    }
  

    I2CWrite(DIGITAL_GAIN_HEADSET_RIGHT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_fade_speed
 *
 * RET  : Return the value of register 0xDigitalGainHeadsetRight
 *
 * Notes : From register 0xD50, bits 7:6
 *
 **************************************************************************/
unsigned char get_headset_fade_speed()
  {
    unsigned char value;


    /* 
     * Approximate Time between each 1dB step Gain Change in 
     * digital gain controls 
     */ 
    value = (I2CRead(DIGITAL_GAIN_HEADSET_RIGHT_REG) & HEADSET_FADE_SPEED_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_gain_side_tone1
 *
 * IN   : param, a value to write to the regiter DigitalGainSideTone1
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainSideTone1
 *
 * Notes : From register 0xD51, bits 4:0
 *
 **************************************************************************/
unsigned char set_digital_gain_side_tone1( enum digital_gain_side_tone param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_SIDE_TONE_1_REG);

    /* 
     * Digital gain for side tone 1 path. 
     */ 
  
    value =  old_value & ~DIGITAL_GAIN_SIDE_TONE_1_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_SIDE_TONE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_gain_side_tone1
 *
 * RET  : Return the value of register 0xDigitalGainSideTone1
 *
 * Notes : From register 0xD51, bits 4:0
 *
 **************************************************************************/
unsigned char get_digital_gain_side_tone1()
  {
    unsigned char value;


    /* 
     * Digital gain for side tone 1 path. 
     */ 
    value = (I2CRead(DIGITAL_GAIN_SIDE_TONE_1_REG) & DIGITAL_GAIN_SIDE_TONE_1_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_gain_side_tone2
 *
 * IN   : param, a value to write to the regiter DigitalGainSideTone2
 * OUT  : 
 *
 * RET  : Return the value of register 0xDigitalGainSideTone2
 *
 * Notes : From register 0xD52, bits 4:0
 *
 **************************************************************************/
unsigned char set_digital_gain_side_tone2( enum digital_gain_side_tone param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_GAIN_SIDE_TONE_2_REG);

    /* 
     * Digital gain for side tone 2 path. 
     */ 
  
    value =  old_value & ~DIGITAL_GAIN_SIDE_TONE_2_MASK;


    value |=  param ;  
    I2CWrite(DIGITAL_GAIN_SIDE_TONE_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_gain_side_tone2
 *
 * RET  : Return the value of register 0xDigitalGainSideTone2
 *
 * Notes : From register 0xD52, bits 4:0
 *
 **************************************************************************/
unsigned char get_digital_gain_side_tone2()
  {
    unsigned char value;


    /* 
     * Digital gain for side tone 2 path. 
     */ 
    value = (I2CRead(DIGITAL_GAIN_SIDE_TONE_2_REG) & DIGITAL_GAIN_SIDE_TONE_2_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir_coefficient_pointer
 *
 * IN   : param, a value to write to the regiter AncDigitalGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xAncDigitalGain
 *
 * Notes : From register 0xD53, bits 0:0
 *
 **************************************************************************/
unsigned char set_fir_coefficient_pointer( enum fir_coefficient_pointer param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANC_DIGITAL_GAIN_REG);

    /* 
     * 0: ANC FIR coefficient pointer is resetted.1: ANC FIR 
     * coefficient pointer is removed from reset and IIR 
     * coefficients can be updated. 
     */ 
    switch( param ){
        case POINTER_REMOVED_E: 
           value = old_value | FIR_COEFFICIENT_POINTER_MASK; 
           break;
        case POINTER_RESET_E: 
           value = old_value & ~ FIR_COEFFICIENT_POINTER_MASK;
           break;
    }
  

    I2CWrite(ANC_DIGITAL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir_coefficient_pointer
 *
 * RET  : Return the value of register 0xAncDigitalGain
 *
 * Notes : From register 0xD53, bits 0:0
 *
 **************************************************************************/
unsigned char get_fir_coefficient_pointer()
  {
    unsigned char value;


    /* 
     * 0: ANC FIR coefficient pointer is resetted.1: ANC FIR 
     * coefficient pointer is removed from reset and IIR 
     * coefficients can be updated. 
     */ 
    value = (I2CRead(ANC_DIGITAL_GAIN_REG) & FIR_COEFFICIENT_POINTER_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_iir_status
 *
 * IN   : param, a value to write to the regiter AncDigitalGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xAncDigitalGain
 *
 * Notes : From register 0xD53, bits 1:1
 *
 **************************************************************************/
unsigned char set_iir_status( enum iir_status param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANC_DIGITAL_GAIN_REG);

    /* 
     * 0: IIR initialization cannot start; 
     * 1: Indicate the start of IIR initialization. IIR 
     * coefficients can be written for the first time. 
     */ 
    switch( param ){
        case IIR_STARTED_E: 
           value = old_value | IIR_STATUS_MASK; 
           break;
        case IIR_CANNOT_START_E: 
           value = old_value & ~ IIR_STATUS_MASK;
           break;
    }
  

    I2CWrite(ANC_DIGITAL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_iir_status
 *
 * RET  : Return the value of register 0xAncDigitalGain
 *
 * Notes : From register 0xD53, bits 1:1
 *
 **************************************************************************/
unsigned char get_iir_status()
  {
    unsigned char value;


    /* 
     * 0: IIR initialization cannot start; 
     * 1: Indicate the start of IIR initialization. IIR 
     * coefficients can be written for the first time. 
     */ 
    value = (I2CRead(ANC_DIGITAL_GAIN_REG) & IIR_STATUS_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_operation
 *
 * IN   : param, a value to write to the regiter AncDigitalGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xAncDigitalGain
 *
 * Notes : From register 0xD53, bits 2:2
 *
 **************************************************************************/
unsigned char set_anc_operation( enum anc_operation_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANC_DIGITAL_GAIN_REG);

    /* 
     * 0: ANC operation are disabled. FIR and IIR coefficients 
     * cannot be accessed for writing 
     * 1: ANC operation are enabled 
     */ 
    switch( param ){
        case ANC_OPERATION_ENABLE_E: 
           value = old_value | ANC_OPERATION_MASK; 
           break;
        case ANC_OPERATION_DISABLE_E: 
           value = old_value & ~ ANC_OPERATION_MASK;
           break;
    }
  

    I2CWrite(ANC_DIGITAL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_anc_operation
 *
 * RET  : Return the value of register 0xAncDigitalGain
 *
 * Notes : From register 0xD53, bits 2:2
 *
 **************************************************************************/
unsigned char get_anc_operation()
  {
    unsigned char value;


    /* 
     * 0: ANC operation are disabled. FIR and IIR coefficients 
     * cannot be accessed for writing 
     * 1: ANC operation are enabled 
     */ 
    value = (I2CRead(ANC_DIGITAL_GAIN_REG) & ANC_OPERATION_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_iir_update
 *
 * IN   : param, a value to write to the regiter AncDigitalGain
 * OUT  : 
 *
 * RET  : Return the value of register 0xAncDigitalGain
 *
 * Notes : From register 0xD53, bits 3:3
 *
 **************************************************************************/
unsigned char set_anc_iir_update( enum anc_iir_pointer param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANC_DIGITAL_GAIN_REG);

    /* 
     * 0: ANC IIR coefficient pointer is reset (unless 
     * ANCIIRInit is used). 
     * 1: ANC IIR coefficient pointer is removed from reset and 
     * IIR coefficients can be updated. 
     */ 
    switch( param ){
        case ANC_IIR_POINTER_UPDATED_E: 
           value = old_value | ANC_IIR_UPDATE_MASK; 
           break;
        case ANC_IIR_POINTER_RESET_E: 
           value = old_value & ~ ANC_IIR_UPDATE_MASK;
           break;
    }
  

    I2CWrite(ANC_DIGITAL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_anc_iir_update
 *
 * RET  : Return the value of register 0xAncDigitalGain
 *
 * Notes : From register 0xD53, bits 3:3
 *
 **************************************************************************/
unsigned char get_anc_iir_update()
  {
    unsigned char value;


    /* 
     * 0: ANC IIR coefficient pointer is reset (unless 
     * ANCIIRInit is used). 
     * 1: ANC IIR coefficient pointer is removed from reset and 
     * IIR coefficients can be updated. 
     */ 
    value = (I2CRead(ANC_DIGITAL_GAIN_REG) & ANC_IIR_UPDATE_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_warped_shift_in
 *
 * IN   : param, a value to write to the regiter WarpedDelayLine
 * OUT  : 
 *
 * RET  : Return the value of register 0xWarpedDelayLine
 *
 * Notes : From register 0xD54, bits 4:0
 *
 **************************************************************************/
unsigned char set_warped_shift_in( enum dig_anc_step_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(WARPED_DELAY_LINE_REG);

    /* 
     * Digital gain for Warped Delay Line input. 
     */ 
  
    value =  old_value & ~WARPED_SHIFT_IN_MASK;


    value |=  param ;  
    I2CWrite(WARPED_DELAY_LINE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_warped_shift_in
 *
 * RET  : Return the value of register 0xWarpedDelayLine
 *
 * Notes : From register 0xD54, bits 4:0
 *
 **************************************************************************/
unsigned char get_warped_shift_in()
  {
    unsigned char value;


    /* 
     * Digital gain for Warped Delay Line input. 
     */ 
    value = (I2CRead(WARPED_DELAY_LINE_REG) & WARPED_SHIFT_IN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir_shift_out
 *
 * IN   : param, a value to write to the regiter GainForFirOutput
 * OUT  : 
 *
 * RET  : Return the value of register 0xGainForFirOutput
 *
 * Notes : From register 0xD55, bits 4:0
 *
 **************************************************************************/
unsigned char set_fir_shift_out( enum dig_anc_step_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GAIN_FOR_FIR_OUTPUT_REG);

    /* 
     * Digital gain for FIR output filter. 
     */ 
  
    value =  old_value & ~FIR_SHIFT_OUT_MASK;


    value |=  param ;  
    I2CWrite(GAIN_FOR_FIR_OUTPUT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir_shift_out
 *
 * RET  : Return the value of register 0xGainForFirOutput
 *
 * Notes : From register 0xD55, bits 4:0
 *
 **************************************************************************/
unsigned char get_fir_shift_out()
  {
    unsigned char value;


    /* 
     * Digital gain for FIR output filter. 
     */ 
    value = (I2CRead(GAIN_FOR_FIR_OUTPUT_REG) & FIR_SHIFT_OUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_iir_shift_out
 *
 * IN   : param, a value to write to the regiter GainForIrrOutput
 * OUT  : 
 *
 * RET  : Return the value of register 0xGainForIrrOutput
 *
 * Notes : From register 0xD56, bits 4:0
 *
 **************************************************************************/
unsigned char set_iir_shift_out( enum dig_anc_step_gain param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(GAIN_FOR_IRR_OUTPUT_REG);

    /* 
     * ANC digital gain for IIR output. 
     */ 
  
    value =  old_value & ~IIR_SHIFT_OUT_MASK;


    value |=  param ;  
    I2CWrite(GAIN_FOR_IRR_OUTPUT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_iir_shift_out
 *
 * RET  : Return the value of register 0xGainForIrrOutput
 *
 * Notes : From register 0xD56, bits 4:0
 *
 **************************************************************************/
unsigned char get_iir_shift_out()
  {
    unsigned char value;


    /* 
     * ANC digital gain for IIR output. 
     */ 
    value = (I2CRead(GAIN_FOR_IRR_OUTPUT_REG) & IIR_SHIFT_OUT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_fir_coeff_msb
 *
 * IN   : param, a value to write to the regiter AncFirCoeffMsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xAncFirCoeffMsb
 *
 * Notes : From register 0xD57, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_fir_coeff_msb( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANC_FIR_COEFF_MSB_REG);

    /* 
     * Sets the MSB of the ANC FIR coefficients, the 16 bits 
     * coefficient is updated when the LSB part is written. See 
     * ANCFIRCoeffLSB[7:0] for details 
     */ 
  
    value =  old_value & ~ANC_FIR_COEFF_MSB_MASK;


    value |=  param ;  

    I2CWrite(ANC_FIR_COEFF_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_anc_fir_coeff_msb
 *
 * RET  : Return the value of register 0xAncFirCoeffMsb
 *
 * Notes : From register 0xD57, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_fir_coeff_msb()
  {
    unsigned char value;


    /* 
     * Sets the MSB of the ANC FIR coefficients, the 16 bits 
     * coefficient is updated when the LSB part is written. See 
     * ANCFIRCoeffLSB[7:0] for details 
     */ 
    value = (I2CRead(ANC_FIR_COEFF_MSB_REG) & ANC_FIR_COEFF_MSB_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_anc_fir_coeff_lsb
 *
 * RET  : Return the value of register 0xAncFirCoeffLsb
 *
 * Notes : From register 0xD58, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_fir_coeff_lsb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ANC_FIR_COEFF_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_fir_coeff_lsb
 *
 * IN   : param, a value to write to the regiter AncFirCoeffLsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xAncFirCoeffLsb
 *
 * Notes : From register 0xD58, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_fir_coeff_lsb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(ANC_FIR_COEFF_LSB_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(ANC_FIR_COEFF_LSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_anc_iir_coeff_msb
 *
 * RET  : Return the value of register 0xAncIirCoeffMsb
 *
 * Notes : From register 0xD59, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_iir_coeff_msb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ANC_IIR_COEFF_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_iir_coeff_msb
 *
 * IN   : param, a value to write to the regiter AncIirCoeffMsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xAncIirCoeffMsb
 *
 * Notes : From register 0xD59, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_iir_coeff_msb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(ANC_IIR_COEFF_MSB_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(ANC_IIR_COEFF_MSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_anc_iir_coeff_lsb
 *
 * RET  : Return the value of register 0xAncIirCoeffLsb
 *
 * Notes : From register 0xD5A, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_iir_coeff_lsb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ANC_IIR_COEFF_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_iir_coeff_lsb
 *
 * IN   : param, a value to write to the regiter AncIirCoeffLsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xAncIirCoeffLsb
 *
 * Notes : From register 0xD5A, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_iir_coeff_lsb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(ANC_IIR_COEFF_LSB_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(ANC_IIR_COEFF_LSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_anc_warp_delay_msb
 *
 * RET  : Return the value of register 0xAncWarpDelayMsb
 *
 * Notes : From register 0xD5B, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_warp_delay_msb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ANC_WARP_DELAY_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_warp_delay_msb
 *
 * IN   : param, a value to write to the regiter AncWarpDelayMsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xAncWarpDelayMsb
 *
 * Notes : From register 0xD5B, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_warp_delay_msb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(ANC_WARP_DELAY_MSB_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(ANC_WARP_DELAY_MSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_anc_warp_delay_lsb
 *
 * RET  : Return the value of register 0xAncWarpDelayLsb
 *
 * Notes : From register 0xD5C, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_warp_delay_lsb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ANC_WARP_DELAY_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_warp_delay_lsb
 *
 * IN   : param, a value to write to the regiter AncWarpDelayLsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xAncWarpDelayLsb
 *
 * Notes : From register 0xD5C, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_warp_delay_lsb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(ANC_WARP_DELAY_LSB_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(ANC_WARP_DELAY_LSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_anc_fir_peak_msb
 *
 * RET  : Return the value of register 0xAncFirPeakMsb
 *
 * Notes : From register 0xD5D, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_fir_peak_msb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ANC_FIR_PEAK_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_anc_fir_peak_lsb
 *
 * RET  : Return the value of register 0xAncFirPeakLsb
 *
 * Notes : From register 0xD5E, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_fir_peak_lsb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ANC_FIR_PEAK_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_anc_iir_peak_msb
 *
 * RET  : Return the value of register 0xAncIirPeakMsb
 *
 * Notes : From register 0xD5F, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_iir_peak_msb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ANC_IIR_PEAK_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_anc_iir_peak_lsb
 *
 * RET  : Return the value of register 0xAncIirPeakLsb
 *
 * Notes : From register 0xD60, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_iir_peak_lsb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(ANC_IIR_PEAK_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_address_coefficient
 *
 * RET  : Return the value of register 0xSideToneControl
 *
 * Notes : From register 0xD61, bits 6:0
 *
 **************************************************************************/
unsigned char get_address_coefficient()
  {
    unsigned char value;


    /* 
     * Set the 7 bits address of FIR coefficient that will be 
     * written 
     */ 
    value = (I2CRead(SIDE_TONE_CONTROL_REG) & ADDRESS_COEFFICIENT_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_side_tone_control
 *
 * RET  : Return the value of register 0xSideToneControl
 *
 * Notes : From register 0xD61, bits 7:7
 *
 **************************************************************************/
unsigned char get_side_tone_control()
  {
    unsigned char value;


    /* 
     * 0: Last FIR coefficients not applied; 
     * 1: Applies the last programmed set of FIR coefficients. 
     */ 
    value = (I2CRead(SIDE_TONE_CONTROL_REG) & SIDE_TONE_CONTROL_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_side_tone_coff_msb
 *
 * RET  : Return the value of register 0xSideToneCoffMsb
 *
 * Notes : From register 0xD62, bits 7:0
 *
 **************************************************************************/
unsigned char get_side_tone_coff_msb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(SIDE_TONE_COFF_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_side_tone_coff_msb
 *
 * IN   : param, a value to write to the regiter SideToneCoffMsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xSideToneCoffMsb
 *
 * Notes : From register 0xD62, bits 7:0
 *
 **************************************************************************/
unsigned char set_side_tone_coff_msb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(SIDE_TONE_COFF_MSB_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(SIDE_TONE_COFF_MSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_side_tone_coff_lsb
 *
 * RET  : Return the value of register 0xSideToneCoffLsb
 *
 * Notes : From register 0xD63, bits 7:0
 *
 **************************************************************************/
unsigned char get_side_tone_coff_lsb()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(SIDE_TONE_COFF_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_side_tone_coff_lsb
 *
 * IN   : param, a value to write to the regiter SideToneCoffLsb
 * OUT  : 
 *
 * RET  : Return the value of register 0xSideToneCoffLsb
 *
 * Notes : From register 0xD63, bits 7:0
 *
 **************************************************************************/
unsigned char set_side_tone_coff_lsb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(SIDE_TONE_COFF_LSB_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(SIDE_TONE_COFF_LSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : set_fir_busy
 *
 * IN   : param, a value to write to the regiter FirFiltersControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xFirFiltersControl
 *
 * Notes : From register 0xD64, bits 0:0
 *
 **************************************************************************/
unsigned char set_fir_busy( enum fir_busy param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIR_FILTERS_CONTROL_REG);

    /* 
     * 0: The ST FIR is ready for new coefficients. 
     * 1: The new ST FIR coefficients are not applied yet. 
     */ 
    switch( param ){
        case FIR_BUSY_E: 
           value = old_value | FIR_BUSY_MASK; 
           break;
        case FIR_READY_E: 
           value = old_value & ~ FIR_BUSY_MASK;
           break;
    }
  

    I2CWrite(FIR_FILTERS_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir_busy
 *
 * RET  : Return the value of register 0xFirFiltersControl
 *
 * Notes : From register 0xD64, bits 0:0
 *
 **************************************************************************/
unsigned char get_fir_busy()
  {
    unsigned char value;


    /* 
     * 0: The ST FIR is ready for new coefficients. 
     * 1: The new ST FIR coefficients are not applied yet. 
     */ 
    value = (I2CRead(FIR_FILTERS_CONTROL_REG) & FIR_BUSY_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir_data_rate
 *
 * IN   : param, a value to write to the regiter FirFiltersControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xFirFiltersControl
 *
 * Notes : From register 0xD64, bits 1:1
 *
 **************************************************************************/
unsigned char set_fir_data_rate( enum fir_data_rate param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIR_FILTERS_CONTROL_REG);

    /* 
     * 0: FIR1 and FIR2 Programmable filters run at AUD_IF0 data 
     * rate. 
     * 1: FIR1 and FIR2 Programmable filters run at AUD_IF1 data 
     * rate. 
     */ 
    switch( param ){
        case AUD_IF1_DATA_RATE_E: 
           value = old_value | FIR_DATA_RATE_MASK; 
           break;
        case AUD_IF0_DATA_RATE_E: 
           value = old_value & ~ FIR_DATA_RATE_MASK;
           break;
    }
  

    I2CWrite(FIR_FILTERS_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir_data_rate
 *
 * RET  : Return the value of register 0xFirFiltersControl
 *
 * Notes : From register 0xD64, bits 1:1
 *
 **************************************************************************/
unsigned char get_fir_data_rate()
  {
    unsigned char value;


    /* 
     * 0: FIR1 and FIR2 Programmable filters run at AUD_IF0 data 
     * rate. 
     * 1: FIR1 and FIR2 Programmable filters run at AUD_IF1 data 
     * rate. 
     */ 
    value = (I2CRead(FIR_FILTERS_CONTROL_REG) & FIR_DATA_RATE_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir_filters
 *
 * IN   : param, a value to write to the regiter FirFiltersControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xFirFiltersControl
 *
 * Notes : From register 0xD64, bits 2:2
 *
 **************************************************************************/
unsigned char set_fir_filters( enum fir_filters_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIR_FILTERS_CONTROL_REG);

    /* 
     * 0: ST FIR Filters are disabled. 
     * 1: ST FIR Filters are enabled. 
     */ 
    switch( param ){
        case FIR_FILTERS_ENABLE_E: 
           value = old_value | FIR_FILTERS_MASK; 
           break;
        case FIR_FILTERS_DISABLE_E: 
           value = old_value & ~ FIR_FILTERS_MASK;
           break;
    }
  

    I2CWrite(FIR_FILTERS_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir_filters
 *
 * RET  : Return the value of register 0xFirFiltersControl
 *
 * Notes : From register 0xD64, bits 2:2
 *
 **************************************************************************/
unsigned char get_fir_filters()
  {
    unsigned char value;


    /* 
     * 0: ST FIR Filters are disabled. 
     * 1: ST FIR Filters are enabled. 
     */ 
    value = (I2CRead(FIR_FILTERS_CONTROL_REG) & FIR_FILTERS_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_fir
 *
 * IN   : param, a value to write to the regiter InterruptEnable0
 * OUT  : 
 *
 * RET  : Return the value of register 0xInterruptEnable0
 *
 * Notes : From register 0xD65, bits 0:0
 *
 **************************************************************************/
unsigned char set_interrupt_fir( enum fir_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_0_REG);

    /* 
     * 0: Bit FIR_EV cannot generate an Interrupt Request; 
     * 1: Bit FIR_EV can generate an Interrupt Request 
     */ 
    switch( param ){
        case FIR_ENABLE_E: 
           value = old_value | INTERRUPT_FIR_MASK; 
           break;
        case FIR_DISABLE_E: 
           value = old_value & ~ INTERRUPT_FIR_MASK;
           break;
    }
  

    I2CWrite(INTERRUPT_ENABLE_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_fir
 *
 * RET  : Return the value of register 0xInterruptEnable0
 *
 * Notes : From register 0xD65, bits 0:0
 *
 **************************************************************************/
unsigned char get_interrupt_fir()
  {
    unsigned char value;


    /* 
     * 0: Bit FIR_EV cannot generate an Interrupt Request; 
     * 1: Bit FIR_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_0_REG) & INTERRUPT_FIR_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_short_circuit_detected_ear
 *
 * IN   : param, a value to write to the regiter InterruptEnable1
 * OUT  : 
 *
 * RET  : Return the value of register 0xInterruptEnable1
 *
 * Notes : From register 0xD67, bits 0:0
 *
 **************************************************************************/
unsigned char set_interrupt_short_circuit_detected_ear( enum interrupt_short_circuit_detected_ear_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_1_REG);

    /* 
     * 0: Bit SHRTEAR_EV cannot generate an Interrupt Request; 
     * 1: Bit SHRTEAR_EV can generate an Interrupt Request 
     */ 
    switch( param ){
        case INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_ENABLE_E: 
           value = old_value | INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_MASK; 
           break;
        case INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_DISABLE_E: 
           value = old_value & ~ INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_MASK;
           break;
    }
  

    I2CWrite(INTERRUPT_ENABLE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_short_circuit_detected_ear
 *
 * RET  : Return the value of register 0xInterruptEnable1
 *
 * Notes : From register 0xD67, bits 0:0
 *
 **************************************************************************/
unsigned char get_interrupt_short_circuit_detected_ear()
  {
    unsigned char value;


    /* 
     * 0: Bit SHRTEAR_EV cannot generate an Interrupt Request; 
     * 1: Bit SHRTEAR_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_1_REG) & INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_short_circuit_detected_hsr
 *
 * IN   : param, a value to write to the regiter InterruptEnable1
 * OUT  : 
 *
 * RET  : Return the value of register 0xInterruptEnable1
 *
 * Notes : From register 0xD67, bits 1:1
 *
 **************************************************************************/
unsigned char set_interrupt_short_circuit_detected_hsr( enum interrupt_short_circuit_hsr_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_1_REG);

    /* 
     * 0: Bit SHRTHSR_EV cannot generate an Interrupt Request; 
     * 1: Bit SHRTHSR_EV can generate an Interrupt Request 
     */ 
    switch( param ){
        case INTERRUPT_SHORT_CIRCUIT_HSR_ENABLE_E: 
           value = old_value | INTERRUPT_SHORT_CIRCUIT_DETECTED_HSR_MASK; 
           break;
        case INTERRUPT_SHORT_CIRCUIT_HSR_DISABLE_E: 
           value = old_value & ~ INTERRUPT_SHORT_CIRCUIT_DETECTED_HSR_MASK;
           break;
    }
  

    I2CWrite(INTERRUPT_ENABLE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_short_circuit_detected_hsr
 *
 * RET  : Return the value of register 0xInterruptEnable1
 *
 * Notes : From register 0xD67, bits 1:1
 *
 **************************************************************************/
unsigned char get_interrupt_short_circuit_detected_hsr()
  {
    unsigned char value;


    /* 
     * 0: Bit SHRTHSR_EV cannot generate an Interrupt Request; 
     * 1: Bit SHRTHSR_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_1_REG) & INTERRUPT_SHORT_CIRCUIT_DETECTED_HSR_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_short_circuit_detected_hsl
 *
 * IN   : param, a value to write to the regiter InterruptEnable1
 * OUT  : 
 *
 * RET  : Return the value of register 0xInterruptEnable1
 *
 * Notes : From register 0xD67, bits 2:2
 *
 **************************************************************************/
unsigned char set_interrupt_short_circuit_detected_hsl( enum interrupt_short_circuit_hsl_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_1_REG);

    /* 
     * 0: Bit SHRTHSL_EV cannot generate an Interrupt Request; 
     * 1: Bit SHRTHSL_EV can generate an Interrupt Request 
     */ 
    switch( param ){
        case INTERRUPT_SHORT_CIRCUIT_HSL_ENABLE_E: 
           value = old_value | INTERRUPT_SHORT_CIRCUIT_DETECTED_HSL_MASK; 
           break;
        case INTERRUPT_SHORT_CIRCUIT_HSL_DISABLE_E: 
           value = old_value & ~ INTERRUPT_SHORT_CIRCUIT_DETECTED_HSL_MASK;
           break;
    }
  

    I2CWrite(INTERRUPT_ENABLE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_short_circuit_detected_hsl
 *
 * RET  : Return the value of register 0xInterruptEnable1
 *
 * Notes : From register 0xD67, bits 2:2
 *
 **************************************************************************/
unsigned char get_interrupt_short_circuit_detected_hsl()
  {
    unsigned char value;


    /* 
     * 0: Bit SHRTHSL_EV cannot generate an Interrupt Request; 
     * 1: Bit SHRTHSL_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_1_REG) & INTERRUPT_SHORT_CIRCUIT_DETECTED_HSL_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_vss_ready
 *
 * IN   : param, a value to write to the regiter InterruptEnable1
 * OUT  : 
 *
 * RET  : Return the value of register 0xInterruptEnable1
 *
 * Notes : From register 0xD67, bits 7:7
 *
 **************************************************************************/
unsigned char set_interrupt_vss_ready( enum interrupt_vss_ready_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_1_REG);

    /* 
     * 0: Bit BFIFULL_EV cannot generate an Interrupt Request; 
     * 1: Bit BFIFULL_EV can generate an Interrupt Request 
     */ 
    switch( param ){
        case INTERRUPT_VSS_READY_ENABLE_E: 
           value = old_value | INTERRUPT_VSS_READY_MASK; 
           break;
        case INTERRUPT_VSS_READY_DISABLE_E: 
           value = old_value & ~ INTERRUPT_VSS_READY_MASK;
           break;
    }
  

    I2CWrite(INTERRUPT_ENABLE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_vss_ready
 *
 * RET  : Return the value of register 0xInterruptEnable1
 *
 * Notes : From register 0xD67, bits 7:7
 *
 **************************************************************************/
unsigned char get_interrupt_vss_ready()
  {
    unsigned char value;


    /* 
     * 0: Bit BFIFULL_EV cannot generate an Interrupt Request; 
     * 1: Bit BFIFULL_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_1_REG) & INTERRUPT_VSS_READY_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_wakeup_signal_level
 *
 * IN   : param, a value to write to the regiter FifoInterruptControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xFifoInterruptControl
 *
 * Notes : From register 0xD69, bits 5:0
 *
 **************************************************************************/
unsigned char set_wakeup_signal_level( enum wakeup_signal_level param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIFO_INTERRUPT_CONTROL_REG);

    /* 
     * Wakeup signal is generated when burst 
     */ 
  

     value =  old_value & ~WAKEUP_SIGNAL_LEVEL_MASK ;

    switch(  param ){

           case NO_WAKEUP_SIGNAL_E:
                value =  value | NO_WAKEUP_SIGNAL ;
                break;
           case FIFO_8_SAMPLES_E:
                value =  value | FIFO_8_SAMPLES ;
                break;
           case FIFO_16_SAMPLES_E:
                value =  value | FIFO_16_SAMPLES ;
                break;
           case FIFO_32_SAMPLES_E:
                value =  value | FIFO_32_SAMPLES ;
                break;
           case FIFO_64_SAMPLES_E:
                value =  value | FIFO_64_SAMPLES ;
                break;
           case FIFO_128_SAMPLES_E:
                value =  value | FIFO_128_SAMPLES ;
                break;
           case FIFO_256_SAMPLES_E:
                value =  value | FIFO_256_SAMPLES ;
                break;
           case FIFO_508_SAMPLES_E:
                value =  value | FIFO_508_SAMPLES ;
                break;
    }
  

    I2CWrite(FIFO_INTERRUPT_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_wakeup_signal_level
 *
 * RET  : Return the value of register 0xFifoInterruptControl
 *
 * Notes : From register 0xD69, bits 5:0
 *
 **************************************************************************/
unsigned char get_wakeup_signal_level()
  {
    unsigned char value;


    /* 
     * Wakeup signal is generated when burst 
     */ 
    value = (I2CRead(FIFO_INTERRUPT_CONTROL_REG) & WAKEUP_SIGNAL_LEVEL_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_burst_bit_clock
 *
 * IN   : param, a value to write to the regiter FifoInterruptControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xFifoInterruptControl
 *
 * Notes : From register 0xD69, bits 6:6
 *
 **************************************************************************/
unsigned char set_burst_bit_clock( enum burst_frequency param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIFO_INTERRUPT_CONTROL_REG);

    /* 
     * 0: BitClk0 frequency in burst mode is 38.4 MHz 
     * 1: BitClk0 frequency in burst mode is 19.2 MHz 
     */ 
    switch( param ){
        case BUSRT_FREQUENCY_19_2_MHZ_E: 
           value = old_value | BURST_BIT_CLOCK_MASK; 
           break;
        case BUSRT_FREQUENCY_38_4_MHZ_E: 
           value = old_value & ~ BURST_BIT_CLOCK_MASK;
           break;
    }
  

    I2CWrite(FIFO_INTERRUPT_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_burst_bit_clock
 *
 * RET  : Return the value of register 0xFifoInterruptControl
 *
 * Notes : From register 0xD69, bits 6:6
 *
 **************************************************************************/
unsigned char get_burst_bit_clock()
  {
    unsigned char value;


    /* 
     * 0: BitClk0 frequency in burst mode is 38.4 MHz 
     * 1: BitClk0 frequency in burst mode is 19.2 MHz 
     */ 
    value = (I2CRead(FIFO_INTERRUPT_CONTROL_REG) & BURST_BIT_CLOCK_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_mask
 *
 * IN   : param, a value to write to the regiter FifoInterruptControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xFifoInterruptControl
 *
 * Notes : From register 0xD69, bits 7:7
 *
 **************************************************************************/
unsigned char set_fifo_mask( enum fifo_mask_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIFO_INTERRUPT_CONTROL_REG);

    /* 
     * 0: AD_DATA0 is unmasked in burst mode, it can wake up the 
     * digital audio data source 
     * 1: AD_DATA0 is masked in burst mode, it cannot wake up 
     * the digital audio data source 
     */ 
    switch( param ){
        case FIFO_MASK_ENABLE_E: 
           value = old_value | FIFO_MASK_MASK; 
           break;
        case FIFO_MASK_DISABLE_E: 
           value = old_value & ~ FIFO_MASK_MASK;
           break;
    }
  

    I2CWrite(FIFO_INTERRUPT_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fifo_mask
 *
 * RET  : Return the value of register 0xFifoInterruptControl
 *
 * Notes : From register 0xD69, bits 7:7
 *
 **************************************************************************/
unsigned char get_fifo_mask()
  {
    unsigned char value;


    /* 
     * 0: AD_DATA0 is unmasked in burst mode, it can wake up the 
     * digital audio data source 
     * 1: AD_DATA0 is masked in burst mode, it cannot wake up 
     * the digital audio data source 
     */ 
    value = (I2CRead(FIFO_INTERRUPT_CONTROL_REG) & FIFO_MASK_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_burst_length
 *
 * IN   : param, a value to write to the regiter FifoLength
 * OUT  : 
 *
 * RET  : Return the value of register 0xFifoLength
 *
 * Notes : From register 0xD6A, bits 7:0
 *
 **************************************************************************/
unsigned char set_fifo_burst_length( enum fifo_burst_length param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIFO_LENGTH_REG);

    /* 
     * Burst FIFO length register. It define the burst tranfer 
     * length in master mode. 
     */ 
  

     value =  old_value & ~FIFO_BURST_LENGTH_MASK ;

    switch(  param ){

           case FIFO_BURST_LEN_NO_DATA_E:
                value =  value | FIFO_BURST_LEN_NO_DATA ;
                break;
           case FIFO_BURST_LEN_8_SAMPLES_E:
                value =  value | FIFO_BURST_LEN_8_SAMPLES ;
                break;
           case FIFO_BURST_LEN_16_SAMPLES_E:
                value =  value | FIFO_BURST_LEN_16_SAMPLES ;
                break;
           case FIFO_BURST_LEN_32_SAMPLES_E:
                value =  value | FIFO_BURST_LEN_32_SAMPLES ;
                break;
           case FIFO_BURST_LEN_64_SAMPLES_E:
                value =  value | FIFO_BURST_LEN_64_SAMPLES ;
                break;
           case FIFO_BURST_LEN_128_SAMPLES_E:
                value =  value | FIFO_BURST_LEN_128_SAMPLES ;
                break;
           case FIFO_BURST_LEN_256_SAMPLES_E:
                value =  value | FIFO_BURST_LEN_256_SAMPLES ;
                break;
           case FIFO_BURST_LEN_512_SAMPLES_E:
                value =  value | FIFO_BURST_LEN_512_SAMPLES ;
                break;
           case FIFO_BURST_LEN_1024_SAMPLES_E:
                value =  value | FIFO_BURST_LEN_1024_SAMPLES ;
                break;
           case FIFO_BURST_LEN_1536_SAMPLES_E:
                value =  value | FIFO_BURST_LEN_1536_SAMPLES ;
                break;
           case FIFO_BURST_LEN_2044_SAMPLES_E:
                value =  value | FIFO_BURST_LEN_2044_SAMPLES ;
                break;
    }
  

    I2CWrite(FIFO_LENGTH_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fifo_burst_length
 *
 * RET  : Return the value of register 0xFifoLength
 *
 * Notes : From register 0xD6A, bits 7:0
 *
 **************************************************************************/
unsigned char get_fifo_burst_length()
  {
    unsigned char value;


    /* 
     * Burst FIFO length register. It define the burst tranfer 
     * length in master mode. 
     */ 
    value = (I2CRead(FIFO_LENGTH_REG) & FIFO_BURST_LENGTH_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_bust_mode_run
 *
 * IN   : param, a value to write to the regiter FifoControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xFifoControl
 *
 * Notes : From register 0xD6B, bits 0:0
 *
 **************************************************************************/
unsigned char set_bust_mode_run( enum bust_mode_run param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIFO_CONTROL_REG);

    /* 
     * 0: Burst FIFO interface is stopped 
     * 1: Burst FIFO interface is running 
     */ 
    switch( param ){
        case BURST_MODE_RUNNING_E: 
           value = old_value | BUST_MODE_RUN_MASK; 
           break;
        case BURST_MODE_STOPPED_E: 
           value = old_value & ~ BUST_MODE_RUN_MASK;
           break;
    }
  

    I2CWrite(FIFO_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_bust_mode_run
 *
 * RET  : Return the value of register 0xFifoControl
 *
 * Notes : From register 0xD6B, bits 0:0
 *
 **************************************************************************/
unsigned char get_bust_mode_run()
  {
    unsigned char value;


    /* 
     * 0: Burst FIFO interface is stopped 
     * 1: Burst FIFO interface is running 
     */ 
    value = (I2CRead(FIFO_CONTROL_REG) & BUST_MODE_RUN_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_master_fifo_mode
 *
 * IN   : param, a value to write to the regiter FifoControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xFifoControl
 *
 * Notes : From register 0xD6B, bits 1:1
 *
 **************************************************************************/
unsigned char set_master_fifo_mode( enum master_fifo_mode_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIFO_CONTROL_REG);

    /* 
     * 0: Burst FIFO interface is configured in slave mode 
     * 1: Burst FIFO interface is configured in master mode 
     */ 
    switch( param ){
        case MASTER_FIFO_MODE_ENABLE_E: 
           value = old_value | MASTER_FIFO_MODE_MASK; 
           break;
        case MASTER_FIFO_MODE_DISABLE_E: 
           value = old_value & ~ MASTER_FIFO_MODE_MASK;
           break;
    }
  

    I2CWrite(FIFO_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_master_fifo_mode
 *
 * RET  : Return the value of register 0xFifoControl
 *
 * Notes : From register 0xD6B, bits 1:1
 *
 **************************************************************************/
unsigned char get_master_fifo_mode()
  {
    unsigned char value;


    /* 
     * 0: Burst FIFO interface is configured in slave mode 
     * 1: Burst FIFO interface is configured in master mode 
     */ 
    value = (I2CRead(FIFO_CONTROL_REG) & MASTER_FIFO_MODE_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_number_extra_clocks
 *
 * IN   : param, a value to write to the regiter FifoControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xFifoControl
 *
 * Notes : From register 0xD6B, bits 4:2
 *
 **************************************************************************/
unsigned char set_number_extra_clocks( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIFO_CONTROL_REG);

    /* 
     * Number of extra bit clocks that precede and follow the 
     * first Frame Sync on Aud_IF0 startup or Burst startup 
     */ 
  
    value =  old_value & ~NUMBER_EXTRA_CLOCKS_MASK;


    value |= ( param << 0x2);  

    I2CWrite(FIFO_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_number_extra_clocks
 *
 * RET  : Return the value of register 0xFifoControl
 *
 * Notes : From register 0xD6B, bits 4:2
 *
 **************************************************************************/
unsigned char get_number_extra_clocks()
  {
    unsigned char value;


    /* 
     * Number of extra bit clocks that precede and follow the 
     * first Frame Sync on Aud_IF0 startup or Burst startup 
     */ 
    value = (I2CRead(FIFO_CONTROL_REG) & NUMBER_EXTRA_CLOCKS_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_extra_clock
 *
 * IN   : param, a value to write to the regiter FifoControl
 * OUT  : 
 *
 * RET  : Return the value of register 0xFifoControl
 *
 * Notes : From register 0xD6B, bits 7:5
 *
 **************************************************************************/
unsigned char set_fifo_extra_clock( enum fifo_extra_clock param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIFO_CONTROL_REG);

    /* 
     * Number of extra clock in burst mode at the end of each 
     * frame 
     */ 
  

     value =  old_value & ~FIFO_EXTRA_CLOCK_MASK ;

    switch(  param ){

           case NO_EXTRA_CLOCK_E:
                value  = value  | (NO_EXTRA_CLOCK << 0x5);
                break;
           case MODE_1_SLOT_EXTRA_E:
                value  = value  | (MODE_1_SLOT_EXTRA << 0x5);
                break;
           case MODE_2_SLOT_EXTRA_E:
                value  = value  | (MODE_2_SLOT_EXTRA << 0x5);
                break;
           case MODE_3_SLOT_EXTRA_E:
                value  = value  | (MODE_3_SLOT_EXTRA << 0x5);
                break;
           case MODE_4_SLOT_EXTRA_E:
                value  = value  | (MODE_4_SLOT_EXTRA << 0x5);
                break;
           case MODE_5_SLOT_EXTRA_E:
                value  = value  | (MODE_5_SLOT_EXTRA << 0x5);
                break;
           case MODE_6_SLOT_EXTRA_E:
                value  = value  | (MODE_6_SLOT_EXTRA << 0x5);
                break;
    }
  

    I2CWrite(FIFO_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fifo_extra_clock
 *
 * RET  : Return the value of register 0xFifoControl
 *
 * Notes : From register 0xD6B, bits 7:5
 *
 **************************************************************************/
unsigned char get_fifo_extra_clock()
  {
    unsigned char value;


    /* 
     * Number of extra clock in burst mode at the end of each 
     * frame 
     */ 
    value = (I2CRead(FIFO_CONTROL_REG) & FIFO_EXTRA_CLOCK_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_fifo_switch_frame
 *
 * RET  : Return the value of register 0xFifoSwitchFrame
 *
 * Notes : From register 0xD6C, bits 7:0
 *
 **************************************************************************/
unsigned char get_fifo_switch_frame()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(FIFO_SWITCH_FRAME_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_switch_frame
 *
 * IN   : param, a value to write to the regiter FifoSwitchFrame
 * OUT  : 
 *
 * RET  : Return the value of register 0xFifoSwitchFrame
 *
 * Notes : From register 0xD6C, bits 7:0
 *
 **************************************************************************/
unsigned char set_fifo_switch_frame( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(FIFO_SWITCH_FRAME_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(FIFO_SWITCH_FRAME_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_fifo_wake_up_delay
 *
 * RET  : Return the value of register 0xFifoWakeUpDelay
 *
 * Notes : From register 0xD6D, bits 7:0
 *
 **************************************************************************/
unsigned char get_fifo_wake_up_delay()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(FIFO_WAKE_UP_DELAY_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_wake_up_delay
 *
 * IN   : param, a value to write to the regiter FifoWakeUpDelay
 * OUT  : 
 *
 * RET  : Return the value of register 0xFifoWakeUpDelay
 *
 * Notes : From register 0xD6D, bits 7:0
 *
 **************************************************************************/
unsigned char set_fifo_wake_up_delay( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(FIFO_WAKE_UP_DELAY_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(FIFO_WAKE_UP_DELAY_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_fifo_samples_number
 *
 * RET  : Return the value of register 0xFifoSamplesNumber
 *
 * Notes : From register 0xD6E, bits 7:0
 *
 **************************************************************************/
unsigned char get_fifo_samples_number()
  {
    unsigned char value;


    /* 
     * Get the value from a register 
     */ 
   value = I2CRead(FIFO_SAMPLES_NUMBER_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_samples_number
 *
 * IN   : param, a value to write to the regiter FifoSamplesNumber
 * OUT  : 
 *
 * RET  : Return the value of register 0xFifoSamplesNumber
 *
 * Notes : From register 0xD6E, bits 7:0
 *
 **************************************************************************/
unsigned char set_fifo_samples_number( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(FIFO_SAMPLES_NUMBER_REG);

    /* 
     * Set the value to a register 
     */ 
    I2CWrite(FIFO_SAMPLES_NUMBER_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_ab9540_revison
 *
 * RET  : Return the value of register 0xRevision
 *
 * Notes : From register 0xD6F, bits 4:0
 *
 **************************************************************************/
unsigned char get_ab9540_revison()
  {
    unsigned char value;


    /* 
     * Audio module revision 01001: AB9540 cut 1.0 
     */ 
    value = (I2CRead(REVISION_REG) & AB_9540_REVISON_MASK);
    return value;
  }




#ifdef __cplusplus
}
#endif

#endif
