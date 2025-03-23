#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_INPUT 1024
#define MAX_ARGS 100

void execute_command(char *args[], int background, char *input_file, char *output_file) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        if (input_file) {
            int fd = open(input_file, O_RDONLY);
            if (fd < 0) {
                perror("Input file error");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (output_file) {
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Output file error");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (execvp(args[0], args) < 0) {
            perror("Command execution failed");
            exit(1);
        }
    } else {
        if (!background) {
            waitpid(pid, NULL, 0);
        }
    }
}

void parse_and_execute(char *input) {
    char *args[MAX_ARGS];
    char *token = strtok(input, " ");
    int background = 0;
    char *input_file = NULL;
    char *output_file = NULL;
    int i = 0;

    while (token) {
        if (strcmp(token, "&") == 0) {
            background = 1;
        } else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            input_file = token;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            output_file = token;
        } else {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    if (args[0] == NULL) return;

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else if (chdir(args[1]) != 0) {
            perror("cd failed");
        }
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else {
        execute_command(args, background, input_file, output_file);
    }
}

int main() {
    char input[MAX_INPUT];

    while (1) {
        printf("my_shell> ");
        fflush(stdout);

        if (!fgets(input, MAX_INPUT, stdin)) break;

        input[strcspn(input, "\n")] = '\0';

        parse_and_execute(input);
    }
    return 0;
}
