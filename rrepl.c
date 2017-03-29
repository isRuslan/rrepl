#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "rrepl.h"

#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char **argv){
	rl_bind_key ('\t', rl_insert);

	char *command = malloc(BUFSIZE * sizeof(char *));

	if(command == NULL){
		exit(EXIT_FAILURE);
	}

	if(argc > 1){
		strcpy(command, argv[1]);

		for(int i = 2; i < argc; i++){
			strcat(command, " ");
			strcat(command, argv[i]);
		}
	}

	run_loop(command);

	free(command);

	return EXIT_SUCCESS;
}

char **split_line(char command[], char *line) {
	int bufsize = BUFSIZE, position = 0;
	char **tokens = malloc((size_t) bufsize * sizeof(char*));
	char *token, **tokens_backup;

	char *cmd_buckup = malloc(BUFSIZE * sizeof(char *));

	if (!tokens) {
		fprintf(stderr, "rrpepl: allocation error\n");
		exit(EXIT_FAILURE);
	}

	strcpy(cmd_buckup, command);
    strcat(cmd_buckup, " ");
	strcat(cmd_buckup, line);

	token = strtok(cmd_buckup, DELIMITER);

	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += BUFSIZE;
			tokens_backup = tokens;
			tokens = realloc(tokens, (size_t) bufsize * (int) sizeof(char*));
			if (!tokens) {
				free(tokens_backup);
				fprintf(stderr, "rrepl: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, DELIMITER);
	}

	tokens[position] = NULL;
	return tokens;
}

void run_loop(char *command){
	char *line;
	char **args;
	int status;

	// command + "> " + "\n"
	char *cmd_promt = malloc(strlen(command) + 2 + 1);

	strcpy(cmd_promt, command);
	strcat(cmd_promt, "> ");

	do {
		line = readline(cmd_promt);
		if (line && *line){
			add_history(line);
		}
		args = split_line(command, line);
		status = execute(args);

		free(line);
		free(args);
	} while (status);
}

int execute(char **args){
	pid_t pid, wpid;
	int status;

	pid = fork();

	if (pid == 0){
		if (execvp(args[0], args) == -1) {
			perror("invalid");
		}
		exit(1);
	} else if (pid < 0) {
		perror("zero pid");
	} else {
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}
