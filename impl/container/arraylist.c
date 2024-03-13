#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <drumlin/container/list.h>
#include <drumlin/logging.h>

struct DList
{
    int element_size;
    size_t size;
    size_t capacity;
    void* array;
};

DListHandle 
d_make_list(int element_size,int initial_capacity)
{
    DListHandle arraylist;
    arraylist = malloc(sizeof(struct DList));
    memset(arraylist,0,sizeof(struct DList));
    arraylist->element_size = element_size;

    d_list_setcapacity(arraylist,initial_capacity);

    return arraylist;
}

void*
d_list_append(DListHandle handle,void* data)
{
    if(handle->size + 1 > handle->capacity)
    {
        if(d_list_setcapacity(handle,handle->capacity * 1.5) == -1)
        {
            D_LOG_ERROR("Could not append element", NULL);
            return NULL;
        }
    }
    void* dest = handle->array + (handle->size * handle->element_size);
    memcpy(dest,data,handle->element_size);
    handle->size++;
    return dest;

}

int 
d_list_setcapacity(DListHandle handle,int capacity)
{
    handle->capacity = capacity;
    handle->array = realloc(handle->array,handle->capacity * handle->element_size);
    if(handle->array == NULL)
    {
        D_LOG_ERROR("Failed: realloc(): ", strerror(errno));
        return -1;
    }
    return capacity;
}

void*
d_list_get(DListHandle handle,size_t index)
{
    //void* data = handle->array + (handle->element_size * index);
    return handle->array + (handle->element_size * index);
}

size_t 
d_list_getsize(DListHandle handle)
{
    return handle->size;
}

void 
d_destroy_list(DListHandle handle)
{
    free(handle->array);
    handle->array = NULL;
    free(handle);
    handle = NULL;
    
}