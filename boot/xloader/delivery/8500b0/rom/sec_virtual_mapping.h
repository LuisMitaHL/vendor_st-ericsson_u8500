/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
	\file       sec_virtual_mapping.h
	\brief      This module contains the description of the logical of
	            the secure-side.
 
	\author     STMicroelectronics

 */
/** \internal
 *  \addtogroup SEC_INTERNALS
 * @{
 */
/*----------------------------------------------------------------------*/

#ifndef SEC_VIRTUAL_MAPPING_H
#define SEC_VIRTUAL_MAPPING_H

//--------------------------------------------------------------------
// Virtual address mapping secure-side
//--------------------------------------------------------------------

// These values must be always aligned with values set in secure-side 
// scatter file and also with MMU descriptors in mmu_mgt.c file

//--------------------------------------------------------------------
// Secure eSRAM mapping
//--------------------------------------------------------------------
		
    // free memory area base address in secure eSRAM		
    #define SEC_VIRT_AREA_BASE              0x40007000
    #define SEC_VIRT_AREA_TOP               0x40010FFF
        
    // virtual base address of the secure eSRAM
    #define SEC_VIRT_SEC_ESRAM_BASE         0x40000000
    
    // virtual addresses of ROM code variables and L2 MMU tables    
    #define SEC_VIRT_MMU_L2_BASE            0x40000000
    
    // virtual base address of the initial heap 
    #define SEC_VIRT_INIT_HEAP_BASE         0x40002000
    
    // virtual top address of the initial heap 
    #define SEC_VIRT_INIT_HEAP_TOP          0x40003FFF
    
    // Size of initial secure heap
    #define SEC_INIT_HEAP_SIZE              0x00002000
    
    // virtual base address of the initial stack 
    #define SEC_VIRT_INIT_STACK_BASE        0x40005000
    
    // virtual top address of the initial stack 
    #define SEC_VIRT_INIT_STACK_TOP         0x40006FFF
    
    // Size of initial secure stack
    #define SEC_INIT_STACK_SIZE             0x00002000    
    
//--------------------------------------------------------------------
// Public eSRAM mapping
//--------------------------------------------------------------------    
    
    // virtual base address of the public eSRAM
    #define SEC_VIRT_PUB_ESRAM_START_ADDR   0xA0000000    
	#define	SEC_VIRT_PUB_ESRAM_END_ADDR     (SEC_VIRT_PUB_ESRAM_START_ADDR+BOOT_RAM_LENGTH-1)    
	
//--------------------------------------------------------------------
// Secure backup RAM mapping
//--------------------------------------------------------------------    
    
// virtual base address of the public eSRAM
#define SEC_VIRT_SEC_BACKUP_START_ADDR   0xA0800000    
#define	SEC_VIRT_SEC_BACKUP_END_ADDR     (SEC_VIRT_SEC_BACKUP_START_ADDR+SEC_BACKUP_RAM_LENGTH-1)	

#endif		// SEC_VIRTUAL_MAPPING_H

/* end of sec_virtual_mapping.h file */
/** @} */
