/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   protocol.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <cscall/nmf/host/protocolhandler.nmf>
#include "cscall/nmf/host/protocolhandler/inc/IsiProtocol.hpp"
#include "cscall/nmf/host/protocolhandler/inc/MaiProtocol.hpp"
#include "cscall/nmf/host/protocolhandler/inc/IsiTDProtocol.hpp"

Protocol * Protocol::autoDetectProtocol(OMX_BUFFERHEADERTYPE *msg)
{
  // TODO : real autodetection !!
#if defined(MAI_PROTOCOL)
  return new MaiProtocol;
#else
  return new IsiProtocol;
#endif
}


Protocol * Protocol::createProtocol(CsCallProtocol_t protocol)
{
  switch (protocol)
  {
    case PROTOCOL_ISI    : return new IsiProtocol;
    case PROTOCOL_MAI    : return new MaiProtocol;
    case PROTOCOL_ISI_TD : return new IsiTDProtocol;
    default              : return 0;
  }
}


