/*
 * Thermal Service Manager
 *
 * Entry point for the Thermal Service
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "config.h"
#include "sensors.h"
#include "socket.h"
#include "actions.h"
#include "hwmon.h"
#include "log.h"

#define DEFAULT_CONFIG_FILE_PATH "/system/etc/thermal.conf"

static void usage(int argc, char **argv);

/*
 * Main entry point
 */
int main(int argc, char **argv) {
	int opt_v = 0;
	char *opt_c = NULL;
	char *opt_s = NULL;
	int ch;

	/* check we're running as root */
	if(getuid() != 0) {
		errno = EACCES;
		ERR("I must be run as root\n");
		return -1;
	}

	/* handle command line arguments */
	while ((ch = getopt(argc, argv, "hvc:s:")) != -1) {
		switch (ch) {
		case 'v': // enables verbose logging
			opt_v++;
			break;

		case 'c': // allows use to override location of config
			opt_c = strdup(optarg);
			break;

		case 's': // changes location of unix socket
			opt_s = strdup(optarg);
			break;

		case 'h': // help!
		default:
			usage(argc, argv);
		}
	}

	argc -= optind;
	argv += optind;

	/* set the logging level */
	if (opt_v == 0) {
		opt_v = LOG_LEVEL_DEFAULT;
	}

	log_level(opt_v);

	INF("thermal manager starting...\n");

	/* bring up "subsystems" */
	sensor_init();
	hwmon_init();
	actions_init();

	/* read and parse sensor config file */
	if (opt_c == NULL) {
		opt_c = strdup(DEFAULT_CONFIG_FILE_PATH);
		if (opt_c == NULL) {
			ERR("unable to allocate memory\n");
			return -1;
		}
	}

	INF("parsing thermal.conf...\n");
	if (parse_config(opt_c) < 0) {
		ERR("failed to parse config file\n");
		return -1;
	}

	/* discover HWMON sensors */
	INF("discovering platform HWMON sensors...\n");
	if (hwmon_find_sensors()) {
		ERR("failed to find platform HWMON sensors\n");
		return -1;
	}

	/* startup client socket */
	if (opt_s != NULL) {
		INF("overriding default socket path (%s)!\n", opt_s);
		socket_setunixsocket(opt_s);
	}

	INF("starting client communication socket...\n");
	if (socket_init()) {
		ERR("unable to start socket communication\n");
		return -1;
	}

	/* set sensor values & start monitoring */
	INF("configuring initial sensor value...\n");
	if (sensor_configure_all()) {
		ERR("Unable to configure HWMON sensors\n");
		return -1;
	}

	/* finally stop and wait for the server thread to
	 * shutdown (ie a catostrophic error has occured)
	 */
	socket_wait();

	INF("thermal manager going down!\n");

	free(opt_c);
	if (opt_s) {
		free(opt_s);
	}

	return 0;
}

/*
 * displays info on the command line args for the thermal
 * management service
 */
static void usage(int argc, char **argv) {
	fprintf(stderr, "%s [-h] [-v] [-c path] [-s path]\n", argv[0]);
	fprintf(stderr, "-h\tdisplays this help message\n");
	fprintf(stderr, "-v\tenables verbose logging (use -vv..) for extra levels\n");
	fprintf(stderr, "-c\toverride path to thermal.conf\n");
	fprintf(stderr, "-s\toverride path to unix socket\n");
	fprintf(stderr, "\n");
	exit(-1);
}

