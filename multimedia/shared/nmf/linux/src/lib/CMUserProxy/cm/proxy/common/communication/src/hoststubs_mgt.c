/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <cm/proxy/common/communication/inc/hoststubs_mgt.h>
#include <cm/proxy/inc/osal.h>
#include <inc/nmf-limits.h>

static int StringCompare(const char* str1, const char* str2, int count)
{
    signed char res = 0;

    while (count)
    {
        if ((res = *str1 - *str2++) != 0 || !*str1++)
            break;
        count--;
    }

    return res;
}

typedef struct _t_nmf_stub_register_link {
    t_nmf_stub_register                 *interfaceArray;
    struct _t_nmf_stub_register_link    *next;
} t_nmf_stub_register_link;

typedef struct _t_nmf_skel_register_link {
    t_nmf_skel_register                 *interfaceArray;
    struct _t_nmf_skel_register_link    *next;
} t_nmf_skel_register_link;

static t_nmf_stub_register_link *pFirstStubRegister = NULL;
static t_nmf_skel_register_link *pFirstSkelRegister = NULL;

PUBLIC EXPORT_SHARED void CM_INTERNAL_RegisterHost2MpcInterfaces(t_nmf_stub_register *pInterfacesArray)
{
    t_nmf_stub_register_link* link = (t_nmf_stub_register_link*)OSAL_Alloc(sizeof(t_nmf_stub_register_link));

    link->interfaceArray = pInterfacesArray;

    OSAL_Lock(cmProxyLock);
    link->next = pFirstStubRegister;
    pFirstStubRegister = link;
    OSAL_Unlock(cmProxyLock);
}

PUBLIC EXPORT_SHARED void CM_INTERNAL_UnregisterHost2MpcInterfaces(t_nmf_stub_register *pInterfacesArray)
{
    t_nmf_stub_register_link* link;

    OSAL_Lock(cmProxyLock);
    if(pFirstStubRegister->interfaceArray == pInterfacesArray)
    {
        link = pFirstStubRegister;
        pFirstStubRegister = pFirstStubRegister->next;
    } else
    {
        t_nmf_stub_register_link* prev = pFirstStubRegister;
        while(prev->next->interfaceArray != pInterfacesArray)
            prev = prev->next;
        link = prev->next;
        prev->next = link->next;
    }
    OSAL_Unlock(cmProxyLock);

    OSAL_Free(link);
}

PUBLIC EXPORT_SHARED void CM_INTERNAL_RegisterMpc2HostInterfaces(t_nmf_skel_register *pInterfacesArray)
{
    t_nmf_skel_register_link* link = (t_nmf_skel_register_link*)OSAL_Alloc(sizeof(t_nmf_skel_register_link));

    link->interfaceArray = pInterfacesArray;

    OSAL_Lock(cmProxyLock);
    link->next = pFirstSkelRegister;
    pFirstSkelRegister = link;
    OSAL_Unlock(cmProxyLock);
}

PUBLIC EXPORT_SHARED void CM_INTERNAL_UnregisterMpc2HostInterfaces(t_nmf_skel_register *pInterfacesArray)
{
    t_nmf_skel_register_link* link;

    OSAL_Lock(cmProxyLock);
    if(pFirstSkelRegister->interfaceArray == pInterfacesArray)
    {
        link = pFirstSkelRegister;
        pFirstSkelRegister = pFirstSkelRegister->next;
    } else
    {
        t_nmf_skel_register_link* prev = pFirstSkelRegister;
        while(prev->next->interfaceArray != pInterfacesArray)
            prev = prev->next;
        link = prev->next;
        prev->next = link->next;
    }
    OSAL_Unlock(cmProxyLock);

    OSAL_Free(link);
}

PUBLIC t_cm_error cm_HSM_getItfStubInitFct(t_nmf_bc_coding_style style, const char *itfType, t_nmf_init_stub_fct *pInitStubFct)
{
    t_nmf_stub_register_link *pTmp;

    OSAL_Lock(cmProxyLock);
    pTmp = pFirstStubRegister;
    while (pTmp != NULL)
    {
        if(pTmp->interfaceArray->codingStyle == style)
        {
            t_uint32 index;
            for (index = 0; index < pTmp->interfaceArray->numElems; index++)
            {
                t_nmf_stub_function *pStubFct = (t_nmf_stub_function *)&pTmp->interfaceArray->pStubFctArray[index];
                if (StringCompare(pStubFct->itfType, itfType, MAX_INTERFACE_TYPE_NAME_LENGTH) == 0)
                {
                    // LOG_INTERNAL(3,"%s initStub at 0x%08x\n", pStubFct->itfName, (t_uint32)pStubFct->initStubFct, 0, 0, 0, 0);
                    *pInitStubFct = pStubFct->initStubFct;
		    OSAL_Unlock(cmProxyLock);
                    return CM_OK;
                }
            }
        }
        pTmp = pTmp->next;
    }
    OSAL_Unlock(cmProxyLock);

    return CM_BINDING_COMPONENT_NOT_FOUND;
}

PUBLIC t_cm_error cm_HSM_getItfSkelInitFct(t_nmf_bc_coding_style style, const char *itfType, t_jump_method **jumpMethods)
{
    t_nmf_skel_register_link *pTmp;

    OSAL_Lock(cmProxyLock);
    pTmp = pFirstSkelRegister;
    while (pTmp != NULL)
    {
        if(pTmp->interfaceArray->codingStyle == style)
        {
            t_uint32 index;
            for (index = 0; index < pTmp->interfaceArray->numElems; index++)
            {
                t_nmf_skel_function *pSkelFct = (t_nmf_skel_function *)&pTmp->interfaceArray->pSkelFctArray[index];
                if (StringCompare(pSkelFct->itfType, itfType, MAX_INTERFACE_TYPE_NAME_LENGTH) == 0)
                {
                    // LOG_INTERNAL(3,"%s initSkel at 0x%08x\n", pSkelFct->itfName, (t_uint32)pSkelFct->initSkelFct, 0, 0, 0 ,0);
                    *jumpMethods = pSkelFct->jumpMethods;
		    OSAL_Unlock(cmProxyLock);
                    return CM_OK;
                }
            }
        }
        pTmp = pTmp->next;
    }
    OSAL_Unlock(cmProxyLock);

    return CM_BINDING_COMPONENT_NOT_FOUND;
}
