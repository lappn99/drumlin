#ifndef _DRUMLIN_H
#define _DRUMLIN_H

#include "logging.h"
#include "app.h"
#include "container/list.h"

int drumlin_init(void);
const char* drumlin_tmpfolder(void);

#include <drumlin/tileservice.h>
#include <drumlin/map.h>
#include <drumlin/dataset.h>
#include <drumlin/rasterlayer.h>
#include <drumlin/feature.h>


#endif //_DRUMLIN_H
