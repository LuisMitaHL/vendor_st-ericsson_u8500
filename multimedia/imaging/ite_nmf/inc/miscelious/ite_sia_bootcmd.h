/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ITE_SIA_BOOTCMD_H_
#define ITE_SIA_BOOTCMD_H_

#include <los/api/los_api.h>


#include <inc/type.h>

#include "VhcElementDefs.h"

typedef struct _boardprebootinfo_ {
   t_uint16    	fpExternalClockFrequency_Mhz_MSByte;
   t_uint8 	bSensorSerialAddress;
   t_uint8 	bSensor0XShutDownGPO;
   t_uint8 	bSensor1XShutDownGPO;
   t_uint8    	fSensor0Present;
   t_uint8    	fSensor1Present;
}ts_boardprebootinfo;

typedef struct _sensorprebootinfo_ {
   t_uint8    	fFocusLensActuatorOnSensorPresent;
   t_uint8    	fShutterActuatorOnSensorPresent;
   t_uint16	fpHostRxMaxDataRate_Mbps_MSByte;
   t_uint32 	uwCsiRawFormat_MSByte;
   t_uint8 	bSensorBitsPerSystemClock;
   t_uint32 	VideoTimingMode;
   t_uint8    	fSensorType;		// apparently, only for sensor1
   t_uint8 	bNumOfDataLanes;	// apparently, only for sensor1
   // MUST BE REMOVED
/* For CSI sensor, Ewarp has a bug where it incorrectly sets the digital gain in the sensor,
   so tell Ewarp to not even try */
//   t_uint32 	uwSensorDigitalGainCapability_MSByte;
/* or tell it to set a minimum of 0x100 */
/* Alternatively, I2C sequence in sensor to set gains ourselves:
20E =1, 20F = 0x00, 0x210=1, 0x211=0, 0x212=1, 0x213=0, 0x214=1, 0x215=0 */
//   t_uint32 	uwSensorDigitalGainMinimum_MSByte;
}ts_sensorprebootinfo;

typedef struct _preboot_ {
  ts_boardprebootinfo boardprebootinfo;
  ts_sensorprebootinfo sensor;
}ts_preboot;

extern ts_preboot PreBoot;
extern t_bool g_nvm_raw_data;

#ifdef __cplusplus
extern "C"
{
#endif

/*
void ITE_InitPreBoot(void);
void ITE_SendPreBoot(int smiapp_power_sequence, int sensor_tunning_before_boot);
*/
void ITE_DoPreBootConfiguration();
void ITE_DoPostBootConfiguration();
int ITE_ISP_Boot(int sensorselect, int flag_backup_fw, int smiapp_power_sequence, int sensor_tunning_before_boot);
void ITE_Freefirmware_Ext(void);
t_uint32 t1_to_arm(t_uint32 t1_addr);
#ifdef __TUNING_FILES_AVAILABLE__
t_los_memory_handle ITE_FirmwareTuning(void);
#endif //__TUNING_FILES_AVAILABLE__

#if FLASH_DRIVER_INCLUDE
// ER:408207: Flash driver integration in ITE NMF.

extern int ITE_FlashDriverInit();
extern int ITE_FlashDriverDeInit();
extern int ITE_FlashDriverConfigure();
#endif

#ifdef __cplusplus
}
#endif

#endif /* ITE_SIA_BOOTCMD_H_ */

