/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/** \file cmdebugfs.c
 *
 * Implementation of the CM User Proxy Memory Dump infrastructure.
 * Linked with user space NMF applications
 *
 * This feature is based on debugfs.
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>


#include "cmproxy.h"
#include "cmioctl.h"

#define DEFAULT_DEBUGFS_PATH          "/debug"

#ifdef ANDROID
#define DEFAULT_DUMP_PATH             "/data"
#define NMF_DSP_NODUMP_PROPERTY_NAME  "ste.nmf.dsp.nodump"
#define NMF_DSP_DUMPDIR_PROPERTY_NAME "ste.nmf.dsp.dumpdir"
#else
#define DEFAULT_DUMP_PATH             "/tmp"
#define NMF_DSP_NODUMP_PROPERTY_NAME  "ste_nmf_dsp_nodump"
#define NMF_DSP_DUMPDIR_PROPERTY_NAME "ste_nmf_dsp_dumpdir"
#endif
#define PATH_NAME_MAX 256
#define BUFFER_LENGTH 256

static char *debugfs_path;
static int sort(const struct dirent **, const struct dirent **);

#define MAX_FILE_COUNT 10
#define TIMESTAMP_OFFSET 30
#define PREFIX "mmdsp-"
#define PREFIX_SIZE strlen(PREFIX)




/**
 * Check if debugfs feature is present.
 * Return 1 if present, 0 otherwise.
 */
static int is_debugfs(void)
{
	FILE *file;
	char line[80];
	int found = 0;

	file = fopen("/proc/filesystems", "r" );
	if (file == NULL) {
		ALOGE("Can not open file /proc/filesystems: %s\n",
		     strerror(errno));
		return 0;
	}
	while (fgets(line, sizeof(line), file) != NULL ) {
		if (strstr(line, "debugfs")) {
			found = 1;
			break;
		}
	}
	fclose(file);
	return found;
}

/**
 * Retrieve the debugfs mountpoint if mounted.
 * Return 1 if retrieved succesfully and fill debugfs string.
 * Return 0 otherwise.
 */
static int get_debugfs_mountpoint(void)
{
	FILE *file;
	char fstype[40], mountpt[128];
	int found = 0;
	
	file = fopen("/proc/mounts", "r" );
	if (file == NULL) {
		ALOGE("Can not open file /proc/mounts: %s\n",
		     strerror(errno));
		return 0;
	}
	while (fscanf(file, "%*s %s %s %*s %*d %*d\n", mountpt, fstype) != EOF) {
		if (strcmp(fstype, "debugfs") == 0) {
			found = 1;
			debugfs_path = strdup(mountpt);
			break;
		}
	}
	if ((!found) && (!feof(file)) && (ferror(file)))
		ALOGE("Error parsing file /proc/mounts: %s\n",
		     strerror(errno));
	fclose(file);
	return found;
}

/**
 * Mount the debugfs.
 * Return 1 if ok, 0 otherwise.
 */
static int mount_debugfs(void)
{
	DIR *dir;

	dir = opendir(DEFAULT_DEBUGFS_PATH);
	if (dir == NULL) {
		/* Try to create directory */
		if (mkdir(DEFAULT_DEBUGFS_PATH, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) {
			ALOGE("Can not create directory %s: %s\n",
			     debugfs_path, strerror(errno));
			return 0;
		}
	} else {
		closedir(dir);
	}

	if (mount("nodev", DEFAULT_DEBUGFS_PATH, "debugfs", 0, NULL)) {
		ALOGE("Can not mount debugfs on %s: %s\n",
		     debugfs_path, strerror(errno));
		return 0;
	}

	debugfs_path = strdup(DEFAULT_DEBUGFS_PATH);
	return 1;
}

void cm_debugfs_init(void)
{
	if ((is_debugfs())
	    && (get_debugfs_mountpoint() || (mount_debugfs()))) {
		char path[128];
		int len = strlen(debugfs_path);
		DIR *dir;

		strncpy(path, debugfs_path, 128);
		path[len]='/';
		len++;
		strncat(&path[len], DEBUGFS_ROOT, 128-len);
		free(debugfs_path);

		dir = opendir(path);
		if (dir == NULL) {
			ALOGI("%s not found in debugfs\n", DEBUGFS_ROOT);
			debugfs_path = NULL;
		} else {
			ALOGI("%s found in debugfs, mounted on %s\n",
			     DEBUGFS_ROOT, path);
			closedir(dir);
			debugfs_path = strdup(path);
		}
	}
}

int cm_has_debugfs(void)
{
	return (debugfs_path != NULL);
}

/*!
 * \brief Write out information for the panic.
 *
 * \param[in] data Panic data.
 * \param[in] time Time when the panic occured.
 * \param[in] timestamp The timestamp when the panic occured.
 * \param[in] dir Destination directory.
 *
 * \ingroup CM_INTERNAL_DSP_DEBUG
 */
static void write_metadata(t_nmf_service_data *data, struct tm *time, time_t timestamp, char* dir)
{
	char *dst       = (char*)malloc(strlen(dir) + 32);
	char *buf       = (char*)malloc(BUFFER_LENGTH);
	char *current   = buf;
	FILE *meta;

	if (!buf) {
		return;
	}

	if (!dst) {
		goto out1;
	}

	sprintf(dst, "%s/metadata.txt", dir);

	current += sprintf(current, "Panic %s\n", (data->panic.info.mpc.coreid == SVA_CORE_ID ? "SVA" : "SIA"));
	current += sprintf(current, "Time %d-%.2d-%.2d %.2d:%.2d:%.2d timestamp:%d\n", time->tm_year + 1900,
			time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec,
			(int)timestamp);
	current += sprintf(current, "TimeNormalized %d-%.2d-%.2d_%.2d-%.2d-%.2d_%d\n", time->tm_year + 1900,
			time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec,
			(int)timestamp);
	current += sprintf(current, "Reason %lu\nFaultingComponent 0x%x\nInfo1 0x%x\nInfo2 0x%x\n",
			(t_uint32)data->panic.panicReason, (unsigned int)data->panic.info.mpc.faultingComponent,
			(unsigned int)data->panic.info.mpc.panicInfo1, (unsigned int)data->panic.info.mpc.panicInfo2);
	//printf(buf);

	meta = fopen(dst, "w+");
	if (!meta) {
		ALOGE("Failed to open file %s: %s (errno=%d)\n",
		     dst, strerror(errno), errno);
		goto out;
	}
	fwrite(buf, 1, strlen(buf), meta);
	fclose(meta);

out:
	free(dst);
out1:
	free(buf);
}

/*!
 * \brief Copy file to destination.
 *
 * \param[in] from_file Source file to copy
 * \param[in] to_file Destination file to copy
 * \param[in] base Base of the source file
 * \param[in] prefix Prefix of the source file, appended to the base
 * \param[in] dst Destination directory, appended before to_file
 *
 * \ingroup CM_INTERNAL_DSP_DEBUG
 */
void copy(char *from_file, char *to_file, char* base, char* prefix, char* dst)
{
	char *from = (char*)malloc(strlen(base) + strlen(prefix) + strlen(from_file) + 3); //+2 for 2x/ + \0
	char *to = (char*)malloc(strlen(dst) + strlen(to_file) + 2);                       //+1 for 1x/ + \0
	char buf[4096];
	int in;
	int out;
	int n;

	sprintf(from, "%s/%s/%s", base, prefix, from_file);
	sprintf(to, "%s/%s", dst, to_file);

	in = open(from, O_RDONLY);
	out = open(to, O_WRONLY | O_CREAT);

	if (in == -1) {
		ALOGE("Couldn't open in file %s: %s (errno=%d)\n",
		     from, strerror(errno), errno);
		goto out;
	}
	if (out == -1) {
		ALOGE("Couldn't open out file %s: %s (errno=%d)\n",
		     to, strerror(errno), errno);
		goto out;
	}

	while( (n = read(in, buf, sizeof(buf))) > 0) {
		write(out, buf, n);
	}

	close(out);
	close(in);

out:
	free(from);
	free(to);
}

/*!
 * \brief Do the actual copy of the coredump files.
 *
 * Create destination directory.
 * Write coredump meta-data.
 * Copy snapshot files.
 * Notify cm to release snapshot.
 *
 * \param[in] data Panic data.
 *
 * \ingroup CM_INTERNAL_DSP_DEBUG
 */
void cm_debugfs_dump(t_nmf_service_data *data)
{
	char           base[PATH_NAME_MAX];
	char           dest[PATH_NAME_MAX];
	char           dir_name[PATH_NAME_MAX];
	char           filetime[PATH_NAME_MAX];
	char           prefix[]   = "dsp/sXa/snapshot";
	time_t         timestamp  = time(NULL);
	struct tm     *time       = localtime(&timestamp);
	int            dir;
	struct dirent **namelist;
	int n;
	unsigned char file_count = 0;
	time_t  filetimestamp;
	time_t  dir_timestamp;
	property_get(NMF_DSP_NODUMP_PROPERTY_NAME, base, "0");
	if (strcmp(base, "0"))
		return;
	property_get(NMF_DSP_DUMPDIR_PROPERTY_NAME, base, DEFAULT_DUMP_PATH);
	sprintf(dest, "%s/%s%s_%d-%.2d-%.2d_%.2d-%.2d-%.2d_%d",base,PREFIX,
			(data->panic.info.mpc.coreid == SVA_CORE_ID ? "SVA" : "SIA"),
			time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec,
			(int)timestamp);
	ALOGI("MMDSP-Coredump for %s, %s\n", (data->panic.info.mpc.coreid == SVA_CORE_ID ? "SVA" : "SIA"), dest);
	if (data->panic.info.mpc.coreid == SVA_CORE_ID)
		prefix[5] = 'v';
	if (data->panic.info.mpc.coreid == SIA_CORE_ID)
		prefix[5] = 'i';
	//*REMOVE OLD logs and append new logs if logs are more than Threshold
	n = scandir(DEFAULT_DUMP_PATH, &namelist, 0, sort);
	if (n < 0)
		perror("scandir");
	else
	{
		dir_timestamp = LONG_MAX;
		while(n--)
		{
			if (!strncmp(namelist[n]->d_name,PREFIX,PREFIX_SIZE))
			{
				strcpy(filetime,((char*)namelist[n]->d_name+TIMESTAMP_OFFSET));
				filetimestamp = atoi(filetime);
				//Hold the oldest directory info
				if(dir_timestamp > filetimestamp)
				{
					strcpy(dir_name,namelist[n]->d_name);
					dir_timestamp =  filetimestamp;
				}
				file_count++;
				//remove oldest dump directory if file count is more than MAX_FILE_COUNT threshold
				if(file_count > MAX_FILE_COUNT)
				{
					remove_directory(dir_name);
					ALOGI("remove_directory = %s \n" ,dir_name);
				}
				free(namelist[n]);
			}
		}
		free(namelist);
	}
	//create top directory
	dir = mkdir(dest, S_IRWXU | S_IRWXG | S_IRWXO);
	if (dir == -1) {
		perror(dest);
		return;
	}

	//write coredump meta-data
	write_metadata(data, time, timestamp, dest);

	//copy coredump files from snapshot
	copy("tcm24", "mem_tcm.data", debugfs_path, prefix, dest);
	copy("esram", "mem_esram.data", debugfs_path, prefix, dest);
	copy("sdram_code", "mem_sdram_code.data", debugfs_path, prefix, dest);
	copy("sdram_data", "mem_sdram_data.data", debugfs_path, prefix, dest);
}

int remove_directory(const char *path)
{
	DIR *d;
	unsigned int ret;
	char cwd[PATH_MAX+1];
	getcwd(cwd,PATH_MAX+1);
	int status = -1;
	ret = chdir(DEFAULT_DUMP_PATH);
	if(ret !=0)
	{
		ALOGE("remove_directory ERROR:Unable to change DIR %s \n",DEFAULT_DUMP_PATH);
		return status;
	}
	d = opendir(path);
	size_t path_len = strlen(path);
	if(d)
	{
		struct dirent *p;
		status = 0;
		while (!status && (p=readdir(d)))
		{
			char *buf;
			size_t len;
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
			{
				continue;
			}
			len = path_len + strlen(p->d_name) + 2;
			buf = malloc(len);
			if (buf)
			{
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, p->d_name);
				if (!lstat(buf, &statbuf))
				{
					if (S_ISDIR(statbuf.st_mode))
					{
						status = remove_directory(buf);
					}
					else
					{
						status = unlink(buf);
					}
				}
				free(buf);
			}
		}
		closedir(d);
	}
	if (!status)
	{
		status = rmdir(path);
	}
	chdir(cwd);
	return status;
}
//Function to pass bionic libc error
static int sort(const struct dirent **a, const struct dirent **b)
{
	return strcmp((*a)->d_name, (*b)->d_name);
}
