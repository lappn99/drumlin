
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

    DTileServiceLayer* tileservice = d_make_tileservice(&(DTileServiceLayerDesc){
        .name = "OSM Tile Service",
        .uri_fmt = PROVIDER_OPENSTREETMAP,
        .attribution = "OpenStreetMap"
    });
    
    DTileServiceLayer* hiking_overlay = d_make_tileservice(&(DTileServiceLayerDesc){
        .name = "Hiking Overlay",
        .uri_fmt = "https://tile.waymarkedtrails.org/hiking/%d/%d/%d.png",
        .attribution = "waymarkedtrails"
    });

    DMapHandle map = d_make_map(&(DMapInitDesc){
        .position = latlng(44.301111, -78.333333),
        //.position = latlng(0,0),
        .zoom = 12,
        
    });

    int x,y;
    
    D_LOG_INFO("Map resolution: %lf",d_map_resolution(map));
    d_map_addlayer(map,(DLayer*)tileservice);
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
    d_destroy_tileservice(tileservice);
    d_destroy_map(map);
    drumlin_stop();
}
