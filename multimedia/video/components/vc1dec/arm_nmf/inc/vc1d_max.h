/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_MAX_H_
#define _VC1D_MAX_H_



/*------------------------------------------------------------------------
 * Defines                                                             
 *----------------------------------------------------------------------*/
/* Level dependant constants */

#if defined(MP_LL)

/** \brief See Annex D.2   */
#define MAX_MBF 396

/** \brief for MP@LL max MB/frame is 396.  
 *  assuming a max 19/9 aspect ratio, the Max MB width is sqrt(16x396/9)
 *  for the max mb height we assume square images ie 20 MBs
 */
#define MAX_MB_WIDTH 27
#define MAX_MB_HEIGHT 20

#elif defined(MP_ML)

#define MAX_MBF 1620
#define MAX_MB_WIDTH 54
#define MAX_MB_HEIGHT 41


#elif defined(MP_HL)

#define MAX_MBF 8192
#define MAX_MB_WIDTH 121
#define MAX_MB_HEIGHT 91


#elif defined(AP_L2)

#define MAX_MBF 3680
#define MAX_MB_WIDTH 80
#define MAX_MB_HEIGHT 45


#else
#error "No Level is defined at compile time!"
#endif



#endif /* _VC1D_MAX_H_ */
