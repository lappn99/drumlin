#ifndef _DRUMLIN_RENDERER_H
#define _DRUMLIN_RENDERER_H
#include <drumlin/layer.h>

void d_init_renderer(void);
void d_renderer_drawraster(DLayerRasterGraphic*, int x, int y);
void d_renderer_clear(void);
void d_deinit_renderer(void);


#endif //_DRUMLIN_RENDERER_H