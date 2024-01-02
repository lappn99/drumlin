#include <stdlib.h> //malloc, free
#include <math.h>

#include <drumlin/tileservice.h>
#include <drumlin/map.h>
#include <drumlin/tile.h>
#include <drumlin/renderer.h>

const int MIN_ZOOM = 0;
const int MAX_ZOOM = 18;
const double MIN_RESOLUTION = 156543.03;

struct DMap
{
    DLatLng view_position;
    int zoom;
};

DMapHandle 
d_make_map(DMapInitDesc* init)
{
    DMapHandle handle = malloc(sizeof(struct DMap));
    handle->view_position = init->position;
    handle->zoom = init->zoom;
    
    return handle;
}



void 
d_destroy_map(DMapHandle handle)
{
    free(handle);
}

void
d_map_setview(DMapHandle handle, DLatLng coords, int zoom)
{
    if(zoom < MIN_ZOOM)
    {
        zoom = MIN_ZOOM;
    }
    else if(zoom > MAX_ZOOM)
    {
        zoom = MAX_ZOOM;
    }
    handle->view_position = coords;
    handle->zoom = zoom;
}

void
d_map_render(DMapHandle handle)
{
    int x,y;
    int i,j;
    DBBox bbox;
    y = x = 0;
    d_latlng_to_tilenum(handle->view_position,handle->zoom,&x, &y);
    D_LOG_INFO("Map tile: %d, %d, %d",handle->zoom,x,y);
    d_latlng_to_bbox(handle->view_position,handle->zoom,&bbox);
    int xmin, ymin;
    int xmax, ymax;
    d_latlng_to_tilenum(bbox.min,handle->zoom, &xmin, &ymin);
    d_latlng_to_tilenum(bbox.max,handle->zoom, &xmax, &ymax);
    D_LOG_INFO("Bounding box: %f, %f,%f,%f", bbox.min.lng,bbox.min.lat,bbox.max.lng,bbox.max.lat);

    
    for(i = xmin; i <= xmax; i++)
    {
        for(j = ymin; j <= ymax; j++)
        {
            DTile tile;
            int result = d_tileservice_gettile(handle->zoom,i,j,&tile);
            if(!result)
            {
                D_LOG_ERROR("Could not render %d, %d, %d",handle->zoom,x,y);
                return;
            }
            d_renderer_drawtile(&tile,(i - xmin) * 256,(j - ymin) * 256);
            free(tile.raster);
        }
    }   
    /*DTile tile;
    int result = d_tileservice_gettile(handle->zoom,x,y,&tile);
    if(!result)
    {
        D_LOG_ERROR("Could not render %d, %d, %d",handle->zoom,x,y);
        return;
    }
    d_renderer_drawtile(&tile,0,0);
    free(tile.raster);*/
    
    


}

double 
d_map_resolution(DMapHandle map)
{
    return MIN_RESOLUTION * cos(map->view_position.lat) / (pow(2.0,map->zoom));
}



