/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Component Manager Introspection API.
 *
 * This file contains the Component Manager API for introspect components.
 */
/*!
 * \defgroup COMPONENT_INTROSPECTION CM Components Network Introspection API
 * \ingroup CM_USER_API
 */

#ifndef COMMON_INTROSPECTION_WRAPPER_H
#define COMMON_INTROSPECTION_WRAPPER_H

#include <cm/engine/component/inc/component_type.h>
#include <cm/proxy/common/component/inc/component_info.h>
#include <inc/nmf-limits.h>

/*!
 * \brief Get the executive engine handle for given core.
 *
 * Return the executive engine handle for coreId. In case executive engine is not loaded it return null.
 *
 * \param[in]  domainId The domain identifier for which we want the executive engine handle.
 * \param[out] executiveEngineHandle executive engine instance (null if the executive engine is not loaded)
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetExecutiveEngineHandle(
     t_cm_domain_id               domainId,
     t_cm_instance_handle        *executiveEngineHandle);

/*!
 * \brief Get a component created by the process.
 *
 * Return a loaded component of the instantiated component network.
 *
 * This method can be used to obtain a component that can be used as an entry point
 * for browsing instantiated component network (used conjointly with \ref CM_GetComponentListNext).
 *
 * This code illustrate how browsing component network: \code
   t_cm_instance_handle component;

   CM_GetComponentListHeader(&component);
   while(component != 0) {
      t_nmf_core_id coreId;
      char templateName[MAX_TEMPLATE_NAME_LENGTH];
      char localName[MAX_COMPONENT_NAME_LENGTH];

      CM_GetComponentDescription(component, templateName, MAX_TEMPLATE_NAME_LENGTH, &coreId, localName, MAX_COMPONENT_NAME_LENGTH);
      ALOG("[%x] type=%s dsp=%d name=%s",
          component, templateName, coreId, localName);

      CM_GetComponentListNext(component, &component);
   }
 * \endcode
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentListHeader(
    t_cm_instance_handle        *headerComponent                //!< [out] The instance of a component loaded by the process (null if the no component loaded)
    );


/*!
 * \brief Get a next component.
 *
 * Return a next component in the instantiated component network.
 *
 * This method may be called repeatedly to iterate through the network components.
 * The list resulting from calling this method is ordered by date of instantiation.
 *
 * See \ref CM_GetComponentListHeader for detail about how this method can be used.
 *
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentListNext(
    const t_cm_instance_handle  prevComponent,                  //!< [in] A component instance handle
    t_cm_instance_handle        *nextComponent                  //!< [out] The instance of a next component loaded by the process (null if the last one)
    );


/*!
 * \brief Get a component description
 *
 * Get some description about the component:
 * - the component template name,
 * - the core where the component is loaded,
 * - the component local name (gived during instantiation).
 *
 * This method can be used like this: \code
   t_cm_instance_handle component;
   t_cm_error error;
   t_nmf_core_id coreId;
   char templateName[MAX_TEMPLATE_NAME_LENGTH];
   char localName[MAX_COMPONENT_NAME_LENGTH];

   error = CM_GetComponentDescription(component,
        templateName, MAX_TEMPLATE_NAME_LENGTH, &coreId,
        localName, MAX_COMPONENT_NAME_LENGTH);
 * \endcode
 *
 * \param[in] component The component instance handle.
 * \param[in] templateName Address of buffer where component template name mut be write.
 * \param[in] templateNameLength Size of templateName buffer.
 * \param[out] coreId The core where the component is loaded.
 * \param[in] localName Address of buffer where component local name mut be write.
 * \param[in] localNameLength Size of localName buffer.
 * \param[out] priority The priority of the component (optional, may be NULL).
 *
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentDescription(
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
 * Return the number of required interface that have been declared in the
 * component configuration file.
 *
 * Component introspection required interface methods can be used like this: \code
    t_cm_instance_handle component;
    t_uint8 requireNb, i;

    CM_GetComponentRequiredInterfaceNumber(component, &requireNb);
    for(i = 0; i < requireNb; i++) {
        char itfName[MAX_INTERFACE_NAME_LENGTH];
        char itfType[MAX_INTERFACE_TYPE_NAME_LENGTH];
        t_sint16 collectionSize;
        t_cm_instance_handle server;
        char serverItfName[MAX_INTERFACE_NAME_LENGTH];

        CM_GetComponentRequiredInterface(component, i,
            itfName, MAX_INTERFACE_NAME_LENGTH,
            itfType, MAX_INTERFACE_TYPE_NAME_LENGTH,
            &collectionSize);

        if(collectionSize == -1) {
            ALOG("  -> %s:%s", itfName, itfType);

            CM_GetComponentRequiredInterfaceBinding(component, itfName,
                &server, serverItfName, MAX_INTERFACE_NAME_LENGTH);

            if(server != NULL)
                 ALOG("    [%x].%s", server, serverItfName);
        } else {
            int j;

            ALOG("  -> %s:%s [%d]", itfName, itfType, collectionSize);

            for(j = 0; j < collectionSize; j++) {
                char tmpname[256];
                sprintf(tmpname, "%s[%d]", itfName, j);

                CM_GetComponentRequiredInterfaceBinding(component, tmpname,
                    &server, serverItfName, MAX_INTERFACE_NAME_LENGTH);

                if(server != NULL)
                    ALOG("    %d: [%x].%s", j, server, serverItfName);
            }
        }
    }
 * \endcode
 *
 * \param[in] component The component instance handle.
 * \param[out] numberRequiredInterfaces The number of required interface
 *
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentRequiredInterfaceNumber(
    const t_cm_instance_handle  component,
    t_uint8                     *numberRequiredInterfaces);


/*!
 * \brief Return information about required interface.
 *
 * Return the description of a required interface that have been declared in the
 * component configuration file according its index.
 *
 * See \ref CM_GetComponentRequiredInterfaceNumber for detail about how this method can be used.
 *
 * \param[in] component The component instance handle.
 * \param[in] index Required interface index.
 * \param[out] itfName Address of buffer where name of the required interface must be write.
 * \param[in] itfNameLength Size of itfName buffer.
 * \param[out] itfType Address of buffer where type of the required interface must be write.
 * \param[in] itfTypeLength Size of itfType buffer.
 * \param[out] requireState Address where the required interface state must be write.
 * \param[out] collectionSize Size of the collection (-1 if not a collection).
 *
 * \exception CM_NO_SUCH_REQUIRED_INTERFACE The required interface index is out of range [0 .. numberRequiredInterfaces[.
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentRequiredInterface(
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
 * Return information about the interface provided by a server component that has been
 * binded to a required interface.
 *
 * See \ref CM_GetComponentRequiredInterfaceNumber for detail about how this method can be used.
 *
 * \param[in] component The component instance handle.
 * \param[in] itfName Name of the required interface.
 * \param[out] server Server component instance (NMF_HOST_COMPONENT if component on host call through MPC->Host binding).
 * \param[in] serverItfName Address of buffer where interface name provided by the server component where
 *      is bind the client component must be write.
 * \param[in] serverItfNameLength Size of serverItfName buffer.
 *
 * \exception CM_NO_SUCH_REQUIRED_INTERFACE The required interface name is unknown.
 * \exception CM_INTERFACE_NOT_BINDED The required interface is not binded.
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentRequiredInterfaceBinding(
        const t_cm_instance_handle  component,
        const char                  *itfName,
        t_cm_instance_handle        *server,
        char                        *serverItfName,
        t_uint32                    serverItfNameLength);

/*!
 * \brief Get number of interface provided by a component.
 *
 * Return the number of provided interface that have been declared in the
 * component configuration file.
 *
 * Component introspection provided interface methods can be used like this: \code
    t_cm_instance_handle component;
    t_uint8 provideNb, i;

    CM_GetComponentProvidedInterfaceNumber(component, &provideNb);
    for(i = 0; i < provideNb; i++) {
        char itfName[MAX_INTERFACE_NAME_LENGTH];
        char itfType[MAX_INTERFACE_TYPE_NAME_LENGTH];
        t_sint16 collectionSize;

        CM_GetComponentProvidedInterface(component, i,
                    itfName, MAX_INTERFACE_NAME_LENGTH,
                    itfType, MAX_INTERFACE_TYPE_NAME_LENGTH,
                    &collectionSize);

        if(collectionSize != -1)
            ALOG("  <- %s:%s [%d]", itfName, itfType, collectionSize);
        else
            ALOG("  <- %s:%s", itfName, itfType);
    }
 * \endcode
 *
 * \param[in] component The component instance handle.
 * \param[out] numberProvidedInterfaces The number of provided interface
 *
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentProvidedInterfaceNumber(
    const t_cm_instance_handle  component,
    t_uint8                     *numberProvidedInterfaces);


/*!
 * \brief Return information about provided interface.
 *
 * Return the description of a provided interface that have been declared in the
 * component configuration file according its index.
 *
 * See \ref CM_GetComponentProvidedInterfaceNumber for detail about how this method can be used.
 *
 * \param[in] component The component instance handle.
 * \param[in] index Provided interface index.
 * \param[in] itfName Address of buffer where name of the provided interface must be write.
 * \param[in] itfNameLength Size of itfName buffer.
 * \param[in] itfType Address of buffer where type of the provided interface must be write.
 * \param[in] itfTypeLength Size of itfType buffer.
 * \param[out] collectionSize Size of the collection (-1 if not a collection).
 *
 * \exception CM_NO_SUCH_PROVIDED_INTERFACE The provided interface index is out of range [0 .. numberProvidedInterfaces[.
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentProvidedInterface(
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
 * Return the number of properties that have been declared in the
 * component configuration file.
 *
 * Component introspection property methods can be used like this: \code
   t_cm_instance_handle component;
   t_uint8 propertyNumber, index;

   CM_GetComponentPropertyNumber(
         component,
         &propertyNumber);

   for(index = 0; index < propertyNumber; index++) {
       char propertyName[MAX_PROPERTY_NAME_LENGTH];
       char propertyValue[MAX_PROPERTY_VALUE_LENGTH];
       CM_GetComponentPropertyName(component, index, propertyName, MAX_PROPERTY_NAME_LENGTH);
       CM_GetComponentPropertyValue(component, propertyName, propertyValue, MAX_PROPERTY_VALUE_LENGTH);

       ALOG("Property %s = %s\n", propertyName, propertyValue);
   }
 * \endcode
 *
 * \param[in] component The component instance handle.
 * \param[out] numberProperties The number of properties.
 *
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentPropertyNumber(
    const t_cm_instance_handle  component,
    t_uint8                     *numberProperties);

/*!
 * \brief Return the name of a property.
 *
 * Return the name of a property that have been declared in the
 * component configuration file according its index.
 *
 * See \ref CM_GetComponentPropertyNumber for detail about how this method can be used.
 *
 * \param[in] component The component instance handle.
 * \param[in] index Property index.
 * \param[in] propertyName Address of buffer where property name mut be write.
 * \param[in] propertyNameLength Size of propertyName buffer.
 *
 * \exception CM_NO_SUCH_PROPERTY The property index is out of range [0 .. numberProperties[.
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentPropertyName(
        const t_cm_instance_handle  component,
        const t_uint8               index,
        char                        *propertyName,
        t_uint32                    propertyNameLength);

/*!
 * \brief Get property value of a component.
 *
 * Get the property value of a component. This property must have been declared as an property in the
 * component configuration file.
 *
 * This method can be used like this: \code
   t_cm_instance_handle component;
   t_cm_error error;
   char value[MAX_PROPERTY_VALUE_LENGTH];

   error = CM_GetComponentPropertyValue(
         component,
         "prop",
         value,
         MAX_PROPERTY_VALUE_LENGTH);
 * \endcode
 * The name of the property can be either well-known for the component type or founded dynamically through
 * \ref CM_GetComponentPropertyNumber and \ref CM_GetComponentPropertyName.
 *
 * \param[in] component The component instance handle.
 * \param[in] propertyName The component property name.
 * \param[in] propertyValue Address of buffer where property value mut be write.
 * \param[in] propertyValueLength Size of propertyValue buffer.
 *
 * \exception CM_NO_SUCH_PROPERTY The property name is unknown.
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentPropertyValue(
        const t_cm_instance_handle  component,
        const char                  *propertyName,
        char                        *propertyValue,
        t_uint32                    propertyValueLength);

/*!
 * \brief Get component memory info
 *
 * Get the information about how much memory the compnent would occupy after its instantiation.
 * All memory types are accounted for and two instantiation types are considered
 *   - initial instantiation, and
 *   - subsequent instantiations.
 *
 * \param[in] templateName The component name.
 * \param[out] pInfo The info structure gathering all above information
 *
 * \exception CM_INVALID_COMPONENT_HANDLE

 * \ingroup COMPONENT_INTROSPECTION
 */
PUBLIC IMPORT_SHARED t_cm_error CM_GetComponentMemoryInfo(
        const char* templateName,
        t_cm_component_memory_info *pInfo);

#endif
