#include "toc_parser.h"

#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#define READ_BUFFER_SIZE (4096)

typedef enum {
	TOC_TYPE_NONE,
	TOC_TYPE,
	SUB_TOC_TYPE
} toc_type_t;

char *device_string = "/dev/mmcblk0";

void print_usage(char *name)
{
	printf(
		"\n"
		"Usage:\n"
		"%s [-h] [-dDx devicename] [-l] [-gG partition] [-rw partition:filename]\n"
		"	[-a -m 0X0000200,0x1200,...,TOC_PART -s 0x00123,0x1200,..,SUBTOC_PART] [-e TOC_NAME]\n"
		" or\n"
		"%s -p partition -f filename\n"
		"\n"
		" -h                             Print this help.\n"
		" -d devicename                  Open device as read-only (default).\n"
		" -D devicename                  Open device as read-write.\n"
		" -x devicename                  Open device in special editing mode.\n"
		"                                Should be used only with -a and -e options.\n"
		" -l                             List all partition and image entries.\n"
		" -g partition                   Get information for image inside partition.\n"
		" -G partition                   Get information for partition.\n"
		" -r partition:filename          Read content of partition into file.\n"
		" -w partition:filename          Write content of file into partition.\n"
		" -p partition -f filename       Write content of file into partition.\n"
		" -a -m tocparam -s subtocparam  Add entry in toc .\n"
		" -e tocname                     Remove entry from toc|subtoc list.\n"
		"\n"
		"By default %s will open %s as read-only.\n"
		"If -p and -f are used then %s will be opened as read-write.\n"
		"If -a is used then user must provide to the parameter tocparam (offset, size,\n"
		"flags, align, loadaddress and toc name) or to the parameter subtocparam (subtoc offset,\n"
		"subtoc size, subtoc flags, subtoc align, subtoc loadaddress and subtoc name) or both.\n"
		"\n",
		name, name, name, device_string, device_string);
}

int write_partition(tocparser_handle_t *handle, char *part, char *file)
{
	uint32_t new_size;
	int err;
	struct stat buff;
	int fd;
	tocparser_error_code_t toc_err;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		perror("open()");
		return -1;
	}

	if ((err = fstat(fd, &buff))) {
		perror("fstat()");
		close(fd);
		return -1;
	}

	new_size = (uint32_t) buff.st_size;
	if (new_size == 0) {
		printf("Error: file size is 0\n");
		close(fd);
		return -1;
	}

	printf("Writing file %s to entry %s, file-size 0x%x\n",
		file, part, new_size);

	if ((toc_err = tocparser_write_toc_entry_from_fd(
				handle,
				part,
				fd,
				new_size,
				NULL))) {
		printf("Error: %s\n", tocparser_err2str(toc_err));
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

int write_partition_optarg(tocparser_handle_t *handle, char *optarg)
{
	char *part = optarg;
	char *file = strstr(part, ":");

	if (file == NULL) {
		printf("Error, format should be '-w partition:filename'!\n");
		return -1;
	}

	*file = '\0';
	file++;

	return write_partition(handle, part, file);
}

int read_partition(tocparser_handle_t *handle, char *optarg)
{
	uint8_t *data;
	uint64_t size;
	uint32_t read_size;
	uint64_t tot_read;
	uint64_t offset;
	uint32_t loadaddr;
	int fd;
	tocparser_error_code_t toc_err;

	char *part = optarg;
	char *file = strstr(part, ":");

	if (file == NULL) {
		printf("Error, format should be '-w partition:filename'!\n");
		return -1;
	}

	*file = '\0';
	file++;

	if ((toc_err = tocparser_get_toc_entry_64(handle,
				part,
				&offset,
				&size,
				&loadaddr))) {
		printf("Failed to get %s\n", part);
		printf("Error: %s\n", tocparser_err2str(toc_err));
		return -1;
	}

	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR |
			S_IRGRP | S_IROTH);

	if (fd < 0) {
		perror("open()");
		return -1;
	}

	data = malloc(READ_BUFFER_SIZE);

	tot_read = 0;

	while (tot_read < size) {
		read_size = READ_BUFFER_SIZE < (size - tot_read) ?
			    READ_BUFFER_SIZE :
			    (size - tot_read);

		if ((toc_err = tocparser_load_toc_entry_64(handle,
						part,
						tot_read,
						read_size,
						(uint32_t) data))) {
			printf("Error: %s\n", tocparser_err2str(toc_err));
			close(fd);
			return -1;
		}

		if ((off_t) read_size != write(fd, data, read_size)) {
			printf("Failed to write complete entry"
					" to file!\n");
			close(fd);
			return -1;
		}

		tot_read += read_size;
	}

	close(fd);
	free(data);
	return 0;
}

int get_partition_info(tocparser_handle_t *handle, char *optarg)
{
	tocparser_error_code_t toc_err;
	uint64_t offset = 0;
	uint64_t size = 0;
	printf("\nGet Partition '%s'\n", optarg);
	if ((toc_err = tocparser_get_toc_partition_64(handle,
				optarg,
				&offset,
				&size))) {
		printf("Failed to get entry %s\n", optarg);
		printf("Error: %s\n", tocparser_err2str(toc_err));
		return -1;
	}
	printf("offset  : 0x%016llx\n", offset);
	printf("size    : 0x%016llx\n", size);

	return 0;
}

int get_content_info(tocparser_handle_t *handle, char *optarg)
{
	tocparser_error_code_t toc_err;
	uint64_t offset = 0;
	uint64_t size = 0;
	uint32_t loadaddr = 0;
	printf("\nGet entry '%s'\n", optarg);
	if ((toc_err = tocparser_get_toc_entry_64(handle,
				optarg,
				&offset,
				&size,
				&loadaddr))) {
		printf("Failed to get entry %s\n", optarg);
		printf("Error: %s\n", tocparser_err2str(toc_err));
		return -1;
	}
	printf("offset  : 0x%016llx\n", offset);
	printf("size    : 0x%016llx\n", size);
	printf("loadaddr: 0x%08x\n", loadaddr);
	return 0;
}

int parse_toc_entry_from_string (char *toc_param, tocparser_toc_entry_t *toc_entry_p)
{
	const char delimiters[] = ",";
	char *param = NULL;
	char *errstr;

	if (!toc_param) {
		errstr = "Toc not present\n";
		goto err;
	}

	if ((param = strtok (toc_param,delimiters))){
		toc_entry_p->offset = strtol(param,NULL,0);
		printf("Offset: %s = toc_entry_p->offset: %d\n", param, toc_entry_p->offset);
	} else {
		errstr = "Missing TOC offset entry\n";
		goto err;
	}

	if((param = strtok (NULL,delimiters))){
		toc_entry_p->size = strtol(param,NULL,0);
		printf("Size: %s = toc_entry_p->size: %d\n", param, toc_entry_p->size);
	} else {
		errstr = "Missing TOC size entry\n";
		goto err;
	}

	if((param = strtok (NULL,delimiters))){
		toc_entry_p->flags = strtol(param,NULL,0);
		printf("Flags: %s = toc_entry_p->flags: %d\n", param, toc_entry_p->flags);
	} else {
		errstr = "Missing TOC flags entry\n";
		goto err;
	}

	if((param = strtok (NULL,delimiters))){
		toc_entry_p->align = strtol(param,NULL,0);
		printf("Align: %s = toc_entry_p->align: %d\n", param, toc_entry_p->align);
	} else {
		errstr = "Missing TOC align entry\n";
		goto err;
	}

	if((param = strtok (NULL,delimiters))){
		toc_entry_p->loadaddr = strtol(param,NULL,0);
		printf("Loadaddress: %s = toc_entry_p->loadaddr: %d\n", param, toc_entry_p->loadaddr);
	} else {
		errstr = "Missing TOC loadaddress entry\n";
		goto err;
	}

	if ((param = strtok (NULL,delimiters))){
		memset(toc_entry_p->id, 0x00, TOC_ID_LENGTH);
		memcpy(toc_entry_p->id, param, strlen(param));
		printf("TocName: %s = toc_entry_p->id: %s\n", param, toc_entry_p->id);
	} else {
		errstr = "Missing TOCName entry\n";
		goto err;
	}

	return 0;
err:
	printf("Error...");
	printf("%s\n", errstr);
	return -1;
}



int add_in_toc(tocparser_handle_t *handle, char *toc_param, char *subtoc_param)
{
	tocparser_error_code_t toc_err;
	tocparser_toc_entry_t toc_entry;
	tocparser_toc_entry_t subtoc_entry;

	if ((!toc_param) && (!subtoc_param)) {
		printf("Missing parameters. toc or subtoc entry must be supplied.\n");
		return -1;
	}

	if (subtoc_param) {
		if (parse_toc_entry_from_string (subtoc_param, &subtoc_entry)){
			printf("Error in subtoc input arguments\n");
			return -1;
		}

		if ((toc_err = tocparser_add_toc_entry(handle, NULL, &subtoc_entry, NULL))) {
			printf("Failed to addSubToc entry %s\n", subtoc_entry.id);
			printf("Error: %s\n", tocparser_err2str(toc_err));
			return -1;
		}
	}

	if (toc_param) {
		if (parse_toc_entry_from_string (toc_param, &toc_entry)){
			printf("Error in toc input arguments\n");
			return -1;
		}

		if ((toc_err = tocparser_add_toc_entry(handle, &toc_entry, NULL, NULL))) {
			printf("Failed to add Toc entry %s\n", toc_entry.id);
			printf("Error: %s\n", tocparser_err2str(toc_err));
		return -1;
		}
	}

	if ((toc_err =  tocparser_write_boot_and_toc(handle))) {
		printf("Failed to write toc in boot area %s\n", optarg);
		printf("Error: %s\n", tocparser_err2str(toc_err));
		return -1;
	}

	return 0;
}

int remove_from_toc(tocparser_handle_t *handle, char *TocName)
{
	tocparser_error_code_t toc_err;

	printf("\nRemove Toc '%s'\n", TocName);

	if ((toc_err = tocparser_remove_toc_entry(handle, TocName))) {
		printf("Failed to remove TOC entry %s\n", TocName);
		printf("Error: %s\n", tocparser_err2str(toc_err));
		return -1;
	}

	if ((toc_err =  tocparser_write_boot_and_toc(handle))) {
		printf("Failed to write toc in boot area %s\n", TocName);
		printf("Error: %s\n", tocparser_err2str(toc_err));
		return -1;
	}

	printf("TOC entry %s removed\n", TocName);

	return 0;
}

tocparser_mode_t init_mode (char c)
{
	if (c == 'D')
		return TOCPARSER_READWRITE;
	else if (c == 'x' || c == 'a' || c == 'e')
		return TOCPARSER_EMPTY_TOC;
	else
		return TOCPARSER_READONLY;
}

int main(int argc, char *argv[])
{
	tocparser_error_code_t toc_err;
	tocparser_handle_t *handle = NULL;
	tocparser_mode_t open_mode;
	int o;
	char *write_part = NULL;
	char *write_file = NULL;
	char *toc_param = NULL;
	char *subtoc_param = NULL;
	static bool adding_entry = false;

	while ((o = getopt(argc, argv, ":D:d:x:lw:r:g:p:G:f:am:s:e:")) != -1) {
		char c = (char) o;
		if (handle == NULL &&
				(c != 'h' && c != '?' && c != ':' &&
				c != 'p' && c != 'f' && c != 'm' && c != 's')) {
			if (c == 'd' || c == 'D' || c == 'x') {
				device_string = optarg;
			}
			open_mode = init_mode (c);
			if ((toc_err = tocparser_init(device_string, open_mode, &handle))) {
					printf("Failed to init libtocparser!\n");
					printf("Error: %s\n",
					tocparser_err2str(toc_err));
					return -1;
			}
			continue;
		}
		else if (handle != NULL && (c == 'd' || c == 'D' || c == 'x')) {
			printf("Error: Incorrect order of arguments!\n");
			break;
		}

		/* the partition update described by -p and -f
		 * and the toc entry update described by -a and -e will be executed
		 * after the while-loop.
		 */

		switch (c) {
		case 'p':
			if (write_part) {
				print_usage(argv[0]);
				return -1;
			}
			write_part = strdup(optarg);
			break;
		case 'f':
			if (write_file) {
				print_usage(argv[0]);
				return -1;
			}
			write_file = strdup(optarg);
			break;
		case 'l':
			tocparser_print_toc(handle);
			break;
		case 'G':
			if (get_partition_info(handle, optarg))
				return -1;
			break;
		case 'g':
			if (get_content_info(handle, optarg))
				return -1;
			break;
		case 'w':
			if (write_partition_optarg(handle, optarg))
				return -1;
			break;
		case 'r':
			if (read_partition(handle, optarg))
				return -1;
			break;
		case 'a':
			if (toc_param || subtoc_param) {
				print_usage(argv[0]);
				return -1;
			}
			adding_entry = true;
			break;
		case 'm':
			if (!adding_entry){
				print_usage(argv[0]);
				return -1;
			}
			toc_param = strdup(optarg);
			break;
		case 's':
			if (!adding_entry){
				print_usage(argv[0]);
				return -1;
			}
			subtoc_param = strdup(optarg);
			break;
		case 'e':
			if (remove_from_toc(handle, optarg))
				return -1;
			break;
		case ':':
		case 'h':
		case '?':
		default:
			print_usage(argv[0]);
			break;
		case 'd':
		case 'D':
			/* Already handled outside of switch-case */
			break;
		}
	}

	if (write_part || write_file) {
		if (!write_part || !write_file) {
			printf("Error in usage!\n");
			print_usage(argv[0]);
			return -1;
		}

		if (handle == NULL) {
			if ((toc_err = tocparser_init(device_string,
							TOCPARSER_READWRITE,
							&handle))) {
				printf("Failed to init libtocparser!\n");
				printf("Error: %s\n",
					tocparser_err2str(toc_err));
						                return -1;
			}
		}

		if (!write_partition(handle, write_part, write_file))
			return -1;
	}

	if (adding_entry) {
		if (!add_in_toc(handle, toc_param, subtoc_param))
			return -1;
	}

	if (tocparser_uninit(handle)) {
		printf("Failed to uninit libtocparser!\n");
		return -1;
	}

	free(write_file);
	free(write_part);

	return 0;
}
