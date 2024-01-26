#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <drumlin/app.h>
#include <drumlin/math.h>
#include <drumlin/renderer.h>

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

#define DEG_2_RAD(DEGREES) (DEGREES * M_PI / 180.0)
#define RAD_2_DEG(RADIANS) (RADIANS * (180 / M_PI))

void 
d_latlng_to_tilenum(DLatLng latlng, int zoom, int* x, int* y)
{
    double lat_radians = DEG_2_RAD(latlng.lat);
    
    int n = 1 << zoom;
    *x = (int)((latlng.lng + 180.0) / 360.0 * n);
    *y = (int)((1.0 - asinh(tan(lat_radians))/ M_PI) / 2.0 * n );
}

void 
d_tilenum_to_latlng(int x, int y, int zoom, DLatLng* latlng)
{
    int n = pow(2, zoom);
    double lon_deg = x / (double)n * 360.0 - 180.0;
    double lat_rad = atan(sinh(M_PI * (1 - 2 * y / (double)n)));
    double lat_deg = lat_rad * 180.0 / M_PI;
    latlng->lat = lat_deg;
    latlng->lng = lon_deg;
}

void 
d_latlng_to_bbox(DLatLng latlng, int zoom, DBBox* bbox)
{
    int tile_size = 256;
    int width, height;
    height = width = 0;
    d_app_getwindowsize(&width, &height);


    int xtile, ytile;
    d_latlng_to_tilenum(latlng,zoom,&xtile, &ytile);
    int xs = (xtile * tile_size - width / (double)2) / tile_size;
    int ys = (ytile * tile_size - height / (double)2) / tile_size;
    int xe = (xtile * tile_size + width / (double)2) / tile_size;
    int ye = (ytile * tile_size + height / (double)2) / tile_size;
    DLatLng latlng_s;
    DLatLng latlng_e;
    d_tilenum_to_latlng(xs,ys,zoom,&latlng_s);
    d_tilenum_to_latlng(xe,ye,zoom,&latlng_e);
    *bbox = bbox(latlng_s, latlng_e);
}