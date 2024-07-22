#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>

void search_name(char* dir_path, char* name, char* args) {
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
            search_name(full_path, name, args);
        } else if(entry->d_type == DT_REG) {
            if(strcmp(entry->d_name, name) == 0) {
                struct stat statbuf;
                if(stat(full_path, &statbuf) == 0 && statbuf.st_mode & S_IXUSR) {

                    pid_t pid = fork();

                    if(pid == -1) {
                        perror("fork");
                        exit(1);
                    } else if(pid == 0) {
                        if(chdir(dir_path) ==  -1) {
                            perror("chdir");
                            exit(1);
                        }
                    
                        execvp(full_path, args);
                        perror("execvp");
                        exit(1);
                    } else {
                        int status;
                        waitpid(pid, &status, 0);

                        if(WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                            printf("Command executed successfully: %s\n", full_path);
                        } else {
                            printf("Command failed: %s\n", full_path);
                        }
                    }
                }
            }
        }
    }  
    closedir(dir);
}


int main(int argc, char * argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }
    
    char *name = argv[1];
    char *args = &argv[1];
    char current_dir[1024]; 
    getcwd(current_dir, sizeof(current_dir));

    search_name(current_dir, name, args);    

    return 0;
}