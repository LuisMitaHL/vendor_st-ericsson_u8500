#include <syslog.h>
#include <stdint.h>
#include <stdarg.h>

#include "type_def_wm_isi_if.h"
#include "common_dsp_test_isi.h"
#include "pn_const_modem_ext.h"

#include "tatmodemlib.h"
#include "isi_driver_lib.h"

#include "taterror.h"
#include "isimsg.h"
#include "misc.h"

void tatlrf_syslog(int vp_prio, int vp_output, const char *pp_func,
    const char *pp_file, int vp_line, const char *pp_format, ...)
{
#ifdef _DEBUG
    va_list args;
    va_start(args, pp_format);
    vfprintf(stdout, pp_format, args);
    va_end(args);
#endif
}

#define SYSLOG(prio, fmt , __args__...) tatlrf_syslog(prio, 0, NULL, NULL, \
    0, fmt"\n", ##__args__)
#define SYSLOGSTR(prio, fmt , __args__...) tatlrf_syslog(prio, 0, NULL, NULL, \
    0, fmt, ##__args__)

void tatrf_isi_test_dumpContext(struct tatrf_isi_msg_t *pp_ctx, int vp_prio)
{
    SYSLOG(vp_prio, "a isi_msg struct at %p: msg address = %p"
        "\n\tbuf size = %u"
        "\n\tencoding = %u"
        "\n\taddress of nb subblocks = %p"
        "\n\taddress of sb = %p"
        "\n\tresource ID = %u\n",
        (void*)pp_ctx,
        pp_ctx->msg,
        pp_ctx->size,
        pp_ctx->encoding,
        pp_ctx->nb_sb,
        pp_ctx->sb,
        pp_ctx->resource);

    size_t vl_msg_len = tatrf_isi_total_length(pp_ctx);
    if ( (pp_ctx->msg != NULL) && (vl_msg_len > 0) )
    {
        PrintBinary(pp_ctx->msg, vl_msg_len);
    }
}

int tatrf_isi_test_testInit(struct tatrf_isi_msg_t *pp_st)
{
    tatrf_isi_init(pp_st);
    
    if ( (u8*)pp_st->msg == NULL )
        return 1;

    if ( pp_st->size != ISI_DRIVER_MAX_MESSAGE_LENGTH )
        return 2;

    if ( pp_st->sb != NULL )
        return 3;

    return 0;
}

int tatrf_isi_test_testUtils()
{
    C_TEST_SB_GSM_RX_CONTROL_STR vl_sb;
    vl_sb.sb_id = C_TEST_SB_GSM_RX_CONTROL;
    vl_sb.sb_len = SIZE_C_TEST_SB_GSM_RX_CONTROL_STR;

    u16 vl_sb_id, vl_sb_len;
    tatl17_get_sb_hdr((u16*)&vl_sb, &vl_sb_id, &vl_sb_len);

    if ( vl_sb_id != C_TEST_SB_GSM_RX_CONTROL )
        return 1;

    if ( vl_sb_len != SIZE_C_TEST_SB_GSM_RX_CONTROL_STR )
        return 2;

    /* set dummy sb info */
     tatl17_set_sb_hdr((u16*)&vl_sb, 0x0018, 0x0107);

     tatl17_get_sb_hdr((u16*)&vl_sb, &vl_sb_id, &vl_sb_len);
    if ( vl_sb_id != 0x0018 )
        return 3;

    if ( vl_sb_len != 0x0107 )
        return 4;

    return 0;
}

int tatrf_isi_test_testFindSubblock()
{
    struct tatrf_isi_msg_t vl_st;
    tatrf_isi_init(&vl_st);

    /* create a response from scratch having 4 subblocks including 2 of the same
       type. The subblocks data do not matter. */
    C_TEST_TUNING_RESP_STR* pl_resp = tatrf_isi_msg_begin(&vl_st, PN_COMMON_RF_TEST, C_TEST_TUNING_RESP, numerOfSubBlocks);
    if ( NULL == pl_resp ) return -1;

    C_TEST_SB_GSM_RX_CONTROL_RESP_STR   *pl_sb1 = tatrf_isi_msg_add_sb(&vl_st, C_TEST_SB_GSM_RX_CONTROL_RESP);
    SYSLOG(LOG_DEBUG, "subblock 1 (id=0x%2X): %p", C_TEST_SB_GSM_RX_CONTROL_RESP, pl_sb1);

    C_TEST_SB_PWR_SEQ_MEAS_RESULTS_STR  *pl_sb2 = tatrf_isi_msg_add_sb(&vl_st, C_TEST_SB_PWR_SEQ_MEAS_RESULTS);
    SYSLOG(LOG_DEBUG, "subblock 2 (id=0x%2X): %p", C_TEST_SB_PWR_SEQ_MEAS_RESULTS, pl_sb2);

    C_TEST_SB_PWR_SEQ_MEAS_RESULTS_STR  *pl_sb3 = tatrf_isi_msg_add_sb(&vl_st, C_TEST_SB_PWR_SEQ_MEAS_RESULTS);
    SYSLOG(LOG_DEBUG, "subblock 3 (id=0x%2X): %p", C_TEST_SB_PWR_SEQ_MEAS_RESULTS, pl_sb3);

    C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS_STR *pl_sb4 = tatrf_isi_msg_add_sb(&vl_st, C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS);
    SYSLOG(LOG_DEBUG, "subblock 4 (id=0x%2X): %p", C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS, pl_sb4);

    C_TEST_SB_PWR_SEQ_MEAS_RESULTS_STR  *pl_sb5 = tatrf_isi_msg_add_sb(&vl_st, C_TEST_SB_PWR_SEQ_MEAS_RESULTS);
    SYSLOG(LOG_DEBUG, "subblock 5 (id=0x%2X): %p", C_TEST_SB_PWR_SEQ_MEAS_RESULTS, pl_sb5);

    /* seek first subblock from start */
    if ( tatrf_isi_find_sb(&vl_st, C_TEST_SB_GSM_RX_CONTROL_RESP, NULL) != pl_sb1 )
        return 1;

    /* seek another subblock from start */
    if ( tatrf_isi_find_sb(&vl_st, C_TEST_SB_PWR_SEQ_MEAS_RESULTS, NULL) != pl_sb2 )
        return 2;

    /* seek next subblock of same type */
    if ( tatrf_isi_find_sb(&vl_st, C_TEST_SB_PWR_SEQ_MEAS_RESULTS, pl_sb2) != pl_sb3 )
        return 3;

    if ( tatrf_isi_find_sb(&vl_st, C_TEST_SB_PWR_SEQ_MEAS_RESULTS, pl_sb3) != pl_sb5 )
        return 4;

    /* seek next subblock which is not of the same type */
    if ( tatrf_isi_find_sb(&vl_st, C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS, pl_sb2) != pl_sb4 )
        return 5;

    /* seek subblock which is before starting position */
    if ( tatrf_isi_find_sb(&vl_st, C_TEST_SB_GSM_RX_CONTROL_RESP, pl_sb3) != NULL )
        return 6;

    return 0;
}

int tatrf_isi_test_testSetMsg(struct tatrf_isi_msg_t *pp_st)
{
    C_TEST_TUNING_REQ_STR* pl_req = tatrf_isi_msg_begin(pp_st, PN_COMMON_RF_TEST, C_TEST_TUNING_REQ, numberOfSubBlocks);

    if ( NULL == pl_req )
        return 1;

    u8 vl_trans_id, vl_msg_id;
    tatl17_11get_id(pp_st, &vl_trans_id, &vl_msg_id);
    SYSLOG(LOG_DEBUG, "trans id = %u, msg id = %u", vl_trans_id, vl_msg_id);
    if ( vl_msg_id != C_TEST_TUNING_REQ )
        return 2;

    if ( pp_st->resource != PN_COMMON_RF_TEST )
        return 3;

    if ( pp_st->sb != NULL )
        return 4;

    if ( pp_st->nb_sb != &pl_req->numberOfSubBlocks )
        return 5;

    if ( *pp_st->nb_sb != 0 )
        return 6;

    if ( tatrf_isi_total_length(pp_st) != (sizeof(t_isi_header) + SIZE_C_TEST_TUNING_REQ_STR) )
        return 7;

    return 0;
}

int tatrf_isi_test_testAddFixedSb(struct tatrf_isi_msg_t *pp_st)
{
    u16 vl_nb_sb = *pp_st->nb_sb;

    C_TEST_SB_GSM_RX_CONTROL_STR* pl_sb = tatrf_isi_msg_add_sb(pp_st, C_TEST_SB_GSM_RX_CONTROL);

    pl_sb->mode = C_TEST_RX_ON;
    pl_sb->fill1 = 0xAA55;
    pl_sb->rf_band = INFO_GSM900;
    pl_sb->channel = 38;
    pl_sb->mon_ch = 68;
    pl_sb->afc = 0;
    pl_sb->agc = 1;
    pl_sb->operationMode = C_TEST_CONTINUOUS;
    pl_sb->fill2 = 0xAA55;

    if ( NULL == pl_sb )
        return 1;

    if ( pp_st->sb != (u16*)pl_sb )
        return 2;

    u16 vl_sb_id, vl_sb_len;
    tatl17_get_sb_hdr(pp_st->sb, &vl_sb_id, &vl_sb_len);

    if ( pl_sb->sb_id != C_TEST_SB_GSM_RX_CONTROL )
        return 3;

    if ( pl_sb->sb_len != SIZE_C_TEST_SB_GSM_RX_CONTROL_STR )
        return 4;

    if ( *pp_st->nb_sb != vl_nb_sb+1 )
        return 5;

    return 0;
}

int tatrf_isi_test_testAddSbByCopy(struct tatrf_isi_msg_t *pp_st)
{
    u16 vl_nb_sb = *pp_st->nb_sb;

    C_TEST_SB_GSM_RX_CONTROL_STR vl_gsm_stop;
    /* pl_sb_id and sb_len set in copy must not ignored by tatrf_isi_msg_add_sb_copy, who uses the arguments instead */
    vl_gsm_stop.sb_id = 0x01;
    vl_gsm_stop.sb_len = 4;
    vl_gsm_stop.mode = C_TEST_RX_OFF;
    vl_gsm_stop.fill1 = 0xAA55;
    vl_gsm_stop.rf_band = INFO_GSM900;
    vl_gsm_stop.channel = 0;
    vl_gsm_stop.mon_ch = 0;
    vl_gsm_stop.afc = 0;
    vl_gsm_stop.agc = 0;
    vl_gsm_stop.operationMode = C_TEST_CONTINUOUS;
    vl_gsm_stop.fill2 = 0xAA55;

    C_TEST_SB_GSM_RX_CONTROL_STR* pl_sb = tatrf_isi_msg_add_sb_copy(pp_st, C_TEST_SB_GSM_RX_CONTROL, &vl_gsm_stop, sizeof(vl_gsm_stop));

    if ( NULL == pl_sb )
        return 1;

    if ( pl_sb->sb_id != C_TEST_SB_GSM_RX_CONTROL )
        return 2;

    if ( pl_sb->sb_len != SIZE_C_TEST_SB_GSM_RX_CONTROL_STR )
        return 3;

    if ( pp_st->sb != (u16*)pl_sb )
        return 4;

    if ( *pp_st->nb_sb != vl_nb_sb+1 )
        return 5;

    return 0;
}

int tatrf_isi_test_testAppendData(struct tatrf_isi_msg_t *pp_st)
{
    if ( pp_st->sb != NULL )
    {
        u16 vl_sb_id, vl_sb_len;
        tatl17_get_sb_hdr(pp_st->sb, &vl_sb_id, &vl_sb_len);

        u16 vl_msg_size = tatrf_isi_total_length(pp_st);

        static const u16 al_data[] = { 0x0015, 0xE1C6, 0xF36A, 0x48BE };
        const u16 vl_data_count = 4;
        const u16 vl_data_size_in_bytes = sizeof(al_data);
        int vl_res = tatl17_04appendData(pp_st, al_data, vl_data_count);
        SYSLOG(LOG_DEBUG, "tatl17_04appendData returned %d", vl_res);

        if ( !TAT_OK(vl_res) )
            return 1;

        u16 vl_new_sb_id, vl_new_sb_len;
        tatl17_get_sb_hdr(pp_st->sb, &vl_new_sb_id, &vl_new_sb_len);

        if ( vl_new_sb_id != vl_sb_id )
            return 2;

        if ( vl_new_sb_len != (vl_sb_len + vl_data_size_in_bytes) )
        {
            SYSLOG(LOG_WARNING, "initial size was %u, new size is %u, expected: %u (appended: %u)", vl_sb_len, vl_new_sb_len, vl_sb_len + vl_data_size_in_bytes, vl_data_size_in_bytes);
            return 3;
        }

        int vl_idata;
        const u16 *pl_data_block = (u16*)((u8*)pp_st->sb + vl_sb_len);
        for ( vl_idata = 0; vl_idata < vl_data_count; vl_idata++ )
        {
            if ( pl_data_block[vl_idata] != al_data[vl_idata] )
                return 4;
        }

        if ( tatrf_isi_total_length(pp_st) != (vl_msg_size + vl_data_size_in_bytes) )
            return 5;
    }
    else
    {
        return -1;
    }

    return 0;
}

int tatrf_isi_test_testMarshal(struct tatrf_isi_msg_t *pp_st)
{
    if ( pp_st->msg != NULL )
    {
        /* marshall then unmarshall message should return to the original one */
        static u8 al_initial [ISI_DRIVER_MAX_MESSAGE_LENGTH];
        const size_t vl_size = tatrf_isi_total_length(pp_st);
        memcpy(al_initial, pp_st->msg, vl_size);

        PrintBinary(pp_st->msg, vl_size);

        if ( pp_st->encoding != client )
            return -1;

        tatl17_12marsh_msg(pp_st, ISI_MARSH);
        PrintBinary(pp_st->msg, vl_size);

        if ( pp_st->encoding != modem )
            return 1;

        if ( memcmp(pp_st->msg, al_initial, vl_size ) == 0 )
            return 2;

        tatl17_12marsh_msg(pp_st, ISI_UNMARSH);
        PrintBinary(pp_st->msg, vl_size);

        if ( pp_st->encoding != client )
            return 3;

        if ( memcmp(pp_st->msg, al_initial, vl_size ) != 0 )
            return 4;

        if ( vl_size != tatrf_isi_total_length(pp_st) )
            return 5;

        return 0;
    }

    return -1;
}

#define TEST_CHECK_RESULT(res, val_ok, test, test_nbr) \
    if ( (res) != (val_ok) ) { SYSLOG(LOG_ERR, "Test %d '%s' FAILED with code %d", (test_nbr), (test), (res)); return (test_nbr); }   \
    else { SYSLOG(LOG_INFO, "Test %d '%s' PASSED", (test_nbr), (test)); }

#define TEST_RUN(res, val_ok, test_nbr, test, label, st)  \
    (test_nbr)++; \
    res = test(st); TEST_CHECK_RESULT(res, val_ok, label, test_nbr)

int tatrf_isi_test_testAll(void)
{
    int vl_testRes = 0;
    int vl_test = 0;

    struct tatrf_isi_msg_t vl_ctx;

    /* Tests not covered here:
     * tatl17_05send, tatl17_06read: quite complex.
     */

    /* test initialize structure */
    TEST_RUN(vl_testRes, 0, vl_test, tatrf_isi_test_testInit, "init", &vl_ctx);

    /* test initialize utilitary functions */
    TEST_RUN(vl_testRes, 0, vl_test, tatrf_isi_test_testUtils, "utils", NULL);

    /* test make a C_TEST_TUNING_REQ message */
    TEST_RUN(vl_testRes, 0, vl_test, tatrf_isi_test_testSetMsg, "set message", &vl_ctx);

    /* test add C_SB_GSM_RX_CONTROL */
    TEST_RUN(vl_testRes, 0, vl_test, tatrf_isi_test_testAddFixedSb, "add fixed subblock", &vl_ctx);

    /* test append copy of C_SB_GSM_RX_CONTROL */
    TEST_RUN(vl_testRes, 0, vl_test, tatrf_isi_test_testAddSbByCopy, "add subblock by copy", &vl_ctx);

    /* test marshalling and unmarshalling at once */
    TEST_RUN(vl_testRes, 0, vl_test, tatrf_isi_test_testMarshal, "marshalling/unmarshalling", &vl_ctx);

    /* test append custom data to subblock */
    TEST_RUN(vl_testRes, 0, vl_test, tatrf_isi_test_testAppendData, "append data to subblock", &vl_ctx);

    /* test message cannot be reinitialized while locked */
    TEST_RUN(vl_testRes, 1, vl_test, tatrf_isi_test_testSetMsg, "reinit when locked", &vl_ctx);

    /* test message can be reinitialized if unlocked */
    vl_ctx.locked = 0;
    TEST_RUN(vl_testRes, 0, vl_test, tatrf_isi_test_testSetMsg, "reinit when unlocked", &vl_ctx);

    /* test iterative subblock search */
    TEST_RUN(vl_testRes, 0, vl_test, tatrf_isi_test_testFindSubblock, "find subblocks", NULL);

    return vl_testRes;
}

int main(int argv, char* argc[])
{
    int vl_test_failed = tatrf_isi_test_testAll();
    if ( vl_test_failed == 0 )
    {
        fprintf(stdout, "all tests are OK\n");
    }
    else
    {
        fprintf(stderr, "test %d FAILED\n", vl_test_failed);
    }

    return vl_test_failed; 
}

