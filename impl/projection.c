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
d_projection_transform_coord(DProjectionHandle handle,DCoord2 src_coord, bool inverse)
{
    enum PJ_DIRECTION direction = PJ_FWD;

    if(inverse)
    {
        direction = PJ_INV;
    }

    PJ_COORD a = proj_coord(src_coord.lat, src_coord.lng,0,0);

    PJ_COORD b = proj_trans(handle->projection, direction, a);

    return coord2(b.v[0],b.v[1]);
}

DBBox 
d_projection_transform_bbox(DProjectionHandle handle, DBBox src_bbox, bool inverse)
{
    DBBox dest_bbox = {0};

    dest_bbox.min = d_projection_transform_coord(handle, src_bbox.min, inverse);
    dest_bbox.max = d_projection_transform_coord(handle, src_bbox.max, inverse);

    return dest_bbox;
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