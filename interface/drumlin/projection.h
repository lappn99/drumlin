#ifndef _DRUMLIN_PROJECTION_H
#define _DRUMLIN_PROJECTION_H
#include "math.h"
#include <stdbool.h>

struct DProjection;
typedef struct DProjection* DProjectionHandle ;


DProjectionHandle d_create_projection(const char*, const char*);
void d_destroy_projection(DProjectionHandle);

DCoord2 d_projection_transform_coord(DProjectionHandle, DCoord2, bool);

DBBox d_projection_transform_bbox(DProjectionHandle, DBBox, bool);


#endif