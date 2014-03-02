/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef ITE_BOARDINFO_H
#   define ITE_BOARDINFO_H

#   include <inc/type.h>
#   include "ite_main.h"

#   ifdef __cplusplus
extern "C"
{
#   endif
enum boardRevList
{
    UNKNOWN,        // board version is undefined
    NDK15B02,       // aka NDK15B02 board
    NDK15B02DEBUG,  // aka NDK15B02 board with eWarp debugging patch
    DEVL,           // aka Reference Design board
    NDK15C01,       // aka NDK15C01 board
    NDK15C03,       // aka NDK15C03 board
    NDK20A01,       // aka NDK20A01 board
    NDK20A03,
    MOP500ED,
    MOP500
};

/*
   typedef struct boardRevision{
   char name[160]; // official full board name
   char nikName[32]; // board aka name, ex. DEVL, DNK15B02
   int id; // enum of predefined revision ID's
   int sensPort0; // i2c address to scan for CCP0 socket
   int sensPort1; // i2c address to scan for CCP1 socket
   t_uint8 eWarpDebug; // eWarp debug mode state
   t_uint8 endianCCP0; // sensor little endian i2c mode for CCP0
   t_uint8 endianCCP1; // sensor little endian i2c mode for CCP1
} ts_boardRev, *tps_boardRev;
*/

//void ITE_initBoardInfo(tps_boardRev, char *);
Result_te ITE_switchBoardDirectI2CMode (void);
Result_te ITE_switchBoardIRPI2CMode (void);
Result_te ITE_switchBoardSensor0 (void);
Result_te ITE_switchBoardSensor1 (void);

#   ifdef __cplusplus
}


#   endif
#endif /* ITE_BOARDINFO_H */

