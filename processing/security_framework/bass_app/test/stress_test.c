/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <bass_app_test.h>
#include <debug.h>
#include <tee_client_api.h>
#include <uuid.h>

#define SLEEP_AFTER_SYS_STAT 100000

#ifdef STRESS_TEST_DEBUG
#define SLEEP_VALUE 30000
/* Slow things down a bit to see prints in correct order. */
#define PAUSE do { \
        dprintf(INFO, "%04d\n", ctr++); \
        usleep(SLEEP_VALUE); \
    } while (0)
static uint32_t ctr;
#else
#define PAUSE
#endif

#define TEST_05_NUMBER_OF_LOOPS 100
#define TEST_05_NBR_THREADS 50

/*
 * We use bits to set error codes so it is possible to have several error codes
 * on one uint32_t variable.
 */
#define OPERATION_OK                          0
#define TEEC_INITIALIZE_CONTEXT_FAILED  (1 << 0)
#define TEEC_OPEN_SESSION_FAILED        (1 << 1)
#define TEEC_INVOKE_COMMAND_FAILED      (1 << 2)
#define TEEC_CLOSE_SESSION_FAILED       (1 << 3)
#define TEEC_FINALIZE_CONTEXT_FAILED    (1 << 4)
#define ALLOCATION_FAILED               (1 << 5)

/*
 * Counter and the mutex used to keep track of failing test cases for
 * test_05_multiple_full_access.
 */
static int fail_counter_tc5;
pthread_mutex_t fail_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * Array containing the different sizes that we are allocating and sending to
 * secure world.
 */
const uint32_t malloc_sizes[] = { 1, 2, 4, 8,
    16, 24, 48, 64,
    128, 256, 512, 1024,
    2048, 4096, 8192, 16384,
    32768, 65536, 131072, 262144,
    524288, 1048576 };

#define MALLOC_ARRAY_SIZE (sizeof(malloc_sizes)/sizeof(malloc_sizes[0]))

/*
 * This function is used when checking the memref buffers returned from secure
 * world. The expected values are:
 * memref[0]: \x00\x00\x00...
 * memref[1]: \x01\x01\x01...
 * memref[2]: \x02\x02\x02...
 * memref[3]: \x03\x03\x03...
 *
 * @return 0 if OK, otherwise number of failing bytes.
 */
static uint32_t check_buffers_from_sec_world(TEEC_Operation *op)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t fail = 0;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < op->memRefs[i].size; j++) {
            if (*((char *)op->memRefs[i].buffer + j) != i) {
                dprintf(ERROR, "memref[%d] contain unexpected values\n", i);
                fail++;
                break;
            }
        }
    }

    return fail;
}

/*
 * Generates a random number.
 *
 * @return a random number between min and max give to the function.
 */
static uint32_t get_rand(uint32_t min, uint32_t max)
{
    static uint32_t init_seed = 0;
    uint32_t random_number = 0;

    if (init_seed == 0) {
        srand(time(NULL));
        init_seed = 1;
    }

    /*
     * Example: min = 5, max = 10
     * rand() generates something between 0 and RAND_MAX.
     * Hence we get:
     *   random_number = (rand() % (10 - 5 + 1)) + 5 =>
     *   random_number = (rand() % (6)) + 5 =>
     *   Lets say rand() gives 12345.
     *   random_number = (12345 % 6) + 5 = 8
     */
    random_number = (rand() % (max - min + 1)) + min;
    return random_number;
}

static inline void free_memref_buffers(TEEC_Operation *op)
{
    free(op->memRefs[0].buffer);
    free(op->memRefs[1].buffer);
    free(op->memRefs[2].buffer);
    free(op->memRefs[3].buffer);
}

static uint32_t alloc_memref_buffers(TEEC_Operation *op, uint32_t buffer_size)
{
    size_t i = 0;
    uint8_t alloc_failed = 0;
    uint32_t result = OPERATION_OK;

    for (i = 0; i < TEEC_CONFIG_PAYLOAD_REF_COUNT; ++i) {
        op->memRefs[i].buffer = calloc(buffer_size, sizeof(char));
        if (!op->memRefs[i].buffer) {
            dprintf(ERROR, "Alloc memRefs[%d] with size: 0x%08x failed\n", i,
                    buffer_size);
            /* Mark that the allocation failed. */
            alloc_failed |= (1 << i);
            result = ALLOCATION_FAILED;

        }

#ifdef STRESS_TEST_DEBUG
        dprintf(INFO, "op.memRefs[%d].buffer = 0x%08x\n", i,
                (uint32_t)op->memRefs[i].buffer);
#endif
        op->memRefs[i].size = buffer_size;
        op->memRefs[i].flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
    }

    /*
     * If any allocations failed, deallocate the ones that succeeded. This is
     * done by checking the LSB bit on the alloc_failed variable and if that is
     * zero, then allocation was successful for that index and we need to free
     * it. Then we just right shift alloc_failed and do the same check again
     * until alloc_failed is zero.
     */
    i = 0;
    while (alloc_failed) {
        if ((alloc_failed & 1) == 0) {
            free(op->memRefs[i].buffer);
        }
        alloc_failed >>= 1;
        i++;
    }

    return result;
}

/*
 * This test case will test how tee handles buffer when size increases. In this
 * test we will allocate sizes from 1 byte to 1MB, this is done for all four
 * memrefs. The buffers will be sent to secure world. The stress_ta in secure
 * world will verify that the buffers contains:
 *   01 of every byte of memref[0]
 *   02 of every byte of memref[1]
 *   03 of every byte of memref[2]
 *   04 of every byte of memref[3]
 *
 * After this verification secure world will copy:
 *   memref[2] to memref[3]
 *   memref[1] to memref[2]
 *   memref[0] to memref[1]
 *
 * and then memset memref[0] to zeros. Hence when stress_ta returns it will
 * contain:
 *   00 of every byte of memref[0]
 *   01 of every byte of memref[1]
 *   02 of every byte of memref[2]
 *   03 of every byte of memref[3]
 *
 * User space check that the buffers contains these values and if everything is
 * OK, this test case is considered to pass.
 *
 * @return 0 if OK, otherwise number of fails.
 */
static uint32_t test_01_malloc_memcpy(void)
{
    TEEC_Context ctx = 0;
    TEEC_ErrorOrigin org = TEEC_ORIGIN_API;
    TEEC_Operation op;
    TEEC_Result teec_result = TEEC_ERROR_GENERIC;
    TEEC_Session session;
    TEEC_UUID uuid = STRESS_TA_UUID;

    uint32_t result = OPERATION_OK;
    uint32_t fails = 0;
    uint32_t i = 0;

    memset(&op, 0, sizeof(TEEC_Operation));
    memset(&session, 0, sizeof(TEEC_Session));

    PAUSE;
    teec_result = TEEC_InitializeContext(NULL, &ctx);
    if (teec_result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_InitializeContext failed, code 0x%08x\n",
                teec_result);
        result |= TEEC_INVOKE_COMMAND_FAILED;
        goto out;
    }

    PAUSE;
    teec_result = TEEC_OpenSession(&ctx, &session, &uuid, TEEC_LOGIN_PUBLIC,
                                   NULL, NULL, &org);
    if (teec_result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_OpenSession failed code 0x%08x\n", teec_result);
        result |= TEEC_OPEN_SESSION_FAILED;
        goto out;
    }

    for (i = 0; i < MALLOC_ARRAY_SIZE; ++i) {
        PAUSE;
#ifdef STRESS_TEST_DEBUG
        dprintf(INFO, "Testing buffer size: 0x%08x (loop: %d)\n",
                malloc_sizes[i], i);
#endif
        alloc_memref_buffers(&op, malloc_sizes[i]);
        memset(op.memRefs[0].buffer, 1, malloc_sizes[i]);
        memset(op.memRefs[1].buffer, 2, malloc_sizes[i]);
        memset(op.memRefs[2].buffer, 3, malloc_sizes[i]);
        memset(op.memRefs[3].buffer, 4, malloc_sizes[i]);

        op.flags = TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED |
            TEEC_MEMREF_2_USED | TEEC_MEMREF_3_USED;

        PAUSE;
        teec_result = TEEC_InvokeCommand(&session, TEST_01_MALLOC_MEMCPY,
                                         &op, &org);
        if (teec_result != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_InvokeCommand failed, code 0x%08x\n",
                    teec_result);
            result |= TEEC_INVOKE_COMMAND_FAILED;
            usleep(500000);
            fails++;
        }

        if (check_buffers_from_sec_world(&op)) {
            dprintf(ERROR, "Buffers from secure world not as expected!\n");
        }

        free_memref_buffers(&op);
    }

out:
    if ((result & TEEC_INVOKE_COMMAND_FAILED) == 0 &&
        (result & TEEC_CLOSE_SESSION_FAILED) == 0) {
        teec_result = TEEC_CloseSession(&session);
        if (teec_result != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_CloseSession failed, code 0x%08x\n",
                    teec_result);
            result |= TEEC_CLOSE_SESSION_FAILED;
        }
    }

    if ((result & TEEC_INVOKE_COMMAND_FAILED) == 0) {
        teec_result = TEEC_FinalizeContext(&ctx);
        if (teec_result != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_FinalizeContext failed, code 0x%08x\n",
                    teec_result);
            result |= TEEC_FINALIZE_CONTEXT_FAILED;
        }
    }

    if (fails) {
        dprintf(ERROR, "FAILED! (%d allocations) %d pass / %d fail\n",
                MALLOC_ARRAY_SIZE, (MALLOC_ARRAY_SIZE - fails), fails);
    } else {
        dprintf(ALWAYS, "PASSED! (%d allocations)\n", MALLOC_ARRAY_SIZE);
    }

    return result;
}

/*
 * This test case will test how tee handles multiple sessions, i.e. that the
 * device in Linux /dev/tee can handle to be opened by multiple applications in
 * the same time. We open 1019 connections to tee driver in this case. The
 * reason for 1019 is that a process can open at most 1023 files (after this the
 * kernel will return EMFILE) and we have stdout, stderr, stdin already opened.
 *
 * @return 0 if OK, otherwise number of failing initializes/finalizes.
 */
static uint32_t test_02_multiple_initialize_finalize(void)
{
    /* Explanation to 1019 is in the function header. */
    const uint32_t number_of_tests = 1019;
    uint32_t i = 0;
    uint32_t fail_initialize = 0;
    uint32_t fail_finalize = 0;
    TEEC_Context ctx[number_of_tests];
    TEEC_Result result = TEEC_ERROR_GENERIC;

    for (i = 0; i < number_of_tests; ++i) {
        result = TEEC_InitializeContext(NULL, &ctx[i]);
        if (result != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_InitializeContext failed ctx[%d]\n", i);
            fail_initialize++;
        }
    }

    for (i = 0; i < number_of_tests; ++i) {
        result = TEEC_FinalizeContext(&ctx[i]);
        if (result != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_FinalizeContext failed ctx[%d]\n", i);
            fail_finalize++;
        }
    }

    if (fail_initialize || fail_finalize) {
        dprintf(ERROR, "FAILED! Initialize/Finalize failed (%d/%d) times\n",
                fail_initialize, fail_finalize);
    } else {
        dprintf(ALWAYS, "PASSED!\n");
    }

    return fail_initialize + fail_finalize;
}

/*
 * Negative test case for testing tee when open a session with incorrect
 * parameters.
 *
 * @return 0 if OK, otherwise number of fails.
 */
static uint32_t test_03_opensession_only(void)
{
    TEEC_Context ctx = 0xDEADDEAD;
    TEEC_ErrorOrigin org = TEEC_ORIGIN_API;
    TEEC_Result result = TEEC_ERROR_GENERIC;
    TEEC_Session session;
    TEEC_UUID uuid = STRESS_TA_UUID;
    TEEC_UUID unknown_uuid = UNKNOWN_TA_UUID;
    uint32_t fails = 0;

    result = TEEC_OpenSession(&ctx, &session, &uuid, TEEC_LOGIN_PUBLIC,
                              NULL, NULL, &org);
    if (result == TEEC_SUCCESS) {
        dprintf(ERROR, "Could open session with bogus context\n");
        fails++;
    }

    result = TEEC_OpenSession(NULL, &session, &uuid, TEEC_LOGIN_PUBLIC,
                              NULL, NULL, &org);
    if (result == TEEC_SUCCESS) {
        dprintf(ERROR, "Could open session with NULL context\n");
        fails++;
    }

    result = TEEC_OpenSession(&ctx, NULL, &uuid, TEEC_LOGIN_PUBLIC, NULL,
                              NULL, &org);
    if (result == TEEC_SUCCESS) {
        dprintf(ERROR, "Could open session with NULL session\n");
        fails++;
    }

    result = TEEC_OpenSession(&ctx, &session, &unknown_uuid,
                              TEEC_LOGIN_PUBLIC, NULL, NULL, &org);
    if (result == TEEC_SUCCESS) {
        dprintf(ERROR, "Could open session with bogus context and unknown "
                "uuid\n");
        fails++;
    }

    if (fails) {
        dprintf(ERROR, "FAILED! %d pass / %d failed\n", 4-fails, fails);
    } else {
        dprintf(ALWAYS, "PASSED!\n");
    }

    return fails;
}

/*
 * Testing everything except doing the invoke.
 *
 * @return 0 if OK, otherwise number of fails.
 */
static uint32_t test_04_init_open_close_finalize(void)
{
    TEEC_Context ctx = 0;
    TEEC_ErrorOrigin org = TEEC_ORIGIN_API;
    TEEC_Result result = TEEC_ERROR_GENERIC;
    TEEC_Session session;
    TEEC_UUID uuid = STRESS_TA_UUID;
    uint32_t fails = 0;

    result = TEEC_InitializeContext(NULL, &ctx);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_InitializeContext failed\n");
        fails++;
    }

    result = TEEC_OpenSession(&ctx, &session, &uuid, TEEC_LOGIN_PUBLIC,
                              NULL, NULL, &org);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "Could not open session\n");
        fails++;
    }

    result = TEEC_CloseSession(&session);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_CloseSession failed\n");
        fails++;
    }

    result = TEEC_FinalizeContext(&ctx);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_FinalizeContext failed\n");
        fails++;
    }

    if (fails) {
        dprintf(ERROR, "FAILED!\n");
    } else {
        dprintf(ALWAYS, "PASSED!\n");
    }

    return fails;
}

/*
 * Helper function that is called when we spawn a new thread in test case 05.
 * This function will do a complete call to secure world (actually the same as
 * done in test_01_malloc_memcpy).
 *
 * This function will update the global static variable fail_counter_tc5 when
 * something goes wrong.
 *
 * @return NULL
 */
static void *test_05_thread_func(void *ptr)
{
    uint32_t i = 0;
    uint32_t loop_ctr = 0;
    uint32_t fails = 0;
    uint32_t result = OPERATION_OK;

    (void)ptr;

    TEEC_Context ctx = 0;
    TEEC_ErrorOrigin org = TEEC_ORIGIN_API;
    TEEC_Operation op;
    TEEC_Result teec_result = TEEC_ERROR_GENERIC;
    TEEC_Session session;
    TEEC_UUID uuid = STRESS_TA_UUID;
#ifdef STRESS_TEST_DEBUG
    signed long thread_id = pthread_self();
#endif

    while (loop_ctr < TEST_05_NUMBER_OF_LOOPS) {
        i = get_rand(0, MALLOC_ARRAY_SIZE - 1);
#ifdef STRESS_TEST_DEBUG
        dprintf(INFO, "Thread number %ld running loop %d using "
                "size 0x%08x...\n", thread_id, loop_ctr,
                malloc_sizes[i]);
#endif

        /* Sleep a random number between 0 and 20000us. */
        usleep(get_rand(0, 20000));

        ctx = 0;
        memset(&op, 0, sizeof(TEEC_Operation));
        memset(&session, 0, sizeof(TEEC_Session));

        teec_result = TEEC_InitializeContext(NULL, &ctx);
        if (teec_result != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_InitializeContext failed\n");
            fails++;
            result |= TEEC_INVOKE_COMMAND_FAILED;
            goto out;
        }

        teec_result = TEEC_OpenSession(&ctx, &session, &uuid, TEEC_LOGIN_PUBLIC,
                                       NULL, NULL, &org);
        if (teec_result != TEEC_SUCCESS) {
            dprintf(ERROR, "Could not open session\n");
            fails++;
            result |= TEEC_OPEN_SESSION_FAILED;
            goto out;
        }

        alloc_memref_buffers(&op, malloc_sizes[i]);
        memset(op.memRefs[0].buffer, 1, malloc_sizes[i]);
        memset(op.memRefs[1].buffer, 2, malloc_sizes[i]);
        memset(op.memRefs[2].buffer, 3, malloc_sizes[i]);
        memset(op.memRefs[3].buffer, 4, malloc_sizes[i]);

        op.flags = TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED |
            TEEC_MEMREF_2_USED | TEEC_MEMREF_3_USED;

        teec_result = TEEC_InvokeCommand(&session, TEST_01_MALLOC_MEMCPY, &op,
                                         &org);

        if (teec_result != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_InvokeCommand failed, error 0x%08x\n",
                    teec_result);
            fails++;
            result |= TEEC_INVOKE_COMMAND_FAILED;
        }

        free_memref_buffers(&op);

out:
        if ((result & TEEC_INVOKE_COMMAND_FAILED) == 0 &&
            (result & TEEC_CLOSE_SESSION_FAILED) == 0) {
            teec_result = TEEC_CloseSession(&session);
            if (teec_result != TEEC_SUCCESS) {
                dprintf(ERROR, "TEEC_CloseSession failed\n");
                result |= TEEC_CLOSE_SESSION_FAILED;
            }
        }

        if ((result & TEEC_INVOKE_COMMAND_FAILED) == 0) {
            teec_result = TEEC_FinalizeContext(&ctx);
            if (teec_result != TEEC_SUCCESS) {
                dprintf(ERROR, "TEEC_FinalizeContext failed\n");
                fails++;
                result |= TEEC_FINALIZE_CONTEXT_FAILED;
            }
        }

        if (fails) {
            pthread_mutex_lock(&fail_mutex);
            fail_counter_tc5++;
            pthread_mutex_unlock(&fail_mutex);
        }

        loop_ctr++;
    }

    return NULL;
}

/*
 * This function will test tee by opening several threads which all will try to
 * do operations on the stress ta in the secure world.
 *
 * @return 0 if OK, otherwise number of fails.
 */
static uint32_t test_05_multiple_full_access(void)
{
    size_t i = 0;
    pthread_t thread_id[TEST_05_NBR_THREADS];
    fail_counter_tc5 = 0;

    for (i = 0; i < TEST_05_NBR_THREADS; ++i) {
        pthread_create(&thread_id[i], NULL, test_05_thread_func, NULL);
    }

    for (i = 0; i < TEST_05_NBR_THREADS; ++i) {
        pthread_join(thread_id[i], NULL);
    }

    if (fail_counter_tc5) {
        dprintf(ERROR, "FAILED! %d pass / %d failed\n",
                TEST_05_NUMBER_OF_LOOPS - fail_counter_tc5,
                fail_counter_tc5);
    } else {
        dprintf(ALWAYS, "PASSED!\n");
    }

    return fail_counter_tc5;
}

/* Typedef to the test case function pointer. */
typedef uint32_t (*test_case)(void);

/* Array of function pointers to test functions. */
static test_case test_cases[] = {
    &test_01_malloc_memcpy,
    &test_02_multiple_initialize_finalize,
    &test_03_opensession_only,
    &test_04_init_open_close_finalize,
    &test_05_multiple_full_access
};

uint32_t bass_app_stress_test(void)
{
    uint32_t failing_test_cases = 0;
    size_t i = 0;

    for (i = 0; i < sizeof(test_cases) / sizeof(test_case); i++) {
        if (test_cases[i]()) {
            failing_test_cases |= (1 << i);
        }
        usleep(SLEEP_AFTER_SYS_STAT);
    }

    if (failing_test_cases) {
        for (i = 0; i < sizeof(test_cases) / sizeof(test_case); i++) {
            if (failing_test_cases & (i << 1)) {
                /* Increase with one since the array is zero indexed. */
                dprintf(ERROR, "Test case %d failed\n", i + 1);
            }
        }
        dprintf(ERROR, "The test cases above failed, for more information see "
                "the log file above!\n");
    } else {
        dprintf(ALWAYS, "All tee stress tests passed!\n");
    }

    return failing_test_cases;
}
