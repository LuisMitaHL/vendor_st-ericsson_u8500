/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <sys/statfs.h>
#include <linux/mloader.h>
#include <linux/db8500-modem-trace.h>

#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>

#include "coredump.h"
#include "kerneldump.h"
#include "bass_app.h"
#include "xfiledecoder.h"


static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static int remove_old_trigger_report(const char *directory_name, int is_predefine_dir);

/*
 * coredump_create_archive
 *
 * create the tar.gz archive containing all coredump files
 * input  : pointer on directory name, pointer on archive name
 * output : 0 OK
 *          other value on errors
 */

int coredump_create_archive(char *directory_name, char *archive_name, struct tm *timestamp)
{
    struct tm *time = timestamp;
    int    ret;
    char cmd_line[CMD_LINE_LENGTH + 1];
    char temp_dir[NAME_LENGTH + 1];
    char new_archive_name[NAME_LENGTH + 1];
    char file_path[NAME_LENGTH + 1];
    char time_stamp[NAME_LENGTH + 1];

    /* create timestamp */
    memset(time_stamp, 0, TIMESTAMP_LENGTH);
    snprintf(time_stamp, TIMESTAMP_LENGTH, "%04d-%02d-%02d_%02dh%02dm%02d",
             1900 + time->tm_year,
             time->tm_mon + 1,
             time->tm_mday,
             time->tm_hour,
             time->tm_min,
             time->tm_sec);



    /* Create dump information if modem core dump */
    if (strcmp(archive_name, PREFIX_MODEM_COREDUMP) == 0) {
        // Create /data/xfiles folder to store x-files
        Create_dir(XFILE_DIRECTORY);

        // Get modem shared path
        memset(temp_dir, 0, NAME_LENGTH);
        snprintf(temp_dir, NAME_LENGTH, "%s/%s/%s", TEMP_PATH, TEMP_DIR, MODEM_SHM_DATA);

        memset(file_path, 0, NAME_LENGTH);

        /* If core dump location is USB set dumpinfo.txt directory to default directory on SDcard.*/
        if (strncmp(directory_name, TEMP_PATH, 4) != 0) {
            snprintf(file_path, NAME_LENGTH, "%s/%s%s.txt", directory_name, PREFIX_COREDUMP_INFO, time_stamp);
        } else {
            snprintf(file_path, NAME_LENGTH, "%s/%s%s.txt", DEFAULT_MODEM_DUMP, PREFIX_COREDUMP_INFO, time_stamp);
        }

        ALOGI("Dump info name %s", file_path);
        ret = generate_dump_info(temp_dir, file_path);

        if (ret != 0) {
            ALOGE("Can't create dump info text file");
        }

        // Create x-file from modem shared
        memset(file_path, 0, NAME_LENGTH);
        snprintf(file_path, NAME_LENGTH, "%s%s/%s%s%s", TEMP_PATH, TEMP_DIR, "xfile_", time_stamp, ".mxf");
        ret = generate_xfile(temp_dir, file_path);

        if (ret == 0) {
            // Copy generated x-file to file system
            memset(cmd_line, 0, CMD_LINE_LENGTH);
            snprintf(cmd_line, CMD_LINE_LENGTH, "cat %s >  %s/%s%s%s", file_path, XFILE_DIRECTORY, "xfile_", time_stamp, ".mxf");
            ret = system(cmd_line);

            if (ret != 0) {
                ALOGE("Unable to copy xfile on filesystem");
            }
        }
    }


    /* add time stamp to prefix archive name */
    strncat(archive_name, time_stamp , strlen(time_stamp));
    strncat(archive_name, ".tar.gz" , strlen(".tar.gz"));

    ALOGE("Coredump archive name %s", archive_name);

    memset(temp_dir, 0, NAME_LENGTH);
    snprintf(temp_dir, NAME_LENGTH, "%s%s", TEMP_PATH, TEMP_DIR);
    ret = chdir(temp_dir);

    if (ret != 0) {
        ALOGE("Can't change directory to %s", temp_dir);
        return ret;
    }

    memset(new_archive_name, 0, NAME_LENGTH);
    snprintf(new_archive_name, NAME_LENGTH, "%s/%s", directory_name, archive_name);
    memset(cmd_line, 0, CMD_LINE_LENGTH);
    snprintf(cmd_line, CMD_LINE_LENGTH, "bsdtar -c -z -f %s *", new_archive_name);
    ret = system(cmd_line);

    if (ret != 0) {
        ALOGE("Can't create coredump archive file");
        return ret;
    }

    ret = chdir("/");

    if (ret != 0) {
        ALOGE("Can't change directory to root");
    }

    return ret;
}

/*
 * coredump_write_secure_modem_mem
 *
 * dump modem TCM L1 and TCM L2 memory
 * along with PLL registers
 *
 * input  : pointer on directory name
 * output : 0 OK
 *          other value on errors
 */

int coredump_write_secure_modem_mem(char *directory_name)
{
    int fd, ret = 0;
    size_t idx;
    uint8_t *secure_modem_mem_buf = NULL;
    char file_name[NAME_LENGTH];
    ssize_t count;
    struct secure_modem_mem_dump secure_modem_mem[] = {
        { TCM_L1, "TCM_L1", TCM_L1_MAX_SIZE},
        { TCM_L2, "TCM_L2", TCM_L2_MAX_SIZE},
        { GET_REGISTERS, "PLL", GET_REGISTERS_SIZE},
        { MODEM_SRAM, "modem_sram", MODEM_SRAM_SIZE}
    };

    for (idx = 0; idx < ARRAY_SIZE(secure_modem_mem); idx++) {

        /* allocate buffer */
        secure_modem_mem_buf = malloc(sizeof(uint8_t) * secure_modem_mem[idx].size);

        if (secure_modem_mem_buf == NULL) {
            ALOGD("Failed to allocate memory for secure modem memory image %s", secure_modem_mem[idx].name);
            break;
        }

        /* dump modem secure memory*/
        ret = smcl_get_modem_memory(secure_modem_mem[idx].mem_id,
                                    secure_modem_mem_buf,
                                    secure_modem_mem[idx].size);

        if (ret == SMCL_OK) {
            snprintf(file_name, NAME_LENGTH, "%s/%s",
                     directory_name, secure_modem_mem[idx].name);
            fd = open(file_name, O_CREAT | O_RDWR);

            if (fd < 0) {
                ALOGD("Can't create %s", file_name);
                ret = errno;
                goto out;
            }

            count = write(fd, secure_modem_mem_buf, secure_modem_mem[idx].size);

            if ((unsigned int)count != secure_modem_mem[idx].size) {
                ALOGD("Failed to write to file %s ,only (%d/%d) copied", file_name, (int)count, secure_modem_mem[idx].size);
                close(fd);
                ret = errno;
                goto out;
            }

            close(fd);

            ALOGI("Successfully written secure modem memory image %s of size %d",
                  secure_modem_mem[idx].name,
                  secure_modem_mem[idx].size);
        } else {
            ALOGD("Failed to dump secure modem memory image %s with return code (%d)",
                  secure_modem_mem[idx].name, ret);
        }

        free(secure_modem_mem_buf);
        secure_modem_mem_buf = NULL;
    }

out:

    if (secure_modem_mem_buf != NULL) {
        free(secure_modem_mem_buf);
        secure_modem_mem_buf = NULL;
    }

    return ret;
}


/*
 * coredump_write_modem_data
 *
 * dump modem share memory, modem private memory
 * in separate files
 *
 * input  : pointer on directory name
 * output : 0 OK
 *          other value on errors
 */

int coredump_write_modem_data(char *directory_name)
{
    int fd_mloader, fd_out, image_nb, ret, return_value = 0;
    char file_name[NAME_LENGTH];
    struct dump_image *image_desc;
    char *die_id;
    int image_size = 0;
    int i;
    ssize_t count;
    void *image_dump;

    /* get number of image to dump from mloader_fw driver */
    fd_mloader = open(MLOADER_FW_DRV, O_RDONLY);

    if (fd_mloader == -1) {
        ALOGE("Can't open %s device\n", MLOADER_FW_DRV);
        return errno;
    }

    ret = ioctl(fd_mloader, ML_GET_NBIMAGES, &image_nb);

    if (ret != 0) {
        ALOGE("Error on ioctl ML_GET_NBIMAGES");
        close(fd_mloader);
        return errno;
    }

    ALOGI("ML_GET_NBIMAGES = %d", image_nb);

    /* get image descriptors from mloader_fw driver */
    image_desc = malloc(sizeof(struct dump_image) * image_nb);

    if (image_desc == NULL) {
        ALOGE("Failed to allocate memory for image_desc");
        close(fd_mloader);
        return errno;
    }

    ret = ioctl(fd_mloader, ML_GET_DUMPINFO, image_desc);

    if (ret != 0) {
        ALOGE("ioctl error on ML_GET_DUMPINFO");
        return_value = errno;
        goto out1;
    }

    for (i = 0; i < image_nb ; i++) {
        image_size += image_desc[i].size;
        EXTRADEBUG("name = %s offset = %d size = %d",
                   image_desc[i].name,
                   image_desc[i].offset,
                   image_desc[i].size);
    }

    image_dump = mmap(0, image_size, PROT_READ, MAP_SHARED, fd_mloader, 0);

    if (image_dump == MAP_FAILED) {
        ALOGE("mmap error : Failed to map memory for modem shared memory");
        return_value = errno;
        goto out1;
    }

    for (i = 0; i < image_nb ; i++) {
        snprintf(file_name, NAME_LENGTH, "%s/%s", directory_name, image_desc[i].name);
        EXTRADEBUG("Writing file: %s", file_name);
        fd_out = open(file_name, O_CREAT | O_RDWR);

        if (fd_out < 0) {
            ALOGE("Can't create file %s for modem memory dump", file_name);
            return_value = errno;
            goto out2;
        }

        count = write(fd_out,
                      (void *)((int)image_dump + image_desc[i].offset),
                      image_desc[i].size);

        if ((unsigned int)count != image_desc[i].size) {
            ALOGD("Failed to write to file %s, only (%d/%d) copied from modem memory dump",
                  file_name, (int)count, image_desc[i].size);
            close(fd_out);
            return_value = errno;
            goto out2;
        }

        close(fd_out);
    }

    die_id = malloc(sizeof(char) * 13);

    if (die_id == NULL) {
        ALOGE("Failed to allocate memory for die_id");
        goto out2;
    }

    ret = ioctl(fd_mloader, ML_GET_FUSEINFO, die_id);

    if (ret != 0) {
        ALOGE("ioctl error on ML_GET_FUSEINFO");
        return_value = errno;
        free(die_id);
        goto out2;
    }

    char buff[25];

    for (i = 0; i < 12; i++) {
        sprintf(buff + i * 2, "%02X", (0xFF & die_id[i]));
    }

    free(die_id);
    buff[24] = '\0';

    snprintf(file_name, NAME_LENGTH, "%s/DIE_ID_%s", directory_name, buff);
    EXTRADEBUG("Writing file: %s", file_name);
    fd_out = open(file_name, O_CREAT | O_RDWR);

    if (fd_out < 0) {
        ALOGE("Can't create file %s for modem memory dump", file_name);
        return_value = errno;
        goto out2;
    }

    close(fd_out);
    EXTRADEBUG("coredump_write_modem_data done");

out2:
    ret = munmap(image_dump, image_size);

    if (ret != 0) {
        ALOGE("munmap error for modem memory dump");
        return_value = ret;
    }

out1:
    close(fd_mloader);
    free(image_desc);
    return return_value;
}

/*
 * coredump_write_trace_data
 *
 * dump modem trace data in a file
 *
 * input  : pointer on directory name
 * input  : pointer on file name
 * output : 0 OK
 *          other value on errors
 */

int coredump_write_trace_data(char *directory_name, char *filename)
{

    int fd_trace_modem, fd_out, ret, return_value = 0;
    char file_name[NAME_LENGTH];
    int trace_size = 0;
    ssize_t count;
    void *trace_dump;


    /* get size of trace from trace_modem driver */
    fd_trace_modem = open(TRACE_MODEM_DRV, O_RDONLY);

    if (fd_trace_modem == -1) {
        ALOGE("Can't open %s device", TRACE_MODEM_DRV);
        return errno;
    }

    ret = ioctl(fd_trace_modem, TM_GET_DUMPINFO, &trace_size);

    if (ret != 0) {
        ALOGE("Error on ioctl TM_GET_DUMPINFO");
        close(fd_trace_modem);
        return errno;
    }

    ALOGI("TM_GET_DUMPINFO = %d", trace_size);

    trace_dump = mmap(NULL, trace_size, PROT_READ, MAP_SHARED,
                      fd_trace_modem, 0);

    if (trace_dump == MAP_FAILED) {
        ALOGE("mmap error for modem trace data");
        close(fd_trace_modem);
        return errno;
    }

    snprintf(file_name, NAME_LENGTH, "%s/%s", directory_name, filename);
    fd_out = open(file_name, O_CREAT | O_RDWR);

    if (fd_out < 0) {
        ALOGE("Can't create %s for modem trace data", file_name);
        return_value = errno;
        goto out;
    }

    count = write(fd_out, trace_dump, trace_size);

    if (count != trace_size) {
        ALOGE("Failed to write to file %s, only (%d/%d) copied for modem trace data",
              file_name, (int)count, trace_size);
        close(fd_out);
        return_value = ret;
    } else {
        ALOGI("coredump_write_trace_data done %s", file_name);
        close(fd_out);
    }

out:
    ret = munmap((void *) trace_dump, trace_size);

    if (ret != 0) {
        ALOGE("Error on munmap for modem trace data");
        return_value = ret;
    }

    close(fd_trace_modem);
    return return_value;
}

/*
 * coredump_write_trace_buffer
 *
 * make a snapshot of current circular trace buffer
 * input  : pointer on directory name
 * output : 0 OK
 *          other value on errors
 */

int coredump_write_trace_buffer(char *dest_directory, \
                                char *trace_filename_prefix)
{
    int ret = 0;
    int i, pos;
    char complete_file_name[NAME_LENGTH];
    char src_file_path[NAME_LENGTH];
    char dest_file_path[NAME_LENGTH];

    if (trace_filename_prefix == NULL) {
        ALOGI("No filename, trace evacuated through USB");
        return 0;
    }

    for (i = 0; i < MLR_MAX_EVAC_FILE; i++) {
        memset(complete_file_name, 0, NAME_LENGTH);
        memset(src_file_path, 0, NAME_LENGTH);

        snprintf(src_file_path, NAME_LENGTH, "%s%s", trace_filename_prefix, filename_suffix[i]);
        EXTRADEBUG("complete src file name = %s", src_file_path);

        /* extract only the filename from trace_filename_prefix */
        pos = strrchr(src_file_path, '/') - src_file_path + 1;

        strncpy(complete_file_name, src_file_path + pos, strlen(src_file_path) - pos + 1);

        memset(dest_file_path, 0, NAME_LENGTH);
        snprintf(dest_file_path, NAME_LENGTH, "%s/%s", dest_directory, complete_file_name);
        EXTRADEBUG("complete dst file name = %s", dest_file_path);

        // Move file
        ret = rename(src_file_path, dest_file_path);
    }

    return ret;
}

/*
 * int coredump_write_debugfs_dump_data(char *dest_directory, char *filename, char *debugfs_node)
 *
 * Copy the specified debugfs node dump content to coredump
 *
 * input  : Destination directory path, Coredump file name, debugfs node
 * output : 0 on success
 *          other value on errors
 */

int coredump_write_debugfs_dump_data(char *dest_directory, char *filename, char *debugfs_node)
{
    int ret = 0;
    int fd_register_rd, fd_register_wr;
    char file_path[NAME_LENGTH];
    char dump_info[NAME_LENGTH];
    char command[CMD_LINE_LENGTH];

    // Return if the destination path is wrong
    if ((dest_directory == NULL) || (filename == NULL) || (debugfs_node == NULL)) {
        return -1;
    }

    snprintf(dump_info, NAME_LENGTH, "%s", debugfs_node);
    // open file to read the specified debugfs node
    fd_register_rd = open(dump_info, O_RDONLY);

    if (fd_register_rd == -1) {
        ALOGE("Can't open file %s to read dump info", dump_info);
        return errno;
    }

    close(fd_register_rd);

    // open file to write the dump of specified debugfs node
    snprintf(file_path, NAME_LENGTH, "%s/%s", dest_directory, filename);
    fd_register_wr = open(file_path, O_CREAT | O_RDWR);

    if (fd_register_wr == -1) {
        ALOGE("Can't open file %s to write dump info", file_path);
        return errno;
    }

    close(fd_register_wr);

    // Execute the command to write specified debugfs node file content
    // to coredump file
    memset(command, 0, CMD_LINE_LENGTH);
    snprintf(command, CMD_LINE_LENGTH, "cat %s >  %s", dump_info, file_path);
    ret = system(command);

    if (ret != 0) {
        ALOGE("Unable to process dump on node %s.", dump_info);
        return ret;
    }

    ALOGI("%s debugfs node written to core dump successfully", dump_info);

    return 0;
}

/*
 * check_available_space
 *
 * check available space in specified directory,
 * If number of dumps exceeds user selected value or space is missing
 * delete oldest file.
 * input  : pointer on directory name
 * output : 0 OK
 *          other value on errors
 */
int check_available_space(char *directory_name)
{
    char old_file[PATH_LENGTH] = "\0";
    int nr_of_dumps = 0;
    long unused_space = 0;
    struct dirent **filelist;
    int nb_file;
    struct statfs data;

    /* scan directory    */
    nb_file = scandir(directory_name, &filelist, dir_entry_dump_filter, filecompare);

    if (nb_file < 0) {
        ALOGE("Failed to scan core dump directory");
        return 1;
    } else if (nb_file == 0) {
        nr_of_dumps = 0;
        ALOGI("No dump files available.");
    } else if (nb_file >= dump_files && dump_files != 0) {
        int i;

        for (i = 0; i < nb_file; i++) {
            char path[PATH_LENGTH];
            // get the complete path for the file
            snprintf(path, PATH_LENGTH, "%s/%s", directory_name, filelist[i]->d_name);

            EXTRADEBUG("Dump folder Name: %s", filelist[i]->d_name);

            if ((i + 1) >= dump_files) {
                ALOGI("Deleting core dump %s since max number of dumps exceeded", path);
                delete_dir(path);
            }

            free(filelist[i]);
        }

        free(filelist);
    } else {
        snprintf(old_file, PATH_LENGTH, "%s/%s", directory_name, filelist[nb_file-1]->d_name);

        // Check available disk space on SDcard
        if ((statfs(directory_name, &data)) < 0) {
            ALOGE("Failed to stat directory : %s", directory_name);
        } else {
            unused_space = data.f_bfree * data.f_bsize;
        }

        EXTRADEBUG("Available space %ld", unused_space);

        if (unused_space < CORE_DUMP_DATA_LENGTH) {
            ALOGI("Not enough space available. Deleting oldest core dump %s", old_file);
            delete_dir(old_file);
        }

        while (nb_file > 0) {
            free(filelist[nb_file-1]);
            nb_file--;
        }

        free(filelist);
    }

    return 0;
}

/**
 * \fn int my_write(int fd, void const *buffer, size_t length)
 * \brief
 *
 * \param
 * \return
 */
static int my_write(int fd, void const *buffer, size_t length)
{
    size_t     rlen = length;
    size_t     max_len = 4096;
    size_t     clen;
    char const *ptr = buffer;

    EXTRADEBUG("IN : %d bytes to write on fd %d", length, fd);

    while (rlen > 0) {

        clen = rlen;

        if (rlen > max_len) {
            clen = max_len;
        }

        ssize_t wsize = write(fd, ptr, clen);

        if (wsize == -1) {
            ALOGW("wsize = -1 !!!");
            ALOGW("modemlogrelay:: my_write write error, errno = %d", errno);
            return -1;
        }

        ptr += wsize;
        rlen -= wsize;
    }

    EXTRADEBUG("OUT : %d bytes written on fd %d", length, fd);

    return 0;
}

/**
 * \fn int write_to_socket(char *path)
 * \brief
 *
 * \param
 * \return
 */
static int write_file_to_socket(char *temp_path, char *filename)
{
    char tmp_dump_path[NAME_LENGTH];
    char filenameBuffer[NAME_LENGTH];
    char sizeBuffer[NAME_LENGTH];
    FILE *dump_file;
    char *buffer = NULL;
    long dump_len;

    memset(tmp_dump_path, 0, NAME_LENGTH);
    snprintf(tmp_dump_path, NAME_LENGTH, "%s/%s", temp_path, filename);

    dump_file = fopen(tmp_dump_path, "rb");

    if (!dump_file) {
        ALOGE("Unable to open file %s", tmp_dump_path);
        return -1;
    }

    fseek(dump_file, 0, SEEK_END);
    dump_len = ftell(dump_file);

    if (dump_len < 0) {
        ALOGE("Unable to get the size of the dump_file %s to write to socket", tmp_dump_path);
        fclose(dump_file);
        return -1;
    }

    fseek(dump_file, 0, SEEK_SET);

    //Allocate memory
    buffer = (char *)malloc(dump_len + 1);

    if (!buffer) {
        fclose(dump_file);
        return -1;
    }

    fread(buffer, dump_len, 1, dump_file);
    fclose(dump_file);

    //write filename
    memset(filenameBuffer, 0x00, NAME_LENGTH);
    sprintf(filenameBuffer, "%s\r\n", filename);
    EXTRADEBUG("write_file_to_socket: Name : %s (strlen %d)", filenameBuffer, strlen(filenameBuffer));

    if (my_write(dump_fd, filenameBuffer, strlen(filenameBuffer))) {
        free(buffer);
        return -1;
    }

    //write filesize
    memset(sizeBuffer, 0x00, NAME_LENGTH);
    sprintf(sizeBuffer, "%ld\r\n", dump_len);
    EXTRADEBUG("write_file_to_socket: Size : %s (strlen %d)", sizeBuffer, strlen(sizeBuffer));

    if (my_write(dump_fd, sizeBuffer, strlen(sizeBuffer))) {
        free(buffer);
        return -1;
    }

    //write dump
    if (my_write(dump_fd, buffer, dump_len)) {
        free(buffer);
        return -1;
    }

    // Sleep to ensure file is transfered before MSUP kills target
    sleep(1);

    free(buffer);
    return 0;
}

/*
 * Remove old trigger reports
 *
 * If number of trigger report exceeds maximum, delete oldest reports.
 *
 * input  :  pointer on directory name
 * output :  0  on success
 *          -1 on failure
 */
int remove_old_trigger_report(const char *directory_name, int is_predefine_dir)
{
    int total_no_triggers, idx;
    char path[PATH_LENGTH];
    struct dirent **filelist;

    // Set filter for scandir
    set_predefine_path_filecompare_latest(directory_name);

    if (is_predefine_dir) {
        set_filter_string(DT_REG, PREFIX_REPORT);
    } else {
        set_filter_string(DT_DIR, PREFIX_REPORT);
    }

    EXTRADEBUG("remove_old_trigger_report : %s, is_predefine_dir=%d", directory_name, is_predefine_dir);
    // scan directory for trigger reports
    total_no_triggers = scandir(directory_name, &filelist, scandir_filter, filecompare_latest);

    if (total_no_triggers < 0) {
        ALOGE("Failed to scan trigger directory");
        return -1;
    } else if (total_no_triggers == 0) {
        ALOGI("No trigger report available.");
    } else if ((trigger_files > 0) && (total_no_triggers >= trigger_files)) {
        for (idx = 0; idx < total_no_triggers; idx++) {

            if (idx < trigger_files - 1) {
                free(filelist[idx]);
                continue;
            }

            // get the complete path for the file
            snprintf(path, PATH_LENGTH, "%s/%s", directory_name, filelist[idx]->d_name);

            if (filelist[idx]->d_type == DT_DIR) {
                delete_dir(path);
            } else {
                remove(path);
            }

            ALOGI("Removed trigger report : %s", path);

            free(filelist[idx]);
        }

        free(filelist);
    }

    return 0;
}

/*
 * report_generation
 *
 * write a snapshot of modem trace and circular
 * trace buffer
 * input  : none
 * output : 0 OK
 *          other value on errors
 */
int report_generation(int fd_dump, char *report_path, char *trace_filename,
                      char *report_name, char *modem_version)
{
    int  ret;
    char directory_name[NAME_LENGTH];
    char archive_name[NAME_LENGTH];
    char report_dir[NAME_LENGTH];
    struct tm *time;

    pthread_mutex_lock(&mutex);

    dump_fd = fd_dump;

    /* build the report temp directory in TEMP_PATH/coredump_temp */
    snprintf(directory_name, NAME_LENGTH, "%s%s", TEMP_PATH, TEMP_DIR);
    ret = Create_dir(directory_name);

    if (ret != 0) {
        ALOGE("can't create %s directory", directory_name);
        goto out;
    }

    ret = coredump_write_trace_data(directory_name, "modem_trace");

    if (ret != 0) {
        goto out;
    }

    time = get_time_stamp();
    memset(report_dir, 0, NAME_LENGTH);

    if (trace_filename != NULL) {
        /* create timestamp */
        char time_stamp[TIMESTAMP_LENGTH];
        snprintf(time_stamp, TIMESTAMP_LENGTH, "%04d%02d%02d%02d%02d%02d",
                 1900 + time->tm_year,
                 time->tm_mon + 1,
                 time->tm_mday,
                 time->tm_hour,
                 time->tm_min,
                 time->tm_sec);

        int pos;
        pos = strrchr(trace_filename, '/') - trace_filename;
        strncpy(report_dir, trace_filename, pos + 1);

        // Remove old trigger report directories before creating report directory
        remove_old_trigger_report(report_dir, 0);

        strncat(report_dir, PREFIX_REPORT, strlen(PREFIX_REPORT));
        strncat(report_dir, time_stamp, strlen(time_stamp));
        Create_dir(report_dir);
        coredump_write_trace_buffer(report_dir, trace_filename);
    } else {
        strncpy(report_dir, report_path, NAME_LENGTH);
        report_dir[NAME_LENGTH-1] = '\0';

        // Remove old trigger report files for non-USB case
        if (fd_dump == 0) {
            remove_old_trigger_report(report_dir, 1);
        }

        ALOGI("No filename, trace evacuated through USB or SDcard logging not enabled");
    }

    snprintf(archive_name, NAME_LENGTH, "%s%s%s_", PREFIX_REPORT, report_name,
             modem_version);

    if (dump_fd > 0) {
        ret = coredump_create_archive(directory_name, archive_name, time);

        if (ret != 0) {
            goto out;
        }

        ret = write_file_to_socket(directory_name, archive_name);
    } else {
        ret = coredump_create_archive(report_dir, archive_name, time);
    }

    // Delete the status file to indicate that log is
    // completed successfully
    if (remove(log_complete_status) != 0) {
        ALOGW("Unable to delete status file, but report is created with complete log");
    }

out:

    if (delete_dir(directory_name) != 0) {
        ALOGE("Can't suppress %s directory", directory_name);
    }

    dump_fd = 0;
    pthread_mutex_unlock(&mutex);
    return ret;
}

/*
 * coredump_generation
 *
 * generate a coredump
 * server
 * input  : path where coredump is generated (FS or SDCARD)
 *          path where trace file are located
 * output : 0 OK
 *          other value on errors
 */
int coredump_generation(int fd_dump, char *coredump_path, char *trace_filename)
{
    FILE *fd;
    int  ret;
    char directory_name[NAME_LENGTH];
    char archive_name[NAME_LENGTH];
    char report_dir[NAME_LENGTH];
    struct tm *time;
    fd = fopen(KERNEL_MSG_DEV, "w");
    struct tee_product_config product_cfg;

    ret = get_product_config(&product_cfg);

    if (ret != BASS_RC_SUCCESS) {
        ALOGE("Get product configuration failed, exiting...\n");

        if (fd != 0) {
            fclose(fd);
        }

        exit(1);
    }

    if (!((product_cfg.rt_flags & TEE_RT_FLAGS_MODEM_DEBUG) || (product_cfg.rt_flags & TEE_RT_FLAGS_ALLOW_MODEM_DUMP))) {
        ALOGW("Modem debug deactivated in ISSW. No modem core dump generated!\n");

        if (fd != 0) {
            fclose(fd);
        }

        return -1;
    }

    dump_fd = fd_dump;
    pthread_mutex_lock(&mutex);

    /* build the coredump temp directory in TEMP_PATH/coredump_temp */
    EXTRADEBUG("coredump_path = %s", coredump_path);
    EXTRADEBUG("TEMP_PATH = %s", TEMP_PATH);

    snprintf(directory_name, NAME_LENGTH, "%s%s", TEMP_PATH, TEMP_DIR);
    ret = Create_dir(directory_name);

    if (ret != 0) {
        ALOGE("can't create %s directory", directory_name);
        goto out;
    }

    ret = coredump_write_modem_data(directory_name);

    if (ret != 0) {
        goto out;
    }

    ret = coredump_write_secure_modem_mem(directory_name);

    if (ret != 0) {
        goto out;
    }

    // Read the dump data from debugfs, we don't need to check for
    // return because if the debugfs doesn't exist, we just carry ahead
    // with coredump anyway
    coredump_write_debugfs_dump_data(directory_name, AB8500_DUMP_NAME, AB8500_DUMP_REG);
    coredump_write_debugfs_dump_data(directory_name, PRCMU_MEMDUMP_NAME, PRCMU_DUMP_MEM);
    coredump_write_debugfs_dump_data(directory_name, PRCMU_REGDUMP_NAME, PRCMU_DUMP_REG);
    coredump_write_debugfs_dump_data(directory_name, SHRM_FTRACE_NAME, SHRM_FTRACE);

    time = get_time_stamp();
    /* create timestamp */
    char time_stamp[TIMESTAMP_LENGTH];
    memset(time_stamp, 0, TIMESTAMP_LENGTH);
    snprintf(time_stamp, TIMESTAMP_LENGTH, "%04d%02d%02d%02d%02d%02d",
             1900 + time->tm_year,
             time->tm_mon + 1,
             time->tm_mday,
             time->tm_hour,
             time->tm_min,
             time->tm_sec);

    if (strncmp(coredump_path, PREFIX_COREDUMP_USB, 3) != 0) {
        memset(report_dir, 0, NAME_LENGTH);
        snprintf(report_dir, NAME_LENGTH, "%s/%s%s", coredump_path, PREFIX_MODEM_COREDUMP, time_stamp);

        check_available_space(coredump_path);
        Create_dir(report_dir);

        if (trace_filename != NULL) {
            coredump_write_trace_buffer(report_dir, trace_filename);
        }
    }

    snprintf(archive_name, NAME_LENGTH, "%s", PREFIX_MODEM_COREDUMP);

    if (dump_fd > 0) {
        ret = coredump_create_archive(directory_name, archive_name, time);

        if (ret != 0) {
            goto out;
        }

        ret = write_file_to_socket(directory_name, archive_name);
    } else {
        ret = coredump_create_archive(report_dir, archive_name, time);
    }

    if (ret != 0) {
        goto out;
    }

    if (fd != 0) {
        // Need to print the kernel message to show the dump path
        if (strncmp(coredump_path, PREFIX_COREDUMP_USB, 3) != 0) {
            fprintf(fd, MODEMDUMP_MSG "%s/%s \n", report_dir, archive_name);
        } else {
            fprintf(fd, "Modem coredump sent over USB.\n");
        }
    }

    // Delete the status file to indicate that log is
    // completed successfully
    if (remove(log_complete_status) != 0) {
        ALOGW("Unable to delete status file, but dump is created with complete log");
    }

out:

    /* suppress the coredump temp directory */
    if (delete_dir(directory_name) != 0) {
        ALOGE("Can't suppress %s directory", directory_name);
    }

    dump_fd = 0;
    pthread_mutex_unlock(&mutex);

    if (fd != 0) {
        fclose(fd);
    }

    return ret;
}

