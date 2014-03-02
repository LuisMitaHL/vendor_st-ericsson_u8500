/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _NEW_REGS_H_
#define _NEW_REGS_H_

#if 0 /* Not needed for ndk8500_ed */

#ifndef VLC_BASE
#define VLC_BASE 0xD400U
#endif

#ifndef VLC_REG_SOFT_RESET
#define VLC_REG_SOFT_RESET 0
#endif

#ifndef VLC_REG_DEF
#define VLC_REG_DEF 0
#endif

#ifndef VLC_REG_CFG
#define VLC_REG_CFG 0
#endif

#ifndef VLC_FIFO_DC_PRED
#define VLC_FIFO_DC_PRED 0
#endif

#ifndef VLC_FIFO_DC_CUR
#define VLC_FIFO_DC_CUR 0
#endif

#ifndef VLC_FIFO_CBP
#define VLC_FIFO_CBP 0
#endif

#ifndef VLC_FIFO_CMD
#define VLC_FIFO_CMD 0
#endif

#endif

#define GRB_ERR_IAD_L            0x0126
#define GRB_ERR_IAD_H            0x0127
#define GRB_ERR_ITY_L            0x0128
#define GRB_ERR_ITY_H            0x0129
#define GRB_ERR_ITS_L            0x012A
#define GRB_ERR_ITS_H            0x012B

#define VEC_ERR_IAD_L            0x00A6
#define VEC_ERR_IAD_H            0x00A7
#define VEC_ERR_ITY_L            0x00A8
#define VEC_ERR_ITY_H            0x00A9
#define VEC_ERR_ITS_L            0x00AA
#define VEC_ERR_ITS_H            0x00AB

#define VDC_ERR_IAD_L            0x00E6
#define VDC_ERR_IAD_H            0x00E7
#define VDC_ERR_ITY_L            0x00E8
#define VDC_ERR_ITY_H            0x00E9
#define VDC_ERR_ITS_L            0x00EA
#define VDC_ERR_ITS_H            0x00EB

#define DPL_ERR_IAD_L            0x0166
#define DPL_ERR_IAD_H            0x0167
#define DPL_ERR_ITY_L            0x0168
#define DPL_ERR_ITY_H            0x0169
#define DPL_ERR_ITS_L            0x016A
#define DPL_ERR_ITS_H            0x016B

#define TVD_ERR_IAD_L            0x01A6
#define TVD_ERR_IAD_H            0x01A7
#define TVD_ERR_ITY_L            0x01A8
#define TVD_ERR_ITY_H            0x01A9
#define TVD_ERR_ITS_L            0x01AA
#define TVD_ERR_ITS_H            0x01AB

#endif /* _NEW_REGS_H_ */


