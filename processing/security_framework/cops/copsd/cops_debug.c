/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/

#include <cops_debug.h>
#include <cops_shared.h>
#include <cops_common.h>
#include <string.h>

void cops_debug_print_error_stack(cops_data_t *cd)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    size_t    offset;
    cops_parameter_data_t d;
    const char *filename = (char *) "";
    offset = 0;

    while (cops_data_get_next_parameter(cd, &offset, &d) == COPS_RC_OK &&
            d.data != NULL) {

        /* Compressed COPS error (file, line and err_code in one uint32) */
        if (d.id == COPS_ARGUMENT_ID_ERROR_STACK) {
            uint32_t value;

            COPS_CHK_ASSERTION(d.length == sizeof(uint32_t));
            memcpy(&value, d.data, sizeof(value));
            /* *INDENT-OFF* */

            switch ((value >> 16) & 0xff) {
            case cops_data_imei_c:
                filename = "cops_data_imei.c";
                break;
            case cops_data_lock_c:
                filename = "cops_data_lock.c";
                break;
            case cops_data_parameter_c:
                filename = "cops_data_parameter.c";
                break;
            case cops_data_sipc_c:
                filename = "cops_data_sipc.c";
                break;
            case cops_sipc_message_c:
                filename = "cops_sipc_message.c";
                break;
            case cops_tapp_sipc_c:
                filename = "cops_tapp_sipc.c";
                break;
            case cops_auth_c:
                filename = "cops_auth.c";
                break;
            case cops_error_c:
                filename = "cops_error.c";
                break;
            case cops_extint_emul_c:
                filename = "cops_extint_emul.c";
                break;
            case cops_extint_secure_c:
                filename = "cops_extint_secure.c";
                break;
            case cops_otp_c:
                filename = "cops_otp.c";
                break;
            case cops_ram_otp_c:
                filename = "cops_ram_otp.c";
                break;
            case cops_protect_c:
                filename = "cops_protect.c";
                break;
            case cops_simlock_c:
                filename = "cops_simlock.c";
                break;
            case cops_state_emul_c:
                filename = "cops_state_emul.c";
                break;
            case cops_state_secure_c:
                filename = "cops_state_secure.c";
                break;
            case cops_trusted_application_c:
                filename = "cops_trusted_application.c";
                break;
            case otp_permissions_c:
                filename = "otp_permissions.c";
                break;
            case taf_authenticate_c:
                filename = "taf_authenticate.c";
                break;
            case taf_bind_data_c:
                filename = "taf_bind_data.c";
                break;
            case taf_bind_properties_c:
                filename = "taf_bind_properties.c";
                break;
            case taf_calchash_c:
                filename = "taf_calchash.c";
                break;
            case taf_cops_otp_c:
                filename = "taf_cops_otp.c";
                break;
            case taf_cops_otp_stubbed_c:
                filename = "taf_cops_otp_stubbed.c";
                break;
            case taf_deauthenticate_c:
                filename = "taf_deauthenticate.c";
                break;
            case taf_derive_session_key_c:
                filename = "taf_derive_session_key.c";
                break;
            case taf_derive_session_key_stubbed_c:
                filename = "taf_derive_session_key_stubbed.c";
                break;
            case taf_get_challenge_c:
                filename = "taf_get_challenge.c";
                break;
            case taf_get_device_state_c:
                filename = "taf_get_device_state.c";
                break;
            case taf_get_long_term_shared_key_c:
                filename = "taf_get_long_term_shared_key.c";
                break;
            case taf_get_long_term_shared_key_stubbed_c:
                filename = "taf_get_long_term_shared_key_stubbed.c";
                break;
            case taf_get_seed_c:
                filename = "taf_get_seed.c";
                break;
            case taf_get_session_key_modem_rand_c:
                filename = "taf_session_key_modem_rand.c";
                break;
            case taf_get_session_key_modem_rand_stubbed_c:
                filename = "taf_session_key_modem_rand_stubbed.c";
                break;
            case taf_read_data_c:
                filename = "taf_read_data.c";
                break;
            case taf_read_imei_c:
                filename = "taf_read_imei.c";
                break;
            case taf_read_modem_data_c:
                filename = "taf_read_modem_data.c";
                break;
            case taf_router_c:
                filename = "taf_router.c";
                break;
            case taf_set_modem_band_c:
                filename = "taf_set_modem_band.c";
                break;
            case taf_simlock_change_sim_control_key_c:
                filename = "taf_simlock_change_sim_control_key.c";
                break;
            case taf_simlock_get_lock_attempts_left_c:
                filename = "taf_simlock_get_lock_attempts_left.c";
                break;
            case taf_simlock_get_lock_settings_c:
                filename = "taf_simlock_get_lock_settings.c";
                break;
            case taf_simlock_get_status_c:
                filename = "taf_simlock_get_status.c";
                break;
            case taf_simlock_lock_c:
                filename = "taf_simlock_lock.c";
                break;
            case taf_simlock_mck_reset_c:
                filename = "taf_simlock_mck_reset.c";
                break;
            case taf_simlock_unlock_c:
                filename = "taf_simlock_unlock.c";
                break;
            case taf_simlock_verify_control_keys_c:
                filename = "taf_verify_control_keys.c";
                break;
            case taf_verify_data_binding_c:
                filename = "taf_verify_data_binding.c";
                break;
            case taf_verify_imsi_c:
                filename = "taf_verify_imsi.c";
                break;
            case taf_verify_signedheader_c:
                filename = "taf_verify_signedheader.c";
                break;
            case taf_init_arb_table_c:
                filename = "taf_init_arb_table.c";
                break;
            case taf_check_arb_update_c:
                filename = "taf_check_arb_update.c";
                break;
            case cops_arbhandling_c:
                filename = "cops_arb_handling.c";
                break;
            case taf_init_arb_table_stubbed_c:
                filename = "taf_init_arb_table_stubbed.c";
                break;
            case taf_check_arb_update_stubbed_c:
                filename = "taf_check_arb_update_stubbed.c";
                break;
            case taf_write_secprofile_c:
                filename = "taf_write_secprofile.c";
                break;
            case taf_write_secprofile_stubbed_c:
                filename = "taf_write_secprofile_stubbed.c";
                break;
            case taf_write_rpmb_key_c:
                filename = "taf_write_rpmb_key.c";
                break;
            case taf_write_rpmb_key_stubbed_c:
                filename = "taf_write_rpmb_key_stubbed.c";
                break;
            case taf_handle_simdata_c:
                filename = "taf_handle_simdata.c";
                break;
            case taf_get_product_debug_settings_c:
                filename = "taf_get_product_debug_settings.c";
                break;
            case tapp_test_c:
                filename = "tapp_test.c";
                break;
            default:
                break;
            }

            COPS_LOG(LOG_ERROR, "TAF Error %s:%d code %d\n",
                     filename,
                     (value & 0xffff),          /* line */
                     (value >> 24) & 0xff);     /* error code */
            /* *INDENT-ON* */

        } else if (d.id == COPS_ARGUMENT_ID_ERROR_STRING) {
            /* COPS error as string */
            COPS_LOG(LOG_ERROR, "TAF Error %s\n", (const char *)d.data);
        } else if (d.id == COPS_ARGUMENT_ID_ISSW_ERROR_STACK) {
            /* ISSW error code */
            uint32_t issw_err;
            COPS_CHK_ASSERTION(d.length == sizeof(uint32_t));
            memcpy(&issw_err, d.data, sizeof(issw_err));
            COPS_LOG(LOG_ERROR, "ISSW Error 0x%x\n", issw_err);
        } else if (d.id == COPS_ARGUMENT_ID_SLA_ERROR_STACK) {
            /* SLA (Boot ROM) error code */
            uint32_t sla_err;
            COPS_CHK_ASSERTION(d.length == sizeof(uint32_t));
            memcpy(&sla_err, d.data, sizeof(sla_err));
            COPS_LOG(LOG_ERROR, "SLA Error 0x%x\n", sla_err);
        }
    }

function_exit:
    if (COPS_RC_OK != ret_code) {
        COPS_LOG(LOG_ERROR, "Error(%d) parsing TAF errors\n", ret_code);
    }
}
