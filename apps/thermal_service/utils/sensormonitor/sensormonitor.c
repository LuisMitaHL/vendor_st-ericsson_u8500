/*
 * sensormonitor
 *
 * Discovers all the _temp sensors under /sys/class/hwmon and
 * reads there values every 2 seconds
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define ERR printf
#define DBG printf

typedef struct {
	char *label;
	char *path;
} hwmon_sensor_t;

#define HWMON_MAX_CHIPS (10)
#define HWMON_MAX_SENSORS (100)
#define HWMON_MAX_SENSOR_LABEL_LEN (32)
#define HWMON_ROOT "/sys/class/hwmon/"
#define VALUE_MAX (32)

static inline void remove_newline(char *str) {
	int i = strlen(str);

	for ( ; i > 0; i--) {
		if (str[i] == '\n') {
			str[i] = '\0';
			return;
		}
	}
}

void *sensor_thread(void *arg) {
	hwmon_sensor_t *s = (hwmon_sensor_t *) arg;
	char value[VALUE_MAX];
	FILE *file;

	while (1) {
		file = fopen(s->path, "r");
		if (file == NULL) {
			return NULL;
		}

		fgets(value, VALUE_MAX, file);
		remove_newline(value);

		printf("%s reads %s\n", s->label, value);

		sleep(2);
		fclose(file);
	}
	return NULL;
}

int hwmon_find_sensors(void) {
	struct stat s;
	char *path;
	int pathlen;
	int i;

	/* allocate memory for string "HWMON_ROOT/hwmon[ii]\0" */
	pathlen = strlen(HWMON_ROOT);
	pathlen += strlen("hwmon");
	pathlen += 3; // 2 digits and a \0 terminator

	path = (char *) malloc(sizeof(char) * pathlen);
	if (path == NULL) {
		ERR("unable to allocate memory\n");
		return -1;
	}

	/* check HWMON_ROOT/hwmon[ii] to see if it exists,
	 * if so we have a new bank of sensors we need to
	 * discover. If it fails there will be not be any
	 * i+1 nodes.
	 */
	for (i = 0; i < HWMON_MAX_CHIPS; i++) {
		snprintf(path, pathlen, "%shwmon%d", HWMON_ROOT, i);

		if (stat(path, &s) < 0) {
			goto allfound;
		}

		if (find_sensors(path) < 0) {
			free(path);
			return -1;
		}
	}

allfound:
	free(path);
	return 0;
}

int find_sensors(const char *path) {
	char *sensor;
	int sensorlen;
	int i;

	/* allocate memory for "path/device/temp[i]_label\0" */
	sensorlen = strlen(path);
	sensorlen += strlen("/device/temp_label");
	sensorlen += 3; // 2 digits and a \0 terminator

	sensor = (char *) malloc(sizeof(char) * sensorlen);
	if (sensor == NULL) {
		ERR("unable to allocate memory\n");
		return -1;
	}

	/* as all sensors will have a temp[i]_label sysfs
	 * file we will try and stat it and then attempt to
	 * discover it's over attributes
	 */
	for (i = 1; i < HWMON_MAX_SENSORS; i++) {
		hwmon_sensor_t *newsensor;
		pthread_t pid;

		char label[HWMON_MAX_SENSOR_LABEL_LEN];
		FILE *file;

		snprintf(sensor, sensorlen, "%s/device/temp%i_label", path, i);

		/* try to open & read the sensor label */
		file = fopen(sensor, "r");
		if (file == NULL) {
			goto allfound;
		}

		if (fgets(label, HWMON_MAX_SENSOR_LABEL_LEN, file) == NULL) {
			fclose(file);
			ERR("unable to read sensor label\n");
			continue;
		}
		fclose(file);

		remove_newline(label);

		newsensor = malloc(sizeof(hwmon_sensor_t));
		newsensor->label = strdup(label);
		snprintf(sensor, sensorlen, "%s/device/temp%i_input", path, i);
		newsensor->path = strdup(sensor);

		pthread_create(&pid, NULL, sensor_thread, (void *) newsensor);
	}

allfound:
	free(sensor);

	return 0;
}


int main(void) {
	hwmon_find_sensors();

	while (1) {
		sleep(5);
	}

	return 0;
}
