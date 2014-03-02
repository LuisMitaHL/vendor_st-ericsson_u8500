/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ZIGZAG_H
#define ZIGZAG_H

void INIT_SCAN(int mode, unsigned char matrices[3][64]);

void mp4e_CoeffScan(short *block_in, short *block_out);

#ifdef H263_P3

void mp4e_H263P3_CoeffScan(short *block_in, short *block_out);

#endif


#endif /* ZIGZAG_H */

