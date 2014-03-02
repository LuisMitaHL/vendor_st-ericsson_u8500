
 

#ifndef _mpeg_crc_h_
#define _mpeg_crc_h_

#include "audiolibs_common.h"
#include "vector.h"

#ifdef __flexcc2__ 
#define MPEG_CRC_MEM  EXTERN
#else
#define MPEG_CRC_MEM  
#endif

#ifndef _NMF_MPC_
unsigned short
mpeg_calc_crc_bitwise(unsigned short crc_reg,unsigned short val);

unsigned short
mpeg_calc_crc(unsigned short crc_reg,unsigned short val,int length);

#ifdef ARM
unsigned short
mpeg_calc_crc_arm(unsigned short crc_reg,unsigned short val,int length);

#endif

unsigned short
mpeg_calc_crc_bytewise(unsigned short crc_reg,unsigned short val);

extern unsigned short
mpeg_calc_crc_bytewise_table(unsigned short crc_reg,unsigned short val);

extern unsigned short
mpeg_dab_scf_crc(unsigned short data, unsigned short scrc);

#endif // _NMF_MPC_

#endif // _mpeg_crc_h_ 
