/************************************************************************
 *                                                                      *
 *  Copyright (C) 2011 ST-Ericsson SA                                   *
 *                                                                      *
 *  This software is released under the terms of the BSD-style          *
 *  license accompanying TOC.                                           *
 *                                                                      *
 *  Author: Peter Nessrup <peter.nessrup@stericsson.com>                *
 *  Author: Mikael Larsson <mikael.xt.larsson@stericsson.com>           *
 *                                                                      *
 ************************************************************************/

#ifndef _TOC_PARSER_H_
#define _TOC_PARSER_H_

#include <stdint.h>

#define TOC_ID_LENGTH           12
#define TOC_BOOT_IMAGE_SIZE     0x20000

typedef enum {
	TOCPARSER_OK = 0,
	TOCPARSER_NOT_INITIALIZED,
	TOCPARSER_OUT_OF_MEMORY,
	TOCPARSER_DEVICE_OPEN_FAILED,
	TOCPARSER_OPEN_FAILED,
	TOCPARSER_INIT_FAILED,
	TOCPARSER_READ_FAILED,
	TOCPARSER_SEEK_FAILED,
	TOCPARSER_WRITE_FAILED,
	TOCPARSER_ENTRY_NOT_FOUND,
	TOCPARSER_GET_ENTRY_FAILED,
	TOCPARSER_SET_ENTRY_FAILED,
	TOCPARSER_CLOSE_DEV_FAILED,
	TOCPARSER_ERR_INVALID_PARAM,
	TOCPARSER_ERR_READONLY,
	TOCPARSER_ERR_INVALID_TOC,
	TOCPARSER_ERR_OUT_OF_SPACE,
	TOCPARSER_ADD_ENTRY_FAILED
} tocparser_error_code_t;

typedef enum {
	TOCPARSER_READONLY,
	TOCPARSER_READWRITE,
	TOCPARSER_EMPTY_TOC
} tocparser_mode_t;

typedef struct {
	uint32_t offset;
	uint32_t size;
	uint32_t flags;
	uint32_t align;
	uint32_t loadaddr;
	uint8_t  id[TOC_ID_LENGTH];
} __attribute((packed)) tocparser_toc_entry_t;

typedef struct tocparser_handle_t tocparser_handle_t;

/**
 * tocparser_init() Init handle and populates toc
 * environment variable TOCPARSER_DEBUG set to 1 will enable debug traces
 *
 * @param [in] dev Path to device to operate on.
 * @param [in] mode Select the open mode.
 * @param [out] handle Handle to the tocparser.
 * @return Returns tocparser error code.
 */
tocparser_error_code_t tocparser_init(char *dev,
                                      tocparser_mode_t mode,
				      tocparser_handle_t **handle);

/**
 * tocparser_uninit() Uninit handle and free memory
 *
 * @param [in] handle Handle to the tocparser.
 * @return Returns tocparser error code.
 */
tocparser_error_code_t tocparser_uninit(tocparser_handle_t *handle);

/**
 * tocparser_get_toc_entry() Get toc entry from the specified toc partition
 *
 * @param [in] handle Handle to the tocparser.
 * @param [in] toc_id Name of toc partition (NULL terminated string).
 * @param [in/out] offset Toc start addr (in bytes).
 * @param [in/out] size Toc Size (in bytes).
 * @param [in/out] loadaddr Toc Load Addr (in bytes).
 * @return Returns tocparser error code.
 */
tocparser_error_code_t tocparser_get_toc_entry(tocparser_handle_t *handle,
					       const char *toc_id,
					       uint32_t *offset,
					       uint32_t *size,
					       uint32_t *loadaddr);

tocparser_error_code_t tocparser_get_toc_entry_64(tocparser_handle_t *handle,
						  const char *toc_id,
						  uint64_t *offset,
						  uint64_t *size,
						  uint32_t *loadaddr);

/**
 * tocparser_set_toc_entry() Set toc entry values for the specified toc
 *                           partition. This operation will update the flash
 *                           content.
 *
 * @param [in] handle Handle to the tocparser.
 * @param [in] toc_id Name of toc partition (NULL terminated string).
 * @param [in] size Toc Size (in bytes), NULL keeps old value.
 * @param [in] loadaddr Toc Load Addr (in bytes), NULL keeps old value.
 * @return Returns tocparser error code.
 */
tocparser_error_code_t tocparser_set_toc_entry(tocparser_handle_t *handle,
					       const char *toc_id,
					       uint32_t *size,
					       uint32_t *loadaddr);

/**
 * tocparser_get_toc_entry() Get toc partition entry from the specified toc
 *                           partition
 *
 * @param [in] handle Handle to the tocparser.
 * @param [in] toc_id Name of toc partition (NULL terminated string).
 * @param [in/out] offset Toc start addr (in bytes).
 * @param [in/out] size Toc Size (in bytes).
 * @return Returns tocparser error code.
 */
tocparser_error_code_t tocparser_get_toc_partition(tocparser_handle_t *handle,
					           const char *toc_id,
					           uint32_t *offset,
					           uint32_t *size);

tocparser_error_code_t tocparser_get_toc_partition_64(tocparser_handle_t *handle,
						      const char *toc_id,
						      uint64_t *offset,
						      uint64_t *size);

/**
 * tocparser_load_toc_entry() Loads data from the specified toc partition
 *
 * @param [in] handle Handle to the tocparser.
 * @param [in] toc_id Name of toc partition (NULL terminated string).
 * @param [in] offset Offset into toc partition (in bytes).
 * @param [in] size Size of data to read (in bytes). If 0 entire toc partion
 *		    will be loaded.
 * @param [in] loadaddr Destination address for loaded data. If 0 then address
 *			from toc will be used.
 * @return Returns tocparser error code.
 */
tocparser_error_code_t tocparser_load_toc_entry(tocparser_handle_t *handle,
						const char *toc_id,
						uint32_t offset,
						uint32_t size,
						uint32_t loadaddr);

tocparser_error_code_t tocparser_load_toc_entry_64(tocparser_handle_t *handle,
						   const char *toc_id,
						   uint64_t offset,
						   uint32_t size,
						   uint32_t loadaddr);

/**
 * tocparser_write_toc_entry_from_fd() Write data to a pre-existing partition
 *				       from a file. It is only possible to write
 *				       to an existing partition. The size of the
 *				       data written is limited to the size of
 *				       the corresponding partition-entry.
 *
 * @param [in] handle Handle to the tocparser.
 * @param [in] toc_id Name of toc partition (NULL terminated string).
 * @param [in] filedescriptor filedescriptor to read data from.
 * @param [in] size Size of data to read (in bytes).
 * @param [in] loadaddr Toc Load Addr (in bytes), NULL keeps old value.
 * @return Returns tocparser error code.
 */
tocparser_error_code_t tocparser_write_toc_entry_from_fd(
						tocparser_handle_t *handle,
						const char *toc_id,
						int filedescriptor,
						uint32_t size,
						uint32_t *loadaddr);
/**
 * tocparser_write_toc_entry_from_buffer() Write data to a pre-existing
 *					   partition from a buffer. It is only
 *					   possible to write to an existing
 *					   parition. The size of the data
 *					   written is limited to the size of the
 *					   corresponding partition-entry.
 *
 * @param [in] handle Handle to the tocparser.
 * @param [in] toc_id Name of toc partition (NULL terminated string).
 * @param [in] data Buffer to read data from.
 * @param [in] size Size of data to read (in bytes).
 * @param [in] loadaddr Toc Load Addr (in bytes), NULL keeps old value.
 * @return Returns tocparser error code.
 */

tocparser_error_code_t tocparser_write_toc_entry_from_buffer(
						tocparser_handle_t *handle,
						const char *toc_id,
						uint8_t *data,
						uint32_t size,
						uint32_t *loadaddr);

/**
 * tocparser_print_toc() Prints the entire TOC including subtocs
 *
 * @param [in] handle Handle to the tocparser.
 */
void tocparser_print_toc(tocparser_handle_t *handle);

/**
 * tocparser_add_toc_entry() Add TOC entry to the handle, this operation will
 *                           not update the storage device content.
 *                           This function is used by loaders during flashing.
 *
 * @param [in] handle Handle to the tocparser.
 * @param [in] toc toc content (Packed in TOC format structure).
 * @param [in] subtoc subtoc content if exist (Packed in TOC format structure).
 * @param [in] toc_position The required position of the toc entry.
 * @return Returns tocparser error code.
 */

tocparser_error_code_t tocparser_add_toc_entry(
					tocparser_handle_t *handle,
					tocparser_toc_entry_t *toc,
					tocparser_toc_entry_t *subtoc,
					uint8_t *toc_position);

/**
 * tocparser_remove_toc_entry() Removes toc entry from the toc
 *
 * @param [in] handle Handle to the tocparser.
 * @param [in] toc_id Name of toc partition (NULL terminated string).

 * @return Returns tocparser error code.
 */

tocparser_error_code_t tocparser_remove_toc_entry(
					tocparser_handle_t *handle,
					const char *toc_id);

/**
 * tocparser_write_boot_and_toc() Writes current TOC sector and boot
 *                                area on flash
 *
 * @param [in] handle Handle to the tocparser.
 *
 * @return Returns tocparser error code.
 */

tocparser_error_code_t tocparser_write_boot_and_toc(
					tocparser_handle_t *handle);

/**
 * tocparser_err2str() Convert a tocparser_error_code_t to the corresponding str
 *
 * @param [in] error Error code to convert.
 * @return Returns error as string.
 */

const char *tocparser_err2str(tocparser_error_code_t error);

#endif	/* _TOC_PARSER_H_ */
