#include "minishell.h"
#include "token.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_shell(void) {
	char *line = NULL;
	int last_status = 0;
	size_t line_size = 0;

	sig_shell();
	while (1) {
		printf("mini-shell$ ");
		if (getline(&line, &line_size, stdin) == -1) {
			printf("\n");
			break;
		}
		token *tok = tokenize(line);
		command *cmd = parse_cmd(tok);
		free_tokens(tok);

		if (!cmd || !cmd->argv[0]) {
			free_cmds(cmd);
			continue;
		}
		if (strcmp(cmd->argv[0], "exit") == 0) {
			free_cmds(cmd);
			break;
		}
		if (strcmp(cmd->argv[0], "cd") == 0) {
			char *path = cmd->argv[1];
			if (!path)
				path = getenv("HOME");
			if (!path || chdir(path) != 0) {
				perror("cd");
				last_status = 1;
			} else
				last_status = 0;
			free_cmds(cmd);
			continue;
		}
		if (strcmp(cmd->argv[0], "echo") == 0) {
			int saved[2];
			int i = 1;
			if (cmd->redirs && (save_fds(saved) < 0 || apply_redirs(cmd->redirs) < 0)) {
				last_status = 1;
				free_cmds(cmd);
				continue;
			}
			while (cmd->argv[i]) {
				if (strcmp(cmd->argv[i], "$?") == 0)
					printf("%d", last_status);
				else
					printf("%s", cmd->argv[i]);
				if (cmd->argv[i + 1])
					printf(" ");
				i++;
			}
			printf("\n");
			if (cmd->redirs) {
				fflush(stdout);
				restore_fds(saved);
			}
			last_status = 0;
			free_cmds(cmd);
			continue;
		}
		last_status = exec_cmd(cmd);
		free_cmds(cmd);
	}
	free(line);
	return 0;
}
