
 


#include "mpeg_crc.h"

extern unsigned short const MPEG_CRC_MEM mpeg_crc_table[256];

#ifdef MPEG_CRC_WAITMEM_TABLES

extern unsigned short YMEM  *mpeg_crc_p_table;

#else

#define mpeg_crc_p_table mpeg_crc_table

#endif

