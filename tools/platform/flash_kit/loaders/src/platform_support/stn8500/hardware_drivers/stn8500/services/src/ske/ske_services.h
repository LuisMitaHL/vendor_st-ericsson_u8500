/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  Header file of SKE services API
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __SKE_SERVICES_HEADER
#define __SKE_SERVICES_HEADER

#include "services.h"
#include "ske.h"
#include "ske_irq.h"

#if ((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
#define PRCC_KERNEL_CLK_EN_OFFSET 0x8
#define SKE_AMBA_CLK_EN_VAL		0x00000020
#define SKE_KERNEL_CLK_EN_VAL	0x00000020
#endif
typedef enum
{
    SKE_DEVICE_SCROLL_KEY_0 = 0,
    SKE_DEVICE_SCROLL_KEY_1,
    SKE_DEVICE_KEYPAD, 
    SKE_MAX_DEVICE,
    SKE_DEVICE_INVALID = 0xFF   
}t_ske_device;


typedef struct
{
    t_ske_device ske_device;    
    t_ske_keypad_column ske_keypressed_column; /* Can be ignored for Scroll Key */
    t_ske_keypad_row ske_keypressed_row;       /* Can be ignored for Scroll Key */    
}t_ske_callback_param;

typedef struct 
{
	t_bool overflow_status;
	t_bool underflow_status;
}t_ske_flow_status;

PUBLIC void SER_SKE_Init(IN t_uint8 mask);
void SER_SKE_InterruptHandler_ScrollKey(IN unsigned int irq);
void SER_SKE_InterruptHandler_Keypad(IN unsigned int irq);
PUBLIC int SER_SKE_RegisterCallback(IN t_ske_device ske_device,IN t_callback_fct callback_func,IN void *callback_param);
PUBLIC int SER_SKE_SetKeypadMatrixSize(IN t_uint8 max_columns,IN t_uint8 max_rows);
PUBLIC void SER_SKE_SelectKeyPad(void);
PUBLIC void SER_SKE_SelectScrollKey(void);
PUBLIC void SER_SKE_SelectScrollKey0(void);
PUBLIC void SER_SKE_SelectScrollKey1(void);
PUBLIC void SER_SKE_SelectBothScrollKeyAndKeypad(void);

#endif /* __SKE_SERVICES_HEADER */
