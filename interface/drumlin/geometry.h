#ifndef _DRUMLIN_GEOMETRY_H
#define _DRUMLIN_GEOMETRY_H
#include <drumlin/math.h>
#include <drumlin/container/list.h>
#include <stdbool.h>

typedef struct 
{
    DCoord2 position;
} DPoint;

typedef struct 
{
    union
    {
        struct
        {
            DCoord2 a;
            DCoord2 b;
        } ;
        double raw[4];
    };
    
} DLine;

typedef struct
{
    DListHandle linestrings;
} DLineString;

#define point(POSITION) ((DPoint){.position = POSITION})
#define line(A, B) ((DLine){.a = A, .b = B})



#endif //_DRUMLIN_GEOMETRY_H 