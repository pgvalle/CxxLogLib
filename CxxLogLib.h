#ifndef _CXX_LOG_LIB_H_
#define _CXX_LOG_LIB_H_

#include <stdio.h>
#include <stdbool.h>

enum CLL_LogType
{
  CLL_LOG_INFO = 0,
  CLL_LOG_DEBUG,
  CLL_LOG_WARN,
  CLL_LOG_ERROR,
  CLL_LOG_FATAL
};

void CLL_init();
void CLL_setLoggingStream(FILE *stream);
void CLL_setColors(bool colors);

#define CLL_log(level, format, ...)\
  __CLL_log(level, __func__, __LINE__, __FILE__, format, ##__VA_ARGS__)

#define CLL_assert(condition, format, ...)\
  if (!condition)\
  {\
    __CLL_log(CLL_LOG_FATAL, __func__, __LINE__,\
              __FILE__, format, ##__VA_ARGS__);\
  }

#define CLL_info(format, ...) CLL_log(CLL_LOG_INFO, format, ##__VA_ARGS__)
#define CLL_debug(format, ...) CLL_log(CLL_LOG_DEBUG, format, ##__VA_ARGS__)
#define CLL_warn(format, ...) CLL_log(CLL_LOG_WARN, format, ##__VA_ARGS__)
#define CLL_error(format, ...) CLL_log(CLL_LOG_ERROR, format, ##__VA_ARGS__)
#define CLL_fatal(format, ...) CLL_log(CLL_LOG_FATAL, format, ##__VA_ARGS__)

void __CLL_log(enum CLL_LogType type, const char *func, int line,
               const char *file, const char *format, ...);

#endif // _CXX_LOG_LIB_H
