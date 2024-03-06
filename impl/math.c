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
    int width, height;
    height = width = 0;
    d_app_getwindowsize(&width, &height);

    double metersPerPixelEW = EARTH_CIR_METERS / pow(2,zoom + 8);
    double metersPerPixelNS = EARTH_CIR_METERS / pow(2, zoom + 8) * cos(DEG_2_RAD(latlng.lat));

    double shiftMetersEW = width/2  * metersPerPixelEW;
    double shiftMetersNS = height/2 * metersPerPixelNS;

    double shiftDegreesEW = shiftMetersEW * DEGREES_PER_METER;
    double shiftDegreesNS = shiftMetersNS * DEGREES_PER_METER;

    DLatLng min;
    DLatLng max;
    min.lat = latlng.lat + shiftDegreesNS;
    min.lng = latlng.lng - shiftDegreesEW;
    max.lat = latlng.lat - shiftDegreesNS;
    max.lng = latlng.lng + shiftDegreesEW;
    *bbox = bbox(min,max);
}



DLatLng 
d_latlng_add(DLatLng a, DLatLng b)
{
    return latlng(a.lat + b.lat, a.lng + b.lng);
}

DLatLng
d_latlng_sub(DLatLng a, DLatLng b)
{
    return latlng(a.lat - b.lat, a.lng - b.lng);
}

DLatLng 
d_latlng_radians(DLatLng a)
{
    return latlng(DEG_2_RAD(a.lat), DEG_2_RAD(a.lng));
}

double  
d_resolution_at_latitude(double latitude, int zoom)
{
    return 156543.03  * cos(latitude) / pow(2,zoom);
}