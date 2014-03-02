
#ifndef _crc_h_
#define _crc_h_

extern void aac_set_crc_word(unsigned short val,unsigned short *aac_crc_checksum);

extern int  aac_getbits_crc(void *hBs, int n, unsigned short *aac_crc_checksum);

extern void aac_loop_getbits_crc(void *hBs, int n, unsigned short *aac_crc_checksum);

extern void aac_crc_pad(int n, unsigned short *aac_crc_checksum);

extern void aac_ics_crc(void *hBs, void *saveBs, int *ele_length, int nch, unsigned short *aac_crc_checksum);

extern int  aac_check_crc(unsigned short val, unsigned short aac_crc_checksum);

#endif /* Do not edit below this line */
