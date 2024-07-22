#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

char** vreaddir(const char* path) {
    DIR* dir;
    struct dirent *entry;
    char** fileList = NULL;
    size_t count = 0;

    dir = opendir(path);
    if(dir == NULL) {
        perror("opendir");
        return NULL;
    }

    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char** tempList = realloc(fileList, (count + 2) * sizeof(char *));
        if(tempList == NULL) {
            perror("realloc");
            closedir(dir);
            free(fileList);
            return NULL;
        }
        fileList = tempList;

        fileList[count] = strdup(entry->d_name);
        if(fileList[count] == NULL) {
            perror("strdup");
            closedir(dir);
            for(size_t i = 0; i < count; ++i) {
                free(fileList[i]);
            }
            free(fileList);
            return NULL;
        }

        ++count;
    }

    fileList[count] = NULL;
    closedir(dir);
    return fileList;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Numero di argomenti sbagliato");
        return 1;
    }

    char** files = vreaddir(argv[1]);
    if(files == NULL) {
        fprintf(stderr, "Errore");
        return 1;
    }

    for(size_t i = 0; files[i] != NULL; ++i) {
        printf("%s\n", files[i]);
        free(files[i]);  
    }

    free(files);  

    return 0;
}
