/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   heap.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __HEAP_H_
#define __HEAP_H_

typedef struct t_heap_info
{
  void     *Start;
  t_uint32  SizeAvailable;
  t_uint32  SizeAllocated;
  t_uint32  SizeTotal;
} t_heap_info;

#endif
