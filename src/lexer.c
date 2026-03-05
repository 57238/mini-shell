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

char *join_words(char *word1, char *word2) {
	int len1 = word1 ? strlen(word1) : 0;
	int len2 = word2 ? strlen(word2) : 0;

	char *result = malloc(len1 + len2 + 1);
	if (!result) {
		free(word1);
		free(word2);
		return NULL;
	}
	memcpy(result, word1, len1);
	memcpy(result + len1, word2, len2);

	result[len1+len2] = '\0';

	free(word1);
	free(word2);

	return result;
}

char *read_word(char *line, int *i) {
	char *word = NULL;

	while(line[*i] && !isspace(line[*i]) && !is_op(line[*i])) {
		if (line[*i] == '"' || line[*i] == '\'') {
			int start;
			char quote = line[*i];
			
			(*i)++;
			
			start = (*i); 

			while (line[*i] && line[*i] != quote)
				(*i)++;

			if (line[*i] != quote) { // if unclosed quote
				free(word);
				return NULL;
			}
			
			char *part = strndup(&line[start], *i - start);
			word = join_words(word, part);
				
			(*i)++;
		}
		else {
			int start = *i;
			while(line[*i] && !isspace(line[*i]) && !is_op(line[*i]) && line[*i] != '"' && line[*i] != '\'')
				(*i)++;
			char *part = strndup(&line[start], *i - start);
			word = join_words(word, part);
		}
	}
	return word;	
}


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
		char *word = read_word(line, &i);

		add_token(&head, &tail, make_token(TOK_WORD, word));
		
	}

	return head;
}
