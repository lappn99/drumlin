#ifndef _DRUMLING_FEATURELAYER_H
#define _DRUMLING_FEATURELAYER_H

#include "dataset.h"

struct DFeatureLayer;
typedef struct DFeatureLayer* DFeatureLayerHandle;

typedef struct 
{
    char* name;
    char* attribution;

}DFeatureLayerDesc;

void d_featurelayer_load_fromdataset(DFeatureLayerHandle, DDatasetHandle, const char*);

DFeatureLayerHandle d_make_featurelayer(const DFeatureLayerDesc*);
void d_destroy_featurelayer(const DFeatureLayerHandle);

#endif //_DRUMLING_FEATURE_H