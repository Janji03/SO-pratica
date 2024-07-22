#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

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

void copy_structure(time_t time, char* dir_a, char* dir_b) {
    DIR* dir1= opendir(dir_a);
    if(dir1 == NULL) {
        perror("opendir");
        exit(1);        
    }

    struct stat st;
    if(stat(dir_a, &st) != 0) {
        perror("stat");
        closedir(dir1);
        exit(1);
    }

    if(mkdir(dir_b, st.st_mode) != 0) {
        perror("mkdir");
        closedir(dir1);
        exit(1);
    }

    struct dirent* entry;

    while((entry = readdir(dir1)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        struct stat statbuf;
        if(stat(dir_a, &statbuf) != 0) {
            perror("stat");
            closedir(dir1);
            exit(1);
        }

        char src_path[1024];
        char dest_path[1024];

        snprintf(src_path, sizeof(src_path), "%s/%s", dir_a, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dir_b, entry->d_name);

        if(entry->d_type == DT_DIR) {
            copy_structure(time, src_path, dest_path);
        } else {
            if(statbuf.st_mtime >= time) {
                copy_file(src_path, dest_path);
            }
            else {
                if(link(src_path, dest_path) != 0) {
                    perror("link");
                    closedir(dir1);
                    exit(1); 
                }
            }
        } 
    }
    closedir(dir1);
}

int main(int argc, char* argv[]) {
    if(argc != 4) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }
    time_t time = argv[1];
    char* dir_a = argv[2];
    char* dir_b = argv[3];

    copy_structure(time, dir_a, dir_b);

    return 0;
}