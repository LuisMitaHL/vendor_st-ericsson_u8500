/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file PipeAlgorithm.h
 \brief
 \ingroup SystemConfig
*/
#ifndef _PIPE_ALGORITHRM_H_
#   define _PIPE_ALGORITHRM_H_

#   include "SystemConfig.h"
#   include "FrameDimension_op_interface.h"
#   include "channel_gains_op_interface.h"
#   include "Adsoc.h"
#   include "Babylon.h"
#   include "Mozart.h"
#   include "Norcos.h"
#   include "Scorpio.h"
#   include "BinningRepair.h"
#   include "Gridiron_op_interface.h"
#   include "Duster_op_interface.h"
#   include "SDL_op_interface.h"

extern uint8_t PipeAlgortihm_FrameUpdate ( void ) TO_EXT_DDR_PRGM_MEM;
extern void PipeAlgorithm_UpdatePipe0 (void);
extern void PipeAlgorithm_UpdatePipe1 (void);
extern void PipeAlgorithm_UpdateRE (void);
extern void PipeAlgorithm_UpdateDMCE (void);

extern void PipeAlgorithm_CommitInputPipe (void);
extern void PipeAlgorithm_CommitPipe0 (void);
extern void PipeAlgorithm_CommitPipe1 (void);

extern void CommitDMCE (void);
extern void CommitRE (void);
extern void CommitSDPipe (void);
extern void PipeAlgorithm_UpdateSDPipe ( void ) TO_EXT_DDR_PRGM_MEM;
void    PipeAlgorithm_CommitGlaceAndHistogram (void);
#endif // _PIPE_ALGORITHRM_H_

