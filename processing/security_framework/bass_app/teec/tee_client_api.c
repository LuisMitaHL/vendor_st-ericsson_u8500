/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include <tee_client_api.h>
#include <tee.h>
#include <tee_ta.h>
#include <debug.h>
#include <bass_app.h>
#include <uuid.h>

#define TEE_CONTEXT_TZ 0

#ifndef TEEC_LOAD_PATH
#define TEEC_LOAD_PATH "/lib/tee"
#endif

#ifndef TEEC_DEV_PATH
#define TEEC_DEV_PATH "/dev/tee"
#endif

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

#define MIN(x, y) ((x) < (y)) ? (x) : (y)

#define TA_BINARY_FOUND 0
#define TA_BINARY_NOT_FOUND -1

#ifdef _GNU_SOURCE
static pthread_mutex_t mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
#else
static pthread_mutex_t mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER;
#endif

static uint8_t issw_ta_key_hash[SHA256_HASH_SIZE] = { 0 };
static bool got_ta_key_hash = false;

/*
 * This is a list of static TA's that exist in secure world. We maintain this
 * list here so we can return an error in TEEC_OpenSession if a client tries to
 * call a trusted application with an unknown uuid.
 */
static TEEC_UUID ta_static_uuid_list[] = {
    STATIC_TA_UUID
};

static void copy_tee_op_to_legacy(TEEC_Operation *dest,
                                  const struct tee_operation *src)
{
    size_t i;

    if (src == NULL || dest == NULL) {
        dprintf(ERROR, "NULL parameter as inparameter src: 0x%08x, dest: "
                "0x%08x\n", (uint32_t)src, (uint32_t)dest);
        return;
    }

    for (i = 0; i < TEEC_CONFIG_PAYLOAD_REF_COUNT; i++) {
        dest->memRefs[i].buffer = src->shm[i].buffer;
        dest->memRefs[i].size = src->shm[i].size;
        dest->memRefs[i].flags = src->shm[i].flags;
    }
    dest->flags = src->flags;
}

static void copy_tee_op_from_legacy(struct tee_operation *dest,
                                    const TEEC_Operation *src)
{
    size_t i;

    if (src == NULL || dest == NULL) {
        dprintf(ERROR, "NULL parameter as inparameter src: 0x%08x, dest: "
                "0x%08x\n", (uint32_t)src, (uint32_t)dest);
        return;
    }

    for (i = 0; i < TEEC_CONFIG_PAYLOAD_REF_COUNT; i++) {
        dest->shm[i].buffer = src->memRefs[i].buffer;
        dest->shm[i].size = src->memRefs[i].size;
        dest->shm[i].flags = src->memRefs[i].flags;
    }
    dest->flags = src->flags;
}

static bool get_size_of_file(FILE *file, size_t *size)
{
    long tmp_size;

    if (fseek(file, 0, SEEK_END) != 0) {
        dprintf(ERROR, "fseek file failed\n");
        return false;
    }
    tmp_size = ftell(file);
    if (tmp_size >= 0) {
        *size = tmp_size;
    } else {
        dprintf(ERROR, "failed to get size of file\n");
        return false;
    }
    return true;
}

static bool get_header_size(FILE *file, uint16_t *signed_header_size)
{
    /* Get the offset to the signed header element */
    if (fseek(file, offsetof(struct ta_signed_header,
                             size_of_signed_header), SEEK_SET) != 0) {
        dprintf(ERROR, "failed to get the signed header offset\n");
        return false;
    }

    /* Read the signed header size */
    if (fread(signed_header_size, 1, sizeof(uint16_t), file) !=
        sizeof(uint16_t)) {
        dprintf(ERROR, "failed to read the header size\n");
        return false;
    }
    return true;
}

static bool get_file_size_in_header(FILE *file, size_t *size)
{
    uint16_t header_size;
    uint32_t payload_size;
    struct tee_ta_head ta_header;

    if (get_header_size(file, &header_size) == false) {
        dprintf(ERROR, "get header size failed\n");
        return false;
    }
    /* Get the offset to the payload size in the signed header element */
    if (fseek(file, offsetof(struct ta_signed_header,
                             size_of_payload), SEEK_SET) != 0) {
        dprintf(ERROR, "failed to get payload size offset\n");
        return false;
    }
    /* Read the payload size */
    if (fread(&payload_size, 1, sizeof(uint32_t), file) !=
        sizeof(uint32_t)) {
        dprintf(ERROR, "failed to read payload size\n");
        return false;
    }

   /* Set file to point to the ta_header */
    if (fseek(file, header_size, SEEK_SET) != 0) {
        dprintf(ERROR, "failed to point to ta header\n");
        return false;
    }

    /* Read the ta_header from file */
    if (fread((void *) &ta_header, 1, sizeof(ta_header), file) !=
        sizeof(ta_header)) {
        dprintf(ERROR, "failed to read ta header\n");
        return false;
    }

    *size = header_size + payload_size + ta_header.ro_size + ta_header.rw_size;
    return true;

}

/*
 * This function checks if there is a hash of the key used when signing
 * the ta attached to the end of the TA. If there is, it compares with
 *  current ta-key-hash to check if this is the same.
 */
static bool compare_ta_key_hash(FILE *file)
{
    size_t total_file_size;
    size_t size_from_file;
    uint8_t ta_key_hash_from_ta[SHA256_HASH_SIZE];

    if ((get_size_of_file(file, &total_file_size) == false) ||
        (get_file_size_in_header(file, &size_from_file) == false)) {
        dprintf(ERROR, "failed to get file size\n");
        return false;
    }

    if (total_file_size != (size_from_file + SHA256_HASH_SIZE)) {
        dprintf(FLOOD, "no ta key hash attached to the TA.\n");
        return true;
    }

   /* Set file to point to the ta key hash */
    if (fseek(file, size_from_file, SEEK_SET) != 0) {
        return false;
    }

    /* Read the ta key hash from file */
    if (fread((void *) &ta_key_hash_from_ta, 1, SHA256_HASH_SIZE, file) !=
        SHA256_HASH_SIZE) {
        return false;
    }

    pthread_mutex_lock(&mutex);
    if (memcmp(issw_ta_key_hash, ta_key_hash_from_ta, SHA256_HASH_SIZE)) {
        dprintf(FLOOD, "not correct ta\n");
        pthread_mutex_unlock(&mutex);
        return false;
    }
    pthread_mutex_unlock(&mutex);

    return true;
}



static bool check_uuid(const TEEC_UUID *destination, FILE *file)
{
    TEEC_UUID uuid;
    uint16_t signed_header_size;


    if (get_header_size(file, &signed_header_size) == false)
        return false;

    /* Set file to point to the uuid */
    if (fseek(file, signed_header_size +
              offsetof(struct tee_ta_head, uuid), SEEK_SET) != 0) {
        dprintf(ERROR, "fail to point to uuid\n");
        return false;
    }

    /* Read the uuid from file */
    if (fread((void *) &uuid, 1, sizeof(TEEC_UUID), file) !=
        sizeof(TEEC_UUID)) {
        dprintf(ERROR, "not a valid ta\n");
        return false;
    }

    /* Check if it is the TA that we are looking for. */
    if (memcmp(&uuid, destination, sizeof(TEEC_UUID)) != 0) {
        dprintf(FLOOD, "wrong ta\n");
        return false;
    }
    return true;
}

static bool create_and_populate_ta(FILE *file, void **ta, size_t *ta_size)
{

    /* Get the correct size. */
    if (get_file_size_in_header(file, ta_size) == false) {
        return false;
    }

    dprintf(FLOOD, "ta_size is %zu\n", *ta_size);
    /* Allocate and read the ta into the provided pointer. */
    *ta = malloc(*ta_size);

    if (*ta == NULL) {
        dprintf(INFO, "failed allocating ta\n");
        return false;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        dprintf(ERROR, "fseek file failed\n");
        return TEEC_ERROR_GENERIC;
    }

    if (*ta_size != fread(*ta, 1, *ta_size, file)) {
        dprintf(ERROR, "error fread hdfile\n");
        return false;
    }
    return true;
}

static bool check_magic_header(FILE *file)
{
    uint8_t magic_nbr[4] = { "RDHS" };
    uint8_t magic_nbr_file[4] = { 0 };

    if (fread(&magic_nbr_file, 1, sizeof(magic_nbr_file), file) !=
        sizeof(magic_nbr_file)) {
        dprintf(FLOOD, "failed to read file\n");
        return false;
    }

    if (memcmp(magic_nbr_file, magic_nbr, sizeof(magic_nbr_file)) != 0) {
        dprintf(INFO, "memcmp magic nbr failed %s\n", magic_nbr_file);
        return false;
    }
    return true;
}

/**
 * Based on the uuid this function will try to find a TA-binary on the
 * filesystem and return it back to the caller in the parameter ta.
 *
 * @param: destination  The uuid of the TA we are searching for.
 * @param: ta           A pointer which this function will allocate and copy
 *                      the TA from the filesystem to the pointer itself. It is
 *                      the callers responsibility to free the pointer.
 * @param: ta_size      The size of the TA found on file system. It will be 0
 *                      if no TA was not found.
 *
 * @return              0 if TA was found, otherwise -1.
 */
static int TEECI_LoadSecureModule(const TEEC_UUID *destination, void **ta,
                                  size_t *ta_size)
{
    DIR *dirp;
    struct dirent *entry = NULL;
    TEEC_Result ret = TEEC_ERROR_ITEM_NOT_FOUND;
    char fname[PATH_MAX];
    size_t pos;

    pthread_mutex_lock(&mutex);
    if (!got_ta_key_hash) {
        dprintf(INFO, "ta key hash is not yet found\n");
        pthread_mutex_unlock(&mutex);
        return TA_BINARY_NOT_FOUND;
    }
    pthread_mutex_unlock(&mutex);

    if (!ta_size || !ta || !destination) {
        dprintf(INFO, "wrong inparameter to TEECI_LoadSecureModule\n");
        return TA_BINARY_NOT_FOUND;
    }

    strcpy(fname, TEEC_LOAD_PATH);
    strcat(fname, "/");
    pos = strlen(fname);

    dirp = opendir(TEEC_LOAD_PATH);

    if (!dirp) {
        dprintf(ERROR, "couldn't find folder: %s\n", TEEC_LOAD_PATH);
        return TEEC_ERROR_GENERIC;
    }

    *ta = NULL;
    *ta_size = 0;

    while ((entry = readdir(dirp))) {
        FILE *file = NULL;

        strcpy(&(fname[pos]), entry->d_name);
        file = fopen(fname, "r");

        if (file == NULL) {
            dprintf(INFO, "failed to open the ta TA-file\n");
            continue;
        }
        if (check_magic_header(file) != true) {
            dprintf(FLOOD, "magic header check failed\n");
            fclose(file);
            continue;
        }
        if (compare_ta_key_hash(file) == false) {
            dprintf(FLOOD, "ta key hash compare failed\n");
            fclose(file);
            continue;
        }

        if (check_uuid(destination, file) == false) {
            dprintf(FLOOD, "uuid check failed\n");
            fclose(file);
            continue;
        }

        if (create_and_populate_ta(file, ta, ta_size) != true) {
            dprintf(ERROR, "population of ta failed\n");
            ret = TA_BINARY_NOT_FOUND;
            fclose(file);
            break;
        }

        dprintf(FLOOD, "binary found %zu\n", *ta_size);
        ret = TA_BINARY_FOUND;
        fclose(file);
        break;
    }

    closedir(dirp);
    dprintf(FLOOD, "returning 0x%x\n", ret);
    return ret;
}

static int is_static_ta_available(const TEEC_UUID *uuid)
{
    size_t i;

    for (i = 0; i < sizeof(ta_static_uuid_list); ++i) {
        if (!memcmp(&ta_static_uuid_list[i], uuid, sizeof(TEEC_UUID))) {
            return 1;
        }
    }

    return 0;
}

static TEEC_Result teec_get_ta_key_hash(uint8_t *ta_key_hash)
{

    TEEC_Result result = TEEC_ERROR_GENERIC;
    TEEC_Operation operation;
    TEEC_Session session;
    TEEC_ErrorOrigin errorOrigin;
    TEEC_Context context;
    int ucontext;
    const TEEC_UUID ta_static_uuid = { 0xBC765EDE,
        0x6724,
        0x11DF,
        {0x8E, 0x12, 0xEC, 0xDB, 0xDF, 0xD7, 0x20, 0x85} };

    if (NULL == ta_key_hash) {
        dprintf(ERROR, "NULL == ta_key_hash buffer\n");
        goto function_exit;
    }

    ucontext = open(TEEC_DEV_PATH, O_RDWR);
    if (ucontext < 0) {
        goto function_exit;
    } else {
        context = ucontext;
    }

    memset((void *)(&operation), 0, sizeof(TEEC_Operation));
    *ta_key_hash = 0;
    operation.memRefs[0].buffer = ta_key_hash;
    operation.memRefs[0].size = SHA256_HASH_SIZE;
    operation.memRefs[0].flags = TEEC_MEM_OUTPUT;
    operation.flags = TEEC_MEMREF_0_USED;

    result = TEEC_OpenSession(&context, &session, &ta_static_uuid,
                              TEEC_LOGIN_PUBLIC, NULL, NULL, &errorOrigin);
    if (TEEC_SUCCESS != result) {
        dprintf(ERROR, "TEEC_OpenSession error\n");
        goto finalize;
    }

    result = TEEC_InvokeCommand(&session, BASS_APP_GET_TA_KEY_HASH,
                                &operation, &errorOrigin);
    if (TEEC_SUCCESS != result) {
        dprintf(ERROR, "TEEC_InvokeCommand error\n");
        goto close;
    }

close:
    result = TEEC_CloseSession(&session);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_CloseSession error\n");
    }

finalize:
    result = TEEC_FinalizeContext(&context);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_FinalizeContext error\n");
    }

function_exit:
    return result;
}

/**
 * This function initializes a new TEE Context, connecting this Client
 * application to the TEE indentified by the name name.
 *
 * name == NULL will give the default TEE. This is currently the only
 * supported!
 */
TEEC_Result TEEC_InitializeContext(const char *name,
                                   TEEC_Context *context)
{

    int *ucontext = (int *)context;
    TEEC_Result result;

    /* Specification says that when no name is provided it should fall back on
     * a predefined TEE, hence no check for NULL. */
    (void)name;

    pthread_mutex_lock(&mutex);
    if (!got_ta_key_hash) {
        result = teec_get_ta_key_hash(issw_ta_key_hash);
        if (result != TEEC_SUCCESS) {
            dprintf(ERROR, "failed to get ta_key hash %d\n", result);
            pthread_mutex_unlock(&mutex);
            return result;
        }
        got_ta_key_hash = true;
    }
    pthread_mutex_unlock(&mutex);

    *ucontext = open(TEEC_DEV_PATH, O_RDWR);
    if (*ucontext == -1) {
        return TEEC_ERROR_ITEM_NOT_FOUND;
    }

    return TEEC_SUCCESS;
}

/**
 * This function destroys an initialized TEE Context, closing the connection
 * between the Client and the TEE.
 */
TEEC_Result TEEC_FinalizeContext(TEEC_Context *context)
{
    uint32_t *ucontext = (uint32_t *)context;

    if (context) {
        if (close(*ucontext) == -1) {
            return TEEC_ERROR_GENERIC;
        }
    }

    return TEEC_SUCCESS;
}

/**
 * Allocates or registers shared memory
 */
TEEC_Result TEEC_AllocateSharedMemory(TEEC_Context *context,
                                      TEEC_SharedMemory *sharedMemory)
{
    if (context == NULL || sharedMemory == NULL) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    sharedMemory->buffer = malloc(sharedMemory->size);

    if (sharedMemory->buffer == NULL) {
        return TEEC_ERROR_OUT_OF_MEMORY;
    }

    return TEEC_SUCCESS;
}

/**
 * Releases shared memory
 */
void TEEC_ReleaseSharedMemory(TEEC_SharedMemory *sharedMemory)
{
    if (sharedMemory == NULL) {
        return;
    }

    free(sharedMemory->buffer);
}

/**
 * This function opens a new Session between the Client application and the
 * specified TEE application.
 *
 * Only connectionMethod == TEEC_LOGIN_PUBLIC is supported
 * connectionData and operation shall be set to NULL.
 */
TEEC_Result TEEC_OpenSession(TEEC_Context *context,
                             TEEC_Session *session,
                             const TEEC_UUID *destination,
                             uint32_t connectionMethod,
                             void *connectionData,
                             TEEC_Operation *operation,
                             TEEC_ErrorOrigin *errorOrigin)
{
    size_t out_size;
    size_t ta_size = 0;
    struct tee_read ret;
    struct tee_cmd tc;
    TEEC_ErrorOrigin error_origin = TEEC_ORIGIN_API;
    TEEC_Result res = TEEC_SUCCESS;
    void *ta = NULL;

    (void)connectionData;
    (void)operation;

    if (context == NULL || session == NULL) {
        error_origin = TEEC_ORIGIN_API;
        res = TEEC_ERROR_BAD_PARAMETERS;
        goto error;
    }

    if (connectionMethod != TEEC_LOGIN_PUBLIC) {
        error_origin = TEEC_ORIGIN_API;
        res = TEEC_ERROR_NOT_SUPPORTED;
        goto error;
    }

    memset(&tc, 0, sizeof(struct tee_cmd));

    /*
     * If the TA binary is found on the filesystem then use that, otherwise
     * check if the uuid is available as a static TA. If that is not the case
     * return an error.
     */
    if (TEECI_LoadSecureModule(destination, &ta, &ta_size) == TA_BINARY_FOUND) {
        tc.uuid = NULL;
        tc.data = ta;
        tc.data_size = ta_size;
    } else if (is_static_ta_available(destination)) {
        tc.uuid = (struct tee_uuid *)destination;
        tc.data = NULL;
        tc.data_size = 0;
    } else {
        error_origin = TEEC_ORIGIN_API;
        res = TEEC_ERROR_ITEM_NOT_FOUND;
        goto error;
    }

    /*
     * Save the context in the session for later use when invoke command and
     * close the session.
     */
    session->ctx = *context;

    tc.driver_cmd = TEED_OPEN_SESSION;

    /* Call kernel to open the session. */
    out_size = write(session->ctx, &tc, sizeof(struct tee_cmd));

    if (out_size != sizeof(struct tee_cmd)) {
        error_origin = tc.origin;
        dprintf(ERROR, "Data returned (write) not size of struct tee_cmd, "
                "(out_size: %d, sizeof(struct tee_cmd): %d, " "errno: %s\n",
                out_size, sizeof(struct tee_cmd), strerror(errno));
        res = TEEC_ERROR_ITEM_NOT_FOUND;
        goto error;
    }

    /* Call kernel to read the return code and error origin. */
    out_size = read(session->ctx, &ret, sizeof(struct tee_read));

    if (out_size != sizeof(struct tee_read)) {
        error_origin = TEEC_ORIGIN_API;
        dprintf(ERROR, "Data returned (read) not size of struct tee_read, "
                "(out_size: %d, sizeof(struct tee_read): %d, " "errno: %s\n",
                out_size, sizeof(struct tee_read), strerror(errno));
        res = TEEC_ERROR_COMMUNICATION;
        goto error;
    }

    if (ret.err != TEED_SUCCESS) {
        error_origin = ret.origin;
        res = ret.err;
    }

error:
    /* We do this check at the end instead of checking on every place where we
     * set the error origin */
    if (errorOrigin != NULL) {
        *errorOrigin = error_origin;
    }

    if (ta) {
        free(ta);
    }

    return res;
}

/**
 * This function closes a session which has been opened with a TEE
 * application.
 */
TEEC_Result TEEC_CloseSession(TEEC_Session *session)
{
    struct tee_cmd tc;
    size_t out_size;

    memset(&tc, 0, sizeof(struct tee_cmd));
    tc.driver_cmd = TEED_CLOSE_SESSION;
    out_size = write(session->ctx, &tc, sizeof(struct tee_cmd));

    if (out_size != sizeof(struct tee_cmd)) {
        dprintf(ERROR, "Data returned (write) not size of struct tee_cmd, "
                "(out_size: %d, sizeof(struct tee_cmd): %d, errno: %s\n",
                out_size, sizeof(struct tee_cmd), strerror(errno));
        return TEEC_ERROR_COMMUNICATION;
    }

    return TEEC_SUCCESS;
}

/**
 * Invokes a TEE command (secure service, sub-PA or whatever)
 */
TEEC_Result TEEC_InvokeCommand(TEEC_Session *session,
                               uint32_t commandID,
                               TEEC_Operation *operation,
                               TEEC_ErrorOrigin *errorOrigin)
{
    struct tee_cmd tc;
    size_t out_size;
    struct tee_read ret;

    if (session == NULL || operation == NULL) {
        if (errorOrigin != NULL) {
            *errorOrigin = TEEC_ORIGIN_API;
        }

        return TEEC_ERROR_BAD_PARAMETERS;
    }

    memset(&tc, 0, sizeof(struct tee_cmd));
    memset(&ret, 0, sizeof(struct tee_read));

    tc.driver_cmd = TEED_INVOKE;
    tc.cmd = commandID;
    tc.op = malloc(sizeof(struct tee_operation));
    if (!tc.op) {
        dprintf(ERROR, "Couldn't allocate memory for tc.op\n");
        return TEEC_ERROR_OUT_OF_MEMORY;
    }
    copy_tee_op_from_legacy(tc.op, operation);
    out_size = write(session->ctx, &tc, sizeof(struct tee_cmd));
    copy_tee_op_to_legacy(operation, tc.op);
    free(tc.op);

    if (out_size != sizeof(struct tee_cmd)) {
        if (errorOrigin != NULL) {
            *errorOrigin = TEEC_ORIGIN_API;
        }

        dprintf(ERROR, "Data returned (write) not size of struct tee_cmd, "
                "(out_size: %d, sizeof(struct tee_cmd): %d, errno: %s\n",
                out_size, sizeof(struct tee_cmd), strerror(errno));
        return TEEC_ERROR_COMMUNICATION;
    }

    out_size = read(session->ctx, &ret, sizeof(struct tee_read));

    if (out_size != sizeof(struct tee_read)) {
        if (errorOrigin != NULL) {
            *errorOrigin = TEEC_ORIGIN_API;
        }

        dprintf(ERROR, "Data returned (read) not size of struct tee_read, "
                "(out_size: %d, sizeof(struct tee_read): %d, errno: %s\n",
                out_size, sizeof(struct tee_read), strerror(errno));
        return TEEC_ERROR_COMMUNICATION;
    }

    if (ret.err != TEED_SUCCESS) {
        if (errorOrigin != NULL) {
            *errorOrigin = ret.origin;
        }

        return ret.err;
    }

    return TEEC_SUCCESS;
}
