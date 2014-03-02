
/************************* MPEG-2 NBC Audio Decoder **************************
 *                                                                           *
"This software module was originally developed by
AT&T, Dolby Laboratories, Fraunhofer Gesellschaft IIS
and edited by
Yoshiaki Oikawa (Sony Corporation),
Mitsuyuki Hatanaka (Sony Corporation)
Mike Coleman (Five Bats Research)
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
Copyright(c)1996,1997.
 *                                                                           *
 ****************************************************************************/

#include "aac_local.h"

#ifndef __CC_ARM
#include <fcntl.h>
#endif

#define USE_ATOI

#ifdef WIN32
#include <io.h>
#endif

#define	ARGBEGIN	for(argv++,argc--;\
			    argv[0] && argv[0][0]=='-' && argv[0][1];\
			    argc--, argv++) {\
				char *_args, *_argt;\
				char _argc;\
				_args = &argv[0][1];\
				if((_args[0]=='-') && (_args[1]==0)){\
					argc--; argv++; break;\
				}\
				_argc = 0;\
				while((_argc = *_args++))\
				switch(_argc)
#define	ARGEND		}
#define	ARGF()		(_argt=_args, _args="",\
				(*_argt? _argt: argv[1]? (argc--, *++argv): 0))
#define	ARGC()		_argc

#define	USED(v)		if(v);else


#include <stdio.h>

#define	PATH_LEN    128

extern FILE    *infile;
extern long            total1;
extern long            total2;
extern int             debug[256];
extern unsigned long            aac_framenum;
extern unsigned long            frameStop;
extern Float *pl_ocfg_ptr;
extern CODEC_INTERFACE_T AAC_Interface;

extern void           *mal1(long size);
extern void           *mal2(long size);

extern void            usage(char *s);

#ifdef STD_PARSE
extern void            initio(int argc, char *argv[],
							  CODEC_INTERFACE_T *AAC_Interface, AUDIO_MODE_T *codec_mode);
#else
extern void            initio(int argc, char *argv[],
							  AAC_DECODER_CONFIG_STRUCT_T *p_aac_config, AUDIO_MODE_T *codec_mode);
#endif

extern int             open_output_files(MC_Info * mip, AAC_DECODER_STRUCT_T *p_global);

extern void            fmtchan(char *p, Float *data, int stride);


extern int             getbyte(void);

extern void            closefiles(void);

extern void            toggle_output_buffer(AAC_DECODER_STRUCT_T *p_global,int enbl_24bit_samples);

#ifdef MC
extern void vector_interleave_mc(Float *output, int len ,unsigned int bits_to_output,CODEC_INTERFACE_T *interface_fe );

extern void            output_samples(Float *buffer,CODEC_INTERFACE_T *interface_fe  ,int enbl_24bit_samples);

extern void           writeout(CODEC_INTERFACE_T *interface_fe,int enbl_24bit_samples);

#else
extern void            output_samples(Float *buffer,AAC_DECODER_STRUCT_T *p_global, int enbl_24bit_samples);

extern void            writeout(AAC_DECODER_STRUCT_T *p_global,int enbl_24bit_samples);
#endif
extern void            getbits_restart(void);

extern void
close_ext_files(AAC_DECODER_STRUCT_T *p_global);
