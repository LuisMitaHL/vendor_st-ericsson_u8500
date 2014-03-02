/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "ite_framerate.h"
#include "ite_vpip.h"
#include "ispctl_types.idt.h"
#include "los/api/los_api.h"

/********************************************************/
/* 		Private functions declaration 		*/
/********************************************************/

/********************************************************/
/* 		"Public" functions 	 		*/
/********************************************************/

static t_uint32 g_u32_counter_test_VFR = 0;

//ported on 8500
void ITE_SetFrameRateControl(float curr_fps, float max_fps)
{
    t_uint32    u32_mode = 0;
    u32_mode = ITE_readPE(VariableFrameRateControl_e_Flag_Byte0);
    if (Flag_e_FALSE == u32_mode)
    {
        ITE_writePE(FrameRateControl_f_UserMaximumFrameRate_Hz_Byte0, *( volatile t_uint32 * ) &curr_fps);
        LOS_Log("Frame Rate = %f fr/sec \n", curr_fps);
    }
    else
    {
        ITE_writePE(VariableFrameRateControl_f_CurrentFrameRate_Hz_Byte0, *( volatile t_uint32 * ) &curr_fps);
        ITE_writePE(VariableFrameRateControl_f_MaximumFrameRate_Hz_Byte0, *( volatile t_uint32 * ) &max_fps);
        LOS_Log(" Curr Frame Rate = %f fr/sec Max Frame rate = %f fr/sec \n", curr_fps, max_fps);
    }
}

float ITE_FrameRateProgrammed()
{
    float    f_fps;
    t_uint32 u32_mode;

    u32_mode = ITE_readPE(VariableFrameRateControl_e_Flag_Byte0);

    if(Flag_e_FALSE == u32_mode)
    {
        f_fps = ITE_float_readPE(FrameDimensionStatus_f_CurrentFrameRate_Byte0);
    }
    else
    {
        f_fps = ITE_float_readPE(VariableFrameRateStatus_f_CurrentFrameRate_Hz_Byte0);
    }
    LOS_Log("fps programmed = %f\n",f_fps);
    return f_fps;
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_VariableFrameRate_SetMode_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_VariableFrameRate_SetMode_cmd(
int     a_nb_args,
char    **ap_args)
{
    t_uint32 u32_flag = 0;
    LOS_Log("\n>> C_ite_dbg_VariableFrameRate_Set_cmd");

    switch(a_nb_args)
    {       
        case 2:                 // VariableFramerate_Set  <0/1> 
        {
            u32_flag = atoi(ap_args[1]);
            LOS_Log("2 command line arguments\n"); 
            VariableFrameRate_Set(u32_flag);
            break;
        }

        default:     // wrong syntax : print on stdout how to use VariableFrameRate
        {
            LOS_Log("syntax : VariableFramerate_Set  <0/1> \n");
            break;
        }
    }

    LOS_Log("\n<< C_ite_dbg_VariableFrameRate_Set_cmd\n");
    return (CMD_COMPLETE);
}

void VariableFrameRate_Set(t_uint32 u32_flag)
{
    LOS_Log("\n>> VariableFrameRate_Set : u32_flag = %d\n", u32_flag);

    ITE_writePE(VariableFrameRateControl_e_Flag_Byte0, u32_flag);

    LOS_Log("\n<< VariableFrameRate_Set\n");
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_VariableFrameRate_TestVariableFrameRate_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_VariableFrameRate_TestVariableFrameRate_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];
    UNUSED(ap_args);
    sprintf(mess, "TestVariableFrameRate_%d", (int)g_u32_counter_test_VFR);
    g_u32_counter_test_VFR++;

    MMTE_TEST_START(
    mess,
    "/ite_nmf/test_results_nreg/TestVariableFrameRate",
    "TestVariableFrameRate");

    sprintf(mess, "Testing for variable framerate");
    mmte_testNext(mess);

    LOS_Log("\n>> C_ite_dbg_VariableFrameRate_TestVariableFrameRate_cmd");

    switch(a_nb_args)
    {
        case 1:                 // VariableFrameRate_TestVariableFrameRate  
        {  
            LOS_Log("1 command line arguments\n");
            if(0 == VariableFrameRate_TestVariableFrameRate())
            {
                sprintf(mess, "Variable framerate testing completed successfully\n");
                MMTE_TEST_COMMENT(mess);
                MMTE_TEST_PASSED();
                return (CMD_COMPLETE);
            }
            break;
        }

        default:     // wrong syntax : print on stdout how to use VariableFrameRate
        {
            LOS_Log("syntax : VariableFrameRate_TestVariableFrameRate  \n");
            break;
        }
    }

    sprintf(mess, "Variable framerate testing failed\n");
    MMTE_TEST_COMMENT(mess);
    MMTE_TEST_FAILED();
    LOS_Log("\n<< C_ite_dbg_VariableFrameRate_TestVariableFrameRate_cmd\n");
    return (CMD_COMPLETE);
}

t_uint32 VariableFrameRate_TestVariableFrameRate()
{
    float f_fdm_framerate = 0 ,f_curr_framerate =0;
    char comment[200];
    float f_temp1 = 0;
    float f_temp2 = 0;

    // Test to check if fps has changed during streaming when using variable framerate.mode is used
    LOS_Log("\n>> VariableFrameRate_TestVariableFrameRate\n");

    f_fdm_framerate = ITE_float_readPE(FrameDimensionStatus_f_CurrentFrameRate_Byte0);


    f_curr_framerate = ITE_float_readPE(VariableFrameRateStatus_f_CurrentFrameRate_Hz_Byte0);


    sprintf(comment, "framerate read from frame dimension = %f, frame rate set by host = %f\n", f_fdm_framerate, f_curr_framerate);
    MMTE_TEST_COMMENT(comment);

    if(f_fdm_framerate > f_curr_framerate )
    {
        f_temp1 = f_fdm_framerate;
        f_temp2 = f_curr_framerate;
    }
    else
    {
        f_temp1 = f_curr_framerate;
        f_temp2 = f_fdm_framerate;
    }

    if(((f_temp1 - f_temp2)*100)/f_temp2 > 2.0)
    {
        return 1;        // ERR case
    }

    return 0;
    LOS_Log("\n<< VariableFrameRate_TestVariableFrameRate\n");    
}





