
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

    DImage* skillissue = d_image_loadfromfile("/home/nathanl/Pictures/diagnosis-skill-issue.png");
    DImage* based = d_image_loadfromfile("/home/nathanl/Pictures/based.png");

    DTileServiceLayer* tileservice = d_make_tileservice(&(DTileServiceLayerDesc){
        .name = "OSM Tile Service",
        .uri_fmt = PROVIDER_OPENSTREETMAP,
        .attribution = "OpenStreetMap"
    });

    DRasterLayer* skillissue_quebec = d_make_rasterlayer(&(DRasterLayerDesc){
        .name = "Skill Issue",
        .attribution = "Nathan Lapp",
        .image = skillissue,
        .position = latlng(52, -72)
    });
    DRasterLayer* skillissue_france = d_rasterlayer_copy(skillissue_quebec);
    skillissue_france->position = latlng(48.85, 2.35);

    DRasterLayer* skillissue_russia = d_rasterlayer_copy(skillissue_quebec);
    skillissue_russia->position = latlng(55.755833, 37.617222);

    DRasterLayer* based_orangeville = d_make_rasterlayer(&(DRasterLayerDesc){
        .name = "Based",
        .attribution = "Nathan Lapp",
        .image = based,
        .position = latlng(43.915278, -80.108611)
    });

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
    d_map_addlayer(map,(DLayer*)based_orangeville);

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
    d_destroy_rasterlayer(skillissue_quebec);
    d_destroy_rasterlayer(skillissue_france);
    d_destroy_rasterlayer(skillissue_russia);
    d_destroy_tileservice(tileservice);
    d_destroy_image(skillissue);
    d_destroy_map(map);
    drumlin_stop();
}
