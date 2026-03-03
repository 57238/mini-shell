#include "minishell.h"
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int exec_cmd(char **args) {
	pid_t pid = fork();
	int status = 0;

	if (pid < 0) {
		perror("fork");
		return 1;
	}
	else if (pid == 0) {
		execvp(args[0], args);
		perror("execvp");
		_exit(127);
	}
	else {
		while (1) {
			pid_t w = waitpid(pid, &status, WUNTRACED);
			if (w == -1) {
				perror("mini-shell");
				return 1;
			}
			if (WIFEXITED(status))
                		return WEXITSTATUS(status);
           		if (WIFSIGNALED(status))
                		return 128 + WTERMSIG(status);
		}
	}	
	return 1;
}
