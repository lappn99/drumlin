#include <alloca.h>
#include <curl/curl.h>
#include <errno.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <drumlin/app.h>
#include <drumlin/map.h>
#include <drumlin/math.h>
#include <drumlin/logging.h>
#include <drumlin/tileservice.h>
#include <drumlin/renderer.h>
#include <drumlin/container/list.h>
#include <drumlin/projection.h>

const char CACHE_DIR[] = "/tmp/drumlin/";

const char CACHE_LOCATION_URI[] = "/tmp/drumlin/%d/%d/%d.png";

const char DRUMLIN_TILESERVICE_ENVNAME[] = "DRUMLIN_TILESERVICE_URI";

#define DRUMLIN_CACHE_TILES 1
#ifndef DRUMLIN_CACHE_TILES 
#define DRUMLIN_CACHE_TILES 1
#endif


typedef struct
{
    unsigned char* buffer;
    size_t size;
    const char* cache_location_uri;
    size_t uri_size;
    
} TilePNGData;


typedef struct 
{
    CURL* handle;
    int running_handles;
} CURLHttpOperation;

typedef struct 
{
    int fd;
} AIOOperation;

typedef enum 
{
    OPERATION_NONE,
    OPERATION_AIO,
    OPERATION_CURL

} OperationType;

typedef struct
{
    OperationType operation_type;
    union
    {
        CURLHttpOperation curl_op;
        AIOOperation aio_op;
    };


} AsyncOperation;

static int create_request_uri(char**, const char*, int, int, int);
static size_t recieve_data(void*, size_t, size_t, void*);
static int get_tile_from_server(const char*, int, int, int, TilePNGData*, AsyncOperation*);
static int get_tile_from_cache(int, int, int, TilePNGData*);
static int save_tile_to_cache(const char*, int, TilePNGData*);
static void prepare_curl_easy_handle(CURL*);

static CURLM* curl_multi;
static char* user_agent;

DTileServiceLayer* 
d_make_tileservice(DTileServiceLayerDesc* desc)
{
    DTileServiceLayer* tileservice = malloc(sizeof(DTileServiceLayer));
    memset(tileservice,0,sizeof(DTileServiceLayer));
    tileservice->base.metadata.attribution = desc->attribution;
    strncpy(tileservice->base.metadata.name,desc->name,DLAYER_NAME_MAXLEN);
    strcpy(tileservice->uri_fmt,desc->uri_fmt);
    tileservice->base.render_graphic_func = d_tileservce_render;
    tileservice->base.type = DRUMLIN_LAYER_RASTER;
    
    return tileservice;
}

void
d_destroy_tileservice(DTileServiceLayer* tileservice)
{
    free(tileservice);
}

int 
d_init_tileservice(void)
{
    
    
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl_multi = curl_multi_init();
    curl_version_info_data* version_info = curl_version_info(CURLVERSION_NOW);
    D_LOG_INFO("Curl version: %s",version_info->version);
    
    int ua_size = snprintf(NULL,0,"curl/%s",version_info->version);
    if(ua_size < 0)
    {
        D_LOG_WARNING("Could not set User-Agent header field",NULL);
        
    }
    else
    {
        user_agent = malloc(sizeof(char) * (ua_size + 1));
        ua_size = sprintf(user_agent,"curl/%s",version_info->version);
    }

    
    
    
    if(!curl_multi)
    {
        D_LOG_ERROR("Could not init CURL",NULL);
        return 0;
    }
    

    return 1;


}

int 
d_tileservice_gettile(DTileServiceLayer* tileservice,int z, int x, int y, DTile* tile)
{
    if(!tile)
    {
        D_LOG_WARNING("TILE can not be null", NULL);
        return 0;
    }
    
    char* cache_location_uri;
    TilePNGData tile_png = {0};
    int uri_size = create_request_uri(&cache_location_uri,CACHE_LOCATION_URI,z,x,y);

    if(uri_size == 0)
    {
        return 0;
    }
    int result;
    AsyncOperation async;
    if(access(cache_location_uri,F_OK) == 0)
    {
        result = get_tile_from_cache(z,x,y,&tile_png);
    }
    else
    {
        result = get_tile_from_server(tileservice->uri_fmt,z,x,y,&tile_png,&async);
  
        
        if(result)
        {
            do
            {
                CURLMcode mc = curl_multi_poll(curl_multi,NULL,0,1000,NULL);
                if(mc)
                {
                    D_LOG_WARNING("curl_multi_poll():", curl_multi_strerror(mc));
                    continue;
                }
                mc = curl_multi_perform(curl_multi,&async.curl_op.running_handles);
                if(mc)
                {
                    D_LOG_WARNING("curl_multi_poll():", curl_multi_strerror(mc));
                    continue;
                }
            }while(async.curl_op.running_handles);
            
            curl_multi_remove_handle(curl_multi,async.curl_op.handle);
            curl_easy_cleanup(async.curl_op.handle);

            save_tile_to_cache(cache_location_uri,uri_size,&tile_png);
            
        }
        
    }

    tile->raster = stbi_load_from_memory(tile_png.buffer,tile_png.size,&tile->width, &tile->height, &tile->bands,0);
    
    if(tile->raster == NULL)
    {
        D_LOG_ERROR("Could not decode tile image", NULL);
    }
    free(tile_png.buffer);
    free(cache_location_uri);
    return result;
    


}

int 
d_tileservice_gettiles(DTileServiceLayer* tileservice, int z, int x1, int y1, int x2, int y2, DTile** tiles)
{
    int x,y;
    int width = x2 - x1;
    int height = y2 - y1;
    DListHandle async_operations = d_make_list(sizeof(AsyncOperation),(width + 1) * (height + 1));
    DListHandle pngs = d_make_list(sizeof(TilePNGData*), (width + 1) * (height + 1));
    
    //unsigned char* uri_hash = SHA1((unsigned char*)tileservice->uri_fmt,strlen(tileservice->uri_fmt),NULL);
    

    for(y = y1; y <= y2; y++)
    {
        for(x = x1; x <= x2; x++)
        {
            

            char* cache_location_uri;
            int uri_size = create_request_uri(&cache_location_uri,CACHE_LOCATION_URI,z,x,y);


            AsyncOperation operation = {0};

            TilePNGData* png = malloc(sizeof(TilePNGData));
            
            d_list_append(pngs,&png);
            memset(png,0,sizeof(TilePNGData));
            png->cache_location_uri = cache_location_uri;
            png->uri_size = uri_size;
            
            if(access(cache_location_uri,F_OK) == 0 && DRUMLIN_CACHE_TILES)
            {
                int result = get_tile_from_cache(z,x,y,png);
                if(!result)
                {
                    D_LOG_WARNING("Could not get tile from cache",NULL);
                }
                else
                {
                    (void)d_list_append(async_operations,&operation);
                    continue;
                }
                

            }

            int result = get_tile_from_server(tileservice->uri_fmt,z,x,y,
                png,
                &operation);
            (void)d_list_append(async_operations,&operation);
            if(!result)
            {
                D_LOG_WARNING("Could not get tile: %d/%d/%d",z,x,y);
                continue;
            }            
        }
    }
    
    int running_handles;

    do
    {
        CURLMcode mc = curl_multi_perform(curl_multi,&running_handles);
         
        
        if(mc)
        {
            D_LOG_WARNING("curl_multi_poll(): %s", curl_multi_strerror(mc));
            continue;
        }
         mc = curl_multi_poll(curl_multi,NULL,0,100,NULL);
        if(mc)
        {
            D_LOG_WARNING("curl_multi_poll(): %s", curl_multi_strerror(mc));
            continue;
        }
        

    }while(running_handles > 0);
    
    for(y = 0; y <= height; y++)
    {
        for(x = 0; x <= width; x++)
        {
            AsyncOperation* operation = d_list_get(async_operations,(y * (width + 1)) + x);
            TilePNGData* png = *((TilePNGData**)d_list_get(pngs,(y * (width + 1)) + x));

            DTile* tile = &tiles[y][x];
            tile->y = y + y1;
            tile->x = x + x1;

            tiles[y][x].raster = stbi_load_from_memory(png->buffer,png->size,&tile->width,
                &tile->height,&tile->bands,0);
                
            if(operation->operation_type == OPERATION_CURL)
            {
                if(DRUMLIN_CACHE_TILES)
                {
                    save_tile_to_cache(png->cache_location_uri,png->uri_size,png);
                }
                
                curl_multi_remove_handle(curl_multi,operation->curl_op.handle);
                curl_easy_cleanup(operation->curl_op.handle);   
                
            }
            free((char*)png->cache_location_uri);
            free(png->buffer);
            free(png);
            png = NULL;
        }
    }
    d_destroy_list(pngs);
    d_destroy_list(async_operations);

    return 1;

}

void 
d_tileservce_render(DLayer* layer, DBBox view_box, int zoom, void* userdata)
{
    DTileServiceLayer* tileservice = (DTileServiceLayer*)layer; 
    DLayerGraphic graphic;
    memset(&graphic,0,sizeof(DLayerGraphic));
    DMapHandle map = (DMapHandle)userdata;
    
    int xmin, ymin;
    int xmax, ymax;
    int i;

    d_latlng_to_tilenum(view_box.min,zoom, &xmin, &ymin);
    d_latlng_to_tilenum(view_box.max,zoom, &xmax, &ymax);
    DTile** tiles = malloc(sizeof(DTile*) * ((ymax - ymin) + 1));

    for(i = 0; i <= (ymax - ymin);i++)
    {
        tiles[i] = malloc(sizeof(DTile) * ((xmax - xmin) + 1));
        memset(tiles[i],0,sizeof(DTile) * ((xmax - xmin) + 1));
    }

    d_tileservice_gettiles(tileservice,zoom,xmin,ymin,xmax,ymax,tiles);
    
    int window_width, window_height;
    d_app_getwindowsize(&window_width,&window_height);
    
    DProjectionHandle projection = d_create_projection(d_map_getgcs(map),d_map_getpcs(map));


    for(int y = 0; y <= (ymax - ymin); y++)
    {
        for(int x = 0; x <= (xmax - xmin); x++)
        {
          
            
            DTile* tile = &tiles[y][x];
            DLatLng tile_latlng;

            d_tilenum_to_latlng(tile->x, tile->y, zoom, &tile_latlng);

            
            d_renderer_drawraster_pgeo((DLayerRasterGraphic*)tile,map,tile_latlng,projection);          
            
            free(tile->raster);
        }
    }

    free(projection);

    for(i = 0; i <= (ymax - ymin);i++)
    {
        free(tiles[i]);
    }
    free(tiles);

    
}

static int
get_tile_from_server(const char* tileservice_uri,int z, int x, int y, TilePNGData* tile_png,AsyncOperation* operation)
{
    
    CURL* curl;
    char* url = NULL;
    int url_size = create_request_uri(&url,tileservice_uri,z,x,y);
    
    if(url_size < 0)
    {
        D_LOG_WARNING("Could not get tile %d/%d/%d",z,x,y);
        return 0;
    }

    curl = curl_easy_init();
    if(curl == NULL)
    {
        D_LOG_ERROR("Could not get create CURL handle", NULL);
        return 0;
    }
    operation->operation_type = OPERATION_CURL;
    operation->curl_op.handle = curl;


    D_LOG_INFO("Getting tile from url %s",url);

    prepare_curl_easy_handle(curl);

    curl_easy_setopt(curl,CURLOPT_WRITEDATA,tile_png);
    curl_easy_setopt(curl,CURLOPT_URL,url);

    curl_multi_add_handle(curl_multi,curl);

    
    free(url);
    return 1;
}

static int 
get_tile_from_cache(int z, int x, int y, TilePNGData* png_data)
{
    char* uri = NULL;
    int uri_size = create_request_uri(&uri,CACHE_LOCATION_URI,z,x,y);
    
    if(uri_size < 0)
    {
        D_LOG_WARNING("Could not get tile %d/%d/%d",z,x,y);
        free(uri);
        return 0;
    }

    int fd = open(uri, O_RDONLY);
    int size = lseek(fd,0,SEEK_END);
    if(size < 0)
    {
        D_LOG_WARNING("Could not read cached tile %s: %s", uri,strerror(errno));
        free(uri);
        return 0;
    }
    (void)lseek(fd,0,SEEK_SET);
    png_data->size = size;
    png_data->buffer = malloc(size);
    size = read(fd,png_data->buffer,size);
    if(size < 0)
    {
        D_LOG_WARNING("Could not read cached tile %s: %s", uri,strerror(errno));
        free(uri);
        return 0;
    }
    free(uri);
    return 1;



}

static int 
save_tile_to_cache(const char* uri, int uri_size, TilePNGData* image)
{



    char* tmp_uri = malloc((uri_size * sizeof(char)) + 1);
    memset(tmp_uri,'\0',(uri_size * sizeof(char)) + 1);
    char* next_dir;
    
    next_dir = strchr(uri,'/');
    
    while((next_dir = strchr(next_dir + 1,'/')) != NULL)
    {
        strncpy(tmp_uri,uri,(size_t)(next_dir - uri));
        if(access(tmp_uri,F_OK) != 0)
        {
           
            if(mkdir(tmp_uri,0777 ) < 0)
            {
                D_LOG_WARNING("Could not make cache at %s: %s",tmp_uri, strerror(errno));
                return 0;
            }

        }
    }
    int fd = open(uri, O_RDWR | O_CREAT, 0666);
    if(fd < 0)
    {
        D_LOG_WARNING("Could not create tile cache %s: %s", uri,strerror(errno));
        return 0;
    }
    int written = write(fd,image->buffer,(size_t)image->size);
    if(written < 0)
    {
        D_LOG_WARNING("Could not write to cache file %s: %s",uri,strerror(errno));
        return 0;
    }
    (void)close(fd);
    return 1;
}

static void
prepare_curl_easy_handle(CURL* curl)
{
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,recieve_data);
    curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,recieve_data);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,user_agent);
}

static int
create_request_uri(char** url,const char* prefix, int z, int x, int y)
{
    int size = snprintf(NULL,0,prefix,z,x,y);
    if(size < 0)
    {
        D_LOG_ERROR("Could not create request URL: %s",strerror(errno));
        return -1;
    }
    *url = malloc(sizeof(char) * ( size + 1));
    size = sprintf(*url,prefix,z,x,y);
    if(size < 0)
    {
        D_LOG_ERROR("Could not create request URL: %s", strerror(errno));
    }
    return size;


}

static size_t 
recieve_data(void* data, size_t size, size_t nmemb, void* userdata)
{
    
    TilePNGData* tile = (TilePNGData*)userdata;
    tile->buffer = realloc(tile->buffer,tile->size + (size * nmemb));
    if(tile->buffer == NULL)
    {
        D_LOG_ERROR("realloc(): %s",strerror(errno));
    }
    memcpy(&(tile->buffer[tile->size]),data,size * nmemb);
    tile->size += size * nmemb;
    
    //D_LOG_INFO("Received %d data",size * nmemb);

    return size * nmemb;

}


