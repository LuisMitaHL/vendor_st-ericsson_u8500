/* **************************************************************************
 *
 * cspsalist.c
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Martin Lundholm <martin.xa.lundholm@stericsson.com>
 *
 * DESCRIPTION:
 * Creates a CSPSA binary image by parsing a PFF file structure..
 *
 **************************************************************************** */

#include "cspsa_ll_file.h"
#include "cspsa_plugin.h"
#include "cspsa_core.h"
#include "cspsa.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>

/* ========================================================================== */

#define PROGRAM "cspsalist"
#define VERSION 0.0.2

#define ERRORSTR "ERROR: "

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#define CSPSA_NAME    "WORKAREA"

/* ========================================================================== */

struct cmd_line_options
{
  char *outfile;
};

/* ========================================================================== */

static const char ERRORSTR_INTERNAL[] = ERRORSTR "Internal error at %d.\n";
static const char ERRORSTR_OUT_OF_MEMORY[] = ERRORSTR "Out of memory at %d.\n";

/* ========================================================================== */

static int init_cspsa(CSPSA_Config_t *cspsa_config, const char* const filename, struct stat statinfo)
{
  int result = -1;

  /* Register low level file plugin */
  CSPSA_LL_FILE_Init();

  cspsa_config->MemoryType = T_CSPSA_MEMORY_TYPE_FILE;

  cspsa_config->StartOffsetInBytes = 0;
  cspsa_config->BlockSize = 64 * 1024;
  cspsa_config->SectorSize = 4 * 1024;
  /* Allocate space for twice the block aligned file size. */
  cspsa_config->SizeInBytes = 2 * ((statinfo.st_size + cspsa_config->BlockSize) & ~(cspsa_config->BlockSize - 1));
  cspsa_config->Attributes = 0;
  strncpy(cspsa_config->Name, CSPSA_NAME, D_CSPSA_MAX_NAME_SIZE);
  /* This looks weired but is standard procedure. We're using Extra_p of CSPSA_Config_t
   * to store ll specific stuff. */
  {
    CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) cspsa_config->Extra_p;

    // NOTE: Since this is a command line tool with short life time
    // run on Linux system there is not no need to explictly free this memory.
    LinuxDevice_p = (CSPSA_LinuxDevice_t*) malloc(sizeof(CSPSA_LinuxDevice_t));
    if (LinuxDevice_p == NULL)
    {
      fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
      goto error_exit;
    }
    cspsa_config->SizeOfExtra = sizeof(*LinuxDevice_p);
    memset(LinuxDevice_p, 0, sizeof(CSPSA_LinuxDevice_t));
    cspsa_config->Extra_p = LinuxDevice_p;
    LinuxDevice_p->DeviceName_p = (char*)filename;
  }

  /* Register parameter area */
  if (CSPSA_AddParameterArea(cspsa_config) != T_CSPSA_RESULT_OK)
    goto error_exit;

  result = 0;

error_exit:
  return result;
}

/* ========================================================================== */

static void hexdump(FILE *out, unsigned char *data, uint32_t size)
{
  uint32_t i, j;

  for (i = 0; i < size; i += 16, data += 16)
  {
    fprintf(out, "\t");
    for (j = 0; j < 16 && i + j < size; j++)
      fprintf(out, " %02x", data[j]);
    fprintf(out, "\n");
  }
}

/* ========================================================================== */

static int cspsa_list_items(struct cmd_line_options *cmd_line_options, const CSPSA_Handle_t cspsa_handle, const char* const filename)
{
  int result = EXIT_FAILURE;
  FILE *out = NULL;
  uint32_t buffer_size = 64*1024;
  uint8_t *buffer = (uint8_t *) malloc(buffer_size);
  CSPSA_Result_t cspsa_result;
  CSPSA_Key_t key;
  CSPSA_Size_t size;

  /* open output file (can be stdout) */
  if (!strcmp(cmd_line_options->outfile, "-"))
    out = stdout;
  else
    out = fopen(cmd_line_options->outfile, "w");

  if (buffer == NULL)
  {
    fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
    goto error_exit;
  }

  {
    struct tm *time_p;
    time_t tm;
    char *strtime;

    tm = time(NULL);
    if (tm == (time_t)-1)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }
    time_p = localtime(&tm);
    if (time_p == NULL)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }
    strtime = asctime(time_p);
    if (strtime == NULL)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }
    fprintf(out, "// Created: %s", strtime);
    fprintf(out, "// " PROGRAM " " STRINGIFY(VERSION) " Copyright (C) 2010 ST-Ericsson AB\n");
    fprintf(out, "// Created from input file '%s'\n\n", filename);
    fprintf(out, "// Data format:\n");
    fprintf(out, "// <UNIT>(hex) <DATASIZE>(dec) <BLOCK>(hex) [<DATA>(hex)]\n");
  }

  cspsa_result = CSPSA_CORE_GetSizeOfFirstValue(cspsa_handle, &key, &size);
  if (cspsa_result != T_CSPSA_RESULT_OK)
  {
    fprintf(stderr, ERRORSTR "No CSPSA items found.\n");
    goto error_exit;
  }
  while (cspsa_result == T_CSPSA_RESULT_OK)
  {
    if (size > buffer_size)
    {
      buffer = (uint8_t *) realloc(buffer, buffer_size);
      if (buffer == NULL)
      {
        fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
        goto error_exit;
      }
      buffer_size = size;
    }
    cspsa_result = CSPSA_CORE_ReadValue(cspsa_handle, key, size, buffer);
    if (cspsa_result != T_CSPSA_RESULT_OK)
    {
      if (cspsa_result != T_CSPSA_RESULT_E_END_OF_DATA)
      {
        fprintf(stderr, ERRORSTR "Unexpected CSPSA result: %d.\n", cspsa_result);
        goto error_exit;
      }
      break;
    }
    if (key >= UINT16_MAX)
    {
      fprintf(out, "%08x %u 0000\n", key, size);
    }
    else
    {
      fprintf(out, "%04x %u 0000\n", key, size);
    }
    hexdump(out, buffer, size);
    cspsa_result = CSPSA_CORE_GetSizeOfNextValue(cspsa_handle, &key, &size);
  }

  result = EXIT_SUCCESS;

error_exit:

  free(buffer);

  /* Close output file */
  if (strcmp(cmd_line_options->outfile, "-") && fclose(out))
  {
    fprintf(stderr, ERRORSTR "Can not close output file");
  }

  return result;
}

/* ========================================================================== */

static int process(struct cmd_line_options *cmd_line_options, const char* const filename)
{
  int result = EXIT_FAILURE;
  struct stat statinfo;
  CSPSA_Handle_t cspsa_handle;
  CSPSA_Config_t cspsa_config;

  memset(&cspsa_config, 0, sizeof(cspsa_config));

  if (stat(filename, &statinfo) < 0)
  {
    fprintf(stderr, ERRORSTR "Can not find input file named '%s'.\n", filename);
    goto error_exit;
  }
  if (!S_ISREG(statinfo.st_mode))
  {
    fprintf(stderr, ERRORSTR "Input '%s' is not a regular file.\n", filename);
    goto error_exit;
  }
  if (statinfo.st_size == 0)
  {
    fprintf(stderr, ERRORSTR "Input file '%s' is empty.\n", filename);
    goto error_exit;
  }

  if (init_cspsa(&cspsa_config, filename, statinfo))
  {
    fprintf(stderr, ERRORSTR "Could not initialize CSPSA.\n");
    goto error_exit;
  }
  if (CSPSA_CORE_Open(CSPSA_NAME, &cspsa_handle) != T_CSPSA_RESULT_OK)
  {
    fprintf(stderr, ERRORSTR "CSPSA area could not be opened. Binary file not valid.\n");
    goto error_exit;
  }

  result = cspsa_list_items(cmd_line_options, cspsa_handle, filename);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR "Failed to list CSPSA items.\n");
    goto error_exit;
  }

  /*
   * We don't close the CSPSA because we do not want to write back any change. The
   * current implementation of 'cspsa_ll_file.c' always writes down the cspsa
   * binary image to file when closing. In normal applications this would
   * be a fine, but this application is merely listing the contents of
   * a CSPSA binary. Not closing CSPSA leads to some loss of memory, but as this is
   * intended to be a executable with short life time in a Linux
   * system it should not be a problem.
   * */

error_exit:
  free(cspsa_config.Extra_p);
  cspsa_config.Extra_p = NULL;
  return result;
}

/* ========================================================================== */

static void usage(void)
{
  fprintf(stderr, PROGRAM " " STRINGIFY(VERSION) " Copyright (C) 2010 ST-Ericsson AB\n");
  fprintf(stderr, "Purpose: Lists items in a CSPSA binary image to stdout.\n");
  fprintf(stderr, "Usage: " PROGRAM " [options] <file name>\n");
  fprintf(stderr, "\nWhere [options] may be:\n");
  fprintf(stderr, "\t-h\tprints this instruction\n");
  fprintf(stderr, "\t-o file\tname of output file\n");
}

/* ========================================================================== */

int main(int argc, char **argv)
{
  int result = EXIT_FAILURE;
  int opt;
  char *filename;
  struct cmd_line_options cmd_line_options;

  memset(&cmd_line_options, 0, sizeof(cmd_line_options));
  cmd_line_options.outfile = strdup("-");

  /* Parse command line options */
  while ((opt = getopt(argc, argv, "ho:")) != -1)
  {
    switch (opt)
    {
      case 'h':
      case '?':
        usage();
        result = EXIT_SUCCESS;
        goto error_exit;
        break;

      case 'o':
        free(cmd_line_options.outfile);
        cmd_line_options.outfile = strdup(optarg);
        break;

      default:
        usage();
        goto error_exit;
        break;
    }
  }

  if (argc - optind != 1)
  {
    fprintf(stderr, ERRORSTR "Expected input file argument missing.\n");
    usage();
    goto error_exit;
  }
  filename = argv[optind];

  result = process(&cmd_line_options, filename);

error_exit:
  free(cmd_line_options.outfile);
  return result;
}

/* ========================================================================== */
