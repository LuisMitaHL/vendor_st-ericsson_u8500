/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   BindContext.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef  _BINDCONTEXT_H_
#define  _BINDCONTEXT_H_
#include "ENS_Nmf.h"

//-----------------------------------------------------------------------------
//! \class BindContext
//! \brief Create Ens Context for bindToHost
//!
//! This is a singleton shared by all sources/sinks to bind Nmf component from
//! Mpc to Host. We need to create such context because life cycle of such 
//! interface is not equal to any OMX component life cycle 
//!
//-----------------------------------------------------------------------------
class BindContext {
    public:
        static BindContext * getHandle(void);    //!< singleton: creates unique instance on first call 
        void                 freeHandle(void);   //!< singleton: destroy unique instance on last call

        //! Returns ENS local context that can be used when binding Nmf Mpc component to Host
        inline OMX_PTR getBindContext(void) { return mEnsContext; }

    private:    
        OMX_PTR mEnsContext;        //!< ENS local context
      
        static BindContext * mBindContext; //!< singleton: pointer on unique instance
        static int           mRefCount;    //!< singleton: reference counter

        BindContext();
        ~BindContext();
};

#endif   // _BINDCONTEXT_H_
