#ifndef _extension_h_
#define _extension_h_

#define STD_PARSE
extern int
aac_getdata(void *hBs, void *savedBs,
			int *tag, int *dt_cnt, UCHAR * data_bytes, unsigned short aac_crc_checksum);

extern int
aac_extension_payload(void *hBs, int cnt, 
					  SBRBITSTREAM *streamSBR, 
					  int prev_element
#ifdef STD_PARSE
					  ,unsigned short *EnableSBR
					  , int frame_no
#endif
#ifdef SBR_MC
						,int LeftChannelIndex
						,int RightChannelIndex
#endif
					  );


extern void aac_getfill(void *hBs, 
						SBRBITSTREAM *streamSBR, 
						int prev_element
#ifdef STD_PARSE
						,unsigned short *EnableSBR
						,int frame_no
#endif
#ifdef SBR_MC
						,int LeftChannelIndex
						,int RightChannelIndex
#endif
						);


#undef STD_PARSE


#endif /* Do not edit below this point */
