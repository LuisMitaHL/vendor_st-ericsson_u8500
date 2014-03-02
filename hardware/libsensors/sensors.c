/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* This file is kind of a prototype, or proof of consept if you will.
 * It shows how to communicate with linux kernel sensors with various
 * linux kernel interfaces. Some require ioctl calls, some use
 * the linux kernel input framework and others have simple
 * sysfs interfaces.
 *
 * The code should not be considered production code as such.
 *
 * The file is indeed quite big and messy as the android framework
 * seems to be designed based on assumptions such as all sensors
 * are on the same HW chip. This is not the case in our platform,
 * where we have different HW chips, vendors and also different
 * kernel interface.
 *
 * Another solution for this would be to create a dummy
 * linux kernel driver that acts as a single point communication
 * channel towards Android. That driver will then communicate
 * with the other kernel drivers.
 */

#define LOG_TAG "Sensors"
#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>

#include <linux/input.h>
#include <sys/time.h>
#include <sys/mman.h>

#include <cutils/atomic.h>
#include <cutils/log.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef SENSORS_U8500
#include "sensors_u8500.h"
#endif

#ifdef SENSORS_U5500
#include "sensors_u5500.h"
#endif

#ifdef SENSORS_U9540
#include "sensors_u9540.h"
#endif
#include "Tsl27713.h"

static unsigned int count_mag;
static unsigned int count_acc;
static unsigned int count_gyr;
static unsigned int count_lux;
static unsigned int count_prox;
static unsigned int count_orien;
static unsigned int count_pressure;

float last_light_data = 70000.0f;

unsigned int delay_mag = MINDELAY_MAGNETIC_FIELD;
unsigned int delay_acc = MINDELAY_ACCELEROMETER;
unsigned int delay_gyr = MINDELAY_GYROSCOPE;
unsigned int delay_lux = MINDELAY_LIGHT;
unsigned int delay_prox = MINDELAY_PROXIMITY;
unsigned int delay_orien = MINDELAY_ORIENTATION;
unsigned int delay_pressure = MINDELAY_PRESSURE;

static pthread_cond_t data_available_cv;

static Sensor_messagequeue  stsensor_msgqueue;

static pthread_mutex_t sensordata_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_proxval = PTHREAD_MUTEX_INITIALIZER;
int continue_next;
int events = 0;
static int count_open_sensors = 0;
static int count_delay_sensors = 0;

Sensor_prox  stprox_val;


void *proximity_getdata();
void *acc_getdata();
void *mag_getdata();
void *gyr_getdata();
void *orien_getdata();
void *lux_getdata();
void *press_getdata();
void *poll_proximity();

char acc_thread_exit;
char mag_thread_exit;
char gyr_thread_exit;
char orien_thread_exit;
char prox_thread_exit  = 1;
char lux_thread_exit;
char press_thread_exit;

#ifdef SENSORS_U9540
void *pressure_getdata();
char pres_thread_exit;
#endif

static Sensor_data sensor_data;

/* sensor driver activate,de-activate and poll functions
   implementation */

static void m_determine_acc_chip();
static void m_determine_gyro_chip();
static void set_accel_path();

/*pass values to kernel space*/


static int write_cmd(char const *path, char *cmd, int size)
{
	int fd, ret;
	char * mesg; 

	fd = open(path, O_WRONLY);
	if (fd < 0) {
		mesg= strerror(errno);
		ALOGE("Cannot open %s, fd = %d, msg = %s\n", path, fd, mesg);
		return -ENODEV;
	}

	ret = write(fd, cmd, size);
	if (ret != size) {
		mesg= strerror(errno);
		ALOGE("path = %s\n", path);
		ALOGE("Error. Wrote: %d, should have written: %d, msg = %s\n", ret, size, strerror(errno));
	}

	close(fd);
	return ret;
}


/* implement individual sensor enable and disables */
static int activate_acc(int enable)
{
	int ret = 0;
	pthread_attr_t attr;
	pthread_t acc_thread = -1;

	if (enable) {
		ALOGD("%s: ========= count_acc = %d, accid = %d\n", __func__, count_acc, acc_id);
		if (count_acc == 0) {
			if (acc_id == LSM303DLH_CHIP_ID) {
				ret = write_cmd(sensor_data.path_mode,
						LSM303DLH_A_MODE_NORMAL , 2);
				ret = write_cmd(sensor_data.path_rate,
						LSM303DLH_A_RATE_50, 3);
				ret = write_cmd(sensor_data.path_range,
						LSM303DLH_A_RANGE_2G, 2);
			} else if (acc_id == LSM303DLHC_CHIP_ID) {
				ret = write_cmd(sensor_data.path_mode,
						LSM303DLHC_A_MODE_NORMAL, 2);
				ret = write_cmd(sensor_data.path_range,
					SENSOR_UX500_ACCELEROMETER_RANGE, 2);
			}
			if (ret != -ENODEV) {
				acc_thread_exit = 0;
				pthread_attr_init(&attr);
				/*
				 * Create thread in detached state, so that we
				 * need not join to clear its resources
				 */
				pthread_attr_setdetachstate(&attr,
						PTHREAD_CREATE_DETACHED);
				ret = pthread_create(&acc_thread, &attr,
						acc_getdata, NULL);
				pthread_attr_destroy(&attr);
				count_acc++;
			}
		} else {
			count_acc++;
		}
	} else {
		if (count_acc == 0)
			return 0;
		count_acc--;
		if (count_acc == 0) {
			/*
			 * Enable acc_thread_exit to exit the thread
			 */
			acc_thread_exit = 1;
			write_cmd(sensor_data.path_mode,
				 LSM303DLH_A_MODE_OFF, 2);
		}
	}
	return ret;
}

static int activate_mag(int enable)
{
	int ret = 0;
	pthread_attr_t attr;
	pthread_t mag_thread = -1;

	if (enable) {
		if (count_mag == 0) {
			ret = write_cmd(PATH_MODE_MAG, LSM303DLH_M_MODE_CONTINUOUS,
					2);
			ret = write_cmd(PATH_RATE_MAG, LSM303DLH_M_RATE_15_00, 3);
			ret = write_cmd(PATH_X_SCALE_MAG, sensor_data.magn_range,
					(strlen(sensor_data.magn_range) + 1));

			if (ret != -ENODEV) {
				mag_thread_exit = 0;
				pthread_attr_init(&attr);
				/*
				 * Create thread in detached state, so that we
				 * need not join to clear its resources
				 */
				pthread_attr_setdetachstate(&attr,
						PTHREAD_CREATE_DETACHED);
				ret = pthread_create(&mag_thread, &attr,
						mag_getdata, NULL);
				pthread_attr_destroy(&attr);
				count_mag++;
			}
		} else {
			count_mag++;
		}
	} else {
		if (count_mag == 0)
			return 0;
		count_mag--;
		if (count_mag == 0) {
			/*
			 * Enable mag_thread_exit to exit the thread
			 */
			mag_thread_exit = 1;
			write_cmd(PATH_MODE_MAG, LSM303DLH_M_MODE_SLEEP, 2);
		}
	}
	return ret;
}

static int activate_gyr(int enable)
{
	int ret = 0;
	pthread_attr_t attr;
	pthread_t gyr_thread = -1;
	if (enable) {
		if (count_gyr == 0) {
			ret = write_cmd(sensor_data.gyro_path_mode,
							L3G4200D_MODE_ON,2);
			ret = write_cmd(sensor_data.gyro_path_rate,
							L3G4200D_RATE_100, 4);
			ret = write_cmd(sensor_data.gyro_path_sensitivity,
							L3G4200D_RANGE_250, 8);

			if (ret != -ENODEV) {
				gyr_thread_exit = 0;
				pthread_attr_init(&attr);
				/*
				 * Create thread in detached state, so that we
				 * need not join to clear its resources
				 */
				pthread_attr_setdetachstate(&attr,
						PTHREAD_CREATE_DETACHED);
				ret = pthread_create(&gyr_thread, &attr,
						gyr_getdata, NULL);
				pthread_attr_destroy(&attr);
				count_gyr++;
			}
		} else {
			count_gyr++;
		}
	} else {
		if (count_gyr == 0)
			return 0;
		count_gyr--;
		if (count_gyr == 0) {
			/*
			 * Enable gyr_thread_exit to exit the thread
			 */
			gyr_thread_exit = 1;
			write_cmd(sensor_data.gyro_path_mode,
					L3G4200D_MODE_OFF, 2);
		}
	}
	return ret;
}

#ifdef TSL27713_SENSOR
static int tslFd = -1;
static int power_on_tsl27713()
{
	tslFd = 0;

	tslFd = open("/dev/tsl27713", O_RDONLY);

 	if(tslFd >= 0) {
		ioctl(tslFd, TAOS_IOCTL_SENSOR_ON, 0);
		ioctl(tslFd, TAOS_IOCTL_PROX_CALIBRATE, 0);
		ioctl(tslFd, TAOS_IOCTL_ALS_CALIBRATE, 0);
	} else {
		return -1;
	}

	return 0;
}

static int update_event_loop()
{
	int ret = -1;
	static pthread_t thread = -1;

	if (prox_thread_exit > 0 && 
		(count_lux > 0 || count_prox > 0)){
		prox_thread_exit = 0;
	
		//ALOGD("update_event_loop, %d\n",  prox_thread_exit);
		ret = pthread_create(&thread, NULL, proximity_getdata, NULL);
	}

	if (prox_thread_exit == 0 &&
		(count_lux == 0 && count_prox == 0)){
		prox_thread_exit = 1;

		//ret = pthread_join(thread, NULL);
		//ALOGD("update_event_loop, thread:%d\n",  thread);
	}

	return ret;
}

static int activate_lux(int enable)
{
	int ret = 0;
	pthread_attr_t attr;
	pthread_t lux_thread = -1;

	/*Power On Chip if needed*/
	if (tslFd < 0) 
		power_on_tsl27713();

	if (enable) {
		if (count_lux == 0) {
			ioctl(tslFd, TAOS_IOCTL_ALS_ON, 0);
			lux_thread_exit = 0;

			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr,
					PTHREAD_CREATE_DETACHED);
			ret = pthread_create(&lux_thread, &attr,
					lux_getdata, NULL);
			pthread_attr_destroy(&attr);
			
			count_lux++;
			ret = update_event_loop();
		} else {
			count_lux++;
		}
	} else {
		if (count_lux == 0)
			return 0;
		count_lux--;
		if (count_lux == 0) {
			lux_thread_exit = 1;
			ioctl(tslFd, TAOS_IOCTL_ALS_OFF, 0);
			ret = update_event_loop();
		}
	}

	return ret;
}

static int activate_prox(int enable)
{
	int ret = -1;
	pthread_attr_t attr;
	pthread_t prox_thread = -1;


	/*Power On Chip if needed*/
	if (tslFd < 0) power_on_tsl27713();

	if (enable) {
		if (count_prox == 0) {
			ioctl(tslFd, TAOS_IOCTL_PROX_ON, 0);

			pthread_attr_init(&attr);
			/*
			 * Create thread in detached state, so that we
			 * need not join to clear its resources
			 */
			pthread_attr_setdetachstate(&attr,
					PTHREAD_CREATE_DETACHED);
			ret = pthread_create(&prox_thread, &attr,
					poll_proximity, NULL);
			pthread_attr_destroy(&attr);
			
			count_prox++;
			ret = update_event_loop();
		} else {
			count_prox++;
		}
	} else {
		if (count_prox == 0)
			return 0;
		count_prox--;
		if (count_prox == 0) {
			ioctl(tslFd, TAOS_IOCTL_PROX_OFF, 0);
			ret = update_event_loop();
		}
	}
	
	return ret;
}
#else

static int activate_lux(int enable)
{
	int ret = -1;
	pthread_attr_t attr;
	pthread_t lux_thread = -1;

	if (enable) {
		if (count_lux == 0) {
			ret = write_cmd(PATH_POWER_LUX, BH1780GLI_ENABLE, 2);

			if (ret != -ENODEV) {
				lux_thread_exit = 0;
				pthread_attr_init(&attr);
				/*
				 * Create thread in detached state, so that we
				 * need not join to clear its resources
				 */
				pthread_attr_setdetachstate(&attr,
						PTHREAD_CREATE_DETACHED);
				ret = pthread_create(&lux_thread, &attr,
						lux_getdata, NULL);
				pthread_attr_destroy(&attr);
				count_lux++;
			}
		} else {
			count_lux++;
		}
	} else {
		if (count_lux == 0)
			return 0;
		count_lux--;
		if (count_lux == 0) {
			/*
			 * Enable lux_thread_exit to exit the thread
			 */
			lux_thread_exit = 1;
			write_cmd(PATH_POWER_LUX, BH1780GLI_DISABLE, 2);
		}
	}
	return ret;
}


static int activate_prox(int enable)
{
	int ret = -1;
	pthread_attr_t attr;
	pthread_t prox_thread = -1;

	if (enable) {
		if (count_prox == 0) {
			/*
			 * check for the file path
			 * Initialize prox_thread_exit flag
			 * every time thread is created
			 */
			if ((ret = open(PATH_INTR_PROX, O_RDONLY)) < 0)
				return ret;
			close(ret);
			prox_thread_exit = 0;
			pthread_attr_init(&attr);
			/*
			 * Create thread in detached state, so that we
			 * need not join to clear its resources
			 */
			pthread_attr_setdetachstate(&attr,
					PTHREAD_CREATE_DETACHED);
			ret = pthread_create(&prox_thread, &attr,
					proximity_getdata, NULL);
			pthread_attr_destroy(&attr);
			count_prox++;
		} else {
			count_prox++;
		}
	} else {
		if (count_prox == 0)
			return 0;
		count_prox--;
		if (count_prox == 0) {
			/*
			 * Enable prox_thread_exit to exit the thread
			 */
			prox_thread_exit = 1;
		}
	}
	return ret;
}
#endif

static int activate_orientation(int enable)
{
	int ret = 0;
	pthread_attr_t attr;
	pthread_t orien_thread = -1;

	if (enable) {
		if (count_orien == 0) {
			ret = write_cmd(PATH_MODE_MAG,
					LSM303DLH_M_MODE_CONTINUOUS, 2);
			ret = write_cmd(PATH_RATE_MAG,
					LSM303DLH_M_RATE_15_00, 3);
			ret = write_cmd(PATH_X_SCALE_MAG, sensor_data.magn_range,
					(strlen(sensor_data.magn_range) + 1));

			if (acc_id == LSM303DLH_CHIP_ID) {
				ret = write_cmd(sensor_data.path_mode,
						LSM303DLH_A_MODE_NORMAL, 2);
				ret = write_cmd(sensor_data.path_rate,
						LSM303DLH_A_RATE_50, 2);
				ret = write_cmd(sensor_data.path_range,
						LSM303DLH_A_RANGE_2G, 2);
			} else if (acc_id == LSM303DLHC_CHIP_ID) {
				ret = write_cmd(sensor_data.path_mode,
						LSM303DLHC_A_MODE_NORMAL, 2);
				ret = write_cmd(sensor_data.path_range,
						LSM303DLHC_A_RANGE_2G, 2);
			}

			if (ret != -ENODEV) {
				orien_thread_exit = 0;
				pthread_attr_init(&attr);
				/*
				 * Create thread in detached state, so that we
				 * need not join to clear its resources
				 */
				pthread_attr_setdetachstate(&attr,
						PTHREAD_CREATE_DETACHED);
				ret = pthread_create(&orien_thread, &attr,
						orien_getdata, NULL);
				pthread_attr_destroy(&attr);
				count_orien++;
			}
		} else {
			count_orien++;
		}
	} else {
		if (count_orien == 0)
			return 0;
		count_orien--;
		if (count_orien == 0) {
			/*
			 * Enable orien_thread_exit to exit the thread
			 */
			orien_thread_exit = 1;
		}
	}
	return ret;
}

#ifdef SENSORS_U9540
static int activate_pressure(int enable)
{
	int ret = -1;
	pthread_attr_t attr;
	pthread_t thread = -1;

	if (enable) {
		if (count_pressure == 0) {
			/*
			 * check for the file path
			 * Initialize pres_thread_exit flag
			 * every time thread is created
			 */
			ret = write_cmd(PATH_MODE_PRS, LPS331AP_ENABLE , 2);
			if (ret != -ENODEV) {
			  pres_thread_exit = 0;
			  pthread_attr_init(&attr);
			  /*
			   * Create thread in detached state, so that we
			   * need not join to clear its resources
			   */
			  pthread_attr_setdetachstate(&attr,
					PTHREAD_CREATE_DETACHED);
			  ret = pthread_create(&thread, &attr,
					pressure_getdata, NULL);
			  pthread_attr_destroy(&attr);
			  count_pressure++;
			}
		} else {
			count_pressure++;
		}
	} else {
		if (count_pressure == 0)
			return 0;
		count_pressure--;
		if (count_pressure == 0) {
			/*
			 * Enable pres_thread_exit to exit the thread
			 */
			pres_thread_exit = 1;
			write_cmd(PATH_MODE_PRS, LPS331AP_DISABLE , 2);
		}
	}
	return ret;
}
#else
static int activate_pressure(int enable)
{
	int ret = 0;
	pthread_attr_t attr;
	pthread_t pres_thread = -1;

	if (enable) {
		if (count_pressure == 0) {
			ret = write_cmd(PATH_MODE_PRS, LPS001WM_ENABLE , 2);
			if (ret != -ENODEV) {
				press_thread_exit = 0;
				pthread_attr_init(&attr);
				/*
				 * Create thread in detached state, so that we
				 * need not join to clear its resources
				 */
				pthread_attr_setdetachstate(&attr,
						PTHREAD_CREATE_DETACHED);
				ret = pthread_create(&pres_thread, &attr,
						press_getdata, NULL);
				pthread_attr_destroy(&attr);
				count_pressure++;
			}
		} else
			count_pressure++;
	} else {
		if (count_pressure == 0)
			return 0;
		count_pressure--;
		if (count_pressure == 0) {
			/*
			 * Enable press_thread_exit to exit the thread
			 */
			press_thread_exit = 1;
			write_cmd(PATH_MODE_PRS, LPS001WM_DISABLE , 2);
		}
	}
	return ret;
}
#endif

static int poll_accelerometer(sensors_event_t *values)
{
	int fd;
	int nread;
	int data[3];
	char buf[SIZE_OF_BUF];

	data[0] = 0;
	data[1] = 0;
	data[2] = 0;

	fd = open(sensor_data.path_data, O_RDONLY);
	if (fd < 0) {
		ALOGE("Cannot open %s\n", sensor_data.path_data);
		return -ENODEV;
	}

	memset(buf, 0x00, sizeof(buf));
	lseek(fd, 0, SEEK_SET);
	nread = read(fd, buf, SIZE_OF_BUF);
	if (nread < 0) {
		ALOGE("Error in reading data from accelerometer\n");
		return -1;
	}
	sscanf(buf, "%d:%d:%d", &data[0], &data[1], &data[2]);

	values->acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;
	values->acceleration.x = (float) data[0];
	values->acceleration.x *= CONVERT_A;
	values->acceleration.y = (float) data[1];
	values->acceleration.y *= CONVERT_A;
	values->acceleration.z = (float) data[2];
	values->acceleration.z *= CONVERT_A;

	values->type = SENSOR_TYPE_ACCELEROMETER;
	values->sensor = HANDLE_ACCELEROMETER;
	values->version = sizeof(struct sensors_event_t);

	close(fd);
	return 0;
}

/*
 * Check if same sensor type is already existing in the queue,
 * if so update the element or add at the end of queue.
 */
void add_queue(int sensor_type, sensors_event_t data)
{
	int i;
	pthread_mutex_lock(&sensordata_mutex);
	for (i = 0; i < stsensor_msgqueue.length; i++) {
		if (stsensor_msgqueue.sensor_data[i].sensor == sensor_type) {
			stsensor_msgqueue.sensor_data[i] = data;
			pthread_mutex_unlock(&sensordata_mutex);
			return;
		}
	}
	stsensor_msgqueue.sensor_data[stsensor_msgqueue.length] = data;
	stsensor_msgqueue.length++;
	/* signal event to mpoll if this is the first element in queue */
	if (stsensor_msgqueue.length == 1)
		pthread_cond_signal(&data_available_cv);
	pthread_mutex_unlock(&sensordata_mutex);
	return;
}

void *acc_getdata()
{
	sensors_event_t data;
	int ret;

	while (!acc_thread_exit) {
		usleep(delay_acc);
		ret = poll_accelerometer(&data);
		/* If return value = 0 queue the element */
		if (ret)
			return NULL;
		add_queue(HANDLE_ACCELEROMETER, data);
	}
	return NULL;
}

static int poll_magnetometer(sensors_event_t *values)
{
	int fd;
	int fd_x_gain;
	int fd_z_gain;
	int data[3];
	float x_gain;
	float y_gain;
	float z_gain;
	char buf[SIZE_OF_BUF];
	char buf_gain[SIZE_OF_BUF];
	int nread;

	data[0] = 0;
	data[1] = 0;
	data[2] = 0;

	fd = open(PATH_DATA_MAG, O_RDONLY);
	if (fd < 0) {
		ALOGE("Cannot open %s\n", PATH_DATA_MAG);
		return -ENODEV;
	}

	memset(buf, 0x00, sizeof(buf));
	lseek(fd, 0, SEEK_SET);
	nread = read(fd, buf, SIZE_OF_BUF);
	if (nread < 0) {
		ALOGE("Error in reading data from magnetometer\n");
		return -1;
	}
	sscanf(buf, "%d:%d:%d", &data[0], &data[1], &data[2]);
	close(fd);

	fd_x_gain = open(PATH_X_SCALE_MAG, O_RDONLY);
	if (fd_x_gain < 0) {
		ALOGE("Cannot open %s\n", PATH_X_SCALE_MAG);
		return -ENODEV;
	}

	memset(buf_gain, 0x00, sizeof(buf_gain));
	lseek(fd_x_gain, 0, SEEK_SET);
	nread = read(fd_x_gain, buf_gain, SIZE_OF_BUF);
	if (nread < 0) {
		ALOGE("Error in reading gain from magnetometer\n");
		return -1;
	}
	sscanf(buf_gain, "%f", &x_gain);
	close(fd_x_gain);

	/*As scale for x and y are same no need to read yscale*/
	y_gain = x_gain;

	fd_z_gain = open(PATH_Z_SCALE_MAG, O_RDONLY);
	if (fd_z_gain < 0) {
		ALOGE("Cannot open %s\n", PATH_Z_SCALE_MAG);
		return -ENODEV;
	}

	memset(buf_gain, 0x00, sizeof(buf_gain));
	lseek(fd_z_gain, 0, SEEK_SET);
	nread = read(fd_z_gain, buf_gain, SIZE_OF_BUF);
	if (nread < 0) {
		ALOGE("Error in reading gain from magnetometer\n");
		return -1;
	}
	sscanf(buf_gain, "%f", &z_gain);
	close(fd_z_gain);
	values->magnetic.status = SENSOR_STATUS_ACCURACY_HIGH;
	values->magnetic.x = (data[0] * 100) * x_gain;
	values->magnetic.y = (data[1] * 100) * y_gain;
	values->magnetic.z = (data[2] * 100) * z_gain;
	values->sensor = HANDLE_MAGNETIC_FIELD;
	values->type = SENSOR_TYPE_MAGNETIC_FIELD;
	values->version = sizeof(struct sensors_event_t);

	return 0;
}

void *mag_getdata()
{
	sensors_event_t data;
	int ret;

	while (!mag_thread_exit) {
		usleep(delay_mag);
		ret = poll_magnetometer(&data);
		/* If return value = 0 queue the element */
		if (ret)
			return NULL;
		add_queue(HANDLE_MAGNETIC_FIELD, data);
	}
	return NULL;
}

static int poll_gyroscope(sensors_event_t *values)
{
	int fd;
	int data[3];
	char buf[SIZE_OF_BUF];
	int nread;
	float gyro_sensitivity;
	int fd_sensitivity;

	data[0] = 0;
	data[1] = 0;
	data[2] = 0;

	fd = open(sensor_data.gyro_path_data, O_RDONLY);
	if (fd < 0) {
		ALOGE("Cannot open %s\n", sensor_data.gyro_path_data);
		return -ENODEV;
	}

	memset(buf, 0x00, sizeof(buf));
	lseek(fd, 0, SEEK_SET);
	nread = read(fd, buf, SIZE_OF_BUF);
	if (nread < 0) {
		ALOGE("Error in reading data from Gyroscope\n");
		return -1;
	}
	sscanf(buf, "%d:%d:%d", &data[0], &data[1], &data[2]);

	fd_sensitivity = open(sensor_data.gyro_path_sensitivity, O_RDONLY);
	if (fd_sensitivity < 0) {
		ALOGE("Cannot open %s\n", PATH_SENSITIVITY_GYR);
		return -ENODEV;
	}
	memset(buf, 0x00, sizeof(buf));
	lseek(fd_sensitivity, 0, SEEK_SET);
	nread = read(fd_sensitivity, buf, SIZE_OF_BUF);
	if (nread < 0) {
		ALOGE("Error in reading data from Gyroscope\n");
		return -1;
	}
	sscanf(buf, "%f", &gyro_sensitivity);
	values->magnetic.status = SENSOR_STATUS_ACCURACY_HIGH;
	values->gyro.x = (float)data[0];
	values->gyro.x *= (DEGREES_TO_RADIANS*gyro_sensitivity);
	values->gyro.y = (float)data[1];
	values->gyro.y *= (DEGREES_TO_RADIANS*gyro_sensitivity);
	values->gyro.z = (float)data[2];
	values->gyro.z *= (DEGREES_TO_RADIANS*gyro_sensitivity);
	values->sensor = HANDLE_GYROSCOPE;
	values->type = SENSOR_TYPE_GYROSCOPE;
	values->version = sizeof(struct sensors_event_t);
	close(fd);
	close(fd_sensitivity);
	return 0;
}

void *gyr_getdata()
{
	sensors_event_t data;
	int ret;

	while (!gyr_thread_exit) {
		usleep(delay_gyr);
		ret = poll_gyroscope(&data);
		/* If return value = 0 queue the element */
		if (ret)
			return NULL;
		add_queue(HANDLE_GYROSCOPE, data);
	}
	return NULL;
}

static int poll_orientation(sensors_event_t *values)
{
	int fd_mag;
	int fd_acc;
	int fd_gain;
	int data_mag[3];
	int data_acc[3];
	float gain_mag[2] = {0.0};
	char buf[SIZE_OF_BUF];
	char buf_gain[SIZE_OF_BUF];
	int nread;
	double mag_x, mag_y, mag_xy;
	double acc_x, acc_y, acc_z;

	data_mag[0] = 0;
	data_mag[1] = 0;
	data_mag[2] = 0;

	data_acc[0] = 0;
	data_acc[1] = 0;
	data_acc[2] = 0;

	fd_acc = open(sensor_data.path_data, O_RDONLY);
	if (fd_acc < 0) {
		ALOGE("orien:Cannot open %s\n", sensor_data.path_data);
		return -ENODEV;
	}
	fd_mag = open(PATH_DATA_MAG, O_RDONLY);
	if (fd_mag < 0) {
		ALOGE("orien:Cannot open %s\n", PATH_DATA_MAG);
		return -ENODEV;
	}
	fd_gain = open(PATH_X_SCALE_MAG, O_RDONLY);
	if (fd_gain < 0) {
		ALOGE("orien:Cannot open %s\n", PATH_X_SCALE_MAG);
		return -ENODEV;
	}

	memset(buf, 0x00, sizeof(buf));
	lseek(fd_mag, 0, SEEK_SET);
	nread = read(fd_mag, buf, SIZE_OF_BUF);
	if (nread < 0) {
		ALOGE("orien:Error in reading data from Magnetometer\n");
		return -1;
	}
	sscanf(buf, "%d:%d:%d", &data_mag[0], &data_mag[1], &data_mag[2]);

	memset(buf_gain, 0x00, sizeof(buf_gain));
	lseek(fd_gain, 0, SEEK_SET);
	nread = read(fd_gain, buf_gain, SIZE_OF_BUF);
	if (nread < 0) {
		ALOGE("orien:Error in reading gain from magnetometer\n");
		return -1;
	}
	sscanf(buf_gain, "%f", &gain_mag[0]);

	gain_mag[1] = gain_mag[0];

	mag_x = (data_mag[0] * 100) * gain_mag[0];
	mag_y = (data_mag[1] * 100) * gain_mag[1];
	if (mag_x == 0) {
		if (mag_y < 0)
			values->orientation.azimuth = 180;
		else
			values->orientation.azimuth = 0;
	} else {
		mag_xy = mag_y / mag_x;
		if (mag_x > 0)
			values->orientation.azimuth = round(270 +
						(atan(mag_xy) * RADIANS_TO_DEGREES));
		else
			values->orientation.azimuth = round(90 +
						(atan(mag_xy) * RADIANS_TO_DEGREES));
	}

	memset(buf, 0x00, sizeof(buf));
	lseek(fd_acc, 0, SEEK_SET);
	nread = read(fd_acc, buf, SIZE_OF_BUF);
	if (nread < 0) {
		ALOGE("orien:Error in reading data from Accelerometer\n");
		return -1;
	}
	sscanf(buf, "%d:%d:%d", &data_acc[0], &data_acc[1], &data_acc[2]);

	acc_x = (float) data_acc[0];
	acc_x *= CONVERT_A;
	acc_y = (float) data_acc[1];
	acc_y *= CONVERT_A;
	acc_z = (float) data_acc[2];
	acc_z *= CONVERT_A;

	values->sensor = HANDLE_ORIENTATION;
	values->type = SENSOR_TYPE_ORIENTATION;
	values->version = sizeof(struct sensors_event_t);
	values->orientation.status = SENSOR_STATUS_ACCURACY_HIGH;
	values->orientation.pitch = round(atan(acc_y / sqrt(acc_x*acc_x + acc_z*acc_z)) * RADIANS_TO_DEGREES);
	values->orientation.roll = round(atan(acc_x / sqrt(acc_y*acc_y + acc_z*acc_z)) * RADIANS_TO_DEGREES);

	close(fd_acc);
	close(fd_mag);
	close(fd_gain);
	return 0;
}

void *orien_getdata()
{
	sensors_event_t data;
	int ret;

	while (!orien_thread_exit) {
		usleep(delay_orien);
		ret = poll_orientation(&data);
		/* If return value = 0 queue the element */
		if (ret)
			return NULL;
		add_queue(HANDLE_ORIENTATION, data);
	}
	return NULL;
}

#ifdef TSL27713_SENSOR
Sensor_lux stlux_val;

static int poll_light(sensors_event_t *values)
{
	pthread_mutex_lock( &mutex_proxval );
	if(stlux_val.lux_flag)
	{
		//ALOGD("poll_light 0, %d\n", stlux_val.lux_val);
		stlux_val.lux_flag = 0;
		values->type = SENSOR_TYPE_LIGHT;
		values->sensor = HANDLE_LIGHT;
		values->light = stlux_val.lux_val;
		values->version = sizeof(struct sensors_event_t);
		continue_next = 0;
		events = 1;
	}
	else {
		//ALOGD("poll_light 1\n");
		continue_next = 1;
		if ((count_delay_sensors == 0) && (count_open_sensors > 0))
			events = 0;
	}

	pthread_mutex_unlock( &mutex_proxval );
	return 0;
}

void *poll_proximity()
{
	int ret;
	sensors_event_t values;

	while (!prox_thread_exit) {
		pthread_mutex_lock( &mutex_proxval );
		if(stprox_val.prox_flag)
		{
			stprox_val.prox_flag = 0;
			 /* normalize the distance */
			 if (stprox_val.prox_val == 0)
				values.distance = 0.0f;
			 else
				values.distance = 50.0f;
			 values.sensor = HANDLE_PROXIMITY;
			 values.type = SENSOR_TYPE_PROXIMITY;
			 values.version = sizeof(struct sensors_event_t);
//ALOGD("poll_proximity 0, \n", values.distance);
			 add_queue(HANDLE_PROXIMITY, values);
			 
			 continue_next = 0;
			 events = 1;
		} else {
//ALOGD("poll_proximity 1, \n", values.distance);
			continue_next = 1;
			if ((count_delay_sensors == 0) && (count_open_sensors > 0))
				events = 0;
		}
		
		pthread_mutex_unlock( &mutex_proxval );

		/*
		 * events should be immediate but
		 * hardware & os drivers should support it.
		 * let give chnace to self disable and reading
		 * other sensors
		 */
		usleep(200000);
	}
	return NULL;
}


void *proximity_getdata()
{
	int fd = -1,retval = -1;
	fd_set read_set;
	struct input_event ev;
	struct timeval tv;
	int size = sizeof(struct input_event);
	/* Initialize the structures */
	memset(&ev, 0x00, sizeof(ev));
	memset(&tv, 0x00, sizeof(tv));

	/* open input device */
	if ((fd = open("/dev/input/event0", O_RDONLY)) > 0) {
		while(!prox_thread_exit) {
			/* Intialize the read descriptor */
			FD_ZERO(&read_set);
			FD_SET(fd,&read_set);
			/* Wait up to 0.5 seconds. */
			tv.tv_sec = 0 ;
			tv.tv_usec = 500000;
			retval = select(fd+1, &read_set, NULL, NULL, &tv);
			if (retval > 0) {
				/* FD_ISSET(0, &rfds) will be true. */
				if (FD_ISSET(fd, &read_set)) {
					read(fd, &ev, size );
					if(ev.type == EV_ABS) {
						pthread_mutex_lock( &mutex_proxval );
						switch (ev.code) {
							case ABS_DISTANCE:
								stprox_val.prox_flag = 1;
								stprox_val.prox_val = ev.value;
								break;

							case ABS_MISC:
								stlux_val.lux_flag = 1;
								stlux_val.lux_val = ev.value;
								break;

							default:								
								ALOGE("Got unsupported event: code 0x%x, value %d\n", ev.code, ev.value);
								break;
						}
						pthread_mutex_unlock( &mutex_proxval );
					}
#ifdef DYRON_DEUBG_TSL27713					
					ALOGD("%s: tsl27713 type = %#x, MISC = %#x   DISTANCE = %#x, val = %#x==============", 
						__func__, ev.type == EV_ABS, ev.code == ABS_MISC, ev.code == ABS_DISTANCE, ev.value);
#endif
				}
			}
		}
		close(fd);
	}
	else
	   ALOGD("\n /dev/input/event0 is not a valid device");
	return NULL;
}
#else
static int poll_light(sensors_event_t *values)
{
	int fd;
	char buf[8];
	float reading;
	int ret;

	fd = open(PATH_DATA_LUX, O_RDONLY);
	if (fd < 0) {
		ALOGE("Error in opening %s file", PATH_DATA_LUX);
		return -ENODEV;
	}
	memset(buf, 0x00, sizeof(buf));
	lseek(fd, 0, SEEK_SET);
	read(fd, buf, 8);
	reading = atof(buf);

	/* make assignment only when activated and value has changed*/
	if ((last_light_data != reading) && (count_lux)) {
		values->type = SENSOR_TYPE_LIGHT;
		values->sensor = HANDLE_LIGHT;
		values->light = reading;
		last_light_data = reading;
		values->version = sizeof(struct sensors_event_t);
		ret = 0;
	} else {
		ret = -1;
	}

	close(fd);
	return ret;
}

void *proximity_getdata()
{
	int fd = -1,retval = -1;
	fd_set read_set;
	struct input_event ev;
	struct timeval tv;
	int size = sizeof(struct input_event);
	sensors_event_t data;

	/* Initialize the structures */
	memset(&ev, 0x00, sizeof(ev));
	memset(&tv, 0x00, sizeof(tv));
	/* open input device */
	if ((fd = open(PATH_INTR_PROX, O_RDONLY)) > 0) {
		while (!prox_thread_exit) {
			/* Intialize the read descriptor */
			FD_ZERO(&read_set);
			FD_SET(fd,&read_set);
			/* Wait up to 0.5 seconds. */
			tv.tv_sec = 0 ;
			tv.tv_usec = 500000;
			retval = select(fd+1, &read_set, NULL, NULL, &tv);
			if (retval > 0 && count_prox) {
				/* FD_ISSET(0, &rfds) will be true. */
				if (FD_ISSET(fd, &read_set)) {
					read(fd, &ev, size );
					if (11 == ev.code) {
						data.sensor = HANDLE_PROXIMITY;
						data.type = SENSOR_TYPE_PROXIMITY;
						data.version = sizeof(struct sensors_event_t);
						if (ev.value == 1)
							data.distance = 0.0f;
						else
							data.distance = 50.0f;
						/* queue the element */
						add_queue(HANDLE_PROXIMITY, data);
					}
				}
			}
		}
		close(fd);
	}
	else
	   ALOGD("STE libsensors: /dev/input/event0 is not a valid device\n");
	return NULL;
}

#endif

void *lux_getdata()
{
	sensors_event_t data;
	int ret;

	while (!lux_thread_exit) {
		ret = poll_light(&data);
		/* If return value = 0 queue the element */
		if (!ret)
			add_queue(HANDLE_LIGHT, data);
		/*
		 * events should be immediate but
		 * hardware & os drivers should support it.
		 * let give chnace to self disable and reading
		 * other sensors
		 */
		usleep(200000);
	}
	return NULL;
}



#ifdef SENSORS_U9540
void *pressure_getdata()
{
	int fd = -1,retval = -1;
	fd_set read_set;
	struct input_event ev;
	struct timeval tv;
	int size = sizeof(struct input_event);
	sensors_event_t data;

	/* Initialize the structures */
	memset(&ev, 0x00, sizeof(ev));
	memset(&tv, 0x00, sizeof(tv));
	/* open input device */
	if ((fd = open(PATH_DATA_PRS, O_RDONLY)) > 0) {
		while (!pres_thread_exit) {
			/* Intialize the read descriptor */
			FD_ZERO(&read_set);
			FD_SET(fd,&read_set);
			/* Wait up to 0.5 seconds. */
			tv.tv_sec = 0 ;
			tv.tv_usec = 500000;
			retval = select(fd+1, &read_set, NULL, NULL, &tv);
			if ((retval > 0) && count_pressure) {
				/* FD_ISSET(0, &rfds) will be true. */
				if (FD_ISSET(fd, &read_set)) {
					read(fd, &ev, size );
					if (ABS_PRESSURE == ev.code) {
						/* normalize the pressure - shouldn't this be done via sensors.conf? */
						data.pressure = ev.value/4096;
						data.sensor = HANDLE_PRESSURE;
						data.type = SENSOR_TYPE_PRESSURE;
						data.version = sizeof(struct sensors_event_t);
						/* queue the element */
						add_queue(HANDLE_PRESSURE, data);
					}
				}
			}
		}
		close(fd);
	}
	return NULL;
}
#else
static int poll_pressure(sensors_event_t *values)
{
	int fd;
	float data;
	char buf[8];
	int nread;
	fd = open(PATH_DATA_PRS, O_RDONLY);
	if (fd < 0) {
		ALOGE("\n Error in opening pressure sensor\n");
		return -ENODEV;
	}

	memset(buf, 0x00, sizeof(buf));
	lseek(fd, 0, SEEK_SET);
	nread = read(fd, buf, sizeof(buf));
	if (nread < 0) {
		ALOGE("Error in reading pressure sensor output\n");
		return -1;
	}
	data = atof(buf);
	values->type = SENSOR_TYPE_PRESSURE;
	values->sensor = HANDLE_PRESSURE;
	values->pressure = data;
	values->version = sizeof(struct sensors_event_t);
	close(fd);
	return 0;
}

void *press_getdata()
{
	sensors_event_t data;
	int ret;

	while (!press_thread_exit) {
		usleep(delay_pressure);
		ret = poll_pressure(&data);
		/* If return value > 0 queue the element */
		if (ret)
			return NULL;
		add_queue(HANDLE_PRESSURE, data);
	}
	return NULL;
}
#endif

static void set_accel_path()
{
	/*
	 * check the board and Accelerometer CHIP ID
	 * and set the paths accordingly
	 */
	#if defined SENSORS_U8500 || defined SENSORS_U9540
	if (acc_id == LSM303DLH_CHIP_ID) {
		strncpy(sensor_data.path_mode, PATH_MODE_ACC, (MAX_LENGTH - 1));
		strncpy(sensor_data.path_range, PATH_RANGE_ACC, (MAX_LENGTH - 1));
		strncpy(sensor_data.path_rate, PATH_RATE_ACC, (MAX_LENGTH - 1));
		strncpy(sensor_data.path_data, PATH_DATA_ACC, (MAX_LENGTH - 1));
		strcpy(sensor_data.magn_range, LSM303DLH_M_RANGE_4_0G);
	} else if (acc_id == LSM303DLHC_CHIP_ID) {
		strncpy(sensor_data.path_mode, PATH_MODE_DLHC_ACC, (MAX_LENGTH - 1));
		strncpy(sensor_data.path_range, PATH_RANGE_DLHC_ACC, (MAX_LENGTH - 1));
		strncpy(sensor_data.path_data, PATH_DATA_DLHC_ACC, (MAX_LENGTH - 1));
		strcpy(sensor_data.magn_range, LSM303DLHC_M_RANGE_4_0G);
	}
	#endif
	#ifdef SENSORS_U5500
	strncpy(sensor_data.path_mode, PATH_MODE_ACC, (MAX_LENGTH - 1));
	strncpy(sensor_data.path_range, PATH_RANGE_ACC, (MAX_LENGTH - 1));
	strncpy(sensor_data.path_data, PATH_DATA_ACC, (MAX_LENGTH - 1));
	strcpy(sensor_data.magn_range, LSM303DLH_M_RANGE_4_0G);
	#endif
}

static int m_open_sensors(const struct hw_module_t *module,
		const char *name, struct hw_device_t **device);

static int m_sensors_get_sensors_list(struct sensors_module_t *module,
		struct sensor_t const **list)
{
	*list = sSensorList;

	return sizeof(sSensorList) / sizeof(sSensorList[0]);
}

static struct hw_module_methods_t m_sensors_module_methods = {
	.open = m_open_sensors
};

struct sensors_module_t HAL_MODULE_INFO_SYM = {
	.common = {
		.tag = HARDWARE_MODULE_TAG,
		.version_major = 1,
		.version_minor = 0,
		.id = SENSORS_HARDWARE_MODULE_ID,
		.name = "STE ux500 SENSORS Module",
		.author = "ST-Ericsson AB",
		.methods = &m_sensors_module_methods,
	},
	.get_sensors_list = m_sensors_get_sensors_list
};

/* enable and disable sensors here */
static int m_poll_activate(struct sensors_poll_device_t *dev,
		int handle, int enabled)
{
	int status = 0;
	int count_open_sensors = 0;
	ALOGD("STE libsensors: Entering function %s with handle = %d,"
			" enable = %d\n", __FUNCTION__, handle, enabled);

	switch (handle) {
	case HANDLE_ORIENTATION:
		status = activate_orientation(enabled);
		break;
	case HANDLE_ACCELEROMETER:
		status = activate_acc(enabled);
		break;
	case HANDLE_MAGNETIC_FIELD:
		status = activate_mag(enabled);
		break;
	case HANDLE_GYROSCOPE:
		status = activate_gyr(enabled);
		break;
	case HANDLE_LIGHT:
		status = activate_lux(enabled);
		break;
	case HANDLE_PROXIMITY:
		status = activate_prox(enabled);
		break;
	case HANDLE_PRESSURE:
		status = activate_pressure(enabled);
		break;
	default:
		ALOGD("STE libsensors:This sensor/handle is not supported %s\n",
				__FUNCTION__);
		break;
	}

	/* check if sensor is missing then exit gracefully */
	if (status != -ENODEV) {
		/* count total number of sensors open */
		count_open_sensors = count_acc + count_mag + count_lux +
					count_prox + count_orien +
					count_gyr + count_pressure;
		if (count_open_sensors == 0) {
			pthread_mutex_lock(&sensordata_mutex);
			stsensor_msgqueue.length = 0;
			pthread_mutex_unlock(&sensordata_mutex);
		}
	}

	return 0;
}

static int set_delay_acc(int microseconds)
{
	int ret = 0;
	if (acc_id == LSM303DLH_CHIP_ID) {
		if (microseconds == 20000)
			ret = write_cmd(sensor_data.path_rate,
				LSM303DLH_A_RATE_100, 4);
		else if (microseconds < 20000)
			ret = write_cmd(sensor_data.path_rate,
					LSM303DLH_A_RATE_1000, 5);
	} else if (acc_id == LSM303DLHC_CHIP_ID) {
		if (microseconds == 20000)
			ret = write_cmd(sensor_data.path_mode,
				LSM303DLHC_A_MODE_NORMAL_100HZ, 2);
		else if (microseconds < 20000)
			ret = write_cmd(sensor_data.path_mode,
				LSM303DLHC_A_MODE_NORMAL_1344HZ, 2);
	}
	return ret;
}

static int set_delay_mag(int microseconds)
{
	int ret = 0;
	if (acc_id == LSM303DLH_CHIP_ID) {
		if (microseconds <= 20000)
			ret = write_cmd(PATH_RATE_MAG,
					LSM303DLH_M_RATE_75, 3);
	} else if (acc_id == LSM303DLHC_CHIP_ID) {
		if (microseconds <= 66667)
			ret = write_cmd(PATH_RATE_MAG,
					LSM303DLH_M_RATE_75, 3);
		else if (microseconds < 20000)
			ret = write_cmd(PATH_RATE_MAG,
					LSM303DLH_M_RATE_220, 4);
	}
	return ret;
}

static int m_poll_set_delay(struct sensors_poll_device_t *dev,
		int handle, int64_t ns)
{
	int microseconds = ns / 1000;
	int ret = 0;

	ALOGD("STE libsensors: set delay = %d in microseconds for handle = %d\n"
							, microseconds, handle);

	switch (handle) {
	case HANDLE_ORIENTATION:
		if (microseconds >= MINDELAY_ORIENTATION) {
			delay_orien = microseconds;
			ret = set_delay_acc(microseconds);
			ret = set_delay_mag(microseconds);
		}
		break;
	case HANDLE_ACCELEROMETER:
		if (microseconds >= MINDELAY_ACCELEROMETER) {
			delay_acc = microseconds;
			ret = set_delay_acc(microseconds);
		}
		break;
	case HANDLE_MAGNETIC_FIELD:
		if (microseconds >= MINDELAY_MAGNETIC_FIELD) {
			delay_mag = microseconds;
			ret = set_delay_mag(microseconds);
		}
		break;
	case HANDLE_GYROSCOPE:
		if (microseconds >= MINDELAY_GYROSCOPE) {
			delay_gyr = microseconds;
			if (microseconds < 20000)
				ret = write_cmd(sensor_data.gyro_path_rate,
							L3G4200D_RATE_800, 4);
		}
		break;
	case HANDLE_LIGHT:
		/* ignored */
		break;
	case HANDLE_PROXIMITY:
		/* ignored */
		break;
	case HANDLE_PRESSURE:
		if (microseconds >= MINDELAY_PRESSURE)
			delay_pressure = microseconds;
		break;
	default:
		ALOGD("STE libsensors:This sensor/handle is not supported %s\n",
				__FUNCTION__);
		break;
	}
	if (ret < 0)
		return -1;
	else
		return 0;
}


static int m_poll(struct sensors_poll_device_t *dev,
		sensors_event_t *data, int count)
{
	int i;
	struct timeval time;
	int events = 0;

	pthread_mutex_lock(&sensordata_mutex);
	/* If there are no elements in the queue
	 * wait till queue gets filled
	 */
	if (!stsensor_msgqueue.length)
		pthread_cond_wait(&data_available_cv, &sensordata_mutex);
	memcpy(data, &stsensor_msgqueue.sensor_data[0] ,
			sizeof(stsensor_msgqueue.sensor_data[0]));
	if (stsensor_msgqueue.length > 1) {
		for (i = 0; i < stsensor_msgqueue.length - 1; i++)
			memcpy(&stsensor_msgqueue.sensor_data[i],
			&stsensor_msgqueue.sensor_data[i+1],
			sizeof(stsensor_msgqueue.sensor_data[0]));
	}
	if(stsensor_msgqueue.length > 0)
		stsensor_msgqueue.length--;
	events = 1;
	pthread_mutex_unlock(&sensordata_mutex);
	/* add time stamp on last event */
	gettimeofday(&time, NULL);
	data->timestamp = (time.tv_sec * 1000000000LL) +
					(time.tv_usec * 1000);
	return events;
}

/* close instace of the deevie */
static int m_poll_close(struct hw_device_t *dev)
{
	struct sensors_poll_device_t *poll_device =
		(struct sensors_poll_device_t *) dev;

	ALOGD("STE libsensors: Closing poll data context.\n");

	pthread_cond_destroy(&data_available_cv);
	pthread_mutex_destroy(&sensordata_mutex);

	if (poll_device)
		free(poll_device);
	return 0;
}

static void m_determine_acc_chip()
{
	int fd = -1;
	int nread;
	char buf[4];
	acc_id = 0;

	fd = open(PATH_ID_ACC, O_RDONLY);
	if (fd < 0) {
		/* check if chip is LSM303DHLC */
		fd = open(PATH_ID_DLHC_ACC, O_RDONLY);
		if (fd < 0) {
			ALOGD("STE libsensors: Error in opening file"
					"line no: %d\n", __LINE__);
			return;
		}
	}

	memset(buf, 0x00, sizeof(buf));
	lseek(fd, 0, SEEK_SET);
	nread = read(fd, buf, sizeof(buf));
	sscanf(buf, "%d", &acc_id);
	close(fd);
}

static void m_determine_gyro_chip()
{
	int fd = -1;

	fd = open(PATH_MODE_GYR, O_RDONLY);
	if (fd < 0) {
		/* check if chip is L3GD20 */
		fd = open(PATH_MODE_GYR_L3GD20, O_RDONLY);
		if (fd > 0) {
			strncpy(sensor_data.gyro_path_mode,
				PATH_MODE_GYR_L3GD20, (MAX_LENGTH - 1));
			strncpy(sensor_data.gyro_path_rate,
				PATH_RATE_GYR_L3GD20, (MAX_LENGTH - 1));
			strncpy(sensor_data.gyro_path_data,
				PATH_DATA_GYR_L3GD20, (MAX_LENGTH - 1));
			strncpy(sensor_data.gyro_path_sensitivity,
				PATH_SENSITIVITY_GYR_L3GD20, (MAX_LENGTH - 1));
		} else {
			ALOGD("STE libsensors:Error in opening file lineno %d\n",
							__LINE__);
			return;
		}
	}
	else {
		strncpy(sensor_data.gyro_path_mode, PATH_MODE_GYR,
							(MAX_LENGTH - 1));
		strncpy(sensor_data.gyro_path_rate, PATH_RATE_GYR,
							(MAX_LENGTH - 1));
		strncpy(sensor_data.gyro_path_data, PATH_DATA_GYR,
							(MAX_LENGTH - 1));
		strncpy(sensor_data.gyro_path_sensitivity,
				PATH_SENSITIVITY_GYR, (MAX_LENGTH - 1));
	}
	close(fd);
}

/* open a new instance of a sensor device using name */
static int m_open_sensors(const struct hw_module_t *module,
		const char *name, struct hw_device_t **device)
{
	ALOGD("STE libsensors: Entering function %s with param name = %s\n",
			__FUNCTION__, name);

	int status = -EINVAL;

	if (!strcmp(name, SENSORS_HARDWARE_POLL)) {
		struct sensors_poll_device_t *poll_device;
		poll_device = malloc(sizeof(*poll_device));
		if (!poll_device)
			return status;
		memset(poll_device, 0, sizeof(*poll_device));
		poll_device->common.tag = HARDWARE_DEVICE_TAG;
		poll_device->common.version = 0;
		poll_device->common.module = (struct hw_module_t *) module;
		poll_device->common.close = m_poll_close;
		poll_device->activate = m_poll_activate;
		poll_device->setDelay = m_poll_set_delay;
		poll_device->poll = m_poll;
		*device = &poll_device->common;

		pthread_cond_init(&data_available_cv, NULL);
		pthread_mutex_init(&sensordata_mutex, NULL);

		m_determine_acc_chip();
		m_determine_gyro_chip();
		set_accel_path();

		status = 0;
	}
	return status;
}
