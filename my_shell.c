#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS 100
#define MAX_INPUT 1024

// Function to execute single commands
void execute_command(char *args[], int background, char *input_file, char *output_file) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
    } 
    else if (pid == 0) {  // Child process
        // Handle input redirection
        if (input_file) {
            int in_fd = open(input_file, O_RDONLY);
            if (in_fd < 0) {
                perror("Input file error");
                exit(1);
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }

        // Handle output redirection
        if (output_file) {
            int out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0) {
                perror("Output file error");
                exit(1);
            }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        // Execute command
        if (execvp(args[0], args) == -1) {
            perror("Command execution failed");
        }
        exit(1);
    } 
    else {  // Parent process
        if (!background) {
            waitpid(pid, NULL, 0); // Wait if not background process
        }
    }
}

// Function to execute piped commands
void execute_piped_command(char *cmd1[], char *cmd2[]) {
    int pipefd[2];
    pid_t pid1, pid2;

    if (pipe(pipefd) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    pid1 = fork();
    if (pid1 == 0) { // First child process
        close(pipefd[0]);  // Close read end
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(pipefd[1]);  

        if (execvp(cmd1[0], cmd1) == -1) {
            perror("First command execution failed");
            exit(1);
        }
    }

    pid2 = fork();
    if (pid2 == 0) { // Second child process
        close(pipefd[1]);  // Close write end
        dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin from pipe
        close(pipefd[0]);  

        if (execvp(cmd2[0], cmd2) == -1) {
            perror("Second command execution failed");
            exit(1);
        }
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

// Function to parse and execute user input
void parse_and_execute(char *input) {
    char *args[MAX_ARGS];
    char *cmd1[MAX_ARGS], *cmd2[MAX_ARGS];
    char *input_file = NULL, *output_file = NULL;
    int background = 0, pipe_found = 0;
    int i = 0, j = 0;

    // Tokenize input
    char *token = strtok(input, " ");
    while (token) {
        if (strcmp(token, "|") == 0) {
            pipe_found = 1;
            cmd1[i] = NULL;
            j = 0;
        } 
        else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            input_file = token;
        } 
        else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            output_file = token;
        } 
        else if (strcmp(token, "&") == 0) {
            background = 1;
        } 
        else {
            if (pipe_found) cmd2[j++] = token;
            else cmd1[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    cmd2[j] = NULL;
    cmd1[i] = NULL;

    // Execute commands
    if (pipe_found) {
        execute_piped_command(cmd1, cmd2);
    } 
    else if (strcmp(cmd1[0], "cd") == 0) {  // Built-in `cd`
        if (cmd1[1]) {
            if (chdir(cmd1[1]) != 0) {
                perror("cd failed");
            }
        } else {
            printf("Usage: cd <directory>\n");
        }
    } 
    else if (strcmp(cmd1[0], "exit") == 0) {  // Built-in `exit`
        exit(0);
    } 
    else {
        execute_command(cmd1, background, input_file, output_file);
    }
}

// Main function to run shell loop
int main() {
    char input[MAX_INPUT];

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        if (!fgets(input, MAX_INPUT, stdin)) {
            break;
        }
        
        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;

        if (strlen(input) > 0) {
            parse_and_execute(input);
        }
    }

    return 0;
}
