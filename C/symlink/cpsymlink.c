#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>

void copy_file(char* src_path, char* dest_path) {
    int src_fd = open(src_path, O_RDONLY);
    if(src_fd < 0) {
        perror("open");
        exit(1);
    }
    int dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(dest_fd < 0) {
        perror("open dest");
        close(src_fd);
        exit(1);
    }

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

    if(bytes_read < 0) {
        perror("read");
    }

    close(src_fd);
    close(dest_fd);
}

void cp_sym_link(char* dir_path) {
    DIR* proc_dir = opendir(dir_path);
    if(proc_dir == NULL) {
        perror("opendir");
        exit(1);
    }

    struct dirent* entry;

    while((entry = readdir(proc_dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat buf;
        if(lstat(full_path, &buf) == -1) {
            perror("lstat");
            exit(1);
        }

        if(!S_ISLNK(buf.st_mode)) {
            continue;
        }

        char target_path[1024];
        ssize_t len = readlink(full_path, target_path, sizeof(target_path) - 1);
        if(len == -1) {
            perror("readlink");
            exit(1);
        
        }
        target_path[len] = '\0';

        if(unlink(full_path) == -1) {
            perror("unlink");
            exit(1);
        }

        copy_file(target_path, full_path);

    }
    closedir(proc_dir);
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }
    char* dir_path = argv[1];
    cp_sym_link(dir_path);
    return 0;
}