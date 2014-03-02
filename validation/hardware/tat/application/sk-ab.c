/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   sk-db
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#include <libab.h>

int syslog_trace;

enum {
	CHECK = 0,
	READ,
	WRITE,
};

static inline void syslog_print(const char *pp_format, ...)
{
	if (!syslog_trace)
		return;

	va_list vl_args;
	va_start(vl_args, pp_format);
	vfprintf(stdout, pp_format, vl_args);
	va_end(vl_args);
}

static void usage(void)
{
	printf("sk-ab R <@ register>(16bits)\n");
	printf("sk-ab W <@ register>(16bits)  <val>(8bits)\n");
}

int main(int argc, char *argv[])
{
	uint16_t reg_address = 0;
	uint8_t reg_val = 0;
	int c, ret=0;
	int option_index = 0;
	unsigned char cmd = 0;

	static struct option long_options[] = {
		{"version", no_argument, 0, 'v'},
		{"debug", no_argument, 0, 'd'},
		{"help", no_argument, 0, 'h'},
		{"file", required_argument, 0, 'i'},
		{NULL, 0, NULL, 0}
	};


	while ((c = getopt_long(argc, argv, "dh", long_options, &option_index)) != -1) {
		switch (c) {
		case 'd':
			syslog_trace = 1;
			break;
		case 'h':
			usage();
			break;
		default:
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	/* parse option non preceded by '-' */
	if ((optind < argc) && ((argc - optind) <= 3)) {
		syslog_print("non-option ARGV-elements: %s\n", argv[optind]);
		/* choose command type */
		if ((!strncmp("W", argv[optind], 1)) || (!strncmp("w", argv[optind], 1))) {
			cmd = WRITE;
			syslog_print("write\n");
		} else if ((!strncmp("R", argv[optind], 1)) || (!strncmp("r", argv[optind], 1))) {
			cmd = READ;
			syslog_print("read\n");
		} else {
			goto error_cmd;
		}

		syslog_print("asked command: %d\n", cmd);

		if ((optind + 1) > argc)
			goto error_cmd;

		optind++;

		/* extract adress */
		reg_address = strtoul(argv[optind], NULL, 0);

		/* extract value in case of write */
		if (cmd == WRITE) {
			/* next arg */
			if ((optind + 1) > argc)
				goto error_cmd;

			optind++;

			reg_val = strtoul(argv[optind], NULL, 0);
		}


	} else {
		goto error_cmd;
	}

	switch (cmd) {
		break;
	case READ:
		ret=abxxxx_read(reg_address, &reg_val);
		if(ret >=0 )
		{
			printf("READ: reg_address=0x%08x --> val = 0x%08x\n",
			reg_address, reg_val);
		}
		else
		{
			printf("READ: reg_address=0x%08x failed\n", reg_address);
			return ret;
		}
		break;
	case WRITE:
		ret = abxxxx_write(reg_address, reg_val);
		if(ret >= 0)
		{
			printf("WRITE: val = 0x%08x --> reg_address=0x%08x\n",
			reg_val, reg_address);
		}
		else
		{
			printf("WRITE: val = 0x%08x --> reg_address=0x%08x failed\n",
			reg_val, reg_address);
			return ret;
		}
		break;
	}

	return 0;
error_cmd:
	return -1;
}
