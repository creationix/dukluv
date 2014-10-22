#include "duv.h"

#include "refs.c"
#include "utils.c"
#include "loop.c"
#include "handle.c"
#include "timer.c"

static const duk_function_list_entry duv_funcs[] = {
  // From loop.c
  {"run", duv_run, 0},
  {"walk", duv_walk, 1},

  // From handle.c
  {"close", duv_close, 2},

  // from timer.c
  {"new_timer", duv_new_timer, 0},
  {"timer_start", duv_timer_start, 4},
  {"timer_stop", duv_timer_stop, 1},
  {"timer_again", duv_timer_again, 1},
  {"timer_set_repeat", duv_timer_set_repeat, 2},
  {"timer_get_repeat", duv_timer_get_repeat, 1},

  {NULL, NULL, 0},
};


duk_ret_t dukopen_duv(duk_context *ctx) {

  duv_ref_setup(ctx);

  // Create a uv table on the global
  duk_push_object(ctx);
  duk_put_function_list(ctx, -1, duv_funcs);
  return 1;
}
