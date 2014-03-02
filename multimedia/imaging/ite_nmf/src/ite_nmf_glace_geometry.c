#include "ite_nmf_glace_geometry.h"

#include "pictor_full.h"
#include "ite_platform.h"

static char    arr_configuration_string[200];     
static t_uint32 g_u32_Test_Glace_Geometry_test_counter = 0;
static t_uint32 g_u32_Set_Glace_Geometry_test_counter = 0;
static t_uint32 g_u32_Get_Glace_Geometry_Mode_test_counter = 0;
static t_uint32 g_u32_Set_bms_size_counter = 0;

extern t_uint32 REG_Read_Val;

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Set_Glace_Geometry_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Set_Glace_Geometry_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[200];
    
    sprintf(mess, "setGlaceGeometry_%d", (int)g_u32_Set_Glace_Geometry_test_counter);
    g_u32_Set_Glace_Geometry_test_counter++;   

   
    MMTE_TEST_START(
    mess,
    "/ite_nmf/test_results_nreg/Glace_geometry",
    "setGlaceGeometry");

    sprintf(mess, "Set glace geometry mode");
    mmte_testNext(mess);    
   
    switch(a_nb_args)
    {
        case 8:
            {
                Set_Glace_Geometry(atoi(ap_args[1]), atof(ap_args[2]), atof(ap_args[3]), atof(ap_args[4]), atof(ap_args[5]), atoi(ap_args[6]), atoi(ap_args[7]));                     
                
                sprintf(arr_configuration_string, "mode_%d_block_size_%.3f_%.3f_ROI_fraction_%.3f_%.3f_gridSize_%d_%d", atoi(ap_args[1]), atof(ap_args[2]), atof(ap_args[3]), 
                        atof(ap_args[4]), atof(ap_args[5]), atoi(ap_args[6]), atoi(ap_args[7]));                  
                
                LOS_Log("test configuration is : %s\n", arr_configuration_string);                
                break;
            }
            
        default:        
            {    
                LOS_Log("Syntax: setGlaceGeometry  <mode> <f_hBlockSizeFrac> <f_vBlockSizeFrac> <f_hROIStartFrac> <f_vROIStartFrac> <hGridSize> <vGridSize>, sets value of glace geometry mode \n");                                                 
                
                break;
            }
    }

    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);    
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Get_Glace_Geometry_Mode_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Get_Glace_Geometry_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[200];
    UNUSED(ap_args);

    sprintf(mess, "GetGlaceGeometryMode_%d", (int)g_u32_Get_Glace_Geometry_Mode_test_counter);
    g_u32_Get_Glace_Geometry_Mode_test_counter++;
    
    MMTE_TEST_START(
    mess,
    "/ite_nmf/test_results_nreg/Glace_geometry",
    "GetGlaceGeometryMode");

    sprintf(mess, "Get glace geometry mode test");
    mmte_testNext(mess);    
   
    switch(a_nb_args)
    {
        case 1:
        {
            Get_Glace_Geometry();                     
            break;
        }
            
        default:        
        {    
            LOS_Log("Syntax: getGlaceGeometry , prints value of glace geometry mode \n");                                                 
            break;
        }
    }

    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);    
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Set_BMS_Size_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Set_BMS_Size_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[200];

    
    sprintf(mess, "setBmsSize_%d", (int)g_u32_Set_bms_size_counter);
    g_u32_Set_bms_size_counter++;
    
    MMTE_TEST_START(
    mess,
    "/ite_nmf/test_results_nreg/set_bms_size",
    "set_bms_size");

    sprintf(mess, "Set bms size");
    mmte_testNext(mess);    
   
    switch(a_nb_args)
    {
        case 5:
        {
            Set_bms_size(atoi(ap_args[1]), atoi(ap_args[2]), atoi(ap_args[3]), atoi(ap_args[4]));                     
            break;
        }
            
        default:        
        {    
            LOS_Log("Syntax: setbmssize <woi_x> <woi_y> <out_res_x> <out_res_y> , sets bms capture size \n");                                                 
            break;
        }
    }

    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);    
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Test_Glace_Geometry_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Test_Glace_Geometry_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[200];
    t_uint32       u32_result = 0;
    UNUSED(ap_args);

    /*
    sprintf(mess, "VerifyGlaceGeometry_%d", (int)g_u32_Test_Glace_Geometry_test_counter);
    */
    
   
    /*
    sprintf(arr_configuration_string, "%d_mode_%d_block_size_%s_%s_ROI_fraction_%s_%s_gridSize_%s_%s", g_u32_Test_Glace_Geometry_test_counter, ap_args[1], ap_args[2], ap_args[3], ap_args[4], 
                            ap_args[5], ap_args[6], ap_args[7]);    
    */
    
    sprintf(mess, "TestGlaceGeometry_%d", (unsigned int)g_u32_Test_Glace_Geometry_test_counter);        
    g_u32_Test_Glace_Geometry_test_counter++;   
    
    MMTE_TEST_START(
    mess,
    "/ite_nmf/test_results_nreg/Glace_geometry",
    "Test Glace_geometry_tests");

    sprintf(mess, "Testing for glace geometry test\n");
    MMTE_TEST_NEXT(mess);

    MMTE_TEST_COMMENT(arr_configuration_string);
    
    LOS_Log("******************* Glace geometry testing start ****************\n");
    
    switch(a_nb_args)
    {
        case 1:
            {
                u32_result = Test_Glace_Geometry();                     
                break;
            }
            
        default:        
            {    
                LOS_Log("Syntax: testGlaceGeometry , Tests correctness of glace geometry related PEs \n");                                                 
                break;
            }
    }

    LOS_Log("************************* Glace geometry testing end**********************\n");
    
    if(0 == u32_result)
    {
        LOS_Log("Glace geometry test passed.\n");
        MMTE_TEST_PASSED();
        return (CMD_COMPLETE);        
    }

     LOS_Log("Glace geometry test failed.\n");
    MMTE_TEST_FAILED();
    return (CMD_COMPLETE);    
}


/* -----------------------------------------------------------------------
   FUNCTION : Get_Glace_Geometry_Mode
   PURPOSE  :
   ------------------------------------------------------------------------ */
void Get_Glace_Geometry()
{
    t_uint32 u32_glace_geometry_mode = 0;
    
    t_uint32 u32_hBlockSize = 0;
    t_uint32 u32_vBlockSize = 0;
    
    t_uint32 u32_hROIStart = 0;
    t_uint32 u32_vROIStart = 0;
    
    t_uint32 u32_hGridSize = 0;
    t_uint32 u32_vGridSize = 0;       

    float f_temp_x = 0;
    float f_temp_y = 0;        

    t_uint32 u32_temp_x = 0;
    t_uint32 u32_temp_y = 0;        

    t_uint32 u32_zoom_status_lla_woi_x = 0;
    t_uint32 u32_zoom_status_lla_woi_y = 0;        

    float f_hROISizeFrac = 0;
    float f_vROISizeFrac = 0;    

    float f_hROIStartFrac = 0;
    float f_vROIStartFrac = 0;        
        
    LOS_Log("\n>> Get_Glace_Geometry\n");

    u32_glace_geometry_mode = ITE_readPE(Glace_Status_e_StatisticsFov_Byte0);
    
    u32_hROIStart = ITE_readPE(Glace_Status_u16_HROIStart_Byte0);
    u32_vROIStart = ITE_readPE(Glace_Status_u16_VROIStart_Byte0 );  

    u32_hGridSize = ITE_readPE(Glace_Status_u8_HGridSize_Byte0);          
    u32_vGridSize = ITE_readPE(Glace_Status_u8_VGridSize_Byte0);      

    u32_hBlockSize = ITE_readPE(Glace_Status_u8_HBlockSize_Byte0);        
    u32_vBlockSize = ITE_readPE(Glace_Status_u8_VBlockSize_Byte0);      

    u32_zoom_status_lla_woi_x = ITE_readPE(Zoom_Status_LLA_u16_woi_resX_Byte0);
    u32_zoom_status_lla_woi_y = ITE_readPE(Zoom_Status_LLA_u16_woi_resY_Byte0);        

    u32_temp_x = ITE_readPE(Glace_Control_f_HBlockSizeFraction_Byte0);
    f_hROISizeFrac = *((float *) &u32_temp_x);

    u32_temp_y = ITE_readPE(Glace_Control_f_VBlockSizeFraction_Byte0);
    f_vROISizeFrac = *((float *) &u32_temp_y);

    u32_temp_x = ITE_readPE(Glace_Control_f_HROIStartFraction_Byte0);
    f_hROIStartFrac = *((float *) &u32_temp_x);

    u32_temp_y = ITE_readPE(Glace_Control_f_VROIStartFraction_Byte0);
    f_vROIStartFrac = *((float *) &u32_temp_y);
    
    if(StatisticsFov_e_Sensor == u32_glace_geometry_mode)
    {
        LOS_Log("glace geometry mode = StatisticsFov_e_Sensor\n");
    }
    else if(StatisticsFov_e_Master_Pipe == u32_glace_geometry_mode)
    {
        LOS_Log("glace geometry mode = StatisticsFov_e_Master_Pipe\n");
    }        
    else
    {
        LOS_Log("Invalid glace geometry mode. Value = %u\n", u32_glace_geometry_mode);    
    }

    LOS_Log("f_hROIStartFrac = %f, f_vROIStartFrac = %f\n", f_hROIStartFrac, f_vROIStartFrac);
    LOS_Log("f_hROISizeFrac = %f, f_vROISizeFrac = %f\n", f_hROISizeFrac, f_vROISizeFrac);
    LOS_Log("u32_hROIStart = %u, u32_vROIStart = %u\n", u32_hROIStart, u32_vROIStart);
    LOS_Log("u32_hGridSize = %u, u32_vGridSize = %u\n", u32_hGridSize, u32_vGridSize);
    LOS_Log("u32_hBlockSize = %u, u32_vBlockSize = %u\n", u32_hBlockSize, u32_vBlockSize);    

    LOS_Log("Other relevant information\n");
    
    u32_temp_x = ITE_readPE(DataPathStatus_e_Flag_Pipe0Enable_Byte0);
    if(0 != u32_temp_x)
    {
        LOS_Log("g_DataPathStatus.e_Flag_Pipe0Enable = TRUE\n");    
    }

    u32_temp_x = ITE_readPE(DataPathStatus_e_Flag_Pipe1Enable_Byte0);
    if(0 != u32_temp_x)
    {
        LOS_Log("g_DataPathStatus.e_Flag_Pipe1Enable = TRUE\n");    
    }    

    u32_temp_x = ITE_readPE(DataPathStatus_e_Flag_BayerStore2Enable_Byte0);
    if(0 != u32_temp_x)
    {
        LOS_Log("g_DataPathStatus.e_Flag_BayerStore2Enable = TRUE\n");    
    }    
    
    
    u32_temp_x = ITE_readPE(PipeState_0_f_FOVX_Byte0);
    u32_temp_y = ITE_readPE(PipeState_0_f_FOVY_Byte0);
    f_temp_x = *((float *) &u32_temp_x);
    f_temp_y = *((float *) &u32_temp_y);    
    LOS_Log("g_PipeState[0].f_FOVX = %f, g_PipeState[0].f_FOVY = %f\n", f_temp_x, f_temp_y);    

    u32_temp_x = ITE_readPE(PipeState_1_f_FOVX_Byte0);
    u32_temp_y = ITE_readPE(PipeState_1_f_FOVY_Byte0);
    f_temp_x = *((float *) &u32_temp_x);
    f_temp_y = *((float *) &u32_temp_y) ;    
    LOS_Log("g_PipeState[1].f_FOVX = %f, g_PipeState[1].f_FOVY = %f\n", f_temp_x, f_temp_y);   

    u32_temp_x = ITE_readPE(Zoom_Status_f_FOVX_Byte0);
    u32_temp_y = ITE_readPE(Zoom_Status_f_FOVY_Byte0);
    f_temp_x = *((float *) &u32_temp_x);
    f_temp_y = *((float *) &u32_temp_y) ;        
    LOS_Log("g_Zoom_Status.f_FOVX = %f, g_Zoom_Status.f_FOVY = %f\n", f_temp_x, f_temp_y);       
    
    LOS_Log("g_Zoom_Status_LLA.u16_woi_resX = %u, g_Zoom_Status_LLA.u16_woi_resY = %u\n", u32_zoom_status_lla_woi_x, u32_zoom_status_lla_woi_y);           
    
    
    LOS_Log("<< Get_Glace_Geometry\n");    
}

/* -----------------------------------------------------------------------
   FUNCTION : Set_Glace_Geometry
   PURPOSE  :
   ------------------------------------------------------------------------ */
void Set_Glace_Geometry(t_uint32 u32_geometry_mode,
                                                float f_hBlockSizeFrac, float f_vBlockSizeFrac,
                                                float f_hROIStartFrac, float f_vROIStartFrac,
                                                t_uint32 u32_hGridSize, t_uint32 u32_vGridSize)
{
    t_uint32 u32_hBlockSizeFrac = 0;
    t_uint32 u32_vBlockSizeFrac = 0;
    t_uint32 u32_hROISizeFrac = 0;
    t_uint32 u32_vROISizeFrac = 0;
    
    LOS_Log("\n>> Set_Glace_Geometry\n");

    u32_hBlockSizeFrac = *((t_uint32*)(&f_hBlockSizeFrac));
    u32_vBlockSizeFrac = *((t_uint32*)(&f_vBlockSizeFrac));
    
    u32_hROISizeFrac = *((t_uint32*)(&f_hROIStartFrac));
    u32_vROISizeFrac = *((t_uint32*)(&f_vROIStartFrac));    

    ITE_writePE(Glace_Control_e_StatisticsFov_Byte0, u32_geometry_mode);

    ITE_writePE(Glace_Control_f_HBlockSizeFraction_Byte0, u32_hBlockSizeFrac);
    ITE_writePE(Glace_Control_f_VBlockSizeFraction_Byte0, u32_vBlockSizeFrac);  

    ITE_writePE(Glace_Control_f_HROIStartFraction_Byte0, u32_hROISizeFrac);          
    ITE_writePE(Glace_Control_f_VROIStartFraction_Byte0, u32_vROISizeFrac);      

    ITE_writePE(Glace_Control_u8_HGridSize_Byte0, u32_hGridSize);        
    ITE_writePE(Glace_Control_u8_VGridSize_Byte0, u32_vGridSize);            
    
    
    LOS_Log("<< Set_Glace_Geometry\n");    
}


/* -----------------------------------------------------------------------
   FUNCTION : Test_Glace_Geometry
   PURPOSE  :
   ------------------------------------------------------------------------ */
t_uint32 Test_Glace_Geometry()
{    
    //t_uint32 u32_geometry_mode = 0;
    //t_uint32 u32_woi_x = 0;
    //t_uint32 u32_woi_y = 0;    
    t_uint32 u32_expected_roi_start_x = 0;
    t_uint32 u32_expected_roi_start_y = 0;      
    t_uint32 u32_expected_grid_size_x = 0;
    t_uint32 u32_expected_grid_size_y = 0;    
    t_uint32 u32_expected_block_size_x = 0;     
    t_uint32 u32_expected_block_size_y = 0;       

    t_uint32 u32_err_count = 0;

    LOS_Log("\n>> Test_Glace_Geometry\n");
    
    ComputeExpectedGlaceGeometry(&u32_expected_roi_start_x, &u32_expected_roi_start_y,
                                                            &u32_expected_grid_size_x, &u32_expected_grid_size_y,
                                                            &u32_expected_block_size_x, &u32_expected_block_size_y);
    
    u32_err_count = CompareComputedAndStatusGlaceGeometry(u32_expected_roi_start_x, u32_expected_roi_start_y,
                                                             u32_expected_grid_size_x, u32_expected_grid_size_y,
                                                             u32_expected_block_size_x, u32_expected_block_size_y);
    
    LOS_Log("<< Test_Glace_Geometry\n");    
    return u32_err_count;
}


/* -----------------------------------------------------------------------
   FUNCTION : ComputeExpectedGlaceGeometry
   PURPOSE  :
   ------------------------------------------------------------------------ */
void ComputeExpectedGlaceGeometry(t_uint32 *ptr_u32_expected_roi_start_x, t_uint32 *ptr_u32_expected_roi_start_y, 
                                                                    t_uint32 *ptr_u32_expected_grid_size_x, t_uint32 *ptr_u32_expected_grid_size_y,
                                                                    t_uint32 *ptr_u32_expected_block_size_x, t_uint32 *ptr_u32_expected_block_size_y)
{
    t_uint32 u32_geometry_mode = 0;

    t_uint32 u32_glace_input_resolution_x = 0;
    t_uint32 u32_glace_input_resolution_y = 0;    

    t_uint32   u32_start_roi_fraction_x = 0;
    t_uint32   u32_start_roi_fraction_y = 0;    
    t_uint32   u32_block_size_fraction_x = 0;         
    t_uint32   u32_block_size_fraction_y = 0;    
    float   f_start_roi_fraction_x = 0;
    float   f_start_roi_fraction_y = 0;    
    float   f_block_size_fraction_x = 0;         
    float   f_block_size_fraction_y = 0;    
    
    t_uint32   u32_glace_grid_size_x = 0;         
    t_uint32   u32_glace_grid_size_y = 0;   

    t_uint32   u32_pipe_offset_x = 0;         
    t_uint32   u32_pipe_offset_y = 0;            

    float f_prescale = 0;

    t_uint32 u32_zoom_status_lla_woi_x = 0;
    t_uint32 u32_zoom_status_lla_woi_y = 0;       

    t_uint32 u32_zoom_status_lla_out_x = 0;
    t_uint32 u32_zoom_status_lla_out_y = 0;     

    t_uint32 u32_zoom_status_fov_x = 0;
    t_uint32 u32_zoom_status_fov_y = 0;        

    t_uint32 u8_forcedSensorFOV = 0;        

    float f_zoom_status_fov_x = 0;
    float f_zoom_status_fov_y = 0;            
    
    LOS_Log("\n>> ComputeExpectedGlaceGeometry\n");    


    // read glace geometry mode
    u32_geometry_mode = ITE_readPE(Glace_Status_e_StatisticsFov_Byte0);

    // read grid sizes
    u32_glace_grid_size_x = ITE_readPE(Glace_Control_u8_HGridSize_Byte0);
    u32_glace_grid_size_y = ITE_readPE(Glace_Control_u8_VGridSize_Byte0);      

    u32_glace_grid_size_x = (u32_glace_grid_size_x) & (~0x1);
    u32_glace_grid_size_y = (u32_glace_grid_size_y) & (~0x1);    

    u32_start_roi_fraction_x = ITE_readPE(Glace_Control_f_HROIStartFraction_Byte0);
    f_start_roi_fraction_x = (*((float *)&u32_start_roi_fraction_x));

    u32_start_roi_fraction_y = ITE_readPE(Glace_Control_f_VROIStartFraction_Byte0);
    f_start_roi_fraction_y = (*((float *)&u32_start_roi_fraction_y));    

    u32_block_size_fraction_x = ITE_readPE(Glace_Control_f_HBlockSizeFraction_Byte0);
    f_block_size_fraction_x = (*((float *)&u32_block_size_fraction_x)); 

    u32_block_size_fraction_y = ITE_readPE(Glace_Control_f_VBlockSizeFraction_Byte0);
    f_block_size_fraction_y = (*((float *)&u32_block_size_fraction_y));          

    u32_zoom_status_lla_out_x = ITE_readPE(Zoom_Status_LLA_u16_output_resX_Byte0);
    u32_zoom_status_lla_out_y = ITE_readPE(Zoom_Status_LLA_u16_output_resY_Byte0);           

    u32_zoom_status_lla_woi_x = ITE_readPE(Zoom_Status_LLA_u16_woi_resX_Byte0);
    u32_zoom_status_lla_woi_y = ITE_readPE(Zoom_Status_LLA_u16_woi_resY_Byte0);               

    u32_zoom_status_fov_x = ITE_readPE(Zoom_Status_f_FOVX_Byte0);
    f_zoom_status_fov_x = *((float*)(&u32_zoom_status_fov_x));    
    
    u32_zoom_status_fov_y = ITE_readPE(Zoom_Status_f_FOVY_Byte0);               
    f_zoom_status_fov_y = *((float*)(&u32_zoom_status_fov_y));      

    // sanity check
    if((f_block_size_fraction_x + f_start_roi_fraction_x) > 1.0)
    {
        f_start_roi_fraction_x = 1.0 - f_block_size_fraction_x;
    }

    if((f_block_size_fraction_y + f_start_roi_fraction_y) > 1.0)
    {
        f_start_roi_fraction_y = 1.0 - f_block_size_fraction_y;
    }   

    if
    (
        (ITE_readPE(DataPathStatus_e_Flag_BayerStore2Enable_Byte0))
    &&  (
            (u32_zoom_status_lla_woi_x < f_zoom_status_fov_x)
        ||  (u32_zoom_status_lla_woi_y < f_zoom_status_fov_y)
        )
    )
    {
        // check for forced sensor fov condition
        u8_forcedSensorFOV = 1;
    }    

#if ENABLE_GLACE_GEOMETRY_DEBUG_TRACES
    LOS_Log("u32_geometry_mode = %u\n", u32_geometry_mode);  
    LOS_Log("u32_glace_grid_size_x = %u, u32_glace_grid_size_y = %u\n", u32_glace_grid_size_x, u32_glace_grid_size_y);           
    LOS_Log("f_start_roi_fraction_x = %f, f_start_roi_fraction_y = %f\n", f_start_roi_fraction_x, f_start_roi_fraction_y);      
    LOS_Log("f_block_size_fraction_x = %f, f_block_size_fraction_y = %f\n", f_block_size_fraction_x, f_block_size_fraction_y);     
#endif    

    if((StatisticsFov_e_Sensor == u32_geometry_mode) || (0 != u8_forcedSensorFOV))
    {
        u32_glace_input_resolution_x = u32_zoom_status_lla_out_x;
        *ptr_u32_expected_roi_start_x = f_start_roi_fraction_x * u32_glace_input_resolution_x;
        
        u32_glace_input_resolution_y = u32_zoom_status_lla_out_y;
        *ptr_u32_expected_roi_start_y = f_start_roi_fraction_y * u32_glace_input_resolution_y;
    }
    else if (StatisticsFov_e_Master_Pipe == u32_geometry_mode)
    {
        // A. calculate pre-scale factor        
        f_prescale = ((float)u32_zoom_status_lla_woi_x)/u32_zoom_status_lla_out_x;               

        // 1. compute glace input resolution taking prescale into consideration 
        u32_glace_input_resolution_x = f_zoom_status_fov_x/f_prescale;        
        u32_glace_input_resolution_y = f_zoom_status_fov_y/f_prescale;              

        // 2. find fov offsets
        u32_pipe_offset_x = (u32_zoom_status_lla_woi_x - f_zoom_status_fov_x)/(2 * f_prescale);
        u32_pipe_offset_y = (u32_zoom_status_lla_woi_y - f_zoom_status_fov_y)/(2 * f_prescale);           

       *ptr_u32_expected_roi_start_x = f_start_roi_fraction_x * u32_glace_input_resolution_x  + u32_pipe_offset_x;       
       *ptr_u32_expected_roi_start_y = f_start_roi_fraction_y * u32_glace_input_resolution_y + u32_pipe_offset_y; 
       
#if ENABLE_GLACE_GEOMETRY_DEBUG_TRACES
        LOS_Log("u32_zoom_status_lla_woi_x = %u, u32_zoom_status_lla_out_x = %u\n", u32_zoom_status_lla_woi_x, u32_zoom_status_lla_out_x);          
        LOS_Log("f_prescale = %f\n", f_prescale);        
        LOS_Log("u32_glace_input_resolution_x = %u, u32_glace_input_resolution_y = %u\n", u32_glace_input_resolution_x, u32_glace_input_resolution_y);          
#endif        
    }
    else
    {
        while(1)
        {
            LOS_Log("invalid glace geometry mode\n");
        }
    }      
    
    *ptr_u32_expected_grid_size_x = u32_glace_grid_size_x;
    *ptr_u32_expected_grid_size_y = u32_glace_grid_size_y;        

   *ptr_u32_expected_roi_start_x = (*ptr_u32_expected_roi_start_x) & (~0x1);    
   *ptr_u32_expected_roi_start_y = (*ptr_u32_expected_roi_start_y) & (~0x1);       
    
    *ptr_u32_expected_block_size_x = ((t_uint32)((f_block_size_fraction_x * u32_glace_input_resolution_x)/u32_glace_grid_size_x)) & (~0x1);  // make it even
    *ptr_u32_expected_block_size_y = ((t_uint32)((f_block_size_fraction_y * u32_glace_input_resolution_y)/u32_glace_grid_size_y)) & (~0x1);  // make it even        

#if ENABLE_GLACE_GEOMETRY_DEBUG_TRACES
    LOS_Log("u8_forcedSensorFOV = %u\n", u8_forcedSensorFOV);    
    LOS_Log("*ptr_u32_expected_roi_start_x = %u, *ptr_u32_expected_roi_start_y = %u\n", *ptr_u32_expected_roi_start_x, *ptr_u32_expected_roi_start_y);    
    LOS_Log("*ptr_u32_expected_grid_size_x = %u, *ptr_u32_expected_grid_size_y = %u\n", *ptr_u32_expected_grid_size_x, *ptr_u32_expected_grid_size_y);  
    LOS_Log("*ptr_u32_expected_block_size_x = %u, *ptr_u32_expected_block_size_y = %u\n", *ptr_u32_expected_block_size_x, *ptr_u32_expected_block_size_y);    
#endif    
        
    LOS_Log("<< ComputeExpectedGlaceGeometry\n");  
}



/* -----------------------------------------------------------------------
   FUNCTION : CompareComputedAndStatusGlaceGeometry
   PURPOSE  :
   ------------------------------------------------------------------------ */
t_uint32 CompareComputedAndStatusGlaceGeometry(t_uint32 u32_expected_roi_start_x, t_uint32 u32_expected_roi_start_y, 
                                                                    t_uint32 u32_expected_grid_size_x, t_uint32 u32_expected_grid_size_y,
                                                                    t_uint32 u32_expected_block_size_x, t_uint32 u32_expected_block_size_y)
{
    t_uint32 u32_status_roi_start_x = 0;
    t_uint32 u32_status_roi_start_y = 0;    
    t_uint32 u32_status_grid_size_x = 0;
    t_uint32 u32_status_grid_size_y = 0;    
    t_uint32 u32_status_block_size_x = 0;
    t_uint32 u32_status_block_size_y = 0;     

    t_uint32 errorCount = 0;
    char comment[200];          

    LOS_Log("\n>> CompareComputedAndStatusGlaceGeometry\n");    

    u32_status_roi_start_x = ITE_readPE(Glace_Status_u16_HROIStart_Byte0);
    u32_status_roi_start_y = ITE_readPE(Glace_Status_u16_VROIStart_Byte0);

    u32_status_grid_size_x = ITE_readPE(Glace_Status_u8_HGridSize_Byte0);
    u32_status_grid_size_y = ITE_readPE(Glace_Status_u8_VGridSize_Byte0);

    u32_status_block_size_x = ITE_readPE(Glace_Status_u8_HBlockSize_Byte0);
    u32_status_block_size_y = ITE_readPE(Glace_Status_u8_VBlockSize_Byte0);    

    if(u32_expected_roi_start_x != u32_status_roi_start_x)
    {
        errorCount++;
        sprintf(comment, "ERR : u32_expected_roi_start_x = %u, u32_status_roi_start_x = %u\n", (unsigned int)u32_expected_roi_start_x, (unsigned int)u32_status_roi_start_x); 
        MMTE_TEST_COMMENT(comment);       
    }

    if(u32_expected_roi_start_y != u32_status_roi_start_y)
    {
        errorCount++;
        sprintf(comment, "ERR : u32_expected_roi_start_y = %u, u32_status_roi_start_y = %u\n", (unsigned int)u32_expected_roi_start_y, (unsigned int)u32_status_roi_start_y); 
        MMTE_TEST_COMMENT(comment);       
    }    

    if(u32_expected_grid_size_x != u32_status_grid_size_x)
    {
        errorCount++;
        sprintf(comment, "ERR : u32_expected_grid_size_x = %u, u32_status_grid_size_x = %u\n", (unsigned int)u32_expected_grid_size_x, (unsigned int)u32_status_grid_size_x); 
        MMTE_TEST_COMMENT(comment);       
    }        

    if(u32_expected_grid_size_y != u32_status_grid_size_y)
    {
        errorCount++;
        sprintf(comment, "ERR : u32_expected_grid_size_y = %u, u32_status_grid_size_y = %u\n", (unsigned int)u32_expected_grid_size_y, (unsigned int)u32_status_grid_size_y); 
        MMTE_TEST_COMMENT(comment);       
    }     

    if(u32_expected_block_size_x != u32_status_block_size_x)
    {
        errorCount++;
        sprintf(comment, "ERR : u32_expected_block_size_x = %u, u32_status_block_size_x = %u\n", (unsigned int)u32_expected_block_size_x, (unsigned int)u32_status_block_size_x); 
        MMTE_TEST_COMMENT(comment);       
    }      

    if(u32_expected_block_size_y != u32_status_block_size_y)
    {
        errorCount++;
        sprintf(comment, "ERR : u32_expected_block_size_y = %u, u32_status_block_size_y = %u\n", (unsigned int)u32_expected_block_size_y, (unsigned int)u32_status_block_size_y); 
        MMTE_TEST_COMMENT(comment);       
    }          

    LOS_Log("<< CompareComputedAndStatusGlaceGeometry\n");   
    return errorCount;
}

/* -----------------------------------------------------------------------
   FUNCTION : Get_Glace_Geometry_Mode
   PURPOSE  :
   ------------------------------------------------------------------------ */
void Set_bms_size(t_uint32 u32_woi_x, t_uint32 u32_woi_y, t_uint32 u32_out_res_x, t_uint32 u32_out_res_y)
{
    LOS_Log("\n<< Set_bms_size\n");   

    usecase.BMS_woi_X = u32_woi_x;
    usecase.BMS_woi_Y = u32_woi_y;
    
    usecase.BMS_XSize = u32_out_res_x;    
    usecase.BMS_YSize = u32_out_res_y;

#if ENABLE_GLACE_GEOMETRY_DEBUG_TRACES
    LOS_Log("BMS_woi_X = %u, BMS_woi_Y = %u\n", usecase.BMS_woi_X, usecase.BMS_woi_Y);
    LOS_Log("BMS_XSize = %u, BMS_YSize = %u\n", usecase.BMS_XSize, usecase.BMS_YSize);    
#endif    
    
    LOS_Log("<< Set_bms_size\n");  
}
