#include "duv.h"

#include "refs.c"
#include "utils.c"
#include "loop.c"
#include "handle.c"
#include "timer.c"
#include "misc.c"

static const duk_function_list_entry duv_funcs[] = {
  // loop.c
  {"run", duv_run, 0},
  {"walk", duv_walk, 1},

  // handle.c
  {"close", duv_close, 2},

  // timer.c
  {"new_timer", duv_new_timer, 0},
  {"timer_start", duv_timer_start, 4},
  {"timer_stop", duv_timer_stop, 1},
  {"timer_again", duv_timer_again, 1},
  {"timer_set_repeat", duv_timer_set_repeat, 2},
  {"timer_get_repeat", duv_timer_get_repeat, 1},

  // misc.c
  {"guess_handle", duv_guess_handle, 1},
  {"version", duv_version, 0},
  {"version_string", duv_version_string, 0},
  {"get_process_title", duv_get_process_title, 0},
  {"set_process_title", duv_set_process_title, 1},
  {"resident_set_memory", duv_resident_set_memory, 0},
  {"uptime", duv_uptime, 0},
  {"getrusage", duv_getrusage, 0},
  {"cpu_info", duv_cpu_info, 0},
  {"interface_addresses", duv_interface_addresses, 0},
  {"loadavg", duv_loadavg, 0},
  {"exepath", duv_exepath, 0},
  {"cwd", duv_cwd, 0},
  {"chdir", duv_chdir, 1},
  {"get_total_memory", duv_get_total_memory, 0},
  {"hrtime", duv_hrtime, 0},

  {NULL, NULL, 0},
};


duk_ret_t dukopen_duv(duk_context *ctx) {

  duv_ref_setup(ctx);

  // Create a uv table on the global
  duk_push_object(ctx);
  duk_put_function_list(ctx, -1, duv_funcs);
  return 1;
}
