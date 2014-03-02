/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   audiocodec.h
 * \brief  Some definition shared by Host and Mpc code
 * \author ST-Ericsson
 */
/*****************************************************************************/

#ifndef  _AUDIOCODEC_H_
#define  _AUDIOCODEC_H_

//! defines number of channel for a mono accessory
#define NB_CHANNEL_MONO         1
//! defines number of channel for a stereo accessory
#define NB_CHANNEL_STEREO       2
//! 5ms buffer, mono, 8Khz
#define BT_BLOCKSIZE_5MS_NB     40
//! 5ms buffer, mono, 16Khz
#define BT_BLOCKSIZE_5MS_WB     80
//! 1ms buffer, mono, 48Khz
#define AB_BLOCKSIZE_MONO_1MS   48


//! defines Nmf input port index for avsink accessory
#define AVSINK_INPUT_PORT_IDX   0
//! defines number of channel for avsink accessory
#define AVSINK_NB_CHANNEL       NB_CHANNEL_STEREO

//! defines Nmf input port index for internal hands free accessory
#define IHF_INPUT_PORT_IDX      1
//! defines number of channel for internal hands free accessory
#define IHF_NB_CHANNEL          NB_CHANNEL_STEREO

//! defines Nmf input port index for earpiece accessory
#define EAR_INPUT_PORT_IDX      2
//! defines number of channel for earpiece accessory
#define EAR_NB_CHANNEL          NB_CHANNEL_MONO

//! defines Nmf input port index for vibra1 accessory
#define VIBRAL_INPUT_PORT_IDX   3
//! defines number of channel for vibra1 accessory
#define VIBRAL_NB_CHANNEL       NB_CHANNEL_MONO

//! defines Nmf input port index for vibra2 accessory
#define VIBRAR_INPUT_PORT_IDX   4
//! defines number of channel for vibra2 accessory
#define VIBRAR_NB_CHANNEL       NB_CHANNEL_MONO

//! defines Nmf input port index for FM sink
#define FMTX_INPUT_PORT_IDX     5
//! defines number of channel for FM sink
#define FMTX_NB_CHANNEL         NB_CHANNEL_STEREO

//! defines Nmf input port index for avsource accessory
#define AVSOURCE_PORTIDX        0
//! defines number of channel for avsource accessory
#define AVSOURCE_NB_CHANNEL     NB_CHANNEL_MONO

//! defines Nmf input port index for multimic accessory
#define MULTIMIC_PORTIDX        1
//! defines number of channel for multimic accessory
#define MULTIMIC_NB_CHANNEL     NB_CHANNEL_STEREO

//! defines Nmf input port index for FM source
#define FMRX_PORTIDX            2
//! defines number of channel for FM source
#define FMRX_NB_CHANNEL         NB_CHANNEL_STEREO

#define LLI_RX_TDM_SLOTS_4 (21)
#define LLI_RX_TDM_SLOTS_2 (41)
#define LLI_RX_TDM_SLOTS_1 (61)

#define AB_8500_RX_MSP_SLOTS_MAX  (4)
#define JITTERBUFFER_RX_MS (10)

#define AB_8500_TX_MSP_SLOTS_MAX  (8)
#define JITTERBUFFER_TX_MS (10)

#endif   // _AUDIOCODEC_H_
