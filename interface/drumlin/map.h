#ifndef _DRUMLIN_MAP_H
#define _DRUMLIN_MAP_H
#include <drumlin/drumlin.h>

struct DMap;

typedef struct DMap* DMapHandle;

typedef struct
{
    DLatLng position;
    int zoom;
} DMapInitDesc;

DMapHandle d_make_map(DMapInitDesc);
void d_destroy_map(DMapHandle);


//Latitude, Longitude
//Zoom
void d_map_setview(DMapHandle, DLatLng, int);
void d_map_render(DMapHandle);




#endif //_DRUMLIN_MAP_H