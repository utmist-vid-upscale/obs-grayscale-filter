#include "obs-module.h"
#include <stdbool.h>
#include "grayscale-filter.c"

bool obs_module_load(void)
{
    obs_register_source(&grayscale_filter_info);
    return true;
}