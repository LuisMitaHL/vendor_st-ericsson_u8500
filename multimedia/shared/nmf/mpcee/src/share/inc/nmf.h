/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*!
 * \brief Common Nomadik Multiprocessing Framework type definition
 *
 * This file contains the shared type definitions used into NMF.
 */

#ifndef __INC_NMF_H
#define __INC_NMF_H

#include <inc/typedef.h>

/*!
 * \brief Identification of the various cores (host cpu and Media Processors) into Nomadik Platform
 * In order to improve performance, these ids are those used to interconnect HW Semaphores IP with Cores (Interrupt lines)
 * \ingroup NMF_COMMON
 */
typedef t_uint8 t_nmf_core_id;
#define ARM_CORE_ID                 ((t_nmf_core_id)0)                  //!< HOST CPU Id
#define SVA_CORE_ID                 ((t_nmf_core_id)1)                  //!< Smart Video Accelerator Media Processor Code Id
#define SIA_CORE_ID                 ((t_nmf_core_id)2)                  //!< Smart Imaging Accelerator Media Processor Code Id
#define NB_CORE_IDS                 ((t_nmf_core_id)3)

#define FIRST_CORE_ID               ((t_nmf_core_id)ARM_CORE_ID)
#define FIRST_MPC_ID                ((t_nmf_core_id)SVA_CORE_ID)
#define LAST_CORE_ID                ((t_nmf_core_id)SIA_CORE_ID)
#define LAST_MPC_ID                 ((t_nmf_core_id)SIA_CORE_ID)


/*!
 * \brief Define minimal stack size use by execution engine
 */
#define MIN_STACK_SIZE              128



#endif /* __INC_NMF_H */
