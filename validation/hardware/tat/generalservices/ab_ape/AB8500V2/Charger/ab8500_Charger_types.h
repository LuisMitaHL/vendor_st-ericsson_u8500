/****************************************************************************
 * File : c:/Lisp/Production/ab8500_V2/ab8500_Charger/Linux/ab8500_Charger_types.h
 * 
 *
 * Generated on the 20/04/2011 10:26 by the 'gen-I2C' code generator 
 *
 * Generator written in Common Lisp, created by  Rémi PRUD'HOMME - ST 
 * with the help of : Grégory GOSCINIACK - ST, Irénée BROCHIER - Teamlog
 *
 ***************************************************************************
 *  Copyright STEricsson  2011
 *
 *  Reference document : UM0836 User manual, AB8500_V2 Mixed signal multimedia and power management Rev 1
 *
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB8500_V2_AB8500_CHARGER_LINUX_AB8500_CHARGER_TYPES_H
#define C__LISP_PRODUCTION_AB8500_V2_AB8500_CHARGER_LINUX_AB8500_CHARGER_TYPES_H





typedef enum current_limitation {
    CHARGER_CURRENT_LIMITATION_90_MA = 0x0,
    CHARGER_CURRENT_LIMITATION_290_MA = 0x1,
    CHARGER_CURRENT_LIMITATION_490_MA = 0x2,
    CHARGER_CURRENT_LIMITATION_690_MA = 0x3,
    CHARGER_CURRENT_LIMITATION_890_MA = 0x4,
    CHARGER_CURRENT_LIMITATION_1090_MA = 0x5,
    CHARGER_CURRENT_LIMITATION_1290_MA = 0x6,
    CHARGER_CURRENT_LIMITATION_1490_MA = 0x7
} CURRENT_LIMITATION_T ;



typedef enum voltage {
    VOLTAGE_BELOW_THRESHOLD_E,
    VOLTAGE_ABOVE_THRESHOLD_E
} VOLTAGE_T ;



typedef enum termal_status {
    TEMPERATURE_BELOW_THERMAL_THRESHOLD_E,
    TEMPERATURE_ABOVE_THERMAL_THRESHOLD_E
} TERMAL_STATUS_T ;



//  
//  Result of automatic input current limitation (to avoid Main voltage to drop). Input 
//  current limitation can be from 90mA to 1.3A by about 200mA steps..
//  Register MainChargerStatus 0x0B00, Bits 4:7, typedef current_limitation
//   
//  
#define MAIN_CURRENT_LIMITATION_MASK 0xF0
#define MAIN_CURRENT_LIMITATION_OFFSET 7
#define MAIN_CURRENT_LIMITATION_STOP_BIT 7
#define MAIN_CURRENT_LIMITATION_START_BIT 4
#define MAIN_CURRENT_LIMITATION_WIDTH 4

#define MAIN_CHARGER_STATUS_REG 0xB00
#define MAIN_CURRENT_LIMITATION_PARAM_MASK  0xF0
#define CURRENT_LIMITATION_PARAM_MASK  0xF0

//  
//  0: Charger is not in input current regulation 
//  1: Charger is in input current regulation.
//  Register MainChargerStatus 0x0B00, Bits 3:3, typedef current_regulation_enable
//   
//  
#define MAIN_CURRENT_REGULATION_MASK 0x8
#define MAIN_CURRENT_REGULATION_OFFSET 3
#define MAIN_CURRENT_REGULATION_STOP_BIT 3
#define MAIN_CURRENT_REGULATION_START_BIT 3
#define MAIN_CURRENT_REGULATION_WIDTH 1


typedef enum current_regulation_enable {
    CURRENT_REGULATION_DISABLE_E,
    CURRENT_REGULATION_ENABLE_E
} CURRENT_REGULATION_ENABLE_T ;

#define CURRENT_REGULATION_ENABLE_PARAM_MASK  0x8

//  
//  0: charger is not in voltage regulation 
//  1: charger is in voltage regulation.
//  Register MainChargerStatus 0x0B00, Bits 2:2, typedef voltage_regulation_enable
//   
//  
#define MAIN_VOLTAGE_REGULATION_MASK 0x4
#define MAIN_VOLTAGE_REGULATION_OFFSET 2
#define MAIN_VOLTAGE_REGULATION_STOP_BIT 2
#define MAIN_VOLTAGE_REGULATION_START_BIT 2
#define MAIN_VOLTAGE_REGULATION_WIDTH 1


typedef enum voltage_regulation_enable {
    VOLTAGE_REGULATION_DISABLE_E,
    VOLTAGE_REGULATION_ENABLE_E
} VOLTAGE_REGULATION_ENABLE_T ;

#define VOLTAGE_REGULATION_ENABLE_PARAM_MASK  0x4

//  
//  0: Main charger is off 
//  1: Main charger is on.
//  Register MainChargerStatus 0x0B00, Bits 1:1, typedef main_charge_on
//   
//  
#define MAIN_CHARGE_ON_MASK 0x2
#define MAIN_CHARGE_ON_OFFSET 1
#define MAIN_CHARGE_ON_STOP_BIT 1
#define MAIN_CHARGE_ON_START_BIT 1
#define MAIN_CHARGE_ON_WIDTH 1


typedef enum main_charge_on {
    MAIN_CHARGE_OFF_E,
    MAIN_CHARGE_ON_E
} MAIN_CHARGE_ON_T ;

#define MAIN_CHARGE_ON_PARAM_MASK  0x2

//  
//  0: MainCharger voltage below MainChDet threshold 
//  1: MainCharger voltage above MainChDet threshold.
//  Register MainChargerStatus 0x0B00, Bits 0:0, typedef voltage
//   
//  
#define MAIN_CHARGER_LEVEL_MASK 0x1
#define MAIN_CHARGER_LEVEL_OFFSET 0
#define MAIN_CHARGER_LEVEL_STOP_BIT 0
#define MAIN_CHARGER_LEVEL_START_BIT 0
#define MAIN_CHARGER_LEVEL_WIDTH 1

#define MAIN_CHARGER_LEVEL_PARAM_MASK  0x1
#define VOLTAGE_PARAM_MASK  0x1
#define MAIN_CHARGER_EVENT_REG 0xB01

//  
//  MainCh voltage drops when MainCharger input current is above Wall Charger current 
//  capability.
//  Register MainChargerEvent 0x0B01, Bits 7:7, typedef voltage_drop
//   
//  
#define VOLTAGE_DROP_MASK 0x80
#define VOLTAGE_DROP_OFFSET 7
#define VOLTAGE_DROP_STOP_BIT 7
#define VOLTAGE_DROP_START_BIT 7
#define VOLTAGE_DROP_WIDTH 1

#define VOLTAGE_DROP_PARAM_MASK  0x80

//  
//  0: MainCh is detected after debounce (mean's above ChOVVLow threshold) 
//  1: MainCh is detected after debounce (mean's above ChOVVLow threshold).
//  Register MainChargerEvent 0x0B01, Bits 6:6, typedef main_charger_detected
//   
//  
#define MAIN_CHARGER_DETECTED_MASK 0x40
#define MAIN_CHARGER_DETECTED_OFFSET 6
#define MAIN_CHARGER_DETECTED_STOP_BIT 6
#define MAIN_CHARGER_DETECTED_START_BIT 6
#define MAIN_CHARGER_DETECTED_WIDTH 1

#define MAIN_CHARGER_DETECTED_PARAM_MASK  0x40

//  
//  0: MainCh input voltage is below ChOVVHigh threshold 
//  1: MainCh input voltage is above ChOVVHigh threshold.
//  Register MainChargerEvent 0x0B01, Bits 5:5, typedef input_voltage_high_threshold
//   
//  
#define MAIN_CHARGER_INPUT_HIGH_VOLTAGE_MASK 0x20
#define MAIN_CHARGER_INPUT_HIGH_VOLTAGE_OFFSET 5
#define MAIN_CHARGER_INPUT_HIGH_VOLTAGE_STOP_BIT 5
#define MAIN_CHARGER_INPUT_HIGH_VOLTAGE_START_BIT 5
#define MAIN_CHARGER_INPUT_HIGH_VOLTAGE_WIDTH 1

#define INPUT_VOLTAGE_HIGH_THRESHOLD_PARAM_MASK  0x20

//  
//  0: MainCh input voltage is below ChOVVLow threshold 
//  1: MainCh input voltage is above ChOVVLow threshold.
//  Register MainChargerEvent 0x0B01, Bits 4:4, typedef input_voltage_low_threshold
//   
//  
#define MAIN_CHARGER_INPUT_LOW_VOLTAGE_MASK 0x10
#define MAIN_CHARGER_INPUT_LOW_VOLTAGE_OFFSET 4
#define MAIN_CHARGER_INPUT_LOW_VOLTAGE_STOP_BIT 4
#define MAIN_CHARGER_INPUT_LOW_VOLTAGE_START_BIT 4
#define MAIN_CHARGER_INPUT_LOW_VOLTAGE_WIDTH 1

#define INPUT_VOLTAGE_LOW_THRESHOLD_PARAM_MASK  0x10

//  
//  0: Die temperature below thermal threshold. 
//  1: Die temperature above thermal threshold. Charging is stopped..
//  Register MainChargerEvent 0x0B01, Bits 1:1, typedef termal_status
//   
//  
#define THERMAL_PROTECTION_MASK 0x2
#define THERMAL_PROTECTION_OFFSET 1
#define THERMAL_PROTECTION_STOP_BIT 1
#define THERMAL_PROTECTION_START_BIT 1
#define THERMAL_PROTECTION_WIDTH 1

#define THERMAL_PROTECTION_PARAM_MASK  0x2
#define TERMAL_STATUS_PARAM_MASK  0x2

//  
//  0: MainCharger voltage below MainChOVV threshold (10v). 
//  1: MainCharger voltage above MainChOVV threshold (10v). Overvoltage or sine wave 
//  charger detected. Charger is disabled..
//  Register MainChargerEvent 0x0B01, Bits 0:0, typedef voltage_status
//   
//  
#define VOLTAGE_STATUS_MASK 0x1
#define VOLTAGE_STATUS_OFFSET 0
#define VOLTAGE_STATUS_STOP_BIT 0
#define VOLTAGE_STATUS_START_BIT 0
#define VOLTAGE_STATUS_WIDTH 1

#define VOLTAGE_STATUS_PARAM_MASK  0x1
#define USB_CHARGER_STATUS_REG 0xB02

//  
//  0: Vbus doesn't drops close to battery voltage 
//  1: Vbus drops close to battery voltage.
//  Register UsbChargerStatus 0x0B02, Bits 6:6, typedef close_battery_voltage
//   
//  
#define VOLTAGE_CLOSE_BATTERY_VOLTAGE_MASK 0x40
#define VOLTAGE_CLOSE_BATTERY_VOLTAGE_OFFSET 6
#define VOLTAGE_CLOSE_BATTERY_VOLTAGE_STOP_BIT 6
#define VOLTAGE_CLOSE_BATTERY_VOLTAGE_START_BIT 6
#define VOLTAGE_CLOSE_BATTERY_VOLTAGE_WIDTH 1

#define CLOSE_BATTERY_VOLTAGE_PARAM_MASK  0x40

//  
//  0: charger is not in voltage regulation 
//  1: charger is in voltage regulation.
//  Register UsbChargerStatus 0x0B02, Bits 3:3, typedef voltage_regulation
//   
//  
#define VOLTAGE_REGULATION_MASK 0x8
#define VOLTAGE_REGULATION_OFFSET 3
#define VOLTAGE_REGULATION_STOP_BIT 3
#define VOLTAGE_REGULATION_START_BIT 3
#define VOLTAGE_REGULATION_WIDTH 1

#define VOLTAGE_REGULATION_PARAM_MASK  0x8

//  
//  0: UsbCharger is off 
//  1: Usbcharger is on.
//  Register UsbChargerStatus 0x0B02, Bits 2:2, typedef usb_charger_on
//   
//  
#define USB_CHARGER_ON_MASK 0x4
#define USB_CHARGER_ON_OFFSET 2
#define USB_CHARGER_ON_STOP_BIT 2
#define USB_CHARGER_ON_START_BIT 2
#define USB_CHARGER_ON_WIDTH 1

#define USB_CHARGER_ON_PARAM_MASK  0x4

//  
//  0: VbusDetDbnc is low more than 100ms or VbusChNOK is low 
//  1: VbusDetDbnc is high more than 100ms (usb charger can be enabled).
//  Register UsbChargerStatus 0x0B02, Bits 1:1, typedef usb_charger_can_be_enable
//   
//  
#define VBUS_DETECTION_DEBOUCE_100_MS_MASK 0x2
#define VBUS_DETECTION_DEBOUCE_100_MS_OFFSET 1
#define VBUS_DETECTION_DEBOUCE_100_MS_STOP_BIT 1
#define VBUS_DETECTION_DEBOUCE_100_MS_START_BIT 1
#define VBUS_DETECTION_DEBOUCE_100_MS_WIDTH 1

#define USB_CHARGER_CAN_BE_ENABLE_PARAM_MASK  0x2

//  
//  0: Vbus voltage is below UsbChDetLow threshold more than 1ms (usb charger is 
//  disabled) 
//  1: Vbus voltage is above UsbChDetHigh threshold more than 1ms.
//  Register UsbChargerStatus 0x0B02, Bits 0:0, typedef vbus_detection_debouce1_ms
//   
//  
#define VBUS_DETECTION_DEBOUCE_1_MS_MASK 0x1
#define VBUS_DETECTION_DEBOUCE_1_MS_OFFSET 0
#define VBUS_DETECTION_DEBOUCE_1_MS_STOP_BIT 0
#define VBUS_DETECTION_DEBOUCE_1_MS_START_BIT 0
#define VBUS_DETECTION_DEBOUCE_1_MS_WIDTH 1

#define VBUS_DETECTION_DEBOUCE_1_MS_PARAM_MASK  0x1
#define USB_CHARGER_STATUS_2_REG 0xB03

//  
//  Result of automatic input current limitation (to avoid to Vbus voltage to drop). 
//  Input current limitation can be from 90mA to 1.3A by about 200mA steps.
//  Register UsbChargerStatus2 0x0B03, Bits 4:7, typedef current_limitation
//   
//  
#define AUTOMATIC_USB_CHARGER_CURRENT_MASK 0xF0
#define AUTOMATIC_USB_CHARGER_CURRENT_OFFSET 7
#define AUTOMATIC_USB_CHARGER_CURRENT_STOP_BIT 7
#define AUTOMATIC_USB_CHARGER_CURRENT_START_BIT 4
#define AUTOMATIC_USB_CHARGER_CURRENT_WIDTH 4

#define AUTOMATIC_USB_CHARGER_CURRENT_PARAM_MASK  0xF0

//  
//  0: USB device connected to STw4500 is not "illegal" (does not include VbusOVV) 
//  1: USB device connected to STw4500 is "illegal" (does not include VbusOVV).
//  Register UsbChargerStatus2 0x0B03, Bits 3:3, typedef usb_device_illegal
//   
//  
#define USB_DEVICE_ILLEGAL_MASK 0x8
#define USB_DEVICE_ILLEGAL_OFFSET 3
#define USB_DEVICE_ILLEGAL_STOP_BIT 3
#define USB_DEVICE_ILLEGAL_START_BIT 3
#define USB_DEVICE_ILLEGAL_WIDTH 1

#define  USB_DEVICE_ILLEGAL 8

typedef enum usb_device_illegal {
    USB_DEVICE_LEGAL_E,
    USB_DEVICE_ILLEGAL_E
} USB_DEVICE_ILLEGAL_T ;
#define USB_DEVICE_ILLEGAL_PARAM_MASK  0x8



//  
//  0: Die temperature is below thermal threshold. 
//  1: Die temperature is above thermal threshold. Charger is disabled..
//  Register UsbChargerStatus2 0x0B03, Bits 1:1, typedef termal_status
//   
//  
#define USB_THERMAL_PROTECTION_MASK 0x2
#define USB_THERMAL_PROTECTION_OFFSET 1
#define USB_THERMAL_PROTECTION_STOP_BIT 1
#define USB_THERMAL_PROTECTION_START_BIT 1
#define USB_THERMAL_PROTECTION_WIDTH 1

#define USB_THERMAL_PROTECTION_PARAM_MASK  0x2

//  
//  0: Vbus voltage below VbusOVV threshold (6.3v). 
//  1: Vbus voltage above VbusOVV threshold (6.3v). Charger is disabled.
//  Register UsbChargerStatus2 0x0B03, Bits 0:0, typedef voltage
//   
//  
#define VBUS_PROTECTION_MASK 0x1
#define VBUS_PROTECTION_OFFSET 0
#define VBUS_PROTECTION_STOP_BIT 0
#define VBUS_PROTECTION_START_BIT 0
#define VBUS_PROTECTION_WIDTH 1

#define VBUS_PROTECTION_PARAM_MASK  0x1
#define USB_CHARGER_STATUS_3_REG 0xB05

//  
//  0: indicated that Charging is not stopped by Btemp comparator 
//  1: indicated that Charging is stopped by Btemp comparator.
//  Register UsbChargerStatus3 0x0B05, Bits 3:3, typedef charging_stopped
//   
//  
#define CHARGING_STOPPED_MASK 0x8
#define CHARGING_STOPPED_OFFSET 3
#define CHARGING_STOPPED_STOP_BIT 3
#define CHARGING_STOPPED_START_BIT 3
#define CHARGING_STOPPED_WIDTH 1

#define CHARGING_STOPPED_PARAM_MASK  0x8

//  
//  Btemp comparator status (BtempLow threshold = typical -7°C, BtempMedium 
//  typical 3°C, BtempHigh threshold = typical 52°C or 57°C or 62°C) 
//  00: Btemp < BtempLow 
//  01: BtempLow < Btemp < BtempMedium 
//  10: BtempMedium < Btemp < BtempHigh 
//  11: BtempHigh < Btemp.
//  Register UsbChargerStatus3 0x0B05, Bits 1:2, typedef battery_temperature
//   
//  
#define BATTERY_TEMPERATURE_MASK 0x6
#define BATTERY_TEMPERATURE_OFFSET 2
#define BATTERY_TEMPERATURE_STOP_BIT 2
#define BATTERY_TEMPERATURE_START_BIT 1
#define BATTERY_TEMPERATURE_WIDTH 2

#define  UNDER_LOW 0
#define  UNDER_MEDIUM_LOW 1
#define  UNDER_MEDIUM_HIGH 2
#define  UNDER_HIGH 3
#define BATTERY_TEMPERATURE_PARAM_MASK  0x6



typedef enum battery_temperature {
    UNDER_LOW_E,
    UNDER_MEDIUM_LOW_E,
    UNDER_MEDIUM_HIGH_E,
    UNDER_HIGH_E
} BATTERY_TEMPERATURE_T ;




//  
//  0: Battery voltage is below BatOVV threshold. 
//  1: Battery voltage is above BatOVV threshold. Charger (Main or USB) is disabled when 
//  BattOVV occurs. BattOVV default threshold is 3.7v. 4.75v threshold is selected at 
//  first primary watchdog kick.
//  Register UsbChargerStatus3 0x0B05, Bits 0:0, typedef voltage
//   
//  
#define VOLTAGE_BELOW_THRESHOLD_MASK 0x1
#define VOLTAGE_BELOW_THRESHOLD_OFFSET 0
#define VOLTAGE_BELOW_THRESHOLD_STOP_BIT 0
#define VOLTAGE_BELOW_THRESHOLD_START_BIT 0
#define VOLTAGE_BELOW_THRESHOLD_WIDTH 1

#define VOLTAGE_BELOW_THRESHOLD_PARAM_MASK  0x1
typedef enum volt_level {
    CHARGER_VOLT_LEVEL_3_POINT_500_V = 0x0,
    CHARGER_VOLT_LEVEL_3_POINT_525_V = 0x1,
    CHARGER_VOLT_LEVEL_3_POINT_550_V = 0x2,
    CHARGER_VOLT_LEVEL_3_POINT_575_V = 0x3,
    CHARGER_VOLT_LEVEL_3_POINT_600_V = 0x4,
    CHARGER_VOLT_LEVEL_3_POINT_625_V = 0x5,
    CHARGER_VOLT_LEVEL_3_POINT_650_V = 0x6,
    CHARGER_VOLT_LEVEL_3_POINT_675_V = 0x7,
    CHARGER_VOLT_LEVEL_3_POINT_700_V = 0x8,
    CHARGER_VOLT_LEVEL_3_POINT_725_V = 0x9,
    CHARGER_VOLT_LEVEL_3_POINT_750_V = 0xA,
    CHARGER_VOLT_LEVEL_3_POINT_775_V = 0xB,
    CHARGER_VOLT_LEVEL_3_POINT_800_V = 0xC,
    CHARGER_VOLT_LEVEL_3_POINT_825_V = 0xD,
    CHARGER_VOLT_LEVEL_3_POINT_850_V = 0xE,
    CHARGER_VOLT_LEVEL_3_POINT_875_V = 0xF,
    CHARGER_VOLT_LEVEL_3_POINT_900_V = 0x10,
    CHARGER_VOLT_LEVEL_3_POINT_925_V = 0x11,
    CHARGER_VOLT_LEVEL_3_POINT_950_V = 0x12,
    CHARGER_VOLT_LEVEL_3_POINT_975_V = 0x13,
    CHARGER_VOLT_LEVEL_4_V = 0x14,
    CHARGER_VOLT_LEVEL_4_POINT_025_V = 0x15,
    CHARGER_VOLT_LEVEL_4_POINT_050_V = 0x16,
    CHARGER_VOLT_LEVEL_4_POINT_060_V = 0x17,
    CHARGER_VOLT_LEVEL_4_POINT_070_V = 0x18,
    CHARGER_VOLT_LEVEL_4_POINT_080_V = 0x19,
    CHARGER_VOLT_LEVEL_4_POINT_090_V = 0x1A,
    CHARGER_VOLT_LEVEL_4_POINT_100_V = 0x1B,
    CHARGER_VOLT_LEVEL_4_POINT_110_V = 0x1C,
    CHARGER_VOLT_LEVEL_4_POINT_120_V = 0x1D,
    CHARGER_VOLT_LEVEL_4_POINT_130_V = 0x1E,
    CHARGER_VOLT_LEVEL_4_POINT_140_V = 0x1F,
    CHARGER_VOLT_LEVEL_4_POINT_150_V = 0x20,
    CHARGER_VOLT_LEVEL_4_POINT_160_V = 0x21,
    CHARGER_VOLT_LEVEL_4_POINT_170_V = 0x22,
    CHARGER_VOLT_LEVEL_4_POINT_180_V = 0x23,
    CHARGER_VOLT_LEVEL_4_POINT_190_V = 0x24,
    CHARGER_VOLT_LEVEL_4_POINT_200_V = 0x25,
    CHARGER_VOLT_LEVEL_4_POINT_210_V = 0x26,
    CHARGER_VOLT_LEVEL_4_POINT_220_V = 0x27,
    CHARGER_VOLT_LEVEL_4_POINT_230_V = 0x28,
    CHARGER_VOLT_LEVEL_4_POINT_240_V = 0x29,
    CHARGER_VOLT_LEVEL_4_POINT_250_V = 0x2A,
    CHARGER_VOLT_LEVEL_4_POINT_260_V = 0x2B,
    CHARGER_VOLT_LEVEL_4_POINT_270_V = 0x2C,
    CHARGER_VOLT_LEVEL_4_POINT_280_V = 0x2D,
    CHARGER_VOLT_LEVEL_4_POINT_290_V = 0x2E,
    CHARGER_VOLT_LEVEL_4_POINT_300_V = 0x2F,
    CHARGER_VOLT_LEVEL_4_POINT_310_V = 0x30,
    CHARGER_VOLT_LEVEL_4_POINT_320_V = 0x31,
    CHARGER_VOLT_LEVEL_4_POINT_330_V = 0x32,
    CHARGER_VOLT_LEVEL_4_POINT_340_V = 0x33,
    CHARGER_VOLT_LEVEL_4_POINT_350_V = 0x34,
    CHARGER_VOLT_LEVEL_4_POINT_360_V = 0x35,
    CHARGER_VOLT_LEVEL_4_POINT_370_V = 0x36,
    CHARGER_VOLT_LEVEL_4_POINT_380_V = 0x37,
    CHARGER_VOLT_LEVEL_4_POINT_390_V = 0x38,
    CHARGER_VOLT_LEVEL_4_POINT_400_V = 0x39,
    CHARGER_VOLT_LEVEL_4_POINT_410_V = 0x3A,
    CHARGER_VOLT_LEVEL_4_POINT_420_V = 0x3B,
    CHARGER_VOLT_LEVEL_4_POINT_430_V = 0x3C,
    CHARGER_VOLT_LEVEL_4_POINT_440_V = 0x3D,
    CHARGER_VOLT_LEVEL_4_POINT_450_V = 0x3E,
    CHARGER_VOLT_LEVEL_4_POINT_460_V = 0x3F,
    CHARGER_VOLT_LEVEL_4_POINT_470_V = 0x40,
    CHARGER_VOLT_LEVEL_4_POINT_480_V = 0x41,
    CHARGER_VOLT_LEVEL_4_POINT_490_V = 0x42,
    CHARGER_VOLT_LEVEL_4_POINT_500_V = 0x43,
    CHARGER_VOLT_LEVEL_4_POINT_510_V = 0x44,
    CHARGER_VOLT_LEVEL_4_POINT_520_V = 0x45,
    CHARGER_VOLT_LEVEL_4_POINT_530_V = 0x46,
    CHARGER_VOLT_LEVEL_4_POINT_540_V = 0x47,
    CHARGER_VOLT_LEVEL_4_POINT_550_V = 0x48,
    CHARGER_VOLT_LEVEL_4_POINT_560_V = 0x49,
    CHARGER_VOLT_LEVEL_4_POINT_570_V = 0x4A,
    CHARGER_VOLT_LEVEL_4_POINT_580_V = 0x4B,
    CHARGER_VOLT_LEVEL_4_POINT_590_V = 0x4C,
    CHARGER_VOLT_LEVEL_4_POINT_600_V = 0x4D
} VOLT_LEVEL_T ;



//  
//  Common to Main and USB charger.
//  Register ChVoltLevel 0x0B40, Bits 0:6, typedef volt_level
//   
//  
#define CH_VOLT_LEVEL_MASK 0x7F
#define CH_VOLT_LEVEL_OFFSET 6
#define CH_VOLT_LEVEL_STOP_BIT 6
#define CH_VOLT_LEVEL_START_BIT 0
#define CH_VOLT_LEVEL_WIDTH 7

#define CH_VOLT_LEVEL_REG 0xB40
#define CH_VOLT_LEVEL_PARAM_MASK  0x7F
#define VOLT_LEVEL_PARAM_MASK  0x7F
typedef enum charger_current_level {
    CURRENT_LEVEL_100_MA = 0x0,
    CURRENT_LEVEL_200_MA = 0x1,
    CURRENT_LEVEL_300_MA = 0x2,
    CURRENT_LEVEL_400_MA = 0x3,
    CURRENT_LEVEL_500_MA = 0x4,
    CURRENT_LEVEL_600_MA = 0x5,
    CURRENT_LEVEL_700_MA = 0x6,
    CURRENT_LEVEL_800_MA = 0x7,
    CURRENT_LEVEL_900_MA = 0x8,
    CURRENT_LEVEL_1000_MA = 0x9,
    CURRENT_LEVEL_1100_MA = 0xA,
    CURRENT_LEVEL_1200_MA = 0xB,
    CURRENT_LEVEL_1300_MA = 0xC,
    CURRENT_LEVEL_1400_MA = 0xD,
    CURRENT_LEVEL_1500_MA = 0xE
} CHARGER_CURRENT_LEVEL_T ;



//  
//  Defines maximum charging voltage level. Can be written only once.
//  Register ChargerVoltLevelMax 0x0B41, Bits 0:6, typedef charging_voltage_level
//   
//  
#define MAXIMUM_CHARGING_VOLTAGE_LEVEL_MASK 0x7F
#define MAXIMUM_CHARGING_VOLTAGE_LEVEL_OFFSET 6
#define MAXIMUM_CHARGING_VOLTAGE_LEVEL_STOP_BIT 6
#define MAXIMUM_CHARGING_VOLTAGE_LEVEL_START_BIT 0
#define MAXIMUM_CHARGING_VOLTAGE_LEVEL_WIDTH 7

#define CHARGER_VOLT_LEVEL_MAX_REG 0xB41
#define CHARGING_VOLTAGE_LEVEL_PARAM_MASK  0x7F

//  
//  MainChCurrLevel.
//  Register ChannelOutputCurrentLevel 0x0B42, Bits 0:3, typedef charger_current_level
//   
//  
#define CHANNEL_OUTPUT_CURRENT_LEVEL_MASK 0xF
#define CHANNEL_OUTPUT_CURRENT_LEVEL_OFFSET 3
#define CHANNEL_OUTPUT_CURRENT_LEVEL_STOP_BIT 3
#define CHANNEL_OUTPUT_CURRENT_LEVEL_START_BIT 0
#define CHANNEL_OUTPUT_CURRENT_LEVEL_WIDTH 4

#define CHANNEL_OUTPUT_CURRENT_LEVEL_REG 0xB42
#define CHANNEL_OUTPUT_CURRENT_LEVEL_PARAM_MASK  0xF
#define CHARGER_CURRENT_LEVEL_PARAM_MASK  0xF

//  
//  Defines maximum charging current level. Can be written only once.
//  Register MaximumChargingCurrent 0x0B43, Bits 0:3, typedef maximum_charging_current
//   
//  
#define MAXIMUM_CHARGING_CURRENT_MASK 0xF
#define MAXIMUM_CHARGING_CURRENT_OFFSET 3
#define MAXIMUM_CHARGING_CURRENT_STOP_BIT 3
#define MAXIMUM_CHARGING_CURRENT_START_BIT 0
#define MAXIMUM_CHARGING_CURRENT_WIDTH 4

#define MAXIMUM_CHARGING_CURRENT_REG 0xB43
#define MAXIMUM_CHARGING_CURRENT_PARAM_MASK  0xF

//  
//  When Btemp is above Low and below Medium temperature threshold: 
//  0: Charging current is defined by OTP bits. 
//  1: Charging current is set at 300mA (typ) regardless OTP bits.
//  Register ChannelOutputCurrentLevelBehavor 0x0B44, Bits 0:0, typedef charging_current
//   
//  
#define CHARGING_CURRENT_BEHAVOR_MASK 0x1
#define CHARGING_CURRENT_BEHAVOR_OFFSET 0
#define CHARGING_CURRENT_BEHAVOR_STOP_BIT 0
#define CHARGING_CURRENT_BEHAVOR_START_BIT 0
#define CHARGING_CURRENT_BEHAVOR_WIDTH 1

#define CHANNEL_OUTPUT_CURRENT_LEVEL_BEHAVOR_REG 0xB44
#define  CHARGING_CURRENT 1

typedef enum charging_current {
    CHARGING_CURRENT_DEFINED_BY_OTP_E,
    CHARGING_CURRENT_300_MA_E
} CHARGING_CURRENT_T ;
#define CHARGING_CURRENT_PARAM_MASK  0x1


#define CHARGER_WATCH_DOG_TIMER_REG 0xB50

//  
//  9° bit of ChargerWatchDogTimer 0-7 at 0xB50.
//  Register ChargerWatchDogControl 0x0B51, Bits 1:1, typedef charger_watch_dog_timer9
//   
//  
#define CHARGER_WATCH_DOG_TIMER_9_MASK 0x2
#define CHARGER_WATCH_DOG_TIMER_9_OFFSET 1
#define CHARGER_WATCH_DOG_TIMER_9_STOP_BIT 1
#define CHARGER_WATCH_DOG_TIMER_9_START_BIT 1
#define CHARGER_WATCH_DOG_TIMER_9_WIDTH 1

#define CHARGER_WATCH_DOG_CONTROL_REG 0xB51
#define CHARGER_WATCH_DOG_TIMER_9_PARAM_MASK  0x2

//  
//  0: inactive 
//  1: Kick Watchdog timer (re-start timer). Auto-cleared (cleared once timer setting 
//  done)..
//  Register ChargerWatchDogControl 0x0B51, Bits 0:0, typedef kick_watchdog_timer_enable
//   
//  
#define KICK_WATCHDOG_TIMER_ENABLE_MASK 0x1
#define KICK_WATCHDOG_TIMER_ENABLE_OFFSET 0
#define KICK_WATCHDOG_TIMER_ENABLE_STOP_BIT 0
#define KICK_WATCHDOG_TIMER_ENABLE_START_BIT 0
#define KICK_WATCHDOG_TIMER_ENABLE_WIDTH 1


typedef enum kick_watchdog_timer_enable {
    KICK_WATCHDOG_TIMER_DISABLE_E,
    KICK_WATCHDOG_TIMER_ENABLE_E
} KICK_WATCHDOG_TIMER_ENABLE_T ;

#define KICK_WATCHDOG_TIMER_ENABLE_PARAM_MASK  0x1

//  
//  Defined Btemp high threshold: 
//  00: 57°c (typ) 
//  01: 52°c (typ) 
//  10: 57°c (typ) 
//  11: 62°c (typ).
//  Register ChargerTemperatureHigh 0x0B52, Bits 0:1, typedef temperature_high_threshold
//   
//  
#define CHARGER_TEMPERATURE_HIGH_MASK 0x3
#define CHARGER_TEMPERATURE_HIGH_OFFSET 1
#define CHARGER_TEMPERATURE_HIGH_STOP_BIT 1
#define CHARGER_TEMPERATURE_HIGH_START_BIT 0
#define CHARGER_TEMPERATURE_HIGH_WIDTH 2

#define CHARGER_TEMPERATURE_HIGH_REG 0xB52
#define  TEMPERATURE_HIGH_57_CELSIUS 0
#define  TEMPERATURE_HIGH_52_CELSIUS 1
#define  TEMPERATURE_HIGH_57_CELSIUS_1 2
#define  TEMPERATURE_HIGH_62_CELSIUS 3
#define TEMPERATURE_HIGH_THRESHOLD_PARAM_MASK  0x3



typedef enum temperature_high_threshold {
    TEMPERATURE_HIGH_57_CELSIUS_E,
    TEMPERATURE_HIGH_52_CELSIUS_E,
    TEMPERATURE_HIGH_57_CELSIUS_1_E,
    TEMPERATURE_HIGH_62_CELSIUS_E
} TEMPERATURE_HIGH_THRESHOLD_T ;




//  
//  .
//  Register LedIndicatorPwmControl 0x0B53, Bits 1:2, typedef led_current
//   
//  
#define CHARGER_LED_CURRENT_MASK 0x6
#define CHARGER_LED_CURRENT_OFFSET 2
#define CHARGER_LED_CURRENT_STOP_BIT 2
#define CHARGER_LED_CURRENT_START_BIT 1
#define CHARGER_LED_CURRENT_WIDTH 2

#define LED_INDICATOR_PWM_CONTROL_REG 0xB53
#define  LED_CURRENT_2_5_MA 0
#define  LED_CURRENT_1_MA 1
#define  LED_CURRENT_5_MA 2
#define  LED_CURRENT_10_MA 3
#define LED_CURRENT_PARAM_MASK  0x6



typedef enum led_current {
    LED_CURRENT_2_5_MA_E,
    LED_CURRENT_1_MA_E,
    LED_CURRENT_5_MA_E,
    LED_CURRENT_10_MA_E
} LED_CURRENT_T ;




//  
//  0: disable LedIndicator PWM generator 
//  1: enable LedIndicator PWM generator.
//  Register LedIndicatorPwmControl 0x0B53, Bits 0:0, typedef led_indicator_enable
//   
//  
#define LED_INDICATOR_ENABLE_MASK 0x1
#define LED_INDICATOR_ENABLE_OFFSET 0
#define LED_INDICATOR_ENABLE_STOP_BIT 0
#define LED_INDICATOR_ENABLE_START_BIT 0
#define LED_INDICATOR_ENABLE_WIDTH 1


typedef enum led_indicator_enable {
    LED_INDICATOR_DISABLE_E,
    LED_INDICATOR_ENABLE_E
} LED_INDICATOR_ENABLE_T ;

#define LED_INDICATOR_ENABLE_PARAM_MASK  0x1
#define LED_INDICATOR_DUTY_REG 0xB54

//  
//  0: clock charger dithering disabled 
//  1: clock charger dithering enabled.
//  Register BatteryThresholdControl 0x0B55, Bits 2:2, typedef 
//  clock_charger_dithering_enabled
//   
//  
#define CHARGER_DITHERING_ENABLE_MASK 0x4
#define CHARGER_DITHERING_ENABLE_OFFSET 2
#define CHARGER_DITHERING_ENABLE_STOP_BIT 2
#define CHARGER_DITHERING_ENABLE_START_BIT 2
#define CHARGER_DITHERING_ENABLE_WIDTH 1

#define BATTERY_THRESHOLD_CONTROL_REG 0xB55

typedef enum clock_charger_dithering_enabled {
    CLOCK_CHARGER_DITHERING_DISABLE_E,
    CLOCK_CHARGER_DITHERING_ENABLE_E
} CLOCK_CHARGER_DITHERING_ENABLED_T ;

#define CLOCK_CHARGER_DITHERING_ENABLED_PARAM_MASK  0x4

//  
//  can be enabled by charger state machine or by Sw, default value depends if a charger 
//  is plug in.
//  Register BatteryThresholdControl 0x0B55, Bits 1:1, typedef battery_o_v_v_enable
//   
//  
#define BATTERY_OVV_ENABLE_MASK 0x2
#define BATTERY_OVV_ENABLE_OFFSET 1
#define BATTERY_OVV_ENABLE_STOP_BIT 1
#define BATTERY_OVV_ENABLE_START_BIT 1
#define BATTERY_OVV_ENABLE_WIDTH 1


typedef enum battery_o_v_v_enable {
    BATTERY_OVV_DISABLE_E,
    BATTERY_OVV_ENABLE_E
} BATTERY_OVV_ENABLE_T ;

#define BATTERY_OVV_ENABLE_PARAM_MASK  0x2

//  
//  0: BattOVV threshold = 3.7v (typ) 
//  1: BattOVV threshold = 4.75v (typ).
//  Register BatteryThresholdControl 0x0B55, Bits 0:0, typedef battery_threshold
//   
//  
#define BATTERY_THRESHOLD_MASK 0x1
#define BATTERY_THRESHOLD_OFFSET 0
#define BATTERY_THRESHOLD_STOP_BIT 0
#define BATTERY_THRESHOLD_START_BIT 0
#define BATTERY_THRESHOLD_WIDTH 1

#define  BATTERY_THRESHOLD 1

typedef enum battery_threshold {
    BATTERY_THRESHOLD_3_70_E,
    BATTERY_THRESHOLD_4_75_E
} BATTERY_THRESHOLD_T ;
#define BATTERY_THRESHOLD_PARAM_MASK  0x1



//  
//  0: reset following registers (#) 
//  1: inactive.
//  Register ChargerControl 0x0B56, Bits 0:0, typedef reset_charger_enable
//   
//  
#define RESET_CHARGER_MASK 0x1
#define RESET_CHARGER_OFFSET 0
#define RESET_CHARGER_STOP_BIT 0
#define RESET_CHARGER_START_BIT 0
#define RESET_CHARGER_WIDTH 1

#define CHARGER_CONTROL_REG 0xB56

typedef enum reset_charger_enable {
    RESET_E,
    UN_RESET_E
} RESET_CHARGER_ENABLE_T ;

#define RESET_CHARGER_ENABLE_PARAM_MASK  0x1

typedef enum linear_mode {
    LINEAR_MODE_ENABLE_E,
    LINEAR_MODE_DISABLE_E
} LINEAR_MODE_T ;


#define  THERMAL_THRESHOLD_150 0
#define  THERMAL_THRESHOLD_159 1
#define  THERMAL_THRESHOLD_168 2
#define  THERMAL_THRESHOLD_140 3



typedef enum thermal_threshold {
    THERMAL_THRESHOLD_150_E,
    THERMAL_THRESHOLD_159_E,
    THERMAL_THRESHOLD_168_E,
    THERMAL_THRESHOLD_140_E
} THERMAL_THRESHOLD_T ;




//  
//  0: enable Vbat overshoot control 
//  1: disable Vbat overshoot control.
//  Register MainChargerControl 0x0B80, Bits 4:4, typedef vbat_overshoot_enable
//   
//  
#define VBAT_OVERSHOOT_CONTROL_MASK 0x10
#define VBAT_OVERSHOOT_CONTROL_OFFSET 4
#define VBAT_OVERSHOOT_CONTROL_STOP_BIT 4
#define VBAT_OVERSHOOT_CONTROL_START_BIT 4
#define VBAT_OVERSHOOT_CONTROL_WIDTH 1

#define MAIN_CHARGER_CONTROL_REG 0xB80

typedef enum vbat_overshoot_enable {
    VBAT_OVERSHOOT_ENABLE_E,
    VBAT_OVERSHOOT_DISABLE_E
} VBAT_OVERSHOOT_ENABLE_T ;

#define VBAT_OVERSHOOT_ENABLE_PARAM_MASK  0x10

//  
//  0: Disable MainCharger 
//  1: Enable MainCharger.
//  Register MainChargerControl 0x0B80, Bits 0:0, typedef main_charger_enable
//   
//  
#define MAIN_CHARGER_ENABLE_MASK 0x1
#define MAIN_CHARGER_ENABLE_OFFSET 0
#define MAIN_CHARGER_ENABLE_STOP_BIT 0
#define MAIN_CHARGER_ENABLE_START_BIT 0
#define MAIN_CHARGER_ENABLE_WIDTH 1


typedef enum main_charger_enable {
    MAIN_CHARGER_DISABLE_E,
    MAIN_CHARGER_ENABLE_E
} MAIN_CHARGER_ENABLE_T ;

#define MAIN_CHARGER_ENABLE_PARAM_MASK  0x1

typedef enum max_current {
    CHARGE_MAX_1A5_E,
    CHARGE_MAX_900MA_E
} MAX_CURRENT_T ;



//  
//  0: I charge Max = 900 mA 
//  1: I charge Max = 1.5 A.
//  Register MainChargerControl2 0x0B81, Bits 2:2, typedef max_current
//   
//  
#define MAIN_CHANNEL_MAX_MASK 0x4
#define MAIN_CHANNEL_MAX_OFFSET 2
#define MAIN_CHANNEL_MAX_STOP_BIT 2
#define MAIN_CHANNEL_MAX_START_BIT 2
#define MAIN_CHANNEL_MAX_WIDTH 1

#define MAIN_CHARGER_CONTROL_2_REG 0xB81
#define MAIN_CHANNEL_MAX_PARAM_MASK  0x4
#define MAX_CURRENT_PARAM_MASK  0x4

//  
//  00: Positive thermal threshold = 150 °C 
//  01: Positive thermal threshold = 159 °C 
//  10: Positive thermal threshold = 168 °C 
//  11: Positive thermal threshold = 140 °C.
//  Register MainChargerControl2 0x0B81, Bits 0:1, typedef thermal_threshold
//   
//  
#define THERMAL_THRESHOLD_MASK 0x3
#define THERMAL_THRESHOLD_OFFSET 1
#define THERMAL_THRESHOLD_STOP_BIT 1
#define THERMAL_THRESHOLD_START_BIT 0
#define THERMAL_THRESHOLD_WIDTH 2

#define THERMAL_THRESHOLD_PARAM_MASK  0x3
#define  CURRENT_CHARGE_90MA 0
#define  CURRENT_CHARGE_250MA 1
#define  CURRENT_CHARGE_450MA 2
#define  CURRENT_CHARGE_650MA 3
#define  CURRENT_CHARGE_850MA 4
#define  CURRENT_CHARGE_950MA 5
#define  CURRENT_CHARGE_1100MA 6
#define  CURRENT_CHARGE_1300MA 7



typedef enum input_current_charge {
    CURRENT_CHARGE_90MA_E,
    CURRENT_CHARGE_250MA_E,
    CURRENT_CHARGE_450MA_E,
    CURRENT_CHARGE_650MA_E,
    CURRENT_CHARGE_850MA_E,
    CURRENT_CHARGE_950MA_E,
    CURRENT_CHARGE_1100MA_E,
    CURRENT_CHARGE_1300MA_E
} INPUT_CURRENT_CHARGE_T ;




//  
//  MainChInputCurr.
//  Register MainChargerCurrentControl 0x0B82, Bits 4:7, typedef charger_current_level
//   
//  
#define MAIN_CH_INPUT_CURR_MASK 0xF0
#define MAIN_CH_INPUT_CURR_OFFSET 7
#define MAIN_CH_INPUT_CURR_STOP_BIT 7
#define MAIN_CH_INPUT_CURR_START_BIT 4
#define MAIN_CH_INPUT_CURR_WIDTH 4

#define MAIN_CHARGER_CURRENT_CONTROL_REG 0xB82
#define MAIN_CH_INPUT_CURR_PARAM_MASK  0xF0

//  
//  0: enable Vbat overshoot control 
//  1: disable Vbat overshoot control.
//  Register UsbChargerControl 0x0BC0, Bits 4:4, typedef usb_overshoot_control_enable
//   
//  
#define USB_OVERSHOOT_CONTROL_MASK 0x10
#define USB_OVERSHOOT_CONTROL_OFFSET 4
#define USB_OVERSHOOT_CONTROL_STOP_BIT 4
#define USB_OVERSHOOT_CONTROL_START_BIT 4
#define USB_OVERSHOOT_CONTROL_WIDTH 1

#define USB_CHARGER_CONTROL_REG 0xBC0

typedef enum usb_overshoot_control_enable {
    USB_OVERSHOOT_CONTROL_ENABLE_E,
    USB_OVERSHOOT_CONTROL_DISABLE_E
} USB_OVERSHOOT_CONTROL_ENABLE_T ;

#define USB_OVERSHOOT_CONTROL_ENABLE_PARAM_MASK  0x10

//  
//  0: inactive (current limitation: 900mA in HS mode, 500mA in Chirp mode) 
//  1: disable current limitation (current limitation from ULPI path disable, current max 
//  can be up to 1.5A).
//  Register UsbChargerControl 0x0BC0, Bits 3:3, typedef current_limit_disable
//   
//  
#define UBS_CURRENT_LIMIT_DISABLE_MASK 0x8
#define UBS_CURRENT_LIMIT_DISABLE_OFFSET 3
#define UBS_CURRENT_LIMIT_DISABLE_STOP_BIT 3
#define UBS_CURRENT_LIMIT_DISABLE_START_BIT 3
#define UBS_CURRENT_LIMIT_DISABLE_WIDTH 1

#define  CURRENT_LIMIT_DISABLE 8

typedef enum current_limit_disable {
    CURRENT_LIMIT_DISABLE_INACTIVE_E,
    CURRENT_LIMIT_DISABLE_DISABLE_E
} CURRENT_LIMIT_DISABLE_T ;
#define CURRENT_LIMIT_DISABLE_PARAM_MASK  0x8



//  
//  0: Disable USB charge 
//  1: Enable USB charger.
//  Register UsbChargerControl 0x0BC0, Bits 0:0, typedef ubs_charger_enable
//   
//  
#define UBS_CHARGER_ENABLE_MASK 0x1
#define UBS_CHARGER_ENABLE_OFFSET 0
#define UBS_CHARGER_ENABLE_STOP_BIT 0
#define UBS_CHARGER_ENABLE_START_BIT 0
#define UBS_CHARGER_ENABLE_WIDTH 1


typedef enum ubs_charger_enable {
    UBS_CHARGER_DISABLE_E,
    UBS_CHARGER_ENABLE_E
} UBS_CHARGER_ENABLE_T ;

#define UBS_CHARGER_ENABLE_PARAM_MASK  0x1
typedef enum vbus_voltage_stop {
    STOP_OVER_5_POINT_60_VOLT = 0x0,
    STOP_OVER_5_POINT_70_VOLT = 0x1,
    STOP_OVER_5_POINT_80_VOLT = 0x2,
    STOP_OVER_5_POINT_90_VOLT = 0x3,
    STOP_OVER_6_VOLT = 0x4,
    STOP_OVER_6_POINT_10_VOLT = 0x5,
    STOP_OVER_6_POINT_20_VOLT = 0x6,
    STOP_OVER_6_POINT_30_VOLT = 0x7,
    STOP_OVER_6_POINT_40_VOLT = 0x8
} VBUS_VOLTAGE_STOP_T ;



//  
//  0: Inactive 
//  1: when Vbus drops below VbusValid (4.4V), Usb charging will stop. SW needs to 
//  restart charger by setting 'UsbChEna' bit low then high.
//  Register UsbChargerControl2 0x0BC1, Bits 7:7, typedef vbus_monitor_enable
//   
//  
#define VBUS_MONITOR_MASK 0x80
#define VBUS_MONITOR_OFFSET 7
#define VBUS_MONITOR_STOP_BIT 7
#define VBUS_MONITOR_START_BIT 7
#define VBUS_MONITOR_WIDTH 1

#define USB_CHARGER_CONTROL_2_REG 0xBC1

typedef enum vbus_monitor_enable {
    VBUS_MONITOR_DISABLE_E,
    VBUS_MONITOR_ENABLE_E
} VBUS_MONITOR_ENABLE_T ;

#define VBUS_MONITOR_ENABLE_PARAM_MASK  0x80

//  
//  Charging stops is Vbus voltage is higher than.
//  Register UsbChargerControl2 0x0BC1, Bits 3:6, typedef vbus_voltage_stop
//   
//  
#define UBS_VOLTAGE_STOP_MASK 0x78
#define UBS_VOLTAGE_STOP_OFFSET 6
#define UBS_VOLTAGE_STOP_STOP_BIT 6
#define UBS_VOLTAGE_STOP_START_BIT 3
#define UBS_VOLTAGE_STOP_WIDTH 4

#define UBS_VOLTAGE_STOP_PARAM_MASK  0x78
#define VBUS_VOLTAGE_STOP_PARAM_MASK  0x78

//  
//  0: I charge Max = 900 mA 
//  1: I charge Max = 1.5 A.
//  Register UsbChargerControl2 0x0BC1, Bits 2:2, typedef max_current
//   
//  
#define UBS_CHARGER_MAX_CURRENT_MASK 0x4
#define UBS_CHARGER_MAX_CURRENT_OFFSET 2
#define UBS_CHARGER_MAX_CURRENT_STOP_BIT 2
#define UBS_CHARGER_MAX_CURRENT_START_BIT 2
#define UBS_CHARGER_MAX_CURRENT_WIDTH 1

#define UBS_CHARGER_MAX_CURRENT_PARAM_MASK  0x4

//  
//  00: Positive thermal threshold = 150 °C 
//  01: Positive thermal threshold = 159 °C 
//  10: Positive thermal threshold = 168 °C 
//  11: Positive thermal threshold = 140 °C.
//  Register UsbChargerControl2 0x0BC1, Bits 0:1, typedef thermal_threshold
//   
//  
#define UBS_CHARGER_THERMAL_THRESHOLD_MASK 0x3
#define UBS_CHARGER_THERMAL_THRESHOLD_OFFSET 1
#define UBS_CHARGER_THERMAL_THRESHOLD_STOP_BIT 1
#define UBS_CHARGER_THERMAL_THRESHOLD_START_BIT 0
#define UBS_CHARGER_THERMAL_THRESHOLD_WIDTH 2

#define UBS_CHARGER_THERMAL_THRESHOLD_PARAM_MASK  0x3
#define  USB_CURRENT_CHARGE_50MA 0
#define  USB_CURRENT_CHARGE_98MA 1
#define  USB_CURRENT_CHARGE_193MA 2
#define  USB_CURRENT_CHARGE_290MA 3
#define  USB_CURRENT_CHARGE_380MA 4
#define  USB_CURRENT_CHARGE_450MA 5
#define  USB_CURRENT_CHARGE_500MA 6
#define  USB_CURRENT_CHARGE_600MA 7
#define  USB_CURRENT_CHARGE_700MA 8
#define  USB_CURRENT_CHARGE_800MA 9
#define  USB_CURRENT_CHARGE_900MA 10
#define  USB_CURRENT_CHARGE_1000MA 11
#define  USB_CURRENT_CHARGE_1100MA 12
#define  USB_CURRENT_CHARGE_1300MA 13
#define  USB_CURRENT_CHARGE_1400MA 14
#define  USB_CURRENT_CHARGE_1500MA 15



typedef enum usb_input_current_charge {
    USB_CURRENT_CHARGE_50MA_E,
    USB_CURRENT_CHARGE_98MA_E,
    USB_CURRENT_CHARGE_193MA_E,
    USB_CURRENT_CHARGE_290MA_E,
    USB_CURRENT_CHARGE_380MA_E,
    USB_CURRENT_CHARGE_450MA_E,
    USB_CURRENT_CHARGE_500MA_E,
    USB_CURRENT_CHARGE_600MA_E,
    USB_CURRENT_CHARGE_700MA_E,
    USB_CURRENT_CHARGE_800MA_E,
    USB_CURRENT_CHARGE_900MA_E,
    USB_CURRENT_CHARGE_1000MA_E,
    USB_CURRENT_CHARGE_1100MA_E,
    USB_CURRENT_CHARGE_1300MA_E,
    USB_CURRENT_CHARGE_1400MA_E,
    USB_CURRENT_CHARGE_1500MA_E
} USB_INPUT_CURRENT_CHARGE_T ;




//  
//  .
//  Register UsbChargerControl3 0x0BC2, Bits 4:6, typedef usb_input_current_charge
//   
//  
#define UBS_CHARGER_CURRENT_LIMIT_MASK 0x70
#define UBS_CHARGER_CURRENT_LIMIT_OFFSET 6
#define UBS_CHARGER_CURRENT_LIMIT_STOP_BIT 6
#define UBS_CHARGER_CURRENT_LIMIT_START_BIT 4
#define UBS_CHARGER_CURRENT_LIMIT_WIDTH 3

#define USB_CHARGER_CONTROL_3_REG 0xBC2
#define UBS_CHARGER_CURRENT_LIMIT_PARAM_MASK  0x70
#define USB_INPUT_CURRENT_CHARGE_PARAM_MASK  0x70
#endif
