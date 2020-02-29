// These accessors make hook access slightly more convenient, and most important
// make mocking possible for unit testing.
//
// They must be in a separate file from functions calling them for mock calls to
// work.
#include "rtapi_flavor.h"

#define SET_FLAVOR_DESCRIPTOR_DEFAULT() \
    do { if (f == NULL) f = flavor_descriptor; } while (0)
int flavor_can_run_flavor(flavor_descriptor_ptr f)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->can_run_flavor)
        return f->can_run_flavor();
    else
        return 1;
}
int flavor_exception_handler_hook(
    flavor_descriptor_ptr f, int type, rtapi_exception_detail_t *detail,
    int level)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->exception_handler_hook) {
        f->exception_handler_hook(type, detail, level);
        return 0;
    } else
        return -ENOSYS; // Unimplemented
}
int flavor_module_init_hook(flavor_descriptor_ptr f)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->module_init_hook)
        return f->module_init_hook();
    else
        return 0;
}
void flavor_module_exit_hook(flavor_descriptor_ptr f)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->module_exit_hook)
        f->module_exit_hook();
}
int flavor_task_update_stats_hook(flavor_descriptor_ptr f)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->task_update_stats_hook)
        return f->task_update_stats_hook();
    else
        return 0;
}
void flavor_task_print_thread_stats_hook(flavor_descriptor_ptr f, int task_id)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->task_print_thread_stats_hook)
        f->task_print_thread_stats_hook(task_id);
}
int flavor_task_new_hook(flavor_descriptor_ptr f, task_data *task, int task_id)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (flavor_descriptor->task_new_hook)
        return f->task_new_hook(task, task_id);
    else
        return -ENOSYS;  // Unimplemented
}
int flavor_task_delete_hook(
    flavor_descriptor_ptr f, task_data *task, int task_id)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->task_delete_hook)
        return f->task_delete_hook(task, task_id);
    else
        return 0;
}
int flavor_task_start_hook(
    flavor_descriptor_ptr f, task_data *task, int task_id)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    return f->task_start_hook(task, task_id);
}
int flavor_task_stop_hook(
    flavor_descriptor_ptr f, task_data *task, int task_id)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    return f->task_stop_hook(task, task_id);
}
int flavor_task_pause_hook(
    flavor_descriptor_ptr f, task_data *task, int task_id)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->task_pause_hook)
        return f->task_pause_hook(task, task_id);
    else
        return 0;
}
int flavor_task_wait_hook(flavor_descriptor_ptr f, const int flags)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->task_wait_hook)
        return f->task_wait_hook(flags);
    else
        return 0;
}
int flavor_task_resume_hook(
    flavor_descriptor_ptr f, task_data *task, int task_id)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->task_resume_hook)
        return f->task_resume_hook(task, task_id);
    else
        return -ENOSYS; // Unimplemented
}
void flavor_task_delay_hook(flavor_descriptor_ptr f, long int nsec)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->task_delay_hook)
        f->task_delay_hook(nsec);
}
long long int flavor_get_time_hook(flavor_descriptor_ptr f)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->get_time_hook)
        return f->get_time_hook();
    else
        return -ENOSYS; // Unimplemented
}
long long int flavor_get_clocks_hook(flavor_descriptor_ptr f)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (f->get_clocks_hook)
        return f->get_clocks_hook();
    else
        return -ENOSYS; // Unimplemented
}
int flavor_task_self_hook(flavor_descriptor_ptr f)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (flavor_descriptor->task_self_hook)
        return f->task_self_hook();
    else
        return -ENOSYS;
}
long long flavor_task_pll_get_reference_hook(flavor_descriptor_ptr f)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (flavor_descriptor->task_pll_get_reference_hook)
        return f->task_pll_get_reference_hook();
    else
        return 0;
}
int flavor_task_pll_set_correction_hook(flavor_descriptor_ptr f, long value)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    if (flavor_descriptor->task_pll_set_correction_hook)
        return f->task_pll_set_correction_hook(value);
    else
        return 0;
}

const char * flavor_name(flavor_descriptor_ptr f)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    return f->name;
}

int flavor_id(flavor_descriptor_ptr f)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    return f->flavor_id;
}

int flavor_feature(flavor_descriptor_ptr f, int feature)
{
    SET_FLAVOR_DESCRIPTOR_DEFAULT();
    return (f->flags & feature);
}

#ifdef RTAPI
EXPORT_SYMBOL(flavor_name);
EXPORT_SYMBOL(flavor_feature);
#endif
