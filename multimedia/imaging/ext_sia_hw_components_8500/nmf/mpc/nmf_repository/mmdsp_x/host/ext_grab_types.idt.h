/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Generated ext_grab_types.idt defined type */
#if !defined(_grab_types_idt)
#define _grab_types_idt


#define GRB_NUM_CLIENTS 3

enum e_grabInfo {
  GRBINF_GRAB_LAUNCHED=0x1,
  GRBINF_GRAB_COMPLETED=0x2,
  GRBINF_GRAB_ABORTED=0x3,
  GRBINF_GRAB_TO_REPROG_BY_MMDSP=0x4,
  GRBINF_GRAB_STOPPED=0x5,
  GRBINF_DEBUG_MODE_ON=0x777,
  GRBINF_DEBUG_MODE_OFF=0x0FF};

enum e_grabError {
  GRBERR_NONE=0x00,
  GRBERR_FIFO_OVERFLOW=0x01,
  GRBERR_RAW_BUF_OVERFLOW=0x02,
  GRBERR_FMT_UNSUPPORTED=0x04,
  GRBERR_PIPE_UNSUPPORTED=0x08,
  GRBERR_WATCHDOG_RELOAD=0x0b,
  GRBERR_GRB_IN_PROGRESS=0x10,
  GRBERR_UNKNOWN=0x20,
  GRBERR_STAGINGFULL=0x40,
  GRBERR_FIFO_STA_NOT_EMPTY=0x77,
  IPP_CD_ERROR_CCP=0x80,
  IPP_CD_ERROR_RAW_DATA_NOT_X8_BYTES=0x100,
  IPP_CD_ERROR_RAW_DATA_OVERFLOW=0x101,
  IPP_CD_ERROR_LUMA_NOT_X8_BYTES=0x200,
  IPP_CD_ERROR_LUMA_OVERFLOW=0x201,
  IPP_CD_ERROR_CHROMA_NOT_X16_BYTES_3BUF=0x400,
  IPP_CD_ERROR_CHROMA_NOT_X8_BYTES_2BUF=0x401,
  IPP_CD_ERROR_CHROMA_OVERFLOW=0x402,
  IPP_CD_CSI2_DPHY_ERROR=0x800,
  IPP_CD_CSI2_PACKET_ERROR=0x1000,
  STBUS_PLUG_ERROR=0x2000,
  ERROR_RECOVERY_FRONT_END=0x4001,
  ERROR_RECOVERY_BACK_END=0x4002};

enum e_IPP_CSI2_DPHY_Error {
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_EOT_SYNC_HS_DL1=0x0001,
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_EOT_SYNC_HS_DL2=0x0002,
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_EOT_SYNC_HS_DL3=0x0004,
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_SOT_HS_DL1=0x0008,
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_SOT_HS_DL2=0x0010,
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_SOT_HS_DL3=0x0020,
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_SOT_SYNC_HS_DL1=0x0040,
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_SOT_SYNC_HS_DL2=0x0080,
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_SOT_SYNC_HS_DL3=0x0100,
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_CONTROL_DL1=0x0200,
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_CONTROL_DL2=0x0400,
  IPP_CSI2_DPHY_ERROR_CSI0_ERR_CONTROL_DL3=0x0800,
  IPP_CSI2_DPHY_ERROR_CSI1_ERR_EOT_SYNC_HS_DL1=0x1000,
  IPP_CSI2_DPHY_ERROR_CSI1_ERR_SOT_HS_DL1=0x2000,
  IPP_CSI2_DPHY_ERROR_CSI1_ERR_SOT_SYNC_HS_DL1=0x4000,
  IPP_CSI2_DPHY_ERROR_CSI1_ERR_CONTROL_DL1=0x8000U};

enum e_IPP_CSI2_PACKET_Error {
  IPP_CSI2_PACKET_ERROR_CSI2_PACKET_PAYLOAD_CRC_ERROR=0x0001,
  IPP_CSI2_PACKET_ERROR_CSI2_PACKET_HEADER_ECC_ERROR=0x0002};

enum e_grabDebug {
  GRBDBG_REGVALUE=0x1,
  GRBDBG_EXECUTE_METHOD=0x2,
  GRBDBG_ABORT_METHOD=0x3,
  GRBDBG_SUBSCRIBE_METHOD=0x4,
  GRBDBG_UNSUBSCRIBE_METHOD=0x5,
  GRBDBG_GRAB_HANDLE_DMA_EOT_IT_CALLED=0x6,
  GRBDBG_GRAB_HANDLE_IPP_ERROR_IT_CALLED=0x7,
  GRBDBG_GRAB_BMS_FRAME_IT_CALLED=0x8,
  GRBDBG_STBUS_PLUG_ERROR_IT_CALLED=0x9,
  GRBDBG_SET_FRAME_SKIP_METHOD=0xa};

enum e_grabPipeID {
  GRBPID_PIPE_LR=0,
  GRBPID_PIPE_HR,
  GRBPID_PIPE_RAW_OUT,
  GRBPID_PIPE_RAW_IN,
  GRBPID_PIPE_CAM};

enum e_grabFormat {
  GRBFMT_YUV422_RASTER_INTERLEAVED,
  GRBFMT_YUV420_RASTER_SEMI_PLANAR,
  GRBFMT_YUV420_RASTER_PLANAR_I420,
  GRBFMT_YUV420_RASTER_PLANAR_YV12,
  GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED,
  GRBFMT_JPEG};

struct s_grabParams {
  enum e_grabFormat output_format;
  t_uint16 x_window_size;
  t_uint16 y_window_size;
  t_uint16 x_window_offset;
  t_uint16 y_window_offset;
  t_uint16 x_frame_size;
  t_uint16 y_frame_size;
  t_uint32 dest_buf_addr;
  t_uint16 buf_id;
  t_uint16 rotation_cfg;
  t_uint8 disable_grab_cache;
  t_uint32 cache_buf_addr;
  t_uint8 enable_stab;
  t_uint32 proj_h_buf_addr;
  t_uint32 proj_v_buf_addr;
  t_uint32 buffer_size;
  t_uint8 frameSkip;
};

#endif
