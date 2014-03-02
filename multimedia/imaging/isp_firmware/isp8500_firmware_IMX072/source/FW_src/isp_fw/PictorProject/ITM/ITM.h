/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      ITM.h

 \brief     This file exposes interfaces to allow other modules to enable, disable or clear
            individual interrupts routed through the ITM (ISP Interrupt Manager).

 \ingroup   ITM
 \endif
*/
#ifndef _ITM_H_
#   define _ITM_H_

#   include "PictorhwReg.h"
#   include "CRMhwReg.h"
#   include "IPPhwReg.h"

#include "Profiler.h"

/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct    Interrupts_test_ts
 \brief     structure to count interrupts
 \ingroup   HostInterface
 \endif
*/
typedef struct
{
    uint16_t    u16_INT23_CRM;

    uint16_t    u16_INT20_STAT0_256_bins_histogram_Schedule;

    uint16_t    u16_INT17_GPIO;

    uint16_t    u16_INT16_STAT8_Glace_AWB;

    uint16_t    u16_INT15_STAT7_Acc_Wg_Zones_White_Balance;

     uint16_t    u16_INT13_STAT5_Visual_Significance;

    uint16_t    u16_INT12_STAT4_Skin_Tone_Detection;

    uint16_t    u16_INT11_STAT3_MWWB_White_Balance;

    uint16_t    u16_INT10_STAT2_Auto_Focus;

    uint16_t    u16_INT09_STAT1_6X8_Exposure;

    uint16_t    u16_INT08_STAT0_256_bins_histogram;

    uint16_t    u16_INT07_VIDEOPIPE1;

    uint16_t    u16_INT06_VIDEOPIPE0;

    uint16_t    u16_INT05_SMIARX;
    uint16_t    u16_INT05_SMIARX0;
    uint16_t    u16_INT05_SMIARX1;
    uint16_t    u16_INT05_SMIARX2;
    uint16_t    u16_INT05_SMIARX3;
    uint16_t    u16_INT05_SMIARX4;

    uint16_t    u16_INT04_reserved;

    uint16_t    u16_INT03_Line_blaning_elimination;

    uint16_t    u16_INT02_MASTERCCI;

    uint16_t    u16_INT01_STAT8_Glace_Schedule;

    uint16_t    u16_INT00_USER_INTERFACE;

    uint16_t    u16_INT27_ERROR_EOF;
} Interrupts_test_ts;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 00 Host Comms
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Macro to clear host comms interrupt status at ITM
#   define ITM_ClearHostCommsInterrupt()   Set_ITM_ITM_HCOM_STATUS_BCLR(1)

/// Macro to enable host comms interrupt at ITM
#   define ITM_EnableHostCommsInterrupt()  Set_ITM_ITM_HCOM_EN_BSET(1)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 01 Glace xport Schedule
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Clear interrupt
#   define ITM_Clear_Interrupt_Glace_Schedule()            ITC_CLEAR_PENDING(IT_NO_GLACE_SCHEDULE)

/// Enable interrupt
#   define ITM_Enable_Interrupt_Glace_Schedule()           ITC_SET_PENDING(IT_NO_GLACE_SCHEDULE)


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 02 Master CCI
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Macro to clear master cci interrupt status at ITM
#   define ITM_ClearMasterCCIInterrupt()                       Set_ITM_ITM_MCCI_STATUS_BCLR(1, 1)               // clear EOP interrupt and Error interrupt

/// Macro to enable master cci interrupt at ITM
#   define ITM_EnableMasterCCIInterrupt()                      Set_ITM_ITM_MCCI_EN_BSET(1, 1)                   // enable EOP interrupt and Error interrupt



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 03 Line blanking elimination - LBE
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Clear interrupt
#   define ITM_Clear_Interrupt_LBE()                           Set_ITM_ITM_LBE_STATUS_BCLR(1)                   // LBE_STATUS_BCLR

/// Enable interrupt
#   define ITM_Enable_Interrupt_LBE()                          Set_ITM_ITM_LBE_EN_BSET(1)                       // LBE_EN_BSET


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 04 RE PIPE
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Macro to clear all RE PIPE interrupts at ITM
#   define ITM_ClearInterrupt_REPIPE()                           Set_ITM_ITM_REPIPE_STATUS_BCLR(1, 1, 1)                      // Clear RE PIPE

/// Macro to enable all RE PIPE interrupts at ITM
#   define ITM_EnableInterrupt_REPIPE()                          Set_ITM_ITM_REPIPE_EN_BSET(1, 1, 1)                          // Enable RE PIPE

/// Macro to disable all RE PIPE interrupts at ITM
#   define ITM_DisableInterrupt_REPIPE()                         Set_ITM_ITM_REPIPE_EN_BCLR(1, 1, 1)                          // Disable RE PIPE

/// Macro to clear RE PIPE IN interrupt status at ITM
#   define ITM_ClearInterrupt_REPIPE_IN()                        Set_ITM_ITM_REPIPE_STATUS_BCLR_RE_IN_STATUS_BCLR(1)          // Clear RE PIPE IN

/// Macro to enable RE PIPE IN interrupt at ITM
#   define ITM_EnableInterrupt_REPIPE_IN()                       Set_ITM_ITM_REPIPE_EN_BSET_RE_IN_EN_BSET(1)                  // Enable RE PIPE IN

/// Macro to disable RE PIPE IN interrupt at ITM
#   define ITM_DisableInterrupt_REPIPE_IN()                      Set_ITM_ITM_REPIPE_EN_BCLR_RE_IN_EN_BCLR(1)                  // Disable RE PIPE IN

/// Macro to clear RE PIPE POSTGAIN interrupt status at ITM
#   define ITM_ClearInterrupt_REPIPE_POSTGAIN()                  Set_ITM_ITM_REPIPE_STATUS_BCLR_RE_POSTGAIN_STATUS_BCLR(1)    // Clear RE PIPE POSTGAIN

/// Macro to enable RE PIPE POSTGAIN interrupt at ITM
#   define ITM_EnableInterrupt_REPIPE_POSTGAIN()                 Set_ITM_ITM_REPIPE_EN_BSET_RE_POSTGAIN_EN_BSET(1)            // Enable RE PIPE POSTGAIN

/// Macro to disable RE PIPE POSTGAIN interrupt at ITM
#   define ITM_DisableInterrupt_REPIPE_POSTGAIN()                Set_ITM_ITM_REPIPE_EN_BCLR_RE_POSTGAIN_EN_BCLR(1)            // Disable RE PIPE POSTGAIN

/// Macro to clear RE PIPE OUT_STORE interrupt status at ITM
#   define ITM_ClearInterrupt_REPIPE_OUT_STORE()                 Set_ITM_ITM_REPIPE_STATUS_BCLR_RE_OUT_STORE_STATUS_BCLR(1)   // Clear RE PIPE OUT_STORE

/// Macro to enable RE PIPE OUT_STORE interrupt at ITM
#   define ITM_EnableInterrupt_REPIPE_OUT_STORE()                Set_ITM_ITM_REPIPE_EN_BSET_RE_OUT_STORE_EN_BSET(1)           // Enable RE PIPE OUT_STORE

/// Macro to disable RE PIPE OUT_STORE interrupt at ITM
#   define ITM_DisableInterrupt_REPIPE_OUT_STORE()               Set_ITM_ITM_REPIPE_EN_BCLR_RE_OUT_STORE_EN_BCLR(1)           // Disable RE PIPE OUT_STORE


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 05 SMIA RX
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Macro to clear SMIA RX Start of Frame interrupt status at ITM
#   define ITM_ClearInterrupt_SMIA_RX_SOF()                    Set_ITM_ITM_SMIARX_STATUS_BCLR(1, 0, 0, 0, 0)    // Clear SOF interrupt

/// Macro to enable SMIA RX interrupt at ITM
#   define ITM_EnableInterrupt_SMIA_RX_SOF()                   Set_ITM_ITM_SMIARX_EN_BSET(1, 1, 1, 1, 0)        // SMIARX_0_EN_BSET,SMIARX_1_EN_BSET,SMIARX_2_EN_BSET,SMIARX_3_EN_BSET,SMIARX_4_EN_BSET

/// Macro to trigger Status line interrupt: interrupt 0, The interrupt is used to read embedded data lines and close exposure loop
#   define ITM_Trigger_Interrupt_RX0_Status_Line()             Set_ITM_ITM_SMIARX_STATUS_BSET_SMIARX_0_STATUS_BSET(1)

/// Macro to trigger Status line interrupt: interrupt 1, the interrupt is used for update ISP registers
#   define ITM_Trigger_Interrupt_RX1_ISP_Update()              Set_ITM_ITM_SMIARX_STATUS_BSET_SMIARX_1_STATUS_BSET(1)

/// Macro to trigger Status line interrupt: interrupt 2, the interrupt is used for stopping Rx
#   define ITM_Trigger_Interrupt_RX2_RX_Stop()                 Set_ITM_ITM_SMIARX_STATUS_BSET_SMIARX_2_STATUS_BSET(1)

/// Macro to trigger Status line interrupt: interrupt 3, the interrupt is used to commit various IPs
#   define ITM_Trigger_Interrupt_RX3_Commit_ISP()              Set_ITM_ITM_SMIARX_STATUS_BSET_SMIARX_3_STATUS_BSET(1)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 06 VIDEO PIPE 0
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Macro to clear SMIA RX Start of Frame interrupt status at ITM
#   define ITM_ClearInterrupt_VIDEO_PIPE0()                        Set_ITM_ITM_VIDPIPE0_STATUS_BCLR(1)              // Clear Video PIPE 0
#   define ITM_TriggerVidComplete0Interupt()                       Set_ITM_ITM_VIDPIPE0_STATUS_BSET(0x1)

/// Macro to enable SMIA RX interrupt at ITM
#   define ITM_EnableInterrupt_VIDEO_PIPE0()                   Set_ITM_ITM_VIDPIPE0_EN_BSET(1)                  // Enable Video PIPE 0



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 07 VIDEO PIPE 1
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Macro to clear SMIA RX Start of Frame interrupt status at ITM
#   define ITM_ClearInterrupt_VIDEO_PIPE1()                        Set_ITM_ITM_VIDPIPE1_STATUS_BCLR(1)              // Clear Video PIPE 0
#   define ITM_TriggerVidComplete1Interupt()                       Set_ITM_ITM_VIDPIPE1_STATUS_BSET(0x1)

/// Macro to enable SMIA RX interrupt at ITM
#   define ITM_EnableInterrupt_VIDEO_PIPE1()                   Set_ITM_ITM_VIDPIPE1_EN_BSET(1)                  // Enable Video PIPE 0


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 08 256-bins histogram (AEC)
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Clear interrupt
#   define ITM_Clear_Interrupt_STAT0_256_Bin_Histogram_AEC()   Set_ITM_ITM_GHIST_STATUS_BCLR(1)                 // GHIST_STATUS_BCLR

/// Enable interrupt
#   define ITM_Enable_Interrupt_STAT0_256_Bin_Histogram_AEC()  Set_ITM_ITM_GHIST_EN_BSET(1)                     // GHIST_EN_BSET


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 09 Stat1 6x8 Exposure
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Macro to clear interrupt status at ITM
#   define ITM_Clear_Interrupt_STAT1_Acc_8x6zones()            Set_ITM_ITM_EXP6X8_STATUS_BCLR(1)

/// Macro to enable interrupt at ITM
#   define ITM_Enable_Interrupt_STAT1_Acc_8x6zones()        Set_ITM_ITM_EXP6X8_EN_BSET(1)

/// Trigger Exposure interrupt manually
#   define ITM_Trigger_Interupt_ExpStats()                  Set_ITM_ITM_EXP6X8_STATUS_BSET(1)



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 10 Auto focus
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Clear interrupt

// AF_0_STATUS_BCLR,AF_1_STATUS_BCLR,AF_2_STATUS_BCLR,AF_3_STATUS_BCLR,AF_4_STATUS_BCLR,AF_5_STATUS_BCLR,AF_6_STATUS_BCLR,AF_7_STATUS_BCLR,AF_8_STATUS_BCLR,AF_9_STATUS_BCLR
#   define ITM_Clear_Interrupt_STAT2_Auto_Focus()  Set_ITM_ITM_AF_STATUS_BCLR(1, 1, 1, 1, 1, 1, 1, 1, 1, 1)

/// Enable interrupt

// AF_0_EN_BSET,AF_1_EN_BSET,AF_2_EN_BSET,AF_3_EN_BSET,AF_4_EN_BSET,AF_5_EN_BSET,AF_6_EN_BSET,AF_7_EN_BSET,AF_8_EN_BSET,AF_9_EN_BSET
#   define ITM_Enable_Interrupt_STAT2_Auto_Focus() Set_ITM_ITM_AF_EN_BSET(1, 1, 1, 1, 1, 1, 1, 1, 1, 1)


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 11 Stat3 Minimum Weighted White balance
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Macro to clear interrupt status at ITM
#   define ITM_Clear_Interrupt_STAT3_WHITE_BALANCE()   Set_ITM_ITM_MMWB_STATUS_BCLR(1)

/// Macro to enable interrupt at ITM
#   define ITM_Enable_Interrupt_STAT3_WHITE_BALANCE()  Set_ITM_ITM_MMWB_EN_BSET(1)

/// Trigger White Balance interrupt manually, using MWWB interrupt
#   define ITM_Trigger_Interupt_STAT3_WHITE_BALANCE_Stats()              Set_ITM_ITM_MMWB_STATUS_BSET(1)



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 12 Skin Tone detection
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Clear interrupt
#   define ITM_Clear_Interrupt_STAT4_Skin_tone_detection()     Set_ITM_ITM_SKIND_STATUS_BCLR(1)     //SKIND_STATUS_BCLR

/// Enable interrupt
#   define ITM_Enable_Interrupt_STAT4_Skin_tone_detection()    Set_ITM_ITM_SKIND_EN_BSET(1)         // SKIND_EN_BSET



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 13 DMCE PIPE
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Macro to clear DMCE PIPE interrupt status at ITM
#   define ITM_ClearInterrupt_DMCEPIPE()                            Set_ITM_ITM_DMCEPIPE_STATUS_BCLR(1, 1, 1)                    // Clear DMCE PIPE

/// Macro to enable DMCE PIPE interrupt at ITM
#   define ITM_EnableInterrupt_DMCEPIPE()                           Set_ITM_ITM_DMCEPIPE_EN_BSET(1, 1, 1)                        // Enable DMCE PIPE

/// Macro to disable DMCE PIPE interrupt at ITM
#   define ITM_DisableInterrupt_DMCEPIPE()                          Set_ITM_ITM_DMCEPIPE_EN_BCLR(1, 1, 1)                        // Disable DMCE PIPE

/// Macro to clear DMCE PIPE IN interrupt status at ITM
#   define ITM_ClearInterrupt_DMCEPIPE_IN()                         Set_ITM_ITM_DMCEPIPE_STATUS_BCLR_DMCE_IN_STATUS_BCLR(1)      // Clear DMCE PIPE IN

/// Macro to enable DMCE PIPE IN interrupt at ITM
#   define ITM_EnableInterrupt_DMCEPIPE_IN()                        Set_ITM_ITM_DMCEPIPE_EN_BSET_DMCE_IN_EN_BSET(1)              // Enable DMCE PIPE IN

/// Macro to disable DMCE PIPE IN interrupt at ITM
#   define ITM_DisableInterrupt_DMCEPIPE_IN()                       Set_ITM_ITM_DMCEPIPE_EN_BCLR_DMCE_IN_EN_BCLR(1)              // Disable DMCE PIPE IN

/// Macro to clear DMCE PIPE OUT_STATUS interrupt status at ITM
#   define ITM_ClearInterrupt_DMCEPIPE_OUT()                        Set_ITM_ITM_DMCEPIPE_STATUS_BCLR_DMCE_OUT_STATUS_BCLR(1)     // Clear DMCE PIPE OUT_STATUS

/// Macro to enable DMCE PIPE OUT_STATUS interrupt at ITM
#   define ITM_EnableInterrupt_DMCEPIPE_OUT()                       Set_ITM_ITM_DMCEPIPE_EN_BSET_DMCE_OUT_EN_BSET(1)             // Enable DMCE PIPE OUT_STATUS

/// Macro to disable DMCE PIPE OUT_STATUS interrupt at ITM
#   define ITM_DisableInterrupt_DMCEPIPE_OUT()                      Set_ITM_ITM_DMCEPIPE_EN_BCLR_DMCE_OUT_EN_BCLR(1)             // Disable DMCE PIPE OUT_STATUS

/// Macro to clear DMCE PIPE interrupt status at ITM
#   define ITM_ClearInterrupt_DMCEPIPE_RGB_STORE()                  Set_ITM_ITM_DMCEPIPE_STATUS_BCLR_RGB_STORE_STATUS_BCLR(1)    // Clear DMCE PIPE RGB_STORE

/// Macro to enable DMCE PIPE interrupt at ITM
#   define ITM_EnableInterrupt_DMCEPIPE_RGB_STORE()                 Set_ITM_ITM_DMCEPIPE_EN_BSET_RGB_STORE_EN_BSET(1)            // Enable DMCE PIPE RGB_STORE

/// Macro to disable DMCE PIPE interrupt at ITM
#   define ITM_DisableInterrupt_DMCEPIPE_RGB_STORE()                Set_ITM_ITM_DMCEPIPE_EN_BCLR_RGB_STORE_EN_BCLR(1)            // Disable DMCE PIPE RGB_STORE



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 14 Duster
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Clear interrupt
#   define ITM_Clear_Interrupt_STAT6_Duster()                  Set_ITM_ITM_ARCTIC_STATUS_BCLR(1)    // DUSTER_STATUS_BCLR

/// Enable interrupt
#   define ITM_Enable_Interrupt_STAT6_Duster()                 Set_ITM_ITM_ARCTIC_EN_BSET(1)        // DUSTER_EN_BSET

/// Trigger Interrupt
#   define ITM_TriggerDusterInterrupt()                        Set_ITM_ITM_ARCTIC_STATUS_BSET(0x1)


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 15 Stat7 Acc Wg Zones
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Macro to clear interrupt status at ITM
#   define ITM_Clear_Interrupt_STAT7_ACC_WG_ZONES()            Set_ITM_ITM_EWB_STATUS_BCLR(1, 1, 1, 1)

/// Macro to enable interrupt at ITM
#   define ITM_Enable_Interrupt_STAT7_ACC_WG_ZONES()   Set_ITM_ITM_EWB_EN_BSET(1, 1, 1, 1)

/// Trigger interrupt WB 4x4
#   define ITM_Trigger_Interrupt_STAT7_ACC_WG_ZONES()          Set_ITM_ITM_EWB_STATUS_BSET(1, 0, 0, 0)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 16 Glace (AWB)
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Clear interrupt
#   define ITM_Clear_Interrupt_STAT8_GLACE_AWB()   Set_ITM_ITM_GLACE_STATUS_BCLR(1) // GLACE_STATUS_BCLR

/// Enable interrupt
#   define ITM_Enable_Interrupt_STAT8_GLACE_AWB()  Set_ITM_ITM_GLACE_EN_BSET(1)     // GLACE_EN_BSET

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 17 GPIO (Flash)
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Clear interrupt
#   define ITM_Clear_Interrupt_GPIO()              Set_ITM_ITM_GPIO_STATUS_BCLR_word(0x3fffff) // GPIO_STATUS_BCLR

/// Enable interrupt
#   define ITM_Enable_Interrupt_GPIO()             Set_ITM_ITM_GPIO_EN_BSET_word(0x3fffff)     // GPIO_EN_BSET


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 18 Line BURSTY Elimination
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Clear Line BURSTY Elimination interrupt
#   define ITM_Clear_Interrupt_LINE_BURSTY_ELIM()              Set_ITM_ITM_LINE_BURSTY_ELIM_STATUS_BCLR(1) // LINE_BURSTY_ELIM

/// Enable Line BURSTY Elimination interrupt
#   define ITM_Enable_Interrupt_LINE_BURSTY_ELIM()             Set_ITM_ITM_LINE_BURSTY_ELIM_EN_BSET(1)     // Enable LINE_BURSTY_ELIM

/// Disable Line BURSTY Elimination interrupt
#   define ITM_Disable_Interrupt_LINE_BURSTY_ELIM()            Set_ITM_ITM_LINE_BURSTY_ELIM_EN_BCLR(1)     // Disable LINE_BURSTY_ELIM


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 20 256-bins histogram (AEC) xport Schedule
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Clear interrupt
#   define ITM_Clear_Interrupt_Histogram_Schedule()            ITC_CLEAR_PENDING(IT_NO_HISTOGRAM_SCHEDULE)

/// Enable interrupt
#   define ITM_Enable_Interrupt_Histogram_Schedule()           ITC_SET_PENDING(IT_NO_HISTOGRAM_SCHEDULE)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 23 CRM
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Clear interrupt
//#   define ITM_Clear_Interrupt_CRM()                    Set_CRM_CRM_CHECKER_ITS_BCLR(1, 1, 0, 0)  // CKC_OK_ITS_BCLR,CKC_LOST_ITS_BCLR,CKC_OK_3D_ITS_BCLR,CKC_LOST_3D_ITS_BCLR

/// Enable interrupt on clock detection
#   define ITM_Enable_Clock_Detect_LOSS_Interrupt_CRM()     Set_CRM_CRM_CHECKER_ITM_BSET(1, 1, 0, 0)    // CKC_OK_ITM_BSET,CKC_LOST_ITM_BSET,CKC_OK_3D_ITM_BSET,CKC_LOST_3D_ITM_BSET

/// Disable interrupt on clock detection
#   define ITM_Disable_Clock_Detect_LOSS_Interrupt_CRM()    Set_CRM_CRM_CHECKER_ITM_BCLR(1, 1, 0, 0)    // CKC_OK_ITM_BCLR,CKC_LOST_ITM_BCLR

/// Enable interrupt on clock lost
//#   define ITM_Enable_Clock_Lost_Interrupt_CRM()          Set_CRM_CRM_CHECKER_ITM_BSET(0, 1, 0, 0)    // CKC_OK_ITM_BSET,CKC_LOST_ITM_BSET,CKC_OK_3D_ITM_BSET,CKC_LOST_3D_ITM_BSET

/// Disable interrupt on clock detection
//#   define ITM_Disable_Clock_Detect_Interrupt_CRM()    Set_CRM_CRM_CHECKER_ITM_BCLR(1, 0, 0, 0)   // CKC_OK_ITM_BCLR,CKC_LOST_ITM_BCLR

/// Disable interrupt on clock lost
//#   define ITM_Disable_Clock_Lost_Interrupt_CRM()         Set_CRM_CRM_CHECKER_ITM_BCLR(0, 1, 0, 0)    // CKC_OK_ITM_BCLR,CKC_LOST_ITM_BCLR

#   define ITM_Disable_Clock_Lost_Interrupt_CRM()       Set_CRM_CRM_CHECKER_ITM_BCLR(0, 1, 0, 0)    // CKC_OK_ITM_BCLR,CKC_LOST_ITM_BCLR

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// 27 ERROR_EOF Interrupt Enable / Clear
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/// Enable interrupt
#   define ITM_Enable_Interrupt_CD_ERROR()        Set_IPP_IPP_CD_ERROR_ITM_BSET_CD_ERROR_ITM_BSET(0x07FE)  //To enable Camera Mode CSI Errors
#   define ITM_Enable_Interrupt_CD_RAW_EOF()      Set_IPP_IPP_CD_RAW_EOF_ITM_BSET_CD_RAW_EOF_ITM_BSET(0x01)
#   define ITM_Enable_Interrupt_SD_ERROR()        Set_IPP_IPP_SD_ERROR_ITM_BSET_word(0x0407) // Enabling CSI Packet error and CCP Sync/CRC Interrupt Error
#   define ITM_Enable_CSIPacket_SD_ERROR()        Set_IPP_IPP_CSI2_PACKET_ERROR_ITM_BSET_CSI_PACKET_ERROR_ITM_BSET(0x03) // Enabling CSI 2 Packet Error and EOF Error Interrupts
#   define ITM_Enable_EVT_SD_ERROR()              Set_IPP_IPP_EVT_ITM_BSET_SD_ERROR(1)
#   define ITM_Enable_EVT_CD_ERROR()              Set_IPP_IPP_EVT_ITM_BSET_CD_ERROR(1)
#   define ITM_Enable_EVT_CD_RAW_EOF()            Set_IPP_IPP_EVT_ITM_BSET_CD_RAW_EOF(1)
#   define ITM_Enable_EVT_BML_EOF()               Set_IPP_IPP_EVT_ITM_BSET_BML_EOF(1)

/// Clear interrupt
#   define ITM_Clear_EVT_SD_ERROR()               Set_IPP_IPP_EVT_ITS_BCLR_SD_ERROR(1)
#   define ITM_Clear_EVT_CD_ERROR()               Set_IPP_IPP_EVT_ITS_BCLR_CD_ERROR(1)
#   define ITM_Clear_EVT_CD_RAW_EOF()             Set_IPP_IPP_EVT_ITS_BCLR_CD_RAW_EOF(1)
#   define ITM_Clear_EVT_BML_EOF()                Set_IPP_IPP_EVT_ITS_BCLR_BML_EOF(1)

/// Interrupt Mask for disabling and enabling interrupts.
#   define ITM_INTERRUPT_MASK                    (0x001305E7)

extern Interrupts_test_ts   g_Interrupts_Count;
extern CpuCycleProfiler_ts g_Profile_LCO TO_EXT_DATA_MEM;
extern CpuCycleProfiler_ts g_Profile_ISPUpdate TO_EXT_DATA_MEM;
extern CpuCycleProfiler_ts g_Profile_AEC_Statistics TO_EXT_DATA_MEM;
extern CpuCycleProfiler_ts g_Profile_AWB_Statistics TO_EXT_DATA_MEM;
extern CpuCycleProfiler_ts g_Profile_VID0 TO_EXT_DATA_MEM;
extern CpuCycleProfiler_ts g_Profile_VID1 TO_EXT_DATA_MEM;


#endif //_ITM_H_

