/* Sensor handles */
#define HANDLE_ACCELEROMETER	(0)
#define HANDLE_MAGNETIC_FIELD	(1)
#define HANDLE_ORIENTATION		(2)
#define HANDLE_GYROSCOPE		(3)
#define HANDLE_LIGHT			(4)
#define HANDLE_PRESSURE			(5)
#define HANDLE_TEMPERATURE		(6)
#define HANDLE_PROXIMITY		(8)
#define HANDLE_MAX			    (9)

/* Sensor handles */
#define MINDELAY_ACCELEROMETER	(1000)
#define MINDELAY_MAGNETIC_FIELD	(1000)
#define MINDELAY_ORIENTATION	(1000)
#define MINDELAY_GYROSCOPE		(1000)
#define MINDELAY_LIGHT			(0)
#define MINDELAY_PRESSURE		(1000)
#define MINDELAY_TEMPERATURE	(0)
#define MINDELAY_PROXIMITY		(0)

/* Constants */
#define LSM_M_MAX_CAL_COUNT 300
#define RADIANS_TO_DEGREES (180.0/M_PI)
#define DEGREES_TO_RADIANS (M_PI/180.0)

/* Magnetometer defines */
#define LSM303DLH_M_RANGE_4_0G "2325580"
#define LSM303DLHC_M_RANGE_4_0G "2222220"
#define LSM303DLH_M_MODE_CONTINUOUS "0"
#define LSM303DLH_M_MODE_SLEEP "3"
#define LSM303DLH_M_RATE_15_00 "15"
#define LSM303DLH_M_RATE_75 "75"
#define LSM303DLH_M_RATE_220 "220"
#define SENSOR_UX500_G_To_uT (100.0f)
#define SENSOR_UX500_MAGNETOMETER_RANGE LSM303DLH_MAGNETOMETER_RANGE_1_9G

#if (SENSOR_UX500_MAGNETOMETER_RANGE == LSM303DLH_MAGNETOMETER_RANGE_1_9G)
#define SENSOR_UX500_MAGNETOMETER_MAX (1.9f * SENSOR_UX500_G_To_uT)
#define SENSOR_UX500_MAGNETOMETER_STEP (1.9f * SENSOR_UX500_G_To_uT / 2048.0f)
#else
#error Unknown range
#endif

/* Accelerometer defines */
#define LSM303DLH_A_RANGE_2G "2"
#define LSM303DLH_A_MODE_OFF "0"
#define LSM303DLH_A_MODE_NORMAL "1"
#define LSM303DLH_A_RATE_50 "50"
#define LSM303DLH_A_RATE_100 "100"
#define LSM303DLH_A_RATE_1000 "1000"
#define LSM303DLH_CHIP_ID 50
#define LSM303DLHC_CHIP_ID 51

#define LSM303DLHC_A_MODE_NORMAL "4"
#define LSM303DLHC_A_MODE_NORMAL_100HZ "5"
#define LSM303DLHC_A_MODE_NORMAL_1344HZ "9"
#define LSM303DLHC_A_RANGE_2G "2"
#define LSM303DLHC_A_RANGE_4G "4"

#define SENSOR_UX500_ACCELEROMETER_RANGE LSM303DLHC_A_RANGE_4G
#define SENSOR_UX500_ACCELEROMETER_MAX (4.0f)
#define SENSOR_UX500_ACCELEROMETER_STEP \
	(SENSOR_UX500_ACCELEROMETER_MAX / 4096.0f)

/* Gyroscopre defines */
#define L3G4200D_MODE_ON "1"
#define L3G4200D_MODE_OFF "0"
#define L3G4200D_RATE_100 "100"
#define L3G4200D_RATE_800 "800"
#define L3G4200D_RANGE_250 "8750000"
#define L3G4200D_SENSITIVITY_250 0.00875
#define L3G4200D_SENSITIVITY_500 0.01750
#define L3G4200D_SENSITIVITY_2000 0.07

#define SIZE_OF_BUF 100
#define CONVERT_A  (GRAVITY_EARTH * (1.0f/1000.0f))


/* ambient light defines */
#define BH1780GLI_ENABLE "3"
#define BH1780GLI_DISABLE "0"

/* proximity defines */
#define SFH7741_ENABLE "1"
#define SFH7741_DISABLE "0"

/* pressure defines */
#define LPS331AP_ENABLE "1"
#define LPS331AP_DISABLE "0"

#define MAX_LENGTH 150

/* magnetometer paths*/
char const *const PATH_MODE_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device2/mode";
char const *const PATH_RATE_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device2/sampling_frequency";
char const *const PATH_DATA_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device2/magn_raw";
char const *const PATH_X_SCALE_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device2/in_magn_x_scale";
char const *const PATH_Y_SCALE_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device2/in_magn_y_scale";
char const *const PATH_Z_SCALE_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device2/in_magn_z_scale";

/* accelerometer paths*/
char const *const PATH_MODE_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0018/iio:device0/mode";
char const *const PATH_RANGE_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0018/iio:device0/in_accel_scale";
char const *const PATH_RATE_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0018/iio:device0/sampling_frequency";
char const *const PATH_DATA_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0018/iio:device0/accel_raw";
char const *const PATH_ID_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0018/iio:device0/id";


/* accelerometer paths*/
char const *const PATH_MODE_DLHC_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0019/iio:device0/mode";
char const *const PATH_RANGE_DLHC_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0019/iio:device0/in_accel_scale";
char const *const PATH_DATA_DLHC_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0019/iio:device0/accel_raw";
char const *const PATH_ID_DLHC_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0019/iio:device0/id";


/* gyroscope paths */
char const *const PATH_MODE_GYR =
		"/sys/class/i2c-dev/i2c-2/device/2-0068/iio:device1/mode";
char const *const PATH_RATE_GYR =
		"/sys/class/i2c-dev/i2c-2/device/2-0068/iio:device1/sampling_frequency";
char const *const PATH_DATA_GYR =
		"/sys/class/i2c-dev/i2c-2/device/2-0068/iio:device1/gyro_raw";
char const *const PATH_SENSITIVITY_GYR =
		"/sys/class/i2c-dev/i2c-2/device/2-0068/iio:device1/in_anglvel_scale";
/* gyroscope paths */
char const *const PATH_MODE_GYR_L3GD20 =
		"/sys/class/i2c-dev/i2c-2/device/2-006a/iio:device1/mode";
char const *const PATH_RANGE_GYR_L3GD20  =
		"/sys/class/i2c-dev/i2c-2/device/2-006a/iio:device1/gyro_range";
char const *const PATH_RATE_GYR_L3GD20  =
		"/sys/class/i2c-dev/i2c-2/device/2-006a/iio:device1/sampling_frequency";
char const *const PATH_DATA_GYR_L3GD20  =
		"/sys/class/i2c-dev/i2c-2/device/2-006a/iio:device1/gyro_raw";
char const *const PATH_SENSITIVITY_GYR_L3GD20 =
		"/sys/class/i2c-dev/i2c-2/device/2-006a/iio:device1/in_anglvel_scale";

/* ambient light paths */
char const *const PATH_POWER_LUX =
		"/sys/class/i2c-dev/i2c-2/device/2-0029/power_state";
char const *const PATH_DATA_LUX =
		"/sys/class/i2c-dev/i2c-2/device/2-0029/lux";

/* proximity paths*/
char const *const PATH_POWER_PROX =
		"/sys/devices/platform/sensors1p.0/proximity_activate";
char const *const PATH_DATA_PROX =
		"/sys/devices/platform/sensors1p.0/proximity";
char const *const PATH_INTR_PROX =
                  "/dev/input/event0";


/* pressure paths*/
char const *const PATH_MODE_PRS = "/sys/devices/virtual/input/input5/enable";
char const *const PATH_DATA_PRS = "/dev/input/event5";

/* To store all Sensors data*/
typedef struct {
   sensors_event_t sensor_data[8];
   int length;
} Sensor_messagequeue;

/* Accelerometer sensor path structure */
typedef struct {
    char path_mode[MAX_LENGTH];
    char path_range[MAX_LENGTH];
    char path_rate[MAX_LENGTH];
    char path_data[MAX_LENGTH];
    char gyro_path_mode[MAX_LENGTH];
    char gyro_path_rate[MAX_LENGTH];
    char gyro_path_data[MAX_LENGTH];
    char gyro_path_sensitivity[MAX_LENGTH];
    char magn_range[MAX_LENGTH];
} Sensor_data;

/* sensor API integration */

static const struct sensor_t sSensorList[] = {
	{"Rohm BH1780GLI Ambient Light sensor",
		"ST-Ericsson AB",
		1,
		HANDLE_LIGHT,
		SENSOR_TYPE_LIGHT,
		65535.0f,
		1.0f,
		0.120f,
		MINDELAY_LIGHT,
		{}
	},
	{"SFH7741 Proximity sensor",
		"ST-Ericsson AB",
		1,
		HANDLE_PROXIMITY,
		SENSOR_TYPE_PROXIMITY,
		50.0f,
		1.0f,
		0.45f,
		MINDELAY_PROXIMITY,
		{}
	},
	{"LSM303DLH 3-axis Magnetic field sensor",
		"ST-Ericsson AB",
		1,
		HANDLE_MAGNETIC_FIELD,
		SENSOR_TYPE_MAGNETIC_FIELD,
		SENSOR_UX500_MAGNETOMETER_MAX,
		SENSOR_UX500_MAGNETOMETER_STEP,
		0.83f,
		MINDELAY_MAGNETIC_FIELD,
		{}
	},
	{"LSM303DLH 3-axis Accelerometer sensor",
		"ST-Ericsson AB",
		1,
		HANDLE_ACCELEROMETER,
		SENSOR_TYPE_ACCELEROMETER,
		SENSOR_UX500_ACCELEROMETER_MAX,
		SENSOR_UX500_ACCELEROMETER_STEP,
		0.83f,
		MINDELAY_ACCELEROMETER,
		{}
	},
	{"LSM303DLH 3-axis Orientation sensor",
		"ST-Ericsson AB",
		1,
		HANDLE_ORIENTATION,
		SENSOR_TYPE_ORIENTATION,
		360.0f,
		1.0f,
		1.66f,
		MINDELAY_ORIENTATION,
		{}
	},
	{"L3G4200D 3-axis Gyroscope sensor",
		"ST-Ericsson AB",
		1,
		HANDLE_GYROSCOPE,
		SENSOR_TYPE_GYROSCOPE,
		34.9f,
		0.0174f,
		0.83f,
		MINDELAY_GYROSCOPE,
		{}
	},
	{"LP331AP Pressure sensor",
		"ST-Ericsson AB",
		1,
		HANDLE_PRESSURE,
		SENSOR_TYPE_PRESSURE,
		1260.0f,
		1.0f,
		0.015f,
		MINDELAY_PRESSURE,
		{}
	},
};

static int acc_id;
