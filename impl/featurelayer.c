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
} ;

static void d_featurelayer_render(struct DLayer* , DBBox , int , void* );
static void multilinestring_to_vectorgraphic(OGRFeatureH,DLayerVectorGraphic*);
static void point_to_vectorgraphic(OGRFeatureH, DLayerVectorGraphic*);

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


static void 
d_featurelayer_render(struct DLayer* layer, DBBox view_box, int zoom, void* userdata)
{
    DFeatureLayerHandle feature_layer = (DFeatureLayerHandle)layer;
    DMapHandle map = (DMapHandle)userdata;
    

    

    int num_features = OGR_L_GetFeatureCount(feature_layer->ogrlayer, 1);
    int i = 0;
    for(i = 0; i < num_features; i++)
    {
        DLayerVectorGraphic graphic;
        
        OGRFeatureH feature = OGR_L_GetFeature(feature_layer->ogrlayer,i);
        OGRFeatureDefnH feature_defintion = OGR_F_GetDefnRef(feature);
        OGRwkbGeometryType geom_type =  OGR_FD_GetGeomType(feature_defintion);
        graphic.feature_list = d_make_list(sizeof(DFeature),5);
        switch(geom_type)
        {
        case wkbMultiLineString :
            multilinestring_to_vectorgraphic(feature,&graphic);
            break;
        case wkbPoint:
            point_to_vectorgraphic(feature,&graphic);
        
        default:
            break;
        }

        d_renderer_drawvector_pgeo(&graphic,map,d_map_getprojection(map));

        d_destroy_list(graphic.feature_list);

    }
}

static void
multilinestring_to_vectorgraphic(OGRFeatureH feature, DLayerVectorGraphic* graphic)
{

    
    OGRGeometryH multiline_geom =  OGR_F_GetGeometryRef(feature);
    int num_geom = OGR_G_GetGeometryCount(multiline_geom);
    
    

    for(int g = 0; g < num_geom; g++)
    {
        OGRGeometryH ogr_geometry = OGR_G_GetGeometryRef(multiline_geom,g);

        int num_points = OGR_G_GetPointCount(ogr_geometry);

        

        for(int p = 1; p < num_points; p++)
        {
            double z;
            DFeature feature;
            feature.geom_type = DRUMLIN_GEOM_LINE;
            OGR_G_GetPoint(ogr_geometry,p - 1,&feature.geometry.line_geom.b.x,&feature.geometry.line_geom.b.y,&z);
            OGR_G_GetPoint(ogr_geometry,p,&feature.geometry.line_geom.a.x,&feature.geometry.line_geom.a.y,&z);
            
            
            d_list_append(graphic->feature_list,&feature);
            
        }

    }
    


}

static void 
point_to_vectorgraphic(OGRFeatureH ogr_feature, DLayerVectorGraphic* graphic)
{
    OGRGeometryH ogr_geom = OGR_F_GetGeometryRef(ogr_feature);
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

    d_list_append(graphic->feature_list,&feature);
    

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

