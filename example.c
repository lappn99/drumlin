#include <drumlin/drumlin.h>

int main(int argc, char** argv)
{
    drumlin_init();
    drumlin_start(&(DAppInitDesc){
        .width = 900,
        .height = 720,
        .title = "Drumlin example"
    });

    DMapHandle map = d_make_map(&(DMapInitDesc){
        .position = latlng(43.9153,-80.1086),
        .zoom = 8
    });

    int x,y;
    
    
    D_LOG_INFO("Map resolution: %lf",d_map_resolution(map));
    d_map_render(map);
    while(!drumlin_shouldquit())
    {
        d_app_update();   
    }



    d_destroy_map(map);
    drumlin_stop();
}