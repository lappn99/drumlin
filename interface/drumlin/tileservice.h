#ifndef _DRUMLIN_TILESERVICE_H
#define _DRUMLIN_TILESERVICE_H

#include "tile.h"

int d_init_tileservice(void);

int d_tileservice_gettile(int, int, int, DTile*);

#endif