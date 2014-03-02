/**
 * Gridiron.c
 *
 * Ver: 2.0 on 11.08.2010
 * Ver: 3.0 on 12.08.2010
 \brief This file is a part of the gridiron module release code. It implements a driver layer for the gridiron.
 \ingroup Gridiron
 */

// includes
// ========
#include "Gridiron_op_interface.h"

#include "Gridiron.h"
#include "Gridiron_ip_interface.h"
#include "Gridiron_op_interface.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_Gridiron_GridironTraces.h"
#endif


uint32_t Power(uint8_t X, uint8_t Y);

/* ------------------------Exported VARIABLES--------------------------*/
GridironControl_ts  g_GridironControl =
{
    DEFAULT_GRIDIRON_LIVECAST,
    DEFAULT_GRIDIRON_CAST_POSITION0,
    DEFAULT_GRIDIRON_CAST_POSITION1,
    DEFAULT_GRIDIRON_CAST_POSITION2,
    DEFAULT_GRIDIRON_CAST_POSITION3,
    DEFAULT_GRIDIRON_GRID_WIDTH,
    DEFAULT_GRIDIRON_GRID_HEIGHT,
    DEFAULT_GRIDIRON_ENABLE_FLAG, 
    DEFAULT_GRIDIRON_VER_FLIP,
    DEFAULT_GRIDIRON_HOR_FLIP,
    DEFAULT_ACTIVE_REFERENCE_CAST_COUNT,
    DEFAULT_GRID_PIXEL_ORDER,
};      

GridironStatus_ts  g_GridironStatus = 
{
     DEFAULT_GRIDIRON_LIVECAST,
     DEFAULT_GRIDIRON_SENSOR_HSCALE,
     DEFAULT_GRIDIRON_SENSOR_VSCALE,
     DEFAULT_GRIDIRON_SENSOR_HSCALE_X256,
     DEFAULT_GRIDIRON_SENSOR_VSCALE_X256,
     DEFAULT_GRIDIRON_CR_HSTART,
     DEFAULT_GRIDIRON_CR_VSTART,
     DEFAULT_GRIDIRON_IM_HSIZE,
     DEFAULT_GRIDIRON_IM_VSIZE,
     DEFAULT_GRIDIRON_SENSOR_HSIZE,
     DEFAULT_GRIDIRON_GRID_PITCH,
     DEFAULT_GRIDIRON_E_FLAG_CAST0,
     DEFAULT_GRIDIRON_E_FLAG_CAST1,
     DEFAULT_GRIDIRON_E_FLAG_CAST2,
     DEFAULT_GRIDIRON_E_FLAG_CAST3,
     DEFAULT_GRIDIRON_PHASE,     
     DEFAULT_GRIDIRON_ENABLE_FLAG, 
     DEFAULT_GRIDIRON_VER_FLIP,
     DEFAULT_GRIDIRON_HOR_FLIP,
     DEFAULT_ACTIVE_REFERENCE_CAST_COUNT     
};

/**
*/
void
Gridiron_UpdateGridStaticParameters(void)
{
    uint16_t u16_sensor_hor_max = 0;  
    uint16_t u16_sensor_ver_max = 0; 
    uint16_t mode = 0;
    float f_scale_virtualImage_2_nativeArray_x = 0;
    float f_scale_virtualImage_2_nativeArray_y = 0;    
    uint16_t u16_scale_virtualImage_2_nativeArray_x_x256 = 0;
    uint16_t u16_scale_virtualImage_2_nativeArray_y_x256 = 0;        
    uint32_t u32_sensor_crop_x = 0;
    uint32_t u32_sensor_crop_y = 0;    
    uint16_t u16_virtual_image_size_x = 0;
    uint16_t u16_virtual_image_size_y = 0;  
    uint32_t u32_dx = 0;
    uint32_t u32_dy = 0;
    uint32_t u32_crop_x_start = 0;
    uint32_t u32_crop_y_start = 0;    

    #if ENABLE_GRIDIRON_TRACES
        OstTraceInt0(TRACE_DEBUG, "<GridIron> ******************************************************************************");    
        OstTraceInt1(TRACE_DEBUG, "<GridIron>  g_GridironStatus.e_Flag_Enable = %u", g_GridironStatus.e_Flag_Enable); 
    #endif
    
    //  if (gridiron is enabled in ctrl)
    if (Flag_e_TRUE == g_GridironControl.e_Flag_Enable)
    {
    for(mode = 0; mode < g_ReadLLAConfig_Status.u16_number_of_modes; mode++ )
    {
        if(u16_sensor_ver_max <g_camera_details.p_sensor_details->output_mode_capability.p_modes[mode].woi_res.height)
        {
            u16_sensor_ver_max = g_camera_details.p_sensor_details->output_mode_capability.p_modes[mode].woi_res.height;
        }

        if(u16_sensor_hor_max < g_camera_details.p_sensor_details->output_mode_capability.p_modes[mode].woi_res.width)
        {            
            u16_sensor_hor_max = g_camera_details.p_sensor_details->output_mode_capability.p_modes[mode].woi_res.width;
        }
        OstTraceInt3(TRACE_DEBUG, "<GridIron> mode = %u, u16_sensor_ver_max = %u, u16_sensor_hor_max = %u", mode, u16_sensor_ver_max, u16_sensor_hor_max);
    }

        u32_crop_x_start = (u16_sensor_hor_max - g_Zoom_Status_LLA.u16_Ex_woi_resX)/2;

        u32_crop_y_start = (u16_sensor_ver_max - g_Zoom_Status_LLA.u16_Ex_woi_resY)/2;

    // Update GRIDIRON GridPitch
    g_GridironStatus.u8_LogGridPitch =  CalculateLogGridPitch(u16_sensor_hor_max);

    u16_virtual_image_size_x = (g_GridironControl.u16_GridWidth - 1) * (2 << g_GridironStatus.u8_LogGridPitch);
    u16_virtual_image_size_y = (g_GridironControl.u16_GridHeight - 1) * (2 << g_GridironStatus.u8_LogGridPitch);
    g_GridironStatus.u16_Sensor_HSize = u16_virtual_image_size_x;

    // scale_virtualImage_2_nativeArray_x
    f_scale_virtualImage_2_nativeArray_x = ((float)u16_virtual_image_size_x)/u16_sensor_hor_max;

    // scale_virtualImage_2_nativeArray_y
    f_scale_virtualImage_2_nativeArray_y = ((float)u16_virtual_image_size_y)/u16_sensor_ver_max;

    u16_scale_virtualImage_2_nativeArray_x_x256 = (uint16_t)(f_scale_virtualImage_2_nativeArray_x*16)*16;
    u16_scale_virtualImage_2_nativeArray_y_x256 = (uint16_t)(f_scale_virtualImage_2_nativeArray_y*16)*16; 
    #if ENABLE_GRIDIRON_TRACES
        OstTraceInt2(TRACE_DEBUG, "<GridIron> g_GridironControl.u16_GridWidth - 1 = %u, 2 << g_GridironStatus.u8_LogGridPitch = %u", g_GridironControl.u16_GridWidth - 1, 2 << g_GridironStatus.u8_LogGridPitch);    
        OstTraceInt2(TRACE_DEBUG, "<GridIron> u16_virtual_image_size_x = %u, u16_virtual_image_size_y = %u", u16_virtual_image_size_x, u16_virtual_image_size_y);
        OstTraceInt2(TRACE_DEBUG, "<GridIron> g_GridironStatus.u8_LogGridPitch = %u, g_GridironStatus.u16_Sensor_HSize = %u", g_GridironStatus.u8_LogGridPitch, g_GridironStatus.u16_Sensor_HSize);    
        OstTraceInt2(TRACE_DEBUG, "<GridIron> f_scale_virtualImage_2_nativeArray_x = %f, f_scale_virtualImage_2_nativeArray_y = %f", f_scale_virtualImage_2_nativeArray_x, f_scale_virtualImage_2_nativeArray_y);      
        OstTraceInt2(TRACE_DEBUG, "<GridIron> u16_scale_virtualImage_2_nativeArray_x_x256 = %u, u16_scale_virtualImage_2_nativeArray_y_x256 = %u", u16_scale_virtualImage_2_nativeArray_x_x256, u16_scale_virtualImage_2_nativeArray_y_x256);      
        OstTraceInt2(TRACE_DEBUG, "<GridIron> g_camera_details.p_sensor_details->pixel_orders.normal = %u, GridIron_Sensor_PreScale_Factor = %u", 
            g_camera_details.p_sensor_details->pixel_orders.normal, GridIron_Sensor_PreScale_Factor());
        OstTraceInt2(TRACE_DEBUG, "<GridIron> u32_crop_x_start = %u, u32_crop_y_start = %u", u32_crop_x_start, u32_crop_y_start);                 
        OstTraceInt2(TRACE_DEBUG, "<GridIron> Get_IM_HSIZE = %u, Get_IM_VSIZE = %u", Get_IM_HSIZE(), Get_IM_VSIZE());         
    #endif              

    // if the default order starts with R or GIB: hor_crop_start' = nearest even integer of (sensor_hor_max_size - hor_scaling_factor x image_hor_size - hor_crop_start)   
    if((CAM_DRV_SENS_PIXEL_ORDER_RGrGbB == g_camera_details.p_sensor_details->pixel_orders.normal) 
     ||(CAM_DRV_SENS_PIXEL_ORDER_GbBRGr == g_camera_details.p_sensor_details->pixel_orders.normal))
    {
        u32_sensor_crop_x = u16_sensor_hor_max - (GridIron_Sensor_PreScale_Factor()* Get_IM_HSIZE() + u32_crop_x_start);
    }
    else
    {
        u32_sensor_crop_x = u32_crop_x_start;
    }
    
    // if the default order starts with B or GIB: ver_crop_start' = nearest even integer of (sensor_ver_max_size - ver_scaling_factor x image_ver_size - ver_crop_start)
    if((CAM_DRV_SENS_PIXEL_ORDER_BGbGrR == g_camera_details.p_sensor_details->pixel_orders.normal) 
     ||(CAM_DRV_SENS_PIXEL_ORDER_GbBRGr == g_camera_details.p_sensor_details->pixel_orders.normal))  
    {
        u32_sensor_crop_y = u16_sensor_ver_max - (GridIron_Sensor_PreScale_Factor() * Get_IM_VSIZE() + u32_crop_y_start);
    }
    else
    {
        u32_sensor_crop_y = u32_crop_y_start;
    }


   // 2nd order correction
   if((PixelOrder_e_RGr_GbB == g_GridironControl.e_PixelOrder) || (PixelOrder_e_GbB_RGr == g_GridironControl.e_PixelOrder))
   {
        u32_dx = u16_virtual_image_size_x - ((uint32_t)u16_sensor_hor_max*u16_scale_virtualImage_2_nativeArray_x_x256)/256;
   }

   if((PixelOrder_e_BGb_GrR == g_GridironControl.e_PixelOrder) || (PixelOrder_e_GbB_RGr == g_GridironControl.e_PixelOrder))
   {
        u32_dy = u16_virtual_image_size_y - ((uint32_t)u16_sensor_ver_max*u16_scale_virtualImage_2_nativeArray_y_x256)/256;
   }


    // g_GridironStatus.u16_Crop_HStart and g_GridironStatus.u16_Crop_VStart must be even
    g_GridironStatus.u16_Crop_HStart = (((u32_sensor_crop_x*u16_scale_virtualImage_2_nativeArray_x_x256/256 + u32_dx) >> 1)<<1);
    g_GridironStatus.u16_Crop_VStart = (((u32_sensor_crop_y*u16_scale_virtualImage_2_nativeArray_y_x256/256 + u32_dy) >> 1)<<1);

    g_GridironStatus.u16_Sensor_HScale_x256 = u16_scale_virtualImage_2_nativeArray_x_x256*GridIron_Sensor_PreScale_Factor();
    g_GridironStatus.u16_Sensor_VScale_x256 = u16_scale_virtualImage_2_nativeArray_y_x256*GridIron_Sensor_PreScale_Factor();    

    g_GridironStatus.f_Sensor_HScale = g_GridironStatus.u16_Sensor_HScale_x256/256.0;
    g_GridironStatus.f_Sensor_VScale = g_GridironStatus.u16_Sensor_VScale_x256/256.0;
  
    //Update GRIDIRON IM_HSIZE and IM_VSIZE
    g_GridironStatus.u16_Image_HSize = Get_IM_HSIZE();
    g_GridironStatus.u16_Image_VSize = Get_IM_VSIZE();
    g_GridironStatus.e_Flag_Enable = Flag_e_TRUE;
    #if ENABLE_GRIDIRON_TRACES    
        OstTraceInt2(TRACE_DEBUG, "<GridIron> u32_sensor_crop_x = %u, u32_sensor_crop_y = %u", u32_sensor_crop_x, u32_sensor_crop_y);       
        OstTraceInt1(TRACE_DEBUG, "<GridIron> doing 2nd level correction in crop sizes : g_GridironControl.e_GridPixelOrder = %u", g_GridironControl.e_PixelOrder);      
        OstTraceInt2(TRACE_DEBUG, "<GridIron> u32_dx = %u, u32_dy = %u", u32_dx, u32_dy);         
        OstTraceInt2(TRACE_DEBUG, "<GridIron> g_GridironStatus.u16_Crop_HStart = %u, g_GridironStatus.u16_Crop_VStart  = %u", g_GridironStatus.u16_Crop_HStart, g_GridironStatus.u16_Crop_VStart );         
        OstTraceInt2(TRACE_DEBUG, "<GridIron> g_GridironStatus.u16_Sensor_HScale_x256 = %u, g_GridironStatus.u16_Sensor_VScale_x256 = %u", g_GridironStatus.u16_Sensor_HScale_x256, g_GridironStatus.u16_Sensor_VScale_x256);
        OstTraceInt2(TRACE_DEBUG, "<GridIron> g_GridironStatus.f_Sensor_HScale = %f, g_GridironStatus.f_Sensor_VScale = %f", g_GridironStatus.f_Sensor_HScale, g_GridironStatus.f_Sensor_VScale); 
        OstTraceInt2(TRACE_DEBUG, "<GridIron> g_GridironStatus.u16_Image_HSize = %u, g_GridironStatus.u16_Image_VSize = %u", g_GridironStatus.u16_Image_HSize, g_GridironStatus.u16_Image_VSize);
    #endif
    
    }
    else
    {
        g_GridironStatus.e_Flag_Enable = Flag_e_FALSE;
    }
}


/**
*/
void
Gridiron_CommitGridStaticParameters(void)
{
    if (Flag_e_TRUE == g_GridironStatus.e_Flag_Enable)
    {
        /*
        1. Commit Gridparameters like
        uint16_t u16_Crop_VStart;
        uint16_t u16_Crop_VStart;
        uint16_t u16_Image_HSize;
        uint16_t u16_Image_VSize;
        uint16_t u16_Sensor_HSize;
        uint16_t f_Sensor_HScale;
        uint16_t f_Sensor_VScale;
        uint8_t u8_LogGridPitch;
        uint8_t e_Flag_Enable;
        "Set_ISP_GRIDIRON_ISP_GRIDIRON_ENABLE_enable_gridiron__ENABLE();"
        uint8_t e_Flag_VerFlip;
        uint8_t e_Flag_HorFlip;
        */
        Set_MEM_INIT_DISABLE();

        // Program GRIDIRON_CONTROL : Logridpitch,Mem_init,EnCast0,EnCast1,EnCast2,EnCast3 and SuperSensitive
        Set_Gridiron_LOGRID_PITCH(g_GridironStatus.u8_LogGridPitch);

        Set_SUPERSENSITIVE_NORMAL();

        //Program GRIDIRON_CR_START : HStart and VStart
        Set_Gridiron_CR_HSTART(g_GridironStatus.u16_Crop_HStart);
        Set_Gridiron_CR_VSTART(g_GridironStatus.u16_Crop_VStart);

        //Program GRIDIRON_IM_HSIZE and GRIDIRON_IM_VSIZE
        Set_Gridiron_IM_HSIZE(g_GridironStatus.u16_Image_HSize);
        Set_Gridiron_IM_VSIZE(g_GridironStatus.u16_Image_VSize);

        //Program GRIDIRON_SENSOR_SIZE
        Set_Gridiron_SENSOR_HSIZE(g_GridironStatus.u16_Sensor_HSize);

        // Program GRIDIRON_SCALE: HScale and VScale
        Set_Gridiron_SENSOR_HSCALE(g_GridironStatus.u16_Sensor_HScale_x256);
        Set_Gridiron_SENSOR_VSCALE(g_GridironStatus.u16_Sensor_VScale_x256); 
        
        //Commented  as mentioned in ER#414531
        //Set_PIX_ORDER_DISABLE();

        //<sudeep> Added  as mentioned in ER#414531
       if(SystemConfig_IsInputImageSourceBayerLoad1() || SystemConfig_IsInputImageSourceBayerLoad2())
        {
            /* 	
               			** [CN]Gridiron enabled during BML, 
               			** then PIXEL order in IDP stream is unknown, In that case we need to program T1 
               			** register to correctly configure GI IP  
            */
          	/*Set GI IP to read pixel order from T1 register*/
          	Set_PIX_ORDER_ENABLE();
          	
          	/*Configure T1 register based on pixel order page element*/
          	if(PixelOrder_e_RGr_GbB == g_GridironControl.e_PixelOrder)
          	{
      				 /*Enable mirroring*/	
          		Set_ISP_GRIDIRON_ISP_GRIDIRON_PIX_ORDER_hor_flip__ENABLE();
          	}	
          	else if(PixelOrder_e_BGb_GrR == g_GridironControl.e_PixelOrder)
          	{
      				 /*Enable vertical flipping*/	
          		Set_ISP_GRIDIRON_ISP_GRIDIRON_PIX_ORDER_ver_flip__ENABLE();
          	}	
          	else if(PixelOrder_e_GbB_RGr == g_GridironControl.e_PixelOrder)
          	{
      				 /*Enable mirroring and flipping*/	
         		Set_ISP_GRIDIRON_ISP_GRIDIRON_PIX_ORDER_hor_flip__ENABLE();
         		Set_ISP_GRIDIRON_ISP_GRIDIRON_PIX_ORDER_ver_flip__ENABLE();
       	   }
            	
        }
        else
        {
            	Set_PIX_ORDER_DISABLE();
        }

        Set_ISP_GRIDIRON_ISP_GRIDIRON_ENABLE_enable_gridiron__ENABLE();
    }
    else
    {
        //disable gridiron
        Set_ISP_GRIDIRON_ISP_GRIDIRON_ENABLE_enable_gridiron__DISABLE();
    }

  g_GridironStatus.u8_Active_ReferenceCast_Count = g_GridironControl.u8_Active_ReferenceCast_Count;
}


/**
*/
void
Gridiron_UpdateLiveParameters(void)
{
    float_t   f_CastsReferencesArray[] = { g_GridironControl.f_CastPosition0, g_GridironControl.f_CastPosition1, g_GridironControl.f_CastPosition2, g_GridironControl.f_CastPosition3 };
    bool_t   b_CastEnableArray[] = { Flag_e_FALSE, Flag_e_FALSE, Flag_e_FALSE, Flag_e_FALSE };
    uint32_t u32_phase = 0;
    float status_live_cast = 0;
   
   if (Flag_e_TRUE == g_GridironStatus.e_Flag_Enable)
   {
        /*
        Constraints
        1. All castposition must be in increasing order i.e. cast_position_3 < cast_position_2 < cast_position_1 < cast_position_0.
        2. Live parameter can be any value equal to or in between any 2 consective cast
        if live cast is located between cast_position_1 and cast_postion_2
                 So, fphase = (live_cast - cast_position_2)/ (cast_position_1 - cast_position_2)
                 Please note if the live cast is equal to cast_position the fphase would be 0.
                 If the live cast is less than cast_postion_0 Or if it is greater than cast_position_3 the fphase would be kept Zero.
        [NOTE]: If phase is zero, only one cast should be selected.
        3. ISP FW should 2 consective cast based on live cast.
        4. any fractional number would be multiplied by the gridpitch and rounded to the integer value and used
          Loggridpitch of 4 these parameters can take values 0 to 15 only.
          Loggridpitch of 5 these parameters can take values 0 to 31 only.
          Loggridpitch of 6 these parameters can take values 0 to 63 only
          Loggridpitch of 7 these parameters can take values 0 to 127 only
          Loggridpitch of 8 these parameters can take values 0 to 255 only 
        */

        g_GridironStatus.u16_Phase = 0;

        ComputeGridironCastsInterpolation(K_MAX_CAST_COUNT, g_GridironControl.u8_Active_ReferenceCast_Count, (const float_t *)f_CastsReferencesArray, g_GridironControl.f_LiveCast, 
                                                                g_GridironStatus.u8_LogGridPitch, b_CastEnableArray, &u32_phase, &status_live_cast);
        g_GridironStatus.e_Flag_Cast0 = b_CastEnableArray[0];
        g_GridironStatus.e_Flag_Cast1 = b_CastEnableArray[1];
        g_GridironStatus.e_Flag_Cast2 = b_CastEnableArray[2];
        g_GridironStatus.e_Flag_Cast3 = b_CastEnableArray[3];     
        g_GridironStatus.u16_Phase = u32_phase;
        g_GridironStatus.f_LiveCast = status_live_cast;
   }
}


/**
*/
void
Gridiron_CommitGridLiveParameters(void)
{
   if (Flag_e_TRUE == g_GridironStatus.e_Flag_Enable)
   {
     Set_EnCast0(g_GridironStatus.e_Flag_Cast0);
     Set_EnCast1(g_GridironStatus.e_Flag_Cast1);
     Set_EnCast2(g_GridironStatus.e_Flag_Cast2);
     Set_EnCast3(g_GridironStatus.e_Flag_Cast3);
     
     // Program GRIDIRON_LIVE_CAST_PHASE
     Set_Gridiron_FPHASE(g_GridironStatus.u16_Phase);
   }
}


void
Gridiron_ApplyGridLiveParameters(void)
{
    Gridiron_UpdateLiveParameters();
    Gridiron_CommitGridLiveParameters();
}


void
Gridiron_ApplyGridStaticParameters(void)
{
    Gridiron_UpdateGridStaticParameters();
    Gridiron_CommitGridStaticParameters();   
}


/**
  Calculates value of X^Y (X raised to power)
*/
uint32_t
Power(
uint8_t X,
uint8_t Y)
 {
 // OstTraceInt2(TRACE_DEBUG, "> Power : X = %d, Y=%d", X, Y);
 uint32_t u32_CalculatedValue = 1;
 while(Y)
 {
 u32_CalculatedValue = u32_CalculatedValue * X;
 Y--;
 }


    return (u32_CalculatedValue);
 }


uint32_t
CalculateLogGridPitch(uint32_t sensor_hor_max)
 {
  uint32_t u32_Grid_Pitch = (sensor_hor_max) / (g_GridironControl.u16_GridWidth - 1);
  uint32_t u32_LogGridPitch = 0; 
  uint32_t u32_power_of_two = 1;
  uint32_t lower_bound_log_grid_pitch = 0;
  uint32_t upper_bound_log_grid_pitch = 0;  

   while(u32_power_of_two < u32_Grid_Pitch)
   {
       u32_power_of_two *= 2;
       u32_LogGridPitch++;
   }

   u32_LogGridPitch--;
   #if ENABLE_GRIDIRON_TRACES
      OstTraceInt3(TRACE_DEBUG, "<CalculateLogGridPitch> sensor_hor_max = %u, u32_power_of_two = %u, u32_LogGridPitch = %u", sensor_hor_max, u32_power_of_two, u32_LogGridPitch);   
   #endif
   
    if(Is_8500v1())
    {
        lower_bound_log_grid_pitch = DEFAULT_LOWER_BOUND_LOG_GRID_PITCH_V1;
        upper_bound_log_grid_pitch = DEFAULT_UPPER_BOUND_LOG_GRID_PITCH_V1;
    }
    if (Is_8500v2() || Is_9540v1() || Is_8540v1())
    {
        lower_bound_log_grid_pitch = DEFAULT_LOWER_BOUND_LOG_GRID_PITCH_V2;
        upper_bound_log_grid_pitch = DEFAULT_UPPER_BOUND_LOG_GRID_PITCH_V2;   
    }

    if (u32_LogGridPitch < lower_bound_log_grid_pitch)   
    {
        u32_LogGridPitch = lower_bound_log_grid_pitch; 
    }
    else if (u32_LogGridPitch > upper_bound_log_grid_pitch)
    {
        u32_LogGridPitch = upper_bound_log_grid_pitch;
    }
   #if ENABLE_GRIDIRON_TRACES
       OstTraceInt3(TRACE_DEBUG, "<CalculateLogGridPitch> lower_bound_log_grid_pitch = %u, upper_bound_log_grid_pitch = %u, u32_LogGridPitch = %u", lower_bound_log_grid_pitch, upper_bound_log_grid_pitch, u32_LogGridPitch);   
   #endif
    return (u32_LogGridPitch);
 }


void ComputeGridironCastsInterpolation(uint32_t max_casts_count, uint32_t cur_casts_count, const float_t * casts_references_array, float_t live_cast, uint32_t log_grid_pitch, bool_t * casts_enables_array, uint32_t * phase_ptr, float* status_live_cast_ptr)
{
    uint32_t cast_min_idx = 0;
    uint32_t cast_max_idx = 0;
    uint32_t segment_idx = 0;
    uint32_t cast_idx = 0;

    // initialize all elements in casts_enables_array[] to Flag_e_FALSE
    for (cast_idx = 0; cast_idx < max_casts_count; cast_idx++)
    {
        casts_enables_array[cast_idx] = Flag_e_FALSE;
    }

    // set  cast_min_idx and cast_max_idx when reference casts are in decreasing order
    if (casts_references_array[0] < casts_references_array[cur_casts_count - 1])
    {
        cast_min_idx = 0;
        cast_max_idx = cur_casts_count - 1;
    }
    else             // set  cast_min_idx and cast_max_idx when reference casts are in increasing order
    {
        cast_min_idx = cur_casts_count - 1;
        cast_max_idx = 0;
    }

    // set phase and casts_enables_array[] values when live_cast <= casts_references_array[cast_min_idx]
    if (live_cast <= casts_references_array[cast_min_idx])
    {
        casts_enables_array[cast_min_idx] = Flag_e_TRUE; 
        *status_live_cast_ptr = casts_references_array[cast_min_idx];
        *phase_ptr = 0;
    }
    else if (live_cast >= casts_references_array[cast_max_idx])            // set phase and casts_enables_array[] values when live_cast >= casts_references_array[cast_max_idx]
    {
        casts_enables_array[cast_max_idx] = Flag_e_TRUE;
        *status_live_cast_ptr = casts_references_array[cast_max_idx];        
        *phase_ptr = 0;
    }
    else        
    { // set phase and casts_enables_array[] values for all other cases
        for (segment_idx = 1; segment_idx < cur_casts_count; segment_idx++)
        {
            if (((live_cast >= casts_references_array[segment_idx - 1]) && (live_cast < casts_references_array[segment_idx - 0])) || ((live_cast >= casts_references_array[segment_idx - 0]) && (live_cast < casts_references_array[segment_idx - 1])))
            {
                // We should enter this condition only once.
                *phase_ptr = (uint32_t) ((live_cast - casts_references_array[segment_idx - 1]) / (casts_references_array[segment_idx - 0] - casts_references_array[segment_idx - 1]) * (1L << log_grid_pitch) + 0.5f);

                if (*phase_ptr == 0)
                {
                    casts_enables_array[segment_idx - 1] = Flag_e_TRUE;
                }
                else if (*phase_ptr == (1L << log_grid_pitch))
                {
                    casts_enables_array[segment_idx - 0] = Flag_e_TRUE;
                    *phase_ptr = 0;
                 }
                else
                {
                    casts_enables_array[segment_idx - 1] = Flag_e_TRUE;
                    casts_enables_array[segment_idx - 0] = Flag_e_TRUE;
                }

                // set *status_live_cast_ptr live_cast value 
                *status_live_cast_ptr = live_cast;            
                break;        // do a break so that loop is not entered again.
            }
        }
    }
}


