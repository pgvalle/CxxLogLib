#include "CxxLogLib.h"
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef _WIN32
#include <windows.h>
#endif

static const char *LOG_TYPES[] = {
  "INFO ", "DEBUG", "WARN ", "ERROR", "FATAL"
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

void CLL_setLogStream(FILE *stream)
{
  __stream = stream;
}

void CLL_setColors(bool colors)
{
  __colors = colors;
}

static time_t previousTimeSuffix = 0;

FILE *CLL_createLogFile(const char *prefix)
{
  time_t timeSuffix;
  time(&timeSuffix);

  // 2 or more files will have the same name if the prefix is equal
  if (timeSuffix == previousTimeSuffix)
  {
    CLL_error("Wait 1s to create another log file.", prefix);
    return NULL;
  }

  previousTimeSuffix = timeSuffix;

  const size_t lenPrefix = strlen(prefix);

  if (lenPrefix > 32)
  {
    CLL_error("%s has more than 32 characters.", prefix);
    return NULL;
  }

  char logFileName[64] = "";
  memcpy(logFileName, prefix, lenPrefix);

  logFileName[lenPrefix] = '-';

  const char *suffix = logFileName + lenPrefix + 1;

  
  return fopen(logFileName, "w");
}

void __CLL_log(enum CLL_LogType type, const char *func, int line,
               const char *file, const char *format, ...)
{
  const time_t t = time(NULL);
  struct tm *lt = localtime(&t);

  // time and date
  fprintf(__stream, "[%4d-%02d-%02d %02d:%02d:%02d] ",
          lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
          lt->tm_hour, lt->tm_min, lt->tm_sec);

  if (__colors)
  {
    fprintf(__stream, "[%s%s\033[0m] ", LOG_TYPES_COLORS[type], LOG_TYPES[type]);
  }
  else
  {
    fprintf(__stream, "[%s] ", LOG_TYPES[type]);
  }

  fprintf(__stream, "[%s:%d:%s] ", file, line, func);

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
