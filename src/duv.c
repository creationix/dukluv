#include "duv.h"

#include "refs.c"
#include "utils.c"
#include "loop.c"
#include "handle.c"
#include "timer.c"
#include "stream.c"
#include "tcp.c"
#include "pipe.c"
#include "tty.c"
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

  // stream.c
  {"shutdown", duv_shutdown, 2},

  // tcp.c
  {"new_tcp", duv_new_tcp, 0},
  {"tcp_open", duv_tcp_open, 2},
  {"tcp_nodelay", duv_tcp_nodelay, 2},
  {"tcp_keepalive", duv_tcp_keepalive, 3},
  {"tcp_simultaneous_accepts", duv_tcp_simultaneous_accepts, 2},
  {"tcp_bind", duv_tcp_bind, 4},
  {"tcp_getpeername", duv_tcp_getpeername, 1},
  {"tcp_getsockname", duv_tcp_getsockname, 1},
  {"tcp_connect", duv_tcp_connect, 4},

  // pipe.c
  {"new_pipe", duv_new_pipe, 1},
  {"pipe_open", duv_pipe_open, 2},
  {"pipe_bind", duv_pipe_bind, 2},
  {"pipe_connect", duv_pipe_connect, 3},
  {"pipe_getsockname", duv_pipe_getsockname, 1},
  {"pipe_pending_instances", duv_pipe_pending_instances, 2},
  {"pipe_pending_count", duv_pipe_pending_count, 1},
  {"pipe_pending_type", duv_pipe_pending_type, 1},

  // tty.c
  {"new_tty", duv_new_tty, 2},
  {"tty_set_mode", duv_tty_set_mode, 2},
  {"tty_reset_mode", duv_tty_reset_mode, 0},
  {"tty_get_winsize", duv_tty_get_winsize, 1},

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
