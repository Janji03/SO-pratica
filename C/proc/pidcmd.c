#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

int is_only_numbers(char* name)  {
    if(*name == '\0') {
        return 0;
    }
    while(*name) {
        if(!isdigit(*name)) {
            return 0; 
        }
        name++;  
    }
    return 1;
}

void launched_by_cmd(int argc, char* argv[]) {
    DIR* proc_dir = opendir("/proc");
    if(proc_dir == NULL) {
        perror("opendir");
        exit(1);
    }

    struct dirent* entry;

    while((entry = readdir(proc_dir)) != NULL) {
        if(!is_only_numbers(entry->d_name)) {
            continue;
        }

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "/proc/%s/cmdline", entry->d_name);

        int fd = open(full_path, O_RDONLY);
        if(fd == -1) {
            perror("open");
            continue;
        }
        
        char buffer[4096];
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        close(fd);
        if(bytes_read <= 0) {
            continue;
        }
        buffer[bytes_read] = '\0';

        char* token = strtok(buffer, "\0");
        int i;
        for (i = 1; i < argc && token != NULL; i++) {
            if (strcmp(token, argv[i]) != 0) {
                break;
            }
            token = strtok(NULL, "\0");
        }
        
    }
    closedir(proc_dir);
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }
    launched_by_cmd(argc, argv);
    return 0;
}