/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   FakeFrameGenerator.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#include "cscall/nmf/host/downlink/inc/FakeFrameGenerator.hpp"
#include "cscall/nmf/host/downlink/inc/IsiFakeFrameGenerator.hpp"

FakeFrameGenerator * FakeFrameGenerator::createFakeFrameGenerator(CsCallProtocol_t protocol) {
    switch (protocol)
    {
        case PROTOCOL_ISI    : return new IsiFakeFrameGenerator;
        case PROTOCOL_MAI    : return new FakeFrameGenerator;
        case PROTOCOL_ISI_TD : return new FakeFrameGenerator;
        default              : return new FakeFrameGenerator;
    }
}
