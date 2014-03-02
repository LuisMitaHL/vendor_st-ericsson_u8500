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

char *I2cOpenFileError = "Unable to open I2C device file" ;
char *I2cIoctlError = "Unable to address I2C device file" ;
char *I2cReadError = "Unable to read data from I2C device file" ;
char *I2cOpenError = "Unable to open I2C device file" ;

/**
 *  Perform continuous reading accesses on I2C buses.
 *	@param[in] threadData data needed for I2C processing.
 */
static void tatl06_03I2C_Process(void *threadData)
{
	DTH_I2C_PROCESS_DATA *i2cData ;
	int vl_error = 0;
	int fd = 0;
	int count = 1;
	char buf[1024];

	i2cData = (DTH_I2C_PROCESS_DATA *) threadData ;
	memset(buf, 0, 1024);

	if (strcmp(i2cData->deviceName, I2C3_DEVICE_NAME) == 0) {
	/* on 8520 no I2C3. Need to check board type */
		fd = open("/sys/devices/platform/ab8505-i2c.0/ab8500-usb.0/boot_time_device",O_RDWR);
		if (fd<0){
			/* board 8500 */
			count = 6;
		}
		else {
			/* board 8520 : just need to return OK*/
			close (fd);
			while (i2cData->order == START) {
				i2cData->state = ACTIVATED ;
				i2cData->error = NO_ERROR ;
				usleep(1000);
			}
			i2cData->state = DEACTIVATED ;
			pthread_exit(NULL);
		}
	}

	buf[0] = i2cData->registerAddress ; /* register address to be read. */
	i2cData->state = ACTIVATED ;

	fd = i2c_Open(i2cData->deviceName, O_RDWR);
	if (fd < 0) {
		i2cData->error = I2cOpenError ;
		i2cData->state = DEACTIVATED ;
		pthread_exit(NULL);
	}

	if (i2cData->order == START)
		printf("dev=%s slave_addr=%x reg_addr=%x count=%d\n",
				i2cData->deviceName, i2cData->deviceAddress,
				i2cData->registerAddress, count);

	while (i2cData->order == START) {
		vl_error = i2c_Read(fd, i2cData->deviceAddress, buf, count);
		if (vl_error < 0) {
			i2c_Close(fd);
			i2cData->error = I2cReadError ;
			i2cData->state = DEACTIVATED ;
			pthread_exit(NULL);
		}
		i2cData->error = NO_ERROR ;
		usleep(1000);
	}

	i2c_Close(fd);
	i2cData->error = NO_ERROR ;
	i2cData->state = DEACTIVATED ;
	pthread_exit(NULL);
}

int tatl06_00I2C_Exec(struct dth_element *elem)
{
    int vl_error = 0;

	switch (elem->user_data) {
	case ACT_I2C0:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_i2c0_thread, (void *)&v_tatcoex_i2c0_data,
				(void *)tatl06_03I2C_Process, v_tatcoex_i2c0_data.order, v_tatcoex_i2c0_data.state);
	break ;

	case ACT_I2C2:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_i2c2_thread, (void *)&v_tatcoex_i2c2_data,
				(void *)tatl06_03I2C_Process, v_tatcoex_i2c2_data.order, v_tatcoex_i2c2_data.state);
	break ;

	case ACT_I2C3:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_i2c3_thread, (void *)&v_tatcoex_i2c3_data,
				(void *)tatl06_03I2C_Process, v_tatcoex_i2c3_data.order, v_tatcoex_i2c3_data.state);
		break ;

	default:
			vl_error = EBADRQC;
		break ;
	}

	return vl_error ;
}


int tatl06_01I2C_Get(struct dth_element *elem, void *value)
{
    int vl_error = 0;

	switch (elem->user_data) {
	case ACT_I2C0_STATE:
	{
		*((u8 *)value) = v_tatcoex_i2c0_data.state ;
		SYSLOG(LOG_DEBUG, "Get I2C0 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *)value));
	}
	break ;

	case ACT_I2C0_ERROR:
	{
		strncpy((char *)value, v_tatcoex_i2c0_data.error, MAX_STRNG_SIZE);
		SYSLOG(LOG_DEBUG, "Get I2C0 error string: %s\n", (char *) value);
	}
	break ;

	case ACT_I2C2_STATE:
	{
		*((u8 *)value) = v_tatcoex_i2c2_data.state ;
		SYSLOG(LOG_DEBUG, "Get I2C2 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *)value));
	}
	break ;

	case ACT_I2C2_ERROR:
	{
		strncpy((char *)value, v_tatcoex_i2c2_data.error, MAX_STRNG_SIZE);
		SYSLOG(LOG_DEBUG, "Get I2C2 error string: %s\n", (char *) value);
	}
	break ;

	case ACT_I2C3_STATE:
	{
		*((u8 *)value) = v_tatcoex_i2c3_data.state ;
		SYSLOG(LOG_DEBUG, " Get I2C3 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *)value));
	}
	break ;

	case ACT_I2C3_ERROR:
	{
		strncpy((char *)value, v_tatcoex_i2c3_data.error, MAX_STRNG_SIZE);
		SYSLOG(LOG_DEBUG, " Get I2C3 error string: %s\n", (char *) value);
	}
	break ;

	default:
		vl_error = EBADRQC;
	break ;
	}

	return vl_error ;

}


int tatl06_02I2C_Set(struct dth_element *elem, void *value)
{
    int vl_error = 0;

	switch (elem->user_data) {
	case ACT_I2C0:
	{
		v_tatcoex_i2c0_data.order = *((u8 *)value);
		SYSLOG(LOG_DEBUG, "Set I2C0 order START(0) STOP(1): %i\n", v_tatcoex_i2c0_data.order);
	}
	break ;

	case ACT_I2C2:
	{
		v_tatcoex_i2c2_data.order = *((u8 *)value);
		SYSLOG(LOG_DEBUG, "Set I2C2 order START(0) STOP(1): %i\n", v_tatcoex_i2c2_data.order);
	}
	break ;

	case ACT_I2C3:
	{
		v_tatcoex_i2c3_data.order = *((u8 *)value);
		SYSLOG(LOG_DEBUG, " Set I2C3 order START(0) STOP(1): %i\n", v_tatcoex_i2c3_data.order);
	}
	break ;

	default:
		vl_error = EBADRQC;
	break ;
	}

	return vl_error ;

}

void tatl06_03Init_I2C_Data(DTH_I2C_PROCESS_DATA *data, char *deviceName,
		unsigned char deviceAddress, unsigned char registerAddress)
{
	data->order = STOP ;
	data->state = DEACTIVATED ;
	data->error = NO_ERROR ;
	data->deviceName = deviceName ;
	data->deviceAddress = deviceAddress ;
	data->registerAddress = registerAddress ;
}


