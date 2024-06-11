#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    // Dimensione iniziale dell'input buffer
    size_t buffer_size = 1024;
    char *buffer = malloc(buffer_size);
    if (buffer == NULL) {
        perror("malloc");
        return 1;
    }

    // Leggi tutto l'input dallo standard input
    size_t input_length = 0;
    int c;
    while ((c = getchar()) != EOF) {
        if (input_length >= buffer_size - 1) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL) {
                perror("realloc");
                return 1;
            }
        }
        buffer[input_length++] = (char)c;
    }
    buffer[input_length] = '\0';

    // Conta il numero di argomenti
    int argc = 0;
    for (size_t i = 0; i < input_length; i++) {
        if (buffer[i] == '\0') {
            argc++;
        }
    }

    // Alloca memoria per gli argomenti
    char **argv = malloc((argc + 1) * sizeof(char *));
    if (argv == NULL) {
        perror("malloc");
        free(buffer);
        return 1;
    }

    // Popola l'array degli argomenti
    char *ptr = buffer;
    for (int i = 0; i < argc; i++) {
        argv[i] = ptr;
        ptr += strlen(ptr) + 1;
    }
    argv[argc] = NULL;

    // Esegue il comando
    execvp(argv[0], argv);

    // Se execvp fallisce, stampa un messaggio di errore
    perror("execvp");
    free(buffer);
    free(argv);
    return 1;
}
