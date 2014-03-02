/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _VPP_API_H_
#define _VPP_API_H_

/*****************************************************************************/
/*
 * Includes							       
 */
/*****************************************************************************/
#include "t1xhv_types.h"
#include "t1xhv_hamac_xbus_regs.h" /* register definition */
#include <stwdsp.h>

/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get VPP_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_VPP(a) (*(volatile MMIO t_uint16 *)(VPP_BASE+(a)))
#define HW_VPP_WAIT(a) (*(volatile __XIO t_uint16 *)(VPP_BASE+(a)))

/*****************************************************************************/
/*
 * Types							       
 */
/*****************************************************************************/

typedef enum
{
     VPP_STATE_IDLE    = 0, /**< \brief VPP is idle               */
     VPP_STATE_RUNNING = 1, /**< \brief VPP is running            */
     VPP_STATE_FROZEN  = 2, /**< \brief VPP task is frozen        */
     VPP_STATE_RESUMED = 3  /**< \brief VPP task has been resumed */
} t_vpp_state;


typedef enum
{
     VPP_IT_EOT    = 0x1, /**< \brief VPP end of task               */
     VPP_IT_SYNC   = 0x2  /**< \brief VPP sync                      */
} t_vpp_it;


typedef enum
{
     VPP_TASK_CONV_FROM_420    = 0, /**< \brief convert from 420 input     */
     VPP_TASK_CONV_FROM_422    = 1, /**< \brief convert from 422 input     */
     VPP_TASK_H263_INLOOP      = 2, /**< \brief H263 in-loop filter        */
     VPP_TASK_H264_INLOOP      = 3, /**< \brief H264 in-loop filter        */
     VPP_TASK_VC1_INLOOP       = 4, /**< \brief VC1  in-loop filter        */
     VPP_TASK_MPEG4_POST_FLT   = 5, /**< \brief MPEG4 deblocking/deringing */
     VPP_TASK_DIVX6_POST_FLT   = 6, /**< \brief DivX 6 post-filters        */
     VPP_TASK_H264RCDO_INLOOP  = 7  /**< \brief H264 RCDO in loop          */
} t_vpp_task_type;

typedef enum
{
     VPP_LIF_COLUMN    = 0x0, /**< \brief column-wise               */
     VPP_LIF_ROW       = 0x1  /**< \brief row-wise                  */
} t_vpp_luma_in_fmt;

typedef enum
{
     VPP_CIF_MB           = 0x0, /**< \brief 420 or 422 MB             */
     VPP_CIF_RASTER       = 0x1  /**< \brief planar raster             */
} t_vpp_chroma_in_fmt;

typedef enum
{
     VPP_DEBLOCKING_DISABLE    = 0x0, /**< \brief deblocking filter off     */
     VPP_DEBLOCKING_ENABLE     = 0x1  /**< \brief deblocking filter on      */
} t_vpp_deb_filter;
								
typedef enum
{
     VPP_DEB_HOR_THEN_VERT     = 0x0, /**< \brief general case     */
     VPP_DEB_VERT_THEN_HOR     = 0x1  /**< \brief H264 case        */
} t_vpp_deb_pass_order;
								
typedef enum
{
     VPP_DERINGING_DISABLE    = 0x0, /**< \brief deringing filter off     */
     VPP_DERINGING_ENABLE     = 0x3  /**< \brief deringing filter on      */
} t_vpp_der_filter;

typedef enum
{
     VPP_LOF_COLUMN    = 0x0, /**< \brief column-wise               */
     VPP_LOF_ROW       = 0x1  /**< \brief row-wise                  */
} t_vpp_luma_out_fmt;

typedef enum
{
     VPP_COF_MB           = 0x0, /**< \brief MB or interleaved         */
     VPP_COF_RASTER       = 0x1  /**< \brief planar raster             */
} t_vpp_chroma_out_fmt;

typedef enum
{
     VPP_NO_ITLV_LC_OUT    = 0x0, /**< \brief normal                   */
     VPP_ITLV_LC_OUT       = 0x1  /**< \brief 422 interleaved raster   */
} t_vpp_itlv_lc_out;

typedef enum
{
     VPP_CHROMA_UPS_OFF    = 0x0, /**< \brief normal                   */
     VPP_CHROMA_UPS_ON     = 0x1  /**< \brief 420 to 422               */
} t_vpp_chroma_ups;
typedef enum
{
    VPP_PFR_OFF            = 0x0,
    VPP_PFR_ON             = 0x1
}
t_vpp_prefetch_line;
typedef enum
{
    VPP_MCR_OFF            = 0x0,
    VPP_MCR_ON             = 0x1
}
t_vpp_monochrome;




/*****************************************************************************/
/*
 * Exported macros
 */
/*****************************************************************************/

/*****************************************************************************/
/** 
 * \brief   VPP soft reset, same effect as hard reset	
 * \author  Maurizio Colombo
 * \param   none
 * \return  none
 */
/*****************************************************************************/
#pragma inline
static void VPP_RESET()
{
  HW_VPP_WAIT(VPP_RST) = 1;
}

/*****************************************************************************/
/** 
 * \brief   VPP start task, start or resume a task. No effect if VPP already 
 * \brief     running
 * \author  Maurizio Colombo
 * \param   none
 * \return  none
 */
/*****************************************************************************/
#pragma inline
static void VPP_START_TASK()
{
  HW_VPP_WAIT(VPP_ENR) = 1;
}

/*****************************************************************************/
/** 
 * \brief   VPP abort task 
 * \brief   No effect if VPP already idle
 * \author  Maurizio Colombo
 * \param   none
 * \return  none
 */
/*****************************************************************************/
#pragma inline
static void VPP_ABORT_TASK()
{
  HW_VPP_WAIT(VPP_DIR) = 1;
}

/*****************************************************************************/
/** 
 * \brief   VPP read status
 * \brief   VPP state
 * \author  Maurizio Colombo
 * \param   none
 * \return  VPP state
 */
/*****************************************************************************/
#pragma inline
static t_vpp_state VPP_GET_STATUS()
{
  return HW_VPP_WAIT(VPP_STA);
}


/*****************************************************************************/
/** 
 * \brief   VPP read interrupt status
 * \author  Maurizio Colombo
 * \param   none
 * \return  VPP interrupt status: EOT[0], SYNC[1]
 */
/*****************************************************************************/
#pragma inline
static t_uint16 VPP_GET_INTERRUPT_STATUS()
{
  return HW_VPP_WAIT(VPP_ISR);
}


/*****************************************************************************/
/** 
 * \brief   VPP clear interrupt
 * \author  Maurizio Colombo
 * \param   interrupt(s) to be cleared
 * \return  none
 */
/*****************************************************************************/
#pragma inline
static void VPP_CLEAR_INTERRUPT(t_vpp_it it_line)
{
  HW_VPP_WAIT(VPP_ISR) = it_line;
}


/*****************************************************************************/
/** 
 * \brief   VPP frame-level configuration
 * \author  Maurizio Colombo
 * \param   task_type      :   processing to be performed
 * \param   luma_in_fmt    :   luma input format
 * \param   chroma_in_fmt  :   chroma input format
 * \param   deb_filter     :   deblocking enable/disable
 * \param   deb_pass_order :   deblocking filter pass order
 * \param   der_filter     :   deringing filter enable/disable
 * \param   luma_out_fmt   :   luma output format
 * \param   chroma_out_fmt :   chroma output format
 * \param   itlv_lc_out    :   interleave luma/chroma output
 * \param   chroma_ups     :   chroma upsampling enable/disable
 * \param   prefetch_lined :   PRF:pre-fetch line based, display all MB line before starting a new line  
 * \param   monochrome     :   monochrome mode, if set only LUMA inout ports are used
 * \param        :   
 * \return  none
 */
/*****************************************************************************/
#pragma inline
static void VPP_CONFIGURE(t_vpp_task_type type,
						  t_vpp_luma_in_fmt luma_in_fmt,
						  t_vpp_chroma_in_fmt chroma_in_fmt,
						  t_vpp_deb_filter deb_filter,
						  t_vpp_deb_pass_order deb_pass_order,
						  t_vpp_der_filter der_filter,
						  t_vpp_luma_out_fmt luma_out_fmt,
						  t_vpp_chroma_out_fmt chroma_out_fmt,
						  t_vpp_itlv_lc_out itlv_lc_out,
						  t_vpp_chroma_ups chroma_ups,
						  t_vpp_prefetch_line prefetch_lined,
						  t_vpp_monochrome monochrome)
{
  HW_VPP_WAIT(VPP_CFR) = type                    |
	                (luma_in_fmt         << 3    )|
	                (chroma_in_fmt       << 4    )|
                    (deb_filter        << 5    )|
                    (deb_pass_order    << 6    )|
                    (der_filter        << 7    )|
                    (luma_out_fmt      << 9    )|
                    (chroma_out_fmt    <<10    )|
                    (itlv_lc_out       <<11    )|
                    (chroma_ups        <<12    )|
                    (prefetch_lined    <<13    )|
                    (monochrome        <<14    );
}

/*****************************************************************************/
/** 
 * \brief   VPP set source frame size
 * \author  Maurizio Colombo
 * \param   frame width/height in pixels
 * \return  none
 */
/*****************************************************************************/
#pragma inline
static void VPP_SET_SOURCE_FRAME_SIZE(t_uint16 width, t_uint16 height)
{
  HW_VPP_WAIT(VPP_SFW) = width;
  HW_VPP_WAIT(VPP_SFH) = height;
}


/*****************************************************************************/
/** 
 * \brief   VPP set freeze offset
 * \author  Maurizio Colombo
 * \param   freeze offset in pixels
 * \return  none
 */
/*****************************************************************************/
#pragma inline
static void VPP_SET_FREEZE_OFFSET(t_uint16 horizontal_offset, 
								  t_uint16 vertical_offset)
{
  HW_VPP_WAIT(VPP_HFO) = horizontal_offset;
  HW_VPP_WAIT(VPP_VFO) = vertical_offset;
}

/*****************************************************************************/
/** 
 * \brief   VPP set PQUANT
 * \author  Maurizio Colombo
 * \param   PQUANT picture quantization parameter
 * \return  none
 */
/*****************************************************************************/
#pragma inline
static void VPP_SET_PQUANT(t_uint16 pquant)
{
  HW_VPP_WAIT(VPP_QP) = pquant;
}

/*****************************************************************************/
/** 
 * \brief   VPP set sync offset, for SYNC interrupt
 * \author  Maurizio Colombo
 * \param   sync offset in pixels
 * \return  none
 */
/*****************************************************************************/
#pragma inline
static void VPP_SET_SYNC_OFFSET(t_uint16 horizontal_offset, 
								t_uint16 vertical_offset)
{
  HW_VPP_WAIT(VPP_HSO) = horizontal_offset;
  HW_VPP_WAIT(VPP_VSO) = vertical_offset;
}

/*****************************************************************************/
/** 
 * \brief   wait end of VPP by polling
 * \author  Cyril Enault
 * \param   PQUANT picture quantization parameter
 * \return  none
 */
/*****************************************************************************/
#pragma inline
static void VPP_WAIT_END_DBLK(void)
{
  while((HW_VPP_WAIT(VPP_ENR)) != 0)
  {;}
}
/****************************************************************************/
/**
 * \brief 	Acknowledge interrupt
 * \author 	cyril enault
 *  
 * 
 **/
/****************************************************************************/
#pragma inline
static void VPP_ACK_INTERRUPT(void)
{
    HW_VPP_WAIT(VPP_ISR) = 1;
}




#endif /* _VPP_API_H_ */
