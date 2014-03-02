/* **************************************************************************
 *
 * pff2cspsa.c
 *
 * Copyright (C) 2009 ST-Ericsson SA
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

#include "cspsa.h"
#include "cspsa_core.h"
#include "cspsa_plugin.h"
#include "cspsa_ll_file.h"
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
#include <errno.h>

/* ========================================================================== */

#define PROGRAM "pff2cspsa"
#define VERSION 0.0.2

#define ERRORSTR      "ERROR: "
#define CSPSA_NAME    "WORKAREA"

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#define BLOCK_SIZE  (4 * 1024)
#define SECTOR_SIZE (1 * 1024)

/* ========================================================================== */

struct cmd_line_options
{
  char *output_file_name;
  uint32_t cspsa_area_size;
  uint32_t creation_time_stamp_id;
  bool creation_time_stamp_id_set;
  char *inpath;
  bool debug;
};

/* ========================================================================== */

static const char ERRORSTR_INTERNAL[] = ERRORSTR "Internal error at %d.\n";
static const char ERRORSTR_OUT_OF_MEMORY[] = ERRORSTR "Out of memory at %d.\n";

/* ========================================================================== */

char* cspsa_result_to_string(const CSPSA_Result_t Result)
{
  char* Result_p = "Unknown";

  #define M_MACRO_TO_STRING(__n) case __n: Result_p = #__n;break;
  switch (Result)
  {
    M_MACRO_TO_STRING(T_CSPSA_RESULT_OK)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_WARNINGS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_W_SIZE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_W_NEW_KEY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_W_INVALID_HEADER_FOUND)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_ERRORS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_BAD_PARAMETER)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_INVALID_KEY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_INVALID_SIZE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_OUT_OF_SPACE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_NO_VALID_IMAGE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_READ_ONLY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_READ_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_WRITE_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_END_OF_DATA)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_OPEN_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_ALREADY_EXISTS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_OUT_OF_MEMORY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_INTERNAL_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_SOCKET_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_NUMBER_OF_ITEMS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_UNDEFINED)
  }
  #undef M_MACRO_TO_STRING
  return Result_p;
}

/* ========================================================================== */

static int init_cspsa(CSPSA_Config_t *cspsa_config, struct cmd_line_options *cmd_line_options)
{
  int result = EXIT_FAILURE;

  CSPSA_LL_FILE_Init();

  cspsa_config->MemoryType = T_CSPSA_MEMORY_TYPE_FILE;

  cspsa_config->SizeInBytes = cmd_line_options->cspsa_area_size; // - Pick a size large enough.
  cspsa_config->StartOffsetInBytes = 0;
  cspsa_config->BlockSize = BLOCK_SIZE;
  cspsa_config->SectorSize = SECTOR_SIZE;
  cspsa_config->Attributes = 0;
  strncpy(cspsa_config->Name, CSPSA_NAME, D_CSPSA_MAX_NAME_SIZE);
  {
    CSPSA_LinuxDevice_t* LinuxDevice_p = (CSPSA_LinuxDevice_t*) malloc(sizeof(CSPSA_LinuxDevice_t));
    if (LinuxDevice_p == NULL)
    {
      fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
      goto error_exit;
    }
    memset(LinuxDevice_p, 0, sizeof(CSPSA_LinuxDevice_t));
    cspsa_config->Extra_p = LinuxDevice_p;
    LinuxDevice_p->DeviceName_p = cmd_line_options->output_file_name;
    cspsa_config->SizeOfExtra = sizeof(*LinuxDevice_p);
  }

  if (CSPSA_AddParameterArea(cspsa_config) != T_CSPSA_RESULT_OK)
    goto error_exit;

  result = EXIT_SUCCESS;

error_exit:
  return result;
}

/* ========================================================================== */

static int write_file_to_cspsa(CSPSA_Handle_t cspsa_handle, char *filename, uint32_t key, struct stat statinfo)
{
  int result = EXIT_FAILURE;
  uint8_t *buffer = NULL;
  char *fileext = NULL;
  FILE *f = NULL;

  buffer = (uint8_t *) malloc(statinfo.st_size + 1);
  if (buffer == NULL)
  {
    fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
    goto error_exit;
  }
  buffer[statinfo.st_size] = '\0';

  /* If the file extensions is none of txt, bin or hex the file is ignored. */

  fileext = strrchr(filename, '.');
  if (fileext == NULL)
  {
    fprintf(stderr, ERRORSTR "File '%s' lacks extension.\n", filename);
    goto error_exit;
  }

  f = fopen(filename, "rb");
  if (!f)
  {
    fprintf(stderr, ERRORSTR "Could not open input file.\n");
    goto error_exit;
  }
  if (fread(buffer, 1, statinfo.st_size, f) < (size_t) statinfo.st_size && !feof(f))
  {
    fprintf(stderr, ERRORSTR "Could not read input file.\n");
    goto error_exit;
  }

  fileext++;
  if ((strcmp(fileext, "bin") == 0) || (strcmp(fileext, "txt") == 0))
  {
    if (statinfo.st_size > 0)
    {
      if (CSPSA_CORE_WriteValue(cspsa_handle, key, statinfo.st_size, buffer) >= T_CSPSA_RESULT_ERRORS)
      {
        fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
        goto error_exit;
      }
    }
    else
    {
      printf(PROGRAM ": Skipping file '%s'.\n", filename);
    }
  }
  else if (strcmp(fileext, "hex") == 0)
  {
    uint8_t *hexval_buffer = (uint8_t *) malloc(statinfo.st_size);
    uint8_t *hexval_buffer_p = hexval_buffer;
    uint32_t hexval_buffer_size = 0;
    char *tempstr = (char *) buffer;
    char *endptr;
    uint32_t hexvalue;
    uint32_t size;

    if (hexval_buffer == NULL)
    {
      fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
      goto error_exit;
    }

    // - Remove comments to end of line.
    tempstr = strchr(tempstr, '#');
    while (tempstr)
    {
      while (!(*tempstr == '\0') && !(*tempstr == '\n') && !(*tempstr == '\r'))
      {
        *tempstr = '*'; // Just overwrite comment text with *.
        tempstr++;
      }
      tempstr = strchr(tempstr, '#');
    }
    // - Now start parsing all hex numbers.
    tempstr = (char *) buffer;
    while (*tempstr && !isxdigit(*tempstr))
      tempstr++;
    while (tempstr && *tempstr)
    {
      /* Values may or may not be prefixed with "0x" */
      uint32_t corr = (strstr(tempstr, "0x") == tempstr) ? 2 : 0;
      hexvalue = strtoul(tempstr, &endptr, 16);
      switch(endptr - tempstr - corr)
      {
        case 2:   size = 1;  break;
        case 4:   size = 2;  break;
        case 8:   size = 4;  break;
        default:
          fprintf(stderr, ERRORSTR "Hex file '%s' contains illegal length hex values.\n", filename);
          goto error_exit;

      }
      memcpy(hexval_buffer_p, &hexvalue, size);
      hexval_buffer_size += size;
      hexval_buffer_p += size;
      tempstr = endptr;
      while (*tempstr && !isxdigit(*tempstr))
        tempstr++;
    }
    if (hexval_buffer_size > 0)
    {
      if (CSPSA_CORE_WriteValue(cspsa_handle, key, hexval_buffer_size, hexval_buffer) >= T_CSPSA_RESULT_ERRORS)
      {
        fprintf(stderr, ERRORSTR "Failed to write to CSPSA.\n");
        goto error_exit;
      }
    }
    else
    {
      printf(PROGRAM ": Skipping file '%s'.\n", filename);
    }
    free(hexval_buffer);
  }

  result = EXIT_SUCCESS;

error_exit:

  if(f && fclose(f))
  {
    fprintf(stderr, ERRORSTR "Could not close input file.\n");
  }
  free(buffer);
  return result;
}

/* ========================================================================== */

static int process_dirtree(CSPSA_Handle_t cspsa_handle, int debug, char *realpath, uint32_t range_lo, uint32_t range_hi)
{
  int result = EXIT_FAILURE;
  struct dirent *dirent = NULL;
  char cwd[PATH_MAX];
  DIR *dir = NULL;

  cwd[0] = '\0';
  if (!getcwd(cwd, PATH_MAX))
  {
    fprintf(stderr, ERRORSTR "Can not retrieve working directory.\n");
    goto error_exit;
  }

  if (debug)
    printf(PROGRAM ": Opening directory %s\n", realpath);

  dir = opendir(realpath);
  if (!dir)
  {
    fprintf(stderr, ERRORSTR "Can not open input path '%s'.\n", realpath);
    goto error_exit;
  }

  if (chdir(realpath) < 0)
  {
    fprintf(stderr, ERRORSTR "Can not change into input path (%s).\n", realpath);
    goto error_exit;
  }

  /* For each entry in dir, check ranges */
  do
  {
    struct dirent entry;
    struct stat statinfo;

    if (readdir_r(dir, &entry, &dirent))
    {
      fprintf(stderr, "Can not read input path directory entry (%s).\n", realpath);
      result = EXIT_FAILURE;
      goto error_exit;
    }

    /* End of directory encountered skip entry generation */
    if (dirent == NULL)
      continue;

    if (stat(entry.d_name, &statinfo) < 0)
    {
      fprintf(stderr, "Can not retrieve input file meta data (%s).\n", entry.d_name);
      result = EXIT_FAILURE;
      goto error_exit;
    }

    if (S_ISREG(statinfo.st_mode))
    {
      char itemname[BUFSIZ];
      uint32_t itemkey;
      int r;

      /* File name: "<0xNNNNNNNN>.<name>" */
      r = sscanf(entry.d_name, "%X.%s", &itemkey, itemname);
      if (r < 2)
      {
        printf(PROGRAM ": Skipping file '%s'.\n", entry.d_name);
        continue;
      }

      if (debug)
        printf(PROGRAM ": Checking file %s.\n", entry.d_name);

      if ((itemkey < range_lo) || (itemkey > range_hi))
      {
        fprintf(stderr, ERRORSTR "(%d) Found item '%s' out or range in directory '%s'.\n", __LINE__, entry.d_name, realpath);
        result = EXIT_FAILURE;
        goto error_exit;
      }

      result = write_file_to_cspsa(cspsa_handle, entry.d_name, itemkey, statinfo);
      if (result != EXIT_SUCCESS)
      {
        fprintf(stderr, ERRORSTR "Failed to write file to CSPSA.\n");
        goto error_exit;
      }
    }
    else if (S_ISDIR(statinfo.st_mode))
    {
      char itemname[BUFSIZ];
      uint32_t subdir_range_lo;
      uint32_t subdir_range_hi;
      int r;

      /* Skip dot and dotdot directory entries */
      if (!strcmp(entry.d_name, ".") || !strcmp(entry.d_name, ".."))
        continue;

      /* Directory name: "<0xNNNNNNNN>-<0xMMMMMMMM>.<name>" */
      r = sscanf(entry.d_name, "%X-%X.%s", &subdir_range_lo, &subdir_range_hi, itemname);
      if (r < 3)
      {
        fprintf(stderr, ERRORSTR "Bad directory name: '%s'\n", entry.d_name);
        fprintf(stderr, ERRORSTR "A PFF directory name must be of the form <range lo>-<range hi>.<name>\n");
        result = EXIT_FAILURE;
        goto error_exit;
      }

      /* Check directory range and enter directory recursivly. */
      result = process_dirtree(cspsa_handle, debug, entry.d_name, subdir_range_lo, subdir_range_hi);
      if (result != EXIT_SUCCESS)
      {
        goto error_exit;
      }
    }
    /* Ignore any non-file, non-directory entries */
    else
    {
      fprintf(stderr, ERRORSTR "Ignoring unsupported directory entry: %s/%s\n", realpath, entry.d_name);
    }

  } while (dirent != NULL);

  result = EXIT_SUCCESS;

error_exit:

  if (dir && closedir(dir))
  {
    fprintf(stderr, ERRORSTR "Can not close input path.\n");
    result = EXIT_FAILURE;
    goto error_exit;
  }

  if (strlen(cwd) && chdir(cwd))
  {
    fprintf(stderr, ERRORSTR "Can not change into previous working directory.\n");
    result = EXIT_FAILURE;
    goto error_exit;
  }

  return result;
}

/* ========================================================================== */

static int process(struct cmd_line_options *cmd_line_options)
{
  int result = EXIT_FAILURE;
  struct stat statinfo;
  static CSPSA_Handle_t cspsa_handle;
  uint32_t range_lo = 0;
  uint32_t range_hi = UINT32_MAX;
  int r;
  char *creation_message = NULL;
  CSPSA_Result_t cspsa_result;
  CSPSA_Config_t cspsa_config;

  memset(&cspsa_config, 0, sizeof(cspsa_config));

  if (cmd_line_options->debug)
    printf(PROGRAM ": Processing %s\n", cmd_line_options->inpath);

  if (stat(cmd_line_options->inpath, &statinfo) < 0)
  {
    fprintf(stderr, "Can not retrieve input directory meta data.\n");
    goto error_exit;
  }

  if (!S_ISDIR(statinfo.st_mode))
  {
    fprintf(stderr, "Input directory is not a directory.\n");
    goto error_exit;
  }

  /* It is necessary to delete the file before using it because 'cspsa_ll_file.c' reads
   * up the contents of the file if it exists. */
  r = unlink(cmd_line_options->output_file_name);
  if (r && (errno != ENOENT))
  {
    fprintf(stderr, "Failed to delete file '%s'.\n", cmd_line_options->output_file_name);
    goto error_exit;
  }
  if (init_cspsa(&cspsa_config, cmd_line_options))
  {
    fprintf(stderr, "Failed to initialize CSPSA.\n");
    goto error_exit;
  }
  if (CSPSA_CORE_Create(CSPSA_NAME, &cspsa_handle) != T_CSPSA_RESULT_OK)
  {
    fprintf(stderr, "Failed to create CSPSA.\n");
    goto error_exit;
  }

  result = process_dirtree(cspsa_handle, cmd_line_options->debug, cmd_line_options->inpath, range_lo, range_hi);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, "Failed to process PFF directory tree.\n");
    goto error_exit;
  }

  if (cmd_line_options->creation_time_stamp_id_set)
  {
    struct tm *time_p;
    time_t tm;
    char *strtime;

    creation_message = (char *) malloc(1000);
    if (creation_message == NULL)
    {
      fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
      goto error_exit;
    }
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

    sprintf(creation_message, "%s %s %s", PROGRAM, STRINGIFY(VERSION), strtime);
    creation_message[strlen(creation_message)-1] = '\0';
    printf("Creation message stored in 0x%08X: '%s'\n", cmd_line_options->creation_time_stamp_id, creation_message);
    if (CSPSA_CORE_WriteValue(cspsa_handle, cmd_line_options->creation_time_stamp_id, strlen(creation_message) + 1, (uint8_t *)creation_message) >= T_CSPSA_RESULT_ERRORS)
    {
      fprintf(stderr, ERRORSTR "Writing creation details failed.\n");
      goto error_exit;
    }
  }

  if ((cspsa_result = CSPSA_CORE_Close(&cspsa_handle)) >= T_CSPSA_RESULT_ERRORS)
  {
    fprintf(stderr, ERRORSTR "A CSPSA image could not be written: %s\n", cspsa_result_to_string(cspsa_result));
    goto error_exit;
  }

  printf(PROGRAM " has successfully written file '%s'.\n", cmd_line_options->output_file_name);

  result = EXIT_SUCCESS;

error_exit:
  CSPSA_RemoveParameterArea(&cspsa_config);
  free(creation_message);
  free(cspsa_config.Extra_p);
  return result;
}

/* ========================================================================== */

static void usage(void)
{
  fprintf(stderr, PROGRAM " " STRINGIFY(VERSION) " Copyright (C) 2010 ST-Ericsson AB\n");
  fprintf(stderr, "Purpose: Reads a PFF file structure and creates a CSPSA binary image.\n");
  fprintf(stderr, "Usage: " PROGRAM " [options] -z <size of area> <path>\n");
  fprintf(stderr, "\nWhere [options] may be:\n");
  fprintf(stderr, "\t-h\tprints this instruction\n");
  fprintf(stderr, "\t-f <file name>\tname of created file\n");
  fprintf(stderr, "\t-d\tenable debug logs\n");
  fprintf(stderr, "\t-t m\tstores a creation time stamp in item 'm'\n");
  fprintf(stderr, "\t-z size\tsets the size of the CSPSA (mandatory)\n");
}

/* ========================================================================== */

int main(int argc, char **argv)
{
  int result = EXIT_FAILURE;
  int opt;
  struct cmd_line_options cmd_line_options;

  memset(&cmd_line_options, 0, sizeof(cmd_line_options));
  cmd_line_options.output_file_name = strdup("cspsa.bin");

  /* Parse command line options */
  while ((opt = getopt(argc, argv, "dhf:z:t:")) != -1)
  {
    switch (opt)
    {
      case '?':
      case 'h':
        usage();
        break;

      case 'd':
        cmd_line_options.debug = true;
        break;

      case 'z':
        {
          char *endptr;
          cmd_line_options.cspsa_area_size = strtoul(optarg, &endptr, 0);
          if ((optarg == endptr) || (cmd_line_options.cspsa_area_size % 1024))
          {
            fprintf(stderr, ERRORSTR "CSPSA size must be aligned to %d (0x%08X).\n", BLOCK_SIZE, BLOCK_SIZE);
            goto error_exit;
          }
        }
        break;

      case 'f':
        free(cmd_line_options.output_file_name);
        cmd_line_options.output_file_name = strdup(optarg);
        if (cmd_line_options.output_file_name == NULL)
        {
          fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
          goto error_exit;
        }
        break;

      case 't':
        {
          char *endptr;
          cmd_line_options.creation_time_stamp_id = strtoul(optarg, &endptr, 0);
          if (endptr != optarg)
          {
            cmd_line_options.creation_time_stamp_id_set = true;
          }
          else
          {
            fprintf(stderr, ERRORSTR "Unrecognized argument %s to option '%c'.\n", optarg, optopt);
            goto error_exit;
          }
        }
        break;

      default:
        usage();
        goto error_exit;
    }
  }

  if (argc - optind != 1)
  {
    fprintf(stderr, ERRORSTR "Expected input file argument missing.\n");
    usage();
    goto error_exit;
  }
  cmd_line_options.inpath = argv[optind];

  if (!cmd_line_options.cspsa_area_size)
  {
    fprintf(stderr, ERRORSTR "Argument after -z must be a non-zero value.\n");
    usage();
    goto error_exit;
  }

  result = process(&cmd_line_options);

error_exit:
  free(cmd_line_options.output_file_name);
  return result;
}

/* ========================================================================== */
