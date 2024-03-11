
#include <drumlin/drumlin.h>
#include <drumlin/providers.h>

int main(int argc, char** argv)
{
    drumlin_init();
    drumlin_start(&(DAppInitDesc){
        .width = 1280,
        .height = 720,
        .title = "Drumlin example"
    });

   
    DImage* skillissue_image = d_image_loadfromfile("./example_data/diagnosis-skill-issue.png");
    

    DTileServiceLayer* tileservice = d_make_tileservice(&(DTileServiceLayerDesc){
        .name = "OSM Tile Service",
        .uri_fmt = PROVIDER_OPENSTREETMAP,
        .attribution = "OpenStreetMap"
    });

    DRasterLayerHandle skillissue_quebec = d_make_rasterlayer(&(DRasterLayerDesc){
        .name = "Skill Issue",
        .attribution = "Nathan Lapp",
        
    });


    d_rasterlayer_load_fromimage(skillissue_quebec,skillissue_image);
    d_rasterlayer_setextent(skillissue_quebec,bbox(latlng(52, -72),latlng(50,-70)));

    DRasterLayerHandle skillissue_france = d_rasterlayer_copy(skillissue_quebec);
    d_rasterlayer_setextent(skillissue_france,bbox(latlng(48.85, 2.35),latlng(46.85,4.35)));
    

    DRasterLayerHandle skillissue_russia = d_rasterlayer_copy(skillissue_quebec);
    d_rasterlayer_setextent(skillissue_russia, bbox(latlng(55.755833, 37.617222), latlng(55.755833 - 5, 37.617222 + 5)));

   

    DMapHandle map = d_make_map(&(DMapInitDesc){
        .position = latlng(44.301111, -78.333333),
        .zoom = 12,
        
    });

    int x,y;
    
    D_LOG_INFO("Map resolution: %lf",d_map_resolution(map));
    d_map_addlayer(map,(DLayer*)tileservice);
    d_map_addlayer(map,(DLayer*)skillissue_quebec);
    d_map_addlayer(map,(DLayer*)skillissue_france);
    d_map_addlayer(map,(DLayer*)skillissue_russia);

    //d_map_addlayer(map,(DLayer*)hiking_overlay);
    d_map_render(map);

    while(!drumlin_shouldquit())
    {
        d_app_update();   
        struct DKeyEvent* key_event = d_app_getkeydown();
        double movespeed = 10;
        DCoord2 slide = coord2(0,0);
        int slide_zoom = 0;
        int update = 0;
        if(d_app_iskey(key_event,DRUMLIN_KEY_UP))
        {
            slide = coord2(0,1);
            update = 1;
        }
        else if(d_app_iskey(key_event,DRUMLIN_KEY_LEFT))
        {
            slide = coord2(-1,0);
            update = 1;
        }
        else if(d_app_iskey(key_event,DRUMLIN_KEY_RIGHT))
        {
            slide = coord2(1,0);
            update = 1;
        }
        else if(d_app_iskey(key_event,DRUMLIN_KEY_DOWN))
        {
            slide = coord2(0,-1);
            update = 1;
        }
        else if(d_app_iskey(key_event,DRUMLIN_KEY_SPACE))
        {
            slide_zoom = 1;
            update = 1;
        }
        else if(d_app_iskey(key_event,DRUMLIN_KEY_MINUS))
        {
            slide_zoom = -1;
            update = 1;
        }
        if(update)
        {
            d_map_slide(map,slide_zoom,slide,movespeed);
            d_map_render(map);
        }   
    }
    d_destroy_image(skillissue_image);
    
    d_destroy_rasterlayer(skillissue_quebec);
    d_destroy_rasterlayer(skillissue_france);
    d_destroy_rasterlayer(skillissue_russia);
    d_destroy_tileservice(tileservice);


    d_destroy_map(map);
    drumlin_stop();
}
