#ifndef _DRUMLIN_TILE_H
#define _DRUMLIN_TILE_H

typedef struct
{
    
    int width;
    int height;
    int bands;
    float resolution;
    unsigned char* raster;

    int z;
    int x;
    int y;

} DTile;

#endif //_DRUMLIN_TILE_H