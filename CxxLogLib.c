#include "CxxLogLib.h"
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef _WIN32
#include <windows.h>
#endif

static const char *LOG_TYPES[] = {
  " INFO  ", " DEBUG ", " WARN  ", " ERROR ", " FATAL "
};

static const char *LOG_TYPES_COLORS[] = {
  "\033[32m",
  "\033[36m",
  "\033[33m",
  "\033[31m",
  "\033[35m"
};

static FILE *__stream;
static bool __colors;

void CLL_init()
{
  __stream = stdout;
  __colors = false;

  // windows bullshit to make ansi escape sequences work
#ifndef _WIN32
  const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  DWORD dwMode;
  GetConsoleMode(hOut, &dwMode);

  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#endif
}

void CLL_setLoggingStream(FILE *stream)
{
  __stream = stream;
}

void CLL_setColors(bool colors)
{
  __colors = colors;
}

void __CLL_log(enum CLL_LogType type, const char *func, int line,
               const char *file, const char *format, ...)
{
  time_t t;
  char timeString[25] = "";
  time(&t);
  memcpy(timeString, ctime(&t), 24);

  if (__colors)
  {
    fprintf(__stream, "%s [%s%s\033[0m] %s:%d:%s -> ", timeString,
            LOG_TYPES_COLORS[type], LOG_TYPES[type], file, line, func);
  }
  else
  {
    fprintf(__stream, "%s [%s] %s:%d:%s -> ", timeString,
            LOG_TYPES[type], file, line, func);
  }

  va_list args;
  va_start(args, format);
  vfprintf(__stream, format, args);
  va_end(args);

  fprintf(__stream, "\n");
  fflush(__stream);

  if (type == CLL_FATAL)
  {
    exit(-1);
  }
}
