/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  SDL SDL

 \detail    The SDL module is responsible for managing the Sensor Data Linearization FlexTF module
*/

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      SDL.c
 \brief     Implements the driver for Sensor Data Linearization (SDL) FlexTF module

 \ingroup   SDL
 \endif
*/
#include "SDL_op_interface.h"
#include "SDL_PlatformSpecific.h"

SDL_Control_ts  g_SDL_Control = { SDL_CONTROL_DEFAULT_MODE_CONTROL };
SDL_Status_ts   g_SDL_Status = { SDL_STATUS_DEFAULT_MODE_STATUS, SDL_STATUS_DEFAULT_UPDATE_PENDING };
SDL_Last_ELT_ts g_SDL_ELT =
{
    DEFAULT_ISP_FLEXTF_LINEAR_LAST_ELT,
    DEFAULT_ISP_FLEXTF_LINEAR_LAST_ELT,
    DEFAULT_ISP_FLEXTF_LINEAR_LAST_ELT,
    DEFAULT_ISP_FLEXTF_LINEAR_LAST_ELT,
    DEFAULT_ISP_FLEXTF_LINEAR_PIXEL_SHIFT
};

/**
  \fn void SDL_Commit( void )
   \brief   Top level function to be called to setup Sensor Data Linearization FlexTF

  \return   void
  \param    None
  \callgraph
  \callergraph
  \ingroup SDL
*/
void
SDL_Commit(void)
{
    uint32_t    *ptru32_SDLMemGIR;
    uint32_t    *ptru32_SDLMemRed;
    uint32_t    *ptru32_SDLMemBlue;
    uint32_t    *ptru32_SDLMemGIB;

    uint16_t    u16_LUTIndex;
    uint16_t    u16_OutputValue;

    // if the control and status PE are not same then this is a trigger
    // to accept a new control for SDL
    // Also if the device is not streaming, then the control has to be
    // taken into account unconditionally
    if (!SDL_IsStreaming() || (g_SDL_Status.e_SDLMode_Status != g_SDL_Control.e_SDLMode_Control))
    {
        g_SDL_Status.e_SDLMode_Status = g_SDL_Control.e_SDLMode_Control;

        if (SDL_Linear == g_SDL_Control.e_SDLMode_Control)
        {
            // SDL will be enabled with linear ramp programmed by the firmware
            // the FlexTF memory will be programmed only if the device is not streaming
            if (!SDL_IsStreaming())
            {
                SDL_DisableHW();

                ptru32_SDLMemGIR = ( uint32_t * ) SDL_GIR_MEM_BASE;
                ptru32_SDLMemRed = ( uint32_t * ) SDL_RED_MEM_BASE;
                ptru32_SDLMemBlue = ( uint32_t * ) SDL_BLUE_MEM_BASE;
                ptru32_SDLMemGIB = ( uint32_t * ) SDL_GIB_MEM_BASE;

                // program the FlexTF memories
                for (u16_LUTIndex = 0; u16_LUTIndex < (1 << SDL_MEM_LUT_DEPTH_LOG_BASE_2); u16_LUTIndex++)
                {
                    // the translated value must be in the pixel width domain
                    // i.e. if the pixel width is 12, then the translated value must
                    // be in 12 bit domain
                    u16_OutputValue = u16_LUTIndex * (1 << (SDL_INPUT_PIXEL_WIDTH - SDL_MEM_LUT_DEPTH_LOG_BASE_2));

                    ptru32_SDLMemGIR[u16_LUTIndex] = u16_OutputValue;
                    ptru32_SDLMemRed[u16_LUTIndex] = u16_OutputValue;
                    ptru32_SDLMemBlue[u16_LUTIndex] = u16_OutputValue;
                    ptru32_SDLMemGIB[u16_LUTIndex] = u16_OutputValue;
                }


                // program the last reference value in the IP register
                u16_LUTIndex = (1 << SDL_MEM_LUT_DEPTH_LOG_BASE_2);
                u16_OutputValue = u16_LUTIndex * (1 << (SDL_INPUT_PIXEL_WIDTH - SDL_MEM_LUT_DEPTH_LOG_BASE_2));

                SDL_ProgramLastGIR(u16_OutputValue);
                SDL_ProgramLastRed(u16_OutputValue);
                SDL_ProgramLastBlue(u16_OutputValue);
                SDL_ProgramLastGIB(u16_OutputValue);
                SDL_ProgramPixelShift(0);
            }


            // program the hardware registers
            SDL_EnableHW();
        }
        else if (SDL_Custom == g_SDL_Control.e_SDLMode_Control)
        {
            // SDL will be enabled. FlexTF will be programmed by the host
            // program other registers of SDL
            SDL_DisableHW();

            SDL_ProgramLastGIR(g_SDL_ELT.u16_LastElementinLUT_GIR);
            SDL_ProgramLastRed(g_SDL_ELT.u16_LastElementinLUT_RED);
            SDL_ProgramLastBlue(g_SDL_ELT.u16_LastElementinLUT_BLUE);
            SDL_ProgramLastGIB(g_SDL_ELT.u16_LastElementinLUT_GIB);
            SDL_ProgramPixelShift(g_SDL_ELT.u16_PixelShift);

            SDL_EnableHW();
        }
        else
        {
            // SDL will be disabled
            SDL_DisableHW();
        }
     }

    return;
}

