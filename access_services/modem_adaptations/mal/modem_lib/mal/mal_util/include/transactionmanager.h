/*
 * Copyright (C) ST-Ericsson AB 2010
 * Author: Christian Bejram <christian.bejram@stericsson.com>
 */

#ifndef _TRANSACTION_MANAGER_H
#define _TRANSACTION_MANAGER_H

typedef void *TRANSACTION_MANAGER_HANDLE;

TRANSACTION_MANAGER_HANDLE transaction_manager_create();
void transaction_manager_destroy(TRANSACTION_MANAGER_HANDLE handle);

unsigned char transaction_manager_new_transaction(TRANSACTION_MANAGER_HANDLE handle, void *client_tag);
void *transaction_manager_get_client_tag(TRANSACTION_MANAGER_HANDLE handle, unsigned char transaction_id);
void transaction_manager_delete_transaction(TRANSACTION_MANAGER_HANDLE handle, unsigned char transaction_id);

// TODO: Put inside "debug" ifdef

/* MAL tstu code changes - START */

#ifndef MAL_CONDITIONAL_COMPILE
void debug_transaction_manager_dump_transaction_list(TRANSACTION_MANAGER_HANDLE handle);
#endif
/* MAL tstu code changes - END */

#endif // #ifndef _TRANSACTION_MANAGER_H
