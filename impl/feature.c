#include <drumlin/logging.h>
#include <gdal.h>
#include <ogr_core.h>
#include <ogr_api.h>
#include <drumlin/feature.h>

struct DFeature
{
    GDALDatasetH dataset;
};

void
d_feature_loaddata(DFeatureHandle feature, const char* path)
{
   
    GDALDriverH driver = GDALIdentifyDriver(path,NULL);
    const char* driver_name = GDALGetDriverLongName(driver);
    D_LOG_INFO("%s requires %s driver", path, driver_name);
    //feature->dataset = GDALOpenEx(path,GDAL_OF_VECTOR,NULL,NULL,NULL);
    
}

DFeatureHandle 
d_make_feature(void)
{

    
    return malloc(sizeof(struct DFeature));
}

void 
d_destroy_feature(DFeatureHandle feature)
{
    free(feature);
}




