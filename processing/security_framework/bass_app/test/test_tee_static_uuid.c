#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bass_app_test.h>
#include <tee_client_api.h>
#include <uuid.h>

enum error_code {
    TEST_CASE_OK = 0,
    TEST_CASE_FAILED
};

static int test_incorrect_uuid(void)
{
    TEEC_Context ctx = 0;
    TEEC_ErrorOrigin org = TEEC_ORIGIN_API;
    TEEC_Session session;
    TEEC_UUID uuid = INVALID_TA_UUID;


    memset(&session, 0, sizeof(TEEC_Session));

    if (TEEC_InitializeContext(NULL, &ctx) != TEEC_SUCCESS) {
        fprintf(stderr, "[%s]: TEEC_InitializeContext failed (%d)\n",
                __func__, __LINE__);
        return TEST_CASE_FAILED;
    }

    if (TEEC_OpenSession(&ctx, &session, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL,
                         &org) != TEEC_ERROR_ITEM_NOT_FOUND) {
        fprintf(stderr, "[%s]: TEEC_OpenSession negative test case failed "
                "(%d)\n", __func__, __LINE__);

        if (TEEC_CloseSession(&session) != TEEC_SUCCESS) {
            fprintf(stderr, "[%s]: TEEC_CloseSession failed (%d)\n",
                    __func__, __LINE__);
            return TEST_CASE_FAILED;
        }
    }

    if (TEEC_FinalizeContext(&ctx) != TEEC_SUCCESS) {
        fprintf(stderr, "[%s]: TEEC_FinalizeContext failed (%d)\n",
                __func__, __LINE__);
        return TEST_CASE_FAILED;
    }

    return TEST_CASE_OK;
}

static int test_correct_uuid(void)
{
    TEEC_Context ctx = 0;
    TEEC_ErrorOrigin org = TEEC_ORIGIN_API;
    TEEC_Session session;
    TEEC_UUID uuid = STATIC_TA_UUID;

    memset(&session, 0, sizeof(TEEC_Session));

    if (TEEC_InitializeContext(NULL, &ctx) != TEEC_SUCCESS) {
        fprintf(stderr, "[%s]: TEEC_InitializeContext failed (%d)\n",
                __func__, __LINE__);
        return TEST_CASE_FAILED;
    }

    if (TEEC_OpenSession(&ctx, &session, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL,
                         &org) != TEEC_SUCCESS) {
        fprintf(stderr, "[%s]: TEEC_OpenSession failed (%d)\n",
                __func__, __LINE__);

    }

    if (TEEC_CloseSession(&session) != TEEC_SUCCESS) {
        fprintf(stderr, "[%s]: TEEC_CloseSession failed (%d)\n",
                __func__, __LINE__);
        return TEST_CASE_FAILED;
    }

    if (TEEC_FinalizeContext(&ctx) != TEEC_SUCCESS) {
        fprintf(stderr, "[%s]: TEEC_FinalizeContext failed (%d)\n",
                __func__, __LINE__);
        return TEST_CASE_FAILED;
    }

    return TEST_CASE_OK;
}

uint32_t bass_app_static_uuid_test(void)
{
    int ret = TEST_CASE_OK;

    if (test_incorrect_uuid() == TEST_CASE_FAILED) {
        fprintf(stderr, "test_incorrect_uuid failed\n");
        ret = TEST_CASE_FAILED;
    }

    if (test_correct_uuid() == TEST_CASE_FAILED) {
        fprintf(stderr, "test_correct_uuid failed\n");
        ret = TEST_CASE_FAILED;
    }

    fprintf(stdout, "\n  If you see a message above saying ...\n");
    fprintf(stdout, "  [call_sec_world] failed in secure world\n");
    fprintf(stdout, "  then look at ER337120 for more information.\n");
    fprintf(stdout, "  if the ER is solved, please recompile this\n");
    fprintf(stdout, "  binary and remove this information text.\n\n");

    if (ret == TEST_CASE_OK) {
        fprintf(stderr, "static_uuid_test case PASSED!\n");
    } else {
        fprintf(stderr, "static_uuid_test case FAILED!\n");
    }

    return ret;
}
