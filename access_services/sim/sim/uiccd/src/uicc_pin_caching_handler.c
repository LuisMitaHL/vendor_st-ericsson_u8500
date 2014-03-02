/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "catd_modem_types.h"
#include "sim_internal.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "catd_modem.h"
#include "uicc_state_machine.h"
#include "uicc_interface_handlers.h"
#include "tee_client_api.h" // Don't forget libtee as LOCAL_SHARED_LIBRARY in Android.mk

#define MODEM_SILENT_RESET_FILE_CACHE_PIN    "/data/misc/modemsilentreset_pin"

static char    validated_pin_1[PIN_MAX_LEN + 1];    //pin has been validated ok
static TEEC_Context msr_encryption_ctx = 0;
static uint8_t msr_pin_verifying_state;

/* This is the UUID for the trusted application (TA) called SMCL_TA. */
#define SMCL_TA_UUID 0xb6186b40, \
                  0xcf93, \
                  0x11df, \
                  {0x8f, 0x39, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b}

/*
 * This is the so called "taf-id" (trusted application function id) which
 * identifies the function to call within the trusted application in the
 * secure world.
 */
#define SMCL_ENCRYPT_PIN 0x00000004

/*
 * Defines that tell whether it is encryption or decryption. Values should stay
 * in sync with the values defined in secure world.
 */
#define SMCL_DIR_ENCRYPT (0x8)
#define SMCL_DIR_DECRYPT (0x0)


/* During bootup, when you are root */
int uiccd_msr_init_encryption_context(void)
{
    TEEC_Result result = TEEC_ERROR_GENERIC;

    result = TEEC_InitializeContext(NULL, &msr_encryption_ctx);

    if (result != TEEC_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "uicc : init encryption context failed. result = %d", result);
        return -1;
    }

    return 0;
}

/* Platform shutdown */
void uiccd_msr_finalize_encryption_context(void)
{
    TEEC_Result result = TEEC_ERROR_GENERIC;

    result = TEEC_FinalizeContext(&msr_encryption_ctx);
    if (result != TEEC_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "uicc : finalize encryption context failed. result = %d", result);
    }
}

/* When you actually want to do the encryption/decryption. */
uint32_t uiccd_msr_encrypt_pin(uint8_t *pin,
                               uint32_t pin_length,
                               uint32_t encrypt_dir)
{
    TEEC_Operation op;
    TEEC_ErrorOrigin org = TEEC_ORIGIN_API;
    TEEC_Result result = TEEC_ERROR_GENERIC;
    TEEC_Session session;
    TEEC_UUID uuid = { SMCL_TA_UUID };
    uint32_t ret = 1;

    if (msr_encryption_ctx == 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : encryption context is 0.");
        return 1;
    }
    memset(&session, 0, sizeof(TEEC_Session));
    memset(&op, 0, sizeof(TEEC_Operation));

    /*
    * The trusted application (TA) expect the data that will be
    * encrypted/decrypted to be put on memRefs[0] with the size of 16
    * (since this is the smallest blocksize for AES encryption). It also
    * expects that the variable telling whether it is encryption or decryption
    * should be located on memRefs[1] with the the of an uint32. The TA
    * will put the return value from the trusted application in memRefs[3].
    */
    op.memRefs[0].buffer = (void *)pin;
    op.memRefs[0].size = pin_length;
    op.memRefs[0].flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;

    op.memRefs[1].buffer = (void *)(&encrypt_dir);
    op.memRefs[1].size = sizeof(uint32_t);
    op.memRefs[1].flags = TEEC_MEM_INPUT;

    op.flags = TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED;
    result = TEEC_OpenSession(&msr_encryption_ctx,
                              &session,
                              &uuid,
                              TEEC_LOGIN_PUBLIC,
                              NULL,
                              NULL,
                              &org);
    if (result != TEEC_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "uicc : open encryption session failed. result = %d", result);
        return 1;
    }

    op.memRefs[3].buffer = (void *)(&ret);
    op.memRefs[3].size = sizeof(uint32_t);
    op.memRefs[3].flags = TEEC_MEM_OUTPUT;
    op.flags |= TEEC_MEMREF_3_USED;

    result = TEEC_InvokeCommand(&session,
                                SMCL_ENCRYPT_PIN,
                                &op,
                                &org);
    if (result != TEEC_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "uicc : encryption failed. result = %d", result);
        result = TEEC_CloseSession(&session);
        if (result != TEEC_SUCCESS) {
            catd_log_f(SIM_LOGGING_E, "uicc : close encryption session failed. result = %d", result);
        }
        return 1;
    }

    result = TEEC_CloseSession(&session);
    if (result != TEEC_SUCCESS) {
        catd_log_f(SIM_LOGGING_E, "uicc : close encryption session failed. result = %d", result);
        return 1;
    }

    return 0;
}


static int uiccd_msr_file_exist()
{
    struct stat st;
    int rv;

    rv = stat(MODEM_SILENT_RESET_FILE_CACHE_PIN, &st);

    if(rv == 0) {
        catd_log_f(SIM_LOGGING_I, "uicc : modem silent reset file for pin cache exists.");
        return 1;
    } else if (rv == -1 && errno == ENOENT) {
        catd_log_f(SIM_LOGGING_I, "uicc : modem silent reset file for pin cache does not exist.");
        return 0;
    } else {
        return 0;
    }
}

static int uiccd_msr_file_read(char *pin_p, uint8_t pin_len)
{
    int fp;
    ssize_t len = 0;


    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_msr_file_read.");
    if (pin_p == NULL || pin_len == 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_msr_file_read, buffer for pin is null.");
        return -1;
    }
    fp = open(MODEM_SILENT_RESET_FILE_CACHE_PIN, O_RDONLY);
    if (fp == -1) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_msr_file_read, open failed.");
        return -1;
    }

    len = read(fp, (void *)pin_p, (size_t)pin_len);

    close(fp);
    if (len < 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_msr_file_read, read returned error.");
        return 0;
    }

    return (int)len;
}

static int uiccd_msr_file_write(char *pin_p, uint8_t pin_len)
{
    int fp;
    ssize_t rv = 0;

    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_msr_file_write.");
    if (pin_p == NULL || pin_len == 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_msr_file_write, buffer for pin is null.");
        return -1;
    }
    fp = open(MODEM_SILENT_RESET_FILE_CACHE_PIN, O_WRONLY);
    if (fp == -1) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_msr_file_write, open failed.");
        return -1;
    }

    rv = write(fp, (void *)pin_p, (size_t)pin_len);
    close(fp);
    if (rv < 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_msr_file_write, write returned error.");
        return -1;
    }

    return rv;
}

static int uiccd_msr_file_create()
{
    int fd_msr_pin;

    fd_msr_pin = open(MODEM_SILENT_RESET_FILE_CACHE_PIN, O_RDWR | O_CREAT | O_EXCL,
                      S_IRUSR | S_IWUSR | S_IRGRP| S_IWGRP | S_IROTH | S_IWOTH | S_IXOTH | S_IXUSR | S_IXGRP);

    if (fd_msr_pin < 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_msr_file_create, open failed. returned fd = %d, errno = %d", fd_msr_pin, errno);
        return -1;
    }
    close(fd_msr_pin);
    chmod(MODEM_SILENT_RESET_FILE_CACHE_PIN, 0660);

    catd_log_f(SIM_LOGGING_I, "uicc : modem silent reset file for pin cache created");
    return 0;
}

static void uiccd_msr_file_remove()
{
    int rv = 0;
    FILE *fp;

    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_msr_file_remove.");
    rv = remove(MODEM_SILENT_RESET_FILE_CACHE_PIN);
    catd_log_f(SIM_LOGGING_I, "uicc : modem silent reset file for pin cache removed, rv = %d, errno = %d", rv, errno);
    //if we do not have the privilege to remove the file, then just clean it
    if (rv != 0) {
        fp = fopen(MODEM_SILENT_RESET_FILE_CACHE_PIN, "w");
        fclose(fp);
    }
}

void uiccd_msr_cache_pin()
{
    //create pin file to be used when restart
    //check if PIN is set in the cached value
    catd_log_f(SIM_LOGGING_D, "uicc : strlen(validated_pin_1) = %d", strlen(validated_pin_1));
    if (strlen(validated_pin_1) > 0) {
        uint8_t pin[PIN_MAX_LEN] = {0};

        strncpy((char*)pin, validated_pin_1, PIN_MAX_LEN);
        //encrypt the pin
        if (uiccd_msr_encrypt_pin(pin, sizeof(pin), SMCL_DIR_ENCRYPT) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_msr_encrypt_pin encrypting pin failed!");
            return;
        }

        if (0 == uiccd_msr_file_exist()) {
            if (uiccd_msr_file_create() < 0) {
                catd_log_f(SIM_LOGGING_E, "uicc : file open for MSR PIN failed.");
            } else {
                //write the encrypted pin to the file
                if (uiccd_msr_file_write((char*)pin, sizeof(pin)) < 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : file write for MSR PIN failed.");
                }
            }
        } else {
            //write the encrypted pin to the file
            if (uiccd_msr_file_write((char*)pin, sizeof(pin)) < 0) {
                catd_log_f(SIM_LOGGING_E, "uicc : file write for MSR PIN failed.");
            }
            catd_log_f(SIM_LOGGING_D, "uicc : cached pin to file for MSR.");
        }
    }
}

//set the validated_pin_1 to the input value
void uiccd_msr_set_cached_pin(char * pin_p, uint8_t pin_len)
{
    uint8_t actual_len = 0;

    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_msr_set_cached_pin. pin_len = %d", pin_len);
    if (!pin_p){
        catd_log_f(SIM_LOGGING_E, "uicc : input for uiccd_msr_set_cached_pin is null.");
        return;
    }

    memset(validated_pin_1, 0, PIN_MAX_LEN + 1);
    actual_len = (pin_len < PIN_MAX_LEN ? pin_len : PIN_MAX_LEN);
    strncpy(validated_pin_1, pin_p, actual_len);
}

int uiccd_msr_get_cached_pin(char * pin_p, uint8_t pin_len)
{
    uint8_t  return_len = 0;
    char     cached_pin[PIN_MAX_LEN];
    uint8_t  cached_pin_len = 0;

    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_msr_get_cached_pin.");
    if (!pin_p || pin_len == 0){
        catd_log_f(SIM_LOGGING_E, "uicc : input for uiccd_msr_get_cached_pin is null.");
        return -1;
    }
    memset(pin_p, 0, pin_len);

    if (0 == uiccd_msr_file_exist()) {
        //MSR did not happen
        catd_log_f(SIM_LOGGING_I, "uicc : no cached pin for MSR.");
        return 0;
    } else {
        //if yes, try to verify with the cached pin
        memset(cached_pin, 0, PIN_MAX_LEN);
        cached_pin_len = uiccd_msr_file_read(cached_pin, sizeof(cached_pin));
        if (cached_pin_len <= 0) {
            catd_log_f(SIM_LOGGING_D, "uicc : read from cached pin file return 0.");
            return 0;
        }
        if (uiccd_msr_encrypt_pin((uint8_t*)cached_pin, sizeof(cached_pin), SMCL_DIR_DECRYPT) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_msr_encrypt_pin decrypting pin failed!");
            return 0;
        }
        cached_pin_len = strlen(cached_pin);
        //remove the PIN file for MSR
        uiccd_msr_file_remove();
        catd_log_f(SIM_LOGGING_I, "uicc : cached pin length = %d", cached_pin_len);
    }

    return_len = (pin_len < cached_pin_len ? pin_len : cached_pin_len);
    strncpy(pin_p, cached_pin, return_len);
    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_msr_get_cached_pin. return_len = %d", return_len);

    return return_len;
}

void uiccd_msr_reset_cached_pin()
{
    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_msr_reset_cached_pin.");
    memset(validated_pin_1, 0, PIN_MAX_LEN + 1);
}

static int uiccd_msr_pin_verify_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t    *ctrl_p = (ste_sim_ctrl_block_t*) (ste_msg->client_tag);
    int                      rv;
    uiccd_msg_pin_verify_response_t * rsp_p;

    msr_pin_verifying_state = 0;

    if (ste_msg->type != UICCD_MSG_PIN_VERIFY_RSP) {
        catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for uiccd_msr_pin_verify_transaction_handler: 0x%x.", ste_msg->type);
        return 1;
    }

    if (ctrl_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "uicc : ctrl block is null for uiccd_msr_pin_verify_transaction_handler.");
        return 1;
    }

    rsp_p = (uiccd_msg_pin_verify_response_t*) ste_msg;

    //ctrl_p and the handler data are not used after
    if (ctrl_p->transaction_data_p) {
        ste_uicc_pin_info_t    *pin_info_p = (ste_uicc_pin_info_t*)(ctrl_p->transaction_data_p);

        //check the status code
        if (pin_info_p->pin_id != SIM_UICC_PIN_ID_PIN2 &&
            SIM_UICC_STATUS_CODE_OK == rsp_p->uicc_status_code) {
            uiccd_msr_set_cached_pin(pin_info_p->pin, pin_info_p->pin_len);
        } else {
            catd_log_f(SIM_LOGGING_E, "uicc : pin verify for MSR failed: rsp_p->uicc_status_code = %d.", rsp_p->uicc_status_code);
            free(ctrl_p->transaction_data_p);
            free(ctrl_p);
            return 1;
        }
        free(ctrl_p->transaction_data_p);
    }
    free(ctrl_p);

    return 0;
}

int uiccd_msr_pin_verify(char * pin_p, uint8_t pin_len)
{
    ste_modem_t            *m;
    ste_sim_ctrl_block_t   * ctrl_p;

    catd_log_f(SIM_LOGGING_I, "uicc : PIN verify with cached pin for MSR.");

    if (!pin_p || pin_len == 0){
        catd_log_f(SIM_LOGGING_E, "uicc : input for uiccd_msr_pin_verify is invalid.");
        return -1;
    }

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        return -1;
    } else {
        int i;
        ste_uicc_pin_info_t    *pin_info_p = NULL;

        pin_info_p = (ste_uicc_pin_info_t*)calloc(1, sizeof(*pin_info_p));
        if (!pin_info_p) {
            catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed.");
            return -1;
        }

        ctrl_p = ste_sim_create_ctrl_block(UICCD_CLIENT_TAG, UICCD_FD, UICCD_MSG_PIN_VERIFY_MSR, uiccd_msr_pin_verify_transaction_handler, pin_info_p);

        if (!ctrl_p) {
            catd_log_f(SIM_LOGGING_E, "uicc : failed to create ctrl block for pin verify.");
            free(pin_info_p);
            return -1;
        }

        pin_info_p->pin_id = SIM_UICC_PIN_ID_PIN1;
        pin_info_p->pin_len = pin_len;
        strncpy(pin_info_p->pin, pin_p, PIN_MAX_LEN);

        i = ste_modem_pin_verify(m,
                                 (uintptr_t)ctrl_p,
                                 uicc_get_app_id(),
                                 SIM_UICC_PIN_ID_PIN1,
                                 pin_p,
                                 pin_len);

        if (i != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : ste_modem_pin_verify failed");
            free(pin_info_p);
            free(ctrl_p);
            return -1;
        }
        msr_pin_verifying_state = 1;
    }

    return 0;
}

void uiccd_msr_init_pin_caching()
{
    int rv = -1;

    catd_log_f(SIM_LOGGING_I, "uicc : pin caching init.");
    //try to create the file here due to the privilege downgrade.
    uiccd_msr_reset_cached_pin();
    if (0 == uiccd_msr_file_exist()) {
        (void)uiccd_msr_file_create();
        catd_log_f(SIM_LOGGING_I, "uicc : attempting to create file for pin caching.");
    }
    if (0 == msr_encryption_ctx) {
        rv = uiccd_msr_init_encryption_context();
        if (rv != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : failed to init context for encryption.");
        }
    }
}


int uiccd_msr_get_pin_verifying_state()
{
    catd_log_f(SIM_LOGGING_D, "uicc : uiccd_msr_get_pin_verifying_state = %d",msr_pin_verifying_state);
    return msr_pin_verifying_state;
}
void uiccd_msr_reset_pin_caching()
{
    uiccd_msr_finalize_encryption_context();
}

#endif

