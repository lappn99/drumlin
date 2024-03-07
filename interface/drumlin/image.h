#ifndef _DRUMLIN_IMAGE_H
#define _DRUMLIN_IMAGE_H


typedef struct 
{
    int width;
    int height;
    int channels;
    float resolution;
    unsigned char* data;
    
} DImage;

DImage* d_image_loadfromfile(const char*);
void d_destroy_image(DImage*);


#endif