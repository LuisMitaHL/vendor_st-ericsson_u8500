/*****************************************************************************/
/**
*  � ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of General Purpose Input/Output (GPIO) module
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef __INC_GPIO_IRQ_H
#define __INC_GPIO_IRQ_H

#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"

/* This enumeration contains members for all gpio controller instances
   and is used in GPIO_Init() / GPIO_SetBaseAddress APIs. */


typedef enum
{
    GPIO_CTRL_ID_INVALID = 0x0,
    GPIO_CTRL_ID_0,
    GPIO_CTRL_ID_1,
    GPIO_CTRL_ID_2,
    GPIO_CTRL_ID_3, 
    GPIO_CTRL_ID_4, 
    GPIO_CTRL_ID_5, 
    GPIO_CTRL_ID_6,
    GPIO_CTRL_ID_7
} t_gpio_ctrl_id;

/*GPIO DEVICE ID */
typedef enum
{
    GPIO_DEVICE_ID_INVALID  = 0x0,
    GPIO_DEVICE_ID_0        = 0x01,
    GPIO_DEVICE_ID_1        = 0x02,
    GPIO_DEVICE_ID_2        = 0x04,	
    GPIO_DEVICE_ID_3        = 0x08,		
    GPIO_DEVICE_ID_4        = 0x10,		
    GPIO_DEVICE_ID_5        = 0x20,	
    GPIO_DEVICE_ID_6        = 0x40,
    GPIO_DEVICE_ID_7        = 0x80
} t_gpio_device_id;

/* Refers to the pin that caused the interrupt.*/
typedef enum
{
    GPIO_IRQ_SRC_0,
    GPIO_IRQ_SRC_1,
    GPIO_IRQ_SRC_2,
    GPIO_IRQ_SRC_3,
    GPIO_IRQ_SRC_4,
    GPIO_IRQ_SRC_5,
    GPIO_IRQ_SRC_6,
    GPIO_IRQ_SRC_7,
    GPIO_IRQ_SRC_8,
    GPIO_IRQ_SRC_9,
    GPIO_IRQ_SRC_10,
    GPIO_IRQ_SRC_11,
    GPIO_IRQ_SRC_12,
    GPIO_IRQ_SRC_13,
    GPIO_IRQ_SRC_14,
    GPIO_IRQ_SRC_15,
    GPIO_IRQ_SRC_16,
    GPIO_IRQ_SRC_17,
    GPIO_IRQ_SRC_18,
    GPIO_IRQ_SRC_19,
    GPIO_IRQ_SRC_20,
    GPIO_IRQ_SRC_21,
    GPIO_IRQ_SRC_22,
    GPIO_IRQ_SRC_23,
    GPIO_IRQ_SRC_24,
    GPIO_IRQ_SRC_25,
    GPIO_IRQ_SRC_26,
    GPIO_IRQ_SRC_27,
    GPIO_IRQ_SRC_28,
    GPIO_IRQ_SRC_29,
    GPIO_IRQ_SRC_30,
    GPIO_IRQ_SRC_31,
    GPIO_IRQ_SRC_32,
    GPIO_IRQ_SRC_33,
    GPIO_IRQ_SRC_34,
    GPIO_IRQ_SRC_35,
    GPIO_IRQ_SRC_36,
    GPIO_IRQ_SRC_37,
    GPIO_IRQ_SRC_38,
    GPIO_IRQ_SRC_39,
    GPIO_IRQ_SRC_40,
    GPIO_IRQ_SRC_41,
    GPIO_IRQ_SRC_42,
    GPIO_IRQ_SRC_43,
    GPIO_IRQ_SRC_44,
    GPIO_IRQ_SRC_45,
    GPIO_IRQ_SRC_46,
    GPIO_IRQ_SRC_47,
    GPIO_IRQ_SRC_48,
    GPIO_IRQ_SRC_49,
    GPIO_IRQ_SRC_50,
    GPIO_IRQ_SRC_51,
    GPIO_IRQ_SRC_52,
    GPIO_IRQ_SRC_53,
    GPIO_IRQ_SRC_54,
    GPIO_IRQ_SRC_55,
    GPIO_IRQ_SRC_56,
    GPIO_IRQ_SRC_57,
    GPIO_IRQ_SRC_58,
    GPIO_IRQ_SRC_59,
    GPIO_IRQ_SRC_60,
    GPIO_IRQ_SRC_61,
    GPIO_IRQ_SRC_62,
    GPIO_IRQ_SRC_63,
    GPIO_IRQ_SRC_64,
    GPIO_IRQ_SRC_65,
    GPIO_IRQ_SRC_66,
    GPIO_IRQ_SRC_67,
    GPIO_IRQ_SRC_68,
    GPIO_IRQ_SRC_69,
    GPIO_IRQ_SRC_70,
    GPIO_IRQ_SRC_71,
    GPIO_IRQ_SRC_72,
    GPIO_IRQ_SRC_73,
    GPIO_IRQ_SRC_74,
    GPIO_IRQ_SRC_75,
    GPIO_IRQ_SRC_76,
    GPIO_IRQ_SRC_77,
    GPIO_IRQ_SRC_78,
    GPIO_IRQ_SRC_79,
    GPIO_IRQ_SRC_80,
    GPIO_IRQ_SRC_81,
    GPIO_IRQ_SRC_82,
    GPIO_IRQ_SRC_83,
    GPIO_IRQ_SRC_84,
    GPIO_IRQ_SRC_85,
    GPIO_IRQ_SRC_86,
    GPIO_IRQ_SRC_87,
    GPIO_IRQ_SRC_88,
    GPIO_IRQ_SRC_89,
    GPIO_IRQ_SRC_90,
    GPIO_IRQ_SRC_91,
    GPIO_IRQ_SRC_92,
    GPIO_IRQ_SRC_93,
    GPIO_IRQ_SRC_94,
    GPIO_IRQ_SRC_95,
    GPIO_IRQ_SRC_96,
    GPIO_IRQ_SRC_97,
    GPIO_IRQ_SRC_98,
    GPIO_IRQ_SRC_99,
    GPIO_IRQ_SRC_100,
    GPIO_IRQ_SRC_101,
    GPIO_IRQ_SRC_102,
    GPIO_IRQ_SRC_103,
    GPIO_IRQ_SRC_104,
    GPIO_IRQ_SRC_105,
    GPIO_IRQ_SRC_106,
    GPIO_IRQ_SRC_107,
    GPIO_IRQ_SRC_108,
    GPIO_IRQ_SRC_109,
    GPIO_IRQ_SRC_110,
    GPIO_IRQ_SRC_111,
    GPIO_IRQ_SRC_112,
    GPIO_IRQ_SRC_113,
    GPIO_IRQ_SRC_114,
    GPIO_IRQ_SRC_115,
    GPIO_IRQ_SRC_116,
    GPIO_IRQ_SRC_117,
    GPIO_IRQ_SRC_118,
    GPIO_IRQ_SRC_119,
    GPIO_IRQ_SRC_120,
    GPIO_IRQ_SRC_121,
    GPIO_IRQ_SRC_122,
    GPIO_IRQ_SRC_123,
    GPIO_IRQ_SRC_124,
    GPIO_IRQ_SRC_125,
    GPIO_IRQ_SRC_126,
    GPIO_IRQ_SRC_127,
    GPIO_IRQ_SRC_128,
    GPIO_IRQ_SRC_129,
    GPIO_IRQ_SRC_130,
    GPIO_IRQ_SRC_131,
    GPIO_IRQ_SRC_132,
    GPIO_IRQ_SRC_133,
    GPIO_IRQ_SRC_134,
    GPIO_IRQ_SRC_135,
    GPIO_IRQ_SRC_136,
    GPIO_IRQ_SRC_137,
    GPIO_IRQ_SRC_138,
    GPIO_IRQ_SRC_139,
    GPIO_IRQ_SRC_140,
    GPIO_IRQ_SRC_141,
    GPIO_IRQ_SRC_142,
    GPIO_IRQ_SRC_143,
    GPIO_IRQ_SRC_144,
    GPIO_IRQ_SRC_145,
    GPIO_IRQ_SRC_146,
    GPIO_IRQ_SRC_147,
    GPIO_IRQ_SRC_148,
    GPIO_IRQ_SRC_149,
    GPIO_IRQ_SRC_150,
    GPIO_IRQ_SRC_151,
    GPIO_IRQ_SRC_152,
    GPIO_IRQ_SRC_153,
    GPIO_IRQ_SRC_154,
    GPIO_IRQ_SRC_155,
    GPIO_IRQ_SRC_156,
    GPIO_IRQ_SRC_157,
    GPIO_IRQ_SRC_158,
    GPIO_IRQ_SRC_159,
    GPIO_IRQ_SRC_160,
    GPIO_IRQ_SRC_161,
    GPIO_IRQ_SRC_162,
    GPIO_IRQ_SRC_163,
    GPIO_IRQ_SRC_164,
    GPIO_IRQ_SRC_165,
    GPIO_IRQ_SRC_166,
    GPIO_IRQ_SRC_167,
    GPIO_IRQ_SRC_168,
    GPIO_IRQ_SRC_169,
    GPIO_IRQ_SRC_170,
    GPIO_IRQ_SRC_171,
    GPIO_IRQ_SRC_172,
    GPIO_IRQ_SRC_173,
    GPIO_IRQ_SRC_174,
    GPIO_IRQ_SRC_175,
    GPIO_IRQ_SRC_176,
    GPIO_IRQ_SRC_177,
    GPIO_IRQ_SRC_178,
    GPIO_IRQ_SRC_179,
    GPIO_IRQ_SRC_180,
    GPIO_IRQ_SRC_181,
    GPIO_IRQ_SRC_182,
    GPIO_IRQ_SRC_183,
    GPIO_IRQ_SRC_184,
    GPIO_IRQ_SRC_185,
    GPIO_IRQ_SRC_186,
    GPIO_IRQ_SRC_187,
    GPIO_IRQ_SRC_188,
    GPIO_IRQ_SRC_189,
    GPIO_IRQ_SRC_190,
    GPIO_IRQ_SRC_191,
    GPIO_IRQ_SRC_192,
    GPIO_IRQ_SRC_193,
    GPIO_IRQ_SRC_194,
    GPIO_IRQ_SRC_195,
    GPIO_IRQ_SRC_196,
    GPIO_IRQ_SRC_197,
    GPIO_IRQ_SRC_198,
    GPIO_IRQ_SRC_199,
    GPIO_IRQ_SRC_200,
    GPIO_IRQ_SRC_201,
    GPIO_IRQ_SRC_202,
    GPIO_IRQ_SRC_203,
    GPIO_IRQ_SRC_204,
    GPIO_IRQ_SRC_205,
    GPIO_IRQ_SRC_206,
    GPIO_IRQ_SRC_207,
    GPIO_IRQ_SRC_208,
    GPIO_IRQ_SRC_209,
    GPIO_IRQ_SRC_210,
    GPIO_IRQ_SRC_211,
    GPIO_IRQ_SRC_212,
    GPIO_IRQ_SRC_213,
    GPIO_IRQ_SRC_214,
    GPIO_IRQ_SRC_215,
	GPIO_IRQ_SRC_216,
    GPIO_IRQ_SRC_217,
    GPIO_IRQ_SRC_218,
    GPIO_IRQ_SRC_219,
    GPIO_IRQ_SRC_220,
    GPIO_IRQ_SRC_221,
    GPIO_IRQ_SRC_222,
    GPIO_IRQ_SRC_223,
    GPIO_IRQ_SRC_224,
    GPIO_IRQ_SRC_225,
    GPIO_IRQ_SRC_226,
    GPIO_IRQ_SRC_227
} t_gpio_irq_src_id;


/*------------------------------------------------------------------------
 * Functions declaration
 *----------------------------------------------------------------------*/
/*Initialisation Function */
PUBLIC void GPIO_SetBaseAddress(t_gpio_ctrl_id, t_logical_address);

/* Interrupt Management	*/
PUBLIC t_bool GPIO_IsPendingIRQSrc(t_gpio_irq_src_id);
PUBLIC void GPIO_ClearIRQSrc(t_gpio_irq_src_id);
PUBLIC void GPIO_EnableIRQSrc(t_gpio_irq_src_id);
PUBLIC void GPIO_DisableIRQSrc(t_gpio_irq_src_id);
PUBLIC t_gpio_device_id GPIO_GetDeviceID(t_gpio_irq_src_id);
PUBLIC t_gpio_irq_src_id GPIO_GetIRQSrc(t_gpio_device_id);

#ifdef __cplusplus
}                       /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __INC_GPIO_IRQ_H */

/* End of file - gpio_irq.h */

