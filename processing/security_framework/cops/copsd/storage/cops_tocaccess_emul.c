/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_tocaccess.h>
#include <cops_common.h>
#include <stdio.h>
#include <errno.h>

cops_return_code_t cops_open_tochandler(void)
{
    return COPS_RC_OK;
}

cops_return_code_t
cops_write_tocdata(char TOCid[12], void *buffer, size_t length)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    size_t written;
    FILE *swvertfp = NULL;

    if (buffer == NULL || length == 0) {
        COPS_SET_RC(COPS_RC_ARGUMENT_ERROR,
                    "no data or no length specified!\n");
    }

    swvertfp = fopen(TOCid, "w+");

    if (swvertfp == NULL) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Failed to open file errno: %d\n", errno);
    }

    written = fwrite(buffer, 1, length, swvertfp);

    if (written * 1 != length) {
        COPS_SET_RC(COPS_RC_STORAGE_ERROR,
                    "Write of sw version table failed\n");
    }

function_exit:
    if (swvertfp != NULL) {
        fclose(swvertfp);
    }

    return ret_code;
}

cops_return_code_t cops_close_tochandler(void)
{
    return COPS_RC_OK;
}
