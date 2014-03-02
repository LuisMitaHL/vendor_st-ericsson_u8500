/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#include <sys/param.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cops.h>
#include <cops_test.h>

#ifdef COPS_OSE_ENVIRONMENT
#include "cops_cmd_swbp.h"
#endif

#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#define COPS_CHALLENGE_SIZE (4 + (96/8) + 32)

#define COPS_SIMLOCK_CNL_ENTRY_SIZE 6
/*
 * The following macro is used to remove const cast-away warnings
 * from gcc -Wcast-qual; it should be used with caution because it
 * can hide valid errors; in particular most valid uses are in
 * situations where the API requires it, not to cast away string
 * constants.
 */
#define COPS_UNCONST(a) ((void *)(intptr_t)(const void *)(a))

static int cmd_main(int argc, char *argv[]);

static char *next_param(int *argc, char **argv[]);
static char *restore_param(int *argc, char **argv[]);

static cops_return_code_t cmd_read_imei(cops_context_id_t *ctx,
                                        int *argc, char **argv[]);

static cops_return_code_t cmd_bind_properties(cops_context_id_t *ctx,
                                              int *argc, char **argv[]);

static cops_return_code_t cmd_read_data(cops_context_id_t *ctx,
                                        int *argc, char **argv[]);

static cops_return_code_t cmd_get_nbr_of_otp_rows(cops_context_id_t *ctx,
                                        int *argc, char **argv[]);

static cops_return_code_t cmd_read_otp(cops_context_id_t *ctx,
                                        int *argc, char **argv[]);

static cops_return_code_t cmd_write_otp(cops_context_id_t *ctx,
                                        int *argc, char **argv[]);

static cops_return_code_t cmd_authenticate(cops_context_id_t *ctx,
                                           int *argc, char **argv[]);

static cops_return_code_t cmd_verify_simlock_control_keys(
    cops_context_id_t *ctx,
    int *argc,
    char **argv[]);

static cops_return_code_t cmd_deauthenticate(cops_context_id_t *ctx,
                                             int *argc, char **argv[]);

static cops_return_code_t cmd_get_challenge(cops_context_id_t *ctx,
                                            int *argc, char **argv[]);

static cops_return_code_t cmd_modem_sipc_mx(cops_context_id_t *ctx,
                                            int *argc, char **argv[]);

static cops_return_code_t cmd_simlock_unlock(cops_context_id_t *ctx,
                                             int *argc, char **argv[]);

static cops_return_code_t cmd_ota_simlock_unlock(cops_context_id_t *ctx,
                                                 int *argc, char **argv[]);

static cops_return_code_t cmd_simlock_get_status(cops_context_id_t *ctx,
                                                 int *argc, char **argv[]);

static cops_return_code_t cmd_simlock_lock(cops_context_id_t *ctx,
        int *argc, char **argv[]);

static bool cmd_get_key(cops_simlock_control_key_t *key,
                        const char *key_name,
                        int *argc, char **argv[]);

static bool cmd_get_lock(char *str, cops_simlock_lock_type_t *lock_type);

static bool check_return_value(const char *func_str,
                               cops_return_code_t ret_code, const char *str);

static cops_return_code_t cmd_get_device_state(cops_context_id_t *ctx,
                                               int *argc, char **argv[]);

static cops_return_code_t cmd_bind_data(cops_context_id_t *ctx,
                                        int *argc, char **argv[]);

static cops_return_code_t cmd_verify_data_binding(cops_context_id_t *ctx,
                                                  int *argc, char **argv[]);

static cops_return_code_t cmd_verify_signed_header(cops_context_id_t *ctx,
                                                   int *argc, char **argv[]);

static cops_return_code_t cmd_calcdigest(cops_context_id_t *ctx,
                                         int *argc, char **argv[]);

static cops_return_code_t cmd_verify_imsi(cops_context_id_t *ctx,
                                          int *argc, char **argv[]);

static cops_return_code_t cmd_lock_bootpartition(cops_context_id_t *ctx,
        int *argc, char **argv[]);

static cops_return_code_t
cmd_init_arb_table(cops_context_id_t *ctx, int *argc, char **argv[]);

static cops_return_code_t
cmd_write_secprofile(cops_context_id_t *ctx, int *argc, char **argv[]);

static cops_return_code_t cmd_change_simkey(cops_context_id_t *ctx,
                                            int *argc, char **argv[]);

static cops_return_code_t cmd_write_rpmb_key(cops_context_id_t *ctx,
                                            int *argc, char **argv[]);

static cops_return_code_t cmd_get_product_debug_settings(cops_context_id_t *ctx,
                                                      int *argc, char **argv[]);

static const struct {
    const char *str;
    cops_return_code_t (*func)(cops_context_id_t *ctx,
                               int *argc, char **argv[]);
} api_funcs[] = {
    {"read_imei", cmd_read_imei},
    {"bind_properties", cmd_bind_properties},
    {"read_data", cmd_read_data},
    {"get_nbr_of_otp_rows", cmd_get_nbr_of_otp_rows},
    {"read_otp", cmd_read_otp},
    {"write_otp", cmd_write_otp},
    {"authenticate", cmd_authenticate},
    {"deauthenticate", cmd_deauthenticate},
    {"get_challenge", cmd_get_challenge},
    {"modem_sipc_mx", cmd_modem_sipc_mx},
    {"unlock", cmd_simlock_unlock},
    {"lock", cmd_simlock_lock},
    {"ota_ul", cmd_ota_simlock_unlock},
    {"get_status", cmd_simlock_get_status},
    {"key_ver", cmd_verify_simlock_control_keys},
    {"get_device_state", cmd_get_device_state},
    {"verify_imsi", cmd_verify_imsi},
    {"bind_data", cmd_bind_data},
    {"verify_data_binding", cmd_verify_data_binding},
    {"verify_signed_header", cmd_verify_signed_header},
    {"calcdigest", cmd_calcdigest},
    {"lock_bootpartition", cmd_lock_bootpartition},
    {"init_arb_table", cmd_init_arb_table},
    {"write_secprofile", cmd_write_secprofile},
    {"change_simkey", cmd_change_simkey},
    {"write_rpmb_key", cmd_write_rpmb_key},
    {"get_product_debug_settings", cmd_get_product_debug_settings}

};

static const size_t num_api_funcs = sizeof(api_funcs) / sizeof(api_funcs[0]);

#ifdef COPS_OSE_ENVIRONMENT

#include <r_os.h>
#include <r_idbg.h>
#include <strings.h>

/*lint -e683 -e652 */
/*
 * 652: Redefinition of fprintf
 * 683: function 'fprintf' #define'd, semantics may be lost
*/
#define STDOUT_FILENO 1
#define fprintf(x, ...) printf(__VA_ARGS__)
/*lint +e683 +e652 */
static IDbg_Context_t idbg_context;

static void cops_cmd_idbg(char *cmd_buf, int *arg_index, int args_found)
{
    int counter = 0;
    int argc = args_found + 1;
    char *argv[argc];

    /* Construct argv from cmd_buf */
    argv[counter] = strdup(cmd_buf);
    counter++;

    while (counter < argc) {
        argv[counter] = strdup(cmd_buf + arg_index[counter - 1]);
        counter++;
    }

    (void)cmd_main(argc, argv);

    counter = 0;

    while (counter < argc) {
        free(argv[counter]);
        counter++;
    }
}

IDBG_TBL_START(commands)
    IDBG_TBL_CMD(cops_cmd_idbg, "cops_cmd")
    IDBG_TBL_SUB_DIR(cops_swbp_commands, "swbp_commands")
IDBG_TBL_END

IDBG_TBL_START(command_dir)
  IDBG_TBL_SUB_DIR(commands, "COPS_Tools")
IDBG_TBL_END

OS_PROCESS(COPS_CMD_Process)
{
    union SIGNAL *signal = NIL;
    static const SIGSELECT primitives[] = {0};

    (void)Request_IDbg_Register(WAIT_RESPONSE);

    while (1) {
        signal = RECEIVE(primitives);

        (void)Do_IDbg_HandleSignal(&signal, &idbg_context, command_dir);

        if (signal != NIL) {
            SIGNAL_FREE(&signal);
        }
    }
}
#else
int main(int argc, char *argv[])
{
    return cmd_main(argc, argv);
}
#endif

int cmd_main(int argc, char *argv[])
{
    cops_return_code_t ret_code;
    cops_context_id_t *ctx = NULL;
    size_t    n;
    char     *str;
    int       exit_status = EXIT_SUCCESS;

    (void)next_param(&argc, &argv);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    if (strcmp(str, "-s") == 0) {
        str = next_param(&argc, &argv);

        if (str == NULL) {
            goto usage;
        }

        cops_api_socket_path = strdup(str);

        if (cops_api_socket_path == NULL) {
            fprintf(stderr, "strdup\n%s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        str = next_param(&argc, &argv);
    }

    if (str == NULL || strcmp(str, "-h") == 0) {
        goto usage;
    }

    fprintf(stderr, "Connecting COPS\n");

    ret_code = cops_context_create(&ctx, NULL, NULL);

    if (ret_code != COPS_RC_OK) {
        fprintf(stderr, "cops_context_create: ret_code %d\n", ret_code);
        return EXIT_FAILURE;
    }

    do {
        for (n = 0; n < num_api_funcs; n++) {
            if (strcmp(str, api_funcs[n].str) == 0) {
                fprintf(stderr, "Calling %s\n", api_funcs[n].str);
                ret_code = api_funcs[n].func(ctx, &argc, &argv);
                str = next_param(&argc, &argv);

                if (str != NULL) {
                    if (!check_return_value(api_funcs[n].str, ret_code, str)) {
                        exit_status = EXIT_FAILURE;
                        goto cleanup;
                    }
                } else {
                    if (!check_return_value(api_funcs[n].str, ret_code, "@0")) {
                        exit_status = EXIT_FAILURE;
                        goto cleanup;
                    }
                }

                break;
            }
        }

        if (n == num_api_funcs) {
            fprintf(stderr, "Unknown function \"%s\"\n", str);
            goto usage;
        }

        str = next_param(&argc, &argv);

    } while (str != NULL);

cleanup:

    fprintf(stderr, "Disconnecting COPS\n");
    cops_context_destroy(&ctx);
    return exit_status;

usage:
    fprintf(stderr, "Usage: cops_cmd [-s <socket>] [-h]\n");

    for (n = 0; n < num_api_funcs; n++) {
        fprintf(stderr, "Usage: cops_cmd %s <args...>\n", api_funcs[n].str);
    }

    cops_context_destroy(&ctx);
    return EXIT_FAILURE;
}

static bool cmd_get_lock(char *str, cops_simlock_lock_type_t *lock_type)
{
    bool success = true;

    if (str == NULL) {
        fprintf(stderr, "Lock not found.\n");
        success = false;
    } else if (strcmp(str, "nl") == 0) {
        *lock_type = COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK;
    } else if (strcmp(str, "nsl") == 0) {
        *lock_type = COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK;
    } else if (strcmp(str, "spl") == 0) {
        *lock_type = COPS_SIMLOCK_LOCK_TYPE_SERVICE_PROVIDER_LOCK;
    } else if (strcmp(str, "cl") == 0) {
        *lock_type = COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK;
    } else if (strcmp(str, "siml") == 0) {
        *lock_type = COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK;
    } else if (strcmp(str, "esll") == 0) {
        *lock_type = COPS_SIMLOCK_LOCK_TYPE_FLEXIBLE_ESL_LOCK;
    } else if (strcmp(str, "") == 0) {
        fprintf(stderr, "Empty lock not found.\n");
        success = false;
    } else {
        fprintf(stderr, "Unknown lock (%s)!\n", str);
        success = false;
    }

    if (!success) {
        fprintf(stderr, "<locks>: nl   - network lock\n");
        fprintf(stderr, "         nsl  - network subset lock\n");
        fprintf(stderr, "         spl  - service provide lock\n");
        fprintf(stderr, "         cl   - corporate lock\n");
        fprintf(stderr, "         siml - (U)SIM lock\n");
        fprintf(stderr, "         esll - flexible esl lock\n");
    }

    return success;
}

static cops_return_code_t cmd_simlock_unlock(cops_context_id_t *ctx,
        int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    cops_simlock_lock_type_t lock_type;
    cops_simlock_control_key_t control_key;
    char *str;

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    if (!cmd_get_lock(str, &lock_type)) {
        goto usage;
    }

    if (!cmd_get_key(&control_key, "Sim", argc, argv)) {
        goto usage;
    }

    ret_code = cops_simlock_unlock(ctx, lock_type, &control_key);

    return ret_code;

usage:
    fprintf(stderr, "Usage: unlock <lock> <key>\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t cmd_ota_simlock_unlock(cops_context_id_t *ctx,
        int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    cops_simlock_ota_reply_message_t ota_reply_msg;
    size_t ota_msg_len;
    uint8_t *ota_msg = NULL;
    char     *str;
    size_t n;
    FILE                        *fp;
    uint8_t                     *filedata = NULL;
    size_t                      size;
    size_t                      fsize;
    long                        tmp_size;

    str = next_param(argc, argv);

    if (str == NULL || *str == '@') {
        return COPS_RC_ARGUMENT_ERROR;
    }

    fp = fopen(str, "r");

    if (fp == NULL) {
        fprintf(stderr, "failed to open file %s\n%s\n",
                str, strerror(errno));
        return COPS_RC_ARGUMENT_ERROR;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fprintf(stderr, "failed to seek end of file %s\n%s\n",
                str, strerror(errno));
        ret_code = COPS_RC_ARGUMENT_ERROR;
        goto cleanup;
    }

    tmp_size = ftell(fp);

    if (tmp_size >= 0) {
        size = tmp_size;
    } else {
        fprintf(stderr, "ftell returned negative offset\n");
        ret_code = COPS_RC_STORAGE_ERROR;
        goto cleanup;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Failed to rewind file %s\n", str);
        ret_code = COPS_RC_STORAGE_ERROR;
        goto cleanup;
    }

    filedata = malloc(size);

    if (filedata == NULL) {
        /*lint -e557 Suppress unrecognized format %zu used in Linux */
        fprintf(stderr, "failed to allocate %zu bytes of memory\n%s\n",
                size, strerror(errno));
        /*lint +e557 */
        ret_code = COPS_RC_MEMORY_ALLOCATION_ERROR;
        goto cleanup;
    }

    fsize = fread(filedata, size, sizeof(uint8_t), fp);

    if ((fsize * size) != size) {
        /*lint -e557 Suppress unrecognized format %zu used in Linux */
        fprintf(stderr, "read too few bytes from file. fsize = %zu,"
                "size = %zu\n%s\n", fsize, size, strerror(errno));
        /*lint +e557 */
        ret_code = COPS_RC_STORAGE_ERROR;
        goto cleanup;
    }

    ota_msg_len = size;
    ota_msg = filedata;

    /*lint -e557 Suppress unrecognized format %zu used in Linux */
    fprintf(stderr, "will call ota unlock, len is %zu and data is\n",
            ota_msg_len);
    /*lint +e557 */

    for (n = 0; n < ota_msg_len; n++) {
        fprintf(stderr, "%x ", ota_msg[n]);
    }

    fprintf(stderr, "\n");
    ret_code = cops_simlock_ota_unlock(ctx, ota_msg, ota_msg_len,
                                       &ota_reply_msg);

    if (ret_code == COPS_RC_OK) {
        fprintf(stderr, "OTA_msg_result\n");

        for (n = 0; n < COPS_SIMLOCK_OTA_REPLY_MESSAGE_SIZE; n++) {
            printf("%x ", ota_reply_msg.data[n]);
        }

        printf("\n");
    }

cleanup:

    fclose(fp);

    if (filedata != NULL) {
        free(filedata);
    }

    return ret_code;
}

static cops_return_code_t cmd_simlock_get_status(cops_context_id_t *ctx,
        int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    cops_simlock_status_t simlock_status;
    size_t i;
    cops_simlock_lock_status_t lock_status;

    (void) argc;
    (void) argv;

    ret_code = cops_simlock_get_status(ctx, &simlock_status);

    if (ret_code == COPS_RC_OK) {
        printf("simcardstatus %d\n", simlock_status.sim_card_status);

        for (i = 0; i < 6; i++) {
            switch (i) {
            case 0:
                lock_status = simlock_status.nl_status;
                break;
            case 1:
                lock_status = simlock_status.nsl_status;
                break;
            case 2:
                lock_status = simlock_status.spl_status;
                break;
            case 3:
                lock_status = simlock_status.cl_status;
                break;
            case 4:
                lock_status = simlock_status.siml_status;
                break;
            case 5:
                lock_status = simlock_status.esll_status;
                break;
            default:
                fprintf(stderr, "internal error\n");
                goto function_exit;
            }

            printf("** locktype %d **\n", lock_status.lock_type);
            printf("lockdef %d\n", lock_status.lock_definition);
            printf("locksetting %d\n", lock_status.lock_setting);

            /*lint -e557 Suppress unrecognized format %zu used in Linux */
            printf("attemptsleft %zu\n", lock_status.attempts_left);
            printf("timerrunning %d\n", lock_status.timer_is_running);
            printf("timeleft %d\n", lock_status.time_left_on_running_timer);
            printf("timattemptsleft %zu\n", lock_status.timer_attempts_left);
            /*lint +e557 */
        }
    } else {
        fprintf(stderr, "function call returned with error\n");
    }

function_exit:
    return ret_code;
}

static cops_return_code_t
cmd_read_imei(cops_context_id_t *ctx, int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    cops_imei_t imei;

    (void) argc;
    (void) argv;

    ret_code = cops_read_imei(ctx, &imei);

    if (ret_code == COPS_RC_OK) {
        size_t    n;

        fprintf(stderr, "IMEI ");

        for (n = 0; n < COPS_UNPACKED_IMEI_LENGTH; n++) {
            printf("%d", imei.digits[n]);
        }

        printf("\n");
    }

    return ret_code;
}

static bool cmd_get_key(cops_simlock_control_key_t *key,
                        const char *key_name,
                        int *argc, char **argv[])
{
    char     *str;

    str = next_param(argc, argv);

    if (str == NULL || *str == '@') {
        fprintf(stderr, "%s lock control key is missing\n", key_name);
        return false;
    }

    if (strlen(str) > sizeof(key->value)) {
        fprintf(stderr, "%s lock control key is too long\n", key_name);
        return false;
    }

    strcpy(key->value, str);    /* checked length above */
    return true;
}

static cops_return_code_t cmd_bind_properties(cops_context_id_t *ctx,
        int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    cops_imei_t imei;
    cops_bind_properties_arg_t arg;
    size_t    n;
    char     *str;
    cops_simlock_control_keys_t keys;
    cops_auth_data_t auth_data;
    cops_simlock_control_key_t mck;

    memset(&arg, 0, sizeof(arg));

    while ((str = next_param(argc, argv)) != NULL) {
        if (*str == '@') {
            (void)restore_param(argc, argv);
            break;
        }

        if (strcmp(str, "imei") == 0) {

            if (arg.imei != NULL) {
                fprintf(stderr, "bind_properties: "
                        "imei may only be specified once\n");
                goto usage;
            }

            str = next_param(argc, argv);

            if (str == NULL ||
                    strlen(str) != COPS_UNPACKED_IMEI_LENGTH) {
                fprintf(stderr,
                        "bind_properties: imei must contain %d digits\n",
                        COPS_UNPACKED_IMEI_LENGTH);
                goto usage;
            }

            for (n = 0; n < COPS_UNPACKED_IMEI_LENGTH; n++) {
                if (!isdigit(str[n])) {
                    fprintf(stderr, "bind_properties: "
                            "imei may only contain digits (0 .. 9)\n");
                    goto usage;
                }

                imei.digits[n] = str[n] - '0';
            }

            arg.imei = &imei;
        } else if (strcmp(str, "keys") == 0) {
            if (arg.auth_data != NULL) {
                fprintf(stderr, "bind_properties: "
                        "keys may only be specified once\n");
                goto usage;
            }

            if (!cmd_get_key(&keys.nl_key, "Network", argc, argv) ||
                !cmd_get_key(&keys.nsl_key, "Network subset", argc, argv) ||
                !cmd_get_key(&keys.spl_key, "Service provider", argc, argv) ||
                !cmd_get_key(&keys.cl_key, "Corporate", argc, argv) ||
                !cmd_get_key(&keys.esll_key, "FLexible ESL", argc, argv)) {
                goto usage;
            }

            auth_data.auth_type = COPS_AUTH_TYPE_SIMLOCK_KEYS;
            auth_data.length = sizeof(keys);
            auth_data.data = (uint8_t *)&keys;
            arg.auth_data = &auth_data;
            arg.num_new_auth_data = 1;

        } else if (strcmp(str, "mck") == 0) {
            if (arg.auth_data != NULL) {
                fprintf(stderr, "bind_properties: "
                        "keys may only be specified once\n");
                goto usage;
            }

            if (!cmd_get_key(&mck, "MCK", argc, argv)) {
                goto usage;
            }

            auth_data.auth_type = COPS_AUTH_TYPE_MCK;
            auth_data.length = sizeof(mck);
            auth_data.data = (uint8_t *)&mck;
            arg.auth_data = &auth_data;
            arg.num_new_auth_data = 1;

        } else if (strcmp(str, "auth_data") == 0) {
            fprintf(stderr, "bind_properties: auth_data not supported\n");

            /* Clean up dynamically allocated memory. */
            if (arg.cops_data != NULL) {
                free(COPS_UNCONST(arg.cops_data));
            }

            return COPS_RC_ARGUMENT_ERROR;
        } else if (strcmp(str, "data") == 0) {
            FILE     *f;
            size_t    res;
            uint8_t  *data;
            size_t    datalen;
            uint8_t  *p;
            size_t    l;
            bool      merge = true;

            if (arg.cops_data != NULL) {
                fprintf(stderr, "bind_properties: "
                        "Data may only be specified once\n");
                goto usage;
            }

            str = next_param(argc, argv);

            if (str == NULL) {
                fprintf(stderr, "bind_properties: Data file missing\n");
                goto usage;
            }

            f = fopen(str, "r");

            if (f == NULL) {
                fprintf(stderr, "bind_properties: "
                        "fopen(\"%s\", \"r\") failed: %s\n",
                        str, strerror(errno));
                return COPS_RC_STORAGE_ERROR;
            }

            /*
             * We'd like to do a couple of round in the loop
             * below for testing purposes.
             */
            l = 128;
            p = malloc(l);

            if (p == NULL) {
                fclose(f);
                fprintf(stderr, "malloc\n%s\n", strerror(errno));
                return COPS_RC_MEMORY_ALLOCATION_ERROR;
            }

            datalen = 0;
            data = p;

            for (;;) {
                res = fread(p, 1, l, f);

                if (res != l) {
                    if (ferror(f)) {
                        fprintf(stderr, "bind_properties: "
                                "Error while reading \"%s\": %s\n",
                                str, strerror(errno));
                        fclose(f);
                        free(data);
                        return COPS_RC_STORAGE_ERROR;
                    }
                }

                if (res == 0) {
                    break;
                }

                datalen += res;
                p += res;
                l -= res;

                if (l == 0) {
                    p = realloc(data, datalen * 2);

                    if (p == NULL) {
                        fprintf(stderr, "realloc\n");
                        fclose(f);
                        return COPS_RC_MEMORY_ALLOCATION_ERROR;
                    }

                    data = p;
                    l = datalen;
                    p = data + datalen;
                }
            }

            fclose(f);
            str = next_param(argc, argv);

            if (str != NULL) {
                /* atoi is considered unsafe as it can fail to handle
                   invalid input, but will do the trick in this case. */
                merge = atoi(str);
            }

            arg.cops_data = data;
            data = NULL;
            p = NULL;
            arg.cops_data_length = datalen;
            arg.merge_cops_data = merge;
        } else {
            fprintf(stderr, "bind_properties: Unexpected parameter \"%s\"\n",
                    str);
            goto usage;
        }
    }

    ret_code = cops_bind_properties(ctx, &arg);

    if (arg.cops_data != NULL) {
        free(COPS_UNCONST(arg.cops_data));
    }

    return ret_code;

usage:
    /* Clean up dynamically allocated memory. */
    if (arg.cops_data != NULL) {
        free(COPS_UNCONST(arg.cops_data));
    }

    fprintf(stderr,
            "Usage: bind_properties imei <imei> (15 digits)\n"
            "Usage: bind_properties keys <k1 k2 k3 k4 k5> (keys are space delimited)\n"
            "Usage: bind_properties auth_data <auth_number> <auth data file name>\n"
            "Usage: bind_properties data <data file name> <optional don't merge flag 0, otherwise merge will occur>\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t cmd_read_data(cops_context_id_t *ctx,
                                        int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    char     *str;
    uint8_t  *buf = NULL;
    size_t    blen;
    bool      hexdump = false;
    bool      bindump = false;

    while ((str = next_param(argc, argv)) != NULL) {
        if (*str == '@') {
            (void)restore_param(argc, argv);
            break;
        }

        if (strcmp(str, "hexdump") == 0) {
            if (bindump) {
                fprintf(stderr, "read_data: bindump already specified\n");
                goto usage;
            }

            if (hexdump) {
                fprintf(stderr,
                        "read_data: hexdump may only be specified once\n");
                goto usage;
            }

            hexdump = true;
        } else if (strcmp(str, "bindump") == 0) {
            if (hexdump) {
                fprintf(stderr, "read_data: hexdump already specified\n");
                goto usage;
            }

            if (bindump) {
                fprintf(stderr,
                        "read_data: bindump may only be specified once\n");
                goto usage;
            }

            bindump = true;
        } else {
            fprintf(stderr, "read_data: Unexpected parameter \"%s\"\n", str);
            goto usage;
        }
    }

    if (!hexdump && !bindump) {
        hexdump = true;
    }

    if (bindump && isatty(STDOUT_FILENO)) {
        fprintf(stderr, "read_data: Will not dump binary data on a tty\n");
        return COPS_RC_STORAGE_ERROR;
    }

    ret_code = cops_read_data(ctx, NULL, &blen);

    if (ret_code != COPS_RC_OK) {
        goto function_exit;
    }

    buf = malloc(blen);

    if (buf == NULL) {
        /*lint -e557 Suppress unrecognized format %zu used in Linux */
        fprintf(stderr, "cmd_read_data: malloc(%zu)\n", blen);
        /*lint +e557 */
        return COPS_RC_MEMORY_ALLOCATION_ERROR;
    }

    ret_code = cops_read_data(ctx, buf, &blen);

    if (ret_code != COPS_RC_OK) {
        goto function_exit;
    }

    /*lint -e557 Suppress unrecognized format %zu used in Linux */
    fprintf(stderr, "COPS Data (%zu bytes):\n", blen);
    /*lint +e557 */

    if (hexdump) {
        uint8_t  *p = buf;
        size_t    l = blen;

        while (l > 0) {
            size_t    n;

            printf("%08x  ", (unsigned int)(p - buf));

            for (n = 0; n < 16; n++) {
                if (n < l) {
                    printf("%02x ", (int)p[n]);
                } else {
                    printf("   ");
                }

                if (n == 7) {
                    printf(" ");
                }
            }

            printf(" |");

            for (n = 0; n < 16; n++) {
                if (n < l) {
                    if (isprint(p[n])) {
                        printf("%c", (int)p[n]);
                    } else {
                        printf(".");
                    }
                }
            }

            printf("|\n");

            l -= MIN(l, 16);
            p += 16;
        }
    } else {                    /*bindump */
        if (fwrite(buf, 1, blen, stdout) != blen) {
            /*lint -e557 Suppress unrecognized format %zu used in Linux */
            fprintf(stderr, "read_data: fwrite(buf, 1, %zu, stdout)\n", blen);
            /*lint +e557 */
            ret_code = COPS_RC_STORAGE_ERROR;
            goto function_exit;
        }
    }

function_exit:

    if (buf != NULL) {
        free(buf);
    }

    return ret_code;

usage:
    fprintf(stderr,
            "Usage: read_data [ hexdump | bindump ] (hexdump is default)\n");
    return COPS_RC_ARGUMENT_ERROR;
}

/**
 * @Brief: Read data from file.
 * Read entire content from file, specified by file_name, allocate memory
 * and return pointer to beginning of data and its size.
 * TODO: AP369358
 */
static cops_return_code_t file_read(char *file_name, uint8_t **data,
                                              uint32_t *size)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    FILE *fp = NULL;
    int32_t fsize = 0;

    if (size == NULL) {
        ret_code = COPS_RC_ARGUMENT_ERROR;
        goto function_exit;
    }

    fp = fopen(file_name, "r");
    if (fp == NULL) {
        ret_code = COPS_RC_STORAGE_ERROR;
        fprintf(stderr, "fopen failed. Error: %s\n", strerror(errno));
        goto function_exit;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        ret_code = COPS_RC_STORAGE_ERROR;
        fprintf(stderr, "fseek failed. Error: %s\n", strerror(errno));
        goto function_exit;
    }
    fsize = ftell(fp);
    if (fsize == -1) {
        ret_code = COPS_RC_STORAGE_ERROR;
        fprintf(stderr, "ftell failed. Error: %s\n", strerror(errno));
        goto function_exit;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        ret_code = COPS_RC_STORAGE_ERROR;
        fprintf(stderr, "fseek failed. Error: %s\n", strerror(errno));
        goto function_exit;
    }
    *size = (uint32_t)fsize;

    if (*size > 0) {
        *data = malloc(*size);
        if (*data == NULL) {
            ret_code = COPS_RC_MEMORY_ALLOCATION_ERROR;
            goto function_exit;
        }
        if (fread(*data, *size, 1, fp) == 0) {
            ret_code = COPS_RC_STORAGE_ERROR;
            fprintf(stderr, "fread failed. Error: %s\n", strerror(errno));
        }
    } else {
        *data = NULL;
    }

function_exit:
    if (fp != NULL) {
        fclose(fp);
    }
    return ret_code;
}

/**
 * @Brief: Write data to file.
 * The function write size bytes of data pointed to by data to file file_name.
 *
 * TODO: AP369358
 */
static cops_return_code_t file_write(char *file_name, uint8_t *data,
                                                uint32_t size)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    FILE *fp = NULL;

    if (data == NULL || size == 0) {
        ret_code = COPS_RC_ARGUMENT_ERROR;
        goto function_exit;
    }

    fp = fopen(file_name, "w+");
    if (fp == NULL) {
        ret_code = COPS_RC_STORAGE_ERROR;
        fprintf(stderr, "fopen failed. Error: %s\n", strerror(errno));
        goto function_exit;
    }

    if (fwrite(data, size, 1, fp) == 0) {
        ret_code = COPS_RC_STORAGE_ERROR;
        fprintf(stderr, "fwrite failed. Error: %s\n", strerror(errno));
    }

function_exit:
    if (fp != NULL) {
        fclose(fp);
    }
    return ret_code;
}

static cops_return_code_t cmd_get_nbr_of_otp_rows(cops_context_id_t *ctx,
                                                  int *argc, char **argv[])
{
    cops_return_code_t          ret_code = COPS_RC_OK;
    uint32_t                    nbr_of_otp_rows = 0;

    (void) argc;
    (void) argv;

    ret_code = cops_get_nbr_of_otp_rows(ctx, &nbr_of_otp_rows);
    if (ret_code != COPS_RC_OK) {
        goto function_exit;
    }

    fprintf(stderr, "%u\n", nbr_of_otp_rows);

function_exit:
    return ret_code;
}

/**
 * @Brief: Read OTP.
 * Read OTP data rows specified by input file. Put result in output file.
 */
static cops_return_code_t cmd_read_otp(cops_context_id_t *ctx,
                                        int *argc, char **argv[])
{
    cops_return_code_t          ret_code = COPS_RC_OK;
    char                        *str = NULL;
    char                        *input_file = NULL;
    char                        *output_file = NULL;
    uint32_t                    size = 0;
    uint8_t                     *otp_data = NULL;

    str = next_param(argc, argv);
    if (str == NULL) {
        goto usage;
    }
    input_file = str;

    str = next_param(argc, argv);
    if (str == NULL) {
        goto usage;
    }
    output_file = str;

    ret_code = file_read(input_file, &otp_data, &size);
    if (ret_code != COPS_RC_OK) {
        goto function_exit;
    }

    if (otp_data == NULL || size % 8 != 0 || size == 0) {
        fprintf(stderr, "[cops_cmd] Bad input file format\n");
        ret_code = COPS_RC_ARGUMENT_ERROR;
        goto function_exit;
    }

    ret_code = cops_read_otp(ctx, (uint32_t *)otp_data, size);
    if (ret_code != COPS_RC_OK) {
        goto function_exit;
    }

    ret_code = file_write(output_file, (uint8_t *)otp_data, size);
    if (ret_code != COPS_RC_OK) {
        goto function_exit;
    }
    goto function_exit;

usage:
    fprintf(stderr,
            "Usage: read_otp [Input file] [result file]\n");

function_exit:
    if (otp_data != NULL) {
        free(otp_data);
    }

    return ret_code;
}

/**
 * @Brief: Write data to OTP
 * Write data specified by input file to OTP and write result meta and
 * data to output file.
 */
static cops_return_code_t cmd_write_otp(cops_context_id_t *ctx,
                                        int *argc, char **argv[])
{
    cops_return_code_t          ret_code = COPS_RC_OK;
    char                        *str = NULL;
    char                        *input_file = NULL;
    char                        *output_file = NULL;
    uint32_t                    size = 0;
    uint8_t                     *otp_data = NULL;

    str = next_param(argc, argv);
    if (str == NULL) {
        goto usage;
    }
    input_file = str;

    str = next_param(argc, argv);
    if (str == NULL) {
        goto usage;
    }
    output_file = str;

    ret_code = file_read(input_file, &otp_data, &size);
    if (otp_data == NULL || size == 0 || size % 8 != 0) {
        fprintf(stderr, "[cops_cmd] Bad input file format.\n");
        ret_code = COPS_RC_ARGUMENT_ERROR;
        goto function_exit;
    }

    ret_code = cops_write_otp(ctx, (uint32_t *)otp_data, size);
    if (ret_code != COPS_RC_OK) {
        goto function_exit;
    }

    ret_code = file_write(output_file, otp_data, size);
    if (ret_code != COPS_RC_OK) {
        goto function_exit;
    }
    goto function_exit;

usage:
    fprintf(stderr,
            "Usage: write_otp [Input file] [result file]\n");

function_exit:
    if (otp_data != NULL) {
        free(otp_data);
    }

    return ret_code;
}

static cops_return_code_t cmd_authenticate(cops_context_id_t *ctx,
        int *argc, char **argv[])
{
    cops_return_code_t          ret_code;
    char                        *str;
    cops_simlock_control_keys_t keys;
    cops_auth_data_t            auth_data;
    bool                        permanently = false;
    FILE                        *fp;
    uint8_t                     *filedata = NULL;
    size_t                      size, fsize;
    long                        tmp_size;

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    if (strcmp(str, "-p") == 0) {
        permanently = true;
        str = next_param(argc, argv);

        if (str == NULL) {
            goto usage;
        }
    }

    if (strcmp(str, "keys") == 0) {
        if (!cmd_get_key(&keys.nl_key, "Network", argc, argv) ||
                !cmd_get_key(&keys.nsl_key, "Network subset", argc, argv) ||
                !cmd_get_key(&keys.spl_key, "Service provider", argc, argv) ||
                !cmd_get_key(&keys.cl_key, "Corporate", argc, argv) ||
                !cmd_get_key(&keys.esll_key, "FLexible ESL", argc, argv)) {
            goto usage;
        }

        auth_data.auth_type = COPS_AUTH_TYPE_SIMLOCK_KEYS;
        auth_data.length = sizeof(keys);
        auth_data.data = (uint8_t *)&keys;
    } else if (strcmp(str, "rsa") == 0) {
        str = next_param(argc, argv);

        if (str == NULL) {
            goto usage;
        }

        fp = fopen(str, "r");

        if (fp == NULL) {
            fprintf(stderr, "failed to open file %s\n%s\n",
                    str, strerror(errno));
            return COPS_RC_STORAGE_ERROR;
        }

        if (fseek(fp, 0, SEEK_END) != 0) {
            fprintf(stderr, "failed to seek end of file %s\n%s\n",
                    str, strerror(errno));
            fclose(fp);
            return COPS_RC_STORAGE_ERROR;
        }

        tmp_size = ftell(fp);

        if (tmp_size >= 0) {
            size = tmp_size;
        } else {
            fprintf(stderr, "ftell returned negative offset\n");
            fclose(fp);
            return COPS_RC_STORAGE_ERROR;
        }

        if (fseek(fp, 0, SEEK_SET) != 0) {
            fprintf(stderr, "Failed to rewind file %s\n", str);
            fclose(fp);
            return COPS_RC_STORAGE_ERROR;
        }

        filedata = malloc(size);

        if (filedata == NULL) {
            /*lint -e557 Suppress unrecognized format %zu used in Linux */
            fprintf(stderr, "failed to allocate %zu bytes of memory\n%s\n",
                    size, strerror(errno));
            /*lint +e557 */
            fclose(fp);
            return COPS_RC_MEMORY_ALLOCATION_ERROR;
        }

        fsize = fread(filedata, size, sizeof(uint8_t), fp);

        if ((fsize * size) != size) {
            /*lint -e557 Suppress unrecognized format %zu used in Linux */
            fprintf(stderr, "read too few bytes from file. fsize = %zu, "
                    "size = %zu\n%s\n", fsize, size, strerror(errno));
            /*lint +e557 */
            if (filedata != NULL) {
                    free(filedata);
            }
            fclose(fp);
            return COPS_RC_STORAGE_ERROR;
        }

        auth_data.auth_type = COPS_AUTH_TYPE_RSA_CHALLENGE;
        auth_data.length = size;
        auth_data.data = filedata;

        fclose(fp);
    } else if (strcmp(str, "perm") == 0) {
        auth_data.auth_type = COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION;
        auth_data.length = 0;
        auth_data.data = NULL;
    } else {
        fprintf(stderr, "authenticate: Unexpected parameter \"%s\"\n", str);
        goto usage;
    }

    ret_code = cops_authenticate(ctx, permanently, &auth_data);

    if (filedata != NULL) {
        free(filedata);
    }

    return ret_code;

usage:
    fprintf(stderr,
            "Usage: authenticate [-p] keys <k1 k2 k3 k4 k5>\n"
            "Usage: authenticate [-p] rsa <file with signed challenge>\n"
            "Usage: authenticate [-p] perm (use permanent auth flag from OTP)\n"
            " -p -> set permanent auth flag in OTP\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t
cmd_verify_simlock_control_keys(cops_context_id_t *ctx, int *argc,
                                char **argv[])
{
    cops_return_code_t ret_code;
    cops_simlock_control_keys_t keys;

    if (!cmd_get_key(&keys.nl_key, "Network", argc, argv) ||
            !cmd_get_key(&keys.nsl_key, "Network subset", argc, argv) ||
            !cmd_get_key(&keys.spl_key, "Service provider", argc, argv) ||
            !cmd_get_key(&keys.cl_key, "Corporate", argc, argv) ||
            !cmd_get_key(&keys.esll_key, "FLexible ESL", argc, argv)) {
        goto usage;
    }

    ret_code = cops_simlock_verify_control_keys(ctx, &keys);

    return ret_code;

usage:
    fprintf(stderr, "Usage: key_ver <k1 k2 k3 k4 k5>\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t
cmd_deauthenticate(cops_context_id_t *ctx, int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    char     *str;
    bool      permanently = false;

    str = next_param(argc, argv);

    if (str != NULL) {
        if (strcmp(str, "-p") == 0) {
            permanently = true;
        } else {
            if (*str != '@') {
                goto usage;
            }

            (void)restore_param(argc, argv);
        }
    }

    ret_code = cops_deauthenticate(ctx, permanently);
    return ret_code;

usage:
    fprintf(stderr,
            "Usage: deauthenticate [-p]\n"
            " -p -> clear permanent auth flag in OTP\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t cmd_get_challenge(cops_context_id_t *ctx,
        int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    uint8_t   challenge[COPS_CHALLENGE_SIZE];
    size_t    clen = 0;

    (void) argc;
    (void) argv;

    clen = COPS_CHALLENGE_SIZE;

    ret_code = cops_get_challenge(ctx, COPS_AUTH_TYPE_RSA_CHALLENGE,
                                  challenge, &clen);

    if (ret_code == COPS_RC_OK) {
        uint8_t  *p = challenge;
        size_t    l = COPS_CHALLENGE_SIZE;

        while (l > 0) {
            size_t    n;

            printf("%08x  ", (unsigned int)(p - challenge));

            for (n = 0; n < 16; n++) {
                if (n < l) {
                    printf("%02x ", (int)p[n]);
                } else {
                    printf("   ");
                }

                if (n == 7) {
                    printf(" ");
                }
            }

            printf(" |");

            for (n = 0; n < 16; n++) {
                if (n < l) {
                    if (isprint(p[n])) {
                        printf("%c", (int)p[n]);
                    } else {
                        printf(".");
                    }
                }
            }

            printf("|\n");

            l -= MIN(l, 16);
            p += 16;
        }
    }

    return ret_code;
}

static cops_return_code_t cmd_modem_sipc_mx(cops_context_id_t *ctx,
        int *argc, char **argv[])
{
    char     *str;
    FILE     *input;
    FILE     *output;
    cops_return_code_t ret_code;
    uint8_t  *in = NULL;
    uint8_t  *out = NULL;
    size_t    inlen;
    size_t    outlen = 0;

    str = next_param(argc, argv);

    if (str == NULL) {
        fprintf(stderr, "modem_sipc_mx: expected input file\n");
        goto usage;
    }

    input = fopen(str, "r");

    if (input == NULL) {
        fprintf(stderr, "cmd_modem_sipc_mx: fopen(\"%s\", \"r\")\n%s\n", str,
                strerror(errno));
        return COPS_RC_STORAGE_ERROR;
    }

    str = next_param(argc, argv);

    if (str == NULL) {
        fprintf(stderr, "modem_sipc_mx: expected output file\n");
        fclose(input);
        goto usage;
    }

    output = fopen(str, "w");

    if (output == NULL) {
        fprintf(stderr, "cmd_modem_sipc_mx: fopen(\"%s\", \"w\")\n%s\n", str,
                strerror(errno));
        fclose(input);
        return COPS_RC_STORAGE_ERROR;
    }

    str = next_param(argc, argv);

    if (str != NULL) {
        if (*str != '@') {
            fclose(input);
            fclose(output);
            goto usage;
        }

        (void)restore_param(argc, argv);
    }

    in = malloc(1024);

    if (in == NULL) {
        fprintf(stderr, "malloc\n%s\n", strerror(errno));
        fclose(input);
        fclose(output);
        return COPS_RC_MEMORY_ALLOCATION_ERROR;
    }

    inlen = fread(in, 1, 1024, input);

    if (ferror(input)) {
        fprintf(stderr, "fread.\n%s\n", strerror(errno));
        free(in);
        fclose(input);
        fclose(output);
        return COPS_RC_STORAGE_ERROR;
    }

    if (!feof(input)) {
        fprintf(stderr, "cmd_modem_sipc_mx: expected end of file\n");
        free(in);
        fclose(input);
        fclose(output);
        return COPS_RC_STORAGE_ERROR;
    }

    ret_code = cops_modem_sipc_mx(ctx, in, inlen, &out, &outlen);

    if (ret_code == COPS_RC_OK) {
        size_t    res;

        res = fwrite(out, 1, outlen, output);

        if (res != outlen) {
            fprintf(stderr, "fwrite\n%s\n", strerror(errno));
            free(in);
            fclose(input);
            fclose(output);
            free(out);
            return COPS_RC_STORAGE_ERROR;
        }
    }

    if (in != NULL) {
        free(in);
    }

    if (out != NULL) {
        free(out);
    }

    fclose(input);
    fclose(output);
    return ret_code;

usage:
    fprintf(stderr, "Usage: modem_sipc_mx <input file> <output file>\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static bool check_return_value(const char *func_str,
                               cops_return_code_t ret_code,
                               const char *str)
{
    char     *s;
    char     *s1 = NULL;
    char     *saveptr = NULL;
    char     *token;
    bool      rc_ok = false;

    if (ret_code != COPS_RC_OK) {
        fprintf(stderr, "%s returned %d\n", func_str, ret_code);
    }

    if (*str != '@') {
        fprintf(stderr, "Invalid command separator \"%s\"\n", str);
        return false;
    }

    if (strcmp(str, "@") == 0) {
        if (ret_code == 0) {
            rc_ok = true;
        }
    } else {
        s1 = strdup(str + 1);

        if (s1 == NULL) {
            fprintf(stderr, "strdup\n%s\n", strerror(errno));
            return false;
        }

        s = s1;

        while ((token = strtok_r(s, ",", &saveptr)) != NULL) {
            char     *endptr;
            long      v = strtol(token, &endptr, 10);

            s = NULL;

            if (*endptr != '\0') {
                fprintf(stderr, "Command separator \"%s\" contains "
                        "invalid number\n", str);
                rc_ok = false;
                break;
            }

            if (v == (long)ret_code || v == -1) {
                rc_ok = true;
            }
        }
    }

    if (s1 != NULL) {
        free(s1);
    }

    return rc_ok;
}

static char *first_argv = NULL;

static char *next_param(int *argc, char **argv[])
{
    char     *str;

    if (*argc <= 0) {
        return NULL;
    }

    if (first_argv == NULL) {
        first_argv = **argv;
    }

    str = **argv;

    (*argc)--;
    (*argv)++;
    return str;
}

static char *restore_param(int *argc, char **argv[])
{
    if (**argv == first_argv) {
        return NULL;
    }

    (*argc)++;
    (*argv)--;
    return **argv;
}

static cops_return_code_t
cmd_get_device_state(cops_context_id_t *ctx, int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    cops_device_state_t device_state;

    (void) argc;
    (void) argv;

    ret_code = cops_get_device_state(ctx, &device_state);

    if (ret_code == COPS_RC_OK) {
        fprintf(stderr, "Device state: %d\n", device_state);

        if (COPS_DEVICE_STATE_RELAXED == device_state) {
            printf("Device state: COPS_DEVICE_STATE_RELAXED\n");
        } else if (COPS_DEVICE_STATE_FULL == device_state) {
            printf("Device state: COPS_DEVICE_STATE_FULL\n");
        } else {
            printf("Device state: UNDEFINED\n");
        }
    }

    return ret_code;
}

static cops_return_code_t
cmd_verify_imsi(cops_context_id_t *ctx, int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    char     *str;
    struct cops_simlock_imsi imsi;
    int i;
    unsigned long val;
    char *endptr;

    if (*argc != COPS_PACKED_IMSI_LENGTH) {
        fprintf(stderr, "Too few digits in IMSI (%u)\n", *argc);
        goto usage;
    }

    for (i = 0; i < COPS_PACKED_IMSI_LENGTH; i++) {
        str = next_param(argc, argv);

        if (str == NULL) {
            goto usage;
        }
        val = strtoul(str, &endptr, 0);
        if (*endptr != '\0') {
            fprintf(stderr, "Invalid IMSI: %s\n", str);
            goto usage;
        }
        if (val > 0xff) {
            fprintf(stderr, "Invalid IMSI value: %s\n", str);
            goto usage;
        }

        imsi.data[i] = (uint8_t) val;
    }

    ret_code = cops_simlock_verify_imsi(ctx, &imsi);
    if (ret_code == COPS_RC_OK) {
        fprintf(stderr, "IMSI ok\n");
    } else {
        fprintf(stderr, "IMSI NOT ok\n");
    }
    return ret_code;

usage:
    fprintf(stderr, "Usage: verify_imsi 0x01 0x02 0x03 0x04 0x05 0x05 0x07 "
            "0x08 0x09\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t
cmd_bind_data(cops_context_id_t *ctx, int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    char     *str;
    unsigned long val;
    char *endptr;
    uint8_t *buf;
    uint32_t blen;
    struct cops_digest mac;

    memset(&mac, 0, sizeof(struct cops_digest));

    if (*argc != 2) {
        goto usage;
    }

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    val = strtoul(str, &endptr, 0);
    if (*endptr != '\0') {
        fprintf(stderr, "invalid address: %s\n", str);
        goto usage;
    }
    buf = (uint8_t *)val;

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    val = strtoul(str, &endptr, 0);
    if (*endptr != '\0') {
        fprintf(stderr, "invalid length: %s\n", str);
        goto usage;
    }
    blen = val;

    ret_code = cops_bind_data(ctx, buf, blen, &mac);

    if (ret_code == COPS_RC_OK) {
        uint8_t  *p = mac.value;
        size_t    l = 32;
        while (l > 0) {
            size_t    n;

            printf("%08x  ", (unsigned int)(p - mac.value));

            for (n = 0; n < 16; n++) {
                if (n < l) {
                    printf("%02x ", (int)p[n]);
                } else {
                    printf("   ");
                }

                if (n == 7) {
                    printf(" ");
                }
            }

            printf(" |");

            for (n = 0; n < 16; n++) {
                if (n < l) {
                    if (isprint(p[n])) {
                        printf("%c", (int)p[n]);
                    } else {
                        printf(".");
                    }
                }
            }

            printf("|\n");

            l -= MIN(l, 16);
            p += 16;
        }
    } else {
        fprintf(stderr, "Returned error code %u\n", ret_code);
    }
    return ret_code;

usage:
    fprintf(stderr, "Usage: bind_data <address> <length>\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t
cmd_verify_data_binding(cops_context_id_t *ctx, int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    char     *str;
    unsigned long val;
    char *endptr;
    uint8_t *buf;
    uint32_t blen;
    struct cops_digest mac;
    int i;

    if (*argc != 34) {
        fprintf(stderr, "Too few arguments (%d)\n", *argc);
        goto usage;
    }

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    val = strtoul(str, &endptr, 0);
    if (*endptr != '\0') {
        fprintf(stderr, "invalid address: %s\n", str);
        goto usage;
    }
    buf = (uint8_t *)val;

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    val = strtoul(str, &endptr, 0);
    if (*endptr != '\0') {
        fprintf(stderr, "invalid length: %s\n", str);
        goto usage;
    }
    blen = val;

    for (i = 0; i < 32; i++) {
        str = next_param(argc, argv);

        if (str == NULL) {
            goto usage;
        }
        val = strtoul(str, &endptr, 0);
        if (*endptr != '\0') {
            fprintf(stderr, "invalid MAC: %s\n", str);
            goto usage;
        }
        if (val > 0xff) {
            fprintf(stderr, "Invalid IMSI MAC digit: %s\n", str);
            goto usage;
        }

        mac.value[i] = (uint8_t) val;
    }

    ret_code = cops_verify_data_binding(ctx, buf, blen, &mac);

    if (ret_code == COPS_RC_OK) {
        fprintf(stderr, "MAC ok\n");
    } else {
        fprintf(stderr, "MAC not ok\n");
    }

    return ret_code;

usage:
    fprintf(stderr, "Usage: verify_data_binding <address> <length> "
            "<32 bytes mac>\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t cmd_verify_signed_header(cops_context_id_t *ctx,
                                                   int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    char     *str;
    unsigned long int pltype;
    char *endptr;
    struct cops_vsh hinfo;
    FILE *fp = NULL;
    size_t size, fsize;
    long tmp_size;
    uint8_t *filedata = NULL;

    memset(&hinfo, 0, sizeof(struct cops_vsh));

    if (*argc != 2) {
        goto usage;
    }

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    pltype = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "invalid payload type: %s\n", str);
        goto usage;
    }

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    fp = fopen(str, "r");

    if (fp == NULL) {
        fprintf(stderr, "failed to open file %s\n%s\n",
                str, strerror(errno));
        return COPS_RC_STORAGE_ERROR;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fprintf(stderr, "failed to seek end of file %s\n%s\n",
                str, strerror(errno));
        fclose(fp);
        return COPS_RC_STORAGE_ERROR;
    }

    tmp_size = ftell(fp);

    if (tmp_size >= 0) {
        size = tmp_size;
    } else {
        fprintf(stderr, "ftell returned negative offset\n");
        fclose(fp);
        return COPS_RC_STORAGE_ERROR;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Failed to rewind file %s\n", str);
        fclose(fp);
        return COPS_RC_STORAGE_ERROR;
    }

    filedata = malloc(size);

    if (filedata == NULL) {
        /*lint -e557 Suppress unrecognized %zu which is used in Linux */
        fprintf(stderr, "failed to allocate %zu bytes of memory\n%s\n",
                size, strerror(errno));
        /*lint +e557 */
        fclose(fp);
        return COPS_RC_MEMORY_ALLOCATION_ERROR;
    }

    /* coverity[tainted_data_argument] - fread reads valid data to filedata. */
    fsize = fread(filedata, sizeof(uint8_t), size, fp);

    if (fsize != size) {
        /*lint -e557 Suppress unrecognized format %zu used in Linux */
        fprintf(stderr, "read too few bytes from file. fsize = %zu, "
                "size = %zu\n%s\n", fsize, size, strerror(errno));
        /*lint +e557 */
        free(filedata);
        fclose(fp);
        return COPS_RC_STORAGE_ERROR;
    }

    /* coverity[tainted_data] - filedata is valid and not tainted. */
    ret_code = cops_verify_signed_header(ctx, filedata,
                                         (enum cops_payload_type)pltype,
                                         &hinfo);

    fclose(fp);

    if (ret_code == COPS_RC_OK) {
        fprintf(stderr, "Signed header ok\n");
    } else {
        fprintf(stderr, "Signed header not ok\n");
    }

    free(filedata);

    return ret_code;

usage:
    fprintf(stderr, "Usage: verify_signed_header <payload type> "
            "</path/to/signed header>\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t
cmd_calcdigest(cops_context_id_t *ctx, int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    char     *str;
    unsigned long val;
    char *endptr;
    uint8_t *buf;
    uint32_t blen;
    enum cops_hash_type ht;
    struct cops_digest hash;

    memset(&hash, 0, sizeof(struct cops_digest));

    if (*argc != 3) {
        goto usage;
    }

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    val = strtoul(str, &endptr, 0);
    if (*endptr != '\0') {
        fprintf(stderr, "invalid hash_type: %s\n", str);
        goto usage;
    }
    ht = (enum cops_hash_type)val;

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    val = strtoul(str, &endptr, 0);
    if (*endptr != '\0') {
        fprintf(stderr, "invalid address: %s\n", str);
        goto usage;
    }
    buf = (uint8_t *)val;

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    val = strtoul(str, &endptr, 0);
    if (*endptr != '\0') {
        fprintf(stderr, "invalid length: %s\n", str);
        goto usage;
    }
    blen = val;

    ret_code = cops_calcdigest(ctx, ht, buf, blen, &hash);

    if (ret_code == COPS_RC_OK) {
        uint8_t  *p = hash.value;
        size_t    l;
        if (ht == COPS_HASH_TYPE_SHA1_HASH) {
            l = 20;
        } else {
            l = 32;
        }
        while (l > 0) {
            size_t    n;

            printf("%08x  ", (unsigned int)(p - hash.value));

            for (n = 0; n < 16; n++) {
                if (n < l) {
                    printf("%02x ", (int)p[n]);
                } else {
                    printf("   ");
                }

                if (n == 7) {
                    printf(" ");
                }
            }

            printf(" |");

            for (n = 0; n < 16; n++) {
                if (n < l) {
                    if (isprint(p[n])) {
                        printf("%c", (int)p[n]);
                    } else {
                        printf(".");
                    }
                }
            }

            printf("|\n");

            l -= MIN(l, 16);
            p += 16;
        }
    } else {
        fprintf(stderr, "Returned error code %u\n", ret_code);
    }
    return ret_code;

usage:
    fprintf(stderr, "Usage: calcdigest <hash type> <address> <length>\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t cmd_lock_bootpartition(cops_context_id_t *ctx,
        int *argc, char **argv[])
{
    (void) argc;
    (void) argv;

    return cops_lock_bootpartition(ctx);
}

static cops_return_code_t
cmd_init_arb_table(cops_context_id_t *ctx, int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    char *str;
    cops_init_arb_table_args_t arb_args;
    uint16_t modelid;

    str = next_param(argc, argv);

    if (str == NULL) {
        goto usage;
    }

    if (strcmp(str, "modelid") == 0) {
        str = next_param(argc, argv);

        if (str == NULL) {
            goto usage;
        }

        modelid = strtol(str, NULL, 0);

        arb_args.arb_data_type = COPS_ARB_DATA_TYPE_MODELID;
        arb_args.data = (uint8_t *)&modelid;
        arb_args.data_length = sizeof(modelid);
    } else {
        fprintf(stderr, "cmd_init_arb_table: Unexpected parameter \"%s\"\n",
                str);
        goto usage;
    }

    ret_code = cops_init_arb_table(ctx, &arb_args);
    return ret_code;

usage:
    fprintf(stderr,
            "Usage: init_arb_table modelid <modelid>\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t
cmd_write_secprofile(cops_context_id_t *ctx, int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    cops_write_secprofile_args_t secprofiledata = {
        0, 0, 0, NULL, COPS_SEC_PROFILE_DATA_NONE, 0, NULL
    };
    char *str = 0;
    char *hashfilename = 0;
    FILE *hashfile = 0;
    long hashfilesize = 0;
    void *hashbuffer = 0;

    str = next_param(argc, argv);

    if (str == NULL) {
        fprintf(stderr, "write_secprofile: expected version number\n");
        goto usage;
    }

    secprofiledata.version = strtol(str, NULL, 0);
    str = next_param(argc, argv);

    if (str == NULL) {
        fprintf(stderr, "write_secprofile: expected flags\n");
        goto usage;
    }

    secprofiledata.flags = strtol(str, NULL, 0);
    str = next_param(argc, argv);

    if (str != NULL && *str != '@') {
        hashfilename = str;

        str = next_param(argc, argv);

        if (str != NULL) {
            if (*str != '@') {
                goto usage;
            }

            (void)restore_param(argc, argv);
        }

        hashfile = fopen(hashfilename, "r");

        if (hashfile == NULL) {
            fprintf(stderr, "write_secprofile: fopen(\"%s\", \"r\")\n%s\n",
                    hashfilename, strerror(errno));
            exit(1);
        }

        if (fseek(hashfile, 0, SEEK_END) == -1) {
            fprintf(stderr, "write_secprofile: fseek failed on file %s\n%s\n",
                    hashfilename, strerror(errno));
            exit(1);
        }

        hashfilesize = ftell(hashfile);

        if (hashfilesize == -1) {
            fprintf(stderr, "write_secprofile: getting size of file %s "
                    "failed\n%s\n", hashfilename, strerror(errno));
            exit(1);
        }

        if (fseek(hashfile, 0, SEEK_SET) == -1) {
            fprintf(stderr, "write_secprofile: fseek failed on file %s\n%s\n",
                    hashfilename, strerror(errno));
            exit(1);
        }

        hashbuffer = malloc(hashfilesize);

        if (hashbuffer == NULL) {
            fprintf(stderr, "malloc\n%s\n", strerror(errno));
            exit(1);
        }

        (void)fread(hashbuffer, 1, hashfilesize, hashfile);

        if (ferror(hashfile)) {
            fprintf(stderr, "fread.\n%s\n", strerror(errno));
            exit(1);
        }

        fclose(hashfile);

        secprofiledata.hashlist = hashbuffer;
        secprofiledata.hashlist_len = hashfilesize;
    } else {
        if (str != NULL && *str == '@') {
            (void)restore_param(argc, argv);
        }
    }

    ret_code = cops_write_secprofile(ctx, &secprofiledata);

    if (hashbuffer != NULL) {
        free(hashbuffer);
    }

    return ret_code;

usage:
    fprintf(stderr,
            "Usage: write_secprofile <version> <flags> <file with hashes>\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t cmd_change_simkey(cops_context_id_t *ctx,
                                            int *argc, char **argv[])
{
    cops_simlock_control_key_t old_key;
    cops_simlock_control_key_t new_key;

    if (!cmd_get_key(&old_key, "old_key", argc, argv)) {
        goto usage;
    }

    if (!cmd_get_key(&new_key, "new_key", argc, argv)) {
        goto usage;
    }

    return cops_simlock_change_sim_control_key(ctx, &old_key, &new_key);

usage:
    fprintf(stderr, "Usage: change_simkey <old_key> <new_key>\n");

    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t cmd_simlock_lock(cops_context_id_t *ctx,
                                           int *argc, char **argv[])
{
    struct cops_simlock_lock_arg lockargs;
    struct cops_simlock_explicitlockdata explicitdata;
    cops_return_code_t ret_code = COPS_RC_OK;
    char *str;
    int n;
    FILE *input;
    size_t inlen;
    long size;
    void *settingsdata = NULL;
    char *endptr;

    memset(&lockargs, 0, sizeof(lockargs));
    memset(&explicitdata, 0, sizeof(explicitdata));
    memset(&explicitdata.imsi, 0xFF, sizeof(explicitdata.imsi));

    str = next_param(argc, argv);
    if (str == NULL) {
        goto usage;
    } else {
        (void)restore_param(argc, argv);
    }

    while ((str = next_param(argc, argv)) != NULL) {

        if (strcmp(str, "lm") == 0) {
            str = next_param(argc, argv);

            if (str == NULL) {
                fprintf(stderr, "lock: lockmode expexted\n");
                goto usage;
            }

            if (strcmp(str, "al") == 0) {
                lockargs.lockmode = COPS_SIMLOCK_LOCK_MODE_AUTOLOCK;
            } else if (strcmp(str, "alcnl") == 0) {
                lockargs.lockmode = COPS_SIMLOCK_LOCK_MODE_AUTOLOCK_TO_CNL;
            } else if (strcmp(str, "nl") == 0) {
                lockargs.lockmode = COPS_SIMLOCK_LOCK_MODE_NORMAL_LOCK;
            } else if (strcmp(str, "nlcnl") == 0) {
                lockargs.lockmode = COPS_SIMLOCK_LOCK_MODE_NORMAL_LOCK_TO_CNL;
            } else if (strcmp(str, "e") == 0) {
                lockargs.lockmode = COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_IMSI;
            } else if (strcmp(str, "ecnl") == 0) {
                lockargs.lockmode = COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_CNL;
            } else if (strcmp(str, "edata") == 0) {
                lockargs.lockmode =
                    COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_SETTINGS_DATA;
            } else {
                fprintf(stderr, "lock: unknown lockmode: %s\n", str);
                goto usage;
            }
            continue;
        } else if (strcmp(str, "lt") == 0) {
            str = next_param(argc, argv);

            if (str == NULL) {
                fprintf(stderr, "lock: locktype expexted\n");
                goto usage;
            } else {
                if (!cmd_get_lock(str,
                     (cops_simlock_lock_type_t *)&lockargs.lockdata.locktype)) {
                    goto usage;
                }
            }
            continue;
        } else if (strcmp(str, "key") == 0) {
            if (!cmd_get_key(&lockargs.lockdata.controlkey, "key", argc,
                             argv)) {
                goto usage;
            }
            continue;
        }
        if (strcmp(str, "lockop") == 0) {
            str = next_param(argc, argv);

            if (str == NULL) {
                fprintf(stderr, "lock: lock defintion expected\n");
                goto usage;
            }
            lockargs.lockdata.lockop = strtoul(str, &endptr, 0);
            if (*endptr != '\0') {
                fprintf(stderr, "lock: invalid lock operation: %s\n", str);
                goto usage;
            }
            continue;
        }

        if (strcmp(str, "ld") == 0) {
            lockargs.lockdata.update_lockdef = true;

            str = next_param(argc, argv);

            if (str == NULL) {
                fprintf(stderr, "lock: lock defintion expected\n");
                goto usage;
            }
            lockargs.lockdata.lockdef = strtoul(str, &endptr, 0);
            if (*endptr != '\0') {
                fprintf(stderr, "lock: invalid lock definition: %s\n", str);
                goto usage;
            }
            continue;
        }

        if (strcmp(str, "imsi") == 0) {
            if (lockargs.lockmode !=
                COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_IMSI) {
                fprintf(stderr, "lock: imsi only expected for lm \"e\"\n");
                goto usage;
            }

            str = next_param(argc, argv);

            if (str == NULL || strlen(str) < 1) {
                    fprintf(stderr,
                        "lock: imsi_bs must contain %u digits\n", 1);
                goto usage;
            }

            inlen = strlen(str);

            for (n = 0; n < (ssize_t) inlen; n++) {
                if (!isdigit(str[n])) {
                    fprintf(stderr, "lock: "
                            "imsi must contain digits (0..9)\n");
                    goto usage;
                } else {
                    str[n] -= '0';
                }

                memcpy(explicitdata.imsi.data, str, n+1);
            }
            lockargs.explicitlockdata = &explicitdata;
            continue;
        }

        /* check relevance according to lock type */

        if (strcmp(str, "gid1") == 0) {
            if (lockargs.lockmode !=
                COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_IMSI) {
                fprintf(stderr, "lock: GID1 only expected for lm \"e\"\n");
                goto usage;
            }

            str = next_param(argc, argv);

            if (str == NULL) {
                fprintf(stderr, "lock: no gid1 supplied\n");
                goto usage;
            }
            explicitdata.gid1 = strtoul(str, &endptr, 0);
            if (*endptr != '\0') {
                fprintf(stderr, "invalid gid1 %s. Range 0-255 allowed\n",
                        str);
                goto usage;
            }
            continue;
        }

        if (strcmp(str, "gid2") == 0) {
            if (lockargs.lockmode !=
                COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_IMSI) {
                fprintf(stderr, "lock: GID2 only expected for lm \"e\"\n");
                goto usage;
            }

            str = next_param(argc, argv);

            if (str == NULL) {
                fprintf(stderr, "lock: no gid2 supplied\n");
                goto usage;
            }
            explicitdata.gid2 = strtoul(str, &endptr, 0);
            if (*endptr != '\0') {
                fprintf(stderr, "invalid gid2 %s. Range 0-255 allowed\n",
                        str);
                goto usage;
            }
            continue;
        }

        if (strcmp(str, "data") == 0) {
            str = next_param(argc, argv);

            if (str == NULL) {
                fprintf(stderr, "lock: expected input file\n");
                goto usage;
            }

            input = fopen(str, "r");

            if (input == NULL) {
                fprintf(stderr, "lock: fopen(\"%s\", \"r\"): %s\n", str,
                        strerror(errno));
                goto io_error;
            }

            if (fseek(input, 0, SEEK_END) != 0) {
                fprintf(stderr, "lock: failed to seek end of file %s\n%s\n",
                        str, strerror(errno));
                goto io_error;
            }

            size = ftell(input);
            if (size == -1) {
                fprintf(stderr, "lock: ftell returned error: %s\n",
                        strerror(errno));
                goto io_error;
            }

            if (fseek(input, 0, SEEK_SET) != 0) {
                fprintf(stderr, "lock: failed to rewind file %s\n", str);
                goto io_error;
            }

            settingsdata = malloc(size);
            if (settingsdata == NULL) {
                fprintf(stderr, "couldn't allocate %lu bytes of data\n", size);
                goto io_error;
            }

            inlen = fread(settingsdata, 1, size, input);
            if (inlen != (size_t)size) {
                /*lint -e557 Suppress unrecognized format %zu used in Linux */
                fprintf(stderr, "lock: read %zu bytes, expected %lu\n", inlen,
                        size);
                /*lint +e557 */
                free(settingsdata);
                goto io_error;
            }

            fclose(input);

            explicitdata.data = settingsdata;
            explicitdata.length = size;
            lockargs.explicitlockdata = &explicitdata;
            continue;
        } else {
            fprintf(stderr, "lock: unknown command %s\n", str);
            goto usage;
        }
    }


    ret_code = cops_simlock_lock(ctx, &lockargs);

    if (explicitdata.data != NULL) {
        free(explicitdata.data);
    }

    return ret_code;

usage:
    fprintf(stderr,
            "Usage: lock lm <lockmode:\n"
            "                al (autolock)\n"
            "                alcnl (autolock/CNL)\n"
            "                nl (normal lock)\n"
            "                nlcnl (normal lock/CNL)\n"
            "                e (explicit/IMSI)\n"
            "                ecnl (explicit/CNL)\n"
            "                edata (explicit/data)>\n"
            "            lt <lock type:\n"
            "                nl (network lock)\n"
            "                nsl (netowrk subset lock)\n"
            "                spl (service provider lock)\n"
            "                cl (corporate lock)\n"
            "                siml ((U)SIM Personalization lock\n"
            "                esll (Flexible ESL lock>\n"
            "            key <controlkey>\n"
            "            lockop <lock operation>\n"
            "            <ld lockdefintion>\n"
            "            imsi <imsi> [GID1, GID2] (only valid for lm \"e\")\n"
            "            data <filename> (only valid for lm \"ecnl\" or "
            "\"edata\")\n");

    return COPS_RC_ARGUMENT_ERROR;

io_error:
    if (input != NULL) {
        fclose(input);
    }

    if (explicitdata.data != NULL) {
        free(explicitdata.data);
    }

    return COPS_RC_STORAGE_ERROR;
}

static cops_return_code_t cmd_write_rpmb_key(cops_context_id_t *ctx,
        int *argc, char **argv[])
{
    uint16_t dev_id = 0;
    bool commercial = false;

    char *str;

    if (*argc != 2) {
        goto usage;
    }

    str = next_param(argc, argv);
    if (str == NULL) {
        goto usage;
    }

    dev_id = (uint16_t)strtoul(str, (char **)NULL, 10);
    if (errno != 0) {
        fprintf(stderr, "invalid dev_id! (errno %s)\n", strerror(errno));
        goto usage;
    }

    str = next_param(argc, argv);
    if (str == NULL) {
        goto usage;
    }

    if (strcmp(str, "0") == 0)
        commercial = false;
    else if (strcmp(str, "1") == 0)
        commercial = true;
    else
        goto usage;

    return cops_write_rpmb_key(ctx, dev_id, commercial);

usage:
    fprintf(stderr,
            "Usage: write_rpmb_key <dev_id> <commercial (0 or 1)>\n");
    return COPS_RC_ARGUMENT_ERROR;
}

static cops_return_code_t cmd_get_product_debug_settings(cops_context_id_t *ctx,
                                                       int *argc, char **argv[])
{
    cops_return_code_t ret_code;
    uint32_t debug_settings = 0;

    (void)argc;
    (void)argv;

    ret_code = cops_get_product_debug_settings(ctx, &debug_settings);

    if (ret_code == COPS_RC_OK) {
        fprintf(stderr, "JTAG debug Modem = %d, JTAG debug APE = %d "
                "(enabled = 1, disabled = 0)\n",
                ((debug_settings & COPS_FLAG_JTAG_ENABLED_MODEM) > 0),
                ((debug_settings & COPS_FLAG_JTAG_ENABLED_APE) > 0));
    } else {
        fprintf(stderr, "JTAG debug settings unknown\n");
    }

    return ret_code;
}
