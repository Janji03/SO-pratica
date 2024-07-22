#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

void search_name(char* dir_path, char* name) {
    DIR* dir = opendir(dir_path);
    if(dir == NULL) {
        perror("opendir");
        exit(1);
    }

    struct dirent* entry;

    char full_path[1024];

    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        if(entry->d_type == DT_DIR) {
            search_name(full_path, name);
        } else if(entry->d_type == DT_REG) {
            if(strcmp(entry->d_name, name) == 0) {
                struct stat statbuf;
                if(stat(full_path, &statbuf) == 0 && statbuf.st_mode & S_IXUSR) {
                    FILE *fd = fopen(full_path, "r");
                    if(fd == NULL) {
                        perror("fopen");
                        exit(1);
                    }
                    char script_pattern[3];
                    int n = fread(script_pattern, 1, 2, fd);
                    script_pattern[2] = '\0';

                    if(strcmp(script_pattern, "#!") == 0 && n == 2) {
                        printf("%s: script", full_path);
                    } else {
                        printf("%s: ELF executable", full_path);
                    }
                    
                    fclose(fd);
                }
            }
        }
    }  
    closedir(dir);
}


int main(int argc, char * argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }
    
    char *name = argv[1];
    char current_dir[1024]; 
    getcwd(current_dir, sizeof(current_dir));

    search_name(current_dir, name);    

    return 0;
}