#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Import the actual functions from the production code */
extern void * vsf_arch_heap_malloc(uint_fast32_t size, uint_fast32_t alignment);
extern void * vsf_arch_heap_realloc(void *buffer, uint_fast32_t size);
extern void vsf_arch_heap_free(void *buffer);

START_TEST(test_realloc_does_not_read_beyond_original_allocation)
{
    /* Invariant: When realloc grows a buffer, only the original allocation's
     * content (up to the original size) must be copied. Data beyond the
     * original size in the new buffer must NOT contain data from beyond
     * the original allocation (i.e., no out-of-bounds read). */

    struct { uint_fast32_t orig_size; uint_fast32_t new_size; } cases[] = {
        { 16, 4096 },    /* Exploit case: small -> much larger */
        { 1, 2 },        /* Boundary: minimal growth */
        { 64, 64 },      /* Valid: same size (no growth) */
        { 32, 128 },     /* Moderate growth */
    };
    int num_cases = sizeof(cases) / sizeof(cases[0]);

    for (int i = 0; i < num_cases; i++) {
        uint_fast32_t orig_size = cases[i].orig_size;
        uint_fast32_t new_size = cases[i].new_size;

        /* Allocate original buffer and fill with known pattern */
        uint8_t *buf = (uint8_t *)vsf_arch_heap_malloc(orig_size, 0);
        ck_assert_ptr_nonnull(buf);
        memset(buf, 0xAA, orig_size);

        /* Realloc to new (possibly larger) size */
        uint8_t *new_buf = (uint8_t *)vsf_arch_heap_realloc(buf, new_size);
        ck_assert_ptr_nonnull(new_buf);

        /* The original content must be preserved up to min(orig_size, new_size) */
        uint_fast32_t preserved = (orig_size < new_size) ? orig_size : new_size;
        for (uint_fast32_t j = 0; j < preserved; j++) {
            ck_assert_uint_eq(new_buf[j], 0xAA);
        }

        /* If grown, bytes beyond original size should be zero or indeterminate,
         * but critically the operation must not have crashed due to OOB read.
         * The fact we reach here without ASAN/MSAN/segfault is the key assertion. */

        vsf_arch_heap_free(new_buf);
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_realloc_does_not_read_beyond_original_allocation);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}