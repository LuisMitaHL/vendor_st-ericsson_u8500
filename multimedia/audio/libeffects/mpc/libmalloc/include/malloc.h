/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#ifndef __malloc_h__
#define __malloc_h__
#define __alloc_h__

/*
 * This should be declared before <alloc.h>
 * to avoid error:Cannot convert pointer to
 * '__ALL' into pointer to '__Y'
 */
#include "inc/type.h"
//#define __PRIVATE_ALLOC__ //to not do "#include "flexcc2/stwdsp.h"" in alloc.h
#include <stddef.h>
#include "inc/archi-wrapper.h"
#include "libeffects/mpc/include/memorybank.idt"
#include "audiolibs_trace.h"

// if DISABLE_PREEMPTION_DURING_MALLOC is defined, then all
// allocations/frees will be performed with interrupts masked

#ifdef DISABLE_PREEMPTION_DURING_MALLOC

#define M_ENTER_CRITICAL_SECTION ENTER_CRITICAL_SECTION
#define M_EXIT_CRITICAL_SECTION EXIT_CRITICAL_SECTION

#else

#define M_ENTER_CRITICAL_SECTION
#define M_EXIT_CRITICAL_SECTION

#endif

#ifndef _SIMU_
#define CUSTOM_CHECKSUM 0x5A5AUL
#else
#define CUSTOM_CHECKSUM 0
#endif

#define MEM_BLOC_SIZE   0x1000 //4Ko by block
#define MEM_BANK_16BITS 1
#define MEM_BANK_24BITS 0

#define MEM_STATUS_MASK 0x00F
#define MEM_BLOC_MASK   0x0F0
#define MEM_STACK_MASK  0xF00

typedef enum
{
    DATA_MEM = 0x0,         // => 0x000
    STACK_MEM = 0x100       // => 0x100

} Mem_Stack_Enum;


#define FREE_SUCCESS 0
#define FREE_FAILED -1
#define INVALIDE_ALLOCATED_MEM_ADDR -3
#define DISABLED_EXTMEM_ALLOC -4
#define INVALID_ADDR -5
#define NB_MEMORY_SPACE 6

// Typedef
typedef enum
{
    // !!! only on low byte
  MEM_FREE = 0,      //   for Mem and Segment. => 0x00
  MEM_USED = 0x03    // 3 for Mem.             => 0x03
} Mem_Status_Enum;   //!< Enum for memory blocks.

#define MEM_BANK_TO_MEM_BLOCK(mem) (mem<<4)

typedef enum
{
    // !!! only on high byte
    X_MEM =     MEM_BANK_TO_MEM_BLOCK(MEM_XTCM),
    Y_MEM =     MEM_BANK_TO_MEM_BLOCK(MEM_YTCM),
    EXT_MEM24 = MEM_BANK_TO_MEM_BLOCK(MEM_DDR24),
    EXT_MEM16 = MEM_BANK_TO_MEM_BLOCK(MEM_DDR16),
    ESRAM_24 =  MEM_BANK_TO_MEM_BLOCK(MEM_ESR24),
    ESRAM_16 =  MEM_BANK_TO_MEM_BLOCK(MEM_ESR16)
} Mem_Bloc;


/*!
\struct Z_Mem_Struct
Memory control block.
*/
typedef struct Mem_S{
  size_t size;            //!< Size of the memory block.
  unsigned int  custom;   //!< could be the id of the thread which is allocated this memory block.
  struct Mem_S *prev;     //!< Pointer to the previous contiguous memory block.
  struct Mem_S *next;     //!< Pointer to the next contiguous memory block.
  Mem_Status_Enum status; //!< [Z_FREE / Z_USED] AND [Z_X / Z_Y] AND [Z_DATA / Z_STACK]
} Mem_Struct;             //!< Mem structure.

extern Mem_Struct * heap_ptr_tab[NB_MEMORY_SPACE];
extern char * mem_name_tab[NB_MEMORY_SPACE];

extern unsigned int mem_Size_Left[NB_MEMORY_SPACE];
extern unsigned int mem_Size_Init[NB_MEMORY_SPACE];

#define heap_ptr         heap_ptr_tab[MEM_XTCM]
#define last_Y_ptr       heap_ptr_tab[MEM_YTCM]

#define heap_Ext24_ptr   heap_ptr_tab[MEM_DDR24]
#define heap_Ext16_ptr   heap_ptr_tab[MEM_DDR16]

#define heap_Esram24_ptr heap_ptr_tab[MEM_ESR24]
#define heap_Esram16_ptr heap_ptr_tab[MEM_ESR16]

extern void *Mem_Alloc_Ptr(unsigned int custom, size_t size, int Bloc_stack);

extern int Mem_Free(void *mem);
/*
 * Set available X memories area for malloc use.
 * @param pid: the current thread's Id. When in the stand alone mode, the pid is 0.
 */ 
extern void Init_X(void * available_mem_addr, size_t init_size);

/*
 * Set the custom field of malloc bloc at malloc initialization for the X memory.
 * The default value of custom field is 0 set by the init_alloc();
 * This function is mainly used by ZeOS to initialize custiom field with current thread ID.
 */
extern void Set_init_custom_X(unsigned int custom);

#ifdef Y_ALLOC
/*
 * Set available Y memories area for malloc use.
 * @param pid: the current thread's Id. When in the stand alone mode, the pid is 0.
 */
extern void Init_Y(void * available_mem_addr, size_t init_size);

/*
 * Set the custom field of malloc bloc at malloc initialization for the Y memory.
 * The default value of custom field is 0 set by the init_alloc();
 * This function is mainly used by ZeOS to initialize custiom field with current thread ID.
 */
extern void Set_init_custom_Y(unsigned int custom);
#endif

#if defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
// used for init extmem24 and esram24
extern void Init_ext_mem_24(Mem_Struct ** mem_block_ptr, void * available_mem_addr, size_t init_size, Mem_Bloc bloc, unsigned int custom);
#endif 

#if defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
//use for init extmem16 and esram16
extern void Init_ext_mem_16(Mem_Struct ** mem_block_ptr, void * available_mem_addr, size_t init_size, Mem_Bloc bloc, unsigned int custom);
#endif

#ifdef EXTMEM_ALLOC
/*
 * Set the custom field of malloc bloc at malloc initialization for the 24bits SDRAM memory.
 * The default value of custom field is 0 set by the init_alloc();
 * This function is mainly used by ZeOS to initialize custiom field with current thread ID.
 */
extern void Set_init_custom_ext(unsigned int custom);

/*
 * Set the custom field of malloc bloc at malloc initialization for the 16bits SDRAM memory.
 * The default value of custom field is 0 set by the init_alloc();
 * This function is mainly used by ZeOS to initialize custiom field with current thread ID.
 */
extern void Set_init_custom_ext16(unsigned int custom);
#endif //EXTMEM_ALLOC

#ifdef ESRAM_ALLOC
/*
 * Set the custom field of malloc bloc at malloc initialization for the 24bits ESRAM memory.
 * The default value of custom field is 0 set by the init_alloc();
 * This function is mainly used by ZeOS to initialize custiom field with current thread ID.
 */
extern void Set_init_custom_esram(unsigned int custom);

/*
 * Set the custom field of malloc bloc at malloc initialization for the 16bits ESRAM memory.
 * The default value of custom field is 0 set by the init_alloc();
 * This function is mainly used by ZeOS to initialize custiom field with current thread ID.
 */
extern void Set_init_custom_esram16(unsigned int custom);
#endif //ESRAM_ALLOC

/*
 * Free the blocs who fit conditions:
 * 1. allcoated as STACK_MEM
 * 2. the custom field contains the given custom value "custom"
 * 3. in the memory "mem" (X, Y, SDRAM, ESRAM)
 */
extern void Free_stack_blocs(unsigned int custom, Mem_Bloc mem);

/*
 * This function updates the "tmp_ptr" with the address
 * of the given memory bank and return 16_BITS_MEM_BANK
 * if "mem" indicates a 16 bits memory bank.
 */
extern unsigned int * mattach_to_mem_bank(Mem_Bloc mem, Mem_Struct ** tmp_ptr);

/*
 * This function resets all the heap pointers, must
 * be called once for all  before any allocation
 */
void init_alloc(void);

extern size_t mget_max_free_space(Mem_Bloc mem);

extern size_t mget_mem_size_left(Mem_Bloc mem);


#endif //__malloc.h__
