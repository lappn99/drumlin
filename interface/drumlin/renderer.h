#ifndef _DRUMLIN_RENDERER_H
#define _DRUMLIN_RENDERER_H
#include <drumlin/layer.h>
#include <drumlin/projection.h>
#include <drumlin/map.h>

void d_init_renderer(void);

//Draw raster at absolute position on screen
void d_renderer_drawraster(const DLayerRasterGraphic*, int x, int y);
//Draw raster on screen at lat/lng coorindates with a projection to use
void d_renderer_drawraster_geo(const DLayerRasterGraphic*, const DMapHandle, DLatLng, DBBox, const DProjectionHandle);
void d_renderer_clear(void);
void d_deinit_renderer(void);
void d_renderer_present(void);

#endif //_DRUMLIN_RENDERER_H