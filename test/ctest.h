// ctest.h
//
// https://github.com/tylerneylon/cstructs
//
// A minimalistic test framework.
//

#ifndef __CTEST_CTEST_H__
#define __CTEST_CTEST_H__


////////////////////////////////////////////////////////
// Begin and end functions.

// Send in the program name to start_all_tests.
void start_all_tests(char *name);
int end_all_tests();


////////////////////////////////////////////////////////
// Run a single or multiple tests.

// The parameters are pointers to functions that
// return an int and accept no parameters. Valid
// return values are defined in an enum below.
#define run_test(x) run_test_(x, #x)
#define run_tests(...) run_tests_(#__VA_ARGS__, __VA_ARGS__)


////////////////////////////////////////////////////////
// Things to call from within a test function or any
// function called by a test function.

// Use test_printf just like printf; the output is stored in memory
// and only printed if the test fails.
#define test_printf(...) test_printf_(__VA_ARGS__)

// Send boolean expressions into test_that. If they evaluate to false,
// the test is considered failing.
#define test_that(cond) test_that_(cond, #cond, __FILE__, __LINE__)

// A special case of test_that behavior for testing string equality.
#define test_str_eq(s1, s2) test_str_eq_(s1, s2, __FILE__, __LINE__)

// Directly call test_failed if you perform a custom check.
void test_failed(char *reason_fmt, ...);

// Turn on or off the immediate output from test_printf calls; off by default.
void set_verbose(int be_verbose);

////////////////////////////////////////////////////////
// Constants accepted as test function return values.

enum {
  test_success = 0,
  test_failure = 1
};


////////////////////////////////////////////////////////
// Implementations behind the macros.
// Do not call these directly.

typedef int (*TestFunction)();
void run_test_(TestFunction test_fn, char *new_test_name);
void run_tests_(char *test_names, ...);
int test_printf_(const char *format, ...);
void test_that_(int cond, char *cond_str, char *filename, int line_number);
void test_str_eq_(const char *s1, const char *s2, char *filename, int line_number);

#endif // __CTEST_CTEST_H__
