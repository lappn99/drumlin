#ifndef _DRUMLIN_DATASET_H
#define _DRUMLIN_DATASET_H

struct DDataset;
typedef struct DDataset* DDatasetHandle;

typedef enum
{
    DRUMLIN_DATASET_READ = 1,
    DRUMLIN_DATASET_WRITE = 2,
    DRUMLIN_DATASET_VECTOR,
    DRUMLIN_DATASET_RASTER,
    DRUMLIN_NUM_DATASET_OPTIONS
} DDatasetOptions;

void d_dataset_load(DDatasetHandle, const char*, DDatasetOptions);


DDatasetHandle d_make_dataset(void);
void d_destroy_dataset(DDatasetHandle);

void* d_dataset_getunderlyinghandle(DDatasetHandle);
void d_dataset_listlayers(DDatasetHandle);




#endif //_DRUMLIN_DATASET_H