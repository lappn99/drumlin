#include <stdlib.h>
#include <string.h>
#include <gdal/gdal.h>
#include <linux/limits.h>

#include <drumlin/drumlin.h>
#include <drumlin/rasterlayer.h>
#include <drumlin/renderer.h>

struct DRasterLayer
{
    DLayer base;
    DLatLng position;
    DBBox extent;
    GDALDatasetH dataset;

} ;

static const char* get_tmpfile(const char* extension);

void 
d_rasterlayer_render(struct DLayer* layer, DBBox viewbox, int zoom, void* userdata)
{
    DRasterLayerHandle raster_layer = (DRasterLayerHandle)layer;
    DMapHandle map = (DMapHandle)userdata;
    GDALDatasetH dataset = raster_layer->dataset;

    DLayerRasterGraphic graphic;
    
    graphic.bands = GDALGetRasterCount(dataset);
    graphic.width_px = GDALGetRasterXSize(dataset);
    graphic.height_px = GDALGetRasterYSize(dataset);
    graphic.raster = malloc(sizeof(unsigned char) * ((graphic.width_px * graphic.height_px) * graphic.bands));
    
    CPLErr error = GDALDatasetRasterIO(dataset,GF_Read,0,0,graphic.width_px,graphic.height_px,
        graphic.raster,graphic.width_px,graphic.height_px,
        GDT_Byte,graphic.bands,NULL,
        0,0,0);
    if(error != CE_None)
    {
        D_LOG_ERROR("GDALDatasetRasterIO()", NULL);
        return;
    }
    d_renderer_drawraster_egeo(
        &graphic, map,
        raster_layer->extent,
        d_map_getprojection(map)
    );
    
    free(graphic.raster);
    


}

DRasterLayerHandle
d_make_rasterlayer(const DRasterLayerDesc* desc)
{
    DRasterLayerHandle raster_layer = malloc(sizeof(struct DRasterLayer));
    memset(raster_layer,0,sizeof(struct DRasterLayer));
    raster_layer->base.type = DRUMLIN_LAYER_RASTER;

    raster_layer->base.metadata.attribution = desc->attribution;
    strncpy(raster_layer->base.metadata.name,desc->name,DLAYER_NAME_MAXLEN);
    raster_layer->base.render_graphic_func = d_rasterlayer_render;
    raster_layer->dataset = NULL;
    
    return raster_layer;

}

void d_rasterlayer_load_fromdataset(DRasterLayerHandle handle, const DDatasetHandle src_dataset, const char* layer)
{
    if(handle->dataset == NULL)
    {
        GDALDriverH driver = GDALGetDatasetDriver(d_dataset_getunderlyinghandle(src_dataset));
        const char* extension = GDALGetMetadataItem(driver,"DMD_EXTENSION",NULL);

        const char* path = get_tmpfile(extension);
        D_LOG_INFO("Storing temp in: %s",path);
        

        D_LOG_INFO("Source dataset driver: %s", GDALGetDriverLongName(driver));
        

        handle->dataset = GDALCreateCopy(driver,&path[0],d_dataset_getunderlyinghandle(src_dataset),0,NULL,NULL,NULL);
        if(handle->dataset == NULL)
        {
            D_LOG_ERROR("Could not create copy of dataset at %s: ", path);
        }


    }
}

void d_rasterlayer_load_fromimage(DRasterLayerHandle handle, const DImage* image)
{
    if(handle->dataset == NULL)
    {
        GDALDriverH driver = GDALGetDriverByName("GTiff");
        
        const char* path = get_tmpfile("tiff");
        handle->dataset = GDALCreate(driver,path,image->width,image->height,image->channels,GDT_Byte,NULL);

        CPLErr error = GDALDatasetRasterIO(handle->dataset,GF_Write,0,0,
            image->width,image->height,image->data,image->width,
            image->height,GDT_Byte,image->channels,NULL,0,0,0);
        
        
        if(error != CE_None)
        {
            D_LOG_WARNING("GDALDatasetRasterIO()", NULL);
        }


    }
}


void 
d_destroy_rasterlayer(DRasterLayerHandle layer)
{
    
    
    free(layer);
}

DRasterLayerHandle 
d_rasterlayer_copy(const DRasterLayerHandle layer_to_copy)
{
    DRasterLayerHandle layer = malloc(sizeof(struct DRasterLayer));
    memcpy(layer,layer_to_copy,sizeof(struct DRasterLayer));
    return layer;
}

void  
d_rasterlayer_setextent(DRasterLayerHandle handle, DBBox extent)
{
    handle->extent = extent;
}

DBBox 
d_rasterlayer_getextent(const DRasterLayerHandle handle)
{
    return handle->extent;
}

static const char* 
get_tmpfile(const char* extension)
{
    static char path[PATH_MAX] = {0};

    const char* tmp_folder = drumlin_tmpfolder();
    char tmp_filename_template[PATH_MAX];
    memset(tmp_filename_template,0,PATH_MAX);
    snprintf(&tmp_filename_template[0],PATH_MAX,"%s/XXXXXX",tmp_folder);

    int tmp_file = mkstemp(&tmp_filename_template[0]);
    
    if(tmp_file < 0)
    {
        D_LOG_ERROR("mktemp(): %s",strerror(errno));
        return NULL;
    }

    int path_size = snprintf(&path[0],PATH_MAX,"%s",tmp_filename_template);
    if(path_size < 0)
    {
        D_LOG_ERROR("snprintf(): %s",strerror(errno));
        close(tmp_file);
        return NULL;
    }
    close(tmp_file);

    return path;

}
