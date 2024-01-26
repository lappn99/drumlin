#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <drumlin/drumlin.h>
#include <drumlin/logging.h>
#include <drumlin/tileservice.h>

int 
drumlin_init(void)
{
    d_init_tileservice();
    return 0;
}

void 
d_debug_log(int line, const char* file,enum LogLevel info,const char* fmt, ...)
{
    //Array of prefixes
    //Uses ANSI escape sequences for coloring.
    //Fun!
    const char* LOG_PREFIXES[] = {
        "\x1b[32m[INFO] %s:%d: %s\x1b[0m\n",
        "\x1b[1;31m[ERROR] %s:%d: %s\x1b[0m\n",
        "\x1b[1;33m[WARNING] %s:%d: %s\x1b[0m\n"
    };
    //Get prefix
    const char* prefix = LOG_PREFIXES[info];    
    char* logStr;

    //Get variadic arguments
    va_list ap;
    va_start(ap,fmt);

    vasprintf(&logStr,fmt,ap);
    va_end(ap);
    fprintf(stderr,prefix,file,line,logStr);
    free(logStr);
    
    
}

void 
_d_debug_log_sokol(const char* tag, uint32_t log_level, uint32_t log_item, const char* message, uint32_t line, const char* filename, void* userdata)
{
    d_debug_log(line, filename,log_level,"%s",message);
}

