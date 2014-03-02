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
 * Author: Sebastian Rasmussen <sebastian.rasmussen@stericsson.com>
 *
 * DESCRIPTION:
 * List the contents of a GDF file containing NWM items.
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

/* ========================================================================== */

#define PROGRAM "gdflist"
#define VERSION 0.0.2

#define ERRORSTR "ERROR: "

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

/* ========================================================================== */

/* This block of defines is inherited from cspsa2nwm in order to achieve compatibility */

/** size of a parameter with info on a file that is to be created from a CSPSA parameter */
#define SIZE_OF_FILE_INFO 96
/** Max length of a path name (excluding trailing 0)  */
#define FS_PATH_MAX_LENGTH    52
/** Max length of a file name (excluding trailing 0) */
#define FS_NAME_MAX_LENGTH    12

/** Index in file info that contains the type of the file */
#define TYPE_INDEX           0
/** Index in file info that contains creation time of the file, seconds since 1970 */
#define C_TIME_SEC_INDEX     4
/** Index in file info that contains creation time of the file, micro seconds */
#define C_TIME_USEC_INDEX    8
/** Index in file info that contains the mode of the file */
#define MODE_INDEX          12
/** Index in file info that contains the size of the file */
#define FILE_SIZE_INDEX     16
/** Index in file info that contains the path to the file */
#define PATH_INDEX          24
/** Index in file info that contains the name of the file */
#define NAME_INDEX      ((PATH_INDEX) + FS_PATH_MAX_LENGTH)
/** Index in file info that contains CSPSA key that contains the contents of the file */
#define DUNIT_INDEX     ((NAME_INDEX) + FS_NAME_MAX_LENGTH)
/** Index in file info that unused part of file info */
#define UNUSED_INDEX    ((DUNIT_INDEX) + 4)

/** The item is a directory */
#define FS_TYPE_DIR  0
/** The item is a regular file */
#define FS_TYPE_REGULAR 1

/* This is an arbitrarily chosen limit for this conversion program, in order to
make it easy to separate file info units from file data units. */
#define MAX_FILES 999

/* ========================================================================== */

struct unit
{
  uint32_t key;
  uint32_t size;
  unsigned char *data;
};

struct cmd_line_options
{
  char *outfile;
};

/* ========================================================================== */

static const char ERRORSTR_INTERNAL[] = ERRORSTR "Internal error at %d.\n";
static const char ERRORSTR_OUT_OF_MEMORY[] = ERRORSTR "Out of memory at %d.\n";

/* ========================================================================== */

static int add(struct unit ** unit_array_p, uint32_t *unit_array_count_p, uint32_t key, uint32_t size, unsigned char *data)
{
  int result = EXIT_FAILURE;

  (*unit_array_p) = realloc((*unit_array_p), (*unit_array_count_p + 1) * sizeof (struct unit));
  if (*unit_array_p == NULL)
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

static void hexdump(FILE *out, unsigned char *data, uint32_t size)
{
  uint32_t i, j;

  for (i = 0; i < size; i += 16, data += 16)
  {
    fprintf(out, "\t");
    for (j = 0; j < 16 && i + j < size; j++)
    {
      fprintf(out, "%02x ", data[j]);
    }
    fprintf(out, "\n");
  }
}

/* ========================================================================== */

static int listunits(FILE *out, struct unit *unit_array, uint32_t unit_array_count, uint32_t from_key, uint32_t to_key)
{
  int result = EXIT_FAILURE;
  uint32_t i;

  if (unit_array == NULL)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  for (i = 0; i < unit_array_count; i++)
  {
    if ((from_key <= unit_array[i].key) && (unit_array[i].key <= to_key))
    {
      fprintf(out, "0x%04x, %d bytes:\n", unit_array[i].key, unit_array[i].size);
      hexdump(out, unit_array[i].data, unit_array[i].size);
    }
  }

  result = EXIT_SUCCESS;

error_exit:
  return result;
}

/* ========================================================================== */

static int unitexists(struct unit *unit_array, uint32_t unit_array_count, uint32_t key, bool *exist)
{
  int result = EXIT_FAILURE;
  uint32_t i;

  *exist = false;

  if ((unit_array == NULL) || (exist == NULL))
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  for (i = 0; i < unit_array_count; i++)
  {
    if (unit_array[i].key == key)
    {
      *exist = true;
      break;
    }
  }

  result = EXIT_SUCCESS;

error_exit:
  return result;
}

/* ========================================================================== */

static int getunit(struct unit *unit_array, uint32_t unit_array_count, uint32_t key, struct unit **unit_p)
{
  int result = EXIT_FAILURE;
  uint32_t i;

  *unit_p = NULL;

  if ((unit_array == NULL) || (unit_p == NULL))
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  for (i = 0; i < unit_array_count; i++)
  {
    if (unit_array[i].key == key)
    {
      *unit_p = &unit_array[i];
      break;
    }
  }

  if (*unit_p == NULL)
  {
    fprintf(stderr, ERRORSTR "Can not find unit %d.\n", key);
  }

  result = EXIT_SUCCESS;

error_exit:
  return result;
}

/* ========================================================================== */

static int getword(struct unit *unit, uint32_t offset, uint32_t *word_p)
{
  int result = EXIT_FAILURE;
  uint32_t word = 0;
  unsigned char *data;

  if ((unit == NULL) || ((offset + 4) > unit->size))
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  data = unit->data;
  data += offset;

  word  = data[0] <<  0;
  word |= data[1] <<  8;
  word |= data[2] << 16;
  word |= data[3] << 24;

  *word_p = word;

  result = EXIT_SUCCESS;

error_exit:
  return result;
}

/* ========================================================================== */

static int print_filedata(FILE *out, struct unit *unit)
{
  int result = EXIT_FAILURE;

  if (unit == NULL)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }
  hexdump(out, unit->data, unit->size);
  result = EXIT_SUCCESS;

error_exit:
  return result;
}

/* ========================================================================== */

static int print_fileinfo(FILE *out, struct unit *unit)
{
  int result = EXIT_FAILURE;
  char str[BUFSIZ];
  uint32_t mode;
  struct tm *tm;
  time_t ctime;
  uint32_t word;

  if ((unit == NULL) || (unit->size != SIZE_OF_FILE_INFO))
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  memset(str, 0x00, sizeof (str));
  strncat(str, (char *) &unit->data[PATH_INDEX], FS_PATH_MAX_LENGTH);
  strcat(str, "---");
  strncat(str, (char *) &unit->data[NAME_INDEX], FS_NAME_MAX_LENGTH);
  fprintf(out, "Path/Name: %s\n", str);

  if (unit->data[TYPE_INDEX] == FS_TYPE_DIR)
    fprintf(out, "Type: Directory\n");
  else if (unit->data[TYPE_INDEX] == FS_TYPE_REGULAR)
    fprintf(out, "Type: File\n");
  else
    fprintf(out, "Type: %d\n", unit->data[TYPE_INDEX]);

  result = getword(unit, FILE_SIZE_INDEX, &word);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }
  fprintf(out, "size: %d bytes\n", word);

  result = getword(unit, C_TIME_SEC_INDEX, &word);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }
  ctime = (time_t) word;
  tm = gmtime(&ctime);
  if (!tm)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  {
    uint32_t s, us;
    int r = strftime(str, sizeof (str), "%F %T %Z", tm);

    if (!r)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }
    result = getword(unit, C_TIME_SEC_INDEX, &s);
    if (result != EXIT_SUCCESS)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }
    result = getword(unit, C_TIME_USEC_INDEX, &us);
    if (result != EXIT_SUCCESS)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }
    fprintf(out, "Time: %s (%d seconds %d useconds)\n", str, s, us);
  }

  fprintf(out, "Mode: ");

  result = getword(unit, MODE_INDEX, &mode);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  if (S_ISREG(mode) || (mode & S_IFMT) == 0)
    fprintf(out, "-");
  else if (S_ISDIR(mode))
    fprintf(out, "d");
  else if (S_ISCHR(mode))
    fprintf(out, "c");
  else if (S_ISBLK(mode))
    fprintf(out, "b");
  else if (S_ISFIFO(mode))
    fprintf(out, "f");
  else if (S_ISLNK(mode))
    fprintf(out, "l");
  else if (S_ISSOCK(mode))
    fprintf(out, "s");
  else
    fprintf(out, "%07o", mode & S_IFMT);
  mode &= ~S_IFMT;

  fprintf(out, "%c", mode & S_IRUSR ? 'r' : '-');
  fprintf(out, "%c", mode & S_IWUSR ? 'w' : '-');
  fprintf(out, "%c", mode & S_IXUSR ? 'x' : '-');
  fprintf(out, "%c", mode & S_IRGRP ? 'r' : '-');
  fprintf(out, "%c", mode & S_IWGRP ? 'w' : '-');
  fprintf(out, "%c", mode & S_IXGRP ? 'x' : '-');
  fprintf(out, "%c", mode & S_IROTH ? 'r' : '-');
  fprintf(out, "%c", mode & S_IWOTH ? 'w' : '-');
  fprintf(out, "%c", mode & S_IXOTH ? 'x' : '-');
  mode &= ~S_IRWXU;
  mode &= ~S_IRWXG;
  mode &= ~S_IRWXO;

  fprintf(out, " ");

  if (mode & S_ISUID)
    fprintf(out, "setuid ");
  mode &= ~S_ISUID;

  if (mode & S_ISGID)
    fprintf(out, "setgid ");
  mode &= ~S_ISGID;

  if (mode)
    fprintf(out, "other permissions = %07o", mode);

  fprintf(out, "\n");

error_exit:
  return result;
}

/* ========================================================================== */

static int listfiles(FILE *out, struct unit *unit_array, uint32_t unit_array_count)
{
  int result = EXIT_FAILURE;
  struct unit * unit;
  uint32_t files;
  uint32_t i;

  result = getunit(unit_array, unit_array_count, 0, &unit);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  result = getword(unit, 0, &files);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  fprintf(out, "%d files\n\n", files);

  for (i = 0; i < files; i++)
  {
    uint32_t data;
    result = getunit(unit_array, unit_array_count, 1 + i, &unit);
    if (result != EXIT_SUCCESS)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }

    result = print_fileinfo(out, unit);
    if (result != EXIT_SUCCESS)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }

    result = getword(unit, DUNIT_INDEX, &data);
    if (result != EXIT_SUCCESS)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }

    if (data != 0xffffffff)
    {
      result = getunit(unit_array, unit_array_count, data, &unit);
      if (result != EXIT_SUCCESS)
      {
        fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
        goto error_exit;
      }
      print_filedata(out, unit);
    }
    fprintf(out, "\n");
  }

error_exit:
  return result;
}

/* ========================================================================== */

static int listversion(FILE *out, struct unit *unit_array, uint32_t unit_array_count)
{
  int result = EXIT_FAILURE;
  bool exist;

  result = unitexists(unit_array, unit_array_count, MAX_FILES + 1, &exist);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }
  if (!exist)
  {
    fprintf(out, "Not generated by nwm2gdf!\n");
  }
  else
  {
    struct unit *unit;
    result = getunit(unit_array, unit_array_count, MAX_FILES + 1, &unit);
    if (result != EXIT_SUCCESS)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }
    fprintf(out, "Generated by: %s\n", (char *) unit->data);
  }

error_exit:
  return result;
}

/* ========================================================================== */

static int process(struct cmd_line_options *cmd_line_options, int debug, char *inpath)
{
  int result = EXIT_FAILURE;
  char line[10000];
  struct unit *unit_array = NULL;
  uint32_t unit_array_count = 0;
  FILE *f = NULL;
  FILE *out = NULL;
  unsigned char *data_p = NULL;
  uint32_t unit;
  uint32_t size;
  uint32_t curr_index = 0;
  uint32_t items_imported = 0;
  uint32_t block;

  f = fopen(inpath, "r");
  if (f == NULL)
  {
    fprintf(stderr, ERRORSTR "Cannot open %s\n", inpath);
    goto error_exit;
  }

  /* Open output file (can be stdout) */
  if (!strcmp(cmd_line_options->outfile, "-"))
    out = stdout;
  else
    out = fopen(cmd_line_options->outfile, "w");

  while (fgets(line, sizeof (line), f) != NULL)
  {
    if (strlen(line) == 0 || line[0] == '/')
    {
      continue;
    }
    if (isxdigit(line[0]))
    {
      int n = sscanf(line, "%x %d %d", &unit, &size, &block);
      if (n < 3)
      {
        fprintf(stderr, "Malformatted input line '%s'.\n", line);
        goto error_exit;
      }
      data_p = malloc(size);
      if (data_p == NULL)
      {
        fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
        goto error_exit;
      }
      memset(data_p, 0, size);
      curr_index = 0;
    }
    else if (line[0] == '\t')
    {
      char* Value_p;

      for (Value_p = strtok(line, " \t\n\r"); Value_p != NULL; Value_p = strtok(NULL, " \t\n\r"))
      {
        uint32_t Value;
        int n = sscanf(Value_p, "%x", &Value);
        if (n < 1)
        {
          fprintf(stderr, "Malformatted input value '%s'.\n", Value_p);
          goto error_exit;
        }
        data_p[curr_index] = Value;
        ++curr_index;
      }
      if (curr_index > size)
      {
          fprintf(stderr, ERRORSTR "Input file is malformatted for unit(0x%08X).\n", unit);
          fprintf(stderr, ERRORSTR "Size field does not match number of items.\n");
          fprintf(stderr, ERRORSTR "Failed parsing line starting with '%s'.\n", strtok(line, " \t\n\r"));
          goto error_exit;
      }
      if (curr_index == size)
      {
        result = add(&unit_array, &unit_array_count, unit, size, data_p);
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
      fprintf(stderr, ERRORSTR "Input file is malformatted for unit(0x%08X).\n", unit);
      fprintf(stderr, ERRORSTR "Failed parsing line starting with '%s'.\n", line);
      goto error_exit;
    }
  }

  if (curr_index)
  {
    fprintf(stderr, ERRORSTR "Input file is malformatted for unit(0x%08X).\n", unit);
    fprintf(stderr, ERRORSTR "Size field does not match number of items.\n");
    fprintf(stderr, ERRORSTR "Failed parsing line starting with '%s'.\n", strtok(line, " \t\n\r"));
    goto error_exit;
  }

  if (debug)
  {
    fprintf(out, "%d units imported\n", items_imported);
    listunits(out, unit_array, unit_array_count, 0, UINT32_MAX);
  }

  result = listfiles(out, unit_array, unit_array_count);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  if (debug)
  {
    fprintf(out, "%d units imported\n", items_imported);
    result = listunits(out, unit_array, unit_array_count, 0, UINT32_MAX);
    if (result != EXIT_SUCCESS)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }
  }
  else
  {
    result = listunits(out, unit_array, unit_array_count, UINT16_MAX + 1, UINT32_MAX);
    if (result != EXIT_SUCCESS)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }
  }
  result = listversion(out, unit_array, unit_array_count);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  fprintf(out, "%s %s\n", PROGRAM, STRINGIFY(VERSION));

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
  free(unit_array);
  if (fclose(f))
  {
    fprintf(stderr, ERRORSTR "Cannot close %s\n", inpath);
  }
  if (strcmp(cmd_line_options->outfile, "-") && fclose(out))
  {
    fprintf(stderr, ERRORSTR "Can not close output file");
  }
  return result;
}

/* ========================================================================== */

static void usage(void)
{
  fprintf(stderr, PROGRAM " " STRINGIFY(VERSION) " Copyright (C) 2010 ST-Ericsson AB\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Usage: gdflist [options] <gdf-file>\n");
  fprintf(stderr, "\nWhere [options] may be:\n");
  fprintf(stderr, "\t-d\tenable debug logs\n");
  fprintf(stderr, "\t-o file\tname of output file\n");
}

/* ========================================================================== */

int main(int argc, char **argv)
{
  int result = EXIT_FAILURE;
  int opt;
  char *inpath;
  int debug = 0;
  struct cmd_line_options cmd_line_options;

  memset(&cmd_line_options, 0, sizeof(cmd_line_options));
  cmd_line_options.outfile = strdup("-");

  /* Parse command line options */
  while ((opt = getopt(argc, argv, "dho:")) != -1)
  {
    switch (opt)
    {
      case 'h':
        usage();
        break;

      case 'd':
        debug = 1;
        break;

      case 'o':
        free(cmd_line_options.outfile);
        cmd_line_options.outfile = strdup(optarg);
        break;

      case '?':
        fprintf(stderr, ERRORSTR "Unrecognized option '%c'.\n", optopt);
        goto error_exit;

      default:
        fprintf(stderr, ERRORSTR "Unexpected option '%c'.\n", opt);
        goto error_exit;
    }
  }

  if (argc - optind != 1)
  {
    fprintf(stderr, ERRORSTR "Expected .gdf input file argument missing.\n");
  }
  inpath = argv[optind];

  result = process(&cmd_line_options, debug, inpath);

error_exit:
  free(cmd_line_options.outfile);
  return result;
}

/* ========================================================================== */
