#include "minishell.h"
#include <signal.h>
#include <unistd.h>

void handle_sigint(int sig) {
	(void)sig;
	write(STDOUT_FILENO, "\nmini-shell$ ", 13);
}

void sig_shell(void) {
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
}

void sig_child(void) {
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
