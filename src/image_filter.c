#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "bitmap.h"
#include <fcntl.h> 


#define ERROR_MESSAGE "Warning: one or more filter had an error, so the output image may not be correct.\n"
#define SUCCESS_MESSAGE "Image transformed successfully!\n"


/*
 * Check whether the given command is a valid image filter, and if so,
 * run the process.
 *
 * We've given you this function to illustrate the expected command-line
 * arguments for image_filter. No further error-checking is required for
 * the child processes.
 */
void run_command(const char *cmd) {
    if (strcmp(cmd, "copy") == 0 || strcmp(cmd, "./copy") == 0 ||
        strcmp(cmd, "greyscale") == 0 || strcmp(cmd, "./greyscale") == 0 ||
        strcmp(cmd, "gaussian_blur") == 0 || strcmp(cmd, "./gaussian_blur") == 0 ||
        strcmp(cmd, "edge_detection") == 0 || strcmp(cmd, "./edge_detection") == 0) {
        execl(cmd, cmd, NULL);
    } else if (strncmp(cmd, "scale", 5) == 0) {
        // Note: the numeric argument starts at cmd[6]
        execl("scale", "scale", cmd + 6, NULL);
    } else if (strncmp(cmd, "./scale", 7) == 0) {
        // Note: the numeric argument starts at cmd[8]
        execl("./scale", "./scale", cmd + 8, NULL);
    } else {
        fprintf(stderr, "Invalid command '%s'\n", cmd);
        exit(1);
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: image_filter input output [filter ...]\n");
        exit(1);
    }

    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    int output_fd;
    int fd_in = input_fd;
    int p[2];
    pid_t pid;

    int additional_args = (argc > 3) ? 1 : 0; // Check if additional arguments are provided

    for (int i = additional_args == 1 ? 3 : 0; i < argc || (additional_args == 0 && i == 0); i++) {
        // Create the pipe
        pipe(p);

        // Fork a new process
        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);

        } else if (pid == 0) {
            // Child process
            dup2(fd_in, 0);

            // If not the last program, redirect stdout to the write end of the pipe
            if (additional_args == 1 && i != argc - 1) {
                dup2(p[1], 1);
            } else {
                // If it is the last program, redirect stdout to the output file
                output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (output_fd == -1) {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(output_fd, 1);
            }

            close(p[0]); 
            close(p[1]); 

            // Execute the program or the default program
            if (additional_args == 1) {
                // execl(argv[i], argv[i], NULL);
                run_command(argv[i]);
            } else {
                run_command("./copy");
            }

            // If execl returns, there was an error
            perror("execlp");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            close(fd_in); 
            close(p[1]); 

            // The output of this program will be the input of the next program
            fd_in = p[0];
        }
    }

    // Wait for all child processes to finish
    for (int i = ((additional_args == 1) ? 3 : 0); i < argc || (additional_args == 0 && i == 0); i++) {
        wait(NULL);
    }

    printf(SUCCESS_MESSAGE);
    return 0;
}

