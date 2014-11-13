// ctest.c
//
// https://github.com/tylerneylon/cstructs
//
// A minimalistic test framework.
//

#include "ctest.h"

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Platform-specific includes.
#ifdef _WIN32
#include "winutil.h"
#include <process.h>
#define getpid _getpid
#else
#include <alloca.h>
#include <execinfo.h>
#include <libgen.h>
#include <unistd.h>
#endif


////////////////////////////////////////////////////////
// Constants and static variables.

#define LOG_SIZE 65536
static char *program_name;
static char *test_name;  // A program may run multiple tests.
static char log[LOG_SIZE];
static char *log_cursor;
static char *log_end;
static int log_is_verbose = 0;

////////////////////////////////////////////////////////
// Static (internal) function definitions.

static void print_trace() {
  // Windows doesn't give us easy access to stack traces. For more debug power,
  // turn off the seg fault handler and run the test with visual
  // studio's debug tools. Another option would be to integrate this class:
  // http://stackwalker.codeplex.com/
#ifndef _WIN32
  void *array[10];
  size_t size = backtrace(array, 10);
  char **strings = backtrace_symbols(array, size);
  for (size_t i = 0; i < size; ++i) test_printf("%s\n", strings[i]);
  free(strings);
#endif
}

static void handle_seg_fault(int sig) {
  print_trace();
  test_failed("pid %d received signal %d %s", getpid(), sig, sig == SIGSEGV ? "(SIGSEGV)" : "");
}

////////////////////////////////////////////////////////
// Public function definitions.

void start_all_tests(char *name) {
  program_name = basename(name);
  printf("%s - running ", program_name);
  if (log_is_verbose) {
    printf("\n");
  } else {
    fflush(stdout);
  }
  signal(SIGSEGV, handle_seg_fault);
  log_end = log + LOG_SIZE;
}

int end_all_tests() {
  printf("\r%s - passed \n", program_name);
  return 0;
}

void run_test_(TestFunction test_fn, char *new_test_name) {
  log[0] = '\0';
  log_cursor = log;
  test_name = new_test_name;
  if(test_fn() != test_success) test_failed("Test returned with failed status.");
}

void run_tests_(char *test_names, ...) {
  va_list args;
  va_start(args, test_names);

  char *names = strdup(test_names);
  char *token;
  while ((token = strsep(&names, " \t\n,")) != NULL) {
    if (*token == '\0') continue;  // Avoid empty tokens.
    run_test_(va_arg(args, TestFunction), token);
  }

  va_end(args);
  free(names);
}

int test_printf_(const char *format, ...) {
  int chars_printed;
  va_list args;

  if (log_is_verbose) {

    va_start(args, format);
    chars_printed = vprintf(format, args);
    va_end(args);

  } else {

    static char buffer[LOG_SIZE];
    va_start(args, format);
    chars_printed = vsnprintf(buffer, LOG_SIZE, format, args);
    va_end(args);

    long size_left = log_end - log_cursor - 1;  // Leave room for the last \0.
    if (size_left < chars_printed) {
      // Truncate it if it's too long.
      buffer[size_left] = '\0';
      chars_printed = size_left;
    }
    // It's important that chars_out <= size_left here; the above code ensures this.
    strncpy(log_cursor, buffer, chars_printed);
    log_cursor += chars_printed;
  }

  return chars_printed;
}

void test_that_(int cond, char *cond_str, char *filename, int line_number) {
  if (cond) return;
  test_printf("%s:%d: ", basename(filename), line_number);
  test_printf("The following condition failed: %s\n", cond_str);
  test_failed("");
}

void test_str_eq_(const char *s1, const char *s2, char *filename, int line_number) {
  if (strcmp(s1, s2) == 0) return;
  test_failed("%s:%d: These strings were not equal: \"%s\" vs \"%s\"\n",
              basename(filename), line_number, s1, s2);
}

void test_failed(char *reason_fmt, ...) {
  if (!log_is_verbose) {
    // If log_is_verbose, then the log is already printed out by now.
    printf("\r%s - failed \n\n", program_name);
    printf("Failed in test '%s'; log follows:\n---\n", test_name);
    int ends_with_newline = log[strlen(log) - 1] == '\n';
    printf("%s%s---\n", log, ends_with_newline ? "" : "\n");
  }
  if (reason_fmt && strlen(reason_fmt)) {
    va_list args;
    va_start(args, reason_fmt);

    size_t fmt_size = strlen(reason_fmt) + 3;  // 3 chars for \, n, and \0.
    char  *fmt      = alloca(fmt_size);

    snprintf(fmt, fmt_size, "%s\n", reason_fmt);
    vprintf (fmt, args);

    va_end(args);
  }
  printf("%s failed while running test %s.\n\n", program_name, test_name);
  exit(test_failure);
}

void set_verbose(int be_verbose) {
  log_is_verbose = be_verbose;
}
