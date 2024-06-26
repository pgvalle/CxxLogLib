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

// ansi escape sequences don't work in cmd by default
void fix_windows_console()
{
  const DWORD handles2fix[] = {
    STD_OUTPUT_HANDLE, STD_ERROR_HANDLE
  };

  for (int i = 0; i < 2; i++)
  {
    const HANDLE handle = GetStdHandle(handles2fix[i]);

    DWORD options;
    GetConsoleMode(handle, &options);
    SetConsoleMode(handle, options | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  }
}
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
  if (_.initialized)
  {
    return;
  }

  _.initialized = true;

  _.stream = stdout;
  _.colors = false;

  _.main_thread = pthread_self();
  pthread_mutex_init(&_.log_mutex, NULL);

#ifdef _WIN32
  fix_windows_console();
#endif
}

bool _is_main_thread()
{
  return _.main_thread == pthread_self();
}

void CLL_quit()
{
  if (_.initialized && _is_main_thread())
  {
    _.initialized = false;
    pthread_mutex_destroy(&_.log_mutex);
  }
}

void CLL_set_stream(FILE *stream)
{
  if (_.initialized && _is_main_thread())
  {
    _.stream = stream;
  }
}

void CLL_set_colors(bool colors)
{
  if (_.initialized && _is_main_thread())
  {
    _.colors = colors;
  }
}

const char *_get_log_type_str(enum CLL_LogType type)
{
  static const char *TYPES[] = {
    "INFO", "DEBUG", "WARNING", "ERROR", "FATAL"
  };

  return TYPES[type];
}

const char *_get_log_type_color_str(enum CLL_LogType type)
{
  static const char *COLORS[] = {
    "\033[32m", "\033[36m", "\033[33m", "\033[31m", "\033[35m"
  };

  return COLORS[type];
}

void _log_meta_info(enum CLL_LogType type, const char *func)
{
  const time_t t = time(NULL);
  const struct tm *lt = localtime(&t);

  fprintf(_.stream, "[ %4d-%02d-%02d %02d:%02d:%02d %s ",
          lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
          lt->tm_hour, lt->tm_min, lt->tm_sec,
          func);

  const char *type_str = _get_log_type_str(type);

  if (_.colors)
  {
    const char *color_str = _get_log_type_color_str(type);
    fprintf(_.stream, "%s%s\033[0m ] ", color_str, type_str);
  }
  else
  {
    fprintf(_.stream, "%s ] ", type_str);
  }
}

void _CLL_log(enum CLL_LogType type, const char *func, const char *fmt, ...)
{
  if (!_.initialized)
  {
    return;
  }

  pthread_mutex_lock(&_.log_mutex);  // entire log function should be atomic
  
  _log_meta_info(type, func);

  // log message itself
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
