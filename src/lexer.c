#include "token.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

token *make_token(token_type type, char *text) {
	token *t = malloc(sizeof(*t));
	if (!t)
		return NULL;
	t->type = type;
	t->text = text;
	t->next = NULL;
	return t;
}

void add_token(token **head, token **tail, token *t) {
	if (!t)
		return;
	if (!*head)
		*head = t;
	else 
		(*tail)->next = t;
	*tail = t;
}

void print_tokens(token *head) {

	while (head) {
		if (head->type == TOK_WORD)
			printf("WORD: %s\n", head->text);
		else if (head->type == TOK_PIPE)
			printf("PIPE\n");
		else if (head->type == TOK_REDIR_IN)
			printf("REDIR_IN\n");
		else if (head->type == TOK_REDIR_OUT)
			printf("REDIR_OUT\n");
		else if (head->type == TOK_APPEND)
			printf("APPEND\n");
		else if (head->type == TOK_HEREDOC)
			printf("HEREDOC\n");

        	head = head->next;
	}
}

int is_op(char c) {
	return (c == '|' || c == '<' || c == '>');
}

// forms the linked list of tokens

token *tokenize(char *line) {
	int i = 0;
	token *head = NULL, *tail = NULL;
	
	while (line[i]) {
		while (line[i] && isspace(line[i]))
			i++;
		if (!line[i])
			break;
		if (line[i] == '|') {
			add_token(&head, &tail, make_token(TOK_PIPE, NULL));
			i++;
			continue;
		}
		else if (line[i] == '<') {
			if (line[i+1] == '<') {
				add_token(&head, &tail, make_token(TOK_HEREDOC, NULL));
				i += 2;
			}
			else {
				add_token(&head, &tail, make_token(TOK_REDIR_IN, NULL));
				i++;
			}
			continue;
		}
		else if (line[i] == '>') {
			if (line[i+1] == '>') {
				add_token(&head, &tail, make_token(TOK_APPEND, NULL));
				i += 2;
			}
			else {
				add_token(&head, &tail, make_token(TOK_REDIR_OUT, NULL));
				i++;
			}
			continue;
		}
		int start = i;
		while (line[i] && !isspace(line[i]) && !is_op(line[i]))
			i++;

		char *word = strndup(&line[start], i-start);

		add_token(&head, &tail, make_token(TOK_WORD, word));
		
	}

	return head;
}
