#ifndef _DRUMLIN_GEOMETRY_H
#define _DRUMLIN_GEOMETRY_H
#include <drumlin/math.h>

typedef struct 
{
    DCoord2 position;
} DPoint;

typedef struct 
{
    union
    {
        DLatLng a;
        DLatLng b;
    };
    DLatLng raw[2];
} DLine;

#define point(POSITION) ((DPoint){.position = POSITION})
#define line(A, B) ((DLine){.a = A, .b = B})


#endif //_DRUMLIN_GEOMETRY_H