#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

void abspath(char* path) {

    struct stat buf;

    if(lstat(path, &buf) == -1) {
        perror("lstat");
        exit(1);
    }

    if(!S_ISLNK(buf.st_mode)) {
        fprintf(stderr, "Non è un link simbolico");
        exit(1);
    }

    char target_path[1024];
    ssize_t len = readlink(path, target_path, sizeof(target_path) - 1);
    if(len == -1) {
        perror("readlink");
        exit(1);
    }
    target_path[len] = '\0';

    char abs_target_path[1024];
    if(realpath(target_path, abs_target_path) == NULL) {
        perror("realpath");
        exit(1);
    }

    if(strcmp(target_path, abs_target_path) != 0) {
        if(unlink(path) == -1) {
            perror("unlink");
            exit(1);
        }
        if(symlink(abs_target_path, path) == -1) {
            perror("symlink");
            exit(1);
        }
    }

}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;  
    }

    char* path = argv[1];
    abspath(path);
    return 0;
}