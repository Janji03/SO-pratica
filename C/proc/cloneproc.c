#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MAX_PATH_LEN 1024

void clone_process(int pid) {
    char proc_dir[MAX_PATH_LEN];
    char cmdline_file[MAX_PATH_LEN];
    char cwd_link[MAX_PATH_LEN];
    char environ_file[MAX_PATH_LEN];

    snprintf(proc_dir, sizeof(proc_dir), "/proc/%d", pid);
    snprintf(cmdline_file, sizeof(cmdline_file), "%s/cmdline", proc_dir);
    snprintf(cwd_link, sizeof(cwd_link), "%s/cwd", proc_dir);
    snprintf(environ_file, sizeof(environ_file), "%s/environ", proc_dir);

    char cmdline_buf[MAX_PATH_LEN];
    char environ_buf[MAX_PATH_LEN];

    int cmdline_fd = open(cmdline_file, O_RDONLY);
    if(cmdline_fd == -1) {
        perror("open");
        exit(1);
    }
    ssize_t cmdline_size = read(cmdline_fd, cmdline_buf, sizeof(cmdline_buf) - 1);
    if(cmdline_size == -1) {
        perror("read");
        exit(1);
    }
    close(cmdline_fd);
    cmdline_buf[cmdline_size] = '\0';  

    ssize_t cwd_len = readlink(cwd_link, cmdline_buf, sizeof(cmdline_buf) - 1);
    if(cwd_len == -1) {
        perror("readlink");
        exit(1);
    }
    cmdline_buf[cwd_len] = '\0';  

    int environ_fd = open(environ_file, O_RDONLY);
    if(environ_fd == -1) {
        perror("open");
        exit(1);
    }
    ssize_t environ_size = read(environ_fd, environ_buf, sizeof(environ_buf) - 1);
    if (environ_size == -1) {
        perror("read");
        exit(1);
    }
    close(environ_fd);
    environ_buf[environ_size] = '\0'; 

    pid_t child_pid = fork();
    if(child_pid == -1) {
        perror("fork");
        exit(1);
    } else if(child_pid == 0) {
        char* argv[100];  
        int argc = 0;
        
        char* token = strtok(cmdline_buf, "\0");
        while(token != NULL && argc < 99) {
            argv[argc] = token;
            token = strtok(NULL, "\0");
            argc++;
        }
        argv[argc] = NULL;  

        execve(argv[0], argv, environ_buf);
        perror("execve");
        exit(1);
    } else {
        waitpid(child_pid, NULL, 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }

    int pid = atoi(argv[1]);
    if (pid <= 0) {
        fprintf(stderr, "pid non corretto");
        return 1;
    }

    clone_process(pid);

    return 0;
}
