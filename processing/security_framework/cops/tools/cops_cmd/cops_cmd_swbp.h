/*************************************************************************
 * Copyright ST-Ericsson 2012
 *
 *  This file is only used in OSE environment
 ************************************************************************/
#ifndef COPS_CMD_SWBP_H
#define COPS_CMD_SWBP_H

#include <r_idbg.h>

IDBG_TBL_EXTERN(cops_swbp_commands)

void PrintHexBuffer(const char *BufferName_p,
                    const void *Buffer_p,
                    size_t  BufferLength);

/* Print message in IDBG window */
#define COPS_IDBG_PRINT(msg, ...) \
    do { \
        (void)Request_IDbg_Printf(WAIT_RESPONSE, msg, ##__VA_ARGS__); \
        (void)Request_IDbg_Printf(WAIT_RESPONSE, "\n"); \
    } while (0)

#define COPS_IDBG_HEX(BufferName, Buffer_p, BufferLength) \
            PrintHexBuffer(BufferName, Buffer_p, BufferLength)


#endif /* COPS_CMD_SWBP_H */
