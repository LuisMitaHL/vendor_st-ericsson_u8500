/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * DESCRIPTION:
 * Checks for battery missing and battery voltage during
 * boot time. Any other boot time checks can be added here.
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/reboot.h>
#include <dirent.h>
#include <fcntl.h>

#define ERROR(...) do{fprintf(stderr, __VA_ARGS__);fflush(stderr);}while(0)
#define INFO(...)  do{fprintf(stderr, __VA_ARGS__);fflush(stderr);}while(0)
#ifdef DEBUG_LOGGING
#define DEBUG(...) do{fprintf(stderr, __VA_ARGS__);fflush(stderr);}while(0)
#else
#define DEBUG(...)
#endif

#define FALSE (0)
#define TRUE (!FALSE)

#define POWER_SUPPLY_SYS_PATH "/sys/class/power_supply"

/*
 * Decision to shutdown system is taken based
 * on this flag when battery is missing and
 * battery voltage is below configured value
 */
const bool external_power_boot		= TRUE;

/* Battery voltage in microvolts */
const int battery_voltage_at_boot	= 3250000;

struct battery_sysfs_paths{
	char *battery_present;
	char *battery_voltage;
	char *battery_technology;
};

/*
 * read_int_from_file
 * reads an integer from file, returning TRUE if successful
 */
static int read_int_from_file(const char *file, int *i)
{
	FILE *f = fopen(file, "r");
	int count;

	if (!f) {
		ERROR("Could not open '%s': %s\n", file, strerror(errno));
		return FALSE;
	}

	do {
		count = fscanf(f, "%i", i);
	} while (count != 1 && errno == -EINTR);

#ifdef DEBUG_LOGGING
	if (count != 1)
		DEBUG("Could not read from file '%s': %s\n", file, strerror(errno));
#endif

	fclose(f);

	return count == 1;
}

/*
 * read_from_file
 * reads data from file, returning number of bytes read
 */
static int read_from_file(const char *path, char *buf, size_t size)
{
	int fd;
	size_t count;

	if (!path)
		return -1;

	fd = open(path, O_RDONLY, 0);
	if (fd == -1) {
		ERROR("Could not open '%s'", path);
		return -1;
	}
	count = read(fd, buf, size);
	if (count > 0) {
		count = (count < size) ? count : size - 1;
		while (count > 0 && buf[count-1] == '\n') count--;
			buf[count] = '\0';
	} else {
		buf[0] = '\0';
	}

	close(fd);
	return count;
}

/*
 * find_battery_paths
 * finds battery sysfs paths at runtime, returning TRUE on success
 */
static int find_battery_paths(struct battery_sysfs_paths *paths)
{
	char    path[PATH_MAX], buf[20], *name;
	struct dirent *entry;
	int len;

	DIR *dir = opendir(POWER_SUPPLY_SYS_PATH);
	if (dir == NULL) {
		ERROR("Could not open %s\n", POWER_SUPPLY_SYS_PATH);
		return FALSE;
	}

	while ((entry = readdir(dir))) {
		name = entry->d_name;

		/* ignore "." and ".." */
		if (name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0)))
			continue;

		snprintf(path, sizeof(path), "%s/%s/type", POWER_SUPPLY_SYS_PATH, name);
		bzero(buf, sizeof(buf));
		len = read_from_file(path, buf, sizeof(buf));

		if (len > 0) {
			if (buf[len-1] == '\n')
			buf[len - 1] = 0;

			if (strcmp(buf, "Battery") == 0) {
				snprintf(path, sizeof(path), "%s/%s/present", POWER_SUPPLY_SYS_PATH, name);
				if (access(path, R_OK) == 0)
					paths->battery_present = strdup(path);

				snprintf(path, sizeof(path), "%s/%s/voltage_now", POWER_SUPPLY_SYS_PATH, name);
				if (access(path, R_OK) == 0)
					paths->battery_voltage = strdup(path);

				snprintf(path, sizeof(path), "%s/%s/technology", POWER_SUPPLY_SYS_PATH, name);
				if (access(path, R_OK) == 0)
					paths->battery_technology = strdup(path);
			}
		}
	}
	closedir(dir);

	if (!paths->battery_present)
		ERROR("Battery present sysfs not found");
	if (!paths->battery_voltage)
		ERROR("Battery voltage sysfs not found");
	if (!paths->battery_technology)
		ERROR("Battery technology sysfs not found");
	if (!paths->battery_present ||
		!paths->battery_voltage ||
		!paths->battery_technology)
		return FALSE;
	else
		return TRUE;
}

/*
 * find_battery_missing
 * finds if battery is missing in device, returning TRUE if missing
 */
int find_battery_missing(const char *path)
{
	char buf[30];
	int len = read_from_file(path, buf, sizeof(buf));
	if (len > 0) {
		if (strcmp(buf, "Unknown") == 0)
			return TRUE;
		else
			return FALSE;
	}
	ERROR("Failed to read battery technology");
	return TRUE;
}

int main(void)
{
	int bat_present, bat_voltage, bat_missing;
	struct battery_sysfs_paths paths = {0, 0, 0};

	freopen("/dev/kmsg", "a", stderr);
	if (!find_battery_paths(&paths)) {
		ERROR("Failed to find battery sysfs paths");
		exit(EXIT_FAILURE);
	}

	/*
	 * Check for battery missing and external
	 * power supply is applied
	*/
	if (!read_int_from_file(paths.battery_present, &bat_present)) {
		ERROR("Failed to read battery present sysfs file");
		exit(EXIT_FAILURE);
	}

	bat_missing = find_battery_missing(paths.battery_technology);
	if ((!bat_present || bat_missing) && !external_power_boot) {
		ERROR("Battery is missing. Shutting down system!");
		reboot(RB_POWER_OFF);
	}

	/*
	 * Check battery volatge against configured
	 * threshold voltage
	 */
	if (!read_int_from_file(paths.battery_voltage, &bat_voltage)) {
		ERROR("Failed to read battery voltage sysfs file");
		exit(EXIT_FAILURE);
	}
	if (bat_voltage < battery_voltage_at_boot) {
		ERROR("Battery voltage is less than threshold");
		reboot(RB_POWER_OFF);
	}
	fclose(stderr);
	return 0;
}
