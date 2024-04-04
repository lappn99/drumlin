#ifndef _DRUMLIN_FEATURE_H
#define _DRUMLIN_FEATURE_H

#include "geometry.h"

typedef union DFeatureGeometry
{
    DPoint point_geom;
    DLineString linestring_geom;
    
} DFeatureGeometry;

typedef enum
{
    DRUMLIN_GEOM_POINT,
    DRUMLIN_GEOM_LINESTRING

} DFeatureGeometryType;

typedef struct DFeature
{
    DFeatureGeometry geometry;
    DFeatureGeometryType geom_type;

} DFeature;



#endif //_DRUMLIN_FEATURE_H