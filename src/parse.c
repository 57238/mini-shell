#include "minishell.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static command *new_cmd(void) {
	command *cmd = malloc(sizeof(*cmd));
	if (!cmd)
		return NULL;
	cmd->argv = malloc(sizeof(char *));
	if (!cmd->argv) {
		free(cmd);
		return NULL;
	}
	cmd->argv[0] = NULL;
	cmd->argc = 0;
	cmd->redirs = NULL;
	cmd->next = NULL;
	return cmd;
}

static void add_arg(command *cmd, char *word) {
	char **new_argv = realloc(cmd->argv, (cmd->argc + 2) * sizeof(char *));
	if (!new_argv)
		return;
	cmd->argv = new_argv;
	cmd->argv[cmd->argc++] = strdup(word);
	cmd->argv[cmd->argc] = NULL;
}

static void add_redir(command *cmd, token_type type, char *file) {
	redirect *r = malloc(sizeof(*r));
	redirect *tail;
	if (!r)
		return;
	r->type = type;
	r->file = strdup(file);
	r->fd = -1;
	r->next = NULL;
	if (!cmd->redirs)
		cmd->redirs = r;
	else {
		tail = cmd->redirs;
		while (tail->next)
			tail = tail->next;
		tail->next = r;
	}
}

command *parse_cmd(token *tok) {
	command *head = NULL;
	command *tail = NULL;
	command *current = new_cmd();
	token_type rtype;

	if (!current)
		return NULL;
	while (tok) {
		if (tok->type == TOK_WORD) {
			add_arg(current, tok->text);
		} else if (tok->type == TOK_PIPE) {
			if (!head)
				head = current;
			else
				tail->next = current;
			tail = current;
			current = new_cmd();
			if (!current)
				return head;
		} else {
			rtype = tok->type;
			tok = tok->next;
			if (!tok || tok->type != TOK_WORD)
				break;
			add_redir(current, rtype, tok->text);
		}
		tok = tok->next;
	}
	if (!head)
		head = current;
	else
		tail->next = current;
	return head;
}

void free_cmds(command *cmd) {
	command *next;
	redirect *r;
	redirect *rn;
	int i;

	while (cmd) {
		next = cmd->next;
		i = 0;
		while (cmd->argv[i])
			free(cmd->argv[i++]);
		free(cmd->argv);
		r = cmd->redirs;
		while (r) {
			rn = r->next;
			free(r->file);
			if (r->fd >= 0)
				close(r->fd);
			free(r);
			r = rn;
		}
		free(cmd);
		cmd = next;
	}
}
