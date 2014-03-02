#ifndef _MSA_H
#define _MSA_H

/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#define VERSION "1.0"

#define MSA_VERSION_REPORT "Version: " VERSION \
	" Compiled: " __DATE__ " " __TIME__

#ifndef RPC_DEVICE_FILE
#define RPC_DEVICE_FILE "/dev/rpc"
#endif

#ifndef MSA_ROOT_DEFAULT
#define MSA_ROOT_DEFAULT "/modemfs"
#endif

/* This file will be placed in root directory. It will be created as
copy of the target file whenever MSA receives a write request. The write should
be atomic and the only way to guarantee this is to copy the file, update it
with the write and finally use the POSIX atomic rename operation to put it in
place, overwriting the existing old file atomicly.

This file should have a name longer than 12 chars. This is because the FSA
protocol can't handle file names longer than 12 chars and are ignoring such
files in readdir() operations.

The file will only exist during the write operation, it will otherwise not
exist. It has either finally been renamed to overwrite the old version of
the file, or was removed during its creation on any error. If it exist when
MSA starts, the former write operation had an power failure (or MSA server was
externally killed). MSA will in such case simply remove the temporary
MSA_WRITE_ATOMIC_FILE and we have a rollback of the incompleted write operation.
The file will always be removed upon start of MSA (if it did exist due to a
power failure in former write operation).
*/
#ifndef MSA_WRITE_ATOMIC_FILE
#define MSA_WRITE_ATOMIC_FILE ".msa_write_atomic.file"
#endif

#endif // _MSA_H

