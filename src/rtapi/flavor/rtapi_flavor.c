#include <stdlib.h> // getenv
#include <stdio.h>  // fprintf

#ifdef ULAPI
#include "ulapi.h"
#endif
#ifdef RTAPI
#include "rtapi_flavor.h"
#include "rt-preempt.h"
#endif
#ifdef HAVE_XENOMAI_THREADS
#include "xenomai.h"
#endif

flavor_descriptor_ptr flavor_descriptor = NULL;

static flavor_descriptor_ptr flavor_list[] = {
#ifdef ULAPI
    &flavor_ulapi_descriptor,
#endif
#ifdef RTAPI
    &flavor_posix_descriptor,
    &flavor_rt_prempt_descriptor,
# ifdef HAVE_XENOMAI_THREADS
    &flavor_xenomai_descriptor,
# endif
#endif
    NULL
};

const char * flavor_names(flavor_descriptor_ptr ** fd)
{
    const char * name;
    if (*fd == NULL)
        // Init to beginning of list
        *fd = flavor_list;
    else
        // Go to next in list
        (*fd)++;

    if (**fd == NULL)
        // End of list; no name
        name = NULL;
    else
        // Not end; return name
        name = (**fd)->name;
    return name;
}

rtapi_flavor_id_t flavor_byname(const char *flavorname)
{
    flavor_descriptor_ptr * i;
    for (i = flavor_list; *i != NULL; i++) {
	if (!strcasecmp(flavorname, (*i)->name))
	    return (*i)->flavor_id;
    }
    return RTAPI_FLAVOR_UNCONFIGURED_ID;
}

flavor_descriptor_ptr flavor_byid(rtapi_flavor_id_t flavor_id)
{
    flavor_descriptor_ptr * i;
    for (i = flavor_list; *i != NULL; i++) {
        if ((*i)->flavor_id == flavor_id)
            break;
    }
    return *i;
}

rtapi_flavor_id_t flavor_default(void)
{
    const char *fname = getenv("FLAVOR");
    flavor_descriptor_ptr * flavor = NULL;
    rtapi_flavor_id_t flavor_id = 0;

    // Return flavor passed through environment
    if (fname) {
        flavor_id = flavor_byname(fname);
	if (flavor_id == RTAPI_FLAVOR_UNCONFIGURED_ID) {
	    fprintf(stderr,
		    "FLAVOR=%s: no such flavor -- valid flavors are:\n",
		    fname);
            for (flavor=NULL; (fname=flavor_names(&flavor)); )
		fprintf(stderr, "\t%s\n", (*flavor)->name);
	    exit(1);
	}
        fprintf(stderr, "Picked default flavor '%s' (from environment)\n",
                (*flavor)->name);
        return (*flavor)->flavor_id;
    }

    // Find best flavor
    for (flavor = flavor_list; *flavor != NULL; flavor++) {
        if ((*flavor)->flavor_id > flavor_id
            && ((*flavor)->can_run_flavor == NULL
                || (*flavor)->can_run_flavor())) {
            flavor_id = (*flavor)->flavor_id;
            break;
        }
    }
    if (!flavor_id) {
        // This should never happen:  POSIX can always run
        fprintf(stderr, "ERROR:  Unable to find runnable flavor\n");
        exit(1);
    }
    fprintf(stderr, "Picked default flavor '%s' automatically\n",
            (*flavor)->name);
    return flavor_id;
}

int flavor_install(rtapi_flavor_id_t flavor_id)
{
    flavor_descriptor_ptr * i;
    for (i = flavor_list; *i != NULL; i++) {
        if ((*i)->flavor_id == flavor_id)
            break;
    }
    if (i != NULL)
        flavor_descriptor = *i;
    return i != NULL;
}

int flavor_is_configured(void)
{
    return flavor_descriptor->flavor_id != RTAPI_FLAVOR_UNCONFIGURED_ID;
}
