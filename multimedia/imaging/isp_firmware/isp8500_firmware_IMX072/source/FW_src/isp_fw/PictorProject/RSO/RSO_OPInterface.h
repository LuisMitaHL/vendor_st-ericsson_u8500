/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
\file RSO_OPInterface.h
\brief Contains the definitions of various blocks used in RSO module.
\ingroup WBStats
*/
#ifndef _RSO_OPINTERFACE_H_
#   define _RSO_OPINTERFACE_H_

#   include "Platform.h"
#   include "RSO_ip_interface.h"

//enum to enable the initialization of the variables either
//the default values or those provided by the host.
typedef enum
{
    /// In manual mode, the values to be loaded in the firmware are provided by the host in the g_RSO_DataCtrl page
    RSO_Mode_e_Manual,

    /// In this mode, the firmware invokes damper on the g_RSO_DataCtrl page
    RSO_Mode_e_Adaptive
} RSO_Mode_te;

/**
 \struct RSO_Control_ts
 \brief
 \ingroup WBStats
*/
typedef struct
{
    /// Whenever this flag is Set it switches on the RSO.
    /// Flag_e_TRUE(1)= Enabled Mode
    /// Flag_e_FALSE(0)= Bypass Mode(In this mode, RSO acts like a simple delay element introducing a delay of one clock cycle between I/P and O/P pixel stream.)
    uint8_t e_Flag_EnableRSO;

    ///Whenever this is set,default values are loaded----AUTOMATIC
    ///when it is zero,values are provided by the host.
    /// [DEFAULT]: 0 (RSO_Mode_e_Manual)
    uint8_t e_RSO_Mode_Control;
} RSO_Control_ts;

/**
 \struct RSO_Data_ts
 \brief
 \ingroup WBStats
*/
typedef struct
{
    ///X Coeff. for Gr Channel (18 bits in hw)
    uint32_t    u32_XCoefGr;

    ///Y Coeff. for Gr Channel
    uint32_t    u32_YCoefGr;

    ///X Coeff. for Rr Channel
    uint32_t    u32_XCoefR;

    ///Y Coeff. for Rr Channel
    uint32_t    u32_YCoefR;

    ///X Coeff. for Bb Channel
    uint32_t    u32_XCoefB;

    ///Y Coeff. for Bb Channel
    uint32_t    u32_YCoefB;

    ///X Coeff. for Gb Channel
    uint32_t    u32_XCoefGb;

    ///Y Coeff. for Gb Channel
    uint32_t    u32_YCoefGb;

    ///DC Term for Gr Channel (12 bits in hw)
    uint16_t    u16_DcTermGr;

    ///DC Term for Rr Channel
    uint16_t    u16_DcTermR;

    ///DC Term for Bb Channel
    uint16_t    u16_DcTermB;

    ///DC Term for Gb Channel
    uint16_t    u16_DcTermGb;

    ///X Slant Origin (12 bits in hw)
    uint16_t    u16_XSlantOrigin;

    ///Y Slant Origin
    uint16_t    u16_YSlantOrigin;
} RSO_Data_ts;

//EXPORTED VARIABLES
extern RSO_Control_ts   g_RSO_Control;

/// programmed by the host
extern RSO_Data_ts      g_RSO_DataCtrl;

/// value committed on to the hardware
extern RSO_Data_ts      g_RSO_DataStatus;
#define RSO_DAMPER_ADDR 1000
// Declare structure for damper
//(u8_DimensionCount, u8_BasisPoints0, u8_BasisPoints1, u8_Parameters, damper_name, damper_addr)

// XCoefGr
//, YCoefGr, DcTermGr, XCoefR, YCoefR, DcTermR, XCoefB, YCoefB, DcTermB, XCoefGb, YCoefGb, DcTermGb

//EXPORTED FUNCTIONS
extern void             RSO_Update (void);
extern void             RSO_Commit (void);

//EXPORTED DEFINES

//Check whih mode is in operation(Automatic or Manual)
#   define RSO_GetMode()                       (g_RSO_Control.e_RSO_Mode_Control)
#   define Set_RSO_Data_DcTermGr(u16_Offset)   (g_RSO_DataStatus.u16_DcTermGr = u16_Offset)
#   define Set_RSO_Data_DcTermR(u16_Offset)    (g_RSO_DataStatus.u16_DcTermR = u16_Offset)
#   define Set_RSO_Data_DcTermB(u16_Offset)    (g_RSO_DataStatus.u16_DcTermB = u16_Offset)
#   define Set_RSO_Data_DcTermGb(u16_Offset)   (g_RSO_DataStatus.u16_DcTermGb = u16_Offset)
#   define RSO_IsDamperEnabled()               (RSO_Mode_e_Adaptive == g_RSO_Control.e_RSO_Mode_Control)
#endif // _RSO_OPINTERFACE_H_

