#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

void copy_file(char* src, char* dest) {
    int src_fd = open(src, O_RDONLY);
    if(src_fd < 0) {
        perror("open");
        exit(1);
    }
    int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC);
    if(dest_fd < 0) {
        perror("open");
        close(src_fd);
        exit(1);
    }

    struct stat statbuf;
    if(stat(src, &statbuf) < 0) {
        perror("stat");
        close(src_fd);
        close(dest_fd);
        exit(1);
    }
    off_t size = statbuf.st_size;
    
    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        close(src_fd);
        close(dest_fd);
        exit(1);
    } else if(pid == 0) {
        off_t half_size = size / 2;
        char buffer[4096];
        ssize_t bytes_read, bytes_written;
        while(half_size > 0 && (bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
            bytes_written = write(dest_fd, buffer, bytes_read);
            if(bytes_written != bytes_read) {
                perror("write");
                close(src_fd);
                close(dest_fd);
                exit(1);
            }
            half_size -= bytes_written;
        }
    } else {
        pid_t pid2 = fork();
        if(pid2 == -1) {
            perror("fork");
            close(src_fd);
            close(dest_fd);
            exit(1);
        } else if(pid2 == 0) {
            lseek(dest_fd, size/2, SEEK_SET);
            char buffer[4096];
            ssize_t bytes_read, bytes_written;
            while((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
                bytes_written = write(dest_fd, buffer, bytes_read);
                if(bytes_written != bytes_read) {
                    perror("write");
                    close(src_fd);
                    close(dest_fd);
                    exit(1);
                }
            }
        } else {
            int status, status2;
            waitpid(pid, &status, 0);
            waitpid(pid2, &status2, 0);
            close(src_fd);
            close(dest_fd);
        }
    }
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }

    char* src = argv[1];
    char* dest = argv[2];
    copy_file(src, dest);
    return 0;
}