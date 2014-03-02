#ifndef _huffman_h_
#define _huffman_h_
#ifndef COMPRESS_CB_MAP_AND_SC_FACTORS

#define GET_CB_MAP(a)       (a)
#define SET_CB_MAP(a,b)     (a) = (b)
#define GET_SC_FACTOR(a)    (a)
#define SET_SC_FACTOR(a,b)  (a) = (b)

#else

#define GET_CB_MAP(a)       wextract((a),bit_field(16,8))
#define SET_CB_MAP(a,b)     { (a) &= 0xFFFF; (a) |= ((b)<<16);}
#define GET_SC_FACTOR(a)    wextract((a),bit_field(0,16))
#define SET_SC_FACTOR(a,b)  { (a) &= (int)0xFF0000L ; (b) &= 0xFFFF; (a) |= (b);}

#endif  /* COMPRESS_CB_MAP_AND_SC_FACTORS */
#ifndef COMPRESS_MASK

#define GET_MASK(a,offset)       (a)[offset]
#define SET_MASK(a,offset,val)   (a)[(offset)] = val

#else

/* store mask info by groups of 16 bits.
 *   24 bits could be used but we need a division
 */
#define GET_MASK(a,offset)       aac_get_boolean(&(a)[(offset)>>4],(offset)&0xf)
#define SET_MASK(a,offset,val)   aac_set_boolean(&(a)[(offset)>>4],(offset)&0xf,val)

#endif

extern int
aac_getics(void *hBs,
		   int common_window, UCHAR * win, UCHAR * wshape, UCHAR * group, UCHAR * max_sfb,
		   UCHAR * cb_map, Float * coef, SHORT *global_gain,
		   SHORT *factors,
		   
#ifdef MPEG4V1LTP
       NOK_LT_PRED_STATUS * nok_ltp_status,
#endif

       TNS_frame_info * tns

#ifdef  SSR
       ,int ch
#endif
	   , AAC_DECODER_STRUCT_T *p_global
);

extern int
aac_getmask(void *hBs,
			Info * info, UCHAR * group, UCHAR max_sfb, UCHAR * mask);
extern void
aac_getgroup(void *hBs,
			 Info * info, UCHAR * group);
extern void
aac_huffbookinit(AAC_DECODER_STRUCT_T *p_global);
extern int
aac_huffcb(void *hBs,
		   UCHAR * sect, int *sectbits, int tot_sfb, int sfb_per_sbk,
		   UCHAR max_sfb);

extern int
aac_huffdecode(void *hBs,
			   int id, 
			   MC_Info * mip,
			   Float ** coef,
			   int ignore_crc,
#ifdef MPEG4V1LTP
			   NOK_LT_PRED_STATUS **nok_ltp,
#endif
			   AAC_DECODER_STRUCT_T *p_global);

extern void
aac_get_ics_info(void *hBs
#ifdef MPEG4V1LTP
				 ,UCHAR  win
#endif
				 ,UCHAR * wshape, UCHAR * group, UCHAR * max_sfb

#ifdef MPEG4V1LTP
				 ,NOK_LT_PRED_STATUS * nok_ltp_status, NOK_LT_PRED_STATUS * nok_ltp_status_right, 
				 int stereo_flag, int *ltp_present, int config_ltp
#endif
				 ,MC_Info aac_mc_info, Info *info

);

extern int
aac_hufffac(void *hBs,
			Info * info, UCHAR * group, int nsect, UCHAR * sect,
			SHORT global_gain, SHORT *factors, AAC_DECODER_STRUCT_T *p_global);
extern int
aac_huffspec(void *hBs, Info * info, int nsect, UCHAR * sect, Float * coef, AAC_DECODER_STRUCT_T *p_global);
extern void
aac_hufftab(void);

extern int   aac_getescape(void *hBs, int q);
extern void  aac_get_sign_bits(void *hBs, int *q, int n);
extern void
aac_decode_escape_table(void *hBs, int *qp_dsp,int table, int nb_total,int const HUF_MEM *base,int const *plut);
extern void
aac_decode_signed_dim2_table(void *hBs, int *qp_dsp,int table, int nb_total,int const HUF_MEM *base,int const *plut);
extern void
aac_decode_unsigned_dim2_table(void *hBs, int *qp_dsp,int table, int nb_total,int const HUF_MEM *base,int const *plut);
extern void
aac_decode_signed_dim4_table(void *hBs, int *qp_dsp,int table, int nb_total,int const HUF_MEM *base,int const *plut);
extern void
aac_decode_unsigned_dim4_table(void *hBs, int *qp_dsp,int table, int nb_total,int const HUF_MEM *base,int const *plut);

#ifdef ARM
extern void bs_fast_move_bit_opt(BS_STRUCT_T *bs_struct, int nbit);
extern int
aac_getescape_opt(unsigned int *lookup_bits, int q, int *nbits);

extern int
aac_decode_huff_cw(void *hBs, int table,int const HUF_MEM *base);
#else
extern int
aac_decode_huff_cw(void *hBs, int table,int const HUF_MEM *base,int const *plut);
#endif
#endif /* Do not edit below this point */





