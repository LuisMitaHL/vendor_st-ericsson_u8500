/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   audiomodem_driver_threads.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __audiomodem_driver_threads_h__
#define __audiomodem_driver_threads_h__

extern "C" {
  void * RxThreadEntry(void* arg);
  void * TxThreadEntry(void* arg);
}

#endif //__audiomodem_driver_threads_h__
