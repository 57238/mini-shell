#ifndef MINISHELL_H
#define MINISHELL_H

// shell.c

int run_shell(void);

// parse.c
char **split_line(char *);

// exec.c
int exec_cmd(char**);

// signal.c
void handle_siging(int);
void sig_shell(void);
void sig_child(void);
#endif
