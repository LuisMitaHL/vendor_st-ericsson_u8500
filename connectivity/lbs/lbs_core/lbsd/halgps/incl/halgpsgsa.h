/*
 *****************************************************************************
 * Copyright (c) 2009 ST-Ericsson Ltd.
 *    Filename:  halgpsgsa.h
 *    Description:  The file provides interface to L1S and HWLRFU modules.
 *        Version:  1.0
 *        Created:  07/8/2009
 *       Revision:  none
 *       Compiler:
 *       Author:  Raghvendra GUPTA
 *       Company:  STEricsson BLR
 *
 * *****************************************************************************
 */

#ifndef HALGPS_GSA_H
#define HALGPS_GSA_H


#include "master.h"

/*internal include */
#include "halgpsgsa.hec"
#include "halgpsgsa.hem"
#include "halgpsgsa.hep"
#include "halgpsgsa.het"
#include "halgpsgsainc.h" // included to remove compilation warnings.

#include <math.h>

#define K_GSA_AFC_STATUS_OPEN 0       /**< AFC status: Open*/
#define K_GSA_AFC_STATUS_CLOSED 1     /**< AFC status: Closed*/


/**
 * @brief  AFC algo to provide the reliability of the frequency error samples and frequency error.
 API Functions that may be called from 2G L1S or HWLRFU to update the status and Uncertainity of the

 In 2G, AFC update will be done every 51 bursts in idle mode and 208 bursts in dedicated mode.
 In 3G, AFC update will be each paging (IDLE) or each frame (DCH). AFC update will be done under 2G or
 3G frame interrupt, it is mandatory to achieve a low MIPS consumption.

 As the function is called from the L1S and HWLRFU, the function must be light. It should not do message sending
 and should not be blocking.

 * return  - None
 */
void  GSA_SetAfcUpdate(
   uint8_t vl_Status,               /**<Status of AFC  */
   int32_t FreqError_ppb           /**< Uncertainty of the known frequency [ppb] .
                                                                                       Set the value to  0 if this value is not known.*/
   );




#endif  /*HALGPS_GSA_H*/

