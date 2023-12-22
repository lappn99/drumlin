#ifndef _DRUMLIN_H
#define _DRUMLIN_H




typedef struct
{
    union 
    {
        struct
        {
            double lat;
            double lng;
        };
        double coords[2];
    };
    
} DCoord2;

typedef DCoord2 DLatLng;

#define latlng(LATITUDE, LONGITUDE) ((DLatLng){.lat = LATITUDE, .lng = LONGITUDE})

int drumlin_init(void);


#include <drumlin/map.h>

#endif //_DRUMLIN_H
