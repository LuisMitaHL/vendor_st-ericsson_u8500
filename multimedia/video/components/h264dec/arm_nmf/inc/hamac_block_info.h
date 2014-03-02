/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HAMAC_BLOCK_INFO_H
#define HAMAC_BLOCK_INFO_H

#include "types.h"
#include "hamac_types.h"

#define LUMA_BLOCK 	    1
#define CHROMA_U_BLOCK 	2
#define CHROMA_V_BLOCK 	3

#define BLOCK_NA	    0
#define BLOCK_NB        1
#define BLOCK_NC	    2
#define BLOCK_ND        3

#define I4x4            0
#define I16x16          1
#define INTER           2

#define NOT_CONCEALED   0
#define INTRA_CONCEALED 1
#define INTER_CONCEALED 2
#define COPY_CONCEALED  3


t_uint16 setMacroblockNSlice(t_sint16 slice_num, t_hamac_mb_info *p_hamac);
void setMacroblockQP(t_uint16 QPy, t_uint16 QPc, t_hamac_mb_info *p_hamac);
void setBlockNCoeff(t_uint16 ncoeff, t_uint16 type, t_hamac_residual_info *p_hamac);
void setMacroblockNCoeff(t_uint16 ncoeff, t_hamac_mb_info *p_hamac);	
void setBlockType(t_uint16 type, t_uint16 curr_block, t_hamac_mb_info *p_hamac);
void setBlockPrediction(t_uint16 type, t_uint16 curr_block, t_hamac_mb_info *p_hamac);
void setBlockMotion(t_uint16 curr_block, t_sint16 refidx, t_sint16 *p_mv, t_hamac_mb_info *p_hamac);
void setMacroblockConceal(t_uint16 type, t_uint16 mbaddr, t_hamac_conc_info *p_hamac, t_sint16 *p_mv);

t_sint16 getBlockNCoeff(t_uint16 type, t_uint16 nN, t_hamac_residual_info *p_hamac);
t_sint16 getBlockType(t_uint16 nN, t_uint16 curr_block, t_hamac_mb_info *p_hamac);
void getNeighbourPrediction(t_hamac_mb_info *p_hamac, t_uint16 curr_block, t_uint16 *p_modeA, t_uint16 *p_modeB);
t_uint16 getNeighbourBlockInfo(t_uint16 nN, t_uint16 curr_block, t_hamac_mb_info *p_hamac, t_block_info **p_block);
void getBlocksForFilter(t_hamac_deblocking_info *p_hamac, t_uint16 mbx, t_uint16 mby, t_uint16 blkx, t_uint16 blky, t_uint16 dir, t_block_info **p_blocks);
void getSurroundingMacroblocks(t_uint16 mbaddr, t_hamac_conc_info *p_hamac, t_sint16 p_decoded[4], t_uint16 p_concealed[4]);
t_uint16 IsAvailable(t_uint16 nN, t_uint16 mbx, t_uint16 mby, t_uint16 pic_width_in_mbs_minus1, t_block_info *p_b_info);
t_uint16 IsLost(t_uint16 mbaddr, t_hamac_conc_info *p_hamac);
		
#endif
