#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>


void abs_sym_link(char* dir_path) {
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

        char abs_target_path[1024];
        if(realpath(target_path, abs_target_path) == NULL) {
            perror("realpath");
            exit(1);
        }

        if(symlink(abs_target_path, full_path) == -1) {
            perror("symlink");
            exit(1);
        }
    }
    closedir(proc_dir);
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }
    char* dir_path = argv[1];
    abs_sym_link(dir_path);
    return 0;
}