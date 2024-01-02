#ifndef _DRUMLIN_APP_H
#define _DRUMLIN_APP_H

typedef struct 
{
    int width;
    int height;
    const char* title;
} DAppInitDesc;


int drumlin_start(DAppInitDesc*);
void drumlin_stop(void);
int drumlin_shouldquit(void);
void* d_app_getwindowhandle(void);
void d_app_update(void);
void d_app_getwindowsize(int*, int*);


#endif //_DRUMLIN_APP_H