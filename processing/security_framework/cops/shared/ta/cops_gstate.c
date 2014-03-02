#include <stdlib.h>
#include <cops_gstate.h>

static void *gstate_pointers[TEE_GID_CUSTOM_OFFSET];  /* initialized to NULLs */

void *ISSWAPI_GSTATE_ALLOC(int tgid, size_t len)
{
    if (tgid >= TEE_GID_CUSTOM_OFFSET || (int)tgid < 0 ||
        gstate_pointers[tgid] != NULL)
        return NULL;

    gstate_pointers[tgid] = calloc(1, len);
    return gstate_pointers[tgid];
}

void ISSWAPI_GSTATE_FREE(int tgid)
{
    if (tgid >= TEE_GID_CUSTOM_OFFSET || (int)tgid < 0 ||
        gstate_pointers[tgid] == NULL)
        return;

    free(gstate_pointers[tgid]);
    gstate_pointers[tgid] = NULL;
}

void *ISSWAPI_GSTATE_GET(int tgid)
{
    if (tgid >= TEE_GID_CUSTOM_OFFSET || (int)tgid < 0)
        return NULL;
    return gstate_pointers[tgid];
}
