#ifndef _DRUMLIN_RENDERER_H
#define _DRUMLIN_RENDERER_H
#include "tile.h"

void d_init_renderer(void);
void d_renderer_drawtile(DTile*, int x, int y);
void d_deinit_renderer(void);

#endif //_DRUMLIN_RENDERER_H