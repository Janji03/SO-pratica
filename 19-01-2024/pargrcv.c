#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pipe_path>\n", argv[0]);
        return 1;
    }

    const char *pipe_path = argv[1];

    // Crea la named pipe
    if (mkfifo(pipe_path, 0666) == -1) {
        perror("mkfifo");
        return 1;
    }

    printf("Waiting for input on %s...\n", pipe_path);

    // Apri la named pipe per lettura
    int pipe_fd = open(pipe_path, O_RDONLY);
    if (pipe_fd == -1) {
        perror("open");
        return 1;
    }

    // Dimensione iniziale dell'input buffer
    size_t buffer_size = 1024;
    char *buffer = malloc(buffer_size);
    if (buffer == NULL) {
        perror("malloc");
        close(pipe_fd);
        return 1;
    }

    // Leggi tutto l'input dalla pipe
    size_t input_length = 0;
    ssize_t bytes_read;
    while ((bytes_read = read(pipe_fd, buffer + input_length, buffer_size - input_length)) > 0) {
        input_length += bytes_read;
        if (input_length >= buffer_size) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL) {
                perror("realloc");
                close(pipe_fd);
                return 1;
            }
        }
    }

    if (bytes_read == -1) {
        perror("read");
        free(buffer);
        close(pipe_fd);
        return 1;
    }

    close(pipe_fd);

    // Conta il numero di argomenti
    int argc_count = 0;
    for (size_t i = 0; i < input_length; i++) {
        if (buffer[i] == '\0') {
            argc_count++;
        }
    }

    // Alloca memoria per gli argomenti
    char **argv_exec = malloc((argc_count + 1) * sizeof(char *));
    if (argv_exec == NULL) {
        perror("malloc");
        free(buffer);
        return 1;
    }

    // Popola l'array degli argomenti
    char *ptr = buffer;
    for (int i = 0; i < argc_count; i++) {
        argv_exec[i] = ptr;
        ptr += strlen(ptr) + 1;
    }
    argv_exec[argc_count] = NULL;

    // Esegue il comando
    execvp(argv_exec[0], argv_exec);

    // Se execvp fallisce, stampa un messaggio di errore
    perror("execvp");
    free(buffer);
    free(argv_exec);
    return 1;
}
