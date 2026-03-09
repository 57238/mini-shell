#ifndef PARSER_H
#define PARSER_H

#include "token.h"

typedef struct redirect {
	token_type	type;
	char		*file;
	int		fd;
	struct redirect	*next;
} redirect;

typedef struct command {
	char		**argv;
	int		argc;
	redirect	*redirs;
	struct command	*next;
} command;

command	*parse_cmd(token *tokens);
void	free_cmds(command *cmd);

#endif
