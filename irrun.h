#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct irrun_t irrun_t;

typedef void* (*irrun_resolver_t)(const char* name, void* udata);

typedef enum irrun_optimize_level_t {
  IRRUN_OPTIMIZE_LEVEL_NONE,
  IRRUN_OPTIMIZE_LEVEL_AGGRESSIVE,
} irrun_optimize_level_t;

irrun_t*
irrun_new(
  irrun_resolver_t       resolver,
  void*                  udata,
  irrun_optimize_level_t opt
);

void
irrun_delete(
  irrun_t* ctx
);

const char*
irrun_get_error(
  const irrun_t* ctx
);

int
irrun_add_module_from_file(
  irrun_t*    ctx,
  const char* path
);

void*
irrun_sym(
  irrun_t*    ctx,
  const char* name
);

#ifdef __cplusplus
}
#endif
