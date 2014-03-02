/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file  PipeAlgorithmUpdate.c
 \brief API's to update pixel pipe related variables. The values of variables are programmed in pipe in PipeAlgorithmCommit.c
 \ingroup SystemConfig
 \endif
*/
#include "PipeAlgorithm.h"
#include "FrameDimension_op_interface.h"
#include "ZoomTop.h"
#include "SystemConfig.h"
#include "exposure_statistics_op_interface.h"
#include "FrameRate_op_interface.h"
#include "GPIOManager_OpInterface.h"
#include "Aperture.h"
#include "Duster.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_SystemConfig_PipeAlgorithmUpdateTraces.h"
#endif

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t UpdatePipe(uint8_t u8_PipeNo)
 \brief     The procedure call variaous pipe alogrithm modules and update
            data structure
 \details   The procedure call variaous pipe alogrithm modules and update
            data structure. The values are then programmed in PipeAlgorithmCommit
            It can be called from interrupt
 \param u8_PipeNo: Pipe No in ISP
 \return uint8_t
 Result_e_Success: Update successful
 Result_e_Failure: Update unsuccessful
 \ingroup SystemConfig
 \callgraph
 \callergraph
 \endif
*/
static uint8_t
UpdatePipe(
uint8_t u8_PipeNo)
{
    ColourMatrix_DamperUpdate(u8_PipeNo);

    if(Is_8540v1())
    {
        RgbToYuvMatrix_Update(u8_PipeNo);
        Norcos_Update(u8_PipeNo);
        YuvToRgbMatrix_Update(u8_PipeNo);
    }

    RgbToYuvCoder_Update(u8_PipeNo);

    return (Result_e_Success);
}


static uint8_t PostScalerLineBlanking()
{
    if (FrameDimension_GetMinimumLineBlankingBeyondGPS0() <= FrameDimension_GetMinimumLineBlankingBeyondGPS1())
    {
        return (FrameDimension_GetMinimumLineBlankingBeyondGPS1());
    }
    else
    {
        return (FrameDimension_GetMinimumLineBlankingBeyondGPS0());
    }
}

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t PipeAlgortihm_FrameUpdate( )
 \brief     Procedure update all the frame dimension related parameters
 \details   Procedure update all the frame dimension related parameters including
            video timing, frame dimension, zoom, frame rate etc
 \return    Flag_e_TRUE if the streaming update was successful
            Flag_e_FALSE if the streaming update was unsuccessful
 \ingroup SystemConfig
 \callgraph
 \callergraph
 \endif
*/
uint8_t
PipeAlgortihm_FrameUpdate(void)
{
    uint8_t e_Flag_ZoomStatus;
    uint8_t e_FrameDimensionRequestStatus;

    // invoke the frame dimension manager to find out the
    // maximum addressable FOV at the current streaming parameters
    // only needs to be done if the input image source is a sensor
    // and output image destination is pipe0 or pipe1 i.e. atleast one
    // of the pipes are active
    // If input image source is sensor and output pipe is not active
    // it is a case of memory store operation. In this case, the sensor
    // should stream the complete imaging array. This information is
    // programmed directly into the FDM.
    // If input image source is not sensor and the output pipe is active
    // there is no sense is computing the maximum sensor FOV. The image
    // present in the memory is already the maximum sensor FOV and this
    // information must be programmed into the FDM page elements already
    // by the host.
    e_Flag_ZoomStatus = Flag_e_TRUE;
    e_FrameDimensionRequestStatus = FrameDimensionRequestStatus_e_Accepted;

    if (SystemConfig_IsInputImageSourceSensor())
    {

        // invoke the FDM to compute the maximum usable sensor FOV
        FrameDimension_ComputeMaximumSensorFieldOfView();

        if (SystemConfig_IsAnyPipeActive())
        {
            // invoke the zoom manager to decide on the FOV and also the
            // required frame dimensions
            e_Flag_ZoomStatus = ZoomTop_PreRunUpdate();
        }
        else
        {
            // the input image is from sensor, but none of the pipes are enabled
            // it is a case of BMS only.
            // request FFOV from the sensor
            // <BG> : Call VF in trial mode to see if user settings are valid
            OstTraceInt4(TRACE_FLOW, "<BMS> X: %d, Y: %d, X: %d, Y: %d",g_Pipe_RAW.u16_woi_res_X, g_Pipe_RAW.u16_woi_res_Y, g_Zoom_Status_LLA.u16_Ex_output_resX = g_Pipe_RAW.u16_output_res_X_size, g_Zoom_Status_LLA.u16_Ex_output_resX = g_Pipe_RAW.u16_output_res_Y_size);

            g_Zoom_Status_LLA.u16_Ex_output_resX = g_Pipe_RAW.u16_output_res_X_size;

            g_Zoom_Status_LLA.u16_Ex_output_resY = g_Pipe_RAW.u16_output_res_Y_size;

            g_Zoom_Status_LLA.u16_Ex_woi_resY = g_Pipe_RAW.u16_woi_res_Y;

            g_Zoom_Status_LLA.u16_Ex_woi_resX = g_Pipe_RAW.u16_woi_res_X;

            /// For BMS Requested line length should always be minimum, so programming zero
            g_RequestedFrameDimension.u16_VTLineLengthPck = 0;

            if (Flag_e_FALSE == lla_abstraction_user_BMS_input_valid())
            {
                e_FrameDimensionRequestStatus = FrameDimensionRequestStatus_e_Denied;
            }

            if (Stream_IsSensorStreaming() && (e_FrameDimensionRequestStatus != FrameDimensionRequestStatus_e_Denied))
            {
                    // If sensor is already streaming, find out if current sensor mode
                    // is OK for new BMS request. Otherwise sensor reprogram is required.
                    if (
                          (g_Zoom_Status_LLA.u16_Ex_output_resX == g_Zoom_Status_LLA.u16_output_resX )&&
                          (g_Zoom_Status_LLA.u16_Ex_output_resY == g_Zoom_Status_LLA.u16_output_resY )&&
                          (g_Zoom_Status_LLA.u16_Ex_woi_resY == g_Zoom_Status_LLA.u16_woi_resY) &&
                          (g_Zoom_Status_LLA.u16_Ex_woi_resX == g_Zoom_Status_LLA.u16_woi_resX)
                      )
                    {
                        g_Zoom_CommandStatus.e_ZoomRequestStatus = ZoomRequestStatus_e_Accepted;
                    }
                    else
                    {
                        g_Zoom_CommandStatus.e_ZoomRequestStatus = ZoomRequestStatus_e_ReProgramRequired;
                    }
            }
        }


        if
        (
            (Flag_e_TRUE == e_Flag_ZoomStatus)
        &&  (FrameDimensionRequestStatus_e_Denied != e_FrameDimensionRequestStatus)
        )
        {
            /* TODO: <LLA> <AG>: Very important point to calculate everything? */

            // the current streaming operation can start...
            // OK to setup FDM etc.
            // invoke the FDM to setup the new frame dimension
             if (Stream_IsSensorStreaming() && Zoom_IsFDMRequestPending())
             {
                 // Return if sensor reprogram is required. No need to calculate scalar parameters.
                 // scalar parameters will be calculated after sensor is reprogrammed and
                 // ZOOM_STEP_COMPLETE event will be notified
                  return (Flag_e_TRUE);
             }

            // Sensor configuration is done to apply any new values of
            // exposure, frame rate etc.which can be changed during streaming.
            lla_abstraction_ConfigureSensor();

            // <Todo: BG>: Need to check feedback values. If not same how to proceed?
            g_Zoom_Status_LLA.u16_output_resX = g_config_feedback.output_res.width;
            g_Zoom_Status_LLA.u16_output_resY = g_config_feedback.output_res.height;
            g_Zoom_Status_LLA.u16_woi_resX = g_config_feedback.woi_res.width;
            g_Zoom_Status_LLA.u16_woi_resY = g_config_feedback.woi_res.height;


            // Update X start and Y start based on the current sensor mode selection
            if(FrameDimension_GetMaximumXOutputSize() > g_Zoom_Status_LLA.u16_woi_resX)
            {
                FrameDimension_SetCurrentVTXAddrStart(((FrameDimension_GetMaximumXOutputSize() -  g_Zoom_Status_LLA.u16_woi_resX)/2 - g_Zoom_Status.u8_CenterShiftX));
            }
            else
            {
                FrameDimension_SetCurrentVTXAddrStart(0);
            }

            if (FrameDimension_GetMaximumYOutputSize() > g_Zoom_Status_LLA.u16_woi_resY)
            {
                FrameDimension_SetCurrentVTYAddrStart((FrameDimension_GetMaximumYOutputSize() -  g_Zoom_Status_LLA.u16_woi_resY)/2 - g_Zoom_Status.u8_CenterShiftY);
            }
            else
            {
                FrameDimension_SetCurrentVTYAddrStart(0);
            }

            // tell the zoom manager that the FDM change has been absorbed
            ZoomTop_FDMRequestAccepted();
        }
    }
    else
    {
        // it is a case of BML
        // invoke the zoom module if any of the pipes are enabled
        if (SystemConfig_IsAnyPipeActive())
        {
            g_SystemConfig_Status.e_Flag_FastBML = g_SystemSetup.e_Flag_FastBML;
            e_Flag_ZoomStatus = ZoomTop_PreRunUpdate();

            if (g_RequestedFrameDimension.u16_VTLineLengthPck < g_Zoom_Status_LLA.u16_Ex_output_resX + PostScalerLineBlanking())
            {
                g_RequestedFrameDimension.u16_VTLineLengthPck = g_Zoom_Status_LLA.u16_Ex_output_resX + PostScalerLineBlanking();
            }


            // tell the zoom module that the FDM change has been absorbed
            ZoomTop_FDMRequestAccepted();

            if(Flag_e_TRUE== g_SystemConfig_Status.e_Flag_FastBML)
            {
                    // Currently we are not crpping in X
                    g_BML_Framedimension.u16_BMLXOffset = 0;
                    g_BML_Framedimension.u16_BMLXSize = g_Zoom_Status_LLA.u16_output_resX;
                    if (SystemConfig_IsPipe0Active() && SystemConfig_IsPipe1Active())
                   {
                        // If both the pipes are active, crop should be done so that
                        // both pipe FOV can be accomodated. This can be done by
                        g_BML_Framedimension.u16_BMLYOffset = (g_Pipe_Scalar_Output[0].u16_VCropStart < g_Pipe_Scalar_Output[1].u16_VCropStart )?(g_Pipe_Scalar_Output[0].u16_VCropStart ):(g_Pipe_Scalar_Output[1].u16_VCropStart );
                        g_BML_Framedimension.u16_BMLYSize = (g_Pipe_Scalar_Output[0].u16_VCropSize > g_Pipe_Scalar_Output[1].u16_VCropSize)?(g_Pipe_Scalar_Output[0].u16_VCropSize + FrameDimension_GetISPRowLoss()):(g_Pipe_Scalar_Output[1].u16_VCropSize + FrameDimension_GetISPRowLoss());
                   }
                    else if(SystemConfig_IsPipe0Active())
                    {
                        g_BML_Framedimension.u16_BMLYOffset = g_Pipe_Scalar_Output[0].u16_VCropStart ;

                        // for demosaic loss 4 is added
                        g_BML_Framedimension.u16_BMLYSize =  g_Pipe_Scalar_Output[0].u16_VCropSize + FrameDimension_GetISPRowLoss();
                    }
                    else
                    {
                        g_BML_Framedimension.u16_BMLYOffset = g_Pipe_Scalar_Output[1].u16_VCropStart ;

                         // for demosaic loss 4 is added
                        g_BML_Framedimension.u16_BMLYSize = g_Pipe_Scalar_Output[1].u16_VCropSize + FrameDimension_GetISPRowLoss();
                    }

            }
            else
            {
                    g_BML_Framedimension.u16_BMLXOffset = 0;
                    g_BML_Framedimension.u16_BMLXSize = g_Zoom_Status_LLA.u16_output_resX;
                    g_BML_Framedimension.u16_BMLYOffset = 0;
                    g_BML_Framedimension.u16_BMLYSize = g_Zoom_Status_LLA.u16_output_resY;
            }

            // Boundary check is added for Y BML size. For X this is same as BMS size programmed by Host.
            // Since this size is calculated by scalarbordermanager, if this condition gets hit, error is returned.
            // and no recovery is done.
            if(g_BML_Framedimension.u16_BMLYSize > g_Zoom_Status_LLA.u16_output_resY)
            {
                    e_FrameDimensionRequestStatus = FrameDimensionRequestStatus_e_Denied;
            }
        }
    }


    if ((Flag_e_TRUE == e_Flag_ZoomStatus) && (FrameDimensionRequestStatus_e_Denied != e_FrameDimensionRequestStatus))
    {
        return (Flag_e_TRUE);
    }
    else
    {
        return (Flag_e_FALSE);
    }
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t PipeAlgorithm_UpdatePipe0( )
 \brief     Performs the computations of all the parameters corresponding to pipe0.
 \return Nothing
 \ingroup SystemConfig
 \callgraph
 \callergraph
 \endif
*/
void
PipeAlgorithm_UpdatePipe0(void)
{
    UpdatePipe(0);

    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t PipeAlgorithm_UpdatePipe1( )
 \brief     Performs the computations of all the parameters corresponding to pipe1.
 \return Nothing
 \ingroup SystemConfig
 \callgraph
 \callergraph
 \endif
*/
void
PipeAlgorithm_UpdatePipe1(void)
{
    UpdatePipe(1);

    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t PipeAlgorithm_UpdatePipe1( )
 \brief     Performs the computations of all the parameters corresponding to RE.
 \return Nothing
 \ingroup SystemConfig
 \callgraph
 \callergraph
 \endif
*/
void
PipeAlgorithm_UpdateRE(void)
{
    // update RSO
    RSO_Update();

    Gridiron_UpdateLiveParameters();

    // SDL is handled in commit, No need for any update function
    // update Scorpio
    Scorpio_Update();

    // update Binning Repair
    BinningRepair_Update();

    //Update duster values
    Duster_UpdateGaussian();

    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t PipeAlgorithm_UpdateDMCE( )
 \brief     Performs the computations of all the parameters corresponding to DMCE.
 \return Nothing
 \ingroup SystemConfig
 \callgraph
 \callergraph
 \endif
*/
void
PipeAlgorithm_UpdateDMCE(void)
{
    if(Is_8540v1())
    {
        // call Mozart to update the DMCE for demosaicing
        Mozart_Update();
    }
    else
    {
        // call Babylon to update the DMCE for demosaicing
        Babylon_Update();
    }
    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t PipeAlgorithm_UpdateSDPipe( )
 \brief     Performs the computations of all the parameters corresponding to SD Pipe.
 \return Nothing
 \ingroup SystemConfig
 \callgraph
 \callergraph
 \endif
*/
void
PipeAlgorithm_UpdateSDPipe(void)
{
    return;
}

