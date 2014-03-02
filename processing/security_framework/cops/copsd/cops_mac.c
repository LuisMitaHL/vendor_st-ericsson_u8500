/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_shared.h>
#include <string.h>

/*
 * APE-COPS can't calculate or verify MACs, but cops_sipc.c requires these
 * to be defined at least.
 */

cops_return_code_t
cops_mac_calc(cops_mac_type_t mac_type,
              const uint8_t *data, size_t data_len,
              uint8_t *mac, size_t mac_len)
{
    (void) mac_type;
    (void) data;
    (void) data_len;

    memset(mac, 0, mac_len);
    return COPS_RC_OK;
}

cops_return_code_t
cops_mac_verify(cops_mac_type_t mac_type,
                const uint8_t *data, size_t data_len,
                const uint8_t *mac, size_t mac_len, bool *mac_ok)
{
    (void) mac_type;
    (void) data;
    (void) data_len;
    (void) mac;
    (void) mac_len;

    if (mac_ok != NULL) {
        *mac_ok = false;
        return COPS_RC_OK;
    }

    return COPS_RC_DATA_TAMPERED_ERROR;
}
