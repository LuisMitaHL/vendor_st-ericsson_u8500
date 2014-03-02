/****************************************************************************
 * File : c:/Lisp/Production/ab8500/ab8500_Control/Linux/ab8500_Control_types.h
 * 
 *
 * Generated on the 04/04/2011 14:09 by the 'gen-I2C' code generator 
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

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_AB8500_CONTROL_LINUX_AB8500_CONTROL_TYPES_H
#define C__LISP_PRODUCTION_AB8500_AB8500_CONTROL_LINUX_AB8500_CONTROL_TYPES_H




typedef enum key_detected {
    NOT_TURN_ON_E,
    TURN_ON_E
} KEY_DETECTED_T ;



//  
//  0: UsbIDDetect is not a turn on event 
//  1: UsbIDDetect is a turn on event.
//  Register SystemContolStatus 0x0100, Bits 6:6, typedef usb_detect_on
//   
//  
#define USB_ID_DETECT_MASK 0x40
#define USB_ID_DETECT_OFFSET 6
#define USB_ID_DETECT_STOP_BIT 6
#define USB_ID_DETECT_START_BIT 6
#define USB_ID_DETECT_WIDTH 1

#define SYSTEM_CONTOL_STATUS_REG 0x100

typedef enum usb_detect_on {
    USB_DETECT_OFF_E,
    USB_DETECT_ON_E
} USB_DETECT_ON_T ;

#define USB_DETECT_ON_PARAM_MASK  0x40

//  
//  0: VbusDetect is not a turn on event 
//  1: VbusDetect is a turn on event.
//  Register SystemContolStatus 0x0100, Bits 5:5, typedef vbus_detect_on
//   
//  
#define VBUS_DECTECT_MASK 0x20
#define VBUS_DECTECT_OFFSET 5
#define VBUS_DECTECT_STOP_BIT 5
#define VBUS_DECTECT_START_BIT 5
#define VBUS_DECTECT_WIDTH 1


typedef enum vbus_detect_on {
    VBUS_DETECT_OFF_E,
    VBUS_DETECT_ON_E
} VBUS_DETECT_ON_T ;

#define VBUS_DETECT_ON_PARAM_MASK  0x20

//  
//  0: MainChDetect is not a turn on event 
//  1: MainChDetect is a turn on event.
//  Register SystemContolStatus 0x0100, Bits 4:4, typedef main_ch_detect_on
//   
//  
#define MAIN_CH_DETECT_MASK 0x10
#define MAIN_CH_DETECT_OFFSET 4
#define MAIN_CH_DETECT_STOP_BIT 4
#define MAIN_CH_DETECT_START_BIT 4
#define MAIN_CH_DETECT_WIDTH 1


typedef enum main_ch_detect_on {
    MAIN_CH_DETECT_OFF_E,
    MAIN_CH_DETECT_ON_E
} MAIN_CH_DETECT_ON_T ;

#define MAIN_CH_DETECT_ON_PARAM_MASK  0x10

//  
//  0: RtcAlarmDetect is not a turn on event 
//  1: RtcAlarmDetect is a turn on event.
//  Register SystemContolStatus 0x0100, Bits 3:3, typedef rtc_alarm_detect_on
//   
//  
#define RTC_ALARM_DETECT_MASK 0x8
#define RTC_ALARM_DETECT_OFFSET 3
#define RTC_ALARM_DETECT_STOP_BIT 3
#define RTC_ALARM_DETECT_START_BIT 3
#define RTC_ALARM_DETECT_WIDTH 1


typedef enum rtc_alarm_detect_on {
    RTC_ALARM_DETECT_OFF_E,
    RTC_ALARM_DETECT_ON_E
} RTC_ALARM_DETECT_ON_T ;

#define RTC_ALARM_DETECT_ON_PARAM_MASK  0x8

//  
//  0: PowerOnKey2Detect is not a turn on event 
//  1: PowerOnKey2Detect is a turn on event.
//  Register SystemContolStatus 0x0100, Bits 2:2, typedef key_detected
//   
//  
#define POWER_ON_KEY_2_DETECT_MASK 0x4
#define POWER_ON_KEY_2_DETECT_OFFSET 2
#define POWER_ON_KEY_2_DETECT_STOP_BIT 2
#define POWER_ON_KEY_2_DETECT_START_BIT 2
#define POWER_ON_KEY_2_DETECT_WIDTH 1

#define POWER_ON_KEY_2_DETECT_PARAM_MASK  0x4
#define KEY_DETECTED_PARAM_MASK  0x4

//  
//  0: PowerOnKey1Detect is not a turn on event 
//  1: PowerOnKey1Detect is a turn on event.
//  Register SystemContolStatus 0x0100, Bits 1:1, typedef key_detected
//   
//  
#define POWER_ON_KEY_1_DETECT_MASK 0x2
#define POWER_ON_KEY_1_DETECT_OFFSET 1
#define POWER_ON_KEY_1_DETECT_STOP_BIT 1
#define POWER_ON_KEY_1_DETECT_START_BIT 1
#define POWER_ON_KEY_1_DETECT_WIDTH 1

#define POWER_ON_KEY_1_DETECT_PARAM_MASK  0x2

//  
//  0: PowerOnKey1Detect is not a turn on event 
//  1: PowerOnKey1Detect is a turn on event.
//  Register SystemContolStatus 0x0100, Bits 0:0, typedef battery_detect_on
//   
//  
#define BATTERY_DETECT_MASK 0x1
#define BATTERY_DETECT_OFFSET 0
#define BATTERY_DETECT_STOP_BIT 0
#define BATTERY_DETECT_START_BIT 0
#define BATTERY_DETECT_WIDTH 1


typedef enum battery_detect_on {
    BATTERY_DETECT_OFF_E,
    BATTERY_DETECT_ON_E
} BATTERY_DETECT_ON_T ;

#define BATTERY_DETECT_ON_PARAM_MASK  0x1

//  
//  Software Reset status.
//  Register ResetStatus 0x0101, Bits 2:2, typedef software_reset_status
//   
//  
#define SOFTWARE_RESET_FLAG_MASK 0x4
#define SOFTWARE_RESET_FLAG_OFFSET 2
#define SOFTWARE_RESET_FLAG_STOP_BIT 2
#define SOFTWARE_RESET_FLAG_START_BIT 2
#define SOFTWARE_RESET_FLAG_WIDTH 1

#define RESET_STATUS_REG 0x101
#define SOFTWARE_RESET_STATUS_PARAM_MASK  0x4

//  
//  Reset 4500 status.
//  Register ResetStatus 0x0101, Bits 0:0, typedef reset4500_status
//   
//  
#define RESET_4500_MASK 0x1
#define RESET_4500_OFFSET 0
#define RESET_4500_STOP_BIT 0
#define RESET_4500_START_BIT 0
#define RESET_4500_WIDTH 1

#define RESET_4500_STATUS_PARAM_MASK  0x1

//  
//  A 7 bit counter is used to measure PonKey1db pressure duration with steps 
//  of 125 msec and a range of 10 sec. PonKey1PressTime[6:0] indicates number of 125ms 
//  steps..
//  Register SystemContolStatus2 0x0102, Bits 0:6, typedef press_time
//   
//  
#define POWER_ON_KEY_1_PRESS_TIME_MASK 0x7F
#define POWER_ON_KEY_1_PRESS_TIME_OFFSET 6
#define POWER_ON_KEY_1_PRESS_TIME_STOP_BIT 6
#define POWER_ON_KEY_1_PRESS_TIME_START_BIT 0
#define POWER_ON_KEY_1_PRESS_TIME_WIDTH 7

#define SYSTEM_CONTOL_STATUS_2_REG 0x102
#define PRESS_TIME_PARAM_MASK  0x7F
#define SYS_CLK_REQ_REG 0x142

//  
//  Thermal DB8500 Software Off.
//  Register Control 0x0180, Bits 2:2, typedef thermal_software_off
//   
//  
#define THERMAL_SOFTWARE_OFF_MASK 0x4
#define THERMAL_SOFTWARE_OFF_OFFSET 2
#define THERMAL_SOFTWARE_OFF_STOP_BIT 2
#define THERMAL_SOFTWARE_OFF_START_BIT 2
#define THERMAL_SOFTWARE_OFF_WIDTH 1

#define CONTROL_REG 0x180
#define  THERMAL_SOFTWARE_OFF 4

typedef enum thermal_software_off {
    THERMAL_SOFTWARE_OFF_E,
    THERMAL_SOFTWARE_ON_E
} THERMAL_SOFTWARE_OFF_T ;
#define THERMAL_SOFTWARE_OFF_PARAM_MASK  0x4



//  
//  0: Reset Ab8500 registers 
//  1: inactive.
//  Register Control 0x0180, Bits 1:1, typedef software_reset
//   
//  
#define SOFTWARE_RESET_MASK 0x2
#define SOFTWARE_RESET_OFFSET 1
#define SOFTWARE_RESET_STOP_BIT 1
#define SOFTWARE_RESET_START_BIT 1
#define SOFTWARE_RESET_WIDTH 1


typedef enum software_reset {
    SOFTWARE_RESET_E,
    SOFTWARE_UN_RESET_E
} SOFTWARE_RESET_T ;

#define SOFTWARE_RESET_PARAM_MASK  0x2

//  
//  0: inactive 
//  1: Turn off Ab8500.
//  Register Control 0x0180, Bits 0:0, typedef software_off
//   
//  
#define SOFTWARE_OFF_MASK 0x1
#define SOFTWARE_OFF_OFFSET 0
#define SOFTWARE_OFF_STOP_BIT 0
#define SOFTWARE_OFF_START_BIT 0
#define SOFTWARE_OFF_WIDTH 1

#define  SOFTWARE_OFF 1

typedef enum software_off {
    TURN_OFF_STW8500_INACTIVE_E,
    TURN_OFF_STW8500_E
} SOFTWARE_OFF_T ;
#define SOFTWARE_OFF_PARAM_MASK  0x1



typedef enum reset_valid {
    STAY_PROGRAMMED_ON_RESET_E,
    RESET_TO_DEFAULT_ON_RESET_E
} RESET_VALID_T ;



//  
//  0: Doesn't valid reset of 'Vsmps1Regu[1:0]', 'Vsmps1RequestCtrl[1:0]' and 
//  Vsmps1Sel(i)[5:0] bits when AB8500 is reset 
//  1: Allow to valid reset of 'Vsmps1Regu[1:0]', 'Vsmps1RequestCtrl[1:0]' and 
//  'Vsmps1Sel(i)[5:0]' bits when AB8500 is reset.
//  Register Control2 0x0181, Bits 7:7, typedef reset_valid
//   
//  
#define RESET_VSMPS_1_VALID_MASK 0x80
#define RESET_VSMPS_1_VALID_OFFSET 7
#define RESET_VSMPS_1_VALID_STOP_BIT 7
#define RESET_VSMPS_1_VALID_START_BIT 7
#define RESET_VSMPS_1_VALID_WIDTH 1

#define CONTROL_2_REG 0x181
#define RESET_VSMPS_1_VALID_PARAM_MASK  0x80
#define RESET_VALID_PARAM_MASK  0x80

//  
//  0: Doesn't valid reset of 'VextSupply3Regu[1:0]', 'VextSupply3RequestCtrl[1:0] 
//  ExtSup13LPnClkSel and ExtSupply13LP bits when AB8500 is reset 
//  1: Allow to valid reset of 'VextSupply3Regu[1:0]', 
//  'VextSupply3RequestCtrl[1:'ExtSup13LPnClkSel' and 'ExtSupply13LP' bits when AB8500 is 
//  reset.
//  Register Control2 0x0181, Bits 6:6, typedef reset_valid
//   
//  
#define RESET_VEXT_SUPPLY_3_VALID_MASK 0x40
#define RESET_VEXT_SUPPLY_3_VALID_OFFSET 6
#define RESET_VEXT_SUPPLY_3_VALID_STOP_BIT 6
#define RESET_VEXT_SUPPLY_3_VALID_START_BIT 6
#define RESET_VEXT_SUPPLY_3_VALID_WIDTH 1

#define RESET_VEXT_SUPPLY_3_VALID_PARAM_MASK  0x40

//  
//  0: Doesn't valid reset of 'VextSupply2Regu[1:0]', 'VextSupply2RequestCtrl[1:0] 
//  ExtSup12LPnClkSel and ExtSupply12LP bits when AB8500 is reset 
//  1: Allow to valid reset of 'VextSupply2Regu[1:0]', 
//  'VextSupply2RequestCtrl[1:'ExtSup12LPnClkSel' and 'ExtSupply12LP' bits when AB8500 is 
//  reset.
//  Register Control2 0x0181, Bits 5:5, typedef reset_valid
//   
//  
#define RESET_VEXT_SUPPLY_2_VALID_MASK 0x20
#define RESET_VEXT_SUPPLY_2_VALID_OFFSET 5
#define RESET_VEXT_SUPPLY_2_VALID_STOP_BIT 5
#define RESET_VEXT_SUPPLY_2_VALID_START_BIT 5
#define RESET_VEXT_SUPPLY_2_VALID_WIDTH 1

#define RESET_VEXT_SUPPLY_2_VALID_PARAM_MASK  0x20

//  
//  0: Doesn't valid reset of 'VextSupply1Regu[1:0]', 
//  'VextSupply1RequestCtrl[1:0]','ExtSup12LPnClkSel' and 'ExtSupply12LP' bits when 
//  AB8500 is reset 
//  1: Allow to valid reset of 'VextSupply1Regu[1:0]', 'VextSupply1RequestCtrl[1:0]', 
//  'ExtSup12LPnClkSel' and 'ExtSupply12LP' bits when AB8500 is reset.
//  Register Control2 0x0181, Bits 4:4, typedef reset_valid
//   
//  
#define RESET_VEXT_SUPPLY_VALID_MASK 0x10
#define RESET_VEXT_SUPPLY_VALID_OFFSET 4
#define RESET_VEXT_SUPPLY_VALID_STOP_BIT 4
#define RESET_VEXT_SUPPLY_VALID_START_BIT 4
#define RESET_VEXT_SUPPLY_VALID_WIDTH 1

#define RESET_VEXT_SUPPLY_VALID_PARAM_MASK  0x10

//  
//  0: VmodRegu, VmodRequestCtrl and VmodSel bits stay at their programmed value when 
//  Ab8500 is resetted 
//  1: VmodRegu, VmodRequestCtrl and Vaux3Sel bits are reset to their default values when 
//  Ab8500 is resetted.
//  Register Control2 0x0181, Bits 3:3, typedef reset_valid
//   
//  
#define RESET_VMOD_VALID_MASK 0x8
#define RESET_VMOD_VALID_OFFSET 3
#define RESET_VMOD_VALID_STOP_BIT 3
#define RESET_VMOD_VALID_START_BIT 3
#define RESET_VMOD_VALID_WIDTH 1

#define RESET_VMOD_VALID_PARAM_MASK  0x8

//  
//  0: Doesn't valid reset of Vaux3Regu[1:0], Vaux3RequestCtrl[1:0] and Vaux3Sel[2:0] 
//  bits when AB8500 is reset 
//  1: allow to valid reset of Vaux3Regu[1:0], Vaux3RequestCtrl[1:0] and Vaux3Sel[2:0] 
//  bits when AB8500 is reset.
//  Register Control2 0x0181, Bits 2:2, typedef reset_valid
//   
//  
#define RESET_VAUX_3_VALID_MASK 0x4
#define RESET_VAUX_3_VALID_OFFSET 2
#define RESET_VAUX_3_VALID_STOP_BIT 2
#define RESET_VAUX_3_VALID_START_BIT 2
#define RESET_VAUX_3_VALID_WIDTH 1

#define RESET_VAUX_3_VALID_PARAM_MASK  0x4

//  
//  0: Vaux2Regu, Vaux2RequestCtrl and Vaux2Sel bits stay at their programmed value when 
//  Ab8500 is resetted 
//  1: Vaux2Regu, Vaux2RequestCtrl and Vaux2Sel bits are reset to their default values 
//  when Ab8500 is resetted.
//  Register Control2 0x0181, Bits 1:1, typedef reset_valid
//   
//  
#define RESET_VAUX_2_VALID_MASK 0x2
#define RESET_VAUX_2_VALID_OFFSET 1
#define RESET_VAUX_2_VALID_STOP_BIT 1
#define RESET_VAUX_2_VALID_START_BIT 1
#define RESET_VAUX_2_VALID_WIDTH 1

#define RESET_VAUX_2_VALID_PARAM_MASK  0x2

//  
//  0: Vaux1Regu, Vaux1RequestCtrl and Vaux1Sel bits stay at their programmed value when 
//  Ab8500 is resetted 
//  1: Vaux1Regu, Vaux1RequestCtrl and Vaux1Sel bits are reset to their default values 
//  when Ab8500 is resetted.
//  Register Control2 0x0181, Bits 0:0, typedef reset_valid
//   
//  
#define RESET_VAUX_1_VALID_MASK 0x1
#define RESET_VAUX_1_VALID_OFFSET 0
#define RESET_VAUX_1_VALID_STOP_BIT 0
#define RESET_VAUX_1_VALID_START_BIT 0
#define RESET_VAUX_1_VALID_WIDTH 1

#define RESET_VAUX_1_VALID_PARAM_MASK  0x1

//  
//  0: disable thermal shutdown 
//  1: enable thermal shutdown.
//  Register Control3 0x0200, Bits 3:3, typedef thermal_shutdown_enable
//   
//  
#define THERMAL_SHUTDOWN_MASK 0x8
#define THERMAL_SHUTDOWN_OFFSET 3
#define THERMAL_SHUTDOWN_STOP_BIT 3
#define THERMAL_SHUTDOWN_START_BIT 3
#define THERMAL_SHUTDOWN_WIDTH 1

#define CONTROL_3_REG 0x200

typedef enum thermal_shutdown_enable {
    THERMAL_SHUTDOWN_DISABLE_E,
    THERMAL_SHUTDOWN_ENABLE_E
} THERMAL_SHUTDOWN_ENABLE_T ;

#define THERMAL_SHUTDOWN_ENABLE_PARAM_MASK  0x8

//  
//  0: reset VideoDenc registers 
//  1: inactive.
//  Register Control3 0x0200, Bits 2:2, typedef video_denc_reset
//   
//  
#define VIDEO_DENC_RESET_MASK 0x4
#define VIDEO_DENC_RESET_OFFSET 2
#define VIDEO_DENC_RESET_STOP_BIT 2
#define VIDEO_DENC_RESET_START_BIT 2
#define VIDEO_DENC_RESET_WIDTH 1


typedef enum video_denc_reset {
    VIDEO_DENC_RESET_E,
    VIDEO_DENC_UN_RESET_E
} VIDEO_DENC_RESET_T ;

#define VIDEO_DENC_RESET_PARAM_MASK  0x4

//  
//  0: reset Audio registers 
//  1: inactive.
//  Register Control3 0x0200, Bits 1:1, typedef audio_reset
//   
//  
#define AUDIO_RESET_MASK 0x2
#define AUDIO_RESET_OFFSET 1
#define AUDIO_RESET_STOP_BIT 1
#define AUDIO_RESET_START_BIT 1
#define AUDIO_RESET_WIDTH 1


typedef enum audio_reset {
    AUDIO_RESET_E,
    AUDIO_UN_RESET_E
} AUDIO_RESET_T ;

#define AUDIO_RESET_PARAM_MASK  0x2

//  
//  0: inactive 1: disable Clk32KOut2 output buffer.
//  Register Control3 0x0200, Bits 0:0, typedef clock32_output_buffer_enable
//   
//  
#define CLOCK_32_MASK 0x1
#define CLOCK_32_OFFSET 0
#define CLOCK_32_STOP_BIT 0
#define CLOCK_32_START_BIT 0
#define CLOCK_32_WIDTH 1


typedef enum clock32_output_buffer_enable {
    CLOCK_32_OUTPUT_BUFFER_ENABLE_E,
    CLOCK_32_OUTPUT_BUFFER_DISABLE_E
} CLOCK_32_OUTPUT_BUFFER_ENABLE_T ;

#define CLOCK_32_OUTPUT_BUFFER_ENABLE_PARAM_MASK  0x1

//  
//  0: when Watchdog expires AB8500 is turned off 
//  1: when Watchdog expires AB8500 is turned off and then turned On running start up 
//  sequence..
//  Register MainWatchDogControl 0x0201, Bits 4:4, typedef main_watch_dog_write_enable
//   
//  
#define TURN_ON_VALID_ON_EXPIRE_MASK 0x10
#define TURN_ON_VALID_ON_EXPIRE_OFFSET 4
#define TURN_ON_VALID_ON_EXPIRE_STOP_BIT 4
#define TURN_ON_VALID_ON_EXPIRE_START_BIT 4
#define TURN_ON_VALID_ON_EXPIRE_WIDTH 1

#define MAIN_WATCH_DOG_CONTROL_REG 0x201
#define  MAIN_WATCH_DOG_WRITE_ENABLE 16

typedef enum main_watch_dog_write_enable {
    STAY_TURNED_OFF_E,
    TURN_OFF_AND_STARTUP_E
} MAIN_WATCH_DOG_WRITE_ENABLE_T ;
#define MAIN_WATCH_DOG_WRITE_ENABLE_PARAM_MASK  0x10



//  
//  Kick Main watchdog. Auto-cleared.
//  Register MainWatchDogControl 0x0201, Bits 1:1, typedef kick_main_watchdog_enable
//   
//  
#define MAIN_WATCHDOG_KICK_MASK 0x2
#define MAIN_WATCHDOG_KICK_OFFSET 1
#define MAIN_WATCHDOG_KICK_STOP_BIT 1
#define MAIN_WATCHDOG_KICK_START_BIT 1
#define MAIN_WATCHDOG_KICK_WIDTH 1


typedef enum kick_main_watchdog_enable {
    KICK_MAIN_WATCHDOG_DISABLE_E,
    KICK_MAIN_WATCHDOG_ENABLE_E
} KICK_MAIN_WATCHDOG_ENABLE_T ;

#define KICK_MAIN_WATCHDOG_ENABLE_PARAM_MASK  0x2

//  
//  Enable Main watchdog.
//  Register MainWatchDogControl 0x0201, Bits 0:0, typedef main_watchdog_enable
//   
//  
#define MAIN_WATCHDOG_ENABLE_MASK 0x1
#define MAIN_WATCHDOG_ENABLE_OFFSET 0
#define MAIN_WATCHDOG_ENABLE_STOP_BIT 0
#define MAIN_WATCHDOG_ENABLE_START_BIT 0
#define MAIN_WATCHDOG_ENABLE_WIDTH 1


typedef enum main_watchdog_enable {
    MAIN_WATCHDOG_DISABLE_E,
    MAIN_WATCHDOG_ENABLE_E
} MAIN_WATCHDOG_ENABLE_T ;

#define MAIN_WATCHDOG_ENABLE_PARAM_MASK  0x1

//  
//  Defined Main watchdog time from 1s to 128s by 1s steps, default value is 
//  32s.
//  Register MainWatchDogTimer 0x0202, Bits 0:6, typedef main_watchdog_timer
//   
//  
#define MAIN_WATCHDOG_TIMER_MASK 0x7F
#define MAIN_WATCHDOG_TIMER_OFFSET 6
#define MAIN_WATCHDOG_TIMER_STOP_BIT 6
#define MAIN_WATCHDOG_TIMER_START_BIT 0
#define MAIN_WATCHDOG_TIMER_WIDTH 7

#define MAIN_WATCH_DOG_TIMER_REG 0x202
#define MAIN_WATCHDOG_TIMER_PARAM_MASK  0x7F
typedef enum main_watchdog_timer {
    WATCH_DOG_TIMER_0_SECOND = 0x0,
    WATCH_DOG_TIMER_1_SECOND = 0x1,
    WATCH_DOG_TIMER_2_SECOND = 0x2,
    WATCH_DOG_TIMER_3_SECOND = 0x3,
    WATCH_DOG_TIMER_4_SECOND = 0x4,
    WATCH_DOG_TIMER_5_SECOND = 0x5,
    WATCH_DOG_TIMER_6_SECOND = 0x6,
    WATCH_DOG_TIMER_7_SECOND = 0x7,
    WATCH_DOG_TIMER_8_SECOND = 0x8,
    WATCH_DOG_TIMER_9_SECOND = 0x9,
    WATCH_DOG_TIMER_10_SECOND = 0xA,
    WATCH_DOG_TIMER_11_SECOND = 0xB,
    WATCH_DOG_TIMER_12_SECOND = 0xC
} MAIN_WATCHDOG_TIMER_T ;

//  
//  Defined LowBat threshold from 2.3v to 3.85v 25mv steps.
//  Register MainWatchDogTimer1 0x0203, Bits 1:6, typedef low_bat_threshold
//   
//  
#define LOW_BAT_THRESHOLD_MASK 0x7E
#define LOW_BAT_THRESHOLD_OFFSET 6
#define LOW_BAT_THRESHOLD_STOP_BIT 6
#define LOW_BAT_THRESHOLD_START_BIT 1
#define LOW_BAT_THRESHOLD_WIDTH 6

#define MAIN_WATCH_DOG_TIMER_1_REG 0x203
#define LOW_BAT_THRESHOLD_PARAM_MASK  0x7E
typedef enum low_bat_threshold {
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2300_MV = 0x0,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2325_MV = 0x1,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2350_MV = 0x2,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2375_MV = 0x3,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2400_MV = 0x4,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2425_MV = 0x5,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2450_MV = 0x6,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2475_MV = 0x7,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2500_MV = 0x8,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2525_MV = 0x9,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2550_MV = 0xA,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2575_MV = 0xB,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2600_MV = 0xC,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2625_MV = 0xD,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2650_MV = 0xE,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2675_MV = 0xF,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2700_MV = 0x10,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2725_MV = 0x11,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2750_MV = 0x12,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2775_MV = 0x13,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2800_MV = 0x14,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2825_MV = 0x15,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2850_MV = 0x16,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2875_MV = 0x17,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2900_MV = 0x18,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2925_MV = 0x19,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2950_MV = 0x1A,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_2975_MV = 0x1B,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3000_MV = 0x1C,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3025_MV = 0x1D,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3050_MV = 0x1E,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3075_MV = 0x1F,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3100_MV = 0x20,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3125_MV = 0x21,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3150_MV = 0x22,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3175_MV = 0x23,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3200_MV = 0x24,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3225_MV = 0x25,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3250_MV = 0x26,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3275_MV = 0x27,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3300_MV = 0x28,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3325_MV = 0x29,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3350_MV = 0x2A,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3375_MV = 0x2B,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3400_MV = 0x2C,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3425_MV = 0x2D,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3450_MV = 0x2E,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3475_MV = 0x2F,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3500_MV = 0x30,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3525_MV = 0x31,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3550_MV = 0x32,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3575_MV = 0x33,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3600_MV = 0x34,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3625_MV = 0x35,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3650_MV = 0x36,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3675_MV = 0x37,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3700_MV = 0x38,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3725_MV = 0x39,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3750_MV = 0x3A,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3775_MV = 0x3B,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3800_MV = 0x3C,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3825_MV = 0x3D,
    LOW_BAT_LOWBAT_THRESHOLDHRESHOLD_3850_MV = 0x3E
} LOW_BAT_THRESHOLD_T ;

//  
//  0: disable LowBat function 
//  1: enable LowBat function.
//  Register MainWatchDogTimer1 0x0203, Bits 0:0, typedef low_battery_enable
//   
//  
#define LOW_BATTERY_ENABLE_MASK 0x1
#define LOW_BATTERY_ENABLE_OFFSET 0
#define LOW_BATTERY_ENABLE_STOP_BIT 0
#define LOW_BATTERY_ENABLE_START_BIT 0
#define LOW_BATTERY_ENABLE_WIDTH 1


typedef enum low_battery_enable {
    LOW_BATTERY_DISABLE_E,
    LOW_BATTERY_ENABLE_E
} LOW_BATTERY_ENABLE_T ;

#define LOW_BATTERY_ENABLE_PARAM_MASK  0x1


typedef enum battery_ok2_threshold {
    BATTERY_OK2_2360_MILLIVOLT = 0x0,
    BATTERY_OK2_2410_MILLIVOLT = 0x1,
    BATTERY_OK2_2460_MILLIVOLT = 0x2,
    BATTERY_OK2_2510_MILLIVOLT = 0x3,
    BATTERY_OK2_2560_MILLIVOLT = 0x4,
    BATTERY_OK2_2610_MILLIVOLT = 0x5,
    BATTERY_OK2_2660_MILLIVOLT = 0x6,
    BATTERY_OK2_2710_MILLIVOLT = 0x7,
    BATTERY_OK2_2760_MILLIVOLT = 0x8,
    BATTERY_OK2_2810_MILLIVOLT = 0x9,
    BATTERY_OK2_2860_MILLIVOLT = 0xA,
    BATTERY_OK2_2910_MILLIVOLT = 0xB,
    BATTERY_OK2_2960_MILLIVOLT = 0xC,
    BATTERY_OK2_3010_MILLIVOLT = 0xD,
    BATTERY_OK2_3060_MILLIVOLT = 0xE
} BATTERY_OK_2_THRESHOLD_T ;




typedef enum battery_ok1_threshold {
    BATTERY_OK1_2360_MILLIVOLT = 0x0,
    BATTERY_OK1_2410_MILLIVOLT = 0x1,
    BATTERY_OK1_2460_MILLIVOLT = 0x2,
    BATTERY_OK1_2510_MILLIVOLT = 0x3,
    BATTERY_OK1_2560_MILLIVOLT = 0x4,
    BATTERY_OK1_2610_MILLIVOLT = 0x5,
    BATTERY_OK1_2660_MILLIVOLT = 0x6,
    BATTERY_OK1_2710_MILLIVOLT = 0x7,
    BATTERY_OK1_2760_MILLIVOLT = 0x8,
    BATTERY_OK1_2810_MILLIVOLT = 0x9,
    BATTERY_OK1_2860_MILLIVOLT = 0xA,
    BATTERY_OK1_2910_MILLIVOLT = 0xB,
    BATTERY_OK1_2960_MILLIVOLT = 0xC,
    BATTERY_OK1_3010_MILLIVOLT = 0xD,
    BATTERY_OK1_3060_MILLIVOLT = 0xE
} BATTERY_OK_1_THRESHOLD_T ;



//  
//  BattOk2 falling thresholds management.
//  Register BatteryOkThreshold 0x0204, Bits 4:7, typedef battery_ok2_threshold
//   
//  
#define BATTERY_OK_2_THRESHOLD_MASK 0xF0
#define BATTERY_OK_2_THRESHOLD_OFFSET 7
#define BATTERY_OK_2_THRESHOLD_STOP_BIT 7
#define BATTERY_OK_2_THRESHOLD_START_BIT 4
#define BATTERY_OK_2_THRESHOLD_WIDTH 4

#define BATTERY_OK_THRESHOLD_REG 0x204
#define BATTERY_OK_2_THRESHOLD_PARAM_MASK  0xF0

//  
//  BattOk1 falling thresholds management.
//  Register BatteryOkThreshold 0x0204, Bits 0:3, typedef battery_ok1_threshold
//   
//  
#define BATTERY_OK_1_THRESHOLD_MASK 0xF
#define BATTERY_OK_1_THRESHOLD_OFFSET 3
#define BATTERY_OK_1_THRESHOLD_STOP_BIT 3
#define BATTERY_OK_1_THRESHOLD_START_BIT 0
#define BATTERY_OK_1_THRESHOLD_WIDTH 4

#define BATTERY_OK_1_THRESHOLD_PARAM_MASK  0xF

//  
//  System clock timer management. Adjustement value multiple of 32 Khz.
//  Register SystemClockTimer 0x0205, Bits 4:7, typedef timer_adjustement
//   
//  
#define SYSTEM_CLOCK_TIMER_ADJUSTEMENT_MASK 0xF0
#define SYSTEM_CLOCK_TIMER_ADJUSTEMENT_OFFSET 7
#define SYSTEM_CLOCK_TIMER_ADJUSTEMENT_STOP_BIT 7
#define SYSTEM_CLOCK_TIMER_ADJUSTEMENT_START_BIT 4
#define SYSTEM_CLOCK_TIMER_ADJUSTEMENT_WIDTH 4

#define SYSTEM_CLOCK_TIMER_REG 0x205
#define TIMER_ADJUSTEMENT_PARAM_MASK  0xF0
typedef enum timer_adjustement {
    ADJUSTEMENT_0_OF_32_KHZ = 0x0,
    ADJUSTEMENT_1_OF_32_KHZ = 0x1,
    ADJUSTEMENT_2_OF_32_KHZ = 0x2,
    ADJUSTEMENT_3_OF_32_KHZ = 0x3,
    ADJUSTEMENT_4_OF_32_KHZ = 0x4,
    ADJUSTEMENT_5_OF_32_KHZ = 0x5,
    ADJUSTEMENT_6_OF_32_KHZ = 0x6,
    ADJUSTEMENT_7_OF_32_KHZ = 0x7,
    ADJUSTEMENT_8_OF_32_KHZ = 0x8,
    ADJUSTEMENT_9_OF_32_KHZ = 0x9,
    ADJUSTEMENT_10_OF_32_KHZ = 0xA,
    ADJUSTEMENT_11_OF_32_KHZ = 0xB,
    ADJUSTEMENT_12_OF_32_KHZ = 0xC,
    ADJUSTEMENT_13_OF_32_KHZ = 0xD,
    ADJUSTEMENT_14_OF_32_KHZ = 0xE,
    ADJUSTEMENT_15_OF_32_KHZ = 0xF
} TIMER_ADJUSTEMENT_T ;

//  
//  System clock timer management. value multiple of 32 Khz.
//  Register SystemClockTimer 0x0205, Bits 0:3, typedef clock_timer
//   
//  
#define SYSTEM_CLOCK_TIMER_MASK 0xF
#define SYSTEM_CLOCK_TIMER_OFFSET 3
#define SYSTEM_CLOCK_TIMER_STOP_BIT 3
#define SYSTEM_CLOCK_TIMER_START_BIT 0
#define SYSTEM_CLOCK_TIMER_WIDTH 4

#define CLOCK_TIMER_PARAM_MASK  0xF
typedef enum clock_timer {
    PERIOD_0_OF_32_KHZ = 0x0,
    PERIOD_13_OF_32_KHZ = 0x1,
    PERIOD_14_OF_32_KHZ = 0x2,
    PERIOD_334_OF_32_KHZ = 0x3
} CLOCK_TIMER_T ;

//  
//  Force internal 3.2MHz oscillator ON.
//  Register SmpsClockControl 0x0206, Bits 2:2, typedef force_smps_clock_on
//   
//  
#define FORCE_SMPS_CLOCK_MASK 0x4
#define FORCE_SMPS_CLOCK_OFFSET 2
#define FORCE_SMPS_CLOCK_STOP_BIT 2
#define FORCE_SMPS_CLOCK_START_BIT 2
#define FORCE_SMPS_CLOCK_WIDTH 1

#define SMPS_CLOCK_CONTROL_REG 0x206

typedef enum force_smps_clock_on {
    FORCE_SMPS_CLOCK_OFF_E,
    FORCE_SMPS_CLOCK_ON_E
} FORCE_SMPS_CLOCK_ON_T ;

#define FORCE_SMPS_CLOCK_ON_PARAM_MASK  0x4

//  
//  00: smps internal clock is SysClk (for Early Drop to change in inactive) 
//  01: smps internal clock is internal 3.2Mhz oscillator 
//  10: smps internal is first SysClk (if present ) or secondly internal 3.2Mhz 
//  oscillator 
//  11: smps internal is first SysClk (if present) or secondly ulp clock (if present) or 
//  third internal 3.2Mhz oscillator.
//  Register SmpsClockControl 0x0206, Bits 0:1, typedef smps_clock_selection
//   
//  
#define SMPS_CLOCK_SELECTION_MASK 0x3
#define SMPS_CLOCK_SELECTION_OFFSET 1
#define SMPS_CLOCK_SELECTION_STOP_BIT 1
#define SMPS_CLOCK_SELECTION_START_BIT 0
#define SMPS_CLOCK_SELECTION_WIDTH 2

#define  SYS_CLOCK 0
#define  INTERNAL_OSCILLATOR 1
#define  SYS_CLOCK_FISRT_OR_INTERNAL_OSCILLATOR 2
#define  SYS_CLOCK_FISRT_OR_ULP_CLOKC_OR_INTERNAL_OSCILLATOR 3
#define SMPS_CLOCK_SELECTION_PARAM_MASK  0x3



typedef enum smps_clock_selection {
    SYS_CLOCK_E,
    INTERNAL_OSCILLATOR_E,
    SYS_CLOCK_FISRT_OR_INTERNAL_OSCILLATOR_E,
    SYS_CLOCK_FISRT_OR_ULP_CLOKC_OR_INTERNAL_OSCILLATOR_E
} SMPS_CLOCK_SELECTION_T ;



#define  PHASE_0 0
#define  PHASE_60 1
#define  PHASE_120 2
#define  PHASE_180 3
#define  PHASE_240 4
#define  PHASE_300 5



typedef enum smps_ckock_phase {
    PHASE_0_E,
    PHASE_60_E,
    PHASE_120_E,
    PHASE_180_E,
    PHASE_240_E,
    PHASE_300_E
} SMPS_CKOCK_PHASE_T ;




//  
//  Smps clock phase management versus IntClk clock.
//  Register SmpsClockPhase1 0x0207, Bits 3:5, typedef smps_ckock_phase
//   
//  
#define VAPE_CLOCK_SEL_MASK 0x38
#define VAPE_CLOCK_SEL_OFFSET 5
#define VAPE_CLOCK_SEL_STOP_BIT 5
#define VAPE_CLOCK_SEL_START_BIT 3
#define VAPE_CLOCK_SEL_WIDTH 3

#define SMPS_CLOCK_PHASE_1_REG 0x207
#define VAPE_CLOCK_SEL_PARAM_MASK  0x38
#define SMPS_CKOCK_PHASE_PARAM_MASK  0x38

//  
//  Smps clock phase management versus IntClk clock.
//  Register SmpsClockPhase1 0x0207, Bits 0:2, typedef smps_ckock_phase
//   
//  
#define VARM_CLOCK_SEL_MASK 0x7
#define VARM_CLOCK_SEL_OFFSET 2
#define VARM_CLOCK_SEL_STOP_BIT 2
#define VARM_CLOCK_SEL_START_BIT 0
#define VARM_CLOCK_SEL_WIDTH 3

#define VARM_CLOCK_SEL_PARAM_MASK  0x7

//  
//  Smps clock phase management versus IntClk clock.
//  Register SmpsClockPhase2 0x0208, Bits 3:5, typedef smps_ckock_phase
//   
//  
#define VSMPS_1_CLOCK_SEL_MASK 0x38
#define VSMPS_1_CLOCK_SEL_OFFSET 5
#define VSMPS_1_CLOCK_SEL_STOP_BIT 5
#define VSMPS_1_CLOCK_SEL_START_BIT 3
#define VSMPS_1_CLOCK_SEL_WIDTH 3

#define SMPS_CLOCK_PHASE_2_REG 0x208
#define VSMPS_1_CLOCK_SEL_PARAM_MASK  0x38

//  
//  Smps clock phase management versus IntClk clock.
//  Register SmpsClockPhase2 0x0208, Bits 0:2, typedef smps_ckock_phase
//   
//  
#define VSMOD_CLOCK_SEL_MASK 0x7
#define VSMOD_CLOCK_SEL_OFFSET 2
#define VSMOD_CLOCK_SEL_STOP_BIT 2
#define VSMOD_CLOCK_SEL_START_BIT 0
#define VSMOD_CLOCK_SEL_WIDTH 3

#define VSMOD_CLOCK_SEL_PARAM_MASK  0x7

//  
//  Smps clock phase management versus IntClk clock.
//  Register SmpsClockPhase3 0x0209, Bits 3:5, typedef smps_ckock_phase
//   
//  
#define VSMPS_3_CLOCK_SEL_MASK 0x38
#define VSMPS_3_CLOCK_SEL_OFFSET 5
#define VSMPS_3_CLOCK_SEL_STOP_BIT 5
#define VSMPS_3_CLOCK_SEL_START_BIT 3
#define VSMPS_3_CLOCK_SEL_WIDTH 3

#define SMPS_CLOCK_PHASE_3_REG 0x209
#define VSMPS_3_CLOCK_SEL_PARAM_MASK  0x38

//  
//  Smps clock phase management versus IntClk clock.
//  Register SmpsClockPhase3 0x0209, Bits 0:2, typedef smps_ckock_phase
//   
//  
#define VSMPS_2_CLOCK_SEL_MASK 0x7
#define VSMPS_2_CLOCK_SEL_OFFSET 2
#define VSMPS_2_CLOCK_SEL_STOP_BIT 2
#define VSMPS_2_CLOCK_SEL_START_BIT 0
#define VSMPS_2_CLOCK_SEL_WIDTH 3

#define VSMPS_2_CLOCK_SEL_PARAM_MASK  0x7

typedef enum strength {
    LOW_STRENGTH_E,
    HIGH_STRENGTH_E
} STRENGTH_T ;



//  
//  enable output buffer pulldown.
//  Register SystemUlpClockConf 0x020A, Bits 7:7, typedef ouput_buffer_p_d_enable
//   
//  
#define CLOCK_27_OUPUT_BUFFER_DP_MASK 0x80
#define CLOCK_27_OUPUT_BUFFER_DP_OFFSET 7
#define CLOCK_27_OUPUT_BUFFER_DP_STOP_BIT 7
#define CLOCK_27_OUPUT_BUFFER_DP_START_BIT 7
#define CLOCK_27_OUPUT_BUFFER_DP_WIDTH 1

#define SYSTEM_ULP_CLOCK_CONF_REG 0x20A

typedef enum ouput_buffer_p_d_enable {
    OUPUT_BUFFER_PD_DISABLE_E,
    OUPUT_BUFFER_PD_ENABLE_E
} OUPUT_BUFFER_PD_ENABLE_T ;

#define OUPUT_BUFFER_PD_ENABLE_PARAM_MASK  0x80

//  
//  enable output buffer.
//  Register SystemUlpClockConf 0x020A, Bits 6:6, typedef ouput_buffer_enable
//   
//  
#define CLOCK_27_OUPUT_BUFFER_MASK 0x40
#define CLOCK_27_OUPUT_BUFFER_OFFSET 6
#define CLOCK_27_OUPUT_BUFFER_STOP_BIT 6
#define CLOCK_27_OUPUT_BUFFER_START_BIT 6
#define CLOCK_27_OUPUT_BUFFER_WIDTH 1


typedef enum ouput_buffer_enable {
    OUPUT_BUFFER_DISABLE_E,
    OUPUT_BUFFER_ENABLE_E
} OUPUT_BUFFER_ENABLE_T ;

#define OUPUT_BUFFER_ENABLE_PARAM_MASK  0x40

//  
//  0: Low strength (able to drive a capacitor load up to 20pF) 
//  1: High strength (able to drive a capacitor load up to 40pF).
//  Register SystemUlpClockConf 0x020A, Bits 5:5, typedef strength
//   
//  
#define ULTA_LOW_POWER_CLOCK_STRENGTH_MASK 0x20
#define ULTA_LOW_POWER_CLOCK_STRENGTH_OFFSET 5
#define ULTA_LOW_POWER_CLOCK_STRENGTH_STOP_BIT 5
#define ULTA_LOW_POWER_CLOCK_STRENGTH_START_BIT 5
#define ULTA_LOW_POWER_CLOCK_STRENGTH_WIDTH 1

#define ULTA_LOW_POWER_CLOCK_STRENGTH_PARAM_MASK  0x20
#define STRENGTH_PARAM_MASK  0x20

//  
//  internal TVout clock is inverted in order to add internal delay.
//  Register SystemUlpClockConf 0x020A, Bits 4:4, typedef tvout_clock_inverted
//   
//  
#define TVOUT_CLOCK_INVERTED_MASK 0x10
#define TVOUT_CLOCK_INVERTED_OFFSET 4
#define TVOUT_CLOCK_INVERTED_STOP_BIT 4
#define TVOUT_CLOCK_INVERTED_START_BIT 4
#define TVOUT_CLOCK_INVERTED_WIDTH 1

#define  TVOUT_CLOCK_INVERTED 16

typedef enum tvout_clock_inverted {
    NON_INVERTED_E,
    CLOCK_INVERTED_E
} TVOUT_CLOCK_INVERTED_T ;
#define TVOUT_CLOCK_INVERTED_PARAM_MASK  0x10



//  
//  internal input buffer is enabled in order to add internally delay to TVout 
//  clock.
//  Register SystemUlpClockConf 0x020A, Bits 3:3, typedef tvout_clock_input_enable
//   
//  
#define TVOUT_CLOCK_INPUT_MASK 0x8
#define TVOUT_CLOCK_INPUT_OFFSET 3
#define TVOUT_CLOCK_INPUT_STOP_BIT 3
#define TVOUT_CLOCK_INPUT_START_BIT 3
#define TVOUT_CLOCK_INPUT_WIDTH 1


typedef enum tvout_clock_input_enable {
    TVOUT_CLOCK_INPUT_ENABLE_E,
    TVOUT_CLOCK_INPUT_DISABLE_E
} TVOUT_CLOCK_INPUT_ENABLE_T ;

#define TVOUT_CLOCK_INPUT_ENABLE_PARAM_MASK  0x8

//  
//  0: Low strength (able to drive a capacitor load up to 20pF) 
//  1: High strength (able to drive a capacitor load up to 40pF).
//  Register SystemUlpClockConf 0x020A, Bits 2:2, typedef strength
//   
//  
#define CLOCK_27_STRENGTH_MASK 0x4
#define CLOCK_27_STRENGTH_OFFSET 2
#define CLOCK_27_STRENGTH_STOP_BIT 2
#define CLOCK_27_STRENGTH_START_BIT 2
#define CLOCK_27_STRENGTH_WIDTH 1

#define CLOCK_27_STRENGTH_PARAM_MASK  0x4

//  
//  UlpClk pad configurations. ab8500 centric.
//  Register SystemUlpClockConf 0x020A, Bits 0:1, typedef ulta_low_power_padonnf
//   
//  
#define ULTA_LOW_POWER_PAD_MASK 0x3
#define ULTA_LOW_POWER_PAD_OFFSET 1
#define ULTA_LOW_POWER_PAD_STOP_BIT 1
#define ULTA_LOW_POWER_PAD_START_BIT 0
#define ULTA_LOW_POWER_PAD_WIDTH 2

#define  ULTRA_LOW_POWER_PAD_NO_FUNTION 0
#define  ULTRA_LOW_POWER_PAD_OUTPUT 1
#define  ULTRA_LOW_POWER_PAD_INPUT 2
#define ULTA_LOW_POWER_PADONNF_PARAM_MASK  0x3



typedef enum ulta_low_power_padonnf {
    ULTRA_LOW_POWER_PAD_NO_FUNTION_E,
    ULTRA_LOW_POWER_PAD_OUTPUT_E,
    ULTRA_LOW_POWER_PAD_INPUT_E
} ULTA_LOW_POWER_PADONNF_T ;




//  
//  1: enable clock buffer 4 of U8500 RF / U8500 RF update device.
//  Register ClockManagement 0x020B, Bits 7:7, typedef activate_sys_clock_buffer4
//   
//  
#define SYS_CLK_BUF_4_REQ_MASK 0x80
#define SYS_CLK_BUF_4_REQ_OFFSET 7
#define SYS_CLK_BUF_4_REQ_STOP_BIT 7
#define SYS_CLK_BUF_4_REQ_START_BIT 7
#define SYS_CLK_BUF_4_REQ_WIDTH 1

#define CLOCK_MANAGEMENT_REG 0x20B
#define  ACTIVATE_SYS_CLOCK_BUFFER_4 128

typedef enum activate_sys_clock_buffer4 {
    INACTIVE_SYS_CLOCK_BUFFER_4_E,
    ACTIVATE_SYS_CLOCK_BUFFER_4_E
} ACTIVATE_SYS_CLOCK_BUFFER_4_T ;
#define ACTIVATE_SYS_CLOCK_BUFFER_4_PARAM_MASK  0x80



//  
//  1: enable clock buffer 3 of U8500 RF / U8500 RF update device.
//  Register ClockManagement 0x020B, Bits 6:6, typedef activate_sys_clock_buffer3
//   
//  
#define SYS_CLK_BUF_3_REQ_MASK 0x40
#define SYS_CLK_BUF_3_REQ_OFFSET 6
#define SYS_CLK_BUF_3_REQ_STOP_BIT 6
#define SYS_CLK_BUF_3_REQ_START_BIT 6
#define SYS_CLK_BUF_3_REQ_WIDTH 1

#define  ACTIVATE_SYS_CLOCK_BUFFER_3 64

typedef enum activate_sys_clock_buffer3 {
    INACTIVE_SYS_CLOCK_BUFFER_3_E,
    ACTIVATE_SYS_CLOCK_BUFFER_3_E
} ACTIVATE_SYS_CLOCK_BUFFER_3_T ;
#define ACTIVATE_SYS_CLOCK_BUFFER_3_PARAM_MASK  0x40



//  
//  1: enable clock buffer 2 of U8500 RF / U8500 RF update device.
//  Register ClockManagement 0x020B, Bits 5:5, typedef activate_sys_clock_buffer2
//   
//  
#define SYS_CLK_BUF_2_REQ_MASK 0x20
#define SYS_CLK_BUF_2_REQ_OFFSET 5
#define SYS_CLK_BUF_2_REQ_STOP_BIT 5
#define SYS_CLK_BUF_2_REQ_START_BIT 5
#define SYS_CLK_BUF_2_REQ_WIDTH 1

#define  ACTIVATE_SYS_CLOCK_BUFFER_2 32

typedef enum activate_sys_clock_buffer2 {
    INACTIVE_SYS_CLOCK_BUFFER_2_E,
    ACTIVATE_SYS_CLOCK_BUFFER_2_E
} ACTIVATE_SYS_CLOCK_BUFFER_2_T ;
#define ACTIVATE_SYS_CLOCK_BUFFER_2_PARAM_MASK  0x20



//  
//  enable audio clock to audio block.
//  Register ClockManagement 0x020B, Bits 4:4, typedef audio_clock_enable
//   
//  
#define AUDIO_CLOCK_MASK 0x10
#define AUDIO_CLOCK_OFFSET 4
#define AUDIO_CLOCK_STOP_BIT 4
#define AUDIO_CLOCK_START_BIT 4
#define AUDIO_CLOCK_WIDTH 1


typedef enum audio_clock_enable {
    AUDIO_CLOCK_DISABLE_E,
    AUDIO_CLOCK_ENABLE_E
} AUDIO_CLOCK_ENABLE_T ;

#define AUDIO_CLOCK_ENABLE_PARAM_MASK  0x10

//  
//  Ab8500 request system clock.
//  Register ClockManagement 0x020B, Bits 3:3, typedef system_clock_request_enable
//   
//  
#define SYSTEM_CLOCK_REQUEST_MASK 0x8
#define SYSTEM_CLOCK_REQUEST_OFFSET 3
#define SYSTEM_CLOCK_REQUEST_STOP_BIT 3
#define SYSTEM_CLOCK_REQUEST_START_BIT 3
#define SYSTEM_CLOCK_REQUEST_WIDTH 1


typedef enum system_clock_request_enable {
    SYSTEM_CLOCK_REQUEST_DISABLE_E,
    SYSTEM_CLOCK_REQUEST_ENABLE_E
} SYSTEM_CLOCK_REQUEST_ENABLE_T ;

#define SYSTEM_CLOCK_REQUEST_ENABLE_PARAM_MASK  0x8

//  
//  Ab8500 request ulp clock.
//  Register ClockManagement 0x020B, Bits 2:2, typedef ulp_request_enable
//   
//  
#define ULP_REQUEST_MASK 0x4
#define ULP_REQUEST_OFFSET 2
#define ULP_REQUEST_STOP_BIT 2
#define ULP_REQUEST_START_BIT 2
#define ULP_REQUEST_WIDTH 1


typedef enum ulp_request_enable {
    ULP_REQUEST_DISABLE_E,
    ULP_REQUEST_ENABLE_E
} ULP_REQUEST_ENABLE_T ;

#define ULP_REQUEST_ENABLE_PARAM_MASK  0x4

//  
//  internal clock switching.
//  Register ClockManagement 0x020B, Bits 0:1, typedef sys_ultra_low_power_selection
//   
//  
#define SYS_ULTRA_LOW_POWER_SELECTION_MASK 0x3
#define SYS_ULTRA_LOW_POWER_SELECTION_OFFSET 1
#define SYS_ULTRA_LOW_POWER_SELECTION_STOP_BIT 1
#define SYS_ULTRA_LOW_POWER_SELECTION_START_BIT 0
#define SYS_ULTRA_LOW_POWER_SELECTION_WIDTH 2

#define  INTERNAL_CLOCK_SYSTEM_CLOCK 0
#define  INTERNAL_CLOCK_ULP_CLOCK 1
#define  INTERNAL_CLOCK_SWITCHING_SYS_CLOCK_FISRT 2
#define SYS_ULTRA_LOW_POWER_SELECTION_PARAM_MASK  0x3



typedef enum sys_ultra_low_power_selection {
    INTERNAL_CLOCK_SYSTEM_CLOCK_E,
    INTERNAL_CLOCK_ULP_CLOCK_E,
    INTERNAL_CLOCK_SWITCHING_SYS_CLOCK_FISRT_E
} SYS_ULTRA_LOW_POWER_SELECTION_T ;




//  
//  0: disable SysClk path to USB block 
//  1: enable SysClk path to USB block.
//  Register ClockValid 0x020C, Bits 2:2, typedef usb_clock_enable
//   
//  
#define USB_CLOCK_ENABLE_MASK 0x4
#define USB_CLOCK_ENABLE_OFFSET 2
#define USB_CLOCK_ENABLE_STOP_BIT 2
#define USB_CLOCK_ENABLE_START_BIT 2
#define USB_CLOCK_ENABLE_WIDTH 1

#define CLOCK_VALID_REG 0x20C

typedef enum usb_clock_enable {
    USB_CLOCK_DISABLE_E,
    USB_CLOCK_ENABLE_E
} USB_CLOCK_ENABLE_T ;

#define USB_CLOCK_ENABLE_PARAM_MASK  0x4

//  
//  0: disable 27Mhz clock to TVout block 
//  1: enable 27Mhz clock to TVout block.
//  Register ClockValid 0x020C, Bits 1:1, typedef t_vout_clk_enable
//   
//  
#define T_VOUT_CLOCK_ENABLE_MASK 0x2
#define T_VOUT_CLOCK_ENABLE_OFFSET 1
#define T_VOUT_CLOCK_ENABLE_STOP_BIT 1
#define T_VOUT_CLOCK_ENABLE_START_BIT 1
#define T_VOUT_CLOCK_ENABLE_WIDTH 1


typedef enum t_vout_clk_enable {
    T_VOUT_CLK_DISABLE_E,
    T_VOUT_CLK_ENABLE_E
} T_VOUT_CLK_ENABLE_T ;

#define T_VOUT_CLK_ENABLE_PARAM_MASK  0x2

//  
//  enable TVout pll (generate 27Mhz from SysClk).
//  Register ClockValid 0x020C, Bits 0:0, typedef tv_out_pll_enable
//   
//  
#define TV_OUT_PLL_ENABLE_MASK 0x1
#define TV_OUT_PLL_ENABLE_OFFSET 0
#define TV_OUT_PLL_ENABLE_STOP_BIT 0
#define TV_OUT_PLL_ENABLE_START_BIT 0
#define TV_OUT_PLL_ENABLE_WIDTH 1


typedef enum tv_out_pll_enable {
    TV_OUT_PLL_DISABLE_E,
    TV_OUT_PLL_ENABLE_E
} TV_OUT_PLL_ENABLE_T ;

#define TV_OUT_PLL_ENABLE_PARAM_MASK  0x1

//  
//  0: doesn't valid Usb block to request System 
//  1: valid Usb block to request System clock.
//  Register ClockValid2 0x020D, Bits 2:2, typedef usb_sys_clockr_request1_enable
//   
//  
#define USB_SYS_CLOCKR_REQUEST_1_VALID_MASK 0x4
#define USB_SYS_CLOCKR_REQUEST_1_VALID_OFFSET 2
#define USB_SYS_CLOCKR_REQUEST_1_VALID_STOP_BIT 2
#define USB_SYS_CLOCKR_REQUEST_1_VALID_START_BIT 2
#define USB_SYS_CLOCKR_REQUEST_1_VALID_WIDTH 1

#define CLOCK_VALID_2_REG 0x20D

typedef enum usb_sys_clockr_request1_enable {
    USB_SYS_CLOCKR_REQUEST_1_DISABLE_E,
    USB_SYS_CLOCKR_REQUEST_1_ENABLE_E
} USB_SYS_CLOCKR_REQUEST_1_ENABLE_T ;

#define USB_SYS_CLOCKR_REQUEST_1_ENABLE_PARAM_MASK  0x4

//  
//  valid ulp clock request from SysClkReq1 input.
//  Register ClockValid2 0x020D, Bits 1:1, typedef ultra_low_power_request1_enable
//   
//  
#define ULTRA_LOW_POWER_REQUEST_1_VALID_MASK 0x2
#define ULTRA_LOW_POWER_REQUEST_1_VALID_OFFSET 1
#define ULTRA_LOW_POWER_REQUEST_1_VALID_STOP_BIT 1
#define ULTRA_LOW_POWER_REQUEST_1_VALID_START_BIT 1
#define ULTRA_LOW_POWER_REQUEST_1_VALID_WIDTH 1


typedef enum ultra_low_power_request1_enable {
    ULTRA_LOW_POWER_DOWN_E,
    ULTRA_LOW_POWER_UP_E
} ULTRA_LOW_POWER_REQUEST_1_ENABLE_T ;

#define ULTRA_LOW_POWER_REQUEST_1_ENABLE_PARAM_MASK  0x2

//  
//  Sys valid system clock request from SysClkReq1input.
//  Register ClockValid2 0x020D, Bits 0:0, typedef sys_clock_request1_valid
//   
//  
#define SYS_CLOCK_REQUEST_1_VALID_MASK 0x1
#define SYS_CLOCK_REQUEST_1_VALID_OFFSET 0
#define SYS_CLOCK_REQUEST_1_VALID_STOP_BIT 0
#define SYS_CLOCK_REQUEST_1_VALID_START_BIT 0
#define SYS_CLOCK_REQUEST_1_VALID_WIDTH 1

#define  SYS_CLOCK_REQUEST_1_VALID 1

typedef enum sys_clock_request1_valid {
    APE_DONT_VALID_ULP_RESQUEST_E,
    APE_VALID_ULP_RESQUEST_E
} SYS_CLOCK_REQUEST_1_VALID_T ;
#define SYS_CLOCK_REQUEST_1_VALID_PARAM_MASK  0x1



//  
//  0: Int8500n is a push/pull output 
//  1: Int8500n is an OpenDrain output.
//  Register ControlBall 0x020F, Bits 4:4, typedef ball_configuration
//   
//  
#define INTERRUPT_8500_OPEN_DRAIN_MASK 0x10
#define INTERRUPT_8500_OPEN_DRAIN_OFFSET 4
#define INTERRUPT_8500_OPEN_DRAIN_STOP_BIT 4
#define INTERRUPT_8500_OPEN_DRAIN_START_BIT 4
#define INTERRUPT_8500_OPEN_DRAIN_WIDTH 1

#define CONTROL_BALL_REG 0x20F
#define  BALL_CONFIGURATION 16

typedef enum ball_configuration {
    PUSH_PULL_E,
    OPEN_DRAIN_E
} BALL_CONFIGURATION_T ;
#define BALL_CONFIGURATION_PARAM_MASK  0x10



//  
//  00: select ExtSup3LPn function = ExtSup3LP OR ExtSup3LP 
//  01: select ExtSupply3Clk function. ExtSupply3Clk can be internal oscillator, Sysclk 
//  or UlpClk. 
//  10 or 11: select (ExtSupply3LPn AND ExtSupply3Clk).
//  Register ControlBall 0x020F, Bits 2:3, typedef configuration3
//   
//  
#define EXT_SUP_3L_PN_CLK_SEL_MASK 0xC
#define EXT_SUP_3L_PN_CLK_SEL_OFFSET 3
#define EXT_SUP_3L_PN_CLK_SEL_STOP_BIT 3
#define EXT_SUP_3L_PN_CLK_SEL_START_BIT 2
#define EXT_SUP_3L_PN_CLK_SEL_WIDTH 2

#define  EXT_SUP3_LPN 0
#define  EXT_SUP_3LPN 1
#define  EXT_SUPPLY3_LPN_AND_EXT_SUPPLY3_CLK 2
#define CONFIGURATION_3_PARAM_MASK  0xC



typedef enum configuration3 {
    EXT_SUP3_LPN_E,
    EXT_SUP_3LPN_E,
    EXT_SUPPLY3_LPN_AND_EXT_SUPPLY3_CLK_E
} CONFIGURATION_3_T ;




//  
//  00: select ExtSup12LPn function = ExtSup12LP OR ExtSup12LP 
//  01: select ExtSupply12Clk function. ExtSupply12Clk can be internal oscillator, Sysclk 
//  or UlpClk. 
//  10 or 11: select (ExtSupply12LPn AND ExtSupply12Clk).
//  Register ControlBall 0x020F, Bits 0:1, typedef configuration12
//   
//  
#define EXT_SUP_12L_PN_CLK_SEL_MASK 0x3
#define EXT_SUP_12L_PN_CLK_SEL_OFFSET 1
#define EXT_SUP_12L_PN_CLK_SEL_STOP_BIT 1
#define EXT_SUP_12L_PN_CLK_SEL_START_BIT 0
#define EXT_SUP_12L_PN_CLK_SEL_WIDTH 2

#define  EXT_SUP12_LPN 0
#define  EXT_SUPPLY_12CLK 1
#define  EXT_SUPPLY12_LPN_AND_EXT_SUPPLY12_CLK 2
#define CONFIGURATION_12_PARAM_MASK  0x3



typedef enum configuration12 {
    EXT_SUP12_LPN_E,
    EXT_SUPPLY_12CLK_E,
    EXT_SUPPLY12_LPN_AND_EXT_SUPPLY12_CLK_E
} CONFIGURATION_12_T ;




typedef enum sys_clock_resquest {
    SYS_CLOCK_NOT_VALID_E,
    SYS_CLOCK_RESQUET_E
} SYS_CLOCK_RESQUEST_T ;



//  
//  0: SysClkReq1 is not associated to clock buffer 4 of Levi/Yllas RF I.C. 
//  1: SysClkReq1 is associated to clock buffer 4 of Levi/Yllas RF I.C..
//  Register ClockValid3 0x0210, Bits 4:4, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_4_REQ_1_MASK 0x10
#define SYS_CLOCK_BUF_4_REQ_1_OFFSET 4
#define SYS_CLOCK_BUF_4_REQ_1_STOP_BIT 4
#define SYS_CLOCK_BUF_4_REQ_1_START_BIT 4
#define SYS_CLOCK_BUF_4_REQ_1_WIDTH 1

#define CLOCK_VALID_3_REG 0x210
#define SYS_CLOCK_BUF_4_REQ_1_PARAM_MASK  0x10
#define SYS_CLOCK_RESQUEST_PARAM_MASK  0x10

//  
//  0: SysClkReq1 is not associated to clock buffer 3 of Levi/Yllas RF I.C. 
//  1: SysClkReq1 is associated to clock buffer 3 of Levi/Yllas RF I.C..
//  Register ClockValid3 0x0210, Bits 3:3, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_3_REQ_1_MASK 0x8
#define SYS_CLOCK_BUF_3_REQ_1_OFFSET 3
#define SYS_CLOCK_BUF_3_REQ_1_STOP_BIT 3
#define SYS_CLOCK_BUF_3_REQ_1_START_BIT 3
#define SYS_CLOCK_BUF_3_REQ_1_WIDTH 1

#define SYS_CLOCK_BUF_3_REQ_1_PARAM_MASK  0x8

//  
//  0: SysClkReq1 is not associated to clock buffer 2 of Levi/Yllas RF I.C. 
//  1: SysClkReq1 is associated to clock buffer 2 of Levi/Yllas RF I.C..
//  Register ClockValid3 0x0210, Bits 2:2, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_2_REQ_1_MASK 0x4
#define SYS_CLOCK_BUF_2_REQ_1_OFFSET 2
#define SYS_CLOCK_BUF_2_REQ_1_STOP_BIT 2
#define SYS_CLOCK_BUF_2_REQ_1_START_BIT 2
#define SYS_CLOCK_BUF_2_REQ_1_WIDTH 1

#define SYS_CLOCK_BUF_2_REQ_1_PARAM_MASK  0x4

//  
//  0: SysClkReq2 is not associated to clock buffer 4 of Levi/Yllas RF I.C. 
//  1: SysClkReq2 is associated to clock buffer 4 of Levi/Yllas RF I.C..
//  Register ClockValid4 0x0211, Bits 4:4, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_4_REQ_2_MASK 0x10
#define SYS_CLOCK_BUF_4_REQ_2_OFFSET 4
#define SYS_CLOCK_BUF_4_REQ_2_STOP_BIT 4
#define SYS_CLOCK_BUF_4_REQ_2_START_BIT 4
#define SYS_CLOCK_BUF_4_REQ_2_WIDTH 1

#define CLOCK_VALID_4_REG 0x211
#define SYS_CLOCK_BUF_4_REQ_2_PARAM_MASK  0x10

//  
//  0: SysClkReq2 is not associated to clock buffer 3 of Levi/Yllas RF I.C. 
//  1: SysClkReq2 is associated to clock buffer 3 of Levi/Yllas RF I.C..
//  Register ClockValid4 0x0211, Bits 3:3, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_3_REQ_2_MASK 0x8
#define SYS_CLOCK_BUF_3_REQ_2_OFFSET 3
#define SYS_CLOCK_BUF_3_REQ_2_STOP_BIT 3
#define SYS_CLOCK_BUF_3_REQ_2_START_BIT 3
#define SYS_CLOCK_BUF_3_REQ_2_WIDTH 1

#define SYS_CLOCK_BUF_3_REQ_2_PARAM_MASK  0x8

//  
//  0: SysClkReq2 is not associated to clock buffer 2 of Levi/Yllas RF I.C. 
//  1: SysClkReq2 is associated to clock buffer 2 of Levi/Yllas RF I.C..
//  Register ClockValid4 0x0211, Bits 2:2, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_2_REQ_2_MASK 0x4
#define SYS_CLOCK_BUF_2_REQ_2_OFFSET 2
#define SYS_CLOCK_BUF_2_REQ_2_STOP_BIT 2
#define SYS_CLOCK_BUF_2_REQ_2_START_BIT 2
#define SYS_CLOCK_BUF_2_REQ_2_WIDTH 1

#define SYS_CLOCK_BUF_2_REQ_2_PARAM_MASK  0x4

//  
//  0: SysClkReq3 is not associated to clock buffer 4 of Levi/Yllas RF I.C. 
//  1: SysClkReq3 is associated to clock buffer 4 of Levi/Yllas RF I.C..
//  Register ClockValid5 0x0212, Bits 4:4, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_4_REQ_3_MASK 0x10
#define SYS_CLOCK_BUF_4_REQ_3_OFFSET 4
#define SYS_CLOCK_BUF_4_REQ_3_STOP_BIT 4
#define SYS_CLOCK_BUF_4_REQ_3_START_BIT 4
#define SYS_CLOCK_BUF_4_REQ_3_WIDTH 1

#define CLOCK_VALID_5_REG 0x212
#define SYS_CLOCK_BUF_4_REQ_3_PARAM_MASK  0x10

//  
//  0: SysClkReq3 is not associated to clock buffer 3 of Levi/Yllas RF I.C. 
//  1: SysClkReq3 is associated to clock buffer 3 of Levi/Yllas RF I.C..
//  Register ClockValid5 0x0212, Bits 3:3, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_3_REQ_3_MASK 0x8
#define SYS_CLOCK_BUF_3_REQ_3_OFFSET 3
#define SYS_CLOCK_BUF_3_REQ_3_STOP_BIT 3
#define SYS_CLOCK_BUF_3_REQ_3_START_BIT 3
#define SYS_CLOCK_BUF_3_REQ_3_WIDTH 1

#define SYS_CLOCK_BUF_3_REQ_3_PARAM_MASK  0x8

//  
//  0: SysClkReq3 is not associated to clock buffer 2 of Levi/Yllas RF I.C. 
//  1: SysClkReq3 is associated to clock buffer 2 of Levi/Yllas RF I.C..
//  Register ClockValid5 0x0212, Bits 2:2, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_2_REQ_3_MASK 0x4
#define SYS_CLOCK_BUF_2_REQ_3_OFFSET 2
#define SYS_CLOCK_BUF_2_REQ_3_STOP_BIT 2
#define SYS_CLOCK_BUF_2_REQ_3_START_BIT 2
#define SYS_CLOCK_BUF_2_REQ_3_WIDTH 1

#define SYS_CLOCK_BUF_2_REQ_3_PARAM_MASK  0x4

//  
//  0: SysClkReq4 is not associated to clock buffer 4 of Levi/Yllas RF I.C. 
//  1: SysClkReq4 is associated to clock buffer 4 of Levi/Yllas RF I.C..
//  Register ClockValid6 0x0213, Bits 4:4, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_4_REQ_4_MASK 0x10
#define SYS_CLOCK_BUF_4_REQ_4_OFFSET 4
#define SYS_CLOCK_BUF_4_REQ_4_STOP_BIT 4
#define SYS_CLOCK_BUF_4_REQ_4_START_BIT 4
#define SYS_CLOCK_BUF_4_REQ_4_WIDTH 1

#define CLOCK_VALID_6_REG 0x213
#define SYS_CLOCK_BUF_4_REQ_4_PARAM_MASK  0x10

//  
//  0: SysClkReq4 is not associated to clock buffer 3 of Levi/Yllas RF I.C. 
//  1: SysClkReq4 is associated to clock buffer 3 of Levi/Yllas RF I.C..
//  Register ClockValid6 0x0213, Bits 3:3, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_3_REQ_4_MASK 0x8
#define SYS_CLOCK_BUF_3_REQ_4_OFFSET 3
#define SYS_CLOCK_BUF_3_REQ_4_STOP_BIT 3
#define SYS_CLOCK_BUF_3_REQ_4_START_BIT 3
#define SYS_CLOCK_BUF_3_REQ_4_WIDTH 1

#define SYS_CLOCK_BUF_3_REQ_4_PARAM_MASK  0x8

//  
//  0: SysClkReq4 is not associated to clock buffer 2 of Levi/Yllas RF I.C. 
//  1: SysClkReq4 is associated to clock buffer 2 of Levi/Yllas RF I.C..
//  Register ClockValid6 0x0213, Bits 2:2, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_2_REQ_4_MASK 0x4
#define SYS_CLOCK_BUF_2_REQ_4_OFFSET 2
#define SYS_CLOCK_BUF_2_REQ_4_STOP_BIT 2
#define SYS_CLOCK_BUF_2_REQ_4_START_BIT 2
#define SYS_CLOCK_BUF_2_REQ_4_WIDTH 1

#define SYS_CLOCK_BUF_2_REQ_4_PARAM_MASK  0x4

//  
//  0: SysClkReq5 is not associated to clock buffer 4 of Levi/Yllas RF I.C. 
//  1: SysClkReq5 is associated to clock buffer 4 of Levi/Yllas RF I.C..
//  Register ClockValid7 0x0214, Bits 4:4, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_4_REQ_5_MASK 0x10
#define SYS_CLOCK_BUF_4_REQ_5_OFFSET 4
#define SYS_CLOCK_BUF_4_REQ_5_STOP_BIT 4
#define SYS_CLOCK_BUF_4_REQ_5_START_BIT 4
#define SYS_CLOCK_BUF_4_REQ_5_WIDTH 1

#define CLOCK_VALID_7_REG 0x214
#define SYS_CLOCK_BUF_4_REQ_5_PARAM_MASK  0x10

//  
//  0: SysClkReq5 is not associated to clock buffer 3 of Levi/Yllas RF I.C. 
//  1: SysClkReq5 is associated to clock buffer 3 of Levi/Yllas RF I.C..
//  Register ClockValid7 0x0214, Bits 3:3, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_3_REQ_5_MASK 0x8
#define SYS_CLOCK_BUF_3_REQ_5_OFFSET 3
#define SYS_CLOCK_BUF_3_REQ_5_STOP_BIT 3
#define SYS_CLOCK_BUF_3_REQ_5_START_BIT 3
#define SYS_CLOCK_BUF_3_REQ_5_WIDTH 1

#define SYS_CLOCK_BUF_3_REQ_5_PARAM_MASK  0x8

//  
//  0: SysClkReq5 is not associated to clock buffer 2 of Levi/Yllas RF I.C. 
//  1: SysClkReq5 is associated to clock buffer 2 of Levi/Yllas RF I.C..
//  Register ClockValid7 0x0214, Bits 2:2, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_2_REQ_5_MASK 0x4
#define SYS_CLOCK_BUF_2_REQ_5_OFFSET 2
#define SYS_CLOCK_BUF_2_REQ_5_STOP_BIT 2
#define SYS_CLOCK_BUF_2_REQ_5_START_BIT 2
#define SYS_CLOCK_BUF_2_REQ_5_WIDTH 1

#define SYS_CLOCK_BUF_2_REQ_5_PARAM_MASK  0x4

//  
//  0: SysClkReq6 is not associated to clock buffer 4 of Levi/Yllas RF I.C. 
//  1: SysClkReq6 is associated to clock buffer 4 of Levi/Yllas RF I.C..
//  Register ClockValid8 0x0215, Bits 4:4, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_4_REQ_6_MASK 0x10
#define SYS_CLOCK_BUF_4_REQ_6_OFFSET 4
#define SYS_CLOCK_BUF_4_REQ_6_STOP_BIT 4
#define SYS_CLOCK_BUF_4_REQ_6_START_BIT 4
#define SYS_CLOCK_BUF_4_REQ_6_WIDTH 1

#define CLOCK_VALID_8_REG 0x215
#define SYS_CLOCK_BUF_4_REQ_6_PARAM_MASK  0x10

//  
//  0: SysClkReq6 is not associated to clock buffer 3 of Levi/Yllas RF I.C. 
//  1: SysClkReq6 is associated to clock buffer 3 of Levi/Yllas RF I.C..
//  Register ClockValid8 0x0215, Bits 3:3, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_3_REQ_6_MASK 0x8
#define SYS_CLOCK_BUF_3_REQ_6_OFFSET 3
#define SYS_CLOCK_BUF_3_REQ_6_STOP_BIT 3
#define SYS_CLOCK_BUF_3_REQ_6_START_BIT 3
#define SYS_CLOCK_BUF_3_REQ_6_WIDTH 1

#define SYS_CLOCK_BUF_3_REQ_6_PARAM_MASK  0x8

//  
//  0: SysClkReq6 is not associated to clock buffer 2 of Levi/Yllas RF I.C. 
//  1: SysClkReq6 is associated to clock buffer 2 of Levi/Yllas RF I.C..
//  Register ClockValid8 0x0215, Bits 2:2, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_2_REQ_6_MASK 0x4
#define SYS_CLOCK_BUF_2_REQ_6_OFFSET 2
#define SYS_CLOCK_BUF_2_REQ_6_STOP_BIT 2
#define SYS_CLOCK_BUF_2_REQ_6_START_BIT 2
#define SYS_CLOCK_BUF_2_REQ_6_WIDTH 1

#define SYS_CLOCK_BUF_2_REQ_6_PARAM_MASK  0x4

//  
//  0: SysClkReq7 is not associated to clock buffer 4 of Levi/Yllas RF I.C. 
//  1: SysClkReq7 is associated to clock buffer 4 of Levi/Yllas RF I.C..
//  Register ClockValid9 0x0216, Bits 4:4, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_4_REQ_7_MASK 0x10
#define SYS_CLOCK_BUF_4_REQ_7_OFFSET 4
#define SYS_CLOCK_BUF_4_REQ_7_STOP_BIT 4
#define SYS_CLOCK_BUF_4_REQ_7_START_BIT 4
#define SYS_CLOCK_BUF_4_REQ_7_WIDTH 1

#define CLOCK_VALID_9_REG 0x216
#define SYS_CLOCK_BUF_4_REQ_7_PARAM_MASK  0x10

//  
//  0: SysClkReq7 is not associated to clock buffer 3 of Levi/Yllas RF I.C. 
//  1: SysClkReq7 is associated to clock buffer 3 of Levi/Yllas RF I.C..
//  Register ClockValid9 0x0216, Bits 3:3, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_3_REQ_7_MASK 0x8
#define SYS_CLOCK_BUF_3_REQ_7_OFFSET 3
#define SYS_CLOCK_BUF_3_REQ_7_STOP_BIT 3
#define SYS_CLOCK_BUF_3_REQ_7_START_BIT 3
#define SYS_CLOCK_BUF_3_REQ_7_WIDTH 1

#define SYS_CLOCK_BUF_3_REQ_7_PARAM_MASK  0x8

//  
//  0: SysClkReq7 is not associated to clock buffer 2 of Levi/Yllas RF I.C. 
//  1: SysClkReq7 is associated to clock buffer 2 of Levi/Yllas RF I.C..
//  Register ClockValid9 0x0216, Bits 2:2, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_2_REQ_7_MASK 0x4
#define SYS_CLOCK_BUF_2_REQ_7_OFFSET 2
#define SYS_CLOCK_BUF_2_REQ_7_STOP_BIT 2
#define SYS_CLOCK_BUF_2_REQ_7_START_BIT 2
#define SYS_CLOCK_BUF_2_REQ_7_WIDTH 1

#define SYS_CLOCK_BUF_2_REQ_7_PARAM_MASK  0x4

//  
//  0: SysClkReq8 is not associated to clock buffer 4 of Levi/Yllas RF I.C. 
//  1: SysClkReq8 is associated to clock buffer 4 of Levi/Yllas RF I.C..
//  Register ClockValid10 0x0217, Bits 4:4, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_4_REQ_8_MASK 0x10
#define SYS_CLOCK_BUF_4_REQ_8_OFFSET 4
#define SYS_CLOCK_BUF_4_REQ_8_STOP_BIT 4
#define SYS_CLOCK_BUF_4_REQ_8_START_BIT 4
#define SYS_CLOCK_BUF_4_REQ_8_WIDTH 1

#define CLOCK_VALID_10_REG 0x217
#define SYS_CLOCK_BUF_4_REQ_8_PARAM_MASK  0x10

//  
//  0: SysClkReq8 is not associated to clock buffer 3 of Levi/Yllas RF I.C. 
//  1: SysClkReq8 is associated to clock buffer 3 of Levi/Yllas RF I.C..
//  Register ClockValid10 0x0217, Bits 3:3, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_3_REQ_8_MASK 0x8
#define SYS_CLOCK_BUF_3_REQ_8_OFFSET 3
#define SYS_CLOCK_BUF_3_REQ_8_STOP_BIT 3
#define SYS_CLOCK_BUF_3_REQ_8_START_BIT 3
#define SYS_CLOCK_BUF_3_REQ_8_WIDTH 1

#define SYS_CLOCK_BUF_3_REQ_8_PARAM_MASK  0x8

//  
//  0: SysClkReq8 is not associated to clock buffer 2 of Levi/Yllas RF I.C. 
//  1: SysClkReq8 is associated to clock buffer 2 of Levi/Yllas RF I.C..
//  Register ClockValid10 0x0217, Bits 2:2, typedef sys_clock_resquest
//   
//  
#define SYS_CLOCK_BUF_2_REQ_8_MASK 0x4
#define SYS_CLOCK_BUF_2_REQ_8_OFFSET 2
#define SYS_CLOCK_BUF_2_REQ_8_STOP_BIT 2
#define SYS_CLOCK_BUF_2_REQ_8_START_BIT 2
#define SYS_CLOCK_BUF_2_REQ_8_WIDTH 1

#define SYS_CLOCK_BUF_2_REQ_8_PARAM_MASK  0x4

//  
//  Dithering delay 2.5ns, 3ns 3.5ns 4ns.
//  Register DitherClkCtrl 0x0220, Bits 6:7, typedef dithering_delay
//   
//  
#define DITHERING_DELAY_MASK 0xC0
#define DITHERING_DELAY_OFFSET 7
#define DITHERING_DELAY_STOP_BIT 7
#define DITHERING_DELAY_START_BIT 6
#define DITHERING_DELAY_WIDTH 2

#define DITHER_CLK_CTRL_REG 0x220
#define  DITHERING_DELAY_2_5_NS 0
#define  DITHERING_DELAY_3_NS 1
#define  DITHERING_DELAY_3_5_NS 2
#define  DITHERING_DELAY_4_NS 3
#define DITHERING_DELAY_PARAM_MASK  0xC0



typedef enum dithering_delay {
    DITHERING_DELAY_2_5_NS_E,
    DITHERING_DELAY_3_NS_E,
    DITHERING_DELAY_3_5_NS_E,
    DITHERING_DELAY_4_NS_E
} DITHERING_DELAY_T ;




//  
//  VapeDitherEna.
//  Register DitherClkCtrl 0x0220, Bits 5:5, typedef vape_dither_enable
//   
//  
#define VAPE_DITHER_MASK 0x20
#define VAPE_DITHER_OFFSET 5
#define VAPE_DITHER_STOP_BIT 5
#define VAPE_DITHER_START_BIT 5
#define VAPE_DITHER_WIDTH 1


typedef enum vape_dither_enable {
    VAPE_DITHER_DISABLE_E,
    VAPE_DITHER_ENABLE_E
} VAPE_DITHER_ENABLE_T ;

#define VAPE_DITHER_ENABLE_PARAM_MASK  0x20

//  
//  VmodDitherEna.
//  Register DitherClkCtrl 0x0220, Bits 4:4, typedef vmod_dither_enable
//   
//  
#define VMOD_DITHER_MASK 0x10
#define VMOD_DITHER_OFFSET 4
#define VMOD_DITHER_STOP_BIT 4
#define VMOD_DITHER_START_BIT 4
#define VMOD_DITHER_WIDTH 1


typedef enum vmod_dither_enable {
    VMOD_DITHER_DISABLE_E,
    VMOD_DITHER_ENABLE_E
} VMOD_DITHER_ENABLE_T ;

#define VMOD_DITHER_ENABLE_PARAM_MASK  0x10

//  
//  Vsmps2DitherEna.
//  Register DitherClkCtrl 0x0220, Bits 3:3, typedef vsmps2_dither_enable
//   
//  
#define VSMPS_2_DITHER_MASK 0x8
#define VSMPS_2_DITHER_OFFSET 3
#define VSMPS_2_DITHER_STOP_BIT 3
#define VSMPS_2_DITHER_START_BIT 3
#define VSMPS_2_DITHER_WIDTH 1


typedef enum vsmps2_dither_enable {
    VSMPS_2_DITHER_DISABLE_E,
    VSMPS_2_DITHER_ENABLE_E
} VSMPS_2_DITHER_ENABLE_T ;

#define VSMPS_2_DITHER_ENABLE_PARAM_MASK  0x8

//  
//  Vsmps1DitherEna.
//  Register DitherClkCtrl 0x0220, Bits 2:2, typedef vsmps1_dither_enable
//   
//  
#define VSMPS_1_DITHER_MASK 0x4
#define VSMPS_1_DITHER_OFFSET 2
#define VSMPS_1_DITHER_STOP_BIT 2
#define VSMPS_1_DITHER_START_BIT 2
#define VSMPS_1_DITHER_WIDTH 1


typedef enum vsmps1_dither_enable {
    VSMPS_1_DITHER_DISABLE_E,
    VSMPS_1_DITHER_ENABLE_E
} VSMPS_1_DITHER_ENABLE_T ;

#define VSMPS_1_DITHER_ENABLE_PARAM_MASK  0x4

//  
//  Vsmps3DitherEna.
//  Register DitherClkCtrl 0x0220, Bits 1:1, typedef vsmps3_dither_enable
//   
//  
#define VSMPS_3_DITHER_MASK 0x2
#define VSMPS_3_DITHER_OFFSET 1
#define VSMPS_3_DITHER_STOP_BIT 1
#define VSMPS_3_DITHER_START_BIT 1
#define VSMPS_3_DITHER_WIDTH 1


typedef enum vsmps3_dither_enable {
    VSMPS_3_DITHER_DISABLE_E,
    VSMPS_3_DITHER_ENABLE_E
} VSMPS_3_DITHER_ENABLE_T ;

#define VSMPS_3_DITHER_ENABLE_PARAM_MASK  0x2

//  
//  VarmDitherEna.
//  Register DitherClkCtrl 0x0220, Bits 0:0, typedef varm_dither_enable
//   
//  
#define VARM_DITHER_MASK 0x1
#define VARM_DITHER_OFFSET 0
#define VARM_DITHER_STOP_BIT 0
#define VARM_DITHER_START_BIT 0
#define VARM_DITHER_WIDTH 1


typedef enum varm_dither_enable {
    VARM_DITHER_DISABLE_E,
    VARM_DITHER_ENABLE_E
} VARM_DITHER_ENABLE_T ;

#define VARM_DITHER_ENABLE_PARAM_MASK  0x1

//  
//  Set 'Swat[5]' (each updated value will be sent through Swat interface).
//  Register SwatCtrl 0x0230, Bits 6:6, typedef swat_bit5_set
//   
//  
#define SWAT_BIT_5_SET_MASK 0x40
#define SWAT_BIT_5_SET_OFFSET 6
#define SWAT_BIT_5_SET_STOP_BIT 6
#define SWAT_BIT_5_SET_START_BIT 6
#define SWAT_BIT_5_SET_WIDTH 1

#define SWAT_CTRL_REG 0x230

typedef enum swat_bit5_set {
    SWAT_BIT_5_RESET_SET_E,
    SWAT_BIT_5_SET_E
} SWAT_BIT_5_SET_T ;

#define SWAT_BIT_5_SET_PARAM_MASK  0x40

//  
//  Defined timer of RF turn off sequence, Timer = (RfOffTimer[2:0] +1) x periods of 
//  32kHz.
//  Register SwatCtrl 0x0230, Bits 2:4, typedef off_timer
//   
//  
#define RF_OFF_TIMER_MASK 0x1C
#define RF_OFF_TIMER_OFFSET 4
#define RF_OFF_TIMER_STOP_BIT 4
#define RF_OFF_TIMER_START_BIT 2
#define RF_OFF_TIMER_WIDTH 3

#define  TURN_OFF_1_PERIOD_OF_32KHZ 0
#define  TURN_OFF_2_PERIOD2_OF_32KHZ 1
#define  TURN_OFF_3_PERIOD2_OF_32KHZ 2
#define  TURN_OFF_4_PERIOD2_OF_32KHZ 3
#define  TURN_OFF_5_PERIOD2_OF_32KHZ 4
#define  TURN_OFF_6_PERIOD2_OF_32KHZ 5
#define  TURN_OFF_7_PERIOD2_OF_32KHZ 6
#define  TURN_OFF_8_PERIOD2_OF_32KHZ 7
#define OFF_TIMER_PARAM_MASK  0x1C



typedef enum off_timer {
    TURN_OFF_1_PERIOD_OF_32KHZ_E,
    TURN_OFF_2_PERIOD2_OF_32KHZ_E,
    TURN_OFF_3_PERIOD2_OF_32KHZ_E,
    TURN_OFF_4_PERIOD2_OF_32KHZ_E,
    TURN_OFF_5_PERIOD2_OF_32KHZ_E,
    TURN_OFF_6_PERIOD2_OF_32KHZ_E,
    TURN_OFF_7_PERIOD2_OF_32KHZ_E,
    TURN_OFF_8_PERIOD2_OF_32KHZ_E
} OFF_TIMER_T ;




//  
//  0: Disable Swat block 
//  1: Enable Swat block.
//  Register SwatCtrl 0x0230, Bits 1:1, typedef swat_enable
//   
//  
#define SWAT_ENABLE_MASK 0x2
#define SWAT_ENABLE_OFFSET 1
#define SWAT_ENABLE_STOP_BIT 1
#define SWAT_ENABLE_START_BIT 1
#define SWAT_ENABLE_WIDTH 1


typedef enum swat_enable {
    SWAT_DISABLE_E,
    SWAT_ENABLE_E
} SWAT_ENABLE_T ;

#define SWAT_ENABLE_PARAM_MASK  0x2

//  
//  0: RF I.C. is Levi (allow to configure Swat message) 
//  1: RF I.C. is Yllas (allow to configure Swat message).
//  Register SwatCtrl 0x0230, Bits 0:0, typedef levi_or_yllas
//   
//  
#define LEVI_OR_YLLAS_MASK 0x1
#define LEVI_OR_YLLAS_OFFSET 0
#define LEVI_OR_YLLAS_STOP_BIT 0
#define LEVI_OR_YLLAS_START_BIT 0
#define LEVI_OR_YLLAS_WIDTH 1

#define  LEVI_OR_YLLAS 1

typedef enum levi_or_yllas {
    RF_IC_LEVI_E,
    RF_IC_YLLAS_E
} LEVI_OR_YLLAS_T ;
#define LEVI_OR_YLLAS_PARAM_MASK  0x1



//  
//  0: doesn't valid SysClkReq8 input to control HiqClkEna ball 
//  1: valid SysClkReq8 input to control HiqClkEna ball.
//  Register HiqClkCtrl 0x0232, Bits 7:7, typedef sys_clk_req8_hiq_enable
//   
//  
#define SYS_CLK_REQ_8_HIQ_ENABLE_MASK 0x80
#define SYS_CLK_REQ_8_HIQ_ENABLE_OFFSET 7
#define SYS_CLK_REQ_8_HIQ_ENABLE_STOP_BIT 7
#define SYS_CLK_REQ_8_HIQ_ENABLE_START_BIT 7
#define SYS_CLK_REQ_8_HIQ_ENABLE_WIDTH 1

#define HIQ_CLK_CTRL_REG 0x232

typedef enum sys_clk_req8_hiq_enable {
    SYS_CLK_REQ_8_HIQ_DISABLE_E,
    SYS_CLK_REQ_8_HIQ_ENABLE_E
} SYS_CLK_REQ_8_HIQ_ENABLE_T ;

#define SYS_CLK_REQ_8_HIQ_ENABLE_PARAM_MASK  0x80

//  
//  0: doesn't valid SysClkReq7 input to control HiqClkEna ball 
//  1: valid SysClkReq7 input to control HiqClkEna ball.
//  Register HiqClkCtrl 0x0232, Bits 6:6, typedef sys_clk_req7_hiq_enable
//   
//  
#define SYS_CLK_REQ_7_HIQ_ENABLE_MASK 0x40
#define SYS_CLK_REQ_7_HIQ_ENABLE_OFFSET 6
#define SYS_CLK_REQ_7_HIQ_ENABLE_STOP_BIT 6
#define SYS_CLK_REQ_7_HIQ_ENABLE_START_BIT 6
#define SYS_CLK_REQ_7_HIQ_ENABLE_WIDTH 1


typedef enum sys_clk_req7_hiq_enable {
    SYS_CLK_REQ_7_HIQ_DISABLE_E,
    SYS_CLK_REQ_7_HIQ_ENABLE_E
} SYS_CLK_REQ_7_HIQ_ENABLE_T ;

#define SYS_CLK_REQ_7_HIQ_ENABLE_PARAM_MASK  0x40

//  
//  0: doesn't valid SysClkReq6 input to control HiqClkEna ball 
//  1: valid SysClkReq6 input to control HiqClkEna ball.
//  Register HiqClkCtrl 0x0232, Bits 5:5, typedef sys_clk_req6_hiq_enable
//   
//  
#define SYS_CLK_REQ_6_HIQ_ENABLE_MASK 0x20
#define SYS_CLK_REQ_6_HIQ_ENABLE_OFFSET 5
#define SYS_CLK_REQ_6_HIQ_ENABLE_STOP_BIT 5
#define SYS_CLK_REQ_6_HIQ_ENABLE_START_BIT 5
#define SYS_CLK_REQ_6_HIQ_ENABLE_WIDTH 1


typedef enum sys_clk_req6_hiq_enable {
    SYS_CLK_REQ_6_HIQ_DISABLE_E,
    SYS_CLK_REQ_6_HIQ_ENABLE_E
} SYS_CLK_REQ_6_HIQ_ENABLE_T ;

#define SYS_CLK_REQ_6_HIQ_ENABLE_PARAM_MASK  0x20

//  
//  0: doesn't valid SysClkReq5 input to control HiqClkEna ball 
//  1: valid SysClkReq5 input to control HiqClkEna ball.
//  Register HiqClkCtrl 0x0232, Bits 4:4, typedef sys_clk_req5_hiq_enable
//   
//  
#define SYS_CLK_REQ_5_HIQ_ENABLE_MASK 0x10
#define SYS_CLK_REQ_5_HIQ_ENABLE_OFFSET 4
#define SYS_CLK_REQ_5_HIQ_ENABLE_STOP_BIT 4
#define SYS_CLK_REQ_5_HIQ_ENABLE_START_BIT 4
#define SYS_CLK_REQ_5_HIQ_ENABLE_WIDTH 1


typedef enum sys_clk_req5_hiq_enable {
    SYS_CLK_REQ_5_HIQ_DISABLE_E,
    SYS_CLK_REQ_5_HIQ_ENABLE_E
} SYS_CLK_REQ_5_HIQ_ENABLE_T ;

#define SYS_CLK_REQ_5_HIQ_ENABLE_PARAM_MASK  0x10

//  
//  0: doesn't valid SysClkReq4 input to control HiqClkEna ball 
//  1: valid SysClkReq4 input to control HiqClkEna ball.
//  Register HiqClkCtrl 0x0232, Bits 3:3, typedef sys_clk_req4_hiq_enable
//   
//  
#define SYS_CLK_REQ_4_HIQ_ENABLE_MASK 0x8
#define SYS_CLK_REQ_4_HIQ_ENABLE_OFFSET 3
#define SYS_CLK_REQ_4_HIQ_ENABLE_STOP_BIT 3
#define SYS_CLK_REQ_4_HIQ_ENABLE_START_BIT 3
#define SYS_CLK_REQ_4_HIQ_ENABLE_WIDTH 1


typedef enum sys_clk_req4_hiq_enable {
    SYS_CLK_REQ_4_HIQ_DISABLE_E,
    SYS_CLK_REQ_4_HIQ_ENABLE_E
} SYS_CLK_REQ_4_HIQ_ENABLE_T ;

#define SYS_CLK_REQ_4_HIQ_ENABLE_PARAM_MASK  0x8

//  
//  0: doesn't valid SysClkReq3 input to control HiqClkEna ball 
//  1: valid SysClkReq3 input to control HiqClkEna ball.
//  Register HiqClkCtrl 0x0232, Bits 2:2, typedef sys_clk_req3_hiq_enable
//   
//  
#define SYS_CLK_REQ_3_HIQ_ENABLE_MASK 0x4
#define SYS_CLK_REQ_3_HIQ_ENABLE_OFFSET 2
#define SYS_CLK_REQ_3_HIQ_ENABLE_STOP_BIT 2
#define SYS_CLK_REQ_3_HIQ_ENABLE_START_BIT 2
#define SYS_CLK_REQ_3_HIQ_ENABLE_WIDTH 1


typedef enum sys_clk_req3_hiq_enable {
    SYS_CLK_REQ_3_HIQ_DISABLE_E,
    SYS_CLK_REQ_3_HIQ_ENABLE_E
} SYS_CLK_REQ_3_HIQ_ENABLE_T ;

#define SYS_CLK_REQ_3_HIQ_ENABLE_PARAM_MASK  0x4

//  
//  0: doesn't valid SysClkReq2 input to control HiqClkEna ball 
//  1: valid SysClkReq2 input to control HiqClkEna ball.
//  Register HiqClkCtrl 0x0232, Bits 1:1, typedef sys_clk_req2_hiq_enable
//   
//  
#define SYS_CLK_REQ_2_HIQ_ENABLE_MASK 0x2
#define SYS_CLK_REQ_2_HIQ_ENABLE_OFFSET 1
#define SYS_CLK_REQ_2_HIQ_ENABLE_STOP_BIT 1
#define SYS_CLK_REQ_2_HIQ_ENABLE_START_BIT 1
#define SYS_CLK_REQ_2_HIQ_ENABLE_WIDTH 1


typedef enum sys_clk_req2_hiq_enable {
    SYS_CLK_REQ_2_HIQ_DISABLE_E,
    SYS_CLK_REQ_2_HIQ_ENABLE_E
} SYS_CLK_REQ_2_HIQ_ENABLE_T ;

#define SYS_CLK_REQ_2_HIQ_ENABLE_PARAM_MASK  0x2

//  
//  0: doesn't valid SysClkReq1 input to control HiqClkEna ball 
//  1: valid SysClkReq1 input to control HiqClkEna ball.
//  Register HiqClkCtrl 0x0232, Bits 0:0, typedef sys_clk_req1_hiq_enable
//   
//  
#define SYS_CLK_REQ_1_HIQ_ENABLE_MASK 0x1
#define SYS_CLK_REQ_1_HIQ_ENABLE_OFFSET 0
#define SYS_CLK_REQ_1_HIQ_ENABLE_STOP_BIT 0
#define SYS_CLK_REQ_1_HIQ_ENABLE_START_BIT 0
#define SYS_CLK_REQ_1_HIQ_ENABLE_WIDTH 1


typedef enum sys_clk_req1_hiq_enable {
    SYS_CLK_REQ_1_HIQ_DISABLE_E,
    SYS_CLK_REQ_1_HIQ_ENABLE_E
} SYS_CLK_REQ_1_HIQ_ENABLE_T ;

#define SYS_CLK_REQ_1_HIQ_ENABLE_PARAM_MASK  0x1

//  
//  0: doesn't valid SysClkReq8 input to request Vsim ldo 
//  1: valid SysClkReq8 inputs to request Vsim ldo.
//  Register VsimSysClkCtrl 0x0233, Bits 7:7, typedef vsim_sys_clk_req8_hiq_enable
//   
//  
#define VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_MASK 0x80
#define VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_OFFSET 7
#define VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_STOP_BIT 7
#define VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_START_BIT 7
#define VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_WIDTH 1

#define VSIM_SYS_CLK_CTRL_REG 0x233

typedef enum vsim_sys_clk_req8_hiq_enable {
    VSIM_SYS_CLK_REQ_8_HIQ_DISABLE_E,
    VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_E
} VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_T ;

#define VSIM_SYS_CLK_REQ_8_HIQ_ENABLE_PARAM_MASK  0x80

//  
//  0: doesn't valid SysClkReq7 input to request Vsim ldo 
//  1: valid SysClkReq7 inputs to request Vsim ldo.
//  Register VsimSysClkCtrl 0x0233, Bits 6:6, typedef vsim_sys_clk_req7_hiq_enable
//   
//  
#define VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_MASK 0x40
#define VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_OFFSET 6
#define VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_STOP_BIT 6
#define VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_START_BIT 6
#define VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_WIDTH 1


typedef enum vsim_sys_clk_req7_hiq_enable {
    VSIM_SYS_CLK_REQ_7_HIQ_DISABLE_E,
    VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_E
} VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_T ;

#define VSIM_SYS_CLK_REQ_7_HIQ_ENABLE_PARAM_MASK  0x40

//  
//  0: doesn't valid SysClkReq6 input to request Vsim ldo 
//  1: valid SysClkReq6 inputs to request Vsim ldo.
//  Register VsimSysClkCtrl 0x0233, Bits 5:5, typedef vsim_sys_clk_req6_hiq_enable
//   
//  
#define VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_MASK 0x20
#define VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_OFFSET 5
#define VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_STOP_BIT 5
#define VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_START_BIT 5
#define VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_WIDTH 1


typedef enum vsim_sys_clk_req6_hiq_enable {
    VSIM_SYS_CLK_REQ_6_HIQ_DISABLE_E,
    VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_E
} VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_T ;

#define VSIM_SYS_CLK_REQ_6_HIQ_ENABLE_PARAM_MASK  0x20

//  
//  0: doesn't valid SysClkReq5 input to request Vsim ldo 
//  1: valid SysClkReq5 inputs to request Vsim ldo.
//  Register VsimSysClkCtrl 0x0233, Bits 4:4, typedef vsim_sys_clk_req5_hiq_enable
//   
//  
#define VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_MASK 0x10
#define VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_OFFSET 4
#define VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_STOP_BIT 4
#define VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_START_BIT 4
#define VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_WIDTH 1


typedef enum vsim_sys_clk_req5_hiq_enable {
    VSIM_SYS_CLK_REQ_5_HIQ_DISABLE_E,
    VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_E
} VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_T ;

#define VSIM_SYS_CLK_REQ_5_HIQ_ENABLE_PARAM_MASK  0x10

//  
//  0: doesn't valid SysClkReq4 input to request Vsim ldo 
//  1: valid SysClkReq4 inputs to request Vsim ldo.
//  Register VsimSysClkCtrl 0x0233, Bits 3:3, typedef vsim_sys_clk_req4_hiq_enable
//   
//  
#define VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_MASK 0x8
#define VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_OFFSET 3
#define VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_STOP_BIT 3
#define VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_START_BIT 3
#define VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_WIDTH 1


typedef enum vsim_sys_clk_req4_hiq_enable {
    VSIM_SYS_CLK_REQ_4_HIQ_DISABLE_E,
    VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_E
} VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_T ;

#define VSIM_SYS_CLK_REQ_4_HIQ_ENABLE_PARAM_MASK  0x8

//  
//  0: doesn't valid SysClkReq3 input to request Vsim ldo 
//  1: valid SysClkReq2 inputs to request Vsim ldo.
//  Register VsimSysClkCtrl 0x0233, Bits 2:2, typedef vsim_sys_clk_req3_hiq_enable
//   
//  
#define VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_MASK 0x4
#define VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_OFFSET 2
#define VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_STOP_BIT 2
#define VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_START_BIT 2
#define VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_WIDTH 1


typedef enum vsim_sys_clk_req3_hiq_enable {
    VSIM_SYS_CLK_REQ_3_HIQ_DISABLE_E,
    VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_E
} VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_T ;

#define VSIM_SYS_CLK_REQ_3_HIQ_ENABLE_PARAM_MASK  0x4

//  
//  0: doesn't valid SysClkReq2 input to request Vsim ldo 
//  1: valid SysClkReq2 inputs to request Vsim ldo.
//  Register VsimSysClkCtrl 0x0233, Bits 1:1, typedef vsim_sys_clk_req2_hiq_enable
//   
//  
#define VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_MASK 0x2
#define VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_OFFSET 1
#define VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_STOP_BIT 1
#define VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_START_BIT 1
#define VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_WIDTH 1


typedef enum vsim_sys_clk_req2_hiq_enable {
    VSIM_SYS_CLK_REQ_2_HIQ_DISABLE_E,
    VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_E
} VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_T ;

#define VSIM_SYS_CLK_REQ_2_HIQ_ENABLE_PARAM_MASK  0x2

//  
//  0: doesn't valid SysClkReq1 input to request Vsim ldo 
//  1: valid SysClkReq1 inputs to request Vsim ldo.
//  Register VsimSysClkCtrl 0x0233, Bits 0:0, typedef vsim_sys_clk_req1_hiq_enable
//   
//  
#define VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_MASK 0x1
#define VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_OFFSET 0
#define VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_STOP_BIT 0
#define VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_START_BIT 0
#define VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_WIDTH 1


typedef enum vsim_sys_clk_req1_hiq_enable {
    VSIM_SYS_CLK_REQ_1_HIQ_DISABLE_E,
    VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_E
} VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_T ;

#define VSIM_SYS_CLK_REQ_1_HIQ_ENABLE_PARAM_MASK  0x1
#endif
