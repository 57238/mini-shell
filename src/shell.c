#include "minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_shell(void) {
	char *line;
	char **args;
	size_t line_size = 0;
	
	sig_shell();
	while(1) {
		printf("mini-shell$ ");
		if (getline(&line, &line_size, stdin) == -1) {
			printf("\n");
			break;
		}
		args = split_line(line);
		if (!args || !args[0]) {
			free(args);
			continue;
		}
		if (strcmp(args[0], "exit") == 0) {
			free(args);
			break;
		}
		if (strcmp(args[0], "cd") == 0) {
			char *path = args[1];
			if (!path)
				path = getenv("HOME");
			if(!path || chdir(path) != 0)
				perror("cd");
			free(args);
			continue;
		}			
		exec_cmd(args);
		free(args);

	}
	free(line);
	return 0;
}
