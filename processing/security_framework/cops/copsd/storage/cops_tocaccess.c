/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_common.h>
#include <cops_tocaccess.h>
#ifdef COPS_TOCACCESS_SUPPORT
#include <toc_parser.h>

#define BLOCK_DEVICE_NAME "/dev/block/mmcblk0"

static tocparser_handle_t *tocparserhandle = 0;
#endif

cops_return_code_t cops_open_tochandler(void)
{
    cops_return_code_t ret_code = COPS_RC_OK;
#ifdef COPS_TOCACCESS_SUPPORT
    tocparser_error_code_t result;

    result = tocparser_init(BLOCK_DEVICE_NAME, TOCPARSER_READWRITE,
                            &tocparserhandle);

    if (TOCPARSER_OK != result) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Failed to init tocparser (%d)!\n", result);
    }

function_exit:
#endif
    return ret_code;
}

cops_return_code_t
cops_write_tocdata(char TOCid[12], void *buffer, size_t length)
{
    cops_return_code_t ret_code = COPS_RC_OK;
#ifdef COPS_TOCACCESS_SUPPORT
    tocparser_error_code_t result;
    char *tocid = 0;

    if (buffer == NULL || length == 0) {
        COPS_SET_RC(COPS_RC_ARGUMENT_ERROR,
                    "no data or no length specified!\n");
    }

    result = tocparser_write_toc_entry_from_buffer(tocparserhandle, TOCid,
                                                   buffer, length, NULL);

    if (TOCPARSER_OK != result) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Failed to write to tocentry (%d)!\n", result);
    }

function_exit:
#endif
    return ret_code;
}

cops_return_code_t cops_close_tochandler(void)
{
    cops_return_code_t ret_code = COPS_RC_OK;
#ifdef COPS_TOCACCESS_SUPPORT
    tocparser_error_code_t result;

    result = tocparser_uninit(tocparserhandle);

    if (TOCPARSER_OK != result) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Failed to close tocparser (%d)!\n", result);
    }

    tocparserhandle = 0;

function_exit:
#endif
    return ret_code;
}
