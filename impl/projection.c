#include <stdio.h>
#include <stdlib.h>
#include <proj.h>

#include <drumlin/logging.h>
#include <drumlin/projection.h>

struct DProjection
{
    PJ_CONTEXT* context;
    PJ* projection;
};





DCoord2 
d_projection_transformcoord(DProjectionHandle handle,DCoord2 src_coord)
{
    PJ_COORD a = proj_coord(src_coord.lat, src_coord.lng,0,0);

    PJ_COORD b = proj_trans(handle->projection, PJ_FWD,a);

    return coord2(b.v[0],b.v[1]);
}


DProjectionHandle 
d_create_projection(const char* src, const char* dst)
{
    DProjectionHandle handle = malloc(sizeof(struct DProjection));
    handle->context = proj_context_create();
    handle->projection = proj_create_crs_to_crs(handle->context,src,dst,NULL);
    if(handle->projection == NULL)
    {
        D_LOG_WARNING("Could not create projection object for projection: %s to %d", src, dst);
    }
    return handle;

}

void
d_destroy_projection(DProjectionHandle handle)
{
    proj_destroy(handle->projection);
    proj_context_destroy(handle->context);
    free(handle);
}