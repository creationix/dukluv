#include "duv.h"

duk_ret_t duv_guess_handle(duk_context *ctx) {
  uv_file file = duk_require_uint(ctx, 0);
  switch (uv_guess_handle(file)) {
#define XX(uc, lc) case UV_##uc: duk_push_string(ctx, #uc); break;
  UV_HANDLE_TYPE_MAP(XX)
#undef XX
    case UV_FILE: duk_push_string(ctx, "FILE"); break;
    default: return 0;
  }
  return 1;
}

duk_ret_t duv_version(duk_context *ctx) {
 duk_push_uint(ctx, uv_version());
 return 1;
}

duk_ret_t duv_version_string(duk_context *ctx) {
 duk_push_string(ctx, uv_version_string());
 return 1;
}

duk_ret_t duv_get_process_title(duk_context *ctx) {
  char title[MAX_TITLE_LENGTH];
  duv_check(ctx, uv_get_process_title(title, MAX_TITLE_LENGTH));
  duk_push_string(ctx, title);
  return 1;
}

duk_ret_t duv_set_process_title(duk_context *ctx) {
  const char* title = duk_require_string(ctx, 0);
  duv_check(ctx, uv_set_process_title(title));
  return 0;
}

duk_ret_t duv_resident_set_memory(duk_context *ctx) {
  size_t rss;
  duv_check(ctx, uv_resident_set_memory(&rss));
  duk_push_number(ctx, rss);
  return 1;
}

duk_ret_t duv_uptime(duk_context *ctx) {
  double uptime;
  duv_check(ctx, uv_uptime(&uptime));
  duk_push_number(ctx, uptime);
  return 1;
}

static void duv_push_timeval_table(duk_context *ctx, const uv_timeval_t* t) {
  duk_push_object(ctx);
  duk_push_uint(ctx, t->tv_sec);
  duk_put_prop_string(ctx, -2, "sec");
  duk_push_uint(ctx, t->tv_usec);
  duk_put_prop_string(ctx, -2, "usec");
}

duk_ret_t duv_getrusage(duk_context *ctx) {
  uv_rusage_t rusage;
  duv_check(ctx, uv_getrusage(&rusage));
  duk_push_object(ctx);
  // user CPU time used
  duv_push_timeval_table(ctx, &rusage.ru_utime);
  duk_put_prop_string(ctx, -2, "utime");
  // system CPU time used
  duv_push_timeval_table(ctx, &rusage.ru_stime);
  duk_put_prop_string(ctx, -2, "stime");
  // maximum resident set size
  duk_push_uint(ctx, rusage.ru_maxrss);
  duk_put_prop_string(ctx, -2, "maxrss");
  // integral shared memory size
  duk_push_uint(ctx, rusage.ru_ixrss);
  duk_put_prop_string(ctx, -2, "ixrss");
  // integral unshared data size
  duk_push_uint(ctx, rusage.ru_idrss);
  duk_put_prop_string(ctx, -2, "idrss");
  // integral unshared stack size
  duk_push_uint(ctx, rusage.ru_isrss);
  duk_put_prop_string(ctx, -2, "isrss");
  // page reclaims (soft page faults)
  duk_push_uint(ctx, rusage.ru_minflt);
  duk_put_prop_string(ctx, -2, "minflt");
  // page faults (hard page faults)
  duk_push_uint(ctx, rusage.ru_majflt);
  duk_put_prop_string(ctx, -2, "majflt");
  // swaps
  duk_push_uint(ctx, rusage.ru_nswap);
  duk_put_prop_string(ctx, -2, "nswap");
  // block input operations
  duk_push_uint(ctx, rusage.ru_inblock);
  duk_put_prop_string(ctx, -2, "inblock");
  // block output operations
  duk_push_uint(ctx, rusage.ru_oublock);
  duk_put_prop_string(ctx, -2, "oublock");
  // IPC messages sent
  duk_push_uint(ctx, rusage.ru_msgsnd);
  duk_put_prop_string(ctx, -2, "msgsnd");
  // IPC messages received
  duk_push_uint(ctx, rusage.ru_msgrcv);
  duk_put_prop_string(ctx, -2, "msgrcv");
  // signals received
  duk_push_uint(ctx, rusage.ru_nsignals);
  duk_put_prop_string(ctx, -2, "nsignals");
  // voluntary context switches
  duk_push_uint(ctx, rusage.ru_nvcsw);
  duk_put_prop_string(ctx, -2, "nvcsw");
  // involuntary context switches
  duk_push_uint(ctx, rusage.ru_nivcsw);
  duk_put_prop_string(ctx, -2, "nivcsw");
  return 1;
}

duk_ret_t duv_cpu_info(duk_context *ctx) {
  uv_cpu_info_t* cpu_infos;
  int count, i;
  duv_check(ctx, uv_cpu_info(&cpu_infos, &count));
  duk_push_array(ctx);

  for (i = 0; i < count; i++) {
    duk_push_object(ctx);
    duk_push_string(ctx, cpu_infos[i].model);
    duk_put_prop_string(ctx, -2, "model");
    duk_push_uint(ctx, cpu_infos[i].speed);
    duk_put_prop_string(ctx, -2, "speed");
    duk_push_object(ctx);
    duk_push_uint(ctx, cpu_infos[i].cpu_times.user);
    duk_put_prop_string(ctx, -2, "user");
    duk_push_uint(ctx, cpu_infos[i].cpu_times.nice);
    duk_put_prop_string(ctx, -2, "nice");
    duk_push_uint(ctx, cpu_infos[i].cpu_times.sys);
    duk_put_prop_string(ctx, -2, "sys");
    duk_push_uint(ctx, cpu_infos[i].cpu_times.idle);
    duk_put_prop_string(ctx, -2, "idle");
    duk_push_uint(ctx, cpu_infos[i].cpu_times.irq);
    duk_put_prop_string(ctx, -2, "irq");
    duk_put_prop_string(ctx, -2, "times");
    duk_put_prop_index(ctx, -2, i);
  }

  uv_free_cpu_info(cpu_infos, count);
  return 1;
}

duk_ret_t duv_interface_addresses(duk_context *ctx) {
  uv_interface_address_t* interfaces;
  int count, i;
  char ip[INET6_ADDRSTRLEN];

  uv_interface_addresses(&interfaces, &count);

  duk_push_object(ctx);

  for (i = 0; i < count; i++) {

    duk_get_prop_string(ctx, -1, interfaces[i].name);
    if (!duk_is_object(ctx, -1)) {
      duk_pop(ctx);
      duk_push_array(ctx);
      duk_dup(ctx, -1);
      duk_put_prop_string(ctx, -3, interfaces[i].name);
    }

    duk_push_object(ctx);

    duk_push_boolean(ctx, interfaces[i].is_internal);
    duk_put_prop_string(ctx, -2, "internal");

    if (interfaces[i].address.address4.sin_family == AF_INET) {
      uv_ip4_name(&interfaces[i].address.address4, ip, sizeof(ip));
    } else if (interfaces[i].address.address4.sin_family == AF_INET6) {
      uv_ip6_name(&interfaces[i].address.address6, ip, sizeof(ip));
    } else {
      strncpy(ip, "<unknown sa family>", INET6_ADDRSTRLEN);
    }
    duk_push_string(ctx, ip);
    duk_put_prop_string(ctx, -2, "ip");

    duk_push_string(ctx, duv_protocol_to_string(interfaces[i].address.address4.sin_family));
    duk_put_prop_string(ctx, -2, "family");

    duk_put_prop_index(ctx, -2, duk_get_length(ctx, -2));

    duk_pop(ctx);
  }

  uv_free_interface_addresses(interfaces, count);
  return 1;
}

duk_ret_t duv_loadavg(duk_context *ctx) {
  double avg[3];
  int i;
  uv_loadavg(avg);
  duk_push_array(ctx);
  for (i = 0; i < 3; i++) {
    duk_push_number(ctx, avg[i]);
    duk_put_prop_index(ctx, -2, i);
  }
  return 1;
}

duk_ret_t duv_exepath(duk_context *ctx) {
  size_t size = 2*PATH_MAX;
  char exe_path[2*PATH_MAX];
  duv_check(ctx, uv_exepath(exe_path, &size));
  duk_push_lstring(ctx, exe_path, size);
  return 1;
}

duk_ret_t duv_cwd(duk_context *ctx) {
  size_t size = 2*PATH_MAX;
  char path[2*PATH_MAX];
  duv_check(ctx, uv_cwd(path, &size));
  duk_push_lstring(ctx, path, size);
  return 1;
}

duk_ret_t duv_chdir(duk_context *ctx) {
  const char* path = duk_require_string(ctx, 0);
  duv_check(ctx, uv_chdir(path));
  return 0;
}

duk_ret_t duv_get_total_memory(duk_context *ctx) {
  duk_push_number(ctx, uv_get_total_memory());
  return 1;
}

duk_ret_t duv_hrtime(duk_context *ctx) {
  duk_push_number(ctx, uv_hrtime());
  return 1;
}

