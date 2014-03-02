/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_ctmtx.c
 * \brief    CTM uplink (TX)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs_ctmtx.h"
#include <string.h>
#include <stdlib.h>

#ifdef VCSCTM
# include "tty.h"
#include "vcs_queue.h"
#endif

#define VCS_CTMTX_MAXSYNCQUEUE (4)
struct vcs_ctmtx_s
{
#ifdef VCSCTM
  TTY_TxState_t     *tty;
  vcs_queue_t    *queue;
  vcs_queue_t    *freequeue;
#endif
  int       enabled;
  int       running;
  vcs_log_t *mylog;
};
#define VCS_CTM_DETECTHANDOVER (1)
#ifdef VCSCTM
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmtx_enable(vcs_ctmtx_t *const ctmtx, const vcs_algo_mode_t *const mode)
{
  int result = VCS_OK;

  if (mode->samplerate == 8000)
  {
    ctmtx->enabled = 1;
    if (!ctmtx->running)
    {
      if (ctmtx->tty == NULL)
      {
        ctmtx->tty = TTY_TX_Construct();
      }
      if (ctmtx->tty == NULL)
      {
        ctmtx->mylog->log(ctmtx->mylog, VCS_ERROR, "TTY TX allocation failed");
        result = VCS_ERROR_MEMORY;
      }
      else
      {
        TTY_TX_Init(ctmtx->tty);
        ctmtx->running = 1;
        ctmtx->mylog->log(ctmtx->mylog, VCS_DEBUG, "TTY TX enabled");
      }
    }
  }
  else /* If 16Khz */
  {
    ctmtx->mylog->log(ctmtx->mylog, VCS_ERROR, "TTY is not supported in 16kHz");
  }

  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_disable(vcs_ctmtx_t *ctmtx)
{
  if (ctmtx->running)
  {
    TTY_TX_Destruct(ctmtx->tty);
    ctmtx->tty     = NULL;
    ctmtx->running = 0;
  }
  ctmtx->enabled = 0;
  ctmtx->mylog->log(ctmtx->mylog, VCS_DEBUG, "TTY RX disabled");
}


/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmtx_is_running(vcs_ctmtx_t *ctmtx)
{
  return ctmtx->running;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmtx_is_enabled(vcs_ctmtx_t *ctmtx)
{
  return ctmtx->enabled;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
vcs_ctmtx_t* vcs_ctmtx_create(vcs_log_t *const log)
{
  vcs_ctmtx_t* ctmtx = (vcs_ctmtx_t*) calloc(1, sizeof(vcs_ctmtx_t));
  if (ctmtx != NULL)
  {
    int i;
	ctmtx->mylog = log;
    ctmtx->queue = vcs_queue_alloc(VCS_CTMTX_MAXSYNCQUEUE);
	ctmtx->freequeue = vcs_queue_alloc(VCS_CTMTX_MAXSYNCQUEUE);
	if (ctmtx->queue == 0 || ctmtx->freequeue == 0) {
	   vcs_queue_destroy(ctmtx->queue, free);
       vcs_queue_destroy(ctmtx->freequeue, free);
	   free(ctmtx);
	   log->log(log, VCS_ERROR, "vcs_ctmtx_create: failure allocating resources");
	   return 0;
	}
    
	for (i = 0; i < VCS_CTMTX_MAXSYNCQUEUE; i++) {
	   TTY_RxSyncState_t* tmp = (TTY_RxSyncState_t*) malloc(sizeof(TTY_RxSyncState_t));
	   if (tmp != 0) vcs_queue_add(ctmtx->freequeue, tmp);
	   else ctmtx->mylog->log(ctmtx->mylog, VCS_ERROR, "vcs_ctmtx_create: malloc failed");
	}
	ctmtx->mylog->log(ctmtx->mylog, VCS_ERROR, "vcs_ctmtx_create: sync queue %d", VCS_CTMTX_MAXSYNCQUEUE);
  }

  return ctmtx;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_destroy(vcs_ctmtx_t *ctmtx)
{
  vcs_ctmtx_disable(ctmtx);
  vcs_queue_destroy(ctmtx->queue, free);
  vcs_queue_destroy(ctmtx->freequeue, free);
  free(ctmtx);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmtx_reset(vcs_ctmtx_t *const ctmtx, const vcs_algo_mode_t *const mode)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmtx_configure(vcs_ctmtx_t *const ctmtx, const vcs_algo_mode_t *const mode, const vcs_ctm_config_t *const param)
{
  int result = VCS_OK;

  if (param->enabled)
  {
    result = vcs_ctmtx_enable(ctmtx, mode);
  } 
  else
  {
    vcs_ctmtx_disable(ctmtx);
  } 

  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_execute(vcs_ctmtx_t *ctmtx, short** input, const int nb_input, short** output, const int nb_output)
{
  TTY_RxSyncState_t rxsync;
  if (vcs_queue_isempty(ctmtx->queue)) {
    memset(&rxsync, 0, sizeof(TTY_RxSyncState_t));
	ctmtx->mylog->log(ctmtx->mylog, VCS_DEBUG, "vcs_ctmtx_execute using default TTY_RxSyncState_t");
  } else {
    TTY_RxSyncState_t *tmp = (TTY_RxSyncState_t*) vcs_queue_dequeue(ctmtx->queue);
	rxsync = *tmp;
	vcs_queue_add(ctmtx->freequeue, tmp);
  }
  TTY_TX_Execute(ctmtx->tty, &rxsync, input[0], output[0]);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_config_tostring(char *const buffer, const unsigned int len, const vcs_ctm_config_t *const param)
{
  buffer[0] = '\0';
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_export(vcs_ctmtx_t *const ctmtx, vcs_ctmtx_export_t *const data)
{
  if (data != 0)
  {
    if (ctmtx->running)
    {
      
    }
  }
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_import(vcs_ctmtx_t *const ctmtx, vcs_ctmrx_export_t *const data)
{
   if (data != 0 && ctmtx->running)
  {
    TTY_RxSyncState_t *rxsync;
    if (vcs_queue_isfull(ctmtx->queue)) {
	   ctmtx->mylog->log(ctmtx->mylog, VCS_DEBUG, "vcs_ctmtx_import sync queue full, dropping oldest one");
	   rxsync = vcs_queue_dequeue(ctmtx->queue);
	} else {
	   rxsync = (TTY_RxSyncState_t*) vcs_queue_dequeue(ctmtx->freequeue);
	}
	if (rxsync != 0) {
	  *rxsync = data->rxsync;
	  vcs_queue_add(ctmtx->queue, rxsync);
	} else {
	  ctmtx->mylog->log(ctmtx->mylog, VCS_ERROR, "vcs_ctmtx_import an internal error");
	}
    if (data->handover_detected) {
#ifdef VCS_CTM_DETECTHANDOVER
    	 TTY_TX_Init(ctmtx->tty);
    	 ctmtx->mylog->log(ctmtx->mylog, VCS_DEBUG, "Handover Detected, Reset TTY TX");
#endif
    }
  }
}

#else  //********************** border line *******************************

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
vcs_ctmtx_t* vcs_ctmtx_create(vcs_log_t* log)
{
  vcs_ctmtx_t* ctmtx = (vcs_ctmtx_t*) calloc(1, sizeof(vcs_ctmtx_t));
  if (ctmtx != NULL)
  {
    ctmtx->mylog = log;
  }
  return ctmtx;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_destroy(vcs_ctmtx_t *ctmtx)
{
  if (ctmtx)
    free(ctmtx);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmtx_reset(vcs_ctmtx_t *const ctmtx, const vcs_algo_mode_t *const mode)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmtx_configure(vcs_ctmtx_t *const ctmtx, const vcs_algo_mode_t *const mode, const vcs_ctm_config_t *const param)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_disable(vcs_ctmtx_t *ctmtx)
{
}


/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmtx_is_enabled(vcs_ctmtx_t *ctmtx)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_ctmtx_is_running(vcs_ctmtx_t *ctmtx)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_execute(vcs_ctmtx_t *ctmtxy, short** input, int nb_input, short** output, int nb_output)
{
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_config_tostring(char* buffer, unsigned int len, const vcs_ctm_config_t* param)
{
  buffer[0] = '\0';
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_export(vcs_ctmtx_t *const tx, vcs_ctmtx_export_t *const data)
{
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_ctmtx_import(vcs_ctmtx_t *const tx, vcs_ctmrx_export_t *const data)
{
}

#endif // VCSCTM


