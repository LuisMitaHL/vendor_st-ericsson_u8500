/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _ITE_HOST2SENSOR_H
#define _ITE_HOST2SENSOR_H


#include "ite_buffer_management.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum {
 NORMAL_MODE = 0,
 COLORBAR,
 FADETOGREY_COLORBAR,
 PN28,
 HORIZ_GREY_SCALE,
 VERT_GREY_SCALE,
 SOLIDBAR_YELLOW,
 SOLIDBAR_CYAN,
 SOLIDBAR_MAGENTA,
 SOLIDBAR_BLACK,
 SOLIDBAR_WHITE,
 SOLIDBAR_RED,
 SOLIDBAR_BLUE,
 SOLIDBAR_GREEN
} e_sensormode;
 

#ifdef __cplusplus
extern "C"
{
#endif

void ITE_SensorMode(e_sensormode sensormode);
void ITE_SensorPutCross(t_uint16 Xpos,t_uint16 Ypos,t_uint16 Xwidth,t_uint16 Ywidth,e_basic_color color);
void ITE_SensorRemoveCross(void);

int SensorRead(int deviceid, int index,int count);
int SensorWrite(int deviceid, int index,int count, int data);
void ITE_WaitI2CBusEwarp(void);
void ITE_WaitI2CCommandEwarp(t_uint16 commandCoin);
void ITE_I2CWriteWord2Sensor(t_uint16 sensorIdx, t_uint16 data);
void ITE_I2CWriteByte2Sensor(t_uint16 sensorIdx, t_uint8 data);
t_uint16 ITE_I2CReadWordFromSensor(t_uint16 sensorIdx);
t_uint16 ITE_I2CReadByteFromSensor(t_uint16 sensorIdx);
void ITE_WriteByteI2CSequence(t_uint16 (*array)[2]);
void ITE_WriteWordI2CSequence(t_uint16 (*array)[2]);
void ITE_ReadWordI2CSequenceEwarp(t_uint16 (*array)[2]);
void ITE_ReadByteI2CSequenceEwarp(t_uint16 (*array)[2]);
void ITE_SendByteToSensorStr(char *str);

#ifdef __cplusplus
}
#endif

#endif   /* _ITE_HOST2SENSOR_H */
