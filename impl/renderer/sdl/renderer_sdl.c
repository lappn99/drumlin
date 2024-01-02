#include <SDL2/SDL.h>
#include <drumlin/renderer.h>
#include <drumlin/app.h>
#include <drumlin/logging.h>

static SDL_Renderer* renderer;

void
d_init_renderer(void)
{
    SDL_Window* window = (SDL_Window*)d_app_getwindowhandle();
    renderer = SDL_CreateRenderer(window,-1,0);
    if(renderer == NULL)
    {
        D_LOG_ERROR("Could not create SDL Renderer: %s", SDL_GetError());
        return;
    }

}

void 
d_renderer_drawtile(DTile* tile, int x , int y)
{

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(tile->raster,tile->width,tile->height,tile->bands * 8,tile->width * 3,
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

    if(!surface)
    {
        D_LOG_WARNING("Could not create tile surface: %s", SDL_GetError());
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
    if(!texture)
    {
        D_LOG_WARNING("Could not create tile texture: %s", SDL_GetError());
    }
    SDL_Rect dst_rect = {x,y,tile->width,tile->height};
    SDL_RenderCopy(renderer,texture, NULL, &dst_rect);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    
}

void 
d_deinit_renderer(void)
{
    SDL_DestroyRenderer(renderer);
}
