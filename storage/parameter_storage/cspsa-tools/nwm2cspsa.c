/* **************************************************************************
 *
 * nwm2cspsa.c
 *
 * Copyright (C) 2012 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2012, johan.rudholm@stericsson.com
 *
 * Based on nwm2gdf.c
 *
 * DESCRIPTION:
 * Reads a NWM file tree and writes it to CSPSA.
 *
 **************************************************************************** */

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "nwmcommon.h"
#include "cspsa.h"

/* ========================================================================== */

#define PROGRAM "nwm2cspsa"
#define VERSION 0.0.1

/* ========================================================================== */

static int init_cspsa(char *area, CSPSA_Handle_t *handle)
{
  CSPSA_Result_t result;

  result = CSPSA_Open(area, handle);
  if (result > T_CSPSA_RESULT_ERRORS)
  {
    fprintf(stderr, ERRORSTR "Failed to open CSPSA area %s (%d).\n", area, result);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}

/* ========================================================================== */

static int exit_cspsa(CSPSA_Handle_t *handle)
{
  CSPSA_Result_t result;

  result = CSPSA_Close(handle);
  if (result > T_CSPSA_RESULT_ERRORS)
  {
    fprintf(stderr, ERRORSTR "Failed to close CSPSA (%d).\n", result);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}

/* ========================================================================== */

static int flush_cspsa(CSPSA_Handle_t *handle)
{
  CSPSA_Result_t result;

  result = CSPSA_Flush(*handle);
  if (result > T_CSPSA_RESULT_ERRORS)
  {
    fprintf(stderr, ERRORSTR "Failed to flush CSPSA (%d).\n", result);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}

/* ========================================================================== */
static int emit_parameter_to_cspsa(struct cmd_line_options* cmd_line_options,
				CSPSA_Handle_t *handle, int key,
				unsigned int size, unsigned char *data)
{
  CSPSA_Result_t result;

  result = CSPSA_WriteValue(*handle, key, size, (uint8_t *) data);
  if (result > T_CSPSA_RESULT_ERRORS)
  {
    fprintf(stderr, ERRORSTR "Failed to write key %d, size %d, to CSPSA (%d).\n", key, size, result);
    return EXIT_FAILURE;
  }
  if (cmd_line_options->debug)
    printf(PROGRAM ": Wrote key %d, size %d, to CSPSA.\n", key, size);

  return EXIT_SUCCESS;

}

/* ========================================================================== */

static int process(struct cmd_line_options* cmd_line_options)
{
  int result = EXIT_FAILURE;
  uint32_t entries;
  struct state state;
  param_t *par;
  CSPSA_Handle_t handle;
  char idstr[BUFSIZ];
  int i;
  int cspsa_written = 0;

  memset(&state, 0, sizeof(state));

  if (cmd_line_options->debug) {
    printf(PROGRAM ": Processing %s (only stuff below", cmd_line_options->inpath);
    for (i = 0; i < cmd_line_options->subpaths; i++)
      printf(" %s", cmd_line_options->subpath[i]);
    printf("), writing to %s.\n", cmd_line_options->area);
  }

  result = init_cspsa(cmd_line_options->area, &handle);
  if (result != EXIT_SUCCESS)
    goto error_exit;

  /* unit id 0 is reserved for the number of entries */
  state.entries = 1;

  /* create entries in a linked list for each file
   * or directory in the input directory tree */
  if (cmd_line_options->subpaths)
  {
    for (i = 0; i < cmd_line_options->subpaths; i++)
    {
      result |= process_dirtree(cmd_line_options, cmd_line_options->inpath, "/",
					cmd_line_options->subpath[i], &state);
    }
  }
  else
  {
    result = process_dirtree(cmd_line_options, cmd_line_options->inpath, "/",
					NULL, &state);
  }

  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  /* Create small header */
  result = create_id(PROGRAM, STRINGIFY(VERSION), idstr, BUFSIZ);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  /* Wipe key 0, we have no parameter entries in CSPSA yet */
  entries = 0;
  cspsa_written = 1;
  result = emit_parameter_to_cspsa(cmd_line_options, &handle, 0,
				sizeof (entries), (unsigned char *) &entries);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  /* Make sure key 0 hits storage */
  result = flush_cspsa(&handle);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  /* Write header */
  result = emit_parameter_to_cspsa(cmd_line_options, &handle, MAX_FILES + 1,
				strlen(idstr) + 1, (unsigned char *) idstr);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  /* Dump the stuff, par points to head of linked list */
  par = state.par;
  while (par != NULL)
  {
    result = emit_parameter_to_cspsa(cmd_line_options, &handle, par->key,
							par->size, par->data);
    if (result != EXIT_SUCCESS)
      goto error_exit;
    par = next_param(par);
  }

  /* Make sure the parameters hit storage before key 0 */
  result = flush_cspsa(&handle);
  if (result != EXIT_SUCCESS)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  /* a unit id 0 output the number of files+dirs */
  entries = state.entries - 1;
  result = emit_parameter_to_cspsa(cmd_line_options, &handle, 0,
				sizeof (entries), (unsigned char *) &entries);

  printf(PROGRAM ": Read %d entries from %s, wrote to %s\n", entries,
			cmd_line_options->inpath, cmd_line_options->area);

error_exit:

  if (state.par)
    free_param(state.par);
  result |= exit_cspsa(&handle);

  if (result != EXIT_SUCCESS)
  {
    if (cspsa_written)
      fprintf(stderr, ERRORSTR PROGRAM ": WARNING: CSPSA was most likely modified!\n");
    else
      fprintf(stderr, ERRORSTR PROGRAM ": CSPSA was NOT modified.\n");
  }

  return result;
}

/* ========================================================================== */

static void usage(void)
{
  fprintf(stderr, PROGRAM " " STRINGIFY(VERSION) " Copyright (C) 2012 ST-Ericsson AB\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Usage: " PROGRAM " [options] -c <CSPSA-area> <directory-path> [<subpath> ...]\n");
  fprintf(stderr, "\nWhere [options] may be:\n");
  fprintf(stderr, "\t-d\tenable debug logs\n");
}

/* ========================================================================== */

int main(int argc, char **argv)
{
  int result = EXIT_FAILURE;
  struct cmd_line_options cmd_line_options;
  int opt, i;

  memset(&cmd_line_options, 0, sizeof(cmd_line_options));

  cmd_line_options.program = PROGRAM;

  /* Parse command line options */
  while ((opt = getopt(argc, argv, "dhc:")) != -1)
  {
    switch (opt)
    {
      case '?':
      case 'h':
        usage();
        break;

      case 'd':
        cmd_line_options.debug = 1;
        break;

      case 'c':
        cmd_line_options.area = strdup(optarg);
        break;

      default:
        goto error_exit;
    }
  }

  if (argc - optind < 1)
  {
    fprintf(stderr, ERRORSTR "Expected input path arguments missing.\n");
    goto error_exit;
  }
  cmd_line_options.inpath = strdup(argv[optind]);
  cmd_line_options.subpaths = argc - optind - 1;
  if (cmd_line_options.subpaths)
  {
    cmd_line_options.subpath = malloc(cmd_line_options.subpaths * sizeof(char *));
    if (!cmd_line_options.subpath)
    {
      fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
      goto error_exit;
    }

    for (i = 0; i < cmd_line_options.subpaths; i++)
      cmd_line_options.subpath[i] = NULL;

    for (i = 0; i < cmd_line_options.subpaths; i++)
    {
      cmd_line_options.subpath[i] = strdup(argv[optind + 1 + i]);
      if (!cmd_line_options.subpath[i])
      {
        fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
        goto error_exit;
      }

      if (cmd_line_options.subpath[i][strlen(cmd_line_options.subpath[i]) - 1] == '/')
        cmd_line_options.subpath[i][strlen(cmd_line_options.subpath[i]) - 1] = '\0';

      if (cmd_line_options.subpath[i][0] != '/')
      {
        fprintf(stderr, ERRORSTR "Subpath must begin with a '/': %s.\n",
						cmd_line_options.subpath[i]);
        goto error_exit;
      }
    }
  }
  else
  {
    cmd_line_options.subpath = NULL;
  }

  if (!cmd_line_options.area)
  {
    fprintf(stderr, ERRORSTR "Missing option -c, CSPSA area.\n");
    goto error_exit;
  }

  result = process(&cmd_line_options);

error_exit:
  free(cmd_line_options.inpath);
  free(cmd_line_options.area);
  for (i = 0; (i < cmd_line_options.subpaths) && cmd_line_options.subpath[i]; i++)
    free(cmd_line_options.subpath[i]);
  free(cmd_line_options.subpath);
  return result;
}

/* ========================================================================== */
