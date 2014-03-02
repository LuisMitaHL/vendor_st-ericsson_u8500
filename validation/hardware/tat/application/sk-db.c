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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#include <libdb.h>

#define DB8500_REGISTER_REV     0x9001FFF4 /* DB8500 version register address */
#define DB8500_REGISTER_REV_V2  0x9001DBF4 /* DB8500 V2 version register address */
#define DB8500_REVISION_ED      0x00850001 /* DB8500_REGISTER_REV value for DB8500 ED version */
#define DB8500_REVISION_V1      0x008500A0 /* DB8500_REGISTER_REV value for DB8500 V1 version ( ! LSB NOT knwon for the moment ! ) */
#define DB8500_REVISION_V1_1    0x008500A1 /* DB8500_REGISTER_REV value for DB8500 V1.1 version */
#define DB8500_REVISION_V2      0x008500B0 /* DB8500_REGISTER_REV value for DB8500 V2 version */
#define DB8500_REVISION_V2_1    0x008500B1 /* DB8500_REGISTER_REV value for DB8500 V2_1 version */

enum {
	CHECK = 0,
	READ,
	WRITE,
};

/* Execution args modified by command line predicates */
struct {
	uint32_t range_min;
	uint32_t range_max;
	char *input_file;
	int show_mapped_io;
	int stress_io;
	int quit_on_error;
	int pause_on_test;
	int user_write;
} exec_args;

regDB_addr_t reg_address = 0;
regDB_val_t reg_val = 0xDEADBEEF;

#define ADDRESS_IN_RANGE(addr)      \
	((exec_args.range_min <= addr) && (exec_args.range_max >= addr))

/*
 * Global variables definition
 */
char *programVersion = "2.0";
int syslog_trace;

static inline void syslog_print(const char *pp_format, ...)
{
	if (!syslog_trace)
		return;

	va_list vl_args;
	va_start(vl_args, pp_format);
	vfprintf(stdout, pp_format, vl_args);
	va_end(vl_args);
}

/* Copy up to size chars in buf read from fp.
 * This function deals with Windows and Unix string terminators.
 * String terminator is removed from buf, if present.
 */
static uint32_t tat_bb_fgets(char *buf, uint32_t size, FILE * fp)
{
	uint32_t len = 0;
	if (fgets(buf, size, fp) != NULL) {
		len = strlen(buf);

		/* Windows file format */
		if (strncmp(buf + len - 2, "\r\n", 2) == 0) {
			/* Suppress last 2 chars */
			buf[len - 1] = '\0';
			buf[len - 2] = '\0';
		} else if (buf[len - 1] == '\n') {	/* Unix file format */
			/* Suppress last char */
			buf[len - 1] = '\0';
		} else if (len) {
			/* Error: no Unix or Windows string terminator! (programming error) */
			printf
			    ("no Unix or Windows string terminator found (Programming error)!\n");
		}

		/* str length should have change */
		len = strlen(buf);
	}

	return len;
}

static void print_usage(void)
{
	printf("tat-bb R source [-i file] [-r min--max]\n" "If -r or -i is set then source is the pathname of the file describing \
		   the baseband registers and the program reads read-write/read-only \
		   registers for the range and/or selection and prints their values.\n" "If neither -r nor -i option is set then the program reads the value \
		   of the register whose address is specified by source.\n"
	       "Addresses and values are in hexadecimal prefixed by \"0x\". " "Reading\
		   a write-only register is possible but may result in a freeze.\n\n");

	printf("tat-bb W source value\n" "Writes a single value into source,\
			which is the register address.\n\n");

	printf("tat-bb C source [-a] [-i file] [-p] [-q] [-r min--max] [-s]\n"
	       "Checks hwreg mapping compatibility with identified baseband.\
		   source is the pathname of the .ini file \
		   describing the baseband.\n\n");

	printf("Options:\n" "  -a     display mapped registers. by default, only non mapped \
		   registers are displayed.\n" "  -i     process only registers in this file. register addresses \
		   are separated by line feed.\n" "  -p     pause after each test then type 'q' to quit or another \
		   key to continue.\n" "  -q     quit on error\n" "  -r     process only registers within specified range. " "range is specified by a \"min--max\" where min and max \
		   are hexadecimal addresses. Either min or max can be missing.\n" "  -s     stress: read/write sample data to register\
		   when applicable.\n"
	       "\n");
}

static int parse_addr_range(char *arg)
{
	char *pEnd = NULL;

	if (strlen(arg) <= 5)
		goto error_range;

	/* arg should be a "A--B" string */
	char *ptr1 = arg;
	char *ptr2 = strstr(arg, "--");
	char tmp[50];

	if (ptr2 == NULL)
		goto error_range_arg;
	/* extract left param */
	int len = ptr2 - ptr1;
	if (len > 0) {
		strncpy(tmp, ptr1, len);
		tmp[len] = '\0';
		exec_args.range_min = strtoul(tmp, &pEnd, 0);
		if (*pEnd != '\0') {
			printf("Bad format for min range. \
					Hex must begin by 0x else value is decimal.\n");
			return -1;
		}
		syslog_print("exec_args.range_min: %u \n", exec_args.range_min);
	}

	/* extract right param */
	len = strlen(ptr1) - len - 2;
	if (len > 0) {
		strncpy(tmp, ptr2 + 2, len);
		tmp[len] = '\0';
		exec_args.range_max = strtoul(tmp, &pEnd, 0);
		if (*pEnd != '\0') {
			printf("Bad format for max range. \
					Hex must begin by 0x else value is decimal.\n");
			return -1;
		}
		syslog_print("exec_args.range_max: %u \n", exec_args.range_max);
	}

	return 1;

error_range_arg:
	/* not a valid range argument */
	printf("Specify range as 'a--b' where -- means \"to\", \
			a and b are hexadecimal and a xor b can be missing.\n");
	return -2;

error_range:
	printf("Address range not specified\n");
	return -3;
}

/* This function read or/and write data in/to
 * register at address <var>addr</addr>
 * if this register has read/write access.
 * @param access_str this string specifies access mode.
 * Can be "READ", "WRITE" or "RW".
 * @return 0 if the stress succeeds else a negative value is returned.
 */
static int tat_bb_stress_io(regDB_addr_t addr, char *access_str)
{
	int i;
	int ret = 0;
	regDB_val_t vl_value, vl_sample, vl_read;
	char *pEnd;
	syslog_print("Stressing register 0x%08x...\n", addr);

	/* R/W io stress is done by writting a sample long into the register
	 * and then reading from it.
	 * The returned value could not be the one written to it, especially
	 * if it is a bit-field.
	 * This should not be considered as a problem.
	 */
	if (strcmp(access_str, "RW") == 0) {
		/* Reading a register can cause a segment fault if the register is
		 * not readable due to hardware setting.
		 * In this case, the .ini file definition would not correspond to
		 * real register protection and the current process would abort.
		 * Running the reading process in a fork first helps detecting
		 * segfault without killing the current process!
		 */
		ret = db_u32_read(addr, &vl_value);
		if (ret == 0) {
			int run_default = 1;
			char buf[124];
			if (exec_args.user_write == 1) {
				while (run_default) {
					printf
					    ("value to write to register [default]?\n");
					fgets(buf, 124, stdin);
					if (strlen(buf) > 0) {
						vl_sample =
						    strtoul(buf, &pEnd, 0);
						if (pEnd == NULL)
							run_default = 0;
					}
				}

			}

			int is_io_written = 0;
			if (run_default == 0) {
				/* Write user data into register */
				ret = db_u32_write(addr, vl_sample);
				if (ret == 0) {
					/* Remind to restore to previous value at the end */
					is_io_written = 1;

					/* Read current value from register */
					ret = db_u32_read(addr, &vl_read);
				}
			} else {
				/* Write sample values into register */
				for (i = 0; (i < 3) && (ret == 0); i++) {
					switch (i) {
					case 0:
						vl_sample = 0x00000000;
						break;
					case 1:
						vl_sample = 0xFFFFFFFF;
						break;
					default:
						vl_sample = 0xFEEBDAED;
					}

					/* Write sample data in register */
					ret = db_u32_write(addr, vl_sample);
					if (ret == 0) {
						/* Remind to restore to previous value at the end */
						is_io_written = 1;

						/* Read current value from register */
						ret =
						    db_u32_read(addr, &vl_read);
						/* If not equal to sample value then stop writting stress */
						if ((ret == 0)
						    && (vl_read != vl_sample))
							break;
					}
				}

				if (ret == 0) {
					if (i > 2)
						printf
						    ("W-R stress status: %d. All pattern fully written\n",
						     ret);
					else
						printf
						    ("W-R stress status: %d. Leave on pattern out=0x%08x, in=0x%08x\n",
						     ret, vl_sample, vl_read);
				}
			}

			if (is_io_written == 1 && ret == 0) {
				/* restore previous value */
				db_u32_write(addr, vl_value);
			}
		} else {
			syslog_print
			    ("Attempt to read register data has failed.\n");
		}
	}
	/* read value from RO io */
	else if (strcmp(access_str, "READ") == 0) {
		ret = db_u32_read(addr, &vl_read);
		printf("RO register read status : %d, value=0x%08x\n", ret,
		       vl_read);
	}
	/* Write sample data to io but can't verify that the register
	 * value has been overriden.
	 * Because the value in the register has changed after the function
	 * call, it can produce unexpected result. */
	else if (strcmp(access_str, "WRITE") == 0) {
		ret = db_u32_write(addr, 0xD08A16B3);
		printf("WO register write status : %d\n", ret);
	}
	/* Not supposed to happen */
	else {
		ret = -1;
	}

	return ret;
}

static int is_register_sel(regDB_addr_t addr)
{
	int ret = 0;

	/* select registers within range */
	if (ADDRESS_IN_RANGE(addr))
		ret = 1;

	/* if 'inputfile' predicate is set then select only registers within the file. */
	if ((ret > 0) && (strlen(exec_args.input_file) > 0)) {
		char addr_as_hex[11];
		snprintf(addr_as_hex, 11, "0x%08x", addr);
		char buf[1024];

		ret = 0;

		FILE *fp = fopen(exec_args.input_file, "r+t");
		if (fp) {
			do {
				long posfp = ftell(fp);
				fgets(buf, 1024, fp);

				if (strncasecmp(buf, addr_as_hex, 10) == 0
				    && posfp >= 0) {
					fseek(fp, posfp, SEEK_SET);
					fputc('#', fp);
					fflush(fp);
					ret = 1;
				}
			} while (!feof(fp) && (ret == 0));

			fclose(fp);
		} else {
			syslog_print("Fail to open input file %s",
				     exec_args.input_file);
			ret = -1;
		}
	}

	return ret;
}

/* Checks if HWREG ioremap complies with inifile.
   @param inifile   INI file pathname describing DB8500 registers
   If all registers are accessible then 0 is returned. */
int tat_bb_multi_cmd(char *inifile, int cmd)
{
	int ret = 0;
	FILE *vl_fp;
	char vl_buf[255];
	char vl_key[100];
	char vl_value[100];
	char vl_name[100];
	char vl_access[20];	/* "READ", "WRITE", "RW" or null string */
	uint32_t vl_regAddress;
	int32_t vl_checkRes = 0;
	uint32_t vl_lc;
	long vl_iomapIndex;
	int vl_nbEntries, vl_nbMissingEntries;
	regDB_val_t vl_reg_val = 0;

	printf("Inspected range: 0x%08x --> 0x%08x\n", exec_args.range_min,
	       exec_args.range_max);

	/* Checks access to ASIC version register */
	db_check_addr(DB8500_REGISTER_REV, &vl_iomapIndex);

	/* Open the ini file */
	vl_fp = fopen(inifile, "rt");
	if (!vl_fp)
		goto error_fopen;

	/* Skip ini file heading until having reach [SECTION_NAME] */
	vl_lc = 0;
	do {
		tat_bb_fgets(vl_buf, 255, vl_fp);
	} while (!feof(vl_fp) && strncmp(vl_buf, "[SECTION_NAME]", 14) != 0);

	if (feof(vl_fp)) {
		/* err: section name not found in file! */
		printf("Bad file format: SECTION_NAME not found.\n");
		goto error_empty_file;
	}

	/* current section lists DB8500 memory sections */
	/* move to next ini file section, which is the first record */
	do {
		tat_bb_fgets(vl_buf, 255, vl_fp);
	} while ((vl_buf[0] != '[') && !feof(vl_fp));

	vl_nbEntries = 0;
	vl_nbMissingEntries = 0;

	/* Current section should be a register entry */
	while (!feof(vl_fp)) {
		/* Read next register section in .INI file */
		vl_regAddress = 0xDEADBEEF;
		memset(vl_key, '\0', 100);
		memset(vl_name, '\0', 100);
		strncpy(vl_name, "<noname>", 9);
		memset(vl_access, '\0', 20);
		memset(vl_value, '\0', 100);
		do {
			tat_bb_fgets(vl_buf, 255, vl_fp);

			if ((vl_buf[0] != '[') && !feof(vl_fp)) {
				char *ptr = strtok(vl_buf, "=");
				if (strlen(ptr) < 100)
					strncpy(vl_key, ptr, strlen(ptr) + 1);
				else
					printf
					    ("tat_bb_multi_cmd ERROR to save vl_key strlen(ptr):%d\n",
					     strlen(ptr));

				ptr = strtok(NULL, "=");
				if (ptr != NULL) {
					if (strlen(ptr) < 100)
						strncpy(vl_value, ptr,
							strlen(ptr) + 1);
					else
						printf
						    ("tat_bb_multi_cmd ERROR to save vl_value strlen(ptr):%d\n",
						     strlen(ptr));
				}

				if (strcmp(vl_key, "REGISTER_NAME") == 0) {
					if (strlen(vl_value) < 100)
						strncpy(vl_name, vl_value,
							strlen(vl_value) + 1);
					else
						printf
						    ("tat_bb_multi_cmd ERROR to save vl_name strlen(vl_value):%d\n",
						     strlen(vl_value));

				} else if (strcmp(vl_key, "REGISTER_ADR") == 0) {
					vl_regAddress =
					    strtoul(vl_value, NULL, 16);
				} else if (strcmp(vl_key, "REGISTER_ACCES") ==
					   0) {
					if (strlen(vl_value) < 20)
						strncpy(vl_access, vl_value,
							strlen(vl_value) + 1);
					else
						printf
						    ("tat_bb_multi_cmd ERROR to save vl_access strlen(vl_value):%d\n",
						     strlen(vl_value));
				}
			}
			/* End of current register section */
		} while ((vl_buf[0] != '[') && !feof(vl_fp));
		ret = is_register_sel(vl_regAddress);
		if (ret <= 0)
			continue;

		if ((CHECK == cmd) && (exec_args.stress_io != 0))
			printf
			    ("register address 0x%08x, name='%s', access=%s\n",
			     vl_regAddress, vl_name, vl_access);

		vl_nbEntries++;

		if (READ == cmd) {
			if ((strcmp(vl_access, "READ") == 0)
			    || (strcmp(vl_access, "RW") == 0)) {
				vl_reg_val = 0xDEADBEEF;
				db_u32_read(vl_regAddress, &vl_reg_val);
				printf("%s at 0x%08x: 0x%08x\n", vl_name,
				       vl_regAddress, vl_reg_val);
			}
		} else if (CHECK == cmd) {
			/* Checks that register address is in ioremap range. */
			vl_checkRes =
			    db_check_addr(vl_regAddress, &vl_iomapIndex);
			if (vl_checkRes == 0) {
				if (vl_iomapIndex >= 0) {
					if (exec_args.show_mapped_io != 0)
						printf
						    ("0x%08X named '%s' with access '%s' is mapped in hwreg at index %ld.\n",
						     vl_regAddress, vl_name,
						     vl_access, vl_iomapIndex);

					/* Stress io if requested to do so */
					if (exec_args.stress_io != 0) {
						ret =
						    tat_bb_stress_io
						    (vl_regAddress, vl_access);
						if ((ret != 0)
						    && (exec_args.
							quit_on_error != 0))
							break;
					}
				} else {
					printf
					    ("0x%08X named '%s' with access '%s' is NOT mapped in hwreg.\n",
					     vl_regAddress, vl_name, vl_access);
					vl_nbMissingEntries++;
				}
			} else {
				printf
				    ("Checking %s at addr 0x%08x failed with code %d.\n",
				     vl_name, vl_regAddress, vl_checkRes);
				if (exec_args.quit_on_error != 0)
					break;
			}

			if (exec_args.pause_on_test != 0) {
				/* pause until keybord is hit */
				if (getchar() == 'q')
					break;
			}
		}
	}

	if (CHECK == cmd) {
		/* Print overall stats */
		printf("%d entries processed, %d not mapped in hwreg.\n",
		       vl_nbEntries, vl_nbMissingEntries);
	}

error_empty_file:

	fclose(vl_fp);

error_fopen:
	/* err opening ini file */
	printf("Fail to open file %s!\n", inifile);

	return ret;
}

static int sk_db_read(void)
{
	int ret = -1;
	if ((exec_args.range_min != 0) || (exec_args.range_max != 0xFFFFFFFF)
	    || exec_args.input_file != NULL) {

		/* multiple address read */
		syslog_print("Registers definition file=%s",
			     exec_args.input_file);
		tat_bb_multi_cmd(exec_args.input_file, READ);

	} else {

		/* Read  Register */
		ret = db_u32_read(reg_address, &reg_val);
		/* single address read */
		syslog_print("address=0x%08x ret=%d\n", reg_address, ret);
		printf("READ: reg_address=0x%08x --> val = 0x%08x\n",
		       reg_address, reg_val);

	}
	return ret;

}

static int sk_db_write(void)
{
	int ret = -1;
	syslog_print("WRITE");
	syslog_print("address=0x%x ", reg_address);
	syslog_print("value=0x%x\n", reg_val);

	/* Write register */
	ret = db_u32_write(reg_address, reg_val);
	printf("WRITE: Address=0x%08x Data=0x%08x\n", reg_address, reg_val);

	return ret;

}

/* Execute program in self mode.
 * Command line args tells what to do.
 * @param argc arg count. same as main()
 * @param argv args array. same as main()
 * @return 0 if the command line was successfully processed else an error code.
 */
int main(int argc, char *argv[])
{
	int c;
	int option_index = 0;
	unsigned char cmd = 0;

	static struct option long_options[] = {
		{"version", no_argument, 0, 'v'},
		{"debug", no_argument, 0, 'd'},
		{"help", no_argument, 0, 'h'},
		{"file", required_argument, 0, 'i'},
		{NULL, 0, NULL, 0}
	};

	exec_args.input_file = NULL;
	exec_args.show_mapped_io = 0;
	exec_args.range_min = 0;
	exec_args.range_max = 0xFFFFFFFF;
	exec_args.stress_io = 0;
	exec_args.quit_on_error = 0;
	exec_args.pause_on_test = 0;
	exec_args.user_write = 0;

	while ((c =
		getopt_long(argc, argv, "vdhi:pqr:sa", long_options,
			    &option_index)) != -1) {
		switch (c) {
		case 'v':
			printf("%s version %s\n", argv[0], programVersion);
			break;
		case 'd':
			syslog_trace = 1;
			break;
		case 'h':
			print_usage();
			break;
		case 'i':
			/* file */
			exec_args.input_file = malloc(strlen(optarg) + 1);
			exec_args.input_file = optarg;
			break;
		case 'p':
			/* pause */
			exec_args.pause_on_test = 1;
			break;
		case 'q':
			/* quit on error */
			exec_args.quit_on_error = 1;
			break;
		case 's':
			/* stress */
			exec_args.stress_io = 1;
			break;
		case 'r':
			/* -r min--max */
			parse_addr_range(optarg);
			break;
		case 'a':
			/* display mapped registers */
			exec_args.show_mapped_io = 1;
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
		} else if ((!strncmp("C", argv[optind], 1)) || (!strncmp("c", argv[optind], 1))) {
			cmd = CHECK;
			syslog_print("check\n");
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
	case CHECK:
		if (exec_args.input_file == NULL) {
			printf("Not enough arguments. See --help\n");
		} else {
			syslog_print("CHECK: inifile=%s", exec_args.input_file);
			tat_bb_multi_cmd(exec_args.input_file,
					 CHECK);
		}
		break;
	case READ:
		sk_db_read();
		break;
	case WRITE:
		sk_db_write();
		break;
	}

	return 0;

error_cmd:
	return -1;
}
