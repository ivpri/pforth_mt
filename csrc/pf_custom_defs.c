/* pforth_mt complement code for custom words
 */

#include "pf_all.h"

#if defined PFCUSTOM_PLATFORM_FILE || defined PFCUSTOM_FILE
#define PFCUSTOM_DEFS
#include "pfcustom_mt.c"
#undef PFCUSTOM_DEFS
#endif
