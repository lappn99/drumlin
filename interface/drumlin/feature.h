#ifndef _DRUMLING_FEATURE_H
#define _DRUMLING_FEATURE_H

struct DFeature;

typedef struct DFeature* DFeatureHandle;

DFeatureHandle d_make_feature(void);
void d_destroy_feature(DFeatureHandle);

void d_feature_loaddata(DFeatureHandle, const char*);


#endif //_DRUMLING_FEATURE_H