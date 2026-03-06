#include "minishell.h"
#include "parser.h"
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int save_fds(int saved[2]) {
	saved[0] = dup(STDIN_FILENO);
	saved[1] = dup(STDOUT_FILENO);
	if (saved[0] < 0 || saved[1] < 0)
		return -1;
	return 0;
}

void restore_fds(int saved[2]) {
	dup2(saved[0], STDIN_FILENO);
	dup2(saved[1], STDOUT_FILENO);
	close(saved[0]);
	close(saved[1]);
}

int apply_redirs(redirect *r) {
	int fd;

	while (r) {
		fd = -1;
		if (r->type == TOK_REDIR_IN)
			fd = open(r->file, O_RDONLY);
		else if (r->type == TOK_REDIR_OUT)
			fd = open(r->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else if (r->type == TOK_APPEND)
			fd = open(r->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else if (r->type == TOK_HEREDOC) {
			fprintf(stderr, "mini-shell: heredoc not yet supported\n");
			return -1;
		}
		if (fd < 0) {
			perror(r->file);
			return -1;
		}
		if (r->type == TOK_REDIR_IN)
			dup2(fd, STDIN_FILENO);
		else
			dup2(fd, STDOUT_FILENO);
		close(fd);
		r = r->next;
	}
	return 0;
}

int exec_cmd(command *cmd) {
	pid_t pid = fork();
	int status = 0;

	if (pid < 0) {
		perror("fork");
		return 1;
	}
	if (pid == 0) {
		sig_child();
		if (apply_redirs(cmd->redirs) < 0)
			_exit(1);
		execvp(cmd->argv[0], cmd->argv);
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
