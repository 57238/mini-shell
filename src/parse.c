#include "minishell.h"
#include <stdlib.h>
#include <string.h>


char **split_line(char *line) {
	int count = 0, max_tokens = 64;
	char *token;
	char **tokens = malloc(max_tokens * sizeof(char*));
	
	if (!tokens) return NULL;
	token = strtok(line, " \t\r\n\a");

	while (token && count < 63) {
		tokens[count++] = token;
		token = strtok(NULL, " \t\r\n\a");
	}
	tokens[count] = NULL;
	return tokens;
}
