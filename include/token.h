#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
	TOK_WORD = 0,
	TOK_PIPE,
	TOK_REDIR_IN,
	TOK_REDIR_OUT,
	TOK_APPEND,
	TOK_HEREDOC
} token_type;

typedef struct token {
	token_type type;
	char *text;
	struct token *next;
} token;

token *tokenize(char *line);
void print_tokens(token *head);
#endif
