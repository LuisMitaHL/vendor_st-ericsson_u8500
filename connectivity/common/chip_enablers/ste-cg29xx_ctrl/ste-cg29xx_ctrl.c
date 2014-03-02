
/**
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#ifdef ANDROID
#include <private/android_filesystem_config.h>
#endif
#include <sys/poll.h>
#include <sys/ioctl.h>

#include "cspsa.h"

#define CSPSA_BDA_KEY		0x10100
#define CSPSA_AREA_NAME		"CSPSA0"

#ifndef N_HCI
#define N_HCI			15
#endif

#ifndef N_CG2900_HCI
#define N_CG2900_HCI		23
#endif

#define HCI_UART_STE_MFD		5
#define HCI_UART_STE_STAGING	6

/* Ioctls */
#define HCIUARTSETPROTO		_IOW('U', 200, int)


#define DEFAULT_UART_BAUDRATE	115200
#define INIT_TIMEOUT		25

#define NOT_CANCELLED		0
#define CANCELLED		1

#define BDA_SIZE		6
#define COMPANY_SIZE	3

#ifdef ANDROID
#define BDA_TEST_FILE		"/data/misc/bluetooth/cg2900-test-bd_address.conf"
#define BDA_RAND_FILE		"/data/misc/bluetooth/cg2900-rand-bd_address.conf"
#else
#define BDA_TEST_FILE		"/etc/cg2900-test-bd_address.conf"
#define BDA_RAND_FILE		"/etc/cg2900-rand-bd_address.conf"
#endif

#define BDA_APPLY		"/sys/module/cg2900_core/parameters/bd_address"

static int use_debug;

#define DBG_ERR(fmt, arg...)	fprintf(stderr, "ste-cg29xx_ctrl %s: " fmt "(%d, %s)\n", __func__ , ## arg, errno, strerror(errno))
#define DBG_INFO(fmt, arg...)	if (use_debug) { printf("ste-cg29xx_ctrl %s: " fmt "\n" , __func__ , ## arg); }

/**
 * @brief
 * Default BD address in the CSPSA. Returned when no real address exists.
 */
static const uint8_t def_bdaddr[] = {0x00, 0x80, 0xE1, 0x00, 0x00, 0x00};

/**
 * @brief
 * Used to set when IO operation (such as UART open) has been canceled.
 */
static volatile sig_atomic_t io_canceled = NOT_CANCELLED;

/**
 * @brief
 * Called when alarm is raised due to expired timer.
 *
 * @param [in] sig	Signal ID (not used).
 */
static void sig_alarm(int sig)
{
	DBG_ERR("Initialization timed out.");
	exit(1);
}

/**
 * @brief
 * Called when IO has hung up (closed).
 *
 * @param [in] sig	Signal ID (not used).
 */
static void sig_hangup(int sig)
{
}

/**
 * @brief
 * Called when IO has terminated (process KILL signal received).
 *
 * @param [in] sig	Signal ID (not used).
 */
static void sig_terminated(int sig)
{
	io_canceled = CANCELLED;
}

/**
 * @brief
 * Convert integer value to UART define.
 *
 * @param [in] speed	UART baudrate (integer).
 * @return UART baudrate (define).
 */
static int convert_uart_speed(int speed)
{
	switch (speed) {
	case 9600:		return B9600;
	case 19200:		return B19200;
	case 38400:		return B38400;
	case 57600:		return B57600;
	case 115200:	return B115200;
	case 230400:	return B230400;
	case 460800:	return B460800;
	case 500000:	return B500000;
	case 576000:	return B576000;
	case 921600:	return B921600;
	case 1000000:	return B1000000;
	case 1152000:	return B1152000;
	case 1500000:	return B1500000;
	case 2000000:	return B2000000;
#ifdef B2500000
	case 2500000:	return B2500000;
#endif
#ifdef B3000000
	case 3000000:	return B3000000;
#endif
#ifdef B3500000
	case 3500000:	return B3500000;
#endif
#ifdef B4000000
	case 4000000:	return B4000000;
#endif
	default:
		errno = EINVAL;
		DBG_ERR("%d not a valid speed. Setting default speed %d",
				speed, DEFAULT_UART_BAUDRATE);
		return DEFAULT_UART_BAUDRATE;
	}
}

/**
 * @brief
 * Initialize UART driver.
 *
 * @param [in] name		Name of UART device.
 * @param [in] speed	Baudrate in integer format.
 * @param [in] protocol_id	Protocol ID.
 * @param [in] ldisc	TTY line discipline.
 * @return File descriptor, -1 upon error.
 */
static int init_transport_uart(char *name, int speed, int protocol_id, int ldisc)
{
	struct termios ti;
	int fd_uart;
	int converted_speed = convert_uart_speed(speed);

	DBG_INFO("Opening UART %s at %d BAUD", name, speed);

	/* Open the UART... */
	fd_uart = open(name, O_RDWR | O_NOCTTY);
	if (fd_uart < 0) {
		DBG_ERR("Opening UART %s failed", name);
		goto finished;
	}

	/* ...and flush all the data if there is something in the FIFOs. */
	tcflush(fd_uart, TCIOFLUSH);

	if (tcgetattr(fd_uart, &ti) < 0) {
		DBG_ERR("Failed to get settings from UART %s", name);
		goto handle_err;
	}

	/* Set TERMIOS to default settings */
	cfmakeraw(&ti);

	/* Set the TERMIOS flags */
	ti.c_cflag |= (CLOCAL | CRTSCTS);

	/* And store the UART settings */
	if (tcsetattr(fd_uart, TCSANOW, &ti) < 0) {
		DBG_ERR("Failed to set settings to UART %s", name);
		goto handle_err;
	}

	/* Set initial baudrate */
	cfsetospeed(&ti, converted_speed);
	cfsetispeed(&ti, converted_speed);
	if (tcsetattr(fd_uart, TCSANOW, &ti) < 0) {
		DBG_ERR("Failed to set baud rate for UART %s", name);
		goto handle_err;
	}

	/* Flush UART again since changing baudrate might have created false characters */
	tcflush(fd_uart, TCIOFLUSH);

	/* Set TTY to the input line discipline */
	if (ioctl(fd_uart, TIOCSETD, &ldisc) < 0) {
		DBG_ERR("Failed to set line discipline on UART %s to %d", name, ldisc);
		goto handle_err;
	}
	/*
	 * Set protocol ID to be used.
	 */
	if (ioctl(fd_uart, HCIUARTSETPROTO, protocol_id) < 0) {
		DBG_ERR("Failed to set protocol %d.", protocol_id);
		goto handle_err;
	}

	DBG_INFO("UART %s is opened", name);

	goto finished;

handle_err:
	close(fd_uart);
	fd_uart = -1;
finished:
	return fd_uart;
}

/**
 * @brief
 * Reads BD address from given conf file.
 * @par Description
 * This function reads  bd address from the given conf file.
 *
 * @param [in] path	Absolute path to configuration file.
 * @param [out] bda	Pointer to memory where read address is stored.
 * @return 0 on success; negative number on error.
 */
static int read_bda_from_file(const char *path, unsigned char *bda)
{
	FILE *fd = NULL;
	int ret = 0;

	if (!path || !bda)
		return -1;

	fd = fopen(path, "r");

	if (!fd) {
		DBG_INFO("Can't open %s (errno %d %s).",
				path, errno, strerror(errno));
		return -1;
	}

	ret = fscanf(fd, "%hhX%hhX%hhX%hhX%hhX%hhX", &(bda[0]), &(bda[1]),
				&(bda[2]), &(bda[3]), &(bda[4]), &(bda[5]));

	if (ret <= 0 || ret != BDA_SIZE) {
		DBG_INFO("No or wrong bd_addr in %s (errno %d).", path, errno);
		fclose(fd);
		return -1;
	}

	DBG_INFO("Read %d bytes %02X %02X %02X %02X %02X %02X", ret,
		bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
	fclose(fd);

	return 0;
}


/**
 * @brief
 * Sets BD address in CG2900 driver.
 * @par Description
 * This function sets given address in the CG2900 driver module parameter.
 *
 * @param [in] bda	Device address to be set.
 * @return 0 on success; negative number on error.
 */
static int set_bda(unsigned char *bda)
{
	FILE *fd = NULL;
	int ret = 0;

	if(!bda)
		return -EINVAL;

	fd = fopen(BDA_APPLY, "w");
	if (!fd) {
		DBG_ERR("Can't set bd_address in the chip (fopen errno %d).",
								errno);
		return -errno;
	}
	ret = fprintf(fd, "%d,%d,%d,%d,%d,%d\n", bda[5], bda[4], bda[3],
						bda[2], bda[1], bda[0]);
	if (ret <= 0) {
		DBG_ERR("Can't set bd_address in the chip (fprintf errno %d).",
							errno);
		fclose(fd);
		return -errno;
	}

	fclose(fd);
	return 0;
}

/**
 * @brief
 * Sets BD address in CG2900 driver.
 * @par Description
 * This function first tries to read out a BD address from file system. If it
 * fails, it tries to read it out from CSPSA. At the end sets th BD address
 * in the CG2900 driver parameter.
 */
static void setup_bda(void)
{
	int ret = 0;
	FILE *fd;
	unsigned char bda[BDA_SIZE];
	CSPSA_Result_t result;
	CSPSA_Handle_t handle;
	CSPSA_Size_t size;
	CSPSA_Data_t *data = NULL;
	CSPSA_Key_t key = CSPSA_BDA_KEY;

	DBG_INFO("Setting BD address");

	/* Try to read bd_addr from test conf file. */

	if(read_bda_from_file(BDA_TEST_FILE, bda) < 0)
		goto cspsa;

	ret = set_bda(bda);
	if (ret < 0)
		DBG_ERR("Can't setup bd_addr in the chip(ret %d)",ret);

	return;

cspsa:
	/*Try to read bd_addr from CSPSA if there isn't a conf file */

	result = CSPSA_Open(CSPSA_AREA_NAME, &handle);
	if (result != T_CSPSA_RESULT_OK) {
		DBG_ERR("Can't open CSPSA area (result 0x%X) ", result);
		goto rand_file;
	}

	result = CSPSA_GetSizeOfValue(handle, key, &size);
	if (result != T_CSPSA_RESULT_OK) {
		DBG_ERR("Can't get size of key (h %p key 0x%x result 0x%X).",
			handle, key, result);
		CSPSA_Close(&handle);
		goto rand_file;
	}

	data = (CSPSA_Data_t *) malloc(size);
	if (!data) {
		DBG_ERR("Can't malloc %d bytes.", size);
		CSPSA_Close(&handle);
		goto rand_file;
	}

	result = CSPSA_ReadValue(handle, key, size, data);
	if (result != T_CSPSA_RESULT_OK) {
		DBG_ERR("Can't read from CSPSA (h %p  key 0x%x size %d res 0x%X).",
			handle, key, size, result);
		free(data);
		CSPSA_Close(&handle);
		goto rand_file;
	}

	DBG_INFO("Read %d bytes %02X %02X %02X %02X %02X %02X", size,
			data[0], data[1], data[2],
			data[3], data[4], data[5]);

	if (!memcmp(data, def_bdaddr, sizeof(def_bdaddr))) {
		DBG_ERR("CSPSA contains only default BD address. Switch to random.");
		free(data);
		CSPSA_Close(&handle);
		goto rand_file;
	}

	ret = set_bda(data);
	if (ret < 0)
		DBG_ERR("Can't setup bd_addr in the chip(ret %d)",ret);

	free(data);
	CSPSA_Close(&handle);
	return;

rand_file:
	/* Try to read random bda previously used if reading CSPSA failed */

	if(read_bda_from_file(BDA_RAND_FILE, bda) < 0)
		goto rand;

	ret = set_bda(bda);
	if (ret < 0)
		DBG_ERR("Can't setup bd_addr in the chip(ret %d)",ret);

	return;

rand:
	/* Generate random bd address and store it.*/

	fd = fopen("/dev/urandom", "r");
	if (!fd) {
		DBG_ERR("Can't open urandom (%d, %s)", errno, strerror(errno));
		return;
	}

	/*
	 * Copy first 3 bytes of the default address (contains STM as manufacturer).
	 */
	memcpy(bda, def_bdaddr, COMPANY_SIZE);

	/* Then create the next 3 bytes as random */
	ret = fread(&bda[COMPANY_SIZE], sizeof(bda) - COMPANY_SIZE, 1, fd);
	fclose(fd);

	if (ret != 1) {
		DBG_ERR("Failed reading random bytes");
		return;
	}

	fd = fopen(BDA_RAND_FILE, "w");
	if (!fd) {
		DBG_ERR("Can't open bd_address as writable");
		return;
	}

	/* Store the new BD address in the file */
	ret = fprintf(fd, "0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
			bda[0], bda[1], bda[2],
			bda[3], bda[4], bda[5]);

	fclose(fd);

	if (ret > 0) {
		DBG_INFO("Created new address: %02X %02X %02X %02X %02X %02X",
				bda[0], bda[1], bda[2],
				bda[3], bda[4], bda[5]);
	}

	ret = set_bda(bda);
	if (ret < 0)
		DBG_ERR("Can't setup bd_addr in the chip(ret %d)",ret);

}

/**
 * @brief
 * Executes an external script.
 * @param [in] script	Script file path and name.
 */
static void run_script(char *script)
{
	if (!script)
		return;

	DBG_INFO("Executing %s", script);

	if (access(script, R_OK | X_OK)) {
		DBG_ERR("Init file not readable and executable");
		return;
	}

	if (fork())
		return;

	execv(script, NULL);

	exit(1);
}

/**
 * @brief
 * Prints usage of executable.
 */
static void usage(void)
{
	printf("\nste-cg29xx_ctrl - HCI driver initialization and control utility "
			"for ST-Ericsson CG2900 driver\n"
			"Usage:\n"
			"\tste-cg29xx_ctrl [-d] [-u] [-l line discipline] [-p protocol ID] "
			"[-b initial_baud_rate] [-i init_file] [<tty>]\n\n"
			"\t-d:\tEnable debug\n"
			"\t-u:\tUse UART as transport\n"
			"\t-l:\tTTY line discipline (default %d)\n"
			"\t-p:\tProtocol ID (default %d)\n"
			"\t-b:\tInitial UART baud rate to use (default %d)\n"
			"\t-i:\tShell script to execute at execution end\n"
			"\t<tty>:\tTTY (UART) to use\n\n"
			"\tNote that -u is not needed if -b is stated.\n"
			"\tNote that tty must be stated if -u or -b is stated.\n",
			N_HCI, HCI_UART_STE_MFD, DEFAULT_UART_BAUDRATE);
}

/**
 * @brief
 * Entry function for executable.
 *
 * @param [in] argc	Number of parameters.
 * @param [in] argv	In-parameters.
 * @return 0 if no error, error code otherwise.
 */
int main(int argc, char *argv[])
{
	int option;
	int fd_uart;
	int line_disc = N_HCI;
	int err;
	int arg_nbr;
	int init_timeout = INIT_TIMEOUT;
	int init_speed = DEFAULT_UART_BAUDRATE;
	int use_uart = 0;
	int protocol_id = HCI_UART_STE_MFD;
	char *init_file = NULL;

	struct sigaction signal_action;
	struct pollfd poll_fd;
	char name[PATH_MAX];

	/* Parse the input string */
	while ((option = getopt(argc, argv, "dub:l:p:i:")) != EOF) {
		switch(option) {

		case 'd':
			use_debug = 1;
			break;

		case 'u':
			use_uart = 1;
			break;

		case 'b':
			init_speed = atoi(optarg);
			use_uart = 1;
			break;
		case 'l':
			line_disc = atoi(optarg);
			break;
		case 'p':
			protocol_id = atoi(optarg);
			break;
		case 'i':
			init_file = strdup(optarg);
			break;
		default:
			usage();
			exit(1);
		}
	}

	arg_nbr = argc - optind;
	if (arg_nbr < 1 && use_uart) {
		usage();
		exit(1);
	}

	for (arg_nbr = 0; optind < argc; arg_nbr++, optind++) {
		char *option_value;

		option_value = argv[optind];

		switch(arg_nbr) {
		case 0:
			name[0] = 0;
			if (!strchr(option_value, '/'))
				strcpy(name, "/dev/");
			strcat(name, option_value);
			break;

		default:
			break;
		}
	}

	memset(&signal_action, 0, sizeof(signal_action));
	signal_action.sa_flags   = SA_NOCLDSTOP;
	signal_action.sa_handler = sig_alarm;
	sigaction(SIGALRM, &signal_action, NULL);

	/* 5 seconds should be enough for initialization */
	alarm(init_timeout);

	setup_bda();

	if (!use_uart) {
		DBG_INFO("Will not initialize UART");
		alarm(0);
		exit(0);
	}

	fd_uart = init_transport_uart(name, init_speed, protocol_id, line_disc);
	if (fd_uart < 0) {
		DBG_ERR("Can't initialize UART");
		exit(1);
	}

	sleep(2);

	run_script(init_file);

#ifdef ANDROID
	/* Now reduce ourselves to bluetooth user */
	if (setgid(AID_BLUETOOTH) != 0 || setuid(AID_BLUETOOTH) != 0)
		DBG_ERR("Can't switch to BT user");
#endif

	/* We are now finished. Shut off the timer. */
	alarm(0);

	memset(&signal_action, 0, sizeof(signal_action));
	signal_action.sa_flags   = SA_NOCLDSTOP;
	signal_action.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &signal_action, NULL);
	sigaction(SIGPIPE, &signal_action, NULL);

	signal_action.sa_handler = sig_terminated;
	sigaction(SIGTERM, &signal_action, NULL);
	sigaction(SIGINT,  &signal_action, NULL);

	signal_action.sa_handler = sig_hangup;
	sigaction(SIGHUP, &signal_action, NULL);

	poll_fd.fd = fd_uart;
	poll_fd.events = POLLERR | POLLHUP;

	while (io_canceled == NOT_CANCELLED) {
		poll_fd.revents = 0;
		/* Poll for changes every 5 seconds */
		err = poll(&poll_fd, 1, 5000);
		if (err < 0 && errno == EINTR)
			continue;
		if (err)
			break;
	}

	/* Restore TTY line discipline */
	line_disc = N_TTY;
	if (ioctl(fd_uart, TIOCSETD, &line_disc) < 0) {
		DBG_ERR("Can't restore line discipline");
		exit(1);
	}

	/* And close the UART */
	close(fd_uart);

	return 0;
}
