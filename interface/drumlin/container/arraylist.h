#ifndef _DRUMLIN_CONTAINER_VECTOR_H
#define _DRUMLIN_CONTAINER_VECTOR_H
#include <stddef.h>

struct DArrayList;
typedef struct DArrayList* DArrayListHandle;

DArrayListHandle d_make_arraylist(int,int);
void* d_arraylist_append(DArrayListHandle,void*);
int d_arraylist_setcapacity(DArrayListHandle,int);
void* d_arraylist_get(DArrayListHandle,size_t);
void d_destroy_arraylist(DArrayListHandle);
size_t d_arraylist_getsize(DArrayListHandle);

#endif //_DRUMLIN_CONTAINER_VECTOR_H