#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

char** vreaddir(const char* path) {
    DIR* dir;
    struct dirent* entry;
    size_t count = 0, total_length = 0;

    dir = opendir(path);
    if(dir == NULL) {
        perror("opendir");
        return NULL;
    }

    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            ++count;
            total_length += strlen(entry->d_name) + 1;
        }
    }

    char** fileList = malloc((count + 1) * sizeof(char *) + total_length * sizeof(char));
    if(fileList == NULL) {
        perror("malloc");
        closedir(dir);
        return NULL;
    }

    char* namePtr = (char *)(fileList + count + 1);
    rewinddir(dir);
    size_t index = 0;

    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            fileList[index] = namePtr;
            strcpy(namePtr, entry->d_name);
            namePtr += strlen(entry->d_name) + 1;
            ++index;
        }
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
    }

    free(files);

    return 0;
}
