// winutil.h
//
// A set of windows-focused wrappers to enable easier
// cross-platform code.
//


#ifdef _WIN32

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>

#define strdup _strdup

static char *basename(char *path) {
  static char fname[_MAX_FNAME + _MAX_EXT];
  static char ext  [_MAX_EXT];
  _splitpath_s(
    path,
    NULL,  0,  // drive
    NULL,  0,  // dir
    fname, _MAX_FNAME,
    ext,   _MAX_EXT);
  strcat_s(fname, sizeof(fname), ext);
  return fname;
}

static char *strsep(char **stringp, const char *delim) {
  if (*stringp == NULL) { return NULL; }

  char *token_start = *stringp;
  *stringp = strpbrk(token_start, delim);
  if (*stringp) {
    **stringp = '\0';
    (*stringp)++;
  }

  return token_start;
}

#define vsnprintf(s, n, fmt, args) vsnprintf_s(s, n, _TRUNCATE, fmt, args)
#define strncpy(dst, src, num) strncpy_s(dst, num, src, _TRUNCATE)
#define snprintf(s, n, fmt, ...) sprintf_s(s, n, fmt, __VA_ARGS__)

static int asprintf(char **buffer, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  int num_chars = _vscprintf(fmt, args);
  *buffer = malloc(num_chars + 1);  // + 1 for the final '\0'.
  vsprintf_s(*buffer, num_chars + 1, fmt, args);

  va_end(args);

  return num_chars;
}

#endif
