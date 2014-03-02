/*****************************************************************************/
/*
*  � ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of AB8500(STw4500) Core module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __AB8500_CORE_H__
#define __AB8500_CORE_H__

#include "debug.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * MACROS
 *----------------------------------------------------------------------*/
/*Defines for Version */
#define AB8500_CORE_HCL_VERSION_ID  1
#define AB8500_CORE_HCL_MAJOR_ID    1
#define AB8500_CORE_HCL_MINOR_ID    2


/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/

/*    Define AB8500 errors    */
typedef enum
{
    AB8500_CORE_INTERNAL_ERROR            = HCL_INTERNAL_ERROR,
    AB8500_CORE_OK                        = HCL_OK,
    AB8500_CORE_ERROR                     = HCL_ERROR,
    AB8500_CORE_UNSUPPORTED_FEATURE       = HCL_UNSUPPORTED_FEATURE,
    AB8500_CORE_INVALID_PARAMETER         = HCL_INVALID_PARAMETER,
    AB8500_CORE_TRANSACTION_ON_SPI_FAILED = HCL_MAX_ERROR_VALUE 
}t_ab8500_core_error;

/*    CORE Turnon Cause    */
typedef enum
{
    AB8500_CORE_TURN_ON_BY_PORnVBAT,
    AB8500_CORE_TURN_ON_BY_PONKEY1dbF,
    AB8500_CORE_TURN_ON_BY_PONKEY2dbF,
    AB8500_CORE_TURN_ON_BY_RTCALARM,
    AB8500_CORE_TURN_ON_BY_MAINCHDETECT,
    AB8500_CORE_TURN_ON_BY_VBUSDETECTR,
    AB8500_CORE_TURN_ON_BY_USBIDDETECTR,
    AB8500_CORE_NO_TURNON_CAUSE
}t_ab8500_core_turnon_cause;

/*    Bank Selection for Power  */
typedef enum
{
    AB8500_CORE_STW4500_ALL,
    AB8500_CORE_AUDIO,
    AB8500_CORE_DENC
}t_ab8500_core_bank;

/*    SMPS Selection for Power  */
typedef enum
{
    AB8500_CORE_SMPS_1,
    AB8500_CORE_SMPS_2, 
    AB8500_CORE_SMPS_3
}t_ab8500_core_smps_sel;

/*    Vaux Selection for Power  */
typedef enum
{
    AB8500_CORE_VAUX_1,
    AB8500_CORE_VAUX_2,
    AB8500_CORE_VAUX_3
}t_ab8500_core_vaux;

/*    SMPS Clock Sources        */
typedef enum
{
    AB8500_CORE_SMPS_EXTERNAL_SYSCLK,
    AB8500_CORE_SMPS_INTERNAL_3_2MHZ_CLK
}t_ab8500_core_smps_clk_sel;

/*    SMPS Timer control for Power  */
typedef enum
{
    AB8500_CORE_SMPS_TIMER_DISABLE,
    AB8500_CORE_SMPS_TIMER_ENABLE
}t_ab8500_core_smps_timer_ctrl;

/*   Clock Selection for Enabling  */
typedef enum
{
    AB8500_CORE_TVOUT_INTERNAL_CLK,
    AB8500_CORE_USB_INTERNAL_CLK,
    AB8500_CORE_AUDIO_INTERNAL_CLK
}t_ab8500_core_device_clk;


/*   27MHz clock O/P buffer Control  */
typedef enum
{
    AB8500_CORE_CLK27MHZ_OP_BUF_DISABLE,
    AB8500_CORE_CLK27MHZ_OP_BUF_ENABLE
}t_ab8500_core_clk27_ctrl;

/*    ULPCLK pad for configuration  */
typedef enum
{
    AB8500_CORE_ULPCLK_PAD_NO_FUNC =0,
    AB8500_CORE_ULPCLK_PAD_OUTPUT,
    AB8500_CORE_ULPCLK_PAD_INPUT
}t_ab8500_core_ulpclk_conf;


/*    SYSCLKREQ Selection for Power  */
typedef enum
{
    AB8500_CORE_SYSCLKREQ_1,
    AB8500_CORE_SYSCLKREQ_2,
    AB8500_CORE_SYSCLKREQ_3,
    AB8500_CORE_SYSCLKREQ_4,
    AB8500_CORE_SYSCLKREQ_5,
    AB8500_CORE_SYSCLKREQ_6,
    AB8500_CORE_SYSCLKREQ_7,
    AB8500_CORE_SYSCLKREQ_8
}t_ab8500_core_sysclkreq_sel;

/*    INTn8500 Ball configuration  */
typedef enum
{
    AB8500_CORE_INT8500N_PUSHPULL_OP,
    AB8500_CORE_INT8500N_OPENDRAIN_OP
}t_ab8500_core_int8500n_conf;

/*    SWAT Control      */
typedef enum
{
    AB8500_CORE_SWAT_DIS,
    AB8500_CORE_SWAT_ENA
}t_ab8500_core_swat_ctrl;

/*    RF IC  Selection  */
typedef enum
{
    AB8500_CORE_RF_IC_LEVI,
    AB8500_CORE_RF_IC_DIESEL
}t_ab8500_core_levidiesel_sel;

/*    27MHz CLK Pulldown control  */
typedef enum
{
    AB8500_CORE_CLK27MHZ_PULLDOWN_DISABLED,
    AB8500_CORE_CLK27MHZ_PULLDOWN_ENABLED
}t_ab8500_core_clk27_pud;

/*    TVOut PLL Control for Power  */
typedef enum
{
    AB8500_CORE_DISABLE_TVOUT_PLL,
    AB8500_CORE_ENABLE_TVOUT_PLL
}t_ab8500_core_pll_control;

/*    Digital Buffer Strength      */
typedef enum
{
    AB8500_CORE_DIGBUFFER_STR_0,
    AB8500_CORE_DIGBUFFER_STR_1
}t_ab8500_core_clk_strength;

/*   Selection between ULPCLK and CLK27MHz */
typedef enum
{
    AB8500_CORE_ULPCLK,
    AB8500_CORE_CLK27MHZ
}t_ab8500_core_clk_str;

/*    TVoutCLK Delay configuration  */
typedef enum
{
    AB8500_CORE_TVOUTCLK_DELAYED,
    AB8500_CORE_TVOUTCLK_NOT_DELAYED
}t_ab8500_core_tvout_clk_delay;

/*    TVoutCLK Inversion configuration  */
typedef enum
{
    AB8500_CORE_TVOUTCLK_NOT_INVERTED,
    AB8500_CORE_TVOUTCLK_INVERTED
}t_ab8500_core_tvout_clk_inv;


typedef enum
{
    AB8500_CORE_USB_PLL,
    AB8500_CORE_TVOUT_PLL
}t_ab8500_core_device_pll;


typedef enum
{
    AB8500_CORE_CLK32KOUT2_ENABLED,
    AB8500_CORE_CLK32KOUT2_DISABLE
}t_ab8500_core_clk32kout2_ctrl;


typedef enum
{
    AB8500_CORE_VINTCORE12_SELECT,
    AB8500_CORE_VTVOUT_SELECT
}t_ab8500_core_vintvtvout_select;


typedef enum
{
    AB8500_CORE_VINTVTVOUT_DISABLE,
    AB8500_CORE_VINTVTVOUT_ENABLE
}t_ab8500_core_vintvtvout_control;

/* Vint Low Power Mode Settings         */
typedef enum
{
    AB8500_CORE_VINT_LP_DISABLE,
    AB8500_CORE_VINT_LP_ENABLE
}t_ab8500_core_vint_lp_select;

/*   VintVcore12 Voltage  Selection      */
typedef enum
{
    AB8500_CORE_VINTCORE12_1_2V,           /*            1.2V            */
    AB8500_CORE_VINTCORE12_1_225V,         /*            1.225V          */
    AB8500_CORE_VINTCORE12_1_250V,         /*            1.25V           */
    AB8500_CORE_VINTCORE12_1_275V,         /*            1.275V          */
    AB8500_CORE_VINTCORE12_1_3V,           /*            1.3V            */
    AB8500_CORE_VINTCORE12_1_325V,         /*            1.325V          */
    AB8500_CORE_VINTCORE12_1_350V,         /*            1.35V           */
    AB8500_CORE_VINTCORE12_NA              /*            NA              */
}t_ab8500_core_vintcore12_voltage;

/*   Audio Headset and Mic supply Control */
typedef enum
{
    AB8500_CORE_SUPPLY_DISABLE,
    AB8500_CORE_SUPPLY_ENABLE
}t_ab8500_core_vaudio_hs_mic_supplyctrl;

/* Vamic output Impedance Settings      */
typedef enum
{
    AB8500_CORE_VAMIC_ZOUT_HIGH,
    AB8500_CORE_VAMIC_ZOUT_GROUNDED
}t_ab8500_core_vamic_zout_ctrl;

/*  TVOUT RC load selection as 750 OHM or 750 OHM with Capacitance */
typedef enum
{
    AB8500_CORE_TVOUT_LOAD_75OHM,
    AB8500_CORE_TVOUT_LOAD_75OHM_PLUS_CAP
}t_ab8500_core_tvload_rc;

/*    TVOUT Plug Detection Enable/Disable  */
typedef enum
{
    AB8500_CORE_TV_PLUG_DETECTION_DISABLED,
    AB8500_CORE_TV_PLUG_DETECTION_ENABLED
}t_ab8500_core_plugtv_ctrl;

/*         TVOUT DAC Settings      */
typedef enum
{
    AB8500_CORE_TVOUT_DAC_DISABLED_INACTIVE_OUTPUT,   /*  DAC Disabled and Output Inactive */
    AB8500_CORE_TVOUT_DAC_DISABLED_DC_OUTPUT,         /*  DAC Disabled and Output DC       */
    AB8500_CORE_TVOUT_DAC_ENABLED_INACTIVE_OUTPUT,    /*  DAC Enabled and Output Inactive  */
    AB8500_CORE_TVOUT_DAC_ENABLED_DC_OUTPUT           /*  DAC Enabled and Output DC        */
}t_ab8500_core_tvout_dac_ctrl;

/*  TVPlug detection time configuration  */
typedef enum
{
    AB8500_CORE_0_5SECS,           /*       0.5 secs      */
    AB8500_CORE_1_0SECS,           /*       1.0 secs      */
    AB8500_CORE_1_5SECS,           /*       1.5 secs      */
    AB8500_CORE_2_0SECS,           /*       2.0 secs      */
    AB8500_CORE_2_5SECS,           /*       2.5 secs      */
    AB8500_CORE_3_0SECS            /*       3.0 secs      */
}t_ab8500_core_plugtv_time;

/*   AB8500 Power Regulator Selection  */ 
typedef enum
{
    AB8500_CORE_REGU_VARM_SEL,
    AB8500_CORE_REGU_VAPE_SEL,
    AB8500_CORE_REGU_VSMPS1_SEL,
    AB8500_CORE_REGU_VSMPS2_SEL,
    AB8500_CORE_REGU_VSMPS3_SEL
}t_ab8500_core_regu_sel;

/*   AB8500 Power Regulator Selection for setting regulator mode */ 
typedef enum
{
    AB8500_CORE_REGU_VARM,
    AB8500_CORE_REGU_VAPE,
    AB8500_CORE_REGU_VSMPS1,
    AB8500_CORE_REGU_VSMPS2,
    AB8500_CORE_REGU_VSMPS3,
    AB8500_CORE_REGU_VBBN,
    AB8500_CORE_REGU_VBBP,
    AB8500_CORE_REGU_VPLL,
    AB8500_CORE_REGU_VANA,
    AB8500_CORE_REGU_VEXT_SUPPLY_1,
    AB8500_CORE_REGU_VEXT_SUPPLY_2,
    AB8500_CORE_REGU_VEXT_SUPPLY_3,
    AB8500_CORE_REGU_VAUX_1,
    AB8500_CORE_REGU_VAUX_2,
    AB8500_CORE_REGU_VAUX_3,
    AB8500_CORE_REGU_VRF_1
}t_ab8500_core_regu_mode_sel;

/*   VrefDDR Power Regulator Control   */ 
typedef enum
{
    AB8500_CORE_VREFDDR_DISABLE,
    AB8500_CORE_VREFDDR_ENABLE
}t_ab8500_core_vref_ddr_ctrl;

/*   Ab8500  Power Regulator settings   */
typedef enum
{
    AB8500_CORE_DISABLE = 0x00,
    AB8500_CORE_FORCE_ENABLE_IN_HP_MODE,
    AB8500_CORE_HW_OR_HP_CTRL,
    AB8500_CORE_FORCE_ENABLE_IN_LP_MODE
}t_ab8500_core_regu_mode;

/*   PWM Generator Control             */ 
typedef enum
{
    AB8500_CORE_PWM_DISABLED,
    AB8500_CORE_PWM_ENABLED
}t_ab8500_core_pwm_ctrl;

/*   Ab8500 Automode  Control for power  */ 
typedef enum
{
    AB8500_CORE_AUTOMODE_DISABLED,
    AB8500_CORE_AUTOMODE_ENABLED
}t_ab8500_core_auto_ctrl;

/*   LDO voltage control register selection   */ 
typedef enum
{
    AB8500_CORE_VOLT_CTRL_SEL1_REG,
    AB8500_CORE_VOLT_CTRL_SEL2_REG,
    AB8500_CORE_VOLT_CTRL_SEL3_REG
}t_ab8500_core_volt_ctrl_reg;


typedef enum
{
    AB8500_CORE_CLKREQ_INACTIVE,
    AB8500_CORE_CLKREQ_ACTIVE
}t_ab8500_core_clkreq_ctrl;

typedef enum
{
    AB8500_CORE_INTCLK_SYSCLK,
    AB8500_CORE_INTCLK_ULPCLK,
    AB8500_CORE_INTCLK_SWITCHED
}t_ab8500_core_sysulpintclk_sel;

/*     Defines if SYSCLKREQ1 requests SYSCLK    */
typedef enum
{
    AB8500_CORE_SYSCLK_NO_REQUEST,
    AB8500_CORE_SYSCLK_REQUSET
}t_ab8500_core_sysclkreq1_clkip;

typedef enum
{
    AB8500_CORE_DIS = 0,
    AB8500_CORE_ENA
}t_ab8500_core_ctrl;

typedef enum
{
    AB8500_CORE_SYSCLKREQX_NOT_ASSOC,
    AB8500_CORE_SYSCLKREQX_ASSOC
}t_ab8500_core_sysclkreq_rfclkbuf_assoc;

/*----------------------------------------------------------------------*/
/* Structures                                                           */
/*----------------------------------------------------------------------*/
typedef struct
{
    t_ab8500_core_clkreq_ctrl sysclkreq;
    t_ab8500_core_clkreq_ctrl ulpclkreq;
    t_ab8500_core_sysulpintclk_sel     sysulpintclksel;
}t_ab8500_core_sysulpclkreq;

/* VBBN and VBBP voltage control register settings */
typedef struct
{
    t_ab8500_core_volt_ctrl_reg vbbp_sel_ctrl;
    t_ab8500_core_volt_ctrl_reg vbbn_sel_ctrl;
}t_ab8500_core_vbbvolt_ctrl_reg;

/* configuration of SYSCLKREQ1 Inout as ULPCLKREQ/SYSCLKREQ   */
typedef struct
{
    t_ab8500_core_sysclkreq1_clkip req_ulpclk;
    t_ab8500_core_sysclkreq1_clkip req_sysclk;
}t_ab8500_core_sysclkreq1_input;

/* Vamic1 and Vamic2 Impedance control   settings */
typedef struct
{
    t_ab8500_core_vamic_zout_ctrl vamic1_zout;
    t_ab8500_core_vamic_zout_ctrl vamic2_zout;
}t_ab8500_core_vamic_zout;


typedef struct
{
    t_ab8500_core_sysclkreq_rfclkbuf_assoc clk_buf_2; 
    t_ab8500_core_sysclkreq_rfclkbuf_assoc clk_buf_3; 
    t_ab8500_core_sysclkreq_rfclkbuf_assoc clk_buf_4; 
}t_ab8500_core_rfclkbuf_conf;

/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*                                                              Functions declaration                                                */
/*-----------------------------------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------Initialisation -----------------------------------------------------------------------------*/
PUBLIC t_ab8500_core_error AB8500_CORE_GetVersion                     (OUT t_version *p_version);
PUBLIC t_ab8500_core_error AB8500_CORE_SetDbgLevel                    (IN t_dbg_level ab8500_dbg_level);
PUBLIC t_ab8500_core_error AB8500_CORE_GetTurnOnCause                 (OUT t_ab8500_core_turnon_cause *p_ab8500_turnon_cause);

/*-----------------------------------------------------Power and Control-----------------------------------------------------------------------------*/
PUBLIC t_ab8500_core_error AB8500_CORE_TurnOff                        (void);
PUBLIC t_ab8500_core_error AB8500_CORE_ResetPlatform                  (IN  t_uint8 secs);
PUBLIC t_ab8500_core_error AB8500_CORE_GetPonKey1PressDuration        (OUT t_uint32 *p_msecs);
PUBLIC t_ab8500_core_error AB8500_CORE_ResetRegisters                 (IN t_ab8500_core_bank bank);
PUBLIC t_ab8500_core_error AB8500_CORE_ReleaseRegistersFromReset      (IN t_ab8500_core_bank bank);
PUBLIC t_ab8500_core_error AB8500_CORE_Clk32kOut2Control              (IN t_ab8500_core_clk32kout2_ctrl );
PUBLIC t_ab8500_core_error AB8500_CORE_SetClkStrength                 (IN t_ab8500_core_clk_strength , IN t_ab8500_core_clk_str );
PUBLIC t_ab8500_core_error AB8500_CORE_ControlTvoutClkDelay           (IN t_ab8500_core_tvout_clk_delay );
PUBLIC t_ab8500_core_error AB8500_CORE_ControlTvoutClkInv             (IN t_ab8500_core_tvout_clk_inv );
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigClk27Mhz                 (IN t_ab8500_core_clk27_ctrl , IN t_ab8500_core_clk27_pud );
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigULPClkPad                (IN t_ab8500_core_ulpclk_conf );
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigSysUlpClkReq             (IN t_ab8500_core_sysulpclkreq );
PUBLIC t_ab8500_core_error AB8500_CORE_EnableInternalClk              (IN t_ab8500_core_device_clk );
PUBLIC t_ab8500_core_error AB8500_CORE_DisableInternalClk             (IN t_ab8500_core_device_clk );
PUBLIC t_ab8500_core_error AB8500_CORE_ControlTvoutPll                (IN t_ab8500_core_pll_control );
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigInt8500n                 (IN t_ab8500_core_int8500n_conf );
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigSysClkReq1Input          (IN t_ab8500_core_sysclkreq1_input );

PUBLIC t_ab8500_core_error AB8500_CORE_ConfigSysClkReqRfClkBuf        (IN t_ab8500_core_sysclkreq_sel , IN t_ab8500_core_rfclkbuf_conf );

PUBLIC t_ab8500_core_error AB8500_CORE_SwatControl                    (IN t_ab8500_core_swat_ctrl );
PUBLIC t_ab8500_core_error AB8500_CORE_SelLeviDieselRFIC              (IN t_ab8500_core_levidiesel_sel );
PUBLIC t_ab8500_core_error AB8500_CORE_SelRfOffTmr                    (IN t_uint8 );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVintCore12Voltage           (IN t_ab8500_core_vintcore12_voltage );
PUBLIC t_ab8500_core_error AB8500_CORE_VintVTVoutControl              (IN t_ab8500_core_vintvtvout_select , IN t_ab8500_core_vintvtvout_control );
PUBLIC t_ab8500_core_error AB8500_CORE_ForceVintToLP                  (IN t_ab8500_core_vint_lp_select );
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVaudio                     (IN t_ab8500_core_vaudio_hs_mic_supplyctrl );
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVdmic                      (IN t_ab8500_core_vaudio_hs_mic_supplyctrl );
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVamic1                     (IN t_ab8500_core_vaudio_hs_mic_supplyctrl );
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVamic2                     (IN t_ab8500_core_vaudio_hs_mic_supplyctrl );
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVamicZout                  (IN t_ab8500_core_vamic_zout );
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlTVLoadRC                   (IN t_ab8500_core_tvload_rc );
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlPlugTV                     (IN t_ab8500_core_plugtv_ctrl );
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlTVoutDAC                   (IN t_ab8500_core_tvout_dac_ctrl );
PUBLIC t_ab8500_core_error AB8500_CORE_SetPlugTVTime                  (IN t_ab8500_core_plugtv_time );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVoltReguPWMMode             (IN t_ab8500_core_regu_sel , IN t_ab8500_core_pwm_ctrl  );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVoltReguAUTOMode            (IN t_ab8500_core_regu_sel , IN t_ab8500_core_auto_ctrl  );
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigVoltReguMode             (IN t_ab8500_core_regu_mode_sel , IN t_ab8500_core_regu_mode  );
PUBLIC t_ab8500_core_error AB8500_CORE_SetArmVoltCtrlReg              (IN t_ab8500_core_volt_ctrl_reg  );
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVrefDDR                    (IN t_ab8500_core_vref_ddr_ctrl  );
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVrefDDRSleepMode           (IN t_ab8500_core_vref_ddr_ctrl  );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVarmVoltage                 (IN t_ab8500_core_volt_ctrl_reg  , IN t_uint8 );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVapeVoltage                 (IN t_ab8500_core_volt_ctrl_reg  , IN t_uint8 );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVauxVoltage                 (IN t_ab8500_core_vaux  , IN t_uint8 );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVbbpVoltage                 (IN t_ab8500_core_volt_ctrl_reg  , IN t_uint8 );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVbbnVoltage                 (IN t_ab8500_core_volt_ctrl_reg  , IN t_uint8 );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVrf1Voltage                 (IN t_uint8 );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVsmpsVoltage                (IN t_ab8500_core_smps_sel , IN t_ab8500_core_volt_ctrl_reg  , IN t_uint8 );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVapeCtrlReg                 (IN t_ab8500_core_volt_ctrl_reg  );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVSmpsCtrlReg                (IN t_ab8500_core_smps_sel , IN t_ab8500_core_volt_ctrl_reg );
PUBLIC t_ab8500_core_error AB8500_CORE_SetVBBCtrlReg                  (IN t_ab8500_core_vbbvolt_ctrl_reg  );

/*-----------------------------------------------------USB-------------------------------------------------------------------------------------------*/
PUBLIC t_ab8500_core_error AB8500_CORE_USBOTGSupplyCtrl               (IN t_ab8500_core_ctrl );
PUBLIC t_ab8500_core_error AB8500_CORE_USBOTGIDDeviceCtrl             (IN t_ab8500_core_ctrl );
PUBLIC t_ab8500_core_error AB8500_CORE_USBOTGIDHostCtrl               (IN t_ab8500_core_ctrl );

/*---------------------------------------------Default Init-------------------------------------------------------------------------------------------*/
PUBLIC t_ab8500_core_error AB8500_CORE_TvoutDefInit                   (void);
PUBLIC t_ab8500_core_error AB8500_CORE_AudioDefInit                   (void);


#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __AB8500_CORE_H__ */

/* End of file - ab8500_core.h */

