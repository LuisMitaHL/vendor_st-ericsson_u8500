/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/


#include "ite_host2sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "VhcElementDefs.h"

#include "ite_vpip.h"
#include "ite_event.h"

#include <los/api/los_api.h>
#include "ite_main.h"


/********************************************************/
/*      Private functions declaration       */
/********************************************************/

//#include "strtoul.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include <errno.h>
#ifdef NEED_DECLARATION_ERRNO
extern int errno;
#endif
int errno1;
#include "ctype.h"
#ifndef LONG_MAX
#define LONG_MAX ((long)(ULONG_MAX >> 1)) // 0x7FFFFFFF
#endif

#ifndef LONG_MIN
#define LONG_MIN ((long)(~LONG_MAX))  // 0x80000000
#endif

#ifndef ULONG_MAX
#define ULONG_MAX ((unsigned long)(~0L))  // 0xFFFFFFFF
#endif

extern ts_sia_usecase usecase;
// Prototype required
static inline unsigned long ite_strtoul(const char *nptr,
        char **endptr,
        register int base);


//__NO_WARNING__ Only used locally
static inline unsigned long ite_strtoul(const char *nptr,
        char **endptr,
        register int base)
{
    register const char *s = nptr;
    register unsigned long acc;
    register int c;
    register unsigned long cutoff;
    register int neg = 0, any, cutlim;

    //  See strtol for comments as to the logic used.

    do {
        c = *s++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
    if ((base == 0 || base == 16) &&
        c == '0' && (*s == 'x' || *s == 'X')) {
            c = s[1];
            s += 2;
            base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;
    cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
    cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
    for (acc = 0, any = 0;; c = *s++) {
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    if (any < 0) {
        acc = ULONG_MAX;
        errno1 = ERANGE;
    } else if (neg)
        acc = -acc;
    if (endptr != 0)
        *endptr = (char *) (any ? s - 1 : nptr);
    return (acc);
}


#define TEST_MODES_OF_LLA_FW 1

/********************************************************/
/*      "Public" functions          */
/********************************************************/
//Partly porte on 8500 : only cross bar
void ITE_SensorMode(e_sensormode sensormode) {

    if (usecase.sensormode != sensormode) {
        switch (sensormode) {
 case (NORMAL_MODE):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_Normal);
#else
     ITE_I2CWriteWord2Sensor(0x600, 0);
#endif
     break;

 case (COLORBAR):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_SolidColourBars);
#else
     //ITE_I2CWriteWord2Sensor(0x600, 2);
     ITE_I2CWriteByte2Sensor(0x601, 2);
#endif
     break;

 case (FADETOGREY_COLORBAR):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_SolidColourBarsFade);
#else
     ITE_I2CWriteWord2Sensor(0x600, 3);
#endif
     break;

 case (PN28):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_PN9);
#else
     ITE_I2CWriteWord2Sensor(0x600, 258);
#endif
     break;
 case (HORIZ_GREY_SCALE):
     ITE_I2CWriteWord2Sensor(0x600, 256);
     break;

 case (VERT_GREY_SCALE):
     ITE_I2CWriteWord2Sensor(0x600, 257);
     break;

 case (SOLIDBAR_YELLOW):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_SolidColour);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_red_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenR_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_blue_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenB_Byte0, 0x3ff);
#else
     ITE_I2CWriteWord2Sensor(0x600, 1);
     ITE_I2CWriteWord2Sensor(0x602, 0x3ff);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0x3ff);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0x3ff);   //GreenB
#endif
     break;

 case (SOLIDBAR_CYAN):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_SolidColour);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_red_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenR_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_blue_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenB_Byte0, 0x3ff);
#else
     ITE_I2CWriteWord2Sensor(0x600, 1);
     ITE_I2CWriteWord2Sensor(0x602, 0);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0x3ff);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0x3ff);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0x3ff);   //GreenB
#endif
     break;

 case (SOLIDBAR_MAGENTA):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_SolidColour);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_red_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenR_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_blue_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenB_Byte0, 0);
#else
     ITE_I2CWriteWord2Sensor(0x600, 1);
     ITE_I2CWriteWord2Sensor(0x602, 0x3ff);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0x3ff);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0);   //GreenB
#endif
     break;

 case (SOLIDBAR_BLACK):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_SolidColour);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_red_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenR_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_blue_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenB_Byte0, 0);
#else
     ITE_I2CWriteWord2Sensor(0x600, 1);
     ITE_I2CWriteWord2Sensor(0x602, 0);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0);   //GreenB
#endif
     break;

 case (SOLIDBAR_WHITE):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_SolidColour);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_red_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenR_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_blue_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenB_Byte0, 0x3ff);
#else
     ITE_I2CWriteWord2Sensor(0x600, 1);
     ITE_I2CWriteWord2Sensor(0x602, 0x3ff);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0x3ff);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0x3ff);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0x3ff);   //GreenB
#endif
     break;

 case (SOLIDBAR_RED):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_SolidColour);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_red_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenR_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_blue_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenB_Byte0, 0);
#else
     ITE_I2CWriteWord2Sensor(0x600, 1);
     ITE_I2CWriteWord2Sensor(0x602, 0x3ff);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0);   //GreenB
#endif
     break;

 case (SOLIDBAR_BLUE):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_SolidColour);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_red_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenR_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_blue_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenB_Byte0, 0);
#else
     ITE_I2CWriteWord2Sensor(0x600, 1);
     ITE_I2CWriteWord2Sensor(0x602, 0);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0x3ff);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0);   //GreenB
#endif
     break;

 case (SOLIDBAR_GREEN):
#if TEST_MODES_OF_LLA_FW
     ITE_writePE(TestPattern_Ctrl_e_TestPattern_Byte0, TestPattern_e_SolidColour);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_red_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenR_Byte0, 0x3ff);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_blue_Byte0, 0);
     ITE_writePE(TestPattern_Ctrl_u16_test_data_greenB_Byte0, 0x3ff);
#else
     ITE_I2CWriteWord2Sensor(0x600, 1);
     ITE_I2CWriteWord2Sensor(0x602, 0);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0x3ff);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0x3ff);   //GreenB
#endif
     break;
        }
        usecase.sensormode = sensormode;
    }
}

void ITE_SensorPutCross(t_uint16 Xpos,t_uint16 Ypos,t_uint16 Xwidth,t_uint16 Ywidth,e_basic_color color) {
    switch (color) {
 case (YELLOW_COLOR_E):
     ITE_I2CWriteWord2Sensor(0x602, 0x3ff);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0x3ff);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0x3ff);   //GreenB
     break;
 case (CYAN_COLOR_E):
     ITE_I2CWriteWord2Sensor(0x602, 0);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0x3ff);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0x3ff);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0x3ff);   //GreenB
     break;
 case (MAGENTA_COLOR_E):
     ITE_I2CWriteWord2Sensor(0x602, 0x3ff);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0x3ff);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0);   //GreenB
     break;
 case (BLACK_COLOR_E):
     ITE_I2CWriteWord2Sensor(0x602, 0);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0);   //GreenB
     break;
 case (WHITE_COLOR_E):
     ITE_I2CWriteWord2Sensor(0x602, 0x3ff);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0x3ff);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0x3ff);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0x3ff);   //GreenB
     break;
 case (RED_COLOR_E):
     ITE_I2CWriteWord2Sensor(0x602, 0x3ff);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0);   //GreenB
     break;
 case (BLUE_COLOR_E):
     ITE_I2CWriteWord2Sensor(0x602, 0);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0x3ff);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0);   //GreenB
     break;
 case (GREEN_COLOR_E):
     ITE_I2CWriteWord2Sensor(0x602, 0);   //Red
     ITE_I2CWriteWord2Sensor(0x604, 0x3ff);   //GreenR
     ITE_I2CWriteWord2Sensor(0x606, 0);   //Blue
     ITE_I2CWriteWord2Sensor(0x608, 0x3ff);   //GreenB
     break;
    }
    ITE_I2CWriteWord2Sensor(0x60E, Xwidth);
    ITE_I2CWriteWord2Sensor(0x60A, Ywidth);
    ITE_I2CWriteWord2Sensor(0x610, Xpos);
    ITE_I2CWriteWord2Sensor(0x60C, Ypos);
}

void ITE_SensorRemoveCross(void) {

    ITE_I2CWriteWord2Sensor(0x60E, 0);
    ITE_I2CWriteWord2Sensor(0x60A, 0);
    ITE_I2CWriteWord2Sensor(0x610, 0);
    ITE_I2CWriteWord2Sensor(0x60C, 0);
}

/** Wait for the I2C bus to be available (as reported by Ewarp) */
void ITE_WaitI2CBusEwarp() {
    ITE_WaitExpectedPeValue(MasterI2CStatus_e_Resource_Status_Byte0, 0, 50, 1000);
}

/** Wait for the Ewarp to complete an I2C transaction command */
void ITE_WaitI2CCommandEwarp(t_uint16 commandCoin) {
    ITE_WaitExpectedPeValue(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0, commandCoin, 50, 1000);
}


/***********************************************************************************************************************
* \brief  Read I2C sensor command
* This function read the I2C sensor parameters from PE I2C interface
* @param index: Register index
* @param count: size of data in byte (1/2)
* @return data
* \author Denis Hory
\************************************************************************************************************************/
int SensorRead(int deviceid, int index,int count){

    int CommandCoin,tmp,StatusCoin;
    union u_ITE_Event event;

    /*Read */
    do
    {
        StatusCoin = ITE_readPE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0);
        CommandCoin = ITE_readPE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0);
    } while ( StatusCoin != CommandCoin);
    //CommandCoin = ITE_readPE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0);
    ITE_writePE(HostToMasterI2CAccessControl_u16_DeviceID_Byte0, deviceid);        /*Addr I2C du sensor*/
    ITE_writePE(HostToMasterI2CAccessControl_u16_Index_Byte0, index);
    /*register */
    ITE_writePE(HostToMasterI2CAccessControl_e_HostToMasterI2CRequest_Request_Byte0,HostToMasterI2CRequest_e_ReadNBytes);
    ITE_writePE(HostToMasterI2CAccessControl_u8_NoBytesReadWrite_Byte0, count);
    //ISP_HOST_TO_SENSOR_ACCESS_COMPLETE = 0;
    CommandCoin++;
    ITE_writePE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0,CommandCoin);
    /*  do
    {
    StatusCoin = ITE_readPE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0);
    CommandCoin = ITE_readPE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0);
    } while ( StatusCoin != CommandCoin);
    */
    event = ITE_WaitEvent(ISP_HOST_TO_SENSOR_ACCESS_COMPLETE_EVENTS);
    if(event.ispctlInfo.info_id == ISP_HOST_TO_SENSOR_ACCESS_COMPLETE)
    {LOS_Log("HOST_TO_SENSOR_ACCESS_COMPLETE \n");
    if (ITE_readPE(HostToMasterI2CAccessStatus_e_Result_Driver_Byte0) == Result_e_Success)
    {
        if (count == 1) tmp =ITE_readPE(HostToMasterI2CAccessData_u8_arrData_0_Byte0);
        else tmp =(ITE_readPE(HostToMasterI2CAccessData_u8_arrData_0_Byte0)<<8) + ITE_readPE(HostToMasterI2CAccessData_u8_arrData_1_Byte0);
        return tmp;
    }
    return (-1);
    }
    else
    {
        LOS_Log("!!!!!!!!!!NOT RECEIVED THE CORRECT IT!!!!!!\n");
        return (-1);
    }
}



/***********************************************************************************************************************
* \brief  Write I2C sensor command
* This function write the I2C sensor parameters from PE I2C interface
* @param deviceid: device id du sensor ($20 pour s850 $40 pour s861)
* @param index: Register index
* @param count: size of data in byte (1/2)
* @param data: data to write (byte or word)
* \author Denis Hory
*
************************************************************************************************************************/
int SensorWrite(int deviceid, int index,int count, int data){
    int CommandCoin;
    int StatusCoin;
    union u_ITE_Event event;

    do
    {
        StatusCoin = ITE_readPE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0);
        CommandCoin = ITE_readPE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0);
    } while ( StatusCoin != CommandCoin);
    CommandCoin = ITE_readPE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0);
    ITE_writePE(HostToMasterI2CAccessControl_u16_DeviceID_Byte0, deviceid);        /*Addr I2C du Sensor*/
    ITE_writePE(HostToMasterI2CAccessControl_u16_Index_Byte0, index);          /*Index */
    ITE_writePE(HostToMasterI2CAccessControl_e_HostToMasterI2CRequest_Request_Byte0, HostToMasterI2CRequest_e_WriteNBytes);
    ITE_writePE(HostToMasterI2CAccessControl_u8_NoBytesReadWrite_Byte0, count);
    if (count == 1){
        ITE_writePE(HostToMasterI2CAccessData_u8_arrData_0_Byte0, data);
    }else{
        ITE_writePE(HostToMasterI2CAccessData_u8_arrData_0_Byte0, data>>8);
        ITE_writePE(HostToMasterI2CAccessData_u8_arrData_1_Byte0, data);
    }
    CommandCoin++;
    ITE_writePE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0, CommandCoin);
    /*
    do
    {
    StatusCoin = ITE_readPE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0);
    CommandCoin = ITE_readPE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0);
    } while ( StatusCoin != CommandCoin);
    */
    /*Wait end of host to sensor access notification */
    //LOS_Sleep(500); //mle
    event = ITE_WaitEvent(ITE_EVT_ISPCTL_EVENTS);
    if(event.ispctlInfo.info_id == ISP_HOST_TO_SENSOR_ACCESS_COMPLETE)
    {
        LOS_Log("HOST_TO_SENSOR_ACCESS_COMPLETE \n");
    }
    else
    {
        LOS_Log("!!!!!!!!!!NOT RECEIVED THE CORRECT IT!!!!!!\n");
    }

    if (ITE_readPE(HostToMasterI2CAccessStatus_e_Result_Driver_Byte0) == Result_e_Success)
    {
        return 0;
    }
    return (-1);
}

void ITE_I2CWriteWord2Sensor(t_uint16 sensorIdx, t_uint16 data) {

    //FIXME: 0x20 to be checked for the different sensors
    SensorWrite(0x20, sensorIdx,2, data);
}

//Ported on 8500 V1
void ITE_I2CWriteByte2Sensor(t_uint16 sensorIdx, t_uint8 data) {

    //FIXME: 0x20 to be checked for the different sensors
    SensorWrite(0x20, sensorIdx,1, data);
    /*

    int statusCoin;
    int commandCoin;

    //STep1: check coin_status and coin_command are equal
    do
    {
    statusCoin = ITE_readPE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0);
    commandCoin = ITE_readPE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0);
    } while ( statusCoin != commandCoin);

    //Step2: Program Device ID : I2C slave address
    //FIXME: 0x20 to be checked for the different sensors
    ITE_writePE(HostToMasterI2CAccessControl_u16_DeviceID_Byte0, 0x20);

    //Step3: Program register address
    ITE_writePE(HostToMasterI2CAccessControl_u16_Index_Byte0, sensorIdx);

    //Step4: Program write command
    ITE_writePE(HostToMasterI2CAccessControl_e_HostToMasterI2CRequest_Request_Byte0, HostToMasterI2CRequest_e_WriteNBytes);

    //Step5: Program number of bytes to write
    ITE_writePE(HostToMasterI2CAccessControl_u8_NoBytesReadWrite_Byte0, 1);

    //Step6: Program data to write
    ITE_writePE(HostToMasterI2CAccessData_u8_arrData_0_Byte0, data);

    //Step7: Toggle command coin
    commandCoin++;
    ITE_writePE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0, commandCoin);

    //STep8: check coin_status and coin_command are equal
    do
    {
    statusCoin = ITE_readPE(HostToMasterI2CAccessStatus_e_Coin_Status_Byte0);
    commandCoin = ITE_readPE(HostToMasterI2CAccessControl_e_Coin_Command_Byte0);
    } while ( statusCoin != commandCoin);
    */
}

t_uint16 ITE_I2CReadWordFromSensor(t_uint16 sensorIdx) {

    return(SensorRead(0x20, sensorIdx,2));
}

t_uint16 ITE_I2CReadByteFromSensor(t_uint16 sensorIdx) {

    return(SensorRead(0x20, sensorIdx,1));
}


/** Write a sequence of (address,value) to a sensor through Ewarp's
I2C
\params array: an array of t_uint16[2] = {address,value}. The array must be terminated by a NULL {0, 0} pair.
*/
void ITE_WriteByteI2CSequence(t_uint16 (*array)[2]) {
    int idx;

    for (idx=0;array[idx][0]!=0; idx++) {
        ITE_I2CWriteByte2Sensor(array[idx][0], array[idx][1]);
    }
}

void ITE_WriteWordI2CSequence(t_uint16 (*array)[2]) {
    int idx;

    for (idx=0;array[idx][0]!=0; idx++) {
        ITE_I2CWriteWord2Sensor(array[idx][0], array[idx][1]);
    }
}


void ITE_ReadWordI2CSequenceEwarp(t_uint16 (*array)[2]) {
    int idx;

    for (idx=0;array[idx][0]!=0; idx++) {
        array[idx][1] = ITE_I2CReadWordFromSensor(array[idx][0]);
    }
}

void ITE_ReadByteI2CSequenceEwarp(t_uint16 (*array)[2]) {
    int idx;

    for (idx=0;array[idx][0]!=0; idx++) {
        array[idx][1] = ITE_I2CReadByteFromSensor(array[idx][0]);
    }
}

#define I2CELEMENTSEPARATOR " ,"

/*****************************************************************************/
void ITE_SendByteToSensorStr(char *str) {
    /*****************************************************************************/
    char *token; t_uint16 SensorIndex; t_uint8 data;
    //     unsigned char swap;

    // if(NULL != (token = strtok((char *)str, I2CELEMENTSEPARATOR)))
    if(0x0!= str)
    {
        token = strtok((char *)str, I2CELEMENTSEPARATOR); //mle
        do{
            SensorIndex = (t_uint16) ite_strtoul(token, NULL, 0);
            //      if(endian){
            //      swap = SensorIndex & 0xFF;
            //      SensorIndex = (SensorIndex >> 8) + (swap << 8);
            //      }
            token = strtok(NULL, I2CELEMENTSEPARATOR);
            data = (t_uint8) ite_strtoul(token, NULL, 0);
            ITE_I2CWriteByte2Sensor(SensorIndex, data);
        }while(NULL != (token = strtok(NULL, I2CELEMENTSEPARATOR)));
    }
}



/********************************************************/
/*      "Private" functions             */
/********************************************************/
