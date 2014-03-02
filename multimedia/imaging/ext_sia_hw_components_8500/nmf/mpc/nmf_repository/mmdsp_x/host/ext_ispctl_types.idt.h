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

/* Generated ext_ispctl_types.idt defined type */
#if !defined(_ispctl_type_idt)
#define _ispctl_type_idt


#define ISPCTL_SIZE_TAB_PE 32

#define ISP_SIZE_OF_ITM_REGISTERS 128

#define XP70_NO_ACK_INT 0x0UL

#define XP70_HOST_COMMS_READY 0x1UL

#define XP70_HOST_COMMS_OPERATION_COMPLETE 0x2UL

#define XP70_BOOT_COMPLETE 0x4UL

#define XP70_SLEEPING 0x8UL

#define XP70_WORKEN_UP 0x10UL

#define XP70_ISP_STREAMING 0x20UL

#define XP70_ISP_STOP_STREAMING 0x40UL

#define XP70_SENSOR_STOP_STREAMING 0x80UL

#define XP70_SENSOR_START_STREAMING 0x100UL

#define XP70_ISP_LOAD_READY 0x200UL

#define XP70_ZOOM_CONFIG_REQUEST_DENIED 0x400UL

#define XP70_ZOOM_CONFIG_REPROGRAM_REQUIRED 0x800UL

#define XP70_ZOOM_STEP_COMPLETE 0x1000UL

#define XP70_ZOOM_SET_OUT_OF_RANGE 0x2000UL

#define XP70_STREAMING_ERROR 0x4000UL

#define XP70_ISP_RESET_COMPLETE 0x8000UL

#define XP70_MASTER_I2C_ACCESS_FAILURE 0x10000UL

#define XP70_GLACE_STATS_READY 0x20000UL

#define XP70_HISTOGRAM_STATS_READY 0x40000UL

#define XP70_EXPOSURE_AND_WB_PARAMETERS_UPDATED 0x80000UL

#define XP70_AUTOFOCUS_STATS_READY 0x100000UL

#define XP70_FLADRIVER_LENS_STOP 0x200000UL

#define XP70_ZOOM_OUTPUT_IMAGE_RESOLUTION_READY 0x400000UL

#define XP70_COLOUR_MATRIX_PIPE0_UPDATE_COMPLETE 0x800000UL

#define XP70_COLOUR_MATRIX_PIPE1_UPDATE_COMPLETE 0x1000000UL

#define XP70_SDL_UPDATE_READY 0x2000000UL

#define XP70_HOST_TO_SENSOR_ACCESS_COMPLETE 0x1UL

#define XP70_FLASH_LIT_FRAME 0x1UL

#define CSI2_NULL 0x10

#define CSI2_BLKD 0x11

#define CSI2_EMBD_8BIT 0x12

#define CSI2_RAW6 0x28

#define CSI2_RAW7 0x29

#define CSI2_RAW8 0x2A

#define CSI2_RAW10 0x2B

#define CSI2_RAW12 0x2C

#define CSI2_RAW14 0x2D

typedef enum t_xyuv_t_SensorType_e {
  SensorType_CCP,
  SensorType_CSI} t_SensorType_e;

enum e_CSI2RX_Pixel_Width {
  PIXEL_W_8BIT=0x8,
  PIXEL_W_10BIT=0xA};

struct s_CsiConfig {
  t_uint16 Dphy_Data_Lanes_Map;
  t_uint16 Dphy_Data_Lanes_Number;
  t_uint16 Dphy_clock_Lane_Control_HSRX_Term_Shift_Down_Cl;
  t_uint16 Dphy_clock_Lane_Control_HSRX_Term_Shift_Up_Cl;
  t_uint16 Dphy_clock_Lane_Control_Hs_Invert_Cl;
  t_uint16 Dphy_clock_Lane_Control_Swap_Pin_Cl;
  t_uint16 Dphy_clock_Lane_Control_Mipi_Specs_90_81b;
  t_uint16 Dphy_clock_Lane_Control_Ui_X4;
  t_uint16 Dphy_data_Lane1_Control;
  t_uint16 Dphy_data_Lane2_Control;
  t_uint16 Dphy_data_Lane3_Control;
  t_uint16 Dphy_data_Lane4_Control;
  t_uint16 Dphy_Static_CSI2RX_DataType0;
  t_uint16 Dphy_Static_CSI2RX_DataType1;
  t_uint16 Dphy_Static_CSI2RX_DataType2;
  t_uint16 Dphy_Static_CSI2RX_DataType0_Pixel_Width;
  t_uint16 Dphy_Static_CSI2RX_DataType1_Pixel_Width;
  t_uint16 Dphy_Static_CSI2RX_DataType2_Pixel_Width;
  t_uint16 Dphy_interface_enable;
};

struct s_CcpConfig {
  t_uint16 Ccp_Static_BPP;
  t_uint16 Ccp_Static_Data_Strobe;
};

enum e_ispctlInfo {
  ISP_INF_NONE=0x0,
  ISP_READ_DONE=0x1,
  ISP_WRITE_DONE=0x2,
  ISP_INIT_DONE=0x3,
  ISP_READLIST_DONE=0x4,
  ISP_WRITELIST_DONE=0x5,
  ISP_POLLING_PE_VALUE_DONE=0x6,
  ISP_HOST_COMMS_READY=0x7,
  ISP_BOOT_COMPLETE=0x8,
  ISP_SLEEPING=0x9,
  ISP_WOKEN_UP=0xA,
  ISP_STREAMING=0xB,
  ISP_STOP_STREAMING=0xC,
  ISP_SENSOR_START_STREAMING=0xD,
  ISP_SENSOR_STOP_STREAMING=0xE,
  ISP_HOST_TO_SENSOR_ACCESS_COMPLETE=0xF,
  ISP_LOAD_READY=0x10,
  ISP_ZOOM_CONFIG_REQUEST_DENIED=0x11,
  ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED=0x12,
  ISP_ZOOM_STEP_COMPLETE=0x13,
  ISP_ZOOM_SET_OUT_OF_RANGE=0x14,
  ISP_RESET_COMPLETE=0x15,
  ISP_GLACE_STATS_READY=0x16,
  ISP_HISTOGRAM_STATS_READY=0x17,
  ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED=0x18,
  ISP_AUTOFOCUS_STATS_READY=0x19,
  ISP_FLADRIVER_LENS_STOP=0x20,
  ISP_ZOOM_OUTPUT_IMAGE_RESOLUTION_READY=0x21,
  ISP_COLOUR_MATRIX_PIPE0_UPDATE_COMPLETE=0x22,
  ISP_COLOUR_MATRIX_PIPE1_UPDATE_COMPLETE=0x23,
  ISP_SDL_UPDATE_READY=0x24,
  UNKNOWN_INFO=0x1000};

enum e_ispctlError {
  ISP_NO_ERROR=0x0,
  ISP_POLLING_TIMEOUT_ERROR=0x1,
  ISP_CHECK_VALUE_ERROR=0x2,
  ISP_FORBIDDEN_STATE_TRANSITION_ERROR=0x4,
  ISP_READ_ONLY_ERROR=0x8,
  ISP_READ_ONLY_IN_LIST_ERROR=0x10,
  ISP_WRITELIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR=0x20,
  ISP_READLIST_NOT_DONE_OUT_OF_BOUNDARY_ERROR=0x40,
  ISP_SENSOR_TYPE_NOT_DEFINED_ERROR=0x80,
  ISP_STREAMING_ERROR=0x1000,
  ISP_DOES_NOT_RESPOND_ERROR=0x4000,
  ISP_MASTER_I2C_ACCESS_FAILURE=0x200,
  ISP_ALREADY_IN_PROCESSING_ERROR=0x100,
  ISP_BOOTING_ERROR=0xA,
  ISP_READING_ERROR=0xB,
  ISP_WRITING_ERROR=0xC,
  ISP_POLLING_ERROR=0xD};

enum e_ispctlDebug {
  ISP_DBG_PLACEHOLDER=0x0,
  ISP_DBG_UNHANDLED_IT=0x1,
  ISP_DBG_REGVALUE=0x8,
  ISP_DBG_POLLING_TIMEOUT=0x10,
  ISP_DBG_PANIC=0x20,
  ISP_CHANGE_OVER_STARTED=0x2000,
  ISP_CHANGE_OVER_FINISHED=0x4000};

struct s_PageElement {
  t_uint16 pe_addr;
  t_uint32 pe_data;
};

typedef enum t_xyuv_t_eSignal {
  INIT_STATE_SIG=1,
  ISP_REG_WRITTEN_SIG,
  ISP_REG_READ_SIG,
  ISP_HOST_COMM_READY_SIG} t_eSignal;

typedef enum t_xyuv_t_eStateId {
  ISP_RESET_STATE_ID=0x0,
  ISP_BOOTING_STATE_ID=0x1,
  ISP_POLLING_STATE_ID=0x8,
  ISP_WRITING_STATE_ID=0x10,
  ISP_READING_STATE_ID=0x20} t_eStateId;

typedef struct s_PageElement ts_PageElement;

#endif
