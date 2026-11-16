#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../../linkja-crypto.h"

static void test_hash_string_null(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	assert_false(hash_string(NULL, output));
}

static void test_hash_string_empty(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	assert_true(hash_string("", output));
	char result[HASH_STRING_OUTPUT_BUFFER_LEN];
  bytes_to_hex_string(output, HASH_OUTPUT_BUFFER_LEN, result, HASH_STRING_OUTPUT_BUFFER_LEN);
  assert_string_equal(result, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	// For SHA-512: assert_string_equal(result, "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
}

static void test_hash_string_value(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	assert_true(hash_string("test", output));
	char result[HASH_STRING_OUTPUT_BUFFER_LEN];
  bytes_to_hex_string(output, HASH_OUTPUT_BUFFER_LEN, result, HASH_STRING_OUTPUT_BUFFER_LEN);
  assert_string_equal(result, "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08");
	// For SHA-512: assert_string_equal(result, "ee26b0dd4af7e749aa1a8ee3c10ae9923f618980772e473f8819a5d4940e0db27ac185f8a0e1d5f84f88bc887fd67b143732c304cc5fa9ad8e6f57f50028a8ff");
}

static void test_hash_data_null(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	assert_false(hash_string(NULL, output));
}

static void test_hash_data_empty(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	unsigned char data[] = { 0x0 };
	assert_true(hash_data(data, 0, output));
	char result[HASH_STRING_OUTPUT_BUFFER_LEN];
  bytes_to_hex_string(output, HASH_OUTPUT_BUFFER_LEN, result, HASH_STRING_OUTPUT_BUFFER_LEN);
  assert_string_equal(result, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	// For SHA-512: assert_string_equal(result, "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
}

static void test_hash_data_value(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	unsigned char data[] = { 't', 'e', 's', 't' };
	assert_true(hash_data(data, 4, output));
	char result[HASH_STRING_OUTPUT_BUFFER_LEN];
  bytes_to_hex_string(output, HASH_OUTPUT_BUFFER_LEN, result, HASH_STRING_OUTPUT_BUFFER_LEN);
  assert_string_equal(result, "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08");
	// For SHA-512: assert_string_equal(result, "ee26b0dd4af7e749aa1a8ee3c10ae9923f618980772e473f8819a5d4940e0db27ac185f8a0e1d5f84f88bc887fd67b143732c304cc5fa9ad8e6f57f50028a8ff");
}

// revert_hash is the C part of revertSecureHash.  Its input is the hex string of a
// secure hash, so it must be exactly 64 hex characters.
static const unsigned char revert_supplemental[HASH_OUTPUT_BUFFER_LEN] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

static void test_revert_hash_null(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	assert_false(revert_hash(NULL, 64, revert_supplemental, output));
}

static void test_revert_hash_empty(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	assert_false(revert_hash("", 0, revert_supplemental, output));
}

static void test_revert_hash_10_chars(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	assert_false(revert_hash("0123456789", 10, revert_supplemental, output));
}

static void test_revert_hash_63_chars(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	const char *input = "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
	assert_int_equal(strlen(input), 63);
	assert_false(revert_hash(input, 63, revert_supplemental, output));
}

static void test_revert_hash_65_chars(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	const char *input = "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
	assert_int_equal(strlen(input), 65);
	assert_false(revert_hash(input, 65, revert_supplemental, output));
}

// sscanf("%2hhx") accepts a partial match, leading whitespace and a sign, so
// revert_hash must check each character itself.
static void test_revert_hash_64_z(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	const char *input = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";
	assert_int_equal(strlen(input), 64);
	assert_false(revert_hash(input, 64, revert_supplemental, output));
}

static void test_revert_hash_partial_hex_pairs(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	const char *input = "0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z";
	assert_int_equal(strlen(input), 64);
	assert_false(revert_hash(input, 64, revert_supplemental, output));
}

static void test_revert_hash_leading_space_pairs(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	const char *input = " f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f f";
	assert_int_equal(strlen(input), 64);
	assert_false(revert_hash(input, 64, revert_supplemental, output));
}

static void test_revert_hash_valid_upper_case(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	const char *input = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
	assert_int_equal(strlen(input), 64);
	assert_true(revert_hash(input, 64, revert_supplemental, output));
	char result[HASH_STRING_OUTPUT_BUFFER_LEN];
	bytes_to_hex_string(output, HASH_OUTPUT_BUFFER_LEN, result, HASH_STRING_OUTPUT_BUFFER_LEN);
	assert_string_equal(result, "fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0efeeedecebeae9e8e7e6e5e4e3e2e1e0");
}

static void test_revert_hash_valid(void **state) {
	unsigned char output[HASH_OUTPUT_BUFFER_LEN];
	const char *input = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
	assert_int_equal(strlen(input), 64);
	assert_true(revert_hash(input, 64, revert_supplemental, output));
	char result[HASH_STRING_OUTPUT_BUFFER_LEN];
	bytes_to_hex_string(output, HASH_OUTPUT_BUFFER_LEN, result, HASH_STRING_OUTPUT_BUFFER_LEN);
	assert_string_equal(result, "fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0efeeedecebeae9e8e7e6e5e4e3e2e1e0");
}

int main(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_hash_string_null),
		cmocka_unit_test(test_hash_string_empty),
		cmocka_unit_test(test_hash_string_value),
		cmocka_unit_test(test_hash_data_null),
		cmocka_unit_test(test_hash_data_empty),
		cmocka_unit_test(test_hash_data_value),
		cmocka_unit_test(test_revert_hash_null),
		cmocka_unit_test(test_revert_hash_empty),
		cmocka_unit_test(test_revert_hash_10_chars),
		cmocka_unit_test(test_revert_hash_63_chars),
		cmocka_unit_test(test_revert_hash_65_chars),
		cmocka_unit_test(test_revert_hash_64_z),
		cmocka_unit_test(test_revert_hash_partial_hex_pairs),
		cmocka_unit_test(test_revert_hash_leading_space_pairs),
		cmocka_unit_test(test_revert_hash_valid_upper_case),
		cmocka_unit_test(test_revert_hash_valid)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
