/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mem.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/

/*
 * This should be declared before <alloc.h>
 * to avoid error:Cannot convert pointer to
 * '__ALL' into pointer to '__Y'
 */
#include <archi.h>
// YMEM must be redefined after including archi.h in order to avoid error:
// Cannot convert pointer to
// '__ALL' into pointer to '__Y' with vmalloc_y function
#undef YMEM
#define YMEM
#include <libeffects/mpc/libmalloc.nmf>
#include "malloc.h"
#include "fsm/generic/include/FSM.h"

#include "dsp_mem_map.h"

#ifndef _SIMU_
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "libeffects_mpc_libmalloc_src_memTraces.h"
#endif
#endif

static int init_done;
char * mem_name_tab[NB_MEMORY_SPACE] = {
    "XMEM",
    "YMEM",
    "EXTMEM24",
    "EXTMEM16",
    "ESRAM24",
    "ESRAM16"
};

/*
 * Set available internal memories area for malloc use :
 * @param P_extmemX_start :the start address (MMDSP memory addressing) of available internal X memory
 * @param sz_intmemX : the size (in MMDSP MAU) of the available internal X memory
 * @param p_intmemY_start : the start address (MMDSP memory addressing) of available internal Y memory
 * @param sz_intmemY : the size (in MMDSP MAU) of the available internal Y memory
 */
int init_intmem(void * p_intmemX_start, size_t sz_intmemX, void * p_intmemY_start, size_t sz_intmemY)
{
    if (init_done == 0) {
        init_alloc();
        init_done = 1;
    }

    // if the provided addresses are non-null
    // check that they are consistant
    // then init heap
    if (p_intmemX_start && sz_intmemX)
    {
        if (((((unsigned long)p_intmemX_start)+sz_intmemX) > (U8500_TCM_BASE_ADDRESS + U8500_TCM_SIZE)))
        {
            OstTraceInt0(TRACE_ERROR, "AFM_MPC: init_intmem: invalid XMEM settings");
            return MEM_INVALID_ADDRESS;
        }
        else
        {
            Init_X(p_intmemX_start, sz_intmemX);
        }
    }

    if (p_intmemY_start && sz_intmemY)
    {
        if ((((unsigned long)p_intmemY_start)+sz_intmemY) > (U8500_TCM_BASE_ADDRESS + U8500_TCM_SIZE))
        {
            OstTraceInt0(TRACE_ERROR, "AFM_MPC: init_intmem: invalid YMEM settings");
            return MEM_INVALID_ADDRESS;
        }
        else
        {
            Init_Y(p_intmemY_start, sz_intmemY);
        }
    }

    return MEM_SUCCESS;
}

/*
 * Set available external memories area for malloc use :
 * @param P_extmem24_start :the start address (MMDSP memory addressing) of available 24 bits external memory
 * @param sz_extmem24 : the size (in MMDSP MAU) of the available 24 bits external memory
 * @param p_extmem16_start : the start address (MMDSP memory addressing) of available 16 bits external memory
 * @param sz_extmem16 : the size (in MMDSP MAU) of the available 16 bits external memory
 */
int init_extmem(void * p_extmem24_start, size_t sz_extmem24, void * p_extmem16_start, size_t sz_extmem16){

    if (init_done == 0) {
        init_alloc();
        init_done = 1;
    }

    // if the provided addresses are non-null
    // check that they are consistant
    // then init heap
    if (p_extmem24_start && sz_extmem24)
    {
        if (((unsigned long)p_extmem24_start < (unsigned long)U8500_DDR_24_BASE_ADDRESS) || 
            ((((unsigned long)p_extmem24_start)+sz_extmem24) > (U8500_DDR_24_BASE_ADDRESS + U8500_DDR_24_SIZE)))
        {
            OstTraceInt0(TRACE_ERROR, "AFM_MPC: init_extmem: invalid EXTMEM24 settings");
            return MEM_INVALID_ADDRESS;
        }
        else
        {
            Init_ext_mem_24(&heap_Ext24_ptr, p_extmem24_start, sz_extmem24, EXT_MEM24, CUSTOM_CHECKSUM);
        }
    }

    if (p_extmem16_start && sz_extmem16)
    {
        if (((unsigned long)p_extmem16_start < (unsigned long)U8500_DDR_16_BASE_ADDRESS) || 
            ((((unsigned long)p_extmem16_start)+sz_extmem16) > (U8500_DDR_16_BASE_ADDRESS + U8500_DDR_16_SIZE)))
        {
            OstTraceInt0(TRACE_ERROR, "AFM_MPC: init_extmem: invalid EXTMEM16 settings");
            return MEM_INVALID_ADDRESS;
        }
        else
        {
            Init_ext_mem_16(&heap_Ext16_ptr, p_extmem16_start, sz_extmem16, EXT_MEM16, CUSTOM_CHECKSUM);
        }
    }

    return MEM_SUCCESS;
}

/*
 * Set available embeded SDRAM area for malloc use :
 * @param p_esram24_start : the start address (MMDSP memory addressing) of available 24 bits esram
 * @param sz_esram24 : the size (in MMDSP MAU) of the available 24 bits esram
 * @param p_esram16_start : the start address (MMDSP memory addressing) of available 16 bits esram
 * @param sz_esram16 : the size (in MMDSP MAU) of the available 16 bits esram
 */
int init_esram(void * p_esram24_start, size_t sz_esram24, void * p_esram16_start, size_t sz_esram16){

    if (init_done == 0) {
        init_alloc();
        init_done = 1;
    }

    // if the provided addresses are non-null
    // check that they are consistant
    // then init heap
    if (p_esram24_start && sz_esram24)
    {
        if (((unsigned long)p_esram24_start < (unsigned long)U8500_ESRAM_24_BASE_ADDRESS) || ((((unsigned long)p_esram24_start)+sz_esram24) > (U8500_ESRAM_24_BASE_ADDRESS + U8500_ESRAM_24_SIZE)))
        {
            OstTraceInt0(TRACE_ERROR, "AFM_MPC: init_esram: invalid ESRAM24 settings");
            return MEM_INVALID_ADDRESS;
        }
        else
        {
            Init_ext_mem_24(&heap_Esram24_ptr, p_esram24_start, sz_esram24, ESRAM_24, CUSTOM_CHECKSUM);
        }
    }

    if (p_esram16_start && sz_esram16)
    {
        if (((unsigned long)p_esram16_start < (unsigned long)U8500_ESRAM_16_BASE_ADDRESS) || ((((unsigned long)p_esram16_start)+sz_esram16) > (U8500_ESRAM_16_BASE_ADDRESS + U8500_ESRAM_16_SIZE)))
        {
            OstTraceInt0(TRACE_ERROR, "AFM_MPC: init_esram: invalid ESRAM16 settings");
            return MEM_INVALID_ADDRESS;
        }
        else
        {
            Init_ext_mem_16(&heap_Esram16_ptr, p_esram16_start, sz_esram16, ESRAM_16, CUSTOM_CHECKSUM);
        }
    }

    return MEM_SUCCESS;
}


// Methods provided to the user
void METH(release_heaps)() {
  // set init_done so that the next
  // call to any init will force the init,
  // discarding all previous inits
  init_done = 0;
}

void METH(minit_intmem)(void * p_intmemX_start, t_uint24 sz_intmemX, void * p_intmemY_start, t_uint24 sz_intmemY) {

    if (init_intmem(p_intmemX_start, sz_intmemX, p_intmemY_start, sz_intmemY) != MEM_SUCCESS) {
        OstTraceInt0(TRACE_ERROR, "AFM_MPC: init_intmem: Error: unable to init the intmem heap");
        NmfPrint0(0, "init_intmem: Error: unable to init the intmem heap\n");
    }
}

void METH(minit_extmem)(void * p_extmem24_start, t_uint24 sz_extmem24, void * p_extmem16_start, t_uint24 sz_extmem16) {

    if (init_extmem(p_extmem24_start, sz_extmem24, p_extmem16_start, sz_extmem16) != MEM_SUCCESS) {
        OstTraceInt0(TRACE_ERROR, "init_extmem: Error: unable to init the extmem heap");
        NmfPrint0(0, "init_extmem: Error: unable to init the extmem heap\n");
    }
}

void METH(minit_esram)(void * p_esram24_start, t_uint24 sz_esram24, void * p_esram16_start, t_uint24 sz_esram16) {

    if (init_esram(p_esram24_start, sz_esram24, p_esram16_start, sz_esram16) != MEM_SUCCESS) {
        OstTraceInt0(TRACE_ERROR, "init_estmem: Error: unable to init the esram heap");
        NmfPrint0(0, "init_esram: Error: unable to init the esram heap\n");
    }
}

void *vmalloc(size_t size, t_ExtendedAllocParams * params)
{
    void * ret;
    TRACE_t * this = (TRACE_t *) params->trace_p;

    M_ENTER_CRITICAL_SECTION;
    ret = Mem_Alloc_Ptr(CUSTOM_CHECKSUM,
                        size,
                        DATA_MEM|MEM_BANK_TO_MEM_BLOCK(params->bank));
    M_EXIT_CRITICAL_SECTION;

    if (ret == NULL)
    {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_MPC: libmalloc: vmalloc not able to allocate %d words in heap type %d", size, params->bank);
#ifndef _SIMU_
        NmfPrint2(0, "AFM_MPC: libmalloc ERROR -> vmalloc not able to allocate %d words in heap type %d\n", size, params->bank);
#endif //#ifndef _SIMU_
        mdump_heap_info(params);
    }

    OstTraceFiltInst3(TRACE_DEBUG, "AFM_MPC: libmalloc: vmalloc %d words in heap type %d ptr=0x%x", size, params->bank, ret);

    return ret;
}

void YMEM *vmalloc_y(size_t size, t_ExtendedAllocParams * params)
{
    void YMEM * ret;
    TRACE_t * this = (TRACE_t *) params->trace_p;

    M_ENTER_CRITICAL_SECTION;
    ret = (void YMEM *)Mem_Alloc_Ptr(CUSTOM_CHECKSUM,
                        size,
                        DATA_MEM|MEM_BANK_TO_MEM_BLOCK(params->bank));
    M_EXIT_CRITICAL_SECTION;

    if (ret == NULL)
    {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_MPC: libmalloc: vmalloc_y not able to allocate %d words in heap type %d", size, params->bank);
#ifndef _SIMU_
        NmfPrint2(0, "AFM_MPC: libmalloc ERROR -> vmalloc not able to allocate %d words in heap type %d\n", size, params->bank);
#endif //#ifndef _SIMU_
        mdump_heap_info(params);
    }

    OstTraceFiltInst3(TRACE_DEBUG, "AFM_MPC: libmalloc: vmalloc_y %d words in heap type %d ptr=0x%x", size, params->bank, ret);

    return ret;
}

// Public.
/*!
  Like C standard free(). This function frees memory pointed by "ptr" in any heap.
*/
int vfree(void *ptr, t_ExtendedAllocParams * params)
{
    int status;
    TRACE_t * this = (TRACE_t *) params->trace_p;

    OstTraceFiltInst2(TRACE_DEBUG, "AFM_MPC: libmalloc: vfree 0x%x ptr in heap type %d", ptr, params->bank);

    M_ENTER_CRITICAL_SECTION;
    status = Mem_Free(ptr);
    M_EXIT_CRITICAL_SECTION;

    if (status == FREE_FAILED)
    {
        mdump_heap_info(params);
    }
 
    return (status);
}

/*!
    This function, called by the startup's initializers(), initializes the memory allocation functionality in the standalone mode i.e. without threads management.
*/
void init_alloc(void)
{
    unsigned int i;

    for (i = 0; i < NB_MEMORY_SPACE; i++) {
        mem_Size_Left[i] = 0;
    }

    heap_ptr = NULL;
#ifdef Y_ALLOC
    last_Y_ptr = NULL;
#endif
#ifdef EXTMEM_ALLOC
    heap_Ext24_ptr = NULL;
    heap_Ext16_ptr = NULL;
#endif //EXTMEM_ALLOC
#ifdef ESRAM_ALLOC
    heap_Esram24_ptr = NULL;
    heap_Esram16_ptr = NULL;
#endif //ESRAM_ALLOC
}

// End of file
