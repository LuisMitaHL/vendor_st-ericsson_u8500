/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of AB8500 Core interrupt support
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef __INC_AB8500_CORE_IRQ_H
#define __INC_AB8500_CORE_IRQ_H

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/

	
/*------------------------------------------------------------------------
 * Structures and enums
 *----------------------------------------------------------------------*/
typedef enum
{
    IRQ_AB8500_CORE_OK                            = HCL_OK,
    IRQ_SER_AB8500_CORE_ERROR                     = HCL_ERROR,
    IRQ_SER_AB8500_CORE_UNSUPPORTED_FEATURE       = HCL_UNSUPPORTED_FEATURE,
    IRQ_SER_AB8500_CORE_INVALID_PARAMETER         = HCL_INVALID_PARAMETER,
    IRQ_SER_AB8500_CORE_TRANSACTION_ON_SPI_FAILED = HCL_MAX_ERROR_VALUE - 0
}t_irq_ab8500_core_error;
	
	
/* Ab8500 Core Mask Interrupts */
typedef enum
{
    AB8500_CORE_MASK1_REG  = 0x40,
    AB8500_CORE_MASK2_REG  = 0x41,
    AB8500_CORE_MASK3_REG  = 0x42,
    AB8500_CORE_MASK4_REG  = 0x43,
    AB8500_CORE_MASK5_REG  = 0x44,
    AB8500_CORE_MASK6_REG  = 0x45,
    AB8500_CORE_MASK7_REG  = 0x46,
    AB8500_CORE_MASK8_REG  = 0x47,
    AB8500_CORE_MASK9_REG  = 0x48,
    AB8500_CORE_MASK10_REG = 0x49,
    AB8500_CORE_MASK11_REG = 0x4A,
    AB8500_CORE_MASK12_REG = 0x4B,
    AB8500_CORE_MASK13_REG = 0x4C,
    AB8500_CORE_MASK14_REG = 0x4D,
    AB8500_CORE_MASK20_REG = 0x53,
    AB8500_CORE_MASK21_REG = 0x54,
    AB8500_CORE_MASK22_REG = 0x55
}t_ab8500_core_mask_reg;


/*------------------------------------------------------------------------
 * Functions declaration
 *----------------------------------------------------------------------*/

PUBLIC t_irq_ab8500_core_error AB8500_CORE_EnableIRQSrc(t_ab8500_core_mask_reg , t_uint8 );
PUBLIC t_irq_ab8500_core_error AB8500_CORE_DisableIRQSrc(t_ab8500_core_mask_reg , t_uint8 );
#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __INC_AB8500_CORE_IRQ_H */

/* End of file - ab8500_core_irq.h */

