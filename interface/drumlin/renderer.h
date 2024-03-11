#ifndef _DRUMLIN_RENDERER_H
#define _DRUMLIN_RENDERER_H
#include <drumlin/layer.h>
#include <drumlin/projection.h>
#include <drumlin/map.h>

void d_init_renderer(void);

//Draw raster at absolute position on screen
void d_renderer_drawraster(const DLayerRasterGraphic*, int, int, int, int);
//Draw raster on screen at lat/lng coorindates with a projection to use
void d_renderer_drawraster_pgeo(const DLayerRasterGraphic*, const DMapHandle, DLatLng, const DProjectionHandle);
//Draw raster on screen within extent coorindates with a projection to use
void d_renderer_drawraster_egeo(const DLayerRasterGraphic*, const DMapHandle, DBBox, const DProjectionHandle);
void d_renderer_clear(void);
void d_deinit_renderer(void);
void d_renderer_present(void);

#endif //_DRUMLIN_RENDERER_H