/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of General Purpose Input/Output (GPIO) module.
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __INC_GPIO_H
#define __INC_GPIO_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include "gpio_irq.h"
#include "debug.h"
#ifndef _HCL_DEFS_H
#include "hcl_defs.h"
#endif

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
/*Defines for Version */
#define GPIO_HCL_VERSION_ID 1
#define GPIO_HCL_MAJOR_ID   0
#define GPIO_HCL_MINOR_ID   7
#define GPIO_TOTAL_COUNT    268

/* Error values returned by functions */
typedef enum
{
    GPIO_OK                     = HCL_OK,                       /* (0) */
    GPIO_UNSUPPORTED_HW         = HCL_UNSUPPORTED_HW,           /* (-2) */
    GPIO_UNSUPPORTED_FEATURE    = HCL_UNSUPPORTED_FEATURE,      /* (-3) */
    GPIO_INVALID_PARAMETER      = HCL_INVALID_PARAMETER,        /* (-4) */
    GPIO_REQUEST_NOT_APPLICABLE = HCL_REQUEST_NOT_APPLICABLE,   /* (-5) */
    GPIO_REQUEST_PENDING        = HCL_REQUEST_PENDING,          /* (-6) */
    GPIO_NOT_CONFIGURED         = HCL_NOT_CONFIGURED,           /* (-7) */
    GPIO_INTERNAL_ERROR         = HCL_INTERNAL_ERROR,           /* (-8) */
    GPIO_INTERNAL_EVENT         = HCL_INTERNAL_EVENT,
    GPIO_REMAINING_EVENT        = HCL_REMAINING_PENDING_EVENTS,
    GPIO_NO_MORE_PENDING_EVENT  = HCL_NO_MORE_PENDING_EVENT
} t_gpio_error;

/*Pin description
  To be used in SOFTWARE mode: refers to a pin. */

typedef enum
{
    GPIO_PIN_0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
    GPIO_PIN_16,
    GPIO_PIN_17,
    GPIO_PIN_18,
    GPIO_PIN_19,
    GPIO_PIN_20,
    GPIO_PIN_21,
    GPIO_PIN_22,
    GPIO_PIN_23,
    GPIO_PIN_24,
    GPIO_PIN_25,
    GPIO_PIN_26,
    GPIO_PIN_27,
    GPIO_PIN_28,
    GPIO_PIN_29,
    GPIO_PIN_30,
    GPIO_PIN_31,
    GPIO_PIN_32,
    GPIO_PIN_33,
    GPIO_PIN_34,
    GPIO_PIN_35,
    GPIO_PIN_36,
    GPIO_PIN_37,
    GPIO_PIN_38,
    GPIO_PIN_39,
    GPIO_PIN_40,
    GPIO_PIN_41,
    GPIO_PIN_42,
    GPIO_PIN_43,
    GPIO_PIN_44,
    GPIO_PIN_45,
    GPIO_PIN_46,
    GPIO_PIN_47,
    GPIO_PIN_48,
    GPIO_PIN_49,
    GPIO_PIN_50,
    GPIO_PIN_51,
    GPIO_PIN_52,
    GPIO_PIN_53,
    GPIO_PIN_54,
    GPIO_PIN_55,
    GPIO_PIN_56,
    GPIO_PIN_57,
    GPIO_PIN_58,
    GPIO_PIN_59,
    GPIO_PIN_60,
    GPIO_PIN_61,
    GPIO_PIN_62,
    GPIO_PIN_63,
    GPIO_PIN_64,
    GPIO_PIN_65,
    GPIO_PIN_66,
    GPIO_PIN_67,
    GPIO_PIN_68,
    GPIO_PIN_69,
    GPIO_PIN_70,
    GPIO_PIN_71,
    GPIO_PIN_72,
    GPIO_PIN_73,
    GPIO_PIN_74,
    GPIO_PIN_75,
    GPIO_PIN_76,
    GPIO_PIN_77,
    GPIO_PIN_78,
    GPIO_PIN_79,
    GPIO_PIN_80,
    GPIO_PIN_81,
    GPIO_PIN_82,
    GPIO_PIN_83,
    GPIO_PIN_84,
    GPIO_PIN_85,
    GPIO_PIN_86,
    GPIO_PIN_87,
    GPIO_PIN_88,
    GPIO_PIN_89,
    GPIO_PIN_90,
    GPIO_PIN_91,
    GPIO_PIN_92,
    GPIO_PIN_93,
    GPIO_PIN_94,
    GPIO_PIN_95,
    GPIO_PIN_96,
    GPIO_PIN_97,
    GPIO_PIN_98,
    GPIO_PIN_99,
    GPIO_PIN_100,
    GPIO_PIN_101,
    GPIO_PIN_102,
    GPIO_PIN_103,
    GPIO_PIN_104,
    GPIO_PIN_105,
    GPIO_PIN_106,
    GPIO_PIN_107,
    GPIO_PIN_108,
    GPIO_PIN_109,
    GPIO_PIN_110,
    GPIO_PIN_111,
    GPIO_PIN_112,
    GPIO_PIN_113,
    GPIO_PIN_114,
    GPIO_PIN_115,
    GPIO_PIN_116,
    GPIO_PIN_117,
    GPIO_PIN_118,
    GPIO_PIN_119,
    GPIO_PIN_120,
    GPIO_PIN_121,
    GPIO_PIN_122,
    GPIO_PIN_123,
    GPIO_PIN_124,
    GPIO_PIN_125,
    GPIO_PIN_126,
    GPIO_PIN_127,
    GPIO_PIN_128,
    GPIO_PIN_129,
    GPIO_PIN_130,
    GPIO_PIN_131,
    GPIO_PIN_132,
    GPIO_PIN_133,
    GPIO_PIN_134,
    GPIO_PIN_135,
    GPIO_PIN_136,
    GPIO_PIN_137,
    GPIO_PIN_138,
    GPIO_PIN_139,
    GPIO_PIN_140,
    GPIO_PIN_141,
    GPIO_PIN_142,
    GPIO_PIN_143,
    GPIO_PIN_144,
    GPIO_PIN_145,
    GPIO_PIN_146,
    GPIO_PIN_147,
    GPIO_PIN_148,
    GPIO_PIN_149,
    GPIO_PIN_150,
    GPIO_PIN_151,
    GPIO_PIN_152,
    GPIO_PIN_153,
    GPIO_PIN_154,
    GPIO_PIN_155,
    GPIO_PIN_156,
    GPIO_PIN_157,
    GPIO_PIN_158,
    GPIO_PIN_159,
    GPIO_PIN_160,
    GPIO_PIN_161,
    GPIO_PIN_162,
    GPIO_PIN_163,
    GPIO_PIN_164,
    GPIO_PIN_165,
    GPIO_PIN_166,
    GPIO_PIN_167,
    GPIO_PIN_168,
    GPIO_PIN_169,
    GPIO_PIN_170,
    GPIO_PIN_171,
    GPIO_PIN_172,
    GPIO_PIN_173,
    GPIO_PIN_174,
    GPIO_PIN_175,
    GPIO_PIN_176,
    GPIO_PIN_177,
    GPIO_PIN_178,
    GPIO_PIN_179,
    GPIO_PIN_180,
    GPIO_PIN_181,
    GPIO_PIN_182,
    GPIO_PIN_183,
    GPIO_PIN_184,
    GPIO_PIN_185,
    GPIO_PIN_186,
    GPIO_PIN_187,
    GPIO_PIN_188,
    GPIO_PIN_189,
    GPIO_PIN_190,
    GPIO_PIN_191,
    GPIO_PIN_192,
    GPIO_PIN_193,
    GPIO_PIN_194,
    GPIO_PIN_195,
    GPIO_PIN_196,
    GPIO_PIN_197,
    GPIO_PIN_198,
    GPIO_PIN_199,
    GPIO_PIN_200,
    GPIO_PIN_201,
    GPIO_PIN_202,
    GPIO_PIN_203,
    GPIO_PIN_204,
    GPIO_PIN_205,
    GPIO_PIN_206,
    GPIO_PIN_207,
    GPIO_PIN_208,
    GPIO_PIN_209,
    GPIO_PIN_210,
    GPIO_PIN_211,
    GPIO_PIN_212,
    GPIO_PIN_213,
    GPIO_PIN_214,
    GPIO_PIN_215,
    GPIO_PIN_216,
    GPIO_PIN_217,
    GPIO_PIN_218,
    GPIO_PIN_219,
    GPIO_PIN_220,
    GPIO_PIN_221,
    GPIO_PIN_222,
    GPIO_PIN_223,
    GPIO_PIN_224,
    GPIO_PIN_225,
    GPIO_PIN_226,
    GPIO_PIN_227,
    GPIO_PIN_228,
    GPIO_PIN_229,
    GPIO_PIN_230,
    GPIO_PIN_231,
    GPIO_PIN_232,
    GPIO_PIN_233,
    GPIO_PIN_234,
    GPIO_PIN_235,
    GPIO_PIN_236,
    GPIO_PIN_237,
    GPIO_PIN_238,
    GPIO_PIN_239,
    GPIO_PIN_240,
    GPIO_PIN_241,
    GPIO_PIN_242,
    GPIO_PIN_243,
    GPIO_PIN_244,
    GPIO_PIN_245,
    GPIO_PIN_246,
    GPIO_PIN_247,
    GPIO_PIN_248,
    GPIO_PIN_249,
    GPIO_PIN_250,
    GPIO_PIN_251,
    GPIO_PIN_252,
    GPIO_PIN_253,
    GPIO_PIN_254,
    GPIO_PIN_255,
    GPIO_PIN_256,
    GPIO_PIN_257,
    GPIO_PIN_258,
    GPIO_PIN_259,
    GPIO_PIN_260,
    GPIO_PIN_261,
    GPIO_PIN_262,
    GPIO_PIN_263,
    GPIO_PIN_264,
    GPIO_PIN_265,
    GPIO_PIN_266,
    GPIO_PIN_267
} t_gpio_pin;
		 
typedef enum
{
	GPIO_ALT_SSP0,				/* GPIO[146:143]			     */    
	GPIO_ALT_SSP1,				/* GPIO[142:139] 				 */
	GPIO_ALT_I2C0,				/* GPIO[148:147] 			     */	
	GPIO_ALT_I2C1_1,			/* GPIO[7:6] 				     */
	GPIO_ALT_I2C1_2,            /* GPIO[17:16]                   */
	GPIO_ALT_I2C2_1,			/* GPIO[9:8]				     */
	GPIO_ALT_I2C2_2,            /* GPIO[11:10]                   */
	GPIO_ALT_I2C3_1,			/* GPIO[230:229]    			 */
	GPIO_ALT_I2C3_2,            /* GPIO[218,216]                 */
	GPIO_ALT_I2C4,				/* GPIO[5:4]                     */
	GPIO_ALT_MM_CARD0,			/* GPIO[32:18]			         */
	GPIO_ALT_MM_CARD1,			/* GPIO[218:208]			     */
	GPIO_ALT_MM_CARD2,			/* GPIO[138:128]                 */
	GPIO_ALT_MM_CARD3,        	/* GPIO[225:215]                 */
	GPIO_ALT_MM_CARD4,    		/* GPIO[203:197]                 */
	GPIO_ALT_MM_CARD5,    		/* GPIO[93:86,97:96]             */
	GPIO_ALT_MSP0,				/* GPIO[17:12]	    		     */
	GPIO_ALT_MSP1,				/* GPIO[36:33]   				 */
	GPIO_ALT_MSP2,				/* GPIO[196:192]    			 */
	GPIO_ALT_UART_0,		    /* GPIO[3:0,36:33]               */    
	GPIO_ALT_UART_1,         	/* GPIO[7:4]                     */
	GPIO_ALT_UART_2,         	/* GPIO[32:29]   	             */
	GPIO_ALT_LCD_PANEL,			/* GPIO[67:64,93:70,171:153]     */
	GPIO_ALT_KEYPAD,			/* GPIO[69:64,83:72,139,152,171:170]*/
	GPIO_ALT_HSIT0,				/* GPIO[226:222]		             */ 
	GPIO_ALT_HSIR0,				/* GPIO[221:219]		         */  
	GPIO_ALT_USB_OTG,			/* GPIO[267:256]				 */
	GPIO_ALT_NAND_FLASH,		/* GPIO[69:68,97:86,138:128]*/
	GPIO_ALT_NOR_FLASH,			/* GPIO[69:68,97:96,94:86,138:125] */
	GPIO_ALT_SPI0,				/* GPIO[20,23:25] alt C                */
	GPIO_ALT_SPI1,				/* GPIO[209,212:214] altB             */
	GPIO_ALT_SPI3,				/* GPIO[29:32]  altB                 */
	GPIO_ALT_STMAPE_1,          /* GPIO[70:74]  altC                 */
	GPIO_ALT_STMAPE_2,			/* GPIO[155:159] altC				 */
} t_gpio_alt_function;

typedef enum 
{
	GPIO_BLOCK_32_BITS_0_TO_31,     /* GPIO[31:0], Byte 1, 2, 3 & 4  */
	GPIO_BLOCK_32_BITS_32_TO_63,    /* GPIO[63:32], Byte 5, 6, 7 & 8 */
	GPIO_BLOCK_32_BITS_64_TO_95,    /* GPIO[95:64], Byte 9, 10, 11 & 12 */
	GPIO_BLOCK_32_BITS_96_TO_127,   /* GPIO[127:96], Byte 13, 14, 15 & 16 */
	GPIO_BLOCK_32_BITS_128_TO_159,  /* GPIO[159:128], Byte 17, 18, 19 & 20 */
	GPIO_BLOCK_32_BITS_160_TO_191,  /* GPIO[191:160], Byte 21, 22, 23 & 24 */
	GPIO_BLOCK_32_BITS_192_TO_223,  /* GPIO[207:192], Byte 25, 26, 27 & 28 */		
	GPIO_BLOCK_32_BITS_224_TO_255,  /* GPIO[207:192], Byte 29, 30, 31 & 32 */
	GPIO_BLOCK_32_BITS_256_TO_267,  /* GPIO[207:192], Byte 33, 34 */
	GPIO_BLOCK_16_BITS_0_TO_15,     /* GPIO[15:0],Byte 1 & 2 		*/
	GPIO_BLOCK_16_BITS_8_TO_23,     /* GPIO[23:8], Byte 2 & 3 		*/ 
	GPIO_BLOCK_16_BITS_16_TO_31,    /* GPIO[31:16], Byte 3 & 4 		*/ 
	GPIO_BLOCK_16_BITS_24_TO_39,    /* GPIO[39:24], Byte 4 & 5 		*/
	GPIO_BLOCK_16_BITS_32_TO_47,    /* GPIO[47:32], Byte 5 & 6 		*/
	GPIO_BLOCK_16_BITS_40_TO_55,    /* GPIO[55:40], Byte 6 & 7 		*/
	GPIO_BLOCK_16_BITS_48_TO_63,    /* GPIO[63:48], Byte 7 & 8 		*/
	GPIO_BLOCK_16_BITS_56_TO_71,    /* GPIO[71:56], Byte 8 & 9  	*/
	GPIO_BLOCK_16_BITS_64_TO_79,     /* GPIO[79:64], Byte 9 & 10  	*/
	GPIO_BLOCK_16_BITS_72_TO_87,    /* GPIO[87:72], Byte 11 & 10 	*/
	GPIO_BLOCK_16_BITS_80_TO_95,    /* GPIO[95:80], Byte 12 & 11 	*/
	GPIO_BLOCK_16_BITS_88_TO_103,   /* GPIO[103:88], Byte 12 & 13 	*/
	GPIO_BLOCK_16_BITS_96_TO_111,   /* GPIO[111:96], Byte 13 & 14 	*/
	GPIO_BLOCK_16_BITS_104_TO_119,  /* GPIO[119:104], Byte 14 & 15 	*/
	GPIO_BLOCK_16_BITS_112_TO_127,  /* GPIO[127:112],Byte 15 & 16 	*/
	GPIO_BLOCK_16_BITS_120_TO_135,  /* GPIO[135:120], Byte 16 & 17	*/
	GPIO_BLOCK_16_BITS_128_TO_143,  /* GPIO[143:128], Byte 17 & 18	*/
	GPIO_BLOCK_16_BITS_136_TO_151,  /* GPIO[151:136], Byte 18 & 19 	*/
	GPIO_BLOCK_16_BITS_144_TO_159,  /* GPIO[159:144], Byte 19 & 20 	*/
	GPIO_BLOCK_16_BITS_152_TO_167,  /* GPIO[167:152], Byte 20 & 21 	*/
	GPIO_BLOCK_16_BITS_160_TO_175,  /* GPIO[175:160],Byte 21 & 22	*/
	GPIO_BLOCK_16_BITS_168_TO_183,  /* GPIO[183:168], Byte 22 & 23 	*/
	GPIO_BLOCK_16_BITS_176_TO_191,  /* GPIO[191:176], Byte 23 & 24	*/
	GPIO_BLOCK_16_BITS_184_TO_199,  /* GPIO[199:184], Byte 24 & 25 	*/
	GPIO_BLOCK_16_BITS_192_TO_207,   /* GPIO[203:192],Byte 25 & 26	*/
	GPIO_BLOCK_16_BITS_200_TO_215,   /* GPIO[203:192],Byte 26 & 27	*/
	GPIO_BLOCK_16_BITS_208_TO_223,   /* GPIO[203:192],Byte 27 & 28	*/
	GPIO_BLOCK_16_BITS_216_TO_231,   /* GPIO[203:192],Byte 28 & 29	*/
	GPIO_BLOCK_16_BITS_224_TO_239,   /* GPIO[203:192],Byte 29 & 30	*/
	GPIO_BLOCK_16_BITS_232_TO_247,   /* GPIO[203:192],Byte 30 & 31	*/	
	GPIO_BLOCK_16_BITS_240_TO_255,   /* GPIO[203:192],Byte 31 & 32	*/	
	GPIO_BLOCK_16_BITS_248_TO_263,   /* GPIO[203:192],Byte 32 & 33	*/	
	GPIO_BLOCK_16_BITS_256_TO_267,   /* GPIO[203:192],Byte 33	*/	
} t_gpio_block_id;

/* Interrupt trigger mode */     
typedef enum
{
    GPIO_TRIG_LEAVE_UNCHANGED,      /* Parameter will be ignored by the function */
    GPIO_TRIG_DISABLE,              /* Triggers no IT */
    GPIO_TRIG_RISING_EDGE,          /* Triggers an IT on a rising edge */
    GPIO_TRIG_FALLING_EDGE,         /* Triggers an IT on a falling edge */
    GPIO_TRIG_BOTH_EDGES,           /* Triggers an IT on a rising and a falling edge */
    GPIO_TRIG_HIGH_LEVEL,           /* Triggers an IT on a high level */
    GPIO_TRIG_LOW_LEVEL             /* Triggers an IT on a low level */
} t_gpio_trig;                      /* Interrupt trigger mode, or disable */ 

/* Defines pin assignment(Software mode or Alternate mode) */
typedef enum
{
    GPIO_MODE_LEAVE_UNCHANGED,      /* Parameter will be ignored by the function. */
    GPIO_MODE_SOFTWARE,             /* Pin connected to GPIO (SW controlled) */
    GPIO_MODE_ALT_FUNCTION_A,       /* Pin connected to alternate function 1 (HW periph 1) */
    GPIO_MODE_ALT_FUNCTION_B,       /* Pin connected to alternate function 2 (HW periph 2) */
    GPIO_MODE_ALT_FUNCTION_C        /* Pin connected to alternate function 3 (HW periph 3) */
} t_gpio_mode;

/* Defines GPIO pin direction */
typedef enum
{
    GPIO_DIR_LEAVE_UNCHANGED,       /* Parameter will be ignored by the function. */
    GPIO_DIR_INPUT,                 /* GPIO set as input */
    GPIO_DIR_OUTPUT                 /* GPIO set as output */
} t_gpio_direction;

/* Debounce logic state */
typedef enum
{
    GPIO_DEBOUNCE_UNCHANGED,        /* Parameter will be ignored by the function. */
    GPIO_DEBOUNCE_DISABLE,          /* Debounce is disabled. */
    GPIO_DEBOUNCE_ENABLE            /* Debounce is enabled. */
} t_gpio_debounce;

/* Debounce Time explicit value & units */
typedef enum
{
    GPIO_DEBOUNCE_TIME_30_MICROSEC,
    GPIO_DEBOUNCE_TIME_60_MICROSEC,
    GPIO_DEBOUNCE_TIME_120_MICROSEC,
    GPIO_DEBOUNCE_TIME_240_MICROSEC,
    GPIO_DEBOUNCE_TIME_490_MICROSEC,
    GPIO_DEBOUNCE_TIME_980_MICROSEC,
    GPIO_DEBOUNCE_TIME_2_MILLISEC,
    GPIO_DEBOUNCE_TIME_4_MILLISEC,
    GPIO_DEBOUNCE_TIME_8_MILLISEC,
    GPIO_DEBOUNCE_TIME_16_MILLISEC,
    GPIO_DEBOUNCE_TIME_31_MILLISEC,
    GPIO_DEBOUNCE_TIME_62_MILLISEC,
    GPIO_DEBOUNCE_TIME_125_MILLISEC,
    GPIO_DEBOUNCE_TIME_250_MILLISEC,
    GPIO_DEBOUNCE_TIME_500_MILLISEC,
    GPIO_DEBOUNCE_TIME_1_SEC
} t_gpio_debounce_time;


typedef enum
{
    GPIO_LEVEL_LEAVE_UNCHANGED,      /* Parameter will be ignored by the function. */
    GPIO_LEVEL_PULLUP,             /* Pin connected to GPIO with pull up enabled */
    GPIO_LEVEL_PULLDOWN,       /* Pin connected to GPIO with pull down enabled */
    GPIO_LEVEL_LOW,           /* Pin connected to GPIO with output level low */
    GPIO_LEVEL_HIGH,           /* Pin connected to GPIO with output level high */
    GPIO_HIGH_IMPEDENCE       /*No PullUp or PullDown activated, so high impedance floating input*/
} t_gpio_level;

/* Configuration parameters for one GPIO pin.*/
typedef struct
{
    t_gpio_mode         mode;           /* Defines mode (SOFTWARE or Alternate). */
    t_gpio_direction    direction;      /* Define pin direction (in SOFTWARE mode only). */
    t_gpio_trig         trig;           /* Interrupt trigger (in SOFTWARE mode only) */
    t_gpio_debounce     debounce;       /* Debounce logic control for pin (in SOFTWARE mode only) */
    t_gpio_level	    level;          /*Pull-up,pull-down if direction is i/p else outputlevel low or high*/
} t_gpio_config;

/* GPIO pin data*/
typedef enum
{
    GPIO_DATA_LOW,                      /* GPIO pin status is low.	 */
    GPIO_DATA_HIGH                      /* GPIO pin status is high.	 */
} t_gpio_data;

/* GPIO behaviour in sleep mode */
typedef enum
{
    GPIO_SLEEP_MODE_LEAVE_UNCHANGED,    /* Parameter will be ignored by the function. */
    GPIO_SLEEP_MODE_INPUT_DEFAULTVOLT,  /* GPIO is an input with pull up/down enabled 
									   when in sleep mode. */
    GPIO_SLEEP_MODE_CONTROLLED_BY_GPIO  /* GPIO pin  is controlled by GPIO IP. So mode,
									   direction and data values for GPIO pin in
									   sleep mode are determined by configuration
									   set to GPIO pin before entering to sleep mode. */
} t_gpio_sleep_mode;

/* GPIO ability to wake the system up from sleep mode.*/
typedef enum
{
    GPIO_WAKE_LEAVE_UNCHANGED,          /* Parameter will be ignored by the function.		 */
    GPIO_WAKE_DISABLE,                  /* GPIO will not wake the system from sleep mode. 	 */
    GPIO_WAKE_LOW_LEVEL,                /* GPIO will wake the system up on a LOW level.		 */
    GPIO_WAKE_HIGH_LEVEL,               /* GPIO will wake the system up on a HIGH level.	 */
    GPIO_WAKE_RISING_EDGE,              /* GPIO will wake the system up on a RISING edge.	 */
    GPIO_WAKE_FALLING_EDGE,             /* GPIO will wake the system up on a FALLING edge.	 */
    GPIO_WAKE_BOTH_EDGES				/* GPIO will wake the system up on both RISING and FALLING edge. */
} t_gpio_wake;

/* Configuration parameters for one GPIO pin in sleep mode.*/
typedef struct
{
    t_gpio_sleep_mode   sleep_mode; /* GPIO behaviour in sleep mode. */
    t_gpio_wake         wake;       /* GPIO ability to wake up the system. */
} t_gpio_sleep_config;

/* Enumeration t_gpio_device_id or ORing of enumeration t_gpio_device_id */
typedef t_uint32    t_gpio_multiple_device_id;

typedef t_uint32 t_gpio_wakeup_status;

/* Interrupt status for all GPIO pins */
typedef struct
{
    t_bool  gpio[GPIO_TOTAL_COUNT];
} t_gpio_pin_irq_status;    /* Interrupt status for all GPIO pins */

/*------------------------------------------------------------------------
 * Functions declaration
 *----------------------------------------------------------------------*/
PUBLIC t_gpio_error GPIO_Init(t_gpio_ctrl_id, t_logical_address);
PUBLIC t_gpio_error GPIO_GetVersion(t_version *);
PUBLIC t_gpio_error GPIO_SetDbgLevel(t_dbg_level);
PUBLIC t_gpio_error GPIO_ResetGpioPin(t_gpio_pin);
PUBLIC t_gpio_error GPIO_EnableAltFunction(t_gpio_alt_function);
PUBLIC t_gpio_error GPIO_DisableAltFunction(t_gpio_alt_function);
PUBLIC t_gpio_error GPIO_SetPinConfig(t_gpio_pin, t_gpio_config);
PUBLIC t_gpio_error GPIO_EnableDefaultVoltage(t_gpio_pin);
PUBLIC t_gpio_error GPIO_DisableDefaultVoltage(t_gpio_pin);
PUBLIC t_gpio_error GPIO_SetPinSleepMode(t_gpio_pin, t_gpio_sleep_config);
PUBLIC t_gpio_error GPIO_ReadGpioPin(t_gpio_pin, t_gpio_data *);
PUBLIC t_gpio_error GPIO_SetGpioPin(t_gpio_pin);
PUBLIC t_gpio_error GPIO_ClearGpioPin(t_gpio_pin);
PUBLIC t_gpio_error GPIO_ReadBlock(t_gpio_block_id, t_uint32 *, t_uint32);
PUBLIC t_gpio_error GPIO_WriteBlock(t_gpio_block_id, t_uint32, t_uint32);
PUBLIC void         GPIO_GetMulptipleIRQStatus(t_gpio_multiple_device_id, t_gpio_pin_irq_status *);
PUBLIC t_gpio_error GPIO_GetWakeupStatus(t_gpio_ctrl_id, t_gpio_wakeup_status *);
PUBLIC t_gpio_error GPIO_ConfigureGpioState(t_gpio_pin, t_gpio_config, t_gpio_sleep_config);
#ifdef __cplusplus
}                               /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __INC_GPIO_H */

/* End of file - gpio.h */


