/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   LoopBack.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cscall/nmf/host/loopbackmodem.nmf"
#include "cscall/nmf/host/loopbackmodem/inc/IsiLoopBack.hpp"
#include "cscall/nmf/host/loopbackmodem/inc/MaiLoopBack.hpp"

LoopBack *LoopBack::getLoopBack(CsCallProtocol_t protocolID)
{
  LoopBack *ptr = 0;
  
  if(protocolID == PROTOCOL_ISI)
  {
	ptr = new IsiLoopBack;
  }
  else if (protocolID == PROTOCOL_MAI)
  {
	ptr = new MaiLoopBack;
  }

  return ptr;
}
