#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Numero di argomenti sbagliato\n");
        return 1;
    }
    char* fifo_path = argv[1];

    if(mkfifo(fifo_path, 0666) == -1) {
        perror("mkfifo");
        return 1;
    }

    FILE* fd;
    char buffer[1024];
    while(1) {
        fd = fopen(fifo_path, "r");
        if(!fd) {
            perror("fopen");
            unlink(fifo_path);
            return 1;
        }

        while(fgets(buffer, sizeof(buffer), fd) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0';  

            if(strcmp(buffer, "FINE") == 0) {
                fclose(fd);
                break;
            }else {
                printf("%s\n", buffer);
            }
        }
        
        fclose(fd);
    }

    unlink(fifo_path);
    return 0;
}