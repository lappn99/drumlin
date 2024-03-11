#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <drumlin/drumlin.h>
#include <drumlin/logging.h>
#include <drumlin/tileservice.h>
#include <gdal.h>

static void set_envars(void);

int 
drumlin_init(void)
{
    set_envars();
    GDALAllRegister();
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


static void 
set_envars(void)
{
    setenv("DRUMLIN_TMP_FOLDER","/tmp/drumlin",0);


}

const char* 
drumlin_tmpfolder(void)
{
    char* tmpfolder = getenv("DRUMLIN_TMP_FOLDER");
    if(access(tmpfolder,F_OK) != 0)
    {
        if(mkdir(tmpfolder,0777 ) < 0)
        {
            D_LOG_WARNING("mkdir(): %s", strerror(errno));
            return NULL;
        }
    }

    return getenv("DRUMLIN_TMP_FOLDER");
}

