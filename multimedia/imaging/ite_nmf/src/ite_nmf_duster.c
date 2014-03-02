/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ite_nmf_sensor_tunning_functions.h"
#include "ite_nmf_statistic_functions.h"
#include "ite_testenv_utils.h"

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

//for NMF
#include "ite_sia_interface_data.h"
#include "ite_init.h"
#include "ite_sia_init.h"
//#include "ite_alloc.h"
#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_sia_buffer.h"
#include "ite_host2sensor.h"
#include "ite_sensorinfo.h"

#include <test/api/test.h>
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_sia_buffer.h"


extern ts_sia_usecase usecase;
extern void                     ITE_NMF_GlaceHistoExpStatsRequestAndWait (void);

static t_uint32 g_u32_counter_duster_conf = 0;
static t_uint32 g_u32_counter_duster_test = 0;


/*
t_cmd_list  ite_cmd_list_Duster[] =
{  
    { "ConfigureDuster", C_ite_dbg_ConfigureDuster_cmd, "ConfigureDuster : Configures grid iron \n" }    
    { "TestDuster", C_ite_dbg_TestDuster_cmd, "TestDuster : Tests Duster \n" }    
};
*/


static void Enable_Duster(t_bool enableBit)
{  
    ITE_writePE(DusterControl_e_Flag_DusterEnable_Byte0, enableBit);

    if(HostInterfaceHighLevelState_e_RUNNING == ITE_readPE(HostInterface_Status_e_HostInterfaceHighLevelState_Byte0))
    {      
        ITE_NMF_GlaceHistoExpStatsRequestAndWait();   
    }    
    
}

static void Configure_Duster()
{  
    const t_uint32 frameSigma  = 128;
    const t_uint32 dusterBypassControl = 0x17F;    
    const t_uint32 dusterBypassScytheControl_hi = 0x35;
    const t_uint32 dusterBypassScytheControl_lo = 0x6D; 
    const t_uint32 GaussianWeight = 2; 
    const t_uint32 SigmaWeight = 2;
    const t_uint32 centreCorrection = 5; 
    const t_uint32 ringCorrection = 4;    

    ITE_writePE(DusterControl_u16_FrameSigma_Byte0, frameSigma);     
    ITE_writePE(DusterControl_u16_Duster_ByPass_Ctrl_Byte0, dusterBypassControl);      
    ITE_writePE(DusterControl_u8_GaussianWeight_Byte0, GaussianWeight);  
    ITE_writePE(DusterControl_u8_SigmaWeight_Byte0, SigmaWeight);  
    ITE_writePE(DusterControl_u8_ScytheControl_lo_Byte0, dusterBypassScytheControl_lo);    
    ITE_writePE(DusterControl_u8_ScytheControl_hi_Byte0, dusterBypassScytheControl_hi);  
    ITE_writePE(DusterControl_u8_CenterCorrectionSigmaFactor_Byte0, centreCorrection);    
    ITE_writePE(DusterControl_u8_RingCorrectionNormThr_Byte0, ringCorrection);      
    ITE_writePE(DusterControl_e_Flag_DusterEnable_Byte0, Flag_e_TRUE);  
}


static int Verify_Duster()
{
    unsigned int expected_frameSigma = 128;  
    unsigned int expected_Gaussian_Th1 = 2;  
    unsigned int expected_Gaussian_Th2 = 4;  
    unsigned int expected_Gaussian_Th3 = 6;  
    unsigned int expected_scytheRankHi = 3; 
    unsigned int expected_scytheRankLo = 6;   
    unsigned int expected_scytheSmoothControlHi = 5; 
    unsigned int expected_scytheSmoothControlLo = 0xD; 
    unsigned int expected_ccLocalSigma_Th = 5; 
    unsigned int expected_rcNorm_Th = 4;     
    unsigned int expected_SigmaGaussian = 16; 
    unsigned int expected_gaussianWeight = 2;   

    unsigned int frameSigma = 0;  
    unsigned int Gaussian_Th1 = 0;  
    unsigned int Gaussian_Th2 = 0;  
    unsigned int Gaussian_Th3 = 0;  
    unsigned int scytheRankHi = 0; 
    unsigned int scytheRankLo = 0;   
    unsigned int scytheSmoothControlHi = 0; 
    unsigned int scytheSmoothControlLo = 0; 
    unsigned int ccLocalSigma_Th = 0; 
    unsigned int rcNorm_Th = 0;     
    unsigned int SigmaGaussian = 0; 
    unsigned int gaussianWeight = 0;     
    char comment[200];    

    int errorCount = 0;

    if (ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0) != ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0))
    {
       LOS_Log("Inconsistent state of system coin\n");
    }

    if (Flag_e_TRUE == ITE_readPE(DusterControl_e_Flag_DusterEnable_Byte0))  
    {  
      LOS_Log("Duster is in enabled state\n");
      frameSigma = ITE_readPE(DusterStatus_u16_FrameSigma_Byte0);      
      Gaussian_Th1 = ITE_readPE(DusterStatus_u16_Gaussian_Th1_Byte0);
      Gaussian_Th2 = ITE_readPE(DusterStatus_u16_Gaussian_Th2_Byte0);
      Gaussian_Th3 = ITE_readPE(DusterStatus_u16_Gaussian_Th3_Byte0);
      scytheRankHi = ITE_readPE(DusterStatus_u8_ScytheRank_hi_Byte0);
      scytheRankLo = ITE_readPE(DusterStatus_u8_ScytheRank_lo_Byte0);
      scytheSmoothControlHi = ITE_readPE(DusterStatus_u8_ScytheSmoothControl_hi_Byte0);
      scytheSmoothControlLo = ITE_readPE(DusterStatus_u8_ScytheSmoothControl_lo_Byte0);   
      ccLocalSigma_Th = ITE_readPE(DusterStatus_u8_CCLocalSigma_Th_Byte0);
      rcNorm_Th = ITE_readPE(DusterStatus_u8_RCNorm_Th_Byte0);         
      SigmaGaussian = ITE_readPE(DusterStatus_u8_SigmaGaussian_Byte0);
      gaussianWeight = ITE_readPE(DusterStatus_u8_GaussianWeight_Byte0);      

      if(frameSigma != expected_frameSigma)
      {
          errorCount++; 
          sprintf(comment,"ERR : frameSigma Mismatch. frameSigma = %u, expected_frameSigma = %u\n\n",frameSigma, expected_frameSigma);
          MMTE_TEST_COMMENT(comment);                
      } 
      
      if(Gaussian_Th1 != expected_Gaussian_Th1)
      {
        errorCount++; 
        sprintf(comment,"ERR : Gaussian_Th1 Mismatch. Gaussian_Th1 = %u, expected_Gaussian_Th1 = %u\n\n",Gaussian_Th1, expected_Gaussian_Th1);
        MMTE_TEST_COMMENT(comment);          
      }     
      
      if(Gaussian_Th2 != expected_Gaussian_Th2)
      {
        errorCount++; 
        sprintf(comment,"ERR : Gaussian_Th2Mismatch. Gaussian_Th2 = %u, expected_Gaussian_Th2 = %u\n\n",Gaussian_Th2, expected_Gaussian_Th2);
        MMTE_TEST_COMMENT(comment);             
      } 

       if(Gaussian_Th3 != expected_Gaussian_Th3)
       {
        errorCount++; 
        sprintf(comment,"ERR : Gaussian_Th2Mismatch. Gaussian_Th3 = %u, expected_Gaussian_Th3 = %u\n\n",Gaussian_Th3, expected_Gaussian_Th3);
        MMTE_TEST_COMMENT(comment);              
       } 
      
      if(scytheRankHi != expected_scytheRankHi)
      {
        errorCount++; 
        sprintf(comment,"ERR : scytheRankHi Mismatch. scytheRankHi = %u, expected_scytheRankHi = %u\n\n",scytheRankHi, expected_scytheRankHi);
        MMTE_TEST_COMMENT(comment);         
      }     
      
      if(scytheRankLo != expected_scytheRankLo)
      {
        errorCount++; 
        sprintf(comment,"ERR : scytheRankLo Mismatch. scytheRankLo = %u, expected_scytheRankLo = %u\n\n",scytheRankLo, expected_scytheRankLo);
        MMTE_TEST_COMMENT(comment);              
      } 

      if(scytheSmoothControlHi != expected_scytheSmoothControlHi)
      {
        errorCount++;
        sprintf(comment,"ERR : scytheSmoothControlHi Mismatch. scytheRankLo = %u, expected_scytheRankLo = %u\n\n",scytheSmoothControlHi, expected_scytheSmoothControlHi);
        MMTE_TEST_COMMENT(comment);               
      } 
      
      if(scytheSmoothControlLo != expected_scytheSmoothControlLo)
      {
        errorCount++;
        sprintf(comment,"ERR : scytheSmoothControlLo Mismatch. scytheSmoothControlLo = %u, expected_scytheSmoothControlLo = %u\n\n",scytheSmoothControlLo, expected_scytheSmoothControlLo);
        MMTE_TEST_COMMENT(comment);                
      }      
      
      if(ccLocalSigma_Th != expected_ccLocalSigma_Th)
      {
        errorCount++;
        sprintf(comment,"ERR : ccLocalSigma_Th Mismatch. ccLocalSigma_Th = %u, expected_ccLocalSigma_Th = %u\n\n",ccLocalSigma_Th, expected_ccLocalSigma_Th);
        MMTE_TEST_COMMENT(comment);            
      } 
      
      if(rcNorm_Th != expected_rcNorm_Th)
      {
        errorCount++;
        sprintf(comment,"ERR : rcNorm_Th Mismatch. rcNorm_Th = %u, expected_rcNorm_Th = %u\n\n",rcNorm_Th, expected_rcNorm_Th);
        MMTE_TEST_COMMENT(comment);             
      } 
      
      if(SigmaGaussian != expected_SigmaGaussian)
      {
        errorCount++;
        sprintf(comment,"ERR : SigmaGaussian Mismatch. SigmaGaussian = %u, expected_SigmaGaussian = %u\n\n",SigmaGaussian, expected_SigmaGaussian);
        MMTE_TEST_COMMENT(comment);            
      }    
      
      if(gaussianWeight != expected_gaussianWeight)
      {
        errorCount++;
        sprintf(comment,"ERR : gaussianWeight Mismatch. gaussianWeight = %u, expected_gaussianWeight = %u\n\n",gaussianWeight, expected_gaussianWeight);
        MMTE_TEST_COMMENT(comment);             
      }      

      LOS_Log("frameSigma = %u\n", frameSigma);      
      LOS_Log("Gaussian_Th1 = %u\n", Gaussian_Th1);
      LOS_Log("Gaussian_Th2 = %u\n", Gaussian_Th2);
      LOS_Log("Gaussian_Th3 = %u\n", Gaussian_Th3);
      LOS_Log("scytheRankHi = %u\n", scytheRankHi);      
      LOS_Log("scytheRankLo = %u\n", scytheRankLo);
      LOS_Log("scytheSmoothControlHi = %u\n", scytheSmoothControlHi);      
      LOS_Log("scytheSmoothControlLo = %u\n", scytheSmoothControlLo);
      LOS_Log("ccLocalSigma_Th = %u\n", ccLocalSigma_Th);
      LOS_Log("rcNorm_Th = %u\n", rcNorm_Th);     
      LOS_Log("SigmaGaussian = %u\n", SigmaGaussian);
      LOS_Log("gaussianWeight = %u\n", gaussianWeight);           
    }
    else
    {
        errorCount++;
        sprintf(comment,"ERR : Duster must be enabled before doing verification. \n\n");
        MMTE_TEST_COMMENT(comment);             
    }        

    return errorCount;

  }

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_ConfigureDuster_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_ConfigureDuster_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];

    sprintf(mess, "ConfigureDuster_%d", (int)g_u32_counter_duster_conf);
    g_u32_counter_duster_conf++;    
    
    MMTE_TEST_START(
    mess,
    "/ite_nmf/test_results_nreg/Duster_tests",
    "Test ConfigureDuster");

    sprintf(mess, "Testing for Duster peaking");
    mmte_testNext(mess);

    LOS_Log("************* DOING DUSTER CONFIGURATION  ****************\n\n");
    
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {           
            CLI_disp_error("Syntax: ConfigureDuster , Configures Duster interface provided by ISP FW \n");                                 
        }
   
    }
    else if (a_nb_args == 1)
    {
        Configure_Duster();
        Enable_Duster(Flag_e_TRUE);           
        LOS_Sleep(100);              
    }    


    LOS_Log("\n********************* DUSTER CONFIGURATION DONE *******************\n");

    MMTE_TEST_PASSED();
    return 0;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_VerityDuster_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_VerityDuster_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];
    int error = 0;

    sprintf(mess, "VerityDuster_%d", (int)g_u32_counter_duster_test);
    g_u32_counter_duster_test++;    
    
    MMTE_TEST_START(
    mess,
    "/ite_nmf/test_results_nreg/Duster_tests",
    "Test VerifyDuster");

    sprintf(mess, "Verifying Duster configuration");
    mmte_testNext(mess);

    LOS_Log("********************* VERIFY DUSTER START **********************\n");
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {                         
            CLI_disp_error("Syntax: VerifyDuster, Tests Duster interface provided by ISP FW \n");         
        }        
    }
    else if (a_nb_args == 1)
    {
        error = Verify_Duster();
    }

    LOS_Log("********************* DUSTER VERIFICATION DONE **********************\n");

   if(0 == error)
   {      
        snprintf(mess,sizeof(mess),"Duster verification passed\n");
        MMTE_TEST_COMMENT(mess);
        MMTE_TEST_PASSED();
        return (CMD_COMPLETE);       
   }
   else
   {
        snprintf(mess,sizeof(mess),"Duster verification failed\n");
        MMTE_TEST_COMMENT(mess);        
        MMTE_TEST_FAILED();
        return (CMD_COMPLETE);    
   }
   return (CMD_COMPLETE);
}



