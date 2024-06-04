#include <stdlib.h>
#include <gdal/gdal.h>


#include <drumlin/dataset.h>
#include <drumlin/logging.h>

struct DDataset
{
    GDALDatasetH dataset;
} ;

DDatasetHandle 
d_make_dataset()
{
    DDatasetHandle handle = malloc(sizeof(struct DDataset));
    handle->dataset = NULL;
    return handle;
}

void 
d_dataset_load(DDatasetHandle handle, const char* path, DDatasetOptions options)
{
    
    handle->dataset = GDALOpenEx(path,GDAL_OF_VECTOR | GDAL_OF_RASTER,NULL,NULL,NULL);
    if(!handle->dataset)
    {
        D_LOG_WARNING("Could not open dataset: %s", path);
        return;
    }
    GDALDriverH driver = GDALGetDatasetDriver(handle->dataset);
    if(driver == NULL)
    {
        D_LOG_WARNING("Could not open dataset: %s", path);
    }
    const char* driver_name = GDALGetDriverLongName(driver);
    D_LOG_INFO("Loaded dataset %s with driver %s", path, driver_name);


    

}

void* 
d_dataset_getunderlyinghandle(DDatasetHandle handle)
{
    return handle->dataset;
}

void 
d_dataset_listlayers(DDatasetHandle handle)
{
    int num_layers = GDALDatasetGetLayerCount(handle->dataset);
    int i = 0;
    for(i = 0; i < num_layers; i++)
    {
        OGRLayerH layer = GDALDatasetGetLayer(handle->dataset,i);
        const char* name = OGR_L_GetName(layer);
        D_LOG_INFO("Layer: %s", name);
    }
}

void
d_destroy_dataset(DDatasetHandle handle)
{
    if(handle->dataset != NULL)
    {
        
        GDALClose(handle->dataset);
    }
    free(handle);

}
