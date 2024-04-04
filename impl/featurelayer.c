#include <linux/limits.h>

#include <gdal.h>
#include <ogr_api.h>
#include <ogr_core.h>

#include <drumlin/featurelayer.h>
#include <drumlin/drumlin.h>
#include <drumlin/renderer.h>

struct DFeatureLayer
{
    DLayer base;   
    OGRLayerH ogrlayer;
    DLayerVectorGraphic graphic;
} ;

static void d_featurelayer_render(struct DLayer* , DBBox , int , void* );
static void linestring_to_feature(OGRGeometryH,DFeature*);
static void point_to_feature(OGRGeometryH, DFeature*);
static const char* get_tmpfile(const char*);

void 
d_featurelayer_load_fromdataset(DFeatureLayerHandle handle, DDatasetHandle src_dataset, const char* layer_name)
{
    GDALDatasetH dataset = d_dataset_getunderlyinghandle(src_dataset);
    
    if(layer_name == NULL)
    {
        handle->ogrlayer = GDALDatasetGetLayer(dataset,0);
        return;
    }

    handle->ogrlayer = GDALDatasetGetLayerByName(dataset, layer_name);

}

void
d_featurelayer_load_fromfeature(DFeatureLayerHandle handle, DFeature* feature, const char* layer_name)
{
    GDALDriverH shp_driver = GDALGetDriverByName("ESRI Shapefile");
    const char* path = get_tmpfile("shp");
    GDALDatasetH dataset = GDALCreate(shp_driver,path,0 ,0 ,0, GDT_Unknown, NULL);
    if(dataset == NULL)
    {
        D_LOG_ERROR("Error creating dataset for geometry layer %s", layer_name);
        return;
    }
    OGRLayerH layer = NULL;
    OGRwkbGeometryType ogr_geom_type;
    switch (feature->geom_type)
    {
    case DRUMLIN_GEOM_POINT:
        ogr_geom_type = wkbPoint;
        break;
    case DRUMLIN_GEOM_LINESTRING:
        ogr_geom_type = wkbLineString;
    
    default:
        break;
    }
    layer = GDALDatasetCreateLayer(dataset, layer_name, NULL, ogr_geom_type, NULL);

    if(layer == NULL)
    {
        D_LOG_ERROR("Could not create layer: %s", layer_name);
    }

    OGRFeatureH ogr_feature = OGR_F_Create(OGR_L_GetLayerDefn(layer));
    OGRGeometryH ogr_geom = OGR_G_CreateGeometry(ogr_geom_type);

    switch (feature->geom_type)
    {
    case DRUMLIN_GEOM_POINT:
        DPoint point = feature->geometry.point_geom;
        OGR_G_SetPoint_2D(ogr_geom,0, point.position.x, point.position.y);
        break;

    
    default:
        break;
    }

    OGR_F_SetGeometry(ogr_feature,ogr_geom);
    if(OGR_L_CreateFeature(layer,ogr_feature) != OGRERR_NONE)
    {
        D_LOG_ERROR("Could not create feature for layer %s", layer_name);
        return;
    }
    OGR_F_Destroy(ogr_feature);
    handle->ogrlayer = layer;

    
}

static void 
d_featurelayer_render(struct DLayer* layer, DBBox view_box, int zoom, void* userdata)
{
    DFeatureLayerHandle feature_layer = (DFeatureLayerHandle)layer;
    DMapHandle map = (DMapHandle)userdata;

    int num_features = OGR_L_GetFeatureCount(feature_layer->ogrlayer, 1);
    int i = 0;
    DLayerVectorGraphic graphic;
    graphic.feature_list = d_make_list(sizeof(DFeature),5);
    for(i = 0; i < num_features; i++)
    {
       
        
        OGRFeatureH ogr_feature = OGR_L_GetFeature(feature_layer->ogrlayer,i);
        OGRFeatureDefnH feature_defintion = OGR_F_GetDefnRef(ogr_feature);
        OGRwkbGeometryType ogr_geom_type =  OGR_FD_GetGeomType(feature_defintion);
        OGRGeometryH ogr_geom = OGR_F_GetGeometryRef(ogr_feature);
        DFeature feature;
        
        switch(ogr_geom_type)
        {
        case wkbPoint :
            
            point_to_feature(ogr_geom,&feature);
            d_list_append(graphic.feature_list,&feature);
            break;
        
        case wkbMultiLineString:
            int num_linestrings = OGR_G_GetGeometryCount(ogr_geom);
            for(int i = 0; i < num_linestrings; i++)
            {
                OGRGeometryH ogr_linestring = OGR_G_GetGeometryRef(ogr_geom,i);
                linestring_to_feature(ogr_linestring,&feature);
                d_list_append(graphic.feature_list,&feature);
            }

            break;
        case wkbLineString:
           
            linestring_to_feature(ogr_geom,&feature);
            d_list_append(graphic.feature_list,&feature);
            break;
        default:
            break;
        }
        


    }
    d_renderer_drawvector_pgeo(&graphic,map,d_map_getprojection(map));

    d_destroy_list(graphic.feature_list);
}



static void 
point_to_feature(OGRGeometryH ogr_geom, DFeature* out_feature)
{
    if(OGR_G_GetGeometryType(ogr_geom) != wkbPoint)
    {
        D_LOG_WARNING("Wrong type of geometry",NULL);
        return;
    }

    double z;
    DFeature feature;
    DFeatureGeometry geometry;
    
    OGR_G_GetPoint(ogr_geom,0,&(geometry.point_geom.position.x),&(geometry.point_geom.position.y),&z);
    
    feature.geometry = geometry;
    feature.geom_type = DRUMLIN_GEOM_POINT;

    memcpy(out_feature,&feature,sizeof(DFeature));
    

}


static void 
linestring_to_feature(OGRGeometryH ogr_geom,DFeature* out_feature)
{
    if(OGR_G_GetGeometryType(ogr_geom) != wkbLineString)
    {
        D_LOG_WARNING("Wrong type of geometry",NULL);
        return;
    }

    int num_points = OGR_G_GetPointCount(ogr_geom);
    out_feature->geom_type = DRUMLIN_GEOM_LINESTRING;
    out_feature->geometry.linestring_geom.linestrings = d_make_list(sizeof(DLine),num_points/2);
    
    for(int i = 0; i < num_points - 1; i ++)
    {
        DPoint a;
        DPoint b;
        
        OGR_G_GetPoint(ogr_geom,i,&a.position.x, &a.position.y,NULL);
        OGR_G_GetPoint(ogr_geom,i+1,&b.position.x,&b.position.y,NULL);

        DLine line = line(a.position,b.position);

        d_list_append(out_feature->geometry.linestring_geom.linestrings,&line);
    }



}

DFeatureLayerHandle 
d_make_featurelayer(const DFeatureLayerDesc* desc)
{
    DFeatureLayerHandle feature_layer = malloc(sizeof(struct DFeatureLayer));
    memset(feature_layer,0,sizeof(struct DFeatureLayer));
    feature_layer->base.type = DRUMLIN_LAYER_RASTER;

    feature_layer->base.metadata.attribution = desc->attribution;
    strncpy(feature_layer->base.metadata.name,desc->name,DLAYER_NAME_MAXLEN);
    feature_layer->base.render_graphic_func = d_featurelayer_render;
    feature_layer->ogrlayer = NULL;
    return feature_layer;
}


void 
d_destroy_featurelayer(const DFeatureLayerHandle handle)
{
    free(handle);
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

    int path_size = snprintf(&path[0],PATH_MAX,"%s.%s",tmp_filename_template, extension);
    if(path_size < 0)
    {
        D_LOG_ERROR("snprintf(): %s",strerror(errno));
        close(tmp_file);
        return NULL;
    }
    close(tmp_file);

    return path;

}