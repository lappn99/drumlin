#ifndef _DRUMLIN_FEATURE_H
#define _DRUMLIN_FEATURE_H

#include "geometry.h"

typedef union 
{
    DPoint point_geom;
    DLine line_geom;
    
} DFeatureGeometry;

typedef enum
{
    DRUMLIN_GEOM_POINT,
    DRUMLIN_GEOM_LINE

} DFeatureGeometryType;

typedef struct 
{
    DFeatureGeometry geometry;
    DFeatureGeometryType geom_type;
    
        

} DFeature;

#endif //_DRUMLIN_FEATURE_H