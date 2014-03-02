/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSEE_HIC__
#define __CGPSEE_HIC__
/**
* \file cgpsee.hic
* \date 19/01/2012
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all constant used by cgps4utils.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 19.01.12</TD><TD>Saswatha </TD><TD> Extended Ephemeris </TD>
*     </TR>
* </TABLE>
*/

#define EEClient_Log( args )     ( printf args ); printf("\n");


/**
* \def K_CGPS_MAX_CONSTEL
*
* Maximum number of Constellations
*/
#define K_CGPS_MAX_CONSTEL                  2


/**
 * \def K_CGPS_EE_SUCCESS
 * Defines the return code for functions that complete successfully.
 *
 */
#define K_CGPS_EE_SUCCESS 0


/**
 * \def K_CGPS_EE_FAIL
 * Defines the return code for functions that complete unsuccessfully.
 *
 */
#define K_CGPS_EE_FAIL 255


/**
 * \def K_CGPS_EE_UD_INT
 * 
 *
 */
#define K_CGPS_EE_UD_INT     240       // Ext Eph target Update Interval [seconds]


/**
 * \def K_CGPS_MAX_FILENAME_LEN
 * Extended Ephemers file Maximum Filename Length (including path) [bytes].
 *
 */

#define K_CGPS_MAX_FILENAME_LEN  128


/**
 * \def K_CGPS_CONSTANT_MAX_EPHEMERIS_RECORDS
 * Defines the maximum number of broadcast ephemeris (BCE) records
 * stored within the BCE Store (per PRN).
 *
 */
#define K_CGPS_CONSTANT_MAX_EPHEMERIS_RECORDS 25


/**
 * \def K_CGPS_CONSTANT_EPHEMERIS_RECORD_SIZE
 * Defines the size (in bytes) of a broadcast ephemeris (BCE) record.
 *
 * \note
 * This value may change if compiler is not set to default byte alignment.
 *
 */
#define K_CGPS_CONSTANT_EPHEMERIS_RECORD_SIZE 64


/**
* \def K_CGPS_DATA_FILES_ROOT
* this constant defines directory where the Extended Ephemeris files will be present.
*/
#define K_CGPS_DATA_FILES_ROOT      "/data"

// Broadcast Ephemeris File Size and initial "invalid" data values.
#define K_CGPS_EE_FILE_SIZE     ( K_CGPS_CONSTANT_NUM_GPS_PRNS*K_CGPS_CONSTANT_EPHEMERIS_RECORD_SIZE )
#define K_CGPS_EE_INIT_DATA     0xFF

#define K_CGPS_CUTOFF_TIME 20*1000



#endif /* __CGPSEE_HIC__ */
