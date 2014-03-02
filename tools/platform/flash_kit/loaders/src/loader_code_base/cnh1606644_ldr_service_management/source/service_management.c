/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file  service_management.c
 * @brief Function definitions for service management.
 *
 * @addtogroup ldr_service_management
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "t_service_management.h"
#include "r_service_management.h"
#include "r_stack_utils.h"
#include "r_bitset.h"
#include "r_debug.h"
#include "r_debug_macro.h"

#include "block_device_management.h"
#include "boot_area_management.h"
#include "flash_manager.h"
#include "cops_data_manager.h"
#include "global_data_manager.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/** Maximum size for LIFO. */
#define MAX_SIZE_OF_LIFO_FOR_SERVICES 100

static Stack_t OpenedLifo;
static Stack_t ClosedLifo;

#ifdef CFG_ENABLE_PRINT_SERVER
/*
 * Automatically generated code?
 * This mapping is just for debugging.
 */
static char *ServicesNames[] = {
    "COPS_SERVICE",
    "GD_SERVICE",
    "BDM_SERVICE",
    "BAM_SERVICE",
    "FPD_SERVICE"
};
#endif


/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/
static ErrorCode_e GetDependencies(BitSet32_t *Dependencies_p, Service_t Service);
static ErrorCode_e GetIncompatible(BitSet32_t *Incompatibles_p, Service_t Service);
static void Visit(Service_t *Service, Stack_t *Stack_p);
static ErrorCode_e GetDependent(BitSet32_t *Dependent_p, Service_t Service);
static void StackInitialize(void);

/*
  {SERVICE_NAME, Incompatibles, Dependencies, Dependent, Started (FALSE), ReferenceCount (0), Start function, Stop function}
 */
static ServiceInfo_t ServicesInfo[] = {
    { COPS_SERVICE, 0, (1 << GD_SERVICE),                                 0, FALSE, 0,  Do_COPS_Start,  Do_COPS_Stop},
    { GD_SERVICE,  0, (1 << FPD_SERVICE),                                 0, FALSE, 0,  Do_GD_Start,   Do_GD_Stop },
    { BDM_SERVICE, 0, (1 << FPD_SERVICE),                                 0, FALSE, 0,  Do_BDM_Start,  Do_BDM_Stop},
    { BAM_SERVICE, 0, (1 << FPD_SERVICE),                                 0, FALSE, 0,  Do_BAM_Start,  Do_BAM_Stop},
    { FPD_SERVICE, 0,                0, (1 << BDM_SERVICE | 1 << BAM_SERVICE), FALSE, 0,  Do_FPD_Start,  Do_FPD_Stop}
};

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/**
 * @brief Initializes services.
 *
 * Initializes all services, function pointers and configuration settings
 *
 * @remark Must be called before using any service
 */
void Do_Init_Services(void)
{
    StackInitialize();
    Do_COPS_Init();
    Do_FPD_Init();
    Do_BDM_Init();
    Do_BAM_Init();
    Do_GD_Init();
}

/**
 * @brief Registers usage of specified service.
 *
 * Registers that somebody is using specified service. If servise is not started,
 * it tries to start it.
 *
 * @param[in]  Service   Service that should be registered.
 *
 * @retval E_SERVICE_NOT_SUPPORTED if service is unknown.
 * @retval E_INCONSISTENCY_IN_SERVICES if some unexpected behavior is detected.
 * @retval E_SUCCESS if service was successfuly registered.
 *
 * @remark This function should be called only from Service management module.
 */
ErrorCode_e Do_Register_Service(Service_t Service)
{
    ErrorCode_e ReturnValue = E_SERVICE_NOT_SUPPORTED;
    BitSet32_t Dependencies = Do_BitSet32_EmptySet();
    int i = 0;
    int j = 0;

    VERIFY(Service < NUMBER_OF_SERVICES, E_SERVICE_NOT_SUPPORTED);

    ServicesInfo[Service].ReferenceCount++;

    if (1 == ServicesInfo[Service].ReferenceCount) {
        BitSet32_t Incompatibles = Do_BitSet32_EmptySet();

        //Stop Incompatible services
        ReturnValue = GetIncompatible(&Incompatibles, Service);

        if (E_SUCCESS != ReturnValue) {
            A_(printf("service_management.c(%d): Failed to get incompatible services!\n", __LINE__);)
            ServicesInfo[Service].ReferenceCount--;
            return ReturnValue;
        }

        for (i = Do_BitSet32_GetNext(Incompatibles, -1); i != -1; i = Do_BitSet32_GetNext(Incompatibles, i)) {
            ReturnValue = Do_Stop_Service((Service_t)i);

            if (E_SUCCESS != ReturnValue) {
                B_(printf("service_management.c(%d): Incompatible service %s can't be stopped!\n", __LINE__, ServicesNames[i]);)
                ServicesInfo[Service].ReferenceCount--;
                return ReturnValue;
            }
        }

        ReturnValue = GetDependencies(&Dependencies, Service);

        if (E_SUCCESS != ReturnValue) {
            A_(printf("service_management.c(%d): Failed to get dependenceies!\n", __LINE__);)
            ServicesInfo[Service].ReferenceCount--;
            return ReturnValue;
        }

        for (i = Do_BitSet32_GetPrevious(Dependencies, BITCOUNT); i != -1; i = Do_BitSet32_GetPrevious(Dependencies, i)) {
            if (!ServicesInfo[i].Started) {
                ReturnValue = ServicesInfo[i].Service_Start_Function();

                if (E_SUCCESS != ReturnValue) {
                    /* If the service fails to start, all previos RefCounters should be decremented. NOT TESTED! */
                    B_(printf("service_management.c(%d): Dependent service %s can't be started!\n", __LINE__, ServicesNames[i]);)

                    for (j = Do_BitSet32_GetNext(Dependencies, i); j != -1; j = Do_BitSet32_GetNext(Dependencies, j)) {
                        ServicesInfo[j].ReferenceCount--;
                        A_(printf("service_management.c(%d): Service %s RefCount = %d!\n", __LINE__, ServicesNames[j], ServicesInfo[j].ReferenceCount);)
                    }

                    A_(printf("service_management.c(%d): Failed to start service %s!\n", __LINE__, ServicesNames[Service]);)
                    ServicesInfo[Service].ReferenceCount--;
                    return ReturnValue;
                } else {
                    ServicesInfo[i].Started = TRUE;
                    ServicesInfo[i].ReferenceCount++;
                    C_(printf("service_management.c(%d): Dependent service %s started!\n", __LINE__, ServicesNames[i]);)
                }
            } else {
                ServicesInfo[i].ReferenceCount++;
            }
        }

        if (!IsStarted(Service)) {
            //Start the service
            ReturnValue = ServicesInfo[Service].Service_Start_Function();

            if (E_SUCCESS != ReturnValue) {
                (void)Do_UnRegister_Service(Service);
                A_(printf("service_management.c(%d): Failed to start service %s!\n", __LINE__, ServicesNames[Service]);)
            } else {
                ServicesInfo[Service].Started = TRUE;
                C_(printf("service_management.c(%d): Service %s started!\n", __LINE__, ServicesNames[Service]);)
            }
        }

        return ReturnValue;
    }

    VERIFY(IsStarted(Service), E_INCONSISTENCY_IN_SERVICES);

    ReturnValue = GetDependencies(&Dependencies, Service);

    if (E_SUCCESS != ReturnValue) {
        A_(printf("service_management.c(%d): Failed to get dependenceies!\n", __LINE__);)
        ServicesInfo[Service].ReferenceCount--;
        return ReturnValue;
    }

    for (i = Do_BitSet32_GetPrevious(Dependencies, BITCOUNT); i != -1; i = Do_BitSet32_GetPrevious(Dependencies, i)) {
        ServicesInfo[i].ReferenceCount++;
    }

ErrorExit:
    return ReturnValue;
}

/**
 * @brief Unregisters usage of specified service.
 *
 * Marks that some user is no longer using specified service.
 *
 * @param[in]  Service   Service that should be unregistered.
 *
 * @retval E_INCONSISTENCY_IN_SERVICES if some unexpected behavior is detected.
 * @retval E_SUCCESS if service was successfuly unregistered.
 *
 * @remark This function should be called only from Service management module.
 */
ErrorCode_e Do_UnRegister_Service(Service_t Service)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    BitSet32_t Dependencies = Do_BitSet32_EmptySet();
    int i;

    VERIFY(0 != ServicesInfo[Service].ReferenceCount, E_INCONSISTENCY_IN_SERVICES);

    if (ServicesInfo[Service].ReferenceCount > 0) {
        ServicesInfo[Service].ReferenceCount--;
    }

    ReturnValue = GetDependencies(&Dependencies, Service);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    for (i = Do_BitSet32_GetNext(Dependencies, -1); i != -1; i = Do_BitSet32_GetNext(Dependencies, i)) {
        VERIFY(0 != ServicesInfo[i].ReferenceCount, E_INCONSISTENCY_IN_SERVICES);

        if (ServicesInfo[i].ReferenceCount > 0) {
            ServicesInfo[i].ReferenceCount--;
        }
    }

    C_(printf("service_management.c(%d): Unregister service %s (new RefCount = %d)!\n", __LINE__, ServicesNames[Service], ServicesInfo[Service].ReferenceCount);)

ErrorExit:
    return ReturnValue;
}

/**
 * @brief Stops specified service.
 *
 * If nobody is using specified service, it will try to stop it.
 *
 * @param[in]  Service   Service that should be stopped.
 *
 * @retval E_SERVICE_NOT_SUPPORTED if service is unknown.
 * @retval E_SERVICE_IN_USE if service is in use.
 * @retval E_SUCCESS if service was successfuly stopped.
 *
 * @remark Call this function with NUMBER_OF_SERVICES as input parameter to
 * shutdown all supported services.
 */
ErrorCode_e Do_Stop_Service(Service_t Service)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    int RefCounter = 0;
    int i = 0;
    BitSet32_t Dependent = Do_BitSet32_EmptySet();

    VERIFY(Service <= NUMBER_OF_SERVICES, E_SERVICE_NOT_SUPPORTED);

    if (Service == NUMBER_OF_SERVICES) { //Shut down all services
        for (i = 0; i < NUMBER_OF_SERVICES; i++) {
            RefCounter = GetReferenceCount((Service_t)i);

            //      VERIFY(!(0 < RefCounter), E_SERVICE_IN_USE);
            if (RefCounter > 0) {
                A_(printf("service_management.c(%d): %s service is in use! (RefCount = %d)\n", __LINE__, ServicesNames[i], ServicesInfo[i].ReferenceCount);)
            }

            if (IsStarted((Service_t)i)) {
                //Stop the service
                ReturnValue = ServicesInfo[i].Service_Stop_Function();
                VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

                ServicesInfo[i].Started = FALSE;
                ServicesInfo[i].ReferenceCount = 0;
            }
        }

        goto ErrorExit;
    }

    if (!IsStarted(Service)) {
        return E_SUCCESS;
    }

    ReturnValue = GetDependent(&Dependent, Service);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    for (i = Do_BitSet32_GetNext(Dependent, -1); i != -1; i = Do_BitSet32_GetNext(Dependent, i)) {
        RefCounter = GetReferenceCount((Service_t)i);
        VERIFY(!(0 < RefCounter), E_SERVICE_IN_USE);

        if (IsStarted((Service_t)i)) {
            //Stop the service
            ReturnValue = ServicesInfo[i].Service_Stop_Function();
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            ServicesInfo[i].Started = FALSE;
            ServicesInfo[i].ReferenceCount = 0;
        }
    }

    RefCounter = GetReferenceCount(Service);
    //  VERIFY((0 == RefCounter), E_SERVICE_IN_USE); // xvskoka commented only for test

    //Stop the service
    ReturnValue = ServicesInfo[Service].Service_Stop_Function();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ServicesInfo[Service].Started = FALSE;
    ServicesInfo[Service].ReferenceCount = 0;
    C_(printf("service_management.c(%d): %s service count %d!\n", __LINE__, ServicesNames[Service], ServicesInfo[Service].ReferenceCount);)

ErrorExit:
    return ReturnValue;
}

/**
 * @brief Checks if the service is started.
 *
 * @param[in]  Service   Service that should be checked.
 *
 * @retval FALSE if service is not started.
 * @retval TRUE if service is started.
 */
boolean IsStarted(Service_t Service)
{
    if (Service < NUMBER_OF_SERVICES) {
        return ServicesInfo[Service].Started;
    }

    return FALSE;
}

/**
 * @brief Returns current number of service users.
 *
 * @param[in]  Service   Service that should be checked.
 *
 * @return Number of users.
 */
int GetReferenceCount(Service_t Service)
{
    if (Service < NUMBER_OF_SERVICES) {
        return ServicesInfo[Service].ReferenceCount;
    }

    return FALSE;
}

/*
 * I used Depth first search in this function. I have tested the function and it
 * works fine.
 */
static ErrorCode_e GetDependencies(BitSet32_t *Dependencies_p, Service_t Service)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    VERIFY(NULL != Dependencies_p, E_INVALID_INPUT_PARAMETERS);

    ReturnValue = Do_Stack_Enqueue(&OpenedLifo, (uint32)Service);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    while (!Do_Stack_IsEmpty(&OpenedLifo)) {
        Service_t ServiceNode;

        ServiceNode = (Service_t)Do_Stack_Dequeue(&OpenedLifo);

        VERIFY(!Do_Stack_Contains(&ClosedLifo, (uint32)ServiceNode), E_CYCLIC_GRAPH);

        ReturnValue = Do_Stack_Enqueue(&ClosedLifo, (uint32)ServiceNode);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        // Check if this is a terminal node
        if (ServicesInfo[ServiceNode].Dependencies == Do_BitSet32_EmptySet()) {
            Service_t VisitedService;
            Service_t UnvisitedService;

            //Do_Stack_Print(&ClosedLifo);
            VisitedService = (Service_t)Do_Stack_Dequeue(&ClosedLifo);

            if (!Do_BitSet32_Insert(Dependencies_p, VisitedService)) {
                B_(printf("service_management.c(%d): Do_BitSet32_Insert %s FAILED!\n", __LINE__, ServicesNames[VisitedService]);)
            }

            if (Do_Stack_IsEmpty(&OpenedLifo)) {
                while (!Do_Stack_IsEmpty(&ClosedLifo)) {
                    VisitedService = (Service_t)Do_Stack_Dequeue(&ClosedLifo);
                    VERIFY(Do_BitSet32_Insert(Dependencies_p, VisitedService), E_INCONSISTENCY_IN_SERVICES);
                }

                VERIFY(Do_BitSet32_Remove(Dependencies_p, Service), E_INCONSISTENCY_IN_SERVICES);
                ReturnValue = E_SUCCESS;
                goto ErrorExit;
            }

            UnvisitedService = (Service_t)Do_Stack_Dequeue(&OpenedLifo);
            ReturnValue = Do_Stack_Enqueue(&OpenedLifo, (uint32)UnvisitedService);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            while (!Do_BitSet32_Contains(ServicesInfo[VisitedService].Dependencies, UnvisitedService)) {
                VisitedService = (Service_t)Do_Stack_Dequeue(&ClosedLifo);
                VERIFY(Do_BitSet32_Insert(Dependencies_p, VisitedService), E_INCONSISTENCY_IN_SERVICES);
            }
        } else {
            Visit(&ServiceNode, &OpenedLifo);
        }
    }

ErrorExit:
    memset(OpenedLifo.Buffer_p, 0, MAX_SIZE_OF_LIFO_FOR_SERVICES * sizeof(uint32 *) + 1);
    memset(ClosedLifo.Buffer_p, 0, MAX_SIZE_OF_LIFO_FOR_SERVICES * sizeof(uint32 *) + 1);

    return ReturnValue;
}

static ErrorCode_e GetIncompatible(BitSet32_t *Incompatibles_p, Service_t Service)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    int i = 0;
    BitSet32_t Dependencies = Do_BitSet32_EmptySet();

    VERIFY(NULL != Incompatibles_p, E_INVALID_INPUT_PARAMETERS);

    *Incompatibles_p = ServicesInfo[Service].Incompatibles;

    ReturnValue = GetDependencies(&Dependencies, Service);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    for (i = Do_BitSet32_GetNext(Dependencies, -1); i != -1; i = Do_BitSet32_GetNext(Dependencies, i)) {
        *Incompatibles_p = Do_BitSet32_Union(*Incompatibles_p, ServicesInfo[i].Incompatibles);
    }

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e GetDependent(BitSet32_t *Dependent_p, Service_t Service)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    Service_t i;

    VERIFY(NULL != Dependent_p, E_INVALID_INPUT_PARAMETERS);

    *Dependent_p = Do_BitSet32_EmptySet();

    for (i = (Service_t)0; i < NUMBER_OF_SERVICES; i++) {
        BitSet32_t Dependencies = Do_BitSet32_EmptySet();
        ReturnValue = GetDependencies(&Dependencies, i);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        if (Do_BitSet32_Contains(Dependencies, Service)) {
            VERIFY(Do_BitSet32_Insert(Dependent_p, i), E_INCONSISTENCY_IN_SERVICES);
        }
    }

ErrorExit:
    return ReturnValue;
}

static void Visit(Service_t *Service_p, Stack_t *Stack_p)
{
    BitSet32_t Dependencies;
    int i = 0;;
    Dependencies = ServicesInfo[*Service_p].Dependencies;

    for (i = Do_BitSet32_GetNext(Dependencies, -1); i != -1; i = Do_BitSet32_GetNext(Dependencies, i)) {
        (void)Do_Stack_Enqueue(Stack_p, (uint32)i);
    }
}

static void StackInitialize(void)
{
    Do_Stack_Initialize(&OpenedLifo, MAX_SIZE_OF_LIFO_FOR_SERVICES);
    Do_Stack_Initialize(&ClosedLifo, MAX_SIZE_OF_LIFO_FOR_SERVICES);
}

/** @} */
