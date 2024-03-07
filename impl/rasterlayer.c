#include <stdlib.h>
#include <string.h>
#include <drumlin/rasterlayer.h>

#include <drumlin/renderer.h>



void 
d_rasterlayer_render(struct DLayer* layer, DBBox viewbox, int zoom, void* userdata)
{
    DRasterLayer* raster_layer = (DRasterLayer*)layer;
    DMapHandle map = (DMapHandle)userdata;

    d_renderer_drawraster_geo(
        (const DLayerRasterGraphic*)raster_layer->image, map,
        raster_layer->position,  raster_layer->extent,
        d_map_getprojection(map)
    );
    


}

DRasterLayer*
d_make_rasterlayer(const DRasterLayerDesc* desc)
{
    DRasterLayer* raster_layer = malloc(sizeof(DRasterLayer));
    memset(raster_layer,0,sizeof(DRasterLayer));
    raster_layer->image = desc->image;
    raster_layer->base.type = DRUMLIN_LAYER_RASTER;

    raster_layer->base.metadata.attribution = desc->attribution;
    strncpy(raster_layer->base.metadata.name,desc->name,DLAYER_NAME_MAXLEN);
    raster_layer->base.render_graphic_func = d_rasterlayer_render;

    raster_layer->position = desc->position;
    raster_layer->extent = desc->extent;

    return raster_layer;

}

void 
d_destroy_rasterlayer(DRasterLayer* layer)
{
    free(layer);
}

DRasterLayer* 
d_rasterlayer_copy(const DRasterLayer* layer_to_copy)
{
    DRasterLayer* layer = malloc(sizeof(DRasterLayer));
    memcpy(layer,layer_to_copy,sizeof(DRasterLayer));
    return layer;
}