/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*
 *
 */
#include <cm/proxy/api/cm_proxy.h>
#include <cm/os/api/cm_os.h>
#include <cm/proxy/common/repository/inc/repository.h>
#include <cm/proxy/common/component/inc/component_info.h>
#include <inc/nmf-limits.h>
#include <cm/proxy/inc/osal.h>

PUBLIC EXPORT_SHARED t_cm_error CM_PushComponent(
        const char* templateName) {
    t_nmf_osal_fs_file_handle file;
    void *data;
    t_cm_size size;
    t_cm_error error;

    if((error = cm_openComponent(templateName, &file, &data, &size, 0)) != CM_OK)
        return error;

    error = CM_OS_PushComponent(templateName, data, size);

    cm_closeComponent(file);

    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_ReleaseComponent(
        const char* templateName)
{
    if (templateName == NULL)
        return CM_INVALID_PARAMETER;

    return CM_OS_ReleaseComponent(templateName);
}

PUBLIC EXPORT_SHARED t_cm_error CM_InstantiateComponent(const char* templateName, t_cm_domain_id domainId, t_nmf_ee_priority priority, const char* localName, t_cm_instance_handle *component)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    unsigned diff;
    func[0].name = __func__;
#endif

    /*
     * Sanity check
     */
    if ((templateName == NULL) || (component == NULL))
	    return CM_INVALID_PARAMETER;

    /* For SINGLETON Components, we need to push the component only once, at the first instance.
       To avoid several useless 'push' at each instantiate of the same singleton, we first try
       an intantiation and then push the component only in case of CM_COMPONENT_NOT_FOUND error
     */
    error = CM_OS_InstantiateComponent(templateName, domainId, priority, localName, 0x0, 0, component);
    if (error == CM_COMPONENT_NOT_FOUND)
    {
        // Component not preloaded, retry with file data
        t_nmf_osal_fs_file_handle file;
        void *data;
        t_cm_size size;

        if((error = cm_openComponent(templateName, &file, &data, &size, 0)) != CM_OK)
            return error;

        error = CM_OS_InstantiateComponent(templateName, domainId, priority, localName, (const char*)data, size, component);

        cm_closeComponent(file);
    }

#ifdef CM_API_TIMING
    diff = (unsigned)(OSAL_GetTime() - start);
    OSAL_Log("CM_InstantiateComponent(%s) takes %u us\n",
	     (void *)templateName, (void *)diff, 0);
    func[0].time += diff;
    func[0].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_StartComponent(t_cm_instance_handle component)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[1].name = __func__;
#endif
    // Sanity check
    if (component == 0)
	    return CM_INVALID_PARAMETER;

    error = CM_OS_StartComponent(component);

#ifdef CM_API_TIMING
    func[1].time += OSAL_GetTime() - start;
    func[1].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_StopComponent(t_cm_instance_handle component)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[2].name = __func__;
#endif
    // Sanity check
    if (component == 0)
	    return CM_INVALID_PARAMETER;

    error = CM_OS_StopComponent(component);

#ifdef CM_API_TIMING
    func[2].time += OSAL_GetTime() - start;
    func[2].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_DestroyComponent(t_cm_instance_handle component)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[3].name = __func__;
#endif

    // Sanity check
    if (component == 0)
	    return CM_INVALID_PARAMETER;

    error = CM_OS_DestroyComponent(component);

#ifdef CM_API_TIMING
    func[3].time += OSAL_GetTime() - start;
    func[3].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_BindComponent(const t_cm_instance_handle client, const char* requiredItfClientName, const t_cm_instance_handle server, const char* providedItfServerName)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[4].name = __func__;
#endif

    // Sanity check
    if ((client == 0) || (requiredItfClientName == NULL)
            || (server == 0) || (providedItfServerName == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_BindComponent(client, requiredItfClientName, server, providedItfServerName, FALSE, 0, 0);

#ifdef CM_API_TIMING
    func[4].time += OSAL_GetTime() - start;
    func[4].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_BindComponentTrace(const t_cm_instance_handle client, const char* requiredItfClientName, const t_cm_instance_handle server, const char* providedItfServerName)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[5].name = __func__;
#endif

    // Sanity check
    if ((client == 0) || (requiredItfClientName == NULL)
            || (server == 0) || (providedItfServerName == NULL))
        return CM_INVALID_PARAMETER;

    {
        char components[1][MAX_INTERFACE_TYPE_NAME_LENGTH];
        t_nmf_osal_fs_file_handle files[1];
        void *datas[1];
        t_cm_size sizes[1];

        if((error = CM_OS_GetRequiredComponentFiles(BIND_TRACE, client, requiredItfClientName, server, providedItfServerName,
                components, 1, NULL, NULL)) != CM_OK)
        {
            return error;
        }

        if((error = cm_openComponents(1, components, files, datas, sizes, 0)) != CM_OK)
        {
            return CM_BINDING_COMPONENT_NOT_FOUND;
        }

        error = CM_OS_BindComponent(client, requiredItfClientName, server, providedItfServerName, TRUE, datas[0], sizes[0]);

        cm_closeComponents(1, files);
    }

#ifdef CM_API_TIMING
    func[5].time += OSAL_GetTime() - start;
    func[5].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_UnbindComponent(const t_cm_instance_handle client, const char* requiredItfClientName)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[6].name = __func__;
#endif

    error = CM_OS_UnbindComponent(client, requiredItfClientName);

#ifdef CM_API_TIMING
    func[6].time += OSAL_GetTime() - start;
    func[6].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_BindComponentToVoid(const t_cm_instance_handle client, const char* requiredItfClientName)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[7].name = __func__;
#endif

    // Sanity check
    if ((client == 0) || (requiredItfClientName == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_BindComponentToVoid(client, requiredItfClientName);

#ifdef CM_API_TIMING
    func[7].time += OSAL_GetTime() - start;
    func[7].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_BindComponentAsynchronousEx(
    const t_cm_instance_handle client,
    const char* requiredItfClientName,
    const t_cm_instance_handle server,
    const char* providedItfServerName,
    t_uint32 fifosize,
    t_cm_mpc_memory_type eventMemType)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
#endif

    // Sanity check
    if ((fifosize == 0) || (fifosize > MAX_COMMUNICATION_FIFO_SIZE)
        || (client == 0) || (requiredItfClientName == NULL)
        || (server == 0) || (providedItfServerName == NULL))
            return CM_INVALID_PARAMETER;

    {
        char components[2][MAX_INTERFACE_TYPE_NAME_LENGTH];
        t_nmf_osal_fs_file_handle files[2];
        void *datas[2];
        t_cm_size sizes[2];

        if((error = CM_OS_GetRequiredComponentFiles(BIND_ASYNC, client, requiredItfClientName, server, providedItfServerName,
                components, 2, NULL, NULL)) != CM_OK)
            return error;

        if((error = cm_openComponents(2, components, files, datas, sizes, 0)) != CM_OK)
            return CM_BINDING_COMPONENT_NOT_FOUND;

        error = CM_OS_BindComponentAsynchronous(client, requiredItfClientName, server, providedItfServerName, fifosize, eventMemType,
                datas[0], sizes[0], datas[1], sizes[1]);

        cm_closeComponents(2, files);
    }

#ifdef CM_API_TIMING
    func[8].time += OSAL_GetTime() - start;
    func[8].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_BindComponentAsynchronous(const t_cm_instance_handle client, const char* requiredItfClientName, const t_cm_instance_handle server, const char* providedItfServerName, t_uint32 fifosize)
{
#ifdef CM_API_TIMING
    func[8].name = __func__;
#endif
    return CM_BindComponentAsynchronousEx(
          client,
          requiredItfClientName,
          server,
          providedItfServerName,
          fifosize,
          CM_MM_MPC_TCM24_X /*CM_MM_MPC_SDRAM24*/);
}

PUBLIC EXPORT_SHARED t_cm_error CM_UnbindComponentAsynchronous(const t_cm_instance_handle client, const char* requiredItfClientName)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[9].name = __func__;
#endif

    // Sanity check
    if ((client == 0) || (requiredItfClientName == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_UnbindComponentAsynchronous(client, requiredItfClientName);

#ifdef CM_API_TIMING
    func[9].time += OSAL_GetTime() - start;
    func[9].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_ReadComponentAttribute(const t_cm_instance_handle component, const char* attrName, t_uint24 *value)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[31].name = __func__;
#endif

    // Sanity check
    if ((component == 0) || (attrName == NULL) || (value == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_ReadComponentAttribute(component, attrName, value);

#ifdef CM_API_TIMING
    func[31].time += OSAL_GetTime() - start;
    func[31].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_WriteComponentAttribute(const t_cm_instance_handle component, const char* attrName, t_uint24 value)
{
    //Warning: Please do not use CM_WriteComponentAttribute API by default, unless you are sure what you are doing.
    //This API is not protected against concurrent access.
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[43].name = __func__;
#endif

    // Sanity check
    if ((component == 0) || (attrName == NULL) )
          return CM_INVALID_PARAMETER;

    error = CM_OS_WriteComponentAttribute(component, attrName, value);

#ifdef CM_API_TIMING
    func[43].time += OSAL_GetTime() - start;
    func[43].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetExecutiveEngineHandle(t_cm_domain_id domainId, t_cm_instance_handle *executiveEngineHandle)
{
    t_cm_error error;

    // Sanity check
    if (executiveEngineHandle == NULL)
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetExecutiveEngineHandle(domainId, executiveEngineHandle);

    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentListHeader(t_cm_instance_handle *headerComponent)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[41].name = __func__;
#endif

    // Sanity check
    if (headerComponent == NULL)
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetComponentListHeader(headerComponent);

#ifdef CM_API_TIMING
    func[41].time += OSAL_GetTime() - start;
    func[41].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentListNext(const t_cm_instance_handle prevComponent, t_cm_instance_handle *nextComponent)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[42].name = __func__;
#endif

    // Sanity check
    if ((prevComponent == 0) || (nextComponent == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetComponentListNext(prevComponent, nextComponent);

#ifdef CM_API_TIMING
    func[42].time += OSAL_GetTime() - start;
    func[42].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentDescription(
        const t_cm_instance_handle  component,
        char                        *templateName,
        t_uint32                    templateNameLength,
        t_nmf_core_id               *coreId,
        char                        *localName,
        t_uint32                    localNameLength,
	t_nmf_ee_priority           *priority)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[30].name = __func__;
#endif

    // Sanity check
    if ((component == 0) || (templateName == NULL) || (coreId == NULL) || (localName == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetComponentDescription(component, templateName, templateNameLength, coreId, localName, localNameLength, priority);

#ifdef CM_API_TIMING
    func[30].time += OSAL_GetTime() - start;
    func[30].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentRequiredInterfaceNumber(const t_cm_instance_handle component, t_uint8 *numberRequiredInterfaces)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[32].name = __func__;
#endif

    // Sanity check
    if ((component == 0) || (numberRequiredInterfaces == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetComponentRequiredInterfaceNumber(component, numberRequiredInterfaces);

#ifdef CM_API_TIMING
    func[32].time += OSAL_GetTime() - start;
    func[32].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentRequiredInterface(
        const t_cm_instance_handle  component,
        const t_uint8               index,
        char                        *itfName,
        t_uint32                    itfNameLength,
        char                        *itfType,
        t_uint32                    itfTypeLength,
        t_cm_require_state          *requireState,
        t_sint16                    *collectionSize)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[33].name = __func__;
#endif

    // Sanity check
    if ((component == 0) || (itfName == NULL) || (itfType == NULL) || (collectionSize == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetComponentRequiredInterface(component, index, itfName, itfNameLength, itfType, itfTypeLength, requireState, collectionSize);

#ifdef CM_API_TIMING
    func[33].time += OSAL_GetTime() - start;
    func[33].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentRequiredInterfaceBinding(
        const t_cm_instance_handle  component,
        const char                  *itfName,
        t_cm_instance_handle        *server,
        char                        *serverItfName,
        t_uint32                    serverItfNameLength)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[34].name = __func__;
#endif

    // Sanity check
    if ((component == 0) || (itfName == NULL) || (server == NULL) || (serverItfName == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetComponentRequiredInterfaceBinding(component, itfName, server, serverItfName, serverItfNameLength);

#ifdef CM_API_TIMING
    func[34].time += OSAL_GetTime() - start;
    func[34].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentProvidedInterfaceNumber(const t_cm_instance_handle component, t_uint8 *numberProvidedInterfaces)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[35].name = __func__;
#endif

    // Sanity check
    if ((component == 0) || (numberProvidedInterfaces == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetComponentProvidedInterfaceNumber(component, numberProvidedInterfaces);

#ifdef CM_API_TIMING
    func[35].time += OSAL_GetTime() - start;
    func[35].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentProvidedInterface(
        const t_cm_instance_handle  component,
        const t_uint8               index,
        char                        *itfName,
        t_uint32                    itfNameLength,
        char                        *itfType,
        t_uint32                    itfTypeLength,
        t_sint16                    *collectionSize)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[36].name = __func__;
#endif

    // Sanity check
    if ((component == 0) || (itfName == NULL) || (itfType == NULL) || (collectionSize == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetComponentProvidedInterface(component, index, itfName, itfNameLength, itfType, itfTypeLength, collectionSize);

#ifdef CM_API_TIMING
    func[36].time += OSAL_GetTime() - start;
    func[36].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentPropertyNumber(const t_cm_instance_handle component, t_uint8 *numberProperties)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[37].name = __func__;
#endif

    // Sanity check
    if ((component == 0) || (numberProperties == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetComponentPropertyNumber(component, numberProperties);

#ifdef CM_API_TIMING
    func[37].time += OSAL_GetTime() - start;
    func[37].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentPropertyName(
        const t_cm_instance_handle  component,
        const t_uint8               index,
        char                        *propertyName,
        t_uint32                    propertyNameLength)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[38].name = __func__;
#endif

    // Sanity check
    if ((component == 0) || (propertyName == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetComponentPropertyName(component, index, propertyName, propertyNameLength);

#ifdef CM_API_TIMING
    func[38].time += OSAL_GetTime() - start;
    func[38].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentPropertyValue(
        const t_cm_instance_handle  component,
        const char                  *propertyName,
        char                        *propertyValue,
        t_uint32                    propertyValueLength)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[39].name = __func__;
#endif

    // Sanity check
    if ((component == 0) || (propertyName == NULL) || (propertyValue == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_GetComponentPropertyValue(component, propertyName, propertyValue, propertyValueLength);

#ifdef CM_API_TIMING
    func[39].time += OSAL_GetTime() - start;
    func[39].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetComponentMemoryInfo(
        const char* templateName,
        t_cm_component_memory_info *pInfo)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[40].name = __func__;
#endif

    if ((templateName == 0) || (pInfo == NULL))
        return CM_INVALID_PARAMETER;

    error = cm_getComponentMemoryInfo(templateName, pInfo);
#ifdef CM_API_TIMING
    func[40].time += OSAL_GetTime() - start;
    func[40].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_AllocMpcMemory(
        t_cm_domain_id domainId,
        t_cm_mpc_memory_type memType,
        t_cm_size size,
        t_cm_mpc_memory_alignment memAlignment,
        t_cm_memory_handle *pHandle)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[10].name = __func__;
#endif

    // Sanity check
    if (pHandle == NULL)
	    return CM_INVALID_PARAMETER;

    error = CM_OS_AllocMpcMemory(domainId, memType, size, memAlignment, pHandle);

#ifdef CM_API_TIMING
    func[10].time += OSAL_GetTime() - start;
    func[10].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_FreeMpcMemory(t_cm_memory_handle handle)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[11].name = __func__;
#endif

   // Sanity check
    if (handle == 0)
        return CM_UNKNOWN_MEMORY_HANDLE;

    error = CM_OS_FreeMpcMemory(handle);

#ifdef CM_API_TIMING
    func[11].time += OSAL_GetTime() - start;
    func[11].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetMpcMemorySystemAddress(t_cm_memory_handle handle, t_cm_system_address *pSystemAddress)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[12].name = __func__;
#endif

    // Sanity check
    if (pSystemAddress == NULL)
	    return CM_INVALID_PARAMETER;
    if (handle == 0)
            return CM_UNKNOWN_MEMORY_HANDLE;

    error = CM_OS_GetMpcMemorySystemAddress(handle, pSystemAddress);

#ifdef CM_API_TIMING
    func[12].time += OSAL_GetTime() - start;
    func[12].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetMpcMemoryMpcAddress(t_cm_memory_handle handle, t_uint32 *pMpcAddress)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[13].name = __func__;
#endif

    // Sanity check
    if (pMpcAddress == NULL)
	    return CM_INVALID_PARAMETER;
    if (handle == 0)
            return CM_UNKNOWN_MEMORY_HANDLE;

    error = CM_OS_GetMpcMemoryMpcAddress(handle, pMpcAddress);

#ifdef CM_API_TIMING
    func[13].time += OSAL_GetTime() - start;
    func[13].call_count++;
#endif
   return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetMpcMemoryStatus(t_cm_domain_id domainId, t_cm_mpc_memory_type memType, t_cm_allocator_status *pStatus)
{
    t_cm_error error;
    t_nmf_core_id coreId;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[14].name = __func__;
#endif

    // Sanity check
    if (pStatus == NULL)
        return CM_INVALID_PARAMETER;
    if ((error = CM_OS_GetDomainCoreId(domainId, &coreId)) != CM_OK) {
        return error;
    }

    error = CM_OS_GetMpcMemoryStatus(coreId, memType, pStatus);

#ifdef CM_API_TIMING
    func[14].time += OSAL_GetTime() - start;
    func[14].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetMpcMemoryStatusEx(t_nmf_core_id coreId, t_cm_mpc_memory_type memType, t_cm_allocator_status *pStatus)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[15].name = __func__;
#endif

    // Sanity check
    if (pStatus == NULL)
        return CM_INVALID_PARAMETER;

     error = CM_OS_GetMpcMemoryStatus(coreId, memType, pStatus);

#ifdef CM_API_TIMING
    func[15].time += OSAL_GetTime() - start;
    func[15].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_CreateMemoryDomain(const t_cm_domain_memory *domain, t_cm_domain_id *handle)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[16].name = __func__;
#endif
    error = CM_OS_CreateMemoryDomain(domain, NMF_CURRENT_CLIENT, handle);
#ifdef CM_API_TIMING
    func[16].time += OSAL_GetTime() - start;
    func[16].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_CreateMemoryDomainForClient(const t_cm_domain_memory *domain,  t_nmf_client_id client, t_cm_domain_id *handle)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[16].name = __func__;
#endif
    error = CM_OS_CreateMemoryDomain(domain, client, handle);
#ifdef CM_API_TIMING
    func[16].time += OSAL_GetTime() - start;
    func[16].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_CreateMemoryDomainScratch(const t_cm_domain_id parentId, const t_cm_domain_memory *domain, t_cm_domain_id *handle)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[18].name = __func__;
#endif
    error = CM_OS_CreateMemoryDomainScratch(parentId, domain, handle);
#ifdef CM_API_TIMING
    func[18].time += OSAL_GetTime() - start;
    func[18].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_DestroyMemoryDomain(const t_cm_domain_id handle)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[17].name = __func__;
#endif
    error = CM_OS_DestroyMemoryDomain(handle);
#ifdef CM_API_TIMING
    func[17].time += OSAL_GetTime() - start;
    func[17].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetDomainCoreId(const t_cm_domain_id domainId, t_nmf_core_id *coreId)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[19].name = __func__;
#endif
    error = CM_OS_GetDomainCoreId(domainId, coreId);
#ifdef CM_API_TIMING
    func[19].time += OSAL_GetTime() - start;
    func[19].call_count++;
#endif
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_Migrate(const t_cm_domain_id srcShared, const t_cm_domain_id src, const t_cm_domain_id dst)
{
    t_cm_error error;
    error = CM_OS_Migrate(srcShared, src, dst);
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_Unmigrate(void)
{
    t_cm_error error;
    error = CM_OS_Unmigrate();
    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_getMpcLoadCounter(
    t_nmf_core_id coreId,
    t_cm_mpc_load_counter *pMpcLoadCounter)
{
    t_cm_error error;

    // Sanity check
    if (pMpcLoadCounter == NULL)
	    return CM_INVALID_PARAMETER;

    error = CM_OS_getMpcLoadCounter(coreId, pMpcLoadCounter);

    return error;
}

PUBLIC EXPORT_SHARED t_cm_error CM_SetMode(t_cm_cmd_id aCmdID, t_sint32 aParam)
{
    t_cm_error error;

    error = CM_OS_SetMode(aCmdID,aParam);

	return error;
}

PUBLIC EXPORT_SHARED const char *CM_StringError(t_nmf_error error)
{
    switch(error)
    {
    case NMF_INTEGRATION_ERROR9: return "NMF_INTEGRATION_ERROR9";
    case NMF_INTEGRATION_ERROR8: return "NMF_INTEGRATION_ERROR8";
    case NMF_INTEGRATION_ERROR7: return "NMF_INTEGRATION_ERROR7";
    case NMF_INTEGRATION_ERROR6: return "NMF_INTEGRATION_ERROR6";
    case NMF_INTEGRATION_ERROR5: return "NMF_INTEGRATION_ERROR5";
    case NMF_INTEGRATION_ERROR4: return "NMF_INTEGRATION_ERROR4";
    case NMF_INTEGRATION_ERROR3: return "NMF_INTEGRATION_ERROR3";
    case NMF_INTEGRATION_ERROR2: return "NMF_INTEGRATION_ERROR2";
    case NMF_INTEGRATION_ERROR1: return "NMF_INTEGRATION_ERROR1";
    case NMF_INTEGRATION_ERROR0: return "NMF_INTEGRATION_ERROR0";
    /* Communication */
    case CM_FLUSH_MESSAGE: return "[CM|NMF]_FLUSH_MESSAGE";
    case CM_BUFFER_OVERFLOW: return "CM_BUFFER_OVERFLOW";
    case CM_USER_NOT_REGISTERED: return "CM_USER_NOT_REGISTERED";
    case CM_NO_MESSAGE: return "[CM|NMF]_NO_MESSAGE";
    case CM_PARAM_FIFO_OVERFLOW: return "CM_PARAM_FIFO_OVERFLOW";
    case CM_INTERNAL_FIFO_OVERFLOW: return "CM_INTERNAL_FIFO_OVERFLOW";
    case CM_MPC_NOT_RESPONDING: return "CM_MPC_NOT_RESPONDING";

    /* ELF & File system */
    case CM_FS_ERROR: return "CM_FS_ERROR";
    case CM_NO_SUCH_FILE: return "CM_NO_SUCH_FILE";
    case CM_INVALID_ELF_FILE: return "CM_INVALID_ELF_FILE";
    case CM_NO_SUCH_BASE: return "CM_NO_SUCH_BASE";
    /* Introspection */
    case CM_NO_SUCH_ATTRIBUTE: return "CM_NO_SUCH_ATTRIBUTE";
    case CM_NO_SUCH_PROPERTY: return "CM_NO_SUCH_PROPERTY";
    /* Component life cycle */
    case NMF_COMPONENT_NOT_STOPPED: return "NMF_COMPONENT_NOT_STOPPED";
    case CM_COMPONENT_NOT_UNBINDED: return "CM_COMPONENT_NOT_UNBINDED";
    case CM_COMPONENT_NOT_STARTED: return "CM_COMPONENT_NOT_STARTED";
    case CM_COMPONENT_WAIT_RUNNABLE: return "CM_COMPONENT_WAIT_RUNNABLE";
    case CM_REQUIRE_INTERFACE_UNBINDED: return "CM_REQUIRE_INTERFACE_UNBINDED";
    case CM_INVALID_COMPONENT_HANDLE: return "CM_INVALID_COMPONENT_HANDLE";
    /* Binded */
    case NMF_NO_SUCH_PROVIDED_INTERFACE: return "NMF_NO_SUCH_PROVIDED_INTERFACE";
    case NMF_NO_SUCH_REQUIRED_INTERFACE: return "NMF_NO_SUCH_REQUIRED_INTERFACE";
    case CM_ILLEGAL_BINDING: return "CM_ILLEGAL_BINDING";
    case CM_ILLEGAL_UNBINDING: return "CM_ILLEGAL_UNBINDING";
    case CM_INTERFACE_ALREADY_BINDED: return "CM_INTERFACE_ALREADY_BINDED";
    case NMF_INTERFACE_NOT_BINDED: return "NMF_INTERFACE_NOT_BINDED";
    /* Loader & binder */
    case CM_BINDING_COMPONENT_NOT_FOUND: return "CM_BINDING_COMPONENT_NOT_FOUND";
    case CM_COMPONENT_NOT_FOUND: return "CM_COMPONENT_NOT_FOUND";
    case CM_NO_SUCH_SYMBOL: return "CM_NO_SUCH_SYMBOL";
    case CM_COMPONENT_EXIST: return "CM_COMPONENT_EXIST";
    /* Fifo management related ones */
    case CM_FIFO_FULL: return "CM_FIFO_FULL";
    case CM_FIFO_EMPTY: return "CM_FIFO_EMPTY";
    case CM_UNKNOWN_FIFO_ID: return "CM_UNKNOWN_FIFO_ID";
    /* Memory management related ones */
    case CM_CREATE_ALLOC_ERROR: return "CM_CREATE_ALLOC_ERROR";
    case CM_UNKNOWN_MEMORY_HANDLE: return "CM_UNKNOWN_MEMORY_HANDLE";
    case NMF_NO_MORE_MEMORY: return "NMF_NO_MORE_MEMORY";
    /* Media Processor related ones */
    case CM_BAD_MEMORY_ALIGNMENT: return "CM_BAD_MEMORY_ALIGNMENT";
    /* Domain managment related ones */
    case CM_MEMORY_HANDLE_FREED: return "CM_MEMORY_HANDLE_FREED";
    case CM_INVALID_DOMAIN_DEFINITION: return "CM_INVALID_DOMAIN_DEFINITION";
    case CM_INTERNAL_DOMAIN_OVERFLOW: return "CM_INTERNAL_DOMAIN_OVERFLOW";
    case CM_INVALID_DOMAIN_HANDLE: return "CM_INVALID_DOMAIN_HANDLE";
    case CM_ILLEGAL_DOMAIN_OPERATION: return "CM_ILLEGAL_DOMAIN_OPERATION";
    case CM_DOMAIN_VIOLATION: return "CM_DOMAIN_VIOLATION";

    /* Media Processor related ones */
    case CM_MPC_INVALID_CONFIGURATION: return "CM_MPC_INVALID_CONFIGURATION";
    case CM_MPC_NOT_INITIALIZED: return "CM_MPC_NOT_INITIALIZED";
    case CM_MPC_ALREADY_INITIALIZED: return "CM_MPC_ALREADY_INITIALIZED";
    /* Power Mgt related ones */
    case CM_PWR_NOT_AVAILABLE: return "CM_PWR_NOT_AVAILABLE";
    /* Common errors */
    case CM_INVALID_DATA: return "CM_INVALID_DATA";
    case CM_OUT_OF_LIMITS: return "CM_OUT_OF_LIMITS";
    case NMF_INVALID_PARAMETER: return "NMF_INVALID_PARAMETER";
    case CM_NOT_YET_IMPLEMENTED: return "CM_NOT_YET_IMPLEMENTED";
    case NMF_OK: return "NMF_OK";
    default: return "CM_UNKNOWN_ERROR";
    }
}
