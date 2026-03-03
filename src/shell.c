#include "minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_shell(void) {
	char *line = NULL;
	char **args;
	int last_status = 0;
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
			if(!path || chdir(path) != 0) {
				perror("cd");
				last_status = 1;
			}
			else
				last_status = 0;
			free(args);
			continue;
		}
		if (strcmp(args[0], "echo") == 0) {
			int i = 1;
			while (args[i]) {
				if (strcmp(args[i], "$?") == 0)
					printf("%d", last_status);
				else
					printf("%s", args[i]);
				if (args[i+1])
					printf(" ");
				i++;
			}
			printf("\n");
			free(args);
			last_status = 0;
			continue;		
		}		
		last_status = exec_cmd(args);
		free(args);

	}
	free(line);
	return 0;
}
