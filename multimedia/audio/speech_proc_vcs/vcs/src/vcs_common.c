/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_common.c
 * \brief    Common functions
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs.h"
#include "vcs_algo.h"
#include <assert.h>
#include <string.h>

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_deinterleave(const short *in, int ilen, short *out[], int nb_ch, int len)
{
  int i, j, ch;

  if (ilen != nb_ch*len)
    return;

  for (i = 0, j = 0; i < ilen; j ++)
  {
    for (ch = 0; ch < nb_ch; ch++)
    {
      out[ch][j] = in[i];
      i++;
    }
  }
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_interleave(short *in[], int nb_ch, int ilen, short *out, int olen)
{
  int i, j, ch;
  if (ilen*nb_ch!= olen)
    return;
  
  for (i = 0, j = 0; i < olen; j++)
  {
    for (ch = 0; ch < nb_ch; ch++)
    {
      out[i] = in[ch][j];
      i++;
    }
  }
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_to_algo_mode(vcs_algo_mode_t *algomode, const vcs_mode_t *mode, unsigned int nb_channels, unsigned int interleaved)
{
  algomode->resolution   = mode->resolution;
  algomode->samplerate   = mode->samplerate;
  algomode->framesize    = mode->framesize;
  algomode->interleaved  = interleaved;
  algomode->nb_channels  = nb_channels;
  algomode->nb_channels2 = 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_to_algo_mode2(vcs_algo_mode_t *algomode, const vcs_mode_t *mode, unsigned int nb_channels, unsigned int nb_channels2, unsigned int interleaved)
{
  algomode->resolution   = mode->resolution;
  algomode->samplerate   = mode->samplerate;
  algomode->framesize    = mode->framesize;
  algomode->interleaved  = interleaved;
  algomode->nb_channels  = nb_channels;
  algomode->nb_channels2 = nb_channels2;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_portsdef_init(vcs_portsdef_t *ports, unsigned int pos, unsigned int nb_channels, unsigned int samplerate, unsigned int interleaved)
{
  if (pos == 0) 
    memset(ports, 0, sizeof(vcs_portsdef_t));

  ports->port[pos].nb_channels = nb_channels;
  ports->port[pos].samplerate  = samplerate;
  ports->port[pos].interleaved = interleaved;
  ports->nb_ports              = pos+1;
}


