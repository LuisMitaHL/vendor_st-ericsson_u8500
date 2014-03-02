/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Header files for init_gpio module
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#ifndef _SERVICES_GPIO_
#define _SERVICES_GPIO_
#include "hcl_defs.h"
#include "gpio.h"
#include "services.h"

#define ARRAY_SIZE 96 
#define ARRAY_SIZE 96 
#define DEFAULT_IRQ (t_gpio_irq)100
#define DEFAULT_PIN	(t_gpio_pin)100
#define DEFAULT_TRIG (t_gpio_trig)100

#ifndef ST_8500ED
#define SER_GPIO_PRCC_CLK_ENABLE_5 0x002
#define SER_GPIO_PRCC_CLK_ENABLE_3 0x100
#define SER_GPIO_PRCC_CLK_ENABLE_2 0x800
#define SER_GPIO_PRCC_CLK_ENABLE_1 0x200
#endif

typedef struct
{
	t_gpio_pin_irq_status gpio_irq_param;
}t_ser_gpio_param;

/* Function Prototypes */

void SER_GPIO0_IntHandler(t_uint32 irq);
void SER_GPIO1_IntHandler(t_uint32 irq);
void SER_GPIO2_IntHandler(t_uint32 irq);
void SER_GPIO3_IntHandler(t_uint32 irq);
void SER_GPIO4_IntHandler(t_uint32 irq);
void SER_GPIO5_IntHandler(t_uint32 irq);
void SER_GPIO6_IntHandler(t_uint32 irq);
void SER_GPIO7_IntHandler(t_uint32 irq);
void SER_GPIO8_IntHandler(t_uint32 irq);

PUBLIC void SER_GPIO_Init(t_uint8);
PUBLIC void SER_GPIO_Close(void); 
PUBLIC t_ser_error SER_GPIO_RegisterCallback(t_gpio_device_id, t_callback_fct , void *);
PUBLIC void SER_GPIO_WaitEnd(t_gpio_pin pin);
                               
#endif /* END OF FILE*/


