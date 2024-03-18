#ifndef BUILTIN_H
#define BUILTIN_H

#include <mrsh/builtin.h>

struct mrsh_state;

void print_escaped(const char *value);

int builtin_alias(struct mrsh_state *state, int argc, char *argv[]);
int builtin_bg(struct mrsh_state *state, int argc, char *argv[]);
int builtin_break(struct mrsh_state *state, int argc, char *argv[]);
int builtin_cd(struct mrsh_state *state, int argc, char *argv[]);
int builtin_command(struct mrsh_state *state, int argc, char *argv[]);
int builtin_colon(struct mrsh_state *state, int argc, char *argv[]);
int builtin_dot(struct mrsh_state *state, int argc, char *argv[]);
int builtin_eval(struct mrsh_state *state, int argc, char *argv[]);
int builtin_exec(struct mrsh_state *state, int argc, char *argv[]);
int builtin_exit(struct mrsh_state *state, int argc, char *argv[]);
int builtin_export(struct mrsh_state *state, int argc, char *argv[]);
int builtin_false(struct mrsh_state *state, int argc, char *argv[]);
int builtin_fg(struct mrsh_state *state, int argc, char *argv[]);
int builtin_getopts(struct mrsh_state *state, int argc, char *argv[]);
int builtin_hash(struct mrsh_state *state, int argc, char *argv[]);
int builtin_jobs(struct mrsh_state *state, int argc, char *argv[]);
int builtin_pwd(struct mrsh_state *state, int argc, char *argv[]);
int builtin_read(struct mrsh_state *state, int argc, char *argv[]);
int builtin_return(struct mrsh_state *state, int argc, char *argv[]);
int builtin_set(struct mrsh_state *state, int argc, char *argv[]);
int builtin_shift(struct mrsh_state *state, int argc, char *argv[]);
int builtin_times(struct mrsh_state *state, int argc, char *argv[]);
int builtin_trap(struct mrsh_state *state, int argc, char *argv[]);
int builtin_true(struct mrsh_state *state, int argc, char *argv[]);
int builtin_type(struct mrsh_state *state, int argc, char *argv[]);
int builtin_ulimit(struct mrsh_state *state, int argc, char *argv[]);
int builtin_umask(struct mrsh_state *state, int argc, char *argv[]);
int builtin_unalias(struct mrsh_state *state, int argc, char *argv[]);
int builtin_unset(struct mrsh_state *state, int argc, char *argv[]);
int builtin_wait(struct mrsh_state *state, int argc, char *argv[]);

int builtin_unspecified(struct mrsh_state *state, int argc, char *argv[]);

const char *state_get_options(struct mrsh_state *state);

struct mrsh_collect_var {
	const char *key, *value;
};

/** Collects and alpha-sorts variables matching attribs. Count will be set to
 * the number of matching variables. You are responsible for freeing the return
 * value when you're done.*/
struct mrsh_collect_var *collect_vars(struct mrsh_state *state,
	uint32_t attribs, size_t *count);

#endif
