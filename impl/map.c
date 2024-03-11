#include <string.h>
#include <stdlib.h> //malloc, free
#include <math.h>

#include <drumlin/app.h>
#include <drumlin/tileservice.h>
#include <drumlin/map.h>
#include <drumlin/tile.h>
#include <drumlin/renderer.h>
#include <drumlin/layer.h>
#include <drumlin/container/list.h>
#include <drumlin/math.h>
#include <drumlin/logging.h>
#include <drumlin/projection.h>

const int MIN_ZOOM = 0;
const int MAX_ZOOM = 18;
const double MIN_RESOLUTION = 156543.03;

#define PROJ_STRING_MAXLEN 16
struct DMap
{
    DLatLng view_position;
    int zoom;
    DListHandle layers;
    char gcs[PROJ_STRING_MAXLEN];
    char pcs[PROJ_STRING_MAXLEN];
    DProjectionHandle projection;
    
};

DMapHandle 
d_make_map(DMapInitDesc* init)
{
    DMapHandle handle = malloc(sizeof(struct DMap));
    handle->view_position = init->position;
    handle->zoom = init->zoom;
    handle->layers = d_make_list(sizeof(DLayer*),5);
    strcpy(&handle->pcs[0],"EPSG:3857");
    strcpy(&handle->gcs[0],"EPSG:4326");
    if(init->pcs)
    {
        strncpy(handle->pcs,init->pcs,PROJ_STRING_MAXLEN);
    }

    handle->projection = d_create_projection(handle->gcs,handle->pcs);


    
    return handle;
}



void 
d_destroy_map(DMapHandle handle)
{
    d_destroy_projection(handle->projection);
    d_destroy_list(handle->layers);
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

    if(coords.lng > 180)
    {
        coords.lng = -180;
    }
    else if(coords.lng < -180)
    {
        coords.lng = 180;
    }

    handle->view_position = coords;
    handle->zoom = zoom;
}

int 
d_map_getzoom(DMapHandle handle)
{
    return handle->zoom;
}

void
d_map_render(DMapHandle handle)
{
    d_renderer_clear();
    int x,y;
    DBBox bbox;
    y = x = 0;
    d_latlng_to_tilenum(handle->view_position,handle->zoom,&x, &y);
    
    d_latlng_to_bbox(handle->view_position,handle->zoom,&bbox);
    int xmin, ymin;
    int xmax, ymax;
    d_latlng_to_tilenum(bbox.min,handle->zoom, &xmin, &ymin);
    d_latlng_to_tilenum(bbox.max,handle->zoom, &xmax, &ymax);
    
    int i = 0;
    for(i = 0; i < d_list_getsize(handle->layers);i++)
    {
        DLayer* layer = *((DLayer**)d_list_get(handle->layers,i));
        layer->render_graphic_func(layer, bbox, handle->zoom,handle);
    }
    d_renderer_present();
}

double 
d_map_resolution(DMapHandle map)
{
    return d_resolution_at_latitude(map->view_position.lat,map->zoom);
}

void
d_map_addlayer(DMapHandle map, DLayer* layer)
{
    
    DLayer* result = *((DLayer**)d_list_append(map->layers,&layer));
    D_LOG_INFO("Added layer: %s", result->metadata.name);
}

void 
d_map_slide(DMapHandle map, int zoom, DCoord2 pixels, double speed)
{
    double resolution = d_resolution_at_latitude(0,map->zoom);

    float horizontal_dpi, vertical_dpi;
    d_app_getdpi(&horizontal_dpi,&vertical_dpi);
    
    double horizontal_scale = horizontal_dpi * (1/0.0254) * resolution;
    double vertical_scale = vertical_dpi * (1/0.0254) * resolution;

   

    DCoord2 meters_to_move = coord2((pixels.x * speed) * (horizontal_scale / 1000),(pixels.y * speed) * (vertical_scale / 1000));
    DProjectionHandle projection = d_create_projection(d_map_getgcs(map),d_map_getpcs(map));
    DLatLng degrees_to_move = d_projection_transform_coord(projection,meters_to_move,true);

    d_map_setview(map,d_latlng_add(map->view_position,degrees_to_move),map->zoom + zoom);
    d_destroy_projection(projection);
}

DLatLng 
d_map_getpos(DMapHandle map)
{
    return map->view_position;
}

const char* 
d_map_getgcs(const DMapHandle map)
{
    return map->gcs;
}
const char* 
d_map_getpcs(const DMapHandle map)
{
    return map->pcs;
}

DProjectionHandle 
d_map_getprojection(const DMapHandle map)
{
    return map->projection;
}