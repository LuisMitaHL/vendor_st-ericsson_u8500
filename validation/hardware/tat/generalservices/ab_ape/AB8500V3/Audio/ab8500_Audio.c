/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Audio/Linux/ab8500_Audio.c
 * 
 *
 * Generated on the 25/05/2011 07:53 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyright STEricsson  2011
 *
 *  Reference document : STw4500 Registers Specification Rev 2.61 5 January 2011
 *
 ***************************************************************************/
#include "ab8500_Audio.h"

extern unsigned char I2CRead( unsigned short register_address);
extern void I2CWrite( unsigned short register_address, unsigned char data );


/***************************************************************************
 *
 * Function : set_digital_power_up
 *
 * IN   : param, a value to write to the regiter PowerUpControl
 * OUT  : 
 *
 * RET  : Return the value of register PowerUpControl
 *
 * Notes : From register 0x0D00, bits 7:7
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
           value = old_value | DIGITAL_POWER_UP_PARAM_MASK; 
           break;
        case DIGITAL_POWER_DOWN_E: 
           value = old_value & ~ DIGITAL_POWER_UP_PARAM_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_power_up
 *
 * RET  : Return the value of register PowerUpControl
 *
 * Notes : From register 0x0D00, bits 7:7
 *
 **************************************************************************/
unsigned char get_digital_power_up()
  {
    unsigned char value;


    /* 
     * 0: The audio feature is in power down 
     * 1: The audio feature is in power up 
     */ 
    value = (I2CRead(POWER_UP_CONTROL_REG) & DIGITAL_POWER_UP_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_analog_power_up
 *
 * IN   : param, a value to write to the regiter PowerUpControl
 * OUT  : 
 *
 * RET  : Return the value of register PowerUpControl
 *
 * Notes : From register 0x0D00, bits 3:3
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
           value = old_value | ANALOG_POWER_UP_PARAM_MASK; 
           break;
        case ANALOG_POWER_DOWN_E: 
           value = old_value & ~ ANALOG_POWER_UP_PARAM_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_power_up
 *
 * RET  : Return the value of register PowerUpControl
 *
 * Notes : From register 0x0D00, bits 3:3
 *
 **************************************************************************/
unsigned char get_analog_power_up()
  {
    unsigned char value;


    /* 
     * 0: All the audio analog parts are in power down 
     * 1: All the audio analog parts are in power up 
     */ 
    value = (I2CRead(POWER_UP_CONTROL_REG) & ANALOG_POWER_UP_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_audio_software_reset
 *
 * IN   : param, a value to write to the regiter SoftwareReset
 * OUT  : 
 *
 * RET  : Return the value of register SoftwareReset
 *
 * Notes : From register 0x0D01, bits 7:7
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
           value = old_value | AUDIO_SOFTWARE_RESET_PARAM_MASK; 
           break;
        case AUDIO_SOFTWARE_UN_RESET_E: 
           value = old_value & ~ AUDIO_SOFTWARE_RESET_PARAM_MASK;
           break;
    }
  

    I2CWrite(SOFTWARE_RESET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_audio_software_reset
 *
 * RET  : Return the value of register SoftwareReset
 *
 * Notes : From register 0x0D01, bits 7:7
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
    value = (I2CRead(SOFTWARE_RESET_REG) & AUDIO_SOFTWARE_RESET_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad12_digital_audio_paths_enable
 *
 * IN   : param, a value to write to the regiter DigitalChannelsEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalChannelsEnable
 *
 * Notes : From register 0x0D02, bits 7:7
 *
 **************************************************************************/
unsigned char set_ad12_digital_audio_paths_enable( enum ad12_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_CHANNELS_ENABLE_REG);

    /* 
     * 0: AD1 & AD2 paths are disabled 
     * 1: AD1 & AD2 paths are enabled 
     */ 
    switch( param ){
        case AD_12_ENABLE_E: 
           value = old_value | AD_12_ENABLE_PARAM_MASK; 
           break;
        case AD_12_DISABLE_E: 
           value = old_value & ~ AD_12_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_CHANNELS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad12_digital_audio_paths_enable
 *
 * RET  : Return the value of register DigitalChannelsEnable
 *
 * Notes : From register 0x0D02, bits 7:7
 *
 **************************************************************************/
unsigned char get_ad12_digital_audio_paths_enable()
  {
    unsigned char value;


    /* 
     * 0: AD1 & AD2 paths are disabled 
     * 1: AD1 & AD2 paths are enabled 
     */ 
    value = (I2CRead(DIGITAL_CHANNELS_ENABLE_REG) & AD_12_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad34_digital_audio_paths_enable
 *
 * IN   : param, a value to write to the regiter DigitalChannelsEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalChannelsEnable
 *
 * Notes : From register 0x0D02, bits 5:5
 *
 **************************************************************************/
unsigned char set_ad34_digital_audio_paths_enable( enum ad34_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_CHANNELS_ENABLE_REG);

    /* 
     * 0: AD3 & AD4 paths are disabled 
     * 1: AD3 & AD4 paths are enabled 
     */ 
    switch( param ){
        case AD_34_ENABLE_E: 
           value = old_value | AD_34_ENABLE_PARAM_MASK; 
           break;
        case AD_34_DISABLE_E: 
           value = old_value & ~ AD_34_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_CHANNELS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad34_digital_audio_paths_enable
 *
 * RET  : Return the value of register DigitalChannelsEnable
 *
 * Notes : From register 0x0D02, bits 5:5
 *
 **************************************************************************/
unsigned char get_ad34_digital_audio_paths_enable()
  {
    unsigned char value;


    /* 
     * 0: AD3 & AD4 paths are disabled 
     * 1: AD3 & AD4 paths are enabled 
     */ 
    value = (I2CRead(DIGITAL_CHANNELS_ENABLE_REG) & AD_34_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad5768_digital_audio_paths_enable
 *
 * IN   : param, a value to write to the regiter DigitalChannelsEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalChannelsEnable
 *
 * Notes : From register 0x0D02, bits 3:3
 *
 **************************************************************************/
unsigned char set_ad5768_digital_audio_paths_enable( enum ad5678_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_CHANNELS_ENABLE_REG);

    /* 
     * 0: AD5/7 & AD6/8 paths are disabled 
     * 1: AD5/7 & AD6/8 paths are enabled 
     */ 
    switch( param ){
        case AD_5678_ENABLE_E: 
           value = old_value | AD_5678_ENABLE_PARAM_MASK; 
           break;
        case AD_5678_DISABLE_E: 
           value = old_value & ~ AD_5678_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_CHANNELS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad5768_digital_audio_paths_enable
 *
 * RET  : Return the value of register DigitalChannelsEnable
 *
 * Notes : From register 0x0D02, bits 3:3
 *
 **************************************************************************/
unsigned char get_ad5768_digital_audio_paths_enable()
  {
    unsigned char value;


    /* 
     * 0: AD5/7 & AD6/8 paths are disabled 
     * 1: AD5/7 & AD6/8 paths are enabled 
     */ 
    value = (I2CRead(DIGITAL_CHANNELS_ENABLE_REG) & AD_5678_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da1_digital_audio_paths_enable
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 7:7
 *
 **************************************************************************/
unsigned char set_da1_digital_audio_paths_enable( enum da1_enable param )
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
           value = old_value | DA_1_ENABLE_PARAM_MASK; 
           break;
        case DA_1_DISABLE_E: 
           value = old_value & ~ DA_1_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da1_digital_audio_paths_enable
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 7:7
 *
 **************************************************************************/
unsigned char get_da1_digital_audio_paths_enable()
  {
    unsigned char value;


    /* 
     * 0: DA1 path is disabled 
     * 1: DA1 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_1_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da2_digital_audio_paths_enable
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 6:6
 *
 **************************************************************************/
unsigned char set_da2_digital_audio_paths_enable( enum da2_enable param )
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
           value = old_value | DA_2_ENABLE_PARAM_MASK; 
           break;
        case DA_2_DISABLE_E: 
           value = old_value & ~ DA_2_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da2_digital_audio_paths_enable
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 6:6
 *
 **************************************************************************/
unsigned char get_da2_digital_audio_paths_enable()
  {
    unsigned char value;


    /* 
     * 0: DA2 path is disabled 
     * 1: DA2 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_2_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da3_digital_audio_paths_enable
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 5:5
 *
 **************************************************************************/
unsigned char set_da3_digital_audio_paths_enable( enum da3_enable param )
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
           value = old_value | DA_3_ENABLE_PARAM_MASK; 
           break;
        case DA_3_DISABLE_E: 
           value = old_value & ~ DA_3_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da3_digital_audio_paths_enable
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 5:5
 *
 **************************************************************************/
unsigned char get_da3_digital_audio_paths_enable()
  {
    unsigned char value;


    /* 
     * 0: DA3 path is disabled 
     * 1: DA3 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_3_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da4_digital_audio_paths_enable
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 4:4
 *
 **************************************************************************/
unsigned char set_da4_digital_audio_paths_enable( enum da4_enable param )
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
           value = old_value | DA_4_ENABLE_PARAM_MASK; 
           break;
        case DA_4_DISABLE_E: 
           value = old_value & ~ DA_4_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da4_digital_audio_paths_enable
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 4:4
 *
 **************************************************************************/
unsigned char get_da4_digital_audio_paths_enable()
  {
    unsigned char value;


    /* 
     * 0: DA4 path is disabled 
     * 1: DA4 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_4_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da5_digital_audio_paths_enable
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 3:3
 *
 **************************************************************************/
unsigned char set_da5_digital_audio_paths_enable( enum da5_enable param )
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
           value = old_value | DA_5_ENABLE_PARAM_MASK; 
           break;
        case DA_5_DISABLE_E: 
           value = old_value & ~ DA_5_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da5_digital_audio_paths_enable
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 3:3
 *
 **************************************************************************/
unsigned char get_da5_digital_audio_paths_enable()
  {
    unsigned char value;


    /* 
     * 0: DA5 path is disabled 
     * 1: DA5 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_5_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da6_digital_audio_paths_enable
 *
 * IN   : param, a value to write to the regiter DigitalDaAudioPaths
 * OUT  : 
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 2:2
 *
 **************************************************************************/
unsigned char set_da6_digital_audio_paths_enable( enum da6_enable param )
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
           value = old_value | DA_6_ENABLE_PARAM_MASK; 
           break;
        case DA_6_DISABLE_E: 
           value = old_value & ~ DA_6_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_DA_AUDIO_PATHS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da6_digital_audio_paths_enable
 *
 * RET  : Return the value of register DigitalDaAudioPaths
 *
 * Notes : From register 0x0D03, bits 2:2
 *
 **************************************************************************/
unsigned char get_da6_digital_audio_paths_enable()
  {
    unsigned char value;


    /* 
     * 0: DA6 path is disabled 
     * 1: DA6 path is enabled 
     */ 
    value = (I2CRead(DIGITAL_DA_AUDIO_PATHS_REG) & DA_6_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_headset
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 7:7
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
           value = old_value | AUDIO_LOW_POWER_MODE_PARAM_MASK; 
           break;
        case NORMAL_OPERATION_E: 
           value = old_value & ~ AUDIO_LOW_POWER_MODE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_D_MIC_CONTROLS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_power_headset
 *
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 7:7
 *
 **************************************************************************/
unsigned char get_low_power_headset()
  {
    unsigned char value;


    /* 
     * Low Power Headset 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & AUDIO_LOW_POWER_MODE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_dac_headset
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 6:5
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
  

     value =  old_value & ~LOW_POWER_DAC_HEADSET_PARAM_MASK ;

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
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 6:5
 *
 **************************************************************************/
unsigned char get_low_power_dac_headset()
  {
    unsigned char value;


    /* 
     * Low Power DAC headset 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & LOW_POWER_DAC_HEADSET_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_ear
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 4:4
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
           value = old_value | LOW_POWER_EAR_PARAM_MASK; 
           break;
        case NORMAL_OPERATION_EAR_E: 
           value = old_value & ~ LOW_POWER_EAR_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_D_MIC_CONTROLS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_power_ear
 *
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 4:4
 *
 **************************************************************************/
unsigned char get_low_power_ear()
  {
    unsigned char value;


    /* 
     * Low Power Ear 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & LOW_POWER_EAR_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_selection
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 3:2
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
  

     value =  old_value & ~EAR_SELECTION_PARAM_MASK ;

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
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 3:2
 *
 **************************************************************************/
unsigned char get_ear_selection()
  {
    unsigned char value;


    /* 
     * Ear Selection Common Mode 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & EAR_SELECTION_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_offset_cancelation
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 1:1
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
           value = old_value | HEADSET_OFFSET_CANCELATION_PARAM_MASK; 
           break;
        case HEADSET_OFFSET_CANCELATION_OFF_E: 
           value = old_value & ~ HEADSET_OFFSET_CANCELATION_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_D_MIC_CONTROLS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_offset_cancelation
 *
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 1:1
 *
 **************************************************************************/
unsigned char get_headset_offset_cancelation()
  {
    unsigned char value;


    /* 
     * Offset cancellation for headset driver. 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & HEADSET_OFFSET_CANCELATION_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_driver_low_power
 *
 * IN   : param, a value to write to the regiter AnalogDMicControls
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 0:0
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
           value = old_value | EAR_DRIVER_ENABLE_PARAM_MASK; 
           break;
        case EAR_DRIVER_DISABLE_E: 
           value = old_value & ~ EAR_DRIVER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_D_MIC_CONTROLS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_driver_low_power
 *
 * RET  : Return the value of register AnalogDMicControls
 *
 * Notes : From register 0x0D04, bits 0:0
 *
 **************************************************************************/
unsigned char get_ear_driver_low_power()
  {
    unsigned char value;


    /* 
     * 0: Normal Operation 
     * 1: Ear DAC driver in Low Power 
     */ 
    value = (I2CRead(ANALOG_D_MIC_CONTROLS_REG) & EAR_DRIVER_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_micro1_power
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 7:7
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
           value = old_value | MICRO_1_POWER_PARAM_MASK; 
           break;
        case MICRO_1_POWER_DOWN_E: 
           value = old_value & ~ MICRO_1_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_micro1_power
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 7:7
 *
 **************************************************************************/
unsigned char get_micro1_power()
  {
    unsigned char value;


    /* 
     * Micro 1a and 1b Enable (power up) 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & MICRO_1_POWER_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_micro2_power
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 6:6
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
           value = old_value | MICRO_2_POWER_PARAM_MASK; 
           break;
        case MICRO_2_POWER_DOWN_E: 
           value = old_value & ~ MICRO_2_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_micro2_power
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 6:6
 *
 **************************************************************************/
unsigned char get_micro2_power()
  {
    unsigned char value;


    /* 
     * 0: Mic2 microphone is in power down 
     * 1: Mic2 microphone is in power up 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & MICRO_2_POWER_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_line_in_left_power
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 5:5
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
           value = old_value | LINE_IN_LEFT_POWER_PARAM_MASK; 
           break;
        case LINE_IN_LEFT_POWER_DOWN_E: 
           value = old_value & ~ LINE_IN_LEFT_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_line_in_left_power
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 5:5
 *
 **************************************************************************/
unsigned char get_line_in_left_power()
  {
    unsigned char value;


    /* 
     * 0: Line-in left input is in power down 
     * 1: Line-in left input is in power up 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & LINE_IN_LEFT_POWER_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_line_in_right_power
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 4:4
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
           value = old_value | LINE_IN_RIGHT_POWER_PARAM_MASK; 
           break;
        case LINE_IN_RIGHT_POWER_DOWN_E: 
           value = old_value & ~ LINE_IN_RIGHT_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_line_in_right_power
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 4:4
 *
 **************************************************************************/
unsigned char get_line_in_right_power()
  {
    unsigned char value;


    /* 
     * 0: Line-in right input is in power down 
     * 1: Line-in left input is in power up 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & LINE_IN_RIGHT_POWER_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_analog_micro1_mute
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 3:3
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
           value = old_value | ANALOG_MICRO_1_MUTE_PARAM_MASK; 
           break;
        case ANALOG_MICRO_1_UNMUTE_E: 
           value = old_value & ~ ANALOG_MICRO_1_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_micro1_mute
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 3:3
 *
 **************************************************************************/
unsigned char get_analog_micro1_mute()
  {
    unsigned char value;


    /* 
     * Analog micropnone MIC1A/B is active 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & ANALOG_MICRO_1_MUTE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_analog_micro2_mute
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 2:2
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
           value = old_value | ANALOG_MICRO_2_MUTE_PARAM_MASK; 
           break;
        case ANALOG_MICRO_2_UNMUTE_E: 
           value = old_value & ~ ANALOG_MICRO_2_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_micro2_mute
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 2:2
 *
 **************************************************************************/
unsigned char get_analog_micro2_mute()
  {
    unsigned char value;


    /* 
     * Analog micropnone MIC2 is active 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & ANALOG_MICRO_2_MUTE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_analog_line_in_left_mute
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 1:1
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
           value = old_value | ANALOG_LINE_IN_LEFT_MUTE_PARAM_MASK; 
           break;
        case ANALOG_LINE_IN_LEFT_UNMUTE_E: 
           value = old_value & ~ ANALOG_LINE_IN_LEFT_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_line_in_left_mute
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 1:1
 *
 **************************************************************************/
unsigned char get_analog_line_in_left_mute()
  {
    unsigned char value;


    /* 
     * Analog line in Left is unmuted 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & ANALOG_LINE_IN_LEFT_MUTE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_analog_line_in_right_mute
 *
 * IN   : param, a value to write to the regiter LineInConf
 * OUT  : 
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 0:0
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
           value = old_value | ANALOG_LINE_IN_RIGHT_MUTE_PARAM_MASK; 
           break;
        case ANALOG_LINE_IN_RIGHT_UNMUTE_E: 
           value = old_value & ~ ANALOG_LINE_IN_RIGHT_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(LINE_IN_CONF_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_analog_line_in_right_mute
 *
 * RET  : Return the value of register LineInConf
 *
 * Notes : From register 0x0D05, bits 0:0
 *
 **************************************************************************/
unsigned char get_analog_line_in_right_mute()
  {
    unsigned char value;


    /* 
     * Analog line in Right is active 
     */ 
    value = (I2CRead(LINE_IN_CONF_REG) & ANALOG_LINE_IN_RIGHT_MUTE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro1_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 7:7
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
        case DIGITAL_1_MICRO_UNMUTE_E: 
           value = old_value | DIGITAL_1_MICRO_MUTE_PARAM_MASK; 
           break;
        case DIGITAL_1_MICRO_MUTE_E: 
           value = old_value & ~ DIGITAL_1_MICRO_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro1_mute
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 7:7
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
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_1_MICRO_MUTE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro2_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 6:6
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
        case DIGITAL_2_MICRO_UNMUTE_E: 
           value = old_value | DIGITAL_2_MICRO_MUTE_PARAM_MASK; 
           break;
        case DIGITAL_2_MICRO_MUTE_E: 
           value = old_value & ~ DIGITAL_2_MICRO_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro2_mute
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 6:6
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
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_2_MICRO_MUTE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro3_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 5:5
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
        case DIGITAL_3_MICRO_UNMUTE_E: 
           value = old_value | DIGITAL_3_MICRO_MUTE_PARAM_MASK; 
           break;
        case DIGITAL_3_MICRO_MUTE_E: 
           value = old_value & ~ DIGITAL_3_MICRO_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro3_mute
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 5:5
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
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_3_MICRO_MUTE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro4_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 4:4
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
        case DIGITAL_4_MICRO_UNMUTE_E: 
           value = old_value | DIGITAL_4_MICRO_MUTE_PARAM_MASK; 
           break;
        case DIGITAL_4_MICRO_MUTE_E: 
           value = old_value & ~ DIGITAL_4_MICRO_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro4_mute
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 4:4
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
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_4_MICRO_MUTE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro5_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 3:3
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
        case DIGITAL_5_MICRO_UNMUTE_E: 
           value = old_value | DIGITAL_5_MICRO_MUTE_PARAM_MASK; 
           break;
        case DIGITAL_5_MICRO_MUTE_E: 
           value = old_value & ~ DIGITAL_5_MICRO_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro5_mute
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 3:3
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
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_5_MICRO_MUTE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_micro6_mute
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 2:2
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
        case DIGITAL_6_MICRO_UNMUTE_E: 
           value = old_value | DIGITAL_6_MICRO_MUTE_PARAM_MASK; 
           break;
        case DIGITAL_6_MICRO_MUTE_E: 
           value = old_value & ~ DIGITAL_6_MICRO_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_INPUTS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_micro6_mute
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 2:2
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
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & DIGITAL_6_MICRO_MUTE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fade_speed
 *
 * IN   : param, a value to write to the regiter DigitalInputsEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 1:0
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
  

     value =  old_value & ~FADE_SPEED_PARAM_MASK ;

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
 * RET  : Return the value of register DigitalInputsEnable
 *
 * Notes : From register 0x0D06, bits 1:0
 *
 **************************************************************************/
unsigned char get_fade_speed()
  {
    unsigned char value;


    /* 
     * Approximate Time between each 1dB step Gain Change in 
     * digital gain controls 
     */ 
    value = (I2CRead(DIGITAL_INPUTS_ENABLE_REG) & FADE_SPEED_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc1_connected_to_mic
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 7:7
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
           value = old_value | ADC1_CONNECTED_TO_MIC_A_PARAM_MASK; 
           break;
        case CONNECTED_TO_MIC1A_E: 
           value = old_value & ~ ADC1_CONNECTED_TO_MIC_A_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc1_connected_to_mic
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 7:7
 *
 **************************************************************************/
unsigned char get_adc1_connected_to_mic()
  {
    unsigned char value;


    /* 
     * 1: connected to MIC1B, 0 : connected to MIC1A 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & ADC1_CONNECTED_TO_MIC_A_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc2_connected_to_mic
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 6:6
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
           value = old_value | ADC2_CONNECTED_TO_MIC_PARAM_MASK; 
           break;
        case CONNECTED_TO_MIC2_E: 
           value = old_value & ~ ADC2_CONNECTED_TO_MIC_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc2_connected_to_mic
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 6:6
 *
 **************************************************************************/
unsigned char get_adc2_connected_to_mic()
  {
    unsigned char value;


    /* 
     * 0: connected to MIC2B, 1 : connected to LINR 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & ADC2_CONNECTED_TO_MIC_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hsl_dac_driver_power
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 5:5
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
           value = old_value | HSL_DAC_DRIVER_POWER_PARAM_MASK; 
           break;
        case HSL_DAC_DRIVER_POWER_DOWN_E: 
           value = old_value & ~ HSL_DAC_DRIVER_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hsl_dac_driver_power
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 5:5
 *
 **************************************************************************/
unsigned char get_hsl_dac_driver_power()
  {
    unsigned char value;


    /* 
     * HeadSetLelf Dac driver is power up 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & HSL_DAC_DRIVER_POWER_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hsr_dac_driver_power
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 4:4
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
           value = old_value | HSR_DAC_DRIVER_POWER_PARAM_MASK; 
           break;
        case HSR_DAC_DRIVER_POWER_DOWN_E: 
           value = old_value & ~ HSR_DAC_DRIVER_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hsr_dac_driver_power
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 4:4
 *
 **************************************************************************/
unsigned char get_hsr_dac_driver_power()
  {
    unsigned char value;


    /* 
     * HeadSetRight Dac driver is power up 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & HSR_DAC_DRIVER_POWER_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc3_power
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 2:2
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
           value = old_value | ADC_3_POWER_PARAM_MASK; 
           break;
        case ADC_3_POWER_DOWN_E: 
           value = old_value & ~ ADC_3_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc3_power
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 2:2
 *
 **************************************************************************/
unsigned char get_adc3_power()
  {
    unsigned char value;


    /* 
     * 0: ADC3 is power down 
     * 1: ADC3 is power-up 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & ADC_3_POWER_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc1_power
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 1:1
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
           value = old_value | ADC_1_POWER_PARAM_MASK; 
           break;
        case ADC_1_POWER_DOWN_E: 
           value = old_value & ~ ADC_1_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc1_power
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 1:1
 *
 **************************************************************************/
unsigned char get_adc1_power()
  {
    unsigned char value;


    /* 
     * 0: ADC1 is power down 
     * 1: ADC1 is power-up 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & ADC_1_POWER_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_adc2_power
 *
 * IN   : param, a value to write to the regiter AnalogDigitalConvertersEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 0:0
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
           value = old_value | ADC_2_POWER_PARAM_MASK; 
           break;
        case ADC_2_POWER_DOWN_E: 
           value = old_value & ~ ADC_2_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_adc2_power
 *
 * RET  : Return the value of register AnalogDigitalConvertersEnable
 *
 * Notes : From register 0x0D07, bits 0:0
 *
 **************************************************************************/
unsigned char get_adc2_power()
  {
    unsigned char value;


    /* 
     * 0: ADC2 is power down 
     * 1: ADC2 is power-up 
     */ 
    value = (I2CRead(ANALOG_DIGITAL_CONVERTERS_ENABLE_REG) & ADC_2_POWER_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_internal_pull_down_enabled
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 7:7
 *
 **************************************************************************/
unsigned char set_internal_pull_down_enabled( enum internal_pull_down_enabled param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_ENABLE_REG);

    /* 
     * 0: Internal pull down on VSS is enabled. 
     * 1: Internal pull down on VSS is disabled 
     */ 
    switch( param ){
        case INTERNAL_PULL_DOWN_DISABLE_E: 
           value = old_value | INTERNAL_PULL_DOWN_ENABLED_PARAM_MASK; 
           break;
        case INTERNAL_PULL_DOWN_ENABLE_E: 
           value = old_value & ~ INTERNAL_PULL_DOWN_ENABLED_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_internal_pull_down_enabled
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 7:7
 *
 **************************************************************************/
unsigned char get_internal_pull_down_enabled()
  {
    unsigned char value;


    /* 
     * 0: Internal pull down on VSS is enabled. 
     * 1: Internal pull down on VSS is disabled 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & INTERNAL_PULL_DOWN_ENABLED_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_class_ab_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 6:6
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
           value = old_value | EAR_CLASS_AB_POWER_PARAM_MASK; 
           break;
        case EAR_CLASS_AB_POWER_DOWN_E: 
           value = old_value & ~ EAR_CLASS_AB_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_class_ab_power
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 6:6
 *
 **************************************************************************/
unsigned char get_ear_class_ab_power()
  {
    unsigned char value;


    /* 
     * 1: The EAR Class-AB driver is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & EAR_CLASS_AB_POWER_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_left_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 5:5
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
           value = old_value | HEADSET_LEFT_POWER_PARAM_MASK; 
           break;
        case HEADSET_LEFT_POWER_DOWN_E: 
           value = old_value & ~ HEADSET_LEFT_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_left_power
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 5:5
 *
 **************************************************************************/
unsigned char get_headset_left_power()
  {
    unsigned char value;


    /* 
     * 1: The Headset Left is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & HEADSET_LEFT_POWER_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_right_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 4:4
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
           value = old_value | HEADSET_RIGHT_POWER_PARAM_MASK; 
           break;
        case HEADSET_RIGHT_POWER_DOWN_E: 
           value = old_value & ~ HEADSET_RIGHT_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_right_power
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 4:4
 *
 **************************************************************************/
unsigned char get_headset_right_power()
  {
    unsigned char value;


    /* 
     * 1: The Headset Right is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & HEADSET_RIGHT_POWER_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hf_class_d_left_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 3:3
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
           value = old_value | HF_LEFT_CLASS_D_POWER_PARAM_MASK; 
           break;
        case HF_LEFT_CLASS_D_POWER_DOWN_E: 
           value = old_value & ~ HF_LEFT_CLASS_D_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hf_class_d_left_power
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 3:3
 *
 **************************************************************************/
unsigned char get_hf_class_d_left_power()
  {
    unsigned char value;


    /* 
     * 1: The HFL Class-D driver is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & HF_LEFT_CLASS_D_POWER_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hf_class_d_rigth_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 2:2
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
           value = old_value | HF_RIGHT_CLASS_D_POWER_PARAM_MASK; 
           break;
        case HF_RIGHT_CLASS_D_POWER_DOWN_E: 
           value = old_value & ~ HF_RIGHT_CLASS_D_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hf_class_d_rigth_power
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 2:2
 *
 **************************************************************************/
unsigned char get_hf_class_d_rigth_power()
  {
    unsigned char value;


    /* 
     * 1: The HFR Class-D driver is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & HF_RIGHT_CLASS_D_POWER_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vibrator1_class_d_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 1:1
 *
 **************************************************************************/
unsigned char set_vibrator1_class_d_power( enum vibrator1_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_ENABLE_REG);

    /* 
     * 1: The Vibrator1 Class-D driver is powered up 
     */ 
    switch( param ){
        case VIBRATOR_1_POWER_UP_E: 
           value = old_value | VIBRATOR_1_POWER_PARAM_MASK; 
           break;
        case VIBRATOR_1_POWER_DOWN_E: 
           value = old_value & ~ VIBRATOR_1_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vibrator1_class_d_power
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 1:1
 *
 **************************************************************************/
unsigned char get_vibrator1_class_d_power()
  {
    unsigned char value;


    /* 
     * 1: The Vibrator1 Class-D driver is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & VIBRATOR_1_POWER_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vibrator2_class_d_power
 *
 * IN   : param, a value to write to the regiter AnalogOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 0:0
 *
 **************************************************************************/
unsigned char set_vibrator2_class_d_power( enum vibrator2_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_OUTPUT_ENABLE_REG);

    /* 
     * 1: The Vibrator2 Class-D driver is powered up 
     */ 
    switch( param ){
        case VIBRATOR_2_POWER_UP_E: 
           value = old_value | VIBRATOR_2_POWER_PARAM_MASK; 
           break;
        case VIBRATOR_2_POWER_DOWN_E: 
           value = old_value & ~ VIBRATOR_2_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vibrator2_class_d_power
 *
 * RET  : Return the value of register AnalogOutputEnable
 *
 * Notes : From register 0x0D08, bits 0:0
 *
 **************************************************************************/
unsigned char get_vibrator2_class_d_power()
  {
    unsigned char value;


    /* 
     * 1: The Vibrator2 Class-D driver is powered up 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_ENABLE_REG) & VIBRATOR_2_POWER_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_frequency_lim_enable
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 7:7
 *
 **************************************************************************/
unsigned char set_low_frequency_lim_enable( enum low_frequency_lim_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_OUTPUT_ENABLE_REG);

    /* 
     */ 
    switch( param ){
        case LOW_FREQUENCY_LIM_ENABLE_E: 
           value = old_value | LOW_FREQUENCY_LIM_ENABLE_PARAM_MASK; 
           break;
        case LOW_FREQUENCY_LIM_DISABLE_E: 
           value = old_value & ~ LOW_FREQUENCY_LIM_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_frequency_lim_enable
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 7:7
 *
 **************************************************************************/
unsigned char get_low_frequency_lim_enable()
  {
    unsigned char value;


    /* 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & LOW_FREQUENCY_LIM_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_and_digital_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 6:6
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
           value = old_value | EAR_AND_DIGITAL_PATH_POWER_PARAM_MASK; 
           break;
        case EAR_AND_DIGITAL_PATH_POWER_DOWN_E: 
           value = old_value & ~ EAR_AND_DIGITAL_PATH_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_and_digital_path_power
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 6:6
 *
 **************************************************************************/
unsigned char get_ear_and_digital_path_power()
  {
    unsigned char value;


    /* 
     * 1: EAR DAC and digital dedicated path are powered up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & EAR_AND_DIGITAL_PATH_POWER_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_left_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 5:5
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
           value = old_value | HEADSET_LEFT_PATH_POWER_PARAM_MASK; 
           break;
        case HEADSET_LEFT_PATH_POWER_DOWN_E: 
           value = old_value & ~ HEADSET_LEFT_PATH_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_left_path_power
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 5:5
 *
 **************************************************************************/
unsigned char get_headset_left_path_power()
  {
    unsigned char value;


    /* 
     * 1: HSL DAC and digital dedicated path are powered up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & HEADSET_LEFT_PATH_POWER_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_right_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 4:4
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
           value = old_value | HEADSET_RIGHT_PATH_POWER_PARAM_MASK; 
           break;
        case HEADSET_RIGHT_PATH_POWER_DOWN_E: 
           value = old_value & ~ HEADSET_RIGHT_PATH_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_right_path_power
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 4:4
 *
 **************************************************************************/
unsigned char get_headset_right_path_power()
  {
    unsigned char value;


    /* 
     * 1: HSR DAC and digital dedicated path are powered up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & HEADSET_RIGHT_PATH_POWER_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hf_class_d_left_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 3:3
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
           value = old_value | HF_CLASS_D_LEFT_PATH_POWER_PARAM_MASK; 
           break;
        case HF_CLASS_D_LEFT_PATH_POWER_DOWN_E: 
           value = old_value & ~ HF_CLASS_D_LEFT_PATH_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hf_class_d_left_path_power
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 3:3
 *
 **************************************************************************/
unsigned char get_hf_class_d_left_path_power()
  {
    unsigned char value;


    /* 
     * 1 :The HFL Class-D and digital dedicated path are powered 
     * up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & HF_CLASS_D_LEFT_PATH_POWER_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_hf_class_d_rigth_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 2:2
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
           value = old_value | HF_CLASS_D_RIGTH_PATH_POWER_PARAM_MASK; 
           break;
        case HF_CLASS_D_RIGTH_PATH_POWER_DOWN_E: 
           value = old_value & ~ HF_CLASS_D_RIGTH_PATH_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hf_class_d_rigth_path_power
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 2:2
 *
 **************************************************************************/
unsigned char get_hf_class_d_rigth_path_power()
  {
    unsigned char value;


    /* 
     * 1: The HFR Class-D end digital dedicated path are powered 
     * up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & HF_CLASS_D_RIGTH_PATH_POWER_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vibrator1_class_d_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 1:1
 *
 **************************************************************************/
unsigned char set_vibrator1_class_d_path_power( enum vibrator1_class_d_path_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_OUTPUT_ENABLE_REG);

    /* 
     * 1: The Vibrator 1 and digital dedicated path are powered 
     * up 
     */ 
    switch( param ){
        case VIBRATOR_1_CLASS_D_PATH_POWER_UP_E: 
           value = old_value | VIBRATOR_1_CLASS_D_PATH_POWER_PARAM_MASK; 
           break;
        case VIBRATOR_1_CLASS_D_PATH_POWER_DOWN_E: 
           value = old_value & ~ VIBRATOR_1_CLASS_D_PATH_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vibrator1_class_d_path_power
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 1:1
 *
 **************************************************************************/
unsigned char get_vibrator1_class_d_path_power()
  {
    unsigned char value;


    /* 
     * 1: The Vibrator 1 and digital dedicated path are powered 
     * up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & VIBRATOR_1_CLASS_D_PATH_POWER_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vibrator2_class_d_path_power
 *
 * IN   : param, a value to write to the regiter DigitalOutputEnable
 * OUT  : 
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 0:0
 *
 **************************************************************************/
unsigned char set_vibrator2_class_d_path_power( enum vibrator2_class_d_path_power param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_OUTPUT_ENABLE_REG);

    /* 
     * 1: The Vibrator 2 and digital dedicated path are powered 
     * up 
     */ 
    switch( param ){
        case VIBRATOR_2_CLASS_D_PATH_POWER_UP_E: 
           value = old_value | VIBRATOR_2_CLASS_D_PATH_POWER_PARAM_MASK; 
           break;
        case VIBRATOR_2_CLASS_D_PATH_POWER_DOWN_E: 
           value = old_value & ~ VIBRATOR_2_CLASS_D_PATH_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_OUTPUT_ENABLE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vibrator2_class_d_path_power
 *
 * RET  : Return the value of register DigitalOutputEnable
 *
 * Notes : From register 0x0D09, bits 0:0
 *
 **************************************************************************/
unsigned char get_vibrator2_class_d_path_power()
  {
    unsigned char value;


    /* 
     * 1: The Vibrator 2 and digital dedicated path are powered 
     * up 
     */ 
    value = (I2CRead(DIGITAL_OUTPUT_ENABLE_REG) & VIBRATOR_2_CLASS_D_PATH_POWER_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_class_ab_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 6:6
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
           value = old_value | EAR_CLASS_AB_MUTE_PARAM_MASK; 
           break;
        case EAR_CLASS_AB_UNMUTE_E: 
           value = old_value & ~ EAR_CLASS_AB_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_class_ab_mute
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 6:6
 *
 **************************************************************************/
unsigned char get_ear_class_ab_mute()
  {
    unsigned char value;


    /* 
     * 1: The EAR Class-AB driver is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & EAR_CLASS_AB_MUTE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_left_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 5:5
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
           value = old_value | HEADSET_LEFT_MUTE_PARAM_MASK; 
           break;
        case HEADSET_LEFT_UNMUTE_E: 
           value = old_value & ~ HEADSET_LEFT_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_left_mute
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 5:5
 *
 **************************************************************************/
unsigned char get_headset_left_mute()
  {
    unsigned char value;


    /* 
     * 1: The Headset Left is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & HEADSET_LEFT_MUTE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_right_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 4:4
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
           value = old_value | HEADSET_RIGHT_MUTE_PARAM_MASK; 
           break;
        case HEADSET_RIGHT_UNMUTE_E: 
           value = old_value & ~ HEADSET_RIGHT_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_right_mute
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 4:4
 *
 **************************************************************************/
unsigned char get_headset_right_mute()
  {
    unsigned char value;


    /* 
     * 1: The Headset Right is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & HEADSET_RIGHT_MUTE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_dac_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 2:2
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
           value = old_value | EAR_DAC_MUTE_PARAM_MASK; 
           break;
        case EAR_DAC_UNMUTE_E: 
           value = old_value & ~ EAR_DAC_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_dac_mute
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 2:2
 *
 **************************************************************************/
unsigned char get_ear_dac_mute()
  {
    unsigned char value;


    /* 
     * 0: The Ear DAC is in normal mode 
     * 1: The Ear DAC is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & EAR_DAC_MUTE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_left_dac_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 1:1
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
           value = old_value | HEADSET_LEFT_DAC_MUTE_PARAM_MASK; 
           break;
        case HEADSET_LEFT_DAC_UNMUTE_E: 
           value = old_value & ~ HEADSET_LEFT_DAC_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_left_dac_mute
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 1:1
 *
 **************************************************************************/
unsigned char get_headset_left_dac_mute()
  {
    unsigned char value;


    /* 
     * 0: The HsL DAC is in normal mode 
     * 1: The HsL DAC is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & HEADSET_LEFT_DAC_MUTE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_right_dac_mute
 *
 * IN   : param, a value to write to the regiter AnalogOutputMute
 * OUT  : 
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 0:0
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
           value = old_value | HEADSET_RIGHT_DAC_MUTE_PARAM_MASK; 
           break;
        case HEADSET_RIGHT_DAC_UNMUTE_E: 
           value = old_value & ~ HEADSET_RIGHT_DAC_MUTE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_OUTPUT_MUTE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_right_dac_mute
 *
 * RET  : Return the value of register AnalogOutputMute
 *
 * Notes : From register 0x0D0A, bits 0:0
 *
 **************************************************************************/
unsigned char get_headset_right_dac_mute()
  {
    unsigned char value;


    /* 
     * 0: The HsR DAC is in normal mode 
     * 1: The HsR DAC is muted 
     */ 
    value = (I2CRead(ANALOG_OUTPUT_MUTE_REG) & HEADSET_RIGHT_DAC_MUTE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_auto_switch_off_enable
 *
 * IN   : param, a value to write to the regiter ShortCircuitDetection
 * OUT  : 
 *
 * RET  : Return the value of register ShortCircuitDetection
 *
 * Notes : From register 0x0D0B, bits 7:7
 *
 **************************************************************************/
unsigned char set_ear_auto_switch_off_enable( enum switch_off_enable param )
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
           value = old_value | SWITCH_OFF_ENABLE_PARAM_MASK; 
           break;
        case SWITCH_OFF_DISABLE_E: 
           value = old_value & ~ SWITCH_OFF_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(SHORT_CIRCUIT_DETECTION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_auto_switch_off_enable
 *
 * RET  : Return the value of register ShortCircuitDetection
 *
 * Notes : From register 0x0D0B, bits 7:7
 *
 **************************************************************************/
unsigned char get_ear_auto_switch_off_enable()
  {
    unsigned char value;


    /* 
     * This bit act on Earpiece and Headset drive. 
     * 1: Automatic switch off on short circuit detection is 
     * enabled 
     */ 
    value = (I2CRead(SHORT_CIRCUIT_DETECTION_REG) & SWITCH_OFF_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_detection_enable
 *
 * IN   : param, a value to write to the regiter ShortCircuitDetection
 * OUT  : 
 *
 * RET  : Return the value of register ShortCircuitDetection
 *
 * Notes : From register 0x0D0B, bits 6:6
 *
 **************************************************************************/
unsigned char set_ear_detection_enable( enum ear_detection_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SHORT_CIRCUIT_DETECTION_REG);

    /* 
     * 1: Short circuit detected on EAR driver enabled 
     */ 
    switch( param ){
        case EAR_DETECTION_DISABLE_E: 
           value = old_value | EAR_DETECTION_ENABLE_PARAM_MASK; 
           break;
        case EAR_DETECTION_ENABLE_E: 
           value = old_value & ~ EAR_DETECTION_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(SHORT_CIRCUIT_DETECTION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_detection_enable
 *
 * RET  : Return the value of register ShortCircuitDetection
 *
 * Notes : From register 0x0D0B, bits 6:6
 *
 **************************************************************************/
unsigned char get_ear_detection_enable()
  {
    unsigned char value;


    /* 
     * 1: Short circuit detected on EAR driver enabled 
     */ 
    value = (I2CRead(SHORT_CIRCUIT_DETECTION_REG) & EAR_DETECTION_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_detection_enable
 *
 * IN   : param, a value to write to the regiter ShortCircuitDetection
 * OUT  : 
 *
 * RET  : Return the value of register ShortCircuitDetection
 *
 * Notes : From register 0x0D0B, bits 5:5
 *
 **************************************************************************/
unsigned char set_headset_detection_enable( enum headset_detection_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SHORT_CIRCUIT_DETECTION_REG);

    /* 
     * 1: Short circuit detected on headset Left and Right is 
     * enabled 
     */ 
    switch( param ){
        case HEADSET_DETECTION_DISABLE_E: 
           value = old_value | HEADSET_DETECTION_ENABLE_PARAM_MASK; 
           break;
        case HEADSET_DETECTION_ENABLE_E: 
           value = old_value & ~ HEADSET_DETECTION_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(SHORT_CIRCUIT_DETECTION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_detection_enable
 *
 * RET  : Return the value of register ShortCircuitDetection
 *
 * Notes : From register 0x0D0B, bits 5:5
 *
 **************************************************************************/
unsigned char get_headset_detection_enable()
  {
    unsigned char value;


    /* 
     * 1: Short circuit detected on headset Left and Right is 
     * enabled 
     */ 
    value = (I2CRead(SHORT_CIRCUIT_DETECTION_REG) & HEADSET_DETECTION_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_pull_down_enable
 *
 * IN   : param, a value to write to the regiter ShortCircuitDetection
 * OUT  : 
 *
 * RET  : Return the value of register ShortCircuitDetection
 *
 * Notes : From register 0x0D0B, bits 4:4
 *
 **************************************************************************/
unsigned char set_headset_pull_down_enable( enum headset_pull_down_enable param )
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
           value = old_value | HEADSET_PULL_DOWN_ENABLE_PARAM_MASK; 
           break;
        case HEADSET_PULL_DOWN_DISABLE_E: 
           value = old_value & ~ HEADSET_PULL_DOWN_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(SHORT_CIRCUIT_DETECTION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_headset_pull_down_enable
 *
 * RET  : Return the value of register ShortCircuitDetection
 *
 * Notes : From register 0x0D0B, bits 4:4
 *
 **************************************************************************/
unsigned char get_headset_pull_down_enable()
  {
    unsigned char value;


    /* 
     * 1: HSL and HSR outputs are pulled down to ground 
     * 0: HSL and HSR outputs are in high impedance 
     */ 
    value = (I2CRead(SHORT_CIRCUIT_DETECTION_REG) & HEADSET_PULL_DOWN_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_charge_pump_enable
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 7:7
 *
 **************************************************************************/
unsigned char set_charge_pump_enable( enum charge_pump_enable param )
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
           value = old_value | CHARGE_PUMP_ENABLE_PARAM_MASK; 
           break;
        case CHARGE_PUMP_DISABLE_E: 
           value = old_value & ~ CHARGE_PUMP_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_charge_pump_enable
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 7:7
 *
 **************************************************************************/
unsigned char get_charge_pump_enable()
  {
    unsigned char value;


    /* 
     * 0: Charge Pump disabled. If HSAUTOENSEL=1 also HSAUTOEN 
     * must me resetted. 
     * 1: Charge Pump enabled. 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & CHARGE_PUMP_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_line_out_input
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 5:5
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
           value = old_value | LEFT_LINE_INPUT_PARAM_MASK; 
           break;
        case LEFT_LINEOUT_INPUT_EAD_DAC_E: 
           value = old_value & ~ LEFT_LINE_INPUT_PARAM_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_line_out_input
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 5:5
 *
 **************************************************************************/
unsigned char get_left_line_out_input()
  {
    unsigned char value;


    /* 
     * 0: Left Line-Out driver input is Ear-DAC. 
     * 1: Left Line-Out driver input is HsL-DAC. 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & LEFT_LINE_INPUT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_line_out_input
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 4:4
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
           value = old_value | RIGHT_LINE_INPUT_PARAM_MASK; 
           break;
        case RIGTH_LINEOUT_INPUT_EAD_DAC_E: 
           value = old_value & ~ RIGHT_LINE_INPUT_PARAM_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_line_out_input
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 4:4
 *
 **************************************************************************/
unsigned char get_right_line_out_input()
  {
    unsigned char value;


    /* 
     * 0: Right Line-Out driver input is Ear-DAC. 
     * 1: Right Line-Out driver input is HsR-DAC. 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & RIGHT_LINE_INPUT_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_line_out_driver
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 3:3
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
           value = old_value | LEFT_LINE_OUT_ENABLE_PARAM_MASK; 
           break;
        case LEFT_LINE_OUT_DISABLE_E: 
           value = old_value & ~ LEFT_LINE_OUT_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_line_out_driver
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 3:3
 *
 **************************************************************************/
unsigned char get_left_line_out_driver()
  {
    unsigned char value;


    /* 
     * 0: Left Line-Out driver is disabled. 
     * 1: Left Line-Out driver is enabled 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & LEFT_LINE_OUT_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_line_out_driver
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 2:2
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
           value = old_value | RIGHT_LINE_OUT_ENABLE_PARAM_MASK; 
           break;
        case RIGHT_LINE_OUT_DISABLE_E: 
           value = old_value & ~ RIGHT_LINE_OUT_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_line_out_driver
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 2:2
 *
 **************************************************************************/
unsigned char get_right_line_out_driver()
  {
    unsigned char value;


    /* 
     * 0: Right Line-Out driver is disabled. 
     * 1: Right Line-Out driver is enabled 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & RIGHT_LINE_OUT_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_auto_headset_power_up_enable
 *
 * IN   : param, a value to write to the regiter PowerUpHeadSet
 * OUT  : 
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 0:0
 *
 **************************************************************************/
unsigned char set_auto_headset_power_up_enable( enum auto_headset_power param )
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
           value = old_value | AUTO_HEADSET_POWER_PARAM_MASK; 
           break;
        case AUTO_HEADSET_POWER_DOWN_E: 
           value = old_value & ~ AUTO_HEADSET_POWER_PARAM_MASK;
           break;
    }
  

    I2CWrite(POWER_UP_HEAD_SET_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_auto_headset_power_up_enable
 *
 * RET  : Return the value of register PowerUpHeadSet
 *
 * Notes : From register 0x0D0C, bits 0:0
 *
 **************************************************************************/
unsigned char get_auto_headset_power_up_enable()
  {
    unsigned char value;


    /* 
     * For a complete auto power-up sequence, It should be 
     * enabled or disabled together or after ENCPHS. 
     */ 
    value = (I2CRead(POWER_UP_HEAD_SET_REG) & AUTO_HEADSET_POWER_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_high_threshold
 *
 * IN   : param, a value to write to the regiter EnvelopeControl
 * OUT  : 
 *
 * RET  : Return the value of register EnvelopeControl
 *
 * Notes : From register 0x0D0D, bits 7:4
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
  
    value =  old_value & ~HIGH_THRESHOLD_PARAM_MASK;


     value |= ( param << 0x4);  
    I2CWrite(ENVELOPE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_high_threshold
 *
 * RET  : Return the value of register EnvelopeControl
 *
 * Notes : From register 0x0D0D, bits 7:4
 *
 **************************************************************************/
unsigned char get_high_threshold()
  {
    unsigned char value;


    /* 
     * High thresold for Charge Pump Low Voltage selection 
     */ 
    value = (I2CRead(ENVELOPE_CONTROL_REG) & HIGH_THRESHOLD_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_threshold
 *
 * IN   : param, a value to write to the regiter EnvelopeControl
 * OUT  : 
 *
 * RET  : Return the value of register EnvelopeControl
 *
 * Notes : From register 0x0D0D, bits 3:0
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
  
    value =  old_value & ~LOW_THRESHOLD_PARAM_MASK;


     value |=  param ;  
    I2CWrite(ENVELOPE_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_threshold
 *
 * RET  : Return the value of register EnvelopeControl
 *
 * Notes : From register 0x0D0D, bits 3:0
 *
 **************************************************************************/
unsigned char get_low_threshold()
  {
    unsigned char value;


    /* 
     * Low thresold for Charge Pump Low Voltage selection 
     */ 
    value = (I2CRead(ENVELOPE_CONTROL_REG) & LOW_THRESHOLD_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_input_voltage_control
 *
 * IN   : param, a value to write to the regiter EnvelopeThreshold
 * OUT  : 
 *
 * RET  : Return the value of register EnvelopeThreshold
 *
 * Notes : From register 0x0D0E, bits 5:5
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
           value = old_value | INPUT_VOLTAGE_CONTROL_PARAM_MASK; 
           break;
        case INPUT_LOW_VOLTAGE_E: 
           value = old_value & ~ INPUT_VOLTAGE_CONTROL_PARAM_MASK;
           break;
    }
  

    I2CWrite(ENVELOPE_THRESHOLD_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_input_voltage_control
 *
 * RET  : Return the value of register EnvelopeThreshold
 *
 * Notes : From register 0x0D0E, bits 5:5
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
    value = (I2CRead(ENVELOPE_THRESHOLD_REG) & INPUT_VOLTAGE_CONTROL_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_envelope_detection_c_p_enable
 *
 * IN   : param, a value to write to the regiter EnvelopeThreshold
 * OUT  : 
 *
 * RET  : Return the value of register EnvelopeThreshold
 *
 * Notes : From register 0x0D0E, bits 4:4
 *
 **************************************************************************/
unsigned char set_envelope_detection_c_p_enable( enum envelope_detection_c_p_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ENVELOPE_THRESHOLD_REG);

    /* 
     * Envelope detection for charge pump 
     */ 
    switch( param ){
        case ENVELOPE_DETECTION_CP_ENABLE_E: 
           value = old_value | ENVELOPE_DETECTION_CP_ENABLE_PARAM_MASK; 
           break;
        case ENVELOPE_DETECTION_CP_DISABLE_E: 
           value = old_value & ~ ENVELOPE_DETECTION_CP_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ENVELOPE_THRESHOLD_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_envelope_detection_c_p_enable
 *
 * RET  : Return the value of register EnvelopeThreshold
 *
 * Notes : From register 0x0D0E, bits 4:4
 *
 **************************************************************************/
unsigned char get_envelope_detection_c_p_enable()
  {
    unsigned char value;


    /* 
     * Envelope detection for charge pump 
     */ 
    value = (I2CRead(ENVELOPE_THRESHOLD_REG) & ENVELOPE_DETECTION_CP_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_envelope_decay_time
 *
 * IN   : param, a value to write to the regiter EnvelopeThreshold
 * OUT  : 
 *
 * RET  : Return the value of register EnvelopeThreshold
 *
 * Notes : From register 0x0D0E, bits 3:0
 *
 **************************************************************************/
unsigned char set_envelope_decay_time( void )
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
 * RET  : Return the value of register EnvelopeThreshold
 *
 * Notes : From register 0x0D0E, bits 3:0
 *
 **************************************************************************/
unsigned char get_envelope_decay_time()
  {
    unsigned char value;


    /* 
     * Low thresold for Charge Pump Low Voltage selection 
     */ 
    value = (I2CRead(ENVELOPE_THRESHOLD_REG) & DECAY_TIME_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib1_controlled_internal
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 7:7
 *
 **************************************************************************/
unsigned char set_vib1_controlled_internal( enum vib1 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * 0: VIB1 driver is connected to corresponding DA path, 
     * 1: VIB1 driver is controlled by internal PWM Generator 
     */ 
    switch( param ){
        case VIB1_DRIVER_CONNECTED_INTERNAL_GENERATOR_E: 
           value = old_value | VIB_1_PARAM_MASK; 
           break;
        case VIB1_DRIVER_CONNECTED_DA_PATH_E: 
           value = old_value & ~ VIB_1_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib1_controlled_internal
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 7:7
 *
 **************************************************************************/
unsigned char get_vib1_controlled_internal()
  {
    unsigned char value;


    /* 
     * 0: VIB1 driver is connected to corresponding DA path, 
     * 1: VIB1 driver is controlled by internal PWM Generator 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & VIB_1_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib2_controlled_internal
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 6:6
 *
 **************************************************************************/
unsigned char set_vib2_controlled_internal( enum vib2 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * 0: VIB2 driver is connected to corresponding DA path, 
     * 1: VIB2 driver is controlled by internal PWM Generator 
     */ 
    switch( param ){
        case VIB2_DRIVER_CONNECTED_INTERNAL_GENERATOR_E: 
           value = old_value | VIB_2_PARAM_MASK; 
           break;
        case VIB2_DRIVER_CONNECTED_DA_PATH_E: 
           value = old_value & ~ VIB_2_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib2_controlled_internal
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 6:6
 *
 **************************************************************************/
unsigned char get_vib2_controlled_internal()
  {
    unsigned char value;


    /* 
     * 0: VIB2 driver is connected to corresponding DA path, 
     * 1: VIB2 driver is controlled by internal PWM Generator 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & VIB_2_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm1_generator_control
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 5:5
 *
 **************************************************************************/
unsigned char set_pwm1_generator_control( enum pwm1_started_together param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * When PWM1 generator is enable positive and negative are 
     * started together (used in differential) 
     */ 
    switch( param ){
        case PWM1_DUTY_CYCLE_E: 
           value = old_value | PWM_1_STARTED_TOGETHER_PARAM_MASK; 
           break;
        case PWM1_VALUE_E: 
           value = old_value & ~ PWM_1_STARTED_TOGETHER_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm1_generator_control
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 5:5
 *
 **************************************************************************/
unsigned char get_pwm1_generator_control()
  {
    unsigned char value;


    /* 
     * When PWM1 generator is enable positive and negative are 
     * started together (used in differential) 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & PWM_1_STARTED_TOGETHER_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm2_generator_control
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 4:4
 *
 **************************************************************************/
unsigned char set_pwm2_generator_control( enum pwm2_started_together param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * When PWM2 generator is enable positive and negative are 
     * started together (used in differential) 
     */ 
    switch( param ){
        case PWM2_DUTY_CYCLE_E: 
           value = old_value | PWM_2_STARTED_TOGETHER_PARAM_MASK; 
           break;
        case PWM2_VALUE_E: 
           value = old_value & ~ PWM_2_STARTED_TOGETHER_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm2_generator_control
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 4:4
 *
 **************************************************************************/
unsigned char get_pwm2_generator_control()
  {
    unsigned char value;


    /* 
     * When PWM2 generator is enable positive and negative are 
     * started together (used in differential) 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & PWM_2_STARTED_TOGETHER_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm1_generator_neg_enable
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 3:3
 *
 **************************************************************************/
unsigned char set_pwm1_generator_neg_enable( enum pwm1_generator_negative param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * When PWM1 generator is enable negative is started 
     * separatetly from positive (not used in differential) 
     */ 
    switch( param ){
        case PWM1_GENERATOR_NEGATIVE_DUTYCYCLE_E: 
           value = old_value | PWM_1_GENERATOR_NEGATIVE_PARAM_MASK; 
           break;
        case PWM1_NEGATIVE_GENERATOR_POL_E: 
           value = old_value & ~ PWM_1_GENERATOR_NEGATIVE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm1_generator_neg_enable
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 3:3
 *
 **************************************************************************/
unsigned char get_pwm1_generator_neg_enable()
  {
    unsigned char value;


    /* 
     * When PWM1 generator is enable negative is started 
     * separatetly from positive (not used in differential) 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & PWM_1_GENERATOR_NEGATIVE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm1_generator_pos_enable
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 2:2
 *
 **************************************************************************/
unsigned char set_pwm1_generator_pos_enable( enum pwm1_generator_positive param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * When PWM1 generator is enable positive is started 
     * separatetly from negative (used in differential) 
     */ 
    switch( param ){
        case PWM1_GENERATOR_POSITIVE_DUTYCYCLE_E: 
           value = old_value | PWM_1_GENERATOR_POSITIVE_PARAM_MASK; 
           break;
        case PWM1_POSITIVE_GENERATOR_POL_E: 
           value = old_value & ~ PWM_1_GENERATOR_POSITIVE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm1_generator_pos_enable
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 2:2
 *
 **************************************************************************/
unsigned char get_pwm1_generator_pos_enable()
  {
    unsigned char value;


    /* 
     * When PWM1 generator is enable positive is started 
     * separatetly from negative (used in differential) 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & PWM_1_GENERATOR_POSITIVE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm2_generator_neg_enable
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 1:1
 *
 **************************************************************************/
unsigned char set_pwm2_generator_neg_enable( enum pwm2_generator_negative param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * When PWM2 generator is enable negative is started 
     * separatetly from positive (used in differential) 
     */ 
    switch( param ){
        case PWM2N_DUTYCYCLE_E: 
           value = old_value | PWM_2_GENERATOR_NEGATIVE_PARAM_MASK; 
           break;
        case PWM2N_GPOL_E: 
           value = old_value & ~ PWM_2_GENERATOR_NEGATIVE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm2_generator_neg_enable
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 1:1
 *
 **************************************************************************/
unsigned char get_pwm2_generator_neg_enable()
  {
    unsigned char value;


    /* 
     * When PWM2 generator is enable negative is started 
     * separatetly from positive (used in differential) 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & PWM_2_GENERATOR_NEGATIVE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_pwm2_generator_pos_enable
 *
 * IN   : param, a value to write to the regiter ConfigurationClassDDrivers
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 0:0
 *
 **************************************************************************/
unsigned char set_pwm2_generator_pos_enable( enum pwm2_generator_positive param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG);

    /* 
     * When PWM2 generator is enable positive is started 
     * separatetly from negative (used in differential) 
     */ 
    switch( param ){
        case PWM2P_DUTYCYCLE_E: 
           value = old_value | PWM_2_GENERATOR_POSITIVE_PARAM_MASK; 
           break;
        case PWM2P_GPOL_E: 
           value = old_value & ~ PWM_2_GENERATOR_POSITIVE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_CLASS_D_DRIVERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_pwm2_generator_pos_enable
 *
 * RET  : Return the value of register ConfigurationClassDDrivers
 *
 * Notes : From register 0x0D0F, bits 0:0
 *
 **************************************************************************/
unsigned char get_pwm2_generator_pos_enable()
  {
    unsigned char value;


    /* 
     * When PWM2 generator is enable positive is started 
     * separatetly from negative (used in differential) 
     */ 
    value = (I2CRead(CONFIGURATION_CLASS_D_DRIVERS_REG) & PWM_2_GENERATOR_POSITIVE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib1_positive_forced_to_voltage
 *
 * IN   : param, a value to write to the regiter ConfigurationPositiveOutputVib1
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationPositiveOutputVib1
 *
 * Notes : From register 0x0D10, bits 7:7
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
           value = old_value | FORCED_TO_VOLTAGE_PARAM_MASK; 
           break;
        case GND_VOLTAGE_E: 
           value = old_value & ~ FORCED_TO_VOLTAGE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib1_positive_forced_to_voltage
 *
 * RET  : Return the value of register ConfigurationPositiveOutputVib1
 *
 * Notes : From register 0x0D10, bits 7:7
 *
 **************************************************************************/
unsigned char get_vib1_positive_forced_to_voltage()
  {
    unsigned char value;


    /* 
     * 0: Vib1p pin is forced to GndVib voltage, 
     * 1: Vib1p pin is forced to VinVib voltage 
     */ 
    value = (I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG) & FORCED_TO_VOLTAGE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib1_positive_duty_cyle
 *
 * IN   : param, a value to write to the regiter ConfigurationPositiveOutputVib1
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationPositiveOutputVib1
 *
 * Notes : From register 0x0D10, bits 6:0
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
  
    value =  old_value & ~DUTY_CYCLE_PARAM_MASK;


     value |=  param ;  
    I2CWrite(CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib1_positive_duty_cyle
 *
 * RET  : Return the value of register ConfigurationPositiveOutputVib1
 *
 * Notes : From register 0x0D10, bits 6:0
 *
 **************************************************************************/
unsigned char get_vib1_positive_duty_cyle()
  {
    unsigned char value;


    /* 
     * PWMPLDUTYCYCLE(6:0) 
     */ 
    value = (I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_1_REG) & DUTY_CYCLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib1_negative_pin_forced
 *
 * IN   : param, a value to write to the regiter ConfigurationNegativeOutputVib1
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationNegativeOutputVib1
 *
 * Notes : From register 0x0D11, bits 7:7
 *
 **************************************************************************/
unsigned char set_vib1_negative_pin_forced( enum forced_to_voltage param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG);

    /* 
     * 0: Vib1n pin is forced to GndVib voltage, 
     * 1: Vib1n pin is forced to VinVib voltage 
     */ 
    switch( param ){
        case VIN_VOLTAGE_E: 
           value = old_value | FORCED_TO_VOLTAGE_PARAM_MASK; 
           break;
        case GND_VOLTAGE_E: 
           value = old_value & ~ FORCED_TO_VOLTAGE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib1_negative_pin_forced
 *
 * RET  : Return the value of register ConfigurationNegativeOutputVib1
 *
 * Notes : From register 0x0D11, bits 7:7
 *
 **************************************************************************/
unsigned char get_vib1_negative_pin_forced()
  {
    unsigned char value;


    /* 
     * 0: Vib1n pin is forced to GndVib voltage, 
     * 1: Vib1n pin is forced to VinVib voltage 
     */ 
    value = (I2CRead(CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG) & FORCED_TO_VOLTAGE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib1_negative_duty_cyle
 *
 * IN   : param, a value to write to the regiter ConfigurationNegativeOutputVib1
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationNegativeOutputVib1
 *
 * Notes : From register 0x0D11, bits 6:0
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
  
    value =  old_value & ~DUTY_CYCLE_PARAM_MASK;


     value |=  param ;  
    I2CWrite(CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib1_negative_duty_cyle
 *
 * RET  : Return the value of register ConfigurationNegativeOutputVib1
 *
 * Notes : From register 0x0D11, bits 6:0
 *
 **************************************************************************/
unsigned char get_vib1_negative_duty_cyle()
  {
    unsigned char value;


    /* 
     * PWMPLDUTYCYCLE(6:0) 
     */ 
    value = (I2CRead(CONFIGURATION_NEGATIVE_OUTPUT_VIB_1_REG) & DUTY_CYCLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib2_positive_forced_to_voltage
 *
 * IN   : param, a value to write to the regiter ConfigurationPositiveOutputVib2
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationPositiveOutputVib2
 *
 * Notes : From register 0x0D12, bits 7:7
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
           value = old_value | FORCED_TO_VOLTAGE_PARAM_MASK; 
           break;
        case GND_VOLTAGE_E: 
           value = old_value & ~ FORCED_TO_VOLTAGE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_POSITIVE_OUTPUT_VIB_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib2_positive_forced_to_voltage
 *
 * RET  : Return the value of register ConfigurationPositiveOutputVib2
 *
 * Notes : From register 0x0D12, bits 7:7
 *
 **************************************************************************/
unsigned char get_vib2_positive_forced_to_voltage()
  {
    unsigned char value;


    /* 
     * 0: Vib2p pin is forced to GndVib voltage, 
     * 1: Vib2p pin is forced to VinVib voltage 
     */ 
    value = (I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_2_REG) & FORCED_TO_VOLTAGE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib2_positive_duty_cyle
 *
 * IN   : param, a value to write to the regiter ConfigurationPositiveOutputVib2
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationPositiveOutputVib2
 *
 * Notes : From register 0x0D12, bits 6:0
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
  
    value =  old_value & ~DUTY_CYCLE_PARAM_MASK;


     value |=  param ;  
    I2CWrite(CONFIGURATION_POSITIVE_OUTPUT_VIB_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib2_positive_duty_cyle
 *
 * RET  : Return the value of register ConfigurationPositiveOutputVib2
 *
 * Notes : From register 0x0D12, bits 6:0
 *
 **************************************************************************/
unsigned char get_vib2_positive_duty_cyle()
  {
    unsigned char value;


    /* 
     * PWMNRDUTYCYCLE(6:0) 
     */ 
    value = (I2CRead(CONFIGURATION_POSITIVE_OUTPUT_VIB_2_REG) & DUTY_CYCLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib2_negative_forced_to_voltage
 *
 * IN   : param, a value to write to the regiter ConfigurationNegativeOutputVib2
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationNegativeOutputVib2
 *
 * Notes : From register 0x0D13, bits 7:7
 *
 **************************************************************************/
unsigned char set_vib2_negative_forced_to_voltage( enum forced_to_voltage param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_NEGATIVE_OUTPUT_VIB_2_REG);

    /* 
     * 0: Vib2p pin is forced to GndVib voltage, 
     * 1: Vib2p pin is forced to VinVib voltage 
     */ 
    switch( param ){
        case VIN_VOLTAGE_E: 
           value = old_value | FORCED_TO_VOLTAGE_PARAM_MASK; 
           break;
        case GND_VOLTAGE_E: 
           value = old_value & ~ FORCED_TO_VOLTAGE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CONFIGURATION_NEGATIVE_OUTPUT_VIB_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib2_negative_forced_to_voltage
 *
 * RET  : Return the value of register ConfigurationNegativeOutputVib2
 *
 * Notes : From register 0x0D13, bits 7:7
 *
 **************************************************************************/
unsigned char get_vib2_negative_forced_to_voltage()
  {
    unsigned char value;


    /* 
     * 0: Vib2p pin is forced to GndVib voltage, 
     * 1: Vib2p pin is forced to VinVib voltage 
     */ 
    value = (I2CRead(CONFIGURATION_NEGATIVE_OUTPUT_VIB_2_REG) & FORCED_TO_VOLTAGE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vib2_negative_duty_cyle
 *
 * IN   : param, a value to write to the regiter ConfigurationNegativeOutputVib2
 * OUT  : 
 *
 * RET  : Return the value of register ConfigurationNegativeOutputVib2
 *
 * Notes : From register 0x0D13, bits 6:0
 *
 **************************************************************************/
unsigned char set_vib2_negative_duty_cyle( enum duty_cycle param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CONFIGURATION_NEGATIVE_OUTPUT_VIB_2_REG);

    /* 
     * PWMNRDUTYCYCLE(6:0) 
     */ 
  
    value =  old_value & ~DUTY_CYCLE_PARAM_MASK;


     value |=  param ;  
    I2CWrite(CONFIGURATION_NEGATIVE_OUTPUT_VIB_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vib2_negative_duty_cyle
 *
 * RET  : Return the value of register ConfigurationNegativeOutputVib2
 *
 * Notes : From register 0x0D13, bits 6:0
 *
 **************************************************************************/
unsigned char get_vib2_negative_duty_cyle()
  {
    unsigned char value;


    /* 
     * PWMNRDUTYCYCLE(6:0) 
     */ 
    value = (I2CRead(CONFIGURATION_NEGATIVE_OUTPUT_VIB_2_REG) & DUTY_CYCLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_mic1_single_ended_conf
 *
 * IN   : param, a value to write to the regiter AnalogMic1Gain
 * OUT  : 
 *
 * RET  : Return the value of register AnalogMic1Gain
 *
 * Notes : From register 0x0D14, bits 7:7
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
           value = old_value | SINGLE_OR_DIFFERENTIAL_PARAM_MASK; 
           break;
        case DIFFERENTIAL_E: 
           value = old_value & ~ SINGLE_OR_DIFFERENTIAL_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_MIC_1_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mic1_single_ended_conf
 *
 * RET  : Return the value of register AnalogMic1Gain
 *
 * Notes : From register 0x0D14, bits 7:7
 *
 **************************************************************************/
unsigned char get_mic1_single_ended_conf()
  {
    unsigned char value;


    /* 
     * 0: DIfferential microphone is connected to MIC1 inputs , 
     * 1: Single ended microphone is connected to MIC1 inputs 
     */ 
    value = (I2CRead(ANALOG_MIC_1_GAIN_REG) & SINGLE_OR_DIFFERENTIAL_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_micro1_enable
 *
 * IN   : param, a value to write to the regiter AnalogMic1Gain
 * OUT  : 
 *
 * RET  : Return the value of register AnalogMic1Gain
 *
 * Notes : From register 0x0D14, bits 6:6
 *
 **************************************************************************/
unsigned char set_low_power_micro1_enable( enum low_power_micro_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_MIC_1_GAIN_REG);

    /* 
     * New in cut 1.0 Low Power for Micro 1 
     */ 
    switch( param ){
        case LOW_POWER_UP_E: 
           value = old_value | LOW_POWER_MICRO_1_ENABLE_PARAM_MASK; 
           break;
        case LOW_POWER_DOWN_E: 
           value = old_value & ~ LOW_POWER_MICRO_1_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_MIC_1_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_power_micro1_enable
 *
 * RET  : Return the value of register AnalogMic1Gain
 *
 * Notes : From register 0x0D14, bits 6:6
 *
 **************************************************************************/
unsigned char get_low_power_micro1_enable()
  {
    unsigned char value;


    /* 
     * New in cut 1.0 Low Power for Micro 1 
     */ 
    value = (I2CRead(ANALOG_MIC_1_GAIN_REG) & LOW_POWER_MICRO_1_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_mic1_gain
 *
 * IN   : param, a value to write to the regiter AnalogMic1Gain
 * OUT  : 
 *
 * RET  : Return the value of register AnalogMic1Gain
 *
 * Notes : From register 0x0D14, bits 4:0
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
  
    value =  old_value & ~ANALOG_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(ANALOG_MIC_1_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mic1_gain
 *
 * RET  : Return the value of register AnalogMic1Gain
 *
 * Notes : From register 0x0D14, bits 4:0
 *
 **************************************************************************/
unsigned char get_mic1_gain()
  {
    unsigned char value;


    /* 
     * Microphone 1 channel Analog Gain 
     */ 
    value = (I2CRead(ANALOG_MIC_1_GAIN_REG) & ANALOG_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_mic2_single_ended_conf
 *
 * IN   : param, a value to write to the regiter AnalogMic2Gain
 * OUT  : 
 *
 * RET  : Return the value of register AnalogMic2Gain
 *
 * Notes : From register 0x0D15, bits 7:7
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
           value = old_value | SINGLE_OR_DIFFERENTIAL_PARAM_MASK; 
           break;
        case DIFFERENTIAL_E: 
           value = old_value & ~ SINGLE_OR_DIFFERENTIAL_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_MIC_2_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mic2_single_ended_conf
 *
 * RET  : Return the value of register AnalogMic2Gain
 *
 * Notes : From register 0x0D15, bits 7:7
 *
 **************************************************************************/
unsigned char get_mic2_single_ended_conf()
  {
    unsigned char value;


    /* 
     * 0: Differential microphone is connected to MIC2 inputs , 
     * 1: Single ended microphone is connected to MIC2 inputs 
     */ 
    value = (I2CRead(ANALOG_MIC_2_GAIN_REG) & SINGLE_OR_DIFFERENTIAL_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_power_micro2_enable
 *
 * IN   : param, a value to write to the regiter AnalogMic2Gain
 * OUT  : 
 *
 * RET  : Return the value of register AnalogMic2Gain
 *
 * Notes : From register 0x0D15, bits 6:6
 *
 **************************************************************************/
unsigned char set_low_power_micro2_enable( enum low_power_micro_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(ANALOG_MIC_2_GAIN_REG);

    /* 
     * New in cut 1.0 Low Power for Micro 2 
     */ 
    switch( param ){
        case LOW_POWER_UP_E: 
           value = old_value | LOW_POWER_MICRO_2_ENABLE_PARAM_MASK; 
           break;
        case LOW_POWER_DOWN_E: 
           value = old_value & ~ LOW_POWER_MICRO_2_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANALOG_MIC_2_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_power_micro2_enable
 *
 * RET  : Return the value of register AnalogMic2Gain
 *
 * Notes : From register 0x0D15, bits 6:6
 *
 **************************************************************************/
unsigned char get_low_power_micro2_enable()
  {
    unsigned char value;


    /* 
     * New in cut 1.0 Low Power for Micro 2 
     */ 
    value = (I2CRead(ANALOG_MIC_2_GAIN_REG) & LOW_POWER_MICRO_2_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_mic2_gain
 *
 * IN   : param, a value to write to the regiter AnalogMic2Gain
 * OUT  : 
 *
 * RET  : Return the value of register AnalogMic2Gain
 *
 * Notes : From register 0x0D15, bits 4:0
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
  
    value =  old_value & ~ANALOG_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(ANALOG_MIC_2_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_mic2_gain
 *
 * RET  : Return the value of register AnalogMic2Gain
 *
 * Notes : From register 0x0D15, bits 4:0
 *
 **************************************************************************/
unsigned char get_mic2_gain()
  {
    unsigned char value;


    /* 
     * Microphone 1 channel Analog Gain 
     */ 
    value = (I2CRead(ANALOG_MIC_2_GAIN_REG) & ANALOG_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_headset_analog_gain
 *
 * IN   : param, a value to write to the regiter LeftLineInGain
 * OUT  : 
 *
 * RET  : Return the value of register LeftLineInGain
 *
 * Notes : From register 0x0D16, bits 7:4
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
  
    value =  old_value & ~LEFT_HEADSET_ANALOG_GAIN_PARAM_MASK;


     value |= ( param << 0x4);  
    I2CWrite(LEFT_LINE_IN_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_headset_analog_gain
 *
 * RET  : Return the value of register LeftLineInGain
 *
 * Notes : From register 0x0D16, bits 7:4
 *
 **************************************************************************/
unsigned char get_left_headset_analog_gain()
  {
    unsigned char value;


    /* 
     * Left headset analog gain 
     */ 
    value = (I2CRead(LEFT_LINE_IN_GAIN_REG) & LEFT_HEADSET_ANALOG_GAIN_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_headset_analog_gain
 *
 * IN   : param, a value to write to the regiter LeftLineInGain
 * OUT  : 
 *
 * RET  : Return the value of register LeftLineInGain
 *
 * Notes : From register 0x0D16, bits 3:0
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
  
    value =  old_value & ~RIGHT_HEADSET_ANALOG_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(LEFT_LINE_IN_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_headset_analog_gain
 *
 * RET  : Return the value of register LeftLineInGain
 *
 * Notes : From register 0x0D16, bits 3:0
 *
 **************************************************************************/
unsigned char get_right_headset_analog_gain()
  {
    unsigned char value;


    /* 
     * Right headset analog gain 
     */ 
    value = (I2CRead(LEFT_LINE_IN_GAIN_REG) & RIGHT_HEADSET_ANALOG_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_line_analog_gain
 *
 * IN   : param, a value to write to the regiter RightLineInGain
 * OUT  : 
 *
 * RET  : Return the value of register RightLineInGain
 *
 * Notes : From register 0x0D17, bits 7:4
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
  
    value =  old_value & ~LEFT_LINE_ANALOG_GAIN_PARAM_MASK;


     value |= ( param << 0x4);  
    I2CWrite(RIGHT_LINE_IN_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_line_analog_gain
 *
 * RET  : Return the value of register RightLineInGain
 *
 * Notes : From register 0x0D17, bits 7:4
 *
 **************************************************************************/
unsigned char get_left_line_analog_gain()
  {
    unsigned char value;


    /* 
     * Gain for Left Line-In 
     */ 
    value = (I2CRead(RIGHT_LINE_IN_GAIN_REG) & LEFT_LINE_ANALOG_GAIN_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_line_analog_gain
 *
 * IN   : param, a value to write to the regiter RightLineInGain
 * OUT  : 
 *
 * RET  : Return the value of register RightLineInGain
 *
 * Notes : From register 0x0D17, bits 3:0
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
  
    value =  old_value & ~RIGHT_LINE_ANALOG_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(RIGHT_LINE_IN_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_line_analog_gain
 *
 * RET  : Return the value of register RightLineInGain
 *
 * Notes : From register 0x0D17, bits 3:0
 *
 **************************************************************************/
unsigned char get_right_line_analog_gain()
  {
    unsigned char value;


    /* 
     * Gain for Right Line-In 
     */ 
    value = (I2CRead(RIGHT_LINE_IN_GAIN_REG) & RIGHT_LINE_ANALOG_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_line_in_to_headset_gain
 *
 * IN   : param, a value to write to the regiter LeftLineInToHeadsetGain
 * OUT  : 
 *
 * RET  : Return the value of register LeftLineInToHeadsetGain
 *
 * Notes : From register 0x0D18, bits 4:0
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
  
    value =  old_value & ~LEFT_LINE_IN_TO_HEADSET_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(LEFT_LINE_IN_TO_HEADSET_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_line_in_to_headset_gain
 *
 * RET  : Return the value of register LeftLineInToHeadsetGain
 *
 * Notes : From register 0x0D18, bits 4:0
 *
 **************************************************************************/
unsigned char get_left_line_in_to_headset_gain()
  {
    unsigned char value;


    /* 
     * Gain for Line-In Left to Headset loop 
     */ 
    value = (I2CRead(LEFT_LINE_IN_TO_HEADSET_GAIN_REG) & LEFT_LINE_IN_TO_HEADSET_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_line_in_to_headset_gain
 *
 * IN   : param, a value to write to the regiter RightLineInToHeadsetGain
 * OUT  : 
 *
 * RET  : Return the value of register RightLineInToHeadsetGain
 *
 * Notes : From register 0x0D19, bits 4:0
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
  
    value =  old_value & ~RIGHT_LINE_IN_TO_HEADSET_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(RIGHT_LINE_IN_TO_HEADSET_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_line_in_to_headset_gain
 *
 * RET  : Return the value of register RightLineInToHeadsetGain
 *
 * Notes : From register 0x0D19, bits 4:0
 *
 **************************************************************************/
unsigned char get_right_line_in_to_headset_gain()
  {
    unsigned char value;


    /* 
     * Gain for Line-In Right to Headset loop 
     */ 
    value = (I2CRead(RIGHT_LINE_IN_TO_HEADSET_GAIN_REG) & RIGHT_LINE_IN_TO_HEADSET_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_offset_cancel_enable
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 7:7
 *
 **************************************************************************/
unsigned char set_ad1_offset_cancel_enable( enum ad1_offset_cancel param )
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
        case AD1_OFFSET_CANCEL_E: 
           value = old_value | AD1_OFFSET_CANCEL_PARAM_MASK; 
           break;
        case AD1_OFFSET_ENABLE_E: 
           value = old_value & ~ AD1_OFFSET_CANCEL_PARAM_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_offset_cancel_enable
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 7:7
 *
 **************************************************************************/
unsigned char get_ad1_offset_cancel_enable()
  {
    unsigned char value;


    /* 
     * 0: AD1 Path with audio Offset Cancellation filter enabled 
     * 1: AD1 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD1_OFFSET_CANCEL_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_offset_cancel_enable
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 6:6
 *
 **************************************************************************/
unsigned char set_ad2_offset_cancel_enable( enum ad2_offset_cancel param )
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
        case AD2_OFFSET_CANCEL_E: 
           value = old_value | AD2_OFFSET_CANCEL_PARAM_MASK; 
           break;
        case AD2_OFFSET_ENABLE_E: 
           value = old_value & ~ AD2_OFFSET_CANCEL_PARAM_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_offset_cancel_enable
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad2_offset_cancel_enable()
  {
    unsigned char value;


    /* 
     * 0: AD2 Path with audio Offset Cancellation filter enabled 
     * 1: AD2 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD2_OFFSET_CANCEL_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad3_offset_cancel_enable
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 5:5
 *
 **************************************************************************/
unsigned char set_ad3_offset_cancel_enable( enum ad3_offset_cancel param )
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
        case AD3_OFFSET_CANCEL_E: 
           value = old_value | AD3_OFFSET_CANCEL_PARAM_MASK; 
           break;
        case AD3_OFFSET_ENABLE_E: 
           value = old_value & ~ AD3_OFFSET_CANCEL_PARAM_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad3_offset_cancel_enable
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 5:5
 *
 **************************************************************************/
unsigned char get_ad3_offset_cancel_enable()
  {
    unsigned char value;


    /* 
     * 0: AD3 Path with audio Offset Cancellation filter enabled 
     * 1: AD3 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD3_OFFSET_CANCEL_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad4_offset_cancel_enable
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 4:4
 *
 **************************************************************************/
unsigned char set_ad4_offset_cancel_enable( enum ad4_offset_cancel param )
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
        case AD4_OFFSET_CANCEL_E: 
           value = old_value | AD4_OFFSET_CANCEL_PARAM_MASK; 
           break;
        case AD4_OFFSET_ENABLE_E: 
           value = old_value & ~ AD4_OFFSET_CANCEL_PARAM_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad4_offset_cancel_enable
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 4:4
 *
 **************************************************************************/
unsigned char get_ad4_offset_cancel_enable()
  {
    unsigned char value;


    /* 
     * 0: AD4 Path with audio Offset Cancellation filter enabled 
     * 1: AD4 Path with audio Offset Cancellation filter 
     * disabled 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD4_OFFSET_CANCEL_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_voice_filter_enable
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 3:3
 *
 **************************************************************************/
unsigned char set_ad1_voice_filter_enable( enum ad1_voice_filter_enable param )
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
           value = old_value | AD_1_VOICE_FILTER_ENABLE_PARAM_MASK; 
           break;
        case AD_1_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ AD_1_VOICE_FILTER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_voice_filter_enable
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 3:3
 *
 **************************************************************************/
unsigned char get_ad1_voice_filter_enable()
  {
    unsigned char value;


    /* 
     * 0: AD1 channel path use Audio Filters 
     * 1: AD1 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_1_VOICE_FILTER_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_voice_filter_enable
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 2:2
 *
 **************************************************************************/
unsigned char set_ad2_voice_filter_enable( enum ad2_voice_filter_enable param )
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
           value = old_value | AD_2_VOICE_FILTER_ENABLE_PARAM_MASK; 
           break;
        case AD_2_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ AD_2_VOICE_FILTER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_voice_filter_enable
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 2:2
 *
 **************************************************************************/
unsigned char get_ad2_voice_filter_enable()
  {
    unsigned char value;


    /* 
     * 0: AD2 channel path use Audio Filters 
     * 1: AD2 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_2_VOICE_FILTER_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad3_voice_filter_enable
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 1:1
 *
 **************************************************************************/
unsigned char set_ad3_voice_filter_enable( enum ad3_voice_filter_enable param )
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
           value = old_value | AD_3_VOICE_FILTER_ENABLE_PARAM_MASK; 
           break;
        case AD_3_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ AD_3_VOICE_FILTER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad3_voice_filter_enable
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 1:1
 *
 **************************************************************************/
unsigned char get_ad3_voice_filter_enable()
  {
    unsigned char value;


    /* 
     * 0: AD3 channel path use Audio Filters 
     * 1: AD3 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_3_VOICE_FILTER_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad4_voice_filter_enable
 *
 * IN   : param, a value to write to the regiter AudioVoiceFilters
 * OUT  : 
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 0:0
 *
 **************************************************************************/
unsigned char set_ad4_voice_filter_enable( enum ad4_voice_filter_enable param )
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
           value = old_value | AD_4_VOICE_FILTER_ENABLE_PARAM_MASK; 
           break;
        case AD_4_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ AD_4_VOICE_FILTER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(AUDIO_VOICE_FILTERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad4_voice_filter_enable
 *
 * RET  : Return the value of register AudioVoiceFilters
 *
 * Notes : From register 0x0D1a, bits 0:0
 *
 **************************************************************************/
unsigned char get_ad4_voice_filter_enable()
  {
    unsigned char value;


    /* 
     * 0: AD4 channel path use Audio Filters 
     * 1: AD4 channel path use 48kHz Low Latency Filters for 
     * Voice 
     */ 
    value = (I2CRead(AUDIO_VOICE_FILTERS_REG) & AD_4_VOICE_FILTER_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_master0_mode_enable
 *
 * IN   : param, a value to write to the regiter TdmInterface
 * OUT  : 
 *
 * RET  : Return the value of register TdmInterface
 *
 * Notes : From register 0x0D1b, bits 7:7
 *
 **************************************************************************/
unsigned char set_master0_mode_enable( enum master0_mode_enable param )
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
           value = old_value | MASTER_0_MODE_ENABLE_PARAM_MASK; 
           break;
        case MASTER_0_MODE_DISABLE_E: 
           value = old_value & ~ MASTER_0_MODE_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_INTERFACE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_master0_mode_enable
 *
 * RET  : Return the value of register TdmInterface
 *
 * Notes : From register 0x0D1b, bits 7:7
 *
 **************************************************************************/
unsigned char get_master0_mode_enable()
  {
    unsigned char value;


    /* 
     * 0: The Master Generator for AUD_IF1/0 is stopped, 
     * 1: The Master Generator for AUD_IF1/0 is active 
     */ 
    value = (I2CRead(TDM_INTERFACE_REG) & MASTER_0_MODE_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_bit_clock1
 *
 * IN   : param, a value to write to the regiter TdmInterface
 * OUT  : 
 *
 * RET  : Return the value of register TdmInterface
 *
 * Notes : From register 0x0D1b, bits 6:5
 *
 **************************************************************************/
unsigned char set_bit_clock1( enum tdm_bit_clock param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_INTERFACE_REG);

    /* 
     * BitClk1 = 48kHz x N; 00: N = 32 ; 01: N = 64 ; 10: N = 
     * 128; 11: N = 256 
     */ 
  

     value =  old_value & ~BIT_CLOCK_1_PARAM_MASK ;

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
 * Function : get_bit_clock1
 *
 * RET  : Return the value of register TdmInterface
 *
 * Notes : From register 0x0D1b, bits 6:5
 *
 **************************************************************************/
unsigned char get_bit_clock1()
  {
    unsigned char value;


    /* 
     * BitClk1 = 48kHz x N; 00: N = 32 ; 01: N = 64 ; 10: N = 
     * 128; 11: N = 256 
     */ 
    value = (I2CRead(TDM_INTERFACE_REG) & BIT_CLOCK_1_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_bit_clock1_enable
 *
 * IN   : param, a value to write to the regiter TdmInterface
 * OUT  : 
 *
 * RET  : Return the value of register TdmInterface
 *
 * Notes : From register 0x0D1b, bits 4:4
 *
 **************************************************************************/
unsigned char set_bit_clock1_enable( enum bit_clock1_enable param )
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
           value = old_value | BIT_CLOCK_1_ENABLE_PARAM_MASK; 
           break;
        case BIT_CLOCK_1_DISABLE_E: 
           value = old_value & ~ BIT_CLOCK_1_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_INTERFACE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_bit_clock1_enable
 *
 * RET  : Return the value of register TdmInterface
 *
 * Notes : From register 0x0D1b, bits 4:4
 *
 **************************************************************************/
unsigned char get_bit_clock1_enable()
  {
    unsigned char value;


    /* 
     * 0: The FSync1 and BitClk1 bits are stopped; 
     * 1: The FSync1 and BitClk1 bits are enabled 
     */ 
    value = (I2CRead(TDM_INTERFACE_REG) & BIT_CLOCK_1_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_bit_clock0
 *
 * IN   : param, a value to write to the regiter TdmInterface
 * OUT  : 
 *
 * RET  : Return the value of register TdmInterface
 *
 * Notes : From register 0x0D1b, bits 2:1
 *
 **************************************************************************/
unsigned char set_bit_clock0( enum tdm_bit_clock param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(TDM_INTERFACE_REG);

    /* 
     * BitClk0 = 48kHz x N; 00: N = 32 ; 01: N = 64 ; 10: N = 
     * 128; 11: N = 256 
     */ 
  

     value =  old_value & ~BIT_CLOCK_0_PARAM_MASK ;

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
 * Function : get_bit_clock0
 *
 * RET  : Return the value of register TdmInterface
 *
 * Notes : From register 0x0D1b, bits 2:1
 *
 **************************************************************************/
unsigned char get_bit_clock0()
  {
    unsigned char value;


    /* 
     * BitClk0 = 48kHz x N; 00: N = 32 ; 01: N = 64 ; 10: N = 
     * 128; 11: N = 256 
     */ 
    value = (I2CRead(TDM_INTERFACE_REG) & BIT_CLOCK_0_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_bit_clock0_enable
 *
 * IN   : param, a value to write to the regiter TdmInterface
 * OUT  : 
 *
 * RET  : Return the value of register TdmInterface
 *
 * Notes : From register 0x0D1b, bits 0:0
 *
 **************************************************************************/
unsigned char set_bit_clock0_enable( enum bit_clock0_enable param )
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
           value = old_value | BIT_CLOCK_0_ENABLE_PARAM_MASK; 
           break;
        case BIT_CLOCK_0_DISABLE_E: 
           value = old_value & ~ BIT_CLOCK_0_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_INTERFACE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_bit_clock0_enable
 *
 * RET  : Return the value of register TdmInterface
 *
 * Notes : From register 0x0D1b, bits 0:0
 *
 **************************************************************************/
unsigned char get_bit_clock0_enable()
  {
    unsigned char value;


    /* 
     * 0: The FSync0 and BitClk0 bits are stopped; 
     * 1: The FSync0 and BitClk0 bits are enabled 
     */ 
    value = (I2CRead(TDM_INTERFACE_REG) & BIT_CLOCK_0_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_first_slot0_synchro
 *
 * IN   : param, a value to write to the regiter TdmConfiguration
 * OUT  : 
 *
 * RET  : Return the value of register TdmConfiguration
 *
 * Notes : From register 0x0D1C, bits 6:6
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
           value = old_value | FIRST_SLOT_SYNCHRO_0_PARAM_MASK; 
           break;
        case FSYNC_RISING_EDGE_E: 
           value = old_value & ~ FIRST_SLOT_SYNCHRO_0_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_first_slot0_synchro
 *
 * RET  : Return the value of register TdmConfiguration
 *
 * Notes : From register 0x0D1C, bits 6:6
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
    value = (I2CRead(TDM_CONFIGURATION_REG) & FIRST_SLOT_SYNCHRO_0_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_input0_bit_sampled
 *
 * IN   : param, a value to write to the regiter TdmConfiguration
 * OUT  : 
 *
 * RET  : Return the value of register TdmConfiguration
 *
 * Notes : From register 0x0D1C, bits 5:5
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
           value = old_value | INPUT_0_BIT_SAMPLED_PARAM_MASK; 
           break;
        case BIT_CLOCK0_RISING_EDGE_E: 
           value = old_value & ~ INPUT_0_BIT_SAMPLED_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_input0_bit_sampled
 *
 * RET  : Return the value of register TdmConfiguration
 *
 * Notes : From register 0x0D1C, bits 5:5
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
    value = (I2CRead(TDM_CONFIGURATION_REG) & INPUT_0_BIT_SAMPLED_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if0_delayed
 *
 * IN   : param, a value to write to the regiter TdmConfiguration
 * OUT  : 
 *
 * RET  : Return the value of register TdmConfiguration
 *
 * Notes : From register 0x0D1C, bits 4:4
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
           value = old_value | DELAYED_PARAM_MASK; 
           break;
        case NOT_DELAYED_E: 
           value = old_value & ~ DELAYED_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if0_delayed
 *
 * RET  : Return the value of register TdmConfiguration
 *
 * Notes : From register 0x0D1C, bits 4:4
 *
 **************************************************************************/
unsigned char get_if0_delayed()
  {
    unsigned char value;


    /* 
     * 0: IF0 format is not delayed; 
     * 1: IF0 format is delayed 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_REG) & DELAYED_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if0_mode
 *
 * IN   : param, a value to write to the regiter TdmConfiguration
 * OUT  : 
 *
 * RET  : Return the value of register TdmConfiguration
 *
 * Notes : From register 0x0D1C, bits 3:2
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
  

     value =  old_value & ~IF_0_MODE_PARAM_MASK ;

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
 * RET  : Return the value of register TdmConfiguration
 *
 * Notes : From register 0x0D1C, bits 3:2
 *
 **************************************************************************/
unsigned char get_if0_mode()
  {
    unsigned char value;


    /* 
     * 0: IF0 is disabled (no data is read from the interface). 
     * 1: IF0 format is TDM, 1x: IF0 format is I2S Left Aligned 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_REG) & FORMAT_MODE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if0_word_length
 *
 * IN   : param, a value to write to the regiter TdmConfiguration
 * OUT  : 
 *
 * RET  : Return the value of register TdmConfiguration
 *
 * Notes : From register 0x0D1C, bits 1:0
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
  

     value =  old_value & ~IF_0_WORD_LENGTH_PARAM_MASK ;

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
 * RET  : Return the value of register TdmConfiguration
 *
 * Notes : From register 0x0D1C, bits 1:0
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
    value = (I2CRead(TDM_CONFIGURATION_REG) & WORD_LENGTH_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_data0_to_ad_data1_enable
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 7:7
 *
 **************************************************************************/
unsigned char set_da_data0_to_ad_data1_enable( enum da_data0_to_ad_data1_enable param )
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
           value = old_value | DA_DATA_0_TO_AD_DATA_1_ENABLE_PARAM_MASK; 
           break;
        case DA_DATA_0_TO_AD_DATA_1_DISABLE_E: 
           value = old_value & ~ DA_DATA_0_TO_AD_DATA_1_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_data0_to_ad_data1_enable
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 7:7
 *
 **************************************************************************/
unsigned char get_da_data0_to_ad_data1_enable()
  {
    unsigned char value;


    /* 
     * 0: AD_DATA1 pin is used for the default purpose ; 
     * 1: Data on DA_DATA0 pin is sent to AD_DATA1 pin 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & DA_DATA_0_TO_AD_DATA_1_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if0_on_if1_enable
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 6:6
 *
 **************************************************************************/
unsigned char set_if0_on_if1_enable( enum if0_on_if1_enable param )
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
        case IF_0_ON_E: 
           value = old_value | IF_0_ON_IF_1_ENABLE_PARAM_MASK; 
           break;
        case IF_0_OFF_E: 
           value = old_value & ~ IF_0_ON_IF_1_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if0_on_if1_enable
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 6:6
 *
 **************************************************************************/
unsigned char get_if0_on_if1_enable()
  {
    unsigned char value;


    /* 
     * 0: BITCLK1, FSYNC1 pins are used for the default purpose, 
     * 1: Clocks on BITCLK0,FSYNC0 pins are sent to BITCLK1, 
     * FSYNC1 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & IF_0_ON_IF_1_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_master_clock_if1_enable
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 5:5
 *
 **************************************************************************/
unsigned char set_master_clock_if1_enable( enum master_clock_if1_enable param )
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
           value = old_value | MASTER_CLOCK_IF_1_ENABLE_PARAM_MASK; 
           break;
        case MASTER_CLOCK_IF_1_DISABLE_E: 
           value = old_value & ~ MASTER_CLOCK_IF_1_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_master_clock_if1_enable
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 5:5
 *
 **************************************************************************/
unsigned char get_master_clock_if1_enable()
  {
    unsigned char value;


    /* 
     * 0: Pins FSync1 and BitClk1 are set as input ; 
     * 1: Pins FSync1 and BitClk1 are set as output 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & MASTER_CLOCK_IF_1_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_data1_to_ad_data0_enable
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 3:3
 *
 **************************************************************************/
unsigned char set_da_data1_to_ad_data0_enable( enum da_data1_to_ad_data0_enable param )
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
           value = old_value | DA_DATA_1_TO_AD_DATA_0_ENABLE_PARAM_MASK; 
           break;
        case DA_DATA_1_TO_AD_DATA_0_DISABLE_E: 
           value = old_value & ~ DA_DATA_1_TO_AD_DATA_0_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_data1_to_ad_data0_enable
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 3:3
 *
 **************************************************************************/
unsigned char get_da_data1_to_ad_data0_enable()
  {
    unsigned char value;


    /* 
     * 0: AD_DATA0 pin is used for the default purpose ; 
     * 1: Data on DA_DATA1 pin is sent to AD_DATA0 pin 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & DA_DATA_1_TO_AD_DATA_0_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if1_on_if0_enable
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 2:2
 *
 **************************************************************************/
unsigned char set_if1_on_if0_enable( enum if1_on_if0_enable param )
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
        case IF_1_ON_E: 
           value = old_value | IF_1_ON_IF_0_ENABLE_PARAM_MASK; 
           break;
        case IF_1_OFF_E: 
           value = old_value & ~ IF_1_ON_IF_0_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if1_on_if0_enable
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 2:2
 *
 **************************************************************************/
unsigned char get_if1_on_if0_enable()
  {
    unsigned char value;


    /* 
     * 0: BITCLK0, FSYNC0 pins are used for the default purpose, 
     * 1: Clocks on BITCLK1,FSYNC1 pins are sent to BITCLK0, 
     * FSYNC0 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & IF_1_ON_IF_0_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_master_clock_if0_enable
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 1:1
 *
 **************************************************************************/
unsigned char set_master_clock_if0_enable( enum master_clock_if0_enable param )
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
           value = old_value | MASTER_CLOCK_IF_0_ENABLE_PARAM_MASK; 
           break;
        case MASTER_CLOCK_IF_0_DISABLE_E: 
           value = old_value & ~ MASTER_CLOCK_IF_0_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_master_clock_if0_enable
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 1:1
 *
 **************************************************************************/
unsigned char get_master_clock_if0_enable()
  {
    unsigned char value;


    /* 
     * 0: Pins FSync0 and BitClk0 are set as input ; 
     * 1: Pins FSync0 and BitClk0 are set as output 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & MASTER_CLOCK_IF_0_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_burst_mode_enable
 *
 * IN   : param, a value to write to the regiter TdmLoopbackControl
 * OUT  : 
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 0:0
 *
 **************************************************************************/
unsigned char set_burst_mode_enable( enum burst_mode_enable param )
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
           value = old_value | BURST_MODE_ENABLE_PARAM_MASK; 
           break;
        case BURST_MODE_DISABLE_E: 
           value = old_value & ~ BURST_MODE_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_LOOPBACK_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_burst_mode_enable
 *
 * RET  : Return the value of register TdmLoopbackControl
 *
 * Notes : From register 0x0D1D, bits 0:0
 *
 **************************************************************************/
unsigned char get_burst_mode_enable()
  {
    unsigned char value;


    /* 
     * 0: TDM Interface IF0 is configured in normal mode; 
     * 1: TDM Interface IF0 is configured in burst mode 
     */ 
    value = (I2CRead(TDM_LOOPBACK_CONTROL_REG) & BURST_MODE_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_first_slot1_synchro
 *
 * IN   : param, a value to write to the regiter TdmConfiguration1
 * OUT  : 
 *
 * RET  : Return the value of register TdmConfiguration1
 *
 * Notes : From register 0x0D1E, bits 6:6
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
           value = old_value | FIRST_SLOT_SYNCHRO_0_PARAM_MASK; 
           break;
        case FSYNC_RISING_EDGE_E: 
           value = old_value & ~ FIRST_SLOT_SYNCHRO_0_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_first_slot1_synchro
 *
 * RET  : Return the value of register TdmConfiguration1
 *
 * Notes : From register 0x0D1E, bits 6:6
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
    value = (I2CRead(TDM_CONFIGURATION_1_REG) & FIRST_SLOT_SYNCHRO_0_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_input1_bit_sampled
 *
 * IN   : param, a value to write to the regiter TdmConfiguration1
 * OUT  : 
 *
 * RET  : Return the value of register TdmConfiguration1
 *
 * Notes : From register 0x0D1E, bits 5:5
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
           value = old_value | INPUT_1_BIT_SAMPLED_PARAM_MASK; 
           break;
        case BIT_CLOCK1_RISING_EDGE_E: 
           value = old_value & ~ INPUT_1_BIT_SAMPLED_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_input1_bit_sampled
 *
 * RET  : Return the value of register TdmConfiguration1
 *
 * Notes : From register 0x0D1E, bits 5:5
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
    value = (I2CRead(TDM_CONFIGURATION_1_REG) & INPUT_1_BIT_SAMPLED_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if1_delayed
 *
 * IN   : param, a value to write to the regiter TdmConfiguration1
 * OUT  : 
 *
 * RET  : Return the value of register TdmConfiguration1
 *
 * Notes : From register 0x0D1E, bits 4:4
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
           value = old_value | DELAYED_PARAM_MASK; 
           break;
        case NOT_DELAYED_E: 
           value = old_value & ~ DELAYED_PARAM_MASK;
           break;
    }
  

    I2CWrite(TDM_CONFIGURATION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_if1_delayed
 *
 * RET  : Return the value of register TdmConfiguration1
 *
 * Notes : From register 0x0D1E, bits 4:4
 *
 **************************************************************************/
unsigned char get_if1_delayed()
  {
    unsigned char value;


    /* 
     * 0: IF1 format is not delayed; 
     * 1: IF1 format is delayed 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_1_REG) & DELAYED_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if1_mode
 *
 * IN   : param, a value to write to the regiter TdmConfiguration1
 * OUT  : 
 *
 * RET  : Return the value of register TdmConfiguration1
 *
 * Notes : From register 0x0D1E, bits 3:3
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
  

     value =  old_value & ~IF_1_MODE_PARAM_MASK ;

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
 * RET  : Return the value of register TdmConfiguration1
 *
 * Notes : From register 0x0D1E, bits 3:3
 *
 **************************************************************************/
unsigned char get_if1_mode()
  {
    unsigned char value;


    /* 
     * 0: IF1 is disabled (no data is read from the interface). 
     * 1: IF1 format is TDM, 1x: IF1 format is I2S Left Aligned 
     */ 
    value = (I2CRead(TDM_CONFIGURATION_1_REG) & FORMAT_MODE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_if1_word_length
 *
 * IN   : param, a value to write to the regiter TdmConfiguration1
 * OUT  : 
 *
 * RET  : Return the value of register TdmConfiguration1
 *
 * Notes : From register 0x0D1E, bits 1:0
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
  

     value =  old_value & ~IF_1_WORD_LENGTH_PARAM_MASK ;

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
 * RET  : Return the value of register TdmConfiguration1
 *
 * Notes : From register 0x0D1E, bits 1:0
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
    value = (I2CRead(TDM_CONFIGURATION_1_REG) & WORD_LENGTH_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output0
 *
 * IN   : param, a value to write to the regiter SlotAllocation0
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation0
 *
 * Notes : From register 0x0D1F, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_0_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation0
 *
 * Notes : From register 0x0D1F, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output0()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_0_REG) & SLOT_OUTPUT_0_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output1
 *
 * IN   : param, a value to write to the regiter SlotAllocation1
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation1
 *
 * Notes : From register 0x0D1F, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output1( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_1_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_1_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output1
 *
 * RET  : Return the value of register SlotAllocation1
 *
 * Notes : From register 0x0D1F, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output1()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_1_REG) & SLOT_OUTPUT_1_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output2
 *
 * IN   : param, a value to write to the regiter SlotAllocation2
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation2
 *
 * Notes : From register 0x0D20, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_2_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation2
 *
 * Notes : From register 0x0D20, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output2()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_2_REG) & SLOT_OUTPUT_2_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output3
 *
 * IN   : param, a value to write to the regiter SlotAllocation3
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation3
 *
 * Notes : From register 0x0D20, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output3( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_3_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_3_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output3
 *
 * RET  : Return the value of register SlotAllocation3
 *
 * Notes : From register 0x0D20, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output3()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_3_REG) & SLOT_OUTPUT_3_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output4
 *
 * IN   : param, a value to write to the regiter SlotAllocation4
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation4
 *
 * Notes : From register 0x0D21, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_4_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation4
 *
 * Notes : From register 0x0D21, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output4()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_4_REG) & SLOT_OUTPUT_4_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output5
 *
 * IN   : param, a value to write to the regiter SlotAllocation5
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation5
 *
 * Notes : From register 0x0D21, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output5( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_5_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_5_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output5
 *
 * RET  : Return the value of register SlotAllocation5
 *
 * Notes : From register 0x0D21, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output5()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_5_REG) & SLOT_OUTPUT_5_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output6
 *
 * IN   : param, a value to write to the regiter SlotAllocation6
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation6
 *
 * Notes : From register 0x0D22, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_6_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation6
 *
 * Notes : From register 0x0D22, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output6()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_6_REG) & SLOT_OUTPUT_6_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output7
 *
 * IN   : param, a value to write to the regiter SlotAllocation7
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation7
 *
 * Notes : From register 0x0D22, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output7( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_7_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_7_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output7
 *
 * RET  : Return the value of register SlotAllocation7
 *
 * Notes : From register 0x0D22, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output7()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_7_REG) & SLOT_OUTPUT_7_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output8
 *
 * IN   : param, a value to write to the regiter SlotAllocation8
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation8
 *
 * Notes : From register 0x0D23, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_8_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation8
 *
 * Notes : From register 0x0D23, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output8()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_8_REG) & SLOT_OUTPUT_8_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output9
 *
 * IN   : param, a value to write to the regiter SlotAllocation9
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation9
 *
 * Notes : From register 0x0D23, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output9( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_9_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_9_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_9_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output9
 *
 * RET  : Return the value of register SlotAllocation9
 *
 * Notes : From register 0x0D23, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output9()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_9_REG) & SLOT_OUTPUT_9_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output10
 *
 * IN   : param, a value to write to the regiter SlotAllocation10
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation10
 *
 * Notes : From register 0x0D24, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_10_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation10
 *
 * Notes : From register 0x0D24, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output10()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_10_REG) & SLOT_OUTPUT_10_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output11
 *
 * IN   : param, a value to write to the regiter SlotAllocation11
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation11
 *
 * Notes : From register 0x0D24, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output11( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_11_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_11_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_11_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output11
 *
 * RET  : Return the value of register SlotAllocation11
 *
 * Notes : From register 0x0D24, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output11()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_11_REG) & SLOT_OUTPUT_11_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output12
 *
 * IN   : param, a value to write to the regiter SlotAllocation12
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation12
 *
 * Notes : From register 0x0D25, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_12_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation12
 *
 * Notes : From register 0x0D25, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output12()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_12_REG) & SLOT_OUTPUT_12_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output13
 *
 * IN   : param, a value to write to the regiter SlotAllocation13
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation13
 *
 * Notes : From register 0x0D25, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output13( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_13_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_13_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_13_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output13
 *
 * RET  : Return the value of register SlotAllocation13
 *
 * Notes : From register 0x0D25, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output13()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_13_REG) & SLOT_OUTPUT_13_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output14
 *
 * IN   : param, a value to write to the regiter SlotAllocation14
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation14
 *
 * Notes : From register 0x0D26, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_14_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation14
 *
 * Notes : From register 0x0D26, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output14()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_14_REG) & SLOT_OUTPUT_14_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output15
 *
 * IN   : param, a value to write to the regiter SlotAllocation15
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation15
 *
 * Notes : From register 0x0D26, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output15( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_15_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_15_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_15_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output15
 *
 * RET  : Return the value of register SlotAllocation15
 *
 * Notes : From register 0x0D26, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output15()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_15_REG) & SLOT_OUTPUT_15_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output16
 *
 * IN   : param, a value to write to the regiter SlotAllocation16
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation16
 *
 * Notes : From register 0x0D27, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_16_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation16
 *
 * Notes : From register 0x0D27, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output16()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_16_REG) & SLOT_OUTPUT_16_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output17
 *
 * IN   : param, a value to write to the regiter SlotAllocation17
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation17
 *
 * Notes : From register 0x0D27, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output17( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_17_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_17_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_17_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output17
 *
 * RET  : Return the value of register SlotAllocation17
 *
 * Notes : From register 0x0D27, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output17()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_17_REG) & SLOT_OUTPUT_17_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output18
 *
 * IN   : param, a value to write to the regiter SlotAllocation18
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation18
 *
 * Notes : From register 0x0D28, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_18_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation18
 *
 * Notes : From register 0x0D28, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output18()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_18_REG) & SLOT_OUTPUT_18_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output19
 *
 * IN   : param, a value to write to the regiter SlotAllocation19
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation19
 *
 * Notes : From register 0x0D28, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output19( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_19_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_19_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_19_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output19
 *
 * RET  : Return the value of register SlotAllocation19
 *
 * Notes : From register 0x0D28, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output19()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_19_REG) & SLOT_OUTPUT_19_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output20
 *
 * IN   : param, a value to write to the regiter SlotAllocation20
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation20
 *
 * Notes : From register 0x0D29, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_20_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation20
 *
 * Notes : From register 0x0D29, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output20()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_20_REG) & SLOT_OUTPUT_20_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output21
 *
 * IN   : param, a value to write to the regiter SlotAllocation21
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation21
 *
 * Notes : From register 0x0D29, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output21( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_21_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_21_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_21_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output21
 *
 * RET  : Return the value of register SlotAllocation21
 *
 * Notes : From register 0x0D29, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output21()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_21_REG) & SLOT_OUTPUT_21_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output22
 *
 * IN   : param, a value to write to the regiter SlotAllocation22
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation22
 *
 * Notes : From register 0x0D2A, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_22_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation22
 *
 * Notes : From register 0x0D2A, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output22()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_22_REG) & SLOT_OUTPUT_22_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output23
 *
 * IN   : param, a value to write to the regiter SlotAllocation23
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation23
 *
 * Notes : From register 0x0D2A, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output23( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_23_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_23_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_23_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output23
 *
 * RET  : Return the value of register SlotAllocation23
 *
 * Notes : From register 0x0D2A, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output23()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_23_REG) & SLOT_OUTPUT_23_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output24
 *
 * IN   : param, a value to write to the regiter SlotAllocation24
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation24
 *
 * Notes : From register 0x0D2B, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_24_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation24
 *
 * Notes : From register 0x0D2B, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output24()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_24_REG) & SLOT_OUTPUT_24_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output25
 *
 * IN   : param, a value to write to the regiter SlotAllocation25
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation25
 *
 * Notes : From register 0x0D2B, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output25( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_25_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_25_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_25_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output25
 *
 * RET  : Return the value of register SlotAllocation25
 *
 * Notes : From register 0x0D2B, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output25()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_25_REG) & SLOT_OUTPUT_25_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output26
 *
 * IN   : param, a value to write to the regiter SlotAllocation26
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation26
 *
 * Notes : From register 0x0D2C, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_26_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation26
 *
 * Notes : From register 0x0D2C, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output26()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_26_REG) & SLOT_OUTPUT_26_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output27
 *
 * IN   : param, a value to write to the regiter SlotAllocation27
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation27
 *
 * Notes : From register 0x0D2C, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output27( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_27_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_27_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_27_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output27
 *
 * RET  : Return the value of register SlotAllocation27
 *
 * Notes : From register 0x0D2C, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output27()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_27_REG) & SLOT_OUTPUT_27_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output28
 *
 * IN   : param, a value to write to the regiter SlotAllocation28
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation28
 *
 * Notes : From register 0x0D2D, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_28_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation28
 *
 * Notes : From register 0x0D2D, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output28()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_28_REG) & SLOT_OUTPUT_28_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output29
 *
 * IN   : param, a value to write to the regiter SlotAllocation29
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation29
 *
 * Notes : From register 0x0D2D, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output29( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_29_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_29_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_29_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output29
 *
 * RET  : Return the value of register SlotAllocation29
 *
 * Notes : From register 0x0D2D, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output29()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_29_REG) & SLOT_OUTPUT_29_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output30
 *
 * IN   : param, a value to write to the regiter SlotAllocation30
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation30
 *
 * Notes : From register 0x0D2E, bits 3:0
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
  

     value =  old_value & ~SLOT_OUTPUT_30_PARAM_MASK ;

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
 * RET  : Return the value of register SlotAllocation30
 *
 * Notes : From register 0x0D2E, bits 3:0
 *
 **************************************************************************/
unsigned char get_slot_output30()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_30_REG) & SLOT_OUTPUT_30_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_output31
 *
 * IN   : param, a value to write to the regiter SlotAllocation31
 * OUT  : 
 *
 * RET  : Return the value of register SlotAllocation31
 *
 * Notes : From register 0x0D2E, bits 7:4
 *
 **************************************************************************/
unsigned char set_slot_output31( enum slot_output param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_ALLOCATION_31_REG);

    /* 
     * Slot Output State 
     */ 
  

     value =  old_value & ~SLOT_OUTPUT_31_PARAM_MASK ;

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
  

    I2CWrite(SLOT_ALLOCATION_31_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_output31
 *
 * RET  : Return the value of register SlotAllocation31
 *
 * Notes : From register 0x0D2E, bits 7:4
 *
 **************************************************************************/
unsigned char get_slot_output31()
  {
    unsigned char value;


    /* 
     * Slot Output State 
     */ 
    value = (I2CRead(SLOT_ALLOCATION_31_REG) & SLOT_OUTPUT_31_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz0
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 0:0
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
           value = old_value | SLOT_HIZ_0_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_0_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz0
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 0:0
 *
 **************************************************************************/
unsigned char get_slot_hiz0()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_0_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz1
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 1:1
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
           value = old_value | SLOT_HIZ_1_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_1_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz1
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 1:1
 *
 **************************************************************************/
unsigned char get_slot_hiz1()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_1_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz2
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 2:2
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
           value = old_value | SLOT_HIZ_2_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_2_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz2
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 2:2
 *
 **************************************************************************/
unsigned char get_slot_hiz2()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_2_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz3
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 3:3
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
           value = old_value | SLOT_HIZ_3_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_3_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz3
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 3:3
 *
 **************************************************************************/
unsigned char get_slot_hiz3()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_3_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz4
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 4:4
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
           value = old_value | SLOT_HIZ_4_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_4_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz4
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 4:4
 *
 **************************************************************************/
unsigned char get_slot_hiz4()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_4_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz5
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 5:5
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
           value = old_value | SLOT_HIZ_5_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_5_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz5
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 5:5
 *
 **************************************************************************/
unsigned char get_slot_hiz5()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_5_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz6
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 6:6
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
           value = old_value | SLOT_HIZ_6_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_6_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz6
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 6:6
 *
 **************************************************************************/
unsigned char get_slot_hiz6()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_6_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz7
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl0
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 7:7
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
           value = old_value | SLOT_HIZ_7_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_7_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz7
 *
 * RET  : Return the value of register ADslotTristateControl0
 *
 * Notes : From register 0x0D2f, bits 7:7
 *
 **************************************************************************/
unsigned char get_slot_hiz7()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_0_REG) & SLOT_HIZ_7_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz8
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 0:0
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
           value = old_value | SLOT_HIZ_8_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_8_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz8
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 0:0
 *
 **************************************************************************/
unsigned char get_slot_hiz8()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_8_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz9
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 1:1
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
           value = old_value | SLOT_HIZ_9_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_9_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz9
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 1:1
 *
 **************************************************************************/
unsigned char get_slot_hiz9()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_9_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz10
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 2:2
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
           value = old_value | SLOT_HIZ_10_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_10_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz10
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 2:2
 *
 **************************************************************************/
unsigned char get_slot_hiz10()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_10_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz11
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 3:3
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
           value = old_value | SLOT_HIZ_11_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_11_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz11
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 3:3
 *
 **************************************************************************/
unsigned char get_slot_hiz11()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_11_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz12
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 4:4
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
           value = old_value | SLOT_HIZ_12_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_12_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz12
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 4:4
 *
 **************************************************************************/
unsigned char get_slot_hiz12()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_12_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz13
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 5:5
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
           value = old_value | SLOT_HIZ_13_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_13_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz13
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 5:5
 *
 **************************************************************************/
unsigned char get_slot_hiz13()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_13_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz14
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 6:6
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
           value = old_value | SLOT_HIZ_14_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_14_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz14
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 6:6
 *
 **************************************************************************/
unsigned char get_slot_hiz14()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_14_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz15
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl1
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 7:7
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
           value = old_value | SLOT_HIZ_15_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_15_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz15
 *
 * RET  : Return the value of register ADslotTristateControl1
 *
 * Notes : From register 0x0D30, bits 7:7
 *
 **************************************************************************/
unsigned char get_slot_hiz15()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_1_REG) & SLOT_HIZ_15_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz16
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 0:0
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
           value = old_value | SLOT_HIZ_16_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_16_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz16
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 0:0
 *
 **************************************************************************/
unsigned char get_slot_hiz16()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_16_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz17
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 1:1
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
           value = old_value | SLOT_HIZ_17_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_17_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz17
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 1:1
 *
 **************************************************************************/
unsigned char get_slot_hiz17()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_17_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz18
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 2:2
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
           value = old_value | SLOT_HIZ_18_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_18_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz18
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 2:2
 *
 **************************************************************************/
unsigned char get_slot_hiz18()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_18_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz19
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 3:3
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
           value = old_value | SLOT_HIZ_19_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_19_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz19
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 3:3
 *
 **************************************************************************/
unsigned char get_slot_hiz19()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_19_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz20
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 4:4
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
           value = old_value | SLOT_HIZ_20_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_20_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz20
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 4:4
 *
 **************************************************************************/
unsigned char get_slot_hiz20()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_20_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz21
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 5:5
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
           value = old_value | SLOT_HIZ_21_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_21_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz21
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 5:5
 *
 **************************************************************************/
unsigned char get_slot_hiz21()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_21_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz22
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 6:6
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
           value = old_value | SLOT_HIZ_22_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_22_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz22
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 6:6
 *
 **************************************************************************/
unsigned char get_slot_hiz22()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_22_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz23
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl2
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 7:7
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
           value = old_value | SLOT_HIZ_23_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_23_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz23
 *
 * RET  : Return the value of register ADslotTristateControl2
 *
 * Notes : From register 0x0D31, bits 7:7
 *
 **************************************************************************/
unsigned char get_slot_hiz23()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_2_REG) & SLOT_HIZ_23_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz24
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 0:0
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
           value = old_value | SLOT_HIZ_24_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_24_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz24
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 0:0
 *
 **************************************************************************/
unsigned char get_slot_hiz24()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_24_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz25
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 1:1
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
           value = old_value | SLOT_HIZ_25_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_25_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz25
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 1:1
 *
 **************************************************************************/
unsigned char get_slot_hiz25()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_25_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz26
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 2:2
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
           value = old_value | SLOT_HIZ_26_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_26_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz26
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 2:2
 *
 **************************************************************************/
unsigned char get_slot_hiz26()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_26_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz27
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 3:3
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
           value = old_value | SLOT_HIZ_27_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_27_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz27
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 3:3
 *
 **************************************************************************/
unsigned char get_slot_hiz27()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_27_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz28
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 4:4
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
           value = old_value | SLOT_HIZ_28_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_28_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz28
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 4:4
 *
 **************************************************************************/
unsigned char get_slot_hiz28()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_28_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz29
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 5:5
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
           value = old_value | SLOT_HIZ_29_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_29_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz29
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 5:5
 *
 **************************************************************************/
unsigned char get_slot_hiz29()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_29_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz30
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 6:6
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
           value = old_value | SLOT_HIZ_30_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_30_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz30
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 6:6
 *
 **************************************************************************/
unsigned char get_slot_hiz30()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_30_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_hiz31
 *
 * IN   : param, a value to write to the regiter ADslotTristateControl3
 * OUT  : 
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 7:7
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
           value = old_value | SLOT_HIZ_31_PARAM_MASK; 
           break;
        case SLOT_LOW_IMPEDANCE_E: 
           value = old_value & ~ SLOT_HIZ_31_PARAM_MASK;
           break;
    }
  

    I2CWrite(A_DSLOT_TRISTATE_CONTROL_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_hiz31
 *
 * RET  : Return the value of register ADslotTristateControl3
 *
 * Notes : From register 0x0D32, bits 7:7
 *
 **************************************************************************/
unsigned char get_slot_hiz31()
  {
    unsigned char value;


    /* 
     * Slot impedance after last half-lsb 
     */ 
    value = (I2CRead(A_DSLOT_TRISTATE_CONTROL_3_REG) & SLOT_HIZ_31_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da12_voice_filter_enable
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath1
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath1
 *
 * Notes : From register 0x0D33, bits 7:7
 *
 **************************************************************************/
unsigned char set_da12_voice_filter_enable( enum da12_voice_filter_enable param )
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
           value = old_value | DA_12_VOICE_FILTER_ENABLE_PARAM_MASK; 
           break;
        case DA_12_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ DA_12_VOICE_FILTER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da12_voice_filter_enable
 *
 * RET  : Return the value of register SlotSelectionDAPath1
 *
 * Notes : From register 0x0D33, bits 7:7
 *
 **************************************************************************/
unsigned char get_da12_voice_filter_enable()
  {
    unsigned char value;


    /* 
     * 0: DA1 and DA2 channel paths use Audio Filters 
     * 1: DA1 and DA2 channel paths use 48kHz Low Latency 
     * Filters for Voice 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_1_REG) & DA_12_VOICE_FILTER_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_swap12_34
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath1
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath1
 *
 * Notes : From register 0x0D33, bits 6:6
 *
 **************************************************************************/
unsigned char set_swap12_34( enum swap12_34 param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(SLOT_SELECTION_DA_PATH_1_REG);

    /* 
     * Swap chanel 1&2 with 3&4 
     */ 
    switch( param ){
        case SWAP12_34_E: 
           value = old_value | SWAP_1234_PARAM_MASK; 
           break;
        case NO_SWAP12_34_E: 
           value = old_value & ~ SWAP_1234_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_swap12_34
 *
 * RET  : Return the value of register SlotSelectionDAPath1
 *
 * Notes : From register 0x0D33, bits 6:6
 *
 **************************************************************************/
unsigned char get_swap12_34()
  {
    unsigned char value;


    /* 
     * Swap chanel 1&2 with 3&4 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_1_REG) & SWAP_1234_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in7_to_adout1
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath1
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath1
 *
 * Notes : From register 0x0D33, bits 5:5
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
           value = old_value | DA_IN_TO_AD_OUT_PARAM_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_TO_AD_OUT_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in7_to_adout1
 *
 * RET  : Return the value of register SlotSelectionDAPath1
 *
 * Notes : From register 0x0D33, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in7_to_adout1()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT1 is connected to the corresponding AD path; 
     * 1: DA_IN7 is looped back to AD_OUT1 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_1_REG) & DA_IN_TO_AD_OUT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp01
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath1
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath1
 *
 * Notes : From register 0x0D33, bits 4:0
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
  
    value =  old_value & ~SLOT_DSP_PARAM_MASK;


     value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp01
 *
 * RET  : Return the value of register SlotSelectionDAPath1
 *
 * Notes : From register 0x0D33, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp01()
  {
    unsigned char value;


    /* 
     * Data sent to DA1 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_1_REG) & SLOT_DSP_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in8_to_adout2
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath2
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath2
 *
 * Notes : From register 0x0D34, bits 5:5
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
           value = old_value | DA_IN_TO_AD_OUT_PARAM_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_TO_AD_OUT_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in8_to_adout2
 *
 * RET  : Return the value of register SlotSelectionDAPath2
 *
 * Notes : From register 0x0D34, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in8_to_adout2()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT2 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT2 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_2_REG) & DA_IN_TO_AD_OUT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp02
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath2
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath2
 *
 * Notes : From register 0x0D34, bits 4:0
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
  
    value =  old_value & ~SLOT_DSP_PARAM_MASK;


     value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp02
 *
 * RET  : Return the value of register SlotSelectionDAPath2
 *
 * Notes : From register 0x0D34, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp02()
  {
    unsigned char value;


    /* 
     * Data sent to DA2 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_2_REG) & SLOT_DSP_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da34_voice_filter_enable
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath3
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath3
 *
 * Notes : From register 0x0D35, bits 7:7
 *
 **************************************************************************/
unsigned char set_da34_voice_filter_enable( enum da34_voice_filter_enable param )
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
           value = old_value | DA_34_VOICE_FILTER_ENABLE_PARAM_MASK; 
           break;
        case DA_34_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ DA_34_VOICE_FILTER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da34_voice_filter_enable
 *
 * RET  : Return the value of register SlotSelectionDAPath3
 *
 * Notes : From register 0x0D35, bits 7:7
 *
 **************************************************************************/
unsigned char get_da34_voice_filter_enable()
  {
    unsigned char value;


    /* 
     * 0: DA3 and DA3 channel paths use Audio Filters 
     * 1: DA3 and DA3 channel paths use 48kHz Low Latency 
     * Filters for Voice 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_3_REG) & DA_34_VOICE_FILTER_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in7_to_adout3
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath3
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath3
 *
 * Notes : From register 0x0D35, bits 5:5
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
           value = old_value | DA_IN_TO_AD_OUT_PARAM_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_TO_AD_OUT_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in7_to_adout3
 *
 * RET  : Return the value of register SlotSelectionDAPath3
 *
 * Notes : From register 0x0D35, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in7_to_adout3()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT3 is connected to the corresponding AD path; 
     * 1: DA_IN7 is looped back to AD_OUT3 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_3_REG) & DA_IN_TO_AD_OUT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp03
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath3
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath3
 *
 * Notes : From register 0x0D35, bits 4:0
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
  
    value =  old_value & ~SLOT_DSP_PARAM_MASK;


     value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp03
 *
 * RET  : Return the value of register SlotSelectionDAPath3
 *
 * Notes : From register 0x0D35, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp03()
  {
    unsigned char value;


    /* 
     * Data sent to DA3 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_3_REG) & SLOT_DSP_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in8_to_adout4
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath4
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath4
 *
 * Notes : From register 0x0D36, bits 5:5
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
           value = old_value | DA_IN_TO_AD_OUT_PARAM_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_TO_AD_OUT_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in8_to_adout4
 *
 * RET  : Return the value of register SlotSelectionDAPath4
 *
 * Notes : From register 0x0D36, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in8_to_adout4()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT4 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT4 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_4_REG) & DA_IN_TO_AD_OUT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp04
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath4
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath4
 *
 * Notes : From register 0x0D36, bits 4:0
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
  
    value =  old_value & ~SLOT_DSP_PARAM_MASK;


     value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp04
 *
 * RET  : Return the value of register SlotSelectionDAPath4
 *
 * Notes : From register 0x0D36, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp04()
  {
    unsigned char value;


    /* 
     * Data sent to DA4 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_4_REG) & SLOT_DSP_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da56_voice_filter_enable
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath5
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath5
 *
 * Notes : From register 0x0D37, bits 7:7
 *
 **************************************************************************/
unsigned char set_da56_voice_filter_enable( enum da56_voice_filter_enable param )
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
           value = old_value | DA_56_VOICE_FILTER_ENABLE_PARAM_MASK; 
           break;
        case DA_56_VOICE_FILTER_DISABLE_E: 
           value = old_value & ~ DA_56_VOICE_FILTER_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da56_voice_filter_enable
 *
 * RET  : Return the value of register SlotSelectionDAPath5
 *
 * Notes : From register 0x0D37, bits 7:7
 *
 **************************************************************************/
unsigned char get_da56_voice_filter_enable()
  {
    unsigned char value;


    /* 
     * 0: DA5 and DA6 channel paths use Audio Filters 
     * 1: DA5 and DA6 channel paths use 48kHz Low Latency 
     * Filters for Voice 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_5_REG) & DA_56_VOICE_FILTER_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in7_to_adout5
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath5
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath5
 *
 * Notes : From register 0x0D37, bits 5:5
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
           value = old_value | DA_IN_TO_AD_OUT_PARAM_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_TO_AD_OUT_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in7_to_adout5
 *
 * RET  : Return the value of register SlotSelectionDAPath5
 *
 * Notes : From register 0x0D37, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in7_to_adout5()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT8 is connected to the corresponding AD path; 
     * 1: DA_IN7 is looped back to AD_OUT5 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_5_REG) & DA_IN_TO_AD_OUT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp05
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath5
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath5
 *
 * Notes : From register 0x0D37, bits 4:0
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
  
    value =  old_value & ~SLOT_DSP_PARAM_MASK;


     value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp05
 *
 * RET  : Return the value of register SlotSelectionDAPath5
 *
 * Notes : From register 0x0D37, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp05()
  {
    unsigned char value;


    /* 
     * Data sent to DA5 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_5_REG) & SLOT_DSP_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in8_to_adout6
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath6
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath6
 *
 * Notes : From register 0x0D38, bits 5:5
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
           value = old_value | DA_IN_TO_AD_OUT_PARAM_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_TO_AD_OUT_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in8_to_adout6
 *
 * RET  : Return the value of register SlotSelectionDAPath6
 *
 * Notes : From register 0x0D38, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in8_to_adout6()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT6 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT6 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_6_REG) & DA_IN_TO_AD_OUT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp06
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath6
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath6
 *
 * Notes : From register 0x0D38, bits 4:0
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
  
    value =  old_value & ~SLOT_DSP_PARAM_MASK;


     value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp06
 *
 * RET  : Return the value of register SlotSelectionDAPath6
 *
 * Notes : From register 0x0D38, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp06()
  {
    unsigned char value;


    /* 
     * Data sent to DA6 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_6_REG) & SLOT_DSP_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in8_to_adout7
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath7
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath7
 *
 * Notes : From register 0x0D39, bits 5:5
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
           value = old_value | DA_IN_TO_AD_OUT_PARAM_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_TO_AD_OUT_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in8_to_adout7
 *
 * RET  : Return the value of register SlotSelectionDAPath7
 *
 * Notes : From register 0x0D39, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in8_to_adout7()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT7 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT7 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_7_REG) & DA_IN_TO_AD_OUT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp07
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath7
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath7
 *
 * Notes : From register 0x0D39, bits 4:0
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
  
    value =  old_value & ~SLOT_DSP_PARAM_MASK;


     value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_7_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp07
 *
 * RET  : Return the value of register SlotSelectionDAPath7
 *
 * Notes : From register 0x0D39, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp07()
  {
    unsigned char value;


    /* 
     * Data sent to DA7 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_7_REG) & SLOT_DSP_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da_in8_to_adout8
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath8
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath8
 *
 * Notes : From register 0x0D3A, bits 5:5
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
           value = old_value | DA_IN_TO_AD_OUT_PARAM_MASK; 
           break;
        case AD_OUT_CONNECTED_AD_PATH_E: 
           value = old_value & ~ DA_IN_TO_AD_OUT_PARAM_MASK;
           break;
    }
  

    I2CWrite(SLOT_SELECTION_DA_PATH_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da_in8_to_adout8
 *
 * RET  : Return the value of register SlotSelectionDAPath8
 *
 * Notes : From register 0x0D3A, bits 5:5
 *
 **************************************************************************/
unsigned char get_da_in8_to_adout8()
  {
    unsigned char value;


    /* 
     * 0: AD_OUT8 is connected to the corresponding AD path; 
     * 1: DA_IN8 is looped back to AD_OUT6 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_8_REG) & DA_IN_TO_AD_OUT_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_slot_dsp08
 *
 * IN   : param, a value to write to the regiter SlotSelectionDAPath8
 * OUT  : 
 *
 * RET  : Return the value of register SlotSelectionDAPath8
 *
 * Notes : From register 0x0D3A, bits 4:0
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
  
    value =  old_value & ~SLOT_DSP_PARAM_MASK;


     value |=  param ;  
    I2CWrite(SLOT_SELECTION_DA_PATH_8_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_slot_dsp08
 *
 * RET  : Return the value of register SlotSelectionDAPath8
 *
 * Notes : From register 0x0D3A, bits 4:0
 *
 **************************************************************************/
unsigned char get_slot_dsp08()
  {
    unsigned char value;


    /* 
     * Data sent to DA8 input of DA filter (DSP) 
     */ 
    value = (I2CRead(SLOT_SELECTION_DA_PATH_8_REG) & SLOT_DSP_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_hf_output_bridged_enable
 *
 * IN   : param, a value to write to the regiter ClassDEMIControl
 * OUT  : 
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 7:7
 *
 **************************************************************************/
unsigned char set_hf_output_bridged_enable( enum hf_output_bridged_enable param )
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
           value = old_value | HF_OUTPUT_BRIDGED_ENABLE_PARAM_MASK; 
           break;
        case HF_OUTPUT_BRIDGED_DISABLE_E: 
           value = old_value & ~ HF_OUTPUT_BRIDGED_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CLASS_DEMI_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_hf_output_bridged_enable
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 7:7
 *
 **************************************************************************/
unsigned char get_hf_output_bridged_enable()
  {
    unsigned char value;


    /* 
     * 0: The HFL and HFR Outputs are independent; 
     * 1: The HFL and HFR Outputs are bridged. HFL output is 
     * used for both 
     */ 
    value = (I2CRead(CLASS_DEMI_CONTROL_REG) & HF_OUTPUT_BRIDGED_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_vrib_output_bridged_enable
 *
 * IN   : param, a value to write to the regiter ClassDEMIControl
 * OUT  : 
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 6:6
 *
 **************************************************************************/
unsigned char set_vrib_output_bridged_enable( enum vrib_output_bridged_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLASS_DEMI_CONTROL_REG);

    /* 
     * 0: The VIB1 and VIB2 Outputs are independent; 
     * 1: The VIBL and VIBR Outputs are bridged. VIBL output is 
     * used for both 
     */ 
    switch( param ){
        case VRIB_OUTPUT_BRIDGED_ENABLE_E: 
           value = old_value | VRIB_OUTPUT_BRIDGED_ENABLE_PARAM_MASK; 
           break;
        case VRIB_OUTPUT_BRIDGED_DISABLE_E: 
           value = old_value & ~ VRIB_OUTPUT_BRIDGED_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CLASS_DEMI_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_vrib_output_bridged_enable
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 6:6
 *
 **************************************************************************/
unsigned char get_vrib_output_bridged_enable()
  {
    unsigned char value;


    /* 
     * 0: The VIB1 and VIB2 Outputs are independent; 
     * 1: The VIBL and VIBR Outputs are bridged. VIBL output is 
     * used for both 
     */ 
    value = (I2CRead(CLASS_DEMI_CONTROL_REG) & VRIB_OUTPUT_BRIDGED_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_emi_mode_vib1_enable
 *
 * IN   : param, a value to write to the regiter ClassDEMIControl
 * OUT  : 
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 3:3
 *
 **************************************************************************/
unsigned char set_low_emi_mode_vib1_enable( enum low_emi_mode_vib1_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLASS_DEMI_CONTROL_REG);

    /* 
     * 1: Low EMI mode for VIB1 disabled. 
     * 0: Low EMI mode for VIB1 enabled. 
     */ 
    switch( param ){
        case LOW_EMI_MODE_VIB_1_ENABLE_E: 
           value = old_value | LOW_EMI_MODE_VIB_1_ENABLE_PARAM_MASK; 
           break;
        case LOW_EMI_MODE_VIB_1_DISABLE_E: 
           value = old_value & ~ LOW_EMI_MODE_VIB_1_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CLASS_DEMI_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_emi_mode_vib1_enable
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 3:3
 *
 **************************************************************************/
unsigned char get_low_emi_mode_vib1_enable()
  {
    unsigned char value;


    /* 
     * 1: Low EMI mode for VIB1 disabled. 
     * 0: Low EMI mode for VIB1 enabled. 
     */ 
    value = (I2CRead(CLASS_DEMI_CONTROL_REG) & LOW_EMI_MODE_VIB_1_ENABLE_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_emi_mode_vib2_enable
 *
 * IN   : param, a value to write to the regiter ClassDEMIControl
 * OUT  : 
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 2:2
 *
 **************************************************************************/
unsigned char set_low_emi_mode_vib2_enable( enum low_emi_mode_vib2_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLASS_DEMI_CONTROL_REG);

    /* 
     * 1: Low EMI mode for VIB2 disabled. 
     * 0: Low EMI mode for VIB2 enabled. 
     */ 
    switch( param ){
        case LOW_EMI_MODE_VIB_2_ENABLE_E: 
           value = old_value | LOW_EMI_MODE_VIB_2_ENABLE_PARAM_MASK; 
           break;
        case LOW_EMI_MODE_VIB_2_DISABLE_E: 
           value = old_value & ~ LOW_EMI_MODE_VIB_2_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CLASS_DEMI_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_emi_mode_vib2_enable
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 2:2
 *
 **************************************************************************/
unsigned char get_low_emi_mode_vib2_enable()
  {
    unsigned char value;


    /* 
     * 1: Low EMI mode for VIB2 disabled. 
     * 0: Low EMI mode for VIB2 enabled. 
     */ 
    value = (I2CRead(CLASS_DEMI_CONTROL_REG) & LOW_EMI_MODE_VIB_2_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_emi_mode_hf_left_enable
 *
 * IN   : param, a value to write to the regiter ClassDEMIControl
 * OUT  : 
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 1:1
 *
 **************************************************************************/
unsigned char set_low_emi_mode_hf_left_enable( enum low_emi_mode_hf_left_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLASS_DEMI_CONTROL_REG);

    /* 
     * 1: Low EMI mode for HFL disabled. 
     * 0: Low EMI mode for HFL enabled. 
     */ 
    switch( param ){
        case LOW_EMI_MODE_HF_LEFT_ENABLE_E: 
           value = old_value | LOW_EMI_MODE_HF_LEFT_ENABLE_PARAM_MASK; 
           break;
        case LOW_EMI_MODE_HF_LEFT_DISABLE_E: 
           value = old_value & ~ LOW_EMI_MODE_HF_LEFT_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CLASS_DEMI_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_emi_mode_hf_left_enable
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 1:1
 *
 **************************************************************************/
unsigned char get_low_emi_mode_hf_left_enable()
  {
    unsigned char value;


    /* 
     * 1: Low EMI mode for HFL disabled. 
     * 0: Low EMI mode for HFL enabled. 
     */ 
    value = (I2CRead(CLASS_DEMI_CONTROL_REG) & LOW_EMI_MODE_HF_LEFT_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_low_emi_mode_hf_right_enable
 *
 * IN   : param, a value to write to the regiter ClassDEMIControl
 * OUT  : 
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 0:0
 *
 **************************************************************************/
unsigned char set_low_emi_mode_hf_right_enable( enum low_emi_mode_hf_right_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(CLASS_DEMI_CONTROL_REG);

    /* 
     * 1: Low EMI mode for HFR disabled. 
     * 0: Low EMI mode for HFR enabled. 
     */ 
    switch( param ){
        case LOW_EMI_MODE_HF_RIGHT_ENABLE_E: 
           value = old_value | LOW_EMI_MODE_HF_RIGHT_ENABLE_PARAM_MASK; 
           break;
        case LOW_EMI_MODE_HF_RIGHT_DISABLE_E: 
           value = old_value & ~ LOW_EMI_MODE_HF_RIGHT_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(CLASS_DEMI_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_low_emi_mode_hf_right_enable
 *
 * RET  : Return the value of register ClassDEMIControl
 *
 * Notes : From register 0x0D3B, bits 0:0
 *
 **************************************************************************/
unsigned char get_low_emi_mode_hf_right_enable()
  {
    unsigned char value;


    /* 
     * 1: Low EMI mode for HFR disabled. 
     * 0: Low EMI mode for HFR enabled. 
     */ 
    value = (I2CRead(CLASS_DEMI_CONTROL_REG) & LOW_EMI_MODE_HF_RIGHT_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_class_d_fir_bypass
 *
 * IN   : param, a value to write to the regiter ClassDPath
 * OUT  : 
 *
 * RET  : Return the value of register ClassDPath
 *
 * Notes : From register 0x0D3c, bits 7:4
 *
 **************************************************************************/
unsigned char set_class_d_fir_bypass( enum class__d__fir_bypass param )
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
  

     value =  old_value & ~CLASS_D_FIR_BYPASS_PARAM_MASK ;

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
 * RET  : Return the value of register ClassDPath
 *
 * Notes : From register 0x0D3c, bits 7:4
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
    value = (I2CRead(CLASS_D_PATH_REG) & CLASS_D_FIR_BYPASS_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_high_volume
 *
 * IN   : param, a value to write to the regiter ClassDPath
 * OUT  : 
 *
 * RET  : Return the value of register ClassDPath
 *
 * Notes : From register 0x0D3c, bits 3:0
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
  

     value =  old_value & ~HIGH_VOLUME_PARAM_MASK ;

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
 * RET  : Return the value of register ClassDPath
 *
 * Notes : From register 0x0D3c, bits 3:0
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
    value = (I2CRead(CLASS_D_PATH_REG) & HIGH_VOLUME_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_high_pass_component_dither
 *
 * IN   : param, a value to write to the regiter ClassDControlGain
 * OUT  : 
 *
 * RET  : Return the value of register ClassDControlGain
 *
 * Notes : From register 0x0D3D, bits 7:4
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
  
    value =  old_value & ~HIGH_PASS_COMPONENT_DITHER_PARAM_MASK;


    value |= ( param << 0x4);  

    I2CWrite(CLASS_D_CONTROL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_high_pass_component_dither
 *
 * RET  : Return the value of register ClassDControlGain
 *
 * Notes : From register 0x0D3D, bits 7:4
 *
 **************************************************************************/
unsigned char get_high_pass_component_dither()
  {
    unsigned char value;


    /* 
     * Gain control for the high pass component of dither 
     */ 
    value = (I2CRead(CLASS_D_CONTROL_GAIN_REG) & HIGH_PASS_COMPONENT_DITHER_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_highwhite_component_dither
 *
 * IN   : param, a value to write to the regiter ClassDControlGain
 * OUT  : 
 *
 * RET  : Return the value of register ClassDControlGain
 *
 * Notes : From register 0x0D3D, bits 3:0
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
  
    value =  old_value & ~HIGH_WHITE_COMPONENT_DITHER_PARAM_MASK;


    value |=  param ;  

    I2CWrite(CLASS_D_CONTROL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_highwhite_component_dither
 *
 * RET  : Return the value of register ClassDControlGain
 *
 * Notes : From register 0x0D3D, bits 3:0
 *
 **************************************************************************/
unsigned char get_highwhite_component_dither()
  {
    unsigned char value;


    /* 
     * Gain control for the white component of dither 
     */ 
    value = (I2CRead(CLASS_D_CONTROL_GAIN_REG) & HIGH_WHITE_COMPONENT_DITHER_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_input
 *
 * IN   : param, a value to write to the regiter DecimatorFilter
 * OUT  : 
 *
 * RET  : Return the value of register DecimatorFilter
 *
 * Notes : From register 0x0D3E, bits 7:7
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
           value = old_value | ANC_INPUT_PARAM_MASK; 
           break;
        case ANC_INPUT_AD6_E: 
           value = old_value & ~ ANC_INPUT_PARAM_MASK;
           break;
    }
  

    I2CWrite(DECIMATOR_FILTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_anc_input
 *
 * RET  : Return the value of register DecimatorFilter
 *
 * Notes : From register 0x0D3E, bits 7:7
 *
 **************************************************************************/
unsigned char get_anc_input()
  {
    unsigned char value;


    /* 
     * 0: ANC input is AD6 
     * 1: ANC input is AD5 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & ANC_INPUT_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ear_dac_source
 *
 * IN   : param, a value to write to the regiter DecimatorFilter
 * OUT  : 
 *
 * RET  : Return the value of register DecimatorFilter
 *
 * Notes : From register 0x0D3E, bits 6:6
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
           value = old_value | EAR_DAC_SOURCE_PARAM_MASK; 
           break;
        case SIGNAL_FORM_DA1_PATH_E: 
           value = old_value & ~ EAR_DAC_SOURCE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DECIMATOR_FILTER_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ear_dac_source
 *
 * RET  : Return the value of register DecimatorFilter
 *
 * Notes : From register 0x0D3E, bits 6:6
 *
 **************************************************************************/
unsigned char get_ear_dac_source()
  {
    unsigned char value;


    /* 
     * 0: EarDAC signal comes from DA1 path 
     * 1: EarDAC signal comes from DA3 path 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & EAR_DAC_SOURCE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic1
 *
 * RET  : Return the value of register DecimatorFilter
 *
 * Notes : From register 0x0D3E, bits 5:5
 *
 **************************************************************************/
unsigned char get_decimator_mic1()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC1; 
     * 1: Sinc3 decimator filter is selected for DMIC1 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_1_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic2
 *
 * RET  : Return the value of register DecimatorFilter
 *
 * Notes : From register 0x0D3E, bits 4:4
 *
 **************************************************************************/
unsigned char get_decimator_mic2()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC2; 
     * 1: Sinc3 decimator filter is selected for DMIC2 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_2_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic3
 *
 * RET  : Return the value of register DecimatorFilter
 *
 * Notes : From register 0x0D3E, bits 3:3
 *
 **************************************************************************/
unsigned char get_decimator_mic3()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC3; 
     * 1: Sinc3 decimator filter is selected for DMIC3 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_3_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic4
 *
 * RET  : Return the value of register DecimatorFilter
 *
 * Notes : From register 0x0D3E, bits 2:2
 *
 **************************************************************************/
unsigned char get_decimator_mic4()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC4; 
     * 1: Sinc3 decimator filter is selected for DMIC4 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_4_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic5
 *
 * RET  : Return the value of register DecimatorFilter
 *
 * Notes : From register 0x0D3E, bits 1:1
 *
 **************************************************************************/
unsigned char get_decimator_mic5()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC5; 
     * 1: Sinc3 decimator filter is selected for DMIC5 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_5_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_decimator_mic6
 *
 * RET  : Return the value of register DecimatorFilter
 *
 * Notes : From register 0x0D3E, bits 0:0
 *
 **************************************************************************/
unsigned char get_decimator_mic6()
  {
    unsigned char value;


    /* 
     * 0: Sinc5 decimator filter is selected for DMIC6; 
     * 1: Sinc3 decimator filter is selected for DMIC6 
     */ 
    value = (I2CRead(DECIMATOR_FILTER_REG) & DECIMATOR_MIC_6_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_output_digital_in1
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 7:7
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
           value = old_value | PATH_IN_1_PARAM_MASK; 
           break;
        case IN1_CONNECTED_TO_HSL_E: 
           value = old_value & ~ PATH_IN_1_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_output_digital_in1
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 7:7
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
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & PATH_IN_1_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_output_digital_in2
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 6:6
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
           value = old_value | PATH_IN_2_PARAM_MASK; 
           break;
        case IN2_CONNECTED_TO_HSR_E: 
           value = old_value & ~ PATH_IN_2_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_output_digital_in2
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 6:6
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
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & PATH_IN_2_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dig_micro1_for_out1
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 5:5
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
           value = old_value | DIG_MICRO_1_FOR_OUT_1_PARAM_MASK; 
           break;
        case MICRO_ANALOG_E: 
           value = old_value & ~ DIG_MICRO_1_FOR_OUT_1_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dig_micro1_for_out1
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 5:5
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
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & DIG_MICRO_1_FOR_OUT_1_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dig_micro2_for_out2
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 4:4
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
           value = old_value | DIG_MICRO_2_FOR_OUT_2_PARAM_MASK; 
           break;
        case MICRO_ANALOG_E: 
           value = old_value & ~ DIG_MICRO_2_FOR_OUT_2_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dig_micro2_for_out2
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 4:4
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
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & DIG_MICRO_2_FOR_OUT_2_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dig_micro3_for_out3
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 3:3
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
           value = old_value | DIG_MICRO_3_FOR_OUT_3_PARAM_MASK; 
           break;
        case MICRO_ANALOG_E: 
           value = old_value & ~ DIG_MICRO_3_FOR_OUT_3_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dig_micro3_for_out3
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 3:3
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
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & DIG_MICRO_3_FOR_OUT_3_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dig_micro5_for_out5
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 2:2
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
           value = old_value | DIG_MICRO_5_FOR_OUT_5_PARAM_MASK; 
           break;
        case MICRO_ANALOG_E: 
           value = old_value & ~ DIG_MICRO_5_FOR_OUT_5_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dig_micro5_for_out5
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 2:2
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
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & DIG_MICRO_5_FOR_OUT_5_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_dig_micro6_for_out6
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 1:1
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
           value = old_value | DIG_MICRO_6_FOR_OUT_6_PARAM_MASK; 
           break;
        case MICRO_ANALOG_E: 
           value = old_value & ~ DIG_MICRO_6_FOR_OUT_6_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_dig_micro6_for_out6
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 1:1
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
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & DIG_MICRO_6_FOR_OUT_6_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_loop_mixed
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexers
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 0:0
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
           value = old_value | ANC_LOOP_MIXED_PARAM_MASK; 
           break;
        case NOT_MIXED_E: 
           value = old_value & ~ ANC_LOOP_MIXED_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_anc_loop_mixed
 *
 * RET  : Return the value of register DigitalMultiplexers
 *
 * Notes : From register 0x0D3F, bits 0:0
 *
 **************************************************************************/
unsigned char get_anc_loop_mixed()
  {
    unsigned char value;


    /* 
     * 0: ANC loop not mixed driver. 
     * 1: ANC loop mixed in earpiece driver. 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_REG) & ANC_LOOP_MIXED_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da4_or_anc_loop_path
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 7:7
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
           value = old_value | DA_4_OR_ANC_MIXING_ENABLE_PARAM_MASK; 
           break;
        case DA_4_OR_ANC_MIXING_DISABLE_E: 
           value = old_value & ~ DA_4_OR_ANC_MIXING_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da4_or_anc_loop_path
 *
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 7:7
 *
 **************************************************************************/
unsigned char get_da4_or_anc_loop_path()
  {
    unsigned char value;


    /* 
     * 0: DA4 or ANC path to HfR is muted 
     * 1: DA4 or ANC path to HfR is enabled 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & DA_4_OR_ANC_MIXING_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da3_or_anc_loop_path
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 6:6
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
           value = old_value | DA_3_OR_ANC_MIXING_ENABLE_PARAM_MASK; 
           break;
        case DA_3_OR_ANC_MIXING_DISABLE_E: 
           value = old_value & ~ DA_3_OR_ANC_MIXING_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da3_or_anc_loop_path
 *
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 6:6
 *
 **************************************************************************/
unsigned char get_da3_or_anc_loop_path()
  {
    unsigned char value;


    /* 
     * 0: DA3 or ANC path to HfL is muted 
     * 1: DA3 or ANC path to HfL is enabled 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & DA_3_OR_ANC_MIXING_ENABLE_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da4_mixing_enable
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 5:5
 *
 **************************************************************************/
unsigned char set_da4_mixing_enable( enum da4_mixing_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG);

    /* 
     * 0: HFR data comes from DA4 if DAToHfREn=1 
     * 1: HFR data comes from ANC if DAToHfREn= 
     */ 
    switch( param ){
        case DA_4_MIXING_DISABLE_E: 
           value = old_value | DA_4_MIXING_ENABLE_PARAM_MASK; 
           break;
        case DA_4_MIXING_ENABLE_E: 
           value = old_value & ~ DA_4_MIXING_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da4_mixing_enable
 *
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 5:5
 *
 **************************************************************************/
unsigned char get_da4_mixing_enable()
  {
    unsigned char value;


    /* 
     * 0: HFR data comes from DA4 if DAToHfREn=1 
     * 1: HFR data comes from ANC if DAToHfREn= 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & DA_4_MIXING_ENABLE_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da3_mixing_enable
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 4:4
 *
 **************************************************************************/
unsigned char set_da3_mixing_enable( enum da3_mixing_enable param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG);

    /* 
     * 0: HFL data comes from DA3 if DAToHfLEn=1 
     * 1: HFL data comes from ANC if DAToHfLEn=1 
     */ 
    switch( param ){
        case DA_3_MIXING_DISABLE_E: 
           value = old_value | DA_3_MIXING_ENABLE_PARAM_MASK; 
           break;
        case DA_3_MIXING_ENABLE_E: 
           value = old_value & ~ DA_3_MIXING_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_MULTIPLEXERS_MSB_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da3_mixing_enable
 *
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 4:4
 *
 **************************************************************************/
unsigned char get_da3_mixing_enable()
  {
    unsigned char value;


    /* 
     * 0: HFL data comes from DA3 if DAToHfLEn=1 
     * 1: HFL data comes from ANC if DAToHfLEn=1 
     */ 
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & DA_3_MIXING_ENABLE_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir1_selection
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 3:2
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
  

     value =  old_value & ~SELECTION_FIR_1_PARAM_MASK ;

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
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 3:2
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
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & SELECTION_FIR_1_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir2_selection
 *
 * IN   : param, a value to write to the regiter DigitalMultiplexersMsb
 * OUT  : 
 *
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 1:0
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
  

     value =  old_value & ~SELECTION_FIR_2_PARAM_MASK ;

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
 * RET  : Return the value of register DigitalMultiplexersMsb
 *
 * Notes : From register 0x0D40, bits 1:0
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
    value = (I2CRead(DIGITAL_MULTIPLEXERS_MSB_REG) & SELECTION_FIR_2_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD1
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD1
 *
 * Notes : From register 0x0D41, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_anti_click
 *
 * RET  : Return the value of register DigitalGainAD1
 *
 * Notes : From register 0x0D41, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad1_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD1 path is active; 
     * 1: Fading (anti-click) on AD1 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD1_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD1
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD1
 *
 * Notes : From register 0x0D41, bits 5:0
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
  
    value =  old_value & ~DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_gain
 *
 * RET  : Return the value of register DigitalGainAD1
 *
 * Notes : From register 0x0D41, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad1_gain()
  {
    unsigned char value;


    /* 
     * AD1 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD1_REG) & DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD2
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD2
 *
 * Notes : From register 0x0D42, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_anti_click
 *
 * RET  : Return the value of register DigitalGainAD2
 *
 * Notes : From register 0x0D42, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad2_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD2 path is active; 
     * 1: Fading (anti-click) on AD2 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD2_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD2
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD2
 *
 * Notes : From register 0x0D42, bits 5:0
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
  
    value =  old_value & ~DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_gain
 *
 * RET  : Return the value of register DigitalGainAD2
 *
 * Notes : From register 0x0D42, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad2_gain()
  {
    unsigned char value;


    /* 
     * AD2 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD2_REG) & DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad3_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD3
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD3
 *
 * Notes : From register 0x0D43, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad3_anti_click
 *
 * RET  : Return the value of register DigitalGainAD3
 *
 * Notes : From register 0x0D43, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad3_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD3 path is active; 
     * 1: Fading (anti-click) on AD3 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD3_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad3_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD3
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD3
 *
 * Notes : From register 0x0D43, bits 5:0
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
  
    value =  old_value & ~DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad3_gain
 *
 * RET  : Return the value of register DigitalGainAD3
 *
 * Notes : From register 0x0D43, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad3_gain()
  {
    unsigned char value;


    /* 
     * AD3 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD3_REG) & DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad4_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD4
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD4
 *
 * Notes : From register 0x0D44, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad4_anti_click
 *
 * RET  : Return the value of register DigitalGainAD4
 *
 * Notes : From register 0x0D44, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad4_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD4 path is active; 
     * 1: Fading (anti-click) on AD4 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD4_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad4_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD4
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD4
 *
 * Notes : From register 0x0D44, bits 5:0
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
  
    value =  old_value & ~DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad4_gain
 *
 * RET  : Return the value of register DigitalGainAD4
 *
 * Notes : From register 0x0D44, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad4_gain()
  {
    unsigned char value;


    /* 
     * AD4 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD4_REG) & DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad5_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD5
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD5
 *
 * Notes : From register 0x0D45, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad5_anti_click
 *
 * RET  : Return the value of register DigitalGainAD5
 *
 * Notes : From register 0x0D45, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad5_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD5 path is active; 
     * 1: Fading (anti-click) on AD5 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD5_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad5_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD5
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD5
 *
 * Notes : From register 0x0D45, bits 5:0
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
  
    value =  old_value & ~DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad5_gain
 *
 * RET  : Return the value of register DigitalGainAD5
 *
 * Notes : From register 0x0D45, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad5_gain()
  {
    unsigned char value;


    /* 
     * AD5 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD5_REG) & DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad6_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD6
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD6
 *
 * Notes : From register 0x0D46, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad6_anti_click
 *
 * RET  : Return the value of register DigitalGainAD6
 *
 * Notes : From register 0x0D46, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad6_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD6 path is active; 
     * 1: Fading (anti-click) on AD6 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD6_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad6_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD6
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD6
 *
 * Notes : From register 0x0D46, bits 5:0
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
  
    value =  old_value & ~DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad6_gain
 *
 * RET  : Return the value of register DigitalGainAD6
 *
 * Notes : From register 0x0D46, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad6_gain()
  {
    unsigned char value;


    /* 
     * AD6 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD6_REG) & DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da1_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDa1
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDa1
 *
 * Notes : From register 0x0D47, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da1_anti_click
 *
 * RET  : Return the value of register DigitalGainDa1
 *
 * Notes : From register 0x0D47, bits 6:6
 *
 **************************************************************************/
unsigned char get_da1_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA1 path is active; 
     * 1: Fading (anti-click) on DA1 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_1_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da1_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDa1
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDa1
 *
 * Notes : From register 0x0D47, bits 5:0
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
  
    value =  old_value & ~DA_DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da1_gain
 *
 * RET  : Return the value of register DigitalGainDa1
 *
 * Notes : From register 0x0D47, bits 5:0
 *
 **************************************************************************/
unsigned char get_da1_gain()
  {
    unsigned char value;


    /* 
     * DA1 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_1_REG) & DA_DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da2_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDa2
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDa2
 *
 * Notes : From register 0x0D48, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da2_anti_click
 *
 * RET  : Return the value of register DigitalGainDa2
 *
 * Notes : From register 0x0D48, bits 6:6
 *
 **************************************************************************/
unsigned char get_da2_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA2 path is active; 
     * 1: Fading (anti-click) on DA2 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_2_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da2_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDa2
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDa2
 *
 * Notes : From register 0x0D48, bits 5:0
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
  
    value =  old_value & ~DA_DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da2_gain
 *
 * RET  : Return the value of register DigitalGainDa2
 *
 * Notes : From register 0x0D48, bits 5:0
 *
 **************************************************************************/
unsigned char get_da2_gain()
  {
    unsigned char value;


    /* 
     * DA2 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_2_REG) & DA_DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da3_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDa3
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDa3
 *
 * Notes : From register 0x0D49, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da3_anti_click
 *
 * RET  : Return the value of register DigitalGainDa3
 *
 * Notes : From register 0x0D49, bits 6:6
 *
 **************************************************************************/
unsigned char get_da3_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA3 path is active; 
     * 1: Fading (anti-click) on DA3 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_3_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da3_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDa3
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDa3
 *
 * Notes : From register 0x0D49, bits 5:0
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
  
    value =  old_value & ~DA_DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA_3_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da3_gain
 *
 * RET  : Return the value of register DigitalGainDa3
 *
 * Notes : From register 0x0D49, bits 5:0
 *
 **************************************************************************/
unsigned char get_da3_gain()
  {
    unsigned char value;


    /* 
     * DA3 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_3_REG) & DA_DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da4_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDa4
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDa4
 *
 * Notes : From register 0x0D4A, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da4_anti_click
 *
 * RET  : Return the value of register DigitalGainDa4
 *
 * Notes : From register 0x0D4A, bits 6:6
 *
 **************************************************************************/
unsigned char get_da4_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA4 path is active; 
     * 1: Fading (anti-click) on DA4 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_4_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da4_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDa4
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDa4
 *
 * Notes : From register 0x0D4A, bits 5:0
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
  
    value =  old_value & ~DA_DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA_4_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da4_gain
 *
 * RET  : Return the value of register DigitalGainDa4
 *
 * Notes : From register 0x0D4A, bits 5:0
 *
 **************************************************************************/
unsigned char get_da4_gain()
  {
    unsigned char value;


    /* 
     * DA4 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_4_REG) & DA_DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da5_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDa5
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDa5
 *
 * Notes : From register 0x0D4B, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da5_anti_click
 *
 * RET  : Return the value of register DigitalGainDa5
 *
 * Notes : From register 0x0D4B, bits 6:6
 *
 **************************************************************************/
unsigned char get_da5_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA5 path is active; 
     * 1: Fading (anti-click) on DA5 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_5_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da5_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDa5
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDa5
 *
 * Notes : From register 0x0D4B, bits 5:0
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
  
    value =  old_value & ~DA_DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA_5_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da5_gain
 *
 * RET  : Return the value of register DigitalGainDa5
 *
 * Notes : From register 0x0D4B, bits 5:0
 *
 **************************************************************************/
unsigned char get_da5_gain()
  {
    unsigned char value;


    /* 
     * DA5 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA_5_REG) & DA_DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_da6_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainDA6
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDA6
 *
 * Notes : From register 0x0D4C, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_DA6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da6_anti_click
 *
 * RET  : Return the value of register DigitalGainDA6
 *
 * Notes : From register 0x0D4C, bits 6:6
 *
 **************************************************************************/
unsigned char get_da6_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on DA6 path is active; 
     * 1: Fading (anti-click) on DA6 path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA6_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_da6_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainDA6
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainDA6
 *
 * Notes : From register 0x0D4C, bits 5:0
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
  
    value =  old_value & ~DA_DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_DA6_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_da6_gain
 *
 * RET  : Return the value of register DigitalGainDA6
 *
 * Notes : From register 0x0D4C, bits 5:0
 *
 **************************************************************************/
unsigned char get_da6_gain()
  {
    unsigned char value;


    /* 
     * DA6 path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_DA6_REG) & DA_DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_hfl_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD1_HFL
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD1_HFL
 *
 * Notes : From register 0x0D4D, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD1HFL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_hfl_anti_click
 *
 * RET  : Return the value of register DigitalGainAD1_HFL
 *
 * Notes : From register 0x0D4D, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad1_hfl_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD1_HFL path is active; 
     * 1: Fading (anti-click) on AD1_HFL path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD1HFL_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad1_hfl_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD1_HFL
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD1_HFL
 *
 * Notes : From register 0x0D4D, bits 5:0
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
  
    value =  old_value & ~DA_DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD1HFL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad1_hfl_gain
 *
 * RET  : Return the value of register DigitalGainAD1_HFL
 *
 * Notes : From register 0x0D4D, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad1_hfl_gain()
  {
    unsigned char value;


    /* 
     * AD1_HFL path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD1HFL_REG) & DA_DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_hfr_r_anti_click
 *
 * IN   : param, a value to write to the regiter DigitalGainAD2_HFR
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD2_HFR
 *
 * Notes : From register 0x0D4E, bits 6:6
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
           value = old_value | FADING_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_AD2HFR_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_hfr_r_anti_click
 *
 * RET  : Return the value of register DigitalGainAD2_HFR
 *
 * Notes : From register 0x0D4E, bits 6:6
 *
 **************************************************************************/
unsigned char get_ad2_hfr_r_anti_click()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on AD2_HFR path is active; 
     * 1: Fading (anti-click) on AD2_HFR path is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD2HFR_REG) & FADING_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_ad2_hfr_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainAD2_HFR
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainAD2_HFR
 *
 * Notes : From register 0x0D4E, bits 5:0
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
  
    value =  old_value & ~DA_DIGITAL_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_AD2HFR_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_ad2_hfr_gain
 *
 * RET  : Return the value of register DigitalGainAD2_HFR
 *
 * Notes : From register 0x0D4E, bits 5:0
 *
 **************************************************************************/
unsigned char get_ad2_hfr_gain()
  {
    unsigned char value;


    /* 
     * AD2_HFR path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_AD2HFR_REG) & DA_DIGITAL_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_sinc1_interpolator
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetLeft
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainHeadsetLeft
 *
 * Notes : From register 0x0D4F, bits 7:7
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
           value = old_value | SINC_1_INTERPOLATOR_ENABLE_PARAM_MASK; 
           break;
        case SINC_1_INTERPOLATOR_DISABLE_E: 
           value = old_value & ~ SINC_1_INTERPOLATOR_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_HEADSET_LEFT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_sinc1_interpolator
 *
 * RET  : Return the value of register DigitalGainHeadsetLeft
 *
 * Notes : From register 0x0D4F, bits 7:7
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
    value = (I2CRead(DIGITAL_GAIN_HEADSET_LEFT_REG) & SINC_1_INTERPOLATOR_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fading_headset_left
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetLeft
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainHeadsetLeft
 *
 * Notes : From register 0x0D4F, bits 4:4
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
           value = old_value | FADING_HEADSET_LEFT_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_HEADSET_LEFT_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_HEADSET_LEFT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fading_headset_left
 *
 * RET  : Return the value of register DigitalGainHeadsetLeft
 *
 * Notes : From register 0x0D4F, bits 4:4
 *
 **************************************************************************/
unsigned char get_fading_headset_left()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on HFL digital gain is active 
     * 1: Fading (anti-click) on HFL digital gain is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_HEADSET_LEFT_REG) & FADING_HEADSET_LEFT_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_left_headset_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetLeft
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainHeadsetLeft
 *
 * Notes : From register 0x0D4F, bits 3:0
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
  
    value =  old_value & ~HEADSET_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_HEADSET_LEFT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_left_headset_gain
 *
 * RET  : Return the value of register DigitalGainHeadsetLeft
 *
 * Notes : From register 0x0D4F, bits 3:0
 *
 **************************************************************************/
unsigned char get_left_headset_gain()
  {
    unsigned char value;


    /* 
     * HSL or Earpiece path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_HEADSET_LEFT_REG) & HEADSET_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_headset_fade_speed
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetRight
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainHeadsetRight
 *
 * Notes : From register 0x0D50, bits 7:6
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
  

     value =  old_value & ~HEADSET_FADE_SPEED_PARAM_MASK ;

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
 * RET  : Return the value of register DigitalGainHeadsetRight
 *
 * Notes : From register 0x0D50, bits 7:6
 *
 **************************************************************************/
unsigned char get_headset_fade_speed()
  {
    unsigned char value;


    /* 
     * Approximate Time between each 1dB step Gain Change in 
     * digital gain controls 
     */ 
    value = (I2CRead(DIGITAL_GAIN_HEADSET_RIGHT_REG) & HEADSET_FADE_SPEED_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fading_headset_right
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetRight
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainHeadsetRight
 *
 * Notes : From register 0x0D50, bits 4:4
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
           value = old_value | FADING_HEADSET_RIGHT_PARAM_MASK; 
           break;
        case FADING_ACTIVE_E: 
           value = old_value & ~ FADING_HEADSET_RIGHT_PARAM_MASK;
           break;
    }
  

    I2CWrite(DIGITAL_GAIN_HEADSET_RIGHT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fading_headset_right
 *
 * RET  : Return the value of register DigitalGainHeadsetRight
 *
 * Notes : From register 0x0D50, bits 4:4
 *
 **************************************************************************/
unsigned char get_fading_headset_right()
  {
    unsigned char value;


    /* 
     * 0: Fading (anti-click) on HFR digital gain is active 
     * 1: Fading (anti-click) on HFR digital gain is disabled 
     */ 
    value = (I2CRead(DIGITAL_GAIN_HEADSET_RIGHT_REG) & FADING_HEADSET_RIGHT_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_right_headset_gain
 *
 * IN   : param, a value to write to the regiter DigitalGainHeadsetRight
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainHeadsetRight
 *
 * Notes : From register 0x0D50, bits 3:0
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
  
    value =  old_value & ~HEADSET_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_HEADSET_RIGHT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_right_headset_gain
 *
 * RET  : Return the value of register DigitalGainHeadsetRight
 *
 * Notes : From register 0x0D50, bits 3:0
 *
 **************************************************************************/
unsigned char get_right_headset_gain()
  {
    unsigned char value;


    /* 
     * Headset Rigth path digital gain 
     */ 
    value = (I2CRead(DIGITAL_GAIN_HEADSET_RIGHT_REG) & HEADSET_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_gain_side_tone1
 *
 * IN   : param, a value to write to the regiter DigitalGainSideTone1
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainSideTone1
 *
 * Notes : From register 0x0D51, bits 4:0
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
  
    value =  old_value & ~DIGITAL_GAIN_SIDE_TONE_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_SIDE_TONE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_gain_side_tone1
 *
 * RET  : Return the value of register DigitalGainSideTone1
 *
 * Notes : From register 0x0D51, bits 4:0
 *
 **************************************************************************/
unsigned char get_digital_gain_side_tone1()
  {
    unsigned char value;


    /* 
     * Digital gain for side tone 1 path. 
     */ 
    value = (I2CRead(DIGITAL_GAIN_SIDE_TONE_1_REG) & DIGITAL_GAIN_SIDE_TONE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_digital_gain_side_tone2
 *
 * IN   : param, a value to write to the regiter DigitalGainSideTone2
 * OUT  : 
 *
 * RET  : Return the value of register DigitalGainSideTone2
 *
 * Notes : From register 0x0D52, bits 4:0
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
  
    value =  old_value & ~DIGITAL_GAIN_SIDE_TONE_PARAM_MASK;


     value |=  param ;  
    I2CWrite(DIGITAL_GAIN_SIDE_TONE_2_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_digital_gain_side_tone2
 *
 * RET  : Return the value of register DigitalGainSideTone2
 *
 * Notes : From register 0x0D52, bits 4:0
 *
 **************************************************************************/
unsigned char get_digital_gain_side_tone2()
  {
    unsigned char value;


    /* 
     * Digital gain for side tone 2 path. 
     */ 
    value = (I2CRead(DIGITAL_GAIN_SIDE_TONE_2_REG) & DIGITAL_GAIN_SIDE_TONE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_iir_update
 *
 * IN   : param, a value to write to the regiter AncDigitalGain
 * OUT  : 
 *
 * RET  : Return the value of register AncDigitalGain
 *
 * Notes : From register 0x0D53, bits 3:3
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
           value = old_value | ANC_IIR_POINTER_PARAM_MASK; 
           break;
        case ANC_IIR_POINTER_RESET_E: 
           value = old_value & ~ ANC_IIR_POINTER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANC_DIGITAL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_anc_iir_update
 *
 * RET  : Return the value of register AncDigitalGain
 *
 * Notes : From register 0x0D53, bits 3:3
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
    value = (I2CRead(ANC_DIGITAL_GAIN_REG) & ANC_IIR_POINTER_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_operation_enable
 *
 * IN   : param, a value to write to the regiter AncDigitalGain
 * OUT  : 
 *
 * RET  : Return the value of register AncDigitalGain
 *
 * Notes : From register 0x0D53, bits 2:2
 *
 **************************************************************************/
unsigned char set_anc_operation_enable( enum anc_operation_enable param )
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
           value = old_value | ANC_OPERATION_ENABLE_PARAM_MASK; 
           break;
        case ANC_OPERATION_DISABLE_E: 
           value = old_value & ~ ANC_OPERATION_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANC_DIGITAL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_anc_operation_enable
 *
 * RET  : Return the value of register AncDigitalGain
 *
 * Notes : From register 0x0D53, bits 2:2
 *
 **************************************************************************/
unsigned char get_anc_operation_enable()
  {
    unsigned char value;


    /* 
     * 0: ANC operation are disabled. FIR and IIR coefficients 
     * cannot be accessed for writing 
     * 1: ANC operation are enabled 
     */ 
    value = (I2CRead(ANC_DIGITAL_GAIN_REG) & ANC_OPERATION_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_iir_status
 *
 * IN   : param, a value to write to the regiter AncDigitalGain
 * OUT  : 
 *
 * RET  : Return the value of register AncDigitalGain
 *
 * Notes : From register 0x0D53, bits 1:1
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
           value = old_value | IIR_STATUS_PARAM_MASK; 
           break;
        case IIR_CANNOT_START_E: 
           value = old_value & ~ IIR_STATUS_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANC_DIGITAL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_iir_status
 *
 * RET  : Return the value of register AncDigitalGain
 *
 * Notes : From register 0x0D53, bits 1:1
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
    value = (I2CRead(ANC_DIGITAL_GAIN_REG) & IIR_STATUS_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir_coefficient_pointer
 *
 * IN   : param, a value to write to the regiter AncDigitalGain
 * OUT  : 
 *
 * RET  : Return the value of register AncDigitalGain
 *
 * Notes : From register 0x0D53, bits 0:0
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
           value = old_value | FIR_COEFFICIENT_POINTER_PARAM_MASK; 
           break;
        case POINTER_RESET_E: 
           value = old_value & ~ FIR_COEFFICIENT_POINTER_PARAM_MASK;
           break;
    }
  

    I2CWrite(ANC_DIGITAL_GAIN_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir_coefficient_pointer
 *
 * RET  : Return the value of register AncDigitalGain
 *
 * Notes : From register 0x0D53, bits 0:0
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
    value = (I2CRead(ANC_DIGITAL_GAIN_REG) & FIR_COEFFICIENT_POINTER_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_warped_shift_in
 *
 * IN   : param, a value to write to the regiter WarpedDelayLine
 * OUT  : 
 *
 * RET  : Return the value of register WarpedDelayLine
 *
 * Notes : From register 0x0D54, bits 4:0
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
  
    value =  old_value & ~DIG_ANC_STEP_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(WARPED_DELAY_LINE_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_warped_shift_in
 *
 * RET  : Return the value of register WarpedDelayLine
 *
 * Notes : From register 0x0D54, bits 4:0
 *
 **************************************************************************/
unsigned char get_warped_shift_in()
  {
    unsigned char value;


    /* 
     * Digital gain for Warped Delay Line input. 
     */ 
    value = (I2CRead(WARPED_DELAY_LINE_REG) & DIG_ANC_STEP_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir_shift_out
 *
 * IN   : param, a value to write to the regiter GainForFirOutput
 * OUT  : 
 *
 * RET  : Return the value of register GainForFirOutput
 *
 * Notes : From register 0x0D55, bits 4:0
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
  
    value =  old_value & ~DIG_ANC_STEP_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(GAIN_FOR_FIR_OUTPUT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir_shift_out
 *
 * RET  : Return the value of register GainForFirOutput
 *
 * Notes : From register 0x0D55, bits 4:0
 *
 **************************************************************************/
unsigned char get_fir_shift_out()
  {
    unsigned char value;


    /* 
     * Digital gain for FIR output filter. 
     */ 
    value = (I2CRead(GAIN_FOR_FIR_OUTPUT_REG) & DIG_ANC_STEP_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_iir_shift_out
 *
 * IN   : param, a value to write to the regiter GainForIrrOutput
 * OUT  : 
 *
 * RET  : Return the value of register GainForIrrOutput
 *
 * Notes : From register 0x0D56, bits 4:0
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
  
    value =  old_value & ~DIG_ANC_STEP_GAIN_PARAM_MASK;


     value |=  param ;  
    I2CWrite(GAIN_FOR_IRR_OUTPUT_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_iir_shift_out
 *
 * RET  : Return the value of register GainForIrrOutput
 *
 * Notes : From register 0x0D56, bits 4:0
 *
 **************************************************************************/
unsigned char get_iir_shift_out()
  {
    unsigned char value;


    /* 
     * ANC digital gain for IIR output. 
     */ 
    value = (I2CRead(GAIN_FOR_IRR_OUTPUT_REG) & DIG_ANC_STEP_GAIN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_fir_coeff_msb
 *
 * IN   : param, a value to write to the regiter AncFirCoeffMsb
 * OUT  : 
 *
 * RET  : Return the value of register AncFirCoeffMsb
 *
 * Notes : From register 0x0D57, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_fir_coeff_msb( unsigned char param )
  {

    /* 
     * Sets the MSB of the ANC FIR coefficients, the 16 bits 
     * coefficient is updated when the LSB part is written. See 
     * ANCFIRCoeffLSB[7:0] for details. 
     */ 
    I2CWrite(ANC_FIR_COEFF_MSB_REG, param);
    return 0;

  }


/***************************************************************************
 *
 * Function : get_anc_fir_coeff_msb
 *
 * RET  : Return the value of register AncFirCoeffMsb
 *
 * Notes : From register 0x0D57, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_fir_coeff_msb()
  {
    unsigned char value;


    /* 
     * Sets the MSB of the ANC FIR coefficients, the 16 bits 
     * coefficient is updated when the LSB part is written. See 
     * ANCFIRCoeffLSB[7:0] for details. 
     */ 
   value = I2CRead(ANC_FIR_COEFF_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_fir_coeff_lsb
 *
 * IN   : param, a value to write to the regiter AncFirCoeffLsb
 * OUT  : 
 *
 * RET  : Return the value of register AncFirCoeffLsb
 *
 * Notes : From register 0x0D58, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_fir_coeff_lsb( unsigned char param )
  {

    /* 
     * Sets the LSB of the ANC FIR coefficients and it updates 
     * the 16 bits coefficient pointer. The FIR coefficients are 
     * 15 and they are written sequentially from 0 to 14. The 
     * coefficient bank must be written setting first 
     * 'ANCFIRUpDate'=1 and it is applied when the last 
     * coefficient is written. When ANCFIRUpdate is set to zero 
     * the coefficient pointer is reset. 
     */ 
    I2CWrite(ANC_FIR_COEFF_LSB_REG, param);
    return 0;

  }


/***************************************************************************
 *
 * Function : get_anc_fir_coeff_lsb
 *
 * RET  : Return the value of register AncFirCoeffLsb
 *
 * Notes : From register 0x0D58, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_fir_coeff_lsb()
  {
    unsigned char value;


    /* 
     * Sets the LSB of the ANC FIR coefficients and it updates 
     * the 16 bits coefficient pointer. The FIR coefficients are 
     * 15 and they are written sequentially from 0 to 14. The 
     * coefficient bank must be written setting first 
     * 'ANCFIRUpDate'=1 and it is applied when the last 
     * coefficient is written. When ANCFIRUpdate is set to zero 
     * the coefficient pointer is reset. 
     */ 
   value = I2CRead(ANC_FIR_COEFF_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_iir_coeff_msb
 *
 * IN   : param, a value to write to the regiter AncIirCoeffMsb
 * OUT  : 
 *
 * RET  : Return the value of register AncIirCoeffMsb
 *
 * Notes : From register 0x0D59, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_iir_coeff_msb( unsigned char param )
  {

    /* 
     * ANC IIR filter coefficient MSB part. MSB part is applied 
     * just when the LSB part is written. 
     */ 
    I2CWrite(ANC_IIR_COEFF_MSB_REG, param);
    return 0;

  }


/***************************************************************************
 *
 * Function : get_anc_iir_coeff_msb
 *
 * RET  : Return the value of register AncIirCoeffMsb
 *
 * Notes : From register 0x0D59, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_iir_coeff_msb()
  {
    unsigned char value;


    /* 
     * ANC IIR filter coefficient MSB part. MSB part is applied 
     * just when the LSB part is written. 
     */ 
   value = I2CRead(ANC_IIR_COEFF_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_iir_coeff_lsb
 *
 * IN   : param, a value to write to the regiter AncIirCoeffLsb
 * OUT  : 
 *
 * RET  : Return the value of register AncIirCoeffLsb
 *
 * Notes : From register 0x0D5A, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_iir_coeff_lsb( unsigned char param )
  {

    /* 
     * Sets the LSB of the ANC FIR coefficients, it updates the 
     * 16 bits coefficient and it increases the coefficient 
     * pointer. The FIR coefficients are 44 and they are written 
     * sequentially from 0 to 43. The coefficient bank must be 
     * written setting first and it is applied when the last 
     * coefficient is written. When ANCIIRUpDate is set to zero 
     * the coefficient pointer is reset. A special procedure is 
     * used for the first IIR coefficients writing: 'ANCIIRInit' 
     * is set to '1' and then to '0', then the 44 ANCIIRCoeff 
     * are written keeping 'ANCIIRUpDate'=0. 
     */ 
    I2CWrite(ANC_IIR_COEFF_LSB_REG, param);
    return 0;

  }


/***************************************************************************
 *
 * Function : get_anc_iir_coeff_lsb
 *
 * RET  : Return the value of register AncIirCoeffLsb
 *
 * Notes : From register 0x0D5A, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_iir_coeff_lsb()
  {
    unsigned char value;


    /* 
     * Sets the LSB of the ANC FIR coefficients, it updates the 
     * 16 bits coefficient and it increases the coefficient 
     * pointer. The FIR coefficients are 44 and they are written 
     * sequentially from 0 to 43. The coefficient bank must be 
     * written setting first and it is applied when the last 
     * coefficient is written. When ANCIIRUpDate is set to zero 
     * the coefficient pointer is reset. A special procedure is 
     * used for the first IIR coefficients writing: 'ANCIIRInit' 
     * is set to '1' and then to '0', then the 44 ANCIIRCoeff 
     * are written keeping 'ANCIIRUpDate'=0. 
     */ 
   value = I2CRead(ANC_IIR_COEFF_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_warp_delay_msb
 *
 * IN   : param, a value to write to the regiter AncWarpDelayMsb
 * OUT  : 
 *
 * RET  : Return the value of register AncWarpDelayMsb
 *
 * Notes : From register 0x0D5B, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_warp_delay_msb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(ANC_WARP_DELAY_MSB_REG);

    /* 
     * Sets the MSB of the ANC warped delay line, the 16 bits 
     * coefficient is updated when the LSB part is written. 
     */ 
    I2CWrite(ANC_WARP_DELAY_MSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_anc_warp_delay_msb
 *
 * RET  : Return the value of register AncWarpDelayMsb
 *
 * Notes : From register 0x0D5B, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_warp_delay_msb()
  {
    unsigned char value;


    /* 
     * Sets the MSB of the ANC warped delay line, the 16 bits 
     * coefficient is updated when the LSB part is written. 
     */ 
   value = I2CRead(ANC_WARP_DELAY_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_warp_delay_lsb
 *
 * IN   : param, a value to write to the regiter AncWarpDelayLsb
 * OUT  : 
 *
 * RET  : Return the value of register AncWarpDelayLsb
 *
 * Notes : From register 0x0D5C, bits 7:0
 *
 **************************************************************************/
unsigned char set_anc_warp_delay_lsb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(ANC_WARP_DELAY_LSB_REG);

    /* 
     * Sets the LSB of the ANC warped delay line coefficients, 
     * it updates the 16 bits coefficient. 
     */ 
    I2CWrite(ANC_WARP_DELAY_LSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_anc_warp_delay_lsb
 *
 * RET  : Return the value of register AncWarpDelayLsb
 *
 * Notes : From register 0x0D5C, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_warp_delay_lsb()
  {
    unsigned char value;


    /* 
     * Sets the LSB of the ANC warped delay line coefficients, 
     * it updates the 16 bits coefficient. 
     */ 
   value = I2CRead(ANC_WARP_DELAY_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_anc_fir_peak_msb
 *
 * RET  : Return the value of register AncFirPeakMsb
 *
 * Notes : From register 0x0D5D, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_fir_peak_msb()
  {
    unsigned char value;


    /* 
     * MSB of ANC FIR peak output. 
     */ 
   value = I2CRead(ANC_FIR_PEAK_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_anc_fir_peak_lsb
 *
 * RET  : Return the value of register AncFirPeakLsb
 *
 * Notes : From register 0x0D5E, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_fir_peak_lsb()
  {
    unsigned char value;


    /* 
     * LSB of ANC IIR peak output. 
     */ 
   value = I2CRead(ANC_FIR_PEAK_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_anc_iir_peak_msb
 *
 * RET  : Return the value of register AncIirPeakMsb
 *
 * Notes : From register 0x0D5F, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_iir_peak_msb()
  {
    unsigned char value;


    /* 
     * MSB of ANC IIR peak output. 
     */ 
   value = I2CRead(ANC_IIR_PEAK_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_anc_iir_peak_lsb
 *
 * RET  : Return the value of register AncIirPeakLsb
 *
 * Notes : From register 0x0D60, bits 7:0
 *
 **************************************************************************/
unsigned char get_anc_iir_peak_lsb()
  {
    unsigned char value;


    /* 
     * LSB of ANC IIR peak output. 
     */ 
   value = I2CRead(ANC_IIR_PEAK_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_anc_fir_coeff
 *
 * IN   : param, a value to write to the regiter AncFirCoeff
 * OUT  : 
 *
 * RET  : Return the value of register AncFirCoeff
 *
 * Notes : NIL
 *
 **************************************************************************/
void set_anc_fir_coeff(unsigned short value)
{
  set_anc_fir_coeff_msb(value >> 8);
  set_anc_fir_coeff_lsb(value &0xFF);
   
}

/***************************************************************************
 *
 * Function : get_anc_fir_coeff
 *
 * RET  : Return the value of register AncFirCoeff
 *
 * Notes : NIL
 *
 **************************************************************************/
unsigned short get_anc_fir_coeff()
{
  unsigned char p0, p1;
  p0 = get_anc_fir_coeff_lsb();
  p1 = get_anc_fir_coeff_msb();
  
  return p0 + (p1 << 8);
}

/***************************************************************************
 *
 * Function : set_anc_iir_coeff
 *
 * IN   : param, a value to write to the regiter AncIirCoeff
 * OUT  : 
 *
 * RET  : Return the value of register AncIirCoeff
 *
 * Notes : NIL
 *
 **************************************************************************/
void set_anc_iir_coeff(unsigned short value)
{
  set_anc_iir_coeff_msb(value >> 8);
  set_anc_iir_coeff_lsb(value &0xFF);
   
}

/***************************************************************************
 *
 * Function : get_anc_iir_coeff
 *
 * RET  : Return the value of register AncIirCoeff
 *
 * Notes : NIL
 *
 **************************************************************************/
unsigned short get_anc_iir_coeff()
{
  unsigned char p0, p1;
  p0 = get_anc_iir_coeff_lsb();
  p1 = get_anc_iir_coeff_msb();
  
  return p0 + (p1 << 8);
}

/***************************************************************************
 *
 * Function : set_anc_warp_delay
 *
 * IN   : param, a value to write to the regiter AncWarpDelay
 * OUT  : 
 *
 * RET  : Return the value of register AncWarpDelay
 *
 * Notes : NIL
 *
 **************************************************************************/
void set_anc_warp_delay(unsigned short value)
{
  set_anc_warp_delay_msb(value >> 8);
  set_anc_warp_delay_lsb(value &0xFF);
   
}

/***************************************************************************
 *
 * Function : get_anc_warp_delay
 *
 * RET  : Return the value of register AncWarpDelay
 *
 * Notes : NIL
 *
 **************************************************************************/
unsigned short get_anc_warp_delay()
{
  unsigned char p0, p1;
  p0 = get_anc_warp_delay_lsb();
  p1 = get_anc_warp_delay_msb();
  
  return p0 + (p1 << 8);
}

/***************************************************************************
 *
 * Function : get_anc_iir_peak
 *
 * RET  : Return the value of register AncIirPeak
 *
 * Notes : NIL
 *
 **************************************************************************/
unsigned short get_anc_iir_peak()
{
  unsigned char p0, p1;
  p0 = get_anc_iir_peak_lsb();
  p1 = get_anc_iir_peak_msb();
  
  return p0 + (p1 << 8);
}

/***************************************************************************
 *
 * Function : get_anc_fir_peak
 *
 * RET  : Return the value of register AncFirPeak
 *
 * Notes : NIL
 *
 **************************************************************************/
unsigned short get_anc_fir_peak()
{
  unsigned char p0, p1;
  p0 = get_anc_fir_peak_lsb();
  p1 = get_anc_fir_peak_msb();
  
  return p0 + (p1 << 8);
}

/***************************************************************************
 *
 * Function : get_side_tone_control
 *
 * RET  : Return the value of register SideToneControl
 *
 * Notes : From register 0x0D61, bits 7:7
 *
 **************************************************************************/
unsigned char get_side_tone_control()
  {
    unsigned char value;


    /* 
     * 0: Last FIR coefficients not applied; 
     * 1: Applies the last programmed set of FIR coefficients. 
     */ 
    value = (I2CRead(SIDE_TONE_CONTROL_REG) & SIDE_TONE_CONTROL_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_address_coefficient
 *
 * RET  : Return the value of register SideToneControl
 *
 * Notes : From register 0x0D61, bits 6:0
 *
 **************************************************************************/
unsigned char get_address_coefficient()
  {
    unsigned char value;


    /* 
     * Set the 7 bits address of FIR coefficient that will be 
     * written 
     */ 
    value = (I2CRead(SIDE_TONE_CONTROL_REG) & ADDRESS_COEFFOCIENT_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_side_tone_coff_msb
 *
 * IN   : param, a value to write to the regiter SideToneCoffMsb
 * OUT  : 
 *
 * RET  : Return the value of register SideToneCoffMsb
 *
 * Notes : From register 0x0D62, bits 7:0
 *
 **************************************************************************/
unsigned char set_side_tone_coff_msb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(SIDE_TONE_COFF_MSB_REG);

    /* 
     * ST FIR memory for filter coefficient, MSB part. The whole 
     * coefficient is applied when the LSB part is written. The 
     * FIR coefficients are applied to filter just when 
     * STFIR_SET bit is set 
     */ 
    I2CWrite(SIDE_TONE_COFF_MSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_side_tone_coff_msb
 *
 * RET  : Return the value of register SideToneCoffMsb
 *
 * Notes : From register 0x0D62, bits 7:0
 *
 **************************************************************************/
unsigned char get_side_tone_coff_msb()
  {
    unsigned char value;


    /* 
     * ST FIR memory for filter coefficient, MSB part. The whole 
     * coefficient is applied when the LSB part is written. The 
     * FIR coefficients are applied to filter just when 
     * STFIR_SET bit is set 
     */ 
   value = I2CRead(SIDE_TONE_COFF_MSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_side_tone_coff_lsb
 *
 * IN   : param, a value to write to the regiter SideToneCoffLsb
 * OUT  : 
 *
 * RET  : Return the value of register SideToneCoffLsb
 *
 * Notes : From register 0x0D63, bits 7:0
 *
 **************************************************************************/
unsigned char set_side_tone_coff_lsb( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(SIDE_TONE_COFF_LSB_REG);

    /* 
     * ST FIR memory for filter coefficient, LSB part. The whole 
     * coefficient is applied when the LSB part is written. The 
     * FIR coefficients are applied to filter just when 
     * STFIR_SET bit is set 
     */ 
    I2CWrite(SIDE_TONE_COFF_LSB_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_side_tone_coff_lsb
 *
 * RET  : Return the value of register SideToneCoffLsb
 *
 * Notes : From register 0x0D63, bits 7:0
 *
 **************************************************************************/
unsigned char get_side_tone_coff_lsb()
  {
    unsigned char value;


    /* 
     * ST FIR memory for filter coefficient, LSB part. The whole 
     * coefficient is applied when the LSB part is written. The 
     * FIR coefficients are applied to filter just when 
     * STFIR_SET bit is set 
     */ 
   value = I2CRead(SIDE_TONE_COFF_LSB_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir_filters_enable
 *
 * IN   : param, a value to write to the regiter FirFiltersControl
 * OUT  : 
 *
 * RET  : Return the value of register FirFiltersControl
 *
 * Notes : From register 0x0D64, bits 2:2
 *
 **************************************************************************/
unsigned char set_fir_filters_enable( enum fir_filters_enable param )
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
           value = old_value | FIR_FILTERS_ENABLE_PARAM_MASK; 
           break;
        case FIR_FILTERS_DISABLE_E: 
           value = old_value & ~ FIR_FILTERS_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(FIR_FILTERS_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir_filters_enable
 *
 * RET  : Return the value of register FirFiltersControl
 *
 * Notes : From register 0x0D64, bits 2:2
 *
 **************************************************************************/
unsigned char get_fir_filters_enable()
  {
    unsigned char value;


    /* 
     * 0: ST FIR Filters are disabled. 
     * 1: ST FIR Filters are enabled. 
     */ 
    value = (I2CRead(FIR_FILTERS_CONTROL_REG) & FIR_FILTERS_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir_data_rate
 *
 * IN   : param, a value to write to the regiter FirFiltersControl
 * OUT  : 
 *
 * RET  : Return the value of register FirFiltersControl
 *
 * Notes : From register 0x0D64, bits 1:1
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
           value = old_value | FIR_DATA_RATE_PARAM_MASK; 
           break;
        case AUD_IF0_DATA_RATE_E: 
           value = old_value & ~ FIR_DATA_RATE_PARAM_MASK;
           break;
    }
  

    I2CWrite(FIR_FILTERS_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir_data_rate
 *
 * RET  : Return the value of register FirFiltersControl
 *
 * Notes : From register 0x0D64, bits 1:1
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
    value = (I2CRead(FIR_FILTERS_CONTROL_REG) & FIR_DATA_RATE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fir_busy
 *
 * IN   : param, a value to write to the regiter FirFiltersControl
 * OUT  : 
 *
 * RET  : Return the value of register FirFiltersControl
 *
 * Notes : From register 0x0D64, bits 0:0
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
           value = old_value | FIR_BUSY_PARAM_MASK; 
           break;
        case FIR_READY_E: 
           value = old_value & ~ FIR_BUSY_PARAM_MASK;
           break;
    }
  

    I2CWrite(FIR_FILTERS_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fir_busy
 *
 * RET  : Return the value of register FirFiltersControl
 *
 * Notes : From register 0x0D64, bits 0:0
 *
 **************************************************************************/
unsigned char get_fir_busy()
  {
    unsigned char value;


    /* 
     * 0: The ST FIR is ready for new coefficients. 
     * 1: The new ST FIR coefficients are not applied yet. 
     */ 
    value = (I2CRead(FIR_FILTERS_CONTROL_REG) & FIR_BUSY_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_hs_offset_status_enable
 *
 * IN   : param, a value to write to the regiter InterruptEnable0
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 7:7
 *
 **************************************************************************/
unsigned char set_interrupt_hs_offset_status_enable( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_0_REG);

    /* 
     * 0: Bit IT_HsOffSt cannot generate an Interrupt Request. 
     * 1:Bit IT_HsOffSt can generate an Interrupt Request 
     */ 
    value =  old_value & ~HS_OFFSET_STATUS_PARAM_MASK;


     param |= (value << 0x7);  
    I2CWrite(INTERRUPT_ENABLE_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_hs_offset_status_enable
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 7:7
 *
 **************************************************************************/
unsigned char get_interrupt_hs_offset_status_enable()
  {
    unsigned char value;


    /* 
     * 0: Bit IT_HsOffSt cannot generate an Interrupt Request. 
     * 1:Bit IT_HsOffSt can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_0_REG) & HS_OFFSET_STATUS_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_buffer_full_enable
 *
 * IN   : param, a value to write to the regiter InterruptEnable0
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 6:6
 *
 **************************************************************************/
unsigned char set_interrupt_buffer_full_enable( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_0_REG);

    /* 
     * 0: Bit BFIFULL_EV cannot generate an Interrupt Request; 
     * 1: Bit BFIFULL_EV can generate an Interrupt Request 
     */ 
    value =  old_value & ~BUFFER_FULL_PARAM_MASK;


     param |= (value << 0x6);  
    I2CWrite(INTERRUPT_ENABLE_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_buffer_full_enable
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 6:6
 *
 **************************************************************************/
unsigned char get_interrupt_buffer_full_enable()
  {
    unsigned char value;


    /* 
     * 0: Bit BFIFULL_EV cannot generate an Interrupt Request; 
     * 1: Bit BFIFULL_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_0_REG) & BUFFER_FULL_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_buffer_empty_enable
 *
 * IN   : param, a value to write to the regiter InterruptEnable0
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 5:5
 *
 **************************************************************************/
unsigned char set_interrupt_buffer_empty_enable( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_0_REG);

    /* 
     * 0: Bit BFIEMPT_EV cannot generate an Interrupt Request; 
     * 1: Bit BFIEMPT_EV can generate an Interrupt Request 
     */ 
    value =  old_value & ~BUFFER_EMPTY_PARAM_MASK;


     param |= (value << 0x5);  
    I2CWrite(INTERRUPT_ENABLE_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_buffer_empty_enable
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 5:5
 *
 **************************************************************************/
unsigned char get_interrupt_buffer_empty_enable()
  {
    unsigned char value;


    /* 
     * 0: Bit BFIEMPT_EV cannot generate an Interrupt Request; 
     * 1: Bit BFIEMPT_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_0_REG) & BUFFER_EMPTY_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_da_chan_enable
 *
 * IN   : param, a value to write to the regiter InterruptEnable0
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 4:4
 *
 **************************************************************************/
unsigned char set_interrupt_da_chan_enable( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_0_REG);

    /* 
     * 0: Bit DACHAN_EV cannot generate an Interrupt Request; 
     * 1: Bit DACHAN_EV can generate an Interrupt Request 
     */ 
    value =  old_value & ~DA_CHAN_PARAM_MASK;


     param |= (value << 0x4);  
    I2CWrite(INTERRUPT_ENABLE_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_da_chan_enable
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 4:4
 *
 **************************************************************************/
unsigned char get_interrupt_da_chan_enable()
  {
    unsigned char value;


    /* 
     * 0: Bit DACHAN_EV cannot generate an Interrupt Request; 
     * 1: Bit DACHAN_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_0_REG) & DA_CHAN_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_gain_enable
 *
 * IN   : param, a value to write to the regiter InterruptEnable0
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 3:3
 *
 **************************************************************************/
unsigned char set_interrupt_gain_enable( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_0_REG);

    /* 
     * 0: Bit GAIN_EV cannot generate an Interrupt Request; 
     * 1: Bit GAIN_EV can generate an Interrupt Request 
     */ 
    value =  old_value & ~GAIN_PARAM_MASK;


     param |= (value << 0x3);  
    I2CWrite(INTERRUPT_ENABLE_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_gain_enable
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 3:3
 *
 **************************************************************************/
unsigned char get_interrupt_gain_enable()
  {
    unsigned char value;


    /* 
     * 0: Bit GAIN_EV cannot generate an Interrupt Request; 
     * 1: Bit GAIN_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_0_REG) & GAIN_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_dsp_af_enable
 *
 * IN   : param, a value to write to the regiter InterruptEnable0
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 2:2
 *
 **************************************************************************/
unsigned char set_interrupt_dsp_af_enable( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_0_REG);

    /* 
     * 0: Bit DSPAD_EV cannot generate an Interrupt Request; 
     * 1: Bit DSPAD_EV can generate an Interrupt Request 
     */ 
    value =  old_value & ~DSP_AD_ENABLE_PARAM_MASK;


     param |= (value << 0x2);  
    I2CWrite(INTERRUPT_ENABLE_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_dsp_af_enable
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 2:2
 *
 **************************************************************************/
unsigned char get_interrupt_dsp_af_enable()
  {
    unsigned char value;


    /* 
     * 0: Bit DSPAD_EV cannot generate an Interrupt Request; 
     * 1: Bit DSPAD_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_0_REG) & DSP_AD_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_dsp_dz_enable
 *
 * IN   : param, a value to write to the regiter InterruptEnable0
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 1:1
 *
 **************************************************************************/
unsigned char set_interrupt_dsp_dz_enable( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_ENABLE_0_REG);

    /* 
     * 0: Bit DSPDA_EV cannot generate an Interrupt Request; 
     * 1: Bit DSPDA_EV can generate an Interrupt Request 
     */ 
    value =  old_value & ~DSP_DA_ENABLE_PARAM_MASK;


     param |= (value << 0x1);  
    I2CWrite(INTERRUPT_ENABLE_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_dsp_dz_enable
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 1:1
 *
 **************************************************************************/
unsigned char get_interrupt_dsp_dz_enable()
  {
    unsigned char value;


    /* 
     * 0: Bit DSPDA_EV cannot generate an Interrupt Request; 
     * 1: Bit DSPDA_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_0_REG) & DSP_DA_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_fir_enable
 *
 * IN   : param, a value to write to the regiter InterruptEnable0
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 0:0
 *
 **************************************************************************/
unsigned char set_interrupt_fir_enable( enum fir_enable param )
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
           value = old_value | FIR_ENABLE_PARAM_MASK; 
           break;
        case FIR_DISABLE_E: 
           value = old_value & ~ FIR_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(INTERRUPT_ENABLE_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_fir_enable
 *
 * RET  : Return the value of register InterruptEnable0
 *
 * Notes : From register 0x0D65, bits 0:0
 *
 **************************************************************************/
unsigned char get_interrupt_fir_enable()
  {
    unsigned char value;


    /* 
     * 0: Bit FIR_EV cannot generate an Interrupt Request; 
     * 1: Bit FIR_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_0_REG) & FIR_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_status0
 *
 * RET  : Return the value of register InterruptStatus0
 *
 * Notes : From register 0x0D66, bits 7:0
 *
 **************************************************************************/
unsigned char get_interrupt_status0()
  {
    unsigned char value;


    /* 
     * InterruptStatus 
     */ 
   value = I2CRead(INTERRUPT_STATUS_0_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_hs_offset_status
 *
 * IN   : param, a value to write to the regiter InterruptStatus0
 * OUT  : 
 *
 * RET  : Return the value of register InterruptStatus0
 *
 * Notes : From register 0x0D66, bits 7:7
 *
 **************************************************************************/
unsigned char set_interrupt_hs_offset_status( unsigned char param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(INTERRUPT_STATUS_0_REG);

    /* 
     * 0: Bit IT_HsOffSt cannot generate an Interrupt Request. 
     * 1:Bit IT_HsOffSt can generate an Interrupt Request 
     */ 
    value =  old_value & ~HS_OFFSET_STATUS_PARAM_MASK;


     param |= (value << 0x7);  
    I2CWrite(INTERRUPT_STATUS_0_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_hs_offset_status
 *
 * RET  : Return the value of register InterruptStatus0
 *
 * Notes : From register 0x0D66, bits 7:7
 *
 **************************************************************************/
unsigned char get_interrupt_hs_offset_status()
  {
    unsigned char value;


    /* 
     * 0: Bit IT_HsOffSt cannot generate an Interrupt Request. 
     * 1:Bit IT_HsOffSt can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_0_REG) & HS_OFFSET_STATUS_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_status_buffer_full
 *
 * RET  : Return the value of register InterruptStatus0
 *
 * Notes : From register 0x0D66, bits 6:6
 *
 **************************************************************************/
unsigned char get_status_buffer_full()
  {
    unsigned char value;


    /* 
     * 0: Burst FIFO is not full. 
     * 1: Burst FIFO full event has occuredt 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_0_REG) & STATUS_BUFFER_FULL_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : get_status_buffer_empty
 *
 * RET  : Return the value of register InterruptStatus0
 *
 * Notes : From register 0x0D66, bits 5:5
 *
 **************************************************************************/
unsigned char get_status_buffer_empty()
  {
    unsigned char value;


    /* 
     * 0: Burst FIFO is not empty.1: Burst FIFO event has 
     * occured. 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_0_REG) & STATUS_BUFFER_EMPTY_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : get_status_da_chan
 *
 * RET  : Return the value of register InterruptStatus0
 *
 * Notes : From register 0x0D66, bits 4:4
 *
 **************************************************************************/
unsigned char get_status_da_chan()
  {
    unsigned char value;


    /* 
     * 0: No saturation occurred; 
     * 1:A saturation has occurred in DA gain 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_0_REG) & STATUS_DA_CHAN_PARAM_MASK) >> 4;
    return value;
  }


/***************************************************************************
 *
 * Function : get_status_gain
 *
 * RET  : Return the value of register InterruptStatus0
 *
 * Notes : From register 0x0D66, bits 3:3
 *
 **************************************************************************/
unsigned char get_status_gain()
  {
    unsigned char value;


    /* 
     * 0: No saturation occurred; 
     * 1:A saturation has occurred in AD gain 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_0_REG) & STATUS_GAIN_PARAM_MASK) >> 3;
    return value;
  }


/***************************************************************************
 *
 * Function : get_status_dsp_ad
 *
 * RET  : Return the value of register InterruptStatus0
 *
 * Notes : From register 0x0D66, bits 2:2
 *
 **************************************************************************/
unsigned char get_status_dsp_ad()
  {
    unsigned char value;


    /* 
     * 0: No saturation occurred; 
     * 1:A saturation has occurred in AD DSP 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_0_REG) & STATUS_DSP_AD_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_status_dsp_da
 *
 * RET  : Return the value of register InterruptStatus0
 *
 * Notes : From register 0x0D66, bits 1:1
 *
 **************************************************************************/
unsigned char get_status_dsp_da()
  {
    unsigned char value;


    /* 
     * 0: Bit DSPDA_EV cannot generate an Interrupt Request; 
     * 1: Bit DSPDA_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_0_REG) & STATUS_DSP_DA_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_status_fir
 *
 * RET  : Return the value of register InterruptStatus0
 *
 * Notes : From register 0x0D66, bits 0:0
 *
 **************************************************************************/
unsigned char get_status_fir()
  {
    unsigned char value;


    /* 
     * 0: Bit FIR_EV cannot generate an Interrupt Request; 
     * 1: Bit FIR_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_0_REG) & STATUS_FIR_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_vss_ready
 *
 * IN   : param, a value to write to the regiter InterruptEnable1
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable1
 *
 * Notes : From register 0x0D67, bits 7:7
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
           value = old_value | INTERRUPT_VSS_READY_ENABLE_PARAM_MASK; 
           break;
        case INTERRUPT_VSS_READY_DISABLE_E: 
           value = old_value & ~ INTERRUPT_VSS_READY_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(INTERRUPT_ENABLE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_vss_ready
 *
 * RET  : Return the value of register InterruptEnable1
 *
 * Notes : From register 0x0D67, bits 7:7
 *
 **************************************************************************/
unsigned char get_interrupt_vss_ready()
  {
    unsigned char value;


    /* 
     * 0: Bit BFIFULL_EV cannot generate an Interrupt Request; 
     * 1: Bit BFIFULL_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_1_REG) & INTERRUPT_VSS_READY_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_short_circuit_detected_hsl
 *
 * IN   : param, a value to write to the regiter InterruptEnable1
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable1
 *
 * Notes : From register 0x0D67, bits 2:2
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
           value = old_value | INTERRUPT_SHORT_CIRCUIT_HSL_ENABLE_PARAM_MASK; 
           break;
        case INTERRUPT_SHORT_CIRCUIT_HSL_DISABLE_E: 
           value = old_value & ~ INTERRUPT_SHORT_CIRCUIT_HSL_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(INTERRUPT_ENABLE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_short_circuit_detected_hsl
 *
 * RET  : Return the value of register InterruptEnable1
 *
 * Notes : From register 0x0D67, bits 2:2
 *
 **************************************************************************/
unsigned char get_interrupt_short_circuit_detected_hsl()
  {
    unsigned char value;


    /* 
     * 0: Bit SHRTHSL_EV cannot generate an Interrupt Request; 
     * 1: Bit SHRTHSL_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_1_REG) & INTERRUPT_SHORT_CIRCUIT_HSL_ENABLE_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_short_circuit_detected_hsr
 *
 * IN   : param, a value to write to the regiter InterruptEnable1
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable1
 *
 * Notes : From register 0x0D67, bits 1:1
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
           value = old_value | INTERRUPT_SHORT_CIRCUIT_HSR_ENABLE_PARAM_MASK; 
           break;
        case INTERRUPT_SHORT_CIRCUIT_HSR_DISABLE_E: 
           value = old_value & ~ INTERRUPT_SHORT_CIRCUIT_HSR_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(INTERRUPT_ENABLE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_short_circuit_detected_hsr
 *
 * RET  : Return the value of register InterruptEnable1
 *
 * Notes : From register 0x0D67, bits 1:1
 *
 **************************************************************************/
unsigned char get_interrupt_short_circuit_detected_hsr()
  {
    unsigned char value;


    /* 
     * 0: Bit SHRTHSR_EV cannot generate an Interrupt Request; 
     * 1: Bit SHRTHSR_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_1_REG) & INTERRUPT_SHORT_CIRCUIT_HSR_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_interrupt_short_circuit_detected_ear
 *
 * IN   : param, a value to write to the regiter InterruptEnable1
 * OUT  : 
 *
 * RET  : Return the value of register InterruptEnable1
 *
 * Notes : From register 0x0D67, bits 0:0
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
        case INTERRUPT_SHORT_CIRCUIT_DETECTED_E: 
           value = old_value | INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_ENABLE_PARAM_MASK; 
           break;
        case INTERRUPT_SHORT_CIRCUIT_UN_DETECTED_E: 
           value = old_value & ~ INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(INTERRUPT_ENABLE_1_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_interrupt_short_circuit_detected_ear
 *
 * RET  : Return the value of register InterruptEnable1
 *
 * Notes : From register 0x0D67, bits 0:0
 *
 **************************************************************************/
unsigned char get_interrupt_short_circuit_detected_ear()
  {
    unsigned char value;


    /* 
     * 0: Bit SHRTEAR_EV cannot generate an Interrupt Request; 
     * 1: Bit SHRTEAR_EV can generate an Interrupt Request 
     */ 
    value = (I2CRead(INTERRUPT_ENABLE_1_REG) & INTERRUPT_SHORT_CIRCUIT_DETECTED_EAR_ENABLE_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : get_status_vss_ready
 *
 * RET  : Return the value of register InterruptStatus1
 *
 * Notes : From register 0x0D68, bits 7:7
 *
 **************************************************************************/
unsigned char get_status_vss_ready()
  {
    unsigned char value;


    /* 
     * 0: VSS from charge pump is not ready. 
     * 1: VSS from charge pump is ready. 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_1_REG) & STATUS_VSS_READY_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : get_status_short_circuit_detected_hsl
 *
 * RET  : Return the value of register InterruptStatus1
 *
 * Notes : From register 0x0D68, bits 2:2
 *
 **************************************************************************/
unsigned char get_status_short_circuit_detected_hsl()
  {
    unsigned char value;


    /* 
     * 0: No short circuit detected, 
     * 1: Short circuit detected on HSL Output driver. 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_1_REG) & STATUS_SHORT_CIRCUIT_DETECTED_HSL_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : get_status_short_circuit_detected_hsr
 *
 * RET  : Return the value of register InterruptStatus1
 *
 * Notes : From register 0x0D68, bits 1:1
 *
 **************************************************************************/
unsigned char get_status_short_circuit_detected_hsr()
  {
    unsigned char value;


    /* 
     * 0: No short circuit detected, 
     * 1: Short circuit detected on HSR Output driver. 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_1_REG) & STATUS_SHORT_CIRCUIT_DETECTED_HSR_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : get_status_short_circuit_detected_ear
 *
 * RET  : Return the value of register InterruptStatus1
 *
 * Notes : From register 0x0D68, bits 0:0
 *
 **************************************************************************/
unsigned char get_status_short_circuit_detected_ear()
  {
    unsigned char value;


    /* 
     * 0: No short circuit detected, 
     * 1: Short circuit detected on Ear Output driver. 
     */ 
    value = (I2CRead(INTERRUPT_STATUS_1_REG) & STATUS_SHORT_CIRCUIT_DETECTED_EAR_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_mask_enable
 *
 * IN   : param, a value to write to the regiter FifoInterruptControl
 * OUT  : 
 *
 * RET  : Return the value of register FifoInterruptControl
 *
 * Notes : From register 0x0D69, bits 7:7
 *
 **************************************************************************/
unsigned char set_fifo_mask_enable( enum fifo_mask_enable param )
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
           value = old_value | FIFO_MASK_ENABLE_PARAM_MASK; 
           break;
        case FIFO_MASK_DISABLE_E: 
           value = old_value & ~ FIFO_MASK_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(FIFO_INTERRUPT_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_fifo_mask_enable
 *
 * RET  : Return the value of register FifoInterruptControl
 *
 * Notes : From register 0x0D69, bits 7:7
 *
 **************************************************************************/
unsigned char get_fifo_mask_enable()
  {
    unsigned char value;


    /* 
     * 0: AD_DATA0 is unmasked in burst mode, it can wake up the 
     * digital audio data source 
     * 1: AD_DATA0 is masked in burst mode, it cannot wake up 
     * the digital audio data source 
     */ 
    value = (I2CRead(FIFO_INTERRUPT_CONTROL_REG) & FIFO_MASK_ENABLE_PARAM_MASK) >> 7;
    return value;
  }


/***************************************************************************
 *
 * Function : set_burst_bit_clock
 *
 * IN   : param, a value to write to the regiter FifoInterruptControl
 * OUT  : 
 *
 * RET  : Return the value of register FifoInterruptControl
 *
 * Notes : From register 0x0D69, bits 6:6
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
           value = old_value | BURST_FREQUENCY_PARAM_MASK; 
           break;
        case BUSRT_FREQUENCY_38_4_MHZ_E: 
           value = old_value & ~ BURST_FREQUENCY_PARAM_MASK;
           break;
    }
  

    I2CWrite(FIFO_INTERRUPT_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_burst_bit_clock
 *
 * RET  : Return the value of register FifoInterruptControl
 *
 * Notes : From register 0x0D69, bits 6:6
 *
 **************************************************************************/
unsigned char get_burst_bit_clock()
  {
    unsigned char value;


    /* 
     * 0: BitClk0 frequency in burst mode is 38.4 MHz 
     * 1: BitClk0 frequency in burst mode is 19.2 MHz 
     */ 
    value = (I2CRead(FIFO_INTERRUPT_CONTROL_REG) & BURST_FREQUENCY_PARAM_MASK) >> 6;
    return value;
  }


/***************************************************************************
 *
 * Function : set_wakeup_signal_level
 *
 * IN   : param, a value to write to the regiter FifoInterruptControl
 * OUT  : 
 *
 * RET  : Return the value of register FifoInterruptControl
 *
 * Notes : From register 0x0D69, bits 5:0
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
  

     value =  old_value & ~WAKEUP_SIGNAL_LEVEL_PARAM_MASK ;

    switch(  param ){

           case NO_WAKEUP_SIGNAL_E:
                value =  value | FIFO_EMPTY ;
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
 * RET  : Return the value of register FifoInterruptControl
 *
 * Notes : From register 0x0D69, bits 5:0
 *
 **************************************************************************/
unsigned char get_wakeup_signal_level()
  {
    unsigned char value;


    /* 
     * Wakeup signal is generated when burst 
     */ 
    value = (I2CRead(FIFO_INTERRUPT_CONTROL_REG) & WAKEUP_SIGNAL_LEVEL_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_burst_length
 *
 * IN   : param, a value to write to the regiter FifoLength
 * OUT  : 
 *
 * RET  : Return the value of register FifoLength
 *
 * Notes : From register 0x0D6A, bits 7:0
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
  

     value =  old_value & (unsigned char) ~FIFO_BURST_LENGTH_PARAM_MASK ;

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
 * RET  : Return the value of register FifoLength
 *
 * Notes : From register 0x0D6A, bits 7:0
 *
 **************************************************************************/
unsigned char get_fifo_burst_length()
  {
    unsigned char value;


    /* 
     * Burst FIFO length register. It define the burst tranfer 
     * length in master mode. 
     */ 
    value = (I2CRead(FIFO_LENGTH_REG) & FIFO_BURST_LENGTH_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_extra_clock
 *
 * IN   : param, a value to write to the regiter FifoControl
 * OUT  : 
 *
 * RET  : Return the value of register FifoControl
 *
 * Notes : From register 0x0D6B, bits 7:5
 *
 **************************************************************************/
unsigned char set_fifo_extra_clock( enum extra_clock param )
  {
    unsigned char value = 0;
    unsigned char old_value;

    old_value = I2CRead(FIFO_CONTROL_REG);

    /* 
     * Number of extra clock in burst mode at the end of each 
     * frame 
     */ 
  

     value =  old_value & ~EXTRA_CLOCK_PARAM_MASK ;

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
 * RET  : Return the value of register FifoControl
 *
 * Notes : From register 0x0D6B, bits 7:5
 *
 **************************************************************************/
unsigned char get_fifo_extra_clock()
  {
    unsigned char value;


    /* 
     * Number of extra clock in burst mode at the end of each 
     * frame 
     */ 
    value = (I2CRead(FIFO_CONTROL_REG) & EXTRA_CLOCK_PARAM_MASK) >> 5;
    return value;
  }


/***************************************************************************
 *
 * Function : set_number_extra_clocks
 *
 * IN   : param, a value to write to the regiter FifoControl
 * OUT  : 
 *
 * RET  : Return the value of register FifoControl
 *
 * Notes : From register 0x0D6B, bits 4:2
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
  
    value =  old_value & ~NUMBER_EXTRA_CLOCKS_PARAM_MASK;


    value |= ( param << 0x2);  

    I2CWrite(FIFO_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_number_extra_clocks
 *
 * RET  : Return the value of register FifoControl
 *
 * Notes : From register 0x0D6B, bits 4:2
 *
 **************************************************************************/
unsigned char get_number_extra_clocks()
  {
    unsigned char value;


    /* 
     * Number of extra bit clocks that precede and follow the 
     * first Frame Sync on Aud_IF0 startup or Burst startup 
     */ 
    value = (I2CRead(FIFO_CONTROL_REG) & NUMBER_EXTRA_CLOCKS_PARAM_MASK) >> 2;
    return value;
  }


/***************************************************************************
 *
 * Function : set_master_fifo_mode_enable
 *
 * IN   : param, a value to write to the regiter FifoControl
 * OUT  : 
 *
 * RET  : Return the value of register FifoControl
 *
 * Notes : From register 0x0D6B, bits 1:1
 *
 **************************************************************************/
unsigned char set_master_fifo_mode_enable( enum master_fifo_mode_enable param )
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
           value = old_value | MASTER_FIFO_MODE_ENABLE_PARAM_MASK; 
           break;
        case MASTER_FIFO_MODE_DISABLE_E: 
           value = old_value & ~ MASTER_FIFO_MODE_ENABLE_PARAM_MASK;
           break;
    }
  

    I2CWrite(FIFO_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_master_fifo_mode_enable
 *
 * RET  : Return the value of register FifoControl
 *
 * Notes : From register 0x0D6B, bits 1:1
 *
 **************************************************************************/
unsigned char get_master_fifo_mode_enable()
  {
    unsigned char value;


    /* 
     * 0: Burst FIFO interface is configured in slave mode 
     * 1: Burst FIFO interface is configured in master mode 
     */ 
    value = (I2CRead(FIFO_CONTROL_REG) & MASTER_FIFO_MODE_ENABLE_PARAM_MASK) >> 1;
    return value;
  }


/***************************************************************************
 *
 * Function : set_bust_mode_run
 *
 * IN   : param, a value to write to the regiter FifoControl
 * OUT  : 
 *
 * RET  : Return the value of register FifoControl
 *
 * Notes : From register 0x0D6B, bits 0:0
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
           value = old_value | BUST_MODE_RUN_PARAM_MASK; 
           break;
        case BURST_MODE_STOPPED_E: 
           value = old_value & ~ BUST_MODE_RUN_PARAM_MASK;
           break;
    }
  

    I2CWrite(FIFO_CONTROL_REG, value);

    return old_value;
  }


/***************************************************************************
 *
 * Function : get_bust_mode_run
 *
 * RET  : Return the value of register FifoControl
 *
 * Notes : From register 0x0D6B, bits 0:0
 *
 **************************************************************************/
unsigned char get_bust_mode_run()
  {
    unsigned char value;


    /* 
     * 0: Burst FIFO interface is stopped 
     * 1: Burst FIFO interface is running 
     */ 
    value = (I2CRead(FIFO_CONTROL_REG) & BUST_MODE_RUN_PARAM_MASK);
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_switch_frame
 *
 * IN   : param, a value to write to the regiter FifoSwitchFrame
 * OUT  : 
 *
 * RET  : Return the value of register FifoSwitchFrame
 *
 * Notes : From register 0x0D6C, bits 7:0
 *
 **************************************************************************/
unsigned char set_fifo_switch_frame( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(FIFO_SWITCH_FRAME_REG);

    /* 
     * It indicates on which frame number the switch from normal 
     * to burst mode will happen. The number of frame is 
     * BFIFOFramSw*8. The switch occurs when the register 
     * content multiplied by 8 matches for the first time an 11 
     * bits internal frame counter (FN, with wrap-around at 
     * 2047-0) that is reset each time the interface starts or 
     * is switched to normal mode. The frame where FN = 
     * BFIFOFramSw*8 will be the last one in normal mode, the 
     * frame where FN = BFIFOFramSw*8 + 1 will be the first one 
     * in burst mode. 
     */ 
    I2CWrite(FIFO_SWITCH_FRAME_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_fifo_switch_frame
 *
 * RET  : Return the value of register FifoSwitchFrame
 *
 * Notes : From register 0x0D6C, bits 7:0
 *
 **************************************************************************/
unsigned char get_fifo_switch_frame()
  {
    unsigned char value;


    /* 
     * It indicates on which frame number the switch from normal 
     * to burst mode will happen. The number of frame is 
     * BFIFOFramSw*8. The switch occurs when the register 
     * content multiplied by 8 matches for the first time an 11 
     * bits internal frame counter (FN, with wrap-around at 
     * 2047-0) that is reset each time the interface starts or 
     * is switched to normal mode. The frame where FN = 
     * BFIFOFramSw*8 will be the last one in normal mode, the 
     * frame where FN = BFIFOFramSw*8 + 1 will be the first one 
     * in burst mode. 
     */ 
   value = I2CRead(FIFO_SWITCH_FRAME_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_wake_up_delay
 *
 * IN   : param, a value to write to the regiter FifoWakeUpDelay
 * OUT  : 
 *
 * RET  : Return the value of register FifoWakeUpDelay
 *
 * Notes : From register 0x0D6D, bits 7:0
 *
 **************************************************************************/
unsigned char set_fifo_wake_up_delay( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(FIFO_WAKE_UP_DELAY_REG);

    /* 
     * It indicates the delay from wake up generation to the 
     * start of burst transmission. This delay do not happen on 
     * the first burst transmission. The delay is 
     * 26.7usec*BFIFOWAKEUP 
     */ 
    I2CWrite(FIFO_WAKE_UP_DELAY_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_fifo_wake_up_delay
 *
 * RET  : Return the value of register FifoWakeUpDelay
 *
 * Notes : From register 0x0D6D, bits 7:0
 *
 **************************************************************************/
unsigned char get_fifo_wake_up_delay()
  {
    unsigned char value;


    /* 
     * It indicates the delay from wake up generation to the 
     * start of burst transmission. This delay do not happen on 
     * the first burst transmission. The delay is 
     * 26.7usec*BFIFOWAKEUP 
     */ 
   value = I2CRead(FIFO_WAKE_UP_DELAY_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : set_fifo_samples_number
 *
 * IN   : param, a value to write to the regiter FifoSamplesNumber
 * OUT  : 
 *
 * RET  : Return the value of register FifoSamplesNumber
 *
 * Notes : From register 0x0D6E, bits 7:0
 *
 **************************************************************************/
unsigned char set_fifo_samples_number( unsigned char param )
  {
    unsigned char value;

    value = I2CRead(FIFO_SAMPLES_NUMBER_REG);

    /* 
     * Number of samples currently present in burst FIFO is 
     * BFIFOSample * 8. 
     */ 
    I2CWrite(FIFO_SAMPLES_NUMBER_REG, param);
    return value;

  }


/***************************************************************************
 *
 * Function : get_fifo_samples_number
 *
 * RET  : Return the value of register FifoSamplesNumber
 *
 * Notes : From register 0x0D6E, bits 7:0
 *
 **************************************************************************/
unsigned char get_fifo_samples_number()
  {
    unsigned char value;


    /* 
     * Number of samples currently present in burst FIFO is 
     * BFIFOSample * 8. 
     */ 
   value = I2CRead(FIFO_SAMPLES_NUMBER_REG) ;
    return value;
  }


/***************************************************************************
 *
 * Function : get_ab8500_revison
 *
 * RET  : Return the value of register Revision
 *
 * Notes : From register 0x0D6F, bits 4:0
 *
 **************************************************************************/
unsigned char get_ab8500_revison()
  {
    unsigned char value;


    /* 
     * Audio module revision 
     * 00000: AB8500 Early drop 
     * 00001: AB8500 cut 1.0 
     * 00010: AB8500 cut 2.0 
     */ 
    value = (I2CRead(REVISION_REG) & AB_8500_REVISON_PARAM_MASK);
    return value;
  }


