#include <stdio.h>
#include "minishell.h"

int main(int argc, char **argv, char **envp){
	(void)argc;
	(void)argv;
	(void)envp;
	run_shell();
	return 0;
}
