#ifndef _DRUMLIN_RASTERLAYER_H
#define _DRUMLIN_RASTERLAYER_H

#include "layer.h"
#include "image.h"

struct DRaster;



typedef struct
{
    char* name;
    char* attribution;
    DImage* image;
    DLatLng position;
    DBBox extent;

} DRasterLayerDesc;

typedef struct
{
    DLayer base;
    DImage* image;
    DLatLng position;
    DBBox extent;

} DRasterLayer;

DRasterLayer* d_make_rasterlayer(const DRasterLayerDesc*);
void d_destroy_rasterlayer(DRasterLayer*);
void d_rasterlayer_render(struct DLayer*, DBBox, int, void*);
DRasterLayer* d_rasterlayer_copy(const DRasterLayer*);





#endif //_DRUMLIN_RASTERLAYER_H