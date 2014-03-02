/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of General Purpose Input/Output (GPIO) module
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef __INC_GPIOIRQP_H
#define __INC_GPIOIRQP_H

#ifdef __cplusplus
extern "C"
{
#endif
#define GPIO_ALL_ZERO   0x00000000

#define GPIO_BLOCKS_COUNT       8
#define GPIO_NO_OF_PINS         32
#define GPIO_IRQ_BIT_POSITION_1 (0xAAAAAAAA)
#define GPIO_IRQ_BIT_POSITION_2 (0xCCCCCCCC)
#define GPIO_IRQ_BIT_POSITION_3 (0xF0F0F0F0)
#define GPIO_IRQ_BIT_POSITION_4 (0xFF00FF00)
#define GPIO_IRQ_BIT_POSITION_5 (0xFFFF0000)

/* GPIO registers */
typedef volatile struct
{
    t_uint32    gpio_dat;                           /* GPIO data register */						/*0x000*/
    t_uint32    gpio_dats;                          /* GPIO data Set register */					/*0x004*/
    t_uint32    gpio_datc;                          /* GPIO data Clear register */					/*0x008*/
    t_uint32    gpio_pdis;                          /* GPIO Pull disable register */				/*0x00C*/
    t_uint32    gpio_dir;                           /* GPIO data direction register */				/*0x010*/
    t_uint32    gpio_dirs;                          /* GPIO data direction Set register */			/*0x014*/
    t_uint32    gpio_dirc;                          /* GPIO data direction Clear register */		/*0x018*/
    t_uint32    gpio_slpm;                          /* GPIO Sleep mode register */					/*0x01C*/
    t_uint32    gpio_afsa;                          /* GPIO Alternate Function Select register A*/	/*0x020*/
    t_uint32    gpio_afsb;                          /* GPIO Alternate Function Select register B*/	/*0x024*/
    t_uint32    gpio_lowemi;                        /* GPIO Low EMI mode register*/                 /*0x28*/
    t_uint32    reserved_1[(0x040 - 0x02C) >> 2];   												
    t_uint32    gpio_rimsc;                         /* GPIO rising edge interrupt set/clear*/		/*0x040*/
    t_uint32    gpio_fimsc;                         /* GPIO falling edge interrupt set/clear register*//*0x044*/
    t_uint32    gpio_mis;                            /* GPIO masked interrupt status register*/		/*0x048*/
    t_uint32    gpio_ic;                            /* GPIO Interrupt Clear register*/				/*0x04C*/

    t_uint32    gpio_rwimsc;                        /* GPIO Rising-edge Wakeup IMSC register*/		/*0x050*/
    t_uint32    gpio_fwimsc;                        /* GPIO Falling-edge Wakeup IMSC register*/	    /*0x054*/
    t_uint32    gpio_wks;                           /* GPIO Wakeup Status register*/				/*0x058*/

    t_uint32    reserved_2[(0x7E0 - 0x05C) >> 2];

    t_uint32    gpio_itcr;                          /* Integration Test control register */			/*0x7E0*/
    t_uint32    gpio_itop;                          /* Intergration Test output register */			/*0x7E4*/
    t_uint32    reserved_3[(0xFE0 - 0x7E8) >> 2];

    t_uint32    gpio_periph_id_0;                   /* Peripheral identification register bits 7:0*//*0xFE0*/
    t_uint32    gpio_periph_id_1;                   /* Peripheral identification register bits 15:8*//*0xFE4*/
    t_uint32    gpio_periph_id_2;                   /* Peripheral identification register bits 23:16*//*0xFE8*/
    t_uint32    gpio_periph_id_3;                   /* Peripheral identification register bits 31:24*//*0xFEC*/
    t_uint32    gpio_pcell_id_0;                    /* Peripheral identification register bits 7:0*//*0xFF0*/
    t_uint32    gpio_pcell_id_1;                    /* Peripheral identification register bits 15:8*//*0xFF4*/
    t_uint32    gpio_pcell_id_2;                    /* Peripheral identification register bits 23:16*//*0xFF8*/
    t_uint32    gpio_pcell_id_3;                    /* Peripheral identification register bits 31:24*//*0xFFC*/
}t_gpio_register, *t_p_gpio_register;

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __INC_GPIOIRQP_H */

/* End of file - gpio_irqp.h */

