/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_ctmrx.c
 * \brief    CTM downlink (RX)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs_ctmrx.h"
#include <string.h>

#ifdef VCSCTM
# include "tty.h"

// timer for TTY Handover check
# include <sys/time.h>
#endif

struct vcs_ctmrx_s
{
#ifdef VCSCTM
  TTY_RxState_t     *tty;
  TTY_RxSyncState_t sync;
  unsigned int      LastTTY_ExecuteInUs;
#endif
  int       enabled;
  int       running;
  int       handover_detected;
  vcs_log_t *mylog;
};
#define VCS_CTM_DETECTHANDOVER (1)
#ifdef VCSCTM
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int CheckIfHandover(vcs_ctmrx_t *const ctmrx)
{
  struct timeval timestamp;
  int RetVal = 0;
  unsigned int CurrDecodeinUs, DiffinUs;
        
  /* Get timestamp */
  gettimeofday(&timestamp, NULL);
  CurrDecodeinUs = (unsigned int) timestamp.tv_usec;

  /* Check if timer has wrapped  (Wraps @ 1000000) */
  if (ctmrx->LastTTY_ExecuteInUs > CurrDecodeinUs)
   DiffinUs = ( (1000000 -  ctmrx->LastTTY_ExecuteInUs) + CurrDecodeinUs);
  else
   DiffinUs = (CurrDecodeinUs - ctmrx->LastTTY_ExecuteInUs);

  /* Check if an handover has occured */
  if ( (DiffinUs > 30000) && (ctmrx->LastTTY_ExecuteInUs != 0) )
  {
    ctmrx->mylog->log(ctmrx->mylog, VCS_DEBUG, "!!!!TimeStamps diff > 30000, (Curr: %u, Prev:%u, Diff: %u)", CurrDecodeinUs, ctmrx->LastTTY_ExecuteInUs, DiffinUs);
    RetVal = 1;
  }
  else
  {
    //ctmrx->mylog->log(ctmrx->mylog, VCS_DEBUG, "TimeStamps Curr: %u, Prev:%u, Diff: %u", CurrDecodeinUs, ctmrx->LastTTY_ExecuteInUs, DiffinUs); // TODO: Remove
  }
  /* Save value for next loop */
  ctmrx->LastTTY_ExecuteInUs = CurrDecodeinUs;

  return RetVal;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmrx_enable(vcs_ctmrx_t *const ctmrx, const vcs_algo_mode_t *const mode)
{
  int result = VCS_OK;

  if (mode->samplerate == 8000)
  {
    ctmrx->enabled = 1;
    if (!ctmrx->running) 
    {
      /* Try allocate TTY RX */
      if (ctmrx->tty == NULL)
      {
        ctmrx->tty = TTY_RX_Construct();
      }

      if (ctmrx->tty == NULL)
      {
        ctmrx->mylog->log(ctmrx->mylog, VCS_ERROR, "TTY RX allocation failed");
        result = VCS_ERROR_MEMORY;
      }
      else
      {
        TTY_RX_Init(ctmrx->tty, &ctmrx->sync);
        ctmrx->running             = 1;
        ctmrx->LastTTY_ExecuteInUs = 0;
        ctmrx->mylog->log(ctmrx->mylog, VCS_DEBUG, "TTY RX enabled");
      }
    }
  }
  else /* If 16Khz */
  {
    ctmrx->mylog->log(ctmrx->mylog, VCS_ERROR, "TTY is not supported in 16kHz");
  }

  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_disable(vcs_ctmrx_t *ctmrx)
{
  if (ctmrx->running)
  {
    TTY_RX_Destruct(ctmrx->tty);
    ctmrx->tty                 = NULL;
    ctmrx->running             = 0;
    ctmrx->LastTTY_ExecuteInUs = 0;
  }
  ctmrx->enabled = 0;
  ctmrx->mylog->log(ctmrx->mylog, VCS_DEBUG, "TTY RX disabled");
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmrx_is_running(vcs_ctmrx_t *ctmrx)
{
  return ctmrx->running;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmrx_is_enabled(vcs_ctmrx_t *ctmrx)
{
  return ctmrx->enabled;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
vcs_ctmrx_t* vcs_ctmrx_create(vcs_log_t* log)
{
  vcs_ctmrx_t* ctmrx = (vcs_ctmrx_t*) calloc(1, sizeof(vcs_ctmrx_t));

  if (ctmrx != NULL)
  {
    ctmrx->mylog = log;
  }
  return ctmrx;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_destroy(vcs_ctmrx_t *ctmrx)
{
  vcs_ctmrx_disable(ctmrx);
  free(ctmrx);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmrx_reset(vcs_ctmrx_t *const ctmrx, const vcs_algo_mode_t *const mode)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmrx_configure(vcs_ctmrx_t *ctmrx, const vcs_algo_mode_t *mode, const vcs_ctm_config_t* param)
{
  int result = VCS_OK;

  if (param->enabled)
  {
    result = vcs_ctmrx_enable(ctmrx, mode);
  } 
  else
  {
    vcs_ctmrx_disable(ctmrx);
  }
  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_execute(vcs_ctmrx_t *const ctmrx, short** input, const int nb_input, short** output, const int nb_output)
{
  if (CheckIfHandover(ctmrx) != 0)
  {
#ifdef VCS_CTM_DETECTHANDOVER
    TTY_RX_Init(ctmrx->tty, &ctmrx->sync);
    ctmrx->mylog->log(ctmrx->mylog, VCS_DEBUG, "Handover Detected, Reset TTY RX");
#endif
    ctmrx->handover_detected = 1;
  }

  TTY_RX_Execute(ctmrx->tty, &ctmrx->sync, input[0], output[0]);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_export(vcs_ctmrx_t *ctmrx, vcs_ctmrx_export_t  *data)
{
  if (data != 0) 
  {
    if (ctmrx->running)
    {
      memcpy(&data->rxsync, &ctmrx->sync, sizeof(TTY_RxSyncState_t) );
      data->handover_detected = ctmrx->handover_detected;
      ctmrx->handover_detected = 0;
    }
  }
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_import(vcs_ctmrx_t *const ctmrx, vcs_ctmtx_export_t *const data)
{
  if (data != 0 && ctmrx->running)
  {
    
  }
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_config_tostring(char *const buffer, const unsigned int len, const vcs_ctm_config_t *const param)
{
  buffer[0] = '\0';
}

#else //***************** border line **************************************

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
vcs_ctmrx_t* vcs_ctmrx_create(vcs_log_t* log)
{
  vcs_ctmrx_t* ctmrx = (vcs_ctmrx_t*) calloc(1, sizeof(vcs_ctmrx_t));
  
  if (ctmrx != NULL)
  {
    ctmrx->mylog = log;
  }
  return ctmrx;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_destroy(vcs_ctmrx_t *const ctmrx)
{
  if (ctmrx) 
    free(ctmrx);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmrx_reset(vcs_ctmrx_t *const ctmrx, const vcs_algo_mode_t *const mode)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmrx_configure(vcs_ctmrx_t *const ctmrx, const vcs_algo_mode_t *const mode, const vcs_ctm_config_t *const param)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_disable(vcs_ctmrx_t *const ctmrx)
{
}


/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmrx_is_enabled(vcs_ctmrx_t *const ctmrx)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmrx_is_running(vcs_ctmrx_t *const ctmrx)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_execute(vcs_ctmrx_t *const ctmrxy, short** input, const int nb_input, short** output, const int nb_output)
{
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_export(vcs_ctmrx_t *const ctmrx, vcs_ctmrx_export_t *const data)
{
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_import(vcs_ctmrx_t *const ctmrx, vcs_ctmtx_export_t *const data)
{
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmrx_config_tostring(char *const buffer, const unsigned int len, const vcs_ctm_config_t *const param)
{
  buffer[0] = '\0';
}

#endif // VCSCTM


