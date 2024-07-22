#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

void list_dir(char* dir_path) {

    DIR* dir = opendir(dir_path);
    if(dir == NULL) {
        perror("opendir");
        exit(1);
    }

    struct dirent* entry;

    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char full_path[1024];

        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        if(entry->d_type == DT_REG) {
            char abs_path[1024];
            if(realpath(full_path, abs_path) == NULL) {
                continue;
            }
            printf("%s\n", abs_path);
        } else if(entry->d_type == DT_LNK) {
            char target_path[1024];
            ssize_t len = readlink(full_path, target_path, sizeof(target_path) - 1);
            if(len == -1) {
                continue;
            }
            target_path[len] = '\0';

            char abs_target_path[1024];
            if(realpath(full_path, abs_target_path) == NULL) {
                continue;
            }
            printf("%s\n", abs_target_path);
        } else if(entry->d_type == DT_DIR) {
            list_dir(full_path);
        }

    }

    closedir(dir);

}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;  
    }

    char* dir_path = argv[1];
    list_dir(dir_path);

    return 0;
}