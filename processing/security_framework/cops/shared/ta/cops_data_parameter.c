/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_data.h>
#include <cops_shared.h>
#include <cops_common.h>
#include <string.h>

#ifndef COPS_FILE_NUMBER
#define COPS_FILE_NUMBER cops_data_parameter_c
#endif

#define COPS_DATA_MAJOR_REVISION    2
#define COPS_DATA_MINOR_REVISION    0

/* Note that these functions doesn't check for any errors */
static uint16_t get_uint16(const cops_data_t *cd, size_t *offset);
static void put_uint16(cops_data_t *cd, size_t *offset, uint16_t val);

static size_t verify_revision(const cops_data_t *cd);


cops_return_code_t cops_data_init(cops_data_t *cd)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    COPS_DEBUG_CHK_ASSERTION(cd != NULL);
    COPS_CHK_ASSERTION(cd->length == 0);
    COPS_CHK_ASSERTION(cd->max_length > 4);

    put_uint16(cd, &cd->length, COPS_DATA_MAJOR_REVISION);
    put_uint16(cd, &cd->length, COPS_DATA_MINOR_REVISION);

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_get_parameter(const cops_data_t *cd, cops_parameter_data_t *data)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    size_t    offset = 0;

    while (true) {
        cops_parameter_data_t d;

        COPS_CHK_RC(cops_data_get_next_parameter(cd, &offset, &d));

        if (d.data == NULL) {
            /* Param was not found */
            data->data = NULL;
            data->length = 0;
            break;
        }

        if (d.id == data->id) {
            /* Found it */
            *data = d;
            break;
        }
    }
function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_get_next_parameter(const cops_data_t *cd, size_t *offset,
                             cops_parameter_data_t *data)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    uint16_t  id;
    uint16_t  len;

    if (*offset == 0) {
        /*
         * Reset offset and verify major and minor version
         */
        *offset = verify_revision(cd);
        COPS_CHK(*offset != 0, COPS_RC_DATA_TAMPERED_ERROR);
    }

    /* Reached end of parameters */
    if (*offset == cd->length) {
        memset(data, 0, sizeof(*data));
        goto function_exit;
    }

    COPS_CHK(*offset + sizeof(uint16_t) * 2 <= cd->length,
             COPS_RC_DATA_TAMPERED_ERROR);

    id = get_uint16(cd, offset);
    len = get_uint16(cd, offset);

    COPS_CHK(len >= COPS_PARAMETER_HEADER_SIZE, COPS_RC_DATA_TAMPERED_ERROR);

    /* We have already read the header */
    len -= COPS_PARAMETER_HEADER_SIZE;

    /* Check there's space for parameter payload */
    COPS_CHK(*offset + len <= cd->length, COPS_RC_DATA_TAMPERED_ERROR);

    data->id = id;
    data->data = cd->data + *offset;
    data->length = len;
    *offset += len;

    /* Check that the referenced data is inside of COPS Data */
    COPS_DEBUG_CHK_ASSERTION(cd->data + cd->length >=
                             data->data + data->length);
    COPS_DEBUG_CHK_ASSERTION(cd->data < data->data);

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_set_parameter(cops_data_t *cd, cops_parameter_data_t *data)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_parameter_data_t d;

    d.id = data->id;

    /*
     * If the parameter exists multiple times, now is a good time
     * to remove it.
     */
    while (true) {

        COPS_CHK_RC(cops_data_get_parameter(cd, &d));

        if (d.data == NULL) {
            break;
        }

        COPS_CHK_RC(cops_data_remove_parameter(cd, &d));
    }

    COPS_CHK_RC(cops_data_add_parameter(cd, data));

function_exit:
    return ret_code;
}

cops_return_code_t
cops_data_add_parameter(cops_data_t *cd, cops_parameter_data_t *data)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    size_t    offset;

    /*
     * Check that there's space for the parameter, we don't want to
     * use COPS_CHK_XXX here in case we are adding to rvs. In that
     * case this function would recurse until stack overflow.
     */
    if (data->length + COPS_PARAMETER_HEADER_SIZE >
        cd->max_length - cd->length) {
        return COPS_RC_BUFFER_TOO_SMALL;
    }

    /*
     * Reset offset and verify major and minor version
     */
    offset = verify_revision(cd);
    COPS_CHK(offset != 0, COPS_RC_DATA_TAMPERED_ERROR);

    /*
     * Find position where to insert the parameter.
     * Parameter ids should allways be sorted.
     */
    while (offset + sizeof(uint16_t) * 2 < cd->length) {
        uint16_t  id = get_uint16(cd, &offset);
        uint16_t  len = get_uint16(cd, &offset);

        COPS_CHK(len >= COPS_PARAMETER_HEADER_SIZE,
                 COPS_RC_DATA_TAMPERED_ERROR);

        /* We have already read the header */
        len -= COPS_PARAMETER_HEADER_SIZE;

        /* Check that there's space for parameter payload */
        COPS_CHK(offset + len <= cd->length, COPS_RC_BUFFER_TOO_SMALL);

        if (id > data->id) {
            offset -= sizeof(uint16_t) * 2;
            break;
        }

        offset += len;
    }

    /* Make room for the new parameter */
    memmove(cd->data + offset + data->length + COPS_PARAMETER_HEADER_SIZE,
            cd->data + offset, cd->length - offset);
    cd->length += data->length + COPS_PARAMETER_HEADER_SIZE;

    /* Write the new parameter */
    put_uint16(cd, &offset, data->id);
    put_uint16(cd, &offset, data->length + COPS_PARAMETER_HEADER_SIZE);
    memcpy(cd->data + offset, data->data, data->length);

function_exit:
    return ret_code;
}

static uint16_t get_uint16(const cops_data_t *cd, size_t *offset)
{
    uint16_t  val;

    memcpy(&val, cd->data + *offset, sizeof(val));
    *offset += sizeof(val);
    return val;
}

static void put_uint16(cops_data_t *cd, size_t *offset, uint16_t val)
{
    memcpy(cd->data + *offset, &val, sizeof(val));
    *offset += sizeof(val);
}

static size_t verify_revision(const cops_data_t *cd)
{
    size_t    offset;
    uint16_t  major_revision;
    uint16_t  minor_revision;

    if (cd->length < 4) {
        return 0;
    }

    offset = 0;
    major_revision = get_uint16(cd, &offset);
    minor_revision = get_uint16(cd, &offset);

    if (major_revision == COPS_DATA_MAJOR_REVISION &&
        minor_revision == COPS_DATA_MINOR_REVISION) {
        return sizeof(uint16_t) * 2;
    }

    return 0;
}

cops_return_code_t
cops_data_remove_parameter(cops_data_t *cd, cops_parameter_data_t *d)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    uint8_t  *para;
    size_t    plen;
    size_t    restlen; /* Len of rest of cd, after the para to rm */

    para = COPS_UNCONST(d->data - COPS_PARAMETER_HEADER_SIZE);
    plen = d->length + COPS_PARAMETER_HEADER_SIZE;
    restlen = cd->data + cd->length - (para + plen);
    memmove(para, para + plen, restlen);
    cd->length -= plen;

    return ret_code;
}
