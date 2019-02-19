#ifndef RTAPI_FLAVOR_H
#define RTAPI_FLAVOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rtapi_common.h"
// Put these in order of preference
typedef enum RTAPI_FLAVOR_ID {
    RTAPI_FLAVOR_UNCONFIGURED_ID = 0,
    RTAPI_FLAVOR_ULAPI_ID,
    RTAPI_FLAVOR_POSIX_ID,
    RTAPI_FLAVOR_RT_PREEMPT_ID,
    RTAPI_FLAVOR_XENOMAI_ID,
    // Bogus
    RTAPI_FLAVOR_MAX_ID
} rtapi_flavor_id_t;


// Flavor features:  flavor_descriptor_t.flags bits for configuring flavor
// - Handy accessor
#define FLAVOR_FEATURE(f) (flavor_descriptor->flags & f)
// - Whether iopl() needs to be called
#define  FLAVOR_DOES_IO                    RTAPI_BIT(0)
// - Whether flavor has hard real-time latency
#define  FLAVOR_IS_RT                      RTAPI_BIT(1)
// - Whether flavor has hard real-time latency
#define  FLAVOR_TIME_NO_CLOCK_MONOTONIC    RTAPI_BIT(2)
// - Whether flavor runs outside RTAPI threads
#define  FLAVOR_NOT_RTAPI                  RTAPI_BIT(3)

// The exception code puts structs in shm in an opaque blob; this is used to
// check the allocated storage is large enough
// https://stackoverflow.com/questions/807244/
#define ASSERT_SIZE_WITHIN(type, size) \
    typedef char assertion_failed_##type##_[2*!!(sizeof(type) <= size)-1]

typedef int (*rtapi_can_run_flavor_t)(void);
typedef void (*rtapi_exception_handler_hook_t)(
    int type, rtapi_exception_detail_t *detail, int level);
typedef int (*rtapi_module_init_hook_t)(void);
typedef void (*rtapi_module_exit_hook_t)(void);
typedef int (*rtapi_task_update_stats_hook_t)(void);
typedef void (*rtapi_print_thread_stats_hook_t)(int task_id);
typedef int (*rtapi_task_new_hook_t)(task_data *task, int task_id);
typedef int (*rtapi_task_delete_hook_t)(task_data *task, int task_id);
typedef int (*rtapi_task_start_hook_t)(task_data *task, int task_id);
typedef void (*rtapi_task_stop_hook_t)(task_data *task, int task_id);
typedef int (*rtapi_task_pause_hook_t)(task_data *task, int task_id);
typedef int (*rtapi_wait_hook_t)(const int flags);
typedef int (*rtapi_task_resume_hook_t)(task_data *task, int task_id);
typedef void (*rtapi_delay_hook_t)(long int nsec);
typedef long long int (*rtapi_get_time_hook_t)(void);
typedef long long int (*rtapi_get_clocks_hook_t)(void);
typedef int (*rtapi_task_self_hook_t)(void);
typedef long long (*rtapi_task_pll_get_reference_hook_t)(void);
typedef int (*rtapi_task_pll_set_correction_hook_t)(long value);

typedef struct {
    const char *name;
    const int flavor_id;
    const unsigned long flags;
    const unsigned has_rt;
    const int time_no_clock_monotonic;
    const rtapi_can_run_flavor_t can_run_flavor;
    const rtapi_exception_handler_hook_t exception_handler_hook;
    const rtapi_module_init_hook_t module_init_hook;
    const rtapi_module_exit_hook_t module_exit_hook;
    const rtapi_task_update_stats_hook_t task_update_stats_hook;
    const rtapi_print_thread_stats_hook_t print_thread_stats_hook;
    const rtapi_task_new_hook_t task_new_hook;
    const rtapi_task_delete_hook_t task_delete_hook;
    const rtapi_task_start_hook_t task_start_hook;
    const rtapi_task_stop_hook_t task_stop_hook;
    const rtapi_task_pause_hook_t task_pause_hook;
    const rtapi_wait_hook_t wait_hook;
    const rtapi_task_resume_hook_t task_resume_hook;
    const rtapi_delay_hook_t delay_hook;
    const rtapi_get_time_hook_t get_time_hook;
    const rtapi_get_clocks_hook_t get_clocks_hook;
    const rtapi_task_self_hook_t task_self_hook;
    const rtapi_task_pll_get_reference_hook_t task_pll_get_reference_hook;
    const rtapi_task_pll_set_correction_hook_t task_pll_set_correction_hook;
} flavor_descriptor_t;
typedef flavor_descriptor_t * flavor_descriptor_ptr;

extern flavor_descriptor_ptr flavor_descriptor;

extern const char * flavor_names(flavor_descriptor_ptr ** fd);
extern rtapi_flavor_id_t flavor_byname(const char *flavorname);
extern flavor_descriptor_ptr flavor_byid(rtapi_flavor_id_t flavor_id);
extern rtapi_flavor_id_t flavor_default(void);
extern int flavor_is_configured(void);
extern int flavor_install(rtapi_flavor_id_t flavor_id);

#ifdef __cplusplus
}
#endif


#endif
