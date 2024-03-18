#include <assert.h>
#include <mrsh/builtin.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "builtin.h"
#include "shell/shell.h"

static const struct mrsh_builtin builtins[] = {
	// Keep alpha sorted
	{ ".", builtin_dot, true },
	{ ":", builtin_colon, true },
	{ "alias", builtin_alias, false },
	{ "bg", builtin_bg, false },
	{ "break", builtin_break, true },
	{ "cd", builtin_cd, false },
	{ "command", builtin_command, false },
	{ "continue", builtin_break, true },
	{ "eval", builtin_eval, true },
	{ "exec", builtin_exec, true },
	{ "exit", builtin_exit, true },
	{ "export", builtin_export, true },
	{ "false", builtin_false, false },
//	{ "fc", builtin_fc, false },
	{ "fg", builtin_fg, false },
	{ "getopts", builtin_getopts, false },
	{ "hash", builtin_hash, false },
	{ "jobs", builtin_jobs, false },
//	{ "kill", builtin_kill, false },
//	{ "newgrp", builtin_newgrp, false },
	{ "pwd", builtin_pwd, false },
	{ "read", builtin_read, false },
	{ "readonly", builtin_export, true },
	{ "return", builtin_return, true },
	{ "set", builtin_set, true },
	{ "shift", builtin_shift, true },
	{ "times", builtin_times, true },
	{ "trap", builtin_trap, true },
	{ "true", builtin_true, false },
	{ "type", builtin_type, false },
	{ "ulimit", builtin_ulimit, false },
	{ "umask", builtin_umask, false },
	{ "unalias", builtin_unalias, false },
	{ "unset", builtin_unset, true },
	{ "wait", builtin_wait, false },
};

static struct mrsh_builtin *extra_builtins = NULL;
static int extra_builtin_count = 0;

static int builtin_compare(const void *_a, const void *_b) {
	const struct mrsh_builtin *a = _a, *b = _b;
	return strcmp(a->name, b->name);
}

static const struct mrsh_builtin *get_builtin(const char *name) {
	void *builtin = NULL;
	struct mrsh_builtin key = { .name = name };
	builtin = bsearch(&key, builtins, sizeof(builtins) / sizeof(builtins[0]),
		sizeof(builtins[0]), builtin_compare);
	if (builtin != NULL)
		return (struct mrsh_builtin*)builtin;

	return bsearch(&key, extra_builtins, extra_builtin_count,
		sizeof(struct mrsh_builtin), builtin_compare);
}

bool mrsh_add_extra_builtin(struct mrsh_builtin extra_builtin) {
	size_t new_size = extra_builtin_count + 1;
	void *new_ptr = realloc(extra_builtins, new_size * sizeof(struct mrsh_builtin));
	if (new_ptr == NULL) {
		fprintf(stderr, "failed to add new builtin: %s\n", strerror(errno));
		return false;
	}
	extra_builtins = (struct mrsh_builtin*)new_ptr;
	extra_builtins[extra_builtin_count++] = extra_builtin;
	return true;
}

bool mrsh_add_extra_builtins(struct mrsh_builtin *list, size_t count) {
	size_t new_size = extra_builtin_count + count;
	void *new_ptr = realloc(extra_builtins, new_size * sizeof(struct mrsh_builtin));
	if (new_ptr == NULL) {
		fprintf(stderr, "failed to add new builtins: %s\n", strerror(errno));
		return false;
	}
	memcpy(extra_builtins + extra_builtin_count, list, count);
	return true;
}

bool mrsh_has_builtin(const char *name) {
	return get_builtin(name) != NULL;
}

bool mrsh_has_special_builtin(const char *name) {
	const struct mrsh_builtin *builtin = get_builtin(name);
	return builtin != NULL && builtin->special;
}

int mrsh_run_builtin(struct mrsh_state *state, int argc, char *argv[]) {
	assert(argc > 0);

	const char *name = argv[0];
	const struct mrsh_builtin *builtin = get_builtin(name);
	if (builtin == NULL) {
		return -1;
	}

	return builtin->func(state, argc, argv);
}

void print_escaped(const char *value) {
	const char safe[] = "@%+=:,./-";
	size_t i;
	for (i = 0; value[i] != '\0'; ++i) {
		if (!isalnum(value[i]) && !strchr(safe, value[i])) {
			break;
		}
	}
	if (value[i] == '\0' && i > 0) {
		printf("%s", value);
	} else {
		printf("'");
		for (size_t i = 0; value[i] != '\0'; ++i) {
			if (value[i] == '\'') {
				printf("'\"'\"'");
			} else {
				printf("%c", value[i]);
			}
		}
		printf("'");
	}
}

struct collect_iter {
	size_t cap, len;
	uint32_t attribs;
	struct mrsh_collect_var *values;
};

static void collect_vars_iterator(const char *key, void *_var, void *data) {
	const struct mrsh_variable *var = _var;
	struct collect_iter *iter = data;
	if (iter->attribs != MRSH_VAR_ATTRIB_NONE
			&& !(var->attribs & iter->attribs)) {
		return;
	}
	if ((iter->len + 1) * sizeof(struct mrsh_collect_var) >= iter->cap) {
		iter->cap *= 2;
		iter->values = realloc(iter->values,
				iter->cap * sizeof(struct mrsh_collect_var));
	}
	iter->values[iter->len].key = key;
	iter->values[iter->len++].value = var->value;
}

static int varcmp(const void *p1, const void *p2) {
	const struct mrsh_collect_var *v1 = p1;
	const struct mrsh_collect_var *v2 = p2;
	return strcmp(v1->key, v2->key);
}

struct mrsh_collect_var *collect_vars(struct mrsh_state *state,
		uint32_t attribs, size_t *count) {
	struct mrsh_state_priv *priv = state_get_priv(state);

	struct collect_iter iter = {
		.cap = 64,
		.len = 0,
		.values = malloc(64 * sizeof(struct mrsh_collect_var)),
		.attribs = attribs,
	};
	mrsh_hashtable_for_each(&priv->variables, collect_vars_iterator, &iter);
	qsort(iter.values, iter.len, sizeof(struct mrsh_collect_var), varcmp);
	*count = iter.len;
	return iter.values;
}
