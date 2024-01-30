#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <drumlin/app.h>
#include <drumlin/renderer.h>
#include <drumlin/logging.h>

struct DKeyEvent
{
    DEventType ev_type;
    SDL_KeyboardEvent kb_event;
};

static SDL_Window* window;
static int quit = 0;
static struct DKeyEvent last_event;
static int key_lookup[DRUMLIN_NUM_KEYS];

static void create_key_lut(void);

int 
drumlin_start(DAppInitDesc* desc)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        D_LOG_ERROR("Could not init SDL", NULL);
        return 0;
    }

    window = SDL_CreateWindow(
        desc->title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        desc->width, desc->height,
        SDL_WINDOW_SHOWN
    );
    if(window == NULL)
    {
        D_LOG_ERROR("Could not create window", NULL);
        return 0;
    }

    //surface = SDL_GetWindowSurface(window);
    //SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format,0xFF,0xFF,0xFF));
    SDL_UpdateWindowSurface(window);
    d_init_renderer();
    create_key_lut();

    D_LOG_INFO("Drumlin started", NULL);



    return 1;

}

void 
drumlin_stop(void)
{
    d_deinit_renderer();
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int
drumlin_shouldquit(void)
{
    
    return quit;
}

void*
d_app_getwindowhandle(void)
{
    return window;
}

void 
d_app_update(void)
{
    SDL_Event e;
    last_event.ev_type = DRUMLIN_EVENT_NONE;
    while(SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            quit = 1;
            break;
        case SDL_KEYDOWN:
            last_event.ev_type = DRUMLIN_EVENT_KEYDOWN;
            last_event.kb_event = e.key;
            break;
        
        default:
            break;
        }
    }
    SDL_UpdateWindowSurface(window);

}

void 
d_app_getwindowsize(int* w, int* h)
{
    SDL_GetWindowSize(window, w, h);
}



struct DKeyEvent*
d_app_getkeydown(void)
{
    return &last_event;
}

int 
d_app_iskey(struct DKeyEvent* event, DKey key)
{
    if(event->kb_event.keysym.sym == key_lookup[key] && event->ev_type != DRUMLIN_EVENT_NONE)
    {
        return 1;
    }
    return 0;
}

static void 
create_key_lut(void)
{
    key_lookup[DRUMLIN_KEY_UP] = SDLK_UP;
    key_lookup[DRUMLIN_KEY_LEFT] = SDLK_LEFT;
    key_lookup[DRUMLIN_KEY_DOWN] = SDLK_DOWN;
    key_lookup[DRUMLIN_KEY_RIGHT] = SDLK_RIGHT;
    key_lookup[DRUMLIN_KEY_PLUS] = SDLK_PLUS;
    key_lookup[DRUMLIN_KEY_MINUS] = SDLK_MINUS;
    key_lookup[DRUMLIN_KEY_SPACE] = SDLK_SPACE;
}