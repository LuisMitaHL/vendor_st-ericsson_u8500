/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <cops_test.h>
#include <cops_test_framework.h>

typedef struct {
    char *imsi;
    char *req_vimsi_file;
    char *exp_resp_vimsi_file;
    char *exp_nbr_active_locks;
    char *exp_blocking_lock;
    char *exp_errorcode;
    bool gid_included;
    char *req_rsimd_file;
    char *resp_rsimd_file;
    char *gid_1;
    char *gid_2;
    bool include_cnl;
    char *nbr_cnl_entries;
    char *cnl_entries;
    bool compare_buffers;
} test_verify_imsi_params;

typedef struct {
    int eventtype;
    char *aux;
    cops_simlock_ota_unlock_status_t ota_unlock_status;
    cops_sim_status_t sim_status;
} event_expected_data;

event_expected_data event_cb_expected_data;

/**
 * Brief: Struct used to read back specific OTP content.
 */
struct {
    uint32_t meta;
    uint32_t data;
} cops_otp;

/**
 * Brief: Enum to determine OTP variant.
 */
enum {
    COPS_REAL_OTP = 0,                  /* Real OTP on HW                 */
    COPS_RAM_OTP,                       /* Emulated OTP in RAM, HW or PC. */
    COPS_FILE_OTP                       /* OTP in file on Ubuntu PC.      */
} cops_otp_variant;

static void setup(void);

static void cleanup(void);

static void test_authenticate(void);

static void test_1_write_default_data(void);

static void test_2_verify_imsi(void);

static void test_3_get_seed(void);
#if 0
static void test_4_vsh(void);   /* verify signed header */
#endif
static void test_5_vsc(void);   /* verify signed challenge */

static void test_6_getchallenge(void);

static void test_7_update_cops_data(void);

static void test_8_simlock(void);

static void test_9_autolock(void);

static void test_10_unlock_ota(void);

static void test_11_verify_keys(void);  /* verify simlock control keys */

static void test_12_check_gid_order(void);

static void test_13_sl_wildcard(void);

static void test_15_sipc_counters(void);

static void test_16_get_device_state(void);

static void test_17_unlimited_simlock(void);

static void test_18_rolock(void);

static void test_19_init_arb_table(void);

static void test_20_write_sec_info(void);

static void test_21_sim_change_key(void);

static void test_22_simlock_lock(void);

static void test_23_otp(void);

static void test_24_verify_sim_personalization_key(void);

static void test_25_write_rpmb_key(void);

static void test_26_get_product_debug_settings(void);

static void test_201_mck_generic(void);

static void wait_and_dispatch_status_events(cops_context_id_t *ctxp,
                                         uint8_t eventmask,
                                         char *aux,
                                         bool network,
                                         bool network_subset,
                                         bool service_provider,
                                         bool corporate,
                                         cops_simlock_card_status_t card_status,
                                         cops_simlock_lock_type_t failed_lt);

static void wait_and_dispatch_events(cops_context_id_t *ctxp);

static void read_file(const char *fname, uint8_t **buf, size_t *blen);

static void dump_buf(const char *bname, uint8_t *buf, size_t blen);

static void write_default_data(cops_bind_properties_arg_t arg,
                               char *csd_file, bool merge);

static void auth_wdefaultd_deauth(cops_bind_properties_arg_t arg,
                                  char *csd_file, bool merge);

static void verify_imsi(test_verify_imsi_params params, bool restart,
                        bool restart_again);

static void ota_unlock_status_cb(void *event_aux,
                                         const
                                         cops_simlock_ota_unlock_status_t
                                        *unlock_status);

static void simlock_status_cb(void *event_aux,
                              const cops_sim_status_t *status);

static void authentication_changed_cb(void *event_aux);

static void imei_cb(void *async_aux, cops_return_code_t rc,
                         const cops_imei_t *imei);

static cops_event_callbacks_t eventcallbacks = {
    .ota_unlock_status_updated_cb = ota_unlock_status_cb,
    .simlock_status_changed_cb = simlock_status_cb
};

static cops_event_callbacks_t eventcallbacks2 = {
    .ota_unlock_status_updated_cb = ota_unlock_status_cb,
    .simlock_status_changed_cb = simlock_status_cb,
    .authentication_changed_cb = authentication_changed_cb
};

static cops_async_callbacks_t async_cb = {.read_imei_cb = imei_cb};

static char *evaux = "Client";

static pid_t cops_pid = -1;

static pid_t old_cops_pid = -1;

static char *copsd_path;

static char *tapp_test_path;

static char *template_dir;

static char *gen_sipc_path;

static char *sdata_path;

static char *indication_files_dir;

char *TCID = "cops_test";

int TST_TOTAL = 1;

static FILE *log_file = NULL;

static cops_context_id_t *ctx = NULL;

static cops_context_id_t *temp_ctx = NULL;

static const cops_imei_t IMEI = {
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 7}
};

static cops_simlock_control_keys_t keys = {
    {"11111111"},
    {"22222222"},
    {"33333333"},
    {"44444444"},
    {"55555555"}
};

/* sipc message session counter */
static uint32_t modem_counter = 0;
static uint32_t ape_counter = 0;

#define COPS_CHK_RC(x, rc) \
    do {                                                                    \
        cops_return_code_t cops_rc = (x);                                   \
        if (log_file != NULL)                                               \
            fprintf(log_file, "Call: %s\n", #x);                            \
        if (cops_rc != (rc)) {                                              \
            tst_brkm(TFAIL, cleanup,                                        \
                    "%s:%d \"%s\" unexpected return code %d (expected %d)", \
                    __FILE__, __LINE__, #x, cops_rc, (rc));                 \
        }                                                                   \
    } while (0)

#define RUN_CMD(...) \
    do {                                                            \
        char cmd[2048];                                             \
        snprintf(cmd, sizeof(cmd), __VA_ARGS__);                    \
        if (log_file != NULL)                                       \
            fprintf(log_file, "CMD: %s\n", cmd);                    \
        if (system(cmd) != 0) {                                     \
            tst_brkm(TFAIL, cleanup, "%s:%d command \"%s\" failed", \
                     __FILE__, __LINE__, cmd);                      \
        }                                                           \
    } while (0)

#define CMP_BUF(b1, b1_len, b2, b2_len) \
        do { \
            if (b1_len != b2_len) { \
                dump_buf(#b1, b1, b1_len); \
                dump_buf(#b2, b2, b2_len); \
                tst_brkm(TFAIL, cleanup, \
                        "%s:%zu buffer lengths %s (%zu) "\
                        "and %s (%zu) not equal",\
                         __FILE__, (size_t)__LINE__, #b1_len, (size_t)b1_len,\
                         #b2_len, (size_t) b2_len); \
            } \
            if (memcmp(b1, b2, b2_len) != 0) {\
                dump_buf(#b1, b1, b1_len); \
                dump_buf(#b2, b2, b2_len); \
                tst_brkm(TFAIL, cleanup, "%s:%zu buffers %s and %s not equal", \
                         __FILE__, (size_t)__LINE__, #b1, #b2);         \
            } \
        } while (0)

/* Bit masks for the different events */
#define OTA_EVENT 1
#define SIMLOCK_EVENT 2
#define AUTHENTICATION_EVENT 4

int main(int argc, char *argv[])
{

#ifdef RUNNING_ON_HW
    tst_resm(TPASS, "Test on HW passed");
#else
    setup();

    /* First test authenticate to make sure it works.
     * All other tests uses authenticate functionality
     **/
    test_authenticate();
    test_1_write_default_data();
    test_2_verify_imsi();
    test_3_get_seed();
    /* OTA test moved as restarting the test suite confuses it */
    test_10_unlock_ota();
#if 0
    test_4_vsh();
#endif
    test_5_vsc();
    test_6_getchallenge();
    test_7_update_cops_data();
    test_8_simlock();
    test_9_autolock();
    test_11_verify_keys();
    test_12_check_gid_order();
    test_13_sl_wildcard();
    test_17_unlimited_simlock();
    test_18_rolock();
    test_19_init_arb_table();
    test_20_write_sec_info();
    test_23_otp();
    test_24_verify_sim_personalization_key();
    /* TODO: Add test for wakelock, i.e. call cops_wakelock_active()
     * during some other call. */
    test_25_write_rpmb_key();
    test_26_get_product_debug_settings();
    test_201_mck_generic();

    /*
     * this test must be run last as it modifies sipc counters and will
     * break other tests
    */
    test_15_sipc_counters();
    test_16_get_device_state();
    test_21_sim_change_key();
    test_22_simlock_lock();
    tst_resm(TPASS, "Test passed");
    cleanup();
#endif
    return 0;
}

static void test_is_authenticated()
{
    cops_bind_properties_arg_t arg;
    char default_cd[PATH_MAX];
    uint8_t *p = NULL;

    memset(&arg, 0, sizeof(arg));
    strcpy(default_cd, template_dir);
    strcat(default_cd, "/default1.csd");
    read_file(default_cd, &p, &arg.cops_data_length);
    arg.cops_data = p;
    COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
    free(p);
}

static void test_is_not_authenticated()
{
    cops_bind_properties_arg_t arg;
    uint8_t *p = NULL;
    char default_cd[PATH_MAX];

    memset(&arg, 0, sizeof(arg));
    strcpy(default_cd, template_dir);
    strcat(default_cd, "/default1.csd");
    read_file(default_cd, &p, &arg.cops_data_length);
    arg.cops_data = p;

    COPS_CHK_RC(cops_bind_properties(ctx, &arg),
                COPS_RC_NOT_AUTHENTICATED_ERROR);
    free(p);
}

static void test_authenticate_permanent(void)
{
#define OTP_PERM_AUTH_OFFSET 0x64
    cops_bind_properties_arg_t arg;
    char *otp_file = "otp.dat";
    size_t size;
    uint8_t *data = NULL;
    uint8_t *data_p = NULL;
    uint16_t auth_bits_in_otp;
    uint8_t i;

    /* authentication data for permanent authentication */
    cops_auth_data_t auth_data_permanent = {
        COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
    };

    memset(&arg, 0, sizeof(arg));

    /* Make the device appear as production unit. */
    RUN_CMD("touch %s/prodchip_flag", indication_files_dir);

    /* OTP prerequisite */
    RUN_CMD("cp %s/../../copsd/otp.dat .", template_dir);

    /* Check that OTP[25] is empty, meaning we are authenticated. */
    read_file(otp_file, &data, &size);
    data_p = data + OTP_PERM_AUTH_OFFSET;
    if (OTP_PERM_AUTH_OFFSET + sizeof(auth_bits_in_otp) > size)
        tst_brkm(TFAIL, cleanup, "Internal error");
    memcpy(&auth_bits_in_otp, data_p, sizeof(auth_bits_in_otp));
    if (auth_bits_in_otp != 0) {
        tst_brkm(TFAIL, cleanup, "Auth bits in OTP is already in use");
    }
    COPS_CHK_RC(cops_authenticate(ctx, true, &auth_data_permanent), COPS_RC_OK);
    test_is_authenticated();
    free(data);

    /* Make sure that we are permanently deauthenticated */
    COPS_CHK_RC(cops_deauthenticate(ctx, true), COPS_RC_OK);

    read_file(otp_file, &data, &size);
    data_p = data + OTP_PERM_AUTH_OFFSET;
    if (OTP_PERM_AUTH_OFFSET + sizeof(auth_bits_in_otp) > size)
        tst_brkm(TFAIL, cleanup, "Internal error");
    memcpy(&auth_bits_in_otp, data_p, sizeof(auth_bits_in_otp));
    if (auth_bits_in_otp != 1) {
        tst_brkm(TFAIL, cleanup, "Incorrect auth bit match in OTP.");
    }
    test_is_not_authenticated();
    free(data);

    /* Authenticate using permanent auth type should now return error. */
    COPS_CHK_RC(cops_authenticate(ctx, true, &auth_data_permanent),
                COPS_RC_NOT_AUTHENTICATED_ERROR);
    test_is_not_authenticated();

    /* Now we know that the OTP authenticate mechanism is working. Next step
     * is to test that authentication state will be stored in cops_data
     * when OTP[25] cannot be used.
     *
     * Put device into development state again. */
    RUN_CMD("rm %s/prodchip_flag", indication_files_dir);

    for (i = 0; i <= 16; i++) {
        COPS_CHK_RC(cops_authenticate(ctx, true, &auth_data_permanent),
                    COPS_RC_OK);
        test_is_authenticated();

        COPS_CHK_RC(cops_deauthenticate(ctx, true), COPS_RC_OK);
        test_is_not_authenticated();
    }

    /* Turn back to a product device again. */
    RUN_CMD("touch %s/prodchip_flag", indication_files_dir);
    test_is_not_authenticated();

    /* Check that we are no longer allowed to authenticate. */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_permanent),
                COPS_RC_NOT_AUTHENTICATED_ERROR);
    COPS_CHK_RC(cops_authenticate(ctx, true, &auth_data_permanent),
                COPS_RC_NOT_AUTHENTICATED_ERROR);

    /* Check that the ISSW permanent auth RT flag works. */
    RUN_CMD("touch %s/permauth_flag", indication_files_dir);
    COPS_CHK_RC(cops_authenticate(ctx, true, &auth_data_permanent),
                COPS_RC_OK);
    test_is_authenticated();

    /* Remove RT flag, deauthenticate and check that we cannot
     * authenticate again. */
    RUN_CMD("rm %s/permauth_flag", indication_files_dir);

    /* Check that we still are authenticated. */
    test_is_authenticated();

    COPS_CHK_RC(cops_deauthenticate(ctx, true), COPS_RC_OK);
    test_is_not_authenticated();

    /* It shouldn't be possible to authenticate, RT flag is deleted. */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_permanent),
                COPS_RC_NOT_AUTHENTICATED_ERROR);
    COPS_CHK_RC(cops_authenticate(ctx, true, &auth_data_permanent),
                COPS_RC_NOT_AUTHENTICATED_ERROR);

    tst_resm(TINFO, "Test permanent authentication OK");
}

static void test_authenticate_simlock(void)
{
    cops_bind_properties_arg_t arg;

    cops_auth_data_t auth_data_key;

    cops_auth_data_t auth_data_perm;
    cops_simlock_control_keys_t bad_keys = {
        {"00000000"}, {"00000000"}, {"00000000"}, {"00000000"},
        {"00000000"}
    };
    uint8_t *p = NULL;
    char default_cd[PATH_MAX];

    /* Set up authentication data for permanent authentication */
    auth_data_perm.auth_type = COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION;
    auth_data_perm.length = 0;
    auth_data_perm.data = NULL;

    /* Set up authentication data for authentication using simlock keys */
    auth_data_key.auth_type = COPS_AUTH_TYPE_SIMLOCK_KEYS;
    auth_data_key.length = sizeof(keys);
    auth_data_key.data = (uint8_t *) &keys;

    /* Switch to R&D device which is a SIMLock test requirement. */
    RUN_CMD("rm %s/prodchip_flag", indication_files_dir);

    /* Make sure that we are deauthenticated */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Authenticate using permanent authentication type */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_perm),
                COPS_RC_OK);

    /* Prepare bind properties argument with default data
     * authenticate data with keys
     * */
    memset(&arg, 0, sizeof(arg));
    strcpy(default_cd, template_dir);
    strcat(default_cd, "/default1.csd");
    read_file(default_cd, &p, &arg.cops_data_length);
    arg.cops_data = p;
    arg.num_new_auth_data = 1;
    arg.auth_data = &auth_data_key;

    /* Store new known keys by using bind properties */
    COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);

    /* Make sure that we are deauthenticated */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Test that we aren't authenticated */
    test_is_not_authenticated();

    /* Authenticate using simlock keys */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_key), COPS_RC_OK);
    /* Test that we are authenticated */
    test_is_authenticated();

    /* While we are authenticated, store the simlock keys again since
     * they are disturbed when testing if authenticated
     * */
    auth_data_key.auth_type = COPS_AUTH_TYPE_SIMLOCK_KEYS;
    auth_data_key.length = sizeof(keys);
    auth_data_key.data = (uint8_t *)&keys;

    /* Prepare bind properties argument with authenticate data with keys */
    memset(&arg, 0, sizeof(arg));
    arg.num_new_auth_data = 1;
    arg.auth_data = &auth_data_key;

    /* Store the keys by using bind properties */
    COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
    free(p);

    /* Make sure that we are deauthenticated */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Test that we aren't authenticated */
    test_is_not_authenticated();

    /* Authenticate using bad/wrong simlock keys */
    auth_data_key.data = (uint8_t *) &bad_keys;
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    /* Test that we aren't authenticated */
    test_is_not_authenticated();

    tst_resm(TINFO, "SIMLock Authentication OK");

}

static void test_authenticate_rsa(void)
{
    size_t len = 0;

    size_t expected_len = 48;

    uint8_t challenge[48];

    uint8_t *filedata = NULL;

    size_t fsize;

    char str[512];

    cops_auth_data_t auth_data;
    char sfile[] = { "/signedfile_sec.bin" };


    cops_bind_properties_arg_t arg;

    char locked_cd[PATH_MAX]; /* default data with locks set */

    memset(&arg, 0, sizeof(arg));

    COPS_CHK_RC(cops_get_challenge
                (ctx, COPS_AUTH_TYPE_RSA_CHALLENGE, NULL, &len),
                COPS_RC_OK);

    if (len != expected_len) {
        tst_brkm(TFAIL, cleanup, "%s:%d buffers %s and %s not equal",
                 __FILE__, __LINE__, (char *) len, (char *) expected_len);
    }

    COPS_CHK_RC(cops_get_challenge
                (ctx, COPS_AUTH_TYPE_RSA_CHALLENGE, challenge, &len),
                COPS_RC_OK);

    snprintf(str, sizeof(str), "%s%s", sdata_path, sfile);
    read_file(str, &filedata, &fsize);

    auth_data.auth_type = COPS_AUTH_TYPE_RSA_CHALLENGE;
    auth_data.length = fsize;
    auth_data.data = filedata;

    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);

    /* Write the default data with locks set */
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_SIMLOCK_KEYS, sizeof(keys),
            (uint8_t *) &keys
        };
        uint8_t *p;

        memset(&arg, 0, sizeof(arg));
        arg.imei = &IMEI;
        arg.num_new_auth_data = 1;
        arg.auth_data = &auth_data;
        strcpy(locked_cd, template_dir);
        strcat(locked_cd, "/NL_NSL_SPL_CL_Locked.csd");
        read_file(locked_cd, &p, &arg.cops_data_length);
        arg.cops_data = p;
        COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
        free(p);
    }

    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    free(filedata);

    tst_resm(TINFO, "RSA Authentication OK");
    return;
}

static void test_authenticate_engineering_mode(void)
{
    uint8_t *resultbuffer;
    size_t resultlen;
    uint8_t *actualbuffer;
    size_t actuallen;
    /* authentication data for engineering mode authentication */
    cops_auth_data_t auth_data_engineeringmode = {
        COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
    };

    /* Add indication file to make the device appear as production unit */
    RUN_CMD("echo 1 >%s/prodchip_flag", indication_files_dir);

    /* OTP prerequisite */
    RUN_CMD("cp %s/../../copsd/otp.dat .", template_dir);

    /* Easy way out to not mess to much with read_file below */
    RUN_CMD("cp %s/bootpartition_result.bin .", template_dir);

    /* Set the ro lock to unlocked */
    RUN_CMD("echo -n rw >bootpartition_sysfs_rolock.bin");

    /* Setup bootpartition with defined content */
    RUN_CMD("dd if=%s/bootpartition_engmode.bin of=bootpartition.bin "
            "2>/dev/null", template_dir);

    /* Make sure that we are deauthenticated */
    test_is_authenticated();

    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Authenticate using engineering mode */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_engineeringmode),
                COPS_RC_OK);

    /* Test that we are authenticated */
    test_is_authenticated();

    /* Make sure that we are deauthenticated */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Test that we aren't authenticated */
    test_is_not_authenticated();

    /* Should be possible to authenticate again using engineeringmode */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_engineeringmode),
                COPS_RC_OK);

    /* Test that we are authenticated */
    test_is_authenticated();

    /* trying to remove the lock when temporarily locked should fail */
    RUN_CMD("echo -n pwr_ro >bootpartition_sysfs_rolock.bin");
    COPS_CHK_RC(cops_deauthenticate(ctx, true), COPS_RC_STORAGE_ERROR);

    /* This also clears the flag in memory
     * (and state file for emulated mode) */
    RUN_CMD("echo -n rw >bootpartition_sysfs_rolock.bin");
    COPS_CHK_RC(cops_deauthenticate(ctx, true), COPS_RC_OK);

    /* Authenticate using engineering mode */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_engineeringmode),
                COPS_RC_NOT_AUTHENTICATED_ERROR);

    /* Test that we aren't authenticated */
    test_is_not_authenticated();

    /* Permanent deauthentication a second time is ok */
    COPS_CHK_RC(cops_deauthenticate(ctx, true), COPS_RC_OK);

    /* now check that written content is ok */
    read_file("bootpartition.bin", &actualbuffer, &actuallen);
    read_file("bootpartition_result.bin", &resultbuffer, &resultlen);

    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    tst_resm(TINFO, " Bootpartition content: OK");
    free(actualbuffer);
    free(resultbuffer);

    read_file("bootpartition_sysfs_rwflag.bin", &actualbuffer, &actuallen);
    RUN_CMD("echo -n 0 >bootpartition_sysfs_rwflag_result.bin");
    read_file("bootpartition_sysfs_rwflag_result.bin", &resultbuffer,
              &resultlen);

    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    tst_resm(TINFO, " Sysfs rwflag content: OK");
    free(actualbuffer);
    free(resultbuffer);

    /* At this stage the rolock should be untouched */
    read_file("bootpartition_sysfs_rolock.bin", &actualbuffer, &actuallen);
    RUN_CMD("echo -n rw >bootpartition_sysfs_rolock_result.bin");
    read_file("bootpartition_sysfs_rolock_result.bin", &resultbuffer,
              &resultlen);

    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    tst_resm(TINFO, " Sysfs rolock content: OK");
    free(actualbuffer);
    free(resultbuffer);

    /* restore to R&D unit */
    RUN_CMD("rm %s/prodchip_flag", indication_files_dir);

    tst_resm(TINFO, "Flash permanent mode Authentication OK");
}

static void test_authentication_changed_callbacks(void)
{
    cops_auth_data_t auth_data_perm;

    /* Set up authentication data for permanent authentication */
    auth_data_perm.auth_type = COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION;
    auth_data_perm.length = 0;
    auth_data_perm.data = NULL;

    /* create temporary context that only listens for events */
    if (cops_context_create_async(&temp_ctx, &eventcallbacks2,
                                  evaux, NULL, NULL) != COPS_RC_OK) {
        tst_brkm(TFAIL, cleanup, "memory allocation failed");
    }

    /* Deauthenticate and wait for callback */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    wait_and_dispatch_status_events(temp_ctx, AUTHENTICATION_EVENT, evaux,
                                    false, false, false, false, 0, 0);

    /* Authenticate and wait for callback */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_perm),
                COPS_RC_OK);

    wait_and_dispatch_status_events(temp_ctx, AUTHENTICATION_EVENT, evaux,
                                    false, false, false, false, 0, 0);

    tst_resm(TINFO, "Authentication changed callbacks OK");
}

static void test_authenticate(void)
{
    /* keeping the engineering mode authentication before the "regular"
     * permanent authentication test helps to detect problems in the
     * ISSWAPI_IS_PRODCHIP emulation */
    test_authenticate_engineering_mode();
    test_authenticate_permanent();
    test_authenticate_simlock();
    test_authenticate_rsa();
    test_authentication_changed_callbacks();
    tst_resm(TINFO, "Authenticate OK");
}

static void test_1_write_default_data(void)
{
    cops_bind_properties_arg_t arg;

    if (cops_context_create_async(&temp_ctx, NULL, NULL, &async_cb, 0) !=
       COPS_RC_OK) {
        tst_brkm(TFAIL, cleanup, "Couldn't create context");
    }

    memset(&arg, 0, sizeof(arg));

    /* Make sure that we are deauthenticated */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Test that we aren't authenticated */
    COPS_CHK_RC(cops_bind_properties(ctx, &arg),
                COPS_RC_NOT_AUTHENTICATED_ERROR);

    /* Authenticate using permanent authentication */
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
        };
        COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);
    }

    /* Write default data */
    {

        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_SIMLOCK_KEYS, sizeof(keys),
            (uint8_t *) &keys
        };
        uint8_t *p = NULL;
        char default_cd[PATH_MAX];

        memset(&arg, 0, sizeof(arg));
        arg.imei = &IMEI;
        arg.num_new_auth_data = 1;
        arg.auth_data = &auth_data;
        strcpy(default_cd, template_dir);
        strcat(default_cd, "/default1.csd");
        read_file(default_cd, &p, &arg.cops_data_length);
        arg.cops_data = p;
        COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
        free(p);
    }

    /* Try to write IMEI with invalid luhn digit */
    {
        cops_imei_t bad_imei = {
            {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5}
        };

        memset(&arg, 0, sizeof(arg));
        arg.imei = &bad_imei;
        COPS_CHK_RC(cops_bind_properties(ctx, &arg),
                    COPS_RC_ARGUMENT_ERROR);
    }

    /* Deauthenticate */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Test that we aren't authenticated */
    memset(&arg, 0, sizeof(arg));
    COPS_CHK_RC(cops_bind_properties(ctx, &arg),
                COPS_RC_NOT_AUTHENTICATED_ERROR);

    /* Read IMEI */
    {
        cops_imei_t imei;

        COPS_CHK_RC(cops_read_imei(ctx, &imei), COPS_RC_OK);

        /* We expect only one IMEI */
        if (memcmp(&imei, &IMEI, sizeof(imei)) != 0) {
            CMP_BUF((uint8_t *) &imei, sizeof(imei), (uint8_t *) &IMEI,
                    sizeof(imei));
            tst_brkm(TFAIL, cleanup, "Got invalid IMEI");
        }
    }

    /* Read IMEI Async */
    COPS_CHK_RC(cops_read_imei(temp_ctx, NULL), COPS_RC_ASYNC_CALL);
    wait_and_dispatch_events(temp_ctx);

    cops_context_destroy(&temp_ctx);

    tst_resm(TINFO, "Write default data OK");
}

static void test_2_verify_imsi(void)
{
    uint8_t *in_sipc;

    size_t in_sipc_len;

    uint8_t *out_sipc;

    size_t out_sipc_len;

    uint8_t *exp_out_sipc;

    size_t exp_out_sipc_len;

    /* Verify IMSI without any locks in place */
    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 111112345678901 > vimsi_req.1 2> /dev/null",
            gen_sipc_path, modem_counter);

    RUN_CMD("%s verify_imsi_resp %u 0 0 0xff > exp_vimsi_resp.1",
            gen_sipc_path,
            modem_counter);


    read_file("vimsi_req.1", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_vimsi_resp.1", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    /* Verify IMSI with shorter IMSI */
    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 2460813579 > vimsi_req.2 2> /dev/null",
            gen_sipc_path, modem_counter);

    RUN_CMD("%s verify_imsi_resp %u 0 0 0xff > exp_vimsi_resp.2",
            gen_sipc_path,
            modem_counter);

    read_file("vimsi_req.2", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_vimsi_resp.2", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
        };
        COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);
    }

    {
        uint8_t *p;
        cops_bind_properties_arg_t arg;
        char simple_edn_cd[PATH_MAX]; /* simple EDN rule without GIDs */

        memset(&arg, 0, sizeof(arg));
        strcpy(simple_edn_cd, template_dir);
        strcat(simple_edn_cd, "/simple_edn.csd");
        read_file(simple_edn_cd, &p, &arg.cops_data_length);
        arg.cops_data = p;
        arg.merge_cops_data = true;
        COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
        free(p);
    }
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Positive test */
    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 111112345678901 > vimsi_req.3 2> /dev/null",
    gen_sipc_path, modem_counter);
    RUN_CMD("%s verify_imsi_resp %u 0 1 0xff > exp_vimsi_resp.3",
            gen_sipc_path, modem_counter);

    read_file("vimsi_req.3", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_vimsi_resp.3", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    /* Positive test with shorter IMSI */
    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 11111 > vimsi_req.4 2> /dev/null",
    gen_sipc_path, modem_counter);
    RUN_CMD("%s verify_imsi_resp %u 0 1 0xff > exp_vimsi_resp.4",
            gen_sipc_path, modem_counter);

    read_file("vimsi_req.4", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_vimsi_resp.4", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    /* Negative test */
    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 123456789012345 > vimsi_req.5 2> /dev/null",
            gen_sipc_path, modem_counter);
    RUN_CMD("%s verify_imsi_resp %u 16 1 5 > exp_vimsi_resp.5", gen_sipc_path,
            modem_counter);

    read_file("vimsi_req.5", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_vimsi_resp.5", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    /* Negative test  with shorter IMSI */
    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 12 > vimsi_req.6 2> /dev/null",
            gen_sipc_path, modem_counter);
    RUN_CMD("%s verify_imsi_resp %u 16 1 5 > exp_vimsi_resp.6", gen_sipc_path,
            modem_counter);


    read_file("vimsi_req.6", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_vimsi_resp.6", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    /* Shorter IMSI than the number of digits used in the lock should fail */
    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 1111 > vimsi_req.7 2> /dev/null",
            gen_sipc_path, modem_counter);
    RUN_CMD("%s verify_imsi_resp %u 16 1 5 > exp_vimsi_resp.7", gen_sipc_path,
            modem_counter);

    read_file("vimsi_req.7", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_vimsi_resp.7", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
        };
        COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);
    }

    {
        uint8_t *p;
        cops_bind_properties_arg_t arg;
        char simple_edn_gid_cd[PATH_MAX]; /* simple EDN rule with GIDs */

        memset(&arg, 0, sizeof(arg));

        strcpy(simple_edn_gid_cd, template_dir);
        strcat(simple_edn_gid_cd, "/simple_edn_gid.csd");
        read_file(simple_edn_gid_cd, &p, &arg.cops_data_length);
        arg.cops_data = p;
        arg.merge_cops_data = true;
        COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
        free(p);
        /* This will trigger read sim data so we have to increment counter*/
        ape_counter++;
    }
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /*
     * verify imsi req ->
     * <- sim data req
     * sim data resp ->
     * <- verify imsi resp
     */

    /* Positive test */

    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 111112345678901 > vimsi_req.8 2> /dev/null",
            gen_sipc_path, modem_counter);
    RUN_CMD("%s verify_imsi_resp %u 16 1 5 > exp_vimsi_resp.8", gen_sipc_path,
            modem_counter);
    ape_counter++;
    RUN_CMD("%s read_sim_data_req -c %d > exp_rsimd_req.1",
            gen_sipc_path, ape_counter);
    RUN_CMD("%s read_sim_data_resp -c %d 111112345678901 2 6 > rsimd_resp.1",
            gen_sipc_path, ape_counter);

    read_file("vimsi_req.8", &in_sipc, &in_sipc_len);

    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);

    read_file("exp_rsimd_req.1", &exp_out_sipc, &exp_out_sipc_len);
    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    read_file("rsimd_resp.1", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_vimsi_resp.8", &exp_out_sipc, &exp_out_sipc_len);
    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    /* Positive test with shorter IMSI */
    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 1111 > vimsi_req.9 2> /dev/null",
            gen_sipc_path, modem_counter);
    RUN_CMD("%s verify_imsi_resp %u 16 1 5 > exp_vimsi_resp.9", gen_sipc_path,
            modem_counter);

    ape_counter++;
    RUN_CMD("%s read_sim_data_req -c %d > exp_rsimd_req.2",
            gen_sipc_path, ape_counter);
    RUN_CMD("%s read_sim_data_resp -c %d 1111 2 6 > rsimd_resp.2 2> /dev/null",
            gen_sipc_path, ape_counter);

    read_file("vimsi_req.9", &in_sipc, &in_sipc_len);

    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);

    read_file("exp_rsimd_req.2", &exp_out_sipc, &exp_out_sipc_len);
    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    read_file("rsimd_resp.2", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_vimsi_resp.9", &exp_out_sipc, &exp_out_sipc_len);
    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    /* Negative test */

    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 111112345678901 > vimsi_req.10 2> /dev/null",
            gen_sipc_path, modem_counter);
    RUN_CMD("%s verify_imsi_resp %u 0 1 0xff > exp_vimsi_resp.10",
            gen_sipc_path,
            modem_counter);

    ape_counter++;
    RUN_CMD("%s read_sim_data_req -c %d > exp_rsimd_req.3",
    gen_sipc_path, ape_counter);
    RUN_CMD("%s read_sim_data_resp -c %d 111112345678901 2 5 > rsimd_resp.3",
    gen_sipc_path, ape_counter);

    read_file("vimsi_req.10", &in_sipc, &in_sipc_len);

    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_rsimd_req.3", &exp_out_sipc, &exp_out_sipc_len);
    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    read_file("rsimd_resp.3", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_vimsi_resp.10", &exp_out_sipc, &exp_out_sipc_len);
    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    /* Negative test with shorter IMSI */
    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 1111123456789 > vimsi_req.11 2> /dev/null",
            gen_sipc_path, modem_counter);
    RUN_CMD("%s verify_imsi_resp %u 0 1 0xff > exp_vimsi_resp.11",
            gen_sipc_path,
            modem_counter);

    ape_counter++;
    RUN_CMD("%s read_sim_data_req -c %d > exp_rsimd_req.4",
    gen_sipc_path, ape_counter);
    RUN_CMD("%s read_sim_data_resp -c %d 1111123456789 2 5 "
            "> rsimd_resp.4 2> /dev/null", gen_sipc_path, ape_counter);

    read_file("vimsi_req.11", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_rsimd_req.4", &exp_out_sipc, &exp_out_sipc_len);
    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    read_file("rsimd_resp.4", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("exp_vimsi_resp.11", &exp_out_sipc, &exp_out_sipc_len);
    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    tst_resm(TINFO, "Verify IMSI OK");
}

static void test_3_get_seed(void)
{
    uint8_t *in_sipc;

    size_t in_sipc_len;

    uint8_t *out_sipc;

    size_t out_sipc_len;

    uint8_t *exp_out_sipc;

    size_t exp_out_sipc_len;

    uint32_t seed = 0;

    /* Generate get seed request message */
    modem_counter++;
    RUN_CMD("%s get_seed_req %u > get_seed.in", gen_sipc_path, modem_counter);

    /* Generate response packet (hard coded seed - 0xdeadbeef) */
    RUN_CMD("%s get_seed_resp 0 %u > get_seed.out.1", gen_sipc_path,
            modem_counter);

    read_file("get_seed.in", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
    &out_sipc, &out_sipc_len), COPS_RC_OK);
    read_file("get_seed.out.1", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);

    /* Copy the seed from return message */
    memcpy(&seed, out_sipc + 12, sizeof(uint32_t));
    CMP_BUF(out_sipc + 12, sizeof(uint32_t), (uint8_t *) &seed,
    sizeof(uint32_t));

    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    tst_resm(TINFO, "Get Seed: OK");

}
#if 0
static void test_4_vsh(void)
{
    uint8_t *in_sipc;

    size_t in_sipc_len;

    uint8_t *out_sipc;

    size_t out_sipc_len;

    uint8_t *exp_out_sipc;

    size_t exp_out_sipc_len;

    int pl_type;

    /* Generate verify signed header (for signedfile oem1) request message */

    /* Test 1 should succeed - oem1
     * Test 2 should fail (wrong pl_type) - oem1
     * Test 3 should succeed - oem2
     * Test 4 should fail (wrong pl_type) - oem2
     * Test 5 should succeed - secure
     * Test 6 should fail (wrong pl_type) - secure */

    pl_type = 5;
    RUN_CMD
    ("%s verify_signed_header_req %s/signedfile_oem1.bin %d > vsh_oem1.in.1",
     gen_sipc_path, sdata_path, pl_type);

    /* Generate response packet (hard coded payload hash) */
    RUN_CMD
    ("%s verify_signed_header_resp 0 %s/signedfile_oem1.bin > vsh_oem1.out.1",
     gen_sipc_path, sdata_path);

    read_file("vsh_oem1.in.1", &in_sipc, &in_sipc_len);

    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);

    read_file("vsh_oem1.out.1", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    tst_resm(TINFO, "Verify signed OEM1 payload: OK");

    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    /* Generate verify signed header (for signedfile oem1) request message
     * should fail - wrong payload type */
    pl_type = 0;
    RUN_CMD
    ("%s verify_signed_header_req %s/signedfile_oem1.bin %d > vsh_oem1.in.2",
     gen_sipc_path, sdata_path, pl_type);

    /* Generate response packet (hard coded payload hash) */
    RUN_CMD
    ("%s verify_signed_header_resp 10 %s/signedfile_oem1.bin > vsh_oem1.out.2",
     gen_sipc_path, sdata_path);

    read_file("vsh_oem1.in.2", &in_sipc, &in_sipc_len);

    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);

    read_file("vsh_oem1.out.2", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);

    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    tst_resm(TINFO, "Verify signed OEM1 payload failed: OK");

    /* This is skipped because the test data file is wrong since
     * the payload type signed by OEM2 key can't not be AUTHCERT(0x0A)!
     * It should be the AUTH_CHALLENGE instead! */
    /*
     * // OEM2
     *
     *
     * pl_type = -1;
     * RUN_CMD("%s verify_signed_header_req %s/signedfile_oem2.bin %d "
     *         "> vsh_oem2.in.1", gen_sipc_path, sdata_path, pl_type);
     *
     * // generate response packet (hard coded payload hash)
     * RUN_CMD("%s verify_signed_header_resp 0 %s/signedfile_oem2.bin "
     *         "> vsh_oem2.out.1", gen_sipc_path, sdata_path);
     *
     * read_file("vsh_oem2.in.1", &in_sipc, &in_sipc_len);
     *
     * COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
     * &out_sipc, &out_sipc_len), COPS_RC_OK);
     *
     * read_file("vsh_oem2.out.1", &exp_out_sipc, &exp_out_sipc_len);
     *
     * CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
     * tst_resm(TINFO, "Verify signed OEM2 payload: OK");
     *
     * free(in_sipc);
     * free(out_sipc);
     * free(exp_out_sipc);
     *
     * // generate verify signed header (for signedfile oem2) request message
     * // should fail - wrong payload type
     * pl_type = 0;
     * RUN_CMD("%s verify_signed_header_req %s/signedfile_oem2.bin %d "
     *         "> vsh_oem2.in.2", gen_sipc_path, sdata_path, pl_type);
     *
     * // generate response packet (hard coded payload hash)
     * RUN_CMD("%s verify_signed_header_resp 10 %s/signedfile_oem2.bin "
     *         "> vsh_oem2.out.2", gen_sipc_path, sdata_path);
     *
     * read_file("vsh_oem2.in.2", &in_sipc, &in_sipc_len);
     *
     * COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
     * &out_sipc, &out_sipc_len), COPS_RC_OK);
     *
     * read_file("vsh_oem2.out.2", &exp_out_sipc, &exp_out_sipc_len);
     *
     * CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
     *
     * free(in_sipc);
     * free(out_sipc);
     * free(exp_out_sipc);
     *
     * tst_resm(TINFO, "Verify signed OEM2 payload failed: OK");
     */

    /* Secure World */


    pl_type = 0;
    RUN_CMD
    ("%s verify_signed_header_req %s/signedfile_secure.bin %d > "
    "vsh_secure.in.1",
    gen_sipc_path, sdata_path, pl_type);

    /* Generate response packet (hard coded payload hash) */
    RUN_CMD
    ("%s verify_signed_header_resp 0 %s/signedfile_secure.bin > "
    "vsh_secure.out.1",
    gen_sipc_path, sdata_path);

    read_file("vsh_secure.in.1", &in_sipc, &in_sipc_len);

    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);

    read_file("vsh_secure.out.1", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    tst_resm(TINFO, "Verify signed Secure World payload: OK");

    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    /* Generate verify signed header (for signedfile secure world)
     * request message
     * should fail - wrong payload type */
    pl_type = 10;
    RUN_CMD("%s verify_signed_header_req %s/signedfile_secure.bin %d "
            "> vsh_secure.in.2", gen_sipc_path, sdata_path, pl_type);

    /* Generate response packet (hard coded payload hash) */
    RUN_CMD("%s verify_signed_header_resp 10 %s/signedfile_secure.bin "
            "> vsh_secure.out.2", gen_sipc_path, sdata_path);

    read_file("vsh_secure.in.2", &in_sipc, &in_sipc_len);

    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);

    read_file("vsh_secure.out.2", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);

    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    tst_resm(TINFO, "Verify signed Secure World payload failed: OK");
}
#endif
static void test_5_vsc(void)
{
    /*
     * uint8_t  *in_sipc;
     * size_t    in_sipc_len;
     * uint8_t  *out_sipc;
     * size_t    out_sipc_len;
     * uint8_t  *exp_out_sipc;
     * size_t    exp_out_sipc_len;
     * int       pl_type;
     */
    uint8_t *filedata = NULL;

    size_t fsize;

    char str[512];

    cops_auth_data_t auth_data;
    char sfile[] = { "/signedfile_sec.bin" };

    snprintf(str, sizeof(str), "%s%s", sdata_path, sfile);

    read_file(str, &filedata, &fsize);

    auth_data.auth_type = COPS_AUTH_TYPE_RSA_CHALLENGE;
    auth_data.length = fsize;
    auth_data.data = filedata;

    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);

    free(filedata);

    tst_resm(TINFO, "Verify signed Challenge: OK");
    return;
}

static void test_6_getchallenge(void)
{
    uint8_t buf[48];

    uint8_t tmpbuf[48];

    size_t len = 0;

    size_t expected_len = 48;

    uint32_t version = 0x00000001;

    int i;

    int count = 0;

    /*
     * uint8_t  *in_sipc;
     * size_t    in_sipc_len;
     * uint8_t  *out_sipc;
     * size_t    out_sipc_len;
     * uint8_t  *exp_out_sipc;
     * size_t    exp_out_sipc_len;
     * int       pl_type;
     */
    /* test 1, ask for an invalid challenge type
     * test 2, ask for a correct challenge type, NULL buffer <- return
     *         needed length
     * test 3, ask for a correct challenge type, NULL length in
     * test 4, ask for a correct challenge type, ok buf and length in */

    COPS_CHK_RC(cops_get_challenge
                (ctx, COPS_AUTH_TYPE_SIMLOCK_KEYS, NULL, NULL),
                COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_get_challenge
                (ctx, COPS_AUTH_TYPE_RSA_CHALLENGE, NULL, &len),
                COPS_RC_OK);

    if (len != expected_len) {
        tst_brkm(TFAIL, cleanup, "%s:%d buffers %s and %s not equal",
        __FILE__, __LINE__, (char *) len, (char *) expected_len);
    }

    COPS_CHK_RC(cops_get_challenge
                (ctx, COPS_AUTH_TYPE_RSA_CHALLENGE, tmpbuf, NULL),
                COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_get_challenge
                (ctx, COPS_AUTH_TYPE_RSA_CHALLENGE, tmpbuf, &len),
                COPS_RC_OK);

    if (len != expected_len) {
        tst_brkm(TFAIL, cleanup, "%s:%d lenght(s) %s and %s not equal",
                 __FILE__, __LINE__, (char *) len, (char *) expected_len);
    }

    memset(buf, 0, 48);
    memcpy(buf, &version, sizeof(version));
    /* Assume chip unique ID is 0 for now */

    /* Can only verify the first 16 bytes to be the same */
    CMP_BUF(tmpbuf, 16, buf, 16);

    /* Check that the random data isn't all zero */
    for (i = 16; i < 48; i++) {
        if (tmpbuf[i] == 0) {
            count++;
        }
    }

    if (count == 32) {
        tst_brkm(TFAIL, cleanup, "%s:%d Random challenge is only zeros.",
        __FILE__, __LINE__);
    }

    tst_resm(TINFO, "Get Challenge: OK");
    return;
}

static void test_7_update_cops_data(void)
{
    cops_bind_properties_arg_t arg;
    size_t blen;
    uint8_t *cops_data;
    uint8_t *data_to_update;
    size_t updatelen;
    size_t updateddatalen;
    char default2_cd[PATH_MAX];/* default data that includes modem data */

    memset(&arg, 0, sizeof(arg));

    /* Authenticate using permanent authentication */
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
        };
        COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);
    }

    /* Write the default data including modem data */
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_SIMLOCK_KEYS, sizeof(keys),
            (uint8_t *) &keys
        };
        uint8_t *p;

        memset(&arg, 0, sizeof(arg));
        arg.imei = &IMEI;
        arg.num_new_auth_data = 1;
        arg.auth_data = &auth_data;
        strcpy(default2_cd, template_dir);
        strcat(default2_cd, "/default2.csd");
        read_file(default2_cd, &p, &arg.cops_data_length);
        arg.cops_data = p;
        COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
        free(p);
    }

    /* Read current COPS Data */
    COPS_CHK_RC(cops_read_data(ctx, NULL, &blen), COPS_RC_OK);
    cops_data = malloc(blen);

    if (cops_data == NULL) {
        tst_brkm(TFAIL, cleanup, "memory allocation failed");
    }

    COPS_CHK_RC(cops_read_data(ctx, cops_data, &blen), COPS_RC_OK);

    /* Find Modem data */
    COPS_CHK_RC(cops_util_find_parameter(cops_data, blen, 0x1000,
                                         &data_to_update, &updatelen),
                COPS_RC_OK);
    /* Update Modem data */
    {
        uint8_t i;

        for (i = 0; i < updatelen; i++) {
            *(data_to_update + i) = i;
        }
    }
    /* Write down the data */
    {
        memset(&arg, 0, sizeof(arg));
        arg.cops_data = cops_data;
        arg.cops_data_length = blen;
        arg.merge_cops_data = true;
        COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
        free(cops_data);
    }

    /* Read back the data and check that it is correctly written */
    {
        uint8_t i;

        COPS_CHK_RC(cops_read_data(ctx, NULL, &blen), COPS_RC_OK);
        cops_data = malloc(blen);

        if (cops_data == NULL) {
            tst_brkm(TFAIL, cleanup, "memory allocation failed");
        }

        COPS_CHK_RC(cops_read_data(ctx, cops_data, &blen), COPS_RC_OK);
        COPS_CHK_RC(cops_util_find_parameter(cops_data, blen, 0x1000,
                                             &data_to_update,
                                             &updateddatalen), COPS_RC_OK);

        if (updateddatalen != updatelen)
            tst_brkm(TFAIL, cleanup,
                     "length missmatch of the paramter to update");

        for (i = 0; i < updatelen; i++) {
            if (*(data_to_update + i) != i) {
                tst_brkm(TFAIL, cleanup, "parameter incorrect updated");
            }
        }
    }

    memset(&arg, 0, sizeof(arg));

    /* Write the standard default data */
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_SIMLOCK_KEYS, sizeof(keys),
            (uint8_t *) &keys
        };
        uint8_t *p;
        char default_cd[PATH_MAX];

        memset(&arg, 0, sizeof(arg));
        arg.imei = &IMEI;
        arg.num_new_auth_data = 1;
        arg.auth_data = &auth_data;
        arg.merge_cops_data = false;
        strcpy(default_cd, template_dir);
        strcat(default_cd, "/default1.csd");
        read_file(default_cd, &p, &arg.cops_data_length);
        arg.cops_data = p;
        COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
        free(p);
    }

    free(cops_data);
    tst_resm(TINFO, "Update COPS data OK");
}

static void test_8_simlock(void)
{
    cops_bind_properties_arg_t arg;
    cops_simlock_control_key_t network_key = { "11111111" };
    cops_simlock_control_key_t ns_key = { "22222222" };
    cops_simlock_control_key_t wrong_key = { "01111111" };
    cops_simlock_status_t simlock_status;
    cops_simlock_control_key_t sim_key = {"000000"};
    cops_simlock_control_key_t sim_key_short = {"00000"};
    cops_simlock_control_key_t sim_key_long = {"1234567"};

    /* create temporary context that only listens for events */
    if (cops_context_create_async(&temp_ctx, &eventcallbacks, evaux, NULL,
                                  NULL) != COPS_RC_OK) {
        tst_brkm(TFAIL, cleanup, "memory allocation failed");
    }

    RUN_CMD("cp %s/%s .", template_dir, "NL_NSL_SPL_CL_Locked.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(arg, "NL_NSL_SPL_CL_Locked.csd", false);

    /* unlock network lock */
    COPS_CHK_RC(cops_simlock_unlock
                (ctx, COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK, &network_key),
                COPS_RC_OK);

    /* Check that network lock is unlocked */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_UNLOCKED) {
        tst_brkm(TFAIL, cleanup, "failed to unlock nl simlock 1");
    }

    /* unlock network subset lock */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK,
                                    &ns_key), COPS_RC_OK);
    /* Check that network lock is unlocked */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_UNLOCKED) {
        tst_brkm(TFAIL, cleanup, "failed to unlock nsl simlock");
    }

    /* recover it to locked */
    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(arg, "NL_NSL_SPL_CL_Locked.csd", false);
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
        };
        COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);
    }

    /* check that the attempts has correct value */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.attempts_left != 2) {
        tst_brkm(TFAIL, cleanup,
        "get attempts left from get_status failed");
    }

    /* try to unlock network subset with wrong key */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    /* Check that no unlock is done */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "simlock unlocked unallowed");
    }

    /* check that the attempts has decreased */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.attempts_left != 1) {
        tst_brkm(TFAIL, cleanup, "decreasing attempts failed");
    }

    /* try to unlock network subset with wrong key */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    /* test max attempts */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK,
                                    &ns_key),
                COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT);
    /* test timer */
    tst_resm(TINFO, "...Entering timer tests, wait a few minutes...");

    /* check that timer is not running for sp-lock */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.spl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "timer is running incorrectly");
    }

    /* try to unlock sp-lock with wrong key to trig a timer */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                  COPS_SIMLOCK_LOCK_TYPE_SERVICE_PROVIDER_LOCK,
                                  &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);
    /* check that timer is running */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (!simlock_status.spl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "timer is not running 1");
    }

    sleep(1);
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.spl_status.timer_attempts_left != 5) {
        tst_brkm(TFAIL, cleanup, "timer attempts left wrong");
    }

    /* check that timer is running */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                  COPS_SIMLOCK_LOCK_TYPE_SERVICE_PROVIDER_LOCK,
                                  &wrong_key), COPS_RC_TIMER_RUNNING);
    /* timer attempts left shall not decrease since no attempt is done */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.spl_status.timer_attempts_left != 5) {
        tst_brkm(TFAIL, cleanup, "timer attempts left wrong");
    }

    if (!simlock_status.spl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "timer is not running 2");
    }

    if (simlock_status.spl_status.attempts_left != 0) {
        tst_brkm(TFAIL, cleanup, "attempts left wrong 1");
    }

    sleep(1);
    /* check that timer is not running  */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.spl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "timer is running incorrectly");
    }

    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                  COPS_SIMLOCK_LOCK_TYPE_SERVICE_PROVIDER_LOCK,
                                  &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.spl_status.timer_attempts_left != 4) {
        tst_brkm(TFAIL, cleanup, "timer attempts left wrong");
    }

    if (!simlock_status.spl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "timer is not running 3");
    }

    if (simlock_status.spl_status.attempts_left != 0) {
        tst_brkm(TFAIL, cleanup, "attempts left wrong 2");
    }

    /* Delay should now be (delay+add)*mul/10 = (2+2)*20/10 */
    sleep(7);
    /* Timer should still run */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (!simlock_status.spl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "timer is not running 4");

        if (simlock_status.spl_status.time_left_on_running_timer != 1) {
            tst_brkm(TFAIL, cleanup,
            "time left on timer has wrong value 1");
        }
    }

    sleep(1);
    /* Timer should be out */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.spl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "timer is running incorrectly");
    }

    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                  COPS_SIMLOCK_LOCK_TYPE_SERVICE_PROVIDER_LOCK,
                                  &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    /* Delay should now be (8+2)*2 */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (!simlock_status.spl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "time is not running 5");
    }

    if (simlock_status.spl_status.time_left_on_running_timer != 20) {
        tst_brkm(TFAIL, cleanup, "time left on timer has wrong value 2");
    }

    sleep(19);
    /* Timer should still run */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (!simlock_status.spl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "timer is not running 6");
    }

    if (simlock_status.spl_status.time_left_on_running_timer != 1) {
        tst_brkm(TFAIL, cleanup, "time left on timer has wrong value 3");
    }

    sleep(1);
    /* Timer should be out */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.spl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "timer is running incorrectly");
    }

    if (simlock_status.spl_status.timer_attempts_left != 3) {
        tst_brkm(TFAIL, cleanup, "timer attempts left wrong");
    }

    if (simlock_status.spl_status.attempts_left != 0) {
        tst_brkm(TFAIL, cleanup, "attempts left wrong 3");
    }

    /* try to unlock cl-lock with wrong key  */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);
    /* check that timer is not running for cl-lock since 2 attempts
     * are allowed before timer */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "timer is running incorrectly");
    }

    if (simlock_status.cl_status.timer_attempts_left != 2) {
        tst_brkm(TFAIL, cleanup, "cl timer attempts left wrong 1");
    }

    if (simlock_status.cl_status.attempts_left != 1) {
        tst_brkm(TFAIL, cleanup, "attempts left wrong");
    }

    /* try to unlock cl-lock with wrong key to trig a timer */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);
    /* check that timer is running */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (!simlock_status.cl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "cl timer is not running 1");
    }

    if (simlock_status.cl_status.timer_attempts_left != 2) {
        tst_brkm(TFAIL, cleanup, "cl timer attempts left wrong 2");
    }

    if (simlock_status.cl_status.attempts_left != 0) {
        tst_brkm(TFAIL, cleanup, "attempts left wrong");
    }

    sleep(3);
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.timer_is_running) {
        tst_brkm(TFAIL, cleanup, "cl timer is incorrectly running 1");
    }

    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.timer_attempts_left != 1) {
        tst_brkm(TFAIL, cleanup, "cl timer attempts left wrong 3");
    }

    sleep(3);
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.timer_attempts_left != 0) {
        tst_brkm(TFAIL, cleanup, "cl timer attempts left wrong 4");
    }

    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK,
                                    &wrong_key),
                COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT);

    /* new test data with NL locked with 1 attempts, timer start val 2
     * 99 ta, 1 add, 15 mul */
    RUN_CMD("cp %s/%s .", template_dir, "NL_a_1_t_99_2_1_15.csd");

    auth_wdefaultd_deauth(arg, "NL_a_1_t_99_2_1_15.csd", false);

    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    sleep(2);
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.attempts_left != 0) {
        tst_brkm(TFAIL, cleanup, "NLa1t99115 attempts left wrong 1");
    }

    if (simlock_status.nl_status.timer_attempts_left != 99) {
        tst_brkm(TFAIL, cleanup, "NLa1t99115 timeatt left wrong 1");
    }

    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.attempts_left != 0) {
        tst_brkm(TFAIL, cleanup, "NLa1t99115 attempts left wrong 1");
    }

    if (simlock_status.nl_status.timer_attempts_left != 98) {
        tst_brkm(TFAIL, cleanup, "NLa1t99115 timer attempts left wrong 2");
    }

    /* delay to next try is (2+1)*1,5 = 4 */
    sleep(3);
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                    &network_key), COPS_RC_TIMER_RUNNING);
    sleep(1);
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);
    /* delay to next try is (4+1)*1,5 = 7 */
    sleep(6);
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                    &network_key), COPS_RC_TIMER_RUNNING);
    sleep(1);
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);
    /* delay to next try is (7+1)*1,5 = 12 */
    sleep(11);
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                    &network_key), COPS_RC_TIMER_RUNNING);
    sleep(1);
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                    &network_key), COPS_RC_OK);

    /* Just gather all events here to not mess with the timing things above*/
    wait_and_dispatch_status_events(temp_ctx, SIMLOCK_EVENT, evaux,
                                false, false, false, false,
                                COPS_SIMLOCK_CARD_STATUS_NOT_APPROVED,
                                COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK);

    wait_and_dispatch_status_events(temp_ctx, SIMLOCK_EVENT, evaux,
                                false, false, false, false,
                                COPS_SIMLOCK_CARD_STATUS_APPROVED, 0xff);

    cops_context_destroy(&temp_ctx);

    /* lock to (U)SIM lock */

    /* unlock the lock and expect STATUS_APPROVED
       too short key, wrong key & correct key
    2 attempts + 2 timer attempts
    timer start = 3
    timer addition = 1
    */

    RUN_CMD("cp %s/%s .", template_dir, "sim_personalization.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(arg, "sim_personalization.csd", false);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.siml_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Lock setting for (U)SIM Lock error: %u",
                 simlock_status.siml_status.lock_setting);
    }

    COPS_CHK_RC(cops_simlock_unlock
                (ctx, COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK, &sim_key_short),
                COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_simlock_unlock
                (ctx, COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK, &sim_key_long),
                COPS_RC_INVALID_SIMLOCK_KEY);

    COPS_CHK_RC(cops_simlock_unlock
                (ctx, COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK, &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    sleep(3);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.siml_status.attempts_left != 0) {
        tst_brkm(TFAIL, cleanup, "Attempts left for (U)SIM lockt wrong %zu",
                 simlock_status.siml_status.attempts_left);
    }

    if (simlock_status.siml_status.timer_attempts_left != 2) {
        tst_brkm(TFAIL, cleanup, "(U)SIM timer attempts left wrong %zu",
                 simlock_status.siml_status.timer_attempts_left);
    }

    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.siml_status.attempts_left != 0) {
        tst_brkm(TFAIL, cleanup, "Attempts left for (U)SIM lockt wrong %zu",
                 simlock_status.siml_status.attempts_left);
    }

    if (simlock_status.siml_status.timer_attempts_left != 1) {
        tst_brkm(TFAIL, cleanup, "(U)SIM timer attempts left wrong %zu",
                 simlock_status.siml_status.timer_attempts_left);
    }
    /* delay to next try is (3+1)*1,5 = 6 */
    sleep(5);
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK,
                                    &network_key), COPS_RC_TIMER_RUNNING);
    sleep(1);
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    /* no more attempts left */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.siml_status.timer_attempts_left != 0) {
        tst_brkm(TFAIL, cleanup, "(U)SIM timer attempts left wrong %zu",
                 simlock_status.siml_status.timer_attempts_left);
    }

    /* delay to next try is (6+1)*1.5 = 10 */
    sleep(9);
    COPS_CHK_RC(cops_simlock_unlock(ctx, COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK,
                                    &network_key),
                COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT);

    auth_wdefaultd_deauth(arg, "sim_personalization.csd", false);

    COPS_CHK_RC(cops_simlock_unlock
                (ctx, COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK, &sim_key), COPS_RC_OK);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.siml_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_UNLOCKED) {
        tst_brkm(TFAIL, cleanup, "Lock setting for (U)SIM Lock error: %u",
                 simlock_status.siml_status.lock_setting);
    }

    if (simlock_status.sim_card_status !=
        COPS_SIMLOCK_CARD_STATUS_APPROVED) {
        tst_brkm(TFAIL, cleanup, "Lock status for (U)SIM Lock error: %u",
                 simlock_status.sim_card_status);
    }

    /* Test that number of attempts are reset after successful unlock */
    {
        size_t exp_attempts_left;

        COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);
        exp_attempts_left = simlock_status.nl_status.attempts_left;

        COPS_CHK_RC(cops_simlock_unlock(ctx,
                                        COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                        &wrong_key),
                    COPS_RC_INVALID_SIMLOCK_KEY);

        COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);
        if (simlock_status.nl_status.attempts_left != exp_attempts_left - 1) {
            tst_brkm(TFAIL, cleanup,
            "Number of attempts not decreased: %lu (exp: %lu)",
            simlock_status.nl_status.attempts_left, exp_attempts_left - 1);
        }

        COPS_CHK_RC(cops_simlock_unlock(ctx,
                                        COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                        &network_key),
                    COPS_RC_OK);
        COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

        if (simlock_status.nl_status.attempts_left != exp_attempts_left) {
            tst_brkm(TFAIL, cleanup,
            "Number of attempts not restored after successful unlock: %lu<>%lu",
            exp_attempts_left, simlock_status.nl_status.attempts_left);
        }
    }


    tst_resm(TINFO, "SIMLOCK OK");
}

static void test_10_unlock_ota(void)
{
    cops_bind_properties_arg_t arg;
    cops_simlock_status_t simlock_status;
    uint8_t *ota_msg;
    size_t ota_msg_len;
    cops_simlock_ota_reply_message_t ota_reply_msg;
    size_t i;
    cops_imei_t imei;
    /* Expected reply message*/
    cops_simlock_ota_reply_message_t ota_exp_reply_msg;
    cops_auth_data_t auth_data_key;
    uint8_t *p;
    cops_auth_data_t auth_data;
    static cops_simlock_control_keys_t longkeys = {
        {"111111112222"},
        {"2222222211111111"},
        {"44444444"},
        {"555555552222"},
        {"6234567822222222"}
    };
    char locked_cd[PATH_MAX]; /* default data with locks set */

    /* create temporary context that only listens for events */
    if (cops_context_create_async(&temp_ctx, &eventcallbacks2, evaux, NULL,
                                 NULL) != COPS_RC_OK) {
        tst_brkm(TFAIL, cleanup, "memory allocation failed");
    }

    ota_msg_len = 40 + 32 + 16;
    ota_msg = malloc(ota_msg_len);

    if (ota_msg == NULL) {
        tst_brkm(TFAIL, cleanup, "memory allocation failed");
    }

    memset(ota_msg + 40, '1', 8);
    memset(ota_msg + 48, '2', 8);
    memset(ota_msg + 56, '3', 8);
    memset(ota_msg + 64, '4', 8);

    COPS_CHK_RC(cops_read_imei(ctx, &imei), COPS_RC_OK);

    for (i = 0; i < 15; i++) {
        *(ota_msg + 40 + 32 + i) = imei.digits[i] + '0';
        ota_exp_reply_msg.data[i] = imei.digits[i] + '0';
    }

    *(ota_msg + 40 + 32 + 15) = '0';
    ota_exp_reply_msg.data[15] = '0';

    ota_exp_reply_msg.data[16] = '0';
    ota_exp_reply_msg.data[17] = '0';
    ota_exp_reply_msg.data[18] = '1';
    ota_exp_reply_msg.data[19] = '0';

    memset(&arg, 0, sizeof(arg));

    /* Authenticate using permanent authentication */
    auth_data.auth_type = COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION;
    auth_data.length = 0;
    auth_data.data = NULL;
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);

    /* Write the default data with locks set */
    auth_data_key.auth_type = COPS_AUTH_TYPE_SIMLOCK_KEYS;
    auth_data_key.length = sizeof(keys);
    auth_data_key.data = (uint8_t *)&keys;
    memset(&arg, 0, sizeof(arg));
    arg.imei = &IMEI;
    arg.num_new_auth_data = 1;
    arg.auth_data = &auth_data_key;
    strcpy(locked_cd, template_dir);
    strcat(locked_cd, "/ota1.csd");
    read_file(locked_cd, &p, &arg.cops_data_length);
    arg.cops_data = p;
    COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
    free(p);

    wait_and_dispatch_status_events(temp_ctx,
                                    SIMLOCK_EVENT | AUTHENTICATION_EVENT, evaux,
                                    false, false, false, false,
                                    COPS_SIMLOCK_CARD_STATUS_NOT_APPROVED,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK);

    /* Deauthenticate */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Check that network lock is locked */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " nl simlock is not locked 1");
    }

    /* Unlock ota */
    COPS_CHK_RC(cops_simlock_ota_unlock(ctx, ota_msg, ota_msg_len,
                                        &ota_reply_msg), COPS_RC_OK);

    /* Check the reply message */
    if (memcmp(&(ota_reply_msg.data), &(ota_exp_reply_msg.data),
               COPS_SIMLOCK_OTA_REPLY_MESSAGE_SIZE) != 0) {
        tst_brkm(TFAIL, cleanup, "unexpected OTA reply message");
    }

    /* Check that network lock is unlocked */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_UNLOCKED) {
        tst_brkm(TFAIL, cleanup, "failed to OTA unlock nl simlock 33");
    }

    free(ota_msg);

    wait_and_dispatch_status_events(temp_ctx,
                               OTA_EVENT | SIMLOCK_EVENT | AUTHENTICATION_EVENT,
                               evaux, true, true, false, true,
                               COPS_SIMLOCK_CARD_STATUS_APPROVED, 0xff);

    /* Try to use longer keys */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);

    memset(&arg, 0, sizeof(arg));
    auth_data_key.length = sizeof(longkeys);
    auth_data_key.data = (uint8_t *)&longkeys;
    arg.num_new_auth_data = 1;
    arg.auth_data = &auth_data_key;
    strcpy(locked_cd, template_dir);
    strcat(locked_cd, "/ota1.csd");
    read_file(locked_cd, &p, &arg.cops_data_length);
    arg.cops_data = p;
    arg.imei = &IMEI;
    COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
    free(p);

    wait_and_dispatch_status_events(temp_ctx,
                                    SIMLOCK_EVENT | AUTHENTICATION_EVENT, evaux,
                                    false, false, false, false,
                                    COPS_SIMLOCK_CARD_STATUS_NOT_APPROVED,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK);

    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Check that network lock is locked */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " nl simlock is not locked 2");
    }

    ota_msg_len = 40 + 32 + 16 + 32;
    ota_msg = malloc(ota_msg_len);

    if (ota_msg == NULL) {
        tst_brkm(TFAIL, cleanup, "memory allocation failed");
    }

    memset(ota_msg + 40, '1', 8);
    memset(ota_msg + 48, '2', 8);
    memset(ota_msg + 56, '4', 8);
    memset(ota_msg + 64, '5', 8);

    COPS_CHK_RC(cops_read_imei(ctx, &imei), COPS_RC_OK);

    for (i = 0; i < 15; i++) {
        *(ota_msg + 40 + 32 + i) = imei.digits[i] + '0';
        ota_exp_reply_msg.data[i] = imei.digits[i] + '0';
    }

    *(ota_msg + 40 + 32 + 15) = '0';

    memset(ota_msg + 88, '2', 4);
    memset(ota_msg + 92, 'F', 4);
    memset(ota_msg + 96, '1', 8);
    memset(ota_msg + 104, 'F', 8);
    memset(ota_msg + 112, '2', 4);
    memset(ota_msg + 116, 'F', 4);

    /* Unlock ota */
    COPS_CHK_RC(cops_simlock_ota_unlock(ctx, ota_msg, ota_msg_len,
                                        &ota_reply_msg), COPS_RC_OK);

    /* Check the reply message */
    if (memcmp(&(ota_reply_msg.data), &(ota_exp_reply_msg.data),
               COPS_SIMLOCK_OTA_REPLY_MESSAGE_SIZE) != 0) {
        tst_brkm(TFAIL, cleanup,
                 "unexpected OTA reply message when long keys");
    }

    /* Check that corporate lock is unlocked */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_UNLOCKED) {
        tst_brkm(TFAIL, cleanup, "failed to OTA unlock cl simlock");
    }

    wait_and_dispatch_status_events(temp_ctx,
                               OTA_EVENT | SIMLOCK_EVENT | AUTHENTICATION_EVENT,
                               evaux, true, true, false, true,
                               COPS_SIMLOCK_CARD_STATUS_APPROVED, 0xff);

    /*
     * Try to ota unlock when one key is wrong (the other locks should be
     * unlocked anyway
     */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);

    memset(&arg, 0, sizeof(arg));
    auth_data_key.data[0] = '5';
    arg.num_new_auth_data = 1;
    arg.auth_data = &auth_data_key;
    strcpy(locked_cd, template_dir);
    strcat(locked_cd, "/ota1.csd");
    read_file(locked_cd, &p, &arg.cops_data_length);
    arg.cops_data = p;
    arg.imei = &IMEI;
    COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
    free(p);
    wait_and_dispatch_status_events(temp_ctx,
                                    SIMLOCK_EVENT | AUTHENTICATION_EVENT, evaux,
                                    false, false, false, false,
                                    COPS_SIMLOCK_CARD_STATUS_NOT_APPROVED,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK);

    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Unlock ota */
    COPS_CHK_RC(cops_simlock_ota_unlock(ctx, ota_msg, ota_msg_len,
                                        &ota_reply_msg), COPS_RC_OK);

    /* Check the reply message */
    ota_exp_reply_msg.data[16] = '2';

    if (memcmp(&(ota_reply_msg.data), &(ota_exp_reply_msg.data),
               COPS_SIMLOCK_OTA_REPLY_MESSAGE_SIZE) != 0) {
        tst_brkm(TFAIL, cleanup,
                 "unexpected OTA reply message when long keys 2");
    }

    /* Check that corporate lock is unlocked */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_UNLOCKED) {
        tst_brkm(TFAIL, cleanup, "failed to OTA unlock cl simlock");
    }

    free(ota_msg);

    wait_and_dispatch_status_events(temp_ctx, OTA_EVENT | AUTHENTICATION_EVENT,
                                    evaux, false, true, false, true, 0, 0);

    cops_context_destroy(&temp_ctx);

    tst_resm(TINFO, "Unlock OTA OK");
}

static void test_11_verify_keys(void)
{
    cops_bind_properties_arg_t arg;

    static cops_simlock_control_keys_t wrong_keys = {
        {"11111111"},
        {"22232222"},
        {"33333333"},
        {"44444444"},
        {"55555555"}
    };

    memset(&arg, 0, sizeof(arg));

    /* Authenticate using permanent authentication */
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
        };
        COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);
    }

    /* Bind properties and set simlock keys */
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_SIMLOCK_KEYS, sizeof(keys),
            (uint8_t *) &keys
        };
        uint8_t *p;
        char default_cd[PATH_MAX];

        memset(&arg, 0, sizeof(arg));
        arg.imei = &IMEI;
        arg.num_new_auth_data = 1;
        arg.auth_data = &auth_data;
        strcpy(default_cd, template_dir);
        strcat(default_cd, "/default1.csd");
        read_file(default_cd, &p, &arg.cops_data_length);
        arg.cops_data = p;
        COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
        free(p);
    }

    /* Deauthenticate */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* verify simlock keys with correct keys */
    COPS_CHK_RC(cops_simlock_verify_control_keys(ctx, &keys), COPS_RC_OK);

    {
        uint8_t i;

        for (i = 0; i < 3; i++) {
            /* verify simlock keys with incorrect keys */

            COPS_CHK_RC(cops_simlock_verify_control_keys(ctx, &wrong_keys),
                        COPS_RC_INVALID_SIMLOCK_KEY);
        }
    }
    /* verify simlock keys with correct keys */
    COPS_CHK_RC(cops_simlock_verify_control_keys(ctx, &keys),
                COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT);

    tst_resm(TINFO, "Verify simlock control keys OK");


}

static void test_9_autolock(void)
{
    cops_bind_properties_arg_t bind_properties_arg;

    cops_simlock_status_t simlock_status;

    test_verify_imsi_params vimsi_params;

    int test_counter = 0;

    char *autolock_file = "";

    cops_simlock_control_key_t sim_key = {"000000"};

    /* Initialize verify imsi params */
    vimsi_params.imsi = "";
    vimsi_params.req_vimsi_file = "";
    vimsi_params.exp_resp_vimsi_file = "";
    vimsi_params.exp_nbr_active_locks = "";
    vimsi_params.exp_blocking_lock = "";
    vimsi_params.exp_errorcode = "";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.gid_included = false;
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;

    tst_resm(TINFO, "Test_9_1_Autolock");

    /*
     * Test NL and MCC = 2.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: NL with MCC = 2", test_counter);

    RUN_CMD("cp %s/%s .", template_dir, "autolock_NL_2.csd");
    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_NL_2.csd", false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.20";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.20";
    vimsi_params.imsi = "111182545678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;

    /**
     * Need to restart test suite as autolock only is allowed during startup
     */

    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock NL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.21";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.21";
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";      /* Invalid IMSI */
    vimsi_params.req_vimsi_file = "vimsi_req.22";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.22";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    verify_imsi(vimsi_params, true, false);

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.21";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.21";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.imsi = "111182545678901";      /* Valid IMSI */
    verify_imsi(vimsi_params, true, false);

    /*
     * Test NL and MCC = 3.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: NL with MCC = 3", test_counter);

    RUN_CMD("cp %s/%s .", template_dir, "autolock_NL_3.csd");
    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_NL_3.csd", false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.23";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.23";
    vimsi_params.imsi = "111116345678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock NL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.24";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.24";
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";      /* Invalid IMSI */
    vimsi_params.req_vimsi_file = "vimsi_req.25";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.25";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    verify_imsi(vimsi_params, true, !false);

    /*
     * Test NSL and MCC = 2.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: NSL with MCC = 2", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_NSL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_NSL_2.csd", false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.26";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.26";
    vimsi_params.imsi = "111112445678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";

    verify_imsi(vimsi_params, true, true);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock NSL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.27";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.27";
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";      /* Invalid IMSI */
    vimsi_params.req_vimsi_file = "vimsi_req.28";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.28";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "1";
    verify_imsi(vimsi_params, true, true);

    RUN_CMD("cp %s/%s .", template_dir, "exkl_autolock_NSL_2.csd");
    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */

    /* Make it possible to add NSL over preious lock. */
    auth_wdefaultd_deauth(bind_properties_arg, "exkl_autolock_NSL_2.csd", true);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI & lock NSL over NSL");
    vimsi_params.req_vimsi_file = "vimsi_req.0";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.0";
    vimsi_params.imsi = "211182545678901";
    vimsi_params.exp_nbr_active_locks = "0";
    vimsi_params.exp_nbr_active_locks = "1";
    /* Should be ok, since no lock codes available -> latest lock valid */
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, true);

    /*
     * Test NSL and MCC = 3.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: NSL with MCC = 3", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_NSL_3.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_NSL_3.csd", false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.29";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.29";
    vimsi_params.imsi = "111112345678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock NSL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.30";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.30";
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.31";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.31";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "1";
    verify_imsi(vimsi_params, true, false);

    /*
     * Test SPL and MCC = 2.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: SPL with MCC = 3", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_SPL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_SPL_2.csd", false);

    /* Verify IMSI to trig autolock with GID */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.32";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.32";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.32";
    vimsi_params.resp_rsimd_file = "rsimd_resp.32";
    vimsi_params.imsi = "211182345678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "2";
    vimsi_params.gid_2 = "6";
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.spl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock SPL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.33";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.33";
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "211192345678901";
    vimsi_params.req_vimsi_file = "vimsi_req.34";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.34";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "2";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.34";
    vimsi_params.resp_rsimd_file = "rsimd_resp.34";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid GID */
    tst_resm(TINFO, "    Check invalid GID");
    vimsi_params.imsi = "211182345678901";      /* Valid IMSI */
    vimsi_params.req_vimsi_file = "vimsi_req.35";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.35";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.35";
    vimsi_params.resp_rsimd_file = "rsimd_resp.35";
    vimsi_params.gid_1 = "3";
    verify_imsi(vimsi_params, true, false);

    /*
     * Test SPL and MCC = 3.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: SPL with MCC = 3", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_SPL_3.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_SPL_3.csd", false);

    /* Verify IMSI to trig autolock with GID */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.36";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.36";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.36";
    vimsi_params.resp_rsimd_file = "rsimd_resp.36";
    vimsi_params.imsi = "211182345678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "4";
    vimsi_params.gid_2 = "6";
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.spl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock SPL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.37";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.37";
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "211192345678901";
    vimsi_params.req_vimsi_file = "vimsi_req.38";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.38";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "2";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.38";
    vimsi_params.resp_rsimd_file = "rsimd_resp.38";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid GID */
    tst_resm(TINFO, "    Check invalid GID");
    vimsi_params.imsi = "211182345678901";      /* Valid IMSI */
    vimsi_params.req_vimsi_file = "vimsi_req.39";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.39";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.39";
    vimsi_params.resp_rsimd_file = "rsimd_resp.39";
    vimsi_params.gid_1 = "5";
    verify_imsi(vimsi_params, true, false);

    /*
     * Test CL and MCC = 2.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: CL with MCC = 2", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_CL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_CL_2.csd", false);

    /* Verify IMSI to trig autolock with GID */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.40";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.40";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.40";
    vimsi_params.resp_rsimd_file = "rsimd_resp.40";
    vimsi_params.imsi = "211182345678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "6";
    vimsi_params.gid_2 = "8";
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock CL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.41";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.41";
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "211192345678901";
    vimsi_params.req_vimsi_file = "vimsi_req.42";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.42";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "3";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.42";
    vimsi_params.resp_rsimd_file = "rsimd_resp.42";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid GID */
    tst_resm(TINFO, "    Check invalid GID");
    vimsi_params.imsi = "211182345678901";      /* Valid IMSI */
    vimsi_params.req_vimsi_file = "vimsi_req.43";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.43";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.43";
    vimsi_params.resp_rsimd_file = "rsimd_resp.43";
    vimsi_params.gid_2 = "5";
    verify_imsi(vimsi_params, true, false);

    /*
     * Test CL autolock when no GID's on SIM (MCC = 2).
     */
    test_counter++;
    tst_resm(TINFO, "  %d: CL no GID on SIM", test_counter);

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_CL_2.csd", false);

    /* Verify IMSI to trig autolock with GID */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.300";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.300";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.300";
    vimsi_params.resp_rsimd_file = "rsimd_resp.300";
    vimsi_params.imsi = "211182345678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "0";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_1 = "0xff";
    vimsi_params.gid_2 = "0xff";
    vimsi_params.gid_included = true;
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /*
     * Test CL and MCC = 3.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: CL with MCC = 3", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_CL_3.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_CL_3.csd", false);

    /* Verify IMSI to trig autolock with GID */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.44";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.44";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.44";
    vimsi_params.resp_rsimd_file = "rsimd_resp.44";
    vimsi_params.imsi = "211182345678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "6";
    vimsi_params.gid_2 = "8";
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock CL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.45";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.45";
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "211192345678901";
    vimsi_params.req_vimsi_file = "vimsi_req.46";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.46";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "3";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.46";
    vimsi_params.resp_rsimd_file = "rsimd_resp.46";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid GID */
    tst_resm(TINFO, "    Check invalid GID");
    vimsi_params.imsi = "211182345678901";      /* Valid IMSI */
    vimsi_params.req_vimsi_file = "vimsi_req.47";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.47";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.47";
    vimsi_params.resp_rsimd_file = "rsimd_resp.47";
    vimsi_params.gid_2 = "5";
    verify_imsi(vimsi_params, true, false);

    /*
     * Test (U)SIM personalization lock
     *  Locked to IMSI 123456789876543
     */
    test_counter++;
    tst_resm(TINFO,
             "  %d: (U)SIM Personalization", test_counter);

    autolock_file = "sim_autolock.csd";
    RUN_CMD("cp %s/%s .", template_dir, autolock_file);
    RUN_CMD("cp %s/%s .", template_dir, "sim_personalization.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, autolock_file, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.siml_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "(U)SIM Lock error (wrong lock_setting %d)",
                 simlock_status.siml_status.lock_setting);
    }

    if (simlock_status.sim_card_status !=
        COPS_SIMLOCK_CARD_STATUS_APPROVED) {
        tst_brkm(TFAIL, cleanup, "(U)SIM Lock failed. "
                 "Wrong card status %u", simlock_status.sim_card_status);
    }

    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.48";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.48";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.imsi = "123456789876543";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.sim_card_status !=
        COPS_SIMLOCK_CARD_STATUS_APPROVED) {
        tst_brkm(TFAIL, cleanup, "(U)SIM Lock failed (wrong card_status, %d)",
                 simlock_status.sim_card_status);
    }

    autolock_file = "sim_personalization.csd";
    auth_wdefaultd_deauth(bind_properties_arg, autolock_file, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.siml_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Wrong lockstatus for (U)SIM Lock %u",
                 simlock_status.siml_status.lock_setting);
    }

    if (simlock_status.sim_card_status !=
        COPS_SIMLOCK_CARD_STATUS_APPROVED) {
        tst_brkm(TFAIL, cleanup, "(U)SIM Lock failed. "
                 "Wrong card status %u", simlock_status.sim_card_status);
    }

    /* Verify IMSI with correct IMSI */
    tst_resm(TINFO, "    Verify IMSI");
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.48";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.48";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.imsi = "123456789876543";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.sim_card_status !=
        COPS_SIMLOCK_CARD_STATUS_APPROVED) {
        tst_brkm(TFAIL, cleanup, "(U)SIM Lock failed. "
                 "Wrong card status %u", simlock_status.sim_card_status);
    }

    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.48a";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.48a";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "4";
    vimsi_params.imsi = "12345678987654";      /* invalid IMSI */
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.sim_card_status !=
        COPS_SIMLOCK_CARD_STATUS_NOT_APPROVED) {
        tst_brkm(TFAIL, cleanup, "Lock failed for (U)SIM Lock, "
                 "(wrong card_status %d)", simlock_status.sim_card_status);
    }

    COPS_CHK_RC(cops_simlock_unlock
                (ctx, COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK, &sim_key), COPS_RC_OK);

    /*
     * Test NL and NSL with MCC = 2.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: NL (MCC = 2) and NSL (MCC = 2)", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_NL_NSL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_NL_NSL_2.csd", false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.49";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.49";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.imsi = "111182445678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "2";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /* Check the locks. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.nsl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock NL and NSL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.50";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.50";
    vimsi_params.exp_nbr_active_locks = "2";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.51";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.51";
    vimsi_params.exp_nbr_active_locks = "2";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    verify_imsi(vimsi_params, true, true);

    /*
     * Test NL with MCC = 2 and NSL with MCC = 3.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: NL (MCC = 2) and NSL (MCC = 3)", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_NL_2_NSL_3.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_NL_2_NSL_3.csd",
                          false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.52";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.52";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.imsi = "111182425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "2";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /* Check the locks. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.nsl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock NL and NSL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.53";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.53";
    vimsi_params.exp_nbr_active_locks = "2";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.54";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.54";
    vimsi_params.exp_nbr_active_locks = "2";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    verify_imsi(vimsi_params, true, true);

    /*
     * Test NL with MCC = 2 and SPL with MCC = 2.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: NL (MCC = 2) and SPL (MCC = 2)", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_NL_SPL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_NL_SPL_2.csd", false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.55";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.55";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.55";
    vimsi_params.resp_rsimd_file = "rsimd_resp.55";
    vimsi_params.imsi = "111182425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "2";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "3";
    vimsi_params.gid_2 = "4";
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /* Check the locks. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.spl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock NL and SPL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.56";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.56";
    vimsi_params.exp_nbr_active_locks = "2";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.57";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.57";
    vimsi_params.exp_nbr_active_locks = "2";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    verify_imsi(vimsi_params, true, true);

    /*
     * Test NL and NSL (MCC = 2), SPL and CL (MCC = 3).
     */
    test_counter++;
    tst_resm(TINFO, "  %d: NL and NSL (MCC = 2), SPL and CL (MCC = 3)",
             test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_NL_NSL_2_SPL_CL_3.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg,
                          "autolock_NL_NSL_2_SPL_CL_3.csd", false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.58";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.58";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.58";
    vimsi_params.resp_rsimd_file = "rsimd_resp.58";
    vimsi_params.imsi = "111184425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "4";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "3";
    vimsi_params.gid_2 = "4";
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /* Check the locks. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.nsl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.spl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.cl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock NL, NSL, SPL or CL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.59";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.59";
    vimsi_params.exp_nbr_active_locks = "4";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.60";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.60";
    vimsi_params.exp_nbr_active_locks = "4";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    verify_imsi(vimsi_params, true, true);

    /*
     * Test NL and CL (MCC = 3), NSL and SPL (MCC = 2).
     */
    test_counter++;
    tst_resm(TINFO, "  %d: NL and CL (MCC = 3), NSL and SPL (MCC = 2)",
             test_counter);  /* Conflicting locks? */
    RUN_CMD("cp %s/%s .", template_dir, "autolock_NL_CL_3_NSL_SPL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_NL_CL_3_NSL_SPL_2.csd",
                          false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.61";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.61";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.61";
    vimsi_params.resp_rsimd_file = "rsimd_resp.61";
    vimsi_params.imsi = "111184425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "4";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "3";
    vimsi_params.gid_2 = "4";
    vimsi_params.include_cnl = false;
    verify_imsi(vimsi_params, true, false);

    /* Check the locks. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.nsl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.spl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.cl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock NL, NSL, SPL or CL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.62";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.62";
    vimsi_params.exp_nbr_active_locks = "4";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.63";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.63";
    vimsi_params.exp_nbr_active_locks = "4";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    verify_imsi(vimsi_params, true, false);

    /*
     * Test CNL: NL (MCC = 2)
     */
    test_counter++;
    tst_resm(TINFO, "  %d: CNL and NL (MCC = 2)", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_CNL_NL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_CNL_NL_2.csd",
                          false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.64";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.64";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.64";
    vimsi_params.resp_rsimd_file = "rsimd_resp.64";
    vimsi_params.imsi = "111184425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "3";
    vimsi_params.gid_2 = "5";
    vimsi_params.include_cnl = true;
    vimsi_params.nbr_cnl_entries = "2";
    /* CNL example:
     * IMSI = 123456789012345
     * CNL entry (MNC=2) = 0x21, 0xf3, 0x54, 0xff, 0xff, 0xff
     *           (MNC=3) = 0x21, 0x63, 0x54, 0xff, 0xff, 0xff
     */
    vimsi_params.cnl_entries = "0x34 0x05 0x67 0x89 0xff 0xff "
                               "0x11 0xf1 0x81 0xff 0xff 0xff";
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock CNL NL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.65";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.65";
    vimsi_params.include_cnl = false;
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.66";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.66";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    verify_imsi(vimsi_params, true, false);

    /*
     * Test CNL: NL (MCC = 3)
     */
    test_counter++;
    tst_resm(TINFO, "  %d: CNL and NL (MCC = 3)", test_counter);

    RUN_CMD("cp %s/%s .", template_dir, "autolock_CNL_NL_3.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */

    auth_wdefaultd_deauth(bind_properties_arg, "autolock_CNL_NL_3.csd", false);

    /* Verify IMSI, to trig autolock. Incorrect IMSI in CNL */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.67";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.67";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.67";
    vimsi_params.resp_rsimd_file = "rsimd_resp.67";
    vimsi_params.imsi = "111184425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "3";
    vimsi_params.gid_2 = "5";
    vimsi_params.include_cnl = true;
    vimsi_params.nbr_cnl_entries = "2";
    vimsi_params.compare_buffers = false;
    /* CNL example:
     * IMSI = 123456789012345
     * CNL entry (MNC=2) = 0x21, 0xf3, 0x54, 0xff, 0xff, 0xff
     *           (MNC=3) = 0x21, 0x63, 0x54, 0xff, 0xff, 0xff
     */
    vimsi_params.cnl_entries = "0x34 0x05 0x67 0x89 0xff 0xff "
                               "0x11 0x41 0x89 0xff 0xff 0xff";
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock CNL NL failed");
    }
    RUN_CMD("cp %s/%s .", template_dir, "autolock_CNL_NL_3.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_CNL_NL_3.csd", false);

    /* Verify IMSI, to trig autolock. Correct IMSI in CNL */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.68";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.68";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.68";
    vimsi_params.resp_rsimd_file = "rsimd_resp.68";
    vimsi_params.imsi = "111184425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "0";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "3";
    vimsi_params.gid_2 = "5";
    vimsi_params.include_cnl = true;
    vimsi_params.nbr_cnl_entries = "2";
    /* CNL example:
     * IMSI = 123456789012345
     * CNL entry (MNC=2) = 0x21, 0xf3, 0x54, 0xff, 0xff, 0xff
     *           (MNC=3) = 0x21, 0x63, 0x54, 0xff, 0xff, 0xff
     */
    vimsi_params.cnl_entries = "0x34 0x05 0x67 0x89 0xff 0xff "
                               "0x11 0x41 0x81 0xff 0xff 0xff";
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock CNL NL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.69";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.69";
    vimsi_params.include_cnl = false;
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.70";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.70";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    verify_imsi(vimsi_params, true, false);

    /*
     * Test CNL: NSL (MCC = 2)
     */
    test_counter++;
    tst_resm(TINFO, "  %d: CNL and NSL (MCC = 2)", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_CNL_NSL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_CNL_NSL_2.csd", false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.71";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.71";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.71";
    vimsi_params.resp_rsimd_file = "rsimd_resp.71";
    vimsi_params.imsi = "111184425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "3";
    vimsi_params.gid_2 = "5";
    vimsi_params.include_cnl = true;
    vimsi_params.nbr_cnl_entries = "4";
    vimsi_params.compare_buffers = true;
    /* CNL example:
     * IMSI = 123456789012345
     * CNL entry (MNC=2) = 0x21, 0xf3, 0x54, 0xff, 0xff, 0xff
     *           (MNC=3) = 0x21, 0x63, 0x54, 0xff, 0xff, 0xff
     */
    vimsi_params.cnl_entries =
        "0x34 0x05 0x67 0x89 0xff 0xff "
        "0x11 0x21 0x81 0x44 0xff 0xff "
        "0x11 0x21 0x81 0x24 0xff 0xff "
        "0x11 0xf1 0x81 0x44 0xff 0xff";
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock CNL NSL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.72";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.72";
    vimsi_params.include_cnl = false;
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.73";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.73";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "1";
    verify_imsi(vimsi_params, true, false);

    /*
     * Test CNL: NSL (MCC = 3)
     */
    test_counter++;
    tst_resm(TINFO, "  %d: CNL and NSL (MCC = 3)", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_CNL_NSL_3.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_CNL_NSL_3.csd", false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.73";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.74";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.74";
    vimsi_params.resp_rsimd_file = "rsimd_resp.74";
    vimsi_params.imsi = "111184425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "3";
    vimsi_params.gid_2 = "5";
    vimsi_params.include_cnl = true;
    vimsi_params.nbr_cnl_entries = "2";
    /* CNL example:
     * IMSI = 123456789012345
     * CNL entry (MNC=2) = 0x21, 0xf3, 0x54, 0xff, 0xff, 0xff
     *           (MNC=3) = 0x21, 0x63, 0x54, 0xff, 0xff, 0xff
     */
    vimsi_params.cnl_entries = "0x34 0x05 0x67 0x89 0xff 0xff "
                               "0x11 0x41 0x81 0x24 0xff 0xff";
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock CNL NSL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.75";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.75";
    vimsi_params.include_cnl = false;
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.76";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.76";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "1";
    verify_imsi(vimsi_params, true, true);

    /*
     * Test NSL (MCC = 2) CNL: NL (MCC = 2)
     */
    test_counter++;
    tst_resm(TINFO, "  %d: NSL and CNL with NL (MCC = 2)", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_NSL_CNL_NL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_NSL_CNL_NL_2.csd",
                          false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.77";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.77";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.77";
    vimsi_params.resp_rsimd_file = "rsimd_resp.77";
    vimsi_params.imsi = "111184425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "2";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "3";
    vimsi_params.gid_2 = "5";
    vimsi_params.include_cnl = true;
    vimsi_params.nbr_cnl_entries = "2";
    /* CNL example:
     * IMSI = 123456789012345
     * CNL entry (MNC=2) = 0x21, 0xf3, 0x54, 0xff, 0xff, 0xff
     *           (MNC=3) = 0x21, 0x63, 0x54, 0xff, 0xff, 0xff
     */
    vimsi_params.cnl_entries = "0x34 0x05 0x67 0x89 0xff 0xff "
                               "0x11 0xf1 0x81 0x24 0xff 0xff";
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock NSL CNL NL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.78";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.78";
    vimsi_params.include_cnl = false;
    vimsi_params.gid_included = false;
    vimsi_params.exp_nbr_active_locks = "2";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.79";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.79";
    vimsi_params.exp_nbr_active_locks = "2";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    verify_imsi(vimsi_params, true, false);

    /*
     * Test CNL: NL, NSL, CL and SPL (MCC = 2)
     */
    test_counter++;
    tst_resm(TINFO, "  %d: CNL: NL, NSL, CL, SPL (MCC = 2)", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_CNL_NL_NSL_CL_SPL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg,
                          "autolock_CNL_NL_NSL_CL_SPL_2.csd", false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.80";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.80";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.80";
    vimsi_params.resp_rsimd_file = "rsimd_resp.80";
    vimsi_params.imsi = "111184425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "4";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "3";
    vimsi_params.gid_2 = "5";
    vimsi_params.include_cnl = true;
    vimsi_params.nbr_cnl_entries = "4";
    /* CNL example:
     * IMSI = 123456789012345
     * CNL entry (MNC=2) = 0x21, 0xf3, 0x54, 0xff, 0xff, 0xff
     *           (MNC=3) = 0x21, 0x63, 0x54, 0xff, 0xff, 0xff
     */
    vimsi_params.cnl_entries = "0x34 0x05 0x67 0x89 0xff 0xff "
                               "0x11 0xf1 0x81 0x24 0x33 0x33 "
                              " 0x11 0xf1 0x81 0x44 0x30 0x50 "
                               "0x55 0xf1 0x81 0x24 0x30 0xff";
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.cl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED ||
            simlock_status.spl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock CNL: NL, NSL, CL, SPL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.81";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.81";
    vimsi_params.include_cnl = false;
    vimsi_params.gid_included = true;
    vimsi_params.exp_nbr_active_locks = "4";

    verify_imsi(vimsi_params, true, false);
    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.82";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.82";
    vimsi_params.exp_nbr_active_locks = "4";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    vimsi_params.gid_included = true;
    verify_imsi(vimsi_params, true, false);

    /*
     * Test CNL with > 42 (max) entries: NL (MCC = 2)
     * Expected to be ok, even though the maximum allowed is 42, since
     * the restriction is related to the SIM card.
     */
    test_counter++;
    tst_resm(TINFO, "  %d: CNL > 42 (max) entries: NL (MCC = 2)",
             test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_CNL_NL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_CNL_NL_2.csd", false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI");
    vimsi_params.req_vimsi_file = "vimsi_req.83";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.80";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.83";
    vimsi_params.resp_rsimd_file = "rsimd_resp.83";
    vimsi_params.imsi = "111184425678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "3";
    vimsi_params.gid_2 = "5";
    vimsi_params.include_cnl = true;
    /* Expected to be OK. No check in code. since restriction on SIM card */
    vimsi_params.nbr_cnl_entries = "44";
    /* CNL example:
     * IMSI = 123456789012345
     * CNL entry (MNC=2) = 0x21, 0xf3, 0x54, 0xff, 0xff, 0xff
     *           (MNC=3) = 0x21, 0x63, 0x54, 0xff, 0xff, 0xff
     */
    vimsi_params.cnl_entries =
        "0x01 0x05 0x67 0x89 0xff 0xff "
        "0x02 0x05 0x67 0x89 0xff 0xff " "0x03 0x05 0x67 0x89 0xff 0xff "
        "0x04 0x05 0x67 0x89 0xff 0xff " "0x05 0x05 0x67 0x89 0xff 0xff "
        "0x06 0x05 0x67 0x89 0xff 0xff " "0x07 0x05 0x67 0x89 0xff 0xff "
        "0x08 0x05 0x67 0x89 0xff 0xff " "0x09 0x05 0x67 0x89 0xff 0xff "
        "0x10 0x05 0x67 0x89 0xff 0xff " "0x11 0xf1 0x81 0x44 0xff 0xff "
        "0x12 0xf1 0x81 0x44 0x30 0x50 " "0x13 0xf1 0x81 0x24 0x30 0xff "
        "0x14 0xf1 0x81 0x44 0x30 0x50 " "0x15 0xf1 0x81 0x44 0x30 0x50 "
        "0x16 0xf1 0x81 0x44 0x30 0x50 " "0x17 0xf1 0x81 0x44 0x30 0x50 "
        "0x18 0xf1 0x81 0x44 0x30 0x50 " "0x19 0xf1 0x81 0x44 0x30 0x50 "
        "0x20 0xf1 0x81 0x44 0x30 0x50 " "0x21 0xf1 0x81 0x44 0x30 0x50 "
        "0x22 0xf1 0x81 0x44 0x30 0x50 " "0x23 0xf1 0x81 0x44 0x30 0x50 "
        "0x24 0xf1 0x81 0x44 0x30 0x50 " "0x25 0xf1 0x81 0x44 0x30 0x50 "
        "0x26 0xf1 0x81 0x44 0x30 0x50 " "0x27 0xf1 0x81 0x44 0x30 0x50 "
        "0x28 0xf1 0x81 0x44 0x30 0x50 " "0x29 0xf1 0x81 0x44 0x30 0x50 "
        "0x30 0xf1 0x81 0x44 0x30 0x50 " "0x31 0xf1 0x81 0x44 0x30 0x50 "
        "0x32 0xf1 0x81 0x44 0x30 0x50 " "0x33 0xf1 0x81 0x44 0x30 0x50 "
        "0x34 0xf1 0x81 0x44 0x30 0x50 " "0x35 0xf1 0x81 0x44 0x30 0x50 "
        "0x36 0xf1 0x81 0x44 0x30 0x50 " "0x37 0xf1 0x81 0x44 0x30 0x50 "
        "0x38 0xf1 0x81 0x44 0x30 0x50 " "0x39 0xf1 0x81 0x44 0x30 0x50 "
        "0x40 0xf1 0x81 0x44 0x30 0x50 " "0x41 0xf1 0x81 0x44 0x30 0x50 "
        "0x42 0xf1 0x81 0x44 0x30 0x50 " "0x43 0xf1 0x81 0x44 0x30 0x50 "
        "0x44 0xf1 0x81 0x44 0x30 0x50";
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock CNL: NL failed");
    }

    /* Check valid IMSI. */
    tst_resm(TINFO, "    Check valid IMSI (NL)");
    vimsi_params.req_vimsi_file = "vimsi_req.84";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.84";
    vimsi_params.include_cnl = false;
    vimsi_params.gid_included = false;
    vimsi_params.exp_nbr_active_locks = "1";
    verify_imsi(vimsi_params, true, false);

    /* Check invalid IMSI. */
    tst_resm(TINFO, "    Check invalid IMSI");
    vimsi_params.imsi = "123456789012345";
    vimsi_params.req_vimsi_file = "vimsi_req.85";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.85";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0";
    vimsi_params.gid_included = false;
    verify_imsi(vimsi_params, true, true);


    /*
     * Test: NSL on NL (MCC = 2).
     * 1. Lock to NL, IMSI: 1111824425678901
     * 2. Successful lock?
     * 3. Lock to NSL, IMSI: 1111824425678901
     */
    test_counter++;
    tst_resm(TINFO, "  %d: NSL over NL (MCC = 2)", test_counter);
    RUN_CMD("cp %s/%s .", template_dir, "autolock_NL_2.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(bind_properties_arg, "autolock_NL_2.csd" , false);

    /* Verify IMSI, to trig autolock. */
    tst_resm(TINFO, "    Verify IMSI & lock NL");
    vimsi_params.req_vimsi_file = "vimsi_req.86";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.86";
    vimsi_params.imsi = "111182545678901";      /* Valid IMSI */
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* Check the lock. */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Autolock NL failed");
    }

    /* Check valid (subset of previous NL lock) IMSI. */
    tst_resm(TINFO, "    Check valid IMSI (NSL)");
    vimsi_params.imsi = "111182545678901";      /* Valid IMSI */
    vimsi_params.req_vimsi_file = "vimsi_req.89";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.89";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.include_cnl = false;
    vimsi_params.gid_included = false;
    verify_imsi(vimsi_params, true, false);

    tst_resm(TINFO, "Test_9_Autolock OK");
}




static void test_12_check_gid_order(void)
{
    cops_bind_properties_arg_t arg;
    cops_simlock_status_t simlock_status;
    test_verify_imsi_params vimsi_params;

    /* This test checks that the internal order of the GID1 and the
     * GID2 values in the CNL-list are correctly interpreted when
     * autolocking to CNL */

    /* Test autolock to CNL and GID */

    /* Initialize verify imsi params */
    vimsi_params.imsi = "";
    vimsi_params.req_vimsi_file = "";
    vimsi_params.exp_resp_vimsi_file = "";
    vimsi_params.exp_nbr_active_locks = "";
    vimsi_params.exp_blocking_lock = "";
    vimsi_params.exp_errorcode = "";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.gid_included = false;
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;

    /* Verify IMSI so autolock on CL is trigged
     * verify imsi req ->
     * <- sim data req
     * sim data resp ->
     * <- verify imsi resp
     */

    RUN_CMD("cp %s/%s .", template_dir, "autolockcnl.csd");
    auth_wdefaultd_deauth(arg, "autolockcnl.csd", false);

    vimsi_params.req_vimsi_file = "vimsi_req.50";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.50";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.50";
    vimsi_params.resp_rsimd_file = "rsimd_resp.50";
    vimsi_params.imsi = "123451277678901";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "5";
    vimsi_params.gid_2 = "7";
    vimsi_params.include_cnl = true;
    vimsi_params.nbr_cnl_entries = "8";
    vimsi_params.cnl_entries =
        "0x11 0xF1 0x11 0x11 0x11 0x11 "
        "0x22 0xF2 0x22 0x22 0x22 0x22 "
        "0x21 0xF3 0x54 0x21 0x50 0x70 "
        "0x42 0xF0 0x99 0x21 0x43 0x87 "
        "0x33 0xF3 0x33 0x33 0x33 0x33 "
        "0x32 0xF4 0x76 0x65 0x80 0x90 "
        "0x54 0xF6 0x98 0x54 0x70 0x80 "
        "0x55 0xF5 0x55 0x55 0x55 0x55";
    vimsi_params.compare_buffers = true;

    verify_imsi(vimsi_params, true, false);

    /* Check that the lock is really locked */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "autolocking corporate lock failed");
    }

    /* Check that an IMSI and GID from the CNL-list is valid
     * verify imsi req ->
     * <- sim data req
     * sim data resp ->
     * <- verify imsi resp
     */

    vimsi_params.req_vimsi_file = "vimsi_req.51";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.51";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.51";
    vimsi_params.resp_rsimd_file = "rsimd_resp.51";
    vimsi_params.imsi = "456892345678901";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "7";
    vimsi_params.gid_2 = "8";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;

    verify_imsi(vimsi_params, true, false);

    tst_resm(TINFO, "Test 12 check GID order OK");
}


static void test_13_sl_wildcard(void)
{
    uint8_t *in_sipc;
    size_t in_sipc_len;
    uint8_t *out_sipc;
    size_t out_sipc_len;
    uint8_t *exp_out_sipc;
    size_t exp_out_sipc_len;
    cops_bind_properties_arg_t arg;

    /* Test Flexible ESL with wildcard in the middle of the range */
    /* Authenticate using permanent authentication */
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
        };
        COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);
    }

    /* Write the default data with wildcard in the middle of the range:
     * 2409930****2*** **    */
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_SIMLOCK_KEYS, sizeof(keys),
            (uint8_t *)&keys
        };
        uint8_t *p;
        char wildcard_cd[PATH_MAX];

        memset(&arg, 0, sizeof(arg));
        arg.imei = &IMEI;
        arg.num_new_auth_data = 1;
        arg.auth_data = &auth_data;
        strcpy(wildcard_cd, template_dir);
        strcat(wildcard_cd, "/correslwildcard.csd");
        read_file(wildcard_cd, &p, &arg.cops_data_length);
        arg.cops_data = p;
        COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
        free(p);
    }

    /* Deauthenticate */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Verify IMSI */

    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u 240993000002019 > vimsi_req.60 2> /dev/null",
            gen_sipc_path, modem_counter);
    RUN_CMD("%s verify_imsi_resp %u 0 1 0xff > exp_vimsi_resp.60",
            gen_sipc_path,
            modem_counter);

    read_file("vimsi_req.60", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);

    read_file("exp_vimsi_resp.60", &exp_out_sipc, &exp_out_sipc_len);
    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    tst_resm(TINFO, "Test 13 SIMLock Wildcard OK");
}

static void test_15_sipc_counters(void)
{
    uint8_t *in_sipc;
    uint8_t *out_sipc;
    uint8_t *exp_out_sipc;

    size_t in_sipc_len;
    size_t out_sipc_len;
    size_t exp_out_sipc_len;
    int i;

    char cwd[PATH_MAX];
    char buf[PATH_MAX + 10 + 1];
    FILE *fp;
    uint32_t sessions[2] = { 0 };
    uint32_t rc, erc = 0x0a;

    /* call get_seed and assume it works */
    modem_counter++;
    RUN_CMD("%s get_seed_req %u > get_seed.in.2", gen_sipc_path,
            modem_counter);
    read_file("get_seed.in.2", &in_sipc, &in_sipc_len);

    RUN_CMD("%s get_seed_resp 0 %u > get_seed.out.2", gen_sipc_path,
            modem_counter);

    read_file("get_seed.out.2", &exp_out_sipc, &exp_out_sipc_len);

    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);

    free(out_sipc);
    free(exp_out_sipc);

    /* call get_seed and assume it fails, modem counter is bad (replayed) */
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len),
                COPS_RC_OK);

    /* check for error code 0xa */
    memcpy(&rc, out_sipc + 8, sizeof(rc));

    if (rc != erc) {
        tst_brkm(TFAIL, cleanup, "%s:%d Wrong error code returned.",
                 __FILE__, __LINE__);
    }

    free(in_sipc);
    free(out_sipc);

    /* update counters to test wrap-around */
    sessions[0] = 0xFFFFFFF8;
    sessions[1] = 0xFFFFFFF8;

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        tst_brkm(TBROK, cleanup, "Failed to getcwd");
    }

    snprintf(buf, sizeof(buf), "%s/%s", cwd, "gstate.bin");
    fp = fopen(buf, "w+");

    if (fp == NULL) {
        tst_brkm(TBROK, cleanup, "Failed to create file.");
    }

    in_sipc_len = fwrite(sessions, 1, sizeof(sessions), fp);

    if (in_sipc_len != sizeof(sessions)) {
        tst_brkm(TBROK, cleanup, "fwrite failed. Wrote %lu, expected %lu",
                 in_sipc_len * sizeof(sessions), sizeof(sessions));
    }

    fclose(fp);

    modem_counter = sessions[0];

    for (i = 0; i < 10; i++) {
        modem_counter++;
        RUN_CMD("%s get_seed_req %u > get_seed.in.%u", gen_sipc_path,
                modem_counter, i + 2);

        memset(cwd, 0, sizeof(cwd));

        if (snprintf(cwd, sizeof(cwd), "get_seed.in.%u", i + 2) < 0) {
            tst_brkm(TBROK, cleanup, "snprintf failed.");
        }

        read_file(cwd, &in_sipc, &in_sipc_len);

        COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                       &out_sipc, &out_sipc_len), COPS_RC_OK);

        RUN_CMD("%s get_seed_resp 0 %u > get_seed.out.%u", gen_sipc_path,
                modem_counter, i + 2);

        memset(cwd, 0, sizeof(cwd));

        if (snprintf(cwd, sizeof(cwd), "get_seed.out.%u", i + 2) < 0) {
            tst_brkm(TBROK, cleanup, "snprintf failed.");
        }

        read_file(cwd, &exp_out_sipc, &exp_out_sipc_len);

        CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);

        free(in_sipc);
        free(out_sipc);
        free(exp_out_sipc);
    }

    fp = fopen(buf, "w+");

    if (fp == NULL) {
        tst_brkm(TBROK, cleanup, "Failed to create file %s.", buf);
    }

    in_sipc_len = fwrite(sessions, 1, sizeof(sessions), fp);

    if (in_sipc_len != sizeof(sessions)) {
        tst_brkm(TBROK, cleanup, "fwrite failed. Wrote %lu, expected %lu",
                 in_sipc_len * sizeof(sessions), sizeof(sessions));
    }

    fclose(fp);

    /*
     * Verify IMSI with simple EDN rule with GIDs
     */


    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
        };
        COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);
    }

    {
        uint8_t *p;
        cops_bind_properties_arg_t arg;
        char simple_edn_gid_cd[PATH_MAX];


        memset(&arg, 0, sizeof(arg));

        strcpy(simple_edn_gid_cd, template_dir);
        strcat(simple_edn_gid_cd, "/simple_edn_gid.csd");
        read_file(simple_edn_gid_cd, &p, &arg.cops_data_length);
        arg.cops_data = p;
        arg.merge_cops_data = true;
        COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
        free(p);
        /* This will trigger read sim data so we have to increment counter */
        sessions[1]++;
    }
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /*
     * verify imsi req ->
     * <- sim data req
     * sim data resp ->
     * <- verify imsi resp
     */

    /* Positive test */
    modem_counter = sessions[0];
    ape_counter = sessions[1];

    for (i = 0; i < 10; i++) {
        modem_counter++;
        RUN_CMD("%s verify_imsi_req %u 111112345678901 "
                "> vimsi_req.8 2> /dev/null",
                gen_sipc_path, modem_counter);
        RUN_CMD("%s verify_imsi_resp %u 16 1 5 > exp_vimsi_resp.8",
                gen_sipc_path, modem_counter);

        ape_counter++;
        RUN_CMD("%s read_sim_data_req -c %d > exp_rsimd_req.1",
                gen_sipc_path, ape_counter);
        RUN_CMD("%s read_sim_data_resp -c %d 111112345678901 "
                "2 6 > rsimd_resp.1", gen_sipc_path, ape_counter);

        read_file("vimsi_req.8", &in_sipc, &in_sipc_len);
        COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                       &out_sipc, &out_sipc_len), COPS_RC_OK);

        read_file("exp_rsimd_req.1", &exp_out_sipc, &exp_out_sipc_len);
        CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
        free(in_sipc);
        free(out_sipc);
        free(exp_out_sipc);

        read_file("rsimd_resp.1", &in_sipc, &in_sipc_len);
        COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                       &out_sipc, &out_sipc_len), COPS_RC_OK);
        read_file("exp_vimsi_resp.8", &exp_out_sipc, &exp_out_sipc_len);
        CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
        free(in_sipc);
        free(out_sipc);
        free(exp_out_sipc);
    }

    tst_resm(TINFO, "SIPC Counters test: OK");

}

static void test_201_mck_generic(void)
{
    uint8_t *in_sipc;
    size_t in_sipc_len;
    uint8_t *out_sipc;
    size_t out_sipc_len;
    cops_bind_properties_arg_t arg;
    cops_auth_data_t auth_data_mck;
    cops_simlock_control_key_t mck = { "99999999" };
    cops_auth_data_t auth_data_key;

    RUN_CMD("cp %s/%s .", template_dir, "mck_test.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(arg, "mck_test.csd", false);

    /* Authenticate using permanent authentication */
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
        };
        COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);
    }

    /* Set up authentication data for authentication using simlock keys */
    auth_data_key.auth_type = COPS_AUTH_TYPE_SIMLOCK_KEYS;
    auth_data_key.length = sizeof(keys);
    auth_data_key.data = (uint8_t *)&keys;

    /* Prepare bind properties argument with default data
     * authenticate data with keys
     * */
    memset(&arg, 0, sizeof(arg));
    arg.num_new_auth_data = 1;
    arg.auth_data = &auth_data_key;

    /* Store new known keys by using bind properties */
    COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);

    /* Write down MCK */
    auth_data_mck.auth_type = COPS_AUTH_TYPE_MCK;
    auth_data_mck.length = sizeof(mck);
    auth_data_mck.data = (uint8_t *)&mck;

    /* Prepare bind properties argument with default data
     * authenticate data with keys
     * */
    memset(&arg, 0, sizeof(arg));
    arg.num_new_auth_data = 1;
    arg.auth_data = &auth_data_mck;

    /* Store the keys by using bind properties */
    COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);

    /* Try to do MCK reset for NL. Should fail - unknown msg type */
    RUN_CMD("%s simlock_mck_reset_req %u 0 99999999 > mck_req.1",
                gen_sipc_path, modem_counter);
    RUN_CMD("%s simlock_mck_reset_resp %u 22 > exp_mck_resp.1", gen_sipc_path,
            modem_counter);
    read_file("mck_req.1", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len),
                COPS_RC_INTERNAL_ERROR);
    free(in_sipc);
    tst_resm(TINFO, "Test 201 mck generic test OK");
}

static void cleanup(void)
{
    {
        char cmd[1024];

        snprintf(cmd, sizeof(cmd), "cp cops.log %s", template_dir);
        system(cmd);
        snprintf(cmd, sizeof(cmd), "cp cops.log2 %s", template_dir);
        system(cmd);
    }

    cops_context_destroy(&ctx);

    /* Kill COPS */
    if (cops_pid != -1) {
        fprintf(stderr, "killing copsd (pid=%u)\n", cops_pid);

        if (kill(cops_pid, SIGKILL) != 0)
            fprintf(stderr, "kill(%d [cops], %d): %s",
                    (int) cops_pid, (int) SIGKILL, strerror(errno));

        /* Ugly hack. we seem to spawn an extra copsd (why?)
         * it remains suspended after execution unless we
         * kill it here. */
        if (kill(cops_pid + 1, SIGKILL) != 0)
            fprintf(stderr, "kill(%d [cops], %d): %s",
                    (int) cops_pid, (int) SIGKILL, strerror(errno));

        cops_pid = -1;
    }

    /* Remove all temp files */
    /*
     * Set PAUSE_CLEANUP to avoid cleanup to facilate debugging of secure
     *  world
     */
#if PAUSE_CLEANUP
    {
        int i;
        scanf("%d", &i);
    }
#endif
    tst_rmdir();

    tst_exit();
}
/* set to indicate that the test shouldn't re-create the directory
   to run in and such init stuff. Bascially only re-fork */
static bool setup_done;

static void setup(void)
{
    FILE *f;
    int i;

    /* Create temp directory */
    if (!setup_done) {
        tst_tmpdir();
    }

    /* Start COPS */
    copsd_path = getenv("COPSD");

    if (copsd_path == NULL) {
        tst_brkm(TBROK, cleanup, "COPSD not defined, can't find copsd");
    }

    indication_files_dir = getenv("INDICATION_DIR");

    if (indication_files_dir == NULL)
        tst_brkm(TBROK, cleanup,
                 "INDICATION_DIR not defined, can't create flag files for"
                 "emulation");

    tapp_test_path = getenv("TAPP_TEST");

    if (tapp_test_path == NULL)
        tst_brkm(TBROK, cleanup,
                 "TAPP_TEST not defined, can't find tapp_test");

    template_dir = getenv("TEMPLATES_DIR");

    if (template_dir == NULL)
        tst_brkm(TBROK, cleanup, "TEMPLATES_DIR not defined, "
                 "can't find template files");

    gen_sipc_path = getenv("GEN_SIPC");

    if (gen_sipc_path == NULL)
        tst_brkm(TBROK, cleanup,
                 "GEN_SIPC not defined, can't find gen_sipc");

    sdata_path = getenv("SDATA");

    if (sdata_path == NULL) {
        tst_brkm(TBROK, cleanup, "SDATA not defined, can't find sdata");
    }

    if (!setup_done) {
        f = fopen("tapp_test", "w");

        if (f == NULL)
            tst_brkm(TBROK, cleanup, "fopen(%s, \"w\"): %s",
                     "tapp_test", strerror(errno));

        fprintf(f, "#!/bin/sh\nexec \"%s\" \"$@\"\n", tapp_test_path);
        fclose(f);

        if (chmod("tapp_test", 0555) != 0)
            tst_brkm(TBROK, cleanup, "chmod(%s, 0555): %s",
                     "tapp_test", strerror(errno));

        log_file = fopen("cops.log2", "a");

        if (log_file == NULL) {
            fprintf(stderr, "fopen(\"cops.log\"): %s\n", strerror(errno));
        }
    }

    /* setup for emulated bootpartition access */
    RUN_CMD("cp %s/bootpartition_start.bin bootpartition.bin", template_dir);
    RUN_CMD("echo -n 1 >bootpartition_sysfs_rwflag.bin");
    RUN_CMD("echo -n rw >bootpartition_sysfs_rolock.bin");

    cops_pid = fork();

    if (cops_pid == 0) {
        /* child */
        RUN_CMD("exec %s 2>> cops.log", copsd_path);
        exit(1);
    } else {
        if (!setup_done)
            fprintf(stderr, "starting copsd (pid=%u)\n", cops_pid);
    }

    cops_api_socket_path = "cops.socket";

    for (i = 0; i < 20; i++) {
        struct timeval tv = { 0, 100000 };
        select(0, NULL, NULL, NULL, &tv);

        if (cops_context_create(&ctx, NULL, NULL) == COPS_RC_OK) {
            break;
        }
    }
    setup_done = true;
}

static void ota_unlock_status_cb(void *event_aux,
                                         const
                                         cops_simlock_ota_unlock_status_t
                                        *unlock_status)
{
    if (!(event_cb_expected_data.eventtype & 1)) {
        tst_brkm(TFAIL, cleanup, "OTA Unlock CB: wrong eventtype");
    }

    event_cb_expected_data.eventtype ^= OTA_EVENT;

    if (event_cb_expected_data.aux != event_aux) {
        tst_brkm(TFAIL, cleanup, "OTA Unlock CB: wrong aux-pointer");
    }

    if (event_cb_expected_data.ota_unlock_status.network !=
       unlock_status->network) {
        tst_brkm(TFAIL, cleanup, "OTA Unlock CB: Network lock wrong status");
    }

    if (event_cb_expected_data.ota_unlock_status.network_subset !=
       unlock_status->network_subset) {
        tst_brkm(TFAIL, cleanup,
                 "OTA Unlock CB: Network Subset lock wrong status");
    }

    if (event_cb_expected_data.ota_unlock_status.service_provider !=
       unlock_status->service_provider) {
        tst_brkm(TFAIL, cleanup,
                 "OTA Unlock CB: Service Provider lock wrong status");
    }

    if (event_cb_expected_data.ota_unlock_status.corporate !=
       unlock_status->corporate) {
        tst_brkm(TFAIL, cleanup, "OTA Unlock CB: Corporate lock wrong status");
    }

    tst_resm(TINFO, "OTA Unlock CB OK");
}

static void simlock_status_cb(void *event_aux,
                              const cops_sim_status_t *status)
{
    if (!(event_cb_expected_data.eventtype & 2)) {
        tst_brkm(TFAIL, cleanup, "simlock status cb: wrong eventtype");
    }

    event_cb_expected_data.eventtype ^= SIMLOCK_EVENT;

    if (event_cb_expected_data.aux != event_aux) {
        tst_brkm(TFAIL, cleanup, "simlock status cb: wrong aux-pointer");
    }

    if (event_cb_expected_data.sim_status.card_status !=
       status->card_status) {
        tst_brkm(TFAIL, cleanup, "simlock status cb: sim card status wrong,"
                 " expected %x result %x",
                 event_cb_expected_data.sim_status.card_status,
                 status->card_status);
    }

    if (event_cb_expected_data.sim_status.failed_lt != status->failed_lt) {
        tst_brkm(TFAIL, cleanup, "simlock status cb: failed lt value wrong,"
                 " expected %x result %x",
                 event_cb_expected_data.sim_status.failed_lt,
                 status->failed_lt);
    }

    tst_resm(TINFO, "simlock status cb OK");
}

static void authentication_changed_cb(void *event_aux)
{
    if (!(event_cb_expected_data.eventtype & 4)) {
        tst_brkm(TFAIL, cleanup, "authentication changed cb: wrong eventtype");
    }

    event_cb_expected_data.eventtype ^= AUTHENTICATION_EVENT;

    if (event_cb_expected_data.aux != event_aux) {
        tst_brkm(TFAIL, cleanup, "authentication changed cb: wrong aux");
    }

    tst_resm(TINFO, "authentication changed cb OK");
}

static void imei_cb(void *async_aux, cops_return_code_t rc,
                         const cops_imei_t *imei)
{
    /* We expect only one IMEI */
    if (memcmp(imei, &IMEI, sizeof(*imei)) != 0) {
        CMP_BUF((uint8_t *) imei, sizeof(imei), (uint8_t *) &IMEI,
        sizeof(*imei));
        tst_brkm(TFAIL, cleanup, "Got invalid IMEI");
    } else {
        tst_resm(TINFO, "Async IMEI OK");
    }
}

static void wait_and_dispatch_status_events(cops_context_id_t *ctxp,
                                         uint8_t eventmask,
                                         char *aux,
                                         bool network,
                                         bool network_subset,
                                         bool service_provider,
                                         bool corporate,
                                         cops_simlock_card_status_t card_status,
                                         cops_simlock_lock_type_t failed_lt)
{
    event_cb_expected_data.eventtype = 0;
    event_cb_expected_data.aux = aux;

    if (eventmask & OTA_EVENT) {
        event_cb_expected_data.eventtype |= 1;
        event_cb_expected_data.ota_unlock_status.network = network;
        event_cb_expected_data.ota_unlock_status.network_subset =
            network_subset;
        event_cb_expected_data.ota_unlock_status.service_provider =
            service_provider;
        event_cb_expected_data.ota_unlock_status.corporate = corporate;
    }

    if (eventmask & SIMLOCK_EVENT) {
        event_cb_expected_data.eventtype |= 2;
        event_cb_expected_data.sim_status.card_status = card_status;
        event_cb_expected_data.sim_status.failed_lt = failed_lt;
    }

    if (eventmask & AUTHENTICATION_EVENT) {
        event_cb_expected_data.eventtype |= 4;
    }

    wait_and_dispatch_events(ctxp);
}

static void wait_and_dispatch_events(cops_context_id_t *ctxp)
{
    int res;
    int fd;
    fd_set rfds;

    if (COPS_RC_OK != cops_context_async_fd_get(ctxp, &fd)) {
        fprintf(stderr, "cops_context_async_fd_get failed\n");
    }

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    while (event_cb_expected_data.eventtype) {
        do {
            res = select(fd + 1, &rfds, NULL, NULL, NULL);
        } while (res == -1 && errno == EINTR);

        if (true == cops_context_has_async(ctxp)) {
            if (COPS_RC_OK != cops_context_invoke_callback(ctxp)) {
                fprintf(stderr, "cops_context_invoke_async_callback failed\n");
            }
        }
    }
}

static void read_file(const char *fname, uint8_t **buf, size_t *blen)
{
    FILE *f;
    uint8_t *p = NULL, *p2;
    size_t pos = 0;
    size_t l = 512;
    size_t res;

    f = fopen(fname, "r");

    if (f == NULL) {
        tst_brkm(TBROK, cleanup, "fopen \"%s\" failed", fname);
    }

    for (;;) {
        p2 = realloc(p, l * 2);

        if (p2 == NULL) {
            tst_brkm(TBROK, cleanup, "realloc failed");
        }

        p = p2;
        l *= 2;

        res = fread(p + pos, 1, l - pos, f);

        if (res != (l - pos)) {
            if (ferror(f)) {
                tst_brkm(TBROK, cleanup, "reading \"%s\" failed", fname);
            }

            if (!feof(f)) {
                tst_brkm(TBROK, cleanup, "expected EOF \"%s\"", fname);
            }

            pos += res;
            break;
        }

        pos += res;
    }

    fclose(f);

    *buf = p;
    *blen = pos;
}

static void dump_buf(const char *bname, uint8_t *buf, size_t blen)
{
    fprintf(stderr, "#### %s\n", bname);

    while (blen > 0) {
        size_t n;

        for (n = 0; n < 16; n++) {
            if (n < blen) {
                fprintf(stderr, "%02x ", (int) buf[n]);
            } else {
                fprintf(stderr, "   ");
            }

            if (n == 7) {
                fprintf(stderr, " ");
            }
        }

        fprintf(stderr, " |");

        for (n = 0; n < 16; n++) {
            if (n < blen) {
                if (isprint(buf[n])) {
                    fprintf(stderr, "%c", (int) buf[n]);
                } else {
                    fprintf(stderr, ".");
                }
            }
        }

        fprintf(stderr, "|\n");

        blen -= MIN(blen, 16);
        buf += 16;
    }
}

static void write_default_data(cops_bind_properties_arg_t arg,
                               char *csd_file, bool merge)
{
    cops_auth_data_t auth_data = {
        COPS_AUTH_TYPE_SIMLOCK_KEYS, sizeof(keys), (uint8_t *)&keys
    };
    uint8_t *p;

    memset(&arg, 0, sizeof(arg));
    arg.merge_cops_data = merge;
    arg.imei = &IMEI;
    arg.num_new_auth_data = 1;
    arg.auth_data = &auth_data;
    read_file(csd_file, &p, &arg.cops_data_length);
    arg.cops_data = p;
    COPS_CHK_RC(cops_bind_properties(ctx, &arg), COPS_RC_OK);
    free(p);
}

static void auth_wdefaultd_deauth(cops_bind_properties_arg_t arg,
                                  char *csd_file, bool merge)
{
    /* Authenticate (permanent authentication). */
    {
        cops_auth_data_t auth_data = {
            COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
        };
        COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);
    }

    /* Write the default data. */
    write_default_data(arg, csd_file, merge);

    /* Deauthenticate. */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);
}

static void verify_imsi(test_verify_imsi_params vimsi_params, bool restart,
                        bool restart_again)
{
    uint8_t *in_sipc;
    size_t in_sipc_len;
    uint8_t *out_sipc;
    size_t out_sipc_len;
    uint8_t *exp_out_sipc;
    size_t exp_out_sipc_len;

    /* restart test so that cops_ta may allow autolock to be performed */
    if (restart) {
        old_cops_pid = cops_pid;
        setup();
        modem_counter = ape_counter = 0;
        kill(old_cops_pid, SIGKILL);
        kill(old_cops_pid + 1, SIGKILL);
    }
    modem_counter++;
    RUN_CMD("%s verify_imsi_req %u %s > %s 2> /dev/null", gen_sipc_path,
            modem_counter, vimsi_params.imsi, vimsi_params.req_vimsi_file);

    RUN_CMD("%s verify_imsi_resp %u %s %s %s > %s", gen_sipc_path,
            modem_counter, vimsi_params.exp_errorcode,
            vimsi_params.exp_nbr_active_locks,
            vimsi_params.exp_blocking_lock,
            vimsi_params.exp_resp_vimsi_file);

    if (vimsi_params.gid_included || vimsi_params.include_cnl) {
        char *rsimd_cmd = "-c";

        if (vimsi_params.include_cnl) {
            rsimd_cmd = "-d";
        }

        ape_counter++;
        RUN_CMD("%s read_sim_data_req %s %u > %s",
                gen_sipc_path, rsimd_cmd, ape_counter,
                vimsi_params.req_rsimd_file);
        rsimd_cmd = "-c";

        if (vimsi_params.include_cnl) {
            RUN_CMD("%s read_sim_data_resp %s %u %s %s %s %s %s "
                    "> %s 2> /dev/null",
                    gen_sipc_path, rsimd_cmd, ape_counter,
                    vimsi_params.imsi, vimsi_params.gid_1,
                    vimsi_params.gid_2, vimsi_params.nbr_cnl_entries,
                    vimsi_params.cnl_entries,
                    vimsi_params.resp_rsimd_file);
        } else {
            RUN_CMD("%s read_sim_data_resp %s %u %s %s %s > %s 2> /dev/null",
                    gen_sipc_path, rsimd_cmd, ape_counter,
                    vimsi_params.imsi, vimsi_params.gid_1,
                    vimsi_params.gid_2, vimsi_params.resp_rsimd_file);
        }
    }

    read_file(vimsi_params.req_vimsi_file, &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                   &out_sipc, &out_sipc_len), COPS_RC_OK);

    if (vimsi_params.gid_included || vimsi_params.include_cnl) {
        read_file(vimsi_params.req_rsimd_file, &exp_out_sipc,
                  &exp_out_sipc_len);
        CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
        free(in_sipc);
        free(out_sipc);
        free(exp_out_sipc);

        read_file(vimsi_params.resp_rsimd_file, &in_sipc, &in_sipc_len);
        COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len,
                                       &out_sipc, &out_sipc_len),
                    COPS_RC_OK);
    }

    read_file(vimsi_params.exp_resp_vimsi_file, &exp_out_sipc,
              &exp_out_sipc_len);

    if (vimsi_params.compare_buffers) {
        CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    }

    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    /* By doing restart again the IMSI within COPS is cleared, thus when
       calling bind_properties no implicit verify_imsi will be called.
       Note! card_status is lost after each restart so if a test depends on
       this information, don't restart!
     */

    if (restart_again) {
        /* certain tests require cops to restart yet again to work */
        old_cops_pid = cops_pid;
        setup();
        modem_counter = ape_counter = 0;
        kill(old_cops_pid, SIGKILL);
        kill(old_cops_pid + 1, SIGKILL);
    }
}

static void test_16_get_device_state(void)
{

    cops_auth_data_t auth_data_perm;
    cops_device_state_t device_state;

    /* Set up authentication data for permanent authentication */
    auth_data_perm.auth_type = COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION;
    auth_data_perm.length = 0;
    auth_data_perm.data = NULL;

    /* Make sure that we are deauthenticated */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* Authenticate using permanent authentication */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_perm),
    COPS_RC_OK);

    /* Get the device state */
    COPS_CHK_RC(cops_get_device_state(ctx, &device_state), COPS_RC_OK);

    /* Device state must now be RELAXED otherwise it is an error */
    if (COPS_DEVICE_STATE_RELAXED != device_state) {
        tst_brkm(TFAIL, cleanup, " Device state is not relaxed");
    }

    /* Deauthenticate */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /*
        The following testcase is disabled since permanent authentication
        is always set in MBL and the testcase will fail.
    */

    /* Get the device state */

    /*
    COPS_CHK_RC(cops_get_device_state(ctx, &device_state), COPS_RC_OK);
    */

    /* Device state must now be FULL otherwise it is an error */
    /*
    if (COPS_DEVICE_STATE_FULL != device_state) {
        tst_brkm(TFAIL, cleanup, " Device state is relaxed");
    }
    */

    tst_resm(TINFO, "Get device state test: OK");
}

static void test_17_unlimited_simlock(void)
{
    cops_bind_properties_arg_t arg;
    cops_simlock_control_key_t network_key = { "11111111" };
    cops_simlock_control_key_t wrong_key = { "01111111" };
    cops_simlock_status_t simlock_status;

    char lock_file[PATH_MAX];

    /* create temporary context that only listens for events */
    if (cops_context_create_async(&temp_ctx, &eventcallbacks, evaux, NULL,
                                  NULL) != COPS_RC_OK) {
        tst_brkm(TFAIL, cleanup, "memory allocation failed");
    }

    strcpy(lock_file, template_dir);
    strcat(lock_file, "/unlimited_attempts1.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(arg, lock_file, false);

    /* try to unlock network lock with wrong key */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    /* check that the attempts hasn't decreased */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.attempts_left != 255) {
        tst_brkm(TFAIL, cleanup, "unlimited attempts failed nl");
    }

    /* try to unlock network lock with correct key */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK,
                                    &network_key),
                COPS_RC_OK);
    /* check that lock is unlocked */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_UNLOCKED) {
        tst_brkm(TFAIL, cleanup, "failed to unlock when unlimited attempts");
    }
    if (simlock_status.nl_status.attempts_left != 255) {
        tst_brkm(TFAIL, cleanup, "wrong attempts after unlocking unlimited");
    }

    /* try to unlock network subset with wrong key */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    /* check that the attempts hasn't decreased */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.attempts_left != 255) {
        tst_brkm(TFAIL, cleanup, "unlimited attempts failed nsl");
    }

    /* try to unlock ESL with wrong key */
    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                  COPS_SIMLOCK_LOCK_TYPE_FLEXIBLE_ESL_LOCK,
                                    &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);
    /* check that the attempts hasn't decreased */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.esll_status.attempts_left != 255) {
        tst_brkm(TFAIL, cleanup, "unlimited attempts failed sp");
    }

    tst_resm(TINFO, "SIMLOCK unlimited attempts OK");
}

static void test_18_rolock(void)
{
#ifdef BOOTPARTTEST_ENABLE
    uint8_t *resultbuffer;
    size_t resultlen;
    uint8_t *actualbuffer;
    size_t actuallen;

    /* Locking fails when already temporarily locked */
    RUN_CMD("echo -n pwr_ro >bootpartition_sysfs_rolock.bin");
    COPS_CHK_RC(cops_lock_bootpartition(ctx), COPS_RC_STORAGE_ERROR);

    /* Lock the partition */
    RUN_CMD("echo -n rw >bootpartition_sysfs_rolock.bin");
    COPS_CHK_RC(cops_lock_bootpartition(ctx), COPS_RC_OK);

    /* Second call should be ok */
    COPS_CHK_RC(cops_lock_bootpartition(ctx), COPS_RC_OK);

    read_file("bootpartition_sysfs_rolock.bin", &actualbuffer, &actuallen);
    RUN_CMD("echo -n perm_ro >bootpartition_sysfs_rolock_result.bin");
    read_file("bootpartition_sysfs_rolock_result.bin", &resultbuffer,
              &resultlen);

    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    free(actualbuffer);
    free(resultbuffer);

    tst_resm(TINFO, "Lock bootpartition: OK");
#endif
}

static void test_19_init_arb_table(void)
{
    cops_auth_data_t auth_data_perm = {
        COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
    };
    uint8_t modelid[2] = {0xff, 0xff};
    cops_init_arb_table_args_t arb_table_args = {
        COPS_ARB_DATA_TYPE_MODELID, sizeof(modelid), (uint8_t *)modelid
    };
    uint8_t *resultbuffer;
    size_t resultlen;
    uint8_t *actualbuffer;
    size_t actuallen;
    struct stat statbuf;

    RUN_CMD("cp %s/arbtest1_arb1.bin .", template_dir);
    RUN_CMD("cp %s/arbtest1_arb2.bin .", template_dir);
    RUN_CMD("cp %s/arbtest2_arb1.bin .", template_dir);
    RUN_CMD("cp %s/arbtest2_arb2.bin .", template_dir);

    /* Make sure that we are deauthenticated */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    COPS_CHK_RC(cops_init_arb_table(ctx, &arb_table_args),
                COPS_RC_NOT_AUTHENTICATED_ERROR);

    /* Authenticate using permanent authentication */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_perm),
                COPS_RC_OK);

    COPS_CHK_RC(cops_init_arb_table(ctx, &arb_table_args), COPS_RC_OK);

    /* This tests if the file exists and return ok if it doesn't */
    COPS_CHK_RC(stat("ARBTBL1TOC", &statbuf), -1);
    COPS_CHK_RC(stat("ARBTBL2TOC", &statbuf), -1);

    modelid[0] = 0x12;
    modelid[1] = 0x34;

    COPS_CHK_RC(cops_init_arb_table(ctx, &arb_table_args), COPS_RC_OK);

    /* check table1 */
    read_file("ARBTBL1TOC", &actualbuffer, &actuallen);
    read_file("arbtest1_arb1.bin", &resultbuffer, &resultlen);
    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    free(actualbuffer);
    free(resultbuffer);

    /* check table2 */
    read_file("ARBTBL2TOC", &actualbuffer, &actuallen);
    read_file("arbtest1_arb2.bin", &resultbuffer, &resultlen);
    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    free(actualbuffer);
    free(resultbuffer);

    modelid[0] = 0x56;
    modelid[1] = 0x78;

    COPS_CHK_RC(cops_init_arb_table(ctx, &arb_table_args), COPS_RC_OK);

    /* check table1 */
    read_file("ARBTBL1TOC", &actualbuffer, &actuallen);
    read_file("arbtest2_arb1.bin", &resultbuffer, &resultlen);
    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    free(actualbuffer);
    free(resultbuffer);

    /* check table2 */
    read_file("ARBTBL2TOC", &actualbuffer, &actuallen);
    read_file("arbtest2_arb2.bin", &resultbuffer, &resultlen);
    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    free(actualbuffer);
    free(resultbuffer);

    /* Make sure that we are deauthenticated */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    tst_resm(TINFO, "ARB table init OK");
}

static void test_20_write_sec_info(void)
{
#ifdef BOOTPARTTEST_ENABLE
    cops_auth_data_t auth_data_perm = {
        COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
    };
    cops_write_secprofile_args_t secprofiledata = {
        1, 0xAA, 0, NULL, COPS_SEC_PROFILE_DATA_NONE, 0, NULL
    };
    uint8_t *resultbuffer;
    size_t resultlen;
    uint8_t *actualbuffer;
    size_t actuallen;
    uint8_t onehash[32] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                           0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                           0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                           0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    uint8_t twohashes[32*2] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                               0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                               0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                               0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                               0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                               0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                               0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                               0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};

    RUN_CMD("cp %s/bootpartition_secprofiledata_*.bin .", template_dir);
    RUN_CMD("cp %s/bootpartition_result.bin .", template_dir);

    /* Make sure write lock is open */
    RUN_CMD("echo -n rw >bootpartition_sysfs_rolock.bin");

    /* Setup bootpartition with defined content */
    RUN_CMD("dd if=bootpartition_result.bin of=bootpartition.bin 2>/dev/null");

    /* Make sure that we are deauthenticated */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    /* must be authenticated */
    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata),
                COPS_RC_NOT_AUTHENTICATED_ERROR);

    /* Authenticate using permanent authentication */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_perm),
                COPS_RC_OK);

    /* different illegal combinations of parameters */
    secprofiledata.hashlist_len = 0;
    secprofiledata.hashlist = onehash;

    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata),
                COPS_RC_ARGUMENT_ERROR);

    secprofiledata.hashlist_len = 4;
    secprofiledata.hashlist = NULL;

    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata),
                COPS_RC_ARGUMENT_ERROR);

    secprofiledata.data_length = 4;
    secprofiledata.data = 0;

    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata),
                COPS_RC_ARGUMENT_ERROR);

    secprofiledata.data_length = 0;
    secprofiledata.data = onehash;

    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata),
                COPS_RC_ARGUMENT_ERROR);

    secprofiledata.sec_profile_data_type = 1;

    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata),
                COPS_RC_ARGUMENT_ERROR);

    /* setup proper info */
    secprofiledata.sec_profile_data_type = COPS_SEC_PROFILE_DATA_NONE;
    secprofiledata.data_length = 0;
    secprofiledata.data = NULL;

    /* providing no hashes makes issw fill in default values */
    secprofiledata.hashlist = NULL;
    secprofiledata.hashlist_len = 0;

    /* Writing when temporarily locked should fail */
    RUN_CMD("echo -n pwr_ro >bootpartition_sysfs_rolock.bin");
    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata),
                COPS_RC_STORAGE_ERROR);

    /* Writing when permanently locked should fail */
    RUN_CMD("echo -n perm_ro >bootpartition_sysfs_rolock.bin");
    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata),
                COPS_RC_STORAGE_ERROR);

    /* Open write lock to continue testing*/
    RUN_CMD("echo -n rw >bootpartition_sysfs_rolock.bin");

    /* still no hash provided*/
    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata), COPS_RC_OK);

    read_file("bootpartition.bin", &actualbuffer, &actuallen);
    read_file("bootpartition_secprofiledata_1.bin", &resultbuffer, &resultlen);

    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    free(actualbuffer);
    free(resultbuffer);

    /* Calling this a second time should give us the same result */
    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata), COPS_RC_OK);

    read_file("bootpartition.bin", &actualbuffer, &actuallen);
    read_file("bootpartition_secprofiledata_1.bin", &resultbuffer, &resultlen);

    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    free(actualbuffer);
    free(resultbuffer);

    /* clear content */
    RUN_CMD("dd bs=256 count=1 if=/dev/zero of=bootpartition.bin 2>/dev/null");

    /* one hash provided */
    secprofiledata.hashlist = onehash;
    secprofiledata.hashlist_len = sizeof(onehash);

    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata), COPS_RC_OK);

    read_file("bootpartition.bin", &actualbuffer, &actuallen);
    read_file("bootpartition_secprofiledata_2.bin", &resultbuffer, &resultlen);

    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    free(actualbuffer);
    free(resultbuffer);

    /* two hashes provided */
    secprofiledata.hashlist = twohashes;
    secprofiledata.hashlist_len = sizeof(twohashes);

    COPS_CHK_RC(cops_write_secprofile(ctx, &secprofiledata), COPS_RC_OK);

    read_file("bootpartition.bin", &actualbuffer, &actuallen);
    read_file("bootpartition_secprofiledata_3.bin", &resultbuffer, &resultlen);

    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    free(actualbuffer);
    free(resultbuffer);

    /* check that the rwflag is ok */
    read_file("bootpartition_sysfs_rwflag.bin", &actualbuffer, &actuallen);
    RUN_CMD("echo -n 0 >bootpartition_sysfs_rwflag_result.bin");
    read_file("bootpartition_sysfs_rwflag_result.bin", &resultbuffer,
              &resultlen);

    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    free(actualbuffer);
    free(resultbuffer);

    /* At this stage the rolock should be untouched */
    read_file("bootpartition_sysfs_rolock.bin", &actualbuffer, &actuallen);
    RUN_CMD("echo -n rw >bootpartition_sysfs_rolock_result.bin");
    read_file("bootpartition_sysfs_rolock_result.bin", &resultbuffer,
              &resultlen);

    CMP_BUF(resultbuffer, resultlen, actualbuffer, actuallen);
    free(actualbuffer);
    free(resultbuffer);

    tst_resm(TINFO, "Write security profile OK");
#endif
}

static void test_21_sim_change_key(void)
{
    cops_bind_properties_arg_t bind_properties_arg;

    cops_simlock_control_key_t short_key = {"0000"};
    cops_simlock_control_key_t bad_key = {"123456789"};
    cops_simlock_control_key_t default_key = {"000000"};
    char *datafile = "";
    cops_simlock_status_t simlock_status;

    datafile = "sim_personalization.csd";

    RUN_CMD("cp %s/%s .", template_dir, datafile);
    auth_wdefaultd_deauth(bind_properties_arg, datafile, false);

    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx,
                 &default_key,
                 &short_key),
                COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx,
                 &short_key,
                 &default_key),
                COPS_RC_ARGUMENT_ERROR);

    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx,
                 &bad_key,
                 &default_key),
                COPS_RC_INVALID_SIMLOCK_KEY);

    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx,
                 &default_key,
                 &bad_key),
                COPS_RC_OK);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.siml_status.lock_setting !=
            COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, "Wrong lock mode for (U)SIM Lock, %u",
                 simlock_status.siml_status.lock_setting);
    }

    COPS_CHK_RC(cops_simlock_unlock(ctx,
                                    COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK,
                                    &bad_key), COPS_RC_OK);

    /* Check that (U)SIM Lock is unlocked */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.siml_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_UNLOCKED) {
        tst_brkm(TFAIL, cleanup, "failed to unlock (U)SIM simlock");
    }

    tst_resm(TINFO, "Change (U)SIM control key: OK");

}

static void test_22_simlock_lock(void)
{
    cops_bind_properties_arg_t arg;
    struct cops_simlock_lock_arg simlockarg;
    struct cops_simlock_explicitlockdata explicitdata;
    test_verify_imsi_params vimsi_params;
    uint8_t explicit_imsi[] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                               0x9, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5};
    uint8_t cnl[] = {
        /*                                     |MCC||MNC |NS| GID1  GID2 */
        0x11, 0xF1, 0x11, 0x11, 0x11, 0x11, /* 1 1 1 1 1 1 1| 0x11, 0x11 */
        0x22, 0xF2, 0x22, 0x22, 0x22, 0x22, /* 2 2 2 2 2 2 2| 0x22, 0x22 */
        0x21, 0xF3, 0x54, 0x21, 0x50, 0x70, /* 1 2 3 4 5 1 2| 0x05, 0x07 */
        0x42, 0xF0, 0x99, 0x21, 0x43, 0x87, /* 2 4 0 9 9 1 2| 0x34, 0x78 */
        0x33, 0xF3, 0x33, 0x33, 0x33, 0x33, /* 3 3 3 3 3 3 3| 0x33, 0x33 */
        0x32, 0xF4, 0x76, 0x65, 0x80, 0x90, /* 2 3 4 6 7 5 6| 0x08, 0x09 */
        0x54, 0xF6, 0x98, 0x54, 0x70, 0x80, /* 4 5 6 8 9 4 5| 0x07, 0x08 */
        0x55, 0xF5, 0x55, 0x55, 0x55, 0x55, /* 5 5 5 5 5 5 5| 0x55, 0x55 */
     };

    uint8_t explicit_nl_settings[] = { 0x05, 0x04, 0x60, 0x50, 0x11,
                                       0x02, 0x08, 0x00 };
    uint8_t explicit_cl_settings[] = { 0x35, 0xc7, 0x07, 0xa8, 0x11, 0x29,
                                       0x0f, 0x03, 0x04, 0x02, 0x03, 0x04,
                                       0x09, 0x08, 0x01, 0x02, 0x03, 0x05,
                                       0x06, 0x07, 0x08, 0x4d };

    cops_simlock_control_key_t wrong_key = { "01111111" };
    cops_simlock_status_t simlock_status;
    cops_auth_data_t auth_data = {
        COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
    };
    int i;
    char *rsimd_cmd = "-c";
    uint8_t *in_sipc;
    size_t in_sipc_len;
    uint8_t *out_sipc;
    size_t out_sipc_len;
    uint8_t *exp_out_sipc;
    size_t exp_out_sipc_len;

    /* Create default data with no locks set */
    RUN_CMD("cp %s/%s .", template_dir, "unlocked.csd");
    auth_wdefaultd_deauth(arg, "unlocked.csd", false);

    /* no data, should fail */
    COPS_CHK_RC(cops_simlock_lock(ctx, NULL), COPS_RC_ARGUMENT_ERROR);

    memset(&simlockarg, 0, sizeof(simlockarg));

    /* explicit setting with length but no data, should fail */
    explicitdata.length = 1;
    explicitdata.data = NULL;
    simlockarg.explicitlockdata = &explicitdata;
    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_ARGUMENT_ERROR);

    /* explicit setting data set but no length, should fail */
    explicitdata.length = 0;
    explicitdata.data = (uint8_t *)1; /* whatever */
    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_ARGUMENT_ERROR);

    /* cleanup */
    memset(&simlockarg.explicitlockdata, 0,
           sizeof(simlockarg.explicitlockdata));

    /* not authorized and wrong key provided, should fail */
    simlockarg.lockmode = COPS_SIMLOCK_LOCK_MODE_AUTOLOCK;
    simlockarg.lockdata.locktype = COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK;

    memcpy(&simlockarg.lockdata.controlkey, &wrong_key,
           sizeof(cops_simlock_control_key_t));

    simlockarg.lockdata.update_lockdef = true;
    simlockarg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_SETLOCKDATA |
        COPS_SIMLOCK_LOCKOP_SETLOCKSETTING | COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA;

    /* get the actual lock definition of the lock and set the new lock
     *  definition to something else
     */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);
    simlockarg.lockdata.lockdef = simlock_status.nl_status.lock_definition +
        0x60;

    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg),
                COPS_RC_NOT_AUTHENTICATED_ERROR);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_UNLOCKED) {
        tst_brkm(TFAIL, cleanup, " Lock is locked but shouldn't be");
    }

    if (simlock_status.nl_status.lock_definition ==
        simlockarg.lockdata.lockdef) {
        tst_brkm(TFAIL, cleanup,
                 " Lock definition updated even though it shouldn't be");
    }
    /* reset the lock definition again */
    simlockarg.lockdata.lockdef = 0x1f;

    /* authenticate and wrong key (this also sets a new lockdefinition and sets
     *   autolock IMSI), ok
     */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);

    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_OK);

    tst_resm(TINFO, "setting same (already locked) lock again, fail");
    /* deauthenticate and give wrong key to see that auth error will
       not come before lock error */
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);
    memcpy(&simlockarg.lockdata.controlkey, &wrong_key,
           sizeof(cops_simlock_control_key_t));
    /* setting same (already locked) lock again, fail */
    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_LOCKING_ERROR);


    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_ENABLED) {
        tst_brkm(TFAIL, cleanup, " Lock not set to autlock but should be");
    }

    if (simlock_status.nl_status.lock_definition !=
        simlockarg.lockdata.lockdef) {
        tst_brkm(TFAIL, cleanup, " Lock definition wasn't updated");
    }

   COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_ENABLED) {
        tst_brkm(TFAIL, cleanup, " The lock is locked  but should still be"
                 "autolock");
    }
    tst_resm(TINFO, "autolock not allowed since no restart");
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_3vimsi_resp.simlocktest";
    vimsi_params.imsi = "111182545678901";
    /* nbr_active_locks will not be given any value since we are returning
     *  with an error
     */
    vimsi_params.exp_nbr_active_locks = "0";
    vimsi_params.exp_errorcode = "23";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, false, false);


    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_ENABLED) {
        tst_brkm(TFAIL, cleanup, " The lock is locked after verify imsi but"
                 "should still be autolock");
    }

    tst_resm(TINFO, "do the autolock");
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_3vimsi_resp.simlocktest";
    vimsi_params.imsi = "111182545678901";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " The lock is not autolocked");
    }

    COPS_CHK_RC(cops_simlock_verify_control_keys(ctx, &keys), COPS_RC_OK);

    simlockarg.lockdata.update_lockdef = false;
    tst_resm(TINFO, "not authorized and correct key");
    /* not authorized and correct key (sets autlock to cnl), ok */
    simlockarg.lockmode = COPS_SIMLOCK_LOCK_MODE_AUTOLOCK_TO_CNL;
    simlockarg.lockdata.locktype = COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK;
    memcpy(&simlockarg.lockdata.controlkey, &keys.nsl_key,
           sizeof(cops_simlock_control_key_t));

    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_OK);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_TO_CNL_ENABLED) {
        tst_brkm(TFAIL, cleanup, "Lock not locked but should be");
    }

    /* reset the locks */
    auth_wdefaultd_deauth(arg, "unlocked.csd", false);
    tst_resm(TINFO, "test explicit lock with IMSI values and lock op");
    /* test explicit lock with IMSI values and lock op */
    memset(&explicitdata, 0, sizeof(explicitdata));
    memset(&explicitdata.imsi, 0xFF, sizeof(explicitdata.imsi));
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data), COPS_RC_OK);

    simlockarg.lockmode = COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_IMSI;
    simlockarg.lockdata.locktype = COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK;
    tst_resm(TINFO, "only store/append data, don't activate lock.");
    simlockarg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_SETLOCKDATA;
    simlockarg.explicitlockdata = &explicitdata;

    for (i = 0; i < 5; i++) {
        memcpy(explicitdata.imsi.data, &explicit_imsi, i);
        COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg),
                    COPS_RC_ARGUMENT_ERROR);
    }

    for (i = 5; i < COPS_NUM_IMSI_DIGITS; i++) {
        memcpy(explicitdata.imsi.data, &explicit_imsi, i);
        explicitdata.gid1 = 33;
        explicitdata.gid2 = 44;
        COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_OK);
    }
    COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);
    tst_resm(TINFO, "don't authenticate, wrong key");
    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg),
                COPS_RC_NOT_AUTHENTICATED_ERROR);

    simlockarg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_SETLOCKSETTING |
        COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA;
    tst_resm(TINFO, "don't authenticate, correct key");
        memcpy(&simlockarg.lockdata.controlkey, &keys.cl_key,
           sizeof(cops_simlock_control_key_t));

    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_OK);

    /* since lock data cleared, lock should be set but no valid imsi */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " Lock not locked but should be");
    }

    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "123456789012345";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0x03";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.simlocktest";
    vimsi_params.resp_rsimd_file = "rsimd_resp.simlocktest";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "33";
    vimsi_params.gid_2 = "44";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* reset the locks */
    auth_wdefaultd_deauth(arg, "unlocked.csd", false);

    tst_resm(TINFO, "Autolock with GID's");
    memset(&simlockarg.explicitlockdata, 0,
           sizeof(simlockarg.explicitlockdata));
    simlockarg.lockmode = COPS_SIMLOCK_LOCK_MODE_AUTOLOCK;
    simlockarg.lockdata.locktype = COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK;
    memcpy(&simlockarg.lockdata.controlkey, &keys.cl_key,
           sizeof(cops_simlock_control_key_t));
    simlockarg.lockdata.update_lockdef = false;
    simlockarg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_SETLOCKDATA |
        COPS_SIMLOCK_LOCKOP_SETLOCKSETTING | COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA;

    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg),
                COPS_RC_OK);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_ENABLED) {
        tst_brkm(TFAIL, cleanup, " Lock is not set to autolock but should be");
    }

    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "123456789012345";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.simlocktest";
    vimsi_params.resp_rsimd_file = "rsimd_resp.simlocktest";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "33";
    vimsi_params.gid_2 = "44";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " Lock is not locked but should be");
    }

   /* reset the locks */
    auth_wdefaultd_deauth(arg, "unlocked.csd", false);

    tst_resm(TINFO, "normallock to IMSI");
    /* normallock to IMSI */
    /* test normallock with sipc messages */
    modem_counter++;
    tst_resm(TINFO, "generate lock req");
    RUN_CMD("%s simlock_normlock_req %u 2 3 44444444 5 > sl_req.1",
            gen_sipc_path, modem_counter);

    ape_counter++;
    tst_resm(TINFO, "generate simdata req");
    RUN_CMD("%s read_sim_data_daemon_req -c %u > %s",
                gen_sipc_path, ape_counter,
                "simdata_req.1");

    tst_resm(TINFO, "generate simdata resp");
    RUN_CMD("%s read_sim_data_daemon_resp -c %d 123456789012345 43 56 "
            "> simdata_resp.1 ",
                    gen_sipc_path, ape_counter);

    tst_resm(TINFO, "generate lock resp");
    RUN_CMD("%s simlock_normlock_resp %u 0 > exp_sl_resp.1",
            gen_sipc_path, modem_counter);

    read_file("sl_req.1", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len, &out_sipc,
                                   &out_sipc_len), COPS_RC_OK);
    read_file("simdata_req.1", &exp_out_sipc,
                  &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    read_file("simdata_resp.1", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len, &out_sipc,
                                   &out_sipc_len), COPS_RC_OK);

    read_file("exp_sl_resp.1", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);

    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " Lock not locked but should be");
    }
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "123456789012335";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.simlocktest";
    vimsi_params.resp_rsimd_file = "rsimd_resp.simlocktest";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "43";
    vimsi_params.gid_2 = "56";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "223456789012345";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0x03";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.simlocktest";
    vimsi_params.resp_rsimd_file = "rsimd_resp.simlocktest";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "43";
    vimsi_params.gid_2 = "56";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);
    tst_resm(TINFO, "reset to continue test");

    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "123456789012345";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0x03";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.simlocktest";
    vimsi_params.resp_rsimd_file = "rsimd_resp.simlocktest";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "43";
    vimsi_params.gid_2 = "55";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* reset the locks */
    auth_wdefaultd_deauth(arg, "unlocked.csd", false);


    tst_resm(TINFO, "explicit lock to imsi");
    simlockarg.lockmode = COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_IMSI;
    simlockarg.lockdata.locktype = COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK;
    simlockarg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA
        | COPS_SIMLOCK_LOCKOP_SETLOCKSETTING
        | COPS_SIMLOCK_LOCKOP_SETLOCKDATA;
    memcpy(&simlockarg.lockdata.controlkey, &keys.cl_key,
           sizeof(cops_simlock_control_key_t));

    memcpy(explicitdata.imsi.data, explicit_imsi,
           sizeof(explicitdata.imsi.data));
    explicitdata.gid1 = 1;
    explicitdata.gid2 = 2;
    explicitdata.length = 0;
    explicitdata.data = 0;
    simlockarg.explicitlockdata = &explicitdata;


    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_OK);


    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " Lock not locked but should be");
    }
    /* test with correct imsi */
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "123456789012345";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.simlocktest";
    vimsi_params.resp_rsimd_file = "rsimd_resp.simlocktest";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "1";
    vimsi_params.gid_2 = "2";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* test with wrong imsi */
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "123446789012345";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0x03";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.simlocktest";
    vimsi_params.resp_rsimd_file = "rsimd_resp.simlocktest";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "1";
    vimsi_params.gid_2 = "2";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* check that it isn't possible to lock an already locked lock */
    simlockarg.lockmode = COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_IMSI;
    simlockarg.lockdata.locktype = COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK;
    simlockarg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA;
    memcpy(&simlockarg.lockdata.controlkey, &keys.cl_key,
           sizeof(cops_simlock_control_key_t));

    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_LOCKING_ERROR);

    /* reset the locks */
    auth_wdefaultd_deauth(arg, "unlocked.csd", false);

    tst_resm(TINFO, "lock to lock settings");
    simlockarg.lockmode = COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_SETTINGS_DATA;
    simlockarg.lockdata.locktype = COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK;
    simlockarg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_SETLOCKSETTING |
        COPS_SIMLOCK_LOCKOP_SETLOCKDATA | COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA ;
    memcpy(&simlockarg.lockdata.controlkey, &keys.nl_key,
           sizeof(cops_simlock_control_key_t));

    explicitdata.data = (uint8_t *)&explicit_nl_settings;
    explicitdata.length = sizeof(explicit_nl_settings);

    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_OK);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " Lock not locked but should be");
    }

    /* Correct imsi */
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "230335678901234";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /*wrong imsi */
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "24133111111111";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0x00";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* reset the locks */
    auth_wdefaultd_deauth(arg, "unlocked.csd", false);

    tst_resm(TINFO, "lock to lock settings, with GID's");
    simlockarg.lockmode = COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_SETTINGS_DATA;
    simlockarg.lockdata.locktype = COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK;
    simlockarg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_SETLOCKSETTING |
        COPS_SIMLOCK_LOCKOP_SETLOCKDATA | COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA ;
    memcpy(&simlockarg.lockdata.controlkey, &keys.cl_key,
           sizeof(cops_simlock_control_key_t));

    explicitdata.data = (uint8_t *)&explicit_cl_settings;
    explicitdata.length = sizeof(explicit_cl_settings);

    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_OK);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " Lock not locked but should be");
    }

    /* correct imsi, correct GID's */
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "003705678901234";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.221";
    vimsi_params.resp_rsimd_file = "exp_rsimd_resp.221";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "4";
    vimsi_params.gid_2 = "77";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* correct IMSI, wrong GID's */
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "003705678901234";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0x03";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.222";
    vimsi_params.resp_rsimd_file = "exp_rsimd_resp.222";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "77";
    vimsi_params.gid_2 = "4";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* wrong IMSI, correct GID's */
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "00374111111111";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0x03";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.23";
    vimsi_params.resp_rsimd_file = "exp_rsimd_resp.223";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "4";
    vimsi_params.gid_2 = "77";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* reset the locks */
    auth_wdefaultd_deauth(arg, "unlocked.csd", false);

    tst_resm(TINFO, "explicit lock to cnl");
    simlockarg.lockmode = COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_CNL;
    simlockarg.lockdata.locktype = COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK;
    simlockarg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_SETLOCKSETTING |
        COPS_SIMLOCK_LOCKOP_SETLOCKDATA | COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA ;
    explicitdata.data = (uint8_t *)&cnl;
    explicitdata.length = sizeof(cnl);
    memcpy(&simlockarg.lockdata.controlkey, &keys.nsl_key,
           sizeof(cops_simlock_control_key_t));


    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_OK);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nsl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " Lock not locked but should be");
    }

    /* correct imsi */
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "456894599999999";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* wrong imsi */
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "535670999999999";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0x01";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* reset the locks */
    auth_wdefaultd_deauth(arg, "unlocked.csd", false);

    tst_resm(TINFO, "explicit lock to cnl, with GID's");
    simlockarg.lockmode = COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_CNL;
    simlockarg.lockdata.locktype = COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK;
    simlockarg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_SETLOCKSETTING |
        COPS_SIMLOCK_LOCKOP_SETLOCKDATA | COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA ;
    explicitdata.data = (uint8_t *)&cnl;
    explicitdata.length = sizeof(cnl);
    memcpy(&simlockarg.lockdata.controlkey, &keys.cl_key,
           sizeof(cops_simlock_control_key_t));


    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_OK);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.cl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " Lock not locked but should be");
    }
    /* correct imsi, correct GID */
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "240991299999999";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.224";
    vimsi_params.resp_rsimd_file = "exp_rsimd_resp.224";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "0x34";
    vimsi_params.gid_2 = "0x78";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* correct imsi but wrong GID */
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "240991299999999";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0x03";
    vimsi_params.req_rsimd_file = "exp_rsimd_req.225";
    vimsi_params.resp_rsimd_file = "exp_rsimd_resp.225";
    vimsi_params.gid_included = true;
    vimsi_params.gid_1 = "7";
    vimsi_params.gid_2 = "8";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* reset the locks */
    auth_wdefaultd_deauth(arg, "unlocked.csd", false);

    /* test normallock with sipc messages */
    simlockarg.explicitlockdata = 0;
    modem_counter++;
    tst_resm(TINFO, "generate lock req");
    rsimd_cmd = "-d";
    RUN_CMD("%s simlock_normlock_req %u 3 0 11111111 7 > sl_req.2",
            gen_sipc_path, modem_counter);

    ape_counter++;
    rsimd_cmd = "-d";
    tst_resm(TINFO, "generate simdata req");
    RUN_CMD("%s read_sim_data_daemon_req %s %u > %s",
                gen_sipc_path, rsimd_cmd, ape_counter,
                "simdata_req.2");

    tst_resm(TINFO, "generate simdata resp");

    RUN_CMD("%s read_sim_data_daemon_resp -c %d 435766789012345 43 "
            "56 2 0x34 0xF5 0x67 0x89 0xff 0xff "
            "0x22 0xF3 0x22 0x22 0xff 0xff > simdata_resp.2",
            gen_sipc_path, ape_counter);

    tst_resm(TINFO, "generate lock resp");
    RUN_CMD("%s simlock_normlock_resp %u 0 > exp_sl_resp.2",
            gen_sipc_path, modem_counter);

    read_file("sl_req.2", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len, &out_sipc,
                                   &out_sipc_len), COPS_RC_OK);
    read_file("simdata_req.2", &exp_out_sipc,
                  &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);
    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    read_file("simdata_resp.2", &in_sipc, &in_sipc_len);
    COPS_CHK_RC(cops_modem_sipc_mx(ctx, in_sipc, in_sipc_len, &out_sipc,
                                   &out_sipc_len), COPS_RC_OK);

    read_file("exp_sl_resp.2", &exp_out_sipc, &exp_out_sipc_len);

    CMP_BUF(exp_out_sipc, exp_out_sipc_len, out_sipc, out_sipc_len);

    free(in_sipc);
    free(out_sipc);
    free(exp_out_sipc);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.nl_status.lock_setting !=
        COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
        tst_brkm(TFAIL, cleanup, " Lock not locked but should be");
    }
    tst_resm(TINFO, "check sim");
    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "223222999999999";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "0";
    vimsi_params.exp_blocking_lock = "0xff";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    vimsi_params.req_vimsi_file = "vimsi_req.simlocktest";
    vimsi_params.exp_resp_vimsi_file = "exp_vimsi_resp.simlocktest";
    vimsi_params.imsi = "112349999999999";
    vimsi_params.exp_nbr_active_locks = "1";
    vimsi_params.exp_errorcode = "16";
    vimsi_params.exp_blocking_lock = "0x0";
    vimsi_params.req_rsimd_file = "";
    vimsi_params.resp_rsimd_file = "";
    vimsi_params.gid_included = false;
    vimsi_params.gid_1 = "";
    vimsi_params.gid_2 = "";
    vimsi_params.include_cnl = false;
    vimsi_params.nbr_cnl_entries = "";
    vimsi_params.cnl_entries = "";
    vimsi_params.compare_buffers = true;
    verify_imsi(vimsi_params, true, false);

    /* attempts tests */
    /* reset the locks */
    auth_wdefaultd_deauth(arg, "unlocked.csd", false);

    simlockarg.lockmode = COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_SETTINGS_DATA;
    simlockarg.lockdata.locktype = COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK;
    simlockarg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_SETLOCKSETTING |
        COPS_SIMLOCK_LOCKOP_SETLOCKDATA | COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA ;
    memcpy(&simlockarg.lockdata.controlkey, &keys.nl_key,
           sizeof(cops_simlock_control_key_t));
    simlockarg.explicitlockdata = &explicitdata;
    explicitdata.data = (uint8_t *) &explicit_nl_settings;
    explicitdata.length = sizeof(explicit_nl_settings);

    COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_OK);

    /* get rid of all attemtps except one */
    {
        uint32_t i;

        for (i = 0; i < 2; i++) {
            COPS_CHK_RC(cops_simlock_unlock
                (ctx, COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK, &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);
        }
        COPS_CHK_RC(cops_simlock_unlock
            (ctx, COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK, &keys.nl_key),
                COPS_RC_OK);

        memcpy(&simlockarg.lockdata.controlkey, &wrong_key,
           sizeof(cops_simlock_control_key_t));
        /* no attempts limitation on simlock lock */
        for (i = 0; i < 5; i++) {
            COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg),
                        COPS_RC_NOT_AUTHENTICATED_ERROR);
        }
        memcpy(&simlockarg.lockdata.controlkey, &keys.nl_key,
           sizeof(cops_simlock_control_key_t));
        COPS_CHK_RC(cops_simlock_lock(ctx, &simlockarg), COPS_RC_OK);

        /* now the attempts should been reset so new attemts are allowed */
        for (i = 0; i < 3; i++) {
            COPS_CHK_RC(cops_simlock_unlock
                (ctx, COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK, &wrong_key),
                COPS_RC_INVALID_SIMLOCK_KEY);
        }
        COPS_CHK_RC(cops_simlock_unlock
            (ctx, COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK, &wrong_key),
                COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT);
        /* since the attempts are gone, correct key should not help */
        COPS_CHK_RC(cops_simlock_unlock
            (ctx, COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK, &keys.nl_key),
                COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT);
    }
    tst_resm(TINFO, "SIMLock lock: OK");
}

/**
 * Internal OTP help functions.
 */

/**
 * @Brief: returns true if ECC is set in OTP metadata.
 */
static bool get_ecc(uint32_t *otp_data)
{
    if (otp_data != NULL) {
        return (*otp_data & 0x80000000) != 0;
    } else {
        return 0;
    }
}

static void set_ecc(uint32_t *otp_data)
{
    if (otp_data != NULL) {
        *otp_data |= 0x80000000;
    }
}

/**
 * @Brief: returns true if write lock is set in OTP metadata.
 */
static bool get_write_lock(uint32_t *otp_data)
{
    if (otp_data != NULL) {
        return (*otp_data & 0x40000000) != 0;
    } else {
        return 0;
    }
}

static void set_write_lock(uint32_t *otp_data)
{
    if (otp_data != NULL) {
        *otp_data |= 0x40000000;
    }
}

/**
 * Brief: Check OTP variant by reading die id rows.
 */
static uint32_t test_otp_detect_variant()
{
#define EMULATED_ID_ROW53 0x4C554D45
#define EMULATED_ID_ROW54 0x454C4946
    uint32_t ret_code = 0;
    char *file_name = "read_die_id.dat";
    size_t size = 0;
    uint8_t *data = NULL;
    uint32_t *otp_data_p = NULL;

    read_file(file_name, &data, &size);
    otp_data_p = (uint32_t *)data;
    COPS_CHK_RC(cops_read_otp(ctx, (uint32_t *)data, size), COPS_RC_OK);

    otp_data_p++; /* Move pointer to first die id data. */
    if (*otp_data_p == EMULATED_ID_ROW53) {
        ret_code = COPS_RAM_OTP;
        otp_data_p += 2; /* Move pointer to next Die id data. */
        if (*otp_data_p == EMULATED_ID_ROW54) {
            ret_code = COPS_FILE_OTP;
        }
    } else {
        ret_code = COPS_REAL_OTP;
    }

    free(data);
    return ret_code;
}

/**
 * Brief: Write row 28, 29.
 * init_write.dat Sets a value for row 28 in redundancy mode
 * Write data to row 29 in ECC mode and perform lock.
 * Should return OK
 */
static void test_otp_init_write(void)
{
    char *file_name = "init_write.dat";
    size_t size = 0;
    uint8_t *data = NULL;
    uint32_t *otp_data = NULL;
    uint32_t *otp_data_p = NULL;

    read_file(file_name, &data, &size);
    COPS_CHK_RC(cops_write_otp(ctx, (uint32_t *)data, size), COPS_RC_OK);

    /* Read OTP and verify ECC, lock_set and data. */
    otp_data = malloc(size);
    memset(otp_data, 0, size);
    otp_data_p = otp_data;

    *otp_data_p = 28;
    otp_data_p += 2;  /* Move to next meta element */
    *otp_data_p = 29;
    otp_data_p = otp_data;

    COPS_CHK_RC(cops_read_otp(ctx, otp_data, size), COPS_RC_OK);

    if (get_ecc(otp_data_p)) {
        tst_brkm(TFAIL, cleanup, "ECC set for OTP[28].");
    }
    if (get_write_lock(otp_data_p)) {
        tst_brkm(TFAIL, cleanup, "Write lock set for OTP[28].");
    }

    otp_data_p++; /* Move to next data element. */
    if (*otp_data_p == 0) {
        tst_brkm(TFAIL, cleanup, "Unexpected empty OTP[28].");
    }

    otp_data_p++; /* Move to next meta element. */
    if (!get_ecc(otp_data_p)) {
        tst_brkm(TFAIL, cleanup, "No ECC set for OTP[29].");
    }
    if (!get_write_lock(otp_data_p)) {
        tst_brkm(TFAIL, cleanup, "No write lock set for OTP[29].");
    }

    otp_data_p++; /* Move to next data element. */
    if (*otp_data_p == 0) {
        tst_brkm(TFAIL, cleanup, "Unexpected empty OTP[29].");
    }

    tst_resm(TINFO, "Test OTP init_write: OK");
    free(otp_data);
    free(data);
}

/**
 * Brief: Multiple write operation
 * fill34_80.dat is a sequense with multiple write operations to the same row,
 * 34. Purpose is to test multiple writes without lock.
 * Should return OK
 */
static void test_otp_fill_34_write(void)
{
    char *file_name = "fill_34_80.dat";
    size_t size = 0;
    uint8_t *data = NULL;

    read_file(file_name, &data, &size);
    COPS_CHK_RC(cops_write_otp(ctx, (uint32_t *)data, size), COPS_RC_OK);

    /* Read OTP and verify that [34] != 0. */
    cops_otp.meta = *data;
    cops_otp.data = 0;
    COPS_CHK_RC(cops_read_otp(ctx, (uint32_t *)&cops_otp,
                              sizeof(cops_otp)), COPS_RC_OK);

    if (cops_otp.data == 0) {
        tst_brkm(TFAIL, cleanup, "Unexpected empty OTP[34].");
    }
    tst_resm(TINFO, "Test OTP multiple_write: OK");
    free(data);
}

/**
 * Brief: Test conflicting write requests
 * bad_write.dat This write operation should not work proper.
 * It first tries to write data to row 35 in redundancy mode.
 * It will then continue with an ECC write operation at the same row.
 * Finally a permanent write lock is requested.
 * Switching between redundancy and ECC should not be ok and the operation
 * should return an error without writing data to the OTP.
 * Should return COPS_RC_ARGUMENT_ERROR.
 * Added an extra check for OTP[35]. It should remains empty.
 */
static void test_otp_bad_write(void)
{
    char *file_name = "bad_write.dat";
    size_t size = 0;
    uint8_t *data = NULL;

    read_file(file_name, &data, &size);

    COPS_CHK_RC(cops_write_otp(ctx, (uint32_t *)data, size),
                COPS_RC_ARGUMENT_ERROR);

    /* Read OTP and verify that [36] = 0. */
    cops_otp.meta = *data;
    cops_otp.data = 0;
    COPS_CHK_RC(cops_read_otp(ctx, (uint32_t *)&cops_otp,
                              sizeof(cops_otp)), COPS_RC_OK);

    if (cops_otp.data != 0) {
        tst_brkm(TFAIL, cleanup, "Unexpected data 0x%08X in OTP.",
                 cops_otp.data);
    }
    tst_resm(TINFO, "Test OTP bad_write: OK");
    free(data);
}

/**
 * Brief: Lock 36 in redundancy mode.
 * lock_36_redundancy.dat Write data to row 36 and lock in redundancy mode.
 * Should return OK
 */
static void test_otp_lock_redundancy(void)
{
    char *file_name = "lock_36_redundancy.dat";
    size_t size = 0;
    uint8_t *data = NULL;

    read_file(file_name, &data, &size);
    COPS_CHK_RC(cops_write_otp(ctx, (uint32_t *)data, size), COPS_RC_OK);

    /* Read OTP and verify that [36] != 0. */
    cops_otp.meta = *data;
    cops_otp.data = 0;
    COPS_CHK_RC(cops_read_otp(ctx, (uint32_t *)&cops_otp,
                              sizeof(cops_otp)), COPS_RC_OK);

    if (cops_otp.data == 0) {
        tst_brkm(TFAIL, cleanup, "Unexpected empty OTP.");
    }

    /* Check that word 36 is locked in redundancy mode. */
    if (get_ecc((uint32_t *)&cops_otp)) {
        tst_brkm(TFAIL, cleanup, "Unexpected ECC in use, OTP[36].");
    }
    if (!get_write_lock((uint32_t *)&cops_otp)) {
        tst_brkm(TFAIL, cleanup, "Unexpected OTP[36] is not locked.");
    }

    tst_resm(TINFO, "Test OTP lock_redundancy: OK");
    free(data);
}

/**
 * Brief: write / read data, operate on out of range rows.
 */
static void test_otp_out_of_range(void)
{
    char *file_name = "out_of_range.dat";
    size_t size = 0;
    uint8_t *data = NULL;

    read_file(file_name, &data, &size);
    COPS_CHK_RC(cops_write_otp(ctx, (uint32_t *)data, size),
                COPS_RC_ARGUMENT_ERROR);

    /* Read the same out of range row. */
    cops_otp.meta = *data;
    cops_otp.data = 0;
    COPS_CHK_RC(cops_read_otp(ctx, (uint32_t *)&cops_otp,
                              sizeof(cops_otp)), COPS_RC_ARGUMENT_ERROR);

    tst_resm(TINFO, "Test OTP out of range: OK");
    free(data);
}

/**
 * Brief: Write to protected rows.
 * protected_rows.dat can be used to test read- or write. The file will
 * operate on rows 5 - 15. Only row 15 is accessible for read or write.
 * Write_otp should not work at all. Read_otp should return data which is
 * undefined for rows 5 - 14. Output data in meta bits should looks like
 * 0x0D008007 for row 14.
 * Correct value should be returned for row 15.
 */
static void test_otp_protected_rows(void)
{
    char *file_name = "protected_rows.dat";
    size_t size = 0;
    uint8_t *data = NULL;

    read_file(file_name, &data, &size);
    COPS_CHK_RC(cops_write_otp(ctx, (uint32_t *)data, size),
                COPS_RC_OTP_PERMISSION_ERROR);

    /* Read OTP, using the same input. */
    COPS_CHK_RC(cops_read_otp(ctx, (uint32_t *)data, size), COPS_RC_OK);

    tst_resm(TINFO, "Test OTP protected rows: OK");
    free(data);
}

/**
 * Brief: Write and read data at root key hash area
 */
static void test_otp_root_key_hash(void)
{
    uint32_t i;
    uint32_t size = 64;   /* Size of cops_otp_data for a root key hash. */
    uint32_t *otp_data = NULL;
    uint32_t *otp_read_back = NULL;
    uint32_t *otp_read_back_p = NULL;
    uint32_t *otp_data_p = NULL;

    srand(time(NULL));

    otp_data = malloc(size);
    otp_read_back = malloc(size);
    if (otp_data == NULL || otp_read_back == NULL) {
        tst_brkm(TFAIL, cleanup, "test_otp_root_key_hash out of memory!");
    }
    otp_data_p = otp_data;
    otp_read_back_p = otp_read_back;

    for (i = 15; i <= 22; i++) {
        *otp_data_p = i; /* Store row in meta data. */
        *otp_read_back_p = i;
        set_ecc(otp_data_p);
        set_write_lock(otp_data_p);

        otp_data_p++; /* Move pointer to data. */
        otp_read_back_p++;

        *otp_data_p = rand();
        *otp_read_back_p = 0;

        otp_data_p++; /* Move pointer to next meta element. */
        otp_read_back_p++;
    }

    /* Now we have two arrays, one with data to be written and one empty
     * for read back from OTP. */
    COPS_CHK_RC(cops_write_otp(ctx, otp_data, size), COPS_RC_OK);
    COPS_CHK_RC(cops_read_otp(ctx, otp_read_back, size), COPS_RC_OK);

    /* Written and read back data should be equivalent */
    if (memcmp(otp_data, otp_read_back, size) != 0) {
        tst_brkm(TFAIL, cleanup, "Unexpected cmp, OTP write / read back test.");
    }

    tst_resm(TINFO, "Test OTP root key hash: OK");
    free(otp_data);
    free(otp_read_back);
}

/**
 * Brief: Test a bad size, not suitable for cops_otp n*8 bytes.
 */
static void test_otp_bad_size(void)
{
    uint32_t size = 10;   /* Not ok size for cops_otp. */
    cops_otp.meta = 40;
    cops_otp.data = 0x3455;

    COPS_CHK_RC(cops_write_otp(ctx, (uint32_t *)&cops_otp, size),
                COPS_RC_ARGUMENT_ERROR);
    COPS_CHK_RC(cops_read_otp(ctx, (uint32_t *)&cops_otp, size),
                COPS_RC_ARGUMENT_ERROR);
    tst_resm(TINFO, "Test OTP bad size argument: OK");
}

/**
 * Brief: Test read and write in non authenticated mode.
 */
static void test_otp_non_auth_op(void)
{
    uint32_t size = 8;
    cops_otp.meta = 41;
    cops_otp.data = 0x3CFF3455;

    set_ecc(&cops_otp.meta);
    set_write_lock(&cops_otp.meta);
    COPS_CHK_RC(cops_write_otp(ctx, (uint32_t *)&cops_otp, size),
                COPS_RC_OTP_PERMISSION_ERROR);
    COPS_CHK_RC(cops_read_otp(ctx, (uint32_t *)&cops_otp, size), COPS_RC_OK);

    /* Now flags for non auth read and write should be set. */
    if ((cops_otp.meta & 0x01000000) &&
        (cops_otp.meta & 0x00800000)) {
        tst_resm(TINFO, "Test OTP Non authenticated mode: OK");
    } else {
        tst_brkm(TFAIL, cleanup, "Test OTP Non authenticated mode: NOK");
    }
}

/**
 * Brief: Writing to locked part of OTP should return error.
 */
static void test_otp_write_locked(void)
{
    uint32_t size = 8;
    cops_otp.meta = 0x1D; /* Used during init_write. */
    cops_otp.data = 0x3CFF3455; /* New value */

    set_ecc(&cops_otp.meta);
    set_write_lock(&cops_otp.meta);
    COPS_CHK_RC(cops_write_otp(ctx, (uint32_t *)&cops_otp, size),
                COPS_RC_OTP_PERMISSION_ERROR);
    COPS_CHK_RC(cops_read_otp(ctx, (uint32_t *)&cops_otp, size), COPS_RC_OK);

    /* Data written during init_write should remain intact. */
    if (cops_otp.data == 0x53413742) {
        tst_resm(TINFO, "Test OTP write locked: OK");
    } else {
        tst_brkm(TFAIL, cleanup, "Test OTP write locked: NOK");
    }
}

/**
 * Brief: Read data from completely locket part of OTP.
 */
static void test_otp_read_locked(void)
{
    uint32_t size = 8;
    cops_otp.meta = 0;
    cops_otp.data = 0x4080;

    COPS_CHK_RC(cops_read_otp(ctx, (uint32_t *)&cops_otp, size), COPS_RC_OK);

    /* Data should remain as is. */
    if (cops_otp.data == 0x4080) {
        tst_resm(TINFO, "Test OTP read locked: OK");
    } else {
        tst_brkm(TFAIL, cleanup, "Test OTP read locked: NOK");
    }
}

static void test_otp_get_nbr_of_rows(void)
{
#define NBR_ROWS_7400 64
    uint32_t nbr = 0;

    COPS_CHK_RC(cops_get_nbr_of_otp_rows(ctx, &nbr), COPS_RC_OK);

    if (nbr == NBR_ROWS_7400) {
        tst_resm(TINFO, "Test OTP nbr_of_rows: OK");
    } else {
        tst_brkm(TFAIL, cleanup, "Test OTP nbr_of_rows: NOK");
    }
}

/**
 * Brief: Setup and call all OTP tests.
 */
void test_23_otp()
{
    uint32_t ret_code;

    RUN_CMD("cp %s/otp/*.dat .", template_dir);
    RUN_CMD("cp %s/../../copsd/otp.dat .", template_dir);

    /* Most tests for OTP should run in authenticated mode. */
    cops_auth_data_t auth_data_permanent = {
        COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL
    };
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_permanent),
                COPS_RC_OK);

    ret_code = test_otp_detect_variant();
    switch (ret_code) {
    case COPS_REAL_OTP:
        test_otp_get_nbr_of_rows();
        test_otp_read_locked();
        tst_resm(TINFO, "Test OTP will stop here. Running on real hardware.");
        break;
    case COPS_RAM_OTP:
    case COPS_FILE_OTP:
        tst_resm(TINFO, "OTP tests in emulated OTP.");
        test_otp_get_nbr_of_rows();
        test_otp_init_write();
        test_otp_fill_34_write();
        test_otp_lock_redundancy();
        test_otp_bad_write();
        test_otp_out_of_range();
        test_otp_protected_rows();
        test_otp_root_key_hash();
        test_otp_bad_size();
        COPS_CHK_RC(cops_deauthenticate(ctx, false), COPS_RC_OK);
        test_otp_non_auth_op();
        COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_permanent),
                    COPS_RC_OK);
        test_otp_write_locked();
        test_otp_read_locked();
        break;
    default:
        break;
    }
}

/**
 * Brief: Test that a successful verification of the SIM Personalization Key
 * will reset number of attempts. Also verify that it works to reset the
 * Control Key to the default value.
 */
static void test_24_verify_sim_personalization_key(void)
{
    cops_bind_properties_arg_t arg;
    cops_simlock_status_t simlock_status;
    cops_simlock_control_key_t sim_key_default = {"000000"};
    cops_simlock_control_key_t sim_key_new = {"111111"};
    cops_simlock_control_key_t sim_key_invalid = {"0000000"};

    RUN_CMD("cp %s/%s .", template_dir, "sim_personalization.csd");

    /* 1. auth      = Authenticate, (permanent authentication).
     * 2. wdefaultd = Write default data.
     * 3. deauth    = Deauthenticate. */
    auth_wdefaultd_deauth(arg, "sim_personalization.csd", false);

    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);

    if (simlock_status.siml_status.attempts_left != 2) {
        tst_brkm(TFAIL, cleanup, "NbrOfAttempts not correct: %lu",
                 simlock_status.siml_status.attempts_left);
    }

    /* Verify default Control Key */
    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx, &sim_key_default, &sim_key_default),
                COPS_RC_OK);

    /* Try to verify Control Key with invalid key */
    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx, &sim_key_invalid, &sim_key_invalid),
                COPS_RC_INVALID_SIMLOCK_KEY);

    /* Number of attempts should have decreased */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);
    if (simlock_status.siml_status.attempts_left != 1) {
        tst_brkm(TFAIL, cleanup, "NbrOfAttempts not correct: %lu",
                 simlock_status.siml_status.attempts_left);
    }

    /* Change to new ControlKey */
    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx, &sim_key_default, &sim_key_new),
                COPS_RC_OK);

    /* Verify that number of attempts are reset */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);
    if (simlock_status.siml_status.attempts_left != 2) {
        tst_brkm(TFAIL, cleanup, "NbrOfAttempts not correct: %lu",
                 simlock_status.siml_status.attempts_left);
    }

   /* Verify that old ControlKey isn't accepted anymore */
    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx, &sim_key_default, &sim_key_default),
                COPS_RC_INVALID_SIMLOCK_KEY);

   /* Verify that new ControlKey is set */
    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx, &sim_key_new, &sim_key_new),
                COPS_RC_OK);

    /* Reset key to default value */
    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx, &sim_key_new, COPS_SIMLOCK_SIM_RESET_KEY),
                COPS_RC_OK);

   /* Verify that the new ControlKey isn't accepted anymore */
    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx, &sim_key_new, &sim_key_new),
                COPS_RC_INVALID_SIMLOCK_KEY);

   /* Verify that the default ControlKey is accepted */
    COPS_CHK_RC(cops_simlock_change_sim_control_key
                (ctx, &sim_key_default, &sim_key_default),
                COPS_RC_OK);

    /* Verify that number of attempts are reset */
    COPS_CHK_RC(cops_simlock_get_status(ctx, &simlock_status), COPS_RC_OK);
    if (simlock_status.siml_status.attempts_left != 2) {
        tst_brkm(TFAIL, cleanup, "NbrOfAttempts not correct: %lu",
                 simlock_status.siml_status.attempts_left);
    }

    tst_resm(TINFO, "Verify SIM-Personalization Key OK");
}

static void test_25_write_rpmb_key(void)
{
    cops_auth_data_t auth_data_permanent = {
        COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION, 0, NULL};

    /* Writing RPMB key should run in authenticated mode. */
    COPS_CHK_RC(cops_authenticate(ctx, false, &auth_data_permanent),
                COPS_RC_OK);

    COPS_CHK_RC(cops_write_rpmb_key(ctx, 0, 0), COPS_RC_OK);

    COPS_CHK_RC(cops_deauthenticate(ctx, true), COPS_RC_OK);

    COPS_CHK_RC(cops_write_rpmb_key(ctx, 0, 0),
                COPS_RC_NOT_AUTHENTICATED_ERROR);

    tst_resm(TINFO, "Write RPMB key OK");
}

static void test_26_get_product_debug_settings(void)
{
    uint32_t debug_settings = 0;

    /* Not much of a test but just to ensure nothing hangs. */

    COPS_CHK_RC(cops_get_product_debug_settings(ctx, &debug_settings),
                COPS_RC_OK);

    tst_resm(TINFO, "Getting product debug settings OK");
}
