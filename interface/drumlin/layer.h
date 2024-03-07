#ifndef _DRUMLIN_LAYER_H
#define _DRUMLIN_LAYER_H


#include "math.h"

#define DLAYER_NAME_MAXLEN 64

typedef struct
{
    int width_px;
    int height_px;
    int bands;
    float resolution;
    unsigned char* raster;
} DLayerRasterGraphic;

typedef struct
{

} DLayerVectorGraphic;

typedef enum
{
    DRUMLIN_LAYER_VECTOR,
    DRUMLIN_LAYER_RASTER
} DLayerType;

typedef struct
{
    DBBox extent;
    union
    {
        DLayerVectorGraphic vector;
        DLayerRasterGraphic raster;
    };

} DLayerGraphic;

typedef struct 
{
    char name[DLAYER_NAME_MAXLEN];
    char* attribution;
    
} DLayerMetadata;

typedef struct DLayer
{
    
    DLayerMetadata metadata;
    DLayerType type;
    void (*render_graphic_func)(struct DLayer*, DBBox, int, void*);

    
} DLayer;




#endif //_DRUMLIN_LAYER_H