/*
 * Thermal Service Manager
 *
 * List of built in mitigation actions. Each action needs to
 * be independent.
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <sys/reboot.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "config.h"
#include "log.h"

/* battery charging control sysfs path */
#ifdef THERMAL_SERVICE_U8500
#define BATTERY_CHARGE_CONTROL "/sys/ab8500_chargalg/chargalg"
#else
#define BATTERY_CHARGE_CONTROL "/sys/abx500_chargalg/chargalg"
#endif

/* sysfs paths for detecting cpu freq opps */
#define CPUFREQ_AVAILABLE_NODE "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies"
#define CPUFREQ_MINFREQ_NODE "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq"
#define CPUFREQ_MAXFREQ_NODE "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
/* cpufreq opp maximum characters */
#define FREQ_MAX_LEN	(10)
/* cpufreq opps supported */
#define FREQ_MAX_OPPS	(10)
/* CPUFREQ_AVAILABLE_NODE maximum size in bytes */
#define FREQ_SYSFS_SIZE	(FREQ_MAX_LEN * FREQ_MAX_OPPS)

/* setting cpu0 also sets the same limits for cpu1 */
#define CPU_MAX_NODE "/sys/kernel/debug/prcmu/arm_max_freq"

/* defines the sysfs nodes to enable/disable the STE usecase governor */
#define UCG_CONTROL_NODE "/sys/devices/system/cpu/usecase/enable"
#define UCG_ENABLE_STRING "1"
#define UCG_DISABLE_STRING "0"

#define CPUFREQ_LEVEL_ONE	(0)
#define CPUFREQ_LEVEL_TWO	(1)
#define CPUFREQ_LEVEL_THREE	(2)
#define CPUFREQ_LEVEL_FOUR	(3)

static char cpufreq_opps[FREQ_MAX_OPPS][FREQ_MAX_LEN] = {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}};
static int cpufreq_opps_cnt = 0;

static void setcpufreq(char *target_freq);
static void disableucg(void);
static void enableucg(void);
static void parse_cpu_freq(char *freq_buf, int size);
static void validate_cpu_freq_opps();
static int compare_opps(const void *a, const void *b);
static void setfreq(int level);

/*
 * Read available cpu frequency opps from sysfs file
 * and sort them high to low. These cpufreq opps will
 * be used to set cpufreq performance levels.
 */
void actions_cpufreq_init(void) {
	int fd;
	char *freq_buf;

	INF("initializing action parameters...\n");

	fd = open(CPUFREQ_AVAILABLE_NODE, O_RDONLY);
	if (fd < 0) {
		ERR("unable to open %s\n", CPUFREQ_AVAILABLE_NODE);
		return;
	}

	freq_buf = (char *) malloc(FREQ_SYSFS_SIZE);
	if (freq_buf == NULL) {
		ERR("unable to allocate memory\n");
		goto out;
	}

	if (read(fd, freq_buf, FREQ_SYSFS_SIZE) == -1) {
		ERR("unable to read %s\n", CPUFREQ_AVAILABLE_NODE);
		goto out_buf;
	}

	parse_cpu_freq(freq_buf, FREQ_SYSFS_SIZE);

out_buf:
	free(freq_buf);
out:
	close(fd);
}

/*
 * Qsort comparator function
 */
static int compare_opps(const void *a, const void *b) {
	return (strtol((char*)b, NULL, 10) - strtol((char*)a, NULL, 10));
}

/*
 * Parse available cpufreq sysfs file contents. As a standard, opps
 * are delimited with space.
 */
static void parse_cpu_freq(char *freq_buf, int size) {
	char *token = strtok(freq_buf, " \n");

	while (token != NULL) {
		strncpy(cpufreq_opps[cpufreq_opps_cnt++], token, strlen(token));
		token = strtok(NULL, " \n");
	}

	/* sort freqency opps from high to low */
	qsort(&cpufreq_opps, cpufreq_opps_cnt, sizeof(char) * FREQ_MAX_LEN, compare_opps);
	validate_cpu_freq_opps();
}

/*
 * Function to validate parsed cpu frequencies. cpufreq_opps[0]
 * should map to CPUFREQ_MAXFREQ_NODE sysfs value and
 * cpufreq_opps[cpufreq_opps_cnt-1] should map to CPUFREQ_MINFREQ_NODE
 * sysfs value. Otherwise cpufreq opps parsing has gone wrong.
 */
static void validate_cpu_freq_opps() {
	int fd;
	char freq[FREQ_MAX_LEN] = {0};

	fd = open(CPUFREQ_MINFREQ_NODE, O_RDONLY);
	if (fd < 0) {
		ERR("unable to open %s\n", CPUFREQ_MINFREQ_NODE);
		return;
	}

	if (read(fd, freq, FREQ_MAX_LEN) == -1) {
		ERR("unable to read %s\n", CPUFREQ_MINFREQ_NODE);
		close(fd);
		return;
	}
	close(fd);

	if (strtol(cpufreq_opps[cpufreq_opps_cnt - 1], NULL, 10) !=
	    strtol(freq, NULL, 10)) {
		ERR("min freq doesn't match, error while parsing\n");
		return;
	}

	memset(freq, 0, FREQ_MAX_LEN);
	fd = open(CPUFREQ_MAXFREQ_NODE, O_RDONLY);
	if (fd < 0) {
		ERR("unable to open %s\n", CPUFREQ_MAXFREQ_NODE);
		return;
	}

	if (read(fd, freq, FREQ_MAX_LEN) == -1) {
		ERR("unable to read %s\n", CPUFREQ_MAXFREQ_NODE);
		close(fd);
		return;
	}
	close(fd);

	if (strtol(cpufreq_opps[0], NULL, 10) != strtol(freq, NULL, 10)) {
		ERR("max freq doesn't match, error while parsing\n");
		return;
	}
}

void action_shutdown(void) {
	int timeout;

	timeout = config_getshutdowntimeout();

	INF("forcing platform shut down in %d seconds...\n", timeout);
	sleep(timeout);

	sync();
	reboot(RB_POWER_OFF);
}

static void setfreq(int level) {
	if (level < cpufreq_opps_cnt) {
		setcpufreq(cpufreq_opps[level]);
	} else {
		ERR("setting invalid cpufreq opps\n");
	}
}

/* set's cpu at full and enables the use case governor
 * to scale as needed */
void action_cpuperflevel1(void) {
	setfreq(CPUFREQ_LEVEL_ONE);
	enableucg();
}

void action_cpuperflevel2(void) {
	disableucg();
	setfreq(CPUFREQ_LEVEL_TWO);
}

void action_cpuperflevel3(void) {
	disableucg();
	setfreq(CPUFREQ_LEVEL_THREE);
}

void action_cpuperflevel4(void) {
	disableucg();
	setfreq(CPUFREQ_LEVEL_FOUR);
}

void action_stopbatterycharge(void) {
	char disable_all_charge[] = "0\n";
	int fd;

	INF("disabling battery charging...\n");

	fd = open(BATTERY_CHARGE_CONTROL, O_WRONLY);
	if (fd < 0) {
		ERR("unable to open %s\n", BATTERY_CHARGE_CONTROL);
		return;
	}

	if (write(fd, disable_all_charge, strlen(disable_all_charge) + 1) == -1) {
		ERR("failed to write to battery controller\n");
	}

	close(fd);
}

void action_startbatterycharge(void) {
	/* values from kernel/drivers/power/ab8500_chargalg.c */
	const char enable_ac_charge[] = "1\n";
	const char enable_usb_charge[] = "2\n";
	int fd;

	INF("enabling battery charging...\n");

	fd = open(BATTERY_CHARGE_CONTROL, O_WRONLY);
	if (fd < 0) {
		ERR("unable to open %s\n", BATTERY_CHARGE_CONTROL);
		return;
	}

	if (write(fd, enable_ac_charge, strlen(enable_ac_charge) + 1) == -1) {
		ERR("failed to write to battery controller\n");
		close(fd);
		return;
	}

	if (write(fd, enable_usb_charge, strlen(enable_usb_charge) + 1) == -1) {
		ERR("failed to write to battery controller\n");
	}

	close(fd);
}

/*
 * sets cpu to specific OPP
 */
static void setcpufreq(char *target_freq) {
	int fd;
	char freq[FREQ_MAX_LEN] = {0};

	INF("setting cpu speeds: max=%s\n", target_freq);

	/* set max freq */
	fd = open(CPU_MAX_NODE, O_WRONLY);
	if (fd < 0) {
		ERR("unable to open %s\n", CPU_MAX_NODE);
		return;
	}
	if (write(fd, target_freq, strlen(target_freq) + 1) == -1) {
		ERR("failed to write to cpufreq max node\n");
	}
	close(fd);
}

/*
 * Renabled use case governor, putting board back into normal cpufreq
 * opperation
 */
static void enableucg(void) {
	int fd;

	fd = open(UCG_CONTROL_NODE, O_WRONLY);
	if (fd < 0) {
		ERR("unable to open %s\n", UCG_CONTROL_NODE);
		return;
	}

	if (write(fd, UCG_ENABLE_STRING, strlen(UCG_ENABLE_STRING) + 1) == -1) {
		ERR("failed to write enable use case governor\n");
	}
	close(fd);
}

/*
 * Disables the use case governor to prevent cpufreq being modified
 * while thermal actions are ongoing
 */
static void disableucg(void) {
	int fd;

	fd = open(UCG_CONTROL_NODE, O_WRONLY);
	if (fd < 0) {
		ERR("unable to open %s\n", UCG_CONTROL_NODE);
		return;
	}

	if (write(fd, UCG_DISABLE_STRING, strlen(UCG_DISABLE_STRING) + 1) == -1) {
		ERR("failed to write disable use case governor\n");
	}
	close(fd);
}
