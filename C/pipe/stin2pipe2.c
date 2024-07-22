#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    
    char *cmds[100];
    char buffer[1024];
    int num_cmds = 0;
    while(num_cmds < 100 && fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        cmds[num_cmds] = strdup(buffer);
        if(cmds[num_cmds] == NULL) {
            perror("strdup");
            return 1;
        }
        num_cmds++;
    }
    int prev_fd = -1;
    int fd[2];

    for(int i = 0; i < num_cmds; i++) {
        if(i < num_cmds - 1) {
            if(pipe(fd) == -1) {
                return 1;
            }
        }

        pid_t pid = fork();

        if(pid == -1) {
            perror("fork");
            return 1;
        } else if(pid == 0) {
            if(i > 0) {
                if(dup2(prev_fd, STDIN_FILENO) == -1) {
                    perror("dup2");
                    return 1;
                }
                close(prev_fd);
            }
            if(i < num_cmds - 1) {
                close(fd[0]);
                if(dup2(fd[1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    return 1;
                }
                close(fd[1]);
            }

            char* argv[100];
            char* token;
            token = strtok(cmds[i], " ");
            int k = 0;
            while(token != NULL) {
                argv[k] = token;
                token = strtok(NULL, " ");
                k++;
            }
            argv[k] = NULL; 
            
            execvp(argv[0], argv);
            perror("execvp");
            return 1;
        } else {
            if(i > 0) {
                close(prev_fd);
            }
            if(i < num_cmds - 1) {
                close(fd[1]);
                prev_fd = fd[0];
            }
        }
    }
    
    for(int i = 0; i < num_cmds; i++) {
        wait(NULL);
    }

    for(int i = 0; i < num_cmds; i++) {
        free(cmds[i]);
    }

    return 0;
}