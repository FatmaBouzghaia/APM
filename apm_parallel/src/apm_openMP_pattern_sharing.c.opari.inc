#include <opari2/pomp2_lib.h>

#define POMP2_DLIST_00001 shared(pomp2_region_1)
static POMP2_Region_handle pomp2_region_1 = NULL;

void POMP2_Init_regions_1604235939176880_1()
{
    POMP2_Assign_handle( &pomp2_region_1, "220*regionType=parallelfor*sscl=/netfs/ei1720/bouzghai/CSC5001/project/apm_parallel/src/apm_openMP_pattern_sharing.c:189:189*escl=/netfs/ei1720/bouzghai/CSC5001/project/apm_parallel/src/apm_openMP_pattern_sharing.c:228:228**" );
}