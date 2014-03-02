/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/**
 * \internal
 */
#ifndef COMMON_INTERFACE_TYPE_H
#define COMMON_INTERFACE_TYPE_H

#include <cm/engine/communication/inc/communication_type.h>

/*!
 * \brief Generic Interface method type.
 *
 * \exception CM_PARAM_FIFO_OVERFLOW
 *
 * \ingroup COMPONENT_HOST
 */
typedef t_cm_error (*t_nmf_itf_method)(void* THIS);


/*!
 * \brief Identifier of the interface descriptor in C
 *
 * This interface descriptor can be used to call a component in C.
 * This generic type must be casted into a specific generated C interface descriptor.
 * Contrary to the Media Processor Core toolset which handle multi-instances automatically,
 * each method in this structure must have a THIS as the first parameter and look like this: \code
 * xxx meth(void* THIS, ...);
 * \endcode
 * This THIS reference must be filled by the data field of this structure
 * before when calling a mathod. Thus, a call look like this:\code
 * t_cm_error error;
 * error = I.meth(i.THIS, ...);
 * \endcode
 * or by using NMFCALL macro (see CM Macros chapter):\code
 * t_cm_error error;
 * error = NMFCALL(I, meth)(...);
 * \endcode
 *
 * This THIS parameter as no type and must be cast manually into the real
 * structure of the Host instance.
 *
 * On a Host to MPC communication, this reference the STUB that must be
 * called to call a component on the MPC.
 *
 * On a MPC to Host communication, this reference the client
 * component that must be called by the MPC component.\n
 * The NMFMETH macro is provided to ease the client method writting.\n
 * By example, the user can write:\code
 * void mynotify(void *THIS, mytype1 myparam1, mytype2 myparam2, ...) {
 * <body of the interface routine>
 * }
 * \endcode
 * or by using the NMFMETH macro (see CM Macros chapter):\code
 * void NMFMETH(mynotify)(mytype1 myparam1, mytype2 myparam2, ...) {
 * <body of the interface routine>
 * }
 * \endcode
 *
 * \ingroup COMPONENT_HOST
 */
typedef struct {
    void                *THIS;      //!< Reference on the component instance address
    t_nmf_itf_method    methods[1];    //!< Reference the methods address
} t_cm_interface_desc;

/*!
 * \brief Identifier of the abstract interface descriptor
 *
 * This interface descriptor can be abstract a interface on a component in C and C++.
 *
 * \ingroup COMPONENT_HOST
 */
typedef void t_nmf_interface_desc;

#endif /* CM_INTERFACE_TYPE_H */
