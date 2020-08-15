#ifndef RTAPI_FLAVOR_H
#define RTAPI_FLAVOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rtapi_common.h"

// Flavor features:  flavor_descriptor_t.flags bits for configuring flavor
// - Whether iopl() needs to be called
#define  FLAVOR_DOES_IO                    RTAPI_BIT(0)
// - Whether flavor has hard real-time latency
#define  FLAVOR_IS_RT                      RTAPI_BIT(1)
// - Whether flavor has hard real-time latency
#define  FLAVOR_TIME_NO_CLOCK_MONOTONIC    RTAPI_BIT(2)
// - Whether flavor runs outside RTAPI threads
#define  FLAVOR_NOT_RTAPI                  RTAPI_BIT(3)

#define MAX_FLAVOR_NAME_LEN 20

// The exception code puts structs in shm in an opaque blob; this is used to
// check the allocated storage is large enough
// https://stackoverflow.com/questions/807244/
#define ASSERT_SIZE_WITHIN(type, size)                                \
   typedef char assertion_failed_##type##_[2*!!(sizeof(type) <= size)-1]


// Put these in order of preference
    typedef enum RTAPI_FLAVOR_ID {
        RTAPI_FLAVOR_UNCONFIGURED_ID = 0,
        RTAPI_FLAVOR_ULAPI_ID,
        RTAPI_FLAVOR_POSIX_ID,
        RTAPI_FLAVOR_RT_PREEMPT_ID,
        RTAPI_FLAVOR_XENOMAI2_ID,
    } rtapi_flavor_id_t;


    // Hook type definitions for the flavor_descriptor_t struct
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
    typedef int (*rtapi_task_stop_hook_t)(task_data *task, int task_id);
    typedef int (*rtapi_task_pause_hook_t)(task_data *task, int task_id);
    typedef int (*rtapi_task_wait_hook_t)(const int flags);
    typedef int (*rtapi_task_resume_hook_t)(task_data *task, int task_id);
    typedef void (*rtapi_delay_hook_t)(long int nsec);
    typedef long long int (*rtapi_get_time_hook_t)(void);
    typedef long long int (*rtapi_get_clocks_hook_t)(void);
    typedef int (*rtapi_task_self_hook_t)(void);
    typedef long long (*rtapi_task_pll_get_reference_hook_t)(void);
    typedef int (*rtapi_task_pll_set_correction_hook_t)(long value);

    // All flavor-specific data is represented in this struct
    typedef struct {
        const char *name;
        const rtapi_flavor_id_t flavor_id;
        const unsigned long flags;
        const rtapi_can_run_flavor_t can_run_flavor;
        rtapi_exception_handler_hook_t exception_handler_hook;
        rtapi_module_init_hook_t module_init_hook;
        rtapi_module_exit_hook_t module_exit_hook;
        rtapi_task_update_stats_hook_t task_update_stats_hook;
        rtapi_print_thread_stats_hook_t task_print_thread_stats_hook;
        rtapi_task_new_hook_t task_new_hook;
        rtapi_task_delete_hook_t task_delete_hook;
        rtapi_task_start_hook_t task_start_hook;
        rtapi_task_stop_hook_t task_stop_hook;
        rtapi_task_pause_hook_t task_pause_hook;
        rtapi_task_wait_hook_t task_wait_hook;
        rtapi_task_resume_hook_t task_resume_hook;
        rtapi_delay_hook_t task_delay_hook;
        rtapi_get_time_hook_t get_time_hook;
        rtapi_get_clocks_hook_t get_clocks_hook;
        rtapi_task_self_hook_t task_self_hook;
        rtapi_task_pll_get_reference_hook_t task_pll_get_reference_hook;
        rtapi_task_pll_set_correction_hook_t task_pll_set_correction_hook;
    } flavor_descriptor_t;
    typedef flavor_descriptor_t * flavor_descriptor_ptr;

    // The global flavor_descriptor; points at the configured flavor
    extern flavor_descriptor_ptr flavor_descriptor;

    // Wrappers around flavor_descriptor
    typedef const char * (flavor_names_t)(flavor_descriptor_ptr ** fd);
    extern flavor_names_t flavor_names;
    typedef flavor_descriptor_ptr (flavor_byname_t)(const char *flavorname);
    extern flavor_byname_t flavor_byname;
    extern flavor_descriptor_ptr flavor_byid(rtapi_flavor_id_t flavor_id);
    typedef flavor_descriptor_ptr (flavor_default_t)(void);
    extern flavor_default_t flavor_default;
    typedef int (flavor_is_configured_t)(void);
    extern flavor_is_configured_t flavor_is_configured;
    typedef void (flavor_install_t)(flavor_descriptor_ptr flavor_id);
    extern flavor_install_t flavor_install;

    // Wrappers for functions in the flavor_descriptor_t
    extern int flavor_can_run_flavor(flavor_descriptor_ptr f);
    extern int flavor_exception_handler_hook(
        flavor_descriptor_ptr f, int type, rtapi_exception_detail_t *detail,
        int level);
    extern int flavor_module_init_hook(flavor_descriptor_ptr f);
    extern void flavor_module_exit_hook(flavor_descriptor_ptr f);
    extern int flavor_task_update_stats_hook(flavor_descriptor_ptr f);
    extern void flavor_task_print_thread_stats_hook(
        flavor_descriptor_ptr f, int task_id);
    extern int flavor_task_new_hook(
        flavor_descriptor_ptr f, task_data *task, int task_id);
    extern int flavor_task_delete_hook(
        flavor_descriptor_ptr f, task_data *task, int task_id);
    extern int flavor_task_start_hook(
        flavor_descriptor_ptr f, task_data *task, int task_id);
    extern int flavor_task_stop_hook(
        flavor_descriptor_ptr f, task_data *task, int task_id);
    extern int flavor_task_pause_hook(
        flavor_descriptor_ptr f, task_data *task, int task_id);
    extern int flavor_task_wait_hook(flavor_descriptor_ptr f, const int flags);
    extern int flavor_task_resume_hook(
        flavor_descriptor_ptr f, task_data *task, int task_id);
    extern void flavor_task_delay_hook(flavor_descriptor_ptr f, long int nsec);
    extern long long int flavor_get_time_hook(flavor_descriptor_ptr f);
    extern long long int flavor_get_clocks_hook(flavor_descriptor_ptr f);
    extern int flavor_task_self_hook(flavor_descriptor_ptr f);
    extern long long flavor_task_pll_get_reference_hook(
        flavor_descriptor_ptr f);
    extern int flavor_task_pll_set_correction_hook(
        flavor_descriptor_ptr f, long value);

    // Accessors for flavor_descriptor
    typedef const char * (flavor_name_t)(flavor_descriptor_ptr f);
    extern flavor_name_t flavor_name;
    extern int flavor_id(flavor_descriptor_ptr f);
    typedef int (flavor_feature_t)(flavor_descriptor_ptr f, int feature);
    extern flavor_feature_t flavor_feature;

    // Prototype for plugin flavor descriptor updater
    typedef void (*plugin_flavor_descriptor_updater_t)(flavor_descriptor_ptr);

    // Help for unit test mocking
    extern int flavor_mocking;
    extern int flavor_mocking_err;

#ifdef __cplusplus
}
#endif


#endif
