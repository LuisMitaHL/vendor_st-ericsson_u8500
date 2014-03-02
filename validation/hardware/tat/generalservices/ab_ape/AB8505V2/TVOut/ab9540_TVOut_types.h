/****************************************************************************
 * File : c:/Lisp/Production/ab9540/ab9540_TVOut/Linux/ab9540_TVOut_types.h
 * 
 *
 * Generated on the 13/01/2012 09:19 by the 'super-cool' code generator 
 *
 * Generator written in Common Lisp, created by  Remi PRUD'HOMME - STEricsson 
 * with the help of : 
 *
 ***************************************************************************
 *  Copyright STEricsson  2012
 *
 *  Reference document : User Manual ref : CD00291561 Rev 3, May 2011
 *
 ***************************************************************************/

// This file is generated. Don't modify it 

#ifndef C__LISP_PRODUCTION_AB9540_AB9540_TVOUT_LINUX_AB9540_TVOUT_TYPES_H
#define C__LISP_PRODUCTION_AB9540_AB9540_TVOUT_LINUX_AB9540_TVOUT_TYPES_H


#ifdef __cplusplus
extern "C" {
#endif


//  
//  TVOut synchro mode selection.
//  Register TvoutConfiguration0 0x0600, Bits 3:5, typedef tvout_synchro_mode
//   
//  
#define TVOUT_SYNCHRO_MODE_MASK 0x38U
#define TVOUT_SYNCHRO_MODE_OFFSET 5
#define TVOUT_SYNCHRO_MODE_STOP_BIT 5
#define TVOUT_SYNCHRO_MODE_START_BIT 3
#define TVOUT_SYNCHRO_MODE_WIDTH 3

#define TVOUT_CONFIGURATION_0_REG 0x600
#define  TVOUT_SYNCHRO_ODDEV_ONLY 0
#define  TVOUT_SYNCHRO_F 1
#define  TVOUT_SYNCHRO_ODDEV_HSYNC 2
#define  TVOUT_SYNCHRO_VSYNC 3
#define  TVOUT_SYNCHRO_VSYN_HSYNC 4
#define  TVOUT_SYNCHRO_MASTER 5
#define  TVOUT_SYNCHRO_AUTOTEST 6

typedef enum tvout_synchro_mode {
    TVOUT_SYNCHRO_ODDEV_ONLY_E,
    TVOUT_SYNCHRO_F_E,
    TVOUT_SYNCHRO_ODDEV_HSYNC_E,
    TVOUT_SYNCHRO_VSYNC_E,
    TVOUT_SYNCHRO_VSYN_HSYNC_E,
    TVOUT_SYNCHRO_MASTER_E,
    TVOUT_SYNCHRO_AUTOTEST_E
} TVOUT_SYNCHRO_MODE_T ;
#define TVOUT_SYNCHRO_MODE_PARAM_MASK  0x38



//  
//  TVOut standard selection.
//  Register TvoutConfiguration0 0x0600, Bits 6:7, typedef tvout_standard
//   
//  
#define TVOUT_STANDARD_MASK 0xC0U
#define TVOUT_STANDARD_OFFSET 7
#define TVOUT_STANDARD_STOP_BIT 7
#define TVOUT_STANDARD_START_BIT 6
#define TVOUT_STANDARD_WIDTH 2

#define  TVOUT_STD_PAL_BDGHI 0
#define  TVOUT_STD_PAL_N 1
#define  TVOUT_STD_NTSC_M 2
#define  TVOUT_STD_PAL_M 3

typedef enum tvout_standard {
    TVOUT_STD_PAL_BDGHI_E,
    TVOUT_STD_PAL_N_E,
    TVOUT_STD_NTSC_M_E,
    TVOUT_STD_PAL_M_E
} TVOUT_STANDARD_T ;
#define TVOUT_STANDARD_PARAM_MASK  0xC0



//  
//  Max dynamic magnitude allowed on YCrCb inputs for encoding. EAV and SAV words are 
//  replaced by blanking values before being fed to luminance and chrominance 
//  processing..
//  Register TvoutConfiguration6 0x0606, Bits 0:0, typedef max_dyn_enable
//   
//  
#define MAX_DYN_MASK 0x1U
#define MAX_DYN_OFFSET 0
#define MAX_DYN_STOP_BIT 0
#define MAX_DYN_START_BIT 0
#define MAX_DYN_WIDTH 1

#define TVOUT_CONFIGURATION_6_REG 0x606

typedef enum max_dyn_enable {
    MAX_DYN_DISABLE_E,
    MAX_DYN_ENABLE_E
} MAX_DYN_ENABLE_T ;
#define MAX_DYN_PARAM_MASK  0x1



//  
//  00: normal mode (no line skip/insert capability) 
//  ITU-R (CCIR): 313/312 or 263/262 
//  non-interlaced: 312/312 or 262/262 
//  0x1: manual mode for: 
//  line insert ('dec_ninc'=0) or, skip ('dec_ninc'=1) capability. Both fields of all the 
//  frames following the writing of this value 
//  are modified according to 'lref[8:0]' and 'ltarg[8:0]' bits of line_reg1 to line_reg3 
//  registers (by default, 'lref[8:0]'=000000000 and 'ltarg[8:0]'=000000001 which leads 
//  to normal mode above). 
//  100: automatic line insert mode. The 2nd field of the frame following the writing of 
//  this value is increased. Line insertion is done after line 245 in 525/60 and after 
//  line 290 in 625/50. 'lref[8:0]' and 'ltarg[8:0]' are ignored. bit 'jump' is 
//  automatically reset after use. Two lines are skipped (inserted) in 525/60 and four 
//  lines in 625/50 standards. 
//  110: automatic line skip mode. The 2nd field of the frame following the writing of 
//  this value is decreased. Line suppression is done after line 245 in 525/60 and after 
//  line 290 in 625/50. 'lref[8:0]' and 'ltarg[8:0]' are ignored. Bit 'jump' is 
//  automatically reset after use. Two lines are skipped (inserted) in 525/60 and four 
//  lines in 625/50 standards. 
//  1x1: must not be used..
//  Register TvoutConfiguration6 0x0606, Bits 4:4, typedef free_jump_enable
//   
//  
#define FREE_JUMP_MASK 0x10U
#define FREE_JUMP_OFFSET 4
#define FREE_JUMP_STOP_BIT 4
#define FREE_JUMP_START_BIT 4
#define FREE_JUMP_WIDTH 1


typedef enum free_jump_enable {
    FREE_JUMP_DISABLE_E,
    FREE_JUMP_ENABLE_E
} FREE_JUMP_ENABLE_T ;
#define FREE_JUMP_PARAM_MASK  0x10



//  
//  00: normal mode (no line skip/insert capability) 
//  ITU-R (CCIR): 313/312 or 263/262 
//  non-interlaced: 312/312 or 262/262 
//  0x1: manual mode for: 
//  line insert ('dec_ninc'=0) or, skip ('dec_ninc'=1) capability. Both fields of all the 
//  frames following the writing of this value 
//  are modified according to 'lref[8:0]' and 'ltarg[8:0]' bits of line_reg1 to line_reg3 
//  registers (by default, 'lref[8:0]'=000000000 and 'ltarg[8:0]'=000000001 which leads 
//  to normal mode above). 
//  100: automatic line insert mode. The 2nd field of the frame following the writing of 
//  this value is increased. Line insertion is done after line 245 in 525/60 and after 
//  line 290 in 625/50. 'lref[8:0]' and 'ltarg[8:0]' are ignored. bit 'jump' is 
//  automatically reset after use. Two lines are skipped (inserted) in 525/60 and four 
//  lines in 625/50 standards. 
//  110: automatic line skip mode. The 2nd field of the frame following the writing of 
//  this value is decreased. Line suppression is done after line 245 in 525/60 and after 
//  line 290 in 625/50. 'lref[8:0]' and 'ltarg[8:0]' are ignored. Bit 'jump' is 
//  automatically reset after use. Two lines are skipped (inserted) in 525/60 and four 
//  lines in 625/50 standards. 
//  1x1: must not be used..
//  Register TvoutConfiguration6 0x0606, Bits 5:5, typedef dec_ninc_enable
//   
//  
#define DEC_NINC_MASK 0x20U
#define DEC_NINC_OFFSET 5
#define DEC_NINC_STOP_BIT 5
#define DEC_NINC_START_BIT 5
#define DEC_NINC_WIDTH 1


typedef enum dec_ninc_enable {
    DEC_NINC_DISABLE_E,
    DEC_NINC_ENABLE_E
} DEC_NINC_ENABLE_T ;
#define DEC_NINC_PARAM_MASK  0x20



//  
//  00: normal mode (no line skip/insert capability) 
//  ITU-R (CCIR): 313/312 or 263/262 
//  non-interlaced: 312/312 or 262/262 
//  0x1: manual mode for: 
//  line insert ('dec_ninc'=0) or, skip ('dec_ninc'=1) capability. Both fields of all the 
//  frames following the writing of this value 
//  are modified according to 'lref[8:0]' and 'ltarg[8:0]' bits of line_reg1 to line_reg3 
//  registers (by default, 'lref[8:0]'=000000000 and 'ltarg[8:0]'=000000001 which leads 
//  to normal mode above). 
//  100: automatic line insert mode. The 2nd field of the frame following the writing of 
//  this value is increased. Line insertion is done after line 245 in 525/60 and after 
//  line 290 in 625/50. 'lref[8:0]' and 'ltarg[8:0]' are ignored. bit 'jump' is 
//  automatically reset after use. Two lines are skipped (inserted) in 525/60 and four 
//  lines in 625/50 standards. 
//  110: automatic line skip mode. The 2nd field of the frame following the writing of 
//  this value is decreased. Line suppression is done after line 245 in 525/60 and after 
//  line 290 in 625/50. 'lref[8:0]' and 'ltarg[8:0]' are ignored. Bit 'jump' is 
//  automatically reset after use. Two lines are skipped (inserted) in 525/60 and four 
//  lines in 625/50 standards. 
//  1x1: must not be used..
//  Register TvoutConfiguration6 0x0606, Bits 6:6, typedef jump_enable
//   
//  
#define JUMP_MASK 0x40U
#define JUMP_OFFSET 6
#define JUMP_STOP_BIT 6
#define JUMP_START_BIT 6
#define JUMP_WIDTH 1


typedef enum jump_enable {
    JUMP_DISABLE_E,
    JUMP_ENABLE_E
} JUMP_ENABLE_T ;
#define JUMP_PARAM_MASK  0x40



//  
//  Software reset. Automatically reset after internal reset generation. Active during 4 
//  PIX_CLK periods. When softreset is activated, all the device is reset as with 
//  hardware reset 
//  except for the first nine user registers.
//  Register TvoutConfiguration6 0x0606, Bits 7:7, typedef soft_reset
//   
//  
#define SOFT_RESET_MASK 0x80U
#define SOFT_RESET_OFFSET 7
#define SOFT_RESET_STOP_BIT 7
#define SOFT_RESET_START_BIT 7
#define SOFT_RESET_WIDTH 1


typedef enum soft_reset {
    SOFT_UN_RESET_E,
    SOFT_RESET_E
} SOFT_RESET_T ;
#define SOFT_RESET_PARAM_MASK  0x80



//  
//  VPS encoding. 
//  0: disabled 
//  1: enabled.
//  Register TvoutConfiguration7 0x0607, Bits 1:1, typedef vps_enable
//   
//  
#define VPS_ENCODING_MASK 0x2U
#define VPS_ENCODING_OFFSET 1
#define VPS_ENCODING_STOP_BIT 1
#define VPS_ENCODING_START_BIT 1
#define VPS_ENCODING_WIDTH 1

#define TVOUT_CONFIGURATION_7_REG 0x607

typedef enum vps_enable {
    VPS_DISABLE_E,
    VPS_ENABLE_E
} VPS_ENABLE_T ;
#define VPS_ENCODING_PARAM_MASK  0x2



//  
//  Enable/Disable blanking of all video lines.
//  Register TvoutConfiguration8 0x0608, Bits 3:3, typedef blanking_all_enable
//   
//  
#define BLANKING_ALL_MASK 0x8U
#define BLANKING_ALL_OFFSET 3
#define BLANKING_ALL_STOP_BIT 3
#define BLANKING_ALL_START_BIT 3
#define BLANKING_ALL_WIDTH 1

#define TVOUT_CONFIGURATION_8_REG 0x608

typedef enum blanking_all_enable {
    BLANKING_ALL_DISABLE_E,
    BLANKING_ALL_ENABLE_E
} BLANKING_ALL_ENABLE_T ;
#define BLANKING_ALL_PARAM_MASK  0x8



//  
//  Should be programmed to 1 only after each reset.
//  Register TvoutConfiguration8 0x0608, Bits 4:4, typedef val422_mux_set
//   
//  
#define VAL_422_MUX_MASK 0x10U
#define VAL_422_MUX_OFFSET 4
#define VAL_422_MUX_STOP_BIT 4
#define VAL_422_MUX_START_BIT 4
#define VAL_422_MUX_WIDTH 1


typedef enum val422_mux_set {
    VAL_422_MUX_NO_ACTION_E,
    VAL_422_MUX_ENABLE_E
} VAL_422_MUX_SET_T ;
#define VAL_422_MUX_PARAM_MASK  0x10



//  
//  Phase reset mode selection Automatically set back to 00 following the oscillator 
//  reset in modes 01 and 10. 
//  00: disabled 
//  01: enabled - phase is updated with value from phase buffer register (see 
//  Configuration2 register bit 'rstosc_buf') at the beginning of the next video line. In 
//  the mean time, the increment is updated with hard or soft values depending on 
//  selrst_inc value (see Configuration5 register). 
//  10: enabled - phase is updated with values from Increment_dfs1 and Increment_dfs2 
//  registers, based on the next increment update from cfc (depending on cfc loading 
//  moment andConfiguration6 register 'cfc[1:0]' bits. 
//  11: enabled - phase is reset following the detection of 'rst' bit on cfc line, up to 
//  9 pix_clk after loading of cfc's LSB..
//  Register TvoutConfiguration8 0x0608, Bits 6:7, typedef phase_reset_mode
//   
//  
#define PHASE_RESET_MODE_MASK 0xC0U
#define PHASE_RESET_MODE_OFFSET 7
#define PHASE_RESET_MODE_STOP_BIT 7
#define PHASE_RESET_MODE_START_BIT 6
#define PHASE_RESET_MODE_WIDTH 2

#define  TVOUT_PHASE_RESET_DISABLED 0
#define  TVOUT_PHASE_MODE_1 1
#define  TVOUT_PHASE_MODE_2 2
#define  TVOUT_PHASE_MODE_3 3

typedef enum phase_reset_mode {
    TVOUT_PHASE_RESET_DISABLED_E,
    TVOUT_PHASE_MODE_1_E,
    TVOUT_PHASE_MODE_2_E,
    TVOUT_PHASE_MODE_3_E
} PHASE_RESET_MODE_T ;
#define PHASE_RESET_MODE_PARAM_MASK  0xC0



//  
//  [00]: DAC is disabled 
//  [01]: DAC is off 
//  [1x]: DAC is enabled.
//  Register TvOutControl 0x0680, Bits 0:1, typedef tvout_dac_enable
//   
//  
#define TVOUT_DAC_CTRL_MASK 0x3U
#define TVOUT_DAC_CTRL_OFFSET 1
#define TVOUT_DAC_CTRL_STOP_BIT 1
#define TVOUT_DAC_CTRL_START_BIT 0
#define TVOUT_DAC_CTRL_WIDTH 2

#define TV_OUT_CONTROL_REG 0x680
#define  TVOUT_DAC_DISABLE 0
#define  TVOUT_DAC_OFF 1
#define  TVOUT_DAC_ENALE 2

typedef enum tvout_dac_enable {
    TVOUT_DAC_DISABLE_E,
    TVOUT_DAC_OFF_E,
    TVOUT_DAC_ENALE_E
} TVOUT_DAC_ENABLE_T ;
#define TVOUT_DAC_CTRL_PARAM_MASK  0x3



//  
//  Enable/Disable plug/unplug TV detection.
//  Register TvOutControl 0x0680, Bits 2:2, typedef plug_tv_detect_enable
//   
//  
#define PLUG_TV_DETECT_MASK 0x4U
#define PLUG_TV_DETECT_OFFSET 2
#define PLUG_TV_DETECT_STOP_BIT 2
#define PLUG_TV_DETECT_START_BIT 2
#define PLUG_TV_DETECT_WIDTH 1


typedef enum plug_tv_detect_enable {
    PLUG_TV_DETECT_DISABLE_E,
    PLUG_TV_DETECT_ENABLE_E
} PLUG_TV_DETECT_ENABLE_T ;
#define PLUG_TV_DETECT_PARAM_MASK  0x4



//  
//  PlugTvTime.
//  Register TvOutControl 0x0680, Bits 3:5, typedef plug_tv_time
//   
//  
#define PLUG_TV_TIME_MASK 0x38U
#define PLUG_TV_TIME_OFFSET 5
#define PLUG_TV_TIME_STOP_BIT 5
#define PLUG_TV_TIME_START_BIT 3
#define PLUG_TV_TIME_WIDTH 3

#define  TV_OUT_PLUG_TIM_0S5 0
#define  TV_OUT_PLUG_TIM_1S 1
#define  TV_OUT_PLUG_TIM_1S5 2
#define  TV_OUT_PLUG_TIM_2S 3
#define  TV_OUT_PLUG_TIM_2S5 4
#define  TV_OUT_PLUG_TIM_3S 5

typedef enum plug_tv_time {
    TV_OUT_PLUG_TIM_0S5_E,
    TV_OUT_PLUG_TIM_1S_E,
    TV_OUT_PLUG_TIM_1S5_E,
    TV_OUT_PLUG_TIM_2S_E,
    TV_OUT_PLUG_TIM_2S5_E,
    TV_OUT_PLUG_TIM_3S_E
} PLUG_TV_TIME_T ;
#define PLUG_TV_TIME_PARAM_MASK  0x38



//  
//  TV capacitive load.
//  Register TvOutControl 0x0680, Bits 6:6, typedef tv_load_r_c_set
//   
//  
#define TV_LOAD_RC_MASK 0x40U
#define TV_LOAD_RC_OFFSET 6
#define TV_LOAD_RC_STOP_BIT 6
#define TV_LOAD_RC_START_BIT 6
#define TV_LOAD_RC_WIDTH 1


typedef enum tv_load_r_c_set {
    TV_LOAD_RC_NO_ACTION_E,
    TV_LOAD_RC_ENABLE_E
} TV_LOAD_RC_SET_T ;
#define TV_LOAD_RC_PARAM_MASK  0x40



//  
//  0: DENC is set in SDR input data flow 
//  1: DENC is set in DDR input data flow.
//  Register TvOutControl2 0x0681, Bits 0:0, typedef double_data_rate
//   
//  
#define TV_DOUBLE_DATA_RATE_MASK 0x1U
#define TV_DOUBLE_DATA_RATE_OFFSET 0
#define TV_DOUBLE_DATA_RATE_STOP_BIT 0
#define TV_DOUBLE_DATA_RATE_START_BIT 0
#define TV_DOUBLE_DATA_RATE_WIDTH 1

#define TV_OUT_CONTROL_2_REG 0x681

typedef enum double_data_rate {
    SIMPLE_DATA_RATE_E,
    DOUBLE_DATA_RATE_E
} DOUBLE_DATA_RATE_T ;
#define TV_DOUBLE_DATA_RATE_PARAM_MASK  0x1



//  
//  0: in DDR mode LSB data is on clock rising edge 
//  1: in DDR mode LSB data is on clock falling edge.
//  Register TvOutControl2 0x0681, Bits 1:1, typedef clock_mode
//   
//  
#define TV_DUAL_DATA_MODE_MASK 0x2U
#define TV_DUAL_DATA_MODE_OFFSET 1
#define TV_DUAL_DATA_MODE_STOP_BIT 1
#define TV_DUAL_DATA_MODE_START_BIT 1
#define TV_DUAL_DATA_MODE_WIDTH 1


typedef enum clock_mode {
    CLOCK_RISING_EDGE_E,
    CLOCK_FAILLING_EDGE_E
} CLOCK_MODE_T ;
#define TV_DUAL_DATA_MODE_PARAM_MASK  0x2


#endif
