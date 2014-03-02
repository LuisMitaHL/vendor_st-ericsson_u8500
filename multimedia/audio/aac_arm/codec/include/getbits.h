#ifndef _getbits_h_
#define _getbits_h_

extern void set_crc_word(int ignore);
extern void set_crc_protected_bits(int n);
extern void crc_pad(void);
extern int  crc_check(unsigned short check);         


#define USE_LIBBITSTREAM

#ifdef USE_LIBBITSTREAM

#include "bitstream_lib_proto.h"

#ifdef ARM
#define aac_readbit(a,b)        bs_read_bit_rev_32b((BS_STRUCT_T *) (a),(b))
#define aac_readbit_long(a,b)   bs_read_bit_rev_32b((BS_STRUCT_T *) (a),(b))
#else
#define aac_readbit(a,b)        bs_read_bit((BS_STRUCT_T *) (a),(b))
#define aac_readbit_long(a,b)   bs_read_bit((BS_STRUCT_T *) (a),(b))
#endif


#define aac_writebit(a,b,c)     bs_write_bit((BS_STRUCT_T *) (a),(b),(c))

#define aac_fast_move_bit(a,b)  bs_fast_move_bit((BS_STRUCT_T *) (a),(b))
#ifdef ARM
#define aac_lookup_32bits(a)    bs_lookup_word_size_bit((BS_STRUCT_T *)(a))
#else /* ARM */
#define aac_lookup_24bits(a)    bs_lookup_word_size_bit((BS_STRUCT_T *)(a))
#endif /* ARM */
#define aac_get_bit_count(a,b)  bs_delta_bit((BS_STRUCT_T *)(a),(BS_STRUCT_T *)(b))
#define aac_restore_getbit(a,b) (a) = (b)
#define aac_byte_align(a,b)     bs_byte_align((BS_STRUCT_T *)(a),(BS_STRUCT_T *)(b))

#else

#define aac_readbit(a,b)        drb_getbits((b))
#define aac_readbit_long(a,b)   drb_getbits_long((b))
#define aac_fast_move_bit(a,b)  drb_fast_forward((b))
#define aac_lookup_24bits(a)    drb_lookup_24bits()
#define aac_get_bit_count(a,b)  drb_get_bit_count()
#define aac_restore_getbit(a,b) drb_restore_getbit()
#define aac_byte_align(a,b)     drb_byte_align()

#endif

//extern void send_ancillary_data(char *p,int size);


#endif /* Do not edit below this point */
