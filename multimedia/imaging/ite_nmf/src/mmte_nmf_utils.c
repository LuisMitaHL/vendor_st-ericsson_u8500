/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     mmte_nmf_utils.c
* \brief    MMTE NMF utilities 
* \author   ST-Ericsson
*/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "mmte_nmf.h"
#include "ite_main.h"

/*
#if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN) || defined(WORKSTATION))
#include <cm/inc/cm.h>
#include "ilos/api/ilos_api.h"
#include "nmf/configuration/api/init_cm.h"
/-----------------------------------------
/ Global and External Variables needed by think to work ...
/----------------------------------------
typedef struct //XXX FIXME TODO this declaration must be removed in next NMF delivery > 1.3.2
{
char    *c_filename;    // name of the file
char    *u32_fileStart; // address in memory of the beginning of the file
char    *u32_fileEnd;   // address of the end of the file
char    *u32_fileSize;  // size in byte of the file (declared as char* in order to avoid warning)
}t_efs_entry;

int GFS_nb_entries = 0;
t_efs_entry GFS_filesystem[] = {NULL,NULL,NULL,NULL};
#endif	// #if !( defined(__ARM_LINUX) || defined(__ARM_SYMBIAN)) 
*/

#ifdef __ARM_SYMBIAN
#define puts(x)             \
        {                   \
        CLI_disp_msg(x);    \
        CLI_disp_msg("\n"); \
        }
#endif

// for memory mle 
extern t_cm_domain_id domainId;

typedef struct {
    t_cm_allocator_status   mpc_tcm16;
    t_cm_allocator_status   mpc_tcm24;
    t_cm_allocator_status   mpc_esram16;
    t_cm_allocator_status   mpc_esram24;
    t_cm_allocator_status   mpc_sdram16;
    t_cm_allocator_status   mpc_sdram24;   
} t_memory_status_core;

static struct {
    t_cm_allocator_status   allocator;
    t_memory_status_core    sva;
    t_memory_status_core    sia;
} memory_status;

static void nmf_memory_status_core_init(t_nmf_core_id core_id, t_memory_status_core* status) {
    t_cm_error error = CM_OK;
    UNUSED(core_id);

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_TCM16, &status->mpc_tcm16);
    assert(error == CM_OK);

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_TCM24, &status->mpc_tcm24);
    assert(error == CM_OK);

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_ESRAM16, &status->mpc_esram16);
    assert(error == CM_OK);

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_ESRAM24, &status->mpc_esram24);
    assert(error == CM_OK);

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_SDRAM16, &status->mpc_sdram16);
    assert(error == CM_OK);

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_SDRAM24, &status->mpc_sdram24);
    assert(error == CM_OK);

    UNUSED(error);
}

void nmf_memory_status_init(void) {

    t_cm_error error;

    error = CM_SetMode(CM_CMD_DBG_MODE, 0);
    assert(error == CM_OK);

    nmf_memory_status_core_init(SVA_CORE_ID, &memory_status.sva);

    nmf_memory_status_core_init(SIA_CORE_ID, &memory_status.sia);

    UNUSED(error);
}


static t_bool nmf_memory_status_compare(t_cm_allocator_status* current_status, t_cm_allocator_status* init_status) {
    return  current_status->domain.accumulate_free_memory == init_status->domain.accumulate_free_memory &&
            current_status->domain.accumulate_used_memory == init_status->domain.accumulate_used_memory ;
}

static t_bool nmf_memory_status_core_check(t_nmf_core_id core_id, t_memory_status_core* status) {
    t_cm_error error = CM_OK;
    t_cm_allocator_status alloc_status;
    UNUSED(core_id);

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_TCM16, &alloc_status);
    assert(error == CM_OK);
    if (!nmf_memory_status_compare(&alloc_status, &status->mpc_tcm16))
        return FALSE;

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_TCM24, &alloc_status);
    assert(error == CM_OK);
    if (!nmf_memory_status_compare(&alloc_status, &status->mpc_tcm24))
        return FALSE;

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_ESRAM16, &alloc_status);
    assert(error == CM_OK);
    if (!nmf_memory_status_compare(&alloc_status, &status->mpc_esram16))
        return FALSE;

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_ESRAM24, &alloc_status);
    assert(error == CM_OK);
    if (!nmf_memory_status_compare(&alloc_status, &status->mpc_esram24))
        return FALSE;
    
    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_SDRAM16, &alloc_status);
    assert(error == CM_OK);
    if (!nmf_memory_status_compare(&alloc_status, &status->mpc_sdram16))
        return FALSE;

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_SDRAM24, &alloc_status);
    assert(error == CM_OK);
    if (!nmf_memory_status_compare(&alloc_status, &status->mpc_sdram24))
        return FALSE;

    UNUSED(error);
    return TRUE;
}

t_bool nmf_memory_status_check(void) {
    t_cm_error error = CM_OK;
    
    error = CM_SetMode(CM_CMD_DBG_MODE, 0);
    assert(error == CM_OK);

    if (!nmf_memory_status_core_check(SVA_CORE_ID, &memory_status.sva))
        return FALSE;
    
    if (!nmf_memory_status_core_check(SIA_CORE_ID, &memory_status.sia))
        return FALSE;

    UNUSED(error);
    return TRUE;
}


static void nmf_memory_status_format(t_cm_allocator_status* status) {
    CLI_disp_msg("  Used blocks           = %u\n", status->global.used_block_number);
    CLI_disp_msg("  Free blocks           = %u\n",  status->global.free_block_number);
    CLI_disp_msg("  Maximum free size     = %lu\n", status->global.maximum_free_size);
    CLI_disp_msg("  Minimum free size     = %lu\n", status->global.minimum_free_size);
    CLI_disp_msg("  Cumulated free memory = %lu\n", status->domain.accumulate_free_memory);
    CLI_disp_msg("  Cumulated used memory = %lu\n", status->domain.accumulate_used_memory);
    puts("");
}

static const char* nmf_get_core_name(t_nmf_core_id core_id) {
    switch (core_id) {
        case ARM_CORE_ID: return "ARM";
        case SVA_CORE_ID: return "SVA";
        case SIA_CORE_ID: return "SIA";
        default: return "(unknown core)";
    }
}

static void nmf_memory_status_core_print(t_nmf_core_id core_id, t_memory_status_core* status) {
    t_cm_error error = CM_OK;
    t_cm_allocator_status alloc_status;
    t_uint16 tcm16_used;
    t_uint16 tcm24_used;
    t_uint16 esram16_used;
    t_uint16 esram24_used;
    t_uint16 sdram16_used;
    t_uint16 sdram24_used;

    CLI_disp_msg("\n==== %s ============\n", nmf_get_core_name(core_id));

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_TCM16, &alloc_status);
    assert(error == CM_OK);
    puts("TCM16 status:");
    nmf_memory_status_format(&alloc_status);
    tcm16_used = alloc_status.domain.accumulate_used_memory - status->mpc_tcm16.domain.accumulate_used_memory;

    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_TCM24, &alloc_status);
    assert(error == CM_OK);
    puts("TCM24 status:");
    nmf_memory_status_format(&alloc_status);
    tcm24_used = alloc_status.domain.accumulate_used_memory - status->mpc_tcm24.domain.accumulate_used_memory;
    
    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_ESRAM16, &alloc_status);
    assert(error == CM_OK);
    puts("ESRAM16 status:");
    nmf_memory_status_format(&alloc_status);
    esram16_used = alloc_status.domain.accumulate_used_memory - status->mpc_esram16.domain.accumulate_used_memory;
    
    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_ESRAM24, &alloc_status);
    assert(error == CM_OK);
    puts("ESRAM24 status:");
    nmf_memory_status_format(&alloc_status);
    esram24_used = alloc_status.domain.accumulate_used_memory - status->mpc_esram24.domain.accumulate_used_memory;
    
    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_SDRAM16, &alloc_status);
    assert(error == CM_OK);
    puts("SDRAM16 status:");
    nmf_memory_status_format(&alloc_status);
    sdram16_used = alloc_status.domain.accumulate_used_memory - status->mpc_sdram16.domain.accumulate_used_memory;
    
    error = CM_GetMpcMemoryStatus(domainId, CM_MM_MPC_SDRAM24, &alloc_status);
    assert(error == CM_OK);
    puts("SDRAM24 status:");
    nmf_memory_status_format(&alloc_status);
    sdram24_used = alloc_status.domain.accumulate_used_memory - status->mpc_sdram24.domain.accumulate_used_memory;
    
    puts("Memory consumption summary:");
    CLI_disp_msg("  Used TCM16   = %lu\n", tcm16_used);
    CLI_disp_msg("  Used TCM24   = %lu\n", tcm24_used);
    CLI_disp_msg("  Used ESRAM16 = %lu\n", esram16_used);
    CLI_disp_msg("  Used ESRAM24 = %lu\n", esram24_used);
    CLI_disp_msg("  Used SDRAM16 = %lu\n", sdram16_used);
    CLI_disp_msg("  Used SDRAM24 = %lu\n", sdram24_used);
    UNUSED(error);
}

void nmf_memory_status_print(void) {
    t_cm_error error = CM_OK;

    error = CM_SetMode(CM_CMD_DBG_MODE, 0);
    assert(error == CM_OK);

    nmf_memory_status_core_print(SVA_CORE_ID, &memory_status.sva);

    nmf_memory_status_core_print(SIA_CORE_ID, &memory_status.sia);
    UNUSED(error);
}

void nmf_network_print(t_bool all_components) {
#if !defined (__ARM_SYMBIAN)
    t_cm_instance_handle component;
    t_cm_error error;
    t_nmf_core_id coreId;
    char templateName[MAX_TEMPLATE_NAME_LENGTH];
    char localName[MAX_COMPONENT_NAME_LENGTH];
    int total_components = 0, user_components = 0;
    t_nmf_ee_priority priority;

    char namePriority[3][120]=
    {
        "PRIORITY_BACKGROUND",
        "PRIORITY_NORMAL",
        "PRIORITY_URGENT"
    };
    
    
    CLI_disp_msg("NMF components:\n");
    error = CM_GetComponentListHeader(&component);
    while (error == CM_OK && component != 0) {
        total_components++;
        error = CM_GetComponentDescription(component, templateName, MAX_TEMPLATE_NAME_LENGTH, &coreId, localName, MAX_COMPONENT_NAME_LENGTH, &priority);
        if (error == CM_OK && (all_components || templateName[0] != '_')) {
            user_components++;
            CLI_disp_msg("  [%s] %s as %s %s\n", nmf_get_core_name(coreId), templateName, localName,namePriority[priority]);
        }

        error = CM_GetComponentListNext(component, &component);
    }
    CLI_disp_msg("> %u user components of %u total\n", user_components, total_components);
#endif
}



EXPORT_C void mmte_nmf_settracelevel(int level){
    CM_SetMode(CM_CMD_TRACE_LEVEL, level);
    CLI_disp_msg("NMF Trace Level set to %d\n", level);
}


