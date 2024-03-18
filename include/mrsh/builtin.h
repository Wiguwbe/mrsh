#ifndef MRSH_BUILTIN_H
#define MRSH_BUILTIN_H

#include <mrsh/shell.h>

typedef int (*mrsh_builtin_func)(struct mrsh_state *state,
	int argc, char *argv[]);

struct mrsh_builtin {
	const char *name;
	mrsh_builtin_func func;
	bool special;
};

bool mrsh_has_builtin(const char *name);
bool mrsh_has_special_builtin(const char *name);
int mrsh_run_builtin(struct mrsh_state *state, int argc, char *argv[]);
bool mrsh_add_extra_builtin(struct mrsh_builtin extra_builtin);
bool mrsh_add_extra_builtins(struct mrsh_builtin *extra_builtins, size_t count);

struct mrsh_init_args {
	const char *command_file;
	const char *command_str;
};

int mrsh_process_args(struct mrsh_state *state, struct mrsh_init_args *args,
	int argc, char *argv[]);

#endif
