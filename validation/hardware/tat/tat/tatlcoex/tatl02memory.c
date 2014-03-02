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

#include "tatlcoex.h"

char *MountError = "Unable to mount memory device";
char *OpenError = "Unable to open file from memory device";
char *WriteError = "Unable to mount memory device";
char *LseekError = "Unable to set offset at the beginning of the file";

int tatl02_00Memory_Exec(struct dth_element *elem)
{
	int vl_error = 0;

	switch (elem->user_data) {
	case ACT_MC0:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_mc0_thread,
					      (void *)&v_tatcoex_mc0_data,
					      (void *)tatl02_04Memory_Process,
					      v_tatcoex_mc0_data.order,
					      v_tatcoex_mc0_data.state);
		break;

	case ACT_MC2:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_mc2_thread,
					      (void *)&v_tatcoex_mc2_data,
					      (void *)tatl02_04Memory_Process,
					      v_tatcoex_mc2_data.order,
					      v_tatcoex_mc2_data.state);
		break;

	case ACT_MC4:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_mc2_thread,
					      (void *)&v_tatcoex_mc4_data,
					      (void *)tatl02_04Memory_Process,
					      v_tatcoex_mc4_data.order,
					      v_tatcoex_mc4_data.state);
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;
}

int tatl02_01Memory_Get(struct dth_element *elem, void *value)
{
	int vl_error = 0;

	switch (elem->user_data) {
	case ACT_MC0_STATE:
		{
			*((u8 *) value) = v_tatcoex_mc0_data.state;
			SYSLOG(LOG_DEBUG, "Get MC0 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_MC0_ERROR:
		{
			strncpy((char *)value, v_tatcoex_mc0_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get MC0 error string: %s\n", (char *)value);
		}
		break;

	case ACT_MC2_STATE:
		{
			*((u8 *) value) = v_tatcoex_mc2_data.state;
			SYSLOG(LOG_DEBUG, "Get MC2 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_MC2_ERROR:
		{
			strncpy((char *)value, v_tatcoex_mc2_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get MC2 error string: %s\n", (char *)value);
		}
		break;

	case ACT_MC4_STATE:
		{
			*((u8 *) value) = v_tatcoex_mc4_data.state;
			SYSLOG(LOG_DEBUG, "Get MC4 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_MC4_ERROR:
		{
			strncpy((char *)value, v_tatcoex_mc4_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get MC4 error string: %s\n", (char *)value);
		}
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

int tatl02_02Memory_Set(struct dth_element *elem, void *value)
{
	int vl_error = 0;

	switch (elem->user_data) {
	case ACT_MC0:
		{
			v_tatcoex_mc0_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set MC0 order START(0) STOP(1): %i\n", v_tatcoex_mc0_data.order);
		}
		break;

	case ACT_MC2:
		{
			v_tatcoex_mc2_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set M2 order START(0) STOP(1): %i\n", v_tatcoex_mc2_data.order);
		}
		break;

	case ACT_MC4:
		{
			v_tatcoex_mc4_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set MC4 order START(0) STOP(1): %i\n", v_tatcoex_mc4_data.order);
		}
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

void tatl02_03Init_Mc_Data(DTH_MEMORY_PROCESS_DATA *data, char *source, char *target, char *file, char *fstype)
{
	data->order = STOP;
	data->state = DEACTIVATED;
	data->error = NO_ERROR;
	data->source = source;
	data->target = target;
	data->file = file;
	data->fstype = fstype;
}

void tatl02_04Memory_Process(void *threadData)
{
	DTH_MEMORY_PROCESS_DATA *memoryData;
	int result;
	int fd;

	memoryData = (DTH_MEMORY_PROCESS_DATA *) threadData;
	result = 0;
	fd = 0;

	/* Mount filesystem. */
	if (mount(memoryData->source, memoryData->target, memoryData->fstype, MS_SYNCHRONOUS, NULL) < 0) {
		SYSLOG(LOG_ERR, "Error during mounting filesystem on %s : %s\n", memoryData->source, strerror(errno));
		memoryData->error = MountError;
		pthread_exit(NULL);
	}

	/* File creation. */
	fd = open(memoryData->file, O_CREAT | O_SYNC | O_WRONLY, 0222);
	if (fd  < 0) {
		SYSLOG(LOG_ERR, "Error during open %s file: %s\n", memoryData->file, strerror(errno));
		memoryData->error = OpenError;

		if (umount(memoryData->target) < 0)
			SYSLOG(LOG_ERR, "Error during unmounting filesystem on %s : %s\n", memoryData->source, strerror(errno));
		pthread_exit(NULL);
	}

	/* Perform continuous writing accesses on device memory */
	memoryData->state = ACTIVATED;
	while (memoryData->order == START) {
		int i;
		for (i = 0; i < 10; i++) {
			if (write(fd, "generate traffic on EMMC/MMC/SD\n", 32) < 0) {
				SYSLOG(LOG_ERR, "Error during writing into %s file: %s\n", memoryData->file, strerror(errno));
				tatl02_05Free_Memory_Device(fd, memoryData, WriteError);
				pthread_exit(NULL);
			}

			/* Set offset at the beginning of the file. */
			if (lseek(fd, 0, SEEK_SET) < 0) {
				SYSLOG(LOG_ERR, "Error during lseek operation in %s file: %s\n", memoryData->file, strerror(errno));
				tatl02_05Free_Memory_Device(fd, memoryData, LseekError);
				pthread_exit(NULL);
			}
			memoryData->error = NO_ERROR;
		}
		usleep(500);
	}

	/* End of the process. */
	tatl02_05Free_Memory_Device(fd, memoryData, NO_ERROR);
	pthread_exit(NULL);

}

void tatl02_05Free_Memory_Device(int fd, DTH_MEMORY_PROCESS_DATA *threadData, char *error)
{
	char command[200];

	memset(command, 0, 200);

	/* Close file used for continous writing accesses. */
	close(fd);

	/* Delete file from filesystem. */
	snprintf(command, 200, "rm -f %s", threadData->file);
	if (system(command) < 0)
		SYSLOGSTR(LOG_WARNING, "Unable to delete file fom filesystem: %s\n", threadData->file);

	/* Umount device memory file system */
	if (umount(threadData->target) < 0)
		SYSLOG(LOG_WARNING, "Error during unmounting filesystem on %s : %s\n", threadData->source, strerror(errno));

	/* Update data. */
	threadData->error = error;
	threadData->state = DEACTIVATED;
}
