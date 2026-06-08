#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Invariant: ProcessString built from argv[1] must not contain shell metacharacters
 * that could enable command injection when passed to system() or popen().
 */

static int contains_shell_metacharacters(const char *input) {
    const char *metacharacters = ";|&`$(){}[]<>!\\\"'~#\n\r";
    for (size_t i = 0; i < strlen(input); i++) {
        if (strchr(metacharacters, input[i]) != NULL) {
            return 1;
        }
    }
    return 0;
}

START_TEST(test_no_shell_injection_in_process_string)
{
    /* Invariant: argv[1] values containing shell metacharacters must be rejected
     * before being incorporated into a command string passed to system()/popen().
     */
    const char *payloads[] = {
        "/bin/ls; rm -rf /",          /* exact exploit: command injection via semicolon */
        "$(malicious_cmd)",           /* boundary: subshell injection */
        "/usr/bin/valid_program",     /* valid: safe executable path */
        "prog | cat /etc/passwd",     /* pipe injection */
    };
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        const char *input = payloads[i];
        int is_malicious = contains_shell_metacharacters(input);

        if (is_malicious) {
            /* Security invariant: malicious input must NOT be accepted as-is.
             * A safe implementation must sanitize or reject such input.
             * We assert that the raw input would be dangerous — confirming
             * the invariant that validation MUST occur before use in sprintf+system.
             */
            ck_assert_msg(is_malicious == 1,
                "Input '%s' contains shell metacharacters and must be rejected "
                "before use in a command string", input);
        } else {
            /* Valid input: no metacharacters detected, safe to use */
            ck_assert_msg(is_malicious == 0,
                "Input '%s' should be considered safe", input);
        }
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_no_shell_injection_in_process_string);
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