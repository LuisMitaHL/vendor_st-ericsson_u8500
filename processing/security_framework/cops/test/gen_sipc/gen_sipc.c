/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <sys/types.h>

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

/*
 * copied from sec_gits/issw/delivery/issw/isswapi_types.h
 * to avoid inclusion problems between cops git and issw git.
 */
typedef struct issw_signed_header {
    uint32_t    magic;
    uint16_t    size_of_signed_header;
    uint16_t    size_of_signature;
    uint32_t    sign_hash_type; /* see t_hash_type */
    uint32_t    signature_type; /* see t_signature_type */
    uint32_t    hash_type;      /* see t_hash_type */
    uint32_t    payload_type;   /* see enum issw_payload_type */
    uint32_t    flags;          /* reserved */
    uint32_t    size_of_payload;
    uint32_t    sw_vers_nbr;
    uint32_t    load_address;
    uint32_t    startup_address;
    uint32_t    spare;          /* reserved */
#if 0
    /* Pseudo code visualize layout of signed header */
    uint8_t     hash[get_hash_length(this.hash_type)];
    uint8_t     signature[size_of_signature];
#endif
} issw_signed_header_t;

struct sipc_message {
    uint8_t   msg_type;
    uint8_t   prot_scm;
    uint16_t  length;
    uint32_t  counter;
} __attribute((__packed__));

char      bind_data_buffer[] = "Test of BindData() function.";
char      verify_bind_buffer[] = "Test of Verify Data Binding() function.";

uint32_t  FB_ModemData[] = {
    120, 4
};  /* Periodicity, Timeout */

unsigned char MB_ModemData[] = {
    0x02, 0x00, 0x00, 0x00, 0x00, 0x10, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x9b, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char ModemBandData[] = {
    0x01, 0x01, 0x01, 0x01,
    0x02, 0x03, 0x04, 0x05,
    0x03, 0x04, 0x05, 0x06,
    0x04, 0x05, 0x06, 0x07,
    0x05, 0x06, 0x07, 0x08
};

/**
 * The following table is copied from cops_sipc_message.h
 */

typedef enum gen_sipc_mtype {
    /* Generic requests from modem 0x10-0x2F */
    GEN_SIPC_READ_IMEI_MREQ = 0x10,
    GEN_SIPC_READ_IMEI_MRESP,
    GEN_SIPC_VERIFY_IMSI_MREQ,
    GEN_SIPC_VERIFY_IMSI_MRESP,
    GEN_SIPC_READ_MODEM_DATA_MREQ,
    GEN_SIPC_READ_MODEM_DATA_MRESP,
    GEN_SIPC_VERIFY_DATA_BINDING_MREQ,
    GEN_SIPC_VERIFY_DATA_BINDING_MRESP,
    GEN_SIPC_BIND_DATA_MREQ,
    GEN_SIPC_BIND_DATA_MRESP,
    GEN_SIPC_GET_SEED_MREQ,
    GEN_SIPC_GET_SEED_MRESP,
    GEN_SIPC_VERIFY_SIGNED_HEADER_MREQ,
    GEN_SIPC_VERIFY_SIGNED_HEADER_MRESP,
    GEN_SIPC_CALCULATE_HASH_MREQ,
    GEN_SIPC_CALCULATE_HASH_MRESP,

    /* Generic request from host 0x30- */
    GEN_SIPC_READ_SIM_DATA_HREQ = 0x30,
    GEN_SIPC_READ_SIM_DATA_HRESP,
    GEN_SIPC_INITIATE_IMSI_VERIFICATION_HREQ,
    GEN_SIPC_INITIATE_IMSI_VERIFICATION_HRESP,
    GEN_SIPC_VERIFY_SIM_PIM_HREQ,
    GEN_SIPC_VERIFY_SIM_PIM_HRESP,

    /* Customized requests from modem 0x50- */

    GEN_SIPC_SET_MODEM_BAND_MREQ = 0x50,
    GEN_SIPC_SET_MODEM_BAND_MRESP,
    GEN_SIPC_SIMLOCK_UNLOCK_MREQ,
    GEN_SIPC_SIMLOCK_UNLOCK_MRESP,
    GEN_SIPC_SIMLOCK_GET_LOCK_SETTINGS_MREQ,
    GEN_SIPC_SIMLOCK_GET_LOCK_SETTINGS_MRESP,
    GEN_SIPC_SIMLOCK_GET_LOCK_ATTEMPTS_MREQ,
    GEN_SIPC_SIMLOCK_GET_LOCK_ATTEMPTS_MRESP,
    GEN_SIPC_SIMLOCK_LOCK_MREQ,
    GEN_SIPC_SIMLOCK_LOCK_MRESP,
    GEN_SIPC_SIMLOCK_MCK_RESET_MREQ,
    GEN_SIPC_SIMLOCK_MCK_RESET_MRESP,
} gen_sipc_mtype_t;

static struct sipc_message *alloc_msg(uint8_t msg_type, uint32_t counter,
                                      uint8_t sender);
static void finalize_msg(struct sipc_message *m, uint8_t *d);
static void write_msg(struct sipc_message *m);

static char *next_param(int *argc, char **argv[]);
static char *restore_param(int *argc, char **argv[]);

static void gen_verify_imsi_req(int argc, uint8_t sender, char *argv[]);
static void gen_verify_imsi_resp(int argc, uint8_t sender, char *argv[]);

static void gen_get_seed_req(int argc, uint8_t sender, char *argv[]);
static void gen_get_seed_resp(int argc, uint8_t sender, char *argv[]);

static void gen_bind_data_req(int argc, uint8_t sender, char *argv[]);
static void gen_bind_data_resp(int argc, uint8_t sender, char *argv[]);

static void gen_verify_bind_req(int argc, uint8_t sender, char *argv[]);
static void gen_verify_bind_resp(int argc, uint8_t sender, char *argv[]);

static void gen_read_sim_data_req(int argc, uint8_t sender, char *argv[]);
static void gen_read_sim_data_resp(int argc, uint8_t sender, char *argv[]);

static void gen_read_modem_data_req(int argc, uint8_t sender, char *argv[]);
static void gen_read_modem_data_resp(int argc, uint8_t sender, char *argv[]);

static void gen_verify_signed_header_req(int argc, uint8_t sender,
                                         char *argv[]);
static void gen_verify_signed_header_resp(int argc, uint8_t sender,
                                          char *argv[]);

static void gen_calchash_req(int argc, uint8_t sender, char *argv[]);
static void gen_calchash_resp(int argc, uint8_t sender, char *argv[]);

static void gen_set_modem_band_req(int argc, uint8_t sender, char *argv[]);
static void gen_set_modem_band_resp(int argc, uint8_t sender, char *argv[]);

static void gen_simlock_unlock_req(int argc, uint8_t sender, char *argv[]);
static void gen_simlock_unlock_resp(int argc, uint8_t sender, char *argv[]);

static void gen_simlock_get_lock_settings_req(int argc, uint8_t sender,
                                              char *argv[]);
static void gen_simlock_get_lock_settings_resp(int argc, uint8_t sender,
                                               char *argv[]);

static void gen_simlock_get_lock_attempts_req(int argc, uint8_t sender,
                                              char *argv[]);
static void gen_simlock_get_lock_attempts_resp(int argc, uint8_t sender,
                                               char *argv[]);

static void gen_simlock_lock_req(int argc, uint8_t sender, char *argv[]);
static void gen_simlock_normlock_req(int argc, uint8_t sender, char *argv[]);
static void gen_simlock_lock_resp(int argc, uint8_t sender, char *argv[]);

static void gen_simlock_mck_reset_req(int argc, uint8_t sender, char *argv[]);
static void gen_simlock_mck_reset_resp(int argc, uint8_t sender, char *argv[]);

static const struct {
    const char *str;
    int       args;
    uint8_t sender;
    void (*func)(int argc, uint8_t sender, char *argv[]);
} funcs[] = {
    {"verify_imsi_req", 1, 0, gen_verify_imsi_req},
    {"verify_imsi_resp", 1, 0, gen_verify_imsi_resp},
    {"get_seed_req", 1,  0, gen_get_seed_req},
    {"get_seed_resp", 1, 0, gen_get_seed_resp},
    {"bind_data_req", 0, 0, gen_bind_data_req},
    {"bind_data_resp", 1, 0, gen_bind_data_resp},
    {"verify_bind_req", 0, 0, gen_verify_bind_req},
    {"verify_bind_resp", 1, 0, gen_verify_bind_resp},
    {"read_sim_data_req", 1, 0, gen_read_sim_data_req},
    {"read_sim_data_resp", 1, 0, gen_read_sim_data_resp},
    {"read_sim_data_daemon_req", 1, 2, gen_read_sim_data_req},
    {"read_sim_data_daemon_resp", 1, 2, gen_read_sim_data_resp},
    {"read_modem_data_req", 1, 0, gen_read_modem_data_req},
    {"read_modem_data_resp", 1, 0, gen_read_modem_data_resp},
    {"verify_signed_header_req", 1, 0, gen_verify_signed_header_req},
    {"verify_signed_header_resp", 1, 0, gen_verify_signed_header_resp},
    {"calchash_req", 1, 0, gen_calchash_req},
    {"calchash_resp", 1, 0, gen_calchash_resp},
    {"set_modem_band_req", 0, 0, gen_set_modem_band_req},
    {"set_modem_band_resp", 1, 0, gen_set_modem_band_resp},
    {"simlock_unlock_req", 1, 0, gen_simlock_unlock_req},
    {"simlock_unlock_resp", 1, 0, gen_simlock_unlock_resp},
    {"simlock_get_lock_settings_req", 1, 0, gen_simlock_get_lock_settings_req},
    {"simlock_get_lock_settings_resp", 1, 0,
     gen_simlock_get_lock_settings_resp},
    {"simlock_get_lock_attempts_req", 1,  0, gen_simlock_get_lock_attempts_req},
    {"simlock_get_lock_attempts_resp", 1, 0,
     gen_simlock_get_lock_attempts_resp},
    {"simlock_lock_req", 1,  0, gen_simlock_lock_req},
    {"simlock_normlock_req", 1, 2, gen_simlock_normlock_req},
    {"simlock_lock_resp", 1, 0, gen_simlock_lock_resp},
    {"simlock_normlock_resp", 1, 2, gen_simlock_lock_resp},
    {"simlock_mck_reset_req", 1, 0, gen_simlock_mck_reset_req},
    {"simlock_mck_reset_resp", 1, 0, gen_simlock_mck_reset_resp}
};

static const size_t num_funcs = sizeof(funcs) / sizeof(funcs[0]);

int main(int argc, char *argv[])
{
    char     *str;
    size_t    n;

    (void)restore_param;        /* gcc warning */
    (void)next_param(&argc, &argv);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    for (n = 0; n < num_funcs; n++) {
        if (strcmp(str, funcs[n].str) == 0) {
            funcs[n].func(argc, funcs[n].sender, argv);
            break;
        }
    }

    if (n == num_funcs) {
        fprintf(stderr, "Unknown message \"%s\"\n", str);
        goto usage;
    }

    return EXIT_SUCCESS;

usage:

    for (n = 0; n < num_funcs; n++) {
        if (funcs[n].args) {
            fprintf(stderr, "Usage: gen_sipc %s <args...>\n", funcs[n].str);
        } else {
            fprintf(stderr, "Usage: gen_sipc %s\n", funcs[n].str);
        }
    }

    return EXIT_FAILURE;
}

static void gen_verify_imsi_req(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    size_t    n;
    size_t len;
    uint32_t sess = 0;
    char     *endptr;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_VERIFY_IMSI_MREQ, sess, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);

    if (len != 15) {
        fprintf(stderr, "verify_imsi_req: imsi will be padded to 15 digits\n");
    }

    for (n = 0; n < len; n++) {
        if (!isdigit(str[n])) {
            fprintf(stderr, "verify_imsi_req: "
                    "imsi may only contain digits (0 .. 9)\n");
            goto usage;
        }

        *d = str[n] - '0';
        d++;
    }

    /*pad with 0x0f up to 15 bytes */
    for (n = len; n < 15; n++) {
        *d = 0x0f;
        d++;
    }

    *d = 0;                     /* padding */
    d++;

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "verify_imsi_req: Unexpected trailing arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: <session counter> verify_imsi_req "
            "<15 imsi digits>\n");
    exit(EXIT_FAILURE);
}

static void gen_verify_imsi_resp(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    char     *endptr;
    uint8_t  activelocks = 0;
    uint8_t  failed_lt = 0xFF;
    uint32_t sess = 0;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_VERIFY_IMSI_MRESP, sess, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "verify_imsi_resp: invalid return code \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    activelocks = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "verify_imsi_resp: invalid activelocks \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &activelocks, sizeof(activelocks));
    d += sizeof(activelocks);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    failed_lt = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "verify_imsi_resp: invalid failed_lt \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &failed_lt, sizeof(failed_lt));
    d += sizeof(failed_lt);

    /* padding */
    memset(d, 0, 2);
    d += 2;


    ret_code = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "verify_imsi_resp: invalid return code \"%s\"\n", str);
        goto usage;
    }

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "verify_imsi_resp: Unexpected trailing arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: verify_imsi_resp <session counter> <return_code> "
            "<activelocks>\n");
    exit(EXIT_FAILURE);
}

static void gen_simlock_unlock_req(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    size_t    n;
    size_t len;
    uint32_t sess = 0;
    char     *endptr;


    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_SIMLOCK_UNLOCK_MREQ, sess, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);

    if (len != 1) {
        goto usage;
    }

    *d = str[0]-'0';
    d++;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);

    for (n = 0; n < len; n++) {
        if (!isdigit(str[n])) {
            fprintf(stderr, "simlock_unlock_req: "
                    "controlkey may only contain digits (0 .. 9)\n");
            goto usage;
        }

        *d = str[n];
        d++;
    }

    /*pad with 0xff up to 16 bytes */
    for (n = len; n < 16; n++) {
        *d = 0xff;
        d++;
    }


    /* end with \0 */
    *d = '\0';
    d++;

    *d = 0;                     /* padding */
    d++;
    *d = 0;                     /* padding */
    d++;

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "simlock_unlock_req: Unexpected trailing arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: simlock_unlock_rq <session_counter>"
            "<lock_type> <control_key>\n");
    exit(EXIT_FAILURE);

}

static void gen_simlock_unlock_resp(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    char     *endptr;
    uint32_t sess = 0;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_SIMLOCK_UNLOCK_MRESP, sess, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "simlock_unlock_resp: invalid return code \"%s\"\n",
                str);
        goto usage;
    }

    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "simlock_unlock_resp: Unexpected trailing arg \"%s\"\n"
                , str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: simlock_unlock_resp <session_counter>"
            "<return_code>\n");
    exit(EXIT_FAILURE);
}

static void gen_simlock_get_lock_settings_req(int argc, uint8_t sender,
                                              char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t sess = 0;
    char     *endptr;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }
    m = alloc_msg(GEN_SIPC_SIMLOCK_GET_LOCK_SETTINGS_MREQ, sess, sender);
    d = (uint8_t *)(m + 1);


    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr,
             "simlock_get_lock_settings_req: Unexpected trailing arg \"%s\"\n",
             str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: simlock_get_lock_settings_rq"
            "<session_counter>\n");
    exit(EXIT_FAILURE);

}

static void gen_simlock_get_lock_settings_resp(int argc, uint8_t sender,
                                               char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    char     *endptr;
    size_t   len;
    uint8_t i;
    uint32_t sess = 0;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_SIMLOCK_GET_LOCK_SETTINGS_MRESP, sess, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr,
          "simlock_get_lock_settings_resp: invalid return code \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);


    for (i = 0; i < 6; i++) {
        str = next_param(&argc, &argv);

        if (str == NULL) {
            fprintf(stderr, "simlock_get_lock_settings_resp: fail\"%s\"\n",
                    str);
            goto usage;
        }
        len = strtoul(str, &endptr, 0);


        if (*endptr != '\0') {
            fprintf(stderr,
          "simlock_get_lock_settings_resp: invalid setting \"%s\"\n", str);
            goto usage;
        }

        *d = str[0]-'0';
        if (*d >= 6)
            goto usage;
        d++;
    }

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "simlock_get_lock_settings_resp: Unexpected trailing"
                "arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: simlock_get_lock_settings_resp"
            "<session counter> <return_code>"
            "<nl-settning> <nsl-settning> <spl-settning> <cp-settning>"
            "<ull-settning><esll-settning>\n");
    exit(EXIT_FAILURE);
}

static void gen_simlock_get_lock_attempts_req(int argc, uint8_t sender,
                                              char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t sess = 0;
    char     *endptr;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(0x56, sess, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr,
             "simlock_get_lock_attempts_req: Unexpected trailing arg \"%s\"\n",
             str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: simlock_get_lock_attempts_rq <session_counter>"
            "\n");
    exit(EXIT_FAILURE);

}

static void gen_simlock_get_lock_attempts_resp(int argc, uint8_t sender,
                                               char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    char     *endptr;
    size_t   len;
    uint8_t i;
    uint32_t sess = 0;


    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(0x57, sess, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr,
          "simlock_get_lock_attempts_resp: invalid return code \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);


    for (i = 0; i < 6; i++) {
        str = next_param(&argc, &argv);

        if (str == NULL) {
            fprintf(stderr, "simlock_get_lock_attempts_resp: fail\"%s\"\n",
                    str);
            goto usage;
        }
        len = strtoul(str, &endptr, 0);


        if (*endptr != '\0') {
            fprintf(stderr,
          "simlock_get_lock_attempts_resp: invalid setting \"%s\"\n", str);
            goto usage;
        }

        *d = str[0]-'0';
        d = d + 4;
    }

    str = next_param(&argc, &argv);
    if (str != NULL) {
        fprintf(stderr, "simlock_get_lock_attempts_resp: Unexpected trailing"
                "arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: simlock_get_lock_attempts_resp <session_counter>"
            "<return_code>"
            "<nl-attempts> <nsl-attempts> <spl-attempts> <cp-attempts>"
            "<ull-attempts> <esll-attempts>\n");
    exit(EXIT_FAILURE);
}

static void gen_simlock_lock_req(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    size_t    n;
    size_t len;
    char *endptr;
    uint32_t lock_def;
    uint8_t gid1;
    uint8_t gid2;
    uint8_t lock_to_sim;
    uint32_t sess = 0;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_SIMLOCK_LOCK_MREQ, sess, sender);
    d = (uint8_t *)(m + 1);

    /* Lock type*/
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);

    if (len != 1) {
        goto usage;
    }

    *d = str[0]-'0';
    d++;

    /* Control key*/
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);

    for (n = 0; n < len; n++) {
        if (!isdigit(str[n])) {
            fprintf(stderr, "simlock_lock_req: "
                    "controlkey may only contain digits (0 .. 9)\n");
            goto usage;
        }

        *d = str[n];
        d++;
    }

    /*pad with 0xff up to 16 bytes */
    for (n = len; n < 16; n++) {
        *d = 0xff;
        d++;
    }

    /* end with \0 */
    *d = '\0';
    d++;

    /* Lock definition */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    lock_def = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "simlock_lock_req: invalid lock definition \"%s\"\n",
                str);
        goto usage;
    }

    memcpy(d, &lock_def, sizeof(lock_def));
    d += sizeof(lock_def);

    /* IMSI */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);

    if (len < 15) {
        fprintf(stderr, "simlock_lock_req: IMSI will be padded to 15"
                " digits\n");
    }

    for (n = 0; n < len; n++) {
        if (!isdigit(str[n])) {
            fprintf(stderr, "simlock_lock_req: "
                    "imsi may only contain digits (0 .. 9)\n");
            goto usage;
        }

        *d = str[n] - '0';
        d++;
    }

    /*pad with 0xff up to 15 bytes */
    for (n = len; n < 15; n++) {
        *d = 0xff;
        d++;
    }

    /* GID1 */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    gid1 = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "simlock_lock_req: invalid GID1 \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &gid1, sizeof(gid1));
    d += sizeof(gid1);

    /* GID2 */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    gid2 = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "simlock_lock_req: invalid GID2 \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &gid2, sizeof(gid2));
    d += sizeof(gid2);

    /* LockToSim */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    lock_to_sim = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "simlock_lock_req: invalid LockToSim \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &lock_to_sim, sizeof(lock_to_sim));
    d += sizeof(lock_to_sim);

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "simlock_lock_req: Unexpected trailing arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: simlock_lock_rq <lock_type> <control_key> "
            "<session_counter> <lock_definition> <imsi> <gid1> <gid2>"
            "<lock_to_sim>\n");
    exit(EXIT_FAILURE);

}

static void gen_simlock_normlock_req(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    size_t    n;
    size_t len;
    char *endptr;
    uint32_t sess = 0;
    uint32_t lock_def;
    uint8_t count = 0;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }
    m = alloc_msg(GEN_SIPC_SIMLOCK_LOCK_MREQ, sess, sender);
    d = (uint8_t *)(m + 1);

    /* Lock mode */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);

    if (len != 1) {
        goto usage;
    }
    *d = str[0]-'0';
    if ((*d != 2) & (*d != 3)) {
        fprintf(stderr, "lock mode must be normal or normalcnl");
        goto usage;
    }
    d += sizeof(uint32_t);
    count = count+4;

    /* Lock type */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);

    if (len != 1) {
        goto usage;
    }
    *d = str[0]-'0';
    d += sizeof(uint32_t);
    count = count+4;

    /* Control key */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);

    for (n = 0; n < len; n++) {
        if (!isdigit(str[n])) {
            fprintf(stderr, "simlock_lock_req: "
                    "controlkey may only contain digits (0 .. 9)\n");
            goto usage;
        }

        *d = str[n];
        d++;
        count++;
    }

    /*pad with 0xff up to 16 bytes */
    for (n = len; n < 16; n++) {
        *d = 0xff;
        d++;
        count++;
    }

    /* end with \0 */
    *d = '\0';
    d++;
    count++;

    /* Lock op */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);

    if (len != 1) {
        goto usage;
    }

    *d = str[0]-'0';
    d += sizeof(uint32_t);
    count = count+4;

    /* Lock definition */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        /* update_lockdef is false */
        *d = 0;
        d++;
        count++;
        memset(d, 0, sizeof(lock_def));
    } else {
        /* update_lockdef is true */
        *d = 1;
        d++;
        count++;
        lock_def = strtoul(str, &endptr, 0);
        if (*endptr != '\0') {
            fprintf(stderr, "simlock_lock_req: invalid lock definition"
                    "\"%s\"\n", str);
            goto usage;
        }
        memcpy(d, &lock_def, sizeof(lock_def));

    }
    d += sizeof(lock_def);
    count += sizeof(lock_def);

    /* dummy for pointers */
    *d = 0;
    d += 8;
    count += 8;

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "simlock_lock_req: Unexpected trailing arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: simlock_normlock_rq<lock_mode> <lock_type>"
            "<control_key> <lockop> <lock_definition (optional)>\n");
    exit(EXIT_FAILURE);

}



static void gen_simlock_lock_resp(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    char     *endptr;
    uint32_t sess = 0;


    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_SIMLOCK_LOCK_MRESP, sess, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }
    ret_code = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "simlock_lock_resp: invalid return code \"%s\"\n",
                str);
        goto usage;
    }
    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "simlock_lock_resp: Unexpected trailing arg \"%s\"\n"
                , str);
        goto usage;
    }
    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: simlock_lock_resp <session_counter>"
            "<return_code>\n");
    exit(EXIT_FAILURE);
}

static void gen_simlock_mck_reset_req(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    size_t    n;
    size_t len;
    uint32_t sess = 0;
    char     *endptr;


    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_SIMLOCK_MCK_RESET_MREQ, sess, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);

    if (len != 1) {
        goto usage;
    }

    *d = str[0]-'0';
    d++;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strlen(str);


    for (n = 0; n < len; n++) {
        if (!isdigit(str[n])) {
            fprintf(stderr, "simlock_mck_reset_req: "
                    "controlkey may only contain digits (0 .. 9)\n");
            goto usage;
        }

        *d = str[n];
        d++;
    }

    /*pad with 0xff up to 16 bytes */
    for (n = len; n < 16; n++) {
        *d = 0xff;
        d++;
    }

    *d = 0;                     /* padding */
    d++;
    *d = 0;                     /* padding */
    d++;
    *d = 0;                     /* padding */
    d++;

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr,
                "simlock_mck_reset_req: Unexpected trailing arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr,
            "Usage: simlock_mck_reset_rq <session_counter>"
            "<lock_type> <control_key>\n");
    exit(EXIT_FAILURE);

}

static void gen_simlock_mck_reset_resp(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    char     *endptr;
    uint32_t sess = 0;


    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    } else {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_SIMLOCK_MCK_RESET_MRESP, sess, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "simlock_mck_reset_resp: invalid return code \"%s\"\n",
                str);
        goto usage;
    }

    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr,
                "simlock_mck_reset_resp: Unexpected trailing arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: simlock_mck_reset_resp <session_counter>"
            "<return_code>\n");
    exit(EXIT_FAILURE);
}

static void gen_get_seed_req(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char *str;
    char *endptr;
    uint32_t sess = 0;

    str = next_param(&argc, &argv);

    if (str != NULL) {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_GET_SEED_MREQ, sess, sender);
    d = (uint8_t *)(m + 1);

    finalize_msg(m, d);
    write_msg(m);
    return;
}

static void gen_get_seed_resp(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    char     *endptr;
    uint32_t  seed = 0xdeadbeef;
    uint32_t sess = 0;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "get_seed_resp: invalid return code \"%s\"\n", str);
        goto usage;
    }

    str = next_param(&argc, &argv);

    if (str != NULL) {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_GET_SEED_MRESP, sess, sender);
    d = (uint8_t *)(m + 1);

    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);

    memcpy(d, &seed, sizeof(seed));
    d += sizeof(seed);
    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "get_seed_resp: Unexpected trailing arg \"%s\"\n", str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: get_seed_resp <session (optional, default 0)>"
            "<return_code>\n");
    exit(EXIT_FAILURE);
}

static void gen_bind_data_req(int argc, uint8_t sender, char *argv[])
{
    (void) argc;
    (void) argv;

    struct sipc_message *m;
    uint8_t  *d;

    (void)argc;
    (void)argv;

    m = alloc_msg(GEN_SIPC_BIND_DATA_MREQ, 0, sender);
    d = (uint8_t *)(m + 1);

    memcpy(d, &bind_data_buffer, sizeof(bind_data_buffer) - 1);
    d += (sizeof(bind_data_buffer) - 1);

    finalize_msg(m, d);
    write_msg(m);
    return;
}

static void gen_bind_data_resp(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    char     *endptr;
    uint8_t   mac_data[32];
    unsigned int mlen = 32;
    uint32_t  key = 2;

    m = alloc_msg(GEN_SIPC_BIND_DATA_MRESP, 0, sender);
    d = (uint8_t *)(m + 1);

   str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "bind_data_resp: invalid return code \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);

    HMAC(EVP_sha256(), &key, sizeof(key),
         (const unsigned char *)bind_data_buffer, sizeof(bind_data_buffer) - 1,
         (unsigned char *)mac_data, &mlen);

    memcpy(d, &mac_data, sizeof(mac_data));
    d += sizeof(mac_data);

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "bind_data_resp: Unexpected trailing arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: get_seed_resp <return_code>\n");
    exit(EXIT_FAILURE);
}

static void gen_verify_bind_req(int argc, uint8_t sender, char *argv[])
{
    (void) argc;
    (void) argv;

    struct sipc_message *m;
    uint8_t  *d;
    uint8_t   mac_data[32];
    unsigned int mlen = 32;
    uint32_t  key = 2;

    (void)argc;
    (void)argv;

    m = alloc_msg(GEN_SIPC_VERIFY_DATA_BINDING_MREQ, 0, sender);
    d = (uint8_t *)(m + 1);

    HMAC(EVP_sha256(), &key, sizeof(key),
         (const unsigned char *)verify_bind_buffer,
         sizeof(verify_bind_buffer) - 1, (unsigned char *)mac_data, &mlen);

    memcpy(d, &mac_data, sizeof(mac_data));
    d += sizeof(mac_data);

    memcpy(d, verify_bind_buffer, sizeof(verify_bind_buffer) - 1);
    d += sizeof(verify_bind_buffer) - 1;

    finalize_msg(m, d);
    write_msg(m);
    return;
}

static void gen_verify_bind_resp(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    char     *endptr;

    m = alloc_msg(GEN_SIPC_VERIFY_DATA_BINDING_MRESP, 0, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "bind_data_resp: invalid return code \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "bind_data_resp: Unexpected trailing arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: get_verify_bind_resp <return_code>\n");
    exit(EXIT_FAILURE);
}

static void gen_read_sim_data_req(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    unsigned long counter = 0;
    uint8_t  cnl = 0;

    str = next_param(&argc, &argv);

    if (str != NULL) {
        char     *endptr;
        if (strcmp(str, "-c") != 0) {
            if (strcmp(str, "-d") == 0) {
                cnl = 1;
            } else {
                goto usage;
            }
        }

        str = next_param(&argc, &argv);

        if (str == NULL) {
            goto usage;
        }

        counter = strtoul(str, &endptr, 0);

        if (*endptr != '\0') {
            fprintf(stderr, "gen_read_sim_data_req: "
                    "invalid counter \"%s\"\n", str);
            goto usage;
        }
    }

    m = alloc_msg(GEN_SIPC_READ_SIM_DATA_HREQ, counter, sender);
    d = (uint8_t *)(m + 1);

    /* include_cnl */
    *d = cnl;
    d++;

    /* padding */
    memset(d, 0, 3);
    d += 3;

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "gen_read_sim_data_req: "
                "Unexpected trailing arg \"%s\"\n", str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: read_sim_data_req [-c <counter>] if cnl should not"
            " be used or [-d <counter> if cnl should be used\n");
    exit(EXIT_FAILURE);
}

static void gen_read_sim_data_resp(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    char     *endptr;
    unsigned long counter = 0;
    size_t    n;
    unsigned long val;
    unsigned long nrcnl;
    size_t   len;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    if (strcmp(str, "-c") == 0) {
        str = next_param(&argc, &argv);

        if (str == NULL) {
            fprintf(stderr, "gen_read_sim_data_resp: -c requires argument\n");
            goto usage;
        }

        counter = strtoul(str, &endptr, 0);

        if (*endptr != '\0') {
            fprintf(stderr, "gen_read_sim_data_resp: "
                    "invalid counter \"%s\"\n", str);
            goto usage;
        }

        str = next_param(&argc, &argv);

        if (str == NULL) {
            goto usage;
        }
    }


    m = alloc_msg(GEN_SIPC_READ_SIM_DATA_HRESP, counter, sender);
    d = (uint8_t *)(m + 1);

    /* error_code */
    memset(d, 0, 4);
    d += 4;


    /* IMSI */
    len = strlen(str);

    if (len != 15) {
        fprintf(stderr, "gen_read_sim_data_resp: "
                "imsi will be padded to 15 digits is only %x digits\n",
                (unsigned int)len);
    }

    for (n = 0; n < len; n++) {
        if (!isdigit(str[n])) {
            fprintf(stderr, "gen_read_sim_data_resp: "
                    "imsi may only contain digits (0 .. 9)\n");
            goto usage;
        }

        *d = str[n] - '0';
        d++;
    }

    /*pad with 0x0f up to 15 bytes */
    for (n = len; n < 15; n++) {
        *d = 0x0f;
        d++;
    }

    /* GID1 */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    val = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "gen_read_sim_data_resp: invalid GID1 \"%s\"\n", str);
        goto usage;
    }

    if (val > 0xff) {
        fprintf(stderr, "gen_read_sim_data_resp: invalid GID1 \"%s\"\n", str);
        goto usage;
    }

    *d = (uint8_t)val;
    d++;

    /* GID2 */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    val = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "gen_read_sim_data_resp: invalid GID2 \"%s\"\n", str);
        goto usage;
    }

    if (val > 0xff) {
        fprintf(stderr, "gen_read_sim_data_resp: invalid GID2 \"%s\"\n", str);
        goto usage;
    }

    *d = (uint8_t)val;
    d++;

    /* num_cnl_entries */
    str = next_param(&argc, &argv);

    if (str == NULL) {
        *d = 0;
        d++;
        goto no_cnl;
    }

    nrcnl = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr,
                "gen_read_sim_data_resp: invalid number of cnl \"%s\"\n", str);
        goto usage;
    }

    *d = (uint8_t)nrcnl;
    d++;

    /* padding1 */
    memset(d, 0, 2);
    d += 2;

    /* cnl_entries */
    {
        size_t i;

        for (i = 0; i < nrcnl; i++) {
            for (n = 0; n < 6; n++) {
                str = next_param(&argc, &argv);

                if (str == NULL) {
                    goto usage;
                }

                val = strtoul(str, &endptr, 0);

                if (*endptr != '\0') {
                    fprintf(stderr, "gen_read_sim_data_resp: "
                            "invalid CNL \"%s\"\n", str);
                    goto usage;
                }

                if (val > 0xff) {
                    fprintf(stderr, "gen_read_sim_data_resp: "
                            "invalid CNL \"%s\"\n", str);
                    goto usage;
                }

                *d = (uint8_t)val;
                d++;
            }
        }
    }



no_cnl:

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "gen_read_sim_data_resp: "
                "Unexpected trailing arg \"%s\"\n", str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr,
            "Usage: gen_read_sim_data_resp [-c <counter>] <15 imsi digits> "
             "<GID1> <GID2> <number of cnl's> <cnl1, cnl2 etc>\n");
    exit(EXIT_FAILURE);
}

static void gen_read_modem_data_req(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char *str;
    char *endptr;
    uint32_t sess = 0;

    str = next_param(&argc, &argv);

    if (str != NULL) {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_READ_MODEM_DATA_MREQ, sess, sender);
    d = (uint8_t *)(m + 1);

    finalize_msg(m, d);
    write_msg(m);
    return;
}

static void gen_read_modem_data_resp(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    char     *endptr;
    int type;
    uint32_t sess = 0;
    size_t len;
    uint8_t *basep = NULL;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "read_modem_data_resp: invalid return code \"%s\"\n",
                str);
        goto usage;
    }

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    type = strtoul(str, &endptr, 0);

    if (*endptr != '\0') {
        fprintf(stderr, "read_modem_data_resp: invalid me type\"%s\"."
                "Must be 0 == mb else type == fb\n", str);
        goto usage;
    }

    if (type == 0) {
        basep = MB_ModemData;
        len = sizeof(MB_ModemData);
    } else {
        basep = (uint8_t *)FB_ModemData;
        len = sizeof(FB_ModemData);
    }

    str = next_param(&argc, &argv);

    if (str != NULL) {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_READ_MODEM_DATA_MRESP, sess, sender);
    d = (uint8_t *)(m + 1);

    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);

    memcpy(d, basep, len);
    d += len;

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr,
                "read_modem_data_resp: Unexpected trailing arg \"%s\"\n", str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: read_modem_data_resp <return_code> "
            "<type, 0 = MB, else FB)> <session (optional, default 0)>\n");
    exit(EXIT_FAILURE);
}

static void gen_vsh_helper(char *str, uint8_t **file_p, size_t *fsize_p)
{
    FILE *fp;
    size_t size, fsize;
    uint8_t *filedata = NULL;

    /* assume str points to a filename which we should read */
    fp = fopen(str, "r");

    if (fp == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", str);
        goto function_exit;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fprintf(stderr, "fseek(fp, 0, SEEK_END) failed\n");
        goto function_exit;
    }

    size = ftell(fp);

    if (fsize_p != NULL) {
        *fsize_p = size;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "fseek(fp, 0, SEEK_SET) failed\n");
        goto function_exit;
    }

    filedata = malloc(size);

    if (filedata == NULL) {
        fprintf(stderr, "Malloc of size %zu failed\n", size);
        goto function_exit;
    }

    fsize = fread(filedata, size, sizeof(uint8_t), fp);

    if ((fsize * size) != size) {
        fprintf(stderr, "File: %s, size: %zu, fread == %zu\n", str, size,
                fsize);
        free(filedata);
        goto function_exit;
    }

    *file_p = filedata;

function_exit:

    if (fp != NULL) {
        fclose(fp);
    }

    return;
}


static void gen_verify_signed_header_req(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char *str;
    char     *endptr;
    issw_signed_header_t *hdr = NULL;
    uint32_t pl_type;
    uint8_t *filedata = NULL;

    m = alloc_msg(GEN_SIPC_VERIFY_SIGNED_HEADER_MREQ, 0, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    gen_vsh_helper(str, &filedata, NULL);

    if (filedata == NULL) {
        fprintf(stderr, "verify_signed_header_req: filedata is NULL\n");
        exit(EXIT_FAILURE);
    }

    hdr = (issw_signed_header_t *)filedata;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    pl_type = strtoul(str, &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "verify_signed_header_req: "
                "invalid payload type\"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &pl_type, sizeof(uint32_t));
    d += sizeof(uint32_t);

    memcpy(d, hdr, hdr->size_of_signed_header);
    d += hdr->size_of_signed_header;

    finalize_msg(m, d);
    write_msg(m);

    free(filedata);

    return;

usage:
    fprintf(stderr, "Usage: verify_signed_header_req "
            "<filename (signed file)> <payload_type>\n");
    return;
}

static void gen_verify_signed_header_resp(int argc, uint8_t sender,
                                          char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    issw_signed_header_t *hdr = NULL;
    uint32_t  ret_code;
    char     *endptr;
    uint8_t  *filedata = NULL;
    uint8_t  *p = NULL;
    uint8_t   ehash[32];

    m = alloc_msg(GEN_SIPC_VERIFY_SIGNED_HEADER_MRESP, 0, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "verify_signed_header_resp: "
                "invalid return code \"%s\"\n", str);
        goto usage;
    }

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    gen_vsh_helper(str, &filedata, NULL);

    if (filedata == NULL) {
        return;
    }

    hdr = (issw_signed_header_t *) filedata;

    memcpy(d, &ret_code, sizeof(uint32_t));
    d += sizeof(uint32_t);

    /* p points at the payload. */
    p = (uint8_t *) hdr + hdr->size_of_signed_header;

    SHA256((unsigned char *) p, hdr->size_of_payload, ehash);

    if (ret_code == 0) { /* COPS_RC_OK */
        memcpy(d, &ehash, sizeof(ehash));
        d += sizeof(ehash);
    }

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "verify_signed_header_resp: "
                "Unexpected trailing arg \"%s\"\n", str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);

    free(filedata);

    return;

usage:
    fprintf(stderr, "Usage: verify_signed_header_resp <return_code> "
            "<filename (signed file)>\n");
    exit(EXIT_FAILURE);
}

static void gen_calchash_req(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t             *d;
    char                *str;
    char                *endptr = NULL;
    uint32_t             ht; /* hashtype */
    uint32_t             address = 0;
    uint32_t             len = 0;

    m = alloc_msg(GEN_SIPC_CALCULATE_HASH_MREQ, 0, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ht = strtoul(str, &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "calchash_req: invalid hash type \"%s\"\n", str);
        goto usage;
    }

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    address = strtoul(str, &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "calchash_req: invalid address \"%s\"\n", str);
        goto usage;
    }


    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    len = strtoul(str, &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "calchash_req: invalid length \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &ht, sizeof(ht));
    d += sizeof(ht);

    memcpy(d, &address, sizeof(address));
    d += sizeof(address);

    memcpy(d, &len, sizeof(len));
    d += sizeof(len);

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "calchash_req: Unexpected trailing arg \"%s\"\n", str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);

    return;

usage:
    fprintf(stderr, "Usage: calchash <hash type> <address> <len>\n");
    exit(EXIT_FAILURE);
}

static void gen_calchash_resp(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    uint32_t  ht = 0;
    char     *endptr;
    uint8_t   sha1sum[] = {0x2f, 0xd4, 0xe1, 0xc6,
                           0x7a, 0x2d, 0x28, 0xfc,
                           0xed, 0x84, 0x9e, 0xe1,
                           0xbb, 0x76, 0xe7, 0x39,
                           0x1b, 0x93, 0xeb, 0x12
                          };

    uint8_t   sha256sum[] = {0xd7, 0xa8, 0xfb, 0xb3,
                             0x07, 0xd7, 0x80, 0x94,
                             0x69, 0xca, 0x9a, 0xbc,
                             0xb0, 0x08, 0x2e, 0x4f,
                             0x8d, 0x56, 0x51, 0xe4,
                             0x6d, 0x3c, 0xdb, 0x76,
                             0x2d, 0x02, 0xd0, 0xbf,
                             0x37, 0xc9, 0xe5, 0x92
                            };



    m = alloc_msg(GEN_SIPC_CALCULATE_HASH_MRESP, 0, sender);
    d = (uint8_t *)(m + 1);

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "get_calchash_resp: invalid return code \"%s\"\n", str);
        goto usage;
    }

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ht = strtoul(str, &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "get_calchash_resp: invalid hash type \"%s\"\n", str);
        goto usage;
    }

    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);

    memcpy(d, &ht, sizeof(ht));
    d += sizeof(ht);

    /* sha1 */
    if (ht == 0) {
        memcpy(d, &sha1sum, sizeof(sha1sum));
        d += sizeof(sha1sum);
    } else {
        memcpy(d, &sha256sum, sizeof(sha256sum));
        d += sizeof(sha256sum);
    }

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "get_calchash_resp: Unexpected trailing arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: get_calchash_resp <return_code> <hash type>\n");
    exit(EXIT_FAILURE);
}

static void gen_set_modem_band_req(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    uint32_t sess = 0;
    char *str;
    char *endptr;

    str = next_param(&argc, &argv);

    if (str != NULL) {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_SET_MODEM_BAND_MREQ, sess, sender);
    d = (uint8_t *)(m + 1);

    memcpy(d, &ModemBandData, sizeof(ModemBandData));
    d += (sizeof(ModemBandData));

    finalize_msg(m, d);
    write_msg(m);
    return;
}

static void gen_set_modem_band_resp(int argc, uint8_t sender, char *argv[])
{
    struct sipc_message *m;
    uint8_t  *d;
    char     *str;
    uint32_t  ret_code;
    char     *endptr;
    uint32_t sess = 0;

    str = next_param(&argc, &argv);

    if (str == NULL) {
        goto usage;
    }

    ret_code = strtoul(str, &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "set_modem_band_resp: invalid return code \"%s\"\n",
                str);
        goto usage;
    }

    str = next_param(&argc, &argv);

    if (str != NULL) {
        sess = strtoul(str, &endptr, 10);

        if (*endptr != '\0') {
            sess = 0;
        }
    }

    m = alloc_msg(GEN_SIPC_SET_MODEM_BAND_MRESP, sess, sender);
    d = (uint8_t *)(m + 1);

    memcpy(d, &ret_code, sizeof(ret_code));
    d += sizeof(ret_code);

    str = next_param(&argc, &argv);

    if (str != NULL) {
        fprintf(stderr, "set_modem_band_resp: Unexpected trailing arg \"%s\"\n",
                str);
        goto usage;
    }

    finalize_msg(m, d);
    write_msg(m);
    return;

usage:
    fprintf(stderr, "Usage: set_modem_band_resp <return_code>\n");
    exit(EXIT_FAILURE);
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

static struct sipc_message *alloc_msg(uint8_t msg_type, uint32_t counter,
                                      uint8_t sender) {
    struct sipc_message *m;

    (void)sender;
    m = calloc(1, 1024);

    if (m == NULL) {
        fprintf(stderr, "calloc");
        exit(1);
    }

    m->msg_type = msg_type;
    m->prot_scm = 1;
    m->length = 1024;
    m->counter = counter;
    return m;
}

static void finalize_msg(struct sipc_message *m, uint8_t *d)
{
    size_t    l;
    unsigned int mlen = 32;
    char key[128] = {0};
    size_t keylen;

    strncpy(key,
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-$",
            64);
    keylen = 64;

    l = d - (uint8_t *)m;
    l += 32;

    if (l > m->length) {
        fprintf(stderr, "finalize_msg: d beyond end of message");
        exit(1);
    }

    m->length = l;

    HMAC(EVP_sha256(), key, keylen,
         (const unsigned char *)m, l - 32, (unsigned char *)d, &mlen);
}

static void write_msg(struct sipc_message *m)
{
    if (isatty(STDOUT_FILENO)) {
        fprintf(stderr, "write_msg: Will not dump binary data on a tty");
        exit(1);
    }

    if (fwrite(m, 1, m->length, stdout) != m->length) {
        fprintf(stderr, "write_msg: fwrite(m, 1, %u, stdout)", m->length);
        exit(1);
    }
}
