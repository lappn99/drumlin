#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>


#include <drumlin/app.h>
#include <drumlin/renderer.h>
#include <drumlin/logging.h>

static SDL_Window* window;

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
    SDL_Event e;
    int quit = 0;
    while(SDL_PollEvent(&e))
    {
        if(e.type == SDL_QUIT)
        {
            return quit = 1;
        }
    }
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
    //d_renderer_clear();
    SDL_UpdateWindowSurface(window);

}

void 
d_app_getwindowsize(int* w, int* h)
{
    SDL_GetWindowSize(window, w, h);
}