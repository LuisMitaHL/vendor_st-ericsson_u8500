/* **************************************************************************
 *
 * nwmcommon.c
 *
 * Copyright (C) 2012 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, sebastian.rasmussen@stericsson.com
 * Author: 2012, johan.rudholm@stericsson.com
 *
 * DESCRIPTION:
 * Contains functions common to the nwm*-commands
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


/* ========================================================================== */

static param_t *alloc_param(param_t *par, int size) {
  param_t *newp;

  newp = malloc(sizeof(param_t));
  if (!newp)
    return NULL;

  if (size > 0)
  {
    newp->data = malloc(size);
    if (!newp->data)
    {
      free(newp);
      return NULL;
    }
  }
  else
  {
    newp->data = NULL;
  }
  newp->size = size;

  /* Insert a new parameter after par */
  if (par)
  {
    newp->next = par->next;
    if (par->next)
      par->next->prev = newp;
    par->next = newp;
  }
  else
  {
    newp->next = NULL;
  }

  newp->prev = par;

  return newp;

}

/* ========================================================================== */

static param_t *rewind_param(param_t *par)
{

  while (par->prev)
    par = par->prev;

  return par;

}

/* ========================================================================== */

param_t *next_param(param_t *par)
{

  if (par)
    return par->next;
  else
    return NULL;

}

/* ========================================================================== */

void free_param(param_t *par)
{
  param_t *next;

  par = rewind_param(par);

  while (par)
  {
    next = next_param(par);
    if (par->data)
      free(par->data);
    free(par);
    par = next;
  }

}

/* ========================================================================== */

static void set_bytes(unsigned char *buf, int index, unsigned int size, unsigned char *bytes)
{
  memcpy(&buf[index], bytes, size);
}

/* ========================================================================== */

static int set_word(unsigned char *buf, int index, uint32_t value)
{
  int result = EXIT_FAILURE;
  if (index % 4 != 0)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  set_bytes(buf, index, sizeof (uint32_t), (unsigned char *) &value);

  result = EXIT_SUCCESS;

error_exit:
  return result;
}

/* ========================================================================== */

int create_id(char *program, char *version, char *paramstr, unsigned int size)
{
  int result = EXIT_FAILURE;
  char timestr[BUFSIZ];
  struct tm *tm;
  time_t now;
  int r;

  now = time(NULL);
  if (now == (time_t) -1)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  tm = gmtime(&now);
  if (!tm)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  if (strftime(timestr, sizeof (timestr), "%F %T %Z", tm) <= 0)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  r = snprintf(paramstr, size, "%s %s %s", program, version, timestr);
  if (r < 0 || (unsigned int) r >= size)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  result = EXIT_SUCCESS;

error_exit:
  return result;
}

/* ========================================================================== */

static int pack_fileinfo(param_t *par, char *path, char *name,
				struct stat *statinfo)
{
  unsigned int len;
  uint32_t mode;
  int r = 0;

  /* More endianness concerns here, pray that they are right */
  memset(par->data, 0x00, par->size);

  if (S_ISREG(statinfo->st_mode))
    r |= set_word(par->data, TYPE_INDEX, FS_TYPE_REGULAR);
  else if (S_ISDIR(statinfo->st_mode))
    r |= set_word(par->data, TYPE_INDEX, FS_TYPE_DIR);

  r |= set_word(par->data, C_TIME_SEC_INDEX, statinfo->st_mtime);
  r |= set_word(par->data, C_TIME_USEC_INDEX, 0);

  mode = statinfo->st_mode & (S_IFMT | S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID);
  r |= set_word(par->data, MODE_INDEX, mode);

  r |= set_word(par->data, FILE_SIZE_INDEX, statinfo->st_size);

  len = MIN(FS_PATH_MAX_LENGTH, strlen(path));
  set_bytes(par->data, PATH_INDEX, len, (unsigned char *) path);

  len = MIN(FS_NAME_MAX_LENGTH, strlen(name));
  set_bytes(par->data, NAME_INDEX, len, (unsigned char *) name);

  if (S_ISREG(statinfo->st_mode))
    r |= set_word(par->data, DUNIT_INDEX, MAX_FILES + 1 + par->key);
  else
    r |= set_word(par->data, DUNIT_INDEX, 0xffffffff);

  r |= set_word(par->data, UNUSED_INDEX, 0xffffffff);

  if (r)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/* ========================================================================== */

/* path = /path/to/stuff(/), returns stuff */
void get_basename(const char *path, char *name)
{
  char *c, *tmp;

  tmp = strdup(path);

  /* remove trailing slash */
  if (tmp[strlen(tmp) - 1] == '/')
    tmp[strlen(tmp) - 1] = '\0';

  c = strrchr(tmp, '/');
  if (!c)
    name[0] = '\0';
  else
    strcpy(name, c+1);

  free(tmp);

}

/* ========================================================================== */

/* path = /path/to/stuff, returns /path/to/ */
void get_dirname(char *path, char *dir)
{
  char *c;

  strcpy(dir, path);

  /* remove trailing slash */
  if (dir[strlen(dir) - 1] == '/')
    dir[strlen(dir) - 1] = '\0';

  c = strrchr(dir, '/');
  if (!c)
    dir[0] = '\0';
  else
    *(c + 1) = '\0';

}

/* ========================================================================== */

int process_dirtree(struct cmd_line_options* cmd_line_options,
			char *realpath, char *path, char *subpath,
			struct state *state)
{
  int result = EXIT_FAILURE;
  struct dirent *dirent;
  char cwd[PATH_MAX];
  DIR *dir = NULL;
  struct stat statinfo;
  param_t *par = NULL;
  char name[PATH_MAX], dirname[PATH_MAX];
  int r;

  if (!realpath && !path && !state)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    goto error_exit;
  }

  if (strlen(path) > FS_PATH_MAX_LENGTH)
  {
    fprintf(stderr, ERRORSTR "Path too long: %s\n", path);
    goto error_exit;
  }

  if (!getcwd(cwd, PATH_MAX))
  {
    fprintf(stderr, ERRORSTR "Can not retrieve working directory.\n");
    goto error_exit;
  }

  if (stat(realpath, &statinfo))
  {
    fprintf(stderr, ERRORSTR "Can not retrieve input directory meta data about '%s'.\n", realpath);
    goto error_exit;
  }

  if (!S_ISDIR(statinfo.st_mode))
  {
    fprintf(stderr, ERRORSTR "Input directory '%s' is not a directory.\n", realpath);
    goto error_exit;
  }

  /* Add the current directory */
  get_basename(path, name);
  get_dirname(path, dirname);

  if (cmd_line_options->debug)
    printf("%s: Generating directory %s in %s\n", cmd_line_options->program, name, dirname);

  state->par = alloc_param(state->par, SIZE_OF_FILE_INFO);
  if (!state->par)
  {
    fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
    goto error_exit;
  }
  par = state->par;
  par->key = state->entries;

  /* Create entry without parent for root directory */
  if (!strlen(dirname))
    result = pack_fileinfo(par, "", "/", &statinfo);
  else
    result = pack_fileinfo(par, dirname, name, &statinfo);

  if (result != EXIT_SUCCESS)
    goto error_exit;

  if (++state->entries > MAX_FILES)
  {
    fprintf(stderr, ERRORSTR "Too many files and directories in dirtree, MAX_FILES = %d\n", MAX_FILES);
    goto error_exit;
  }

  /* If subpath, descend into it without searching all other
   * objects in the directory */
  if (subpath && strncmp(path, subpath, strlen(subpath))) {
    char *next;
    int i = 0;

    /* Find next path component */
    if (strlen(path) > strlen(subpath))
    {
      fprintf(stderr, ERRORSTR "Can not match subpath '%s' in path '%s'.\n", subpath, path);
      goto error_exit;
    }

    next = strdup(subpath + strlen(path));
    while (next[i] != '/' && next[i] != '\0')
      i++;

    next[i] = '\0';

    r = snprintf(dirname, PATH_MAX, "%s%s/", path, next);
    if (r < 0 || r >= PATH_MAX)
    {
      fprintf(stderr, ERRORSTR "Can not format directory path '%s' + '%s'\n", path, next);
      free(next);
      goto error_exit;
    }
    if (cmd_line_options->debug)
      printf("%s: Skipping into %s\n", cmd_line_options->program, dirname);

    if (chdir(realpath))
    {
      fprintf(stderr, ERRORSTR "Can not change into input path '%s'.\n", realpath);
      free(next);
      goto error_exit;
    }

    result = process_dirtree(cmd_line_options, next, dirname, subpath, state);
    free(next);

    if (result != EXIT_SUCCESS)
    {
      fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
      goto error_exit;
    }

    return EXIT_SUCCESS;
  }

  if (cmd_line_options->debug)
    printf("%s: Opening real directory %s (known as %s)\n", cmd_line_options->program, realpath, path);

  dir = opendir(realpath);
  if (!dir)
  {
    fprintf(stderr, ERRORSTR "Can not open input path '%s'.\n", realpath);
    goto error_exit;
  }

  if (chdir(realpath))
  {
    fprintf(stderr, ERRORSTR "Can not change into input path '%s'.\n", realpath);
    goto error_exit;
  }

  /* For each entry in dir, add corresponding entries to the linked list */
  do
  {
    struct dirent entry;

    if (readdir_r(dir, &entry, &dirent))
    {
      fprintf(stderr, ERRORSTR "Can not read input path directory entry '%s'.\n", realpath);
      goto error_exit;
    }

    /* End of directory encountered skip entry generation */
    if (dirent == NULL)
      continue;

    /* Ignore overlong file names */
    if (strlen(entry.d_name) > FS_NAME_MAX_LENGTH)
    {
      fprintf(stderr, ERRORSTR "File name too long: '%s'\n", entry.d_name);
      goto error_exit;
    }

    if (stat(entry.d_name, &statinfo))
    {
      fprintf(stderr, ERRORSTR "Can not retrieve input file ('%s') meta data.\n", entry.d_name);
      goto error_exit;
    }

    if (S_ISREG(statinfo.st_mode))
    {
      FILE *f = NULL;

      if (cmd_line_options->debug)
        printf("%s: Generating file %s (%s%s)\n", cmd_line_options->program, entry.d_name, path, entry.d_name);

      f = fopen(entry.d_name, "rb");
      if (!f)
      {
        fprintf(stderr, ERRORSTR "Could not open input file '%s'.\n", entry.d_name);
        goto error_exit;
      }

      if (!statinfo.st_size)
      {
        fprintf(stderr, ERRORSTR "Files with size 0 are not supported: '%s'.\n", entry.d_name);
        fclose(f);
        goto error_exit;
      }

      par = alloc_param(par, statinfo.st_size);
      if (!par)
      {
        fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
        fclose(f);
        goto error_exit;
      }

      par->key = MAX_FILES + 1 + state->entries;
      if (fread(par->data, 1, statinfo.st_size, f) < (size_t) statinfo.st_size && !feof(f))
      {
        fprintf(stderr, ERRORSTR "Could not read input file '%s'.\n", entry.d_name);
        fclose(f);
        goto error_exit;
      }

      if (fclose(f))
      {
        fprintf(stderr, ERRORSTR "Could not close input file '%s'.\n", entry.d_name);
        goto error_exit;
      }

      /* hook up a new param to the next pointer */
      par = alloc_param(par, SIZE_OF_FILE_INFO);
      if (!par)
      {
        fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
        goto error_exit;
      }

      /* the meta info of the parameter */
      par->key = state->entries;
      result = pack_fileinfo(par, path, entry.d_name, &statinfo);
      if (result != EXIT_SUCCESS)
      {
        fprintf(stderr, ERRORSTR "Failed to pack NWM file info for '%s'.\n", entry.d_name);
        goto error_exit;
      }

      if (++state->entries > MAX_FILES)
      {
        fprintf(stderr, ERRORSTR "Too many files and directories in dirtree, MAX_FILES = %d\n", MAX_FILES);
        goto error_exit;
      }
    }
    else if (S_ISDIR(statinfo.st_mode))
    {
      /* Skip dot and dotdot directory entries */
      if (!strcmp(entry.d_name, ".") || !strcmp(entry.d_name, ".."))
        continue;

      r = snprintf(dirname, PATH_MAX, "%s%s/", path, entry.d_name);
      if (r < 0 || r >= PATH_MAX)
      {
        fprintf(stderr, ERRORSTR "Can not format directory path '%s' + '%s'\n", path, entry.d_name);
        goto error_exit;
      }

      result = process_dirtree(cmd_line_options, entry.d_name, dirname, subpath, state);
      if (result != EXIT_SUCCESS)
      {
        fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
        goto error_exit;
      }
    }
    /* Ignore any non-file, non-directory entries */
    else
    {
      fprintf(stderr, ERRORSTR "Unsupported directory entry: %s/%s\n", realpath, entry.d_name);
      goto error_exit;
    }

  } while (dirent != NULL);

  if (closedir(dir))
  {
    fprintf(stderr, ERRORSTR "Can not close input path '%s'.\n", realpath);
    dir = NULL;
    goto error_exit;
  }

  if (chdir(cwd))
  {
    fprintf(stderr, ERRORSTR "Can not change into previous working directory '%s'.\n", cwd);
    dir = NULL;
    goto error_exit;
  }

  state->par = rewind_param(state->par);

  return EXIT_SUCCESS;

error_exit:
  if(dir)
    closedir(dir);
  if (state->par)
  {
    free_param(state->par);
    state->par = NULL;
  }
  return EXIT_FAILURE;
}
