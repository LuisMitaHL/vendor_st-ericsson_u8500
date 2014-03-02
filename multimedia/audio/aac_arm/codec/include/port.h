
/************************* MPEG-2 NBC Audio Decoder **************************
 *                                                                           *
"This software module was originally developed by
AT&T, Dolby Laboratories, Fraunhofer Gesellschaft IIS and edited by
Yoshiaki Oikawa (Sony Corporaion),
Mitsuyuki Hatanaka (Sony Corporation),
in the course of development of the MPEG-2 NBC/MPEG-4 Audio standard ISO/IEC 13818-7,
14496-1,2 and 3. This software module is an implementation of a part of one or more
MPEG-2 NBC/MPEG-4 Audio tools as specified by the MPEG-2 NBC/MPEG-4
Audio standard. ISO/IEC  gives users of the MPEG-2 NBC/MPEG-4 Audio
standards free license to this software module or modifications thereof for use in
hardware or software products claiming conformance to the MPEG-2 NBC/MPEG-4
Audio  standards. Those intending to use this software module in hardware or
software products are advised that this use may infringe existing patents.
The original developer of this software module and his/her company, the subsequent
editors and their companies, and ISO/IEC have no liability for use of this software
module or modifications thereof in an implementation. Copyright is not released for
non MPEG-2 NBC/MPEG-4 Audio conforming products.The original developer
retains full right to use the code for his/her  own purpose, assign or donate the
code to a third party and to inhibit third party from using the code for non
MPEG-2 NBC/MPEG-4 Audio conforming products. This copyright notice must
be included in all copies or derivative works."
Copyright(c)1996.
 *                                                                           *
 ****************************************************************************/

#ifndef	_port_h_
#define	_port_h_
//#include "aac_local.h"
void
aac_adts_set_default_position(AAC_DECODER_STRUCT_T *p_global);
void            aac_init_default_position(int *first_cpe);
void            aac_force_default_position(int *first_cpe);
#ifdef ARM
int             aac_ch_index(MC_Info * mip, int id, int cpe, int tag);
#endif
int             aac_check_mc_info(MC_Info * mip, int new_config, MC_Info * aac_save_mc_info);
int             aac_chn_config(int id, int tag, int common_window,
			   MC_Info * mip, AAC_DECODER_STRUCT_T *p_global);
int             aac_enter_chn(int cpe, int tag, char position,
			      int common_window, MC_Info * mip);
int             aac_enter_mc_info(ProgConfig * pcp, AAC_DECODER_STRUCT_T *p_global);
int             aac_get_adif_header(void *hBs, AAC_DECODER_STRUCT_T *p_global);
void            aac_get_adts_header(void *hBs, AAC_DECODER_STRUCT_T *p_global);
int             aac_getAudioSpecificConfig(void *hBs, void *p_global, unsigned short size);
int             aac_get_prog_config_a(void *hBs, AAC_DECODER_STRUCT_T *p_global);
int             aac_get_prog_config(void *hBs, ProgConfig * p, AAC_DECODER_STRUCT_T *p_global);

void            aac_infoinit(SR_Info const SR_MEM * sip, AAC_DECODER_STRUCT_T *p_global);
void            aac_init(AAC_DECODER_STRUCT_T *p_global);
int             aac_get_required_ram(void);
int             aac_max_dep_cc(int nch, MC_Info aac_mc_info);
int             aac_max_indep_cc(int nch, MC_Info aac_mc_info);
int             aac_max_lfe_chn(int nch, MC_Info aac_mc_info);

void            aac_myexit(char *s);

void            aac_print_mc_info(MC_Info * mip);


void            aac_reset_mc_info(MC_Info * mip, int block_number, int aac_default_config);
void            aac_reset_pred_state(PRED_STATUS * psp);

extern void
aac_reset_pcm_buffers(Float *coef, int num);

extern void aac_parse_and_decode_huffman(void *hBs,
										 STATE_STRUCT_T *codec_state,
										 Float *coef[Chans],
										int ignore_crc,
										 AAC_DECODER_STRUCT_T *p_global,
										 int raw_synchro);


#ifdef ARM

extern void
aac_rescale_channel(Info * info, int *coef, int ch, int wn, AAC_DECODER_STRUCT_T *p_global);

extern void
aac_rescale(Float *coef[Chans],  int ch, int mute,AAC_DECODER_STRUCT_T *p_global );

extern void
aac_rescale_and_filter(Float *coef[Chans],int first,int last, int mute, int headroom,AAC_DECODER_STRUCT_T *p_global,
					   int concealment_on,
					   int badFrame );

extern void
aac_filterbank(int *coef_fix[Chans], int ch, int headroom, AAC_DECODER_STRUCT_T *p_global);

#else

extern void
aac_rescale_channel(Info * info, Float *coef, int *exponent, int ch, int wn, AAC_DECODER_STRUCT_T *p_global);

extern void
aac_rescale_and_filter(Float *coef[Chans],int first,int last, int mute, int headroom,AAC_DECODER_STRUCT_T *p_global,
					   int concealment_on,
					   int badFrame );


extern void
aac_rescale(Float *coef_fix[Chans], int ch, int mute,AAC_DECODER_STRUCT_T *p_global );

extern void
aac_filterbank(Float *coef[Chans], int ch, int headroom, AAC_DECODER_STRUCT_T *p_global);

#endif

#ifdef MC
extern void 
vector_interleave_mc(Float *output, int len,  unsigned int bits_to_output, CODEC_INTERFACE_T *interface_fe);
#endif

#ifdef STD_PARSE
extern void aac_std_init(CODEC_INTERFACE_T *interface_fe);
extern void aac_reset(CODEC_INTERFACE_T *eaacPlus_Interface);
extern int aac_parse_adts_header(void *hBs,  AAC_DECODER_CONFIG_STRUCT_T *p_aac_config, int *bits_consumed);
extern int aac_parse_adif_header(void *hBs, AAC_DECODER_CONFIG_STRUCT_T *p_aac_config);
#endif


#endif				/* _port_h_ */





