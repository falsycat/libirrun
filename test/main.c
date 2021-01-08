#undef NDEBUG

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../irrun.h"

static void* resolve_(const char* name, void* udata) {
  typedef  struct symbol_t {
    const char* name;
    void*       addr;
  } symbol_t;

  static const symbol_t table_[] = {
    { "printf", &printf, },
  };

  for (size_t i = 0; i < sizeof(table_)/sizeof(table_[0]); ++i) {
    if (strcmp(name, table_[i].name) == 0) return table_[i].addr;
  }
  fprintf(stderr, "unknown function: %s\n", name);
  *(bool*) udata = false;
  return NULL;
}

int main(int argc, char** argv) {
  assert(argc >= 2);

  bool resolved = true;
  irrun_t* ctx = irrun_new(&resolve_, &resolved, IRRUN_OPTIMIZE_LEVEL_AGGRESSIVE);
  assert(ctx != NULL);

  if (!irrun_add_module_from_file(ctx, argv[1])) {
    fprintf(stderr, "%s\n", irrun_get_error(ctx));
    return EXIT_FAILURE;
  }

  int (*f)(int, char**) = irrun_sym(ctx, "jitmain");
  if (f == NULL) {
    fprintf(stderr, "%s\n", irrun_get_error(ctx));
    return EXIT_FAILURE;
  }
  if (!resolved) return EXIT_FAILURE;

  const int ret = f(argc, argv);
  irrun_delete(ctx);
  return ret;
}
