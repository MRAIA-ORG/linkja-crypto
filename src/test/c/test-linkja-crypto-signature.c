#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <cmocka.h>

// src/CMakeLists.txt writes this header at configure time. The library returns
// LINKJA_SECRET_HASH from getLibrarySignature.
#include "../../include/linkja_secret.h"

#define SIGNATURE_LEN 32

static void test_signature_is_32_hex_characters(void **state) {
	(void)state;
	const char *signature = LINKJA_SECRET_HASH;
	assert_int_equal(strlen(signature), SIGNATURE_LEN);
	for (size_t i = 0; i < SIGNATURE_LEN; i++) {
		assert_true(isxdigit((unsigned char)signature[i]));
	}
}

int main(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_signature_is_32_hex_characters),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
