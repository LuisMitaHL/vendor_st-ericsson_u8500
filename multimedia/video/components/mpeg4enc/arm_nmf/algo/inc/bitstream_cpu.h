/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef BITSTREAM_CPU_H
#define BITSTREAM_CPU_H

#define SEQ_HDR 0x00000001
#define PIC_HDR 0x00000002
#define STF_BTS 0x00000003



/* defines the image format up to more than VGA resolution
   (the supported formats).
   FP: added the 16CIF and the two reserved formats as from table 6-25
*/
#define SQCIF_FORMAT              0x01
#define  QCIF_FORMAT              0x02
#define   CIF_FORMAT              0x03
#define _4CIF_FORMAT              0x04
#define _16CIF_FORMAT              0x05
#define   RES1_FORMAT              0x06
#define   RES2_FORMAT              0x07


void mp4e_WriteToNextStartCode(int shdr_flag);
void mp4e_WriteVOPData(mp4_parameters *mp4_par, int quant, int fcode, int frame_num, int inter, struct time_stamp_t *ts);
void mp4e_WriteGOBData(int gob_number, int quant_scale, int change_frame_id, int* gob_frame_id, mp4_parameters *mp4_par);

#endif
