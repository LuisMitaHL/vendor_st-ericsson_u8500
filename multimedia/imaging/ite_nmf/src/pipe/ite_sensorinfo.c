/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "ite_sensorinfo.h"
#include "ite_testenv_utils.h"
#include "ite_pipeinfo.h"
#include <string.h>
#include <stdlib.h>

#include "ite_iniparser.h"
#include "ssc.h"
#include "ite_platform.h"

#include <los/api/los_api.h>
#include "ite_main.h"
#include "cli.h"

extern t_uint32 ite_sensorselect;

char g_sensor_select[20] ;

int discoveredID=0, discoveredRevision=0;
// The dictionary strings are freed in stopenv in ite_nmf_standard_functions
dictionary  *ini = NULL;

ts_sensInfo SensorsInfo[2];

#ifdef __ARM_LINUX
#ifndef ANDROID
    #include <linux/i2c.h>
    #include <linux/i2c-dev.h>
#endif
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <unistd.h>
    #include <stdio.h>
#endif


void ITE_DiscoverSensorsViaI2C(int* discoveredID, int* discoveredRevision)
{
   UNUSED(discoveredID);
   UNUSED(discoveredRevision);
#ifdef __ARM_LINUX
#ifndef ANDROID
    char i2cname[1024];
    int i2cfd = -1;
    char tab_i2c_data[2];


    //Clean parameters
    *discoveredID = 0;
    *discoveredRevision = 0;

    //Open I2C2 device
    strcpy(i2cname,"/dev/i2c-2");
    i2cfd = open(i2cname, O_RDWR);
    if (i2cfd < 0)
    {
     LOS_Log("Error: failed to open %s.\n", i2cname);
    return;
    }

     //Set sensor address
    if (ioctl(i2cfd, I2C_SLAVE, 0x10) < 0)
    {
       //LOS_Log("Error: failed to access slave address 0x20\n");
     return;
    }

    //Set high ID index
    tab_i2c_data[0]=0x00;
    tab_i2c_data[1]=0x00;
    //Write index
    if (write(i2cfd, tab_i2c_data, 2) !=2)
    {
      //LOS_Log("Error: failed to write I2C commands\n");
    }
     //Read Model high ID
    if (read(i2cfd, tab_i2c_data, 1) != 1)
    {
      //LOS_Log("Error: failed to read I2C respond\n");
    }

    *discoveredID = tab_i2c_data[0] << 8;

    //Set low ID index
    tab_i2c_data[0]=0x00;
    tab_i2c_data[1]=0x01;
    //Write index
    if (write(i2cfd, tab_i2c_data, 2) !=2)
    {
      //LOS_Log("Error: failed to write I2C commands\n");
    }
    //Read Model low ID
    if (read(i2cfd, tab_i2c_data, 1) != 1)
    {
      //LOS_Log("Error: failed to read I2C respond\n");
    }

     *discoveredID += tab_i2c_data[0];
    LOS_Log("Near sensor 0x%X detected on sensor0\n", (void*)*discoveredID, NULL, NULL, NULL, NULL, NULL);


     //Set revision index
    tab_i2c_data[0]=0x00;
    tab_i2c_data[1]=0x02;
    //Send index
    if (write(i2cfd, tab_i2c_data, 2) !=2)
    {
      //LOS_Log("Error: failed to write I2C commands\n");
    }
    //Read revision ID
    if (read(i2cfd, tab_i2c_data, 1) != 1)
    {
      //LOS_Log("Error: failed to read I2C respond\n");
    }

    LOS_Log("Rev = %x\n",tab_i2c_data[0]);
    *discoveredRevision = tab_i2c_data[0];
    close(i2cfd);
#endif // ANDROID
#endif // __ARM_LINUX

}


int ITE_autoDetectSensors(void)
{
    int errorCode = 0; // sensor identity absent in sensInfoDatabase
    Result_te err = Result_e_Success;

    SensorsInfo[0].revision.manufacture = NULL;
    SensorsInfo[1].revision.manufacture = NULL;
    SensorsInfo[0].revision.number = 0;
    SensorsInfo[1].revision.number = 0;
#ifndef ANDROID
    // Activate I2C2 for enabling communication with sensors from the ARM
    err = ITE_switchBoardDirectI2CMode();
     if(err != Result_e_Success)
     {
        LOS_Log("ITE_autoDetectSensors ITE_switchBoardDirectI2CMode ERROR %d:",err);
        ITE_NMF_ASSERT(err);
     }
#endif

 if(ite_sensorselect==1)
 {
    err = ITE_InitSecondary();
    if(err != Result_e_Success)
    {
        LOS_Log("ITE_autoDetectSensors ITE_InitSecondary ERROR %d:",err);
        ITE_NMF_ASSERT(err);
    }


 #ifndef ANDROID
    err = ITE_switchBoardSensor1();
    if (0 != errorCode)
    {
        LOS_Log("ITE_autoDetectSensors ITE_switchBoardSensor1 = %d", errorCode);
        ITE_NMF_ASSERT(0);
    }
#endif

 }

else
 {

    err = ITE_InitPrimary();
    if(err != Result_e_Success)
    {
        LOS_Log("ITE_autoDetectSensors ITE_InitPrimary ERROR %d:",err);
        ITE_NMF_ASSERT(err);
    }


#if !(defined(__ARM_LINUX))
else
 {
    err = ITE_switchBoardSensor0();
    if(err != Result_e_Success)
    {
        LOS_Log("ITE_autoDetectSensors ITE_switchBoardSensor0 ERROR %d:",err);
        ITE_NMF_ASSERT(err);
    }
    ITE_DiscoverSensorsViaI2C( &discoveredID, &discoveredRevision);
 }
#endif

#ifndef ANDROID
    err = ITE_switchBoardSensor0();
    if(err != Result_e_Success)
    {
        LOS_Log("ITE_autoDetectSensors ITE_switchBoardSensor0 ERROR %d:",err);
        ITE_NMF_ASSERT(err);
    }
#endif

}

#ifndef ANDROID
    // Re-enabling IPI2C on GPIO 8&9
    err = ITE_switchBoardIRPI2CMode();
    if(err != Result_e_Success)
    {
        LOS_Log("ITE_autoDetectSensors ITE_switchBoardIRPI2CMode ERROR %d:",err);
        ITE_NMF_ASSERT(err);
    }
#endif

    return errorCode;
}

