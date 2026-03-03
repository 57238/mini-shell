#include "minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int run_shell(void) {
	char *line;
	char **args;
	size_t line_size = 0;

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

		exec_cmd(args);
		free(args);

	}
	free(line);
	return 0;
}
