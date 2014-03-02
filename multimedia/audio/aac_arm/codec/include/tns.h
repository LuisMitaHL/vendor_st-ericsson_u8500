
/************************* MPEG-2 NBC Audio Decoder **************************
 *                                                                           *
"This software module was originally developed by
AT&T, Dolby Laboratories, Fraunhofer Gesellschaft IIS in the course of
development of the MPEG-2 NBC/MPEG-4 Audio standard ISO/IEC 13818-7,
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

#ifndef	_tns_h_
#define	_tns_h_


//#define TNS_MAX_ORDER	31 BUG

#define TNS_MAX_ORDER   20
#define	TNS_MAX_WIN	8
#define TNS_MAX_FILT	3
#define MAX_SFB_PER_SBK 51


#ifdef BIT_FIELDS
#define SIZE_TNS_COEF     (TNS_MAX_ORDER/ ((MAX_BITS+1)/(LEN_TAG)) +1 )

typedef struct {
    int             coef[SIZE_TNS_COEF];
    unsigned int    start_band:8;
    unsigned int    stop_band:8;
    unsigned int    order:5;
    unsigned int    direction:1;
    unsigned int    coef_compress:1;

}               TNSfilt;

typedef struct {
    TNSfilt         filt[TNS_MAX_FILT];
    unsigned int    n_filt:8;
    unsigned int    coef_res:8;

}               TNSinfo;

#else

#define SIZE_TNS_COEF     TNS_MAX_ORDER

typedef struct {
    int             start_band;
    int             stop_band;
    int             order;
    int             direction;
    int             coef_compress;
    int             coef[SIZE_TNS_COEF];
}               TNSfilt;

typedef struct {
    int             n_filt;
    int             coef_res;
    TNSfilt         filt[TNS_MAX_FILT];
}               TNSinfo;

#endif /* BIT_FIELDS */

typedef TNSinfo TNS_frame_info[TNS_MAX_WIN];

extern void
aac_clr_tns(Info * info, TNS_frame_info * tns_frame_info);
extern int
aac_get_tns(void *hBs, Info * info, TNS_frame_info * tns_frame_info);
extern void
aac_print_tns(TNSinfo * tns_info);

#ifdef ARM
extern void
aac_tns_ar_filter(int *spec_fix,  int size, int inc,
		  		  int  FT_MEM *lpc, int order			  
				  );

extern void
aac_tns_inv_filter(int *spec, int size, int inc, int FT_MEM * lpc, int order		  
				  );

extern void
aac_tns_decode_coef(int order, int coef_res, int compress, SHORT * coef,
					 int *lpc);

extern void
aac_tns_decode_subblock(int *spec,  int nbands,
						SHORT const SR_MEM * sfb_top,
						int max_bands, int max_order, TNSinfo * tns_info);

extern void
aac_tns_filter_subblock(int *spec_fix,int nbands, SHORT const SR_MEM *sfb_top, int islong,
					TNSinfo *tns_info,MC_Info aac_mc_info		
			   	   );

extern void
aac_tns_decode(Info * info, int *coef_fix,  int nbands,
			   TNS_frame_info * tns_frame_info, MC_Info aac_mc_info
			   );

#else /* ARM */

extern void
aac_tns_ar_filter(Float *spec, int *exponent, int size, int inc,
		  		  Float  FT_MEM *lpc, int order
#ifdef MMDSP				  
				  ,int start_sfb
				  ,int stop_sfb
				  ,SHORT const SR_MEM * sfb_top
#endif				  
				  );

extern void
aac_tns_inv_filter(Float *spec, int size, int inc, Float FT_MEM * lpc, int order
#ifdef MMDSP				  
				  ,int *exponent 	
#endif				  
				  );

extern void
aac_tns_decode_coef(int order, int coef_res, int compress, int *coef,
		    Float FT_MEM *lpc);


extern void
aac_tns_decode_subblock(Float * spec, int *exponent, int nbands,
			SHORT const SR_MEM *sfb_top,
			int max_bands, int max_order,
			TNSinfo * tns_info
#ifdef MMDSP
#ifdef BUFFER_ALLOCATION_IN_SCRATCH_BUFFER
			,Float YMEM *aac_scratch_buffer_y
#else
			,Float YMEM *coefslpc_y
#endif
#endif
);

void
aac_tns_decode(Info * info, Float *coef, int *exponent, int nbands,
			   TNS_frame_info * tns_frame_info, MC_Info aac_mc_info
#ifdef MMDSP
#ifdef BUFFER_ALLOCATION_IN_SCRATCH_BUFFER
			   , Float YMEM *aac_scratch_buffer_y
#else
			   , Float YMEM *coefslpc_y
#endif
#endif
			   );

extern void
aac_tns_filter_subblock(Float *spec,int *exponent, int nbands, 
						SHORT const SR_MEM *sfb_top, int islong,
						TNSinfo *tns_info,MC_Info aac_mc_info
#ifdef MMDSP						
#ifdef BUFFER_ALLOCATION_IN_SCRATCH_BUFFER
						,Float YMEM *aac_scratch_buffer_y
#else
						,Float YMEM *coefslpc_y
#endif
#endif
				   	   );
				  
#endif /* ARM */

extern int      aac_tns_max_bands(int win, MC_Info aac_mc_info);
extern int      aac_tns_max_order(int islong, MC_Info aac_mc_info);

void		clr_tns( Info *info, TNS_frame_info *tns_frame_info );
int			get_tns( void *hBs,Info *info, TNS_frame_info *tns_frame_info );

void		tns_ar_filter( Float *spec, int size, int inc, Float *lpc, int order );
void		tns_decode_coef( int order, int coef_res, SHORT *coef, Float *a );
void		tns_decode_subblock(Float *spec, int nbands, SHORT const SR_MEM *sfb_top, int islong,TNSinfo *tns_info,MC_Info aac_mc_info);
void        tns_filter_subblock(Float *spec, int nbands, SHORT const SR_MEM *sfb_top, int islong,
				    TNSinfo *tns_info,MC_Info aac_mc_info);
int		tns_max_bands(int win,MC_Info aac_mc_info);
int		tns_max_order(int islong,MC_Info aac_mc_info);


#endif				/* _tns_h_ */
