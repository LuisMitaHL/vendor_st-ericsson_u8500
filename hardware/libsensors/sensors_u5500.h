/* Sensor handles */
#define HANDLE_ACCELEROMETER	(0)
#define HANDLE_MAGNETIC_FIELD	(1)
#define HANDLE_ORIENTATION		(2)
#define HANDLE_PRESSURE			(3)
#define HANDLE_TEMPERATURE		(4)
#define HANDLE_MAX			    (5)

/* dummy defines, set them to high value */
#define HANDLE_GYROSCOPE		(9999)
#define HANDLE_LIGHT			(9998)
#define HANDLE_PROXIMITY		(9997)

/* Sensor handles */
#define MINDELAY_ACCELEROMETER	(1000)
#define MINDELAY_MAGNETIC_FIELD	(1000)
#define MINDELAY_ORIENTATION	(1000)
#define MINDELAY_GYROSCOPE		(1000)
#define MINDELAY_LIGHT			(0)
#define MINDELAY_PRESSURE		(0)
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

#define SENSOR_UX500_ACCELEROMETER_RANGE LSM303DLHC_A_RANGE_2G
#define SENSOR_UX500_ACCELEROMETER_MAX (4.0f)
#define SENSOR_UX500_ACCELEROMETER_STEP \
	(SENSOR_UX500_ACCELEROMETER_MAX / 4096.0f)

/* Gyroscopre defines */
#define L3G4200D_MODE_ON "1"
#define L3G4200D_MODE_OFF "0"
#define L3G4200D_RATE_100 "100"
#define L3G4200D_RATE_800 "800"
#define L3G4200D_RANGE_250 "0"
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
#define LPS001WM_ENABLE "1"
#define LPS001WM_DISABLE "0"

#define MAX_LENGTH 150

/* magnetometer paths*/
char const *const PATH_MODE_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device1/mode";
char const *const PATH_RATE_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device1/sampling_frequency";
char const *const PATH_DATA_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device1/magn_raw";
char const *const PATH_X_SCALE_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device2/in_magn_x_scale";
char const *const PATH_Y_SCALE_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device2/in_magn_y_scale";
char const *const PATH_Z_SCALE_MAG =
		"/sys/class/i2c-dev/i2c-2/device/2-001e/iio:device2/in_magn_z_scale";



/* accelerometer paths*/
char const *const PATH_MODE_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0019/iio:device0/mode";
char const *const PATH_RANGE_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0019/iio:device0/in_accel_scale";
char const *const PATH_DATA_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0019/iio:device0/accel_raw";
char const *const PATH_ID_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0019/iio:device0/id";
char const *const PATH_ID_DLHC_ACC =
		"/sys/class/i2c-dev/i2c-2/device/2-0019/iio:device0/id";

/* gyroscope paths dummy declarations, as devices are absent*/
char const *const PATH_MODE_GYR = "";
char const *const PATH_RANGE_GYR = "";
char const *const PATH_RATE_GYR = "";
char const *const PATH_DATA_GYR = "";
char const *const PATH_SENSITIVITY_GYR ="";
/* gyroscope paths */
char const *const PATH_MODE_GYR_L3GD20 = "";
char const *const PATH_RANGE_GYR_L3GD20 = "";
char const *const PATH_RATE_GYR_L3GD20 = "";
char const *const PATH_DATA_GYR_L3GD20 = "";
char const *const PATH_SENSITIVITY_GYR_L3GD20 ="";

/* ambient light paths */
char const *const PATH_POWER_LUX = NULL;
char const *const PATH_DATA_LUX = NULL;

/* proximity paths*/
char const *const PATH_POWER_PROX = NULL;
char const *const PATH_DATA_PROX = NULL;
char const *const PATH_INTR_PROX = NULL;

/* pressure paths*/
char const *const PATH_MODE_PRS = NULL;
char const *const PATH_DATA_PRS = NULL;

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
}Sensor_data;

/* To store all Sensors data*/
typedef struct {
   sensors_event_t sensor_data[8];
   int length;
} Sensor_messagequeue;

/* sensor API integration */

static const struct sensor_t sSensorList[] = {
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
};

static int acc_id;
