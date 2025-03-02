#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_HISTORY 10
#define MAX_TOKENS 100
#define DELIMITERS " \t\r\n"

char *history[MAX_HISTORY];
int history_counter = 0;

// Function prototypes
void ExecuteCommand(char *command);
void handle_exit(char **args);
void handle_history(char **args);
void add_to_history(char *command);
void external_command(char **tokens);
char** tokenizer(char *command);

typedef struct {
    char *name;
    void (*func)(char **args);
} BuiltInCommand;

BuiltInCommand builtins[] = {
    {"history", handle_history},
    {"exit", handle_exit},
    {NULL, NULL} // End marker
};

char** tokenizer(char *command) {
    char **tokens = malloc(MAX_TOKENS * sizeof(char*));
    char *token;
    int i = 0;

    if (!tokens) {
        fprintf(stderr, "memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(command, DELIMITERS);
    while (token != NULL) {
        if (i >= MAX_TOKENS - 1) break; // Prevent buffer overflow
        tokens[i++] = strdup(token);
        token = strtok(NULL, DELIMITERS);
    }
    tokens[i] = NULL;
    return tokens;
}

void handle_exit(char **args) {
    printf("Exiting shell...\n");
    exit(0);
}

void handle_history(char **args) {
    for (int i = 0; i < history_counter; i++) {
        printf("%d- %s\n", i + 1, history[i]);
    }
}

void add_to_history(char *command) {
    if (history_counter < MAX_HISTORY) {
        history[history_counter++] = strdup(command);
    } else {
        free(history[0]);
        for (int i = 1; i < MAX_HISTORY; i++) {
            history[i - 1] = history[i];
        }
        history[MAX_HISTORY - 1] = strdup(command);
    }
}

void ExecuteCommand(char *command) {
    add_to_history(command);
    char **tokens = tokenizer(command);

    if (tokens[0] == NULL) { // Empty input
        free(tokens);
        return;
    }
    
    // check if there direction < >
	for)
    // Check if it's a built-in command
    buildin_commands(tokens);

    external_command(tokens);
}
void buildin_commands(char**tokens){
    for (int i = 0; builtins[i].name != NULL; i++) {
        if (strcmp(builtins[i].name, tokens[0]) == 0) {
            builtins[i].func(tokens);
            free(tokens);
            return;
        }
    }
}
void external_command(char **tokens) {
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        return;
    } else if (pid == 0) {
        execvp(tokens[0], tokens);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        waitpid(pid, NULL, 0);
    }

    int i = 0;
    while (tokens[i] != NULL) {
        free(tokens[i++]);
    }
    free(tokens);
}

int main() {
    char command[1024];

    while (1) {
        printf("osh> ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // Handle Ctrl+D (EOF)
        }

        command[strcspn(command, "\n")] = 0; // Remove newline
        ExecuteCommand(command);
    }

    return 0;
}

