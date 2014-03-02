/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   afmdebug.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __AFM_DEBUG_H__
#define __AFM_DEBUG_H__

extern const char debugInfoEnumType[AFM_NB__ENUM][32];
extern const char debugInfoEnumState[AFM_NB_STATE_ENUM][32];
extern const char debugInfoEnumEos[AFM_NB_EOS_ENUM][32];
extern const char debugInfoEnumError[AFM_NB_ERROR_ENUM][32];
extern void AfmPrintEnum(char *name,int typeOfInfo, int info);
extern void AfmPrintMemUsed(void);
#endif //  __AFM_DEBUG_H__
