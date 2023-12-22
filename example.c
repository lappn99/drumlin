#include <drumlin/drumlin.h>

int main(int argc, char** argv)
{
    DMapHandle map = d_make_map((DMapInitDesc){
        .position = latlng(0,0),
        .zoom = 19
    });

    

    d_destroy_map(map);

}