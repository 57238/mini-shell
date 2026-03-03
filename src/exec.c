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
	if (pid == 0) {
		sig_child();
		execvp(args[0], args);
		perror("execvp");
		_exit(127);
	}
	signal(SIGINT, SIG_IGN); // ignore sigint while waiting
			
	if (waitpid(pid, &status, 0) == -1) {
		perror("mini-shell");
		sig_shell();
		return 1;
	}
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		write(1, "\n", 1);
	sig_shell();

	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	if (WIFSIGNALED(status))
		return 128 + WTERMSIG(status);
	return 1;
}
