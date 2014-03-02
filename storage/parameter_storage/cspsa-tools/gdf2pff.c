/* **************************************************************************
 *
 * gdf2pff.c
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: Martin Lundholm <martin.xa.lundholm@stericsson.com>
 * Author: Sebastian Rasmussen <sebastian.rasmussen@stericsson.com>
 *
 * DESCRIPTION:
 * Converts a GDF formated file to PFF file structure.
 *
 **************************************************************************** */

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
#include <string.h>
#include <errno.h>

/* ========================================================================== */

#define PROGRAM "gdf2pff"
#define VERSION 0.0.3

#define ERRORSTR "ERROR: "

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

/* ========================================================================== */

struct unit
{
  uint32_t key;
  uint32_t size;
  uint8_t *data;
};

struct cmd_line_options
{
  char *outputpath;
  char *itemname;
  bool hexoutput;
  bool txtoutput;
  char *inpath;
  bool debug;
};

/* ========================================================================== */

static const char ERRORSTR_INTERNAL[] = ERRORSTR "Internal error at %d.\n";
static const char ERRORSTR_OUT_OF_MEMORY[] = ERRORSTR "Out of memory at %d.\n";

/* ========================================================================== */

static int add(struct unit **unit_array_p, uint32_t *unit_array_count_p, uint32_t key, uint32_t size, uint8_t *data)
{
  int result = EXIT_FAILURE;

  *unit_array_p = realloc(*unit_array_p, (*unit_array_count_p + 1) * sizeof(struct unit));
  if (!*unit_array_p)
  {
    fprintf(stderr, ERRORSTR "Out of memory.\n");
    goto error_exit;
  }

  (*unit_array_p)[*unit_array_count_p].key = key;
  (*unit_array_p)[*unit_array_count_p].size = size;
  (*unit_array_p)[*unit_array_count_p].data = data;
  (*unit_array_count_p)++;

  result = EXIT_SUCCESS;

error_exit:
  return result;
}

/* ========================================================================== */

static void hexdump(uint8_t *data, uint32_t size)
{
  uint32_t i, j;

  for (i = 0; i < size; i += 16, data += 16)
  {
    printf("\t");
    for (j = 0; j < 16 && i + j < size; j++)
      printf("%02x ", data[j]);
    printf("\n");
  }
}

/* ========================================================================== */

static int hexwrite(FILE *f, uint8_t *data, uint32_t size)
{
  int result = EXIT_FAILURE;
  uint32_t i, j;

  for (i = 0; i < size; i += 16, data += 16)
  {
    for (j = 0; j < 16 && i + j < size; j++)
      if (fprintf(f, "%02x ", data[j]) < 0)
      {
        fprintf(stderr, ERRORSTR "hexwrite fprintf failed.\n");
        goto error_exit;
      }
    if (fprintf(f, "\n") < 0)
    {
      fprintf(stderr, ERRORSTR "hexwrite fprintf failed.\n");
      goto error_exit;
    }
  }

  result = EXIT_SUCCESS;

error_exit:
  return result;
}

/* ========================================================================== */

static void listunits(struct unit *unit_array, uint32_t unit_array_count)
{
  uint32_t i;

  for (i = 0; i < unit_array_count; i++)
  {
    printf("0x%08X, %d bytes:\n", unit_array[i].key, unit_array[i].size);
    hexdump(unit_array[i].data, unit_array[i].size);
  }
}

/* ========================================================================== */

static int writepffunits(struct unit *unit_array, uint32_t unit_array_count, struct cmd_line_options* cmd_line_options_p)
{
  int result = EXIT_FAILURE;
  uint32_t i;
  FILE *f = NULL;
  char filename[512];

  for (i = 0; i < unit_array_count; i++)
  {
    filename[0] = '\0';
    bool is_textfile = cmd_line_options_p->txtoutput;

    if (cmd_line_options_p->txtoutput)
    {
      /* Check that all data are printable characters, otherwise write as hex
       * or binary. */
      uint32_t j;
      uint32_t unit_size = unit_array[i].size;
      for (j = 0; j < unit_size; j++)
      {
        is_textfile = is_textfile && isprint(unit_array[i].data[j]);
      }
    }

    if (is_textfile)
    {
      size_t n;
      int r = sprintf(filename, "%s/0x%08X.%s_%04X.txt", cmd_line_options_p->outputpath, unit_array[i].key, cmd_line_options_p->itemname, i);
      if (r < 0)
      {
        fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
        goto error_exit;
      }
      f = fopen(filename, "wb");
      if (f == NULL)
      {
        fprintf(stderr, ERRORSTR "Could not open file '%s'.\n", filename);
        goto error_exit;
      }
      n = fwrite(unit_array[i].data, unit_array[i].size, 1, f);
      if (n != 1)
      {
        fprintf(stderr, ERRORSTR "Could not write to file '%s'.\n", filename);
        goto error_exit;
      }
    }
    else
    if (cmd_line_options_p->hexoutput)
    {
      int r = sprintf(filename, "%s/0x%08X.%s_%04X.hex", cmd_line_options_p->outputpath, unit_array[i].key, cmd_line_options_p->itemname, i);
      if (r < 0)
      {
        fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
        goto error_exit;
      }
      f = fopen(filename, "wb");
      if (f == NULL)
      {
        fprintf(stderr, ERRORSTR "Could not open file '%s'.\n", filename);
        goto error_exit;
      }
      result = hexwrite(f, unit_array[i].data, unit_array[i].size);
      if (result != EXIT_SUCCESS)
      {
        fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
        goto error_exit;
      }
    }
    else
    {
      size_t n;
      int r = sprintf(filename, "%s/0x%08X.%s_%04X.bin", cmd_line_options_p->outputpath, unit_array[i].key, cmd_line_options_p->itemname, i);
      if (r < 0)
      {
        fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
        goto error_exit;
      }
      f = fopen(filename, "w");
      if (f == NULL)
      {
        fprintf(stderr, ERRORSTR "Could not open file '%s'.\n", filename);
        goto error_exit;
      }
      n = fwrite(unit_array[i].data, unit_array[i].size, 1, f);
      if (n != 1)
      {
        fprintf(stderr, ERRORSTR "Could not write to file '%s'.\n", filename);
        goto error_exit;
      }
    }

    if (fclose(f))
    {
      fprintf(stderr, ERRORSTR "Could not close file '%s'.\n", filename);
      f = NULL;
      goto error_exit;
    }
    f = NULL;
    if (chmod(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))
    {
      fprintf(stderr, ERRORSTR "Could not chmod file '%s'.\n", filename);
      goto error_exit;
    }
    printf("File written: '%s', %d bytes:\n", filename, unit_array[i].size);
  }

  result = EXIT_SUCCESS;

error_exit:

  if (f && fclose(f))
  {
    fprintf(stderr, ERRORSTR "Could not close file '%s'.\n", filename);
  }

  return result;
}

/* ========================================================================== */

static int process(struct cmd_line_options* cmd_line_options_p)
{
  int result = EXIT_FAILURE;
  char line[10000];
  struct unit *unit_array = NULL;
  uint32_t unit_array_count = 0;
  FILE *f = NULL;
  uint8_t *data_p = NULL;
  uint32_t key;
  uint32_t size;
  uint32_t curr_index = 0;
  uint32_t items_imported = 0;
  uint32_t block;

  f = fopen(cmd_line_options_p->inpath, "r");
  if (f == NULL)
  {
    printf("Cannot open %s.\n", cmd_line_options_p->inpath);
    goto error_exit;
  }

  while (fgets(line, sizeof (line), f) != NULL)
  {
    if (ferror(f))
    {
      fprintf(stderr, ERRORSTR "Failed to read from file '%s'.\n", cmd_line_options_p->inpath);
      goto error_exit;
    }
    if (line[0] == '/' || line[0] == '\n')
    {
      continue;
    }
    if (isxdigit(line[0]))
    {
      int n;
      n = sscanf(line, "%x %d %d", &key, &size, &block);
      if (n < 3)
      {
        fprintf(stderr, ERRORSTR "Input file is malformatted for key(0x%08X).\n", key);
        fprintf(stderr, ERRORSTR "Failed parsing line starting with '%s'.\n", strtok(line, " \t\n\r"));
        goto error_exit;
      }
      data_p = malloc(size);
      if (!data_p)
      {
        fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
        goto error_exit;
      }
      curr_index = 0;
    }
    else if ((line[0] == '\t') && (data_p != NULL))
    {
      char* value_p;

      for (value_p = strtok(line, " \t\n\r"); value_p != NULL; value_p = strtok(NULL, " \t\n\r"))
      {
        uint32_t value;
        int n;

        n = sscanf(value_p, "%x", &value);
        if (n < 1)
        {
          fprintf(stderr, ERRORSTR "Input file is malformatted for key(0x%08X).\n", key);
          fprintf(stderr, ERRORSTR "Failed parsing line starting with '%s'.\n", strtok(line, " \t\n\r"));
          goto error_exit;
        }
        data_p[curr_index] = value;
        ++curr_index;
      }
      if (curr_index > size)
      {
          fprintf(stderr, ERRORSTR "Input file is malformatted for key(0x%08X).\n", key);
          fprintf(stderr, ERRORSTR "Size field does not match number of items.\n");
          fprintf(stderr, ERRORSTR "Failed parsing line starting with '%s'.\n", strtok(line, " \t\n\r"));
          goto error_exit;
      }
      if (curr_index == size)
      {
        result = add(&unit_array, &unit_array_count, key, size, data_p);
        if (result != EXIT_SUCCESS)
        {
          fprintf(stderr, ERRORSTR "Failed to add item to unit array.\n");
          goto error_exit;
        }
        data_p = NULL;
        ++items_imported;
        curr_index = 0;
      }
    }
    else
    /* By calling strtok here we allow trailing blanks in the file */
    if (strtok(line, " \t\n\r"))
    {
      fprintf(stderr, ERRORSTR "Input file is malformatted for key(0x%08X).\n", key);
      fprintf(stderr, ERRORSTR "Failed parsing line starting with '%s'.\n", line);
      goto error_exit;
    }
  }

  if (curr_index)
  {
    fprintf(stderr, ERRORSTR "Input file is malformatted for key(0x%08X).\n", key);
    fprintf(stderr, ERRORSTR "Size field does not match number of items.\n");
    fprintf(stderr, ERRORSTR "Failed parsing line starting with '%s'.\n", strtok(line, " \t\n\r"));
    goto error_exit;
  }

  if (cmd_line_options_p->debug)
  {
    printf("%d units imported.\n", items_imported);
    listunits(unit_array, unit_array_count);
  }

  result = writepffunits(unit_array, unit_array_count, cmd_line_options_p);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR "Writing PFF files failed.\n");
    goto error_exit;
  }

  printf("%s %s\n", PROGRAM, STRINGIFY(VERSION));

  result = EXIT_SUCCESS;

error_exit:
  if (unit_array != NULL)
  {
    uint32_t i;
    struct unit *temp_unit_array = unit_array;
    for (i = 0; i < unit_array_count; i++)
    {
      free(temp_unit_array->data);
      temp_unit_array++;
    }
  }
  if (fclose(f))
  {
    printf("Cannot close %s.\n", cmd_line_options_p->inpath);
  }
  free(unit_array);
  return result;
}

/* ========================================================================== */

static void usage(void)
{
  fprintf(stderr, PROGRAM " " STRINGIFY(VERSION) " Copyright (C) 2010 ST-Ericsson AB\n");
  fprintf(stderr, "Purpose: Parses a GDF file and writes coresponding PFF files.\n");
  fprintf(stderr, "Usage: " PROGRAM " [options] <gdf-file>\n");
  fprintf(stderr, "Where [options] may be:\n");
  fprintf(stderr, "\t-h\t: prints this instruction\n");
  fprintf(stderr, "\t-d\t: enable debug logs\n");
  fprintf(stderr, "\t-x\t: hex output\n");
  fprintf(stderr, "\t-t\t: text output if data is in text format\n");
  fprintf(stderr, "\t-p path\t: output path\n");
  fprintf(stderr, "\t-n name\t: default file name\n");
  fprintf(stderr, "Example:\n\t'./" PROGRAM " -n modem_files -t -x -p testout test_gdf.gdf'\n\tparses 'test_gdf.gdf' "
                  "and creates text and hex files in directory 'testout'.\n");
}

/* ========================================================================== */

int main(int argc, char **argv)
{
  int result = EXIT_FAILURE;
  struct cmd_line_options cmd_line_options;
  int opt;

  memset(&cmd_line_options, 0, sizeof(cmd_line_options));

  cmd_line_options.itemname = strdup("no_name");

  /* Parse command line options */
  while ((opt = getopt(argc, argv, "dhxp:tn:")) != -1)
  {
    switch (opt)
    {
      case 'h':
      case '?':
        usage();
        result = EXIT_SUCCESS;
        goto error_exit;

      case 'd':
        cmd_line_options.debug = true;
        break;

      case 'x':
        cmd_line_options.hexoutput = true;
        break;

      case 't':
        cmd_line_options.txtoutput = true;
        break;

      case 'n':
        if ((optarg == NULL) || (optarg[0] == '-'))
        {
          fprintf(stderr, ERRORSTR "Invalid path in argument.\n");
        }
        else
        {
          free(cmd_line_options.itemname);
          cmd_line_options.itemname = strdup(optarg);
          if (!cmd_line_options.itemname)
          {
            fprintf(stderr, ERRORSTR "Out of memory.\n");
            goto error_exit;
          }
        }
        break;

      case 'p':
        if ((optarg == NULL) || (optarg[0] == '-'))
        {
          fprintf(stderr, ERRORSTR "Invalid path in argument.\n");
        }
        else
        {
          int status = mkdir(optarg, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
          if (status == -1)
          {
            switch (errno)
            {
              case EEXIST:
                break;
              default:
                fprintf(stderr, ERRORSTR "Could not create directory '%s', errno = %d.\n", optarg, errno);
                goto error_exit;
            }
          }
          cmd_line_options.outputpath = strdup(optarg);
          if (!cmd_line_options.outputpath)
          {
            fprintf(stderr, ERRORSTR "Out of memory.\n");
            goto error_exit;
          }
          printf("Files are put in folder '%s'.\n", cmd_line_options.outputpath);
        }
        break;

      default:
        usage();
        goto error_exit;
    }
  }

  if (argc - optind != 1)
  {
    fprintf(stderr, ERRORSTR "Expected .gdf input file argument missing.\n");
    usage();
    goto error_exit;
  }
  cmd_line_options.inpath = argv[optind];

  result = process(&cmd_line_options);
  if (result != EXIT_SUCCESS)
    goto error_exit;

  result = EXIT_SUCCESS;

error_exit:

  free(cmd_line_options.itemname);
  free(cmd_line_options.outputpath);

  return result;
}

/* ========================================================================== */
