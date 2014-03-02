/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   sk-i2c
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <libi2c.h>

#define MAX_BUFF_SIZE		1024

char *programVersion = "0.01";

static inline int read_value(char *argv, unsigned char *reg_adress)
{
	char *endp;

	*reg_adress = strtol(argv, &endp, 0);

	if (!strcmp(argv, endp))
		return 0;

	return 1;

}

static void usage(void)
{
	printf("sk-i2c <i2c device> R <@ slave> <@ register> <count>\n");
	printf("sk-i2c <i2c device> W <@ slave> <@ register> <val> <val>\n");
	printf("sk-i2c v : programVersion\n");
}

static void sk_read(const int argc, char *argv[])
{
	unsigned char slave_address, i;
	unsigned short count;
	unsigned int reg_address;
	char buf[MAX_BUFF_SIZE] = { 0 };
	char *endp;
	unsigned char instance = atoi(argv[1]);

	/* Read command */
	if (argc == 6) {
		if (read_value(argv[3], &slave_address))
			printf("\nSlave Adress 0x%02x\n", slave_address);

		count = strtol(argv[5], &endp, 0);
		if (count > MAX_BUFF_SIZE)
			count = MAX_BUFF_SIZE;

		printf("count %d\n", count);

		reg_address = strtol(argv[4], &endp, 0);
		printf("\nReg Adress 0x%02x\n", reg_address);

		i2c_RDWR(instance, slave_address, 1, reg_address, count, buf);

		printf("Addr: %02x Data: ", reg_address & 0xff);
		if (count <= MAX_BUFF_SIZE) {
			for (i = 0; i < count; i++)
				printf("%02x ", buf[i] & 0xff);

			printf("\n");
		} else {
			printf("ERROR: count greater than %d\n", MAX_BUFF_SIZE);
		}
	} else {
		usage();
	}

}

static void sk_write(const int argc, char *argv[])
{
	unsigned char slave_address, reg_address, i;
	unsigned short count;
	char buf[MAX_BUFF_SIZE] = { 0 };
	unsigned char val;
	unsigned char instance = atoi(argv[1]);

	/* Write command */
	if (argc > 5) {
		if (read_value(argv[3], &slave_address))
			printf("\nSlave Adress 0x%02x\n", slave_address);

		if (read_value(argv[4], &reg_address))
			printf("Reg Adress 0x%02x\n", reg_address);

		count = 1;
		for (i = 5; i < argc; i++) {
			if (read_value(argv[i], &val)) {
				buf[count++] = val;
				printf("0x%02x ", val);
			}
		}
		buf[0] = reg_address;

		/* Register Adress */
		i2c_RDWR(instance, slave_address, 0, 0, count, buf);

		printf("\n");
	} else {
		usage();
	}

}

int main(int argc, char *argv[])
{

	if (argc < 2) {
		usage();
	} else {
		/* Version command */
		if (argv[1][0] == 'v') {
			printf("%s\n", programVersion);
			return 1;
		}
		if (argc > 2) {
			/* Cmd Read / Write / version */
			switch (argv[2][0]) {
			case 'r':
			case 'R':
				sk_read(argc, argv);
				break;

			case 'w':
			case 'W':
				sk_write(argc, argv);
				break;
			}
		}

	}

	return 1;

}
