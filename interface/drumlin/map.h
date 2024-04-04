#ifndef _DRUMLIN_MAP_H
#define _DRUMLIN_MAP_H

#include "math.h"
#include <drumlin/projection.h>

struct DLayer;

struct DMap;

typedef struct DMap* DMapHandle;

typedef struct
{
    DLatLng position;
    int zoom;
    const char* pcs;

    

} DMapInitDesc;

DMapHandle d_make_map(DMapInitDesc*);
void d_destroy_map(DMapHandle);


//Latitude, Longitude
//Zoom
void d_map_setview(DMapHandle, DLatLng, int);
int d_map_getzoom(DMapHandle);
void d_map_render(DMapHandle);
double d_map_resolution(DMapHandle);
void d_map_addlayer(DMapHandle, struct DLayer*);
void d_map_slide(DMapHandle, int,DCoord2, double);
DLatLng d_map_getpos(DMapHandle);
const char* d_map_getgcs(const DMapHandle);
const char* d_map_getpcs(const DMapHandle);
DProjectionHandle d_map_getprojection(const DMapHandle);



#endif //_DRUMLIN_MAP_H