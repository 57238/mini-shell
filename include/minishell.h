#ifndef MINISHELL_H
#define MINISHELL_H

// shell.c

int run_shell(void);

// exec.c
#include "parser.h"
int  exec_cmd(command *cmd);
int  apply_redirs(redirect *r);
int  save_fds(int saved[2]);
void restore_fds(int saved[2]);

// signal.c
void handle_siging(int);
void sig_shell(void);
void sig_child(void);
#endif
