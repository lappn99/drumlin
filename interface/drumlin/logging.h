
#ifndef _DRUMLIN_LOGGING_H
#define _DRUMLIN_LOGGING_H
/*
    Little logging utility made for for fun
    Made by Nathan Lapp. March 2023
*/
#include <stdarg.h>
#include <stdint.h>

//Enum of different log levels, indicates indice into LOG_PREFIXES
enum LogLevel
{
    INFO,
    ERROR,
    WARNING
};




void d_debug_log(int line, const char* file,enum LogLevel level,const char* fmt,...);
void _d_debug_log_sokol(const char*, uint32_t, uint32_t, const char*, uint32_t, const char*, void* );


//Helper macro to print out INFO logs
#define D_LOG_INFO(fmt,...) d_debug_log(__LINE__,__FILE__,INFO,fmt,__VA_ARGS__)
//Helper macro to print out ERROR logs
#define D_LOG_ERROR(fmt,...) d_debug_log(__LINE__,__FILE__,ERROR,fmt,__VA_ARGS__)
//Helper macro to print out WARNING logs
#define D_LOG_WARNING(fmt,...) d_debug_log(__LINE__,__FILE__,WARNING,fmt,__VA_ARGS__)
//Helper macro for assertions
#define D_ASSERT(expr,fmt,...) if(!expr){\
d_debug_log(__LINE__,__FILE__,ERROR,fmt,__VA_ARGS__);\
assert(expr);\
}
#endif