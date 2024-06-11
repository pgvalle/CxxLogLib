#include "CxxLogLib.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#ifdef _WIN32
#include <windows.h>
#endif

static struct
{
  bool initialized;

  FILE *stream;
  bool colors;

  pthread_t main_thread;
  pthread_mutex_t log_mutex;
} _;

void CLL_init()
{
  // Call CLL_init only once. To call it again, call CLL_quit before.
  if (_.initialized)
  {
    return;
  }

  _.initialized = true;

  _.stream = stdout;
  _.colors = false;

  _.main_thread = pthread_self();
  pthread_mutex_init(&_.log_mutex, NULL);

  // windows bullshit to make ansi escape sequences work
#ifdef _WIN32
  const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  DWORD dwMode;
  GetConsoleMode(hOut, &dwMode);

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#endif  
}

void CLL_quit()
{
  if (_.initialized && _.main_thread == pthread_self())
  {
    _.initialized = false;
    pthread_mutex_destroy(&_.log_mutex);
  }
}

void CLL_set_log_stream(FILE *stream)
{
  if (_.initialized && _.main_thread == pthread_self())
  {
    _.stream = stream;
  }
}

void CLL_set_colors(bool colors)
{
  if (_.initialized && _.main_thread == pthread_self())
  {
    _.colors = colors;
  }
}

void _CLL_log(enum CLL_LogType type, const char *func, const char *fmt, ...)
{
  static const char *TYPE_INFO[2][5] = {
        "INFO",    "DEBUG",  "WARNING",    "ERROR",    "FATAL",
    "\033[32m", "\033[36m", "\033[33m", "\033[31m", "\033[35m"
  };

  if (!_.initialized)
  {
    return;
  }

  pthread_mutex_lock(&_.log_mutex);  // entire log function should be atomic
  
  const time_t t = time(NULL);
  const struct tm *lt = localtime(&t);

  const char *color_str = _.colors ? TYPE_INFO[1][type] : "",
             *type_str  = TYPE_INFO[0][type],
             *reset_str = _.colors ? "\033[0m" : "";

  fprintf(_.stream, "[ %4d-%02d-%02d %02d:%02d:%02d %s %s%s%s ] ",
          lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
          lt->tm_hour, lt->tm_min, lt->tm_sec,
          func,
          color_str, type_str, reset_str);

  va_list args;
  va_start(args, fmt);
  vfprintf(_.stream, fmt, args);
  va_end(args);

  fprintf(_.stream, "\n");
  fflush(_.stream);

  pthread_mutex_unlock(&_.log_mutex);  // Now we're safe
  
  if (type == CLL_FATAL)
  {
    exit(-1);
  }
}
