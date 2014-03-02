/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*   
#############################################################
File:     ast_glob.h

Author:   Daniele Bagni
email:    daniele.bagni@st.com  

Org:      Agrate Advanced System Technologies lab.
          STMicroelectronics S.r.l.

Created:  04.03.99

Purpose:  macros visible from all the modules 

Notes:    Remember that the declaration of an "extern" variable 
          does not allocate memory for it as its definition.
          Only one C module has to define these (extern) 
          variables, with inherent memory allocations: main.h

Status:   

Modified: 

COPYRIGHT:
This program is property of Agrate Advanced System Technology
lab, from STMicroelectronics S.r.l. It should not be 
communicated outside STMicroelectronics without authorization.
################################################################
*/

#ifndef H_AST_GLOB_H
#define H_AST_GLOB_H

#include "ast_types.h"
#include <stdio.h>

#ifndef MMTC_TRACE_FILE_ASCII
/* ### MMTC add (03/20/02)         ### */
/* ### macro to update time        ###*/
#ifdef MMTC_TRACE_FILE
//#include <mtc_file_types.h>
#include "mmtc_file.h"
#endif
/* ### End of MMTC add (03/20/02)  ### */
#endif

/* 
global variables:
choose between declaration (AST_GLOBAL undefined)
and definition (AST_GLOBAL defined)
AST_GLOBAL is defined in exactly one file (main.h)
*/

#undef EXTERN

#ifndef AST_GLOBAL
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN unsigned char guc_verbose
#ifdef AST_GLOBAL
= 2
#endif
;

EXTERN unsigned char guc_verb
#ifdef AST_GLOBAL
= 1
#endif
;

EXTERN unsigned char guc_severity
#ifdef AST_GLOBAL
= 1
#endif
;


#ifdef STATISTIC
EXTERN  FILE        *fpstatistic;
EXTERN struct statistic_s s;
#endif /* STATISTICS */

#ifndef MMTC_TRACE_FILE_ASCII
/* ### MMTC add (03/20/02)         ### */
/* ### macro to update time        ###*/
#ifdef MMTC_TRACE_FILE
#define maddtime(x) (Gs_mtc_m4e_api.time_out+=(x))

/** \brief global variables used */
EXTERN struct s_mtc_m4d_api {
	
    /* ptr on file to dump data */
    t_pt_file_mgt   p_file_mgt_out;  

    /* ID of hw data */
    t_sint32 id_hw_mb_number;
    t_sint32 id_hw_dct_in;
    t_sint32 id_hw_dct_out;
    t_sint32 id_hw_q_out;
    t_sint32 id_hw_mecc_pa_out;
    t_sint32 id_hw_recw_out;

    /* Structure for hw variables */
    t_dval_rl_block  	hw_dct_out;  	/**<\brief For IDCT output */
    t_dval_rl_block  	hw_q_out;  		/**<\brief For Q output */
    t_dval_rl_block  	hw_dct_in;  	/**<\brief For IDCT input */
    t_dval_tabint32		hw_recw_out;	/**<\brief For Reconstruction Fifo */

	/* ID of all blocks */
	t_sint32 id_parameters_in;
	t_sint32 id_parameters_inout_before;
	t_sint32 id_parameters_inout_after;
	t_sint32 id_parameters_out;
	t_sint32 id_buffer;
	t_sint32 id_header;
	t_sint32 id_init;
	t_sint32 id_host;
	t_sint32 id_rlc;
	t_sint32 id_packer;
	t_sint32 id_mecc_in;
	t_sint32 id_mecc_out; 
	t_sint32 id_enc_qp; 

	/* Structure for output variables */
    t_dval_tabint16	  parameters_in;     /**<\brief tructure for in parameters */
    t_dval_tabint16	  parameters_inout_before; /**<\brief structure for io parameters */
    t_dval_tabint16	  parameters_inout_after; /**<\brief structure for io parameters */
    t_dval_tabint16	  parameters_out;    /**<\brief structure for out parameters */
    t_dval_tabint16	  buffer;	/**<\brief structure for buffer 			*/
	t_dval_blockreg	  host;		/**<\brief structure for host interface 	*/
	t_dval_rl_event   rlc;		/**<\brief structure for rlc 				*/
	t_dval_pack 	  packer;	/**<\brief structure for packer 			*/
	t_dval_to_mecc 	  mecc_in;	/**<\brief structure for mecc(MV...) 		*/
	t_dval_from_mecc  mecc_out;	/**<\brief structure for mecc (SAD...) 		*/
    t_dval			  enc_qp;  	/**<\brief One value for QP (for each MB) 	*/

	/* Time for output file */
	long long int time_out;

	/* ### Variable to know if current bits are for header ###*/
	t_uint32 header_var;

	/* ### Copy of mp4_param for SHDR FLAG. (info not present 
       ### in Algorithmic Data Encode, where it is needed)*/
	t_uint32 short_header;
    t_uint32 gob_header;

	/* ### To allow or not dump of packed value  ###*/
	t_uint32 save_packer;

	/* ### To save bitstream offset and value after header.  ###*/
    t_uint32 offset_size;
	unsigned long int offset_value;
	unsigned long long int bitstream_value;
	t_uint32 bitstream_position;

	/* ### To make a copy of DC coeff before prediction.  ###*/
    t_sint32 DC[6];

    /* ### Other parameters collected in various files to be
       dumped in .pf files */
    t_uint32 vp_size_max;            /* bitstream.c */
    t_sint32 modulo_time_base;       /* bitstream_cpu.c */
    t_uint32 slice_loss_first_mb[8]; /* dynopt.c */
    t_uint32 slice_loss_mb_num[8];   /* dynopt.c */
    /* in param for BRC */
    t_uint16 brc_type;             /* Not used yet */      
    t_uint32 brc_frame_target;     /* MaSa_ratectrl.c */      
    t_uint32 brc_target_min_pred;  /* MMS_ratectrl.c */
    t_uint32 brc_target_max_pred;  /* MMS_ratectrl.c */
  t_uint16 vop_time_increment;   /* for HECs */

    t_uint32 bit_rate;
    t_uint16 framerate;
    t_sint32 delta_target;
    t_uint16 minQp;
    t_uint16 maxQp;
    t_uint16 vop_time_increment_resolution;
    t_uint16 fixed_vop_time_increment;
    t_uint32 Smax;
    t_uint16 min_base_quality;     /* in VBR */
    t_uint16 min_framerate;        /* in VBR */
    t_uint32 max_buff_level;       /* in CBR */
    t_uint32 ext_skip;             /* in CBR */

    /* inout param for BRC */
    t_uint32 bitstream_size;
    t_uint32 stuffing_bits;

    t_uint32 pictCount;
    t_uint16 I_Qp;
    t_uint16 P_Qp;
    t_uint32 last_I_Size;
    t_uint32 comp_SUM;
    t_uint32 comp_count;
    t_uint16 BUFFER_mod;
    t_sint16 ts_modulo_old;
    t_uint32 ts_seconds_old;
    t_uint32 avgSAD;                  /* in VBR */
    t_uint32 seqSAD;                  /* in VBR */
    t_uint16 min_pict_quality;        /* in VBR */
    t_uint16 diff_min_quality;        /* in VBR */
    t_uint32 TotSkip;                 /* in VBR */
    t_uint32 Skip_Current;            /* in VBR and CBR */
    t_uint16 Cprev;                   /* in CBR */
    t_uint16 BPPprev;                 /* in CBR */
    t_uint32 PictQpSum;               /* in CBR */
    t_uint32 PictQpSumIntra;               /* in CBR */
    t_uint32 S_overhead;              /* in CBR */
    t_sint32 buffer_fullness;         /* in CBR */
    t_uint32 ts_seconds;	  
    t_sint16 ts_modulo;
    t_uint16 hec_count;
  t_sint32	buffer_fullness_fake_TS;
  t_uint32	BUFFER_depletion_fake_TS; /* added in MAINVER2.5a */
  t_uint16	gov_flag;
  
  t_sint32 ts_vector[6];
  
  t_uint16 brc_skip_prev;

  /* added from MAINVER2.4d */
  t_uint16	first_I_skipped_flag;
  t_sint16	init_ts_modulo_old;
  t_uint32	BUFFER_depletion;
  t_uint16	buffer_saved;

  t_uint16 intra_Qp_flag;
  t_uint16 old_p_Qp_vbr;
  t_uint32 pictCount_prev;

} Gs_mtc_m4e_api;
#endif
/* ### End of MMTC add (03/20/02)  ### */

#else
/* Dump IN/OUT params in ASCII format */
/* Comes from /MPU_HAMACV_FW/common/inc/t1xhv_bt_utils.h */
typedef struct {
        t_uint16 length;
        t_sint16 value;
} ts_packer_bt;

typedef struct {
        ts_packer_bt *ps_packer_bt;
        t_uint16 nb_elt;
} ts_fifo_packer_bt;

/* Comes from /MMTC_TOOLS/Hamac/Design/MTC_FILE/delivery/inc/mtc_file_types.h 
              /MMTC_TOOLS/Hamac/Design/MTC_FILE/delivery/inc/mtc_file_const.h */

#define maddtime(x) (Gs_mtc_m4e_api.time_out+=(x))
#define MTC_NB_ID             0x80
#define MTC_FILE_NBVAR        200
#define MTC_NB_VAR            10

typedef struct {
	int			high;
	int			low;
} t_tr_time;

typedef struct {
	int			magic;
	int			file_type;
	float		version;
	int			nb_var;
	int			scale;
	t_tr_time	start_time;
	t_tr_time	end_time;
	char		comment[256];
	char		seq_name[80];	/* name of video sequence  	*/
	int			norme;			/* type  MPEG4, H263, ...	*/
	int			option;			/* type  ShortHeader, ...	*/
	int			format;			/* type  QCIF, CIF, VGA, ...*/
	int			type;			/* YUV420, RGB,....			*/
	int			frame_rate;		/* type  15Hz, 30Hz, ...*/
	int			nb_frame;
	int			frame_width;
	int			frame_height;
	int			bite_rate;
	int			dummy[11];		/* for evolution purpose  */
} t_header, *t_pt_header;

typedef struct {
	int   	id;				
	int		nb_value;				
	int		data_type;				
	char		name[64];
} t_defvar, *t_pt_defvar;

typedef struct {
	t_tr_time	time;
	int	   	nb_dumpvalue;
} t_hdval, *t_pt_hdval;

typedef struct {
	int	value;
} t_dval, *t_pt_dval;

typedef struct {   
	t_uint16 	rnd;
	t_uint16 	image_width;
	short 	scx_stab;
}  t_dval_config_mecc, *t_pt_dval_config_mecc;

typedef struct {   
	t_uint16 	command; 
    t_uint16        decision;    /* duplicates a part of command */
    t_uint16        nb_mv;       /* duplicates a part of command */
	short 	pc_in[9][2]; /* 0-> x, 1->y */
} t_dval_to_mecc, *t_pt_dval_to_mecc;

typedef struct {   
	t_uint16 	mean;
	t_uint16 	mad;
	t_uint16 	sad;
	t_uint16 	pc_out[2];   /* 0-> x, 1->y */
	t_uint16 	sad_d;
	t_uint16 	pc_out_d[2]; /* 0-> x, 1->y */
	t_uint16 	last_sad;
} t_dval_from_mecc, *t_pt_dval_from_mecc;

typedef struct {
	t_uint32	block_number;
	t_uint32	block_type;
	t_uint32	nb_evt;
	unsigned char		last[64];
	unsigned char		run[64];
	t_uint16	level[64];
} t_dval_rl_event, *t_pt_dval_rl_event;

typedef struct {
	t_uint32	block_number;
	t_uint32	block_type;
	t_uint16	block[64];
} t_dval_rl_block, *t_pt_dval_rl_block;

typedef struct {    
    t_uint32	cmd:2;    /* Num of buffer or flush */
    t_uint32    length:5; /* nb of bits added */
    t_uint32    value:16; /* value to add */
    t_uint32    dummy:9;  /* to fill a 32bit word */
} t_dval_pack, *t_pt_dval_pack;

typedef struct {
	int	nb_vect;
	char	x[8];
	char	y[8];
} t_dval_dma, *t_pt_dval_dma;

typedef struct {
	short		val[64];	/* Pixel or coefficient		*/
	t_uint32	where;		/* after DCT, after Q...	*/
	t_uint32	MB_x;		/* coordonnes of MacroBlock */ 
	t_uint32	MB_y;		/* the bloc belongs to 		*/
	short		type;		/* Y1, Y2, Y3, Y$, U or V	*/
} t_dval_bloc, *t_pt_dval_bloc;

typedef struct {
	char		rate_ctrl;
} t_dval_rate, *t_pt_dval_rate;

typedef struct {
	int  	nb;
	char 	*value;
} t_dval_tabbyte, *t_pt_dval_tabbyte;

typedef struct {
	int  	nb;
	short	*value;
} t_dval_tabint16, *t_pt_dval_tabint16;

typedef struct {
	int  	nb;
	int	*value;
} t_dval_tabint32, *t_pt_dval_tabint32;

typedef struct {
	short  	X_addr;
	short  	X_data_w;
	short  	X_data_r;
	short  	X_write;		/* 1->write, 0->read	*/
} t_dval_xbus, *t_pt_dval_xbus;

typedef struct ts_fw_profile {
    unsigned char	read_write;		/* Access type on XBUS */
    unsigned char	bloc_id;		/* HW bloc ID              */
    unsigned short	command;		/* Offset of address (command on block) */
    unsigned char	end;			/* Return of XIORDY        */
    unsigned char	function_id;	/* Id for current function execution */
    unsigned char	group_id;		/* Id for group associated to current function .*/
} t_dval_fw_prof, *t_pt_dval_fw_prof;

typedef struct {
	char name[16];
	int offset;
	int value;
} t_mtc_reg, *t_pt_mtc_reg;

typedef struct {
	char name[16];
	int addr_base;
	int nb_reg;
	t_pt_mtc_reg pt_reg;
} t_dval_blockreg, *t_pt_dval_blockreg;

typedef struct {
	void (*skip)(int **);
	void (*dump)(int **, void *);
	int (*diff)(void *, void *, int, int *);
	void (*print)(int **, int);   
} t_type_mgt, *t_pt_type_mgt;

typedef struct {
	int			state;
	int			nb_var;				/* number of variables in the file */
	int 		fd;					/* file descriptor	*/
	char		filename[1024];
	t_tr_time	cycle;
	t_tr_time	end_time;
	int			flag;   
	t_pt_hdval	pt_hdval;			/* current position needed element */
	t_pt_dval	pt_dval;
	int			cur_var;
	t_pt_header	pt_header;
	t_pt_defvar	pt_defvar;
	int			typetab[MTC_FILE_NBVAR];	/* type of each variable   */
	t_pt_hdval	*pt_hdtab;
	int			nb_page;
	int			size_buff;
	int			nb;
	int			flip_flop;
	int 		*pt_id;
	char 		*pt_crt;
	char 		*pt_flip;				/* dual write buffer  */
	char 		*pt_limite_flip;
	char 		*pt_flop;
	char 		*pt_limite_flop;
	t_type_mgt	type[MTC_NB_ID];
	char	Var_Name[MTC_NB_ID][32];
	char	File_Norme[MTC_NB_VAR][32];
	char	File_Option[MTC_NB_VAR][32];
	char	File_Type[MTC_NB_VAR][32];
	char	File_Format[MTC_NB_VAR][32];
	char	File_FRate[MTC_NB_VAR][32];
	t_header	tmp_header;
} t_file_mgt, *t_pt_file_mgt;

/** \brief global variables used */
EXTERN struct s_mtc_m4d_api {
	
    /* ptr on file to dump data */
    t_pt_file_mgt   p_file_mgt_out;  

    /* ID of hw data */
    t_sint32 id_hw_mb_number;
    t_sint32 id_hw_dct_in;
    t_sint32 id_hw_dct_out;
    t_sint32 id_hw_q_out;
    t_sint32 id_hw_mecc_pa_out;
    t_sint32 id_hw_recw_out;

    /* Structure for hw variables */
    t_dval_rl_block  	hw_dct_out;  	/**<\brief For IDCT output */
    t_dval_rl_block  	hw_q_out;  		/**<\brief For Q output */
    t_dval_rl_block  	hw_dct_in;  	/**<\brief For IDCT input */
    t_dval_tabint32		hw_recw_out;	/**<\brief For Reconstruction Fifo */

	/* ID of all blocks */
	t_sint32 id_parameters_in;
	t_sint32 id_parameters_inout_before;
	t_sint32 id_parameters_inout_after;
	t_sint32 id_parameters_out;
	t_sint32 id_buffer;
	t_sint32 id_header;
	t_sint32 id_init;
	t_sint32 id_host;
	t_sint32 id_rlc;
	t_sint32 id_packer;
	t_sint32 id_mecc_in;
	t_sint32 id_mecc_out; 
	t_sint32 id_enc_qp; 

	/* Structure for output variables */
    t_dval_tabint16	  parameters_in;     /**<\brief tructure for in parameters */
    t_dval_tabint16	  parameters_inout_before; /**<\brief structure for io parameters */
    t_dval_tabint16	  parameters_inout_after; /**<\brief structure for io parameters */
    t_dval_tabint16	  parameters_out;    /**<\brief structure for out parameters */
    t_dval_tabint16	  buffer;	/**<\brief structure for buffer 			*/
	t_dval_blockreg	  host;		/**<\brief structure for host interface 	*/
	t_dval_rl_event   rlc;		/**<\brief structure for rlc 				*/
	t_dval_pack 	  packer;	/**<\brief structure for packer 			*/
	t_dval_to_mecc 	  mecc_in;	/**<\brief structure for mecc(MV...) 		*/
	t_dval_from_mecc  mecc_out;	/**<\brief structure for mecc (SAD...) 		*/
    t_dval			  enc_qp;  	/**<\brief One value for QP (for each MB) 	*/

	/* Time for output file */
#if _MSVC
	/* Time for output file */
	__int64 time_out;
#else
	/* Time for output file */
	long long int time_out;
#endif

	/* ### Variable to know if current bits are for header ###*/
	t_uint32 header_var;

	/* ### Copy of mp4_param for SHDR FLAG. (info not present 
       ### in Algorithmic Data Encode, where it is needed)*/
	t_uint32 short_header;
    t_uint32 gob_header;

	/* ### To allow or not dump of packed value  ###*/
	t_uint32 save_packer;

	/* ### To save bitstream offset and value after header.  ###*/
    t_uint32 offset_size;
	unsigned long int offset_value;

#if _MSVC
	unsigned __int64 bitstream_value;
#else
	unsigned long long int bitstream_value;
#endif
	t_uint32 bitstream_position;

	/* ### To make a copy of DC coeff before prediction.  ###*/
    t_sint32 DC[6];

    /* ### Other parameters collected in various files to be
       dumped in .pf files */
    t_uint32 vp_size_max;            /* bitstream.c */
    t_sint32 modulo_time_base;       /* bitstream_cpu.c */
    t_uint32 slice_loss_first_mb[8]; /* dynopt.c */
    t_uint32 slice_loss_mb_num[8];   /* dynopt.c */
    /* in param for BRC */
    t_uint16 brc_type;             /* Not used yet */      
    t_uint32 brc_frame_target;     /* MaSa_ratectrl.c */      
    t_uint32 brc_target_min_pred;  /* MMS_ratectrl.c */
    t_uint32 brc_target_max_pred;  /* MMS_ratectrl.c */
  t_uint16 vop_time_increment;   /* for HECs */

    t_uint32 bit_rate;
    t_uint16 framerate;
    t_sint32 delta_target;
    t_uint16 minQp;
    t_uint16 maxQp;
    t_uint16 vop_time_increment_resolution;
    t_uint16 fixed_vop_time_increment;
    t_uint32 Smax;
    t_uint16 min_base_quality;     /* in VBR */
    t_uint16 min_framerate;        /* in VBR */
    t_uint32 max_buff_level;       /* in CBR */
    t_uint32 ext_skip;             /* in CBR */

    /* inout param for BRC */
    t_uint32 bitstream_size;
    t_uint32 stuffing_bits;

    t_uint32 pictCount;
    t_uint16 I_Qp;
    t_uint16 P_Qp;
    t_uint32 last_I_Size;
    t_uint32 comp_SUM;
    t_uint32 comp_count;
    t_uint32 BUFFER_mod;
    t_sint16 ts_modulo_old;
    t_uint32 ts_seconds_old;
    t_uint32 avgSAD;                  /* in VBR */
    t_uint32 seqSAD;                  /* in VBR */
    t_uint16 min_pict_quality;        /* in VBR */
    t_uint16 diff_min_quality;        /* in VBR */
    t_uint32 TotSkip;                 /* in VBR */
    t_uint32 Skip_Current;            /* in VBR and CBR */
    t_uint16 Cprev;                   /* in CBR */
    t_uint16 BPPprev;                 /* in CBR */
    t_uint32 PictQpSum;               /* in CBR */
    t_uint32 PictQpSumIntra;          /* in CBR */
    t_uint32 S_overhead;              /* in CBR */
    t_sint32 buffer_fullness;         /* in CBR */
    t_uint32 ts_seconds;	  
    t_sint16 ts_modulo;
    t_uint16 hec_count;
  t_sint32	buffer_fullness_fake_TS;
  t_uint32	BUFFER_depletion_fake_TS; /* added in MAINVER2.5a */
  t_uint16	gov_flag;
  
  t_sint32 ts_vector[6];
  
  t_uint16 brc_skip_prev;

  /* added from MAINVER2.4d */
  t_uint16	first_I_skipped_flag;
  t_sint16	init_ts_modulo_old;
  t_uint32	BUFFER_depletion;
  t_uint16	buffer_saved;

  t_uint16 intra_Qp_flag;
  t_uint16 old_p_Qp_vbr;
  t_uint32 pictCount_prev;
  t_uint16 mv_field_index;
  t_uint16 BPPmbPrevAct;
  t_uint16 BPPmbPrevEst;
  t_uint16 CodedMB;
  t_uint16 CodedMB_flag;
  t_sint32 Lprev;
  t_sint32 BPPmbErr;

} Gs_mtc_m4e_api;
/* End of Dump IN/OUT params in ASCII format */
#endif

#endif   /* H_AST_GLOB_H */










