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
void fixWindowsConsole()
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

  pthread_t mainThread;
  pthread_mutex_t logLock;
} _;

bool _isMainThread()
{
  return _.mainThread == pthread_self();
}

void CLL_init()
{
  if (_.initialized)
  {
    return;
  }

  _.initialized = true;

  _.stream = stdout;
  _.colors = false;

  _.mainThread = pthread_self();
  pthread_mutex_init(&_.logLock, NULL);

#ifdef _WIN32
  fixWindowsConsole();
#endif
}

void CLL_quit()
{
  if (_.initialized && _isMainThread())
  {
    _.initialized = false;
    pthread_mutex_destroy(&_.logLock);
  }
}

void CLL_setStream(FILE *stream)
{
  if (_.initialized && _isMainThread())
  {
    _.stream = stream;
  }
}

void CLL_setColors(bool colors)
{
  if (_.initialized && _isMainThread())
  {
    _.colors = colors;
  }
}

const char *_getLogTypeStr(enum CLL_LogType type)
{
  static const char *TYPES[] = {
    "INFO", "DEBUG", "WARNING", "ERROR", "FATAL"
  };

  return TYPES[type];
}

const char *_getLogTypeColor(enum CLL_LogType type)
{
  static const char *COLORS[] = {
    "\033[32m", "\033[36m", "\033[33m", "\033[31m", "\033[35m"
  };

  return COLORS[type];
}

void _logMetaInfo(enum CLL_LogType type, const char *func)
{
  const time_t t = time(NULL);
  const struct tm *lt = localtime(&t);

  fprintf(_.stream, "[ %4d-%02d-%02d %02d:%02d:%02d %s ",
          lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
          lt->tm_hour, lt->tm_min, lt->tm_sec,
          func);

  const char *typeStr = _getLogTypeStr(type);

  if (_.colors)
  {
    const char *colorStr = _getLogTypeColor(type);
    fprintf(_.stream, "%s%s\033[0m ] ", colorStr, typeStr);
  }
  else
  {
    fprintf(_.stream, "%s ] ", typeStr);
  }
}

void _CLL_log(enum CLL_LogType type, const char *func, const char *fmt, ...)
{
  if (!_.initialized)
  {
    return;
  }

  pthread_mutex_lock(&_.logLock);  // entire log function should be atomic
  
  _logMetaInfo(type, func);

  // log message itself
  va_list args;
  va_start(args, fmt);
  vfprintf(_.stream, fmt, args);
  va_end(args);

  fprintf(_.stream, "\n");
  fflush(_.stream);

  pthread_mutex_unlock(&_.logLock);  // Now we're safe
  
  if (type == CLL_FATAL)
  {
    exit(-1);
  }
}
