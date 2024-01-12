#ifndef _DRUMLIN_TILESERVICE_H
#define _DRUMLIN_TILESERVICE_H

#include "tile.h"
#include "layer.h"

#define DTILESERVICE_MAX_URI_LENGTH 2048

typedef struct 
{
    DLayer base;
    char uri_fmt[DTILESERVICE_MAX_URI_LENGTH];
} DTileServiceLayer;

typedef struct
{
    char* attribution;
    char uri_fmt[DTILESERVICE_MAX_URI_LENGTH];
} DTileServiceLayerDesc;

int d_init_tileservice(void);
int d_tileservice_gettile(int, int, int, DTile*);

DTileServiceLayer* d_make_tileservice(DTileServiceLayerDesc*);
void d_destroy_tileservice(DTileServiceLayer*);

#endif