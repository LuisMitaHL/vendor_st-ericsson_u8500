
#ifndef _decode_book_h_
#define _decode_book_h_

#ifndef HUFF_CONCAT

extern int const HUF_MEM             aac_decode_book1[80][4];
extern int const HUF_MEM             aac_decode_book2[80][4];
extern int const HUF_MEM             aac_decode_book3[80][4];
extern int const HUF_MEM             aac_decode_book4[80][4];
extern int const HUF_MEM             aac_decode_book5[80][4];
extern int const HUF_MEM             aac_decode_book6[80][4];
extern int const HUF_MEM             aac_decode_book7[63][4];
extern int const HUF_MEM             aac_decode_book8[63][4];
extern int const HUF_MEM             aac_decode_book9[168][4];
extern int const HUF_MEM             aac_decode_book10[168][4];
extern int const HUF_MEM             aac_decode_book11[288][4];
extern int const HUF_MEM             aac_decode_bookscl[120][4];

#else		/* HUFF_CONCAT */

extern int const HUF_MEM aac_decode_book1[80];
extern int const HUF_MEM aac_decode_book2[80];
extern int const HUF_MEM aac_decode_book3[80];
extern int const HUF_MEM aac_decode_book4[80];
extern int const HUF_MEM aac_decode_book5[80];
extern int const HUF_MEM aac_decode_book6[80];
extern int const HUF_MEM aac_decode_book7[63];
extern int const HUF_MEM aac_decode_book8[63];
extern int const HUF_MEM aac_decode_book9[168];
extern int const HUF_MEM aac_decode_book10[168];
extern int const HUF_MEM aac_decode_book11[288];
#ifdef ARM
extern int const HUF_MEM aac_decode_bookscl[157];
#else
extern int const HUF_MEM aac_decode_bookscl[120];
#endif

#endif		/* HUFF_CONCAT */


extern int const HUF_MEM            * const HUF_MEM aac_decode_book[12];

#endif /* Do not edit below this line */
