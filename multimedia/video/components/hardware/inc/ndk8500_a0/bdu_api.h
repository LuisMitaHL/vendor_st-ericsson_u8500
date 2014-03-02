/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _BDU_API_H_
#define _BDU_API_H_

/*
 * Includes							       
 */
#include <stwdsp.h>             /* for mmdsp intrinsics */
#include "macros.h"            /* for assertion function in debug mode */
#include "t1xhv_types.h"        /* for basic types definitions */
#include "t1xhv_hamac_xbus_regs.h"
#include "hed_api.h"

/*
 * Defines			       
 */
#define BDU_SHB						(BDU_SHB1-1) 
#define BDU_GTB						(BDU_GTB1-1) 

/*
 * Variables
 */			       
/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get DMA_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_BDU(a) (*(volatile MMIO t_uint16 *)(BDU_BASE+a))
#define HW_BDU_WAIT(a) (*( (volatile __XIO t_uint16 *)(BDU_BASE) + a))



/*
 * Types							       
 */

/** @{ \name BDU_CFR register bitfield values
 *     \warning DO NOT change enum values
 */
typedef enum
{
    BDU_CFR_MPEG4        =0,    /**< \brief  use the BDU in mpeg 4 mode*/
    BDU_CFR_SHORT_HEADER =1,    /**< \brief  use the BDU for short header mode*/
    BDU_CFR_REVERSE_VLC  =2,    /**< \brief use the BDU to perform reverse VLC   */
    BDU_CFR_H263_I       =3,    /**< \brief use bdu to perform H263 annex I parsing   */
    BDU_CFR_H263_T       =4,    /**< \brief use bdu to perform H263 annex T parsing   */
    BDU_CFR_JPEG         =5,    /**< \brief use bdu to perform Jpeg (byte stuffing removal) parsing   */
    BDU_CFR_MPEG1        =6,    /**< \brief use bdu in MPEG1 mode */
    BDU_CFR_MPEG2        =7,    /**< \brief use bdu in MPEG2 mode */
    BDU_CFR_H264         =8,     /**< \brief use bdu in h264 mode (anti-emulation byte removal */
    BDU_CFR_VC1          =9,
    BDU_CFR_JPEG_STANDBY =10,
    BDU_CFR_H264_STANDBY =11,
    BDU_CFR_MPEG2_VLC1   =12
} t_bdu_cfr;


typedef enum
{
    BDU_TCOEF_INTER    = 0x1,
    BDU_TCOEF_INTRA    = 0x3,
    BDU_TCOEF_INTRA64  = 0x5,
    BDU_TCOEF_RINTER   = 0x9,
    BDU_TCOEF_RINTRA   = 0xB,
    BDU_TCOEF_RINTRA64 = 0xD
} t_bdu_tcoef;

typedef enum
{
    RST_MARKER_DISABLE = 0,
    RST_MARKER_ENABLE = 1

}t_bdu_jpg_rst_marker;

typedef enum
{
    BDU_VC1_DC_LOW_MOT = 0,
    BDU_VC1_DC_HIGH_MOT = 1

}t_bdu_vc1_dc_table;

typedef enum
{
    BDU_VC1_CHROMA_LUMA_HIGH_MOT = 0,
    BDU_VC1_CHROMA_LUMA_LOW_MOT = 1,
    BDU_VC1_CHROMA_LUMA_MID_RATE = 2,
    BDU_VC1_CHROMA_LUMA_HIGH_RATE = 3

}t_bdu_vc1_chroma_luma_table;
typedef enum
{
    I_FRAME =0,
    P_FRAME = 1,
    B_FRAME = 2

}t_bdu_frame_type;
typedef enum
{
    BDU_VC1_8x8 = 0,
    BDU_VC1_8x4 = 1,
    BDU_VC1_4x8 = 2,
    BDU_VC1_4x4 = 3

}t_bdu_vc1_block_partition;

typedef enum
{
    BDU_VC1_CHROMA = 0,
    BDU_VC1_LUMA = 1
}t_bdu_vc1_block_type;
typedef enum
{
    BDU_VC1_INTER = 0,
    BDU_VC1_INTRA = 1
}t_bdu_vc1_block;

typedef enum
{
    BDU_VC1_1MV = 0,
    BDU_VC1_4MV = 1
}t_bdu_vc1_hdr_mv;

typedef enum
{
    MVMODE_1MV                = 0,   /** 1MV     0.25 pel bicubic   */
    MVMODE_MIXED_MV            = 1,   /** MixedMV 0.25 pel bicubic   */
    MVMODE_1MV_HALF_PEL         = 2,   /** 1MV     0.50 pel bicubic   */
    MVMODE_1MV_HALF_PEL_BILINEAR = 3,   /** 1MV     0.50 pel bilinear  */
    MVMODE_INTENSITY_COMPENSATION     /** VARIABLE LENGTH CODE escape flag */
} t_vc1_mv_mode;


typedef enum
{
    MVRANGE_64_32    = 0,    /* x=-64 to 63.f by y=-32 to 31.f */
    MVRANGE_128_64   = 1,
    MVRANGE_512_128  = 2,
    MVRANGE_1024_256 = 3
} t_vc1_mv_range;


typedef enum
{

    BDU_VC1_QUANT_OTHERS = 0,
    BDU_VC1_QUANT_MODE_MB_DUAL = 2,
    BDU_VC1_QUANT_MODE_MB_ANY = 3

}t_bdu_vc1_hdr_quant_mode;


typedef enum
{
    BDU_VC1_CBPCY_TABLE0 = 0,
    BDU_VC1_CBPCY_TABLE1 = 1,
    BDU_VC1_CBPCY_TABLE2 = 2,
    BDU_VC1_CBPCY_TABLE3 = 3

}t_bdu_vc1_hdr_cbpcy_table;

typedef enum
{
    BDU_VC1_MV_TABLE0 = 0,
    BDU_VC1_MV_TABLE1 = 1,
    BDU_VC1_MV_TABLE2 = 2,
    BDU_VC1_MV_TABLE3 = 3
}t_bdu_vc1_hdr_mv_table;


typedef enum
{

    BDU_VC1_SKIP_BITPLANE_CODED = 0,
    BDU_VC1_SKIP_RAW_CODED = 1
}t_bdu_vc1_hdr_skip_mode;

typedef enum
{
    BDU_VC1_DIRECT_BITPLANE_CODED = 0,
    BDU_VC1_DIRECT_RAW_CODED = 1
}t_bdu_vc1_hdr_direct_mode;

typedef enum
{

    BDU_VC1_MB_ISNOT_SKIPPED = 0,
    BDU_VC1_MB_IS_SKIPPED = 1
}t_bdu_vc1_hdr_mb_skipped;


typedef enum
{

    BDU_VC1_MB_ISNOT_DIRECT = 0,
    BDU_VC1_MB_IS_DIRECT = 1
}t_bdu_vc1_hdr_mb_direct;


typedef enum
{

    BDU_VC1_VSTE_ENABLED = 0,
    BDU_VC1_VSTE_DISABLED = 1
}t_bdu_vc1_hdr_vste;


typedef enum
{

    BDU_VC1_MV_BITPLANE_CODED = 0,
    BDU_VC1_MV_RAW_CODED = 1
}t_bdu_vc1_hdr_mv_mode;

typedef enum
{
    BDU_VC1_TTMB_TABLE1_4 = 0,
    BDU_VC1_TTMB_TABLE5_12 = 1,
    BDU_VC1_TTMB_TABLE13_31 = 2
}t_bdu_vc1_hdr_ttmb_table;

typedef enum
{
    BDU_VC1_B_FRACTION_UNDER_HALF = 0,
    BDU_VC1_B_FRACTION_OVER_HALF = 1
}t_bdu_vc1_hdr_b_fraction;

typedef enum
{
    BDU_VC1_PQUANT_1_7 = 0,/*See the standard*/
    BDU_VC1_PQUANT_8_31 = 1

}t_bdu_vc1_esc3_table;

typedef enum
{
    BDU_VC1_NOT_P_PICT = 0,
    BDU_VC1_P_PICT = 1
}t_bdu_vc1_picture_type;


typedef enum
{
    ACE_DISABLE = 0,
    ACE_ENABLE = 1

}t_bdu_jpg_ace;

typedef enum
{
    IS_NOT_LAST = 0,    /**< \brief the current coeff is NOT the last of the current block   */
    IS_LAST =1          /**< \brief the current coeff is the last of the current block   */

} t_last;

typedef enum
{
  H264_CAVLC_MODE_INTRADC_LUMALEVEL = 0,
  H264_CAVLC_MODE_INTRAAC_CHROMAAC  = 1,
  H264_CAVLC_MODE_DUMMY             = 2,
  H264_CAVLC_MODE_CHROMA_DC         = 3
} t_cavlc_mode;

typedef enum
{
  H264_IPCM_CHROMA = 1,
  H264_IPCM_LUMA   = 3
} t_ipcm;

typedef enum
{
    H264_CAVLC = 0,
    H264_CABAC = 1
}
t_bdu_h264_coding_mode;
typedef enum
{
    H264_T4X4 = 0,
    H264_T8X8 = 1
}
t_bdu_h264_t8x8;
typedef enum
{
    H264_NOT_I16X16 = 0,
    H264_IS_I16X16  = 1
}
t_bdu_h264_i16x16;
/** @} */


/****************************************************************************/
/**
 * \brief 	set a coefficient position and level in the PXP using the 
 *              bdu as a bypass
 * \author 	jean-marc volle
 * \param 	position: position of the coefficient from 0 to 63 relative 
 *              to the scanning order programmed in the PXP
 * \param 	level: level of the coefficient
 * \param 	last: flag stating that this was the last coeff for the current block
 */
/****************************************************************************/
#pragma inline
static void BDU_SET_PXP_COEFF_RUN_LEVEL(t_uint16 position, 
                                    t_sint16 level,
                                    t_last   last)
{
    ASSERT(position < 64);
    ASSERT(level <2047);
    ASSERT(level >-2048);

    HW_BDU_WAIT(BDU_PXP0) = (last << 6) | position;
    HW_BDU_WAIT(BDU_PXP1) = (level & 0x1fff);

}/* end of inline BDU_SET_PXP_COEFF_RUN_LEVEL */
/****************************************************************************/
/**
 * \brief 	set a coefficient position and level in the PXP using the 
 *              bdu as a bypass
 * \author 	jean-marc volle
 * \param 	position: position of the coefficient from 0 to 63 relative 
 *              to the scanning order programmed in the PXP
 * \param 	level: level of the coefficient
 * \param 	last: flag stating that this was the last coeff for the current block
 */
/****************************************************************************/
#pragma inline
static void BDU_SET_PXP_COEFF_RUN_LEVEL_H264(t_uint16 position, 
                                    t_sint16 level,
                                    t_last   last)
{
    ASSERT(position < 64);

    HW_BDU_WAIT(BDU_PXP0) = (last << 6) | position;
    HW_BDU_WAIT(BDU_PXP1) = level;

}/* end of inline BDU_SET_PXP_COEFF_RUN_LEVEL_H264 */


/****************************************************************************/
/**
 * \brief 	get nb_of_bits from the bdu fifo
 * \author 	jean-marc volle
 * \param 	nb_of_bits to get from [1,16]
 * \warning     to be tested without the XIOCLASS
 * \return 	got bits
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_GET_BITS_16(t_uint16 nb_of_bits)
{
    ASSERT(nb_of_bits <= 16);  
    return HW_BDU_WAIT(BDU_GTB+(nb_of_bits));
} 

/****************************************************************************/
/**
 * \brief 	show the next nb_of_bits in the bitstream
 * \author 	jean-marc volle
 * \param 	nb_of_bits: nb of bits to show
 * \return 	 showed bits
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_SHOW_BITS_16(t_uint16 nb_of_bits)
{
    ASSERT(nb_of_bits <= 16);
    return HW_BDU_WAIT(BDU_SHB+(nb_of_bits));
} 

/****************************************************************************/
/**
 * \brief 	show the next nb_of_bits >16 in the bitstream
 * \author 	jean-marc volle
 * \param 	nb_of_bits: nb of bits to show
 * \return 	 showed bits
 */
/****************************************************************************/
#pragma inline
static t_uint32 BDU_SHOW_BITS_32(t_uint16 nb_of_bits)
{
    t_uint16 msb;
    t_uint16 lsb;
    ASSERT(nb_of_bits > 16);

    msb = BDU_SHOW_BITS_16(nb_of_bits-16);
    lsb = BDU_SHOW_BITS_16(nb_of_bits);

    return (MAKE_LONG(msb,lsb));

} 

/****************************************************************************/
/**
 * \brief 	get the next nb_of_bits >16 in the bitstream
 * \author 	jean-marc volle
 * \param 	nb_of_bits: nb of bits to show
 * \return 	 showed bits
 */
/****************************************************************************/
#pragma inline
static t_uint32 BDU_GET_BITS_32(t_uint16 nb_of_bits)
{
    t_uint16 msb;
    t_uint16 lsb;
    ASSERT(nb_of_bits > 16);

    msb = BDU_GET_BITS_16(nb_of_bits-16);
    lsb = BDU_GET_BITS_16(16);

    return (MAKE_LONG(msb,lsb));

} 

/****************************************************************************/
/**
 * \brief 	shift  nb_of_bits in the bitstream
 * \author 	jean-marc volle
 * \param 	nb_of_bits: nb of bits to shift
 * \note         in jpeg 1<=nb_of_bits<=16
 * \note         in other modes  1<=nb_of_bits<=32
 */
/****************************************************************************/
#pragma inline
static void  BDU_SHIFT_BITS(t_uint16 nb_of_bits)
{
    ASSERT(nb_of_bits <= 32);
    ASSERT ( !( ((HW_BDU_WAIT(BDU_CFR) == BDU_CFR_JPEG) || (HW_BDU_WAIT(BDU_CFR) == BDU_CFR_H264)) && (nb_of_bits >16 )));

    HW_BDU_WAIT(BDU_SFT) = nb_of_bits;
} 

/****************************************************************************/
/**
 * \brief 	shift  nb_of_bits in the bitstream
 * \author 	jean-marc volle
 * \param 	nb_of_bits: nb of bits to shift
 * \note         in jpeg 1<=nb_of_bits<=16
 * \note         in other modes  1<=nb_of_bits<=32
 */
/****************************************************************************/
#pragma inline
static void  BDU_SHIFT_BITS_OPT(t_uint16 nb_of_bits)
{
    ASSERT(nb_of_bits <= 32);
    ASSERT ( !( ((HW_BDU_WAIT(BDU_CFR) == BDU_CFR_JPEG) || (HW_BDU_WAIT(BDU_CFR) == BDU_CFR_H264)) && (nb_of_bits >16 )));

    HW_BDU_WAIT(BDU_SFT) = nb_of_bits;
}
/****************************************************************************/
/**
 * \brief 	shift  nb_of_bits in the bitstream
 * \author 	cyril enault
 * \param 	nb_of_bits: nb of bits to flush
 * \note                1<=nb_of_bits<=32
 */
/****************************************************************************/
#pragma inline
static void  BDU_FLUSH_BITS(t_uint16 nb_of_bits)
{
    ASSERT(nb_of_bits <= 32);

    HW_BDU_WAIT(BDU_FLU) = nb_of_bits;
} 


/****************************************************************************/
/**
 * \brief 	get the position of the current bit in the bitstream
 * \author 	jean-marc volle
 * \return 	 bit position
 * \note        The bit position in the bitstream does not equal the number
 *              of bit read in jpeg mode 
 */
/****************************************************************************/
#pragma inline
static t_uint32  BDU_GET_CURRENT_BIT_POS()
{
    t_uint16 lsb;
    t_uint16 msb;
    
    lsb = HW_BDU_WAIT(BDU_CBP_L);
    msb = HW_BDU_WAIT(BDU_CBP_H);
    return  (MAKE_LONG(msb,lsb));
} 
/****************************************************************************/
/**
 * \brief 	configure the BDU for a given parsing
 * \author 	jean-marc volle
 * \param 	cfr: parsing chosen
 */
/****************************************************************************/
#pragma inline
static void BDU_CONFIGURE(t_bdu_cfr cfr)
{
    /* then we configure it: */
    HW_BDU_WAIT(BDU_CFR) = cfr;
}

/****************************************************************************/
/**
 * \brief 	ENABLE the BDU for a given parsing
 * \author 	jean-marc volle
 */
/****************************************************************************/
#pragma inline
static void BDU_ENABLE(void)
{
    /* then we configure it: */
    HW_BDU_WAIT(BDU_ENA) = 0;
}

/****************************************************************************/
/**
 * \brief 	reset the BDU
 * \author 	jean-marc volle
 *  
 * This function resets the BDU
 */
/****************************************************************************/
#pragma inline
static void BDU_INIT(void)
{
    HW_BDU_WAIT(BDU_RST) = 0;
}


/****************************************************************************/
/**
 * \brief 	configure and start the BDU
 * \author 	jean-marc volle
 * \param 	cfr: configuration
 */
/****************************************************************************/
#pragma inline
static void BDU_INIT_CONFIGURE_AND_ENABLE(t_bdu_cfr cfr) 
{
    /* first we reset the BDU */
    BDU_INIT();
    /* then we configure it: */
    BDU_CONFIGURE(cfr);
    
    /* Then we enable it: */
    BDU_ENABLE();
    
}
/****************************************************************************/
/**
 * \brief 	start decode of a mb
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_JPEG_DECODE_START()
{
    HW_BDU_WAIT(BDU_JPG_START_DECODE) = 1;
}
/****************************************************************************/
/**
 * \brief 	get status after decode of a mb
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_JPEG_GET_DECODE_STATUS()
{
    return HW_BDU_WAIT(BDU_JPG_GET_DECODE_STATUS);
}

/****************************************************************************/
/**
 * \brief 	get status after decode of a mb
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static t_sint16 BDU_JPEG_GET_DC_COEFF()
{
    return HW_BDU_WAIT(BDU_JPG_MBDC);
}
/****************************************************************************/
/**
 * \brief 	configure Jpeg parameters
 * \author 	Aroua BEN DARYOUG
 * \param 	number_red_chroma
 * \param 	number_blue_chroma
 * \param       nb_luma
 * \param       rst_marker
 * \param       ace 
 */
/****************************************************************************/
#pragma inline
static void BDU_JPEG_CONFIGURE(t_uint16 number_red_chroma, t_uint16 number_blue_chroma, t_uint16 nb_luma ,t_bdu_jpg_rst_marker rst_marker,t_bdu_jpg_ace ace) 
{
    HW_BDU_WAIT(BDU_JPG_PARAMS) = number_red_chroma |
                                   number_blue_chroma << 4 |
                                   nb_luma << 8 |
                                   rst_marker << 12|
                                   ace << 13;
    
}
/****************************************************************************/
/**
 * \brief 	set MB quantizer
 * \author  Bipul Singh Lodhi
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_JPEG_SET_DC_PERDICTORS ( t_sint16 LU ,t_sint16 CB ,t_sint16 CR ) 
{

HW_BDU_WAIT( BDU_JPG_DCPRED_LU ) = LU;
HW_BDU_WAIT( BDU_JPG_DCPRED_CB ) = CB;
HW_BDU_WAIT( BDU_JPG_DCPRED_CR ) = CR;

}

#pragma inline
static t_sint16 BDU_JPEG_GET_DC_PERDICTOR_LU()
{
 return HW_BDU_WAIT(BDU_JPG_DCPRED_LU);
}

#pragma inline
static t_sint16 BDU_JPEG_GET_DC_PERDICTOR_CB()
{
 return HW_BDU_WAIT(BDU_JPG_DCPRED_CB);
}

#pragma inline
static t_sint16 BDU_JPEG_GET_DC_PERDICTOR_CR()
{
 return HW_BDU_WAIT(BDU_JPG_DCPRED_CR);
}
/****************************************************************************/
/**
 * \brief 	set MB quantizer
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
static void BDU_VC1_SET_MB_QUANT(t_uint16 mb_quant)
{
    HW_BDU_WAIT(BDU_VC1_QUANT)=mb_quant;
}

/****************************************************************************/
/**
 * \brief 	set VC1 tables
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_SET_TABLES(t_bdu_vc1_chroma_luma_table chroma_tables,t_bdu_vc1_chroma_luma_table luma_tables,t_bdu_vc1_dc_table dc_tables,t_bdu_vc1_esc3_table esc3_table,t_bdu_vc1_picture_type frame_type)
{
    HW_BDU_WAIT(BDU_VC1_TABLES)=chroma_tables |
                                 luma_tables << 2 |
                                 dc_tables << 4   |
                                 esc3_table << 5 |
                                 frame_type << 6 ;
}
/****************************************************************************/
/**
 * \brief 	set VC1 tables
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_SET_TABLES_INTER(t_uint16 prg)
{
    HW_BDU_WAIT(BDU_VC1_TABLES)=prg ;
}
/****************************************************************************/
/**
 * \brief 	set block CBP
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_SET_CBP(t_uint16 cbp)
{
    HW_BDU_WAIT(BDU_VC1_CSBP)=cbp;
}
/****************************************************************************/
/**
 * \brief 	start block decode
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_DECODE_START(t_bdu_vc1_block_partition block_partition,t_bdu_vc1_block_type block_type,t_bdu_vc1_block block)
{
    HW_BDU_WAIT(BDU_VC1_START_DECODE)=1|
                                       block_partition <<1|
                                       block_type << 3|
                                       block << 4;
}
/****************************************************************************/
/**
 * \brief 	get decode status
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_VC1_GET_STATUS()
{
    return HW_BDU_WAIT(BDU_VC1_GET_DECODE_STATUS);
}
/****************************************************************************/
/**
 * \brief 	configure I frame
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_CFR_I_FRAME()
{
    HW_BDU_WAIT(BDU_VC1_HDR_CFG)=I_FRAME;
}
/****************************************************************************/
/**
 * \brief 	configure P frame first phase
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_CFR_P_FRAME(t_vc1_mv_mode mv_mode,t_bdu_vc1_hdr_skip_mode is_skipped_mb_raw, t_bdu_vc1_hdr_mv_mode is_mv_raw,t_vc1_mv_range mv_range,t_bdu_vc1_hdr_quant_mode quant_mode,t_bdu_vc1_hdr_vste vste)
{
    HW_BDU_WAIT(BDU_VC1_HDR_CFG)=P_FRAME| 
        (mv_mode << 2) |
        (is_skipped_mb_raw << 8)|
        (is_mv_raw << 10)|
        (mv_range << 11)|
        (quant_mode << 13)|
        (vste << 15);
}
/****************************************************************************/
/**
 * \brief 	configure P frame (with mv_type in bitplane mode)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_CFR_P_FRAME_MV_BITPLANE(t_vc1_mv_mode mv_mode,t_bdu_vc1_hdr_mv mv_type,t_bdu_vc1_hdr_skip_mode is_skipped_mb_raw, t_bdu_vc1_hdr_mv_mode is_mv_raw,t_vc1_mv_range mv_range,t_bdu_vc1_hdr_quant_mode quant_mode,t_bdu_vc1_hdr_vste vste)
{
    HW_BDU_WAIT(BDU_VC1_HDR_CFG)=P_FRAME| 
        (mv_mode << 2) |
        (mv_type << 7)|
        (is_skipped_mb_raw << 8)|
        (is_mv_raw << 10)|
        (mv_range << 11)|
        (quant_mode << 13)|
        (vste << 15);
}
/****************************************************************************/
/**
 * \brief 	configure P frame (with skipped in bitplane mode)
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_CFR_P_FRAME_SKIP_BITPLANE(t_vc1_mv_mode mv_mode,t_bdu_vc1_hdr_mv mv_type,t_bdu_vc1_hdr_mb_skipped is_mb_skipped,t_bdu_vc1_hdr_skip_mode is_skipped_mb_raw,t_bdu_vc1_hdr_mv_mode is_mv_raw,t_vc1_mv_range mv_range,t_bdu_vc1_hdr_quant_mode quant_mode,t_bdu_vc1_hdr_vste vste)
{
    HW_BDU_WAIT(BDU_VC1_HDR_CFG)=P_FRAME|
        (mv_mode << 2) |
        (is_mb_skipped<<5)|
        (mv_type << 7)|
        (is_skipped_mb_raw << 8)|
        (is_mv_raw << 10)|
        (mv_range << 11)|
        (quant_mode << 13)|
        (vste << 15);
}

/****************************************************************************/
/**
 * \brief 	configure B frame without bitplane
 * \author 	Aroua BEN DARYOUG
 * \param
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_CFR_B_FRAME(t_vc1_mv_mode mv_mode,t_bdu_vc1_hdr_b_fraction b_fraction,t_bdu_vc1_hdr_skip_mode is_skipped_mb_raw,t_bdu_vc1_hdr_direct_mode is_direct_raw ,t_vc1_mv_range mv_range,t_bdu_vc1_hdr_quant_mode quant_mode,t_bdu_vc1_hdr_vste vste)
{
    HW_BDU_WAIT(BDU_VC1_HDR_CFG)=B_FRAME|
        (mv_mode << 2) |
        (b_fraction << 4) |
        (is_skipped_mb_raw << 8)|
        (is_direct_raw << 9)|
        (mv_range << 11)|
        (quant_mode << 13)|
        (vste << 15);
}

/****************************************************************************/
/**
 * \brief 	configure B frame without bitplane
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_CFR_B_FRAME_DIRECT_BITPLANE(t_vc1_mv_mode mv_mode,t_bdu_vc1_hdr_b_fraction b_fraction,t_bdu_vc1_hdr_mb_direct is_mb_direct,t_bdu_vc1_hdr_skip_mode is_skipped_mb_raw,t_bdu_vc1_hdr_direct_mode is_direct_raw ,t_vc1_mv_range mv_range,t_bdu_vc1_hdr_quant_mode quant_mode,t_bdu_vc1_hdr_vste vste)
{
    HW_BDU_WAIT(BDU_VC1_HDR_CFG)=B_FRAME| 
        (mv_mode << 2) |
        (b_fraction << 4) |
        (is_mb_direct << 6)|
        (is_skipped_mb_raw << 8)|
        (is_direct_raw << 9)|
        (mv_range << 11)|
        (quant_mode << 13)|
        (vste << 15);
}
/****************************************************************************/
/**
 * \brief 	configure B frame without bitplane
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_CFR_B_FRAME_SKIP_BITPLANE(t_vc1_mv_mode mv_mode,t_bdu_vc1_hdr_b_fraction b_fraction,t_bdu_vc1_hdr_mb_skipped is_mb_skipped,t_bdu_vc1_hdr_mb_direct is_mb_direct,t_bdu_vc1_hdr_skip_mode is_skipped_mb_raw,t_bdu_vc1_hdr_direct_mode is_direct_raw ,t_vc1_mv_range mv_range,t_bdu_vc1_hdr_quant_mode quant_mode,t_bdu_vc1_hdr_vste vste)
{
    HW_BDU_WAIT(BDU_VC1_HDR_CFG)=B_FRAME| 
        (mv_mode << 2) |
        (b_fraction << 4) |
        (is_mb_skipped<<5)|
        (is_mb_direct << 6)|
        (is_skipped_mb_raw << 8)|
        (is_direct_raw << 9)|
        (mv_range << 11)|
        (quant_mode << 13)|
        (vste << 15);
}


/****************************************************************************/
/**
 * \brief 	set quantification parameters
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_SET_QUANT_PARAMS(t_uint8 pquant, t_uint8 altpquant, t_uint8 half_step)
{
    HW_BDU_WAIT(BDU_VC1_HDR_MBQUANT) = pquant|
        (altpquant << 5)|
        (half_step << 10);
}
/****************************************************************************/
/**
 * \brief 	set tables used for parsing header
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_SET_HDR_TABLES(t_bdu_vc1_hdr_cbpcy_table cbpcy_table ,t_bdu_vc1_hdr_mv_table mv_table,t_bdu_vc1_hdr_ttmb_table ttmb_table )
{
    HW_BDU_WAIT(BDU_VC1_HDR_TABLES) = cbpcy_table|
        (mv_table << 2)|
        (ttmb_table << 4);
}
/****************************************************************************/
/**
 * \brief 	get CBPCY
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_VC1_GET_CBPCY()
{
    return HW_BDU_WAIT(BDU_VC1_HDR_CBPCY);
}

/****************************************************************************/
/**
 * \brief 	get QUANTOUT params
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_VC1_GET_QUANT_PARAMS()
{
    return HW_BDU_WAIT(BDU_VC1_HDR_QUANTOUT);
}
/****************************************************************************/
/**
 * \brief 	get CBPCY
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_VC1_GET_INTRA_MVFLAGS( )
{
    return HW_BDU_WAIT(BDU_VC1_HDR_MVFLAGS);
}
/****************************************************************************/
/**
 * \brief 	get MVDX1
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_VC1_GET_MVDXn(t_uint16 blk_id)
{
    return HW_BDU_WAIT(BDU_VC1_HDR_MVDX1+blk_id);
}

/****************************************************************************/
/**
 * \brief 	get MVDY1
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_VC1_GET_MVDYn(t_uint16 blk_id )
{
    return HW_BDU_WAIT(BDU_VC1_HDR_MVDY1+blk_id);
}


/****************************************************************************/
/**
 * \brief 	start MB header parsing
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_HDR_START_DECODE()
{
    HW_BDU_WAIT(BDU_VC1_HDR_START)=1;
}
/****************************************************************************/
/**
 * \brief 	start MB header parsing
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static void BDU_VC1_HDR_START_DECODE_4MV(t_uint16 acpred)
{
    HW_BDU_WAIT(BDU_VC1_HDR_START)=1 | (acpred <<1);
}
/****************************************************************************/
/**
 * \brief 	get MB header parsing status
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_VC1_HDR_GET_STS()
{
    return HW_BDU_WAIT(BDU_VC1_HDR_GET_STATUS);
}
/****************************************************************************/
/**
 * \brief 	get CBPCY
 * \author 	Aroua BEN DARYOUG
 * \param 	
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_VC1_HDR_GET_CBPCY()
{
    return HW_BDU_WAIT(BDU_VC1_HDR_CBPCY);
}

/****************************************************************************/
/**
 * \brief  BDU command SearchStartCodeAny
 * \author Victor Macela
 * \return Type of the found resynchronization point
 *
 * This function implements the SearchStartCodeAny BDU command which allows
 * to search for a start code prefix or a any possible resynchronization
 * marker in a Simple Profile bitstream.
 * The resynchronization point type can be as follows:
 * - 0: 17-bit resynchronization marker,
 * - 1: 18-bit resynchronization marker,
 * - 2: 19-bit resynchronization marker,
 * - 3: 20-bit resynchronization marker,
 * - 4: 21-bit resynchronization marker,
 * - 5: 22-bit resynchronization marker,
 * - 6: 23-bit resynchronization marker,
 * - 7: 24-bit start code prefix.
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_SEARCH_RESYNC_POINT()
{
    return HW_BDU_WAIT(BDU_SCS_ANY);
}

/****************************************************************************/
/**
 * \brief 	transfer a block of intra coeff from the BDU to the PXP
 * \author 	jean-marc volle
 * \return 	 error code
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_TX_PXP_INTRA()
{
  /*return HW_BDU_WAIT(BDU_INTRA);*/
  HW_BDU_WAIT(BDU_TCOEF_START) = 0x3;
  return HW_BDU_WAIT(BDU_TCOEF_GET_STATUS);
}

/****************************************************************************/
/**
 * \brief 	transfer a block of inter coeff from the BDU to the PXP
 * \author 	jean-marc volle
 * \return 	 error code
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_TX_PXP_INTER()
{
  /*return HW_BDU_WAIT(BDU_INTER);*/
  HW_BDU_WAIT(BDU_TCOEF_START) = 0x1;
  return HW_BDU_WAIT(BDU_TCOEF_GET_STATUS);
}

/****************************************************************************/
/**
 * \brief 	transfer a block of intra coeff from the BDU to the PXP
 * \author 	jean-marc volle
 * \return 	 error code
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_TX_PXP_INTRA_64()
{
  /*    return HW_BDU_WAIT(BDU_INTRA64);*/
  HW_BDU_WAIT(BDU_TCOEF_START) = 0x5;
  return HW_BDU_WAIT(BDU_TCOEF_GET_STATUS);
}

/****************************************************************************/
/**
 * \brief  BDU command StartTcoef
 * \author Victor Macela
 *
 * This function implements the StartTcoef BDU command which allows,
 * depending on the argument:
 * - to forward decode the VLC or RVLC coded coefficients of an inter block
 *   (up to 64 coefficients),
 * - to forward decode the VLC or RVLC coded AC coefficients of an intra block
 *   (up to 63 coefficients, the overall number of decoded coefficients
 *   is initialized to 1 inside BDU to take account of the DC coefficient
 *   decoded apart with specific VLC tables),
 * - to forward decode the VLC or RVLC coded coefficients of an intra block
 *   (up to 64 coefficients),
 * - to backward decode the RVLC coded coefficients of an inter block
 *   (up to 64 coefficients),
 * - to backward decode the RVLC coded AC coefficients of an intra block
 *   (up to 63 coefficients, the overall number of decoded coefficients
 *   is initialized to 1 inside BDU to take account of the DC coefficient
 *   decoded apart with specific VLC tables),
 * - to backward decode the RVLC coded coefficients of an intra block
 *   (up to 64 coefficients).
 * When forward decoding, the obtained last-position-level triplets
 * are transferred to the next VCU unit (IQ or IPRD) according
 * to the DFI unit programming.
 * When backward decoding, the decoded data are not transmitted
 * to any other VCU unit.
 * Any error that may occur results in BDU aborting the decoding
 * and updating he error status accordingly.
 */
/****************************************************************************/
#pragma inline
static void BDU_START_TCOEF(t_bdu_tcoef tcoef)
{
    HW_BDU_WAIT(BDU_TCOEF_START) = tcoef;
}

/****************************************************************************/
/**
 * \brief  BDU command GetTcoefStatus
 * \author Victor Macela
 * \return Error status
 *
 * This function implements the GetTcoefStatus BDU command which allows
 * the error status to be reported after a StartTcoef command has been issued.
 * The error status can be as follows:
 * - no error detected during decoding,
 *   error status = 0,
 * - syntax error (VLC or RVLC table not hit or RVLC table with its codes
 *   reversed not hit or error in ESCAPE code),
 *   error status = 1,
 * - more than 64 (65 when backward decoding) decoded coefficients since
 *   the StartTcoef command has been issued,
 *   error status = 2,
 * - 64 (65 when backward decoding) decoded coefficients since
 *   the StartTcoef command has been issued and last coefficient
 *   of the block still not decoded,
 *   error status = 4,
 * - when backward decoding, the last part of the last-run-level event
 *   corresponding to the first decoded coefficient does not indicate
 *   that this coefficient is the last one of a block (as it should be
 *   because of the backward decoding direction),
 *   error status = 8.
 */
/****************************************************************************/
#pragma inline
static t_uint16 GET_TCOEF_STATUS()
{
    return HW_BDU_WAIT(BDU_TCOEF_GET_STATUS);
}

/****************************************************************************/
/**
 * \brief  BDU command Stop
 * \author Victor Macela
 *
 * This function implements the Stop BDU command which allows to reinitialize
 * properly the Barrel Shifter.
 * Inside BDU unit, only the FSM of the Barrel Shifter is reset.
 * As a result, the Barrel Shifter FIFO (three 32-bit registers) is reset.
 */
/****************************************************************************/
#pragma inline
static void BDU_STOP_CMD()
{
    HW_BDU_WAIT(BDU_STOP) = 0;
}

/****************************************************************************/
/**
 * \brief  BDU command Reverse
 * \author Victor Macela
 *
 * This function implements the Reverse BDU command which allows to switch
 * between the forward and backward decoding directions.
 * A BDU reinitialization sets the decoding direction to forward, which means
 * that the 32-bit words read from the STBus Plug FIFO are not bit swapped
 * and the current bitstream pointer accumulation mode is set to addition.
 * Then, each time the BDU_REV register is written (whatever the value),
 * the decoding direction is switched, which implies that the bit swap 
 * of the 32-bit words read from the STBus Plug FIFO as well as the current
 * bitstream pointer accumulation mode (addition or subtraction)
 * are programmed accordingly.
 */
/****************************************************************************/
#pragma inline
static void BDU_REV_CMD(void)
{
    HW_BDU_WAIT(BDU_REV) = 0;
}

/****************************************************************************/
/**
 * \brief  BDU command MP4_Get_MVD
 * \author Victor Macela
 * \return Motion vector difference or error code
 *
 * This function implements the MP4_Get_MVD BDU command which allows to decode
 * an horizontal or vertical motion vector difference according to table B-12
 * of the standard.
 * Error code 0x8000 is returned when no match has been found.
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_GET_MVD(void)
{
    return HW_BDU_WAIT(BDU_MP4_GET_MVD);
}

/****************************************************************************/
/**
 * \brief 	Read a H264 tcoeff, trailing ones
 * \author 	philippe Rochette
 * \return 	 value read from table
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_H264_TABLE_NCOEFF(t_sint16 value)
{
    return HW_BDU_WAIT(BDU_H264_CT_NC0+value);
}


/****************************************************************************/
/**
 * \brief 	Read a H264 tcoeff, trailing ones
 * \author 	philippe Rochette
 * \return 	 value read from table
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_H264_TABLE_PREFIX()
{
    return HW_BDU_WAIT(BDU_H264_PREFIX);
}


/****************************************************************************/
/**
 * \brief 	Read a H264 tcoeff, trailing ones
 * \author 	philippe Rochette
 * \return 	 value read from table
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_H264_TABLE_TOTAL_ZERO(t_uint16 value)
{
    return HW_BDU_WAIT(BDU_H264_TZ_TC1-1+value);
}


/****************************************************************************/
/**
 * \brief 	Read a H264 tcoeff, trailing ones
 * \author 	philippe Rochette
 * \return 	 value read from table
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_H264_TABLE_TOTAL_ZERO_DC(t_uint16 value)
{
    return HW_BDU_WAIT(BDU_H264_TZC_TC1-1+value);
}


/****************************************************************************/
/**
 * \brief 	Read a H264 tcoeff, trailing ones
 * \author 	philippe Rochette
 * \return 	 value read from table
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_H264_TABLE_TOTAL_RUN_BEFORE(t_uint16 value)
{
    return HW_BDU_WAIT(BDU_H264_RUN_BEF_ZL1+value-1);
}
/****************************************************************************/
/**
 * \brief 	Write H264 total coeff
 * \author 	Cyril Enault
 * \return 	none 
 */
/****************************************************************************/
#pragma inline 
static void BDU_SET_H264_TOTAL_COEFF(t_uint16 value)
{
    HW_BDU_WAIT(BDU_CAVLC_TOTAL_COEFF) = value;
}
/****************************************************************************/
/**
 * \brief 	Write H264 trailing ones
 * \author 	Cyril Enault
 * \return 	none 
 */
/****************************************************************************/
#pragma inline 
static void BDU_SET_H264_TRAILING_ONES(t_uint16 value)
{
    HW_BDU_WAIT(BDU_CAVLC_TRAILING_ONES) = value;
}
/****************************************************************************/
/**
 * \brief 	SET H264 CAVLC mode
 * \author 	Cyril Enault
 * \return 	none 
 */
/****************************************************************************/
#pragma inline 
static void BDU_SET_H264_CAVLC_MODE(t_cavlc_mode mode )
{
    HW_BDU_WAIT(BDU_CAVLC_MODE) = mode;
}
/****************************************************************************/
/**
 * \brief 	Start H264 CAVLC 
 * \author 	Cyril Enault
 * \return 	none 
 */
/****************************************************************************/
#pragma inline 
static void BDU_START_H264_CAVLC()
{
    HW_BDU_WAIT(BDU_CAVLC_START) = 1; 
}
/****************************************************************************/
/**
 * \brief 	Get H264 CAVLC
 * \author 	Cyril Enault
 * \return 	return 0 if OK, other values in other case.
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_H264_CAVLC(void)
{
    return HW_BDU_WAIT(BDU_CAVLC_STA);
}
/****************************************************************************/
/**
 * \brief 	Write H264 GOLOMB UE
 * \author 	Cyril Enault
 * \return 	EXP-GOLOMB UE 
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_GOLOMB_UE(void)
{
    return HW_BDU_WAIT(BDU_EXPGOL0);
}
/****************************************************************************/
/**
 * \brief 	Write H264 GOLOMB TE
 * \author 	Cyril Enault
 * \return 	EXP-GOLOMB TE
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_GOLOMB_TE(void)
{
    return HW_BDU_WAIT(BDU_EXPGOL1);
}
/****************************************************************************/
/**
 * \brief 	Write H264 GOLOMB SE
 * \author 	Cyril Enault
 * \return 	EXP-GOLOMB SE
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_GOLOMB_SE(void)
{
    return HW_BDU_WAIT(BDU_EXPGOL2);
}
/****************************************************************************/
/**
 * \brief 	Write H264 GOLOMB UE
 * \author 	Cyril Enault
 * \return 	EXP-GOLOMB ME_INTRA4X4
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_GOLOMB_ME_INTRA4X4(void)
{
    return HW_BDU_WAIT(BDU_EXPGOL3);
}
/****************************************************************************/
/**
 * \brief 	Write H264 GOLOMB UE
 * \author 	Cyril Enault
 * \return 	EXP-GOLOMB ME_INTER
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_GOLOMB_ME_INTER(void)
{
    return HW_BDU_WAIT(BDU_EXPGOL4);
}
/****************************************************************************/
/**
 * \brief 	Get H264 error status
 * \author 	Cyril Enault
 * \return 	0 if OK, 
 *          1 error in anti-emulation byte removal,
 *          2 ExpGolomb : no prefix found.
 *          3 ExpGolomb : bad index for me(v) tables
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_H264_ERROR(void)
{
    return HW_BDU_WAIT(BDU_HERR);
}
/****************************************************************************/
/**
 * \brief 	Write H264 IPCM
 * \author 	Cyril Enault
 * \return 	
 */
/****************************************************************************/
#pragma inline 
static void BDU_H264_IPCM_START(void) 
{
     HW_BDU_WAIT(BDU_IPCM_START) = H264_IPCM_LUMA;
}
/****************************************************************************/
/**
 * \brief 	Write H264 IPCM Luma
 * \author 	Cyril Enault
 * \return 	
 */
/****************************************************************************/
/* Will be removed for next BDU release */
#pragma inline 
static void BDU_H264_IPCM_START_LUMA(void) 
{
     HW_BDU_WAIT(BDU_IPCM_START) = H264_IPCM_LUMA;
}
/****************************************************************************/
/**
 * \brief 	Write H264 IPCM Chroma
 * \author 	Cyril Enault
 * \return 	
 */
/****************************************************************************/
/* Will be removed for next BDU release */
#pragma inline 
static void BDU_H264_IPCM_START_CHROMA(void) 
{
     HW_BDU_WAIT(BDU_IPCM_START) = H264_IPCM_CHROMA;
}
/****************************************************************************/
/**
 * \brief 	Read H264 IPCM status
 * \author 	Cyril Enault
 * \return 	
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_H264_IPCM_GET_STATUS(void)
{
    return HW_BDU_WAIT(BDU_IPCM_GET_STATUS);
}
/****************************************************************************/
/**
 * \brief 	Set bitstream size in bits
 * \author 	Cyril Enault
 * \param 	bitstream_size : nb of bits in bitstream
 * \return 	
 */
/****************************************************************************/
#pragma inline 
static void BDU_SET_BITSTREAM_SIZE(t_uint32 bitstream_size)
{
    HW_BDU_WAIT(BDU_BTS_H) = MS_SHORT(bitstream_size);
    HW_BDU_WAIT(BDU_BTS_L) = LS_SHORT(bitstream_size);
}
/****************************************************************************/
/**
 * \brief 	Read H264 MoreData status
 * \author 	Cyril Enault
 * \return 	0 if end of bitstream has been reached, 1 in the other case.
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_MOREDATA(void)
{
    return HW_BDU_WAIT(BDU_MOREDATA);
}
static t_uint16 BDU_GET_HED_MOREDATA(void)
{
    return (BDU_GET_BITS_16(HED_MBT_SZ)!= HED_MBT_LAST);
}

/****************************************************************************/
/**
 * \brief 	Read H264 MB type
 * \author 	Cyril Enault
 * \return 	Macroblock type of H264 current MB.
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_MBTYPE(void)
{
    return BDU_GET_BITS_16(HED_MBT_SZ);
}
/****************************************************************************/
/**
 * \brief 	Read H264 Transform size 8x8 flag
 * \author 	Cyril Enault
 * \return 	0 if luma inverse transform is 4x4, 1 if 8x8.
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_TS8X8(void)
{
    t_uint16 value;
    value = BDU_GET_BITS_16(HED_TS8X8_SZ);
    return value;
}
/****************************************************************************/
/**
 * \brief 	Read H264 intra chroma pred mode
 * \author 	Cyril Enault
 * \return 	return intra_chroma_pred_mode.
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_ICP(void)
{
    t_uint16 value;
    value = BDU_GET_BITS_16(HED_ICP_SZ);
    return value;
}
/****************************************************************************/
/**
 * \brief 	Read H264 intra chroma pred mode
 * \author 	Cyril Enault
 * \return 	return intra_chroma_pred_mode.
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_CBP(void)
{
    t_uint16 value;
    value = BDU_GET_BITS_16(HED_CBP_SZ);
    return value;
}
/****************************************************************************/
/**
 * \brief 	Get Length (number of 32 bits words) for current MB
 * \author 	Cyril Enault
 * \return 	return Length.
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_LENGTH(void)
{
    t_uint16 value;
    value = BDU_GET_BITS_16(HED_LENGTH_SZ);
    return value;
}
/****************************************************************************/
/**
 * \brief 	Get Length (number of 32 bits words) for current MB
 * \author 	Cyril Enault
 * \return 	return mb_skip_run.
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_MB_SKIP_RUN(void)
{
    t_uint16 value;
    value = BDU_GET_BITS_16(HED_MB_SKIP_RUN_SZ);
    return value;
}
/****************************************************************************/
/**
 * \brief 	Get Length (number of 32 bits words) for current MB
 * \author 	Cyril Enault
 * \return 	return mb qp delta.
 */
/****************************************************************************/
#pragma inline 
static t_sint16 BDU_GET_MB_QP_DELTA(void)
{
    t_sint16 value;
    value = BDU_GET_BITS_16(HED_MB_QP_DELTA_SZ);
    value = wextract(value,0x800); // get 8 lsb with sign extension
    return value;
}
/****************************************************************************/
/**
 * \brief 	Get Field flag
 * \author 	Cyril Enault
 * \return 	return 0 (as only frame / no field).
 */
/****************************************************************************/
#pragma inline 
static t_sint16 BDU_GET_MB_FI(void)
{
    t_sint16 value;
    value = BDU_GET_BITS_16(HED_FI_SZ);
    return value;
}
/****************************************************************************/
/**
 * \brief 	Get H264 I4X4 pred
 * \author 	Cyril Enault
 * \return 	return I4X4_X[3:0] I4X4_X[0] = prev_flag for block X
 *                             I4X4_X[3:1] = rem[2:0].
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_I4X4(void)
{
    t_uint16 value;
    value = BDU_GET_BITS_16(HED_I4X4_SZ);
    return value;
}
/****************************************************************************/
/**
 * \brief 	Get H264 sub_mb_type
 * \author 	Cyril Enault
 * \return 	return sub_mb_type (8x8, 8x4, 4x8, 4x4).
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_SUB_MBT(void)
{
    t_uint16 value;
    value = BDU_GET_BITS_16(HED_SUB_MBT_SZ);
    return value;
}
/****************************************************************************/
/**
 * \brief 	Get H264 refidx
 * \author 	Cyril Enault
 * \return 	return refidx.
 */
/****************************************************************************/
#pragma inline 
static t_uint16 BDU_GET_REFIDX(void)
{
    t_uint16 value;
    value = BDU_GET_BITS_16(HED_REFIDX_SZ);
    return value;
}

/****************************************************************************/
/**
 * \brief 	Get H264 MV from bins 
 * \author 	Cyril Enault
 * \return 	return refidx.
 */
/****************************************************************************/
#pragma inline
static t_sint16 BDU_GET_UEG_MVD()
{
#if 1
    return HW_BDU_WAIT(BDU_H264_UEG_MVD);
#else
    t_uint16 Nbits,suffix_val=0;
    t_sint16 value;
    //printf("Start : SHOWBITS 0x%.6lX POS %lX\n",BDU_SHOW_BITS_32(24),BDU_GET_CURRENT_BIT_POS());
    if(BDU_SHOW_BITS_16(9) == 0x1FF) {
        /* Prefix detected :
         * => Count number (Nbits) of '1' bits after prefix 
         * => Read (Nbits) value after these '1' bits followed by one '0'
         * => Return value = 14 + 8*(2^Nbits) + suffix_val(N)
         */
         BDU_SHIFT_BITS(9);

        /* Count number of '1' bits after prefix */
    //printf("prefix done : SHOWBITS 0x%.6lX POS %lX\n",BDU_SHOW_BITS_32(24),BDU_GET_CURRENT_BIT_POS());
        Nbits = 1;
        while (BDU_SHOW_BITS_16(Nbits)&1)
        {
          Nbits++;
        }
          //printf("Shift %d bits\n",Nbits);
          BDU_SHIFT_BITS(Nbits);
        Nbits-=1;
        
        /* Case of error */

        //printf("Shift UEG RES 14-bit prefix\n");
        //printf("Consecutive bits N = %d\n", Nbits);

        /* Read bits for suffix value */
    //printf(" suffix done : SHOWBITS 0x%.6lX POS %lX\n",BDU_SHOW_BITS_32(24),BDU_GET_CURRENT_BIT_POS());
        {
          suffix_val = BDU_GET_BITS_16(Nbits+3);
          //printf("Shift %d bits\n",Nbits);
        }

        //printf("Suffix value on %d bits = %d (0x%x)\n", Nbits, suffix_val, suffix_val);

        /* Compute MVD value */
        value = 9 + 8*((1<<Nbits)-1) + suffix_val;

    } else {
        /* Prefix not detected :
         * => Count number of '1' bits (Nbits)
         * => Return Nbits result between 0 and 8
         */

        Nbits = 1;
        while (BDU_SHOW_BITS_16(Nbits)&1)
        {
          Nbits++;
        }
        BDU_SHIFT_BITS(Nbits);
        Nbits-=1;
        //printf("Consecutive bits N = %d\n", Nbits);

        /* Case of error */

        /* MVD value = Number of consecutive '1' bit + 1 if no prefix found */
        value = Nbits;
    }

    //printf("Sign : SHOWBITS 0x%.6lX POS %lX\n",BDU_SHOW_BITS_32(24),BDU_GET_CURRENT_BIT_POS());
    /* Read one more bit for sign flag */
    if (value)
    {
        if(BDU_GET_BITS_16(1)==1) {

            //printf("Coeff sign flag = 1 (Negative)\n");

            value = value * (t_sint16)-1;        
        }
    }

    //printf("UEG RES value = %d \n", (t_sint16)value);

    /* Return 0 (OK) */
    return value;
#endif
}

/****************************************************************************/
/**
 * \brief 	Get H264 residual levels from bins 
 * \author 	Cyril Enault
 * \return 	return refidx.
 */
/****************************************************************************/
static t_uint16 BDU_GET_UEG_RES_LEVEL(t_sint16 *value)
{
#if 1
    *value = HW_BDU_WAIT(BDU_H264_UEG_RES);
    return BDU_GET_H264_ERROR();
#else
    t_uint16 Nbits,suffix_val=0;
    //printf("SHOWBITS 0x%.6lX\n",BDU_SHOW_BITS_32(24));
    if(BDU_SHOW_BITS_16(14) == 0x3FFF) {
        /* Prefix detected :
         * => Count number (Nbits) of '1' bits after prefix 
         * => Read (Nbits) value after these '1' bits followed by one '0'
         * => Return value = 14 + 8*(2^Nbits) + suffix_val(N)
         */
         BDU_SHIFT_BITS(14);

        /* Count number of '1' bits after prefix */
        Nbits = 1;
        while (BDU_SHOW_BITS_16(Nbits)&1)
        {
          Nbits++;
        }
        if (Nbits)
        {
          //printf("Shift %d bits\n",Nbits);
          BDU_SHIFT_BITS(Nbits);
        }
        Nbits-=1;
        
        /* Case of error */
        if(Nbits==-1) { return 1; }

        //printf("Shift UEG RES 14-bit prefix\n");
        //printf("Consecutive bits N = %d\n", Nbits);

        /* Read bits for suffix value */
        if (Nbits)
        {
          suffix_val = BDU_GET_BITS_16(Nbits);
          //printf("Shift %d bits\n",Nbits);
        }

        //printf("Suffix value on %d bits = %d (0x%x)\n", Nbits, suffix_val, suffix_val);

        /* Compute MVD value */
        *value = 14 + (1<<Nbits) + suffix_val;

    } else {
        /* Prefix not detected :
         * => Count number of '1' bits (Nbits)
         * => Return Nbits result between 0 and 8
         */

        Nbits = 1;
        while (BDU_SHOW_BITS_16(Nbits)&1)
        {
          Nbits++;
        }
        if (Nbits)
          BDU_SHIFT_BITS(Nbits);
        Nbits-=1;
        //printf("Consecutive bits N = %d\n", Nbits);

        /* Case of error */
        if(Nbits==-1) { return 1; }

        /* MVD value = Number of consecutive '1' bit + 1 if no prefix found */
        *value = Nbits + 1;
    }

    /* Read one more bit for sign flag */
    if(BDU_GET_BITS_16(1)==1) {

        //printf("Coeff sign flag = 1 (Negative)\n");

        *value = *value * (t_sint16)-1;        
    }

    //printf("UEG RES value = %d \n", (t_sint16)*value);

    /* Return 0 (OK) */
    return 0;
#endif
}

/****************************************************************************/
/**
 * \brief 	Start H264 decode residuals for 1 MB (from IB buffer)
 * \author 	Cyril Enault
 * \param 	coding_mode : H264_CAVLC (cavlc to entropy coding mode) / 
 *                        H264_CABAC (cabac to entropy coding mode)
 * \param 	t8x8 : H264_T4X4 (decode on 4x4 blocks) / 
 *                 H264_T8X8 (decode on 8x8 blocks)
 * \param 	i16x16 :  H264_NOT_I16X16 (P/B or INXN) / 
 *                    H264_IS_16X16 (I16X16) 
 * \param 	cbp : coded_block_pattern 
 */
/****************************************************************************/
#pragma inline
static void BDU_H264_MB_RES_START(t_bdu_h264_coding_mode coding_mode,
                                           t_bdu_h264_t8x8        t8x8,
                                           t_bdu_h264_i16x16      i16x16,
                                           t_uint16               cbp)
{
    HW_BDU_WAIT(BDU_H264_MB_START) = ( ( coding_mode      )
                                     | (        t8x8 << 1 ) 
                                     | (      i16x16 << 2 )
                                     | (         cbp << 3 ));
    
}
/****************************************************************************/
/**
 * \brief 	Get H264 status for decode residuals (from IB buffer)
 * \author 	Cyril Enault
 * \return 	status :  
 */
/****************************************************************************/
#pragma inline
static t_uint16 BDU_GET_H264_MB_STA(void)
{
    return HW_BDU_WAIT(BDU_H264_MB_STA);
}
/****************************************************************************/
/**
 * \brief 	Get H264 status for decode residuals (from IB buffer)
 * \author 	Cyril Enault
 * \return 	 Number of decoded coeff for current block 
 *           (-1 indicates end of coeffs) 
 */
/****************************************************************************/
#pragma inline
static t_sint16 BDU_GET_H264_COEFFNB(void)
{
    return HW_BDU_WAIT(BDU_H264_MB_COEFNB);
}
/****************************************************************************/
/**
 * \brief 	start H264 MB header of MB directly sent to CUP
 * \author 	Cyril Enault
 * \param 	  
 */
/****************************************************************************/
#pragma inline
static void BDU_START_H264_HDR(t_uint16 entropy_coding_mode,t_uint16 is_intra, t_uint16 nb_mv)
{
    HW_BDU_WAIT(BDU_H264_HDR_START) = (nb_mv<<2)|
                                      (is_intra<<1)|
                                      entropy_coding_mode;
}
/****************************************************************************/
/**
 * \brief 	Get H264 status MB H264 header
 * \author 	Cyril Enault
 * \return 	  
 */
/****************************************************************************/
#pragma inline
static t_sint16 BDU_GET_H264_HDR_STA(void)
{
    return HW_BDU_WAIT(BDU_H264_HDR_STA);
}





#endif /* _BDU_API_H_ */
