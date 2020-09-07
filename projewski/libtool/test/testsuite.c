#include <check.h>

#include "stringsuite.h"

Suite* masterSuite() {
    Suite *suite;
    suite = suite_create("libtool test suite");
    return suite;
}

int main() {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = masterSuite();
    sr = srunner_create(s);
    srunner_add_suite(sr, stringSuite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? 0 : 1;
}