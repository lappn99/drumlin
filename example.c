#include <drumlin/drumlin.h>

int main(int argc, char** argv)
{
    //D_LOG_INFO("Start",NULL);
    drumlin_init();
    drumlin_start(&(DAppInitDesc){
        .width = 900,
        .height = 720,
        .title = "Drumlin example"
    });

    DTileServiceLayer* tileservice = d_make_tileservice(&(DTileServiceLayerDesc){
        .name = "OSM Tile Service",
        .uri_fmt = "https://tile.openstreetmap.org/%d/%d/%d.png",
        .attribution = "OpenStreetMap"
    });

    DMapHandle map = d_make_map(&(DMapInitDesc){
        .position = latlng(43.915278, -80.108611),
        .zoom = 12
    });

    int x,y;
    
    
    D_LOG_INFO("Map resolution: %lf",d_map_resolution(map));
    d_map_addlayer(map,(DLayer*)tileservice);
    
    d_map_render(map);
    while(!drumlin_shouldquit())
    {
        
        d_app_update();   
        
    }


    d_destroy_tileservice(tileservice);
    d_destroy_map(map);
    drumlin_stop();
}
