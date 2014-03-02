/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Components Component Manager API.
 *
 * This file contains the Component Manager API for manipulating components.
 */

#ifndef COMMON_COMPONENT_WRAPPER_H
#define COMMON_COMPONENT_WRAPPER_H

#include <cm/engine/component/inc/component_type.h>
#include <cm/engine/communication/inc/communication_type.h>
#include <cm/proxy/common/component/inc/interface_type.h>

#include <cm/engine/memory/inc/domain_type.h>

/*!
 * \brief Push a file component in the Component Manager file cache.
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_PushComponent(
    const char* templateName);

/*!
 * \brief Release a file component in the Component Manager file cache.
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_ReleaseComponent(
    const char* templateName);

/*!
 * \brief Instantiate a new component.
 *
 * Instantiate a new component from a specific template type into
 * a specific MPC by allocating memory for component instance inside the
 * MPC memories (XRAM, YRAM, EMBEDDED, EXTERNAL).
 *
 * If the template has not been loaded yet, this method
 * load the code and the shared read only data inside memories
 * (XROM, YROM, EMBEDDED_ROM, EXTERNAL_ROM, PROG, EMBEDDED_PROG).
 *
 * This method can be used like this: \code
 * t_cm_instance_handle mytest;
 * t_cm_error error;
 *
 * error = CM_InstantiateComponent(
 *      "test",
 *      SVA_CORE_ID,
 *      NMF_SCHED_NORMAL,
 *      "mytest",
 *      &mytest);
 *
 * error = CM_StartComponent(mytest);
 *
 * ...
 *
 * error = CM_StopComponent(mytest);
 *
 * error = CM_DestroyComponent(mytest);
 * \endcode
 *
 * In fact, this method do exactly the same thing as the definition below in the
 * architecture file in the static NMF approach:
 * \code
 * contains test as mytest
 * \endcode
 *
 * Concerning Singleton behavior:
 *  - Same instance handle is returned.
 *  - First instance of the singleton is instantiated in the domain passed as parameter
 *  - Binding is immutable and thus first binding will be the winner, other binding must be exactly the same for coherence purpose
 *  - Component is start during the first start
 *  - Component is stop during the last stop
 *  - Each interface required by the singleton is really unbinded when the last binded interface is really unbinded
 *  - Component is really destroy when the last handle is destroyed
 * Internally client action is memorized in order to correctly manage singleton in multi-process management.
 *
 * \param[in] templateName The template type name.
 * \param[in] domainId Domain identifier where the component must be instantiated.
 * \param[in] priority Priority of the component into the MPC Executive Engine (all the provided interface methods will be executed at this given priority). A
 *            sub priority can be given by bitwise or with priority level.
 * \param[in] localName A optional name inside the MPC.
 * \param[out] component The component instance handle reference.
 *
 * \exception CM_COMPONENT_NOT_FOUND    The ELF file corresponding to the component template name is not found.
 * \exception CM_INVALID_ELF_FILE       The ELF file is not a MPC ELF file or not conform to the file name.
 * \exception CM_MPC_NOT_RESPONDING     Fifo not responding to sended command
 * \exception CM_INVALID_DOMAIN_HANDLE  Invalid domain handle
 * \exception CM_NO_MORE_MEMORY         No more memory
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_InstantiateComponent(
	const char* templateName,
 	t_cm_domain_id domainId,
        t_nmf_ee_priority priority,
 	const char* localName,
 	t_cm_instance_handle *component);

/*!
 * \brief Start execution of a component.
 *
 * Start a component which has been fully configured and which has all required
 * interfaces binded.
 *
 * See \ref CM_InstantiateComponent for example.
 *
 * \exception CM_COMPONENT_NOT_STOPPED      Component already started.
 * \exception CM_REQUIRE_INTERFACE_UNBINDED Each required interfaces must be binded before starting a component.
 * \exception CM_MPC_NOT_RESPONDING         Fifo not responding to sent command and the component was not started
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_StartComponent(
    const t_cm_instance_handle component                        //!< [in] The component instance handle
    );

/*!
 * \brief Stop execution of a component.
 *
 * Stop a component previously started using \ref CM_StartComponent.
 *
 * See \ref CM_InstantiateComponent for example.
 *
 * \exception CM_COMPONENT_WAIT_RUNNABLE    Start acknowledgment must be receive before stopping it.
 * \exception CM_COMPONENT_NOT_STARTED      Component must has been started before stopping.
 * \exception CM_MPC_NOT_RESPONDING         Fifo not responding to sent command (nevertheless, the component has been stopped)
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_StopComponent(
    const t_cm_instance_handle component                        //!< [in] The component instance handle
    );


/*!
 * \brief Destroy a component.
 *
 * Destroy a previously instanciated component with \ref CM_InstantiateComponent
 * by freeing memories allocated for component instance. This component must been
 * previously stopped using \ref CM_StopComponent.
 *
 * Furthermore, if it is the last instance of a component
 * template, this method unload it and free all code and shared data memories.
 *
 * See \ref CM_InstantiateComponent for example.
 *
 * \exception CM_COMPONENT_NOT_STOPPED      Component must be stopped before perform destroy operation.
 * \exception CM_COMPONENT_NOT_UNBINDED     Component must be fully unbinded before perform destroy operation.
 * \exception CM_MPC_NOT_RESPONDING         Fifo not responding to sent command (nevertheless, the component has been stopped)
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_DestroyComponent(
 	t_cm_instance_handle component                                //!< [in] The component instance handle
 	);

/*!
 * \brief Bind two components together.
 *
 * Bind a required interface of a client component with a provided interface
 * of a server component.
 *
 * This method can be used like this: \code
 * t_cm_instance_handle client, server;
 * t_cm_error error;
 *
 * error = CM_BindComponent(
 *     client,
 *     "output",
 *     server,
 *     "input");
 *
 * error = CM_UnbindComponent(
 *     client,
 *     "output");
 * \endcode
 *
 * In fact, this method do exactly the same thing as the definition bellow in the
 * architecture file in the static Nomadik Kernel approach:\code
 * binds clientName.output to serverName.input
 * \endcode
 *
 * \param[in] client The client component instance handle.
 * \param[in] requiredItfClientName The client required interface name (either collection or not collection name allowed).
 * \param[in] server The server component instance handle.
 * \param[in] providedItfServerName The server provided interface name (either collection or not collection name allowed).
 *
 * \exception CM_NO_SUCH_REQUIRED_INTERFACE     Interface name not required by the component.
 * \exception CM_NO_SUCH_PROVIDED_INTERFACE     Interface name not provided by the component.
 * \exception CM_ILLEGAL_BINDING                Type mismatch between client and server interfaces.
 * \exception CM_INTERFACE_ALREADY_BINDED       Required interface must be unbinded before rebind.
 * \exception CM_COMPONENT_NOT_STOPPED          Client component must be stopped before reconfigure it
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_BindComponent(
	const t_cm_instance_handle client,
	const char* requiredItfClientName,
	const t_cm_instance_handle server,
	const char* providedItfServerName);

/*!
 * \brief Unbind a component either traced or not.
 *
 * Unbind a required interface of a client component previoulsy binded with
 * \ref CM_BindComponent or CM_BindComponentTrace.
 *
 * See \ref CM_BindComponent for example.
 *
 * \param[in] client The client component instance handle.
 * \param[in] requiredItfClientName The client required interface name (either collection or not collection name allowed).
 *
 * \exception CM_NO_SUCH_REQUIRED_INTERFACE     Interface name not required by the component.
 * \exception CM_INTERFACE_NOT_BINDED           The required interface is not binded.
 * \exception CM_COMPONENT_NOT_STOPPED          Client component must be stopped before reconfigure it
 * \exception CM_ILLEGAL_UNBINDING              Try to unbind component bind with other binding factories.
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_UnbindComponent(
	const t_cm_instance_handle client,
	const char* requiredItfClientName);

/*!
 * \brief Bind two components together with trace.
 *
 * Bind a required interface of a client component with a provided interface
 * of a server component.
 *
 * This method can be used like this: \code
 * t_cm_instance_handle client, server;
 * t_cm_error error;
 *
 * error = CM_BindComponentTrace(
 *     client,
 *     "output",
 *     server,
 *     "input");
 *
 * error = CM_UnbindComponent(
 *     client,
 *     "output");
 * \endcode
 *
 * In fact, this method do exactly the same thing as the definition bellow in the
 * architecture file in the static ADL approach:\code
 * binds clientName.output to serverName.input
 * \endcode
 *
 * \param[in] client The client component instance handle.
 * \param[in] requiredItfClientName The client required interface name (either collection or not collection name allowed).
 * \param[in] server The server component instance handle.
 * \param[in] providedItfServerName The server provided interface name (either collection or not collection name allowed).
 *
 * \exception CM_NO_SUCH_REQUIRED_INTERFACE     Interface name not required by the component.
 * \exception CM_NO_SUCH_PROVIDED_INTERFACE     Interface name not provided by the component.
 * \exception CM_ILLEGAL_BINDING                Type mismatch between client and server interfaces.
 * \exception CM_INTERFACE_ALREADY_BINDED       Required interface must be unbinded before rebind.
 * \exception CM_COMPONENT_NOT_STOPPED          Client component must be stopped before reconfigure it
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_BindComponentTrace(
    const t_cm_instance_handle client,
    const char* requiredItfClientName,
    const t_cm_instance_handle server,
    const char* providedItfServerName);

/*!
 * \brief Bind a component to void (silently ignore a call).
 *
 * Bind to void mean that calling a method from this interface do nothing and
 * the caller can continu its execution.
 *
 * This method can be used like this: \code
 * t_cm_instance_handle client;
 * t_cm_error error;
 *
 * error = CM_BindComponentToVoid(
 *     client,
 *     "output");
 *
 * error = CM_UnbindComponent(
 *     client,
 *     "output");
 * \endcode
 *
 * \param[in] client The client component instance handle.
 * \param[in] requiredItfClientName The client required interface name (either collection or not collection name allowed).
 *
 * \exception CM_NO_SUCH_REQUIRED_INTERFACE     Interface name not required by the component.
 * \exception CM_INTERFACE_ALREADY_BINDED       Required interface must be unbinded before rebind.
 * \exception CM_COMPONENT_NOT_STOPPED          Client component must be stopped before reconfigure it
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_BindComponentToVoid(
    const t_cm_instance_handle client,
    const char* requiredItfClientName);

/*!
 * \brief Bind two components together in an asynchronous way
 * (the components can be on the same MPC or on two different MPC)
 *
 * Bind a required interface of a client component with a provided interface
 * of a server component via a asynchronous binding. If client and server components reside on
 * the same MPC then a pure asynchronous binding is created. If they are residing on distinct MPC
 * then a distributed binding is created. This distributed binding automatically marshall parameters
 * directly in the shared memory (DMA is not involve in this binding).
 *
 * This method can be used like this: \code
 * t_cm_instance_handle client, server;
 * t_cm_error error;
 *
 * error = CM_BindComponentAsynchronous(
 *     client,
 *     "output",
 *     server,
 *     "input",
 *     4);
 *
 * error = CM_UnbindComponentAsynchronous(
 *     client,
 *     "output");
 * \endcode
 *
 * In fact, this method do exactly the same thing as the definition bellow in the
 * architecture file in the static Nomadik Kernel approach:\code
 * binds clientName.output to serverName.input
 * \endcode
 *
 * \param[in] client The client component instance handle.
 * \param[in] requiredItfClientName The client required interface name (either collection or not collection name allowed).
 * \param[in] server The server component instance handle.
 * \param[in] providedItfServerName The server provided interface name (either collection or not collection name allowed).
 * \param[in] fifosize the number of element in the event fifo (number of simultaneously waiting request)
 *
 * \exception CM_NO_SUCH_REQUIRED_INTERFACE Interface name not required by the component.
 * \exception CM_NO_SUCH_PROVIDED_INTERFACE Interface name not provided by the component.
 * \exception CM_ILLEGAL_BINDING Type mismatch between client and server interfaces.
 * \exception CM_INTERFACE_ALREADY_BINDED       Required interface must be unbinded before rebind.
 * \exception CM_BINDING_COMPONENT_NOT_FOUND Binding component require for this binding not found in the
 *    component repository.
 * \exception CM_COMPONENT_NOT_STOPPED          Client component must be stopped before reconfigure it
 * \exception CM_INVALID_PARAMETER              Invalid parameter (Fifo too bug, ...)
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_BindComponentAsynchronous(
	const t_cm_instance_handle client,
	const char* requiredItfClientName,
	const t_cm_instance_handle server,
	const char* providedItfServerName,
	t_uint32 fifosize);

/*!
 * \brief Bind two components together in an asynchronous way and put event fifo in specified memory.
 * (the components can be on the same MPC or on two different MPC)
 *
 * \param[in] client The client component instance handle.
 * \param[in] requiredItfClientName The client required interface name (either collection or not collection name allowed).
 * \param[in] server The server component instance handle.
 * \param[in] providedItfServerName The server provided interface name (either collection or not collection name allowed).
 * \param[in] fifosize the number of element in the event fifo (number of simultaneously waiting request)
 * \param[in] eventMemType Memory type where allocating event fifo used for scheduling
 *
*/
PUBLIC IMPORT_SHARED t_cm_error CM_BindComponentAsynchronousEx(
        const t_cm_instance_handle client,
        const char* requiredItfClientName,
        const t_cm_instance_handle server,
        const char* providedItfServerName,
        t_uint32 fifosize,
        t_cm_mpc_memory_type eventMemType
        );

/*!
 * \brief Unbind a component previously binded asynchronously
 *
 * Unbind a required interface of a client component previoulsy binded with
 * \ref CM_BindComponentAsynchronous.
 *
 * See \ref CM_BindComponentAsynchronous for example.
 *
 * \param[in] client The client component instance handle.
 * \param[in] requiredItfClientName The client required interface name (either collection or not collection name allowed).
 *
 * \exception CM_NO_SUCH_REQUIRED_INTERFACE Interface name not required by the component.
 * \exception CM_INTERFACE_NOT_BINDED The required interface is not binded.
 * \exception CM_COMPONENT_NOT_STOPPED          Client component must be stopped before reconfigure it
 * \exception CM_ILLEGAL_UNBINDING              Try to unbind component bind with other binding factories.
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_UnbindComponentAsynchronous(
	const t_cm_instance_handle client,
	const char* requiredItfClientName);


/*!
 * \brief Read a value on an attribute exported by a component instance.
 *
 * This attribute must have been declared as an attribute in the component configuration
 * file.
 *
 * \warning {
 *      Be careful, only attribute filling in one MPC word could be get through this method.
 *
 *      Thus on MMDSP only attribute of t_[u|s]int[24|16|8] could be well returned.
 *      Elsewhere, only one part of attribute could be returned.
 * }
 *
 * This method can be used like this: \code
 * t_cm_instance_handle component;
 * t_cm_error error;
 * t_uint24 value;
 *
 * error = CM_ReadComponentAttribute(
 *       component,
 *       "count",
 *       &value);
 * \endcode
 *
 * \param[in] component The component instance handle.
 * \param[in] attrName The component attribute name.
 * \param[out] value Read value
 *
 * \exception CM_NO_SUCH_ATTRIBUTE Attribute name not provide by a component.
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_ReadComponentAttribute(
	const t_cm_instance_handle component,
	const char* attrName,
	t_uint24 *value);

/*!
 * \brief Write a value on an attribute exported by a component instance.
 *
 * This attribute must have been declared as an attribute in the component configuration
 * file.
 *
 * \warning {
 *      Be careful, only attribute filling in one MPC word could be get through this method.
 *
 *      Thus on MMDSP only attribute of t_[u|s]int[24|16|8] could be well written.
 *      Elsewhere, only one part of attribute could be written.
 *
 *      Please do not use CM_WriteComponentAttribute API by default, unless you are sure what you are doing.
 *      This API is not protected against concurrent access.
 * }
 *
 * This method can be used like this: \code
 * t_cm_instance_handle component;
 * t_cm_error error;
 * t_uint24 value;
 *
 * error = CM_WriteComponentAttribute(
 *       component,
 *       "attrName",
 *       value);
 * \endcode
 *
 * \param[in] component The component instance handle.
 * \param[in] attrName The component attribute name.
 * \param[in] value Write value
 *
 * \exception CM_NO_SUCH_ATTRIBUTE Attribute name not provide by a component.
 * \exception CM_INVALID_COMPONENT_HANDLE
 *
 * \ingroup COMPONENT
 */
PUBLIC IMPORT_SHARED t_cm_error CM_WriteComponentAttribute(
	const t_cm_instance_handle component,
	const char* attrName,
	t_uint24 value);

#endif
