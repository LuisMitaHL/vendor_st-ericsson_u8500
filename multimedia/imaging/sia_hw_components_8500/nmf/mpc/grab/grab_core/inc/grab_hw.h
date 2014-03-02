/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef GRAB_HW_H_
#define GRAB_HW_H_

#include <grab_types.idt.h>
#include "crm_mmdsp.h" /* register definition */
#include "dma_mmdsp.h" /* register definition */
#include "pictor_mmdsp.h" /* register definition */
#include "proj_mmdsp.h" /* register definition */
#include "sia_mmdsp.h" /* register definition */
#include "stbp_mmdsp.h" /* register definition */
#include <stwdsp.h>

#include "ckg_api.h"
#include "sia_api.h"
#include "proj_api.h"
#include "irp_api.h"
#include "dma_api.h"
#include "stbp_sia_api.h"
#include "ipp_api.h"
#include "itc_api.h" /* for ITC_GET_TIMER_32() in IT handlers */

#ifdef SOC8500V1
#define ESRAM_START_ADDRESS_8500V1 (0x40000000UL)
#define ESRAM_SIZE_8500V1          (0x9FFFFUL)
#else
#error "Unknown platform. Need to know platform to know Memory mapping for STBP configuration!"
#endif

#define ESRAM_START_ADDRESS ESRAM_START_ADDRESS_8500V1
#define ESRAM_SIZE          ESRAM_SIZE_8500V1
#define ESRAM_END_ADDRESS (ESRAM_START_ADDRESS+ESRAM_SIZE)

/**************************
 * Defines
 **************************/

/**************************
 * Types
 **************************/
struct s_grab_config {
    t_uint16 pry_format;
    t_uint16 dma_endianness;
    t_uint16 dma_chroma_fmt;
    t_uint16 dma_raster_en;
    t_uint16 dma_interlace_en;
    t_uint32 dma_channel0_start_addr;
    t_uint32 dma_channel1_start_addr;
    t_uint32 dma_channel2_start_addr;
    t_uint32 dma_cache_channel0_start_addr;
    t_uint32 dma_cache_channel1_start_addr;
    t_uint32 dma_v_curve_start_addr;
    t_uint32 dma_h_curve_start_addr;
    t_uint16 dma_frame_width;
    t_uint16 dma_frame_height;
    t_uint16 dma_window_width;
    t_uint16 dma_window_height;
    t_uint16 processes;
    t_uint16 stbp_port_ctrl;
    t_uint16 vcf_enable;
    t_uint16 rotation_cfg;
};

extern struct s_grab_config G_grab_config_HR;
extern struct s_grab_config G_grab_config_LR;
extern struct s_grab_config G_grab_config_BMS;
extern struct s_grab_config G_grab_config_BML;

/*
mParams[0]- GRBPID_PIPE_LR
mParams[1]- GRBPID_PIPE_HR
mParams[2]- GRBPID_PIPE_RAW_OUT
mParams[3] -GRBPID_PIPE_RAW_IN
*/
extern struct s_grabParams mParams[4];

extern int G_isDebug;
extern t_uint16 G_shared_variable_addr;
extern volatile t_uint32 G_share_variable_state_saved;
extern volatile t_uint32 G_share_variable_FrameID_saved;
extern volatile t_uint32 G_share_variable_Indicator_for_Videostab;
extern volatile t_uint16 G_Var_Rendez_Vous_AO_LR;
extern volatile t_uint16 G_Var_Rendez_Vous_AO_HR;
extern volatile t_uint16 G_Var_Rendez_Vous_EOT_LR;
extern volatile t_uint16 G_Var_Rendez_Vous_EOT_HR;
extern volatile t_uint16 G_Var_Process_LR_Ongoing;
extern volatile t_uint16 G_Var_Process_HR_Ongoing;
extern volatile t_uint16 G_Var_adaptive_Overscanned_it_occurred_LR;
extern volatile t_uint16 G_Var_adaptive_Overscanned_it_occurred_HR;
extern volatile t_uint8  G_KeepCounter[5];
extern volatile t_uint8  G_KeepRatio[5];
extern volatile t_uint16 G_sia_hw_revision;

extern volatile t_uint16 G_Adaptive_Overscan_supported;
extern volatile t_uint16 FrameInvalid;
extern volatile t_uint32 DMAPending;

/**************************
 * function prototypes
 **************************/
t_uint16 grab_program_hw(enum e_grabPipeID pipe, struct s_grabParams params);


enum e_grabError grab_prepare_pipe0_config(struct s_grab_config *config, struct s_grabParams *grab_param);
enum e_grabError grab_prepare_pipe1_config(struct s_grab_config *config, struct s_grabParams *grab_param);
enum e_grabError grab_prepare_pipe2o_config(struct s_grab_config *config, struct s_grabParams *grab_param);
enum e_grabError grab_prepare_pipe2i_config(struct s_grab_config *config, struct s_grabParams *grab_param);


void grab_abort_hw(enum e_grabPipeID pipe);
void stbus_plug_init(void);
void mmdsp_restart_grab(enum e_grabPipeID pipe);


/* interrupt handlers */
void grab_handle_ipp_err_it(void);
void grab_handle_dma_eot_it(void);
void Isp_ITM3_handler(void);
void stbus_plug_error_handler(void);

/******************************
 * Inlined functions
 ******************************/

/* DMA processses MASK */
#define PROCESSES_LR (IRP1_PRA_W)
#define PROCESSES_HR (IRP03_PRA_W | IRP03_L_W | IRP03_C1_W | IRP03_C2_W | IRP03_C_W | IRP03_GCF_L_R | IRP03_GCF_L_W | IRP03_GCF_C_R | IRP03_GCF_C_W | IRP03_VCP_W | IRP03_HCP_R | IRP03_HCP_W)
#define PROCESSES_STAB_LR (IRP03_L_W |IRP03_C_W| IRP03_GCF_L_R | IRP03_VCP_W | IRP03_HCP_R | IRP03_HCP_W )
#define PROCESSES_BMS (IRP2_PRA_W)
#define PROCESSES_BML (IRP0_PRA_R)
#define PROCESSES_CAM (IRP3_JPG_W | IRP03_PRA_W | IRP03_L_W | IRP03_C1_W | IRP03_C2_W | IRP03_C_W | IRP03_GCF_L_R | IRP03_GCF_L_W | IRP03_GCF_C_R | IRP03_GCF_C_W | IRP03_VCP_W | IRP03_HCP_R | IRP03_HCP_W)

/**********************************
 * SIA HW VERSION - SIA_IDN_HRV
***********************************/
#define SIA_8820A    0x1
#define SIA_8820B    0x2
#define SIA_8500ED   0x3
#define SIA_8500V1   0x4
#define SIA_8500V2   0x5
#define SIA_5500V1   0x6
#define SIA_5500V2   0x7
#define SIA_9540V1   0x8
#define SIA_9600V1   0x9
#define SIA_9600V2   0xA
#define SIA_8540V1   0xC


#endif /*GRAB_HW_H_*/
