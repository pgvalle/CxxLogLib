#ifndef _CXX_LOG_LIB_H_
#define _CXX_LOG_LIB_H_

#include <stdio.h>
#include <stdbool.h>

enum CLL_LogType
{
  CLL_INFO = 0,
  CLL_DEBUG,
  CLL_WARNING,
  CLL_ERROR,
  CLL_FATAL
};

// The library will consider the main thread as the one you call this function in.
void CLL_init();
void CLL_quit();

// Safe to call in any thread, but only works in the thread CLL_init was called.
void CLL_setLogStream(FILE *stream);
// Colors get messed up in non-console/non-terminal streams.
// Safe to call in any thread, but only works in the thread CLL_init was called.
void CLL_setColors(bool colors);

// You can call it only once per second and in the main thread.
// Prefix is a non-empty string that must be up to 32 characters.
FILE *CLL_createLogFile(const char *name);

#define CLL_log(level, format, ...)\
  __CLL_log(level, __func__, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define CLL_assert(condition, format, ...)\
  if (!condition)\
  {\
    CLL_log(CLL_FATAL, format, ##__VA_ARGS__);\
  }

#define CLL_info(format, ...) CLL_log(CLL_INFO, format, ##__VA_ARGS__)
#define CLL_debug(format, ...) CLL_log(CLL_DEBUG, format, ##__VA_ARGS__)
#define CLL_warning(format, ...) CLL_log(CLL_WARNING, format, ##__VA_ARGS__)
#define CLL_error(format, ...) CLL_log(CLL_ERROR, format, ##__VA_ARGS__)
#define CLL_fatal(format, ...) CLL_log(CLL_FATAL, format, ##__VA_ARGS__)

void __CLL_log(enum CLL_LogType type, const char *func, int line,
               const char *file, const char *format, ...);

#endif // _CXX_LOG_LIB_H
