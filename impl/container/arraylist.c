#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <drumlin/container/arraylist.h>
#include <drumlin/logging.h>

struct DArrayList
{
    int element_size;
    size_t size;
    size_t capacity;
    void* array;
} ;



DArrayListHandle 
d_make_arraylist(int element_size,int initial_capacity)
{
    DArrayListHandle arraylist;
    arraylist = malloc(sizeof(struct DArrayList));
    memset(arraylist,0,sizeof(struct DArrayList));
    arraylist->element_size = element_size;

    d_arraylist_setcapacity(arraylist,initial_capacity);

    return arraylist;
}

void 
d_arraylist_append(DArrayListHandle handle,void* data)
{
    if(handle->size + 1 > handle->capacity)
    {
        if(d_arraylist_setcapacity(handle,handle->capacity * 1.5))
        {
            D_LOG_ERROR("Could not append element", NULL);
            return;
        }
    }
    memcpy(handle->array + (handle->size * handle->element_size),data,handle->element_size);
    handle->size++;

}

int 
d_arraylist_setcapacity(DArrayListHandle handle,int capacity)
{
    handle->capacity = capacity;
    handle->array = realloc(handle->array,handle->capacity * handle->element_size);
    if(handle->array == NULL)
    {
        D_LOG_ERROR("realloc(): ", strerror(errno));
        return -1;
    }
    return capacity;
}

void 
d_arraylist_get(DArrayListHandle handle,size_t index,void* out)
{
    memcpy(out,handle->array + (handle->element_size * index),handle->element_size);
}

void 
d_destroy_arraylist(DArrayListHandle handle)
{
    free(handle->array);
    free(handle);
}