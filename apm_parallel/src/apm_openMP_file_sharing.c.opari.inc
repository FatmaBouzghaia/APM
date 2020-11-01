#include <opari2/pomp2_lib.h>

static POMP2_Region_handle pomp2_region_1 = NULL;
#define POMP2_DLIST_00002 shared(pomp2_region_2)
static POMP2_Region_handle pomp2_region_2 = NULL;

void POMP2_Init_regions_1604235938978529_2()
{
    POMP2_Assign_handle( &pomp2_region_1, "206*regionType=for*sscl=/netfs/ei1720/bouzghai/CSC5001/project/apm_parallel/src/apm_openMP_file_sharing.c:189:189*escl=/netfs/ei1720/bouzghai/CSC5001/project/apm_parallel/src/apm_openMP_file_sharing.c:228:228**" );
    POMP2_Assign_handle( &pomp2_region_2, "214*regionType=parallelfor*sscl=/netfs/ei1720/bouzghai/CSC5001/project/apm_parallel/src/apm_openMP_file_sharing.c:198:198*escl=/netfs/ei1720/bouzghai/CSC5001/project/apm_parallel/src/apm_openMP_file_sharing.c:227:227**" );
}
