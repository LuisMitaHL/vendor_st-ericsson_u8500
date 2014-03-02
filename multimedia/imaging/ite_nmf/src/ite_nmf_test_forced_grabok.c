/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Test_RGB2YUVCoder_cmd
   PURPOSE  : To tests funcionality of RGB2YUVCoder ISP FW module
   ------------------------------------------------------------------------ */

#include "ite_nmf_test_forced_grabok.h"


extern ts_sia_usecase usecase;


static t_uint32 g_u32_Grab_Abort_Count_Begin = 0;    

static t_uint32 g_u32_GrabOK_Count_Begin = 0;    
static t_uint32 g_u32_Grab_NOK_Count_Begin = 0;

static t_uint32 g_u32_grabok_counter_config = 0;

/*  purpose : This function sets ForceGrabOK configuration begin point
  *  pre-condition : ISP is booted/running
  *  return-value : none
  *  post-condition :  
  *  Important : 
*/

CMD_COMPLETION
C_ite_dbg_Configure_GrabMode_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];

    sprintf(mess, "Forced_Grab_Mode_%d", (int)g_u32_grabok_counter_config);
    g_u32_grabok_counter_config++;

    MMTE_TEST_START(
    mess,
    "/ite_nmf/test_results_nreg/Forced_Grab_Mode",
    "Forced_Grab_Mode");

    sprintf(mess, "Configuring begin point for grab ok mechanism");
    mmte_testNext(mess);    
    
    switch(a_nb_args)
    {       
        case 2:                 // configureForcedGrabMode  <forced_grab_mode>
        {                              
            ConfigureForcedGrabMode(atoi(ap_args[1]));                
            break;
        }

        default:     // wrong syntax : print on stdout how to use test framework for grabOK testing
        {
            LOS_Log("syntax : configureForcedGrabMode  <forced_grab_mode> \n"); 
            break;            
        }                     
    }

    sprintf(mess, "Configuration of forced grab mode mechanism done\n");
    mmte_testComment(mess);
    MMTE_TEST_PASSED();          

    return 0;
}


/*  purpose : This function tests ForcedGrabOK mechanism
  *  pre-condition : ISP is running/stopped
  *  return-value : none
  *  post-condition :  
  *  Important : 
*/


CMD_COMPLETION
C_ite_dbg_Test_Forced_GrabOK_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];
    char    test_name_string[50];
    t_uint32 u32_test_name_id = 0;

    switch(a_nb_args)
    {       
        case 2:                 // TestForcedGrabMode <forced_grab_mode>
        {
            if(GrabMode_e_FORCE_OK == atoi(ap_args[1]))
            {
                LOS_Log("setting test name to Forced_GrabMode_OK\n");
                strcpy(test_name_string, "Test_Forced_GrabMode_OK");
                u32_test_name_id = GrabMode_e_FORCE_OK;
            }    
            else if(GrabMode_e_FORCE_NOK == atoi(ap_args[1]))
            {
                LOS_Log("setting test name to Forced_GrabMode_NOK\n");
                strcpy(test_name_string, "Test_Forced_GrabMode_NOK");    
                u32_test_name_id = GrabMode_e_FORCE_NOK;
            }
            else
            {
                LOS_Log("bad arguments\n");
                return CMD_ERR_ARGS;            
            }
         }
        default:     // wrong syntax : print on stdout how to use test framework for grabOK testing
        {
            LOS_Log("syntax : TestForcedGrabMode <forced_grab_mode>\n"); 
            break;  
         }
         return 0;
     }
    

           

    sprintf(mess, "%s_%d", test_name_string, (int)g_u32_grabok_counter_config);
    g_u32_grabok_counter_config++;

    MMTE_TEST_START(
    mess,
    "/ite_nmf/test_results_nreg/Forced_Grab_Mode",
    "Forced_Grab_Mode");

    sprintf(mess, "Testing grab OK mechanism");
    mmte_testNext(mess);    
                            
    if(GrabMode_e_FORCE_OK == u32_test_name_id)            
    {
        if(0 != TestForcedGrabMode_OK())    
        {
            LOS_Log("Testing of forced grab mode OK mechanism failed\n");
            sprintf(mess, "Testing of forced grab mode OK mechanism failed\n");
            mmte_testComment(mess);
            MMTE_TEST_FAILED();       
            return (CMD_ERR_ARGS);
        }
    }
    else if(GrabMode_e_FORCE_NOK == u32_test_name_id)            
    {
        if(0 != TestForcedGrabMode_NOK())    
        {
            LOS_Log("Testing of forced grab mode NOK mechanism failed\n");
            sprintf(mess, "Testing of forced grab mode NOK mechanism failed\n");
            mmte_testComment(mess);
            MMTE_TEST_FAILED();       
            return (CMD_ERR_ARGS);
        }
     }              
    else
    {      
        while(1)
        {
            LOS_Log("Invalid condition\n");
        }
    }

    sprintf(mess, "Testing of force grab ok mechanism done\n");
    mmte_testComment(mess);
    MMTE_TEST_PASSED();          

    return 0;
}

    

// Note :  ITE_GetEventCount() mechanism can not be used here because it waits for at least one occurrence of specified event.
// Here we dont want to wait for this. We simply want to count no. of events corresponding to Grab_ok and Grab_nok. Grab_nok should be 0
// in success case. In failure case grab_nok will be non zero
    
t_uint32 ConfigureForcedGrabMode(GrabMode_te e_Grab_Mode)
{
    LOS_Log("\n>> ConfigureForcedGrabMode \n");

    // set beginning counter for GRABOK done thus far
    g_u32_GrabOK_Count_Begin = ITE_readPE(Event3_Count_u16_EVENT3_0_DMA_GRAB_OK_Byte0); 

    // set beginning counter for GRAB NOK done thus far
    g_u32_Grab_NOK_Count_Begin = ITE_readPE(Event3_Count_u16_EVENT3_0_DMA_GRAB_NOK_Byte0);     

    // set beginning counter for total no. of bms abort done thus far
    g_u32_Grab_Abort_Count_Begin = ITE_readPE(Event3_Count_u16_EVENT3_2_DMA_GRAB_Abort_Byte0);     

    LOS_Log("g_u32_GrabOK_Count_Begin = %u, g_u32_Grab_NOK_Count_Begin = %u, g_u32_Grab_Abort_Count_Begin = %u\n", g_u32_GrabOK_Count_Begin, g_u32_Grab_NOK_Count_Begin, g_u32_Grab_Abort_Count_Begin);
    
    // now set ForceGrabOk PE to value as indicated by caller
    ITE_writePE(SystemSetup_e_GrabMode_Ctrl_Byte0, e_Grab_Mode);
    
    LOS_Log("<< ConfigureForcedGrabMode \n");

    return 0;
}

t_uint32 TestForcedGrabMode_OK()
{
    
    t_uint32 u32_Grab_Abort_Count_End = 0;
    t_uint32 u32_GrabOK_Count_End = 0;
    t_uint32 u32_Grab_NOK_Count_End = 0;    
    t_sint32 s32_error = 0;
    
    LOS_Log("\n>> TestForcedGrabMode_OK \n");

    // set end counter for GRABOK done thus far
    u32_GrabOK_Count_End = ITE_readPE(Event3_Count_u16_EVENT3_0_DMA_GRAB_OK_Byte0); 

    // set end counter for GRAB NOK done thus far
    u32_Grab_NOK_Count_End = ITE_readPE(Event3_Count_u16_EVENT3_0_DMA_GRAB_NOK_Byte0);      

    // set end counter for total no. of bms aborts done so far
    u32_Grab_Abort_Count_End = ITE_readPE(Event3_Count_u16_EVENT3_2_DMA_GRAB_Abort_Byte0);         

    LOS_Log("u32_GrabOK_Count_End = %u, u32_Grab_NOK_Count_End = %u, u32_Grab_Abort_Count_End = %u\n", u32_GrabOK_Count_End, u32_Grab_NOK_Count_End, u32_Grab_Abort_Count_End);

    if((u32_GrabOK_Count_End > g_u32_GrabOK_Count_Begin) && (u32_Grab_NOK_Count_End == g_u32_Grab_NOK_Count_Begin) && (u32_Grab_Abort_Count_End == g_u32_Grab_Abort_Count_Begin))
    {
        LOS_Log("ForcedGrabMode_OK test is successfull\n");
    }
    else
    {
        LOS_Log("ForcedGrabMode_OK test is failed\n");
        s32_error = -1;
    }
        
    LOS_Log("<< TestForcedGrabMode_OK \n");
    return s32_error;
}

t_uint32 TestForcedGrabMode_NOK()
{
    
    t_uint32 u32_Grab_Abort_Count_End = 0;
    t_uint32 u32_GrabOK_Count_End = 0;
    t_uint32 u32_Grab_NOK_Count_End = 0;    
    t_sint32 s32_error = 0;
    
    LOS_Log("\n>> TestForcedGrabMode_NOK \n");

    // set end counter for GRABOK done thus far
    u32_GrabOK_Count_End = ITE_readPE(Event3_Count_u16_EVENT3_0_DMA_GRAB_OK_Byte0); 

    // set end counter for GRAB NOK done thus far
    u32_Grab_NOK_Count_End = ITE_readPE(Event3_Count_u16_EVENT3_0_DMA_GRAB_NOK_Byte0);      

    // set end counter for total no. of bms aborts done so far
    u32_Grab_Abort_Count_End = ITE_readPE(Event3_Count_u16_EVENT3_2_DMA_GRAB_Abort_Byte0);         

    LOS_Log("u32_GrabOK_Count_End = %u, u32_Grab_NOK_Count_End = %u, u32_Grab_Abort_Count_End = %u\n", u32_GrabOK_Count_End, u32_Grab_NOK_Count_End, u32_Grab_Abort_Count_End);

    if((u32_Grab_NOK_Count_End > g_u32_Grab_NOK_Count_Begin) && (u32_GrabOK_Count_End == g_u32_GrabOK_Count_Begin) && (u32_Grab_Abort_Count_End == g_u32_Grab_Abort_Count_Begin))
    {
        LOS_Log("ForcedGrabMode_NOK test is successfull\n");
    }
    else
    {
        LOS_Log("ForcedGrabMode_NOK test is failed\n");
        s32_error = -1;
    }
        
    LOS_Log("<< TestForcedGrabMode_NOK \n");
    return s32_error;
}

