/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ite_testenv_utils.h"
#include "cli.h"
#include "ite_ispblocks_tests.h"

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

//for NMF
#include "ite_sia_interface_data.h"
#include "ite_sia_bootcmd.h"
#include "ite_init.h"
#include "ite_sia_init.h"

//#include "ite_alloc.h"
#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_colormatrix.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_framerate.h"
#include "ite_sia_buffer.h"

//#include "ite_irq_handler.h"
//#include "ite_ske.h"
#include "ite_buffer_management.h"
#include "grab_types.idt.h"

#include "ite_nmf_standard_functions.h"
#include "ite_nmf_framerate.h"

#include <test/api/test.h>
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_main.h"

#include "ite_host2sensor.h"
#include "hi_register_acces.h"
#include "pictor_full.h"
#include "ite_host2sensor.h"
#include "ite_convf900.h"

extern ts_sia_usecase   usecase;

/* Defines */
#if (defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))

//Linux
#   define GET_PARAM_APPLIED_READ_REG(address, mask) \
        ((IspRegRead_Value((address##_OFFSET), ( long * ) &REG_Read_Val, 1)) & (mask##_MASK)) >> (mask##_OFFSET)
#   define VERIFY_PARAM_APPLIED_READ_REG(address, mask, expectVal)                                      \
    LOS_Log(                                                                                            \
    "|- HV : Reading Register @ 0x%X MASK 0x%X",                                                        \
    (address##_OFFSET),                                                                                 \
    (mask##_MASK));                                                                                     \
    REG_Read_Val = GET_PARAM_APPLIED_READ_REG(address, mask);                                           \
    LOS_Log(" VALUE 0x%X\n", REG_Read_Val);                                                             \
    if ((expectVal) != REG_Read_Val)                                                                    \
    {                                                                                                   \
        LOS_Log("*** Register @ 0x%X MASK 0x%X not applied. ***\n", (address##_OFFSET), (mask##_MASK)); \
        Test_Result = TEST_FAILED;                                                                      \
    }                                                                                                   \
                                                                                                     \
                                                                                                     \
    do                                                                                                  \
    {                                                                                                   \
    } while (FALSE)
#   define VERIFY_PARAM_APPLIED_READ_REG_DEBUG(address, mask, expectVal)                            \
    LOS_Log("***DEBUG MODE ON***\n");                                                               \
LOS_Log("|- HV : Reading Register @ 0x%X MASK 0x%X..\n", (address##_OFFSET), (mask##_MASK));        \
REG_Read_Val = IspRegRead_Value((address##_OFFSET), ( long * ) &REG_Read_Val, 1);                   \
LOS_Log(" VALUE (before mask) = 0x%X\n", REG_Read_Val);                                             \
REG_Read_Val = (REG_Read_Val) & (mask##_MASK);                                                      \
LOS_Log(" VALUE (after mask) = 0x%X\n", REG_Read_Val);                                              \
REG_Read_Val = REG_Read_Val >> (mask##_OFFSET);                                                     \
LOS_Log(" FINAL VALUE (after shift) = 0x%X\n", REG_Read_Val);                                       \
if ((expectVal) != REG_Read_Val)                                                                    \
{                                                                                                   \
    LOS_Log("*** Register @ 0x%X MASK 0x%X not applied. ***\n", (address##_OFFSET), (mask##_MASK)); \
}                                                                                                   \
                                                                                                     \
                                                                                                     \
LOS_Log("***DEBUG MODE OFF***\n");

#else

//Think
#   define GET_PARAM_APPLIED_READ_REG(address, mask)   ((ISP_R_R(address)) & (mask##_MASK)) >> (mask##_OFFSET)
#   define VERIFY_PARAM_APPLIED_READ_REG(address, mask, expectVal)                                      \
    LOS_Log(                                                                                            \
    "|- HV : Reading Register @ 0x%X MASK 0x%X",                                                        \
    (address##_OFFSET),                                                                                 \
    (mask##_MASK));                                                                                     \
    REG_Read_Val = GET_PARAM_APPLIED_READ_REG(address, mask);                                           \
    LOS_Log(" VALUE 0x%X\n", REG_Read_Val);                                                             \
    if ((expectVal) != REG_Read_Val)                                                                    \
    {                                                                                                   \
        LOS_Log("*** Register @ 0x%X MASK 0x%X not applied. ***\n", (address##_OFFSET), (mask##_MASK)); \
        Test_Result = TEST_FAILED;                                                                      \
    }                                                                                                   \
                                                                                                     \
                                                                                                     \
    do                                                                                                  \
    {                                                                                                   \
    } while (FALSE)
#   define VERIFY_PARAM_APPLIED_READ_REG_DEBUG(address, mask, expectVal)                            \
    LOS_Log("***DEBUG MODE ON***\n");                                                               \
LOS_Log("|- HV : Reading Register @ 0x%X MASK 0x%X..\n", (address##_OFFSET), (mask##_MASK));        \
REG_Read_Val = ISP_R_R(address);                                                                    \
LOS_Log(" VALUE (before mask) = 0x%X\n", REG_Read_Val);                                             \
REG_Read_Val = (REG_Read_Val) & (mask##_MASK);                                                      \
LOS_Log(" VALUE (after mask) = 0x%X\n", REG_Read_Val);                                              \
REG_Read_Val = REG_Read_Val >> (mask##_OFFSET);                                                     \
LOS_Log(" FINAL VALUE (after shift) = 0x%X\n", REG_Read_Val);                                       \
if ((expectVal) != REG_Read_Val)                                                                    \
{                                                                                                   \
    LOS_Log("*** Register @ 0x%X MASK 0x%X not applied. ***\n", (address##_OFFSET), (mask##_MASK)); \
}                                                                                                   \
                                                                                                     \
                                                                                                     \
LOS_Log("***DEBUG MODE OFF***\n");
#endif
#define VERIFY_PARAM_APPLIED_READ_PE(x, y)                        \
    if ((y) != ITE_readPE(x))                                     \
    {                                                             \
        LOS_Log("*** Page Element 0x%X not applied. ***\n", (x)); \
        LOS_Log("*** Expected Value was 0x%X ***\n", (y));        \
    }                                                             \
                                                               \
                                                               \
    do                                                            \
    {                                                             \
    } while (FALSE)
#define IS_BML_ACTIVE()                                                                         \
        (                                                                                       \
            (InputImageSource_e_BayerLoad1 == ITE_readPE(SystemSetup_e_InputImageSource_Byte0)) \
        ||  (InputImageSource_e_BayerLoad2 == ITE_readPE(SystemSetup_e_InputImageSource_Byte0)) \
        )
#define DECL_ISPBLOCK_OPERATIONS(ispblockname)                                               \
    static void  Enable_##ispblockname (t_bool);                                             \
static void                                                 Configure_##ispblockname (void); \
static void                                                                                  \
Verify_##ispblockname##_Applied ( void )
#define ADD_ISPBLOCK_ENTRY(ispblockname)                                                                      \
    {                                                                                                         \
        #ispblockname, 1, 1, Enable_##ispblockname, Configure_##ispblockname, Verify_##ispblockname##_Applied \
    }


/* Function Declarations */
DECL_ISPBLOCK_OPERATIONS (RSO);
DECL_ISPBLOCK_OPERATIONS (SDL);

//DECL_ISPBLOCK_OPERATIONS(Gridiron);
DECL_ISPBLOCK_OPERATIONS (ChannelGains);
DECL_ISPBLOCK_OPERATIONS (Scorpio);
DECL_ISPBLOCK_OPERATIONS (Dustor);
DECL_ISPBLOCK_OPERATIONS (BinningRepair);
DECL_ISPBLOCK_OPERATIONS (Babylon);
DECL_ISPBLOCK_OPERATIONS (ColourMatrix);

static void Toggle_System_Coin (void);
static void Wait_After_Toggle (void);

//static void Wait_For_ParamsCommit(void);
static void Configure_All_Blocks (void);
static void Verify_All_Params_Applied (char *);

//static void checkISPBlocks(char* blockname,char* enablebit);

/* Typedefs */
typedef void (*EnableBlockPtr) (t_bool);
typedef void (*ConfigureBlockPtr) ( void );
typedef void (*ParamsAppliedPtr) ( void );

typedef struct
{
    char                *Name;
    t_bool              EnableBit;
    t_bool              TestBit;
    EnableBlockPtr      EnableBlock;
    ConfigureBlockPtr   ConfigureBlock;
    ParamsAppliedPtr    ParamsApplied;
} TIspBlocksStatus;

/* Globals */
TIspBlocksStatus    IspBlocksStatusList[] =
{
    ADD_ISPBLOCK_ENTRY(RSO),
    ADD_ISPBLOCK_ENTRY(SDL),

    //ADD_ISPBLOCK_ENTRY(Gridiron),
    ADD_ISPBLOCK_ENTRY(ChannelGains),
    ADD_ISPBLOCK_ENTRY(Scorpio),
    ADD_ISPBLOCK_ENTRY(Dustor),
    ADD_ISPBLOCK_ENTRY(BinningRepair),
    ADD_ISPBLOCK_ENTRY(Babylon),
    ADD_ISPBLOCK_ENTRY(ColourMatrix)
};

t_uint32            TOTAL_NO_OF_BLOCKS = sizeof(IspBlocksStatusList) / sizeof(IspBlocksStatusList[0]);

t_cmd_list          ite_cmd_list_ispblocksconfig[] =
{
    { "TestIspBlock", C_ite_dbg_TestIspBlock_cmd,
            "TestIspBlock: TestIspBlock {blockname} {0|1} {test|<notest|skip>}\n" },
    { "ConfigureIspBlocks", C_ite_dbg_ConfigureIspBlocks_cmd,
            "ConfigureIspBlocks: ConfigureIspBlocks {toggle|notoggle} {wait|nowait}\n" },
    { "VerifyParamsApplied", C_ite_dbg_VerifyParamsApplied_cmd,
            "VerifyParamsApplied: VerifyParamsApplied {use_case_name} {wait|nowait}\n" },
    { "VerifyGlaceEvent", C_ite_dbg_VerifyGlaceEvent_cmd, "VerifyGlaceEvent: VerifyGlaceEvent}\n" }
};

//volatile t_uint32 REG_enablebit = 0xFF;
volatile t_uint32   REG_Read_Val = 0xFFFF;
static t_uint32     Glace_InterruptCount = 0;
t_test_status       Test_Result = TEST_PASSED;

/* -----------------------------------------------------------------------
FUNCTION : Init_ISP_Blocks_Configure_ITECmdList
PURPOSE  :
------------------------------------------------------------------------ */
void
Init_ISP_Blocks_Configure_ITECmdList(void)
{
    CLI_register_interface(
    "ISPConfig_ITE_CMB",
    sizeof(ite_cmd_list_ispblocksconfig) / (sizeof(ite_cmd_list_ispblocksconfig[0])),
    ite_cmd_list_ispblocksconfig,
    1);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_TestIspBlock_cmd
PURPOSE  : External configuration of Enable/Disable Bits for Isp Blocks
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_TestIspBlock_cmd(
int     a_nb_args,
char    **ap_args)
{
    t_uint32    block_index;

    if ((a_nb_args == 3) || (a_nb_args == 4))
    {
        for (block_index = 0; block_index < TOTAL_NO_OF_BLOCKS; block_index++)
        {
            if (0 == strcmp(ap_args[1], IspBlocksStatusList[block_index].Name))
            {
                //handle enable bit
                IspBlocksStatusList[block_index].EnableBit = ITE_ConvToInt8(ap_args[2]);
                (IspBlocksStatusList[block_index].EnableBlock) (IspBlocksStatusList[block_index].EnableBit);
                LOS_Log("\n");

                //handle test bit, if provided
                if (a_nb_args == 4)
                {
                    if ((0 == strcmp(ap_args[3], "notest")) || (0 == strcmp(ap_args[3], "skip")))
                    {
                        IspBlocksStatusList[block_index].TestBit = 0;
                    }
                    else if (0 == strcmp(ap_args[3], "test"))
                    {
                        IspBlocksStatusList[block_index].TestBit = 1;
                    }


                    //else retain previous value
                }
            }
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        return (CMD_ERR_ARGS);
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_ConfigureIspBlocks_cmd
PURPOSE  :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_ConfigureIspBlocks_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        //handle coin toggle
        if (0 == strcmp(ap_args[1], "notoggle"))
        {
            Configure_All_Blocks();
        }
        else if (0 == strcmp(ap_args[1], "toggle"))
        {
            Configure_All_Blocks();

            ITE_Log_RW_pageelements_disable();
            Toggle_System_Coin();
            ITE_Log_RW_pageelements_enable();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
            return (CMD_ERR_ARGS);
        }


        //handle wait after coin toggle
        if (0 == strcmp(ap_args[2], "nowait"))
        {
            //will be generally chosen when firmware is in STOP state
            /* do nothing */
        }
        else if (0 == strcmp(ap_args[2], "wait"))
        {
            //will generally come here when firmware is in RUN state
            ITE_Log_RW_pageelements_disable();
            Wait_After_Toggle();

            //assuming firrmware is in RUN state. Otherwise, this would be zero.
            Glace_InterruptCount = ITE_readPE(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
            ITE_Log_RW_pageelements_enable();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
            return (CMD_ERR_ARGS);
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        return (CMD_ERR_ARGS);
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_VerifyParamsApplied_cmd
PURPOSE  :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_VerifyParamsApplied_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        if (0 == strcmp(ap_args[2], "nowait"))
        {
            Verify_All_Params_Applied(ap_args[1]);
        }
        else if (0 == strcmp(ap_args[2], "wait"))
        {
            //ITE_Log_RW_pageelements_disable();
            ITE_Log_RW_pageelements_enable();

            Verify_All_Params_Applied(ap_args[1]);
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
            return (CMD_ERR_ARGS);
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        return (CMD_ERR_ARGS);
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_VerifyGlaceEvent_cmd
PURPOSE  :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_VerifyGlaceEvent_cmd(
int     a_nb_args,
char    **ap_args)
{
    //t_uint32  lastGlaceInterruptCount=Glace_InterruptCount;
    t_sint16            err;
    UNUSED(ap_args);
    LOS_Log("\nVerifying Glace Notify. \n");

    if (a_nb_args == 1)
    {
        /*
            while (lastGlaceInterruptCount == Glace_InterruptCount)
            {
              Glace_InterruptCount = ITE_readPE(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
            }
            LOS_Log("%d Glace Notification(s) Received!\n", (Glace_InterruptCount-lastGlaceInterruptCount));
            */
        Glace_InterruptCount = ITE_readPE(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
        LOS_Log("%d Total Glace Interrupts(s) Received!\n", Glace_InterruptCount);

        //METHOD 2: Wait for Glace Stats Event
        ITE_RefreshEventCount(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
        Toggle_System_Coin();

            ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
            err = ITE_GetEventCount(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);

            if (err == -1)
            {
                LOS_Log("EVENT ERROR ENCOUNTERED ,EVENT COUNT NOT EQUAL TO 1\n");
                return (CMD_ERR_ARGS);
            }

        LOS_Log("Glace Notification Received!\n");
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        return (CMD_ERR_ARGS);
    }


    return (CMD_COMPLETE);
}


/*
 * ********************
 * Static Functions now!
 * ********************
 */
static void
Enable_RSO(
t_bool  enableBit)
{
    ITE_writePE(RSO_Control_e_Flag_EnableRSO_Byte0, enableBit);
}


static void
Configure_RSO(void)
{
    ITE_writePE(RSO_Control_e_RSO_Mode_Control_Byte0, RSO_NEW_MODE);        //TBC
    ITE_writePE(RSO_DataCtrl_u32_XCoefGr_Byte0, RSO_NEW_GR_X_COEFF);
    ITE_writePE(RSO_DataCtrl_u32_YCoefGr_Byte0, RSO_NEW_GR_Y_COEFF);
    ITE_writePE(RSO_DataCtrl_u32_XCoefR_Byte0, RSO_NEW_R_X_COEFF);
    ITE_writePE(RSO_DataCtrl_u32_YCoefR_Byte0, RSO_NEW_R_Y_COEFF);
    ITE_writePE(RSO_DataCtrl_u32_XCoefB_Byte0, RSO_NEW_B_X_COEFF);
    ITE_writePE(RSO_DataCtrl_u32_YCoefB_Byte0, RSO_NEW_B_Y_COEFF);
    ITE_writePE(RSO_DataCtrl_u32_XCoefGb_Byte0, RSO_NEW_GB_X_COEFF);
    ITE_writePE(RSO_DataCtrl_u32_YCoefGb_Byte0, RSO_NEW_GB_Y_COEFF);
    ITE_writePE(RSO_DataCtrl_u16_DcTermGr_Byte0, RSO_NEW_GR_DC_TERM);
    ITE_writePE(RSO_DataCtrl_u16_DcTermR_Byte0, RSO_NEW_R_DC_TERM);
    ITE_writePE(RSO_DataCtrl_u16_DcTermB_Byte0, RSO_NEW_B_DC_TERM);
    ITE_writePE(RSO_DataCtrl_u16_DcTermGb_Byte0, RSO_NEW_GB_DC_TERM);
    ITE_writePE(RSO_DataCtrl_u16_XSlantOrigin_Byte0, RSO_NEW_X_SLANT_ORIGIN);
    ITE_writePE(RSO_DataCtrl_u16_YSlantOrigin_Byte0, RSO_NEW_Y_SLANT_ORIGIN);
}


static void
Verify_RSO_Applied(void)
{
    if (Flag_e_TRUE == GET_PARAM_APPLIED_READ_REG(ISP_RSO_SLANT_CTRL, ISP_RSO_SLANT_CTRL_rso_en))
    {
        LOS_Log("RSO: enabled.\n");
        if (RSO_NEW_MODE == RSO_Mode_e_Manual)
        {
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u32_XCoefGr_Byte0, RSO_NEW_GR_X_COEFF);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u32_YCoefGr_Byte0, RSO_NEW_GR_Y_COEFF);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u32_XCoefR_Byte0, RSO_NEW_R_X_COEFF);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u32_YCoefR_Byte0, RSO_NEW_R_Y_COEFF);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u32_XCoefB_Byte0, RSO_NEW_B_X_COEFF);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u32_YCoefB_Byte0, RSO_NEW_B_Y_COEFF);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u32_XCoefGb_Byte0, RSO_NEW_GB_X_COEFF);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u32_YCoefGb_Byte0, RSO_NEW_GB_Y_COEFF);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u16_DcTermGr_Byte0, RSO_NEW_GR_DC_TERM);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u16_DcTermR_Byte0, RSO_NEW_R_DC_TERM);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u16_DcTermB_Byte0, RSO_NEW_B_DC_TERM);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u16_DcTermGb_Byte0, RSO_NEW_GB_DC_TERM);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u16_XSlantOrigin_Byte0, RSO_NEW_X_SLANT_ORIGIN);
            VERIFY_PARAM_APPLIED_READ_PE(RSO_DataStatus_u16_YSlantOrigin_Byte0, RSO_NEW_Y_SLANT_ORIGIN);
        }
        else
        {
            LOS_Log("RSO: RSO not in Manual mode.\n");
        }
    }
    else
    {
        LOS_Log("RSO: disabled.\n");
    }
}


static void
Enable_SDL(
t_bool  enableBit)
{
    ITE_writePE(SDL_Control_e_SDLMode_Control_Byte0, enableBit);
}


static void
Configure_SDL(void)
{
    if (SDL_NEW_ENABLE == SDL_Custom)
    {
        ITE_writePE(SDL_Control_e_SDLMode_Control_Byte0, SDL_NEW_ENABLE);   //TBC
        ITE_writePE(SDL_ELT_u16_LastElementinLUT_GIR_Byte0, SDL_ELT_LASTELEMENTINLUT_GIR_BYTE0);
        ITE_writePE(SDL_ELT_u16_LastElementinLUT_RED_Byte0, SDL_NEW_LASTELEMENTINLUT_RED_BYTE0);
        ITE_writePE(SDL_ELT_u16_LastElementinLUT_BLUE_Byte0, SDL_NEW_LASTELEMENTINLUT_BLUE_BYTE0);
        ITE_writePE(SDL_ELT_u16_LastElementinLUT_GIB_Byte0, SDL_NEW_LASTELEMENTINLUT_GIB_BYTE0);
        ITE_writePE(SDL_ELT_u16_PixelShift_Byte0, SDL_NEW_PIXELSHIFT_BYTE0);
    }
}


static void
Verify_SDL_Applied(void)
{
    if (0 < ITE_readPE(SDL_Status_e_SDLMode_Status_Byte0))
    {
        LOS_Log("SDL: enabled.\n");
        if ((SDL_NEW_ENABLE == SDL_Custom) && (Flag_e_FALSE == ITE_readPE(SDL_Status_e_Flag_SDLUpdatePending_Byte0)))
        {
            //read registers here
            VERIFY_PARAM_APPLIED_READ_REG(
            ISP_FLEXTF_LINEAR_LAST_ELT_GREEN_GIR,
            ISP_FLEXTF_LINEAR_LAST_ELT_GREEN_GIR_last_elt_green_gir,
            SDL_ELT_LASTELEMENTINLUT_GIR_BYTE0);
            VERIFY_PARAM_APPLIED_READ_REG(
            ISP_FLEXTF_LINEAR_LAST_ELT_RED,
            ISP_FLEXTF_LINEAR_LAST_ELT_RED_last_elt_red,
            SDL_NEW_LASTELEMENTINLUT_RED_BYTE0);
            VERIFY_PARAM_APPLIED_READ_REG(
            ISP_FLEXTF_LINEAR_LAST_ELT_BLUE,
            ISP_FLEXTF_LINEAR_LAST_ELT_BLUE_last_elt_blue,
            SDL_NEW_LASTELEMENTINLUT_BLUE_BYTE0);
            VERIFY_PARAM_APPLIED_READ_REG(
            ISP_FLEXTF_LINEAR_LAST_ELT_GIB,
            ISP_FLEXTF_LINEAR_LAST_ELT_GIB_last_elt_gib,
            SDL_NEW_LASTELEMENTINLUT_GIB_BYTE0);
            VERIFY_PARAM_APPLIED_READ_REG(
            ISP_FLEXTF_LINEAR_PIXELIN_SHIFT,
            ISP_FLEXTF_LINEAR_PIXELIN_SHIFT_pixel_shift,
            SDL_NEW_PIXELSHIFT_BYTE0);
        }
        else
        {
            LOS_Log("SDL: Update pending.\n");
        }
    }
    else
    {
        LOS_Log("SDL: disabled.\n");
    }
}


//[SK]New test has been written for Gridiron ite_nmf_gridiron.c

/*static void Enable_Gridiron(t_bool enableBit)
{
  ITE_writePE(GridironControl_e_Flag_Enable_Disable_Gridiron_Byte0, enableBit);
}


static void Configure_Gridiron()
{

  ITE_writePE(GridironControl_e_GridironMode_Byte0, GRIDIRON_NEW_MODE);

  ITE_writePE(GridironControl_f_LiveCast_Byte0, GRIDIRON_NEW_LIVECAST);

  ITE_writePE(GridironControl_f_Phase_Byte0, GRIDIRON_NEW_FPHASE);
  //ITE_writePE(GridironControl_u8_Phase_Byte0, GRIDIRON_NEW_U8_PHASE);

  ITE_writePE(GridironControl_b_EnCast0_Byte0, GRIDIRON_NEW_EN_CAST0);
  ITE_writePE(GridironControl_b_EnCast1_Byte0, GRIDIRON_NEW_EN_CAST1);
  ITE_writePE(GridironControl_b_EnCast2_Byte0, GRIDIRON_NEW_EN_CAST2);
  ITE_writePE(GridironControl_b_EnCast3_Byte0, GRIDIRON_NEW_EN_CAST3);

  ITE_writePE(GridironControl_b_HorFlip_Byte0, GRIDIRON_NEW_VER_FLIP);
  ITE_writePE(GridironControl_b_VerFlip_Byte0, GRIDIRON_NEW_HOR_FLIP);

}


static void Verify_Gridiron_Applied()
{
  //t_uint8 calculated_phase;

  if (Flag_e_TRUE == GET_PARAM_APPLIED_READ_REG(ISP_GRIDIRON_ENABLE, ISP_GRIDIRON_ENABLE_enable_gridiron))
  {
    LOS_Log("Gridiron: enabled.\n");
    //calculated_phase = GRIDIRON_NEW_U8_PHASE; //TBC

    //This register is not shown in T32 SIA peripherals
    //VERIFY_PARAM_APPLIED_READ_REG(ISP_GRIDIRON_LIVE_CAST_PHASE, ISP_GRIDIRON_LIVE_CAST_PHASE_fphase, calculated_phase); //TBC

    VERIFY_PARAM_APPLIED_READ_REG(ISP_GRIDIRON_CTRL, ISP_GRIDIRON_CTRL_en_cast0, GRIDIRON_NEW_EN_CAST0);
    VERIFY_PARAM_APPLIED_READ_REG(ISP_GRIDIRON_CTRL, ISP_GRIDIRON_CTRL_en_cast1, GRIDIRON_NEW_EN_CAST1);
    VERIFY_PARAM_APPLIED_READ_REG(ISP_GRIDIRON_CTRL, ISP_GRIDIRON_CTRL_en_cast2, GRIDIRON_NEW_EN_CAST2);
    VERIFY_PARAM_APPLIED_READ_REG(ISP_GRIDIRON_CTRL, ISP_GRIDIRON_CTRL_en_cast3, GRIDIRON_NEW_EN_CAST3);

    if (IS_BML_ACTIVE())
    {
      VERIFY_PARAM_APPLIED_READ_REG(ISP_GRIDIRON_PIX_ORDER, ISP_GRIDIRON_PIX_ORDER_ver_flip, GRIDIRON_NEW_VER_FLIP);
      VERIFY_PARAM_APPLIED_READ_REG(ISP_GRIDIRON_PIX_ORDER, ISP_GRIDIRON_PIX_ORDER_hor_flip, GRIDIRON_NEW_HOR_FLIP);
    }
  }
  else
  {
    LOS_Log("Gridiron: disabled.\n");
  }
}
*/
static void
Enable_ChannelGains(
t_bool  enableBit)
{
    ITE_writePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, enableBit);
}


static void
Configure_ChannelGains(void)
{
    //nothing else in user's control!
    //TBC
}


static void
Verify_ChannelGains_Applied(void)
{
    if (Flag_e_TRUE == GET_PARAM_APPLIED_READ_REG(ISP_CHG_ENABLE, ISP_CHG_ENABLE_chg_enable))
    {
        LOS_Log("ChannelGains: enabled.\n");
    }
    else
    {
        LOS_Log("ChannelGains: disabled.\n");
    }
}


static void
Enable_Scorpio(
t_bool  enableBit)
{
    ITE_writePE(Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0, enableBit);
}


static void
Configure_Scorpio(void)
{
    ITE_writePE(Scorpio_Ctrl_e_ScorpioMode_Byte0, SCORPIO_NEW_MODE);
    ITE_writePE(Scorpio_Ctrl_u8_CoringLevel_Ctrl_Byte0, SCORPIO_NEW_CORING_LEVEL);
}


static void
Verify_Scorpio_Applied(void)
{
    if (Flag_e_TRUE == GET_PARAM_APPLIED_READ_REG(ISP_SCORPIO_ENABLE, ISP_SCORPIO_ENABLE_scorpio_enable_status))
    {
        LOS_Log("Scorpio: enabled.\n");
        VERIFY_PARAM_APPLIED_READ_PE(Scorpio_Ctrl_u8_CoringLevel_Status_Byte0, SCORPIO_NEW_CORING_LEVEL);
    }
    else
    {
        LOS_Log("Scorpio: disabled.\n");
    }
}


static void
Enable_Dustor(
t_bool  enableBit)
{
    //TBD
    UNUSED(enableBit);
}


static void
Configure_Dustor(void)
{
    //TBD
}


static void
Verify_Dustor_Applied(void)
{
    //TBD
}


static void
Enable_BinningRepair(
t_bool  enableBit)
{
    ITE_writePE(BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0, enableBit);
}


static void
Configure_BinningRepair(void)
{
    ITE_writePE(BinningRepair_Ctrl_e_Flag_H_Jog_Enable_Byte0, BINNING_NEW_REPAIR_H_JOG_ENABLE);
    ITE_writePE(BinningRepair_Ctrl_e_Flag_V_Jog_Enable_Byte0, BINNING_NEW_REPAIR_V_JOG_ENABLE);
    ITE_writePE(BinningRepair_Ctrl_e_BinningRepairMode_Byte0, BINNING_NEW_REPAIR_MODE);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_00_Byte0, BINNING_NEW_REPAIR_COEFF_00);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_01_Byte0, BINNING_NEW_REPAIR_COEFF_01);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_10_Byte0, BINNING_NEW_REPAIR_COEFF_10);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_11_Byte0, BINNING_NEW_REPAIR_COEFF_11);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_shift_Byte0, BINNING_NEW_REPAIR_COEFF_SHIFT);
    ITE_writePE(BinningRepair_Ctrl_u8_BinningRepair_factor_Byte0, BINNING_NEW_REPAIR_FACTOR);
}


static void
Verify_BinningRepair_Applied(void)
{
    if
    (
        Flag_e_TRUE == GET_PARAM_APPLIED_READ_REG(
            ISP_BINNING_REPAIR_ENABLE,
            ISP_BINNING_REPAIR_ENABLE_binning_repair_enable)
    )
    {
        LOS_Log("BinningRepair: enabled.\n");

        VERIFY_PARAM_APPLIED_READ_REG(
        ISP_BINNING_REPAIR_CONTROL,
        ISP_BINNING_REPAIR_CONTROL_br_coef_hjog,
        BINNING_NEW_REPAIR_H_JOG_ENABLE);
        VERIFY_PARAM_APPLIED_READ_REG(
        ISP_BINNING_REPAIR_CONTROL,
        ISP_BINNING_REPAIR_CONTROL_br_coef_vjog,
        BINNING_NEW_REPAIR_V_JOG_ENABLE);

        VERIFY_PARAM_APPLIED_READ_REG(
        ISP_BINNING_REPAIR_COEF00,
        ISP_BINNING_REPAIR_COEF00_br_coef00,
        BINNING_NEW_REPAIR_COEFF_00);
        VERIFY_PARAM_APPLIED_READ_REG(
        ISP_BINNING_REPAIR_COEF01,
        ISP_BINNING_REPAIR_COEF01_br_coef01,
        BINNING_NEW_REPAIR_COEFF_01);
        VERIFY_PARAM_APPLIED_READ_REG(
        ISP_BINNING_REPAIR_COEF10,
        ISP_BINNING_REPAIR_COEF10_br_coef10,
        BINNING_NEW_REPAIR_COEFF_10);
        VERIFY_PARAM_APPLIED_READ_REG(
        ISP_BINNING_REPAIR_COEF11,
        ISP_BINNING_REPAIR_COEF11_br_coef11,
        BINNING_NEW_REPAIR_COEFF_11);

        VERIFY_PARAM_APPLIED_READ_REG(
        ISP_BINNING_REPAIR_COEF_SFT,
        ISP_BINNING_REPAIR_COEF_SFT_br_coef_sft,
        BINNING_NEW_REPAIR_COEFF_SHIFT);
    }
    else
    {
        LOS_Log("BinningRepair: disabled.\n");
    }
}


static void
Enable_Babylon(
t_bool  enableBit)
{
    ITE_writePE(Babylon_Ctrl_e_Flag_BabylonEnable_Byte0, enableBit);
}


static void
Configure_Babylon(void)
{
    ITE_writePE(Babylon_Ctrl_u8_ZipperKill_Byte0, BABYLON_NEW_ZIPPERKILL);
    ITE_writePE(Babylon_Ctrl_u8_Flat_Threshold_Byte0, BABYLON_NEW_FLAT_THRESHOLD);
    ITE_writePE(Babylon_Ctrl_u8_Flat_Threshold_Status_Byte0, BABYLON_NEW_FLAT_THRESHOLD);
    ITE_writePE(Babylon_Ctrl_e_BabylonMode_Byte0, BABYLON_NEW_MODE);
}


static void
Verify_Babylon_Applied(void)
{
    if (Flag_e_TRUE == GET_PARAM_APPLIED_READ_REG(ISP_BABYLON_ENABLE, ISP_BABYLON_ENABLE_enable_babylon))
    {
        LOS_Log("Babylon: enabled.\n");

        VERIFY_PARAM_APPLIED_READ_REG(
        ISP_BABYLON_ZIPPERKILL,
        ISP_BABYLON_ZIPPERKILL_zipperkill,
        BABYLON_NEW_ZIPPERKILL);
        VERIFY_PARAM_APPLIED_READ_REG(ISP_BABYLON_FLAT_TH, ISP_BABYLON_FLAT_TH_flat_th, BABYLON_NEW_FLAT_THRESHOLD);
    }
    else
    {
        LOS_Log("Babylon: disabled.\n");
    }
}


static void
Enable_ColourMatrix(
t_bool  enableBit)
{
    REG_Read_Val = GET_PARAM_APPLIED_READ_REG(ISP_CE0_MATRIX_ENABLE, ISP_CE0_MATRIX_ENABLE_enable); //TBC
    REG_Read_Val = GET_PARAM_APPLIED_READ_REG(ISP_CE1_MATRIX_ENABLE, ISP_CE1_MATRIX_ENABLE_enable); //TBC
    UNUSED(enableBit);
}


static void
Configure_ColourMatrix(void)
{
    volatile float  temp;

    //CE0
    LOS_Log("\nConfiguring for CE0\n");

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_0_REDINRED);
    ITE_writePE(CE_ColourMatrixFloat_0_f_RedInRed_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_0_GREENINRED);
    ITE_writePE(CE_ColourMatrixFloat_0_f_GreenInRed_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_0_BLUEINRED);
    ITE_writePE(CE_ColourMatrixFloat_0_f_BlueInRed_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_0_REDINGREEN);
    ITE_writePE(CE_ColourMatrixFloat_0_f_RedInGreen_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_0_GREENINGREEN);
    ITE_writePE(CE_ColourMatrixFloat_0_f_GreenInGreen_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_0_BLUEINGREEN);
    ITE_writePE(CE_ColourMatrixFloat_0_f_BlueInGreen_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_0_REDINBLUE);
    ITE_writePE(CE_ColourMatrixFloat_0_f_RedInBlue_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_0_GREENINBLUE);
    ITE_writePE(CE_ColourMatrixFloat_0_f_GreenInBlue_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_0_BLUEINBLUE);
    ITE_writePE(CE_ColourMatrixFloat_0_f_BlueInBlue_Byte0, *( volatile t_uint32 * ) &temp);

    //ITE_writePE(CE_ColourMatrixFloat_0_e_SwapColour_Red_Byte0, CE_COLOURMATRIX_NEW_FLOAT_0_SWAPCOLOUR_RED);
    //ITE_writePE(CE_ColourMatrixFloat_0_e_SwapColour_Green_Byte0, CE_COLOURMATRIX_NEW_FLOAT_0_SWAPCOLOUR_GREEN);
    //ITE_writePE(CE_ColourMatrixFloat_0_e_SwapColour_Blue_Byte0, CE_COLOURMATRIX_NEW_FLOAT_0_SWAPCOLOUR_BLUE);
    ITE_writePE(CE_ColourMatrixCtrl_0_s16_Offset_R_Byte0, CE_COLOURMATRIX_NEW_CTRL_0_OFFSET_R);
    ITE_writePE(CE_ColourMatrixCtrl_0_s16_Offset_G_Byte0, CE_COLOURMATRIX_NEW_CTRL_0_OFFSET_G);
    ITE_writePE(CE_ColourMatrixCtrl_0_s16_Offset_B_Byte0, CE_COLOURMATRIX_NEW_CTRL_0_OFFSET_B);
#if 0
    ITE_writePE(CE_ColourMatrixCtrl_0_e_ColourMatrixMode_Byte0, CE_COLOURMATRIX_NEW_0_MODE);

    //checking toggle coin in case of manual mode
    if (ColourMatrixMode_e_Manual == CE_COLOURMATRIX_NEW_0_MODE)
    {
        ITE_writePE(CE_ColourMatrixCtrl_0_e_Coin_Ctrl_Byte0, !(ITE_readPE(CE_ColourMatrixCtrl_0_e_Coin_Ctrl_Byte0)));
    }


#endif

    //CE1
    LOS_Log("\nConfiguring for CE1\n");

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_1_REDINRED);
    ITE_writePE(CE_ColourMatrixFloat_1_f_RedInRed_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_1_GREENINRED);
    ITE_writePE(CE_ColourMatrixFloat_1_f_GreenInRed_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_1_BLUEINRED);
    ITE_writePE(CE_ColourMatrixFloat_1_f_BlueInRed_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_1_REDINGREEN);
    ITE_writePE(CE_ColourMatrixFloat_1_f_RedInGreen_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_1_GREENINGREEN);
    ITE_writePE(CE_ColourMatrixFloat_1_f_GreenInGreen_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_1_BLUEINGREEN);
    ITE_writePE(CE_ColourMatrixFloat_1_f_BlueInGreen_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_1_REDINBLUE);
    ITE_writePE(CE_ColourMatrixFloat_1_f_RedInBlue_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_1_GREENINBLUE);
    ITE_writePE(CE_ColourMatrixFloat_1_f_GreenInBlue_Byte0, *( volatile t_uint32 * ) &temp);

    temp = ( float ) (CE_COLOURMATRIX_NEW_FLOAT_1_BLUEINBLUE);
    ITE_writePE(CE_ColourMatrixFloat_1_f_BlueInBlue_Byte0, *( volatile t_uint32 * ) &temp);

    //ITE_writePE(CE_ColourMatrixFloat_1_e_SwapColour_Red_Byte0, CE_COLOURMATRIX_NEW_FLOAT_1_SWAPCOLOUR_RED);
    //ITE_writePE(CE_ColourMatrixFloat_1_e_SwapColour_Green_Byte0, CE_COLOURMATRIX_NEW_FLOAT_1_SWAPCOLOUR_GREEN);
    //ITE_writePE(CE_ColourMatrixFloat_1_e_SwapColour_Blue_Byte0, CE_COLOURMATRIX_NEW_FLOAT_1_SWAPCOLOUR_BLUE);
    ITE_writePE(CE_ColourMatrixCtrl_1_s16_Offset_R_Byte0, CE_COLOURMATRIX_NEW_CTRL_1_OFFSET_R);
    ITE_writePE(CE_ColourMatrixCtrl_1_s16_Offset_G_Byte0, CE_COLOURMATRIX_NEW_CTRL_1_OFFSET_G);
    ITE_writePE(CE_ColourMatrixCtrl_1_s16_Offset_B_Byte0, CE_COLOURMATRIX_NEW_CTRL_1_OFFSET_B);
#if 0
    ITE_writePE(CE_ColourMatrixCtrl_1_e_ColourMatrixMode_Byte0, CE_COLOURMATRIX_NEW_1_MODE);

    //checking toggle coin in case of manual mode
    if (ColourMatrixMode_e_Manual == CE_COLOURMATRIX_NEW_1_MODE)
    {
        ITE_writePE(CE_ColourMatrixCtrl_1_e_Coin_Ctrl_Byte0, !(ITE_readPE(CE_ColourMatrixCtrl_1_e_Coin_Ctrl_Byte0)));
    }


#endif
}


static void
Verify_ColourMatrix_Applied(void)
{
    t_uint32        DAMPER_FACTOR = 1;
    volatile float  CE_Multiplier = DAMPER_FACTOR * 1024.0;

    //CE0
    if (Flag_e_TRUE == GET_PARAM_APPLIED_READ_REG(ISP_CE0_MATRIX_ENABLE, ISP_CE0_MATRIX_ENABLE_enable))
    {
        LOS_Log("\nCE0 Matrix: enabled. Verifying params now.\n");
        if ((Flag_e_TRUE == ITE_readPE(DataPathStatus_e_Flag_Pipe0Enable_Byte0)))
        {
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_RedInRed_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_0_REDINRED * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_GreenInRed_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_0_GREENINRED * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_BlueInRed_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_0_BLUEINRED * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_RedInGreen_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_0_REDINGREEN * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_GreenInGreen_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_0_GREENINGREEN * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_BlueInGreen_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_0_BLUEINGREEN * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_RedInBlue_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_0_REDINBLUE * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_GreenInBlue_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_0_GREENINBLUE * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_BlueInBlue_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_0_BLUEINBLUE * CE_Multiplier) & 0xFFFF);

            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_Offset_R_Byte0,
            CE_COLOURMATRIX_NEW_CTRL_0_OFFSET_R);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_Offset_G_Byte0,
            CE_COLOURMATRIX_NEW_CTRL_0_OFFSET_G);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_0_s16_Offset_B_Byte0,
            CE_COLOURMATRIX_NEW_CTRL_0_OFFSET_B);
        }
        else
        {
            LOS_Log("CE0 Matrix: Pipe0 disabled.\n");
        }
    }
    else
    {
        LOS_Log("CE0 Matrix: disabled.\n");
    }


    //CE1
    if (Flag_e_TRUE == GET_PARAM_APPLIED_READ_REG(ISP_CE1_MATRIX_ENABLE, ISP_CE1_MATRIX_ENABLE_enable))
    {
        LOS_Log("\nCE1 Matrix: enabled. Verifying params now.\n");

        if ((Flag_e_TRUE == ITE_readPE(DataPathStatus_e_Flag_Pipe1Enable_Byte0)))
        {
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_RedInRed_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_1_REDINRED * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_GreenInRed_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_1_GREENINRED * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_BlueInRed_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_1_BLUEINRED * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_RedInGreen_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_1_REDINGREEN * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_GreenInGreen_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_1_GREENINGREEN * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_BlueInGreen_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_1_BLUEINGREEN * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_RedInBlue_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_1_REDINBLUE * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_GreenInBlue_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_1_GREENINBLUE * CE_Multiplier) & 0xFFFF);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_BlueInBlue_Byte0,
            (t_sint32) (( float ) CE_COLOURMATRIX_NEW_FLOAT_1_BLUEINBLUE * CE_Multiplier) & 0xFFFF);

            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_Offset_R_Byte0,
            CE_COLOURMATRIX_NEW_CTRL_1_OFFSET_R);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_Offset_G_Byte0,
            CE_COLOURMATRIX_NEW_CTRL_1_OFFSET_G);
            VERIFY_PARAM_APPLIED_READ_PE(
            CE_ColourMatrixDamped_1_s16_Offset_B_Byte0,
            CE_COLOURMATRIX_NEW_CTRL_1_OFFSET_B);
        }
        else
        {
            LOS_Log("CE1 Matrix: Pipe1 disabled.\n");
        }
    }
    else
    {
        LOS_Log("CE1 Matrix: disabled.\n");
    }
}


static void
Toggle_System_Coin(void)
{
    Coin_te control_coin,
            status_coin;

    LOS_Log("\nToggle System Coin.\n");

    control_coin = (Coin_te) ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
    status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);

    if (control_coin != status_coin)
    {
        LOS_Log("Coins not in proper state to execute the command.\n");
        LOS_Log("Status and Control Coins should be equal before executing the command.\n");
        return;
    }


    ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !control_coin);
}


static void
Wait_After_Toggle(void)
{
    Coin_te     control_coin,
                status_coin;
    t_uint32    Exposure_InterruptCount;

    LOS_Log("\nWait after Toggle Coin.\n");

    //get current Exposure Interrupt count
    Exposure_InterruptCount = ITE_readPE(Event0_Count_u16_EVENT0_19_EXPOSURE_AND_WB_PARAMETERS_UDPATE_Byte0);

    /* Now wait for exposure event */
    while (Exposure_InterruptCount == ITE_readPE(Event0_Count_u16_EVENT0_19_EXPOSURE_AND_WB_PARAMETERS_UDPATE_Byte0))
    {
    };

    /* Check the status coin */
    control_coin = (Coin_te) ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
    status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);
    if (control_coin == status_coin)
    {
        LOS_Log("Coin toggled!\n");
    }
    else
    {
        LOS_Log("Status coin not updated.\n");
    }
}


static void
Configure_All_Blocks(void)
{
    t_uint32    block_index;

    for (block_index = 0; block_index < TOTAL_NO_OF_BLOCKS; block_index++)
    {
        if ((TRUE == IspBlocksStatusList[block_index].EnableBit) && (TRUE == IspBlocksStatusList[block_index].TestBit))
        {
            //call the configuring function of the corresponding block
            LOS_Log("\nConfiguring %s\n", IspBlocksStatusList[block_index].Name);
            (IspBlocksStatusList[block_index].ConfigureBlock) ();
        }
    }
}


static void
Verify_All_Params_Applied(
char    *usecasename)
{
    t_uint32    block_index;
    char        testComment[200];
    char        pathname[200];
    char        filename[200];
    char        *namesuffix = "_isp_blocks";

    if (usecase.sensor == 0)
    {
        sprintf(filename, "Cam0_");
    }
    else
    {
        sprintf(filename, "Cam1_");
    }


    strcat(filename, usecasename);
    strncat(filename, namesuffix, strlen(namesuffix) + 1);

    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg/isp_blocks_tests");
    strcat(g_out_path, pathname);
    init_test_results_nreg(g_out_path);
    mmte_testStart(filename, "Test the various Pictor blocks", g_out_path);

    for (block_index = 0; block_index < TOTAL_NO_OF_BLOCKS; block_index++)
    {
        if (TRUE == IspBlocksStatusList[block_index].TestBit)
        {
            sprintf(testComment, "Validating %s", IspBlocksStatusList[block_index].Name);
            mmte_testNext(testComment);

            LOS_Log("\nValidating %s\n", IspBlocksStatusList[block_index].Name);
            LOS_Log("\n%s\n", testComment);

            //call the validating function of the corresponding block
            (IspBlocksStatusList[block_index].ParamsApplied) ();
            mmte_testResult(TEST_PASSED);
        }
    }


    mmte_testEnd();
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

