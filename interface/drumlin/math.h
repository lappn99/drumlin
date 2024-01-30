#ifndef _DRUMLIN_MATH_H
#define _DRUMLIN_MATH_H

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

typedef struct
{
    union
    {
        struct
        {
           DCoord2 min;
           DCoord2 max;
        };
        double raw[4];
    };
} DBBox;

typedef DCoord2 DLatLng;
#define latlng(LATITUDE, LONGITUDE) ((DLatLng){.lat = LATITUDE, .lng = LONGITUDE})
#define bbox(MIN_LATLNG, MAX_LATLNG) ((DBBox){\
        .min = MIN_LATLNG,\
        .max = MAX_LATLNG\
})

void d_latlng_to_tilenum(DLatLng, int, int*, int*);
DLatLng d_latlng_add(DLatLng, DLatLng);

void d_tilenum_to_latlng(int, int, int, DLatLng*);
void d_latlng_to_bbox(DLatLng, int, DBBox*);

#endif //_DRUMLIN_MATH_H
