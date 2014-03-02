/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   DTH server helper
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <syslog.h>
#include <dlfcn.h>
#include <dirent.h>

#include "dthsrvhelper.h"
#include "dthsrvpub.h"

static const char *sepDesc = "@@";
static const char *typeDesc = "type=";
static const char *enumDesc = "enum=";
static const char *infoDesc = "info=";
static const char *minmaxDesc = "minmax=";

/* size of dth_push_message function internal buffer for message format */
#define PUSH_MESSAGE_SIZE           1024
/* message stack size in number of chars including trailing '\0' */
#define MESSAGE_STACK_SIZE			2048
#define MESSAGE_STACK_SEPARATOR     '|'

char vg_dth_message_stack[MESSAGE_STACK_SIZE] = { '\0' };

void *handle[MAX_TAT_MODULES];

const struct dth_file_ops dth_helper_file_ops = {
	NULL,			/* open */
	dth_helper_read,	/* read */
	dth_helper_write,	/* write */
	NULL,			/* close */
	NULL			/* flush */
};

const struct dth_file_ops dth_helper_file_elem_generic_ops = {
	NULL,			/* open */
	dth_helper_file_read,	/* read */
	dth_helper_file_write,	/* write */
	NULL,			/* close */
	NULL			/* flush */
};

/* parse the enum values and check if the tested value is one of them
 * \retval 0 if test is one of the valid enum values
 * \retval -1 if not a valid value of an unexpected error occurred.
 * \retval ENOMEM alloc failed */
static int dth_check_enum(const struct dth_element *elem, long long test)
{
	int result = -1; /* 0 if success else a code meaning an error occurred */

	long long enum_value = -1LL;
	const char *enum_value_str = NULL;
	char *enum_str = strdup(elem->enumString);
	if (NULL == enum_str)
		goto err_alloc;

	char *item = strtok(enum_str, "\n");
	while ((NULL != item) && (0 != result))
	{
		enum_value_str = strchr(item, '\t');

		if (enum_value_str) {
			enum_value = atoll(enum_value_str + 1);

			if (enum_value == test)
				result = 0; /* the tested value is a valid enum value */
		}
		else {
			/* enum item with wrong format. should not happened */
			fprintf(stderr, "error: invalid enum string for %s\n",
				elem->path);
			result = -1;
			goto end;
		}

		item = strtok(NULL, "\n");
	}

	if (result)
		fprintf(stderr, "error: %lld is not a valid enum for %s\n",
			test, elem->path);
end:
	free(enum_str);
	return result;

err_alloc:
	return ENOMEM;
}

/**
 * Check numeric DTH value range validity .
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the value to check.
 * @retval 0 the value is valid.
 * @retval -1 the value is not valid.
 */
static int dth_check_numeric_value_range(const struct dth_element *elem, const dth_s64 value)
{
	int result = -1;

	/* check type */
	switch (elem->type) {
	case DTH_TYPE_U8:
	case DTH_TYPE_U8B:
		if (((dth_u8) value <= (dth_u8) elem->max) &&
			((dth_u8) value >= (dth_u8) elem->min))
			result = 0;
		break;
	case DTH_TYPE_S8:
		if (((dth_s8) value <= (dth_s8) elem->max) &&
			((dth_s8) value >= (dth_s8) elem->min))
			result = 0;
		break;
	case DTH_TYPE_U16:
	case DTH_TYPE_U16B:
		if (((dth_u16) value <= (dth_u16) elem->max) &&
			((dth_u16) value >= (dth_u16) elem->min))
			result = 0;
		break;
	case DTH_TYPE_S16:
		if (((dth_s16) value <= (dth_s16) elem->max) &&
			((dth_s16) value >= (dth_s16) elem->min))
			result = 0;
		break;
	case DTH_TYPE_U32:
	case DTH_TYPE_U32B:
		if (((dth_u32) value <= (dth_u32) elem->max) &&
			((dth_u32) value >= (dth_u32) elem->min))
			result = 0;
		break;
	case DTH_TYPE_S32:
		if (((dth_s32) value <= (dth_s32) elem->max) &&
			((dth_s32) value >= (dth_s32) elem->min))
			result = 0;
		break;
	case DTH_TYPE_U64:
	case DTH_TYPE_U64B:
		if (((dth_u64) value <= (dth_u64) elem->max) &&
			((dth_u64) value >= (dth_u64) elem->min))
			result = 0;
		break;
	case DTH_TYPE_S64:
		if (((dth_s64) value <= (dth_s64) elem->max) &&
			((dth_s64) value >= (dth_s64) elem->min))
			result = 0;
		break;
	case DTH_TYPE_FLOAT:
		if (((dth_float) value <= (dth_float) elem->max) &&
			((dth_float) value >= (dth_float) elem->min))
			result = 0;
		break;
	case DTH_TYPE_STRING:
		if (((dth_s32) value <= (dth_s32) elem->max) &&
			((dth_s32) value >= (dth_s32) elem->min))
			result = 0;
		break;
	case DTH_TYPE_FILE:
		/* nothing to compare */
		break;
	default:
		/* check bit field */
		if (elem->type >= DTH_BITFIELD_1_bits && elem->type <= DTH_BITFIELD_8_bits) {
			if (((dth_u8) value <= (dth_u8) elem->max) &&
				((dth_u8) value >= (dth_u8) elem->min))
				result = 0;
		} else {
			if (elem->type <= DTH_BITFIELD_16_bits) {
				if (((dth_u16) value <= (dth_u16) elem->max) &&
					((dth_u16) value >= (dth_u16) elem->min))
					result = 0;
			} else {
				if (elem->type <= DTH_BITFIELD_32_bits) {
					if (((dth_u32) value <= (dth_u32) elem->max) &&
						((dth_u32) value >= (dth_u32) elem->min))
						result = 0;
				} else {
					if (elem->type <= DTH_BITFIELD_64_bits) {
						if (((dth_u64) value <= (dth_u64) elem->max) &&
							((dth_u64) value >= (dth_u64) elem->min))
							result = 0;
					}
				}
			}
		}
		break;
	}

	return result;
}

/**
 * Cast the value of a DTH element in another type.
 * @param[in] type specifies the DTH element type.
 * @param[in] value specifies the data to cast.
 * retval number of bytes written in data buffer.
 */
dth_s64 dth_cast_value(const int type, const dth_s64 value)
{
	dth_s64 value_cast;

	union value_union {
		dth_s64 value_dth_s64;
		float value_float;
	} vl_val_union;

	value_cast = 0;

	switch (type) {
	case DTH_TYPE_U8:
	case DTH_TYPE_U8B:
		value_cast = (dth_u8) value;
		break;

	case DTH_TYPE_S8:
		value_cast = (dth_s8) value;
		break;

	case DTH_TYPE_U16:
	case DTH_TYPE_U16B:
		value_cast = (dth_u16) value;
		break;

	case DTH_TYPE_S16:
		value_cast = (dth_s16) value;
		break;

	case DTH_TYPE_U32:
	case DTH_TYPE_U32B:
		value_cast = (dth_u32) value;
		break;

	case DTH_TYPE_S32:
		value_cast = (dth_s32) value;
		break;

	case DTH_TYPE_U64:
	case DTH_TYPE_U64B:
		value_cast = (dth_u64) value;
		break;

	case DTH_TYPE_S64:
		value_cast = (dth_s64) value;
		break;

	case DTH_TYPE_FLOAT:
		{
			vl_val_union.value_dth_s64 = value;
			value_cast = vl_val_union.value_float;
#if defined(_DEBUG)
			dth_s32 temp_S32=0;
			float * temp_float=NULL;
			printf("dth_cast_value: Element value (s32): %d\n", (dth_s32) temp_S32);
			printf("\t\tElement value (float): %f\n", (float)*temp_float);
			printf("\t\tElement value (s64): %lld\n", (dth_s64) * temp_float);
#endif
		}
		break;

	default:
		if ((type >= DTH_BITFIELD_PART_MIN_SIZE)
		    && (type <= DTH_BITFIELD_PART_MAX_SIZE))
			value_cast = (dth_u64) value;
	}

	return value_cast;
}

/**
 * Get the number of bytes needed to allocate the specified type.
 * @param[in] type specifies the DTH element type.
 */
static unsigned int dth_get_size_type(const int type)
{

	int size;
	size = 0;

	switch (type) {
	case DTH_TYPE_U8:
	case DTH_TYPE_U8B:
		size = sizeof(dth_u8);
		break;

	case DTH_TYPE_S8:
		size = sizeof(dth_s8);
		break;

	case DTH_TYPE_U16:
	case DTH_TYPE_U16B:
		size = sizeof(dth_u16);
		break;

	case DTH_TYPE_S16:
		size = sizeof(dth_s16);
		break;

	case DTH_TYPE_U32:
	case DTH_TYPE_U32B:
		size = sizeof(dth_u32);
		break;

	case DTH_TYPE_S32:
		size = sizeof(dth_s32);
		break;

	case DTH_TYPE_U64:
	case DTH_TYPE_U64B:
		size = sizeof(dth_u64);
		break;

	case DTH_TYPE_S64:
		size = sizeof(dth_s64);
		break;

	case DTH_TYPE_FLOAT:
		size = sizeof(dth_float);
		break;

	case DTH_TYPE_STRING:
		size = sizeof(dth_s32);
		break;

	default:
		if ((type >= DTH_BITFIELD_PART_MIN_SIZE)
		    && (type <= DTH_BITFIELD_PART_MAX_SIZE))
			size = sizeof(dth_u64);
		else
			size = 0;
	}

	return size;
}

/**
 * Check numeric DTH value validity.
 * @param[in] elem specifies the DTH element.
 * @param[in] data specifies the value to write.
 * @retval 0 the value is valid.
 * @retval -1 the value is not valid.
 */
static int dth_check_numeric_value(const struct dth_element *elem, const dth_u8 * data)
{
	dth_s64 val_cast;
	int result;

	result = -1;
	val_cast = 0;

	if (elem->type == DTH_TYPE_STRING) {
		/* Check if string match the size defined */
		val_cast = (dth_s64)strlen((char*)data);
	} else {		/* Numeric DTH element. */
		dth_s64 val;
		dth_u8 *buff;
		char *typeString;
		int i;
		int size;

		val = 0;
		buff = (dth_u8 *) & val;
		typeString = NULL;
		size = dth_get_size_type(elem->type);

		/* Copy data in val via an u8 buffer. */
		for (i = 0; i < size; i++)
			*(buff + i) = *(data + i);

		/* Cast value in the correct type. */
		val_cast = dth_cast_value(elem->type, val);
	}
#if defined(_DEBUG)
	printf("dthsrvhelper : value to write: %lld\n", val_cast);
#endif

	if ((elem->min != 0) || (elem->max != 0)) {
#if defined(_DEBUG)
		printf("dthsrvhelper : value min: %lld  value max: %lld\n", elem->min, elem->max);
#endif
		result = dth_check_numeric_value_range(elem, val_cast);
	}

	return result;
}

static int dth_get_long_long(const struct dth_element *elem,
		const dth_u8 *data, long long *value)
{
	int result = 0;
	switch (elem->type) {
	case DTH_TYPE_S8:
		*value = *((char *)data);
		break;
	case DTH_TYPE_U8:
		*value = *((unsigned char *)data);
		break;
	case DTH_TYPE_S16:
		*value = *((short *)data);
		break;
	case DTH_TYPE_U16:
		*value = *((unsigned short *)data);
		break;
	case DTH_TYPE_S32:
		*value = *((long *)data);
		break;
	case DTH_TYPE_U32:
		*value = *((unsigned long *)data);
		break;
	case DTH_TYPE_S64:
		*value = *((long long *)data);
		break;
	case DTH_TYPE_U64:
		*value = (long long)*((unsigned long long *)data);
		break;
	default:
		fprintf(stderr, "not a valid type for enum element %s\n",
			elem->path);
		result = -1;
	}

	return result;
}

/**
 * Check DTH value validity.
 * @param[in] elem specifies the DTH element.
 * @param[in] data specifies the value to write.
 * @retval 0 the value is valid.
 * @retval -1 the value is not valid.
 */
static int dth_check_value(const struct dth_element *elem, const dth_u8 * data)
{
	int result = -1;

	if (elem->enumString != NULL) {

		long long enum_value = -1LL;
		if (dth_get_long_long(elem, data, &enum_value) == 0)
			if (dth_check_enum(elem, enum_value) == 0)
				result = 0;

	} else if ((elem->min != 0) || (elem->max != 0)) {
		result = dth_check_numeric_value(elem, data);
	} else if (elem->type == DTH_TYPE_STRING) {
		if (strlen((char*)data) <= TNPS_MSIZE - DNPS_TWRITESIZE)
			result = 0;
	} else	{
		result = 0;
	}

#if defined(_DEBUG)
	if (result == 0)
		printf("dthsrvhelper : value OK\n\n");
	else if (result == -1)
		printf("dthsrvhelper : value KO\n\n");
#endif
	return result;
}

/**
 * Check DTH size validity.
 * @param[in] elem specifies the DTH element.
 * @param[in] count specifies the 9p count data.
 * @retval 0 the value is valid.
 * @retval -1 the value is not valid.
 */
static int dth_check_size(const struct dth_element *elem, const int count)
{
	int result = -1;

	if (elem->type == DTH_TYPE_STRING) {
		if((elem->min == 0) && (elem->max==0))
			result = 0;
		else if (count >= elem->max)
			result = 0;
	} else {
		if ( (unsigned int)count >= dth_get_size_type(elem->type))
			result = 0;
	}

#if defined(_DEBUG)
	if (result == 0)
		printf("dthsrvhelper : size OK\n\n");
	else if (result == -1)
		printf("dthsrvhelper : size KO\n\n");
#endif
	return result;
}

static void uninit_tat_services()
{
	void (*undo_proc) ();
	int imodule = 0;

	/* Call dth_uninit_service for each loaded service which provides that
	 * interface.
	 */
	for (imodule = 0;
	     (handle[imodule] != NULL) && (imodule < MAX_TAT_MODULES);
	     imodule++) {
		fprintf(stdout, "uninit of service %d\n", imodule);

		undo_proc = dlsym(handle[imodule], DTH_UNINIT_SERV);
		fprintf(stdout, "undo proc address: %p\n", undo_proc);

		/* the module may have not defined such interface.
		 * it is optional */
		if (undo_proc != NULL) {
			/* call the undo init procedure of the module */
			undo_proc();
		}
	}
}

inline void stop_tat_services()
{
	int vl_handleitem = 0;

	fprintf(stdout, "unloading libraries...\n");

	while (handle[vl_handleitem] != NULL) {
		dlclose(handle[vl_handleitem]);
		vl_handleitem++;
	}
}

static inline int publish(struct dirent *dir_read, int vl_handleitem)
{
	int (*init_service) ();
	char *error = NULL;
	int err = 0;

	dth_publish_set_module(dir_read->d_name);

	/*Load the TAT library. */
	handle[vl_handleitem] = dlopen(dir_read->d_name, RTLD_LAZY);
	if (handle[vl_handleitem]) {
		/* Initialize the TAT library. */
		init_service = dlsym(handle[vl_handleitem], DTH_INIT_SERV);
		error = dlerror();
		if (error != NULL) {
			printf("error durin dlsym of dth_init_service\n");
			err = -1;
			goto error;
		}

		err = init_service();
		if (err < 0) {
			printf("Error during init of %s\n", dir_read->d_name);
			goto error;
		}
	} else {
		printf("Error opening %s: %s\n", dir_read->d_name, dlerror());
		err = -1;
		goto error;
	}

error:
	return err;
}

/* @return
0 : success, all services initialized
1 : fail to initialize VFS
2 : an error occurred when accessing plugins directory
3 : fail to load a service
4 : error when retrieving dth_init_service interface of a module
5 : a module dth_init_service interface returned an error
*/
int start_tat_services(void)
{
	int err = 0;
	int vl_handleitem = 0;
	void *dll;
	char *error = NULL;
	int (*init_service) ();
	DIR *plugin_dir = NULL;
	struct dirent *dir_read;

	/* Load and initialize all TAT services. */
	plugin_dir = opendir(PLUGINS_PATH);
	if (plugin_dir != NULL) {
		errno = 0;
		do {
			dir_read = readdir(plugin_dir);
			if ((dir_read != NULL)
			    && (vl_handleitem < MAX_TAT_MODULES - 1)) {
#define DOT_SO_LENGTH 3
				int nameLength = strlen(dir_read->d_name);
				if (DOT_SO_LENGTH <= nameLength) {
					if (0 ==strncmp(".so",&(dir_read->d_name[nameLength -
						       DOT_SO_LENGTH]),
						    DOT_SO_LENGTH))
#undef DOT_SO_LENGTH
					{
						/*Load the TAT library. */
#ifdef DEBUG_9PSERVER
						fprintf(stdout,
							"loading %s...\n",
							dir_read->d_name);
#endif
						dll =
						    dlopen(dir_read->d_name,
							   RTLD_LAZY);

						if (dll) {
#ifdef DEBUG_9PSERVER
							printf
							    ("module %d address=%p.\n",
							     vl_handleitem,
							     dll);
#endif

							handle[vl_handleitem] =
							    dll;
							vl_handleitem++;

#ifdef DEBUG_9PSERVER
							printf
							    ("module loaded.\n");
#endif
							/* Initialize the TAT library. */
							init_service =
							    dlsym(dll,
								  "dth_init_service");
							if (NULL !=
							    init_service) {
#ifdef DEBUG_9PSERVER
								printf
								    ("initializing %s...\n",
								     dir_read->d_name);
#endif
								err =
								    init_service
								    ();
								if (err == 0) {
#ifdef DEBUG_9PSERVER
									printf
									    ("%s initialized.\n",
									     dir_read->d_name);
#endif
								} else {
									fprintf
									    (stderr,
									     "error while initializing %s!\n",
									     dir_read->d_name);
									err = 5;
								}
							} else {
								fprintf(stderr,
									"no dth_init_service interface found!\n");
								error = dlerror();
								    if (error !=NULL)
									fputs(error,stderr);
								err = 4;
							}
						} else {
							error = dlerror();
							    if (error !=NULL)
								fputs(error,stdout);

						}
					}
				}
			} else {
				if ((0 != errno) && (ENOENT != errno)
				    && (dir_read != NULL))
					fprintf(stderr,
						"readdir %s: errno = %d: %s\n",
						PLUGINS_PATH, errno,
						strerror(errno));
			}
		} while ((dir_read != NULL) && ((0 == err) || (4 == err) || (5 == err)));
		closedir(plugin_dir);
	} else {
		fprintf(stderr, "OpenDir %s: %s\n", PLUGINS_PATH,
			strerror(errno));
		fprintf(stderr, "No TAT services have been loaded\n");
		err = 2;
	}

#if defined(DUMP_DTH_SERVER_CONTENTS)
	/* dump what is really in 9P server! */
	dth_dump_tree_elt_args args_dmp;
	args_dmp.output = fopen("dthserver.txt", "w");
	if (args_dmp.output) {
		args_dmp.options = 0;	/* not used yet */
		dth_enum_tree(&root->file, dth_dump_tree_elt, NULL, &args_dmp);
		fclose(args_dmp.output);
	}
#endif
	return err;
}

static int gen_api_start_tat_services()
{
	int err = 0;
	int vl_handleitem = 0;
	DIR *plugin_dir = NULL;
	struct dirent *dir_read = NULL;

	/* Load and initialize all TAT services. */
	plugin_dir = opendir(PLUGINS_PATH);
	if (plugin_dir == NULL) {
		printf("OpenDir %s: %s\n", PLUGINS_PATH, strerror(errno));
		printf("No TAT services have been loaded\n");
		goto error;
	}

	do {
		dir_read = readdir(plugin_dir);
		if (dir_read != NULL) {
			int nameLength = strlen(dir_read->d_name);
			if (!strncmp(".so", &(dir_read->d_name[nameLength - 3]), 3)) {

					vl_handleitem++;
					err = publish(dir_read, vl_handleitem);
					if (err)
						goto error;

			} else {
				if (errno != 0)
					printf("readdir %s: %s\n", PLUGINS_PATH, strerror(errno));
			}
		}
	} while ((dir_read != NULL) && (vl_handleitem < MAX_TAT_MODULES));

	closedir(plugin_dir);
	return 0;

error:
	closedir(plugin_dir);
	stop_tat_services();
	return -1;

}

int genAPIdoc(const char *api_doc_filename, const int debuglevel)
{
	int err = 0;

	dth_publish_verbose(debuglevel);
	dth_publish_init(api_doc_filename, 'c');

	/* Load and publish all TAT libraries. */
	err = gen_api_start_tat_services();

	/* unload TAT libraries */
	uninit_tat_services();
	stop_tat_services();

	if (err == 0) {
		dth_publish_validate(1);
	} else {
		dth_publish_validate(0);
		err = 5;
	}
	dth_publish_term();

	return err;
}

int dth_push_message(int level, const char *pp_format, ...)
{
	static char vl_msg[PUSH_MESSAGE_SIZE];
	static const size_t vl_max_len = MESSAGE_STACK_SIZE - 1;

	va_list vl_list;
	uint32_t vl_msg_len, vl_stack_len;
	vl_msg_len = vl_stack_len = 0;

	/* message string begins with the message criticity */
	snprintf(vl_msg, PUSH_MESSAGE_SIZE, "#%d ", level);
	vl_msg_len = strlen(vl_msg);

	/* format message body */
	va_start(vl_list, pp_format);
	vsnprintf(vl_msg + vl_msg_len, PUSH_MESSAGE_SIZE - vl_msg_len, pp_format, vl_list);
	vl_msg_len = strlen(vl_msg);
	va_end(vl_list);

	/* concatenate resulting string and escape any occurence of record seperator
	 * during the process */
	vl_stack_len = strlen(vg_dth_message_stack);

	if (vl_stack_len < (vl_max_len - 2)) {
		char *pl_dst = vg_dth_message_stack + vl_stack_len;
		*pl_dst++ = MESSAGE_STACK_SEPARATOR;
		vl_stack_len++;

		char *pl_src;
		for (pl_src = vl_msg; *pl_src && (vl_stack_len < vl_max_len); pl_src++) {
			*pl_dst++ = *pl_src;
			vl_stack_len++;

			if ((*pl_src == MESSAGE_STACK_SEPARATOR) && (vl_stack_len < vl_max_len)) {
				*pl_dst++ = MESSAGE_STACK_SEPARATOR;
				vl_stack_len++;
			}
		}

		*pl_dst = '\0';
	}

	vl_stack_len = strlen(vg_dth_message_stack);

	/* @postcond may never buffer overflow */
	if (vl_max_len < vl_stack_len)
		fprintf(stderr, "message stack overflow: file %s, line %d\n", __FILE__, __LINE__);

	return vl_stack_len;
}

void dth_reset_message_pump(void)
{
	vg_dth_message_stack[0] = '\0';
}

const char *dth_get_message_pump(void)
{
	return vg_dth_message_stack;
}

/**
 * Prepare the representation of a DTH array element
 * @param[in] elem specifies the DTH element.
 * @param[in,out] array specifies the DTH array where each cell are stored.
 * @param[in] offset specifies the position where data
 *				reading or writing starts.
 * @param[in] count specifies the number of bytes to read/write.
 * retval number of bytes to read/write in the DTH array.
 */
static int dth_prepare_array(struct dth_element *elem, struct dth_array *dthArray, const dth_u64 offset, const dth_u32 count)
{
	unsigned int n;
	unsigned int size;
	int result;

	size = dth_get_size_type(elem->type);
	n = elem->cols * elem->rows * size;
	result = 0;

	if ((offset == 0) && (count >= n)) {
		/*Access to the whole array */
		dthArray->col = elem->cols;
		dthArray->row = elem->rows;
		result = n;
	} else if ((offset < n) && (count >= size)) {
		/* Access to a single array element. */
		int sizeOfRow = 0;
		sizeOfRow = elem->cols * size;
		dthArray->row = offset / sizeOfRow;
		dthArray->col = (offset - (dthArray->row * sizeOfRow)) / size;
		result = size;
	} else {
		/* Offset is out the array */
		if(offset > n)
			result = -1;
	}

	return result;
}

int dth_helper_write(struct dth_file *file, const dth_u8 * data,
		     const dth_u64 offset, const dth_u32 count, int *error)
{
	struct dth_element *elem = NULL;
	int result = 0;
	int n = -1;

	/* reset message stack */
	dth_reset_message_pump();

	if (file != NULL)
		elem = (struct dth_element *)file->parent->file.service_data;

	if ((elem != NULL)) {
		int size;
		int nb_bytes;

		/* Set data size. */
		nb_bytes = 1;
		size = dth_get_size_type(elem->type);
		nb_bytes *= size;

		if (elem->type == DTH_TYPE_FILE) {	/* Access into a  DTH file  element. */
			struct dth_file_data myFile;
#if defined(_DEBUG)
			printf("%s : Write access into a DTH file\n", __FILE__);
#endif
			myFile.pathname = NULL;	/* unused here */
			myFile.offset = offset;
			myFile.count = count;
			myFile.data = (char *)data;

			/* Call DTH service callback. */
			result = elem->set(elem, (void *)&myFile);
			if (result == 0)
				n = count;
			else
				*error = (result>0)?result:EIO;
		} else if ((elem->cols > 0) && (elem->rows > 0)) {	/* Access into a  DTH array  element. */
			struct dth_array myArray;

#if defined(_DEBUG)
			printf
			    ("%s : Write access into a DTH array\n", __FILE__);
#endif

			myArray.array = (dth_s64 *) data;
			nb_bytes =
			    dth_prepare_array(elem, &myArray, offset, count);

			if (nb_bytes >= 0) {
				if ((offset == 0) && (myArray.col == elem->cols)
				    && (myArray.row == elem->rows)) {
					/* Check values for the entire table. */
					int i;
#if defined(_DEBUG)
					printf
					    ("%s : Writing of the entire table\n",
					     __FILE__);
#endif
					for (i = 0; i < elem->cols * elem->rows;
					     i++) {
#if defined(_DEBUG)
						printf
						    ("%s : Index element : %d",
						     __FILE__, (size * i));
						if (size == 1)
							printf
							    (" Element value (size 1): %d\n",
							     (dth_s8) * (data +
									 (size *
									  i)));
						else if (size == 2)
							printf
							    (" Element value (size 2): %d\n",
							     (dth_s16) * (data +
									  (size
									   *
									   i)));
						else if (size == 4)
							printf
							    (" Element value (size 4): %d\n",
							     (dth_s32) * (data +
									  (size
									   *
									   i)));
						else if (size == 8)
							printf
							    (" Element value (size 8): %lld\n",
							     (dth_s64) *(data +
									  (size
									   *
									   i)));
#endif
						if (dth_check_value
						    (elem,
						     data + (size * i)) < 0)
							goto bad_value;
					}
				} else {	/* Check values for one element of the table. */
#if defined(_DEBUG)
					printf
					    ("%s : Writing one element of the table\n",
					     __FILE__);
#endif
					if (dth_check_value(elem, data) < 0)
						goto bad_value;
				}

				/* Call DTH service callback. */
#if defined(_DEBUG)
				printf
				    ("%s : Call DTH service callback to write into an array\n",
				     __FILE__);
#endif

				result = elem->set(elem, (void *)&myArray);
				if (result == 0)
					n = nb_bytes;
				else
					*error = (result)?result:EIO;
			} else
				goto offset_out;
		} else {	/* Access to a string or a numeric DTH element. */
			if (offset != 0)
				goto no_data;

			/* Write data. */
			if (elem->type == DTH_TYPE_STRING) {
				char *buffer;

				buffer = calloc(count + 1, sizeof(char));
				if (!buffer)
					return ENOMEM;

				strncpy(buffer, (char *)data, count);

				/* Ckeck value validity. */
				if (dth_check_value(elem, (dth_u8 *) buffer) <
				    0) {
					if (buffer != NULL) {
						free(buffer);
					}
					goto bad_value;
				}
#if defined(_DEBUG)
				printf
				    ("%s : Call DTH service callback to set a string\n",
				     __FILE__);
#endif

				/* Call DTH service callback. */
				result = elem->set(elem, (void *)buffer);
				if (result == 0)
					n = strlen((char *)buffer);
				else
					*error = (result)?result:EIO;

				free(buffer);
			} else {
				/* Ckeck value validity. */
				if (dth_check_value(elem, data) < 0)
					goto bad_value;

#if defined(_DEBUG)
				printf
				    ("%s : Call DTH service callback to set a numeric value\n",
				     __FILE__);
#endif

				/* Call DTH service callback. */
				result = elem->set(elem, (void *)data);
				if (result == 0)
					n = size;
				else
					*error = (result)?result:EIO;
#if defined(_DEBUG)
				printf
				    ("%s : set a numeric value return %i. Size=%i, n=%i \n",
				     __FILE__, result, size, n);
#endif
			}
		}
	}

	return n;

no_data:
	return 0;

bad_value:
	*error = EINVAL;	/* Invalid argument. */
	return -1;

offset_out:
	*error = EACCES;	/* Permission denied. */
	return -1;
}

int dth_helper_file_write(struct dth_file *file, const dth_u8 * data,
			  const dth_u64 offset, const dth_u32 count, int *error)
{
	struct dth_element *elem = NULL;
	int n = -1;
	char *Filepath = NULL;
	char *Filename = NULL;
	char *pl_File = NULL;
	FILE *fd = NULL;
	int i = 0, j = 0;
	char command[1024] = "";
	char tarfile[20] = ".tar.gz";

	/* reset message stack */
	dth_reset_message_pump();

	if (file != NULL)
		elem = (struct dth_element *)file->parent->file.service_data;

	if (elem != NULL) {
		if ((elem->type == DTH_TYPE_FILE)
		    && (strncmp(file->name, "value", file->nlen) == 0) && (strlen(elem->info) > 0)) {
			if (strstr(elem->info, "Filepath=") != NULL) {
				Filepath =
				    strstr(elem->info,
					   "Filepath=") + strlen("Filepath=");
				i = strcspn(Filepath, "\n");
			}

			if (strstr(elem->info, "Filename=") != NULL) {
				Filename =
				    strstr(elem->info,
					   "Filename=") + strlen("Filename=");
				j = strcspn(Filename, "\n");
			} else {
				/* we want to write a tar.gz file and decompress it */
				j = strlen(tarfile);
			}

			if ((i != 0) && (j != 0)) {
				pl_File = malloc(i + 1 + j + 1);

				if (pl_File != NULL) {
					memcpy(pl_File, Filepath, i);
					if (Filename == NULL) {
						pl_File[i] = '\0';
						snprintf(command, 1024,
							 "cd ; tar -zxvf %s%s",
							 pl_File, tarfile);
						memcpy(&(pl_File[i]), tarfile,
						       j);
						pl_File[i + j] = '\0';
					} else {
						memcpy(&(pl_File[i]), "/", 1);
						memcpy(&(pl_File[i + 1]),
						       Filename, j);
						pl_File[i + j + 1] = '\0';
					}

#if defined(_DEBUG)
					printf("%s: %s\n",
					       __FUNCTION__, pl_File);
#endif

					if ((count == 0)
					    && (Filename == NULL)) {
						system(command);
						free(pl_File);
						return 0;
					} else {
						if (offset == 0)
							fd = fopen(pl_File,
								   "w");
						else
							fd = fopen(pl_File,
								   "a+");

						if (fd != NULL) {
							n = fwrite(data, 1,
								   count, fd);
#if defined(_DEBUG)
							printf
							    ("%s: %d bytes at offset=%d\n",
							     __FUNCTION__, n,
							     (int)
							     offset);
#endif

							fclose(fd);
							free(pl_File);
							return n;
						}

					}
					free(pl_File);
				}
			}
		}
	}

	*error = ENOENT;
	return -1;		/*No such file or directory */
}

int dth_helper_file_read(const struct dth_file *file, dth_u8 * data,
			 const dth_u64 offset, const dth_u32 count, int *error)
{
	struct dth_element *elem = NULL;
	int n = -1;		/* return code: -1 = No such file or directory, -2 = get handler existed and failed */
	char *Filepath = NULL;
	char *Filename = NULL;
	char *pl_File = NULL;
	char command[1024] = "";
	char tarfile[20] = ".tar.gz";
	int vl_error = 0;

	FILE *fd = NULL;
	int i = 0, j = 0;

	/* reset message stack */
	dth_reset_message_pump();

	if (file != NULL)
		elem = (struct dth_element *)file->parent->file.service_data;

	if (elem != NULL) {
#if defined(_DEBUG)
		printf("%s\n", __FUNCTION__);
#endif
		if ((elem->type == DTH_TYPE_FILE)
		    && (dth_str_cmp(file->name, file->nlen, "value") == 0)
		    && (strlen(elem->info) > 0)) {
			if (strstr(elem->info, "Filepath=") != NULL) {
				Filepath =
				    strstr(elem->info,
					   "Filepath=") + strlen("Filepath=");
				i = strcspn(Filepath, "\n");
			}

			if (strstr(elem->info, "Filename=") != NULL) {
				Filename =
				    strstr(elem->info,
					   "Filename=") + strlen("Filename=");
				j = strcspn(Filename, "\n");
			} else {
				/* we want to get all the files from the directory */
				/*  tar -zcvf Filepath.tar.gz Filepath */
				j = strlen(tarfile);
			}

			if ((i != 0) && (j != 0)) {
				pl_File = (char *)malloc(i + 1 + j + 1);

				if (pl_File != NULL) {
					memcpy(pl_File, Filepath, i);
					if (Filename == NULL) {
						if (offset == 0) {
							pl_File[i] = '\0';
							snprintf(command, 1024,
								 "tar -zcvf %s%s %s",
								 pl_File,
								 tarfile,
								 pl_File);
							system(command);
						}
						memcpy(&(pl_File[i]), tarfile,
						       j);
						pl_File[i + j] = '\0';
					} else {
						memcpy(&(pl_File[i]), "/", 1);
						memcpy(&(pl_File[i + 1]),
						       Filename, j);
						pl_File[i + j + 1] = '\0';
					}

					if ((NULL != elem->get)
					    && (0 == offset)) {
						struct dth_file_data myFile;

#if defined(_DEBUG)
						printf
						    ("%s: call service installed get procedure\n",
						     __FUNCTION__);
#endif
						myFile.pathname =
						    strdup(pl_File);
						myFile.offset = offset;
						myFile.count = count;
						myFile.data = (char *)data;

						vl_error =
						    elem->get(elem, &myFile);

						free(myFile.pathname);
					}

					if (vl_error == EACCES) {
						free(pl_File);
						return 0;
					}
#if defined(_DEBUG)
					printf("%s: %s\n",
					       __FUNCTION__, pl_File);
#endif

					fd = fopen(pl_File, "r");
					if (fd != NULL) {
						fseek(fd, offset, SEEK_SET);
						n = fread(data, 1, count, fd);

						fclose(fd);
#if defined(_DEBUG)
						printf
						    ("%s: %d bytes read\n",
						     __FUNCTION__, n);
#endif
					}
					free(pl_File);
				} else {
					n = -2;
					*error = (vl_error>0)?vl_error:EIO;
				}
				goto end;
			}
		}
	}

	*error = ENOENT;
end:
	return n;
}

inline int dth_helper_init(void)
{

	printf("DTH helper initialisation\n");
	int i, err;

	/* Initialize the DTH file System. */
	rootDir = malloc(sizeof(struct dth_directory));
	err = dth_initialize_directory(rootDir, "", 0);
	if (err < 0) {
		printf("Error during virtual file system initilization\n");
		return 1;
	}

	/* initialize TAT modules handles to null */
	for (i = 0; i < MAX_TAT_MODULES; i++)
		handle[i] = NULL;

	printf("start tat services\n");
	err = start_tat_services();

	return err;

}

inline void dth_helper_uninit(void)
{

	uninit_tat_services();
	stop_tat_services();

}

int dth_dispose_file(struct dth_directory *parent, const char *name)
{
	int ret = 0;

	/* if name is fully qualified or parent is NULL then return error
	 * dth_delete_file change value of parent directory, in case of error,
	 * root directory can be change. If root directory is corrupt all VFS is
	 * unusable*/
	if ((name[0] == '/') || (NULL == parent)) {
		ret = -1;
		goto error;
	}

	ret = dth_delete_file(parent, name);
	if (ret) {
		int vl_errno = errno;
		fprintf(stderr, "remove file path=%s, name=%s failed: %s\n",
			parent->file.name, name, strerror(vl_errno));
	}

error:
	return ret;
}

int dth_unregister_file(struct dth_directory *parent, const int type)
{
	char *filename = "";
	int ret = 0;

	switch (type) {
	case DTH_VALUE:
		filename = "value";
		break;
	case DTH_TYPE:
		filename = "type";
		break;
	case DTH_ENUM:
		filename = "enum";
		break;
	case DTH_MIN:
		filename = "min";
		break;
	case DTH_MAX:
		filename = "max";
		break;
	case DTH_DESC:
		filename = "desc";
		break;
	}
	if (*filename)
		ret = dth_dispose_file(parent, filename);
	else
		ret = -1;

	return ret;
}

int dth_unregister_branch(const char *path)
{
	int result = 0;
	struct dth_directory *dir;
	char *lastdir;

	/* precondition: root must be set. */
	if (rootDir == NULL) {
#if defined(_DEBUG)
		fprintf(stderr, "root directory is not set!\n");
#endif
		return -1;
	}

	/* nothing to be done when invoked in publishing mode */
	if (!dth_publish_is_enabled()) {
		dir = dth_walk_directories(rootDir, path, &lastdir);
		if (NULL != dir) {
			/* cannot remove the directory himself because we don't know if
			 * there are children DTH elements there. It is imperative not
			 * removing more than asked so we need to delete the files
			 * explictely. After having unregistered the element,
			 * the 9P server should run some kind of garbage collector to
			 * remove the directory if is empty.
			 */
			dth_delete_branch(&dir->file);
		} else {
			/* else the element may have already been removed */
			result = 1;	/* no effect */
		}
	}

	return result;
}

/**
 * Get a string describing the type of a DTH element.
 * @param[out] typeString specifies the string where
 *				the DTH element type description must be stored.
 * @param[in] type specifies the DTH element type.
 * @param[in] cols specifies cols number of an element.
 * @param[in] rows specifies rows number of an element.
 * @retval pointer to typeString.
 */
static char *dth_get_string_type(char **typeString, const int type,
				 const int cols, const int rows)
{
	int result = 0;

	*typeString = (char *)malloc(MAX_SIZE_STRING * sizeof(char));
	if (!*typeString)
		return NULL;

	switch (type) {
	case DTH_TYPE_U8:
		*typeString = strncpy(*typeString, "u8", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_U8B:
		*typeString = strncpy(*typeString, "b8", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_S8:
		*typeString = strncpy(*typeString, "s8", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_U16:
		*typeString = strncpy(*typeString, "u16", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_U16B:
		*typeString = strncpy(*typeString, "b16", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_S16:
		*typeString = strncpy(*typeString, "s16", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_U32:
		*typeString = strncpy(*typeString, "u32", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_U32B:
		*typeString = strncpy(*typeString, "b32", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_S32:
		*typeString = strncpy(*typeString, "s32", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_U64:
		*typeString = strncpy(*typeString, "u64", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_U64B:
		*typeString = strncpy(*typeString, "b64", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_S64:
		*typeString = strncpy(*typeString, "s64", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_STRING:
		*typeString = strncpy(*typeString, "string", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_FLOAT:
		*typeString = strncpy(*typeString, "float", MAX_SIZE_STRING);
		break;

	case DTH_TYPE_FILE:
		*typeString = strncpy(*typeString, "file", MAX_SIZE_STRING);
		break;

	default:
		if ((type >= DTH_BITFIELD_PART_MIN_SIZE)
		    && (type <= DTH_BITFIELD_PART_MAX_SIZE)) {
			snprintf(*typeString, MAX_SIZE_STRING, "bitfield(%d)",
				 type);
		}
	}
	if ((cols > 0) && (type > DTH_BITFIELD_PART_MAX_SIZE)) {
		char *s;
		s = *typeString + strlen(*typeString);
		result =
		    snprintf(s, MAX_SIZE_STRING - strlen(*typeString), "(%d)",
			     cols);
	}

	if ((rows > 0) && (type > DTH_BITFIELD_PART_MAX_SIZE)) {
		char *s;
		s = *typeString + strlen(*typeString);
		result =
		    snprintf(s, MAX_SIZE_STRING - strlen(*typeString), "(%d)",
			     rows);
	}

	return *typeString;
}

/**
 * Write data from source buffer to receiver buffer.
 * @param[in] src specifies the source buffer.
 * @param[in,out] dest specifies the 9P buffer.
 * @param[in] count specifies the size of the 9P buffer.
 * @param[in] nbbytes specifies the number of bytes stored into source buffer.
 * @param[in] truncate specifies if buff can be truncated or not.
 * retval number of bytes written in receiver buffer.
*/
static int dth_write_data(const dth_u8 *src, dth_u8 *dest,
			  const dth_u32 count, const unsigned int nbbytes,
			  int *error, int truncate)
{
	unsigned int i;
#if defined(_DEBUG)
	printf("%s: count=%i, nb_bytes=%i\n",
			__FUNCTION__, count, nbbytes);
#endif
	if (count >= nbbytes) {
		for (i = 0; i < nbbytes; i++)
			*(dest + i) = *(src + i);
	} else {
		if (truncate == 1) {
			for (i = 0; i < count; i++)
				*(dest + i) = *(src + i);
		} else {
			goto buffer_to_small;
		}
	}

	return i;

buffer_to_small:
	*error = EOVERFLOW;
	return -1;		/* buffer to small. */
}

/**
 * Check min & max according to element type
 * @param[in] elem specifies the DTH element.
 * @retval 0 success.
 * @retval errorCode if maximum and minimum are not relevant
 */
static int dth_check_MinMaxType(const struct dth_element *elem)
{
	int result = 0;

	/* For a DTH element that requires min and max values,
	   the max value must be greater than the min value. */
	if ((elem->max != 0) && (elem->min != 0)) {
		/* check type */
		switch (elem->type) {
		case DTH_TYPE_U8:
			if ((dth_u8) elem->max <= (dth_u8) elem->min)
				result = -1;
			break;
		case DTH_TYPE_S8:
			if ((dth_s8) elem->max <= (dth_s8) elem->min)
				result = -1;
			break;
		case DTH_TYPE_U16:
			if ((dth_u16) elem->max <= (dth_u16) elem->min)
				result = -1;
			break;
		case DTH_TYPE_S16:
			if ((dth_s16) elem->max <= (dth_s16) elem->min)
				result = -1;
			break;
		case DTH_TYPE_U32:
			if ((dth_u32) elem->max <= (dth_u32) elem->min)
				result = -1;
			break;
		case DTH_TYPE_S32:
			if ((dth_s32) elem->max <= (dth_s32) elem->min)
				result = -1;
			break;
		case DTH_TYPE_U64:
			if ((dth_u64) elem->max <= (dth_u64) elem->min)
				result = -1;
			break;
		case DTH_TYPE_S64:
			if ((dth_s64) elem->max <= (dth_s64) elem->min)
				result = -1;
			break;
		case DTH_TYPE_FLOAT:
			if ((dth_float) elem->max <= (dth_float) elem->min)
				result = -1;
			break;
		case DTH_TYPE_STRING:
			if (((dth_s32) elem->max <= (dth_s32) elem->min)
					&& (elem->max < TNPS_MSIZE - DNPS_TWRITESIZE))
				result = -1;
			break;
		case DTH_TYPE_FILE:
			/* nothing to compare */
			break;
		case DTH_TYPE_U8B:
			if ((dth_u8) elem->max <= (dth_u8) elem->min)
				result = -1;
			break;
		case DTH_TYPE_U16B:
			if ((dth_u16) elem->max <= (dth_u16) elem->min)
				result = -1;
			break;
		case DTH_TYPE_U32B:
			if ((dth_u32) elem->max <= (dth_u32) elem->min)
				result = -1;
			break;
		case DTH_TYPE_U64B:
			if ((dth_u64) elem->max <= (dth_u64) elem->min)
				result = -1;
			break;
		default:
			/* check bit field */
			if (elem->type >= DTH_BITFIELD_1_bits
			    && elem->type <= DTH_BITFIELD_8_bits) {
				if ((dth_u8) elem->max <= (dth_u8) elem->min)
					result = -1;
			} else {
				if (elem->type <= DTH_BITFIELD_16_bits) {
					if ((dth_u16) elem->max <=
					    (dth_u16) elem->min)
						result = -1;
				} else {
					if (elem->type <= DTH_BITFIELD_32_bits) {
						if ((dth_u32) elem->max <=
						    (dth_u32) elem->min)
							result = -1;
					} else {
						if (elem->type <=
						    DTH_BITFIELD_64_bits) {
							if ((dth_u64) elem->max
							    <=
							    (dth_u64) elem->min)
								result = -1;
						}
					}
				}
			}
			break;
		}
	}

	return result;
}

static int dth_check_element(const struct dth_element *elem)
{
	int result;
	char *space;

	result = 0;

	/* The element path does not contain space characters. */
	space = strchr(elem->path, ' ');
	if (space != NULL) {
		printf("%s element invalid : the path does not contain \
				spaces characters\n", elem->path);
		goto bad_element;
	}

	/* The type of a DTH element shall be defined in the "dth_type" enum. */
	if ((elem->type < DTH_BITFIELD_PART_MIN_SIZE)
	    || (elem->type >= DTH_TYPE_ENUM_MAX)) {
		printf("%s element invalid : incorrect type value\n",
		       elem->path);
		goto bad_element;
	}

	/* For a DTH element that requires min and max values,
	   the max value must be greater than the min value. */
	if (dth_check_MinMaxType(elem) != 0) {
		printf("%s element invalid : max value must be greater \
	  than min value\n", elem->path);
		goto bad_element;
	}

	/* A DTH array element does not support string type. */
	if ((elem->cols > 0) && (elem->rows > 0)) {
		if (elem->type == DTH_TYPE_STRING) {
			printf
			    ("%s element invalid : A array does not support string\n",
			     elem->path);
			goto bad_element;
		}
	}

	return result;

bad_element:
	return -1;
}

/**
 * Register a new file in the 9P server file tree.
 * @param[in] elem specifies the DTH element.
 * @param[in] parent specifies the directory where the file shall be created.
 * @param[in] type specifies the type of file.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
static int dth_register_file(const struct dth_element *elem,
			     struct dth_directory *parent, const int type)
{
	struct dth_file *file = NULL;
	int result;

	result = 0;

	switch (type) {
	case DTH_VALUE:
		{
			int perm = 0;

			if (elem->type != DTH_TYPE_FILE) {
				if (elem->set != NULL)
					perm += 0222;	/* add W perm */

				if ((elem->get != NULL)
				    || (elem->default_value != NULL)
				    || (elem->exec != NULL))
					perm += 0444;	/* add R perm */

				if (elem->exec != NULL)
					perm += 0111;	/* add Exec perm */

				file =
				    dth_create_file(parent, "value", perm,
						    &dth_helper_file_ops);
			} else {
				if ((strstr(elem->info, "Automatic Procedure")
				     != NULL)
				    && (strstr(elem->info, "Filepath=") !=
					NULL)) {
					perm += 0666;	/* add RW perm */
					file =
					    dth_create_file(parent, "value",
							    perm,
							    &dth_helper_file_elem_generic_ops);
#if defined(_DEBUG)
					printf
					    ("dth_create_file dth_helper_file_elem_generic_ops\n");
#endif
				} else {
					if (elem->set != NULL)
						perm += 0222;	/* add W perm */
					if (elem->get != NULL)
						perm += 0444;	/* add R perm */
					file =
					    dth_create_file(parent, "value",
							    perm,
							    &dth_helper_file_ops);
					printf("dth_create_file perm=%d %s\n",
					       perm, elem->info);
				}
			}
		}
		break;
	case DTH_DESC:
		/* Creates /desc file with read-only permission */
		file =
		    dth_create_file(parent, "desc", 0444, &dth_helper_file_ops);
		break;

	case DTH_TYPE:
		/* Creates /type file with read-only permission */
		file =
		    dth_create_file(parent, "type", 0444, &dth_helper_file_ops);
		break;

	case DTH_ENUM:
		/* Creates /enum file with read-only permission */
		file =
		    dth_create_file(parent, "enum", 0444, &dth_helper_file_ops);
		break;

	case DTH_MIN:
		/* Creates /min file with read-only permission */
		file =
		    dth_create_file(parent, "min", 0444, &dth_helper_file_ops);
		break;

	case DTH_MAX:
		/* Creates /max file with read-only permission */
		file =
		    dth_create_file(parent, "max", 0444, &dth_helper_file_ops);
		break;

	case DTH_INFO:
		/* Creates /info file with read-only permission */
		file =
		    dth_create_file(parent, "info", 0444, &dth_helper_file_ops);
		break;

	default:
		break;
	}

	if (file == NULL)
		result = -1;

	return result;
}

/**
 * Register a new element in DTH virtual file system.
 * @param[in] elem specifies the DTH element.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
static int _register_element(struct dth_element *elem)
{
	struct dth_directory *parent;
	int result;

	parent = NULL;
	result = 0;

	/* Check if the root directory and if the DTH element exist. */
	if ((rootDir == NULL) || (elem == NULL)) {
		printf("dth_register_element failure: root or element NULL\n");
		return -1;
	}

	/* Check if the DTH element is valid. */
	result = dth_check_element(elem);
	if (result < 0) {
		printf("dth_register_element failure: %s is a bad element\n",
		       elem->path);
		return -1;
	}
#if defined(_DEBUG)
	fprintf(stdout, "register %s\n", elem->path);
#endif

	char *missing_dirs = NULL;
	parent = dth_walk_directories(rootDir, elem->path, &missing_dirs);
	if (parent == NULL || (*missing_dirs == '\0')) {
		fprintf(stderr,
			"dth_register_element failure: wrong directories sequence: %s\n",
			elem->path);
		return -1;
	}

	/*fprintf(stdout, "missing directories: %s\n", missing_dirs); */
	/* No directories are registered in VFS. So, they are all created. */
	parent = dth_create_directories(parent, missing_dirs);
	if (parent == NULL) {
		fprintf(stderr,
			"dth_register_element failure: fail to create directory %s\n",
			elem->path);
		return -1;
	}
	parent->file.service_data = elem;

	/* Creates /value file (mandatory). */
	result = dth_register_file(elem, parent, DTH_VALUE);
	if (result < 0) {
		fprintf(stderr,
			"dth_register_element failure: unable to register value file for element %s\n",
			elem->path);
		return -1;
	}

	/* Creates /type file (mandatory). */
	if ((result = dth_register_file(elem, parent, DTH_TYPE)) < 0) {
		fprintf(stderr,
			"dth_register_element failure: unable to register type file for element %s\n",
			elem->path);
		return -1;
	}

	/* Creates /desc file (mandatory). */
	result = dth_register_file(elem, parent, DTH_DESC);
	    if (result < 0) {
		fprintf(stderr,
			"dth_register_element failure: unable to register type file for element %s\n",
			elem->path);
		return -1;
	}

	/* Creates /enum or /min /max files if needed. */
	if (elem->enumString != NULL) {
		result = dth_register_file(elem, parent, DTH_ENUM);
		if (result < 0) {
			fprintf(stderr,
				"dth_register_element failure: unable to register enum file for element %s\n",
				elem->path);
			return -1;
		}
	} else if ((elem->min != 0) || (elem->max != 0)) {
		result = dth_register_file(elem, parent, DTH_MIN);
		if (result < 0) {
			fprintf(stderr,
				"dth_register_element failure: unable to register min file for element %s\n",
				elem->path);
			return -1;
		}

		result = dth_register_file(elem, parent, DTH_MAX);
		if (result < 0) {
			fprintf(stderr,
				"dth_register_element failure: unable to register max file for element %s\n",
				elem->path);
			return -1;
		}
	}

	/* Creates /info file if necessary. */
	if (elem->info != NULL) {
		result = dth_register_file(elem, parent, DTH_INFO);
		if (result < 0) {
			fprintf(stderr,
				"dth_register_element failure: unable to register info file for element %s\n",
				elem->path);
			return -1;
		}
	}

	return result;
}

/**
 * Get the default value of a DTH element.
 * @param[in] elem specifies the DTH element.
 * @param[in,out] data specifies the buffer where the
 *					default value will be stored.
 * @param[in] count specifies the size of data buffer.
 * retval number of bytes written in data buffer.
 */
static int dth_get_default_value(const struct dth_element *elem, dth_u8 * data,
				 const dth_u32 count, int *error)
{
	dth_u8 *p;
	int size;
	int n;
	int result;

	size = dth_get_size_type(elem->type);
	result = 0;

	if ((elem->cols > 0) && (elem->rows > 0)) {	/* It is an array */
		struct dth_array *default_array;
		dth_s64 *intermediare;

		n = elem->cols * elem->rows * size;
		default_array = (struct dth_array *)elem->default_value;
		intermediare = (dth_s64 *) default_array->array;
		p = (dth_u8 *) intermediare;

		result = dth_write_data(p, data, count, n, error, 0);

	} else {		/* It is a string or a numeric value */
		if (elem->type == DTH_TYPE_STRING) {
			n = strlen((char *)elem->default_value);
			p = (dth_u8 *) elem->default_value;
			result = dth_write_data(p, data, count, n, error, 0);
		} else {
			dth_s64 *default_value;

			default_value = (dth_s64 *) elem->default_value;
			p = (dth_u8 *) default_value;

			result = dth_write_data(p, data, count, size, error, 0);
		}
	}

	return result;
}

void dth_get_minmax_string(const struct dth_element *elem, char** minmaxString)
{
	/* check type */
	switch (elem->type) {
	case DTH_TYPE_U8:
	case DTH_TYPE_U8B:
		sprintf(*minmaxString, "%u/%u", (dth_u8)elem->min, (dth_u8)elem->max);
		break;
	case DTH_TYPE_U16:
	case DTH_TYPE_U16B:
		sprintf(*minmaxString, "%u/%u", (dth_u16)elem->min, (dth_u16)elem->max);
		break;
	case DTH_TYPE_U32:
	case DTH_TYPE_U32B:
		sprintf(*minmaxString, "%u/%u", (dth_u32)elem->min, (dth_u32)elem->max);
		break;
	case DTH_TYPE_S8:
		sprintf(*minmaxString, "%i/%i", (dth_s8)elem->min,(dth_s8) elem->max);
		break;
	case DTH_TYPE_S16:
		sprintf(*minmaxString, "%i/%i", (dth_s16)elem->min,(dth_s16) elem->max);
		break;
	case DTH_TYPE_S32:
		sprintf(*minmaxString, "%i/%i", (dth_s32)elem->min,(dth_s32) elem->max);
		break;
	case DTH_TYPE_U64:
	case DTH_TYPE_U64B:
		sprintf(*minmaxString, "%llu/%llu", (dth_u64)elem->min, (dth_u64)elem->max);
		break;
	case DTH_TYPE_S64:
		sprintf(*minmaxString, "%lli/%lli", (dth_s64)elem->min, (dth_s64)elem->max);
		break;
	case DTH_TYPE_FLOAT:
		sprintf(*minmaxString, "%f/%f", (dth_float)elem->min, (dth_float)elem->max);
		break;
	case DTH_TYPE_STRING:
		sprintf(*minmaxString, "%i/%i", (dth_s32)elem->min, (dth_s32)elem->max);
		break;
	default:
		/* check bit field */
		if (elem->type >= DTH_BITFIELD_1_bits
		    && elem->type <= DTH_BITFIELD_8_bits) {
			sprintf(*minmaxString, "%u/%u",
					(dth_u8)elem->min, (dth_u8)elem->max);
		} else {
			if (elem->type <= DTH_BITFIELD_16_bits) {
				sprintf(*minmaxString, "%u/%u",
						(dth_u16)elem->min, (dth_u16)elem->max);
			} else {
				if (elem->type <= DTH_BITFIELD_32_bits) {
					sprintf(*minmaxString, "%u/%u",
							(dth_u32)elem->min, (dth_u32)elem->max);
				} else {
					if (elem->type <=
					    DTH_BITFIELD_64_bits) {
						sprintf(*minmaxString, "%lli/%lli",
								(dth_s64)elem->min, (dth_s64)elem->max);
					}
				}
			}
		}
		break;
	}
}

int dth_get_desc(const struct dth_element *elem, dth_u8 ** data,
				 int *error)
{
	char *typeString = NULL;
	char *descString = NULL;
	char *savedescString = NULL;
	char *minmaxString = NULL;
	int sepDescSize = strlen(sepDesc);
	int descStringLen = 0;

	/*Allocate max buffer size + 1 for '\0' */
	descString = calloc(DNPS_RREADDATASIZE+1, sizeof(char));

	/* Copy the string describing the DTH element type into data buffer. */
	if ((descString!=NULL) && (typeString =
	     dth_get_string_type(&typeString, elem->type, elem->cols,
				 elem->rows)) != NULL) {
		sprintf(descString, "%s%s%s", sepDesc, typeDesc,
			typeString);
#if defined(_DEBUG)
		fprintf(stdout, "%s add type\n",
				__FUNCTION__);
#endif
		/* min and max part*/
		if ((elem->min != 0) || (elem->max != 0)) {
			minmaxString = malloc(256);
			if (minmaxString == NULL)
				goto bad_pointer;
			dth_get_minmax_string(elem, &minmaxString);
			sprintf(descString, "%s%s%s%s", descString,
				sepDesc, minmaxDesc, minmaxString);
#if defined(_DEBUG)
			fprintf(stdout, "%s add min and max\n",
					__FUNCTION__);
#endif
		}
		descStringLen = strlen(descString);

		/* enum part*/
		if (elem->enumString != NULL) {
			if ((descStringLen + sepDescSize +
				strlen(enumDesc) + strlen(elem->enumString)) >
					DNPS_RREADDATASIZE) {
				savedescString = descString;
				descString =
					realloc(descString,
						descStringLen + sepDescSize +
						strlen(enumDesc) + strlen(elem->enumString) +
						1);
			}
			if (descString == NULL)
				goto bad_pointer;

			sprintf(descString, "%s%s%s%s", descString,
				sepDesc, enumDesc, elem->enumString);
			descStringLen = strlen(descString);
#if defined(_DEBUG)
			fprintf(stdout, "%s add enum (len=%i)\n",
					__FUNCTION__, strlen(elem->enumString));
#endif
		}
		/* info part*/
		if (elem->info != NULL) {
			if ((descStringLen + sepDescSize +
				strlen(infoDesc) + strlen(elem->info)) >
					DNPS_RREADDATASIZE) {
				savedescString = descString;
				descString =
					realloc(descString,
						descStringLen + sepDescSize +
						strlen(infoDesc) + strlen(elem->info) +
						1);
			}
			if (descString == NULL)
				goto bad_pointer;

			sprintf(descString, "%s%s%s%s", descString,
				sepDesc, infoDesc, elem->info);
			descStringLen = strlen(descString);
#if defined(_DEBUG)
			fprintf(stdout, "%s add info (len=%i)\n",
					__FUNCTION__, strlen(elem->info));
#endif
		}
	} else {
		goto bad_pointer;
	}
	free(typeString);
	*data = (dth_u8 *)descString;
	return descStringLen;

bad_pointer:
	*error = ENOMEM;
	free(typeString);
	if (savedescString)
		free(descString);
	printf("Bad pointer!\n");
	return -1;
}

int dth_helper_read(const struct dth_file *file, dth_u8 * data,
		    const dth_u64 offset, const dth_u32 count, int *error)
{
	struct dth_element *elem = NULL;
	int result = 0;
	int nb_bytes = 0;
	int n = -1;
	char *typeString = NULL;
	char *descString = NULL;

	/* reset message stack */
	dth_reset_message_pump();

	if (file != NULL)
		elem = (struct dth_element *)file->parent->file.service_data;

	if (elem == NULL)
		goto bad_pointer;

	/* For /type, /min, /max, /enum and /info files, the value to read
	   is the default value provides during registering phase. */
	if (dth_str_cmp(file->name, file->nlen, "enum") == 0) {
		/* Copy the string enum into data buffer. */
		nb_bytes = strlen(elem->enumString);
		n = dth_write_data((dth_u8 *) elem->enumString + offset, data,
				count, nb_bytes - offset, error, 1);
	} else if (dth_str_cmp(file->name, file->nlen, "desc") == 0) {

		nb_bytes = dth_get_desc(elem, (dth_u8**)&descString, error);
		if (nb_bytes >= 0) {
			/* Copy the string info into data buffer. */
			n = dth_write_data((dth_u8 *) descString + offset, data, count,
				nb_bytes - offset, error, 1);
			free(descString);
		}
	} else if (dth_str_cmp(file->name, file->nlen, "type") == 0) {

		if (offset != 0)
			goto offset_out;

		/* Copy the string describing the DTH element type into data buffer. */
		if (file != NULL) {
			typeString =
			    dth_get_string_type(&typeString, elem->type,
						elem->cols, elem->rows);
			if (typeString != NULL) {
				nb_bytes = strlen(typeString);
				n = dth_write_data((dth_u8 *) typeString, data, count,
						nb_bytes, error, 0);
				free(typeString);
			}
		}
	} else if (dth_str_cmp(file->name, file->nlen, "info") == 0) {
		/* Copy the string info into data buffer. */
		nb_bytes = strlen(elem->info);
		n = dth_write_data((dth_u8 *) elem->info + offset, data,
				count, nb_bytes - offset, error, 1);
	} else if (dth_str_cmp(file->name, file->nlen, "min") == 0) {
		if (offset == 0) {
			/* Copy the min value into data buffer. */
			int size = 0;

			if ((elem->type >= DTH_BITFIELD_PART_MIN_SIZE)
			    && (elem->type <= DTH_BITFIELD_PART_MAX_SIZE))
				size = sizeof(dth_s64);
			else
				size = dth_get_size_type(elem->type);

			n = dth_write_data((dth_u8 *) & elem->min, data, count,
					   size, error, 0);
		} else {
			goto offset_out;
		}
	} else if (dth_str_cmp(file->name, file->nlen, "max") == 0) {
		if (offset == 0) {
			/* Copy the max value into data buffer. */
			int size;

			if ((elem->type >= DTH_BITFIELD_PART_MIN_SIZE)
			    && (elem->type <= DTH_BITFIELD_PART_MAX_SIZE))
				size = sizeof(dth_s64);
			else
				size = dth_get_size_type(elem->type);

			n = dth_write_data((dth_u8 *) & elem->max, data, count,
					   size, error, 0);
		} else {
			goto offset_out;
		}
	} else if (dth_str_cmp(file->name, file->nlen, "value") == 0) {
		/* By reading the /value file, it can be possible to:
		   - to read a content of a file associated to a DTH file element
		   - call the exec callback if the DTH element is a DTH Action.
		   - get the default value of the DTH element.
		   - call the get callback to read data from DTH services.
		 */
		/* Access into a  DTH file  element. */
		if (elem->type == DTH_TYPE_FILE) {
			struct dth_file_data myFile;
#if defined(_DEBUG)
			printf("%s : Read access into a DTH file\n", __FILE__);
#endif
			myFile.pathname = NULL;	/* unused here */
			myFile.offset = offset;
			myFile.count = count;
			myFile.data = (char *)data;

			/* Call DTH service callback. */
			result = elem->get(elem, (void *)&myFile);
			if (result == 0)
				n = count;
			else
				*error = (result>0)?result:EIO;

		} else if (elem->exec != NULL) {
			/* It's a DTH action. */
#if defined(_DEBUG)
			printf
			    ("%s : Call exec callback to launch the execution of the DTH Action\n",
			     __FILE__);
#endif
			result = elem->exec(elem);
			if (result == 0)
				n = 0;	/* No Data. */
			else
				*error = (result>0)?result:EIO;
		} else if (elem->get == NULL) {
			/* Copy the default value into data buffer. */
			if (offset != 0)
				goto offset_out;

			n = dth_get_default_value(elem, data, count, error);

		} else {
			/* The value to read is provided by calling DTH callback. */
			if ((elem->cols > 0) && (elem->rows > 0)) {
				/* Access to a numeric array DTH element. */
				struct dth_array myArray = {
					0, 0, NULL
				};

#if defined(_DEBUG)
				printf
				    ("%s : Read acess to a DTH array\n",
				     __FILE__);
#endif

				myArray.array = (dth_s64 *) data;
				nb_bytes =
				    dth_prepare_array(elem, &myArray, offset,
						      count);
				if (nb_bytes >= 0) {
#if defined(_DEBUG)
					printf
					    ("%s : Call DTH service callback to get values(s) from an array\n",
					     __FILE__);
#endif
					result =
					    elem->get(elem, (void *)&myArray);
					if (result == 0)
						n = nb_bytes;
					else
						*error = (result>0)?result:EIO;
				} else {
					goto offset_out;
				}
			} else {
				if (offset == 0) {
					/*Access to a string or a numeric DTH element. */
#if defined(_DEBUG)
					printf
					    ("%s : Call DTH service callback to get a string or a numeric value\n",
					     __FILE__);
#endif
					result = dth_check_size(elem, count);
					if (result == 0) {
						result = elem->get(elem, data);
						if (result == 0) {
							if (elem->type ==
							    DTH_TYPE_STRING)
								n = strlen((char *)
								   data);
							else
								n = dth_get_size_type
									(elem->type);
						} else {
							*error = (result>0)?result:EIO;
						}

					} else {
						*error = (result>0)?result:EIO;
					}
				} else {
					goto offset_out;
				}
			}
		}
	} else {
		goto filename_error;
	}

	return n;

offset_out:
	return 0;		/* No data */

bad_pointer:
	*error = ENOMEM;
	printf("Bad pointer!\n");
	return -1;

filename_error:
	*error = ENOENT;
	return -1;		/*No such file or directory */

}

int dth_register_element(struct dth_element *elem)
{
	int result = 0;

	if (dth_publish_is_enabled())
		result = dth_publish_element(elem);
	else
		result = _register_element(elem);

	return result;
}
