#include <stdlib.h> //malloc, free

#include <drumlin/map.h>

const int MIN_ZOOM = 0;
const int MAX_ZOOM = 19;

struct DMap
{
    DLatLng view_position;
    int zoom;
};

DMapHandle 
d_make_map(DMapInitDesc init)
{
    DMapHandle handle = malloc(sizeof(struct DMap));
    handle->view_position = init.position;
    handle->zoom = init.zoom;

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



