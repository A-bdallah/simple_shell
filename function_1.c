#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main(void) {
    char buffer[BUFFER_SIZE];
    char *args[2]; // Command and NULL terminator
    int status;

    while (1) {
        printf("#cisfun$ ");

        // Read command from user
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            if (feof(stdin)) {
                printf("\n"); // Print newline if Ctrl+D is pressed
                exit(EXIT_SUCCESS);
            } else {
                perror("fgets");
                exit(EXIT_FAILURE);
            }
        }

        // Remove newline character from the end
        buffer[strcspn(buffer, "\n")] = 0;

        // Tokenize the command
        args[0] = buffer;
        args[1] = NULL;

        // Fork a child process
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            if (execve(args[0], args, NULL) == -1) {
                perror("execve");
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent process
            // Wait for the child process to complete
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}
