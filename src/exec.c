#include "minishell.h"
#include "parser.h"
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
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

int exec_pipeline(command *cmds) {
	int n = 0;
	command *c;
	int (*pipes)[2];
	pid_t *pids;
	int i;
	int status;
	int last_status = 0;

	for (c = cmds; c; c = c->next)
		n++;
	if (n == 1)
		return exec_cmd(cmds);

	pipes = malloc((n - 1) * sizeof(*pipes));
	pids = malloc(n * sizeof(pid_t));
	if (!pipes || !pids) {
		free(pipes);
		free(pids);
		return 1;
	}
	for (i = 0; i < n - 1; i++) {
		if (pipe(pipes[i]) < 0) {
			perror("pipe");
			free(pipes);
			free(pids);
			return 1;
		}
	}
	c = cmds;
	for (i = 0; i < n; i++, c = c->next) {
		pids[i] = fork();
		if (pids[i] < 0) {
			perror("fork");
			free(pipes);
			free(pids);
			return 1;
		}
		if (pids[i] == 0) {
			sig_child();
			if (i > 0)
				dup2(pipes[i - 1][0], STDIN_FILENO);
			if (i < n - 1)
				dup2(pipes[i][1], STDOUT_FILENO);
			for (int j = 0; j < n - 1; j++) {
				close(pipes[j][0]);
				close(pipes[j][1]);
			}
			if (apply_redirs(c->redirs) < 0)
				_exit(1);
			execvp(c->argv[0], c->argv);
			perror("execvp");
			_exit(127);
		}
	}
	for (i = 0; i < n - 1; i++) {
		close(pipes[i][0]);
		close(pipes[i][1]);
	}
	signal(SIGINT, SIG_IGN);
	for (i = 0; i < n; i++) {
		waitpid(pids[i], &status, 0);
		if (i == n - 1) {
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = 128 + WTERMSIG(status);
		}
	}
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		write(1, "\n", 1);
	sig_shell();
	free(pipes);
	free(pids);
	return last_status;
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
	signal(SIGINT, SIG_IGN);
			
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
