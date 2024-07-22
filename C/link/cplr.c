#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

void copy_structure(char* dir_a, char* dir_b) {
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

        char src_path[1024];
        char dest_path[1024];

        snprintf(src_path, sizeof(src_path), "%s/%s", dir_a, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dir_b, entry->d_name);

        if(entry->d_type == DT_DIR) {
            copy_structure(src_path, dest_path);
        } else {
            if(link(src_path, dest_path) != 0) {
                perror("link");
                closedir(dir1);
                exit(1); 
            }
        } 
    }
    closedir(dir1);
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }
    char* dir_a = argv[1];
    char* dir_b = argv[2];

    copy_structure(dir_a, dir_b);

    return 0;
}