#ifndef _DRUMLIN_TILESERVICE_H
#define _DRUMLIN_TILESERVICE_H

#include "layer.h"
#include "tile.h"

#define DTILESERVICE_MAX_URI_LENGTH 2048

typedef struct 
{
    DLayer base;
    char uri_fmt[DTILESERVICE_MAX_URI_LENGTH];
} DTileServiceLayer;

typedef struct
{
    char* name;
    char* attribution;
    const char uri_fmt[DTILESERVICE_MAX_URI_LENGTH];
} DTileServiceLayerDesc;

int d_init_tileservice(void);
int d_tileservice_gettile(DTileServiceLayer*, int, int, int, DTile*);
int d_tileservice_gettiles(DTileServiceLayer*, int,int,int,int,int, DTile**);
void d_tileservce_render(DLayer*, DBBox, int);

DTileServiceLayer* d_make_tileservice(DTileServiceLayerDesc*);
void d_destroy_tileservice(DTileServiceLayer*);


#endif