/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
#ifndef STXP70_OPINTERFACE_H_
#define STXP70_OPINTERFACE_H_

/**
 \defgroup STXP70 STXP70 core and interrupt controller interface 
 \detail This module provides an interface to the STXP70 core and interrupt controller registers.
         All functionalities related to the interrupt controller are managed by this module. 
*/

/**
 \file STXP70_OPInterface.h
 \brief This file is a part of the module release code. It provides an 
        interface to the STXP70 module. All functionalities offered by 
        the module are available through this file.
 \ingroup STXP70
*/

#include "stxp70_itc.h"


/// API to initialise an IRQ. It accepts the following arguments:
/// - ID 					: 	Specifies the IRQ number which is to be initialised
/// - Level 				: 	Specifies the level to be assigned to the IRQ
/// - Synchro				: 	If 1, interrupt input synchronization for IRQ ID is 
///								enabled else it is disabled
/// - Enable				:	If 1, then IRQ ID is enabled else it is disabled.
///								A disabled IRQ will not trigger an interrupt into
///								the core even if the corresponding event occurs.
/// - Trigger				:	If 1, then the IRQ status is marked as pending.
///								Normally used to software trigger an IRQ
#define STXP70_InitStaticIrq(ID, Level, Synchro, Enable, Trigger)	ITC_Init_StaticIrq(ID, Level, Synchro, Enable, Trigger)

/// API to set the Interrupt Mask Level (IML). 
/// The Interrupt Mask Level indicates the level of the interrupt 
/// being currently serviced, it is also the interrupt level above 
/// which the incoming interrupts are acknowledged by the STxP70 core.
/// When the core SR.IML is at its maximum value (0x1F), the maskable 
/// interrupts cannot be acknowledged by the STxP70 core, 
/// only a NMI can be acknowledged.
/// 
/// It accepts the following arguments:
///  - NewIml	:	The new interrupt mask level
#define STXP70_SetIml(NewIml)										STxP70_Set_Iml(NewIml)

/// API to read the 4 byte Maskable Interrupt Control (MIC) register  
/// corresponding to IRQ ID.
/// 
/// It accepts the following arguments:
///  - ID	:	Specifies the IRQ corresponding to which the MIC register 
///				is to be read.
#define STXP70_GetMic(ID)											_asm_get_it(ID)

/// API to write to the 4 byte Maskable Interrupt Control (MIC) register  
/// corresponding to IRQ ID.
/// 
/// It accepts the following arguments:
///  - ID		:	Specifies the IRQ corresponding to which the MIC register 
///					is to be written.
///  - conf		:	Specifies the value to be written to the MIC register 
///					corresponding to IRQ ID.
#define STXP70_SetMic(ID, conf)										_asm_set_it(ID, conf)

/// API to enable all the IRQs corresponding to the Mask in a single operation.
/// 
/// It accepts the following arguments:
/// - Mask		:	4 byte mask value. A bit position that is 1 in the mask 
///					will cause the IE bit of the MIC register of the corresponding   
///					IRQ number to be set. Bit positions that are 0 will 
///					have no impact to the corresponding IRQ.
#define STXP70_EnableInterrupts(Mask)								ITC_Disable_Enable(Mask, 1)

/// API to disable all the IRQs corresponding to the Mask in a single operation.
/// 
/// It accepts the following arguments:
///  - Mask		:	4 byte mask value. A bit position that is 1 in the mask 
///					will cause the IE bit of the MIC register of the corresponding   
///					IRQ number to be reset. Bit positions that are 0 will 
///					have no impact to the corresponding IRQ.
#define STXP70_DisableInterrupts(Mask)								ITC_Disable_Enable(Mask, 0)

/// API to trigger interrupt corresponding to IRQ ID. This API can be used to
/// trigger a software interrupt on an IRQ.
/// 
///	It accepts the following inputs:
/// - ID		:	Specifies the IRQ which needs to be triggered.
///
/// It must be noted that an IRQ that is not enabled will not
/// trigger an interrupt either.
#define STXP70_TriggerInterrupt(ID)									ITC_SET_PENDING(ID)

/// API to clear the interrupt status flag in the MIC register 
/// corresponding to IRQ ID. This API can be used to clear an 
/// IRQ without servicing it.
/// 
///	It accepts the following inputs:
/// - ID		:	Specifies the IRQ which needs to be cleared.
#define STXP70_ClearInterruptPending(ID)							ITC_CLEAR_PENDING(ID)

/// API to read the interrupt enable status of all the IRQs 
/// in a single instruction. 
/// 
/// It returns a 4 byte value. Each bit of this value
/// corresponds to the IE bit of the MIC register
/// of the corresponding IRQ.
#define STXP70_GetInterruptEnable()									_asm_get_it_enable()

#endif /*STXP70_OPINTERFACE_H_*/
