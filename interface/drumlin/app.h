#ifndef _DRUMLIN_APP_H
#define _DRUMLIN_APP_H

typedef struct 
{
    int width;
    int height;
    const char* title;
} DAppInitDesc;


struct DKeyEvent;

typedef enum
{
    DRUMLIN_KEY_UP,
    DRUMLIN_KEY_DOWN,
    DRUMLIN_KEY_LEFT,
    DRUMLIN_KEY_RIGHT,
    DRUMLIN_KEY_PLUS,
    DRUMLIN_KEY_MINUS,
    DRUMLIN_KEY_SPACE,
    DRUMLIN_NUM_KEYS

} DKey;

typedef enum
{
    DRUMLIN_EVENT_KEYDOWN,
    DRUMLIN_EVENT_KEYUP,
    DRUMLIN_EVENT_NONE
} DEventType;



int drumlin_start(DAppInitDesc*);
void drumlin_stop(void);
int drumlin_shouldquit(void);
void* d_app_getwindowhandle(void);
void d_app_update(void);
void d_app_getwindowsize(int*, int*);

struct DKeyEvent* d_app_getkeydown(void);
int d_app_iskey(struct DKeyEvent*, DKey);


#endif //_DRUMLIN_APP_H