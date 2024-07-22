#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    char cmd1[1024];
    char cmd2[1024];
    if(fgets(cmd1, sizeof(cmd1), stdin) == NULL) {
        perror("fgets");
        return 1;
    }
    if(fgets(cmd2, sizeof(cmd2), stdin) == NULL) {
        perror("fgets");
        return 1;
    }

    cmd1[strcspn(cmd1,"\n")] = '\0';
    cmd2[strcspn(cmd2,"\n")] = '\0';

    char* argv1[100];
    char* argv2[100];

    char* token;
    token = strtok(cmd1, " ");
    int i = 0;
    while(token != NULL) {
        argv1[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    argv1[i] = NULL;

    token = strtok(cmd2, " ");
    i = 0;
    while(token != NULL) {
        argv2[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    argv2[i] = NULL; 

    int fd[2];
    if(pipe(fd) == -1) {
        return 1;
    }

    pid_t pid1 = fork();

    if(pid1 == -1) {
        perror("fork");
        return 1;
    } else if(pid1 == 0) {
        close(fd[0]);
        if(dup2(fd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            return 1;
        }
        close(fd[1]);
        execvp(argv1[0], argv1);
        perror("execvp");
        return 1;
    } else {
        pid_t pid2 = fork();

        if(pid2 == -1) {
            perror("fork");
            return 1;
        } else if(pid2 == 0) {
            close(fd[1]);
            if(dup2(fd[0], STDIN_FILENO) == -1) {
                perror("dup2");
                return 1;
            }
            close(fd[0]);
            execvp(argv2[0], argv2);
            perror("execvp");
            return 1;
        } else {
            close(fd[0]);
            close(fd[1]);

            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    }   

    return 0;
}