/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  Private Header file of SKE
*  Contains the SKE Hardware IP specific data like registers,macros for writing/reading register
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __SKE_HWP_HEADER
#define __SKE_HWP_HEADER

#include "hcl_defs.h"

typedef volatile struct
{
    t_uint32 ske_cr;                         /* Control register */
    t_uint32 ske_val0;                       /* Scroll Key 0 Value register */
    t_uint32 ske_val1;                       /* Scroll Key 1 Value register */
    t_uint32 ske_dbcr;                       /* Scroll Key & Key Pad Debounce register */
    t_uint32 ske_imsc;                       /* Interrupt mask set and clear register */
    t_uint32 ske_ris;                        /* Raw interrupt status register */
    t_uint32 ske_mis;                        /* Masked interrupt status register */
    t_uint32 ske_icr;                        /* Interrupt clear register */
    t_uint32 ske_asr0;                       /* Key Pad Autoscan Result register 0 */
    t_uint32 ske_asr1;                       /* Key Pad Autoscan Result register 1 */
    t_uint32 ske_asr2;                       /* Key Pad Autoscan Result register 2 */
    t_uint32 ske_asr3;                       /* Key Pad Autoscan Result register 3 */
    t_uint32 reserved1[(0x080-0x030) >> 2];   /* Reserved region */
    t_uint32 ske_itcr;                       /* Integration Test Control register */
    t_uint32 ske_itip;                       /* Integration Test Input register */
    t_uint32 ske_itop;                       /* Integration Test Output register */
    t_uint32 reserved2[(0xFE0-0x08C) >> 2];   /* Reserved region */
    t_uint32 ske_periphid0;                  /* Peripheral identifcation register 0 */
    t_uint32 ske_periphid1;                  /* Peripheral identifcation register 1 */
    t_uint32 ske_periphid2;                  /* Peripheral identifcation register 2 */
    t_uint32 ske_periphid3;                  /* Peripheral identifcation register 3 */
    t_uint32 ske_pcellid0;                   /* PrimeCell identifcation register 0 */
    t_uint32 ske_pcellid1;                   /* PrimeCell identifcation register 1 */
    t_uint32 ske_pcellid2;                   /* PrimeCell identifcation register 2 */
    t_uint32 ske_pcellid3;                   /* PrimeCell identifcation register 3 */
}t_ske_register;


/*---------------- Macros for Control Register bitfield-------------- */

/* Bit Masks */
#define SKE_SKEN0_BIT_MASK	 MASK_BIT0 /* Scroll Key 0 Enable bit */
#define SKE_SKEN1_BIT_MASK	 MASK_BIT0 /* Scroll Key 1 Enable bit */
#define SKE_KPASEN_BIT_MASK	 MASK_BIT0 /* Keypad AutoScan Enable bit */
#define SKE_KPCN_BIT_MASK    0x07 /* Key Pad Column Number bits */
#define SKE_KPMLT_BIT_MASK	 MASK_BIT0 /* Keypad MultiKey Press Enable bit */
#define SKE_KPASON_BIT_MASK	 MASK_BIT0 /* Keypad AutoScan on-going status bit */
#define SKE_KPOC0_BIT_MASK   MASK_BIT0  /* Keypad Output Drive Column 0 */
#define SKE_KPOC1_BIT_MASK   MASK_BIT0  /* Keypad Output Drive Column 1 */
#define SKE_KPOC2_BIT_MASK   MASK_BIT0  /* Keypad Output Drive Column 2 */
#define SKE_KPOC3_BIT_MASK   MASK_BIT0  /* Keypad Output Drive Column 3 */
#define SKE_KPOC4_BIT_MASK   MASK_BIT0  /* Keypad Output Drive Column 4 */
#define SKE_KPOC5_BIT_MASK   MASK_BIT0  /* Keypad Output Drive Column 5 */
#define SKE_KPOC6_BIT_MASK   MASK_BIT0  /* Keypad Output Drive Column 6 */
#define SKE_KPOC7_BIT_MASK   MASK_BIT0  /* Keypad Output Drive Column 7 */
#define SKE_KPOC_BIT_MASK    MASK_BYTE1 /* Keypad Output Drive          */

/*---------------- Macros for Value Register 0 and 1-------------- */

#define SKE_SKEVAL0_BIT_MASK	 MASK_BYTE0  	/* Scroll Key 0 Count */
#define SKE_SKEVAL1_BIT_MASK	 MASK_BYTE0  	/* Scroll Key 1 Count */
#define SKE_OVF0_BIT_MASK	 	 MASK_BIT0  	/* Scroll Key 0 Overflow flag bit */
#define SKE_OVF1_BIT_MASK		 MASK_BIT0  	/* Scroll Key 1 Overflow flag bit */
#define SKE_UDF0_BIT_MASK	 	 MASK_BIT0  	/* Scroll Key 0 Underflow flag bit */
#define SKE_UDF1_BIT_MASK	 	 MASK_BIT0 		/* Scroll Key 1 Underflow flag bit */

/*---------------- Macros for Debounce Register-------------- */

#define SKE_SKEDBC_BIT_MASK	 MASK_BYTE0  /* Scroll Key Debounce */
#define SKE_KPDBC_BIT_MASK	 MASK_BYTE0  /* Keypad Debounce */

/*---------------- Macros for Auto Scan Registers------------- */

#define SKE_KPRVAL0_BIT_MASK   MASK_BYTE0  /* Keypad Autoscan Result for column 0 */
#define SKE_KPRVAL1_BIT_MASK   MASK_BYTE0  /* Keypad Autoscan Result for column 1 */
#define SKE_KPRVAL2_BIT_MASK   MASK_BYTE0  /* Keypad Autoscan Result for column 2 */
#define SKE_KPRVAL3_BIT_MASK   MASK_BYTE0  /* Keypad Autoscan Result for column 3 */
#define SKE_KPRVAL4_BIT_MASK   MASK_BYTE0  /* Keypad Autoscan Result for column 4 */
#define SKE_KPRVAL5_BIT_MASK   MASK_BYTE0  /* Keypad Autoscan Result for column 5 */
#define SKE_KPRVAL6_BIT_MASK   MASK_BYTE0  /* Keypad Autoscan Result for column 6 */
#define SKE_KPRVAL7_BIT_MASK   MASK_BYTE0  /* Keypad Autoscan Result for column 7 */

/*---------------- Macros for Interrupt Registers------------- */

#define SKE_SKEIM0_BIT_MASK	 MASK_BIT0   /* Scroll Key 0 Interrupt Mask */ 
#define SKE_SKEIM1_BIT_MASK	 MASK_BIT0   /* Scroll Key 1 Interrupt Mask */
#define SKE_KPIMA_BIT_MASK	 MASK_BIT0   /* Keypad Autoscan Interrupt Mask */
#define SKE_KPIMS_BIT_MASK	 MASK_BIT0	 /* Keypad Softscan Interrupt Mask */

/*---------------------------------------------------------------------------*/

/*---------------- Macros for Raw Interrupt Status Registers------------- */
#define SKE_KPRISS_BIT_MASK	 MASK_BIT0
#define SKE_KPRISA_BIT_MASK  MASK_BIT0

/*---------------------------------------------------------------------------*/

/* Bit field Shifts */

/*------------------ Bit field Shifts for Control Registers---------------- */
#define SKE_SKEN0_SHIFT	 	0  	/* Scroll Key 0 Enable bit */
#define SKE_SKEN1_SHIFT	 	1  	/* Scroll Key 1 Enable bit */
#define SKE_KPASEN_SHIFT  	2 	/* Keypad AutoScan Enable bit */
#define SKE_KPCN_SHIFT   	3 	/* Key Pad Column Number bits */
#define SKE_KPMLT_SHIFT  	6  	/* Keypad MultiKey Press Enable bit */
#define SKE_KPASON_SHIFT  	7 	/* Keypad AutoScan on-going status bit */
#define SKE_KPOC0_SHIFT   	8 	/* Keypad Output Drive Column 0 */
#define SKE_KPOC1_SHIFT   	9 	/* Keypad Output Drive Column 1 */
#define SKE_KPOC2_SHIFT   	10 	/* Keypad Output Drive Column 2 */
#define SKE_KPOC3_SHIFT   	11 	/* Keypad Output Drive Column 3 */
#define SKE_KPOC4_SHIFT   	12 	/* Keypad Output Drive Column 4 */
#define SKE_KPOC5_SHIFT   	13 	/* Keypad Output Drive Column 5 */
#define SKE_KPOC6_SHIFT   	14 	/* Keypad Output Drive Column 6 */
#define SKE_KPOC7_SHIFT   	15 	/* Keypad Output Drive Column 7 */
#define SKE_KPOC_SHIFT      0   /* Keypad Output Drive Column   */

/*------------------ Bit field Shifts for Value Register 0 and 1---------------- */

#define SKE_SKEVAL0_SHIFT	 0 /* Scroll Key 0 Count */
#define SKE_SKEVAL1_SHIFT	 0 /* Scroll Key 1 Count */ 
#define SKE_UDF0_SHIFT	 	14  /* Scroll Key 0 Underflow flag bit */
#define SKE_UDF1_SHIFT	 	14  /* Scroll Key 1 Underflow flag bit */
#define SKE_OVF0_SHIFT	 	15  /* Scroll Key 0 Overflow flag bit */
#define SKE_OVF1_SHIFT	 	15  /* Scroll Key 1 Overflow flag bit */

/*------------------ Bit field Shifts for Debounce Register---------------- */

#define SKE_SKEDBC_SHIFT	0  /* Scroll Key Debounce */
#define SKE_KPDBC_SHIFT	 	8 		/* Keypad Debounce */

/*------------------ Bit field Shifts for Auto scan Registers---------------- */

#define SKE_KPRVAL0_SHIFT   0  /* Keypad Autoscan Result for column 0 */
#define SKE_KPRVAL1_SHIFT   8  /* Keypad Autoscan Result for column 1 */
#define SKE_KPRVAL2_SHIFT   0  /* Keypad Autoscan Result for column 2 */
#define SKE_KPRVAL3_SHIFT   8  /* Keypad Autoscan Result for column 3 */
#define SKE_KPRVAL4_SHIFT   0  /* Keypad Autoscan Result for column 4 */
#define SKE_KPRVAL5_SHIFT   8  /* Keypad Autoscan Result for column 5 */
#define SKE_KPRVAL6_SHIFT   0  /* Keypad Autoscan Result for column 6 */
#define SKE_KPRVAL7_SHIFT   8  /* Keypad Autoscan Result for column 7 */

/*------------------ Bit field Shifts for Interrupt Registers---------------- */

#define SKE_SKEIM0_SHIFT	0  /* Scroll Key 0 Interrupt Mask */ 
#define SKE_SKEIM1_SHIFT	1  /* Scroll Key 1 Interrupt Mask */
#define SKE_KPIMA_SHIFT	 	2   /* Keypad Autoscan Interrupt Mask */
#define SKE_KPIMS_SHIFT	 	3   /* Keypad Softscan Interrupt Mask */

/*---------------- Bit field Shifts for Raw Interrupt Status Registers------------- */
#define SKE_KPRISS_SHIFT	 3
#define SKE_KPRISA_SHIFT     2

/*---------------------------------------------------------------------------*/


/* Bit field Masks */

/*------------------ Bit field Masks for Control Register---------------- */

#define SKE_SKEN0_MASK	(SKE_SKEN0_BIT_MASK << SKE_SKEN0_SHIFT)  /* Scroll Key 0 Enable bit */
#define SKE_SKEN1_MASK  (SKE_SKEN1_BIT_MASK << SKE_SKEN1_SHIFT)  /* Scroll Key 1 Enable bit */
#define SKE_KPASEN_MASK  (SKE_KPASEN_BIT_MASK << SKE_KPASEN_SHIFT) /* Keypad AutoScan Enable bit */
#define SKE_KPCN_MASK	 (SKE_KPCN_BIT_MASK << SKE_KPCN_SHIFT) /* Key Pad Column Number bits */
#define SKE_KPMLT_MASK  (SKE_KPMLT_BIT_MASK << SKE_KPMLT_SHIFT) /* Keypad MultiKey Press Enable bit */
#define SKE_KPASON_MASK  (SKE_KPASON_BIT_MASK << SKE_KPASON_SHIFT) /* Keypad AutoScan on-going status bit */
#define SKE_KPOC0_MASK  (SKE_KPOC0_BIT_MASK << SKE_KPOC0_SHIFT) /* Keypad Output Drive Column 0 */
#define SKE_KPOC1_MASK  (SKE_KPOC1_BIT_MASK << SKE_KPOC1_SHIFT) /* Keypad Output Drive Column 1 */
#define SKE_KPOC2_MASK  (SKE_KPOC2_BIT_MASK << SKE_KPOC2_SHIFT) /* Keypad Output Drive Column 2 */
#define SKE_KPOC3_MASK  (SKE_KPOC3_BIT_MASK << SKE_KPOC3_SHIFT) /* Keypad Output Drive Column 3 */
#define SKE_KPOC4_MASK  (SKE_KPOC4_BIT_MASK << SKE_KPOC4_SHIFT) /* Keypad Output Drive Column 4 */
#define SKE_KPOC5_MASK  (SKE_KPOC5_BIT_MASK << SKE_KPOC5_SHIFT) /* Keypad Output Drive Column 5 */
#define SKE_KPOC6_MASK  (SKE_KPOC6_BIT_MASK << SKE_KPOC6_SHIFT) /* Keypad Output Drive Column 6 */
#define SKE_KPOC7_MASK  (SKE_KPOC7_BIT_MASK << SKE_KPOC7_SHIFT) /* Keypad Output Drive Column 7 */
#define SKE_KPOC_MASK   (SKE_KPOC_BIT_MASK << SKE_KPOC_SHIFT)   /* Keypad Output Drive Column   */

/*------------------ Bit field Masks for Value Register 0 and 1---------------- */
#define SKE_SKEVAL0_MASK	(SKE_SKEVAL0_BIT_MASK << SKE_SKEVAL0_SHIFT) /* Scroll Key 0 Count */
#define SKE_SKEVAL1_MASK  (SKE_SKEVAL1_BIT_MASK << SKE_SKEVAL1_SHIFT)  /* Scroll Key 1 Count */
#define SKE_OVF0_MASK	 (SKE_OVF0_BIT_MASK << SKE_OVF0_SHIFT)  /* Scroll Key 0 Overflow flag bit */
#define SKE_OVF1_MASK	 (SKE_OVF1_BIT_MASK << SKE_OVF1_SHIFT)  /* Scroll Key 1 Overflow flag bit */
#define SKE_UDF0_MASK	 (SKE_UDF0_BIT_MASK << SKE_UDF0_SHIFT)  /* Scroll Key 0 Underflow flag bit */
#define SKE_UDF1_MASK	 (SKE_UDF1_BIT_MASK << SKE_UDF1_SHIFT)  /* Scroll Key 1 Underflow flag bit */

/*------------------ Bit field Masks for Debounce Register---------------- */

#define SKE_SKEDBC_MASK	(SKE_SKEDBC_BIT_MASK << SKE_SKEDBC_SHIFT) /* Scroll Key Debounce */
#define SKE_KPDBC_MASK  (SKE_KPDBC_BIT_MASK << SKE_KPDBC_SHIFT) /* Keypad Debounce */

/*------------------ Bit field Masks for Auto scan Registers---------------- */

#define SKE_KPRVAL0_MASK   (SKE_KPRVAL0_BIT_MASK << SKE_KPRVAL0_SHIFT)  /* Keypad Autoscan Result for column 0 */
#define SKE_KPRVAL1_MASK   (SKE_KPRVAL1_BIT_MASK << SKE_KPRVAL1_SHIFT)  /* Keypad Autoscan Result for column 1 */
#define SKE_KPRVAL2_MASK   (SKE_KPRVAL2_BIT_MASK << SKE_KPRVAL2_SHIFT)  /* Keypad Autoscan Result for column 2 */
#define SKE_KPRVAL3_MASK   (SKE_KPRVAL3_BIT_MASK << SKE_KPRVAL3_SHIFT)  /* Keypad Autoscan Result for column 3 */
#define SKE_KPRVAL4_MASK   (SKE_KPRVAL4_BIT_MASK << SKE_KPRVAL4_SHIFT)  /* Keypad Autoscan Result for column 4 */
#define SKE_KPRVAL5_MASK   (SKE_KPRVAL5_BIT_MASK << SKE_KPRVAL5_SHIFT)  /* Keypad Autoscan Result for column 5 */
#define SKE_KPRVAL6_MASK   (SKE_KPRVAL6_BIT_MASK << SKE_KPRVAL6_SHIFT)  /* Keypad Autoscan Result for column 6 */
#define SKE_KPRVAL7_MASK   (SKE_KPRVAL7_BIT_MASK << SKE_KPRVAL7_SHIFT)  /* Keypad Autoscan Result for column 7 */

/*------------------ Bit field Masks for Interrupt Registers---------------- */

#define SKE_SKEIM0_MASK	 (SKE_SKEIM0_BIT_MASK << SKE_SKEIM0_SHIFT)  /* Scroll Key 0 Interrupt Mask */ 
#define SKE_SKEIM1_MASK	 (SKE_SKEIM1_BIT_MASK << SKE_SKEIM1_SHIFT)   /* Scroll Key 1 Interrupt Mask */
#define SKE_KPIMA_MASK	 (SKE_KPIMA_BIT_MASK << SKE_KPIMA_SHIFT)   /* Keypad Autoscan Interrupt Mask */
#define SKE_KPIMS_MASK	 (SKE_KPIMS_BIT_MASK << SKE_KPIMS_SHIFT)   /* Keypad Softscan Interrupt Mask */
#define SKE_COMBINED_IRQ_MASK 0x0F /* Mask for all above three Interrupts */

#define SKE_SKEIC0_MASK	SKE_SKEIM0_MASK	
#define SKE_SKEIC1_MASK	SKE_SKEIM1_MASK
#define SKE_KPICA_MASK	SKE_KPIMA_MASK 
#define SKE_KPICS_MASK	SKE_KPIMS_MASK 	/* Keypad Softscan Interrupt Mask */

#define SKE_SKEMIS0_MASK	SKE_SKEIM0_MASK	
#define SKE_SKEMIS1_MASK	SKE_SKEIM1_MASK
#define SKE_KPMISA_MASK	    SKE_KPIMA_MASK /* Keypad Autoscan Interrupt Mask */
#define SKE_KPMISS_MASK	    SKE_KPIMS_MASK /* Keypad Softscan Interrupt Mask */

/*------------------ Bit field Masks for Raw Interrupt Status Registers---------------- */
#define SKE_KPRISS_MASK  (SKE_KPRISS_BIT_MASK << SKE_KPRISS_SHIFT)
#define SKE_KPRISA_MASK  (SKE_KPRISA_BIT_MASK << SKE_KPRISA_SHIFT)

#endif /* __SKE_HWP_HEADER */


