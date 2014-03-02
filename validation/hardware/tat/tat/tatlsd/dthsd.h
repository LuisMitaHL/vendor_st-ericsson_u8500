/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTHSD_H_
#define DTHSD_H_

#include <syslog.h>
#include <dth_tools.h>
#include <dthsrvhelper/dthsrvhelper.h> /* for dth_element definition */

int dth_init_service();

int Dth_ActSDPresence_exec(struct dth_element *elem);
int Dth_ActSDParamDetection_Get(struct dth_element *elem, void *value);
int Dth_ActSDParamVoltage_Get(struct dth_element *elem, void *value);
int Dth_ActSDParamRegulator_Get(struct dth_element *elem, void *value);
int Dth_ActSDParamControler_Get(struct dth_element *elem, void *value);
int Dth_ActSDParamClock_Get(struct dth_element *elem, void *value);

int Dth_ActSDWriteFile_exec(struct dth_element *elem);
int Dth_ActSDMounted_Get(struct dth_element *elem, void *value);
int Dth_ActSDFileCopied_Get(struct dth_element *elem, void *value);
int Dth_ActSDFileChecksum_Get(struct dth_element *elem, void *value);

#define FILE_PATH SCRIPTSLOCATION
#define OUTPUT_PRESENCE_FILE "checkPresence.txt"
#define OUTPUT_MOUNT_FILE "mountSDCarderrorMessage.txt"
#define INPUT_SCRIPT_MOUNTSD_FILE "mountSDCard.sh"
#define INPUT_SCRIPT_PRESENCE_FILE "checkSDCard.sh "
#define INPUT_SCRIPT_CHECKSUM_FILE "checksumFile.sh"
#define INPUT_FILE_TO_COPY "test_file.1"
#define OUTPUT_CHECKSUM_SOURCE_FILE "checksumOfSourceFile.txt"
#define OUTPUT_CHECKSUM_TARGET_FILE "checksumOfTargetFile.txt"
#define SDCARD_MOUNT_PATH "/mnt/SDCard/"
#define OUTPUT_COPY_FILE "checkCopyFile.txt"
#define INPUT_SCRIPT_COPY_FILE "copyFile.sh"
#define ERROR_MESSAGE "failed:"

typedef struct {
	u8 CardInserted;
	u8 PowerSupplyValue;
	u8 RegulatorValue;
	u8 ControlerStatus;
	u8 ClockStatus;
	u8 CardMounted;
	u8 FileCopied;
	u8 ChecksumCheck;
} SDCardState;

SDCardState mySDCardState
#if defined (TATSD0DTH_C)
={0,0,0,0,0,0,0,0}
#endif
;

#endif /* DTHSD_H_ */
