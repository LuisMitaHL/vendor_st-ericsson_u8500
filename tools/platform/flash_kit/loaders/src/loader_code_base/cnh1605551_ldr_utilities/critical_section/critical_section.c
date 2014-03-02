/*******************************************************************************
 * $Copyright ST-Ericsson 2011 $
 ******************************************************************************/

/*
 * @addtogroup critical_section
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "stdlib.h"
#include "t_basicdefinitions.h"
#include "r_critical_section.h"
#include "r_atomic_functions.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#define CS_LOCKED   1
#define CS_UNLOCKED 0
/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/**
 * Perform initialization of Critical Section.
 *
 * Creates a critical section object and returns pointer to it for later usage
 * when it is needed to enter and leave the critical part of the code,
 * and destroy the critical section object when not used anymore.
 *
 * @retval  Pointer to initialized critical section object if initialization was
 *          successful, otherwise NULL.
 */
CriticalSection_t Do_CriticalSection_Create(void)
{
    uint32 *cs = (uint32 *)malloc(sizeof(uint32));

    if (NULL != cs) {
        *cs = CS_UNLOCKED;
    }

    return cs;
}

/**
 * Free resources used for Critical Section object.
 *
 * Free all resources and destroy the critical section object given as parameter
 * to the function.
 *
 * @param [in] CriticalSectionObject  Pointer to pointer at the object to be destroyed.
 *
 * @retval  None.
 */
void Do_CriticalSection_Destroy(CriticalSection_t *CriticalSectionObject)
{
    uint32 **cs = (uint32 **)CriticalSectionObject;

    if (NULL != *cs) {
        free(*cs);
        *cs = NULL;
    }
}

/**
 * Enter to the Critical Section code.
 *
 * Take ownership over critical section object and execute the part of the code
 * covered by the Critical Section object passed as parameter to the function.
 *
 * @param [in] CriticalSectionObject  Pointer to the object used to exclusively
 *                                    lock some critical part of the code.
 *
 * @retval  TRUE if Critical Section code entered successfully, otherwise FALSE.
 */
boolean Do_CriticalSection_Enter(CriticalSection_t CriticalSectionObject)
{
    volatile uint32 *cs = (volatile uint32 *)CriticalSectionObject;

    if (NULL != cs) {
        uint32 cs_status = CS_LOCKED;
        cs_status = Do_Atomic_CompareAndSwap(cs, CS_UNLOCKED, CS_LOCKED);

        if (CS_UNLOCKED == cs_status) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    return TRUE;
}

/**
 * Leave the Critical Section code.
 *
 * Release the Critical Section object when finished execution to allow someone
 * else to execute the code covered by the Critical Section object passed as
 * parameter to the function.
 *
 * @param [in] CriticalSectionObject  Pointer to the object used to exclusively
 *                                    lock some critical part of the code.
 *
 * @retval  None.
 */
void Do_CriticalSection_Leave(CriticalSection_t CriticalSectionObject)
{
    volatile uint32 *cs = (volatile uint32 *)CriticalSectionObject;

    if (NULL != cs) {
        uint32 cs_status = CS_UNLOCKED;
        cs_status = Do_Atomic_CompareAndSwap(cs, CS_LOCKED, CS_UNLOCKED);

        if (CS_LOCKED == cs_status) {
            /* SUCCESS */
        } else {
            /* FAILED */
        }
    }
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

/* @} */
