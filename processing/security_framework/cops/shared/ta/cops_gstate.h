#ifndef COPS_GSTATE_H
#define COPS_GSTATE_H

/* Allocates memory in the global heap.
 *
 * If the memory already is allocated for the gid, NULL is
 * returned.  Should actually generate some kind of error so we
 * can capture it. This is a programming error.
 *
 * input: length - Length of buffer to allcoate
 * input: tgid   - the id of the global data buffer
 *
 * output: p - pointer to data allocated for id. NULL if failure
 */

void    *ISSWAPI_GSTATE_ALLOC(int tgid, size_t len);

/* Frees memory previously allocated. Verifies the address
 * towards the supplied gid.
 *
 * input: tgid - the id of the global data allocated.
 */

void     ISSWAPI_GSTATE_FREE(int tgid);

/* Returns a previously allocated pointer to global storage.
 * NULL if it doesn't exist
 *
 * input: tgid - the id of the global data allocated.
 */

void    *ISSWAPI_GSTATE_GET(int tgid);

#define TEE_GID_COPS_SIPC_MODEM_COUNTER 6 /* message counter, modem -> APE */
#define TEE_GID_COPS_SIPC_APE_COUNTER   7 /* message counter, APE -> modem */
/* message counter, modem -> relay */
#define TEE_GID_COPS_SIPC_MODEM_COUNTER_INTERNAL 8
 /* message counter, APE -> relay */
#define TEE_GID_COPS_SIPC_APE_COUNTER_INTERNAL   9
#define TEE_GID_CUSTOM_OFFSET           10

#endif /* COPS_GSTATE_H */
