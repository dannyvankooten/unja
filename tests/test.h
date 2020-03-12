#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define assert(assertion, format, ...) _assert(assertion, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define TESTNAME(v) strcpy(current_test, v);

char current_test[256] = {'\0'};

void _assert(int assertion, const char filename[64], const int line, char *format, ...)
{
    if (assertion)
    {
        return;
    }

    va_list args;
    va_start(args, format);
    printf("%s:%d:%s failed: ", filename, line, current_test);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}