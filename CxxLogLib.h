#ifndef _CXX_LOG_LIB_H_
#define _CXX_LOG_LIB_H_

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

enum CLL_LogLevel
{
  CLL_LOG_INFO = 0,
  CLL_LOG_DEBUG,
  CLL_LOG_WARNING,
  CLL_LOG_ERROR,
  CLL_LOG_FATAL
};

void CLL_init();
void CLL_setLoggingStream(FILE *stream);

#define CLL_log(level, format, ...)\
  __CLL_log(level, __ASSERT_FUNCTION, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define CLL_assert(condition, format, ...)\
  if (!condition)\
  {\
    __CLL_log(CLL_LOG_FATAL, __ASSERT_FUNCTION, __LINE__,\
              __FILE__, format, ##__VA_ARGS__);\
  }

#define CLL_inform(format, ...) CLL_log(CLL_LOG_INFO, format, ##__VA_ARGS__)
#define CLL_debug(format, ...) CLL_log(CLL_LOG_DEBUG, format, ##__VA_ARGS__)
#define CLL_warn(format, ...) CLL_log(CLL_LOG_WARNING, format, ##__VA_ARGS__)
#define CLL_error(format, ...) CLL_log(CLL_LOG_ERROR, format, ##__VA_ARGS__)
#define CLL_panic(format, ...) CLL_log(CLL_LOG_FATAL, format, ##__VA_ARGS__)

void __CLL_log(enum CLL_LogLevel level, const char *func, int line,
               const char *file, const char *format, ...);

#endif // _CXX_LOG_LIB_H
