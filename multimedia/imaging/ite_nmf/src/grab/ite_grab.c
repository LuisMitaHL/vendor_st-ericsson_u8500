/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ite_grab.h"
#include "VhcElementDefs.h"
#include "ite_sia_init.h"

#include "ite_sia_interface_data.h"
#include <cm/inc/cm_macros.h>
#include "host/grab/api/cmd.h"

#include "ite_event.h"

extern struct s_grabParams grabparamsHR;
extern struct s_grabParams grabparamsLR;
extern struct s_grabParams grabparamsBMS;
extern volatile t_uint32 g_grabHR_infinite;
extern volatile t_uint32 g_grabLR_infinite;
extern volatile t_uint32 g_grabBMS_infinite;
extern volatile t_uint32 g_grabBML_infinite;


/********************************************************/
/*   FUNCTION : ITE_StartGrabNb             */
/*   PURPOSE  : Start the Grab task         */
/*  initialize g_grabXR_infinite globale variable   */
/*  NMFCALL grabcommand, execute            */
/********************************************************/
//ported on 8500 V1
t_uint32 ITE_StartGrabNb(int Pipe, int StreamLength)
{
 if (Pipe == GRBPID_PIPE_HR) {
    g_grabHR_infinite = StreamLength;

    NMFCALL(grabCommand, execute)(GRBPID_PIPE_HR, grabparamsHR, GRBPID_PIPE_HR);

    }
 if (Pipe == GRBPID_PIPE_LR) {
    g_grabLR_infinite = StreamLength;

    NMFCALL(grabCommand, execute)(GRBPID_PIPE_LR, grabparamsLR, GRBPID_PIPE_LR);

    }
 if (Pipe == GRBPID_PIPE_RAW_OUT) {
    g_grabBMS_infinite = StreamLength;

    //FIXME: not yet ported
    NMFCALL(grabCommand, execute)(GRBPID_PIPE_RAW_OUT, grabparamsBMS, GRBPID_PIPE_RAW_OUT);

    }
 if (Pipe == GRBPID_PIPE_RAW_IN) {
    g_grabBML_infinite = StreamLength;

    //FIXME: not yet ported
    NMFCALL(grabCommand, execute)(GRBPID_PIPE_RAW_IN, grabparamsBMS, GRBPID_PIPE_RAW_IN);

    }
  return(0);
}


t_uint32 ITE_AbortGrab(int Pipe)
{
 if (Pipe == GRBPID_PIPE_HR) {
    g_grabHR_infinite = 0;

    NMFCALL(grabCommand, abort)(GRBPID_PIPE_HR, GRBPID_PIPE_HR);

    }
 if (Pipe == GRBPID_PIPE_LR) {
    g_grabLR_infinite = 0;

    NMFCALL(grabCommand, abort)(GRBPID_PIPE_LR, GRBPID_PIPE_LR);

    }
 if (Pipe == GRBPID_PIPE_RAW_OUT) {
    g_grabBMS_infinite = 0;

    //FIXME: not yet ported
    NMFCALL(grabCommand, abort)(GRBPID_PIPE_RAW_OUT, GRBPID_PIPE_RAW_OUT);

    }
 if (Pipe == GRBPID_PIPE_RAW_IN) {
    g_grabBML_infinite = 0;

    //FIXME: not yet ported
    NMFCALL(grabCommand, abort)(GRBPID_PIPE_RAW_IN, GRBPID_PIPE_RAW_IN);

    }
  return(0);
}




/********************************************************/
/*   FUNCTION : ITE_StopGrabNb              */
/*   PURPOSE  : Stop the Grab task          */
/*  reset g_grabXR_infinite globale variable        */
/********************************************************/
t_uint32 ITE_StopGrab(int Pipe)
{
 if (Pipe == GRBPID_PIPE_HR) {
    g_grabHR_infinite = 0;}
 if (Pipe == GRBPID_PIPE_LR) {
    g_grabLR_infinite = 0;}
 if (Pipe == GRBPID_PIPE_RAW_OUT) {
    g_grabBMS_infinite = 0;}
 if (Pipe == GRBPID_PIPE_RAW_IN) {
    g_grabBML_infinite = 0;}
  return(0);
}

/****************************************************************/
/* Function: ITE_IsGrabTaskRunning              */
/* Purpose: return true or false according to Grab Task     */
/* Warning in fact return g_GrabXX_infinite but         */
/* just after a Stop grab command g_grabXX_infinite=0       */
/* but Event End of Grab appear a the end of current frame  */
/****************************************************************/
t_uint32 ITE_IsGrabRunning(int Pipe) {
 if (Pipe == GRBPID_PIPE_HR)        return(g_grabHR_infinite);
 if (Pipe == GRBPID_PIPE_LR)        return(g_grabLR_infinite);
 if (Pipe == GRBPID_PIPE_RAW_OUT)   return(g_grabBMS_infinite);
 if (Pipe == GRBPID_PIPE_RAW_IN)    return(g_grabBML_infinite);
 return 0;
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_setGrabParams
   PURPOSE  :
   WARNING : not fully customized as some parameters are hard coded at first (stab, windowing)
   ------------------------------------------------------------------------ */
void ITE_setGrabParams(struct s_grabParams *pGrabparams, enum e_grabFormat grabFormat, int xframeSize, int yframeSize, int xwindowSize, int xwindowoffset,int destBufferAddr, t_bool disableGrabCache, int cacheBufferAddr)
{
    pGrabparams->output_format=grabFormat;
    //FIXME: windowing not yet managed
    pGrabparams->x_window_size=xwindowSize;
    pGrabparams->y_window_size=yframeSize;
    pGrabparams->x_window_offset=xwindowoffset;
    pGrabparams->y_window_offset=0;
    pGrabparams->x_frame_size=xframeSize;
    pGrabparams->y_frame_size=yframeSize;
    pGrabparams->dest_buf_addr=destBufferAddr;
    pGrabparams->buf_id=0;
    pGrabparams->rotation_cfg= 0;
    pGrabparams->disable_grab_cache=disableGrabCache;
    pGrabparams->cache_buf_addr=cacheBufferAddr;
    //FIXME: stab not yet managed
    pGrabparams->enable_stab=0;

    pGrabparams->x_stab_size_lr=0;
    pGrabparams->y_stab_size_lr=0;
    //pGrabparams->hr_enabled=0;

    pGrabparams->proj_h_buf_addr=0;
    pGrabparams->proj_v_buf_addr=0;
        pGrabparams->lr_alpha = 0;
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_ModifyGrabTask
   PURPOSE  :
   ------------------------------------------------------------------------ */
//WArning: NbOfTask parameter deprecated
void ITE_ModifyGrabTask(struct s_grabParams *pGrabparams,int sfw,int sfh,int sww,int swh,int sho,int svo,int NbOfTask,int pipeNumber)
{
 UNUSED(NbOfTask);
 UNUSED(svo);
 UNUSED(pipeNumber);

   pGrabparams->x_frame_size    = sfw;
   pGrabparams->y_frame_size    = sfh;
   pGrabparams->x_window_size   = sww;
   pGrabparams->y_window_size   = swh;
   pGrabparams->x_window_offset = sho;
   //no pGrabparams->y_window_offset exposed by FW
   //pGrabparams->y_window_offset=

}

/* -----------------------------------------------------------------------
   FUNCTION : VAL_ModifyGrabTaskOutputFormat
   PURPOSE  :
   ------------------------------------------------------------------------ */
//FIXME: 1 only grab task managed at first
void ITE_ModifyGrabTaskOutputFormat(struct s_grabParams *pGrabparams,int NbOfTask, int pipeNumber, enum e_grabFormat outputFormat)
{
 UNUSED(NbOfTask);
 UNUSED(pipeNumber);
  pGrabparams->output_format = outputFormat;

}

