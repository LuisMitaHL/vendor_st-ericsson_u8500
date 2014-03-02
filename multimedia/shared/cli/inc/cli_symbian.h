/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     cli_symbian.h
 * \brief    CLI API for SYMBIAN only
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#ifndef __CLI_SYMBIAN_h_
#define __CLI_SYMBIAN_h_


#ifdef __cplusplus
extern "C"
{
#endif

// Pointer to function for handling console writing into Symbian
typedef void (* CliWriteConsoleSymbianPtr)(int, char *);

// Structure to initialise some Symbian parameters into MMTE
typedef struct
{
    CliWriteConsoleSymbianPtr iCliWriteConsoleSymbianPtr;
} TCliSymbianInitParams;


IMPORT_C void CLI_Init_Symbian(TCliSymbianInitParams*);

void CLI_write_console_symbian(int, char*);

#ifdef __cplusplus
}
#endif

#endif
