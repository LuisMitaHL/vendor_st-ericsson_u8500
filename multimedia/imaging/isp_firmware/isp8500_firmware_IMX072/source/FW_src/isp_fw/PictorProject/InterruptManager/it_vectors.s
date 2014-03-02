//******************************************************************************
//*                                                                            *
//*        Copyright 2005, STMicroelectronics, Incorporated.                   *
//*        All rights reserved                                                 *
//*                                                                            *
//*          STMICROELECTRONICS, INCORPORATED PROPRIETARY INFORMATION          *
//* This software is supplied under the terms of a license agreement or        *
//* nondisclosure agreement with STMicroelectronics and may not be copied or   *
//* disclosed except in accordance with the terms of that agreement.           *
//*                                                                            *
//******************************************************************************
//* Comments:                                                                  *
//*                                                                            *
//*    ivb register is initialized to __IVBR_BASE by boot. According to default*
//* link script, this symbol is defined as the start address of the .ivtable   *
//* (interrupt vector table) section. A sample ivtable is as follows:          *
//*                                                                            *
//******************************************************************************



/**
 \file 		it_vectors.s

 \brief		This file is NOT a part of the module release code.
 			It contains a section declaration ".ivtable" that has
 			the interrupt vector entries of all the IRQs.
 			The system integrator must ensure that the file is present
 			at the project level for the STXP70 interrupt mechanism
 			to work properly.

 \details	If there are N interrupts instantiated in the STXP70 core,
 			then the file must define a ".ivtable" section in the
 			manner shown below:

 			.section ".ivtable", .text

			VECTOR name_of_interrupt_service_routine_0

			VECTOR name_of_interrupt_service_routine_1

			VECTOR name_of_interrupt_service_routine_2

			VECTOR name_of_interrupt_service_routine_3

			VECTOR ...

			VECTOR ...

			VECTOR ...

			VECTOR name_of_interrupt_service_routine_N

 \ingroup STXP70
*/


// ****************************************************************************
// Macro to define a vector (jump to a weak label)
// ****************************************************************************
.macro VECTOR label
    jr %rel2to23(label)
.endm
// ****************************************************************************
// Foolowing handler can be used if one wants to ignore some interrupts
// ****************************************************************************
.macro DO_NOTHING_VECTOR
    rte
.endm

  .section ".ivtable", .text

	VECTOR UserInterface_ISR			// 00 Host Comms
	VECTOR STAT8_GLACE_SCHEDULE_ISR			// 01 Glace Schedule
	VECTOR MasterCCI_ISR				// 02 i2c
	VECTOR Line_Blanking_Elimination_ISR	        // 03 Line blanking elimination
	VECTOR DefaultHandler				// 04 reserved
	VECTOR Smia_Rx_ISR				// 05 Smia line count at Rx
	VECTOR Video_Pipe0_ISR				// 06 Vid0
	VECTOR Video_Pipe1_ISR				// 07 Vid1
	VECTOR STAT0_256_bins_histogram_AEC_ISR	        // 08 256-histogram AEC
	VECTOR STAT1_Acc_6x8_Exposure			// 09 EXP6X8: Exposure ISR
	VECTOR STAT2_Auto_Focus_ISR			// 10 Auto focus
	VECTOR STAT3_White_balance_ISR			// 11 MWWB - minimum weighted white balance
	VECTOR STAT4_Skin_tone_detection_ISR	        // 12 Skin tone detection
	VECTOR DMCE_ISR				// 13 DMCE_ISR
	VECTOR DefaultHandler				// 14 Duster
 	VECTOR STAT7_ACC_Wg_Zones_ISR			// 15 Weighted White balance
	VECTOR STAT8_GLACE_AWB_ISR			// 16 Glace AWB
	VECTOR ITM_GPIO_ISR  				// 17 GPIO
	VECTOR DefaultHandler				// 18
	VECTOR DefaultHandler				// 19
	VECTOR Histogram_AEC_SCHEDULE_ISR		// 20 Histogram Schedule
	VECTOR DefaultHandler				// 21
	VECTOR DefaultHandler				// 22
	VECTOR ClockResetManager_ISR			// 23 CRM
	VECTOR DefaultHandler				// 24
	VECTOR DefaultHandler				// 25
	VECTOR DefaultHandler				// 26
	VECTOR ITM_ERROR_EOF_ISR			// 27 ERROR and EOF
	VECTOR DefaultHandler				// 28
	VECTOR DefaultHandler				// 29
	VECTOR DefaultHandler				// 30
	VECTOR NMIHandler				// 31


