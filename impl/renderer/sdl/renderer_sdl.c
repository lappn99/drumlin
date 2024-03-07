#include <SDL2/SDL.h>

#include <drumlin/renderer.h>
#include <drumlin/app.h>
#include <drumlin/logging.h>
#include <drumlin/math.h>

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
d_renderer_drawraster(const DLayerRasterGraphic* raster, int x, int y)
{

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(raster->raster,raster->width_px,
        raster->height_px,raster->bands * 8,
        raster->width_px * raster->bands,
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
    );

    if(!surface)
    {
        D_LOG_WARNING("Could not create raster surface: %s", SDL_GetError());
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
    if(!texture)
    {
        D_LOG_WARNING("Could not create raster texture: %s", SDL_GetError());
    }
    SDL_Rect dst_rect = {x,y,raster->width_px,raster->height_px};
    SDL_RenderCopy(renderer,texture, NULL, &dst_rect);
    //SDL_RenderPresent(renderer);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    
}

void 
d_renderer_drawraster_geo(const DLayerRasterGraphic* raster,
    const DMapHandle map, DLatLng position, DBBox extent,
    const DProjectionHandle projection)
{
    
    DCoord2 camera_pos_projected = d_projection_transform_coord(projection,d_map_getpos(map),false);
    
    double resolution = d_resolution_at_latitude(0, d_map_getzoom(map));

    int window_width, window_height;
    d_app_getwindowsize(&window_width,&window_height);

    DCoord2 screen_origin;

    screen_origin.x = camera_pos_projected.x - (window_width / 2) * (resolution);
    screen_origin.y = camera_pos_projected.y - (window_height / 2) * (resolution);

    DCoord2 projected_coord = d_projection_transform_coord(projection,position,false);


    DCoord2 screen_coords = coord2(
        (projected_coord.x - screen_origin.x) * (1/resolution), 
        (projected_coord.y - screen_origin.y) * (1/resolution)
    );

    d_renderer_drawraster(raster,screen_coords.x, window_height - screen_coords.y);
    


}   

void 
d_renderer_present(void)
{
    SDL_RenderPresent(renderer);
}

void
d_renderer_clear(void)
{
    if(SDL_RenderClear(renderer) != 0)
    {
        D_LOG_WARNING("Could not clear screen: %s", SDL_GetError());
    }
}

void 
d_deinit_renderer(void)
{
    SDL_DestroyRenderer(renderer);
}
