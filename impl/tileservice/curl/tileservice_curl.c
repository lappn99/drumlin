#include <curl/curl.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <drumlin/math.h>
#include <drumlin/logging.h>
#include <drumlin/tileservice.h>
#include <drumlin/renderer.h>
#include <drumlin/container/list.h>

//const char OSM_TILESERVICE_URL[] = "https://tile.openstreetmap.org/%d/%d/%d.png";
const char CACHE_DIR[] = "/tmp/drumlin/";
const char CACHE_LOCATION_URI[] = "/tmp/drumlin/%d/%d/%d.png";
const char DRUMLIN_TILESERVICE_ENVNAME[] = "DRUMLIN_TILESERVICE_URI";


typedef struct
{
    unsigned char* buffer;
    size_t size;
    
} TilePNGData;

static int create_request_uri(char**,const char* , int, int, int);
static size_t recieve_data(void*, size_t, size_t, void*);
static int get_tile_from_server(const char*,int, int, int, TilePNGData*);
static int save_tile_to_cache(const char*, int,TilePNGData*);
static int get_tile_from_cache(int, int, int, TilePNGData*);


static CURL* curl;



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

    curl = curl_easy_init();
    curl_version_info_data* version_info = curl_version_info(CURLVERSION_NOW);
    D_LOG_INFO("Curl version: %s",version_info->version);
    char* user_agent;
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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,recieve_data);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl,CURLOPT_USERAGENT,user_agent);
    
    
    
    if(!curl)
    {
        D_LOG_ERROR("Could not init CURL",NULL);
        return 0;
    }
    free(user_agent);

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

    if(access(cache_location_uri,F_OK) == 0)
    {
        result = get_tile_from_cache(z,x,y,&tile_png);
    }
    else
    {
        result = get_tile_from_server(tileservice->uri_fmt,z,x,y,&tile_png);
        if(result)
        {
            
            save_tile_to_cache(cache_location_uri,uri_size,&tile_png);
        }
        
    }

    tile->raster = stbi_load_from_memory(tile_png.buffer,tile_png.size,&tile->width, &tile->height, &tile->bands,0);
    
    if(tile->raster == NULL)
    {
        D_LOG_ERROR("Could not decode tile image", NULL);
    }
    free(tile_png.buffer);

    return result;
    


}

void 
d_tileservce_render(DLayer* layer, DBBox view_box, int zoom)
{

    

    DTileServiceLayer* tileservice = (DTileServiceLayer*)layer; 
    DLayerGraphic graphic;
    memset(&graphic,0,sizeof(DLayerGraphic));
    
    
    int xmin, ymin;
    int xmax, ymax;
    int i,j;

    d_latlng_to_tilenum(view_box.min,zoom, &xmin, &ymin);
    d_latlng_to_tilenum(view_box.max,zoom, &xmax, &ymax);
    
    
    
    for(i = xmin; i <= xmax; i++)
    {
        for(j = ymin; j <= ymax; j++)
        {

           

            DTile tile;
            int result = d_tileservice_gettile(tileservice,zoom,i,j,&tile);
            if(!result)
            {
                D_LOG_ERROR("Could not render %d, %d, %d",zoom,i,j);
                return;
            }
            d_renderer_drawraster((DLayerRasterGraphic*)&tile,(i - xmin) * 256,(j - ymin) * 256);
            free(tile.raster);
        }
    }   

    
}

static int
get_tile_from_server(const char* tileservice_uri,int z, int x, int y, TilePNGData* tile_png)
{
    CURLcode res;
    
    char* url = NULL;
    int url_size = create_request_uri(&url,tileservice_uri,z,x,y);
    
    if(url_size < 0)
    {
        D_LOG_WARNING("Could not get tile %d/%d/%d",z,x,y);
        return 0;
    }
    D_LOG_INFO("Getting tile from url %s",url);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,tile_png);
    curl_easy_setopt(curl,CURLOPT_URL,url);
    curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
    res = curl_easy_perform(curl);

    if(res != CURLE_OK)
    {
        D_LOG_ERROR("curl_easy_perform(): %s",curl_easy_strerror(res));
        return 0;
    }
    D_LOG_INFO("Return code: %d", res);
    
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
        return 0;
    }

    int fd = open(uri, O_RDONLY);
    int size = lseek(fd,0,SEEK_END);
    if(size < 0)
    {
        D_LOG_WARNING("Could not read cached tile %s: %s", uri,strerror(errno));
        return 0;
    }
    (void)lseek(fd,0,SEEK_SET);
    png_data->size = size;
    png_data->buffer = malloc(size);
    size = read(fd,png_data->buffer,size);
    if(size < 0)
    {
        D_LOG_WARNING("Could not read cached tile %s: %s", uri,strerror(errno));
        return 0;
    }
    return 1;




    free(uri);
}

static int 
save_tile_to_cache(const char* uri, int uri_size, TilePNGData* image)
{


    char* tmp_uri = malloc((uri_size * sizeof(char)) + 1);
    memset(tmp_uri,'\0',(uri_size * sizeof(char)) + 1);
    char* next_dir;
    D_LOG_INFO("%s",uri);
    next_dir = strchr(uri,'/');
    
    
    
    while((next_dir = strchr(next_dir + 1,'/')) != NULL)
    {
        strncpy(tmp_uri,uri,(size_t)(next_dir - uri));
        if(access(tmp_uri,F_OK) != 0)
        {
            D_LOG_INFO("%s",tmp_uri);

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
    memcpy(&(tile->buffer[tile->size]),data,size * nmemb);
    tile->size += size * nmemb;
    
    D_LOG_INFO("Received %d data",size * nmemb);

    return size * nmemb;

}

