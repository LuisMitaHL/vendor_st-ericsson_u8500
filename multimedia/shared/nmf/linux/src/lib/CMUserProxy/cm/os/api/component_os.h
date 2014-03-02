/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Components Component Manager Operating System API.
 *
 * This file contains the Component Manager Operating System API for manipulating components.
 *
 */

#ifndef COMPONENT_OS_H_
#define COMPONENT_OS_H_

#include <cm/engine/component/inc/component_type.h>
#include <cm/engine/communication/inc/communication_type.h>
#include <cm/engine/api/channel_engine.h>
#include <inc/nmf-limits.h>

/*!
 * \brief Instantiate a new component.
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_InstantiateComponent(
    const char templateName[MAX_TEMPLATE_NAME_LENGTH],  //!< [in] templateName  Null terminated string (Max size=\ref MAX_TEMPLATE_NAME_LENGTH)
    t_cm_domain_id domainId,                            //!< [in] Domain
    t_nmf_ee_priority priority,                         //!< [in] Component priority
    const char localName[MAX_COMPONENT_NAME_LENGTH],    //!< [in] Null terminated string (Max size=\ref MAX_COMPONENT_NAME_LENGTH)
    const char *dataFile,                               //!< [in] Data File (null if take in cache), according your OS integration, don't forgot to do CopyFromUser
    t_uint32 dataFileSize,                              //!< [in] Data File Size
    t_cm_instance_handle *component                     //!< [out] Component handle
    );

/*!
 * \brief Start a component.
 *
 * \param[in] component
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_StartComponent(
    t_cm_instance_handle component);

/*!
 * \brief Stop a component.
 *
 * \param[in] component
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_StopComponent(
    t_cm_instance_handle component);

/*!
 * \brief Destroy a component.
 *
 * \param[in] component
 *
 * \ingroup CM_ENGINE_API
 */
PUBLIC t_cm_error CM_OS_DestroyComponent(
    t_cm_instance_handle component);

/*!
 * \brief Bind two components together.
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_BindComponent(
    const t_cm_instance_handle          client,                                             //!<
    const char                          requiredItfClientName[MAX_INTERFACE_NAME_LENGTH],   //!< Null terminated string (Max size=\ref MAX_INTERFACE_NAME_LENGTH).
    const t_cm_instance_handle          server,                                             //!<
    const char                          providedItfServerName[MAX_INTERFACE_NAME_LENGTH],   //!< Null terminated string (Max size=\ref MAX_INTERFACE_NAME_LENGTH).
    t_bool                              traced,                                             //!< FALSE for synchronous binding, TRUE for traced one
    const char                          *dataFileTrace,                                     //!< Component file data in case on traced (Note: could be null if file already in cache)
    t_uint32                            dataFileTraceSize                                   //!< Component file data size
    );

/*!
 * \brief Unbind a component.
 *
 * \param[in] client
 * \param[in] requiredItfClientName Null terminated string (Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_UnbindComponent(
    const t_cm_instance_handle client,
    const char* requiredItfClientName);

/*!
 * \brief Bind a component to void (silently ignore a call).
 *
 * \param[in] client
 * \param[in] requiredItfClientName Null terminated string (Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_BindComponentToVoid(
    const t_cm_instance_handle client,
    const char* requiredItfClientName);

/*!
 * \brief Bind two components together in an asynchronous way
 * (the components can be on the same MPC or on two different MPC)
 *
 * \param[in] client
 * \param[in] requiredItfClientName Null terminated string (Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 * \param[in] server
 * \param[in] providedItfServerName Null terminated string (Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 * \param[in] fifosize
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_BindComponentAsynchronous(
    const t_cm_instance_handle client,
    const char* requiredItfClientName,
    const t_cm_instance_handle server,
    const char* providedItfServerName,
    t_uint32 fifosize,
    t_cm_mpc_memory_type eventMemType,
    const char *dataFileSkeletonOrEvent, t_uint32 dataFileSkeletonOrEventSize,
    const char *dataFileStub, t_uint32 dataFileStubSize);

/*!
 * \brief Unbind a component previously binded asynchronously
 *
 * \param[in] client
 * \param[in] requiredItfClientName Null terminated string (Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_UnbindComponentAsynchronous(
    const t_cm_instance_handle client,
    const char* requiredItfClientName);

/*!
 * \brief Bind the Host to a component.
 *
 * \param[in] server
 * \param[in] providedItfServerName Null terminated string (Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 * \param[in] fifosize
 * \param[out] host2mpcId
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_BindComponentFromCMCore(
    const t_cm_instance_handle server,
    const char* providedItfServerName,
    t_uint32 fifosize,
    t_cm_mpc_memory_type eventMemType,
    t_cm_bf_host2mpc_handle *host2mpcId,
    const char *dataFileSkeleton,
    t_uint32 dataFileSkeletonSize);

/*!
 * \brief Unbind a component from the Host.
 *
 * \param[in] host2mpcId
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_UnbindComponentFromCMCore(t_cm_bf_host2mpc_handle host2mpcId);

/*!
 * \brief Bind a component to the Host.
 *
 * This method is called by CM Proxy. The implementation of this method must
 * do wrapping required by OS integration and call \ref CM_ENGINE_BindComponentToCMCore.
 *
 * See \ref MPC2HOST "MPC->Host binding" for seeing an integration example.
 *
 * \param[in] channel               The channel on which the bind must be done
 * \param[in] client                Handle of the client component
 * \param[in] requiredItfClientName Interface name of the client component. Null terminated string (Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 * \param[in] fifosize              Number of simultaneous communication
 * \param[in] upLayerThis       Context of the upper layer (a.k.a CM Proxy)
 * \param[out] mpc2hostId       Handle to the MPC->Host binding.
 *
 * \note No implementation of this method is provided in kernel CM engine!!!
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_BindComponentToCMCore(
	const t_os_channel          channel,
        const t_cm_instance_handle  client,
        const char                  *requiredItfClientName,
        t_uint32                    fifosize,
        t_nmf_mpc2host_handle       upLayerThis,
        const char                  *dataFileStub,
        t_uint32                    dataFileStubSize,
        t_cm_bf_mpc2host_handle     *mpc2hostId);

/*!
 * \brief Unbind a component to the Host.
 *
 * \param[in] channel               The channel on which the bind has been done
 * \param[in] client                Handle of the client component
 * \param[in] requiredItfClientName Interface name of the client component. Null terminated string (Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 * \param[out] upLayerThis Return the upLayerThis handle that have been gived during call to \ref CM_OS_BindComponentToCMCore.
 *      The purpose of this information was to allow easy free of user data without need to memorize them by top level user.
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_UnbindComponentToCMCore(
	const t_os_channel              channel,
        const t_cm_instance_handle      client,
        const char                      *requiredItfClientName,
        t_nmf_mpc2host_handle           *upLayerThis);

/*!
 * \brief Read a value on an attribute exported by a component instance.
 *
 * \param[in] component
 * \param[in] attrName  Null terminated string (Max size=\ref MAX_ATTRIBUTE_NAME_LENGTH).
 * \param[out] value
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_ReadComponentAttribute(
    const t_cm_instance_handle component,
    const char* attrName,
    t_uint24 *value);


PUBLIC t_cm_error CM_OS_WriteComponentAttribute(
    const t_cm_instance_handle component,
    const char* attrName,
    t_uint24 value);

/*!
 * \brief Get the older component.
 *
 * \param[out] headerComponent
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetComponentListHeader(
    t_cm_instance_handle        *headerComponent);

/*!
 * \brief Get the next component.
 *
 * \param[in] prevComponent
 * \param[out] nextComponent
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetComponentListNext(
    const t_cm_instance_handle  prevComponent,
    t_cm_instance_handle        *nextComponent);

/*!
 * \brief Get a component description
 *
 * \param[in] component
 * \param[in] templateNameLength
 * \param[in] localNameLength
 * \param[out] templateName         Null terminated string (Size=templateNameLength, Max size=\ref MAX_TEMPLATE_NAME_LENGTH).
 * \param[out] coreId
 * \param[out] localName            Null terminated string (Size=localNameLength, Max size=\ref MAX_COMPONENT_NAME_LENGTH).
 * \param[out] priority             The priority of the component
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetComponentDescription(
        const t_cm_instance_handle  component,
        char                        *templateName,
        t_uint32                    templateNameLength,
        t_nmf_core_id               *coreId,
        char                        *localName,
        t_uint32                    localNameLength,
    t_nmf_ee_priority           *priority);

/*!
 * \brief Get number of interface required by a component.
 *
 * \param[in] component
 * \param[out] numberRequiredInterfaces
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetComponentRequiredInterfaceNumber(
    const t_cm_instance_handle  component,
    t_uint8                     *numberRequiredInterfaces);

/*!
 * \brief Return information about required interface.
 *
 * \param[in] component
 * \param[in] index
 * \param[in] itfNameLength
 * \param[in] itfTypeLength
 * \param[out] itfName          Null terminated string (Size=itfNameLength, Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 * \param[out] itfType          Null terminated string (Size=itfTypeLength, Max size=\ref MAX_INTERFACE_TYPE_NAME_LENGTH).
 * \param[out] requireState
 * \param[out] collectionSize
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetComponentRequiredInterface(
        const t_cm_instance_handle  component,
        const t_uint8               index,
        char                        *itfName,
        t_uint32                    itfNameLength,
        char                        *itfType,
        t_uint32                    itfTypeLength,
        t_cm_require_state          *requireState,
        t_sint16                    *collectionSize);

/*!
 * \brief Get the component binded to a required interface.
 *
 * \param[in] component
 * \param[in] itfName               Null terminated string (Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 * \param[in] serverItfNameLength
 * \param[out] server
 * \param[out] serverItfName        Null terminated string (Size=serverItfNameLength, Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetComponentRequiredInterfaceBinding(
        const t_cm_instance_handle  component,
        const char                  *itfName,
        t_cm_instance_handle        *server,
        char                        *serverItfName,
        t_uint32                    serverItfNameLength);

/*!
 * \brief Get number of interface provided by a component.
 *
 * \param[in] component
 * \param[out] numberProvidedInterfaces
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetComponentProvidedInterfaceNumber(
    const t_cm_instance_handle  component,
    t_uint8                     *numberProvidedInterfaces);

/*!
 * \brief Return information about provided interface.
 *
 * \param[in] component
 * \param[in] index
 * \param[in] itfNameLength
 * \param[in] itfTypeLength
 * \param[out] itfName          Null terminated string (Size=itfNameLength, Max size=\ref MAX_INTERFACE_NAME_LENGTH).
 * \param[out] itfType          Null terminated string (Size=itfTypeLength, Max size=\ref MAX_INTERFACE_TYPE_NAME_LENGTH).
 * \param[out] collectionSize
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetComponentProvidedInterface(
        const t_cm_instance_handle  component,
        const t_uint8               index,
        char                        *itfName,
        t_uint32                    itfNameLength,
        char                        *itfType,
        t_uint32                    itfTypeLength,
        t_sint16                    *collectionSize);

/*!
 * \brief Get number of properties of a component.
 *
 * \param[in] component
 * \param[out] numberProperties
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetComponentPropertyNumber(
    const t_cm_instance_handle  component,
    t_uint8                     *numberProperties);

/*!
 * \brief Return the name of a property.
 *
 * \param[in] component
 * \param[in] index
 * \param[in] propertyNameLength
 * \param[out] propertyName         Null terminated string (Size=propertyNameLength, Max size=\ref MAX_PROPERTY_NAME_LENGTH).
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetComponentPropertyName(
        const t_cm_instance_handle  component,
        const t_uint8               index,
        char                        *propertyName,
        t_uint32                    propertyNameLength);

/*!
 * \brief Get property value of a component.
 *
 * \param[in] component
 * \param[in] propertyName
 * \param[in] propertyValueLength
 * \param[out] propertyValue         Null terminated string (Size=propertyValueLength, Max size=\ref MAX_PROPERTY_VALUE_LENGTH).
 *
 * \ingroup CM_OS_API
 */
PUBLIC t_cm_error CM_OS_GetComponentPropertyValue(
        const t_cm_instance_handle  component,
        const char                  *propertyName,
        char                        *propertyValue,
        t_uint32                    propertyValueLength);

#endif /*COMPONENT_OS_H_*/
