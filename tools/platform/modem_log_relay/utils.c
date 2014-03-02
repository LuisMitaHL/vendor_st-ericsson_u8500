/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "utils.h"

static char filter_dir_path[NAME_LENGTH];
static char filter_string[NAME_LENGTH];
static int  filter_filetype;

struct tm *get_time_stamp() {
    time_t epoch;

    /* get date and time */
    epoch = time(NULL);
    return localtime(&epoch);
}

/*
 * delete_dir
 *
 * delete a directory even if not empty
 * input  : directory parh
 * output : 0 OK
 *          other value on errors
 */
int delete_dir(char *dir_path)
{
    int    nb_file, ret;
    struct dirent **filelist;
    char file_path[NAME_LENGTH];

    /* scan directory and make file list */
    nb_file = scandir(dir_path, &filelist, 0, 0);

    if (nb_file < 0) {
        ALOGW("scandir error while deleting directory");
    } else {
        EXTRADEBUG("found %d file in %s", \
                   nb_file, dir_path);

        while (nb_file--) {

            if ((strcmp(filelist[nb_file]->d_name, ".") == 0) ||
                    (strcmp(filelist[nb_file]->d_name, "..") \
                     == 0)) {
                free(filelist[nb_file]);
                continue;
            }

            memset(file_path, 0, NAME_LENGTH);
            snprintf(file_path, NAME_LENGTH, "%s/%s",
                     dir_path,
                     filelist[nb_file]->d_name);
            EXTRADEBUG("delete file %s (%s)",
                       file_path,
                       filelist[nb_file]->d_name);

            if (remove(file_path) != 0)
                EXTRADEBUG("can't delete file %s (%s)",
                           file_path,
                           filelist[nb_file]->d_name);

            free(filelist[nb_file]);
        }

        free(filelist);
    }

    ret = remove(dir_path);

    if (ret != 0) {
        ALOGW("can't delete directory %s)", dir_path);
    }

    return ret;
}

/*
* filecompare
*
* check the latest core file name
* input  : none
* output : 0 new system coredump generated
*          other value no system coredump
*/
int filecompare(const struct dirent **first_file, const struct dirent **second_file)
{
    int ret = 0;
    ret = strcmp((*second_file)->d_name, (*first_file)->d_name);
    return ret;
}

/*
* dir_entry_dump_filter
*
* check if a directory entry is a directory
*
* input  : *dirent
* output : 0 if dir entry name match prefix
*          -1 if not
*/
int dir_entry_dump_filter(const struct dirent *entry)
{
    /* check if directory prefix is mcd__ */
    if ((entry->d_type == DT_DIR) && (strstr(entry->d_name, PREFIX_MODEM_COREDUMP) != 0)) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * \fn int Create_dir(char *path)
 * \brief Create an unexisted directory
 * \special case : if path is composed by several subdirectory,
 *                 and doesn't exist, create them
 *
 * \param path: path to create directory
 * \return -1 on error, 0 if ok
 */

int Create_dir(char *path)
{
    int ret = 0;
    char *pos;
    char temp_dir[PATH_LENGTH];
    struct stat path_stat;

    pos = strchr(path, '/');

    while ((pos != NULL) && (ret == 0)) {
        memset(temp_dir, '\0', PATH_LENGTH);
        strncpy(temp_dir, path, pos - path + 1);
        temp_dir[pos - path + 1] = '\0';

        if ((stat(temp_dir, &path_stat) != 0) &&
                S_ISDIR(path_stat.st_mode)) {
            ret = mkdir(temp_dir, 0777);
        }

        pos = strchr(pos + 1, '/');
    }

    /* create last sub dir */
    if (!pos) {
        if ((stat(path, &path_stat) != 0) &&
                S_ISDIR(path_stat.st_mode)) {
            ret = mkdir(path, 0777);
        }
    }

    if (ret) {
        ALOGE("Failed to create directory %s", path);
    }

    return ret;
}

/**
 * get_dump_file_prefix
 *
 * Removes the file-ending and sets the coredump prefix.
 * For instance, if dumpfilename is cdump_3.elf,
 * prefix will be cdump_3
 *
 * input: dumpfilename - name of the file for which to get the prefix.
 *        prefix - the dumpfilename without file ending.
 * output: 0 on success
 *        -1 on error
 *
 */
int get_dump_file_prefix(char *dumpfilename, char *prefix)
{
    char *pos;
    int prefix_length = 0;

    pos = strrchr(dumpfilename, '.');

    if (pos != NULL) {
        prefix_length = strlen(dumpfilename) - strlen(pos);
        strncpy(prefix, dumpfilename, prefix_length);
        /*Make sure the prefix string gets null terminated*/
        prefix[prefix_length] = '\0';
        return 0;
    }

    return -1;
}

/*
* dir_entry_filter
*
* check if a directory entry name start with prefix
* mcd_ , scd_ or cdump
*
* input  : *dirent
* output : 1 if dir entry name match prefix
*          0 if not
*/
int dir_entry_filter(const struct dirent *entry)
{
    /* check if file prefix is mcd_ or scd_ */
    if ((strncmp(entry->d_name, PREFIX_MODEM_COREDUMP,
                 strlen(PREFIX_MODEM_COREDUMP)) == 0) ||
            (strncmp(entry->d_name, PREFIX_SYSTEM_COREDUMP,
                     strlen(PREFIX_SYSTEM_COREDUMP)) == 0) ||
            (strncmp(entry->d_name, PREFIX_KERNEL_COREDUMP,
                     strlen(PREFIX_KERNEL_COREDUMP)) == 0)) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief Converts a 16-bit value from network to host byte order.
 *
 * @param [in] val 16-bit integer value.
 *
 * @return Returns 16-bit interger converted value from network to
 *         host byte order.
 */
uint16_t get_uint16(uint16_t val)
{
    if (get_endian() == BIGENDIAN) {
        return val;
    }

    return (val >> 8) | (val << 8);
}

/**
 * @brief Swaps the endinaness of a 32-bit value.
 */
uint32_t swap_endian_32(uint32_t val)
{
    return ((val & 0x000000ff) << 24) +
           ((val & 0x0000ff00) << 8) +
           ((val & 0x00ff0000) >> 8) +
           ((val & 0xff000000) >> 24);
}

/*
* filecompare
*
* check the latest file name based on modified date
* input  : none
* output : 0 new system coredump generated
*          other value no system coredump
*/
int filecompare_latest(const struct dirent **first_file, const struct dirent **second_file)
{
    char first_abs_path[NAME_LENGTH];
    char second_abs_path[NAME_LENGTH];
    struct stat first_stat;
    struct stat second_stat;

    memset(first_abs_path, 0x00, NAME_LENGTH);
    memset(second_abs_path, 0x00, NAME_LENGTH);

    snprintf(first_abs_path, NAME_LENGTH, "%s/%s", filter_dir_path, (*first_file)->d_name);
    snprintf(second_abs_path, NAME_LENGTH, "%s/%s", filter_dir_path, (*second_file)->d_name);

    if ((stat(first_abs_path, &first_stat) == -1) || (stat(second_abs_path, &second_stat) == -1)) {
        return 0;
    }

    // Check the last modified time
    if (first_stat.st_mtime == second_stat.st_mtime) {
        return 0;
    } else if (first_stat.st_mtime < second_stat.st_mtime) {
        return 1;
    } else if (first_stat.st_mtime > second_stat.st_mtime) {
        return -1;
    }

    return 0;
}

/*
* void set_predefine_path_filecompare_latest(const char *path)
*
* Set the filter parameter to be used in scandir for latest file
*
* input  : char* directory path to be used in scandir
*/
void set_predefine_path_filecompare_latest(const char *path)
{
    if (path != NULL) {
        memset(filter_dir_path, 0, NAME_LENGTH);
        snprintf(filter_dir_path, NAME_LENGTH, "%s", path);
        EXTRADEBUG("Scan directory predefined directory string : %s", filter_dir_path);
    }
}

/*
* void set_filter_string(int filetype, const char *filter)
*
* Set the filter parameter to be used in scandir
*
* input  : int  File type (ex. DT_REG, DT_DIR)
*        : char* Filter string to be used in scandir
*/
void set_filter_string(int filetype, const char *filter)
{
    if (filter != NULL) {
        memset(filter_string, 0, NAME_LENGTH);
        snprintf(filter_string, NAME_LENGTH, "%s", filter);
        EXTRADEBUG("Scan directory filter string : %s", filter);
        filter_filetype = filetype;
    }
}

/*
* int  scandir_filter(const struct dirent *entry)
*
* Filter to be used in scandir
*
* input  : const struct dirent  Dirent for each entry
*
* Return : 1, if match found
*          0, if not matched
*/
int  scandir_filter(const struct dirent *entry)
{
    if ((entry->d_type == filter_filetype) && (filter_string != NULL) &&
            (strstr(entry->d_name, filter_string) != 0)) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief Returns the byte-order of the current machine
 *
 * @return 0 if little endien, 1 if big endien
 */
int get_endian(void)
{
    short int word = 0x0001;
    char *byte = (char *)&word;
    return (byte[0] ? LITTLEENDIAN : BIGENDIAN);
}

