#include <stdlib.h> // getenv

#ifdef ULAPI
#include "ulapi.h"
#endif
#ifdef RTAPI
#include "rtapi_flavor.h"
#include "rt-preempt.h"
#endif
#ifdef HAVE_XENOMAI2_THREADS
#include "xenomai2.h"
#endif

// Help for unit test mocking
int flavor_mocking = 0;  // Signal from tests
int flavor_mocking_err = 0; // Pass error to tests
// - Mock exit(status), returning NULL and passing error out of band
#define EXIT_NULL(status) do {                          \
        if (flavor_mocking) {                           \
            flavor_mocking_err = status; return NULL;   \
        } else exit(status);                            \
    } while (0)
// - Mock exit(status), returning (nothing) and passing error out of band
#define EXIT_NORV(status) do {                          \
        if (flavor_mocking) {                           \
            flavor_mocking_err = status; return;        \
        } else exit(status);                            \
    } while (0)


// Global flavor descriptor gets set after a flavor is chosen
flavor_descriptor_ptr flavor_descriptor = NULL;

// List of flavors compiled in
static flavor_descriptor_ptr flavor_list[] = {
#ifdef ULAPI
    &flavor_ulapi_descriptor,
#endif
#ifdef RTAPI
    &flavor_posix_descriptor,
    &flavor_rt_prempt_descriptor,
# ifdef HAVE_XENOMAI2_THREADS
    &flavor_xenomai2_descriptor,
# endif
#endif
    NULL
};


const char * flavor_names(flavor_descriptor_ptr ** fd)
{
    const char * name;
    do {
        if (*fd == NULL)
            // Init to beginning of list
            *fd = flavor_list;
        else
            // Go to next in list
            (*fd)++;
    } while (**fd != NULL && !flavor_can_run_flavor(**fd));

    if (**fd == NULL)
        // End of list; no name
        name = NULL;
    else
        // Not end; return name
        name = (**fd)->name;
    return name;
}

flavor_descriptor_ptr flavor_byname(const char *flavorname)
{
    flavor_descriptor_ptr * i;
    for (i = flavor_list; *i != NULL; i++) {
        if (!strcasecmp(flavorname, (*i)->name))
            break;
    }
    return *i;
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

flavor_descriptor_ptr flavor_default(void)
{
    const char *fname = getenv("FLAVOR");
    flavor_descriptor_ptr * flavor_handle = NULL;
    flavor_descriptor_ptr flavor = NULL;

    if (fname && fname[0]) {
        // $FLAVOR set in environment:  verify it or fail
        flavor = flavor_byname(fname);
	if (flavor == NULL) {
	    rtapi_print_msg(RTAPI_MSG_ERR, "FATAL:  No such flavor '%s';"
                            " valid flavors are\n", fname);
            for (flavor_handle=NULL; (fname=flavor_names(&flavor_handle)); )
		rtapi_print_msg(RTAPI_MSG_ERR, "FATAL:      %s\n",
                                (*flavor_handle)->name);
	    EXIT_NULL(100);
	}
        if (!flavor_can_run_flavor(flavor)) {
            rtapi_print_msg(RTAPI_MSG_ERR, "FATAL:  Flavor '%s' from"
                            " environment cannot run\n", fname);
            EXIT_NULL(101);
        } else {
            rtapi_print_msg(RTAPI_MSG_INFO,
                            "INFO:  Picked flavor '%s' id %d (from environment)\n",
                            flavor->name, flavor->flavor_id);
            return flavor;
        }

    } else {
        // Find best flavor automatically
        flavor = NULL;
        for (flavor_handle = flavor_list;
             *flavor_handle != NULL;
             flavor_handle++) {
            // Best is highest ID that can run
            if ( (!flavor || (*flavor_handle)->flavor_id > flavor->flavor_id)
                 && flavor_can_run_flavor(*flavor_handle) )
                flavor = (*flavor_handle);
        }
        if (!flavor) {
            // This should never happen:  POSIX can always run
            rtapi_print_msg(RTAPI_MSG_ERR,
                            "ERROR:  Unable to find runnable flavor\n");
            EXIT_NULL(102);
        } else {
            rtapi_print_msg(RTAPI_MSG_INFO, "INFO:  Picked default flavor '%s'"
                            " automatically\n", flavor->name);
            return flavor;
        }
    }
}

void flavor_install(flavor_descriptor_ptr flavor)
{
    if (flavor_descriptor != NULL) {
        rtapi_print_msg(RTAPI_MSG_ERR, "FATAL:  Flavor '%s' already"
                        " configured\n", flavor_descriptor->name);
        EXIT_NORV(103);
    }
    if (!flavor_can_run_flavor(flavor)) {
        rtapi_print_msg(RTAPI_MSG_ERR, "FATAL:  Flavor '%s' cannot run\n",
                        flavor->name);
        EXIT_NORV(104);
    }
    flavor_descriptor = flavor;
    rtapi_print_msg(RTAPI_MSG_DBG, "Installed flavor '%s'\n", flavor->name);
}

int flavor_is_configured(void)
{
    return flavor_descriptor != NULL;
}

#ifdef RTAPI
EXPORT_SYMBOL(flavor_names);
EXPORT_SYMBOL(flavor_is_configured);
EXPORT_SYMBOL(flavor_byname);
EXPORT_SYMBOL(flavor_default);
EXPORT_SYMBOL(flavor_install);
#endif
