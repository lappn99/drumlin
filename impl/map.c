#include <stdlib.h> //malloc, free
#include <math.h>


#include <drumlin/tileservice.h>
#include <drumlin/map.h>
#include <drumlin/tile.h>
#include <drumlin/renderer.h>
#include <drumlin/layer.h>
#include <drumlin/container/arraylist.h>
#include <drumlin/math.h>
#include <drumlin/logging.h>

const int MIN_ZOOM = 0;
const int MAX_ZOOM = 18;
const double MIN_RESOLUTION = 156543.03;

struct DMap
{
    DLatLng view_position;
    int zoom;
    DArrayListHandle layers;
    
    
};

DMapHandle 
d_make_map(DMapInitDesc* init)
{
    DMapHandle handle = malloc(sizeof(struct DMap));
    handle->view_position = init->position;
    handle->zoom = init->zoom;
    handle->layers = d_make_arraylist(sizeof(DLayer*),5);
    
    
    return handle;
}



void 
d_destroy_map(DMapHandle handle)
{
    d_destroy_arraylist(handle->layers);
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
    DBBox bbox;
    y = x = 0;
    d_latlng_to_tilenum(handle->view_position,handle->zoom,&x, &y);
    
    d_latlng_to_bbox(handle->view_position,handle->zoom,&bbox);
    int xmin, ymin;
    int xmax, ymax;
    d_latlng_to_tilenum(bbox.min,handle->zoom, &xmin, &ymin);
    d_latlng_to_tilenum(bbox.max,handle->zoom, &xmax, &ymax);
    D_LOG_INFO("Bounding box: %f, %f,%f,%f", bbox.min.lat,bbox.min.lng,bbox.max.lat,bbox.max.lng);

    
    

    int i = 0;
    for(i = 0; i < d_arraylist_getsize(handle->layers);i++)
    {
        DLayer* layer = *((DLayer**)d_arraylist_get(handle->layers,i));
        layer->render_graphic_func(layer, bbox, handle->zoom);
    }
}

double 
d_map_resolution(DMapHandle map)
{
    return MIN_RESOLUTION * cos(map->view_position.lat) / (pow(2.0,map->zoom));
}

void
d_map_addlayer(DMapHandle map, DLayer* layer)
{
    DLayer* result = *((DLayer**)d_arraylist_append(map->layers,&layer));
    D_LOG_INFO("Added layer: %s", result->metadata.name);
}

