#include "runtime/rtapi_flavor.h"

int ulapi_module_init_hook(void) {
    if (global_data->magic != GLOBAL_READY) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"ULAPI:%d ERROR: global segment invalid magic:"
			" expected: 0x%x, actual: 0x%x\n",
			rtapi_instance, GLOBAL_READY,
			global_data->magic);
	return -EINVAL;
    }

    rtapi_print_msg(RTAPI_MSG_DBG, "ulapi_init(): ulapi loaded\n");

    // switch logging level to what was set in global via msgd:
    rtapi_set_msg_level(global_data->user_msg_level);
    return 0;
}

flavor_descriptor_t flavor_ulapi_descriptor = {
    .name = "ulapi",
    .flavor_id = RTAPI_FLAVOR_ULAPI_ID,
    .flags = FLAVOR_NOT_RTAPI,
    .can_run_flavor = NULL,
    .exception_handler_hook = NULL,
    .module_init_hook = ulapi_module_init_hook,
    .module_exit_hook = NULL,
    .task_update_stats_hook = NULL,
    .task_print_thread_stats_hook = NULL,
    .task_new_hook = NULL,
    .task_delete_hook = NULL,
    .task_start_hook = NULL,
    .task_stop_hook = NULL,
    .task_pause_hook = NULL,
    .task_wait_hook = NULL,
    .task_resume_hook = NULL,
    .task_delay_hook = NULL,
    .get_time_hook = NULL,
    .get_clocks_hook = NULL,
    .task_self_hook = NULL,
    .task_pll_get_reference_hook = NULL,
    .task_pll_set_correction_hook = NULL
};

