#ifndef _DRUMLIN_CONTAINER_VECTOR_H
#define _DRUMLIN_CONTAINER_VECTOR_H
#include <stddef.h>

struct DList;
typedef struct DList* DListHandle;

DListHandle d_make_list(int,int);
void* d_list_append(DListHandle,void*);
int d_list_setcapacity(DListHandle,int);
void* d_list_get(DListHandle,size_t);
void d_destroy_list(DListHandle);
size_t d_list_getsize(DListHandle);

#endif //_DRUMLIN_CONTAINER_VECTOR_H