#include "global-inc.h"
#include <mystr.h>
#include <stdio.h>
#include <check.h>
#include <stdarg.h>

// string_assign
START_TEST(test_string_assign_null) {
    char *string = NULL;
    string_assign(string, NULL);
    ck_assert(NULL == string);
}
END_TEST

START_TEST(test_string_assign_to_null) {
    char *source = strdup("twelve");
    char *string = NULL;
    string_assign(string, source);
    ck_assert(source == string);
    string_free(string);
}
END_TEST

START_TEST(test_string_assign_to_not_null) {
    char *source = strdup("twelve");
    char *string = strdup("x");
    string_assign(string, source);
    ck_assert(source == string);
    string_free(string);
}
END_TEST

// string_set
START_TEST(test_string_set_to_null) {
    char *source = strdup("twelve");
    char *string = NULL;
    string_set(string, source);
    ck_assert(source != string);
    ck_assert(strcmp(string, source) == 0);
    string_free(source);
    string_free(string);
}
END_TEST

START_TEST(test_string_set_null_to_null) {
    char *string = NULL;
    string_set(string, NULL);
    ck_assert(string == NULL);
}

START_TEST(test_string_set_const_to_null) {
    const char *source = "twelve";
    char *string = NULL;
    string_set(string, source);
    ck_assert(source != string);
    ck_assert(strcmp(string, source) == 0);
    string_free(string);
}
END_TEST

START_TEST(test_string_set_to_not_null) {
    char *source = strdup("twelve");
    char *string = strdup("teststring");
    string_set(string, source);
    ck_assert(source != string);
    ck_assert(strcmp(string, source) == 0);
    string_free(source);
    string_free(string);
}
END_TEST

START_TEST(test_string_set_null_to_not_null) {
    char *string = strdup("teststring");
    string_set(string, NULL);
    ck_assert(string == NULL);
}

START_TEST(test_string_set_const_to_not_null) {
    const char *source = "twelve";
    char *string = strdup("teststring");
    string_set(string, source);
    ck_assert(source != string);
    ck_assert(strcmp(string, source) == 0);
    string_free(string);
}
END_TEST

START_TEST(test_string_alloc_free) {
    string_alloc(string, 10);
    ck_assert_ptr_nonnull(string);
    string_free(string);
    ck_assert_ptr_null(string);
}END_TEST

START_TEST(test_string_free_null) {
    char *string = NULL;
    string_free(string);
    ck_assert_ptr_null(string);
} END_TEST


void suite_addTestCase(Suite* suite, const char* testCaseName, ...) {
    va_list ap;
    TCase *testCase;
    va_start(ap, testCaseName);
    testCase = tcase_create("testCaseName");
    void* function = NULL;
    while ((function = va_arg(ap, void*))) {
        tcase_add_test(testCase, function);
    }
    va_end(ap);
    suite_add_tcase(suite, testCase);
}

Suite *stringSuite() {
    Suite *suite;
    //TCase *stringAssignTestCase;
    suite = suite_create("stringSuite");

    suite_addTestCase(suite, "string_assign",
                      test_string_assign_to_null,
                      test_string_assign_to_not_null,
                      test_string_assign_null,
                      NULL);

    suite_addTestCase(suite, "string_set",
                      test_string_set_to_null,
                      test_string_set_null_to_null,
                      test_string_set_const_to_null,
                      test_string_set_to_not_null,
                      test_string_set_null_to_not_null,
                      test_string_set_const_to_not_null,
                      NULL);

    suite_addTestCase(suite, "string_alloc_free",
                      test_string_alloc_free,
                      test_string_free_null,
                      NULL
                      );

//    stringAssignTestCase = tcase_create("string_assign");
//    tcase_add_test(stringAssignTestCase, string_assign_to_null);
//    tcase_add_test(stringAssignTestCase, string_assign_to_not_null);
//    suite_add_tcase(suite, stringAssignTestCase);
    return suite;
}

/*
int main()
{
	char *tmp = NULL;
	printf("%d\n", goc_stringEndsWithCase("xxxsd dsd s.MP2", ".mp3"));

	tmp = goc_stringCopy(tmp, "Alfa");
	tmp = goc_stringMultiAdd(tmp, "Beta", "Gamma", "Fi");
	printf("[%s]\n", tmp);

	tmp = goc_stringCopy(tmp, "Alfa");
	tmp = goc_stringMultiAdd(tmp, "Beta");
	printf("[%s]\n", tmp);

	tmp = goc_stringCopy(tmp, "Alfa");
	tmp = goc_stringMultiAdd(tmp, NULL);
	printf("[%s]\n", tmp);

	tmp = goc_stringCopy(tmp, "Alfa");
	tmp = goc_stringMultiAdd(tmp, NULL, "Beta", NULL, "Gamma", NULL);
	printf("[%s]\n", tmp);

	tmp = goc_stringMultiCopy(tmp, "Alfa", "Beta", "Gamma", "Fi");
	printf("[%s]\n", tmp);

	tmp = goc_stringMultiCopy(tmp, "Alfa", "Beta");
	printf("[%s]\n", tmp);

	tmp = goc_stringMultiCopy(tmp, "Alfa");
	printf("[%s]\n", tmp);

	tmp = goc_stringMultiCopy(tmp, "Alfa", NULL, "Beta", NULL, "Gamma");
	printf("[%s]\n", tmp);
}
*/