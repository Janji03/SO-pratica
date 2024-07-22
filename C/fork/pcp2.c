#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <getopt.h>

void copy_file_segment(char* src, char* dest, off_t start, off_t end) {
    int src_fd = open(src, O_RDONLY);
    if(src_fd < 0) {
        perror("open");
        exit(1);
    }
    int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(dest_fd < 0) {
        perror("open");
        close(src_fd);
        exit(1);
    }

    if(lseek(src_fd, start, SEEK_SET) == -1) {
        perror("lseek");
        close(src_fd);
        close(dest_fd);
        exit(1);
    }
    if(lseek(dest_fd, start, SEEK_SET) == -1) {
        perror("lseek");
        close(src_fd);
        close(dest_fd);
        exit(1);
    }

    char buffer[4096];
    ssize_t bytes_read, bytes_written;
    off_t remaining = end - start;
    while (remaining > 0 && (bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("write");
            close(src_fd);
            close(dest_fd);
            exit(1);
        }
        remaining -= bytes_written;
    }

    close(src_fd);
    close(dest_fd);
}

void copy_file_parallel(char* src, char* dest, int num_processes) {
    struct stat statbuf;
    if(stat(src, &statbuf) < 0) {
        perror("stat");
        exit(1);
    }
    off_t size = statbuf.st_size;

    off_t segment_size = size / num_processes;
    off_t start = 0, end = 0;

    for(int i = 0; i < num_processes; ++i) {
        start = i * segment_size;
        end = (i == num_processes - 1) ? size : start + segment_size;

        pid_t pid = fork();
        if(pid == -1) {
            perror("fork");
            exit(1);
        } else if(pid == 0) {
            copy_file_segment(src, dest, start, end);
            exit(0);
        }
    }

    int status;
    for(int i = 0; i < num_processes; ++i) {
        wait(&status);
    }
}

int main(int argc, char* argv[]) {
    int num_processes = 1;
    char opt;
    
    while((opt = getopt(argc, argv, "j:")) != -1) {
        switch(opt) {
            case 'j':
                num_processes = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Numero argomenti sbagliato");
                return 1;
        }
    }
    
    if(argc - optind != 2) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }

    char* src = argv[optind];
    char* dest = argv[optind + 1];

    copy_file_parallel(src, dest, num_processes);

    return 0;
}