#ifndef _VCS_QUEUE_H_
#define _VCS_QUEUE_H_
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_queue.h
 * \brief    standard queue
 * \author   ST-Ericsson
 */
/*****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
  
typedef struct vcs_queue_s vcs_queue_t;


typedef void  (*vcs_queue_proc_fn)(void*);

  vcs_queue_t* vcs_queue_alloc(unsigned int size);
  
  void vcs_queue_destroy(vcs_queue_t*q,
						 vcs_queue_proc_fn foreach);
  
  int vcs_queue_add(vcs_queue_t*q, void*elem);
  
  void* vcs_queue_front(vcs_queue_t*q);
  void* vcs_queue_dequeue(vcs_queue_t*q);
  int vcs_queue_isempty(vcs_queue_t*q);
  int vcs_queue_isfull(vcs_queue_t*q);
  void vcs_queue_empty(vcs_queue_t*q,
					   vcs_queue_proc_fn foreach);
  unsigned int vcs_queue_size(vcs_queue_t*q);
  
  
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
