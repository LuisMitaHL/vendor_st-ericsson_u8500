/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   9P interfaces for DTH server helper
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTH_SRVHELPER_INTERNAL_H_
#define DTH_SRVHELPER_INTERNAL_H_

#include <dthsrvhelper/dthsrvhelper.h>

#define MAX_SIZE_STRING		100

#define LIB_DTHSRVHELPER	"libdthsrvhelper.so"

#define DTH_HLP_INIT		"dth_helper_init"

#define DTH_INIT_SERV		"dth_init_service"
#define DTH_UNINIT_SERV		"dth_uninit_service"
#define DTH_PUSH_MSG		"dth_push_message"
#define DTH_RST_MSG			"dth_reset_message_pump"
#define DTH_GET_MSG			"dth_get_message_pump"

#define PLUGINS_PATH		"/usr/lib/tat/plugins"
#define MAX_TAT_MODULES     50

/* 9P common header size*/
#define NP_IOHDRSZ			7
/* Max size of the 9p message (including protocol data)*/
#define TNPS_MSIZE			4120
/* Maximum data message size without 9p header size.*/
#define DNPS_MSIZE			TNPS_MSIZE - NP_IOHDRSZ
/* 9p rread header size*/
#define DNPS_RREADSIZE		(NP_IOHDRSZ + 4)
/* 9p twrite header size*/
#define DNPS_TWRITESIZE		(NP_IOHDRSZ + 16)
/* Maximum 9p rread data */
#define DNPS_RREADDATASIZE	TNPS_MSIZE - DNPS_RREADSIZE
/**
 *
 * dth_file_type
 *
 * The dth_file_type enumeration defines all 9p type files.
 *
 */
enum dth_file_type {
	DTH_VALUE = 0,
	DTH_TYPE,
	DTH_ENUM,
	DTH_INFO,
	DTH_MIN,
	DTH_MAX,
	DTH_DESC
};

struct dth_directory *rootDir;

const struct dth_file_ops dth_helper_file_ops;

const struct dth_file_ops dth_helper_file_elem_generic_ops;

/**
 * Register a new element in DTH virtual file system.
 * @param[in] dth file.
 * @param[in] read datas.
 * @param[in] offset in file.
 * @param[in] size of data that should be read.
 * @param[in] error.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int dth_helper_read(const struct dth_file *file, dth_u8 * data,
		    const dth_u64 offset, const dth_u32 count, int *error);

/**
 * Register a new element in DTH virtual file system.
 * @param[in] dth file.
 * @param[in] read datas.
 * @param[in] offset in file.
 * @param[in] size of data that should be read.
 * @param[in] error.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int dth_helper_write(struct dth_file *file, const dth_u8 * data, const dth_u64 offset,
		     const dth_u32 count, int *error);

/**
 * Register a new element in DTH virtual file system.
 * @param[in] dth file.
 * @param[in] read datas.
 * @param[in] offset in file.
 * @param[in] size of data that should be read.
 * @param[in] error.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int dth_helper_file_read(const struct dth_file *file, dth_u8 * data,
			 const dth_u64 offset, const dth_u32 count, int *error);

/**
 * Register a new element in DTH virtual file system.
 * @param[in] dth file.
 * @param[in] read datas.
 * @param[in] offset in file.
 * @param[in] size of data that should be read.
 * @param[in] error.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int dth_helper_file_write(struct dth_file *file, const dth_u8 * data,
			  const dth_u64 offset, const dth_u32 count, int *error);

/**
 * Initialize the DTH helper library.
 * @param[in]	root	specifies the root directory of the DTH tree.
 */
inline int dth_helper_init();

/**
 * Uninitialize the DTH helper library.
 */
inline void dth_helper_uninit();

#ifdef _UNUSED
/**
 * Check if DTH element is already registered in VFS.
 * @param[in] elem specifies the DTH element.
 * @param[in] parent specifies the parent directoy of the DTH element.
 * @retval 0 the element is not registered.
 * @retval -1 the element is already registered.
 */
static int dth_check_element_in_VFS(struct dth_element *elem,
		struct dth_directory *parent);

/**
 * Check if the  DTH value validity.
 * @param[in] elem specifies the DTH element.
 * @param[in] data specifies the value to write.
 * @param[in] nb_enum specifies the number of enumeration values.
 * @retval 0 the value is valid.
 * @retval -1 the value is not valid.
 */
static int dth_check_directories(struct dth_element *elem,
		dth_u8 *data, int nb_enum);

#endif


#endif /* DTH_SRVHELPER_INTERNAL_H_ */

