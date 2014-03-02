/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @addtogroup u8500_seclib_functions
 *
 * @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c_system.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "t_loader_sec_lib.h"
#include "r_loader_sec_lib.h"
#include "e_loader_sec_lib.h"


/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/*
 * This routine frees the allocated memory region. Loader Security Library
 * external.
 *
 * @param[in] Buf_pp  Pointer to the allocated memory region to free.
 *
 * @return see \c LoaderSecLib_Output_t for detailed description of the error
 *                codes.
 */
LoaderSecLib_Output_t LoaderSecLib_Free(uint8 **const Buf_pp)
{
    B_(printf("LoaderSecLib_Free\n");)

    if (NULL != *Buf_pp) {
        free(*Buf_pp);
        *Buf_pp = NULL;
        return LOADER_SEC_LIB_SUCCESS;
    }

    //A_(printf("lst8500_seclib_stubs.c (%d): ** ERR: Failed to free memory! **\n",__LINE__);)

    /* TODO: Temporary fix */
    return LOADER_SEC_LIB_SUCCESS; //LOADER_SEC_LIB_FAILURE;
}

/*
 * This routine allocates a memory region. Loader Security Library external.
 *
 * @param[in] BufLength   The length of the buffer to allocate.
 *
 * @return the allocated buffer, the function will return NULL if the allocation
 *                               failed.
 */
void *LoaderSecLib_Allocate(const uint32 BufLength)
{
    B_(printf("LoaderSecLib_Allocate\n");)
    return (void *)malloc(BufLength);
}
/** @} */
