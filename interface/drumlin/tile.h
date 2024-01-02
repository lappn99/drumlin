#ifndef _DRUMLIN_TILE_H
#define _DRUMLIN_TILE_H



typedef struct
{
    int z;
    int x;
    int y;

    unsigned char* raster;
    int width;
    int height;
    int bands;

} DTile;

#endif //_DRUMLIN_TILE_H