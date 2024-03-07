#include <stb_image.h>

#include <drumlin/image.h>


DImage*
d_image_loadfromfile(const char* path)
{
    DImage* image = malloc(sizeof(DImage));
    image->data = stbi_load(path,&image->width,&image->height,&image->channels,4);
    return image;
}

void 
d_destroy_image(DImage* image)
{
    stbi_image_free(image->data);
    free(image);
}


