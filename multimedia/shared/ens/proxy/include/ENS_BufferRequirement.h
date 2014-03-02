/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_BufferReq.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _ENS_BUFFER_REQ_H_
#define _ENS_BUFFER_REQ_H_

#include "ENS_Redefine_Class.h"

#include "OMX_Types.h"
#include "OMX_Component.h"


namespace ens {

// Forward declaration
class  BufferReq;


/** Global method to get max of two buffer requirements */
BufferReq
max(const BufferReq& a, const BufferReq& b);

/** Global method to update a PORTDEFINITIONTYPE structure with new
    buffer requirement */
void
updateBufferReq(OMX_PARAM_PORTDEFINITIONTYPE& portDef,
                const BufferReq& req);


/**
 * Read-only port requirements instrinsec to each OpenMAX port.
 * These requirements are negotiated during port tunneling
 */
class BufferReq
{
public:
    /** @name Constructors and Destructor */
    //@{
    /** Default constructor */
    BufferReq();

    BufferReq(OMX_U32 aBufferCountActual,
              OMX_U32 aBufferCountMin,
              OMX_U32 aBufferSizeMin,
              OMX_BOOL aContiguous,
              OMX_U32 aBufferAlignment);
    
    /** Constructor from OpenMAX PORTDEFINITIONTYPE structure */
    BufferReq(const OMX_PARAM_PORTDEFINITIONTYPE& portDef);

    /** Copy constructor */
    BufferReq(const BufferReq& req);

    /** Destructor */
    ~BufferReq();
    
    
public:
    /** Get  number of buffers that are required on this port before
        it is populated */
    OMX_U32  
    bufferCountActual() const;

    /** Get the minimum number of buffers this port requires */
    OMX_U32  
    bufferCountMin() const;
    
    /** Get the minimum size, in bytes, for buffers to be
        used for this port */
    OMX_U32
    bufferSizeMin() const;
    
    /** Check if this port requires each buffer to be in
        contiguous memory */
    OMX_BOOL
    isBuffersContiguous() const;
    
    /** Minimum size, in bytes, for buffers to be used for this port */
    OMX_U32
    bufferAlignment() const;
    
    BufferReq&
    operator=(const BufferReq& req);
        
private:
    /** The number of buffers that are required on this port before
        it is populated */
    OMX_U32   m_nBufferCountActual;

    /** The minimum number of buffers this port requires */
    OMX_U32   m_nBufferCountMin;
    
    /** Minimum size, in bytes, for buffers to be used for this port */
    OMX_U32   m_nBufferSizeMin;
    
    /** Boolean field that indicates this port requires each buffer
        to be in contiguous memory */
    OMX_BOOL  m_bBuffersContiguous;
    
    /** Specifies in bytes, the alignment the port requires for each
        of its buffer */
    OMX_U32   m_nBufferAlignment;
};

inline OMX_U32  
BufferReq::bufferCountActual() const
{ return m_nBufferCountActual; }

inline OMX_U32  
BufferReq::bufferCountMin() const
{ return m_nBufferCountMin; }

inline OMX_U32
BufferReq::bufferSizeMin() const
{ return m_nBufferSizeMin; }

inline OMX_BOOL
BufferReq::isBuffersContiguous() const
{ return m_bBuffersContiguous; }

inline OMX_U32
BufferReq::bufferAlignment() const
{ return m_nBufferAlignment; }



#endif // _ENS_BUFFER_REQ_H_
