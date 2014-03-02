/*
 * Copyright (C) 2011 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: Sebastian Rasmussen <sebastian.rasmussen@stericsson.com>
 */

#include "make_ext4fs.h"

#ifdef PROGRESS_FLAG_IN_CSPSA
#include <cspsa.h>
#endif //ifdef PROGRESS_FLAG_IN_CSPSA

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#define FACTORYRESET_PROPERTY "ro.factoryresetwithoutrecovery"

struct config
{
	char *filename;
	char *resetreason;
	bool forcereset;
	bool verbose;
	char **blockdevice;
	int blockdevices;
	char *outpath;
	FILE *out;
	FILE *err;
#ifdef PROGRESS_FLAG_IN_CSPSA
	char *cspsaname;
	unsigned long cspsaparam;
	unsigned int cspsaparambit;
#endif //ifdef PROGRESS_FLAG_IN_CSPSA
};

static void verbose(struct config *config, char *fmt, ...)
{
	va_list args;

	if (!config->verbose)
		return;

	va_start(args, fmt);
	vfprintf(config->out, fmt, args);
	fflush(config->out);
	va_end(args);
}

static int check_cspsa_flag(struct config *config)
{
#ifdef PROGRESS_FLAG_IN_CSPSA
	CSPSA_Handle_t handle;
	CSPSA_Result_t result;
	CSPSA_Size_t size;
	uint32_t value;

	if (!config->cspsaname || config->cspsaparam == 0)
		return 0;

	verbose(config, "Checking ongoing reset flag...\n");


	result = CSPSA_Open(config->cspsaname, &handle);
	if (result != T_CSPSA_RESULT_OK)
	{
		fprintf(config->err, "Can not open CSPSA %s, error code %d\n", config->cspsaname, result);
		goto error;
	}

	result = CSPSA_GetSizeOfValue(handle, config->cspsaparam, &size);
	if (result >= T_CSPSA_RESULT_ERRORS)
	{
		fprintf(config->err, "Can not retrieve size of parameter %lu, error code %d\n", config->cspsaparam, result);
		goto error;
	}

	if (size * 8ul < config->cspsaparambit)
	{
		fprintf(config->err, "Bit index %d out of range for parameter %lu of size %lu, error code %d\n", config->cspsaparambit, config->cspsaparam, (unsigned long) size, result);
		goto error;
	}

	result = CSPSA_ReadValue(handle, config->cspsaparam, size, (CSPSA_Data_t *) &value);
	if (result >= T_CSPSA_RESULT_ERRORS)
	{
		fprintf(config->err, "Can not read parameter %lu, error code %d\n", config->cspsaparam, result);
		goto error;
	}

	value &= 1 << config->cspsaparambit;
	value >>= config->cspsaparambit;

	result = CSPSA_Close(&handle);
	if (result != T_CSPSA_RESULT_OK)
	{
		fprintf(config->err, "Can not close CSPSA %s, error code %d\n", config->cspsaname, result);
		return -1;
	}

	if (value)
		verbose(config, "Parameter indicating ongoing factory reset!\n");
	else
		verbose(config, "Parameter not indicating any ongoing factory reset...\n");

	return value;

error:

	result = CSPSA_Close(&handle);
	if (result != T_CSPSA_RESULT_OK)
		fprintf(config->err, "Can not close CSPSA %s, error code %d\n", config->cspsaname, result);

	return -1;
#else // ifdef PROGRESS_FLAG_IN_CSPSA
	(void) config;
	return 0;
#endif // ifdef PROGRESS_FLAG_IN_CSPSA
}

static int set_cspsa_flag(struct config *config, int newvalue)
{
#ifdef PROGRESS_FLAG_IN_CSPSA
	CSPSA_Handle_t handle;
	CSPSA_Result_t result;
	CSPSA_Size_t size;
	uint32_t value;

	if (!config->cspsaname || config->cspsaparam == 0)
		return 0;

	if (newvalue)
		verbose(config, "Setting flag indicating ongoing factory reset.\n");
	else
		verbose(config, "Clearing flag indicating factory reset completed.\n");

	result = CSPSA_Open(config->cspsaname, &handle);
	if (result != T_CSPSA_RESULT_OK)
	{
		fprintf(config->err, "Can not open CSPSA %s, error code %d\n", config->cspsaname, result);
		goto error;
	}

	result = CSPSA_GetSizeOfValue(handle, config->cspsaparam, &size);
	if (result >= T_CSPSA_RESULT_ERRORS)
	{
		fprintf(config->err, "Can not retrieve size of parameter %lu, error code %d\n", config->cspsaparam, result);
		goto error;
	}

	if (size * 8 < config->cspsaparambit)
	{
		fprintf(config->err, "Can not bit index %d out of range for parameter %lu, error code %d\n", config->cspsaparambit, config->cspsaparam, result);
		goto error;
	}

	result = CSPSA_ReadValue(handle, config->cspsaparam, size, (CSPSA_Data_t *) &value);
	if (result >= T_CSPSA_RESULT_ERRORS)
	{
		fprintf(config->err, "Can not read parameter %lu, error code %d\n", config->cspsaparam, result);
		goto error;
	}

	if (newvalue)
		value |= 1 << config->cspsaparambit;
	else
		value &= ~(1 << config->cspsaparambit);

	result = CSPSA_WriteValue(handle, config->cspsaparam, size, (CSPSA_Data_t *) &value);
	if (result >= T_CSPSA_RESULT_ERRORS)
	{
		fprintf(config->err, "Can not write parameter %lu, error code %d\n", config->cspsaparam, result);
		goto error;
	}

	result = CSPSA_Close(&handle);
	if (result != T_CSPSA_RESULT_OK)
	{
		fprintf(config->err, "Can not close CSPSA %s, error code %d\n", config->cspsaname, result);
		return -1;
	}

	verbose(config, "Parameter update completed\n");

	return 0;

error:

	result = CSPSA_Close(&handle);
	if (result != T_CSPSA_RESULT_OK)
		fprintf(config->err, "Can not close CSPSA %s, error code %d\n", config->cspsaname, result);

	fprintf(config->err, "Can not set CSPSA flag indicating ongoing factory reset!\n");

	return -1;
#else // ifdef PROGRESS_FLAG_IN_CSPSA
	(void) config;
	(void) newvalue;
	return 0;
#endif // ifdef PROGRESS_FLAG_IN_CSPSA
}

static int check_reset_reason(struct config *config)
{
	char line[1024];
	FILE *f;

	verbose(config, "Reading reset reason from file '%s'...\n", config->filename);
	f = fopen(config->filename, "r");
	if (!f)
	{
		fprintf(config->err, "Cannot open '%s': %s\n", config->filename, strerror(errno));
		return -1;
	}

	if (!fgets(line, sizeof (line), f))
	{
		if (ferror(f))
			fprintf(config->err, "Cannot read '%s': %s\n", config->filename, strerror(errno));
		else if (feof(f))
			fprintf(config->err, "Cannot read '%s': End of file encountered\n", config->filename);
		if (fclose(f))
			fprintf(config->err, "Cannot close '%s': %s\n", config->filename, strerror(errno));

		return -1;
	}

	if (fclose(f))
		fprintf(config->err, "Cannot close '%s': %s\n", config->filename, strerror(errno));

	while (strrchr(line, '\n'))
		*(strrchr(line, '\n')) = '\0';

	verbose(config, "Factory reset reason indicated by: '%s'\n", config->resetreason);
	verbose(config, "Reset reason in file: '%s'\n", line);

	if (!strcmp(line, config->resetreason))
		return 1;
	else
		return 0;
}

static int factory_reset(struct config *config)
{
	int i, ret;

	printf("Performing factory reset...");

	if (set_cspsa_flag(config, 1) < 0)
		return -1;

	for (i = 0; i < config->blockdevices; i++)
	{
		verbose(config, "Formatting blockdevice '%s'\n", config->blockdevice[i]);
		reset_ext4fs_info();
		ret = make_ext4fs(config->blockdevice[i], get_file_size((int)config->blockdevice[i]),NULL,NULL);
		if (ret != 0)
		{
			fprintf(config->err, "Can not format blockdevice '%s'!\n", config->blockdevice[i]);
			break;
		}
	}

	if (set_cspsa_flag(config, 0) < 0)
		return -1;

	printf("Factory reset completed!\n");

	return 0;
}

static int check_factory_reset(struct config *config)
{
	int result = 0;
	bool resettriggered = false;
	bool resetongoing = false;

	verbose(config, "Checking factory reset status...\n");

	result = check_reset_reason(config);
	resettriggered = result == 1;
	if (!result)
	{
		result = check_cspsa_flag(config);
		resetongoing = result == 1;
	}
	if (result < 0)
	{
		fprintf(config->err, "Can not check factory reset reasons!\n");
		return -1;
	}

	if (config->forcereset)
		verbose(config, "Factory reset is forced!\n");
	else if (resettriggered)
		verbose(config, "A new factory reset was triggered!\n");
	else if (resetongoing)
		verbose(config, "Factory reset was already ongoing!\n");
	else
		return 0;

	return 1;
}

static void usage(struct config *config)
{
	fprintf(config->err, "Usage: factoryreset [options] [devices...]\n");
	fprintf(config->err, "\n");
	fprintf(config->err, "Where devices is a list block device path to format upon factory reset.\n");
	fprintf(config->err, "\n");
	fprintf(config->err, "Options:\n");
	fprintf(config->err, "\t-f file\t\t\tFile indicating reset reason.\n");
	fprintf(config->err, "\t-r string\t\tString read from reset reason file indicating factory reset.\n");
	fprintf(config->err, "\t-F\t\t\tForce factory reset regardless of reset reason.\n");
	fprintf(config->err, "\t-v\t\t\tVerbose output.\n");
	fprintf(config->err, "\t-d path\t\t\tVerbose output to given file.\n");
#ifdef PROGRESS_FLAG_IN_CSPSA
	fprintf(config->err, "\t-c name\t\t\tCSPSA name to look for a parameter in.\n");
	fprintf(config->err, "\t-p parameter:bit\tCSPSA parameter id and bit index storing indicating ongoing factory reset.\n");
	fprintf(config->err, "\n");
	fprintf(config->err, "Example:\n");
	fprintf(config->err, "\tfactoryreset -f /sys/socinfo/reset_reason -r 0x4242 -c CSPSA0 -p 0xfffffffd:1 /dev/block/mmcblk0p4\n");
#else // ifdef PROGRESS_FLAG_IN_CSPSA
	fprintf(config->err, "\n");
	fprintf(config->err, "Example:\n");
	fprintf(config->err, "\tfactoryreset -f /sys/socinfo/reset_reason -r 0x4242 /dev/block/mmcblk0p4\n");
#endif // ifdef PROGRESS_FLAG_IN_CSPSA
	exit(EXIT_FAILURE);
}

static int parse(int argc, char **argv, struct config *config)
{
	int c;
	char *options;

#ifdef PROGRESS_FLAG_IN_CSPSA
	options = ":c:p:f:r:Fvd:h";
#else // ifdef PROGRESS_FLAG_IN_CSPSA
	options = ":f:r:Fvd:h";
#endif // ifdef PROGRESS_FLAG_IN_CSPSA

	while ((c = getopt(argc, argv, options)) != -1)
		switch (c)
		{
#ifdef PROGRESS_FLAG_IN_CSPSA
			case 'c':
				free(config->cspsaname);
				config->cspsaname = strdup(optarg);
				if (!config->cspsaname)
					goto out_of_memory;
				break;

			case 'p':
				{
					char *colon, *end;
					if (strcmp(optarg, "0x"))
						config->cspsaparam = strtoul(optarg, &colon, 16);
					else
						config->cspsaparam = strtoul(optarg, &colon, 10);
					if (config->cspsaparam == ULONG_MAX)
					{
						fprintf(config->err, "Parameter key in option argument '%s' for option '%c' out of range.\n", optarg, optopt);
						return -1;
					}
					if (!colon)
					{
						fprintf(config->err, "Option argument '%s' for option '%c' missing bit index.\n", optarg, optopt);
						return -1;
					}
					colon++;
					config->cspsaparambit = strtoul(colon, &end, 10);
					if (config->cspsaparambit == ULONG_MAX)
					{
						fprintf(config->err, "Bit index in option argument '%s' for option '%c' out of range.\n", optarg, optopt);
						return -1;
					}
					if (*end != '\0')
					{
						fprintf(config->err, "Characters '%s' incorrectly trailing end of option argument '%s' for option '%c'.\n", end, optarg, optopt);
						return -1;
					}
				}
				break;
#endif // ifdef PROGRESS_FLAG_IN_CSPSA

			case 'f':
				free(config->filename);
				config->filename = strdup(optarg);
				if (!config->filename)
					goto out_of_memory;
				break;

			case 'r':
				free(config->resetreason);
				config->resetreason = strdup(optarg);
				if (!config->resetreason)
					goto out_of_memory;
				break;

			case 'F':
				config->forcereset = true;
				break;

			case 'v':
				config->verbose = true;
				break;

			case 'd':
				config->verbose = true;
				free(config->outpath);
				config->outpath = strdup(optarg);
				if (!config->outpath)
					goto out_of_memory;
				break;

			case 'h':
				usage(config);
				break;

			case ':':
				fprintf(config->err, "Option '%c' is missing its argument.\n", optopt);
				return -1;

			case '?':
				fprintf(config->err, "Unknown option '%c'.\n", optopt);
				return -1;

			default:
				fprintf(config->err, "Unhandled option '%c'.\n", optopt);
				return -1;
		}

	if (optind < argc)
	{
		config->blockdevice = &argv[optind];
		config->blockdevices = argc - optind;
	}

	return 0;

out_of_memory:
	fprintf(config->err, "Out of memory!");
	abort();
}

int __system_properties_init(void);

int main(int argc, char **argv)
{
	int status = EXIT_SUCCESS;
	int result;
	struct config config;

	memset(&config, 0x00, sizeof config);
	config.out = stdout;
	config.err = stderr;

	if (parse(argc, argv, &config) < 0)
		goto exit_failure;

	if (config.outpath)
	{
		config.out = fopen(config.outpath, "a");
		if (!config.out)
		{
			fprintf(config.err, "Cannot open desired output stream '%s': %s\n", config.outpath, strerror(errno));
			config.out = stdout;
		}
		config.err = config.out;
	}

	result = check_factory_reset(&config);
	if (result < 0)
		goto exit_failure;

	if (!result)
	{
		verbose(&config, "No cause for a factory reset.\n");
		goto cleanup;
	}

	if (factory_reset(&config) < 0)
	{
		fprintf(config.err, "Factory reset failed!\n");
		goto exit_failure;
	}

	goto cleanup;

exit_failure:
	status = EXIT_FAILURE;

cleanup:
	if (config.out)
		if (fclose(config.out))
			fprintf(config.err, "Can not close output stream!\n");
	if (config.out != config.err)
		if (fclose(config.err))
			fprintf(config.err, "Can not close error output stream!\n");
	free(config.outpath);
	free(config.filename);
	free(config.resetreason);
#ifdef PROGRESS_FLAG_IN_CSPSA
	free(config.cspsaname);
#endif // ifdef PROGRESS_FLAG_IN_CSPSA

	exit(status);
}
