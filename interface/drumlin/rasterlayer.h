#ifndef _DRUMLIN_RASTERLAYER_H
#define _DRUMLIN_RASTERLAYER_H

#include "layer.h"
#include "image.h"
#include "dataset.h"

struct DRasterLayer;
typedef struct DRasterLayer* DRasterLayerHandle;

typedef struct
{
    char* name;
    char* attribution;


} DRasterLayerDesc;



DRasterLayerHandle d_make_rasterlayer(const DRasterLayerDesc*);
void d_destroy_rasterlayer(DRasterLayerHandle);
void d_rasterlayer_render(struct DLayer*, DBBox, int, void*);
DRasterLayerHandle d_rasterlayer_copy(const DRasterLayerHandle);

void d_rasterlayer_setextent(DRasterLayerHandle, DBBox);
DBBox d_rasterlayer_getextent(const DRasterLayerHandle);

void d_rasterlayer_load_fromdataset(DRasterLayerHandle, const DDatasetHandle, const char*);
void d_rasterlayer_load_fromimage(DRasterLayerHandle, const DImage*);



#endif //_DRUMLIN_RASTERLAYER_H