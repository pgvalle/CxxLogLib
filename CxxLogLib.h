#ifndef _CXX_LOG_LIB_H_
#define _CXX_LOG_LIB_H_

#include <stdbool.h>
#include <stdio.h>

enum CLL_LogType
{
  CLL_INFO = 0,
  CLL_DEBUG,
  CLL_WARNING,
  CLL_ERROR,
  CLL_FATAL
};

// The library will consider the main thread as the one you call this function in.
// Set options after calling this.
void CLL_init();
// Safe to call in any thread, but only works in the thread CLL_init was called.
void CLL_quit();

// Safe to call in any thread, but only works in the thread CLL_init was called.
void CLL_set_stream(FILE *stream);
// Colors get messed up in non-console/non-terminal streams.
// Safe to call in any thread, but only works in the thread CLL_init was called.
void CLL_set_colors(bool colors);

#define CLL_log(type, fmt, ...) _CLL_log(type, __func__, fmt, ##__VA_ARGS__)

#define CLL_assert(condition, fmt, ...)\
  if (!condition)\
  {\
    CLL_fatal(fmt, ##__VA_ARGS__);\
  }

#define CLL_info(fmt, ...) CLL_log(CLL_INFO, fmt, ##__VA_ARGS__)
#define CLL_debug(fmt, ...) CLL_log(CLL_DEBUG, fmt, ##__VA_ARGS__)
#define CLL_warning(fmt, ...) CLL_log(CLL_WARNING, fmt, ##__VA_ARGS__)
#define CLL_error(fmt, ...) CLL_log(CLL_ERROR, fmt, ##__VA_ARGS__)
#define CLL_fatal(fmt, ...) CLL_log(CLL_FATAL, fmt, ##__VA_ARGS__)

void _CLL_log(enum CLL_LogType type, const char *func, const char *fmt, ...);

#endif // _CXX_LOG_LIB_H
