
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Test_RGB2YUVCoder_cmd
   PURPOSE  : To tests funcionality of RGB2YUVCoder ISP FW module
   ------------------------------------------------------------------------ */

#include <ite_nmf_exposure.h>

extern ts_sia_usecase usecase;
extern void                     ITE_NMF_GlaceHistoExpStatsRequestAndWait (void);

static t_uint32 g_u32_exposure_counter_test = 0;    


/*  purpose : This function tests whether current value of PE FrameParamStatus_u32_SensorExposureTimeMax_us_Byte0
  *  is correct or not.
  *  pre-condition : ISP is streaming
  *  return-value : none
  *  post-condition :  
  *  Important : this test case tests exposure time on the basis of framerate exported by ISP FW in FrameParamStatus_u32_frameRate_x100_Byte0 page element.
*/
CMD_COMPLETION
C_ite_dbg_Test_MaxExposureTime_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];
    UNUSED(ap_args);
    
    switch(a_nb_args)
    {       
        case 1:                 // test_rgb2yuvcoder  <pipe_no.> <Transform_Type> <Contrast> <MaxContrast> <ColourSaturation> <MaxColourSaturation>
        {                              
            if(0 != TestMaxExposureTime())
            {
                sprintf(mess, "Testing of maximum exposure time failed\n");
                mmte_testComment(mess);
                MMTE_TEST_FAILED();       
                return (CMD_ERR_ARGS);
            }
            break;
        }

        default:     // wrong syntax : print on stdout how to use test framework for rgb2yuvcoder
        {
            LOS_Log("syntax : testMaxExposureTime \n"); 
            break;            
        }                     
    }

    sprintf(mess, "maximum exposure time testing done\n");
    mmte_testComment(mess);
    MMTE_TEST_PASSED();          

    return 0;
}

/*  purpose : This function tests whether current value of PE FrameParamStatus_u32_SensorExposureTimeMax_us_Byte0
  *  is correct or not.
  *  pre-condition : ISP is streaming
  *  return-value : none
  *  post-condition :  
  *  Important : this test case tests exposure time on the basis of framerate exported by ISP FW in FrameParamStatus_u32_frameRate_x100_Byte0 page element.
*/
CMD_COMPLETION
C_ite_dbg_ToggleSystemCoin_cmd(
int     a_nb_args,
char    **ap_args)
{    
  UNUSED(ap_args);
  
    switch(a_nb_args)
    {       
        case 1:                 //toggleSystemCoin                                      
           ToggleSystemCoin();
            break;
        
        default:     // wrong syntax : print on stdout how to use test framework for rgb2yuvcoder        
            LOS_Log("syntax : toggleSystemCoin \n"); 
            break;                                         
    }

    return 0;
}


/*  purpose : This function is used to set exposure time
  *  pre-condition : 
  *  return-value : 
  *  post-condition :  
  *  Important : 
*/
CMD_COMPLETION
C_ite_dbg_Set_ExposureTime_cmd(
int     a_nb_args,
char    **ap_args)
{
    t_uint32 u32_exposure_time = 0;    
    UNUSED(ap_args);
 
    switch(a_nb_args)
    {       
        case 2:                 // setExposureTime  <exp_time> 
        {    
            u32_exposure_time = atoi(ap_args[1]);
            SetExposureTime(u32_exposure_time);            
            break;
        }

        default:     // wrong syntax : print on stdout how to use test framework for rgb2yuvcoder
        {
            LOS_Log("syntax : setExposureTime  <exp_time_us> \n"); 
            break;            
        }                     
    }

    return 0;
}

/*  purpose : This function is used to get exposure time
  *  pre-condition : 
  *  return-value : 
  *  post-condition :  
  *  Important : 
*/
CMD_COMPLETION
C_ite_dbg_Get_ExposureTime_cmd(
int     a_nb_args,
char    **ap_args)
{
   UNUSED(ap_args);
   
    switch(a_nb_args)
    {       
        case 1:                 // getExposureTime  
        {    
            GetExposureTime();            
            break;
        }

        default:     // wrong syntax : print on stdout how to use test framework for rgb2yuvcoder
        {
            LOS_Log("syntax : getExposureTime \n"); 
            break;            
        }                     
    }

    return 0;
}

void ToggleSystemCoin()
{
    LOS_Log("\n>> ToggleSystemCoin \n");
    
    // now toggle system coin if we are in streaming state
    if(HostInterfaceHighLevelState_e_RUNNING == ITE_readPE(HostInterface_Status_e_HostInterfaceHighLevelState_Byte0))
    {      
        ITE_NMF_GlaceHistoExpStatsRequestAndWait();         
    }
    
    LOS_Log("<< ToggleSystemCoin \n");  
}

void SetExposureTime(t_uint32 u32_exposure_time_us)
{
    LOS_Log("\n>> SetExposureTime.  = %u \n", u32_exposure_time_us);
    ITE_writePE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0, u32_exposure_time_us);    
    LOS_Log("<< SetExposureTime \n");  
}

void GetExposureTime()
{
    t_uint32 u32_exposure_time_us = 0;
    LOS_Log("\n>> GetExposureTime \n");
    u32_exposure_time_us = ITE_readPE(Exposure_ParametersApplied_u32_TotalIntegrationTime_us_Byte0);    
    LOS_Log("<< GetExposureTime, u32_exposure_time_us = %u \n", u32_exposure_time_us);  
}


t_uint32 TestMaxExposureTime()
{
    t_uint32 u32_statusMaxExposureTime = 0;
    t_uint32 u32_calculatedMaxExposureTime = 0;
    t_sint32 s32_returnValue = 0;
    float f_percentage_deviation = 0;
    t_uint32 u32_frameRate_x100 = 0;
    char mess[250];
    char comment[200];    

    // read current framerate
    u32_frameRate_x100 = ITE_readPE(FrameParamStatus_u32_frameRate_x100_Byte0);

    u32_statusMaxExposureTime = ITE_readPE(FrameParamStatus_u32_SensorExposureTimeMax_us_Byte0);
    LOS_Log("\n u32_frameRate_x100 = %u, u32_statusMaxExposureTime = %u \n", u32_frameRate_x100, u32_calculatedMaxExposureTime, u32_statusMaxExposureTime);
    
    LOS_Log("\n>> TestMaxExposureTime \n");
    // calculate theoretical value of exposure time
      u32_calculatedMaxExposureTime = (1.0 / (u32_frameRate_x100 / 100.0 )) * 1000000.0;
      LOS_Log("\nu32_calculatedMaxExposureTime = %u \n",u32_calculatedMaxExposureTime);

      LOS_Log("Testing maximum exposure time for frame rate = %u fps\n", (int)u32_frameRate_x100/100);

    sprintf(mess, "TestMaximumExposureTime_%d_%d_fps", (int)g_u32_exposure_counter_test, (int)u32_frameRate_x100/100);
    g_u32_exposure_counter_test++;

    MMTE_TEST_START(
    mess,
    "/ite_nmf/test_results_nreg/exposure_time",
    "TestMaxExposureTime");

    sprintf(mess, "testing maximum exposure time");
    mmte_testNext(mess);

    // u32_calculatedMaxExposureTime should always be greater than or equal to u32_statusMaxExposureTime
    //percentage deviation between u32_calculatedMaxExposureTime and u32_statusMaxExposureTime should be less than 0.5 %
    f_percentage_deviation = (100.0f*(u32_calculatedMaxExposureTime - u32_statusMaxExposureTime))/(float)u32_calculatedMaxExposureTime;

    if((f_percentage_deviation >= 0) && (f_percentage_deviation <= K_TolerancePercentage))
    {
        sprintf(comment, "value of status of max exposure is within tolerance range\n");
        MMTE_TEST_COMMENT(comment);
        sprintf(comment, "f_percentage_deviation = %f, u32_calculatedMaxExposureTime = %u, u32_statusMaxExposureTime = %u\n", f_percentage_deviation, (unsigned int)u32_calculatedMaxExposureTime, (unsigned int)u32_statusMaxExposureTime);
        MMTE_TEST_COMMENT(comment);
        LOS_Log("TEST PASSED");
        s32_returnValue = 0;
    }
    else
    {
        sprintf(comment, "value of status of max exposure is outside tolerance range\n");
        MMTE_TEST_COMMENT(comment);      
        sprintf(comment, "f_percentage_deviation = %f, u32_calculatedMaxExposureTime = %u, u32_statusMaxExposureTime = %u\n", f_percentage_deviation, (unsigned int)u32_calculatedMaxExposureTime, (unsigned int)u32_statusMaxExposureTime);
        MMTE_TEST_COMMENT(comment);      
       LOS_Log("TEST FAILED");
        s32_returnValue = -1;    
    }
    
    LOS_Log("<< TestMaxExposureTime \n");    
    return s32_returnValue;
}


